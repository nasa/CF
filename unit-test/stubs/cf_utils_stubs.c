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
 * Auto-Generated stub implementations for functions defined in cf_utils header
 */

#include "cf_utils.h"
#include "utgenstub.h"

void UT_DefaultHandler_CF_FindTransactionBySequenceNumber(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_FindUnusedTransaction(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_ResetHistory(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_TraverseAllTransactions(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_TraverseAllTransactions_All_Channels(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_TxnStatus_IsError(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_WrappedOpenCreate(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_WriteHistoryQueueDataToFile(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_WriteTxnQueueDataToFile(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FindTransactionBySequenceNumber()
 * ----------------------------------------------------
 */
CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *chan, CF_TransactionSeq_t transaction_sequence_number,
                                                     CF_EntityId_t src_eid)
{
    UT_GenStub_SetupReturnBuffer(CF_FindTransactionBySequenceNumber, CF_Transaction_t *);

    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber, CF_Channel_t *, chan);
    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber, CF_TransactionSeq_t, transaction_sequence_number);
    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber, CF_EntityId_t, src_eid);

    UT_GenStub_Execute(CF_FindTransactionBySequenceNumber, Basic, UT_DefaultHandler_CF_FindTransactionBySequenceNumber);

    return UT_GenStub_GetReturnValue(CF_FindTransactionBySequenceNumber, CF_Transaction_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FindTransactionBySequenceNumber_Impl()
 * ----------------------------------------------------
 */
CFE_Status_t CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *node, CF_Traverse_TransSeqArg_t *context)
{
    UT_GenStub_SetupReturnBuffer(CF_FindTransactionBySequenceNumber_Impl, CFE_Status_t);

    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber_Impl, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber_Impl, CF_Traverse_TransSeqArg_t *, context);

    UT_GenStub_Execute(CF_FindTransactionBySequenceNumber_Impl, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_FindTransactionBySequenceNumber_Impl, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FindUnusedTransaction()
 * ----------------------------------------------------
 */
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *chan)
{
    UT_GenStub_SetupReturnBuffer(CF_FindUnusedTransaction, CF_Transaction_t *);

    UT_GenStub_AddParam(CF_FindUnusedTransaction, CF_Channel_t *, chan);

    UT_GenStub_Execute(CF_FindUnusedTransaction, Basic, UT_DefaultHandler_CF_FindUnusedTransaction);

    return UT_GenStub_GetReturnValue(CF_FindUnusedTransaction, CF_Transaction_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FreeTransaction()
 * ----------------------------------------------------
 */
void CF_FreeTransaction(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_FreeTransaction, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_FreeTransaction, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_InsertSortPrio()
 * ----------------------------------------------------
 */
void CF_InsertSortPrio(CF_Transaction_t *txn, CF_QueueIdx_t queue)
{
    UT_GenStub_AddParam(CF_InsertSortPrio, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_InsertSortPrio, CF_QueueIdx_t, queue);

    UT_GenStub_Execute(CF_InsertSortPrio, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PrioSearch()
 * ----------------------------------------------------
 */
CFE_Status_t CF_PrioSearch(CF_CListNode_t *node, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_PrioSearch, CFE_Status_t);

    UT_GenStub_AddParam(CF_PrioSearch, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_PrioSearch, void *, context);

    UT_GenStub_Execute(CF_PrioSearch, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PrioSearch, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ResetHistory()
 * ----------------------------------------------------
 */
void CF_ResetHistory(CF_Channel_t *chan, CF_History_t *history)
{
    UT_GenStub_AddParam(CF_ResetHistory, CF_Channel_t *, chan);
    UT_GenStub_AddParam(CF_ResetHistory, CF_History_t *, history);

    UT_GenStub_Execute(CF_ResetHistory, Basic, UT_DefaultHandler_CF_ResetHistory);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseAllTransactions()
 * ----------------------------------------------------
 */
CFE_Status_t CF_TraverseAllTransactions(CF_Channel_t *chan, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseAllTransactions, CFE_Status_t);

    UT_GenStub_AddParam(CF_TraverseAllTransactions, CF_Channel_t *, chan);
    UT_GenStub_AddParam(CF_TraverseAllTransactions, CF_TraverseAllTransactions_fn_t, fn);
    UT_GenStub_AddParam(CF_TraverseAllTransactions, void *, context);

    UT_GenStub_Execute(CF_TraverseAllTransactions, Basic, UT_DefaultHandler_CF_TraverseAllTransactions);

    return UT_GenStub_GetReturnValue(CF_TraverseAllTransactions, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseAllTransactions_All_Channels()
 * ----------------------------------------------------
 */
CFE_Status_t CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseAllTransactions_All_Channels, CFE_Status_t);

    UT_GenStub_AddParam(CF_TraverseAllTransactions_All_Channels, CF_TraverseAllTransactions_fn_t, fn);
    UT_GenStub_AddParam(CF_TraverseAllTransactions_All_Channels, void *, context);

    UT_GenStub_Execute(CF_TraverseAllTransactions_All_Channels, Basic,
                       UT_DefaultHandler_CF_TraverseAllTransactions_All_Channels);

    return UT_GenStub_GetReturnValue(CF_TraverseAllTransactions_All_Channels, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseAllTransactions_Impl()
 * ----------------------------------------------------
 */
CFE_Status_t CF_TraverseAllTransactions_Impl(CF_CListNode_t *node, CF_TraverseAll_Arg_t *args)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseAllTransactions_Impl, CFE_Status_t);

    UT_GenStub_AddParam(CF_TraverseAllTransactions_Impl, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_TraverseAllTransactions_Impl, CF_TraverseAll_Arg_t *, args);

    UT_GenStub_Execute(CF_TraverseAllTransactions_Impl, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TraverseAllTransactions_Impl, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Traverse_WriteHistoryQueueEntryToFile()
 * ----------------------------------------------------
 */
CFE_Status_t CF_Traverse_WriteHistoryQueueEntryToFile(CF_CListNode_t *node, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_Traverse_WriteHistoryQueueEntryToFile, CFE_Status_t);

    UT_GenStub_AddParam(CF_Traverse_WriteHistoryQueueEntryToFile, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_Traverse_WriteHistoryQueueEntryToFile, void *, arg);

    UT_GenStub_Execute(CF_Traverse_WriteHistoryQueueEntryToFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Traverse_WriteHistoryQueueEntryToFile, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Traverse_WriteTxnQueueEntryToFile()
 * ----------------------------------------------------
 */
CFE_Status_t CF_Traverse_WriteTxnQueueEntryToFile(CF_CListNode_t *node, void *arg)
{
    UT_GenStub_SetupReturnBuffer(CF_Traverse_WriteTxnQueueEntryToFile, CFE_Status_t);

    UT_GenStub_AddParam(CF_Traverse_WriteTxnQueueEntryToFile, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_Traverse_WriteTxnQueueEntryToFile, void *, arg);

    UT_GenStub_Execute(CF_Traverse_WriteTxnQueueEntryToFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Traverse_WriteTxnQueueEntryToFile, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TxnStatus_From_ConditionCode()
 * ----------------------------------------------------
 */
CF_TxnStatus_t CF_TxnStatus_From_ConditionCode(CF_CFDP_ConditionCode_t cc)
{
    UT_GenStub_SetupReturnBuffer(CF_TxnStatus_From_ConditionCode, CF_TxnStatus_t);

    UT_GenStub_AddParam(CF_TxnStatus_From_ConditionCode, CF_CFDP_ConditionCode_t, cc);

    UT_GenStub_Execute(CF_TxnStatus_From_ConditionCode, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TxnStatus_From_ConditionCode, CF_TxnStatus_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TxnStatus_IsError()
 * ----------------------------------------------------
 */
bool CF_TxnStatus_IsError(CF_TxnStatus_t txn_stat)
{
    UT_GenStub_SetupReturnBuffer(CF_TxnStatus_IsError, bool);

    UT_GenStub_AddParam(CF_TxnStatus_IsError, CF_TxnStatus_t, txn_stat);

    UT_GenStub_Execute(CF_TxnStatus_IsError, Basic, UT_DefaultHandler_CF_TxnStatus_IsError);

    return UT_GenStub_GetReturnValue(CF_TxnStatus_IsError, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TxnStatus_To_ConditionCode()
 * ----------------------------------------------------
 */
CF_CFDP_ConditionCode_t CF_TxnStatus_To_ConditionCode(CF_TxnStatus_t txn_stat)
{
    UT_GenStub_SetupReturnBuffer(CF_TxnStatus_To_ConditionCode, CF_CFDP_ConditionCode_t);

    UT_GenStub_AddParam(CF_TxnStatus_To_ConditionCode, CF_TxnStatus_t, txn_stat);

    UT_GenStub_Execute(CF_TxnStatus_To_ConditionCode, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TxnStatus_To_ConditionCode, CF_CFDP_ConditionCode_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedClose()
 * ----------------------------------------------------
 */
void CF_WrappedClose(osal_id_t fd)
{
    UT_GenStub_AddParam(CF_WrappedClose, osal_id_t, fd);

    UT_GenStub_Execute(CF_WrappedClose, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedLseek()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WrappedLseek(osal_id_t fd, off_t offset, int mode)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedLseek, CFE_Status_t);

    UT_GenStub_AddParam(CF_WrappedLseek, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedLseek, off_t, offset);
    UT_GenStub_AddParam(CF_WrappedLseek, int, mode);

    UT_GenStub_Execute(CF_WrappedLseek, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedLseek, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedOpenCreate()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedOpenCreate, CFE_Status_t);

    UT_GenStub_AddParam(CF_WrappedOpenCreate, osal_id_t *, fd);
    UT_GenStub_AddParam(CF_WrappedOpenCreate, const char *, fname);
    UT_GenStub_AddParam(CF_WrappedOpenCreate, int32, flags);
    UT_GenStub_AddParam(CF_WrappedOpenCreate, int32, access);

    UT_GenStub_Execute(CF_WrappedOpenCreate, Basic, UT_DefaultHandler_CF_WrappedOpenCreate);

    return UT_GenStub_GetReturnValue(CF_WrappedOpenCreate, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedRead()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedRead, CFE_Status_t);

    UT_GenStub_AddParam(CF_WrappedRead, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedRead, void *, buf);
    UT_GenStub_AddParam(CF_WrappedRead, size_t, read_size);

    UT_GenStub_Execute(CF_WrappedRead, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedRead, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedWrite()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedWrite, CFE_Status_t);

    UT_GenStub_AddParam(CF_WrappedWrite, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedWrite, const void *, buf);
    UT_GenStub_AddParam(CF_WrappedWrite, size_t, write_size);

    UT_GenStub_Execute(CF_WrappedWrite, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedWrite, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WriteHistoryEntryToFile()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WriteHistoryEntryToFile(osal_id_t fd, const CF_History_t *history)
{
    UT_GenStub_SetupReturnBuffer(CF_WriteHistoryEntryToFile, CFE_Status_t);

    UT_GenStub_AddParam(CF_WriteHistoryEntryToFile, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WriteHistoryEntryToFile, const CF_History_t *, history);

    UT_GenStub_Execute(CF_WriteHistoryEntryToFile, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WriteHistoryEntryToFile, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WriteHistoryQueueDataToFile()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WriteHistoryQueueDataToFile(osal_id_t fd, CF_Channel_t *chan, CF_Direction_t dir)
{
    UT_GenStub_SetupReturnBuffer(CF_WriteHistoryQueueDataToFile, CFE_Status_t);

    UT_GenStub_AddParam(CF_WriteHistoryQueueDataToFile, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WriteHistoryQueueDataToFile, CF_Channel_t *, chan);
    UT_GenStub_AddParam(CF_WriteHistoryQueueDataToFile, CF_Direction_t, dir);

    UT_GenStub_Execute(CF_WriteHistoryQueueDataToFile, Basic, UT_DefaultHandler_CF_WriteHistoryQueueDataToFile);

    return UT_GenStub_GetReturnValue(CF_WriteHistoryQueueDataToFile, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WriteTxnQueueDataToFile()
 * ----------------------------------------------------
 */
CFE_Status_t CF_WriteTxnQueueDataToFile(osal_id_t fd, CF_Channel_t *chan, CF_QueueIdx_t queue)
{
    UT_GenStub_SetupReturnBuffer(CF_WriteTxnQueueDataToFile, CFE_Status_t);

    UT_GenStub_AddParam(CF_WriteTxnQueueDataToFile, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WriteTxnQueueDataToFile, CF_Channel_t *, chan);
    UT_GenStub_AddParam(CF_WriteTxnQueueDataToFile, CF_QueueIdx_t, queue);

    UT_GenStub_Execute(CF_WriteTxnQueueDataToFile, Basic, UT_DefaultHandler_CF_WriteTxnQueueDataToFile);

    return UT_GenStub_GetReturnValue(CF_WriteTxnQueueDataToFile, CFE_Status_t);
}
