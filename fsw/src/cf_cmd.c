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

#include "cf_cfdp.h"
#include "cf_cmd.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdNoop(CFE_SB_Buffer_t *msg)
{
    CFE_EVS_SendEvent(CF_EID_INF_CMD_NOOP, CFE_EVS_EventType_INFORMATION, "CF: No-Op received, Version %d.%d.%d.%d",
                      CF_MAJOR_VERSION, CF_MINOR_VERSION, CF_REVISION, CF_MISSION_REV);
    ++CF_AppData.hk.counters.cmd;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdReset(CFE_SB_Buffer_t *msg)
{
    CF_UnionArgsCmd_t *cmd      = (CF_UnionArgsCmd_t *)msg;
    static const char *names[5] = {"all", "cmd", "fault", "up", "down"};
    /* 0=all, 1=cmd, 2=fault 3=up 4=down */
    uint8 param = cmd->data.byte[0];
    int   i;
    int   acc = 1;

    if (param > 4)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_RESET_INVALID, CFE_EVS_EventType_ERROR,
                          "CF: Received RESET COUNTERS command with invalid parameter %d", param);
        ++CF_AppData.hk.counters.err;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_RESET, CFE_EVS_EventType_INFORMATION,
                          "CF: Received RESET COUNTERS command: %s", names[param]);

        /* if the param is CF_Reset_command, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_command))
        {
            /* command counters */
            memset(&CF_AppData.hk.counters, 0, sizeof(CF_AppData.hk.counters));
            acc = 0; /* don't increment accept counter on command counter reset */
        }

        /* if the param is CF_Reset_fault, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_fault))
        {
            /* fault counters */
            for (i = 0; i < CF_NUM_CHANNELS; ++i)
                memset(&CF_AppData.hk.channel_hk[i].counters.fault, 0,
                       sizeof(CF_AppData.hk.channel_hk[i].counters.fault));
        }

        /* if the param is CF_Reset_up, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_up))
        {
            /* up counters */
            for (i = 0; i < CF_NUM_CHANNELS; ++i)
                memset(&CF_AppData.hk.channel_hk[i].counters.recv, 0,
                       sizeof(CF_AppData.hk.channel_hk[i].counters.recv));
        }

        /* if the param is CF_Reset_down, or all counters */
        if ((param == CF_Reset_all) || (param == CF_Reset_down))
        {
            /* down counters */
            for (i = 0; i < CF_NUM_CHANNELS; ++i)
                memset(&CF_AppData.hk.channel_hk[i].counters.sent, 0,
                       sizeof(CF_AppData.hk.channel_hk[i].counters.sent));
        }

        if (acc)
        {
            ++CF_AppData.hk.counters.cmd;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdTxFile(CFE_SB_Buffer_t *msg)
{
    CF_TxFileCmd_t *tx = (CF_TxFileCmd_t *)msg;

    /*
     * This needs to validate all its inputs.
     * "keep" should only be 0 or 1 (logical true/false).
     * For priority and dest_id params, anything is acceptable.
     */
    if ((tx->cfdp_class != CF_CFDP_CLASS_1 && tx->cfdp_class != CF_CFDP_CLASS_2) || tx->chan_num >= CF_NUM_CHANNELS ||
        tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_BAD_PARAM, CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_CmdTxFile(): chan=%u, class=%u keep=%u", (unsigned int)tx->chan_num,
                          (unsigned int)tx->cfdp_class, (unsigned int)tx->keep);
        ++CF_AppData.hk.counters.err;
        return;
    }

    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;

    if (CF_CFDP_TxFile(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num, tx->priority,
                       tx->dest_id) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_TX_FILE, CFE_EVS_EventType_INFORMATION,
                          "CF: file transfer successfully initiated");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_TX_FILE, CFE_EVS_EventType_ERROR, "CF: file transfer initiation failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdPlaybackDir(CFE_SB_Buffer_t *msg)
{
    CF_PlaybackDirCmd_t *tx = (CF_PlaybackDirCmd_t *)msg;

    /*
     * This needs to validate all its inputs.
     * "keep" should only be 0 or 1 (logical true/false).
     * For priority and dest_id params, anything is acceptable.
     */
    if ((tx->cfdp_class != CF_CFDP_CLASS_1 && tx->cfdp_class != CF_CFDP_CLASS_2) || tx->chan_num >= CF_NUM_CHANNELS ||
        tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_BAD_PARAM, CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_CmdPlaybackDir(): chan=%u, class=%u keep=%u",
                          (unsigned int)tx->chan_num, (unsigned int)tx->cfdp_class, (unsigned int)tx->keep);
        ++CF_AppData.hk.counters.err;
        return;
    }

    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;

    if (CF_CFDP_PlaybackDir(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num, tx->priority,
                            tx->dest_id) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_PLAYBACK_DIR, CFE_EVS_EventType_INFORMATION,
                          "CF: directory playback initiation successful");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_PLAYBACK_DIR, CFE_EVS_EventType_ERROR,
                          "CF: directory playback initiation failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DoChanAction(CF_UnionArgsCmd_t *cmd, const char *errstr, CF_ChanActionFn_t fn, void *context)
{
    int          i;
    CFE_Status_t ret = 0;

    /* this function is generic for any ground command that takes a single channel
     * argument which must be less than CF_NUM_CHANNELS or 255 which is a special
     * value that means apply command to all channels */
    if (cmd->data.byte[0] == CF_ALL_CHANNELS)
    {
        /* apply to all channels */
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DoFreezeThaw(uint8 chan_num, const CF_ChanAction_BoolArg_t *context)
{
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.hk.channel_hk[chan_num].frozen = context->barg;
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdFreeze(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {1}; /* param is frozen, so 1 means freeze */

    if (CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "freeze", (CF_ChanActionFn_t)CF_DoFreezeThaw, &barg) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_FREEZE, CFE_EVS_EventType_INFORMATION, "CF: freeze successful");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_FREEZE, CFE_EVS_EventType_ERROR, "CF: freeze cmd failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdThaw(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {0}; /* param is frozen, so 0 means thawed */

    if (CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "thaw", (CF_ChanActionFn_t)CF_DoFreezeThaw, &barg) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_THAW, CFE_EVS_EventType_INFORMATION, "CF: thaw successful");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_THAW, CFE_EVS_EventType_ERROR, "CF: thaw cmd failed");
        ++CF_AppData.hk.counters.err;
    }
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
CFE_Status_t CF_TsnChanAction(CF_TransactionCmd_t *cmd, const char *cmdstr, CF_TsnChanAction_fn_t fn, void *context)
{
    CF_Transaction_t *t;
    CFE_Status_t      ret = CF_ERROR;

    if (cmd->chan == CF_COMPOUND_KEY)
    {
        /* special value 254 means to use the compound key (cmd->eid, cmd->ts) to find the transaction
         * to act upon */
        t = CF_FindTransactionBySequenceNumberAllChannels(cmd->ts, cmd->eid);
        if (t)
        {
            fn(t, context);
            ret = 1; /* because one transaction was matched - this should return a count */
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_TRANS_NOT_FOUND, CFE_EVS_EventType_ERROR,
                              "CF: %s cmd: failed to find transaction for (eid %lu, ts %lu)", cmdstr,
                              (unsigned long)cmd->eid, (unsigned long)cmd->ts);
        }
    }
    else if (cmd->chan == CF_ALL_CHANNELS)
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_DoSuspRes_Txn(CF_Transaction_t *t, CF_ChanAction_SuspResArg_t *context)
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_DoSuspRes(CF_TransactionCmd_t *cmd, uint8 action)
{
    /* ok to not bounds check action, because the caller is using it in two places with constant values 0 or 1 */
    static const char *        msgstr[] = {"resume", "suspend"};
    CF_ChanAction_SuspResArg_t args     = {0, action};
    int ret = CF_TsnChanAction(cmd, msgstr[action], (CF_TsnChanAction_fn_t)CF_DoSuspRes_Txn, &args);

    /*
     * Note that this command may affect multiple transactions, depending on the value of the "chan" argument.
     * When acting on multiple channels, the "same" output does not apply.  In reality all it means is
     * that one of the affected channels was already set that way.
     */

    if (ret == 1 && args.same)
    {
        /* A single transaction was mached, and it was already set the same way */
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_SUSPRES_SAME, CFE_EVS_EventType_ERROR,
                          "CF: %s cmd: setting suspend flag to current value of %d", msgstr[action], action);
        ++CF_AppData.hk.counters.err;
    }
    else if (ret <= 0)
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_SUSPRES_CHAN, CFE_EVS_EventType_ERROR, "CF: %s cmd: no transaction found",
                          msgstr[action]);
        ++CF_AppData.hk.counters.err;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_SUSPRES, CFE_EVS_EventType_INFORMATION,
                          "CF: %s cmd: setting suspend flag to %d", msgstr[action], action);
        ++CF_AppData.hk.counters.cmd;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdSuspend(CFE_SB_Buffer_t *msg)
{
    CF_DoSuspRes((CF_TransactionCmd_t *)msg, 1);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdResume(CFE_SB_Buffer_t *msg)
{
    CF_DoSuspRes((CF_TransactionCmd_t *)msg, 0);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdCancel_Txn(CF_Transaction_t *t, void *ignored)
{
    CF_CFDP_CancelTransaction(t);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdCancel(CFE_SB_Buffer_t *msg)
{
    if (CF_TsnChanAction((CF_TransactionCmd_t *)msg, "cancel", CF_CmdCancel_Txn, NULL) > 0)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_CANCEL, CFE_EVS_EventType_INFORMATION,
                          "CF: cancel transaction successfully initiated");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_CANCEL_CHAN, CFE_EVS_EventType_ERROR, "CF: cancel cmd: no transaction found");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdAbandon_Txn(CF_Transaction_t *t, void *ignored)
{
    CF_CFDP_ResetTransaction(t, 0);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdAbandon(CFE_SB_Buffer_t *msg)
{
    if (CF_TsnChanAction((CF_TransactionCmd_t *)msg, "abandon", CF_CmdAbandon_Txn, NULL) > 0)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_ABANDON, CFE_EVS_EventType_INFORMATION, "CF: abandon successful");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ABANDON_CHAN, CFE_EVS_EventType_ERROR,
                          "CF: abandon cmd: no transaction found");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DoEnableDisableDequeue(uint8 chan_num, const CF_ChanAction_BoolArg_t *context)
{
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.config_table->chan[chan_num].dequeue_enabled = context->barg;
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdEnableDequeue(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {1};

    if (CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "enable_dequeue", (CF_ChanActionFn_t)CF_DoEnableDisableDequeue,
                        &barg) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_ENABLE_DEQUEUE, CFE_EVS_EventType_INFORMATION, "CF: dequeue enabled");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENABLE_DEQUEUE, CFE_EVS_EventType_ERROR, "CF: enable dequeue cmd failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdDisableDequeue(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {0};

    if (CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "disable_dequeue", (CF_ChanActionFn_t)CF_DoEnableDisableDequeue,
                        &barg) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_DISABLE_DEQUEUE, CFE_EVS_EventType_INFORMATION, "CF: dequeue disabled");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_DISABLE_DEQUEUE, CFE_EVS_EventType_ERROR, "CF: disable dequeue cmd failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DoEnableDisablePolldir(uint8 chan_num, const CF_ChanAction_BoolMsgArg_t *context)
{
    int          i;
    CFE_Status_t ret = CFE_SUCCESS;
    /* no need to bounds check chan_num, done in caller */
    if (context->msg->data.byte[1] == CF_ALL_POLLDIRS)
    {
        /* all polldirs in channel */
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
        ret = CF_ERROR;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdEnablePolldir(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolMsgArg_t barg = {(CF_UnionArgsCmd_t *)msg, 1};

    if (CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "enable_polldir", (CF_ChanActionFn_t)CF_DoEnableDisablePolldir,
                        &barg) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_ENABLE_POLLDIR, CFE_EVS_EventType_INFORMATION,
                          "CF: enabled polling directory");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENABLE_POLLDIR, CFE_EVS_EventType_ERROR,
                          "CF: enable polling directory cmd failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdDisablePolldir(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolMsgArg_t barg = {(CF_UnionArgsCmd_t *)msg, 0};

    if (CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "disable_polldir", (CF_ChanActionFn_t)CF_DoEnableDisablePolldir,
                        &barg) == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_DISABLE_POLLDIR, CFE_EVS_EventType_INFORMATION,
                          "CF: disabled polling directory");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_DISABLE_POLLDIR, CFE_EVS_EventType_ERROR,
                          "CF: disable polling directory cmd failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PurgeHistory(CF_CListNode_t *n, CF_Channel_t *c)
{
    CF_History_t *h = container_of(n, CF_History_t, cl_node);
    CF_ResetHistory(c, h); /* ok to reset transaction since it's in PEND it hasn't started yet */
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PurgeTransaction(CF_CListNode_t *n, void *ignored)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);
    CF_CFDP_ResetTransaction(t, 0);
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DoPurgeQueue(uint8 chan_num, CF_UnionArgsCmd_t *cmd)
{
    CFE_Status_t ret = CFE_SUCCESS;
    /* no need to bounds check chan_num, done in caller */
    CF_Channel_t *c = &CF_AppData.engine.channels[chan_num];

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
        CF_CList_Traverse(c->qs[CF_QueueIdx_PEND], CF_PurgeTransaction, NULL);
    }

    if (hist)
    {
        CF_CList_Traverse(c->qs[CF_QueueIdx_HIST], (CF_CListFn_t)CF_PurgeHistory, c);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdPurgeQueue(CFE_SB_Buffer_t *msg)
{
    if (CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "purge_queue", (CF_ChanActionFn_t)CF_DoPurgeQueue, msg) ==
        CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_PURGE_QUEUE, CFE_EVS_EventType_INFORMATION, "CF: queue purged");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_PURGE_QUEUE, CFE_EVS_EventType_ERROR, "CF: purge queue cmd failed");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdWriteQueue(CFE_SB_Buffer_t *msg)
{
    CF_WriteQueueCmd_t *wq      = (CF_WriteQueueCmd_t *)msg;
    CF_Channel_t *      c       = &CF_AppData.engine.channels[wq->chan];
    osal_id_t           fd      = OS_OBJECT_ID_UNDEFINED;
    bool                success = true;
    int32               ret;

    /* check the commands for validity */
    if (wq->chan >= CF_NUM_CHANNELS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_CHAN, CFE_EVS_EventType_ERROR, "CF: write queue invalid channel arg");
        ++CF_AppData.hk.counters.err;
        success = false;
    }
    /* only invalid combination is up direction, pending queue */
    else if ((wq->type == CF_Type_up) && (wq->queue == CF_Queue_pend))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_ARGS, CFE_EVS_EventType_ERROR,
                          "CF: write queue invalid command parameters");
        ++CF_AppData.hk.counters.err;
        success = false;
    }
    else
    {
        /* PTFO: queues can be large. may want to split this work up across the state machine and take several wakeups
         * to complete */
        ret = CF_WrappedOpenCreate(&fd, wq->filename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
        if (ret < 0)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_OPEN, CFE_EVS_EventType_ERROR, "CF: write queue failed to open file %s",
                              wq->filename);
            ++CF_AppData.hk.counters.err;
            success = false;
        }
    }

    /* if type is type_up, or all types */
    if (success && ((wq->type == CF_Type_all) || (wq->type == CF_Type_up)))
    {
        /* process uplink queue data */
        if ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_active))
        {
            ret = CF_WriteTxnQueueDataToFile(fd, c, CF_QueueIdx_RX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEQ_RX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_QueueIdx_RX data");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.counters.err;
                success = false;
            }
        }

        if (success && ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_history)))
        {
            ret = CF_WriteHistoryQueueDataToFile(fd, c, CF_Direction_RX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEHIST_RX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write history RX data");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.counters.err;
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
                ret = CF_WriteTxnQueueDataToFile(fd, c, qs[i]);
                if (ret)
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEQ_TX, CFE_EVS_EventType_ERROR,
                                      "CF: write queue failed to write q index %d", qs[i]);
                    CF_WrappedClose(fd);
                    ++CF_AppData.hk.counters.err;
                    success = false;
                    break;
                }
            }
        }

        if (success && ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_pend)))
        {
            /* write pending queue */
            ret = CF_WriteTxnQueueDataToFile(fd, c, CF_QueueIdx_PEND);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEQ_PEND, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write pending queue");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.counters.err;
                success = false;
            }
        }

        if (success && ((wq->queue == CF_Queue_all) || (wq->queue == CF_Queue_history)))
        {
            /* write history queue */
            ret = CF_WriteHistoryQueueDataToFile(fd, c, CF_Direction_TX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEHIST_TX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_QueueIdx_TX data");
                CF_WrappedClose(fd);
                ++CF_AppData.hk.counters.err;
                success = false;
            }
        }
    }

    if (success)
    {
        CFE_EVS_SendEvent(CF_EID_INF_CMD_WQ, CFE_EVS_EventType_INFORMATION, "CF: write queue successful");
        ++CF_AppData.hk.counters.cmd;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CmdValidateChunkSize(uint32 val, uint8 chan_num /* ignored */)
{
    CFE_Status_t ret = CFE_SUCCESS;
    if (val > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        ret = CF_ERROR; /* failed */
    }
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CmdValidateMaxOutgoing(uint32 val, uint8 chan_num)
{
    CFE_Status_t ret = CFE_SUCCESS;

    if (!val && !CF_AppData.config_table->chan[chan_num].sem_name[0])
    {
        /* can't have unlimited messages and no semaphore */
        ret = CF_ERROR; /* failed */
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdGetSetParam(uint8 is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num)
{
    CF_ConfigTable_t *config;
    CFE_Status_t      status = CF_ERROR;
    bool              valid_set;
    struct
    {
        void * ptr;
        uint32 size;
        int (*fn)(uint32, uint8 chan_num);
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
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_GETSET_PARAM, CFE_EVS_EventType_ERROR,
                          "CF: invalid configuration parameter id %d received", param_id);
    }
    else if (chan_num >= CF_NUM_CHANNELS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_GETSET_CHAN, CFE_EVS_EventType_ERROR,
                          "CF: invalid configuration channel id %d received", chan_num);
    }
    else if (is_set)
    {
        if (item.fn)
        {
            if (!item.fn(value, chan_num))
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
            status = CFE_SUCCESS;

            CFE_EVS_SendEvent(CF_EID_INF_CMD_GETSET1, CFE_EVS_EventType_INFORMATION,
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

        CFE_EVS_SendEvent(CF_EID_INF_CMD_GETSET2, CFE_EVS_EventType_INFORMATION, "CF: parameter id %d = %lu", param_id,
                          (unsigned long)value);
    }

    if (status == CFE_SUCCESS)
    {
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdSetParam(CFE_SB_Buffer_t *msg)
{
    CF_SetParamCmd_t *cmd = (CF_SetParamCmd_t *)msg;
    CF_CmdGetSetParam(1, cmd->key, cmd->value, cmd->chan_num);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdGetParam(CFE_SB_Buffer_t *msg)
{
    CF_GetParamCmd_t *cmd = (CF_GetParamCmd_t *)msg;
    CF_CmdGetSetParam(0, cmd->key, 0, cmd->chan_num);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdEnableEngine(CFE_SB_Buffer_t *msg)
{
    if (!CF_AppData.engine.enabled)
    {
        if (CF_CFDP_InitEngine() == CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_INF_CMD_ENABLE_ENGINE, CFE_EVS_EventType_INFORMATION, "CF: enabled CFDP engine");
            ++CF_AppData.hk.counters.cmd;
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENABLE_ENGINE, CFE_EVS_EventType_ERROR,
                              "CF: failed to re-initialize and enable CFDP engine");
            ++CF_AppData.hk.counters.err;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENG_ALREADY_ENA, CFE_EVS_EventType_ERROR,
                          "CF: received enable engine command while engine already enabled");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdDisableEngine(CFE_SB_Buffer_t *msg)
{
    if (CF_AppData.engine.enabled)
    {
        CF_CFDP_DisableEngine();
        CFE_EVS_SendEvent(CF_EID_INF_CMD_DISABLE_ENGINE, CFE_EVS_EventType_INFORMATION, "CF: disabled CFDP engine");
        ++CF_AppData.hk.counters.cmd;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENG_ALREADY_DIS, CFE_EVS_EventType_ERROR,
                          "CF: received disable engine command while engine already disabled");
        ++CF_AppData.hk.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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
        NULL,
        CF_CmdWriteQueue,     /* CF_WRITE_QUEUE_CC */
        CF_CmdEnableDequeue,  /* CF_ENABLE_DEQUEUE_CC */
        CF_CmdDisableDequeue, /* CF_DISABLE_DEQUEUE_CC */
        CF_CmdEnablePolldir,  /* CF_ENABLE_DIR_POLLING_CC */
        CF_CmdDisablePolldir, /* CF_DISABLE_DIR_POLLING_CC */
        NULL,
        CF_CmdPurgeQueue,    /* CF_PURGE_QUEUE_CC */
        CF_CmdEnableEngine,  /* CF_ENABLE_ENGINE_CC */
        CF_CmdDisableEngine, /* CF_DISABLE_ENGINE_CC */
    };

    static const uint16 expected_lengths[CF_NUM_COMMANDS] = {
        sizeof(CF_NoArgsCmd_t),      /* CF_NOOP_CC */
        sizeof(CF_UnionArgsCmd_t),   /* CF_RESET_CC */
        sizeof(CF_TxFileCmd_t),      /* CF_TX_FILE_CC */
        sizeof(CF_PlaybackDirCmd_t), /* CF_PLAYBACK_DIR_CC */
        sizeof(CF_UnionArgsCmd_t),   /* CF_FREEZE_CC */
        sizeof(CF_UnionArgsCmd_t),   /* CF_THAW_CC */
        sizeof(CF_TransactionCmd_t), /* CF_SUSPEND_CC */
        sizeof(CF_TransactionCmd_t), /* CF_RESUME_CC */
        sizeof(CF_TransactionCmd_t), /* CF_CANCEL_CC */
        sizeof(CF_TransactionCmd_t), /* CF_ABANDON_CC */
        sizeof(CF_SetParamCmd_t),    /* CF_SET_MIB_PARAM_CC */
        sizeof(CF_GetParamCmd_t),    /* CF_GET_MIB_PARAM_CC */
        0,
        0,
        0,
        sizeof(CF_WriteQueueCmd_t), /* CF_WRITE_QUEUE_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_ENABLE_DEQUEUE_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_DISABLE_DEQUEUE_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_ENABLE_DIR_POLLING_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_DISABLE_DIR_POLLING_CC */
        0,
        sizeof(CF_UnionArgsCmd_t), /* CF_PURGE_QUEUE_CC */
        sizeof(CF_NoArgsCmd_t),    /* CF_ENABLE_ENGINE_CC */
        sizeof(CF_NoArgsCmd_t),    /* CF_DISABLE_ENGINE_CC */
    };

    CFE_MSG_FcnCode_t cmd = 0;
    size_t            len = 0;

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
            ++CF_AppData.hk.counters.err;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_GCMD_CC, CFE_EVS_EventType_ERROR,
                          "CF: invalid ground command packet cmd_code=0x%02x", cmd);
        ++CF_AppData.hk.counters.err;
    }
}
