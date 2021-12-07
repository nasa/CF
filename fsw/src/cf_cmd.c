/************************************************************************
** File: cf_cmd.c
**
** NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
** Application version 3.0.0”
** Copyright © 2019 United States Government as represented by the
** Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
** Licensed under the Apache License, Version 2.0 (the "License"); you may
** not use this file except in compliance with the License. You may obtain
** a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**
** Purpose:
**  The CF Application command handling source file
**
**  All ground commands are processed in this file. All supporting functions
**  necessary to process the commands are also here.
**
**
**
*************************************************************************/

#include "cf_app.h"
#include "cf_verify.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_utils.h"
#include "cf_version.h"

#include "cf_cfdp.h"

#include <string.h>

#define ALL_CHANNELS 255
#define ALL_POLLDIRS ALL_CHANNELS
#define COMPOUND_KEY 254

typedef int (*chan_action_fn_t)(uint8 chan_num, void *context);

typedef struct
{
    uint8 barg;
} bool_arg_t;

typedef CF_TraverseAllTransactions_fn_t CF_TsnChanAction_fn_t;

typedef struct
{
    int   same; /* out param -- indicates at least one action was set to its current value */
    uint8 action;
} susp_res_arg_t;

typedef struct
{
    const cf_cmd_unionargs_t *msg;
    uint8                     barg;
} bool_msg_arg_t;

/************************************************************************/
/** \brief Increment the command accepted counter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static inline void CF_CmdAcc(void)
{
    ++CF_AppData.hk.counters.cmd;
}

/************************************************************************/
/** \brief Increment the command rejected counter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static inline void CF_CmdRej(void)
{
    ++CF_AppData.hk.counters.err;
}

/************************************************************************/
/** \brief Conditionally increment the command accept or reject counters.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static inline void CF_CmdCond(int cond)
{
    static void (*const fns[])(void) = {CF_CmdAcc, CF_CmdRej};
    fns[!!cond]();
}

/************************************************************************/
/** \brief The no-operation command.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       This function simply prints an event message.
**       Increments the command accept counter.
**       The msg parameter is ignored in this one.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static void CF_CmdNoop(CFE_SB_Buffer_t *msg)
{
    CFE_EVS_SendEvent(CF_EID_INF_CMD_NOOP, CFE_EVS_EventType_INFORMATION, "CF: No-Op received, Version %d.%d.%d",
                      CF_MAJOR_VERSION, CF_MINOR_VERSION, CF_REVISION);
    CF_CmdAcc();
}

/************************************************************************/
/** \brief The reset counters command.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       Resets the given counter, or all.
**       Increments the command accept or reject counter. If the command
**       counters are reset, then there is no increment.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdReset(CFE_SB_Buffer_t *msg)
{
    static const int counters_command = 1;
    static const int counters_fault   = 2;
    static const int counters_up      = 3;
    static const int counters_down    = 4;

    cf_cmd_unionargs_t *cmd      = (cf_cmd_unionargs_t *)msg;
    static const char  *names[5] = {"all", "cmd", "fault", "up", "down"};
    /* 0=all, 1=cmd, 2=fault 3=up 4=down */
    uint8 param = cmd->data.byte[0];
    int   i;
    int   acc = 1;

    if (param > 4)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_RESET_INVALID, CFE_EVS_EventType_ERROR,
                          "CF: Received RESET COUNTERS command with invalid parameter %d", param);
        CF_CmdRej();
        goto err_out;
    }

    CFE_EVS_SendEvent(CF_EID_INF_CMD_RESET, CFE_EVS_EventType_INFORMATION, "CF: Received RESET COUNTERS command: %s",
                      names[param]);

    /* if the param is counters_command, or all counters */
    if (!param || (param == counters_command))
    {
        /* command counters */
        memset(&CF_AppData.hk.counters, 0, sizeof(CF_AppData.hk.counters));
        acc = 0; /* don't increment accept counter on command counter reset */
    }

    /* if the param is counters_fault, or all counters */
    if (!param || (param == counters_fault))
    {
        /* fault counters */
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
            memset(&CF_AppData.hk.channel_hk[i].counters.fault, 0, sizeof(CF_AppData.hk.channel_hk[i].counters.fault));
    }

    /* if the param is counters_up, or all counters */
    if (!param || (param == counters_up))
    {
        /* up counters */
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
            memset(&CF_AppData.hk.channel_hk[i].counters.recv, 0, sizeof(CF_AppData.hk.channel_hk[i].counters.recv));
    }

    /* if the param is counters_down, or all counters */
    if (!param || (param == counters_down))
    {
        /* down counters */
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
            memset(&CF_AppData.hk.channel_hk[i].counters.sent, 0, sizeof(CF_AppData.hk.channel_hk[i].counters.sent));
    }

    if (acc)
    {
        CF_CmdAcc();
    }

err_out:;
}

/************************************************************************/
/** \brief Ground command to start a file transfer.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       Increments the command accept or reject counter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdTxFile(CFE_SB_Buffer_t *msg)
{
    cf_cmd_tx_file_t *tx = (cf_cmd_tx_file_t *)msg;

    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;

    CF_CmdCond(CF_CFDP_TxFile(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num, tx->priority,
                              tx->dest_id));
}

/************************************************************************/
/** \brief Ground command to start directory playback.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       Increments the command accept or reject counter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdPlaybackDir(CFE_SB_Buffer_t *msg)
{
    cf_cmd_playback_dir_t *tx = (cf_cmd_playback_dir_t *)msg;

    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;

    CF_CmdCond(CF_CFDP_PlaybackDir(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num,
                                   tx->priority, tx->dest_id));
}

/************************************************************************/
/** \brief Common logic for all channel-based commands.
**
**  \par Description
**       All the commands that act on channels or have the special
**       "all channels" parameter come through this function. This puts
**       all common logic in one place. It does not handle the command
**       accept or reject counters.
**
**  \par Assumptions, External Events, and Notes:
**       cmd must not be NULL. errstr must not be NULL. fn must be a valid function. context may be NULL.
**
**  \returns
**  \retstmt The return value from the given function. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_DoChanAction(cf_cmd_unionargs_t *cmd, const char *errstr, chan_action_fn_t fn, void *context)
{
    int ret = 0;

    /* this function is generic for any ground command that takes a single channel
     * argument which must be less than CF_NUM_CHANNELS or 255 which is a special
     * value that means apply command to all channels */
    if (cmd->data.byte[0] == ALL_CHANNELS)
    {
        /* apply to all channels */
        int i;
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
            ret |= fn(i, context);
    }
    else if (cmd->data.byte[0] < CF_NUM_CHANNELS)
    {
        ret = fn(cmd->data.byte[0], context);
    }
    else
    {
        /* bad parameter */
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_CHAN_PARAM, CFE_EVS_EventType_ERROR,
                          "CF: %s: channel parameter out of range. received %d", errstr, cmd->data.byte[0]);
        ret = -1;
    }

    return ret;
}

/************************************************************************/
/** \brief Channel action to set the frozen bit for a channel.
**
**  \par Assumptions, External Events, and Notes:
**       context must not be NULL.
**
**  \returns
**  \retstmt Always succeeds, so returns 0. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_DoFreezeThaw(uint8 chan_num, const bool_arg_t *context)
{
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.hk.channel_hk[chan_num].frozen = context->barg;
    return 0;
}

/************************************************************************/
/** \brief Freeze a channel.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdFreeze(CFE_SB_Buffer_t *msg)
{
    bool_arg_t barg = {1}; /* param is frozen, so 1 means freeze */
    CF_CmdCond(CF_DoChanAction((cf_cmd_unionargs_t *)msg, "freeze", (chan_action_fn_t)CF_DoFreezeThaw, &barg));
}

/************************************************************************/
/** \brief Thaw a channel.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdThaw(CFE_SB_Buffer_t *msg)
{
    bool_arg_t barg = {0}; /* param is frozen, so 0 means thawed */
    CF_CmdCond(CF_DoChanAction((cf_cmd_unionargs_t *)msg, "thaw", (chan_action_fn_t)CF_DoFreezeThaw, &barg));
}

/************************************************************************/
/** \brief Search for a transaction across all channels.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt The transaction, if found. Otherwise NULL. \endcode
**  \endreturns
**
*************************************************************************/
static transaction_t *CF_CFDP_FindTransactionBySequenceNumberAllChannels(CF_TransactionSeq_t ts, CF_EntityId_t eid)
{
    int            i;
    transaction_t *ret = NULL;

    /* transaction sequence numbers are referenced with the tuple (eid, tsn)
     * Even though these tuples are unique to a channel, they should be unique
     * across the entire system. Meaning, it isn't correect to have the same
     * EID re-using a TSN in the same system. So, in order to locate the transaction
     * to suspend, we need to search across all channels for it. */
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        ret = CF_CFDP_FindTransactionBySequenceNumber(CF_AppData.engine.channels + i, ts, eid);
        if (ret)
        {
            break;
        }
    }

    return ret;
}

/* CF_TsnChanAction() returns the number of transactions acted upon */
/************************************************************************/
/** \brief Common logic for all transaction sequence number and channel commands.
**
**  \par Description
**       All the commands that on a transaction on a particular channel come
**       through this function. This puts all common logic in one place. It
**       does handle the command accept or reject counters.
**
**  \par Assumptions, External Events, and Notes:
**       cmd must not be NULL. fn must be a valid function. context may be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_TsnChanAction(cf_cmd_transaction_t *cmd, const char *cmdstr, CF_TsnChanAction_fn_t fn, void *context)
{
    int ret = -1;

    if (cmd->chan == COMPOUND_KEY)
    {
        /* special value 254 means to use the compound key (cmd->eid, cmd->ts) to find the transaction
         * to act upon */
        transaction_t *t = CF_CFDP_FindTransactionBySequenceNumberAllChannels(cmd->ts, cmd->eid);
        if (t)
        {
            fn(t, context);
            ret = CFE_SUCCESS;
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_TRANS_NOT_FOUND, CFE_EVS_EventType_ERROR,
                              "CF: %s cmd: failed to find transactino for (eid %d, ts %d)", cmdstr, cmd->eid, cmd->ts);
        }
    }
    else if (cmd->chan == ALL_CHANNELS)
    {
        /* perform action on all channels, all transactions */
        ret = CF_TraverseAllTransactions_All_Channels(fn, context);
    }
    else if (cmd->chan < CF_NUM_CHANNELS)
    {
        /* perform action on a specific channel, all transactions */
        ret = CF_TraverseAllTransactions(CF_AppData.engine.channels + cmd->chan, fn, context);
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_TSN_CHAN_INVALID, CFE_EVS_EventType_ERROR, "CF: %s cmd: invalid channel %d",
                          cmdstr, cmd->chan);
    }

    return ret;
}

/************************************************************************/
/** \brief Set the suspended bit in a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. context must not be NULL.
**
*************************************************************************/
static void CF_DoSuspRes_(transaction_t *t, susp_res_arg_t *context)
{
    CF_Assert(t);
    if (t->flags.com.suspended == context->action)
    {
        context->same = 1;
    }
    else
    {
        t->flags.com.suspended = context->action;
    }
}

/************************************************************************/
/** \brief Handle transaction suspend and resume commands.
**
**  \par Description
**       This is called for both suspend and resume ground commands.
**       It uses the CF_TsnChanAction() function to perform the command.
**
**  \par Assumptions, External Events, and Notes:
**       cmd must not be NULL.
**
*************************************************************************/
static void CF_DoSuspRes(cf_cmd_transaction_t *cmd, uint8 action)
{
    /* ok to not bounds check action, because the caller is using it in two places with constant values 0 or 1 */
    static const char *msgstr[] = {"resume", "suspend"};
    susp_res_arg_t     args     = {0, action};
    int                ret      = CF_TsnChanAction(cmd, msgstr[action], (CF_TsnChanAction_fn_t)CF_DoSuspRes_, &args);

    if (!ret && args.same)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_SUSPRES_SAME, CFE_EVS_EventType_ERROR,
                          "CF: %s cmd: setting suspend flag to current value of %d", msgstr[action], action);
        CF_CmdRej();
    }
    else if (ret)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_SUSPRES_CHAN, CFE_EVS_EventType_ERROR, "CF: %s cmd: no transaction found",
                          msgstr[action]);
        CF_CmdRej();
    }
    else
    {
        CF_CmdAcc();
    }
}

/************************************************************************/
/** \brief Handle transaction suspend command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdSuspend(CFE_SB_Buffer_t *msg)
{
    CF_DoSuspRes((cf_cmd_transaction_t *)msg, 1);
}

/************************************************************************/
/** \brief Handle transaction resume command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdResume(CFE_SB_Buffer_t *msg)
{
    CF_DoSuspRes((cf_cmd_transaction_t *)msg, 0);
}

/************************************************************************/
/** \brief tsn chan action to cancel a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CmdCancel_(transaction_t *t, void *ignored)
{
    CF_CFDP_CancelTransaction(t);
}

/************************************************************************/
/** \brief Handle a cancel ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdCancel(CFE_SB_Buffer_t *msg)
{
    CF_CmdCond(CF_TsnChanAction((cf_cmd_transaction_t *)msg, "cancel", CF_CmdCancel_, NULL));
}

/************************************************************************/
/** \brief tsn chan action to abandon a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdAbandon_(transaction_t *t, void *ignored)
{
    CF_CFDP_ResetTransaction(t, 0);
}

/************************************************************************/
/** \brief Handle an abandon ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdAbandon(CFE_SB_Buffer_t *msg)
{
    CF_CmdCond(CF_TsnChanAction((cf_cmd_transaction_t *)msg, "abandon", CF_CmdAbandon_, NULL));
}

/************************************************************************/
/** \brief Sets the dequeue enable/disable flag for a channel.
**
**  \par Assumptions, External Events, and Notes:
**       context must not be NULL.
**
**  \returns
**  \retstmt Always succeeds, so returns 0. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_DoEnableDisableDequeue(uint8 chan_num, const bool_arg_t *context)
{
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.config_table->chan[chan_num].dequeue_enabled = context->barg;
    return 0;
}

/************************************************************************/
/** \brief Handle an enable dequeue ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdEnableDequeue(CFE_SB_Buffer_t *msg)
{
    bool_arg_t barg = {1};
    CF_CmdCond(CF_DoChanAction((cf_cmd_unionargs_t *)msg, "enable_dequeue", (chan_action_fn_t)CF_DoEnableDisableDequeue,
                               &barg));
}

/************************************************************************/
/** \brief Handle a disable dequeue ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdDisableDequeue(CFE_SB_Buffer_t *msg)
{
    bool_arg_t barg = {0};
    CF_CmdCond(CF_DoChanAction((cf_cmd_unionargs_t *)msg, "disable_dequeue",
                               (chan_action_fn_t)CF_DoEnableDisableDequeue, &barg));
}

/************************************************************************/
/** \brief Sets the enable/disable flag for the specified polling directory.
**
**  \par Assumptions, External Events, and Notes:
**       context must not be NULL.
**
*************************************************************************/
static int CF_DoEnableDisablePolldir(uint8 chan_num, const bool_msg_arg_t *context)
{
    int ret = 0;
    /* no need to bounds check chan_num, done in caller */
    if (context->msg->data.byte[1] == ALL_POLLDIRS)
    {
        /* all polldirs in channel */
        int i;
        for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
            CF_AppData.config_table->chan[chan_num].polldir[i].enabled = context->barg;
    }
    else if (context->msg->data.byte[1] < CF_MAX_POLLING_DIR_PER_CHAN)
    {
        CF_AppData.config_table->chan[chan_num].polldir[context->msg->data.byte[1]].enabled = context->barg;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_POLLDIR_INVALID, CFE_EVS_EventType_ERROR,
                          "CF: enable/disable polldir: invalid polldir %d on channel %d", context->msg->data.byte[1],
                          chan_num);
        ret = -1;
    }

    return ret;
}

/************************************************************************/
/** \brief Enable a polling dir ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdEnablePolldir(CFE_SB_Buffer_t *msg)
{
    bool_msg_arg_t barg = {(cf_cmd_unionargs_t *)msg, 1};
    CF_CmdCond(CF_DoChanAction((cf_cmd_unionargs_t *)msg, "enable_polldir", (chan_action_fn_t)CF_DoEnableDisablePolldir,
                               &barg));
}

/************************************************************************/
/** \brief Disable a polling dir ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdDisablePolldir(CFE_SB_Buffer_t *msg)
{
    bool_msg_arg_t barg = {(cf_cmd_unionargs_t *)msg, 0};
    CF_CmdCond(CF_DoChanAction((cf_cmd_unionargs_t *)msg, "disable_polldir",
                               (chan_action_fn_t)CF_DoEnableDisablePolldir, &barg));
}

/************************************************************************/
/** \brief Purge the history queue for the given channel.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL. c must not be NULL.
**
*************************************************************************/
static int CF_PurgeHistory(clist_node n, channel_t *c)
{
    history_t *h = container_of(n, history_t, cl_node);
    CF_CFDP_ResetHistory(c, h); /* ok to reset transaction since it's in PEND it hasn't started yet */
    return CLIST_CONT;
}

/************************************************************************/
/** \brief Purge the pending transaction queue.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL.
**
*************************************************************************/
static int CF_PurgeTransaction(clist_node n, void *ignored)
{
    transaction_t *t = container_of(n, transaction_t, cl_node);
    CF_CFDP_ResetTransaction(t, 0);
    return CLIST_CONT;
}

/************************************************************************/
/** \brief Channel action command to perform purge queue operations.
**
**  \par Description
**       Determines from the command parameters which queues to traverse
**       and purge state.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt 0 on success; anything else on error. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_DoPurgeQueue(uint8 chan_num, cf_cmd_unionargs_t *cmd)
{
    int ret = 0;
    /* no need to bounds check chan_num, done in caller */
    channel_t *c = &CF_AppData.engine.channels[chan_num];

    int pend = 0;
    int hist = 0;

    switch (cmd->data.byte[1])
    {
        case 0: /* pend */
            pend = 1;
            break;

        case 1: /* history */
            hist = 1;
            break;

        case 2: /* both */
            pend = 1;
            hist = 1;
            break;

        default:
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_PURGE_ARG, CFE_EVS_EventType_ERROR, "CF: purge queue invalid arg %d",
                              cmd->data.byte[1]);
            ret = -1;
            break;
    }

    if (pend)
    {
        CF_CList_Traverse(c->qs[CF_Q_PEND], CF_PurgeTransaction, NULL);
    }

    if (hist)
    {
        CF_CList_Traverse(c->qs[CF_Q_HIST], (clist_fn_t)CF_PurgeHistory, c);
    }

    return ret;
}

/************************************************************************/
/** \brief Ground command to purge either the history or pending queues.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdPurgeQueue(CFE_SB_Buffer_t *msg)
{
    CF_CmdCond(CF_DoChanAction((cf_cmd_unionargs_t *)msg, "purge_queue", (chan_action_fn_t)CF_DoPurgeQueue, msg));
}

/************************************************************************/
/** \brief Ground command to write a file with queue information.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdWriteQueue(CFE_SB_Buffer_t *msg)
{
    /* a type value of 0 means to process both type_up and type_down */
    static const int  type_up   = 1;
    static const int  type_down = 2;
    static const int  q_pend    = 0;
    static const int  q_active  = 1;
    static const int  q_history = 2;
    static const int  q_all     = 3;
    cf_cmd_write_q_t *wq        = (cf_cmd_write_q_t *)msg;
    channel_t        *c         = &CF_AppData.engine.channels[wq->chan];
    osal_id_t         fd;
    int32             ret;

    /* check the commands for validity */
    if (wq->chan >= CF_NUM_CHANNELS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_CHAN, CFE_EVS_EventType_ERROR, "CF: write queue invalid channel arg");
        goto bail;
    }

    /* only invalid combination is up direction, pending queue */
    if ((wq->type == type_up) && (wq->queue == q_pend))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_ARGS, CFE_EVS_EventType_ERROR,
                          "CF: write queue invalid command parameters");
        goto bail;
    }

    /* PTFO: queues can be large. may want to split this work up across the state machine and take several wakeups to
     * complete */
    ret = CF_WrappedOpenCreate(&fd, wq->filename, OS_FILE_FLAG_CREATE, OS_WRITE_ONLY);
    if (ret < 0)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_OPEN, CFE_EVS_EventType_ERROR, "CF: write queue failed to open file %s",
                          wq->filename);
        goto bail;
    }

    /* if type is type_up, or all types */
    if (!wq->type || (wq->type == type_up))
    {
        /* process uplink queue data */
        if ((wq->queue == q_all) || (wq->queue == q_active))
        {
            ret = CF_WriteQueueDataToFile(fd, c, CF_Q_RX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEQ_RX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_Q_RX data");
                goto out_close;
            }
        }

        if ((wq->queue == q_all) || (wq->queue == q_history))
        {
            ret = CF_WriteHistoryQueueDataToFile(fd, c, CF_DIR_RX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEHIST_RX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write history RX data");
                goto out_close;
            }
        }
    }

    /* if type is type_down, or all types */
    if (!wq->type || (wq->type == type_down))
    {
        /* process downlink queue data */
        if ((wq->queue == q_all) || (wq->queue == q_active))
        {
            int              i;
            static const int qs[2] = {CF_Q_TXA, CF_Q_TXW};
            for (i = 0; i < 2; ++i)
            {
                ret = CF_WriteQueueDataToFile(fd, c, qs[i]);
                if (ret)
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEQ_TX, CFE_EVS_EventType_ERROR,
                                      "CF: write queue failed to write q index %d", qs[i]);
                    goto out_close;
                }
            }
        }

        if ((wq->queue == q_all) || (wq->queue == q_pend))
        {
            /* write pending queue */
            ret = CF_WriteQueueDataToFile(fd, c, CF_Q_PEND);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEQ_PEND, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write pending queue");
                goto out_close;
            }
        }

        if ((wq->queue == q_all) || (wq->queue == q_history))
        {
            /* write history queue */
            ret = CF_WriteHistoryQueueDataToFile(fd, c, CF_DIR_TX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEHIST_TX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_Q_TX data");
                goto out_close;
            }
        }
    }

    CF_CmdAcc();
    return;

out_close:
    CF_WrappedClose(fd);

bail:
    CF_CmdRej();
}

/************************************************************************/
/** \brief Ground command to send configuration parameters.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdSendCfgParams(CFE_SB_Buffer_t *msg)
{
    CF_AppData.cfg.ticks_per_second             = CF_AppData.config_table->ticks_per_second;
    CF_AppData.cfg.rx_crc_calc_bytes_per_wakeup = CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup;
    CF_AppData.cfg.ack_timer_s                  = CF_AppData.config_table->ack_timer_s;
    CF_AppData.cfg.nak_timer_s                  = CF_AppData.config_table->nak_timer_s;
    CF_AppData.cfg.inactivity_timer_s           = CF_AppData.config_table->inactivity_timer_s;
    CF_AppData.cfg.outgoing_file_chunk_size     = CF_AppData.config_table->outgoing_file_chunk_size;
    CF_AppData.cfg.ack_limit                    = CF_AppData.config_table->ack_limit;
    CF_AppData.cfg.nak_limit                    = CF_AppData.config_table->nak_limit;
    CF_AppData.cfg.local_eid                    = CF_AppData.config_table->local_eid;

    CFE_MSG_SetMsgTime(&CF_AppData.cfg.tlm_header.Msg, CFE_TIME_GetTime());
    /* return value ignored */ CFE_SB_TransmitMsg(&CF_AppData.cfg.tlm_header.Msg, true);

    CF_CmdAcc();
}

/************************************************************************/
/** \brief Checks if the value is less than or equal to the max pdu size.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt 0 if success, 1 if failed. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CmdValidateChunkSize(uint32 val, uint8 chan_num /* ignored */)
{
    int ret = 0;
    if (val > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        ret = 1; /* failed */
    }
    return ret;
}

/************************************************************************/
/** \brief Checks if the value is
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt 0 if success, 1 if failed. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CmdValidateMaxOutgoing(uint32 val, uint8 chan_num)
{
    int ret = 0;

    if (!val && !CF_AppData.config_table->chan[chan_num].sem_name[0])
    {
        /* can't have unlimited messages and no semaphore */
        ret = 1; /* failed */
    }

    return ret;
}

/************************************************************************/
/** \brief Perform a configuration get/set operation.
**
**  \par Description
**       Combine get and set in one function with common logic.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
/* combine getset into a single function with a branch to avoid wasted memory footprint with duplicate
 * logic for finding the parameter */
static void CF_CmdGetSetParam(uint8 is_set, uint8 param_id, uint32 value, uint8 chan_num)
{
    /* These macros define entries into the paramater array. The mapping of the array is
     * ground parameter to configuration parameter. This logic allows a simple access
     * of the configuration parameter or a check on validity of the parameter and then
     * access. */
#define SPTR(x)                                                               \
    {                                                                         \
        &CF_AppData.config_table->x, sizeof(CF_AppData.config_table->x), NULL \
    }
#define SPTRFN(x, fn)                                                       \
    {                                                                       \
        &CF_AppData.config_table->x, sizeof(CF_AppData.config_table->x), fn \
    }
#define CPTRFN(x, fn)                                                                                     \
    {                                                                                                     \
        &CF_AppData.config_table->chan[chan_num].x, sizeof(CF_AppData.config_table->chan[chan_num].x), fn \
    }
    const struct
    {
        void  *ptr;
        uint32 size;
        int (*fn)(uint32, uint8 chan_num);
    } items[CF_NUM_CFG_PACKET_ITEMS] = {
        SPTR(ticks_per_second),   SPTR(rx_crc_calc_bytes_per_wakeup),
        SPTR(ack_timer_s),        SPTR(nak_timer_s),
        SPTR(inactivity_timer_s), SPTRFN(outgoing_file_chunk_size, CF_CmdValidateChunkSize),
        SPTR(ack_limit),          SPTR(nak_limit),
        SPTR(local_eid),          CPTRFN(max_outgoing_messages_per_wakeup, CF_CmdValidateMaxOutgoing)};

    int acc = 1; /* 1 means to reject */

#if ENDIAN == _EB
    static const int shift_map[5] = {0, 24, 16, 8, 0};
#endif

    if (param_id >= CF_NUM_CFG_PACKET_ITEMS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_GETSET_PARAM, CFE_EVS_EventType_ERROR,
                          "CF: invalid configuration parameter id %d received", param_id);
        goto err_out;
    }

    if (chan_num >= CF_NUM_CHANNELS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_GETSET_CHAN, CFE_EVS_EventType_ERROR,
                          "CF: invalid configuration channel id %d received", chan_num);
        goto err_out;
    }

    if (is_set)
    {
        int valid_set = 0;

        if (items[param_id].fn)
        {
            if (!items[param_id].fn(value, chan_num))
            {
                valid_set = 1;
            }
            else
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_GETSET_VALIDATE, CFE_EVS_EventType_ERROR,
                                  "CF: validation for parameter id %d failed", param_id);
            }
        }
        else
        {
            valid_set = 1;
        }

        if (valid_set)
        {
            CFE_EVS_SendEvent(CF_EID_INF_CMD_GETSET1, CFE_EVS_EventType_INFORMATION,
                              "CF: setting parameter id %d to %d", param_id, value);
#if ENDIAN == _EB
            CF_Assert((items[param_id].size > 0) && (items[param_id].size < 5));
            value <<= shift_map[items[param_id].size];
#endif
            memcpy(items[param_id].ptr, &value, items[param_id].size);
            acc = 0;
        }
    }
    else
    {
        memcpy(&value, items[param_id].ptr, items[param_id].size);
#if ENDIAN == _EB
        CF_Assert((items[param_id].size > 0) && (items[param_id].size < 5));
        value >>= shift_map[items[param_id].size];
#endif
        CFE_EVS_SendEvent(CF_EID_INF_CMD_GETSET2, CFE_EVS_EventType_INFORMATION, "CF: parameter id %d = %d", param_id,
                          value);
        acc = 0;
    }

err_out:
    CF_CmdCond(acc);
}

/************************************************************************/
/** \brief Ground command to set a configuration parameter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdSetParam(CFE_SB_Buffer_t *msg)
{
    cf_cmd_set_param_args_t *cmd = (cf_cmd_set_param_args_t *)msg;
    CF_CmdGetSetParam(1, cmd->key, cmd->value, cmd->chan_num);
}

/************************************************************************/
/** \brief Ground command to set a configuration parameter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdGetParam(CFE_SB_Buffer_t *msg)
{
    cf_cmd_get_param_args_t *cmd = (cf_cmd_get_param_args_t *)msg;
    CF_CmdGetSetParam(0, cmd->key, 0, cmd->chan_num);
}

/************************************************************************/
/** \brief Ground command enable engine.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdEnableEngine(CFE_SB_Buffer_t *msg)
{
    if (!CF_AppData.engine.enabled)
    {
        if (CF_CFDP_InitEngine() == CFE_SUCCESS)
        {
            CF_CmdAcc();
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENABLE_ENGINE, CFE_EVS_EventType_ERROR,
                              "CF: failed to re-initialize and enable CFDP engine");
            CF_CmdRej();
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENG_ALREADY_ENA, CFE_EVS_EventType_ERROR,
                          "CF: received enable engine command while engine already enabled");
        CF_CmdRej();
    }
}

/************************************************************************/
/** \brief Ground command disable engine.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_CmdDisableEngine(CFE_SB_Buffer_t *msg)
{
    if (CF_AppData.engine.enabled)
    {
        CF_CFDP_DisableEngine();
        CF_CmdAcc();
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENG_ALREADY_DIS, CFE_EVS_EventType_ERROR,
                          "CF: received disable engine command while engine already disabled");
        CF_CmdRej();
    }
}

/************************************************************************/
/** \brief Process any ground command contained in the given message.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
void CF_ProcessGroundCommand(CFE_SB_Buffer_t *msg)
{
    static void (*const fns[CF_NUM_COMMANDS])(CFE_SB_Buffer_t *) = {
        CF_CmdNoop,        /* CF_NOOP_CC */
        CF_CmdReset,       /* CF_RESET_CC */
        CF_CmdTxFile,      /* CF_TX_FILE_CC */
        CF_CmdPlaybackDir, /* CF_PLAYBACK_DIR_CC */
        CF_CmdFreeze,      /* CF_FREEZE_CC */
        CF_CmdThaw,        /* CF_THAW_CC */
        CF_CmdSuspend,     /* CF_SUSPEND_CC */
        CF_CmdResume,      /* CF_RESUME_CC */
        CF_CmdCancel,      /* CF_CANCEL_CC */
        CF_CmdAbandon,     /* CF_ABANDON_CC */
        CF_CmdSetParam,    /* CF_SET_MIB_PARAM_CC */
        CF_CmdGetParam,    /* CF_GET_MIB_PARAM_CC */
        NULL,
        NULL,
        CF_CmdSendCfgParams,  /* CF_SEND_CFG_PARAMS_CC */
        CF_CmdWriteQueue,     /* CF_WRITE_QUEUE_CC */
        CF_CmdEnableDequeue,  /* CF_ENABLE_DEQUEUE_CC */
        CF_CmdDisableDequeue, /* CF_DISABLE_DEQUEUE_CC */
        CF_CmdEnablePolldir,  /* CF_ENABLE_DIR_POLLING_CC */
        CF_CmdDisablePolldir, /* CF_DISABLE_DIR_POLLING_CC */
        NULL,                 /* CF_DELETE_QUEUE_NODE_CC */
        CF_CmdPurgeQueue,     /* CF_PURGE_QUEUE_CC */
        CF_CmdEnableEngine,   /* CF_ENABLE_ENGINE_CC */
        CF_CmdDisableEngine,  /* CF_DISABLE_ENGINE_CC */
    };

    static const uint16 expected_lengths[CF_NUM_COMMANDS] = {
        sizeof(cf_cmd_noargs_t),         /* CF_NOOP_CC */
        sizeof(cf_cmd_unionargs_t),      /* CF_RESET_CC */
        sizeof(cf_cmd_tx_file_t),        /* CF_TX_FILE_CC */
        sizeof(cf_cmd_playback_dir_t),   /* CF_PLAYBACK_DIR_CC */
        sizeof(cf_cmd_unionargs_t),      /* CF_FREEZE_CC */
        sizeof(cf_cmd_unionargs_t),      /* CF_THAW_CC */
        sizeof(cf_cmd_transaction_t),    /* CF_SUSPEND_CC */
        sizeof(cf_cmd_transaction_t),    /* CF_RESUME_CC */
        sizeof(cf_cmd_transaction_t),    /* CF_CANCEL_CC */
        sizeof(cf_cmd_transaction_t),    /* CF_ABANDON_CC */
        sizeof(cf_cmd_set_param_args_t), /* CF_SET_MIB_PARAM_CC */
        sizeof(cf_cmd_get_param_args_t), /* CF_GET_MIB_PARAM_CC */
        0,
        0,
        sizeof(cf_cmd_noargs_t),    /* CF_SEND_CFG_PARAMS_CC */
        sizeof(cf_cmd_write_q_t),   /* CF_WRITE_QUEUE_CC */
        sizeof(cf_cmd_unionargs_t), /* CF_ENABLE_DEQUEUE_CC */
        sizeof(cf_cmd_unionargs_t), /* CF_DISABLE_DEQUEUE_CC */
        sizeof(cf_cmd_unionargs_t), /* CF_ENABLE_DIR_POLLING_CC */
        sizeof(cf_cmd_unionargs_t), /* CF_DISABLE_DIR_POLLING_CC */
        0,                          /* CF_DELETE_QUEUE_NODE_CC */
        sizeof(cf_cmd_unionargs_t), /* CF_PURGE_QUEUE_CC */
        sizeof(cf_cmd_noargs_t),    /* CF_ENABLE_ENGINE_CC */
        sizeof(cf_cmd_noargs_t),    /* CF_DISABLE_ENGINE_CC */
    };

    CFE_MSG_FcnCode_t cmd;
    size_t            len;

    CFE_MSG_GetFcnCode(&msg->Msg, &cmd);

    if (cmd < CF_NUM_COMMANDS)
    {
        CFE_MSG_GetSize(&msg->Msg, &len);

        /* first, verify command length */
        if (len == expected_lengths[cmd])
        {
            /* if valid, process command */
            if (fns[cmd])
            {
                fns[cmd](msg);
            }
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_GCMD_LEN, CFE_EVS_EventType_ERROR,
                              "CF: invalid ground command length for command 0x%02x, expected %d got %zd", cmd,
                              expected_lengths[cmd], len);
            CF_CmdRej();
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_GCMD_CC, CFE_EVS_EventType_ERROR,
                          "CF: invalid ground command packet cmd_code=0x%02x", cmd);
        CF_CmdRej();
    }
}
