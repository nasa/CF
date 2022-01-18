/************************************************************************
 *
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 * Copyright © 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
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

#define ALL_CHANNELS 255
#define ALL_POLLDIRS ALL_CHANNELS
#define COMPOUND_KEY 254

/*----------------------------------------------------------------
 *
 * Function: CF_CmdNoop
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdNoop(CFE_SB_Buffer_t *msg)
{
    CFE_EVS_SendEvent(CF_EID_INF_CMD_NOOP, CFE_EVS_EventType_INFORMATION, "CF: No-Op received, Version %d.%d.%d",
                      CF_MAJOR_VERSION, CF_MINOR_VERSION, CF_REVISION);
    CF_CmdAcc();
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdReset
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdReset(CFE_SB_Buffer_t *msg)
{
    static const int counters_command = 1;
    static const int counters_fault   = 2;
    static const int counters_up      = 3;
    static const int counters_down    = 4;

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

/*----------------------------------------------------------------
 *
 * Function: CF_CmdTxFile
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
    if (tx->cfdp_class > CF_CFDP_CLASS_2 || tx->chan_num >= CF_NUM_CHANNELS || tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_BAD_PARAM, CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_CmdTxFile(): chan=%u, class=%u keep=%u", (unsigned int)tx->chan_num,
                          (unsigned int)tx->cfdp_class, (unsigned int)tx->keep);
        CF_CmdRej();
        return;
    }

    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;

    CF_CmdCond(CF_CFDP_TxFile(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num, tx->priority,
                              tx->dest_id));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdPlaybackDir
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
    if (tx->cfdp_class > CF_CFDP_CLASS_2 || tx->chan_num >= CF_NUM_CHANNELS || tx->keep > 1)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_BAD_PARAM, CFE_EVS_EventType_ERROR,
                          "CF: bad parameter in CF_CmdPlaybackDir(): chan=%u, class=%u keep=%u",
                          (unsigned int)tx->chan_num, (unsigned int)tx->cfdp_class, (unsigned int)tx->keep);
        CF_CmdRej();
        return;
    }

    /* make sure that the src and dst filenames are null terminated */
    tx->src_filename[sizeof(tx->src_filename) - 1] = 0;
    tx->dst_filename[sizeof(tx->dst_filename) - 1] = 0;

    CF_CmdCond(CF_CFDP_PlaybackDir(tx->src_filename, tx->dst_filename, tx->cfdp_class, tx->keep, tx->chan_num,
                                   tx->priority, tx->dest_id));
}

/*----------------------------------------------------------------
 *
 * Function: CF_DoChanAction
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_DoChanAction(CF_UnionArgsCmd_t *cmd, const char *errstr, CF_ChanActionFn_t fn, void *context)
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

/*----------------------------------------------------------------
 *
 * Function: CF_DoFreezeThaw
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_DoFreezeThaw(uint8 chan_num, const CF_ChanAction_BoolArg_t *context)
{
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.hk.channel_hk[chan_num].frozen = context->barg;
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdFreeze
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdFreeze(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {1}; /* param is frozen, so 1 means freeze */
    CF_CmdCond(CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "freeze", (CF_ChanActionFn_t)CF_DoFreezeThaw, &barg));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdThaw
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdThaw(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {0}; /* param is frozen, so 0 means thawed */
    CF_CmdCond(CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "thaw", (CF_ChanActionFn_t)CF_DoFreezeThaw, &barg));
}

/*----------------------------------------------------------------
 *
 * Function: CF_FindTransactionBySequenceNumberAllChannels
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
     * across the entire system. Meaning, it isn't correect to have the same
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
 * Function: CF_TsnChanAction
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_TsnChanAction(CF_TransactionCmd_t *cmd, const char *cmdstr, CF_TsnChanAction_fn_t fn, void *context)
{
    int ret = -1;

    if (cmd->chan == COMPOUND_KEY)
    {
        /* special value 254 means to use the compound key (cmd->eid, cmd->ts) to find the transaction
         * to act upon */
        CF_Transaction_t *t = CF_FindTransactionBySequenceNumberAllChannels(cmd->ts, cmd->eid);
        if (t)
        {
            fn(t, context);
            ret = 1; /* because one transaction was matched - this should return a count */
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_TRANS_NOT_FOUND, CFE_EVS_EventType_ERROR,
                              "CF: %s cmd: failed to find transactino for (eid %lu, ts %lu)", cmdstr,
                              (unsigned long)cmd->eid, (unsigned long)cmd->ts);
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

/*----------------------------------------------------------------
 *
 * Function: CF_DoSuspRes_Txn
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
 * Function: CF_DoSuspRes
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_DoSuspRes(CF_TransactionCmd_t *cmd, uint8 action)
{
    /* ok to not bounds check action, because the caller is using it in two places with constant values 0 or 1 */
    static const char         *msgstr[] = {"resume", "suspend"};
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
        CF_CmdRej();
    }
    else if (ret <= 0)
    {
        /* No transaction was matched for the given combination of chan + eid + ts  */
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_SUSPRES_CHAN, CFE_EVS_EventType_ERROR, "CF: %s cmd: no transaction found",
                          msgstr[action]);
        CF_CmdRej();
    }
    else
    {
        CF_CmdAcc();
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdSuspend
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
 * Function: CF_CmdResume
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
 * Function: CF_CmdCancel_Txn
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
 * Function: CF_CmdCancel
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdCancel(CFE_SB_Buffer_t *msg)
{
    /* note that CF_TsnChanAction() returns the number of transactions affected, so <= 0 means failure.
     * CF_CmdCond() accepts 0 (logical false) to mean success, nonzero (logical true) to mean failure */
    CF_CmdCond(CF_TsnChanAction((CF_TransactionCmd_t *)msg, "cancel", CF_CmdCancel_Txn, NULL) <= 0);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdAbandon_Txn
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
 * Function: CF_CmdAbandon
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdAbandon(CFE_SB_Buffer_t *msg)
{
    /* note that CF_TsnChanAction() returns the number of transactions affected, so <= 0 means failure.
     * CF_CmdCond() accepts 0 (logical false) to mean success, nonzero (logical true) to mean failure */
    CF_CmdCond(CF_TsnChanAction((CF_TransactionCmd_t *)msg, "abandon", CF_CmdAbandon_Txn, NULL) <= 0);
}

/*----------------------------------------------------------------
 *
 * Function: CF_DoEnableDisableDequeue
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_DoEnableDisableDequeue(uint8 chan_num, const CF_ChanAction_BoolArg_t *context)
{
    /* no need to bounds check chan_num, done in caller */
    CF_AppData.config_table->chan[chan_num].dequeue_enabled = context->barg;
    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdEnableDequeue
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdEnableDequeue(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {1};
    CF_CmdCond(CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "enable_dequeue", (CF_ChanActionFn_t)CF_DoEnableDisableDequeue,
                               &barg));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdDisableDequeue
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdDisableDequeue(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolArg_t barg = {0};
    CF_CmdCond(CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "disable_dequeue",
                               (CF_ChanActionFn_t)CF_DoEnableDisableDequeue, &barg));
}

/*----------------------------------------------------------------
 *
 * Function: CF_DoEnableDisablePolldir
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_DoEnableDisablePolldir(uint8 chan_num, const CF_ChanAction_BoolMsgArg_t *context)
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

/*----------------------------------------------------------------
 *
 * Function: CF_CmdEnablePolldir
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdEnablePolldir(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolMsgArg_t barg = {(CF_UnionArgsCmd_t *)msg, 1};
    CF_CmdCond(CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "enable_polldir", (CF_ChanActionFn_t)CF_DoEnableDisablePolldir,
                               &barg));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdDisablePolldir
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdDisablePolldir(CFE_SB_Buffer_t *msg)
{
    CF_ChanAction_BoolMsgArg_t barg = {(CF_UnionArgsCmd_t *)msg, 0};
    CF_CmdCond(CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "disable_polldir",
                               (CF_ChanActionFn_t)CF_DoEnableDisablePolldir, &barg));
}

/*----------------------------------------------------------------
 *
 * Function: CF_PurgeHistory
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_PurgeHistory(CF_CListNode_t *n, CF_Channel_t *c)
{
    CF_History_t *h = container_of(n, CF_History_t, cl_node);
    CF_ResetHistory(c, h); /* ok to reset transaction since it's in PEND it hasn't started yet */
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Function: CF_PurgeTransaction
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_PurgeTransaction(CF_CListNode_t *n, void *ignored)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);
    CF_CFDP_ResetTransaction(t, 0);
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Function: CF_DoPurgeQueue
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_DoPurgeQueue(uint8 chan_num, CF_UnionArgsCmd_t *cmd)
{
    int ret = 0;
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
 * Function: CF_CmdPurgeQueue
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdPurgeQueue(CFE_SB_Buffer_t *msg)
{
    CF_CmdCond(CF_DoChanAction((CF_UnionArgsCmd_t *)msg, "purge_queue", (CF_ChanActionFn_t)CF_DoPurgeQueue, msg));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdWriteQueue
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdWriteQueue(CFE_SB_Buffer_t *msg)
{
    /* a type value of 0 means to process both type_up and type_down */
    static const int    type_up   = 1;
    static const int    type_down = 2;
    static const int    q_pend    = 0;
    static const int    q_active  = 1;
    static const int    q_history = 2;
    static const int    q_all     = 3;
    CF_WriteQueueCmd_t *wq        = (CF_WriteQueueCmd_t *)msg;
    CF_Channel_t       *c         = &CF_AppData.engine.channels[wq->chan];
    osal_id_t           fd;
    int32               ret;

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
            ret = CF_WriteTxnQueueDataToFile(fd, c, CF_QueueIdx_RX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEQ_RX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_QueueIdx_RX data");
                goto out_close;
            }
        }

        if ((wq->queue == q_all) || (wq->queue == q_history))
        {
            ret = CF_WriteHistoryQueueDataToFile(fd, c, CF_Direction_RX);
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
            static const int qs[2] = {CF_QueueIdx_TXA, CF_QueueIdx_TXW};
            for (i = 0; i < 2; ++i)
            {
                ret = CF_WriteTxnQueueDataToFile(fd, c, qs[i]);
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
            ret = CF_WriteTxnQueueDataToFile(fd, c, CF_QueueIdx_PEND);
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
            ret = CF_WriteHistoryQueueDataToFile(fd, c, CF_Direction_TX);
            if (ret)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CMD_WQ_WRITEHIST_TX, CFE_EVS_EventType_ERROR,
                                  "CF: write queue failed to write CF_QueueIdx_TX data");
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

/*----------------------------------------------------------------
 *
 * Function: CF_CmdSendCfgParams
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdSendCfgParams(CFE_SB_Buffer_t *msg)
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

/*----------------------------------------------------------------
 *
 * Function: CF_CmdValidateChunkSize
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CmdValidateChunkSize(uint32 val, uint8 chan_num /* ignored */)
{
    int ret = 0;
    if (val > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        ret = 1; /* failed */
    }
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdValidateMaxOutgoing
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CmdValidateMaxOutgoing(uint32 val, uint8 chan_num)
{
    int ret = 0;

    if (!val && !CF_AppData.config_table->chan[chan_num].sem_name[0])
    {
        /* can't have unlimited messages and no semaphore */
        ret = 1; /* failed */
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdGetSetParam
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CmdGetSetParam(uint8 is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num)
{
    CF_ConfigTable_t *config;
    int               acc;
    bool              valid_set;
    struct
    {
        void  *ptr;
        uint32 size;
        int (*fn)(uint32, uint8 chan_num);
    } item;

    acc       = 1; /* 1 means to reject */
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
            item.ptr  = &config->ack_timer_s;
            item.size = sizeof(config->ack_timer_s);
            break;
        case CF_GetSet_ValueID_nak_timer_s:
            item.ptr  = &config->nak_timer_s;
            item.size = sizeof(config->nak_timer_s);
            break;
        case CF_GetSet_ValueID_inactivity_timer_s:
            item.ptr  = &config->inactivity_timer_s;
            item.size = sizeof(config->inactivity_timer_s);
            break;
        case CF_GetSet_ValueID_outgoing_file_chunk_size:
            item.ptr  = &config->outgoing_file_chunk_size;
            item.size = sizeof(config->outgoing_file_chunk_size);
            item.fn   = CF_CmdValidateChunkSize;
            break;
        case CF_GetSet_ValueID_ack_limit:
            item.ptr  = &config->ack_limit;
            item.size = sizeof(config->ack_limit);
            break;
        case CF_GetSet_ValueID_nak_limit:
            item.ptr  = &config->nak_limit;
            item.size = sizeof(config->nak_limit);
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
            acc = 0;

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
        acc = 0;

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

err_out:
    CF_CmdCond(acc);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CmdSetParam
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
 * Function: CF_CmdGetParam
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
 * Function: CF_CmdEnableEngine
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

/*----------------------------------------------------------------
 *
 * Function: CF_CmdDisableEngine
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
        CF_CmdAcc();
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CMD_ENG_ALREADY_DIS, CFE_EVS_EventType_ERROR,
                          "CF: received disable engine command while engine already disabled");
        CF_CmdRej();
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_ProcessGroundCommand
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
        CF_CmdSendCfgParams,  /* CF_SendRet_CFG_PARAMS_CC */
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
        sizeof(CF_NoArgsCmd_t),     /* CF_SendRet_CFG_PARAMS_CC */
        sizeof(CF_WriteQueueCmd_t), /* CF_WRITE_QUEUE_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_ENABLE_DEQUEUE_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_DISABLE_DEQUEUE_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_ENABLE_DIR_POLLING_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_DISABLE_DIR_POLLING_CC */
        0,                          /* CF_DELETE_QUEUE_NODE_CC */
        sizeof(CF_UnionArgsCmd_t),  /* CF_PURGE_QUEUE_CC */
        sizeof(CF_NoArgsCmd_t),     /* CF_ENABLE_ENGINE_CC */
        sizeof(CF_NoArgsCmd_t),     /* CF_DISABLE_ENGINE_CC */
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
