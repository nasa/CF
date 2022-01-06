/************************************************************************
** File: cf_utils.h
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
**  The CF Application utils header file
**
**
**
*************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cf_utils header
 */

#include "cf_utils.h"
#include "utgenstub.h"

extern void UT_DefaultHandler_CF_FindTransactionBySequenceNumber(void *, UT_EntryKey_t, const UT_StubContext_t *);
extern void UT_DefaultHandler_CF_FindUnusedTransaction(void *, UT_EntryKey_t, const UT_StubContext_t *);
extern void UT_DefaultHandler_CF_ResetHistory(void *, UT_EntryKey_t, const UT_StubContext_t *);
extern void UT_DefaultHandler_CF_TraverseAllTransactions(void *, UT_EntryKey_t, const UT_StubContext_t *);
extern void UT_DefaultHandler_CF_TraverseAllTransactions_All_Channels(void *, UT_EntryKey_t, const UT_StubContext_t *);
extern void UT_DefaultHandler_CF_WrappedOpenCreate(void *, UT_EntryKey_t, const UT_StubContext_t *);
extern void UT_DefaultHandler_CF_WriteHistoryQueueDataToFile(void *, UT_EntryKey_t, const UT_StubContext_t *);
extern void UT_DefaultHandler_CF_WriteQueueDataToFile(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FindTransactionBySequenceNumber()
 * ----------------------------------------------------
 */
CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *c, CF_TransactionSeq_t transaction_sequence_number,
                                                     CF_EntityId_t src_eid)
{
    UT_GenStub_SetupReturnBuffer(CF_FindTransactionBySequenceNumber, CF_Transaction_t *);

    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber, CF_Channel_t *, c);
    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber, CF_TransactionSeq_t, transaction_sequence_number);
    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber, CF_EntityId_t, src_eid);

    UT_GenStub_Execute(CF_FindTransactionBySequenceNumber, Basic, UT_DefaultHandler_CF_FindTransactionBySequenceNumber);

    return UT_GenStub_GetReturnValue(CF_FindTransactionBySequenceNumber, CF_Transaction_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FindTransactionBySequenceNumber_()
 * ----------------------------------------------------
 */
int CF_FindTransactionBySequenceNumber_(CF_CListNode_t *n, trans_seq_arg_t *context)
{
    UT_GenStub_SetupReturnBuffer(CF_FindTransactionBySequenceNumber_, int);

    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber_, CF_CListNode_t *, n);
    UT_GenStub_AddParam(CF_FindTransactionBySequenceNumber_, trans_seq_arg_t *, context);

    UT_GenStub_Execute(CF_FindTransactionBySequenceNumber_, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_FindTransactionBySequenceNumber_, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FindUnusedTransaction()
 * ----------------------------------------------------
 */
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *c)
{
    UT_GenStub_SetupReturnBuffer(CF_FindUnusedTransaction, CF_Transaction_t *);

    UT_GenStub_AddParam(CF_FindUnusedTransaction, CF_Channel_t *, c);

    UT_GenStub_Execute(CF_FindUnusedTransaction, Basic, UT_DefaultHandler_CF_FindUnusedTransaction);

    return UT_GenStub_GetReturnValue(CF_FindUnusedTransaction, CF_Transaction_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_FreeTransaction()
 * ----------------------------------------------------
 */
void CF_FreeTransaction(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_FreeTransaction, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_FreeTransaction, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_InsertSortPrio()
 * ----------------------------------------------------
 */
void CF_InsertSortPrio(CF_Transaction_t *t, CF_QueueIdx_t q)
{
    UT_GenStub_AddParam(CF_InsertSortPrio, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_InsertSortPrio, CF_QueueIdx_t, q);

    UT_GenStub_Execute(CF_InsertSortPrio, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_PrioSearch()
 * ----------------------------------------------------
 */
int CF_PrioSearch(CF_CListNode_t *node, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_PrioSearch, int);

    UT_GenStub_AddParam(CF_PrioSearch, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_PrioSearch, void *, context);

    UT_GenStub_Execute(CF_PrioSearch, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_PrioSearch, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ResetHistory()
 * ----------------------------------------------------
 */
void CF_ResetHistory(CF_Channel_t *c, CF_History_t *h)
{
    UT_GenStub_AddParam(CF_ResetHistory, CF_Channel_t *, c);
    UT_GenStub_AddParam(CF_ResetHistory, CF_History_t *, h);

    UT_GenStub_Execute(CF_ResetHistory, Basic, UT_DefaultHandler_CF_ResetHistory);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseAllTransactions()
 * ----------------------------------------------------
 */
int CF_TraverseAllTransactions(CF_Channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseAllTransactions, int);

    UT_GenStub_AddParam(CF_TraverseAllTransactions, CF_Channel_t *, c);
    UT_GenStub_AddParam(CF_TraverseAllTransactions, CF_TraverseAllTransactions_fn_t, fn);
    UT_GenStub_AddParam(CF_TraverseAllTransactions, void *, context);

    UT_GenStub_Execute(CF_TraverseAllTransactions, Basic, UT_DefaultHandler_CF_TraverseAllTransactions);

    return UT_GenStub_GetReturnValue(CF_TraverseAllTransactions, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseAllTransactions_()
 * ----------------------------------------------------
 */
int CF_TraverseAllTransactions_(CF_CListNode_t *n, traverse_all_args_t *args)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseAllTransactions_, int);

    UT_GenStub_AddParam(CF_TraverseAllTransactions_, CF_CListNode_t *, n);
    UT_GenStub_AddParam(CF_TraverseAllTransactions_, traverse_all_args_t *, args);

    UT_GenStub_Execute(CF_TraverseAllTransactions_, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TraverseAllTransactions_, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseAllTransactions_All_Channels()
 * ----------------------------------------------------
 */
int CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseAllTransactions_All_Channels, int);

    UT_GenStub_AddParam(CF_TraverseAllTransactions_All_Channels, CF_TraverseAllTransactions_fn_t, fn);
    UT_GenStub_AddParam(CF_TraverseAllTransactions_All_Channels, void *, context);

    UT_GenStub_Execute(CF_TraverseAllTransactions_All_Channels, Basic,
                       UT_DefaultHandler_CF_TraverseAllTransactions_All_Channels);

    return UT_GenStub_GetReturnValue(CF_TraverseAllTransactions_All_Channels, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseHistory()
 * ----------------------------------------------------
 */
int CF_TraverseHistory(CF_CListNode_t *n, trav_arg_t *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseHistory, int);

    UT_GenStub_AddParam(CF_TraverseHistory, CF_CListNode_t *, n);
    UT_GenStub_AddParam(CF_TraverseHistory, trav_arg_t *, context);

    UT_GenStub_Execute(CF_TraverseHistory, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TraverseHistory, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TraverseTransactions()
 * ----------------------------------------------------
 */
int CF_TraverseTransactions(CF_CListNode_t *n, trav_arg_t *context)
{
    UT_GenStub_SetupReturnBuffer(CF_TraverseTransactions, int);

    UT_GenStub_AddParam(CF_TraverseTransactions, CF_CListNode_t *, n);
    UT_GenStub_AddParam(CF_TraverseTransactions, trav_arg_t *, context);

    UT_GenStub_Execute(CF_TraverseTransactions, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TraverseTransactions, int);
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
int32 CF_WrappedLseek(osal_id_t fd, off_t offset, int mode)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedLseek, int32);

    UT_GenStub_AddParam(CF_WrappedLseek, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedLseek, off_t, offset);
    UT_GenStub_AddParam(CF_WrappedLseek, int, mode);

    UT_GenStub_Execute(CF_WrappedLseek, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedLseek, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedOpenCreate()
 * ----------------------------------------------------
 */
int32 CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedOpenCreate, int32);

    UT_GenStub_AddParam(CF_WrappedOpenCreate, osal_id_t *, fd);
    UT_GenStub_AddParam(CF_WrappedOpenCreate, const char *, fname);
    UT_GenStub_AddParam(CF_WrappedOpenCreate, int32, flags);
    UT_GenStub_AddParam(CF_WrappedOpenCreate, int32, access);

    UT_GenStub_Execute(CF_WrappedOpenCreate, Basic, UT_DefaultHandler_CF_WrappedOpenCreate);

    return UT_GenStub_GetReturnValue(CF_WrappedOpenCreate, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedRead()
 * ----------------------------------------------------
 */
int32 CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedRead, int32);

    UT_GenStub_AddParam(CF_WrappedRead, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedRead, void *, buf);
    UT_GenStub_AddParam(CF_WrappedRead, size_t, read_size);

    UT_GenStub_Execute(CF_WrappedRead, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedRead, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WrappedWrite()
 * ----------------------------------------------------
 */
int32 CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedWrite, int32);

    UT_GenStub_AddParam(CF_WrappedWrite, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedWrite, const void *, buf);
    UT_GenStub_AddParam(CF_WrappedWrite, size_t, write_size);

    UT_GenStub_Execute(CF_WrappedWrite, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedWrite, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WriteHistoryQueueDataToFile()
 * ----------------------------------------------------
 */
int32 CF_WriteHistoryQueueDataToFile(int32 fd, CF_Channel_t *c, CF_Direction_t dir)
{
    UT_GenStub_SetupReturnBuffer(CF_WriteHistoryQueueDataToFile, int32);

    UT_GenStub_AddParam(CF_WriteHistoryQueueDataToFile, int32, fd);
    UT_GenStub_AddParam(CF_WriteHistoryQueueDataToFile, CF_Channel_t *, c);
    UT_GenStub_AddParam(CF_WriteHistoryQueueDataToFile, CF_Direction_t, dir);

    UT_GenStub_Execute(CF_WriteHistoryQueueDataToFile, Basic, UT_DefaultHandler_CF_WriteHistoryQueueDataToFile);

    return UT_GenStub_GetReturnValue(CF_WriteHistoryQueueDataToFile, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WriteQueueDataToFile()
 * ----------------------------------------------------
 */
int32 CF_WriteQueueDataToFile(int32 fd, CF_Channel_t *c, CF_QueueIdx_t q)
{
    UT_GenStub_SetupReturnBuffer(CF_WriteQueueDataToFile, int32);

    UT_GenStub_AddParam(CF_WriteQueueDataToFile, int32, fd);
    UT_GenStub_AddParam(CF_WriteQueueDataToFile, CF_Channel_t *, c);
    UT_GenStub_AddParam(CF_WriteQueueDataToFile, CF_QueueIdx_t, q);

    UT_GenStub_Execute(CF_WriteQueueDataToFile, Basic, UT_DefaultHandler_CF_WriteQueueDataToFile);

    return UT_GenStub_GetReturnValue(CF_WriteQueueDataToFile, int32);
}
