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

void UT_DefaultHandler_CF_ProcessGroundCommand(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_AbandonCmd()
 * ----------------------------------------------------
 */
void CF_AbandonCmd(const CF_AbandonCmd_t *msg)
{
    UT_GenStub_AddParam(CF_AbandonCmd, const CF_AbandonCmd_t *, msg);

    UT_GenStub_Execute(CF_AbandonCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CancelCmd()
 * ----------------------------------------------------
 */
void CF_CancelCmd(const CF_CancelCmd_t *msg)
{
    UT_GenStub_AddParam(CF_CancelCmd, const CF_CancelCmd_t *, msg);

    UT_GenStub_Execute(CF_CancelCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CmdAbandon_Txn()
 * ----------------------------------------------------
 */
void CF_CmdAbandon_Txn(CF_Transaction_t *txn, void *ignored)
{
    UT_GenStub_AddParam(CF_CmdAbandon_Txn, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CmdAbandon_Txn, void *, ignored);

    UT_GenStub_Execute(CF_CmdAbandon_Txn, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CmdCancel_Txn()
 * ----------------------------------------------------
 */
void CF_CmdCancel_Txn(CF_Transaction_t *txn, void *ignored)
{
    UT_GenStub_AddParam(CF_CmdCancel_Txn, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CmdCancel_Txn, void *, ignored);

    UT_GenStub_Execute(CF_CmdCancel_Txn, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CmdValidateChunkSize()
 * ----------------------------------------------------
 */
CFE_Status_t CF_CmdValidateChunkSize(uint32 val, uint8 chan_num)
{
    UT_GenStub_SetupReturnBuffer(CF_CmdValidateChunkSize, CFE_Status_t);

    UT_GenStub_AddParam(CF_CmdValidateChunkSize, uint32, val);
    UT_GenStub_AddParam(CF_CmdValidateChunkSize, uint8, chan_num);

    UT_GenStub_Execute(CF_CmdValidateChunkSize, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CmdValidateChunkSize, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CmdValidateMaxOutgoing()
 * ----------------------------------------------------
 */
CFE_Status_t CF_CmdValidateMaxOutgoing(uint32 val, uint8 chan_num)
{
    UT_GenStub_SetupReturnBuffer(CF_CmdValidateMaxOutgoing, CFE_Status_t);

    UT_GenStub_AddParam(CF_CmdValidateMaxOutgoing, uint32, val);
    UT_GenStub_AddParam(CF_CmdValidateMaxOutgoing, uint8, chan_num);

    UT_GenStub_Execute(CF_CmdValidateMaxOutgoing, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CmdValidateMaxOutgoing, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DisableDequeueCmd()
 * ----------------------------------------------------
 */
void CF_DisableDequeueCmd(const CF_DisableDequeueCmd_t *msg)
{
    UT_GenStub_AddParam(CF_DisableDequeueCmd, const CF_DisableDequeueCmd_t *, msg);

    UT_GenStub_Execute(CF_DisableDequeueCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DisableEngineCmd()
 * ----------------------------------------------------
 */
void CF_DisableEngineCmd(const CF_DisableEngineCmd_t *msg)
{
    UT_GenStub_AddParam(CF_DisableEngineCmd, const CF_DisableEngineCmd_t *, msg);

    UT_GenStub_Execute(CF_DisableEngineCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DisablePolldirCmd()
 * ----------------------------------------------------
 */
void CF_DisablePolldirCmd(const CF_DisableDirPollingCmd_t *msg)
{
    UT_GenStub_AddParam(CF_DisablePolldirCmd, const CF_DisableDirPollingCmd_t *, msg);

    UT_GenStub_Execute(CF_DisablePolldirCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoChanAction()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DoChanAction(const CF_UnionArgs_Payload_t *data, const char *errstr, CF_ChanActionFn_t fn,
                             void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_DoChanAction, CFE_Status_t);

    UT_GenStub_AddParam(CF_DoChanAction, const CF_UnionArgs_Payload_t *, data);
    UT_GenStub_AddParam(CF_DoChanAction, const char *, errstr);
    UT_GenStub_AddParam(CF_DoChanAction, CF_ChanActionFn_t, fn);
    UT_GenStub_AddParam(CF_DoChanAction, void *, context);

    UT_GenStub_Execute(CF_DoChanAction, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoChanAction, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoEnableDisableDequeue()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DoEnableDisableDequeue(uint8 chan_num, const CF_ChanAction_BoolArg_t *context)
{
    UT_GenStub_SetupReturnBuffer(CF_DoEnableDisableDequeue, CFE_Status_t);

    UT_GenStub_AddParam(CF_DoEnableDisableDequeue, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoEnableDisableDequeue, const CF_ChanAction_BoolArg_t *, context);

    UT_GenStub_Execute(CF_DoEnableDisableDequeue, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoEnableDisableDequeue, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoEnableDisablePolldir()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DoEnableDisablePolldir(uint8 chan_num, const CF_ChanAction_BoolMsgArg_t *context)
{
    UT_GenStub_SetupReturnBuffer(CF_DoEnableDisablePolldir, CFE_Status_t);

    UT_GenStub_AddParam(CF_DoEnableDisablePolldir, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoEnableDisablePolldir, const CF_ChanAction_BoolMsgArg_t *, context);

    UT_GenStub_Execute(CF_DoEnableDisablePolldir, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoEnableDisablePolldir, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoFreezeThaw()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DoFreezeThaw(uint8 chan_num, const CF_ChanAction_BoolArg_t *context)
{
    UT_GenStub_SetupReturnBuffer(CF_DoFreezeThaw, CFE_Status_t);

    UT_GenStub_AddParam(CF_DoFreezeThaw, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoFreezeThaw, const CF_ChanAction_BoolArg_t *, context);

    UT_GenStub_Execute(CF_DoFreezeThaw, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoFreezeThaw, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DoPurgeQueue()
 * ----------------------------------------------------
 */
CFE_Status_t CF_DoPurgeQueue(uint8 chan_num, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_DoPurgeQueue, CFE_Status_t);

    UT_GenStub_AddParam(CF_DoPurgeQueue, uint8, chan_num);
    UT_GenStub_AddParam(CF_DoPurgeQueue, void *, arg);

    UT_GenStub_Execute(CF_DoPurgeQueue, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_DoPurgeQueue, CFE_Status_t);
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
void CF_EnableDequeueCmd(const CF_EnableDequeueCmd_t *msg)
{
    UT_GenStub_AddParam(CF_EnableDequeueCmd, const CF_EnableDequeueCmd_t *, msg);

    UT_GenStub_Execute(CF_EnableDequeueCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_EnableEngineCmd()
 * ----------------------------------------------------
 */
void CF_EnableEngineCmd(const CF_EnableEngineCmd_t *msg)
{
    UT_GenStub_AddParam(CF_EnableEngineCmd, const CF_EnableEngineCmd_t *, msg);

    UT_GenStub_Execute(CF_EnableEngineCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_EnablePolldirCmd()
 * ----------------------------------------------------
 */
void CF_EnablePolldirCmd(const CF_EnableDirPollingCmd_t *msg)
{
    UT_GenStub_AddParam(CF_EnablePolldirCmd, const CF_EnableDirPollingCmd_t *, msg);

    UT_GenStub_Execute(CF_EnablePolldirCmd, Basic, NULL);
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
void CF_FreezeCmd(const CF_FreezeCmd_t *msg)
{
    UT_GenStub_AddParam(CF_FreezeCmd, const CF_FreezeCmd_t *, msg);

    UT_GenStub_Execute(CF_FreezeCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_GetParamCmd()
 * ----------------------------------------------------
 */
void CF_GetParamCmd(const CF_GetParamCmd_t *msg)
{
    UT_GenStub_AddParam(CF_GetParamCmd, const CF_GetParamCmd_t *, msg);

    UT_GenStub_Execute(CF_GetParamCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_GetSetParamCmd()
 * ----------------------------------------------------
 */
void CF_GetSetParamCmd(uint8 is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num)
{
    UT_GenStub_AddParam(CF_GetSetParamCmd, uint8, is_set);
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
void CF_NoopCmd(const CF_NoopCmd_t *msg)
{
    UT_GenStub_AddParam(CF_NoopCmd, const CF_NoopCmd_t *, msg);

    UT_GenStub_Execute(CF_NoopCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PlaybackDirCmd()
 * ----------------------------------------------------
 */
void CF_PlaybackDirCmd(const CF_PlaybackDirCmd_t *msg)
{
    UT_GenStub_AddParam(CF_PlaybackDirCmd, const CF_PlaybackDirCmd_t *, msg);

    UT_GenStub_Execute(CF_PlaybackDirCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ProcessGroundCommand()
 * ----------------------------------------------------
 */
void CF_ProcessGroundCommand(CFE_SB_Buffer_t *msg)
{
    UT_GenStub_AddParam(CF_ProcessGroundCommand, CFE_SB_Buffer_t *, msg);

    UT_GenStub_Execute(CF_ProcessGroundCommand, Basic, UT_DefaultHandler_CF_ProcessGroundCommand);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PurgeHistory()
 * ----------------------------------------------------
 */
CFE_Status_t CF_PurgeHistory(CF_CListNode_t *node, CF_Channel_t *chan)
{
    UT_GenStub_SetupReturnBuffer(CF_PurgeHistory, CFE_Status_t);

    UT_GenStub_AddParam(CF_PurgeHistory, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_PurgeHistory, CF_Channel_t *, chan);

    UT_GenStub_Execute(CF_PurgeHistory, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PurgeHistory, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PurgeQueueCmd()
 * ----------------------------------------------------
 */
void CF_PurgeQueueCmd(const CF_PurgeQueueCmd_t *msg)
{
    UT_GenStub_AddParam(CF_PurgeQueueCmd, const CF_PurgeQueueCmd_t *, msg);

    UT_GenStub_Execute(CF_PurgeQueueCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PurgeTransaction()
 * ----------------------------------------------------
 */
CFE_Status_t CF_PurgeTransaction(CF_CListNode_t *node, void *ignored)
{
    UT_GenStub_SetupReturnBuffer(CF_PurgeTransaction, CFE_Status_t);

    UT_GenStub_AddParam(CF_PurgeTransaction, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_PurgeTransaction, void *, ignored);

    UT_GenStub_Execute(CF_PurgeTransaction, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PurgeTransaction, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ResetCmd()
 * ----------------------------------------------------
 */
void CF_ResetCmd(const CF_ResetCmd_t *msg)
{
    UT_GenStub_AddParam(CF_ResetCmd, const CF_ResetCmd_t *, msg);

    UT_GenStub_Execute(CF_ResetCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ResumeCmd()
 * ----------------------------------------------------
 */
void CF_ResumeCmd(const CF_ResumeCmd_t *msg)
{
    UT_GenStub_AddParam(CF_ResumeCmd, const CF_ResumeCmd_t *, msg);

    UT_GenStub_Execute(CF_ResumeCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_SetParamCmd()
 * ----------------------------------------------------
 */
void CF_SetParamCmd(const CF_SetParamCmd_t *msg)
{
    UT_GenStub_AddParam(CF_SetParamCmd, const CF_SetParamCmd_t *, msg);

    UT_GenStub_Execute(CF_SetParamCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_SuspendCmd()
 * ----------------------------------------------------
 */
void CF_SuspendCmd(const CF_SuspendCmd_t *msg)
{
    UT_GenStub_AddParam(CF_SuspendCmd, const CF_SuspendCmd_t *, msg);

    UT_GenStub_Execute(CF_SuspendCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ThawCmd()
 * ----------------------------------------------------
 */
void CF_ThawCmd(const CF_ThawCmd_t *msg)
{
    UT_GenStub_AddParam(CF_ThawCmd, const CF_ThawCmd_t *, msg);

    UT_GenStub_Execute(CF_ThawCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TsnChanAction()
 * ----------------------------------------------------
 */
CFE_Status_t CF_TsnChanAction(const CF_Transaction_Payload_t *payload, const char *cmdstr, CF_TsnChanAction_fn_t fn,
                              void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TsnChanAction, CFE_Status_t);

    UT_GenStub_AddParam(CF_TsnChanAction, const CF_Transaction_Payload_t *, payload);
    UT_GenStub_AddParam(CF_TsnChanAction, const char *, cmdstr);
    UT_GenStub_AddParam(CF_TsnChanAction, CF_TsnChanAction_fn_t, fn);
    UT_GenStub_AddParam(CF_TsnChanAction, void *, context);

    UT_GenStub_Execute(CF_TsnChanAction, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TsnChanAction, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TxFileCmd()
 * ----------------------------------------------------
 */
void CF_TxFileCmd(const CF_TxFileCmd_t *msg)
{
    UT_GenStub_AddParam(CF_TxFileCmd, const CF_TxFileCmd_t *, msg);

    UT_GenStub_Execute(CF_TxFileCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WriteQueueCmd()
 * ----------------------------------------------------
 */
void CF_WriteQueueCmd(const CF_WriteQueueCmd_t *msg)
{
    UT_GenStub_AddParam(CF_WriteQueueCmd, const CF_WriteQueueCmd_t *, msg);

    UT_GenStub_Execute(CF_WriteQueueCmd, Basic, NULL);
}
