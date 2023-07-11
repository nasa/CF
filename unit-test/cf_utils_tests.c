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

/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_test_alt_handler.h"
#include "cf_utils.h"
#include "cf_events.h"

/* A value that may be passed to stubs accepting osal_id_t values */
#define UT_CF_OS_OBJID OS_ObjectIdFromInteger(1)

typedef struct
{
    CF_Transaction_t *t;
    void *            context;
} UT_Callback_CF_TraverseAllTransactions_context_t;

/*******************************************************************************
**
**  cf_utils_tests Setup and Teardown
**
*******************************************************************************/

void cf_utils_tests_Setup(void)
{
    cf_tests_Setup();
}

void cf_utils_tests_Teardown(void)
{
    cf_tests_Teardown();
}

/*******************************************************************************
**
**  cf_utils_tests specific Any functions  NOTE:Some of these may be better as global
**
*******************************************************************************/

CF_QueueIdx_t Any_cf_queue_index_t(void)
{
    return (CF_QueueIdx_t)Any_uint16_LessThan(CF_QueueIdx_NUM);
}

CF_Direction_t Any_direction_t(void)
{
    return (CF_Direction_t)Any_uint8_LessThan(CF_Direction_NUM);
}

CF_CFDP_ConditionCode_t Any_condition_code_t(void)
{
    uint8 codes[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 15};
    return (CF_CFDP_ConditionCode_t)Any_uint8_FromThese(codes, sizeof(codes) / sizeof(codes[0]));
}

void local_handler_OS_close(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
}

/*******************************************************************************
**
**  cf_utils_tests test functions
**
*******************************************************************************/

/*----------------------------------------------------------------
 *
 * A UT-specific callback that can be used with CF_TraverseAllTransactions
 *
 *-----------------------------------------------------------------*/
static void UT_Callback_CF_TraverseAllTransactions(CF_Transaction_t *t, void *context)
{
    UT_Callback_CF_TraverseAllTransactions_context_t *ctxt = UT_CF_GetContextBuffer(
        UT_KEY(UT_Callback_CF_TraverseAllTransactions), UT_Callback_CF_TraverseAllTransactions_context_t);

    if (ctxt)
    {
        ctxt->t       = t;
        ctxt->context = context;
    }
}

/*----------------------------------------------------------------
 *
 * A simple handler that just sets the "t" output in the state object
 *
 *-----------------------------------------------------------------*/
static void UT_AltHandler_CF_CList_Traverse_SeqArg_SetTxn(void *UserObj, UT_EntryKey_t FuncKey,
                                                          const UT_StubContext_t *Context)
{
    CF_Traverse_TransSeqArg_t *arg = UT_Hook_GetArgValueByName(Context, "context", CF_Traverse_TransSeqArg_t *);
    arg->t                         = UserObj;
}

/*******************************************************************************
**
**  cf_utils.h function tests
**
*******************************************************************************/

void Test_CF_ResetHistory(void)
{
    /* Test case for:
     * void CF_ResetHistory(CF_Channel_t *c, CF_History_t *h)
     */
    CF_History_t h;

    memset(&h, 0, sizeof(h));

    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_HIST] = 4;

    /* nominal call */
    UtAssert_VOIDCALL(CF_ResetHistory(&CF_AppData.engine.channels[UT_CFDP_CHANNEL], &h));
}

void Test_CF_FindUnusedTransaction(void)
{
    /* Test case for:
     * CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *c)
     */
    CF_Channel_t *   c;
    CF_Transaction_t txn;
    CF_History_t     hist;

    memset(&hist, 0, sizeof(hist));
    memset(&txn, 0, sizeof(txn));
    memset(&CF_AppData, 0, sizeof(CF_AppData));
    c                                                                  = &CF_AppData.engine.channels[UT_CFDP_CHANNEL];
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_FREE] = 2;
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_HIST_FREE] = 1;
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_HIST]      = 1;

    UtAssert_NULL(CF_FindUnusedTransaction(c));

    c->qs[CF_QueueIdx_FREE]      = &txn.cl_node;
    c->qs[CF_QueueIdx_HIST_FREE] = &hist.cl_node;
    c->qs[CF_QueueIdx_HIST]      = NULL;
    UtAssert_ADDRESS_EQ(CF_FindUnusedTransaction(c), &txn);
    UtAssert_ADDRESS_EQ(txn.history, &hist);

    c->qs[CF_QueueIdx_FREE]      = &txn.cl_node;
    c->qs[CF_QueueIdx_HIST_FREE] = NULL;
    c->qs[CF_QueueIdx_HIST]      = &hist.cl_node;
    UtAssert_ADDRESS_EQ(CF_FindUnusedTransaction(c), &txn);
    UtAssert_ADDRESS_EQ(txn.history, &hist);
}

void Test_CF_FreeTransaction(void)
{
    /* Test case for:
     * void CF_FreeTransaction(CF_Transaction_t *t)
     */
    CF_Transaction_t *t;

    memset(&CF_AppData, 0, sizeof(CF_AppData));
    t = &CF_AppData.engine.transactions[UT_CFDP_CHANNEL];

    UtAssert_VOIDCALL(CF_FreeTransaction(t));

    UtAssert_UINT32_EQ(t->state, CF_TxnState_IDLE);
    UtAssert_UINT32_EQ(t->flags.com.q_index, CF_QueueIdx_FREE);
}

void Test_CF_FindTransactionBySequenceNumber_Impl(void)
{
    /* Test case for:
     * int CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *n, CF_Traverse_TransSeqArg_t *context)
     */
    CF_Traverse_TransSeqArg_t ctxt;
    CF_Transaction_t          txn;
    CF_History_t              hist;

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    memset(&ctxt, 0, sizeof(ctxt));

    txn.history = &hist;

    /* non-matching eid and non-matching sequence */
    hist.src_eid                     = 12;
    ctxt.src_eid                     = 34;
    hist.seq_num                     = 56;
    ctxt.transaction_sequence_number = 78;
    UtAssert_INT32_EQ(CF_FindTransactionBySequenceNumber_Impl(&txn.cl_node, &ctxt), 0);
    UtAssert_NULL(ctxt.t);

    /* matching eid and non-matching sequence */
    hist.src_eid                     = 13;
    ctxt.src_eid                     = 13;
    hist.seq_num                     = 56;
    ctxt.transaction_sequence_number = 78;
    UtAssert_INT32_EQ(CF_FindTransactionBySequenceNumber_Impl(&txn.cl_node, &ctxt), 0);
    UtAssert_NULL(ctxt.t);

    /* non-matching eid and matching sequence */
    hist.src_eid                     = 12;
    ctxt.src_eid                     = 34;
    hist.seq_num                     = 57;
    ctxt.transaction_sequence_number = 57;
    UtAssert_INT32_EQ(CF_FindTransactionBySequenceNumber_Impl(&txn.cl_node, &ctxt), 0);
    UtAssert_NULL(ctxt.t);

    /* matching eid and matching sequence */
    hist.src_eid                     = 23;
    ctxt.src_eid                     = 23;
    hist.seq_num                     = 67;
    ctxt.transaction_sequence_number = 67;
    UtAssert_INT32_EQ(CF_FindTransactionBySequenceNumber_Impl(&txn.cl_node, &ctxt), 1);
    UtAssert_ADDRESS_EQ(ctxt.t, &txn);
}

void Test_CF_FindTransactionBySequenceNumber(void)
{
    /* Test case for:
     * CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *c, CF_TransactionSeq_t
     * transaction_sequence_number, CF_EntityId_t src_eid)
     */

    CF_Transaction_t *t;
    CF_Channel_t *    c;

    memset(&CF_AppData, 0, sizeof(CF_AppData));
    c = &CF_AppData.engine.channels[UT_CFDP_CHANNEL];

    UtAssert_NULL(CF_FindTransactionBySequenceNumber(c, 12, 34));
    UtAssert_STUB_COUNT(CF_CList_Traverse, 4); /* this checks 4 different queues */

    t = &CF_AppData.engine.transactions[UT_CFDP_CHANNEL];
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_SeqArg_SetTxn, t);
    UtAssert_ADDRESS_EQ(CF_FindTransactionBySequenceNumber(c, 12, 34), t);
}

/* CF_DequeueTransaction tests */

void Test_cf_dequeue_transaction_Call_CF_CList_Remove_AndDecrement_q_size(void)
{
    /* Arrange */
    CF_Transaction_t arg_t;
    uint8            chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_CListNode_t **expected_head;
    CF_CListNode_t * expected_cl_node;
    uint16           initial_q_size = Any_uint16_Except(0); /* 0 will CF_Assert */
    uint16           updated_q_size;

    CF_CList_Remove_context_t context_clist_remove;

    memset(&arg_t, 0, sizeof(arg_t));

    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_clist_remove, sizeof(context_clist_remove), false);

    arg_t.chan_num   = chan_num;
    expected_head    = &CF_AppData.engine.channels[arg_t.chan_num].qs[arg_t.flags.com.q_index];
    expected_cl_node = &arg_t.cl_node;

    CF_AppData.hk.channel_hk[arg_t.chan_num].q_size[arg_t.flags.com.q_index] = initial_q_size;

    /* Act */
    CF_DequeueTransaction(&arg_t);

    updated_q_size = CF_AppData.hk.channel_hk[arg_t.chan_num].q_size[arg_t.flags.com.q_index];

    /* Assert */
    UtAssert_ADDRESS_EQ(context_clist_remove.head, expected_head);
    UtAssert_ADDRESS_EQ(context_clist_remove.node, expected_cl_node);
    UtAssert_True(updated_q_size == initial_q_size - 1, "q_size is %d and that is 1 less than initial value %d",
                  updated_q_size, initial_q_size);
}

/* CF_MoveTransaction tests */

void Test_cf_move_transaction_Call_CF_CList_InsertBack_AndSet_q_index_ToGiven_q(void)
{
    /* Arrange */
    CF_Transaction_t              t;
    CF_Transaction_t *            arg_t    = &t;
    uint8                         chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_CListNode_t **             expected_remove_head;
    CF_CListNode_t *              expected_remove_node;
    CF_CListNode_t **             expected_insert_back_head;
    CF_CListNode_t *              expected_insert_back_node;
    CF_QueueIdx_t                 arg_q = Any_cf_queue_index_t();
    CF_CList_Remove_context_t     context_clist_remove;
    CF_CList_InsertBack_context_t context_clist_insert_back;

    memset(&t, 0, sizeof(t));

    arg_t->chan_num = chan_num;

    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_clist_remove, sizeof(context_clist_remove), false);

    expected_remove_head = &CF_AppData.engine.channels[arg_t->chan_num].qs[arg_t->flags.com.q_index];
    expected_remove_node = &arg_t->cl_node;

    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_clist_insert_back, sizeof(context_clist_insert_back), false);

    expected_insert_back_head = &CF_AppData.engine.channels[arg_t->chan_num].qs[arg_q];
    expected_insert_back_node = &arg_t->cl_node;

    /* Queue size needes to be >= 1 */
    CF_AppData.hk.channel_hk[arg_t->chan_num].q_size[arg_t->flags.com.q_index] = 1;

    /* Act */
    CF_MoveTransaction(arg_t, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Remove, 1);
    UtAssert_ADDRESS_EQ(context_clist_remove.head, expected_remove_head);
    UtAssert_ADDRESS_EQ(context_clist_remove.node, expected_remove_node);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.head, expected_insert_back_head);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.node, expected_insert_back_node);
    UtAssert_True(arg_t->flags.com.q_index == arg_q,
                  "t->flags.com.q_index set to %u and should equal passed in q value %u", arg_t->flags.com.q_index,
                  arg_q);
}

/* CF_CList_Remove_Ex tests */

void Test_CF_CList_Remove_Ex_Call_CF_CList_Remove_AndDecrement_q_size(void)
{
    /* Arrange */
    CF_Channel_t *   arg_c     = &CF_AppData.engine.channels[Any_uint32_LessThan(CF_NUM_CHANNELS)];
    CF_QueueIdx_t    arg_index = Any_cf_queue_index_t();
    CF_CListNode_t   node;
    CF_CListNode_t * arg_node = &node;
    CF_CListNode_t **expected_remove_head;
    CF_CListNode_t * expected_remove_node;
    uint16           initial_q_size = Any_uint16_Except(0);
    uint16           updated_q_size;

    CF_CList_Remove_context_t context_clist_remove;

    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_clist_remove, sizeof(context_clist_remove), false);

    expected_remove_head = &arg_c->qs[arg_index];
    expected_remove_node = arg_node;

    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index] = initial_q_size;

    /* Act */
    CF_CList_Remove_Ex(arg_c, arg_index, arg_node);

    updated_q_size = CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index];

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Remove, 1);
    UtAssert_ADDRESS_EQ(context_clist_remove.head, expected_remove_head);
    UtAssert_ADDRESS_EQ(context_clist_remove.node, expected_remove_node);
    UtAssert_True(updated_q_size == initial_q_size - 1, "q_size is %d and that is 1 less than initial value %d",
                  updated_q_size, initial_q_size);
}

/* CF_CList_InsertAfter_Ex tests */

void Test_CF_CList_InsertAfter_Ex_Call_CF_CList_InsertAfter_AndIncrement_q_size(void)
{
    /* Arrange */
    CF_Channel_t *   arg_c     = &CF_AppData.engine.channels[Any_uint32_LessThan(CF_NUM_CHANNELS)];
    CF_QueueIdx_t    arg_index = Any_cf_queue_index_t();
    CF_CListNode_t   start;
    CF_CListNode_t * arg_start = &start;
    CF_CListNode_t   after;
    CF_CListNode_t * arg_after                  = &after;
    CF_CListNode_t **expected_insert_after_head = &arg_c->qs[arg_index];
    uint16           initial_q_size             = Any_uint16();
    uint16           updated_q_size;

    CF_CList_InsertAfter_context_t context_CF_CList_InsertAfter;

    UT_SetDataBuffer(UT_KEY(CF_CList_InsertAfter), &context_CF_CList_InsertAfter, sizeof(context_CF_CList_InsertAfter),
                     false);

    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index] = initial_q_size;

    /* Act */
    CF_CList_InsertAfter_Ex(arg_c, arg_index, arg_start, arg_after);

    updated_q_size = CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index];

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_InsertAfter, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.head, expected_insert_after_head);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.start, arg_start);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.after, arg_after);
    UtAssert_True(updated_q_size == (uint16)(initial_q_size + 1),
                  "q_size is %d and that is 1 more than initial value %d", updated_q_size, initial_q_size);
}

/* CF_CList_InsertBack_Ex tests */

void Test_CF_CList_InsertBack_Ex_Call_CF_CList_InsertBack_AndIncrement_q_size(void)
{
    /* Arrange */
    CF_Channel_t *   arg_c     = &CF_AppData.engine.channels[Any_uint32_LessThan(CF_NUM_CHANNELS)];
    CF_QueueIdx_t    arg_index = Any_cf_queue_index_t();
    CF_CListNode_t   node;
    CF_CListNode_t * arg_node       = &node;
    uint16           initial_q_size = Any_uint16();
    CF_CListNode_t **expected_insert_back_head;
    CF_CListNode_t * expected_insert_back_node;
    uint16           updated_q_size;

    CF_CList_InsertBack_context_t context_clist_insert_back;
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_clist_insert_back, sizeof(context_clist_insert_back), false);

    expected_insert_back_head = &arg_c->qs[arg_index];
    expected_insert_back_node = arg_node;

    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index] = initial_q_size;

    /* Act */
    CF_CList_InsertBack_Ex(arg_c, arg_index, arg_node);

    updated_q_size = CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index];

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.head, expected_insert_back_head);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.node, expected_insert_back_node);
    UtAssert_True(updated_q_size == (uint16)(initial_q_size + 1),
                  "q_size is %d and that is 1 more than initial value %d", updated_q_size, initial_q_size);
}

/*******************************************************************************
**
**  CF_Traverse_WriteHistoryQueueEntryToFile tests
**
*******************************************************************************/

void Test_CF_Traverse_WriteHistoryQueueEntryToFile(void)
{
    /* Test case for:
     * int CF_Traverse_WriteHistoryQueueEntryToFile(CF_CListNode_t *n, void *arg);
     */
    CF_History_t                      hist;
    CF_Traverse_WriteHistoryFileArg_t args;

    memset(&hist, 0, sizeof(hist));
    memset(&args, 0, sizeof(args));

    /* nominal, if everything works, should continue */
    hist.dir        = CF_Direction_TX;
    args.filter_dir = CF_Direction_TX;
    UtAssert_INT32_EQ(CF_Traverse_WriteHistoryQueueEntryToFile(&hist.cl_node, &args), CF_CLIST_CONT);
    UtAssert_UINT32_EQ(args.counter, 1);
    UtAssert_BOOL_FALSE(args.error);

    /* filter disabled (anything goes) */
    hist.dir        = CF_Direction_RX;
    args.filter_dir = CF_Direction_NUM;
    UtAssert_INT32_EQ(CF_Traverse_WriteHistoryQueueEntryToFile(&hist.cl_node, &args), CF_CLIST_CONT);
    UtAssert_UINT32_EQ(args.counter, 2);
    UtAssert_BOOL_FALSE(args.error);

    /* filter no match (does not write) */
    hist.dir        = CF_Direction_RX;
    args.filter_dir = CF_Direction_TX;
    UtAssert_INT32_EQ(CF_Traverse_WriteHistoryQueueEntryToFile(&hist.cl_node, &args), CF_CLIST_CONT);
    UtAssert_UINT32_EQ(args.counter, 2); /* no increment */
    UtAssert_BOOL_FALSE(args.error);

    /* Setup for failure */
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, -1);
    hist.dir        = CF_Direction_RX;
    args.filter_dir = CF_Direction_RX;
    UtAssert_INT32_EQ(CF_Traverse_WriteHistoryQueueEntryToFile(&hist.cl_node, &args), CF_CLIST_EXIT);
    UtAssert_UINT32_EQ(args.counter, 2); /* no increment */
    UtAssert_BOOL_TRUE(args.error);
}

/*******************************************************************************
**
**  CF_Traverse_WriteTxnQueueEntryToFile tests
**
*******************************************************************************/

void Test_CF_Traverse_WriteTxnQueueEntryToFile(void)
{
    /* Test case for:
     * int CF_Traverse_WriteTxnQueueEntryToFile(CF_CListNode_t *n, void *arg);
     */
    CF_Transaction_t              txn;
    CF_History_t                  hist;
    CF_Traverse_WriteTxnFileArg_t args;

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    memset(&args, 0, sizeof(args));
    txn.history = &hist;

    /* nominal, if everything works, should continue */
    UtAssert_INT32_EQ(CF_Traverse_WriteTxnQueueEntryToFile(&txn.cl_node, &args), CF_CLIST_CONT);
    UtAssert_UINT32_EQ(args.counter, 1);
    UtAssert_BOOL_FALSE(args.error);

    /* Setup for failure */
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, -1);
    UtAssert_INT32_EQ(CF_Traverse_WriteTxnQueueEntryToFile(&txn.cl_node, &args), CF_CLIST_EXIT);
    UtAssert_UINT32_EQ(args.counter, 1); /* no increment */
    UtAssert_BOOL_TRUE(args.error);
}

/*******************************************************************************
**
**  CF_WriteHistoryEntryToFile tests
**
*******************************************************************************/

void Test_CF_WriteHistoryEntryToFile(void)
{
    /* Test case for:
     * int CF_WriteHistoryEntryToFile(osal_id_t fd, const CF_History_t *h)
     */
    osal_id_t    arg_fd = OS_ObjectIdFromInteger(1);
    CF_History_t h;

    memset(&h, 0, sizeof(h));
    strcpy(h.fnames.src_filename, "sf");
    strcpy(h.fnames.dst_filename, "df");

    /* Successful write - need to set up for 3 successful calls to OS_write() */
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, 44);
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, strlen(h.fnames.src_filename) + 6);
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, strlen(h.fnames.dst_filename) + 6);
    UtAssert_INT32_EQ(CF_WriteHistoryEntryToFile(arg_fd, &h), 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* Unsuccessful write */
    UT_CF_ResetEventCapture();
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, -1);
    UtAssert_INT32_EQ(CF_WriteHistoryEntryToFile(arg_fd, &h), -1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WHIST_WRITE);
}

/*******************************************************************************
**
**  CF_WriteQueueDataToFile tests
**
*******************************************************************************/

void Test_CF_WriteTxnQueueDataToFile(void)
{
    /* Arrange */
    osal_id_t      arg_fd = OS_ObjectIdFromInteger(1);
    CF_Channel_t   ch;
    CF_CListNode_t node;

    memset(&node, 0, sizeof(node));
    memset(&ch, 0, sizeof(ch));
    ch.qs[CF_QueueIdx_TXA] = &node;

    /* Act */
    /* with no configuration, this should return no error (0) */
    UtAssert_INT32_EQ(CF_WriteTxnQueueDataToFile(arg_fd, &ch, CF_QueueIdx_TXA), 0);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
}

/*******************************************************************************
**
**  CF_WriteHistoryQueueDataToFile tests
**
*******************************************************************************/

void Test_CF_WriteHistoryQueueDataToFile(void)
{
    /* Arrange */
    osal_id_t      arg_fd = OS_ObjectIdFromInteger(1);
    CF_Channel_t   ch;
    CF_CListNode_t node;

    memset(&node, 0, sizeof(node));
    memset(&ch, 0, sizeof(ch));
    ch.qs[CF_QueueIdx_HIST] = &node;

    /* Act */
    /* with no configuration, this should return no error (0) */
    UtAssert_INT32_EQ(CF_WriteHistoryQueueDataToFile(arg_fd, &ch, CF_QueueIdx_HIST), 0);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
}

/*******************************************************************************
**
**  CF_PrioSearch tests
**
*******************************************************************************/

void Test_CF_PrioSearch_When_t_PrioIsGreaterThanContextPrioReturn_CLIST_CONT(void)
{
    /* Arrange */
    CF_Transaction_t          t;
    CF_CListNode_t *          arg_node = &t.cl_node;
    CF_Traverse_PriorityArg_t p;
    void *                    arg_context = (void *)&p;
    int32                     result;

    t.priority = Any_uint8_Except(0);
    p.priority = Any_uint8_LessThan(t.priority);

    /* Act */
    result = CF_PrioSearch(arg_node, arg_context);

    /* Assert */
    UtAssert_INT32_EQ(result, CF_CLIST_CONT);
}

void Test_CF_PrioSearch_When_t_PrioIsEqToContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT(void)
{
    /* Arrange */
    CF_Transaction_t          t;
    CF_CListNode_t *          arg_node = &t.cl_node;
    CF_Traverse_PriorityArg_t p;
    void *                    arg_context = (void *)&p;
    int32                     result;

    memset(&p, 0, sizeof(p));

    /* NOTE: these are inverted from previous test! */
    t.priority = Any_uint8_Except(0);
    p.priority = t.priority;

    /* Act */
    result = CF_PrioSearch(arg_node, arg_context);

    /* Assert */
    UtAssert_INT32_EQ(result, CF_CLIST_EXIT);
    UtAssert_ADDRESS_EQ(p.t, &t);
}

void Test_CF_PrioSearch_When_t_PrioIsLessThanContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT(void)
{
    /* Arrange */
    CF_Transaction_t          t;
    CF_CListNode_t *          arg_node = &t.cl_node;
    CF_Traverse_PriorityArg_t p;
    void *                    arg_context = (void *)&p;
    int32                     result;

    memset(&p, 0, sizeof(p));

    /* NOTE: these are inverted from previous test! */
    p.priority = Any_uint8_Except(0);
    t.priority = Any_uint8_LessThan(p.priority);

    /* Act */
    result = CF_PrioSearch(arg_node, arg_context);

    /* Assert */
    UtAssert_INT32_EQ(result, CF_CLIST_EXIT);
    UtAssert_ADDRESS_EQ(p.t, &t);
}

/*******************************************************************************
**
**  CF_InsertSortPrio tests
**
*******************************************************************************/

void Test_CF_InsertSortPrio_Call_CF_CList_InsertBack_Ex_ListIsEmpty_AndSet_q_index_To_q(void)
{
    /* Arrange */
    CF_Transaction_t  t;
    CF_Transaction_t *arg_t = &t;
    CF_QueueIdx_t     arg_q = Any_cf_queue_index_t();
    CF_Channel_t *    c;
    CF_CListNode_t ** expected_insert_back_head;
    CF_CListNode_t *  expected_insert_back_node;

    CF_CList_InsertBack_context_t context_clist_insert_back;

    /* t settings to bypass CF_Assert */
    t.chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    t.state    = Any_uint8_Except(CF_TxnState_IDLE);

    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_clist_insert_back, sizeof(context_clist_insert_back), false);

    /* setting (&CF_AppData.engine.channels[arg_t->chan_num])->qs[arg_q] to NULL
     * makes the list empty */
    c            = &CF_AppData.engine.channels[arg_t->chan_num];
    c->qs[arg_q] = NULL;

    expected_insert_back_head = &c->qs[arg_q];
    expected_insert_back_node = &arg_t->cl_node;

    /* Act */
    CF_InsertSortPrio(arg_t, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.head, expected_insert_back_head);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.node, expected_insert_back_node);
    UtAssert_True(arg_t->flags.com.q_index == arg_q,
                  "arg_t->flags.com.q_index set to %d and should be %d (CF_QueueIdx_t q)", arg_t->flags.com.q_index,
                  arg_q);
}

void Test_CF_InsertSortPrio_Call_CF_CList_InsertAfter_Ex_AndSet_q_index_To_q(void)
{
    /* Arrange */
    CF_Transaction_t  p_t;
    CF_Transaction_t  t;
    CF_Transaction_t *arg_t = &t;
    CF_QueueIdx_t     arg_q = Any_cf_queue_index_t();
    CF_CListNode_t *  qs;
    CF_Channel_t *    c;
    CF_CListNode_t *  expected_end;
    CF_CListFn_t      expected_fn;
    CF_CListNode_t ** expected_insert_after_head;
    CF_CListNode_t ** expected_insert_after_start;
    CF_CListNode_t ** expected_insert_after_after;

    CF_CList_Traverse_R_context_t  context_cf_clist_traverse_r;
    CF_CList_InsertAfter_context_t context_CF_CList_InsertAfter;

    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse_R), UT_AltHandler_CF_CList_Traverse_R_PRIO,
                          &context_cf_clist_traverse_r);

    /* t settings to bypass CF_Assert */
    t.chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    t.state    = Any_uint8_Except(CF_TxnState_IDLE);

    /* setting (&CF_AppData.engine.channels[arg_t->chan_num])->qs[arg_q] to
     * &qs makes the list NOT empty */
    c            = &CF_AppData.engine.channels[arg_t->chan_num];
    c->qs[arg_q] = (CF_CListNode_t *)&qs;

    /* setup CF_Traverse_PriorityArg_t altered value */
    context_cf_clist_traverse_r.context_t = &p_t;

    /* Arrange for CF_CList_InsertAfter_Ex */
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertAfter), &context_CF_CList_InsertAfter, sizeof(context_CF_CList_InsertAfter),
                     false);

    /* set expected values */
    expected_end                = c->qs[arg_q];
    expected_fn                 = CF_PrioSearch;
    expected_insert_after_head  = (CF_CListNode_t **)&c->qs[arg_q];
    expected_insert_after_start = (CF_CListNode_t **)&p_t.cl_node;
    expected_insert_after_after = (CF_CListNode_t **)&arg_t->cl_node;

    /* Act */
    CF_InsertSortPrio(arg_t, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse_R, 1);
    UtAssert_ADDRESS_EQ(context_cf_clist_traverse_r.end, expected_end);
    UtAssert_True(context_cf_clist_traverse_r.fn == expected_fn, "context_cf_clist_traverse_r.fn ==  expected_fn");
    UtAssert_STUB_COUNT(CF_CList_InsertAfter, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.head, (CF_CListNode_t **)expected_insert_after_head);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.start, (CF_CListNode_t *)expected_insert_after_start);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.after, (CF_CListNode_t *)expected_insert_after_after);
    UtAssert_True(arg_t->flags.com.q_index == arg_q, "t->flags.com.q_index is %u and should be %u (q)",
                  arg_t->flags.com.q_index, arg_q);
}

void Test_CF_InsertSortPrio_When_p_t_Is_NULL_Call_CF_CList_InsertBack_Ex(void)
{
    /* Arrange */
    CF_Transaction_t  t;
    CF_Transaction_t *arg_t = &t;
    CF_QueueIdx_t     arg_q = Any_cf_queue_index_t();
    CF_CListNode_t *  qs;
    CF_Channel_t *    c;
    CF_CListNode_t *  expected_end;
    CF_CListFn_t      expected_fn;
    CF_CListNode_t ** expected_insert_back_head;
    CF_CListNode_t *  expected_insert_back_node;

    CF_CList_Traverse_R_context_t context_cf_clist_traverse_r;
    CF_CList_InsertBack_context_t context_clist_insert_back;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse_R), &context_cf_clist_traverse_r, sizeof(context_cf_clist_traverse_r),
                     false);

    /* t settings to bypass CF_Assert */
    t.chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    t.state    = Any_uint8_Except(CF_TxnState_IDLE);

    /* setting (&CF_AppData.engine.channels[arg_t->chan_num])->qs[arg_q] to
     * &qs makes the list NOT empty */
    c            = &CF_AppData.engine.channels[arg_t->chan_num];
    c->qs[arg_q] = (CF_CListNode_t *)&qs;

    /* setup CF_Traverse_PriorityArg_t altered value */
    context_cf_clist_traverse_r.context_t = NULL;

    /* set expected values */
    expected_end              = c->qs[arg_q];
    expected_fn               = CF_PrioSearch;
    expected_insert_back_head = &c->qs[arg_q];
    expected_insert_back_node = &arg_t->cl_node;

    /* Arrange for CF_CList_InsertBack_Ex */
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_clist_insert_back, sizeof(context_clist_insert_back), false);

    /* Act */
    CF_InsertSortPrio(arg_t, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse_R, 1);
    UtAssert_ADDRESS_EQ(context_cf_clist_traverse_r.end, expected_end);
    UtAssert_True(context_cf_clist_traverse_r.fn == expected_fn, "context_cf_clist_traverse_r.fn ==  expected_fn");
    UtAssert_STUB_COUNT(CF_CList_InsertAfter, 0);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.head, expected_insert_back_head);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.node, expected_insert_back_node);
    UtAssert_True(arg_t->flags.com.q_index == arg_q, "t->flags.com.q_index is %u and should be %u (q)",
                  arg_t->flags.com.q_index, arg_q);
}

/*******************************************************************************
**
**  CF_TraverseAllTransactions_Impl tests
**
*******************************************************************************/

void Test_CF_TraverseAllTransactions_Impl_GetContainer_t_Call_args_fn_AndAdd_1_ToCounter(void)
{
    /* Arrange */
    CF_Transaction_t      t;
    CF_CListNode_t *      arg_n = &t.cl_node;
    CF_TraverseAll_Arg_t  args;
    CF_TraverseAll_Arg_t *arg_args;
    int                   context_val;
    void *                context              = &context_val;
    int                   initial_args_counter = Any_int();
    CF_Transaction_t *    expected_t;
    void *                expected_context;
    int32                 result;

    UT_Callback_CF_TraverseAllTransactions_context_t func_ptr_context;

    args.fn      = UT_Callback_CF_TraverseAllTransactions;
    args.context = context;
    args.counter = initial_args_counter;

    arg_args = &args;

    /* set expected values */
    expected_t       = &t;
    expected_context = context;

    UT_SetDataBuffer(UT_KEY(UT_Callback_CF_TraverseAllTransactions), &func_ptr_context, sizeof(func_ptr_context),
                     false);

    /* Act */
    result = CF_TraverseAllTransactions_Impl(arg_n, arg_args);

    /* Assert */
    UtAssert_ADDRESS_EQ(func_ptr_context.t, expected_t);
    UtAssert_ADDRESS_EQ(func_ptr_context.context, expected_context);
    UtAssert_True(arg_args->counter == initial_args_counter + 1,
                  "CF_TraverseAllTransactions_Impl set args->counter to %d which is 1 more than initial value %d",
                  arg_args->counter, initial_args_counter);
    UtAssert_INT32_EQ(result, CF_CLIST_CONT);
}

/*******************************************************************************
**
**  CF_TraverseAllTransactions tests
**
*******************************************************************************/

void Test_CF_TraverseAllTransactions_CallOtherFunction_CF_Q_RX_TimesAndReturn_args_counter(void)
{
    /* Arrange */
    CF_Channel_t    c;
    CF_Channel_t *  arg_c;
    int             context;
    void *          arg_context    = &context;
    uint8           expected_count = CF_QueueIdx_RX - CF_QueueIdx_PEND + 1;
    CF_CListNode_t *expected_qs_nodes[expected_count];
    int             i = 0;

    CF_TraverseAllTransactions_fn_t                 arg_fn = UT_Callback_CF_TraverseAllTransactions;
    CF_CList_Traverse_TRAVERSE_ALL_ARGS_T_context_t contexts_cf_clist_traverse[expected_count];

    for (i = 0; i < expected_count; ++i)
    {
        c.qs[i] = (CF_CListNode_t *)&expected_qs_nodes[i];
    }

    /* set context */
    /* this must use data buffer hack to pass multiple contexts */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_TRAVERSE_ALL_ARGS_T, NULL);
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), contexts_cf_clist_traverse, sizeof(contexts_cf_clist_traverse), false);

    /* finalize arguments */
    arg_c = &c;

    /* Act */
    UtAssert_INT32_EQ(CF_TraverseAllTransactions(arg_c, arg_fn, arg_context), expected_count);

    /* Assert */
    for (i = 0; i < expected_count; ++i)
    {
        UtAssert_ADDRESS_EQ(contexts_cf_clist_traverse[i].start, (CF_CListNode_t *)&expected_qs_nodes[i]);
        UtAssert_True(contexts_cf_clist_traverse[i].fn == (CF_CListFn_t)CF_TraverseAllTransactions_Impl,
                      "contexts_cf_clist_traverse[i].fn ==  (CF_CListFn_t ) CF_TraverseAllTransactions_Impl");
        UtAssert_True(contexts_cf_clist_traverse[i].context_fn == UT_Callback_CF_TraverseAllTransactions,
                      "contexts_cf_clist_traverse[i].context_fn ==  UT_Callback_CF_TraverseAllTransactions");
        UtAssert_ADDRESS_EQ(contexts_cf_clist_traverse[i].context_context, arg_context);
        /* NOTE: checking the context count really only verifies
        ** CF_CList_Traverse stub works properly -- this could be removed,
        ** should it? Although it is the only thing showing diff in the contexts */
        UtAssert_True(contexts_cf_clist_traverse[i].context_counter == i + 1,
                      "CF_CList_Traverse context_counter[%u] is %d and should be %d (+1 from previous)", i,
                      contexts_cf_clist_traverse[i].context_counter, i + 1);
    }
}

/*******************************************************************************
**
**  CF_TraverseAllTransactions_All_Channels tests
**
*******************************************************************************/

void Test_CF_TraverseAllTransactions_All_Channels_ReturnTotalTraversals(void)
{
    /* Arrange */
    int   context;
    void *arg_context       = &context;
    uint8 per_channel_count = CF_QueueIdx_RX - CF_QueueIdx_PEND + 1;
    int   expected_result   = per_channel_count * CF_NUM_CHANNELS;

    CF_TraverseAllTransactions_fn_t arg_fn = NULL;
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_TRAVERSE_ALL_ARGS_T, NULL);

    /* Act */
    UtAssert_INT32_EQ(CF_TraverseAllTransactions_All_Channels(arg_fn, arg_context), expected_result);
}

/*******************************************************************************
**
**  CF_WrappedOpen tests
**
*******************************************************************************/

void Test_CF_WrappedOpen_Call_OS_OpenCreate_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    osal_id_t  fd;
    osal_id_t *arg_fd = &fd;
    char       fname;
    char *     arg_fname                   = &fname;
    int32      arg_flags                   = Any_uint32();
    int32      arg_access                  = Any_uint32();
    int32      forced_return_OS_OpenCreate = Any_int32();

    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), forced_return_OS_OpenCreate);

    /* Act */
    UtAssert_INT32_EQ(CF_WrappedOpenCreate(arg_fd, arg_fname, arg_flags, arg_access), forced_return_OS_OpenCreate);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_OpenCreate, 1);
}

/*******************************************************************************
**
**  CF_WrappedClose tests
**
*******************************************************************************/

void Test_CF_WrappedClose_DoNotReceive_OS_SUCCESS_From_OS_close_EventSent(void)
{
    /* Arrange */
    UT_SetDefaultReturnValue(UT_KEY(OS_close), Any_int32_Except(OS_SUCCESS));

    /* Act */
    UtAssert_VOIDCALL(CF_WrappedClose(UT_CF_OS_OBJID));

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_close, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void Test_CF_WrappedClose_Receive_OS_SUCCESS_From_OS_close_NoEventSent(void)
{
    /* Arrange */
    UT_SetHandlerFunction(UT_KEY(OS_close), local_handler_OS_close, NULL);
    UT_SetDefaultReturnValue(UT_KEY(OS_close), OS_SUCCESS);

    /* Act */
    UtAssert_VOIDCALL(CF_WrappedClose(UT_CF_OS_OBJID));

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_close, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

/*******************************************************************************
**
**  CF_WrappedRead tests
**
*******************************************************************************/

void Test_CF_WrappedRead_CallsOS_read_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    uint32 arg_read_size = Any_uint32_LessThan_or_EqualTo(10); /* 10 is arbitrary to make test fast */
    uint8  buf[10]       = {0};                                /* 10 to match max read size of 10 (arbitrary) */
    void * arg_buf       = &buf;

    UT_SetDefaultReturnValue(UT_KEY(OS_read), arg_read_size);

    /* Act */
    UtAssert_INT32_EQ(CF_WrappedRead(UT_CF_OS_OBJID, arg_buf, arg_read_size), arg_read_size);
}

/*******************************************************************************
**
**  CF_WrappedWrite tests
**
*******************************************************************************/

void Test_CF_WrappedWrite_Call_OS_write_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    uint8  buf;
    void * arg_buf         = &buf;
    uint32 test_write_size = Any_uint32();
    int32  expected_result = Any_int32();

    UT_SetDefaultReturnValue(UT_KEY(OS_write), expected_result);

    /* Act */
    UtAssert_INT32_EQ(CF_WrappedWrite(UT_CF_OS_OBJID, arg_buf, test_write_size), expected_result);
}

/*******************************************************************************
**
**  CF_WrappedLseek tests
**
*******************************************************************************/

void Test_CF_WrappedLseek_Call_OS_lseek_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    uint32 test_offset     = Any_uint32();
    int    test_mode       = Any_int();
    int32  expected_result = Any_int32();

    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), expected_result);

    /* Act */
    UtAssert_INT32_EQ(CF_WrappedLseek(UT_CF_OS_OBJID, test_offset, test_mode), expected_result);
}

void Test_CF_TxnStatus_IsError(void)
{
    /* Test function for:
     * bool CF_TxnStatus_IsError(CF_TxnStatus_t txn_stat)
     */
    UtAssert_BOOL_FALSE(CF_TxnStatus_IsError(CF_TxnStatus_UNDEFINED));
    UtAssert_BOOL_FALSE(CF_TxnStatus_IsError(CF_TxnStatus_NO_ERROR));
    UtAssert_BOOL_TRUE(CF_TxnStatus_IsError(CF_TxnStatus_INACTIVITY_DETECTED));
    UtAssert_BOOL_TRUE(CF_TxnStatus_IsError(CF_TxnStatus_MAX));
}

void Test_CF_TxnStatus_To_ConditionCode(void)
{
    /* Test function for:
     * CF_CFDP_ConditionCode_t CF_TxnStatus_To_ConditionCode(CF_TxnStatus_t txn_stat)
     */
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_UNDEFINED), CF_CFDP_ConditionCode_NO_ERROR);

    /* for the 4-bit condition codes these should be numerically equivalent to status codes */
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_NO_ERROR), CF_CFDP_ConditionCode_NO_ERROR);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_POS_ACK_LIMIT_REACHED),
                      CF_CFDP_ConditionCode_POS_ACK_LIMIT_REACHED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_KEEP_ALIVE_LIMIT_REACHED),
                      CF_CFDP_ConditionCode_KEEP_ALIVE_LIMIT_REACHED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_INVALID_TRANSMISSION_MODE),
                      CF_CFDP_ConditionCode_INVALID_TRANSMISSION_MODE);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_FILESTORE_REJECTION),
                      CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_FILE_CHECKSUM_FAILURE),
                      CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_FILE_SIZE_ERROR),
                      CF_CFDP_ConditionCode_FILE_SIZE_ERROR);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_NAK_LIMIT_REACHED),
                      CF_CFDP_ConditionCode_NAK_LIMIT_REACHED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_INACTIVITY_DETECTED),
                      CF_CFDP_ConditionCode_INACTIVITY_DETECTED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_INVALID_FILE_STRUCTURE),
                      CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_CHECK_LIMIT_REACHED),
                      CF_CFDP_ConditionCode_CHECK_LIMIT_REACHED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_UNSUPPORTED_CHECKSUM_TYPE),
                      CF_CFDP_ConditionCode_UNSUPPORTED_CHECKSUM_TYPE);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_SUSPEND_REQUEST_RECEIVED),
                      CF_CFDP_ConditionCode_SUSPEND_REQUEST_RECEIVED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_CANCEL_REQUEST_RECEIVED),
                      CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED);

    /* Other extended translations */
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_ACK_LIMIT_NO_FIN),
                      CF_CFDP_ConditionCode_INACTIVITY_DETECTED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_ACK_LIMIT_NO_EOF),
                      CF_CFDP_ConditionCode_INACTIVITY_DETECTED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_PROTOCOL_ERROR),
                      CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED);
    UtAssert_INT32_EQ(CF_TxnStatus_To_ConditionCode(CF_TxnStatus_MAX), CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED);
}

void Test_CF_TxnStatus_From_ConditionCode(void)
{
    /* Test function for:
     * CF_TxnStatus_t CF_TxnStatus_From_ConditionCode(CF_CFDP_ConditionCode_t cc)
     */
    int32 i;

    /* for the 4-bit condition codes these should be numerically equivalent to status codes */
    for (i = 0; i <= 15; ++i)
    {
        UtAssert_INT32_EQ(CF_TxnStatus_From_ConditionCode(i), i);
    }
}

/*******************************************************************************
**
**  cf_utils_tests UtTest_Add groups
**
*******************************************************************************/

void add_cf_utils_h_tests(void)
{
    UtTest_Add(Test_CF_ResetHistory, cf_utils_tests_Setup, cf_utils_tests_Teardown, "CF_ResetHistory");
    UtTest_Add(Test_CF_FindUnusedTransaction, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "CF_FindUnusedTransaction");
    UtTest_Add(Test_CF_FreeTransaction, cf_utils_tests_Setup, cf_utils_tests_Teardown, "CF_FreeTransaction");
    UtTest_Add(Test_CF_FindTransactionBySequenceNumber_Impl, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "CF_FindTransactionBySequenceNumber_Impl");
    UtTest_Add(Test_CF_FindTransactionBySequenceNumber, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "CF_FindTransactionBySequenceNumber");

    /* CF_DequeueTransaction tests */
    UtTest_Add(Test_cf_dequeue_transaction_Call_CF_CList_Remove_AndDecrement_q_size, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_cf_dequeue_transaction_Call_CF_CList_Remove_AndDecrement_q_size");
    /* end CF_DequeueTransaction tests */

    /* CF_MoveTransaction tests */
    UtTest_Add(Test_cf_move_transaction_Call_CF_CList_InsertBack_AndSet_q_index_ToGiven_q, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_cf_move_transaction_Call_CF_CList_InsertBack_AndSet_q_index_ToGiven_q");
    /* end CF_MoveTransaction tests */

    /* CF_CList_Remove_Ex tests */
    UtTest_Add(Test_CF_CList_Remove_Ex_Call_CF_CList_Remove_AndDecrement_q_size, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_CList_Remove_Ex_Call_CF_CList_Remove_AndDecrement_q_size");
    /* end CF_CList_Remove_Ex tests */

    /* CF_CList_InsertAfter_Ex tests */
    UtTest_Add(Test_CF_CList_InsertAfter_Ex_Call_CF_CList_InsertAfter_AndIncrement_q_size, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_CList_InsertAfter_Ex_Call_CF_CList_InsertAfter_AndIncrement_q_size");
    /* end CF_CList_InsertAfter_Ex tests */

    /* CF_CList_InsertBack_Ex tests */
    UtTest_Add(Test_CF_CList_InsertBack_Ex_Call_CF_CList_InsertBack_AndIncrement_q_size, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_CList_InsertBack_Ex_Call_CF_CList_InsertBack_AndIncrement_q_size");
    /* end CF_CList_InsertBack_Ex tests */

    UtTest_Add(Test_CF_TxnStatus_IsError, cf_utils_tests_Setup, cf_utils_tests_Teardown, "CF_TxnStatus_IsError");
    UtTest_Add(Test_CF_TxnStatus_To_ConditionCode, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "CF_TxnStatus_To_ConditionCode");
    UtTest_Add(Test_CF_TxnStatus_From_ConditionCode, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "CF_TxnStatus_From_ConditionCode");
}

void add_CF_Traverse_WriteHistoryToFile_tests(void)
{
    UtTest_Add(Test_CF_Traverse_WriteHistoryQueueEntryToFile, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "CF_Traverse_WriteHistoryQueueEntryToFile");
}

void add_CF_Traverse_WriteAllTxnToFile_tests(void)
{
    UtTest_Add(Test_CF_Traverse_WriteTxnQueueEntryToFile, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "CF_Traverse_WriteTxnQueueEntryToFile");
}

void add_CF_WriteTxnQueueDataToFile_tests(void)
{
    UtTest_Add(Test_CF_WriteTxnQueueDataToFile, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_CF_WriteTxnQueueDataToFile");
}

void add_CF_WriteHistoryQueueDataToFile_tests(void)
{
    UtTest_Add(Test_CF_WriteHistoryQueueDataToFile, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_CF_WriteHistoryQueueDataToFile");
}

void add_CF_PrioSearch_tests(void)
{
    UtTest_Add(Test_CF_PrioSearch_When_t_PrioIsGreaterThanContextPrioReturn_CLIST_CONT, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_PrioSearch_When_t_PrioIsGreaterThanContextPrioReturn_CLIST_CONT");
    UtTest_Add(Test_CF_PrioSearch_When_t_PrioIsEqToContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT,
               cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_CF_PrioSearch_When_t_PrioIsEqToContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT");
    UtTest_Add(Test_CF_PrioSearch_When_t_PrioIsLessThanContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT,
               cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_CF_PrioSearch_When_t_PrioIsLessThanContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT");
}

void add_CF_InsertSortPrio_tests(void)
{
    UtTest_Add(Test_CF_InsertSortPrio_Call_CF_CList_InsertBack_Ex_ListIsEmpty_AndSet_q_index_To_q, cf_utils_tests_Setup,
               cf_utils_tests_Teardown,
               "Test_CF_InsertSortPrio_Call_CF_CList_InsertBack_Ex_ListIsEmpty_AndSet_q_index_To_q");
    UtTest_Add(Test_CF_InsertSortPrio_Call_CF_CList_InsertAfter_Ex_AndSet_q_index_To_q, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_InsertSortPrio_Call_CF_CList_InsertAfter_Ex_AndSet_q_index_To_q");
    UtTest_Add(Test_CF_InsertSortPrio_When_p_t_Is_NULL_Call_CF_CList_InsertBack_Ex, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_InsertSortPrio_When_p_t_Is_NULL_Call_CF_CList_InsertBack_Ex");
}

void add_CF_TraverseAllTransactions_Impl_tests(void)
{
    UtTest_Add(Test_CF_TraverseAllTransactions_Impl_GetContainer_t_Call_args_fn_AndAdd_1_ToCounter,
               cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_CF_TraverseAllTransactions_Impl_GetContainer_t_Call_args_fn_AndAdd_1_ToCounter");
}

void add_CF_TraverseAllTransactions_tests(void)
{
    UtTest_Add(Test_CF_TraverseAllTransactions_CallOtherFunction_CF_Q_RX_TimesAndReturn_args_counter,
               cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_CF_TraverseAllTransactions_CallOtherFunction_CF_Q_RX_TimesAndReturn_args_counter");
}

void add_CF_TraverseAllTransactions_All_Channels_tests(void)
{
    UtTest_Add(Test_CF_TraverseAllTransactions_All_Channels_ReturnTotalTraversals, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_TraverseAllTransactions_All_Channels_ReturnTotalTraversals");
}

void add_CF_WrappedOpen_tests(void)
{
    UtTest_Add(Test_CF_WrappedOpen_Call_OS_OpenCreate_WithGivenArgumentsAndReturnItsReturnValue, cf_utils_tests_Setup,
               cf_utils_tests_Teardown,
               "Test_CF_WrappedOpen_Call_OS_OpenCreate_WithGivenArgumentsAndReturnItsReturnValue");
}

void add_CF_WrappedClose_tests(void)
{
    UtTest_Add(Test_CF_WrappedClose_DoNotReceive_OS_SUCCESS_From_OS_close_EventSent, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_WrappedClose_DoNotReceive_OS_SUCCESS_From_OS_close_EventSent");
    UtTest_Add(Test_CF_WrappedClose_Receive_OS_SUCCESS_From_OS_close_NoEventSent, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_WrappedClose_Receive_OS_SUCCESS_From_OS_close_NoEventSent");
}

void add_CF_WrappedRead_tests(void)
{
    UtTest_Add(Test_CF_WrappedRead_CallsOS_read_WithGivenArgumentsAndReturnItsReturnValue, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_WrappedRead_CallsOS_read_WithGivenArgumentsAndReturnItsReturnValue");
}

void add_CF_WrappedWrite_tests(void)
{
    UtTest_Add(Test_CF_WrappedWrite_Call_OS_write_WithGivenArgumentsAndReturnItsReturnValue, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_WrappedWrite_Call_OS_write_WithGivenArgumentsAndReturnItsReturnValue");
}

void add_CF_WrappedLseek_tests(void)
{
    UtTest_Add(Test_CF_WrappedLseek_Call_OS_lseek_WithGivenArgumentsAndReturnItsReturnValue, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_WrappedLseek_Call_OS_lseek_WithGivenArgumentsAndReturnItsReturnValue");
}

/*******************************************************************************
**
**  cf_utils_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_cf_utils_h_tests();

    add_CF_Traverse_WriteHistoryToFile_tests();

    add_CF_Traverse_WriteAllTxnToFile_tests();

    add_CF_WriteTxnQueueDataToFile_tests();

    add_CF_WriteHistoryQueueDataToFile_tests();

    add_CF_PrioSearch_tests();

    add_CF_InsertSortPrio_tests();

    add_CF_TraverseAllTransactions_Impl_tests();

    add_CF_TraverseAllTransactions_tests();

    add_CF_TraverseAllTransactions_All_Channels_tests();

    add_CF_WrappedOpen_tests();

    add_CF_WrappedClose_tests();

    add_CF_WrappedRead_tests();

    add_CF_WrappedWrite_tests();

    add_CF_WrappedLseek_tests();
}