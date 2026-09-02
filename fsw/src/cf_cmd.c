/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *
 *  The CF Application command handling source file
 *
 *  All ground commands are processed in this file. All supporting functions
 *  necessary to process the commands are also here.
 */

#include "cf_app.h"
#include "cf_verify.h"
#include "cf_eventids.h"
#include "cf_perfids.h"
#include "cf_utils.h"
#include "cf_version.h"
#include "cf_platform_cfg.h"

#include "cf_cfdp.h"
#include "cf_cmd.h"

#include <string.h>

/**
 * Type used for dispatch table in generic handlers
 *
 * Currently this is always just a function pointer, but
 * is declared as a union to permit other items in future use
 */
typedef union CF_GenericAction_TableEntry
{
    CF_GenericAction_Func_t fn;     /**< Implementation to execute for this table entry */
    const void             *object; /**< Generic Pointer to data object */

} CF_GenericAction_TableEntry_t;

/**
 * Helper functions for generic actions
 *
 * This defines _how_ to process the matched entry in the dispatch table
 * If the entry is a function pointer, it may invoke the function, or if
 * the entry is a data object it may call some other routine using
 * that object.
 *
 * The chan pointer is passed through from the initial loop
 * of applicable channels (e.g. when CF_ForEachChannel() is used)
 *
 * @param entry   the dispatcher table entry
 * @param ga      pointer to the (base) context struct
 * @param chan    CFDP channel - passed through to function
 *
 * @returns CFE Status code.  If an error is returned, dispatching will stop
 * @retval  #CFE_SUCCESS in normal operation, will continue dispatching
 */
typedef CFE_Status_t (*CF_GenericAction_HelperFn_t)(CF_GenericAction_TableEntry_t entry,
                                                    CF_GenericAction_Context_t   *ga,
                                                    CF_Channel_t                 *chan);

/**
 * Intermediate context struct for use with CF_ForEachChannel()
 *
 * When operating across multiple channels via CF_ForEachChannel(), this
 * stores the original callback and context, and a local intermediate function
 * is used as the direct callback.  This intermediate function then calls
 * the originally-requested callback with the original context, combined
 * with the channel pointer.
 */
typedef struct CF_ActionContext_FEC
{
    uint32                      match_count;
    CF_GenericAction_Func_t     fn;
    CF_GenericAction_Context_t *context;
} CF_ActionContext_FEC_t;

/**
 * Context object for use when matching transaction-based commands
 *
 * This is used for commands that operate on a transaction, via the #CF_Transaction_Payload_t
 * command structure.  The normal channel loop is combined with CF_TraverseAllTransactions()
 * on each channel to find the applicable transaction(s).
 */
typedef struct
{
    CF_GenericAction_Context_t ga;

    CF_TraverseAllTransactions_fn_t action_cb;
    void                           *action_arg;

    /* original params from request */
    bool use_ts_eid;

    CF_TransactionSeq_t ts;
    CF_EntityId_t       eid;

} CF_GenericAction_TxnContext_t;

/**
 * Context object used with ResetCounters command
 */
typedef struct CF_DoResetFault_Context
{
    CF_GenericAction_Context_t ga; /* always first */

    /* original params from request */
    CF_Reset_Enum_t user_resetsel;

} CF_DoResetFault_Context_t;

/**
 * Context object used with WriteQueue command
 */
typedef struct CF_WriteQueue_FullContext
{
    CF_GenericAction_Context_t ga; /* always first */

    /* file descriptor to write queue data to */
    osal_id_t fd;

    /* original params from request */
    CF_QueueSelect_Enum_t   user_qsel;
    CF_DirectionType_Enum_t user_dirsel;

} CF_WriteQueue_FullContext_t;

/**
 * Intermediate context object used with WriteQueue command
 *
 * The write queue command dispatches on both queue type and direction
 * The direction is dispatched first.  This captures the original request
 * info so that the direction handler can invoke the queue handler.
 */
typedef struct CF_WriteQueue_DirContext
{
    CF_GenericAction_Context_t   ga;       /* always first */
    CF_GenericAction_HelperFn_t  q_action; /* next action (for queues) */
    CF_WriteQueue_FullContext_t *ctxt;
} CF_WriteQueue_DirContext_t;

/**
 * A context object used with purge queue commands
 */
typedef struct CF_DoPurgeQueue_Context
{
    CF_GenericAction_Context_t ga; /* always first */

    /* original params from request */
    CF_QueueSelect_Enum_t user_qsel;

} CF_DoPurgeQueue_Context_t;

/**
 * A context object used with actions requiring only a boolean argument
 *
 * This combines a boolean value with an output that indicates if it
 * was a change or not.
 */
typedef struct CF_GenericAction_BoolArg
{
    CF_GenericAction_Context_t ga; /* always first */

    bool   barg;       /* desired value to set in matched field(s) */
    uint32 noop_count; /* count where the requested value was already set, resulting in no change */

} CF_GenericAction_BoolArg_t;

/**
 * A context object used with enable/disable polling dir commands
 */
typedef struct CF_EnableDisablePollDir_Context
{
    CF_GenericAction_BoolArg_t barg; /* always first */

    /* original params from request */
    CF_PollIdxSelect_t user_pollidx;

} CF_EnableDisablePollDir_Context_t;

/**
 * A context object used with get/set parameter commands
 */
typedef struct CF_GetSetParam_Context
{
    CF_GenericAction_Context_t ga; /* always first */

    /* original params from request */
    bool                is_set;
    CF_GetSet_ValueID_t param_id;
    uint32              value;

} CF_GetSetParam_Context_t;

/*----------------------------------------------------------------
 *
 * Local action helper function
 *
 * This does not invoke the action.  It just checks if the action entry is
 * valid, and increments the valid count.
 *
 * This is intended for pre-validating a command before doing any more
 * expensive work.  The ValidCount should be nonzero after using this.
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t
CF_GenericAction_DoCheck(CF_GenericAction_TableEntry_t entry, CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    if (entry.fn != NULL)
    {
        /* this matched a valid action, increment the count */
        ++ga->ValidCount;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * invokes the action entry for each valid entry
 * at this stage empty entries are considered a no-op (successful)
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t
CF_GenericAction_DoInvokeAction(CF_GenericAction_TableEntry_t entry, CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CFE_Status_t ret;

    if (entry.fn == NULL)
    {
        /*
         * note: if no-ops are considered an error, then CF_GenericAction_DoCheck() should
         * be used to validate the request first.  At this stage empty entries are benign.
         */
        ret = CFE_SUCCESS;
    }
    else
    {
        ret = entry.fn(ga, chan);
        if (ret == CFE_SUCCESS)
        {
            /* this count shows _real_ (non-noop) callbacks that returned success */
            ++ga->SuccessCount;
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Allows a generic action to be invoked through CF_ForEachChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GenericAction_DoChannel(CF_Engine_t *engine, CF_Channel_t *chan, void *arg)
{
    CF_ActionContext_FEC_t *dca_ctxt = arg;

    if (chan != NULL)
    {
        /* execute action operations on this channel, using original values */
        ++dca_ctxt->match_count;
        dca_ctxt->fn(dca_ctxt->context, chan);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Call the handler function for every matched channel
 * Returns the number of channels called
 *
 *-----------------------------------------------------------------*/
static uint32 CF_GenericAction_DispatchChannel(CF_GenericAction_Func_t     func,
                                               CF_GenericAction_Context_t *context,
                                               CF_ChannelSelect_t          chan_select)
{
    CF_ActionContext_FEC_t dca_ctxt;

    memset(&dca_ctxt, 0, sizeof(dca_ctxt));

    dca_ctxt.fn      = func;
    dca_ctxt.context = context;

    /*
     * Currently this is a singleton so its always the same.
     * If that ever changes this should be passed in indicating
     * the CFDP engine to operate on.
     */
    context->engine = CF_GetEngine();

    /* this function is generic for any ground command that takes a single channel
     * argument which must be less than CF_NUM_CHANNELS or 255 which is a special
     * value that means apply command to all channels */
    if (CF_IsAllChannels(chan_select))
    {
        /* apply to all channels */
        CF_ForEachChannel(context->engine, CF_GenericAction_DoChannel, &dca_ctxt);
    }
    else
    {
        /* apply to only the specified channel number */
        CF_GenericAction_DoChannel(context->engine, CF_GetChannelPtr(chan_select), &dca_ctxt);
    }

    /* if successful, return a positive channel count match */
    return dca_ctxt.match_count;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 *
 * Same as CF_GenericAction_DispatchChannel() but sends an event if
 * no channels were match in the process, meaning that the user-supplied
 * channel number was invalid.  The CF_CMD_CHAN_PARAM_ERR_EID event
 * is generated in this case.
 *
 *-----------------------------------------------------------------*/
static int32 CF_GenericAction_CheckAndDispatchChannel(const char                 *action_str,
                                                      CF_GenericAction_Func_t     func,
                                                      CF_GenericAction_Context_t *context,
                                                      CF_ChannelSelect_t          chan_select)
{
    int32 count;

    count = CF_GenericAction_DispatchChannel(func, context, chan_select);

    if (count == 0)
    {
        /* no channels were matched by the chan_select parameter */
        CFE_EVS_SendEvent(CF_CMD_CHAN_PARAM_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: %s: channel parameter out of range (%d)",
                          action_str,
                          CF_ChannelSelect_AsInt(chan_select));
    }

    return count;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Dispatch a generic action using common selector patterns.
 *
 * If the user selection is 0, treat it as "all" and loop through
 * all entries in the map.  Otherwise invoke the single specified
 * handler from the map, or no handler at all if the selector was
 * out of range.
 *
 * NOTE: the action in the map is not _directly_ invoked here.  Rather,
 * the given helper function is invoked with the map entry as a parameter.
 * The action helper may choose to call the routine or do something else
 * like count it.
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GenericAction_DoDispatch(CF_GenericAction_Context_t          *ga,
                                                CF_GenericAction_HelperFn_t          act_helper,
                                                const CF_GenericAction_TableEntry_t *map_baseptr,
                                                size_t                               map_size,
                                                uint32                               selector,
                                                CF_Channel_t                        *chan)
{
    const CF_GenericAction_TableEntry_t *MapCurrPtr;
    uint32                               NumEntries;
    CFE_Status_t                         ret;

    NumEntries = map_size / sizeof(*map_baseptr);
    MapCurrPtr = map_baseptr;
    ret        = CFE_STATUS_RANGE_ERROR;

    if (selector != 0)
    {
        if (selector < NumEntries)
        {
            /* do just a single entry */
            MapCurrPtr += selector;
            NumEntries  = 1;
        }
        else
        {
            NumEntries = 0;
        }
    }

    while (NumEntries > 0)
    {
        ++ga->TotalCount;
        ret = act_helper(*MapCurrPtr, ga, chan);
        if (ret != CFE_SUCCESS)
        {
            break;
        }

        ++MapCurrPtr;
        --NumEntries;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Sets a single boolean while also checking if it was already set to the same value
 *
 *-----------------------------------------------------------------*/
static void CF_GenericAction_DoSetBoolArg(CF_GenericAction_BoolArg_t *barg, bool *ref)
{
    ++barg->ga.TotalCount;
    if (*ref == barg->barg)
    {
        /* it was already set to the requested value, nothing changed */
        ++barg->noop_count;
    }
    else
    {
        *ref = barg->barg;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Invokes per-transaction callback
 *
 * This is an intermediate routine compatible with CF_TraverseAllTransactions()
 *
 *-----------------------------------------------------------------*/
static void CF_GenericAction_DoSingleTxnActionImpl(CF_Transaction_t *txn, void *arg)
{
    CF_GenericAction_TxnContext_t *ctxt = arg;

    ++ctxt->ga.ValidCount;
    ctxt->action_cb(txn, ctxt->action_arg);
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Finds a transaction by sequence and entity id
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_GenericAction_FindTxnActionImpl(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GenericAction_TxnContext_t *ctxt = (CF_GenericAction_TxnContext_t *)ga;
    CF_Transaction_t              *txn;

    if (ctxt->use_ts_eid)
    {
        /* perform action only on  matching ts+eid */
        txn = CF_FindTransactionBySequenceNumber(chan, ctxt->ts, ctxt->eid);
        if (txn != NULL)
        {
            /* got a match */
            CF_GenericAction_DoSingleTxnActionImpl(txn, ga);
        }
    }
    else
    {
        /* perform action on this channel, all transactions */
        CF_TraverseAllTransactions(chan, CF_GenericAction_DoSingleTxnActionImpl, ga);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Common handler for routines that operate on a transaction
 *
 *-----------------------------------------------------------------*/
int32 CF_GenericAction_CheckAndDispatchTxn(const char                     *action_str,
                                           const CF_Transaction_Payload_t *data,
                                           CF_TraverseAllTransactions_fn_t fn,
                                           void                           *arg)
{
    CF_GenericAction_TxnContext_t ctxt;
    int32                         count;

    memset(&ctxt, 0, sizeof(ctxt));

    ctxt.action_cb  = fn;
    ctxt.action_arg = arg;
    ctxt.use_ts_eid = data->use_ts_eid;
    ctxt.eid        = data->eid;
    ctxt.ts         = data->ts;

    /* First dispatch based on chan_num like all other ops do */
    count = CF_GenericAction_CheckAndDispatchChannel(action_str,
                                                     CF_GenericAction_FindTxnActionImpl,
                                                     &ctxt.ga,
                                                     data->chan_num);

    if (count != 0 && ctxt.use_ts_eid && ctxt.ga.ValidCount == 0)
    {
        CFE_EVS_SendEvent(CF_CMD_TRANS_NOT_FOUND_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: %s cmd: failed to find transaction for (eid %lu, ts %lu)",
                          action_str,
                          (unsigned long)data->eid,
                          (unsigned long)data->ts);
    }

    /* this should reflect the matched transaction count */
    return ctxt.ga.ValidCount;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_NoopCmd(const CF_NoopCmd_t *msg)
{
    CFE_EVS_SendEvent(CF_NOOP_INF_EID,
                      CFE_EVS_EventType_INFORMATION,
                      "CF: No-Op received, Version %d.%d.%d.%d",
                      CF_MAJOR_VERSION,
                      CF_MINOR_VERSION,
                      CF_REVISION,
                      CF_MISSION_REV);

    ++CF_AppData.counters.cmd;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, compatible with generic action dispatch table
 * Resets command counters
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_ResetCounters_DoResetCommandCount(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    memset(&CF_AppData.counters, 0, sizeof(CF_AppData.counters));
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, compatible with generic action dispatch table
 * Resets fault counters
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_ResetCounters_DoResetFaultCount(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    memset(&chan->stat.counters.fault, 0, sizeof(chan->stat.counters.fault));
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, compatible with generic action dispatch table
 * Resets up (rx) counters
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_ResetCounters_DoResetUpCount(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    memset(&chan->stat.counters.recv, 0, sizeof(chan->stat.counters.recv));
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, compatible with generic action dispatch table
 * Resets down (tx) counters
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_ResetCounters_DoResetDownCount(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    memset(&chan->stat.counters.sent, 0, sizeof(chan->stat.counters.sent));
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function
 * Dispatches action for Reset counters command
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t
CF_ResetCounters_Dispatch(CF_DoResetFault_Context_t *ctxt, CF_GenericAction_HelperFn_t act_helper, CF_Channel_t *chan)
{
    static const CF_GenericAction_TableEntry_t CF_RESETCOUNTER_HANDLER_MAP[CF_Reset_MAX] = {
        [CF_Reset_command] = { CF_ResetCounters_DoResetCommandCount },
        [CF_Reset_fault]   = { CF_ResetCounters_DoResetFaultCount },
        [CF_Reset_up]      = { CF_ResetCounters_DoResetUpCount },
        [CF_Reset_down]    = { CF_ResetCounters_DoResetDownCount },
    };

    return CF_GenericAction_DoDispatch(&ctxt->ga,
                                       act_helper,
                                       CF_RESETCOUNTER_HANDLER_MAP,
                                       sizeof(CF_RESETCOUNTER_HANDLER_MAP),
                                       ctxt->user_resetsel,
                                       chan);
}

/*----------------------------------------------------------------
 *
 * Local Helper Function, compatible with CF_ForEachChannel()
 * Resets down counters
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_ResetCounters_ChanActionImpl(CF_Engine_t *engine, CF_Channel_t *chan, void *arg)
{
    return CF_ResetCounters_Dispatch(arg, CF_GenericAction_DoInvokeAction, chan);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ResetCountersCmd(const CF_ResetCountersCmd_t *msg)
{
    const CF_ResetCountersCmd_Payload_t *data = &msg->Payload;
    CF_DoResetFault_Context_t            ctxt;

    static const char *names[CF_Reset_MAX] = { [CF_Reset_all]     = "all",
                                               [CF_Reset_command] = "cmd",
                                               [CF_Reset_fault]   = "fault",
                                               [CF_Reset_up]      = "up",
                                               [CF_Reset_down]    = "down" };

    memset(&ctxt, 0, sizeof(ctxt));

    ctxt.user_resetsel = data->ResetType;

    /* checking the validity of parameters first.  if something is bad
     * we should do nothing at all and send the error event */
    CF_ResetCounters_Dispatch(&ctxt, CF_GenericAction_DoCheck, NULL);

    if (ctxt.ga.ValidCount == 0)
    {
        CFE_EVS_SendEvent(CF_CMD_RESET_INVALID_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: Received RESET COUNTERS command with invalid parameter %d",
                          (int)data->ResetType);
        ++CF_AppData.counters.err;
    }
    else
    {
        CFE_EVS_SendEvent(CF_RESET_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: Received RESET COUNTERS command: %s",
                          names[data->ResetType]);

        /* Incrementing the command counter FIRST -- this way, if the reset includes
         * the command counter, then it will be left at 0 rather than 1. */
        ++CF_AppData.counters.cmd;

        CF_ForEachChannel(CF_GetEngine(), CF_ResetCounters_ChanActionImpl, &ctxt);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_TxFileCmd(const CF_TxFileCmd_t *msg)
{
    const CF_TxFile_Payload_t *tx   = &msg->Payload;
    CF_Channel_t              *chan = CF_GetChannelPtr(tx->chan_num);
    CFE_Status_t               ret;

    /*
     * This needs to validate all its inputs.
     * "keep" should only be 0 or 1 (logical true/false).
     * For priority and dest_id params, anything is acceptable.
     */
    if (chan == NULL || !CF_IsValidClass(tx->cfdp_class) || (int)tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_CMD_BAD_PARAM_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_TxFileCmd(): chan=%d, class=%u keep=%u",
                          CF_ChannelSelect_AsInt(tx->chan_num),
                          (unsigned int)tx->cfdp_class,
                          (unsigned int)tx->keep);
        ++CF_AppData.counters.err;
    }
    else
    {
        ret = CF_CFDP_TxFile(tx->src_filename,
                             tx->dst_filename,
                             tx->cfdp_class,
                             tx->keep,
                             chan,
                             tx->priority,
                             tx->dest_id);
        if (ret == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CMD_TX_FILE_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "CF: file transfer successfully initiated");
            ++CF_AppData.counters.cmd;
        }
        else
        {
            CFE_EVS_SendEvent(CF_CMD_TX_FILE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: file transfer initiation failed");
            ++CF_AppData.counters.err;
        }
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PlaybackDirCmd(const CF_PlaybackDirCmd_t *msg)
{
    const CF_TxFile_Payload_t *tx   = &msg->Payload;
    CF_Channel_t              *chan = CF_GetChannelPtr(tx->chan_num);
    CFE_Status_t               ret;

    /*
     * This needs to validate all its inputs.
     * "keep" should only be 0 or 1 (logical true/false).
     * For priority and dest_id params, anything is acceptable.
     */
    if (chan == NULL || !CF_IsValidClass(tx->cfdp_class) || (int)tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_CMD_BAD_PARAM_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_PlaybackDirCmd(): chan=%u, class=%u keep=%u",
                          CF_ChannelSelect_AsInt(tx->chan_num),
                          (unsigned int)tx->cfdp_class,
                          (unsigned int)tx->keep);
        ++CF_AppData.counters.err;
    }
    else
    {
        ret = CF_CFDP_PlaybackDir(tx->src_filename,
                                  tx->dst_filename,
                                  tx->cfdp_class,
                                  tx->keep,
                                  chan,
                                  tx->priority,
                                  tx->dest_id);
        if (ret == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CMD_PLAYBACK_DIR_INF_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "CF: directory playback initiation successful");
            ++CF_AppData.counters.cmd;
        }
        else
        {
            CFE_EVS_SendEvent(CF_CMD_PLAYBACK_DIR_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF: directory playback initiation failed");
            ++CF_AppData.counters.err;
        }
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Sets the "frozen" bool in every matched channel
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_FreezeThaw_ChanActionImpl(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GenericAction_DoSetBoolArg((CF_GenericAction_BoolArg_t *)ga, &chan->stat.frozen);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_FreezeCmd(const CF_FreezeCmd_t *msg)
{
    CF_GenericAction_BoolArg_t barg;

    memset(&barg, 0, sizeof(barg));
    barg.barg = true; /* param is frozen, so true means freeze */

    CF_GenericAction_CheckAndDispatchChannel("freeze",
                                             CF_FreezeThaw_ChanActionImpl,
                                             &barg.ga,
                                             msg->Payload.ChannelSelect);

    if (barg.ga.TotalCount > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_FREEZE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: freeze successful");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_FREEZE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: freeze cmd failed");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ThawCmd(const CF_ThawCmd_t *msg)
{
    CF_GenericAction_BoolArg_t barg;

    memset(&barg, 0, sizeof(barg));
    barg.barg = false; /* param is frozen, so false means thaw */

    CF_GenericAction_CheckAndDispatchChannel("thaw",
                                             CF_FreezeThaw_ChanActionImpl,
                                             &barg.ga,
                                             msg->Payload.ChannelSelect);

    if (barg.ga.TotalCount > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_THAW_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: thaw successful");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_THAW_ERR_EID, CFE_EVS_EventType_ERROR, "CF: thaw cmd failed");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}
/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_CheckAndDispatchTxn()
 * Sets the "suspended" bool in every matched transaction record
 *
 *-----------------------------------------------------------------*/
static void CF_SuspRes_TxnActionImpl(CF_Transaction_t *txn, void *context)
{
    CF_Assert(txn);
    CF_GenericAction_DoSetBoolArg(context, &txn->flags.com.suspended);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_SuspRes_CommonHandler(const char *action_str, const CF_Transaction_Payload_t *payload, bool is_suspend)
{
    CF_GenericAction_BoolArg_t args;
    int                        count;

    memset(&args, 0, sizeof(args));
    args.barg = is_suspend;

    count = CF_GenericAction_CheckAndDispatchTxn(action_str, payload, CF_SuspRes_TxnActionImpl, &args);

    /*
     * Note that this command may affect multiple transactions, depending on the value of the "chan" argument.
     * When acting on multiple channels, the "same" output does not apply.  In reality all it means is
     * that one of the affected channels was already set that way.
     */

    if (count <= 0)
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_CMD_SUSPRES_CHAN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: %s cmd: no transaction found",
                          action_str);
        ++CF_AppData.counters.err;
    }
    else if (count == args.noop_count)
    {
        /* Success, but all matched transaction(s) were already set the same way, so nothing was done */
        CFE_EVS_SendEvent(CF_CMD_SUSPRES_SAME_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: %s cmd: setting suspend flag to current value of %d",
                          action_str,
                          (int)is_suspend);
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_SUSPRES_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: %s cmd: set suspend flag to %d",
                          action_str,
                          (int)is_suspend);
        ++CF_AppData.counters.cmd;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SuspendCmd(const CF_SuspendCmd_t *msg)
{
    CF_SuspRes_CommonHandler("suspend", &msg->Payload, true);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ResumeCmd(const CF_ResumeCmd_t *msg)
{
    CF_SuspRes_CommonHandler("resume", &msg->Payload, false);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_CheckAndDispatchTxn()
 * Cancels every matched transaction record
 *
 *-----------------------------------------------------------------*/
static void CF_Cancel_TxnActionImpl(CF_Transaction_t *txn, void *ignored)
{
    CF_CFDP_CancelTransaction(txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CancelCmd(const CF_CancelCmd_t *msg)
{
    int32 count;

    count = CF_GenericAction_CheckAndDispatchTxn("cancel", &msg->Payload, CF_Cancel_TxnActionImpl, NULL);

    if (count > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_CANCEL_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: cancel transaction successfully initiated");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_CMD_CANCEL_CHAN_ERR_EID, CFE_EVS_EventType_ERROR, "CF: cancel cmd: no transaction found");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_CheckAndDispatchTxn()
 * Abandons every matched transaction record
 *
 *-----------------------------------------------------------------*/
static void CF_Abandon_TxnActionImpl(CF_Transaction_t *txn, void *ignored)
{
    CF_CFDP_FinishTransaction(txn, false);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_AbandonCmd(const CF_AbandonCmd_t *msg)
{
    int32 count;

    count = CF_GenericAction_CheckAndDispatchTxn("abandon", &msg->Payload, CF_Abandon_TxnActionImpl, NULL);

    if (count > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_ABANDON_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: abandon successful");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_CMD_ABANDON_CHAN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: abandon cmd: no transaction found");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Sets the boolean to the configured value in this channel
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_EnaDisaDequeue_ChanActionImpl(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GenericAction_DoSetBoolArg((CF_GenericAction_BoolArg_t *)ga, &chan->config.dequeue_enabled);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableDequeueCmd(const CF_EnableDequeueCmd_t *msg)
{
    CF_GenericAction_BoolArg_t barg;

    memset(&barg, 0, sizeof(barg));
    barg.barg = true;

    CF_GenericAction_CheckAndDispatchChannel("enable_dequeue",
                                             CF_EnaDisaDequeue_ChanActionImpl,
                                             &barg.ga,
                                             msg->Payload.ChannelSelect);

    if (barg.ga.TotalCount > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_DEQUEUE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: dequeue enabled");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_DEQUEUE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: enable dequeue cmd failed");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableDequeueCmd(const CF_DisableDequeueCmd_t *msg)
{
    CF_GenericAction_BoolArg_t barg;

    memset(&barg, 0, sizeof(barg));
    barg.barg = false;

    CF_GenericAction_CheckAndDispatchChannel("disable_dequeue",
                                             CF_EnaDisaDequeue_ChanActionImpl,
                                             &barg.ga,
                                             msg->Payload.ChannelSelect);

    if (barg.ga.TotalCount > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_DEQUEUE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: dequeue disabled");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_DEQUEUE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: disable dequeue cmd failed");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Configures polling directory enable flag
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_EnaDisaPollDir_ChanActionImpl(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_LocalPdConfig_t                *PdConfPtr;
    uint32                             num_pd;
    CF_EnableDisablePollDir_Context_t *ctxt = (CF_EnableDisablePollDir_Context_t *)ga;
    CFE_Status_t                       ret;

    PdConfPtr = chan->config.polldir;
    ret       = CFE_SUCCESS;

    if (ctxt->user_pollidx == CF_ALL_POLLDIRS)
    {
        num_pd = CF_MAX_POLLING_DIR_PER_CHAN;
    }
    else if (ctxt->user_pollidx <= CF_MAX_POLLING_DIR_PER_CHAN)
    {
        PdConfPtr += (ctxt->user_pollidx - 1); /* this index is 1-based */
        num_pd     = 1;
    }
    else
    {
        ret    = CFE_STATUS_VALIDATION_FAILURE;
        num_pd = 0;
    }

    while (num_pd > 0)
    {
        CF_GenericAction_DoSetBoolArg(&ctxt->barg, &PdConfPtr->enabled);
        ++PdConfPtr;
        --num_pd;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Common implementation for enable/disable polling dir command
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t
CF_EnaDisaPollDir_CommonHandler(const char *action_str, const CF_PollDirSelect_Payload_t *payload, bool is_enable)
{
    CF_EnableDisablePollDir_Context_t ctxt;
    CFE_Status_t                      status;
    int32                             count;

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.user_pollidx = payload->PollDirIndx;
    ctxt.barg.barg    = is_enable;

    count = CF_GenericAction_CheckAndDispatchChannel(action_str,
                                                     CF_EnaDisaPollDir_ChanActionImpl,
                                                     &ctxt.barg.ga,
                                                     payload->ChannelSelect);

    if (ctxt.barg.ga.TotalCount != 0)
    {
        status = CFE_SUCCESS;
    }
    else
    {
        status = CFE_STATUS_VALIDATION_FAILURE;

        /* should only be sent if the channel was valid, as an event was already sent if the channel was bad */
        if (count != 0)
        {
            CFE_EVS_SendEvent(CF_CMD_POLLDIR_INVALID_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF: enable/disable polldir: invalid polldir %d on channel %d",
                              (int)payload->PollDirIndx,
                              CF_ChannelSelect_AsInt(payload->ChannelSelect));
        }
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableDirPollingCmd(const CF_EnableDirPollingCmd_t *msg)
{
    CFE_Status_t ret;

    ret = CF_EnaDisaPollDir_CommonHandler("enable_polldir", &msg->Payload, true);

    if (ret == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_POLLDIR_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: enabled polling directory");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_POLLDIR_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: enable polling directory cmd failed");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableDirPollingCmd(const CF_DisableDirPollingCmd_t *msg)
{
    CFE_Status_t ret;

    ret = CF_EnaDisaPollDir_CommonHandler("disable_polldir", &msg->Payload, false);

    if (ret == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_POLLDIR_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: disabled polling directory");
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_POLLDIR_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: disable polling directory cmd failed");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_CList_Traverse()
 * Resets the history for every node
 *
 *-----------------------------------------------------------------*/
static CF_CListTraverse_Status_t CF_PurgeQueue_ResetHistoryImpl(CF_CListNode_t *node, void *arg)
{
    CF_Channel_t *chan    = arg;
    CF_History_t *history = container_of(node, CF_History_t, cl_node);
    CF_ResetHistory(chan, history); /* ok to reset transaction since it's in PEND it hasn't started yet */
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_CList_Traverse()
 * Resets the history for every node
 *
 *-----------------------------------------------------------------*/
static CF_CListTraverse_Status_t CF_PurgeQueue_FinishTransactionImpl(CF_CListNode_t *node, void *ignored)
{
    CF_Transaction_t *txn = container_of(node, CF_Transaction_t, cl_node);
    CF_CFDP_FinishTransaction(txn, false);
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Purges the history queue for this channel
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_PurgeQueue_DoHistoryQ(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_CList_Traverse(chan->qs[CF_QueueIdx_HIST], CF_PurgeQueue_ResetHistoryImpl, chan);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Purges the pending queue for this channel
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_PurgeQueue_DoPendingQ(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_CList_Traverse(chan->qs[CF_QueueIdx_PEND], CF_PurgeQueue_FinishTransactionImpl, NULL);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, dispatch table for Purge Queue command
 * Calls the appropriate routine(s) based on specified queue
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_PurgeQueue_Dispatcher(CF_DoPurgeQueue_Context_t  *ctxt,
                                             CF_GenericAction_HelperFn_t act_helper,

                                             CF_Channel_t *chan)
{
    static const CF_GenericAction_TableEntry_t CF_PURGEQ_HANDLER_MAP[CF_QueueSelect_MAX] = {
        [CF_QueueSelect_Pending] = { CF_PurgeQueue_DoPendingQ },
        [CF_QueueSelect_History] = { CF_PurgeQueue_DoHistoryQ },
    };

    return CF_GenericAction_DoDispatch(&ctxt->ga,
                                       act_helper,
                                       CF_PURGEQ_HANDLER_MAP,
                                       sizeof(CF_PURGEQ_HANDLER_MAP),
                                       ctxt->user_qsel,
                                       chan);
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Calls the appropriate routine(s) based on specified queue(s)
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PurgeQueue_ChanActionImpl(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    return CF_PurgeQueue_Dispatcher((CF_DoPurgeQueue_Context_t *)ga, CF_GenericAction_DoInvokeAction, chan);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PurgeQueueCmd(const CF_PurgeQueueCmd_t *msg)
{
    CF_DoPurgeQueue_Context_t ctxt;
    int32                     count;

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.user_qsel = msg->Payload.QueueSelect;

    count = CF_GenericAction_CheckAndDispatchChannel("purge_queue",
                                                     CF_PurgeQueue_ChanActionImpl,
                                                     &ctxt.ga,
                                                     msg->Payload.ChannelSelect);

    /* If this matched channels but NOT any queues, send the arg err event */
    if (count != 0 && ctxt.ga.TotalCount == 0)
    {
        CFE_EVS_SendEvent(CF_CMD_PURGE_ARG_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: purge queue invalid arg %d",
                          (int)msg->Payload.QueueSelect);
    }

    if (ctxt.ga.TotalCount != 0)
    {
        CFE_EVS_SendEvent(CF_CMD_PURGE_QUEUE_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: %u queue(s) purged",
                          (unsigned int)ctxt.ga.TotalCount);
        ++CF_AppData.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_PURGE_QUEUE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: purge queue cmd failed");
        ++CF_AppData.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Writes Rx Active Queue data
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueue_DoRxActive(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_WriteQueue_FullContext_t *ctxt = (CF_WriteQueue_FullContext_t *)ga;
    CFE_Status_t                 ret;

    ret = CF_WriteTxnQueueDataToFile(ctxt->fd, chan, CF_QueueIdx_RX);
    if (ret != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_WRITEQ_RX_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: write queue failed on CF_QueueIdx_RX data");
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Writes Rx History Queue data
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueue_DoRxHistory(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_WriteQueue_FullContext_t *ctxt = (CF_WriteQueue_FullContext_t *)ga;
    CFE_Status_t                 ret;

    ret = CF_WriteHistoryQueueDataToFile(ctxt->fd, chan, CF_Direction_RX);
    if (ret != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_WRITEHIST_RX_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: write queue failed on history RX data");
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Writes Tx Active Queue data
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueue_DoTxActive(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_WriteQueue_FullContext_t *ctxt = (CF_WriteQueue_FullContext_t *)ga;
    CFE_Status_t                 ret;

    ret = CF_WriteTxnQueueDataToFile(ctxt->fd, chan, CF_QueueIdx_TX);
    if (ret != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_WRITEQ_TX_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: write queue failed on CF_QueueIdx_TX data");
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Writes Tx Pending Queue data
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueue_DoTxPending(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_WriteQueue_FullContext_t *ctxt = (CF_WriteQueue_FullContext_t *)ga;
    CFE_Status_t                 ret;

    ret = CF_WriteTxnQueueDataToFile(ctxt->fd, chan, CF_QueueIdx_PEND);
    if (ret != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_WRITEQ_PEND_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: write queue failed on CF_QueueIdx_PEND data");
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Writes Tx History Queue data
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueue_DoTxHistory(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_WriteQueue_FullContext_t *ctxt = (CF_WriteQueue_FullContext_t *)ga;
    CFE_Status_t                 ret;

    ret = CF_WriteHistoryQueueDataToFile(ctxt->fd, chan, CF_Direction_TX);
    if (ret != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_WRITEHIST_TX_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: write queue failed on CF_QueueIdx_TX data");
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Dispatches write queue actions for the Rx direction (uploads)
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueueCmd_DoProcessRxQ(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_WriteQueue_DirContext_t  *dir_ctxt = (CF_WriteQueue_DirContext_t *)ga;
    CF_WriteQueue_FullContext_t *ctxt     = dir_ctxt->ctxt; /* this is the original */

    static const CF_GenericAction_TableEntry_t CF_WRITEQ_RX_HANDLER_MAP[CF_QueueSelect_MAX] = {
        [CF_QueueSelect_Active]  = { CF_WriteQueue_DoRxActive },
        [CF_QueueSelect_History] = { CF_WriteQueue_DoRxHistory },
    };

    return CF_GenericAction_DoDispatch(&ctxt->ga,
                                       dir_ctxt->q_action,
                                       CF_WRITEQ_RX_HANDLER_MAP,
                                       sizeof(CF_WRITEQ_RX_HANDLER_MAP),
                                       ctxt->user_qsel,
                                       chan);
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Dispatches write queue actions for the Tx direction (downloads)
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueueCmd_DoProcessTxQ(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_WriteQueue_DirContext_t  *dir_ctxt = (CF_WriteQueue_DirContext_t *)ga;
    CF_WriteQueue_FullContext_t *ctxt     = dir_ctxt->ctxt; /* this is the original */

    static const CF_GenericAction_TableEntry_t CF_WRITEQ_TX_HANDLER_MAP[CF_QueueSelect_MAX] = {
        [CF_QueueSelect_Active]  = { CF_WriteQueue_DoTxActive },
        [CF_QueueSelect_Pending] = { CF_WriteQueue_DoTxPending },
        [CF_QueueSelect_History] = { CF_WriteQueue_DoTxHistory },
    };

    return CF_GenericAction_DoDispatch(&ctxt->ga,
                                       dir_ctxt->q_action,
                                       CF_WRITEQ_TX_HANDLER_MAP,
                                       sizeof(CF_WRITEQ_TX_HANDLER_MAP),
                                       ctxt->user_qsel,
                                       chan);
}

/*----------------------------------------------------------------
 *
 * Local Helper function, dispatches write queue command based on direction
 * Calls the appropriate routine(s) based on specified direction(s)
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueueCmd_DirectionDispatch(CF_WriteQueue_FullContext_t *ctxt,
                                                       CF_GenericAction_HelperFn_t  act_helper,
                                                       CF_Channel_t                *chan)
{
    CF_WriteQueue_DirContext_t dir_ctxt;

    static const CF_GenericAction_TableEntry_t CF_WRITEQ_DIR_HANDLER_MAP[CF_DirectionType_MAX] = {
        [CF_DirectionType_up]   = { CF_WriteQueueCmd_DoProcessRxQ },
        [CF_DirectionType_down] = { CF_WriteQueueCmd_DoProcessTxQ },
    };

    /* save the params to pass thru to the next level (queue) */
    memset(&dir_ctxt, 0, sizeof(dir_ctxt));
    dir_ctxt.q_action = act_helper;
    dir_ctxt.ctxt     = ctxt;

    return CF_GenericAction_DoDispatch(&dir_ctxt.ga,
                                       CF_GenericAction_DoInvokeAction,
                                       CF_WRITEQ_DIR_HANDLER_MAP,
                                       sizeof(CF_WRITEQ_DIR_HANDLER_MAP),
                                       ctxt->user_dirsel,
                                       chan);
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Invokes the write queue action for matched directions on this channel
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueueCmd_ChanActionImpl(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    return CF_WriteQueueCmd_DirectionDispatch((CF_WriteQueue_FullContext_t *)ga, CF_GenericAction_DoInvokeAction, chan);
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * Invokes the check action for matched directions on this channel
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_WriteQueueCmd_CheckChannelAction(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    return CF_WriteQueueCmd_DirectionDispatch((CF_WriteQueue_FullContext_t *)ga, CF_GenericAction_DoCheck, chan);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WriteQueueCmd(const CF_WriteQueueCmd_t *msg)
{
    const CF_WriteQueue_Payload_t *wq = &msg->Payload;

    CF_WriteQueue_FullContext_t write_context;
    CFE_Status_t                status;
    int32                       count;

    memset(&write_context, 0, sizeof(write_context));

    write_context.user_dirsel = wq->dir_type;
    write_context.user_qsel   = wq->queue;

    status = CFE_STATUS_VALIDATION_FAILURE;

    /*
     * Do a pre-check of the arguments.  This is done on this command because it opens a file.
     * We should not open the file unless the basic command args are OK, because it would leave
     * a zero-byte file in the filesystem if it fails.  It also has a non-insignificant cost.
     */
    count = CF_GenericAction_CheckAndDispatchChannel("write queue",
                                                     CF_WriteQueueCmd_CheckChannelAction,
                                                     &write_context.ga,
                                                     wq->chan_num);

    if (count != 0)
    {
        /* start by checking the request -- it should match at least one valid q-write impl */
        if (write_context.ga.ValidCount == 0)
        {
            /* the dir/queue combo did not map to any valid combo */
            CFE_EVS_SendEvent(CF_CMD_WQ_ARGS_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF: write queue invalid command parameters");
        }
        else
        {
            /* so far so good, now do the actual work */
            /* PTFO: queues can be large. may want to split this work up across the state machine and take several
             * wakeups to complete */
            status = CF_WrappedOpenCreate(&write_context.fd,
                                          wq->filename,
                                          OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE,
                                          OS_WRITE_ONLY);
            if (status != OS_SUCCESS)
            {
                CFE_EVS_SendEvent(CF_CMD_WQ_OPEN_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to open file %s",
                                  wq->filename);
            }
            else
            {
                /* now repeat the loop and actually do the write */
                /* zero out the counts again so we can check for failures */
                count = write_context.ga.ValidCount;
                memset(&write_context.ga, 0, sizeof(write_context.ga));

                CF_GenericAction_DispatchChannel(CF_WriteQueueCmd_ChanActionImpl, &write_context.ga, wq->chan_num);
                CF_WrappedClose(write_context.fd);

                if (write_context.ga.SuccessCount != count)
                {
                    /* the number of successful writes did not match what it should have been */
                    /* this is just so we increment the error count, not the command count */
                    status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
                }
            }
        }
    }

    if (status == CFE_SUCCESS)
    {
        ++CF_AppData.counters.cmd;
        CFE_EVS_SendEvent(CF_CMD_WQ_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: write queue successful");
    }
    else
    {
        ++CF_AppData.counters.err;
        /* all pertinent event(s) should have already been sent */
    }

    /* all relevant events have been sent and status has been logged, so work is done - return SUCCESS */
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function for all get/set parameter operations
 * This performs the actual get/set and issues the corresponding event
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_DoParam(CF_GetSetParam_Context_t *ctxt,
                                      CF_Channel_t             *chan,
                                      const char               *printable_name,
                                      void                     *param_ptr,
                                      size_t                    param_sz)
{
    uint32             saved_value;
    CF_ChannelSelect_t chan_num;
    uint16             event_id;
    char               msg_buffer[32];

    saved_value = 0;
    chan_num    = CF_GetChannelFromPtr(chan);

    switch (param_sz)
    {
        case sizeof(uint32):
            saved_value = *((uint32 *)param_ptr);
            break;
        case sizeof(uint16):
            saved_value = *((uint16 *)param_ptr);
            break;
        default: /* uint8 is the only other possibility */
            saved_value = *((uint8 *)param_ptr);
            break;
    }

    if (ctxt->is_set)
    {
        switch (param_sz)
        {
            case sizeof(uint32):
                *((uint32 *)param_ptr) = ctxt->value;
                break;
            case sizeof(uint16):
                *((uint16 *)param_ptr) = ctxt->value;
                break;
            default: /* uint8 is the only other possibility */
                *((uint8 *)param_ptr) = ctxt->value;
                break;
        }

        event_id = CF_CMD_GETSET1_INF_EID;
        snprintf(msg_buffer,
                 sizeof(msg_buffer),
                 "changed %u -> %u",
                 (unsigned int)saved_value,
                 (unsigned int)ctxt->value);
    }
    else
    {
        event_id = CF_CMD_GETSET2_INF_EID;
        snprintf(msg_buffer, sizeof(msg_buffer), "value = %u", (unsigned int)saved_value);
    }

    CFE_EVS_SendEvent(event_id,
                      CFE_EVS_EventType_INFORMATION,
                      "CF: channel %d parameter id %s %s",
                      CF_ChannelSelect_AsInt(chan_num),
                      printable_name,
                      msg_buffer);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 * For invalid parameters
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_Invalid(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    return CFE_STATUS_NOT_IMPLEMENTED;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_ticks_per_second(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt   = (CF_GetSetParam_Context_t *)ga;
    CF_Engine_t              *engine = ga->engine;

    return CF_GetSet_DoParam(ctxt,
                             chan,
                             "ticks_per_second",
                             &engine->config.ticks_per_second,
                             sizeof(engine->config.ticks_per_second));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_rx_crc_calc_bytes_per_wakeup(CF_GenericAction_Context_t *ga,

                                                              CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt   = (CF_GetSetParam_Context_t *)ga;
    CF_Engine_t              *engine = ga->engine;

    return CF_GetSet_DoParam(ctxt,
                             chan,
                             "rx_crc_calc_bytes_per_wakeup",
                             &engine->config.rx_crc_calc_bytes_per_wakeup,
                             sizeof(engine->config.rx_crc_calc_bytes_per_wakeup));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_ack_timer_s(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt = (CF_GetSetParam_Context_t *)ga;

    return CF_GetSet_DoParam(ctxt, chan, "ack_timer_s", &chan->config.ack_timer_s, sizeof(chan->config.ack_timer_s));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_nak_timer_s(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt = (CF_GetSetParam_Context_t *)ga;

    return CF_GetSet_DoParam(ctxt, chan, "nak_timer_s", &chan->config.nak_timer_s, sizeof(chan->config.nak_timer_s));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_inactivity_timer_s(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt = (CF_GetSetParam_Context_t *)ga;

    return CF_GetSet_DoParam(ctxt,
                             chan,
                             "inactivity_timer_s",
                             &chan->config.inactivity_timer_s,
                             sizeof(chan->config.inactivity_timer_s));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_outgoing_file_chunk_size(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt   = (CF_GetSetParam_Context_t *)ga;
    CF_Engine_t              *engine = ga->engine;
    CFE_Status_t              ret;

    if (ctxt->is_set && (ctxt->value == 0 || ctxt->value > sizeof(CF_CFDP_PduFileDataContent_t)))
    {
        ret = CFE_STATUS_VALIDATION_FAILURE;
    }
    else
    {
        ret = CF_GetSet_DoParam(ctxt,
                                chan,
                                "outgoing_file_chunk_size",
                                &engine->config.outgoing_file_chunk_size,
                                sizeof(engine->config.outgoing_file_chunk_size));
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_ack_limit(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt = (CF_GetSetParam_Context_t *)ga;

    return CF_GetSet_DoParam(ctxt, chan, "ack_limit", &chan->config.ack_limit, sizeof(chan->config.ack_limit));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_nak_limit(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt = (CF_GetSetParam_Context_t *)ga;

    return CF_GetSet_DoParam(ctxt, chan, "nak_limit", &chan->config.nak_limit, sizeof(chan->config.nak_limit));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_local_eid(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt   = (CF_GetSetParam_Context_t *)ga;
    CF_Engine_t              *engine = ga->engine;

    return CF_GetSet_DoParam(ctxt, chan, "local_eid", &engine->config.local_eid, sizeof(engine->config.local_eid));
}

/*----------------------------------------------------------------
 *
 * Local Helper function, compatible with CF_GenericAction_DispatchChannel()
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSet_Do_chan_max_outgoing_messages_per_wakeup(CF_GenericAction_Context_t *ga,

                                                                       CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt = (CF_GetSetParam_Context_t *)ga;
    CFE_Status_t              ret;

    /* must have a nonzero limit or a synch semaphore */
    if (ctxt->is_set && ctxt->value == 0 && chan->config.sem_name[0] == 0)
    {
        ret = CFE_STATUS_VALIDATION_FAILURE;
    }
    else
    {
        ret = CF_GetSet_DoParam(ctxt,
                                chan,
                                "max_outgoing_messages_per_wakeup",
                                &chan->config.max_outgoing_messages_per_wakeup,
                                sizeof(chan->config.max_outgoing_messages_per_wakeup));
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_GetSetParam_ChanActionImpl(CF_GenericAction_Context_t *ga, CF_Channel_t *chan)
{
    CF_GetSetParam_Context_t *ctxt = (CF_GetSetParam_Context_t *)ga;
    CFE_Status_t              ret;

    static const CF_GenericAction_TableEntry_t CF_GETSETPARAM_HANDLER_MAP[CF_GetSet_ValueID_MAX] = {
        [0]                                              = { CF_GetSet_Do_Invalid },
        [CF_GetSet_ValueID_ticks_per_second]             = { CF_GetSet_Do_ticks_per_second },
        [CF_GetSet_ValueID_rx_crc_calc_bytes_per_wakeup] = { CF_GetSet_Do_rx_crc_calc_bytes_per_wakeup },
        [CF_GetSet_ValueID_ack_timer_s]                  = { CF_GetSet_Do_ack_timer_s },
        [CF_GetSet_ValueID_nak_timer_s]                  = { CF_GetSet_Do_nak_timer_s },
        [CF_GetSet_ValueID_inactivity_timer_s]           = { CF_GetSet_Do_inactivity_timer_s },
        [CF_GetSet_ValueID_outgoing_file_chunk_size]     = { CF_GetSet_Do_outgoing_file_chunk_size },
        [CF_GetSet_ValueID_ack_limit]                    = { CF_GetSet_Do_ack_limit },
        [CF_GetSet_ValueID_nak_limit]                    = { CF_GetSet_Do_nak_limit },
        [CF_GetSet_ValueID_local_eid]                    = { CF_GetSet_Do_local_eid },
        [CF_GetSet_ValueID_chan_max_outgoing_messages_per_wakeup] = { CF_GetSet_Do_chan_max_outgoing_messages_per_wakeup },
    };

    ret = CF_GenericAction_DoDispatch(&ctxt->ga,
                                      CF_GenericAction_DoInvokeAction,
                                      CF_GETSETPARAM_HANDLER_MAP,
                                      sizeof(CF_GETSETPARAM_HANDLER_MAP),
                                      ctxt->param_id,
                                      chan);

    if (ret == CFE_STATUS_VALIDATION_FAILURE)
    {
        ++ctxt->ga.ErrorCount;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Local Helper Function
 * Common implementation for Get Param and Set Param commands
 *
 *-----------------------------------------------------------------*/
static void
CF_GetSetParam_CommonHandler(bool is_set, CF_GetSet_ValueID_t param_id, uint32 value, CF_ChannelSelect_t chan_num)
{
    int32                    count;
    CF_GetSetParam_Context_t ctxt;

    memset(&ctxt, 0, sizeof(ctxt));
    ctxt.is_set   = is_set;
    ctxt.param_id = param_id;
    ctxt.value    = value;

    count = CF_GenericAction_CheckAndDispatchChannel("param", CF_GetSetParam_ChanActionImpl, &ctxt.ga, chan_num);

    /* if anything happened, the overall command was successful
     * (note this is unlikely to have some channels work and some not) */
    if (ctxt.ga.SuccessCount != 0)
    {
        ++CF_AppData.counters.cmd;
    }
    else
    {
        ++CF_AppData.counters.err;

        /* check if the failure was due to key, NOT channel ID */
        if (count != 0)
        {
            /* this counter is incremented for CFE_STATUS_VALIDATION_FAILURE specifically */
            if (ctxt.ga.ErrorCount != 0)
            {
                /* the failure is due to a validation error */
                CFE_EVS_SendEvent(CF_CMD_GETSET_VALIDATE_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "CF: bad value %u for parameter id %d",
                                  (unsigned int)value,
                                  (int)param_id);
            }
            else
            {
                /* the failure must be due to a bad parameter key */
                CFE_EVS_SendEvent(CF_CMD_GETSET_PARAM_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "CF: invalid configuration parameter id %d received",
                                  (int)param_id);
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SetParamCmd(const CF_SetParamCmd_t *msg)
{
    const CF_SetParam_Payload_t *cmd = &msg->Payload;

    CF_GetSetParam_CommonHandler(true, cmd->key, cmd->value, cmd->chan_num);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_GetParamCmd(const CF_GetParamCmd_t *msg)
{
    const CF_GetParam_Payload_t *cmd = &msg->Payload;

    CF_GetSetParam_CommonHandler(false, cmd->key, 0, cmd->chan_num);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableEngineCmd(const CF_EnableEngineCmd_t *msg)
{
    CF_Engine_t *engine = CF_GetEngine();

    if (!engine->enabled)
    {
        if (CF_CFDP_InitEngine(engine) == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CMD_ENABLE_ENGINE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: enabled CFDP engine");
            ++CF_AppData.counters.cmd;
        }
        else
        {
            CFE_EVS_SendEvent(CF_CMD_ENABLE_ENGINE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF: failed to re-initialize and enable CFDP engine");
            ++CF_AppData.counters.err;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENG_ALREADY_ENA_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: received enable engine command while engine already enabled");
        ++CF_AppData.counters.cmd;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableEngineCmd(const CF_DisableEngineCmd_t *msg)
{
    CF_Engine_t *engine = CF_GetEngine();

    if (engine->enabled)
    {
        CF_CFDP_DisableEngine();
        CFE_EVS_SendEvent(CF_CMD_DISABLE_ENGINE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: disabled CFDP engine");
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENG_ALREADY_DIS_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF: received disable engine command while engine already disabled");
    }

    ++CF_AppData.counters.cmd;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Obtains HK stats for a single channel within the CFDP engine
 * Compatible with CF_ForEachChannel()
 *
 *-----------------------------------------------------------------*/
static int32 CF_SendHk_DoChannelStats(CF_Engine_t *engine, CF_Channel_t *chan, void *arg)
{
    CF_HkChannel_Data_t *out_chan_hk;
    CF_HkPacket_t       *hk       = arg;
    const int            chan_num = CF_ChannelSelect_AsInt(CF_GetChannelFromPtr(chan)) - 1;
    int                  q_idx;

    out_chan_hk = &hk->Payload.channel_hk[chan_num];

    out_chan_hk->counters = chan->stat.counters;
    for (q_idx = 0; q_idx < CF_QueueIdx_NUM; ++q_idx)
    {
        out_chan_hk->q_size[q_idx] = chan->stat.q_size[q_idx];
    }
    out_chan_hk->poll_counter     = chan->stat.poll_counter;
    out_chan_hk->playback_counter = chan->stat.playback_counter;
    out_chan_hk->frozen           = chan->stat.frozen;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SendHkCmd(const CF_SendHkCmd_t *msg)
{
    CF_Engine_t     *engine = CF_GetEngine();
    CFE_SB_Buffer_t *sb_buf;
    CF_HkPacket_t   *hk;

    sb_buf = CFE_SB_AllocateMessageBuffer(sizeof(*hk));
    if (sb_buf)
    {
        CFE_MSG_Init(&sb_buf->Msg, CFE_SB_ValueToMsgId(CF_HK_TLM_MID), sizeof(*hk));

        hk                   = (void *)&sb_buf->Msg;
        hk->Payload.counters = CF_AppData.counters;

        CF_ForEachChannel(engine, CF_SendHk_DoChannelStats, hk);

        /* return value ignored */
        CFE_SB_TransmitBuffer(sb_buf, true);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WakeupCmd(const CF_WakeupCmd_t *msg)
{
    CFE_ES_PerfLogEntry(CF_PERF_ID_CYCLE_ENG);
    CF_CFDP_CycleEngine();
    CFE_ES_PerfLogExit(CF_PERF_ID_CYCLE_ENG);

    /* This is also used to check tables */
    CF_CheckTables();

    return CFE_SUCCESS;
}
