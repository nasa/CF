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
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_utils.h"
#include "cf_version.h"
#include "cf_platform_cfg.h"

#include "cf_cfdp.h"
#include "cf_cmd.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_NoopCmd(const CF_NoopCmd_t *msg)
{
    CFE_EVS_SendEvent(CF_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: No-Op received, Version %d.%d.%d.%d",
                      CF_MAJOR_VERSION, CF_MINOR_VERSION, CF_REVISION, CF_MISSION_REV);

    ++CF_AppData.hk.Payload.counters.cmd;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ResetCmd(const CF_ResetCmd_t *msg)
{
    const CF_UnionArgs_Payload_t *data     = &msg->Payload;
    static const char *           names[5] = {"all", "cmd", "fault", "up", "down"};
    /* 0=all, 1=cmd, 2=fault 3=up 4=down */
    uint8 param = data->byte[0];
    int   i;
    int   acc = 1;

    if (param > 4)
    {
        CFE_EVS_SendEvent(CF_CMD_RESET_INVALID_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: Received RESET COUNTERS command with invalid parameter %d", param);
        ++CF_AppData.hk.Payload.counters.err;
    }
    else
    {
        CFE_EVS_SendEvent(CF_RESET_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: Received RESET COUNTERS command: %s",
                          names[param]);

        /* if the param is CF_Reset_command, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_command))
        {
            /* command counters */
            memset(&CF_AppData.hk.Payload.counters, 0, sizeof(CF_AppData.hk.Payload.counters));
            acc = 0; /* don't increment accept counter on command counter reset */
        }

        /* if the param is CF_Reset_fault, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_fault))
        {
            /* fault counters */
            for (i = 0; i < CF_NUM_CHANNELS; ++i)
                memset(&CF_AppData.hk.Payload.channel_hk[i].counters.fault, 0,
                       sizeof(CF_AppData.hk.Payload.channel_hk[i].counters.fault));
        }

        /* if the param is CF_Reset_up, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_up))
        {
            /* up counters */
            for (i = 0; i < CF_NUM_CHANNELS; ++i)
                memset(&CF_AppData.hk.Payload.channel_hk[i].counters.recv, 0,
                       sizeof(CF_AppData.hk.Payload.channel_hk[i].counters.recv));
        }

        /* if the param is CF_Reset_down, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_down))
        {
            /* down counters */
            for (i = 0; i < CF_NUM_CHANNELS; ++i)
                memset(&CF_AppData.hk.Payload.channel_hk[i].counters.sent, 0,
                       sizeof(CF_AppData.hk.Payload.channel_hk[i].counters.sent));
        }

        if (acc)
        {
            ++CF_AppData.hk.Payload.counters.cmd;
        }
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_TxFileCmd(const CF_TxFileCmd_t *msg)
{
    const CF_TxFile_Payload_t *tx = &msg->Payload;

    /*
     * This needs to validate all its inputs.
     * "keep" should only be 0 or 1 (logical true/false).
     * For priority and dest_id params, anything is acceptable.
     */
    if ((tx->cfdp_class != CF_CFDP_CLASS_1 && tx->cfdp_class != CF_CFDP_CLASS_2) || tx->chan_num >= CF_NUM_CHANNELS ||
        (int)tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_CMD_BAD_PARAM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_TxFileCmd(): chan=%u, class=%u keep=%u", (unsigned int)tx->chan_num,
                          (unsigned int)tx->cfdp_class, (unsigned int)tx->keep);
        ++CF_AppData.hk.Payload.counters.err;

        /* This must return CFE_SUCCESS because the command is done (error counter was incremented, no more events) */
        return CFE_SUCCESS;
    }

#ifdef jphfix
    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;
#endif

    if (CF_CFDP_TxFile(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num, tx->priority,
                       tx->dest_id) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_TX_FILE_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "CF: file transfer successfully initiated");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_TX_FILE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: file transfer initiation failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PlaybackDirCmd(const CF_PlaybackDirCmd_t *msg)
{
    const CF_TxFile_Payload_t *tx = &msg->Payload;

    /*
     * This needs to validate all its inputs.
     * "keep" should only be 0 or 1 (logical true/false).
     * For priority and dest_id params, anything is acceptable.
     */
    if ((tx->cfdp_class != CF_CFDP_CLASS_1 && tx->cfdp_class != CF_CFDP_CLASS_2) || tx->chan_num >= CF_NUM_CHANNELS ||
        (int)tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_CMD_BAD_PARAM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_PlaybackDirCmd(): chan=%u, class=%u keep=%u",
                          (unsigned int)tx->chan_num, (unsigned int)tx->cfdp_class, (unsigned int)tx->keep);
        ++CF_AppData.hk.Payload.counters.err;

        /* This must return CFE_SUCCESS because the command is done (error counter was incremented, no more events) */
        return CFE_SUCCESS;
    }

#ifdef jphfix
    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;
#endif

    if (CF_CFDP_PlaybackDir(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num, tx->priority,
                            tx->dest_id) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_CMD_PLAYBACK_DIR_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "CF: directory playback initiation successful");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_PLAYBACK_DIR_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: directory playback initiation failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChanAction_Status_t CF_DoChanAction(const CF_UnionArgs_Payload_t *data, const char *errstr, CF_ChanActionFn_t fn,
                                       void *context)
{
    int                    i;
    CF_ChanAction_Status_t ret = CF_ChanAction_Status_SUCCESS;

    /* this function is generic for any ground command that takes a single channel
     * argument which must be less than CF_NUM_CHANNELS or 255 which is a special
     * value that means apply command to all channels */
    if (data->byte[0] == CF_ALL_CHANNELS)
    {
        /* apply to all channels */
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
            ret |= fn(i, context);
    }
    else if (data->byte[0] < CF_NUM_CHANNELS)
    {
        ret = fn(data->byte[0], context);
    }
    else
    {
        /* bad parameter */
        CFE_EVS_SendEvent(CF_CMD_CHAN_PARAM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: %s: channel parameter out of range. received %d", errstr, data->byte[0]);
        ret = CF_ChanAction_Status_ERROR;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChanAction_Status_t CF_DoFreezeThaw(uint8 chan_num, void *arg)
{
    const CF_ChanAction_BoolArg_t *context = arg;
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.hk.Payload.channel_hk[chan_num].frozen = context->barg;
    return CF_ChanAction_Status_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_FreezeCmd(const CF_FreezeCmd_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {1}; /* param is frozen, so 1 means freeze */

    if (CF_ChanAction_Status_IS_SUCCESS(CF_DoChanAction(&msg->Payload, "freeze", CF_DoFreezeThaw, &barg)))
    {
        CFE_EVS_SendEvent(CF_CMD_FREEZE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: freeze successful");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_FREEZE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: freeze cmd failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ThawCmd(const CF_ThawCmd_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {0}; /* param is frozen, so 0 means thawed */

    if (CF_ChanAction_Status_IS_SUCCESS(CF_DoChanAction(&msg->Payload, "thaw", CF_DoFreezeThaw, &barg)))
    {
        CFE_EVS_SendEvent(CF_CMD_THAW_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: thaw successful");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_THAW_ERR_EID, CFE_EVS_EventType_ERROR, "CF: thaw cmd failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Transaction_t *CF_FindTransactionBySequenceNumberAllChannels(CF_TransactionSeq_t ts, CF_EntityId_t eid)
{
    int               i;
    CF_Transaction_t *ret = NULL;

    /* transaction sequence numbers are referenced with the tuple (eid, tsn)
     * Even though these tuples are unique to a channel, they should be unique
     * across the entire system. Meaning, it isn't correct to have the same
     * EID re-using a TSN in the same system. So, in order to locate the transaction
     * to suspend, we need to search across all channels for it. */
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        ret = CF_FindTransactionBySequenceNumber(CF_AppData.engine.channels + i, ts, eid);
        if (ret)
        {
            break;
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_TsnChanAction(const CF_Transaction_Payload_t *data, const char *cmdstr, CF_TsnChanAction_fn_t fn,
                       void *context)
{
    CF_Transaction_t *txn;
    int32             ret = -1;

    if (data->chan == CF_COMPOUND_KEY)
    {
        /* special value 254 means to use the compound key (data->eid, data->ts) to find the transaction
         * to act upon */
        txn = CF_FindTransactionBySequenceNumberAllChannels(data->ts, data->eid);
        if (txn)
        {
            fn(txn, context);
            ret = 1; /* because one transaction was matched - this should return a count */
        }
        else
        {
            CFE_EVS_SendEvent(CF_CMD_TRANS_NOT_FOUND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: %s cmd: failed to find transaction for (eid %lu, ts %lu)", cmdstr,
                              (unsigned long)data->eid, (unsigned long)data->ts);
        }
    }
    else if (data->chan == CF_ALL_CHANNELS)
    {
        /* perform action on all channels, all transactions */
        ret = CF_TraverseAllTransactions_All_Channels(fn, context);
    }
    else if (data->chan < CF_NUM_CHANNELS)
    {
        /* perform action on a specific channel, all transactions */
        ret = CF_TraverseAllTransactions(CF_AppData.engine.channels + data->chan, fn, context);
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_TSN_CHAN_INVALID_ERR_EID, CFE_EVS_EventType_ERROR, "CF: %s cmd: invalid channel %d",
                          cmdstr, data->chan);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_DoSuspRes_Txn(CF_Transaction_t *txn, CF_ChanAction_SuspResArg_t *context)
{
    CF_Assert(txn);
    if (txn->flags.com.suspended == context->action)
    {
        context->same = 1;
    }
    else
    {
        txn->flags.com.suspended = context->action;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_DoSuspRes(const CF_Transaction_Payload_t *payload, uint8 action)
{
    /* ok to not bounds check action, because the caller is using it in two places with constant values 0 or 1 */
    static const char *        msgstr[] = {"resume", "suspend"};
    CF_ChanAction_SuspResArg_t args     = {0, action};
    int ret = CF_TsnChanAction(payload, msgstr[action], (CF_TsnChanAction_fn_t)CF_DoSuspRes_Txn, &args);

    /*
     * Note that this command may affect multiple transactions, depending on the value of the "chan" argument.
     * When acting on multiple channels, the "same" output does not apply.  In reality all it means is
     * that one of the affected channels was already set that way.
     */

    if (ret == 1 && args.same)
    {
        /* A single transaction was mached, and it was already set the same way */
        CFE_EVS_SendEvent(CF_CMD_SUSPRES_SAME_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: %s cmd: setting suspend flag to current value of %d", msgstr[action], action);
        ++CF_AppData.hk.Payload.counters.err;
    }
    else if (ret <= 0)
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_CMD_SUSPRES_CHAN_ERR_EID, CFE_EVS_EventType_ERROR, "CF: %s cmd: no transaction found",
                          msgstr[action]);
        ++CF_AppData.hk.Payload.counters.err;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_SUSPRES_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "CF: %s cmd: setting suspend flag to %d", msgstr[action], action);
        ++CF_AppData.hk.Payload.counters.cmd;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SuspendCmd(const CF_SuspendCmd_t *msg)
{
    CF_DoSuspRes(&msg->Payload, 1);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ResumeCmd(const CF_ResumeCmd_t *msg)
{
    CF_DoSuspRes(&msg->Payload, 0);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdCancel_Txn(CF_Transaction_t *txn, void *ignored)
{
    CF_CFDP_CancelTransaction(txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CancelCmd(const CF_CancelCmd_t *msg)
{
    if (CF_TsnChanAction(&msg->Payload, "cancel", CF_CmdCancel_Txn, NULL) > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_CANCEL_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "CF: cancel transaction successfully initiated");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_CMD_CANCEL_CHAN_ERR_EID, CFE_EVS_EventType_ERROR, "CF: cancel cmd: no transaction found");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdAbandon_Txn(CF_Transaction_t *txn, void *ignored)
{
    CF_CFDP_ResetTransaction(txn, 0);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_AbandonCmd(const CF_AbandonCmd_t *msg)
{
    if (CF_TsnChanAction(&msg->Payload, "abandon", CF_CmdAbandon_Txn, NULL) > 0)
    {
        CFE_EVS_SendEvent(CF_CMD_ABANDON_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: abandon successful");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_CMD_ABANDON_CHAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: abandon cmd: no transaction found");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChanAction_Status_t CF_DoEnableDisableDequeue(uint8 chan_num, void *arg)
{
    const CF_ChanAction_BoolArg_t *context = arg;
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.config_table->chan[chan_num].dequeue_enabled = context->barg;
    return CF_ChanAction_Status_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableDequeueCmd(const CF_EnableDequeueCmd_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {1};

    if (CF_ChanAction_Status_IS_SUCCESS(
            CF_DoChanAction(&msg->Payload, "enable_dequeue", CF_DoEnableDisableDequeue, &barg)))
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_DEQUEUE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: dequeue enabled");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_DEQUEUE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: enable dequeue cmd failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableDequeueCmd(const CF_DisableDequeueCmd_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {0};

    if (CF_ChanAction_Status_IS_SUCCESS(
            CF_DoChanAction(&msg->Payload, "disable_dequeue", CF_DoEnableDisableDequeue, &barg)))
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_DEQUEUE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: dequeue disabled");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_DEQUEUE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: disable dequeue cmd failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChanAction_Status_t CF_DoEnableDisablePolldir(uint8 chan_num, void *arg)
{
    int                               i;
    const CF_ChanAction_BoolMsgArg_t *context = arg;
    CF_ChanAction_Status_t            ret     = CF_ChanAction_Status_SUCCESS;
    /* no need to bounds check chan_num, done in caller */
    if (context->data->byte[1] == CF_ALL_POLLDIRS)
    {
        /* all polldirs in channel */
        for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
            CF_AppData.config_table->chan[chan_num].polldir[i].enabled = context->barg;
    }
    else if (context->data->byte[1] < CF_MAX_POLLING_DIR_PER_CHAN)
    {
        CF_AppData.config_table->chan[chan_num].polldir[context->data->byte[1]].enabled = context->barg;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_POLLDIR_INVALID_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: enable/disable polldir: invalid polldir %d on channel %d", context->data->byte[1],
                          chan_num);
        ret = CF_ChanAction_Status_ERROR;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableDirPollingCmd(const CF_EnableDirPollingCmd_t *msg)
{
    CF_ChanAction_BoolMsgArg_t barg = {&msg->Payload, 1};

    if (CF_ChanAction_Status_IS_SUCCESS(
            CF_DoChanAction(&msg->Payload, "enable_polldir", CF_DoEnableDisablePolldir, &barg)))
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_POLLDIR_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "CF: enabled polling directory");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENABLE_POLLDIR_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: enable polling directory cmd failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableDirPollingCmd(const CF_DisableDirPollingCmd_t *msg)
{
    CF_ChanAction_BoolMsgArg_t barg = {&msg->Payload, 0};

    if (CF_ChanAction_Status_IS_SUCCESS(
            CF_DoChanAction(&msg->Payload, "disable_polldir", CF_DoEnableDisablePolldir, &barg)))
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_POLLDIR_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "CF: disabled polling directory");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_DISABLE_POLLDIR_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: disable polling directory cmd failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListTraverse_Status_t CF_PurgeHistory(CF_CListNode_t *node, void *arg)
{
    CF_Channel_t *chan    = arg;
    CF_History_t *history = container_of(node, CF_History_t, cl_node);
    CF_ResetHistory(chan, history); /* ok to reset transaction since it's in PEND it hasn't started yet */
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListTraverse_Status_t CF_PurgeTransaction(CF_CListNode_t *node, void *ignored)
{
    CF_Transaction_t *txn = container_of(node, CF_Transaction_t, cl_node);
    CF_CFDP_ResetTransaction(txn, 0);
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChanAction_Status_t CF_DoPurgeQueue(uint8 chan_num, void *arg)
{
    CF_ChanAction_Status_t ret = CF_ChanAction_Status_SUCCESS;
    /* no need to bounds check chan_num, done in caller */
    CF_Channel_t *                chan = &CF_AppData.engine.channels[chan_num];
    const CF_UnionArgs_Payload_t *data = ((CF_ChanAction_MsgArg_t *)arg)->data;

    int pend = 0;
    int hist = 0;

    switch (data->byte[1])
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
            CFE_EVS_SendEvent(CF_CMD_PURGE_ARG_ERR_EID, CFE_EVS_EventType_ERROR, "CF: purge queue invalid arg %d",
                              data->byte[1]);
            ret = CF_ChanAction_Status_ERROR;
            break;
    }

    if (pend)
    {
        CF_CList_Traverse(chan->qs[CF_QueueIdx_PEND], CF_PurgeTransaction, NULL);
    }

    if (hist)
    {
        CF_CList_Traverse(chan->qs[CF_QueueIdx_HIST], CF_PurgeHistory, chan);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PurgeQueueCmd(const CF_PurgeQueueCmd_t *msg)
{
    CF_ChanAction_MsgArg_t arg = {&msg->Payload};
    if (CF_ChanAction_Status_IS_SUCCESS(CF_DoChanAction(&msg->Payload, "purge_queue", CF_DoPurgeQueue, &arg)))
    {
        CFE_EVS_SendEvent(CF_CMD_PURGE_QUEUE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: queue purged");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_PURGE_QUEUE_ERR_EID, CFE_EVS_EventType_ERROR, "CF: purge queue cmd failed");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WriteQueueCmd(const CF_WriteQueueCmd_t *msg)
{
    const CF_WriteQueue_Payload_t *wq = &msg->Payload;

    CF_Channel_t *chan    = &CF_AppData.engine.channels[wq->chan];
    osal_id_t     fd      = OS_OBJECT_ID_UNDEFINED;
    bool          success = true;
    int32         ret;

    /* check the commands for validity */
    if (wq->chan >= CF_NUM_CHANNELS)
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_CHAN_ERR_EID, CFE_EVS_EventType_ERROR, "CF: write queue invalid channel arg");
        ++CF_AppData.hk.Payload.counters.err;
        success = false;
    }
    /* only invalid combination is up direction, pending queue */
    else if ((wq->type == CF_Type_up) && (wq->queue == CF_Queue_pend))
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_ARGS_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: write queue invalid command parameters");
        ++CF_AppData.hk.Payload.counters.err;
        success = false;
    }
    else
    {
        /* PTFO: queues can be large. may want to split this work up across the state machine and take several wakeups
         * to complete */
        ret = CF_WrappedOpenCreate(&fd, wq->filename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
        if (ret < 0)
        {
            CFE_EVS_SendEvent(CF_CMD_WQ_OPEN_ERR_EID, CFE_EVS_EventType_ERROR, "CF: write queue failed to open file %s",
                              wq->filename);
            ++CF_AppData.hk.Payload.counters.err;
            success = false;
        }
    }

    /* if type is type_up, or all types */
    if (success && ((wq->type == CF_Type_all) || (wq->type == CF_Type_up)))
    {
        /* process uplink queue data */
        if ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_active))
        {
            ret = CF_WriteTxnQueueDataToFile(fd, chan, CF_QueueIdx_RX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_CMD_WQ_WRITEQ_RX_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_QueueIdx_RX data");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.Payload.counters.err;
                success = false;
            }
        }

        if (success && ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_history)))
        {
            ret = CF_WriteHistoryQueueDataToFile(fd, chan, CF_Direction_RX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_CMD_WQ_WRITEHIST_RX_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write history RX data");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.Payload.counters.err;
                success = false;
            }
        }
    }

    /* if type is type_down, or all types */
    if (success && ((wq->type == CF_Type_all) || (wq->type == CF_Type_down)))
    {
        /* process downlink queue data */
        if ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_active))
        {
            int              i;
            static const int qs[2] = {CF_QueueIdx_TXA, CF_QueueIdx_TXW};
            for (i = 0; i < 2; ++i)
            {
                ret = CF_WriteTxnQueueDataToFile(fd, chan, qs[i]);
                if (ret)
                {
                    CFE_EVS_SendEvent(CF_CMD_WQ_WRITEQ_TX_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "CF: write queue failed to write q index %d", qs[i]);
                    CF_WrappedClose(fd);
                    ++CF_AppData.hk.Payload.counters.err;
                    success = false;
                    break;
                }
            }
        }

        if (success && ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_pend)))
        {
            /* write pending queue */
            ret = CF_WriteTxnQueueDataToFile(fd, chan, CF_QueueIdx_PEND);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_CMD_WQ_WRITEQ_PEND_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write pending queue");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.Payload.counters.err;
                success = false;
            }
        }

        if (success && ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_history)))
        {
            /* write history queue */
            ret = CF_WriteHistoryQueueDataToFile(fd, chan, CF_Direction_TX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_CMD_WQ_WRITEHIST_TX_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_QueueIdx_TX data");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.Payload.counters.err;
                success = false;
            }
        }
    }

    if (success)
    {
        CFE_EVS_SendEvent(CF_CMD_WQ_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: write queue successful");
        ++CF_AppData.hk.Payload.counters.cmd;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChanAction_Status_t CF_CmdValidateChunkSize(CF_ChunkSize_t val, uint8 chan_num /* ignored */)
{
    CF_ChanAction_Status_t ret = CF_ChanAction_Status_SUCCESS;
    if (val > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        ret = CF_ChanAction_Status_ERROR; /* failed */
    }
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChanAction_Status_t CF_CmdValidateMaxOutgoing(uint32 val, uint8 chan_num)
{
    CF_ChanAction_Status_t ret = CF_ChanAction_Status_SUCCESS;

    if (!val && !CF_AppData.config_table->chan[chan_num].sem_name[0])
    {
        /* can't have unlimited messages and no semaphore */
        ret = CF_ChanAction_Status_ERROR; /* failed */
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_GetSetParamCmd(uint8 is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num)
{
    CF_ConfigTable_t *config;
    CFE_Status_t      status = CF_ERROR;
    bool              valid_set;
    struct
    {
        void * ptr;
        size_t size;
        CF_ChanAction_Status_t (*fn)(uint32, uint8 chan_num);
    } item;

    valid_set = false;
    config    = CF_AppData.config_table;
    memset(&item, 0, sizeof(item));

    switch (param_id)
    {
        case CF_GetSet_ValueID_ticks_per_second:
            item.ptr  = &config->ticks_per_second;
            item.size = sizeof(config->ticks_per_second);
            break;
        case CF_GetSet_ValueID_rx_crc_calc_bytes_per_wakeup:
            item.ptr  = &config->rx_crc_calc_bytes_per_wakeup;
            item.size = sizeof(config->rx_crc_calc_bytes_per_wakeup);
            break;
        case CF_GetSet_ValueID_ack_timer_s:
            item.ptr  = &config->chan[chan_num].ack_timer_s;
            item.size = sizeof(config->chan[chan_num].ack_timer_s);
            break;
        case CF_GetSet_ValueID_nak_timer_s:
            item.ptr  = &config->chan[chan_num].nak_timer_s;
            item.size = sizeof(config->chan[chan_num].nak_timer_s);
            break;
        case CF_GetSet_ValueID_inactivity_timer_s:
            item.ptr  = &config->chan[chan_num].inactivity_timer_s;
            item.size = sizeof(config->chan[chan_num].inactivity_timer_s);
            break;
        case CF_GetSet_ValueID_outgoing_file_chunk_size:
            item.ptr  = &config->outgoing_file_chunk_size;
            item.size = sizeof(config->outgoing_file_chunk_size);
            item.fn   = CF_CmdValidateChunkSize;
            break;
        case CF_GetSet_ValueID_ack_limit:
            item.ptr  = &config->chan[chan_num].ack_limit;
            item.size = sizeof(config->chan[chan_num].ack_limit);
            break;
        case CF_GetSet_ValueID_nak_limit:
            item.ptr  = &config->chan[chan_num].nak_limit;
            item.size = sizeof(config->chan[chan_num].nak_limit);
            break;
        case CF_GetSet_ValueID_local_eid:
            item.ptr  = &config->local_eid;
            item.size = sizeof(config->local_eid);
            break;
        case CF_GetSet_ValueID_chan_max_outgoing_messages_per_wakeup:
            item.ptr  = &config->chan[chan_num].max_outgoing_messages_per_wakeup;
            item.size = sizeof(config->chan[chan_num].max_outgoing_messages_per_wakeup);
            item.fn   = CF_CmdValidateMaxOutgoing;
            break;
        default:
            break;
    };

    if (item.size == 0)
    {
        CFE_EVS_SendEvent(CF_CMD_GETSET_PARAM_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: invalid configuration parameter id %d received", param_id);
    }
    else if (chan_num >= CF_NUM_CHANNELS)
    {
        CFE_EVS_SendEvent(CF_CMD_GETSET_CHAN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: invalid configuration channel id %d received", chan_num);
    }
    else if (is_set)
    {
        if (item.fn)
        {
            if (CF_ChanAction_Status_IS_SUCCESS(item.fn(value, chan_num)))
            {
                valid_set = 1;
            }
            else
            {
                CFE_EVS_SendEvent(CF_CMD_GETSET_VALIDATE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: validation for parameter id %d failed", param_id);
            }
        }
        else
        {
            valid_set = 1;
        }

        if (valid_set)
        {
            status = CFE_SUCCESS;

            CFE_EVS_SendEvent(CF_CMD_GETSET1_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "CF: setting parameter id %d to %lu", param_id, (unsigned long)value);

            /* Store value based on its size */
            if (item.size == sizeof(uint32))
            {
                *((uint32 *)item.ptr) = value;
            }
            else if (item.size == sizeof(uint16))
            {
                *((uint16 *)item.ptr) = value;
            }
            else
            {
                /* uint8 is the only other option */
                *((uint8 *)item.ptr) = value;
            }
        }
    }
    else
    {
        status = CFE_SUCCESS;

        /* Read value depending on its size */
        if (item.size == sizeof(uint32))
        {
            value = *((const uint32 *)item.ptr);
        }
        else if (item.size == sizeof(uint16))
        {
            value = *((const uint16 *)item.ptr);
        }
        else
        {
            /* uint8 is the only other option */
            value = *((const uint8 *)item.ptr);
        }

        CFE_EVS_SendEvent(CF_CMD_GETSET2_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: parameter id %d = %lu", param_id,
                          (unsigned long)value);
    }

    if (status == CFE_SUCCESS)
    {
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        ++CF_AppData.hk.Payload.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SetParamCmd(const CF_SetParamCmd_t *msg)
{
    const CF_SetParam_Payload_t *cmd = &msg->Payload;

    CF_GetSetParamCmd(1, cmd->key, cmd->value, cmd->chan_num);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_GetParamCmd(const CF_GetParamCmd_t *msg)
{
    const CF_GetParam_Payload_t *cmd = &msg->Payload;

    CF_GetSetParamCmd(0, cmd->key, 0, cmd->chan_num);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableEngineCmd(const CF_EnableEngineCmd_t *msg)
{
    if (!CF_AppData.engine.enabled)
    {
        if (CF_CFDP_InitEngine() == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CMD_ENABLE_ENGINE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: enabled CFDP engine");
            ++CF_AppData.hk.Payload.counters.cmd;
        }
        else
        {
            CFE_EVS_SendEvent(CF_CMD_ENABLE_ENGINE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: failed to re-initialize and enable CFDP engine");
            ++CF_AppData.hk.Payload.counters.err;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENG_ALREADY_ENA_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: received enable engine command while engine already enabled");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableEngineCmd(const CF_DisableEngineCmd_t *msg)
{
    if (CF_AppData.engine.enabled)
    {
        CF_CFDP_DisableEngine();
        CFE_EVS_SendEvent(CF_CMD_DISABLE_ENGINE_INF_EID, CFE_EVS_EventType_INFORMATION, "CF: disabled CFDP engine");
        ++CF_AppData.hk.Payload.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_CMD_ENG_ALREADY_DIS_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: received disable engine command while engine already disabled");
        ++CF_AppData.hk.Payload.counters.err;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SendHkCmd(const CF_SendHkCmd_t *msg)
{
    CFE_MSG_SetMsgTime(CFE_MSG_PTR(CF_AppData.hk.TelemetryHeader), CFE_TIME_GetTime());
    /* return value ignored */ CFE_SB_TransmitMsg(CFE_MSG_PTR(CF_AppData.hk.TelemetryHeader), true);

    /* This is also used to check tables */
    CF_CheckTables();

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WakeupCmd(const CF_WakeupCmd_t *msg)
{
    CFE_ES_PerfLogEntry(CF_PERF_ID_CYCLE_ENG);
    CF_CFDP_CycleEngine();
    CFE_ES_PerfLogExit(CF_PERF_ID_CYCLE_ENG);

    return CFE_SUCCESS;
}
