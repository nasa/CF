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
 * Auto-Generated stub implementations for functions defined in cf_cmd header
 */

#include "cf_cmd.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_AbandonCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_AbandonCmd(const CF_AbandonCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_AbandonCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_AbandonCmd, const CF_AbandonCmd_t *, msg);

    UT_GenStub_Execute(CF_AbandonCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_AbandonCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Abandon_TxnCmd()
 * ----------------------------------------------------
 */
void CF_Abandon_TxnCmd(CF_Transaction_t *txn, void *ignored)
{
    UT_GenStub_AddParam(CF_Abandon_TxnCmd, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_Abandon_TxnCmd, void *, ignored);

    UT_GenStub_Execute(CF_Abandon_TxnCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CancelCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_CancelCmd(const CF_CancelCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_CancelCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_CancelCmd, const CF_CancelCmd_t *, msg);

    UT_GenStub_Execute(CF_CancelCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CancelCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Cancel_TxnCmd()
 * ----------------------------------------------------
 */
void CF_Cancel_TxnCmd(CF_Transaction_t *txn, void *ignored)
{
    UT_GenStub_AddParam(CF_Cancel_TxnCmd, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_Cancel_TxnCmd, void *, ignored);

    UT_GenStub_Execute(CF_Cancel_TxnCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DisableDequeueCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DisableDequeueCmd(const CF_DisableDequeueCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_DisableDequeueCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_DisableDequeueCmd, const CF_DisableDequeueCmd_t *, msg);

    UT_GenStub_Execute(CF_DisableDequeueCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DisableDequeueCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DisableDirPollingCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DisableDirPollingCmd(const CF_DisableDirPollingCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_DisableDirPollingCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_DisableDirPollingCmd, const CF_DisableDirPollingCmd_t *, msg);

    UT_GenStub_Execute(CF_DisableDirPollingCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DisableDirPollingCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DisableEngineCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DisableEngineCmd(const CF_DisableEngineCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_DisableEngineCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_DisableEngineCmd, const CF_DisableEngineCmd_t *, msg);

    UT_GenStub_Execute(CF_DisableEngineCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DisableEngineCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoChanAction()
 * ----------------------------------------------------
 */
CF_ChanAction_Status_t CF_DoChanAction(const CF_UnionArgs_Payload_t *data, const char *errstr, CF_ChanActionFn_t fn,
                                       void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_DoChanAction, CF_ChanAction_Status_t);

    UT_GenStub_AddParam(CF_DoChanAction, const CF_UnionArgs_Payload_t *, data);
    UT_GenStub_AddParam(CF_DoChanAction, const char *, errstr);
    UT_GenStub_AddParam(CF_DoChanAction, CF_ChanActionFn_t, fn);
    UT_GenStub_AddParam(CF_DoChanAction, void *, context);

    UT_GenStub_Execute(CF_DoChanAction, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoChanAction, CF_ChanAction_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoEnableDisableDequeue()
 * ----------------------------------------------------
 */
CF_ChanAction_Status_t CF_DoEnableDisableDequeue(uint8 chan_num, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_DoEnableDisableDequeue, CF_ChanAction_Status_t);

    UT_GenStub_AddParam(CF_DoEnableDisableDequeue, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoEnableDisableDequeue, void *, arg);

    UT_GenStub_Execute(CF_DoEnableDisableDequeue, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoEnableDisableDequeue, CF_ChanAction_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoEnableDisablePolldir()
 * ----------------------------------------------------
 */
CF_ChanAction_Status_t CF_DoEnableDisablePolldir(uint8 chan_num, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_DoEnableDisablePolldir, CF_ChanAction_Status_t);

    UT_GenStub_AddParam(CF_DoEnableDisablePolldir, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoEnableDisablePolldir, void *, arg);

    UT_GenStub_Execute(CF_DoEnableDisablePolldir, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoEnableDisablePolldir, CF_ChanAction_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoFreezeThaw()
 * ----------------------------------------------------
 */
CF_ChanAction_Status_t CF_DoFreezeThaw(uint8 chan_num, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_DoFreezeThaw, CF_ChanAction_Status_t);

    UT_GenStub_AddParam(CF_DoFreezeThaw, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoFreezeThaw, void *, arg);

    UT_GenStub_Execute(CF_DoFreezeThaw, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoFreezeThaw, CF_ChanAction_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoPurgeQueue()
 * ----------------------------------------------------
 */
CF_ChanAction_Status_t CF_DoPurgeQueue(uint8 chan_num, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_DoPurgeQueue, CF_ChanAction_Status_t);

    UT_GenStub_AddParam(CF_DoPurgeQueue, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoPurgeQueue, void *, arg);

    UT_GenStub_Execute(CF_DoPurgeQueue, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoPurgeQueue, CF_ChanAction_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoSuspRes()
 * ----------------------------------------------------
 */
void CF_DoSuspRes(const CF_Transaction_Payload_t *payload, uint8 action)
{
    UT_GenStub_AddParam(CF_DoSuspRes, const CF_Transaction_Payload_t *, payload);
    UT_GenStub_AddParam(CF_DoSuspRes, uint8, action);

    UT_GenStub_Execute(CF_DoSuspRes, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoSuspRes_Txn()
 * ----------------------------------------------------
 */
void CF_DoSuspRes_Txn(CF_Transaction_t *txn, CF_ChanAction_SuspResArg_t *context)
{
    UT_GenStub_AddParam(CF_DoSuspRes_Txn, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_DoSuspRes_Txn, CF_ChanAction_SuspResArg_t *, context);

    UT_GenStub_Execute(CF_DoSuspRes_Txn, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_EnableDequeueCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_EnableDequeueCmd(const CF_EnableDequeueCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_EnableDequeueCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_EnableDequeueCmd, const CF_EnableDequeueCmd_t *, msg);

    UT_GenStub_Execute(CF_EnableDequeueCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_EnableDequeueCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_EnableDirPollingCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_EnableDirPollingCmd(const CF_EnableDirPollingCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_EnableDirPollingCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_EnableDirPollingCmd, const CF_EnableDirPollingCmd_t *, msg);

    UT_GenStub_Execute(CF_EnableDirPollingCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_EnableDirPollingCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_EnableEngineCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_EnableEngineCmd(const CF_EnableEngineCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_EnableEngineCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_EnableEngineCmd, const CF_EnableEngineCmd_t *, msg);

    UT_GenStub_Execute(CF_EnableEngineCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_EnableEngineCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FindTransactionBySequenceNumberAllChannels()
 * ----------------------------------------------------
 */
CF_Transaction_t *CF_FindTransactionBySequenceNumberAllChannels(CF_TransactionSeq_t ts, CF_EntityId_t eid)
{
    UT_GenStub_SetupReturnBuffer(CF_FindTransactionBySequenceNumberAllChannels, CF_Transaction_t *);

    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumberAllChannels, CF_TransactionSeq_t, ts);
    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumberAllChannels, CF_EntityId_t, eid);

    UT_GenStub_Execute(CF_FindTransactionBySequenceNumberAllChannels, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_FindTransactionBySequenceNumberAllChannels, CF_Transaction_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FreezeCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_FreezeCmd(const CF_FreezeCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_FreezeCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_FreezeCmd, const CF_FreezeCmd_t *, msg);

    UT_GenStub_Execute(CF_FreezeCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_FreezeCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_GetParamCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_GetParamCmd(const CF_GetParamCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_GetParamCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_GetParamCmd, const CF_GetParamCmd_t *, msg);

    UT_GenStub_Execute(CF_GetParamCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_GetParamCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_GetSetParamCmd()
 * ----------------------------------------------------
 */
void CF_GetSetParamCmd(bool is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num)
{
    UT_GenStub_AddParam(CF_GetSetParamCmd, bool, is_set);
    UT_GenStub_AddParam(CF_GetSetParamCmd, CF_GetSet_ValueID_t, param_id);
    UT_GenStub_AddParam(CF_GetSetParamCmd, uint32, value);
    UT_GenStub_AddParam(CF_GetSetParamCmd, uint8, chan_num);

    UT_GenStub_Execute(CF_GetSetParamCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_NoopCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_NoopCmd(const CF_NoopCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_NoopCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_NoopCmd, const CF_NoopCmd_t *, msg);

    UT_GenStub_Execute(CF_NoopCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_NoopCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PlaybackDirCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_PlaybackDirCmd(const CF_PlaybackDirCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_PlaybackDirCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_PlaybackDirCmd, const CF_PlaybackDirCmd_t *, msg);

    UT_GenStub_Execute(CF_PlaybackDirCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PlaybackDirCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PurgeHistory()
 * ----------------------------------------------------
 */
CF_CListTraverse_Status_t CF_PurgeHistory(CF_CListNode_t *node, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_PurgeHistory, CF_CListTraverse_Status_t);

    UT_GenStub_AddParam(CF_PurgeHistory, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_PurgeHistory, void *, arg);

    UT_GenStub_Execute(CF_PurgeHistory, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PurgeHistory, CF_CListTraverse_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PurgeQueueCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_PurgeQueueCmd(const CF_PurgeQueueCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_PurgeQueueCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_PurgeQueueCmd, const CF_PurgeQueueCmd_t *, msg);

    UT_GenStub_Execute(CF_PurgeQueueCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PurgeQueueCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PurgeTransaction()
 * ----------------------------------------------------
 */
CF_CListTraverse_Status_t CF_PurgeTransaction(CF_CListNode_t *node, void *ignored)
{
    UT_GenStub_SetupReturnBuffer(CF_PurgeTransaction, CF_CListTraverse_Status_t);

    UT_GenStub_AddParam(CF_PurgeTransaction, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_PurgeTransaction, void *, ignored);

    UT_GenStub_Execute(CF_PurgeTransaction, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PurgeTransaction, CF_CListTraverse_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ResetCountersCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_ResetCountersCmd(const CF_ResetCountersCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_ResetCountersCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_ResetCountersCmd, const CF_ResetCountersCmd_t *, msg);

    UT_GenStub_Execute(CF_ResetCountersCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_ResetCountersCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ResumeCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_ResumeCmd(const CF_ResumeCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_ResumeCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_ResumeCmd, const CF_ResumeCmd_t *, msg);

    UT_GenStub_Execute(CF_ResumeCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_ResumeCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_SendHkCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_SendHkCmd(const CF_SendHkCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_SendHkCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_SendHkCmd, const CF_SendHkCmd_t *, msg);

    UT_GenStub_Execute(CF_SendHkCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_SendHkCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_SetParamCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_SetParamCmd(const CF_SetParamCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_SetParamCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_SetParamCmd, const CF_SetParamCmd_t *, msg);

    UT_GenStub_Execute(CF_SetParamCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_SetParamCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_SuspendCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_SuspendCmd(const CF_SuspendCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_SuspendCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_SuspendCmd, const CF_SuspendCmd_t *, msg);

    UT_GenStub_Execute(CF_SuspendCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_SuspendCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ThawCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_ThawCmd(const CF_ThawCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_ThawCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_ThawCmd, const CF_ThawCmd_t *, msg);

    UT_GenStub_Execute(CF_ThawCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_ThawCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TsnChanAction()
 * ----------------------------------------------------
 */
int32 CF_TsnChanAction(const CF_Transaction_Payload_t *data, const char *cmdstr, CF_TsnChanAction_fn_t fn,
                       void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TsnChanAction, int32);

    UT_GenStub_AddParam(CF_TsnChanAction, const CF_Transaction_Payload_t *, data);
    UT_GenStub_AddParam(CF_TsnChanAction, const char *, cmdstr);
    UT_GenStub_AddParam(CF_TsnChanAction, CF_TsnChanAction_fn_t, fn);
    UT_GenStub_AddParam(CF_TsnChanAction, void *, context);

    UT_GenStub_Execute(CF_TsnChanAction, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TsnChanAction, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TxFileCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_TxFileCmd(const CF_TxFileCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_TxFileCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_TxFileCmd, const CF_TxFileCmd_t *, msg);

    UT_GenStub_Execute(CF_TxFileCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TxFileCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ValidateChunkSizeCmd()
 * ----------------------------------------------------
 */
CF_ChanAction_Status_t CF_ValidateChunkSizeCmd(CF_ChunkSize_t val, uint8 chan_num)
{
    UT_GenStub_SetupReturnBuffer(CF_ValidateChunkSizeCmd, CF_ChanAction_Status_t);

    UT_GenStub_AddParam(CF_ValidateChunkSizeCmd, CF_ChunkSize_t, val);
    UT_GenStub_AddParam(CF_ValidateChunkSizeCmd, uint8, chan_num);

    UT_GenStub_Execute(CF_ValidateChunkSizeCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_ValidateChunkSizeCmd, CF_ChanAction_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ValidateMaxOutgoingCmd()
 * ----------------------------------------------------
 */
CF_ChanAction_Status_t CF_ValidateMaxOutgoingCmd(uint32 val, uint8 chan_num)
{
    UT_GenStub_SetupReturnBuffer(CF_ValidateMaxOutgoingCmd, CF_ChanAction_Status_t);

    UT_GenStub_AddParam(CF_ValidateMaxOutgoingCmd, uint32, val);
    UT_GenStub_AddParam(CF_ValidateMaxOutgoingCmd, uint8, chan_num);

    UT_GenStub_Execute(CF_ValidateMaxOutgoingCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_ValidateMaxOutgoingCmd, CF_ChanAction_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WakeupCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WakeupCmd(const CF_WakeupCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_WakeupCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_WakeupCmd, const CF_WakeupCmd_t *, msg);

    UT_GenStub_Execute(CF_WakeupCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WakeupCmd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WriteQueueCmd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WriteQueueCmd(const CF_WriteQueueCmd_t *msg)
{
    UT_GenStub_SetupReturnBuffer(CF_WriteQueueCmd, CFE_Status_t);

    UT_GenStub_AddParam(CF_WriteQueueCmd, const CF_WriteQueueCmd_t *, msg);

    UT_GenStub_Execute(CF_WriteQueueCmd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WriteQueueCmd, CFE_Status_t);
}
