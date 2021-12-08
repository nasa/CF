/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_utils.c"

/* cf_utils_tests globals */
extern type_of_context_CF_CList_Traverse_t type_of_context_CF_CList_Traverse;

/*******************************************************************************
**
**  cf_utils_tests Setup and Teardown
**
*******************************************************************************/

void cf_utils_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_utils_tests_Setup */

void cf_utils_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_utils_tests_Teardown */

/* end cf_utils_tests Setup and Teardown */

/*******************************************************************************
**
**  cf_utils_tests specific Any functions  NOTE:Some of these may be better as global
**
*******************************************************************************/

cf_queue_index_t Any_cf_queue_index_t(void)
{
    return (cf_queue_index_t)Any_uint16_LessThan(CF_Q_NUM);
}

direction_t Any_direction_t(void)
{
    return (direction_t)Any_uint8_LessThan(CF_DIR_NUM);
}

condition_code_t Any_condition_code_t(void)
{
    uint8 codes[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 14, 15};
    return (condition_code_t)Any_uint8_FromThese(codes, sizeof(codes) / sizeof(codes[0]));
}

void local_handler_OS_close(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
}

/*******************************************************************************
**
**  cf_utils_tests dummy test functions
**
*******************************************************************************/

void DummyFunctionFor_CF_TraverseAllTransactions_(transaction_t *t, void *context)
{
    UT_Stub_CopyFromLocal(UT_KEY(DummyFunctionFor_CF_TraverseAllTransactions_), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(DummyFunctionFor_CF_TraverseAllTransactions_), &context, sizeof(context));

    UT_DEFAULT_IMPL(DummyFunctionFor_CF_TraverseAllTransactions_);
}

/*******************************************************************************
**
**  cf_utils.h function tests
**
*******************************************************************************/

/* cf_dequeue_transaction tests */

void Test_cf_dequeue_transaction_AssertsBecause_t_IsNull(void)
{
    // /* Arrange */
    // transaction_t   *arg_t = NULL;

    // /* Act */
    // cf_dequeue_transaction(arg_t);

    // /* Assert */
    // UtAssert_STUB_COUNT(CF_HandleAssert, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t");
} /* end Test_cf_dequeue_transaction_AssertsBecause_t_IsNull */

void Test_cf_dequeue_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS(void)
{
    // /* Arrange */
    // transaction_t   arg_t;
    // clist_node      *expected_qs =
    //   &CF_AppData.engine.channels[arg_t.chan_num].qs[arg_t.flags.com.q_index];
    // uint8           dummy_chan_num =
    //   Any_uint8_GreaterThan_or_EqualTo(CF_NUM_CHANNELS);

    // arg_t.chan_num = dummy_chan_num;

    // /* Act */
    // cf_dequeue_transaction(&arg_t);

    // /* Assert */
    // UtAssert_STUB_COUNT(CF_HandleAssert, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->chan_num<CF_NUM_CHANNELS");
} /* end Test_cf_dequeue_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS */

void Test_cf_dequeue_transaction_AssertsBecause_q_size_Eq0(void)
{
    // /* Arrange */
    // transaction_t   *arg_t = NULL;

    // /* Act */
    // cf_dequeue_transaction(arg_t);

    // /* Assert */
    // UtAssert_STUB_COUNT(CF_HandleAssert, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index]");
} /* end Test_cf_dequeue_transaction_AssertsBecause_q_size_Eq0 */

void Test_cf_dequeue_transaction_Call_CF_CList_Remove_AndDecrement_q_size(void)
{
    /* Arrange */
    transaction_t arg_t;
    uint8         dummy_chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    clist_node   *expected_head;
    clist_node    expected_cl_node;
    uint16        initial_q_size = Any_uint16_Except(0); /* 0 will CF_Assert */

    CF_Clist_Remove_context_t context_clist_remove;
    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_clist_remove, sizeof(context_clist_remove), false);

    arg_t.chan_num   = dummy_chan_num;
    expected_head    = &CF_AppData.engine.channels[arg_t.chan_num].qs[arg_t.flags.com.q_index];
    expected_cl_node = &arg_t.cl_node;

    CF_AppData.hk.channel_hk[arg_t.chan_num].q_size[arg_t.flags.com.q_index] = initial_q_size;

    /* Act */
    cf_dequeue_transaction(&arg_t);

    uint16 updated_q_size = CF_AppData.hk.channel_hk[arg_t.chan_num].q_size[arg_t.flags.com.q_index];

    /* Assert */
    UtAssert_STUB_COUNT(CF_HandleAssert, 0);
    UtAssert_ADDRESS_EQ(context_clist_remove.head, expected_head);
    UtAssert_ADDRESS_EQ(context_clist_remove.node, expected_cl_node);
    UtAssert_True(updated_q_size == initial_q_size - 1, "q_size is %d and that is 1 less than initial value %d",
                  updated_q_size, initial_q_size);

} /* end Test_cf_dequeue_transaction_Call_CF_CList_Remove_AndDecrement_q_size */

/* end cf_dequeue_transaction tests */

/* cf_move_transaction tests */

void Test_cf_move_transaction_AssertsBecause_t_IsNull(void)
{
    // /* Arrange */

    // /* Act */

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t");
} /* end Test_cf_move_transaction_AssertsBecause_t_IsNull */

void Test_cf_move_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS(void)
{
    // /* Arrange */

    // /* Act */

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->chan_num<CF_NUM_CHANNELS");
} /* end Test_cf_move_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS */

void Test_cf_move_transaction_AssertsBecause_channel_hk_Has_q_size_Eq0(void)
{
    // /* Arrange */

    // /* Act */

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index]");
} /* end Test_cf_move_transaction_AssertsBecause_channel_hk_Has_q_size_Eq0 */

void Test_cf_move_transaction_Call_CF_CList_InsertBack_AndSet_q_index_ToGiven_q(void)
{
    /* Arrange */
    transaction_t    dummy_t;
    transaction_t   *arg_t          = &dummy_t;
    uint8            dummy_chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    clist_node      *expected_remove_head;
    clist_node       expected_remove_node;
    clist_node      *expected_insert_back_head;
    clist_node       expected_insert_back_node;
    cf_queue_index_t arg_q = Any_cf_queue_index_t();

    arg_t->chan_num = dummy_chan_num;

    CF_Clist_Remove_context_t context_clist_remove;
    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_clist_remove, sizeof(context_clist_remove), false);

    expected_remove_head = &CF_AppData.engine.channels[arg_t->chan_num].qs[arg_t->flags.com.q_index];
    expected_remove_node = &arg_t->cl_node;

    CF_CList_InsertBack_context_t context_clist_insert_back;
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_clist_insert_back, sizeof(context_clist_insert_back), false);

    expected_insert_back_head = &CF_AppData.engine.channels[arg_t->chan_num].qs[arg_q];
    expected_insert_back_node = &arg_t->cl_node;

    /* after here must have chan_num set */
    CF_AppData.hk.channel_hk[arg_t->chan_num].q_size[arg_t->flags.com.q_index] =
        Any_uint8_LessThanCeilingExcept(CF_Q_NUM + 1, 0);

    /* Act */
    cf_move_transaction(arg_t, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_HandleAssert, 0);
    UtAssert_STUB_COUNT(CF_CList_Remove, 1);
    UtAssert_ADDRESS_EQ(context_clist_remove.head, expected_remove_head);
    UtAssert_ADDRESS_EQ(context_clist_remove.node, expected_remove_node);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.head, expected_insert_back_head);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.node, expected_insert_back_node);
    UtAssert_True(arg_t->flags.com.q_index == arg_q,
                  "t->flags.com.q_index set to %u and should equal passed in q value %u", arg_t->flags.com.q_index,
                  arg_q);
} /* end Test_cf_move_transaction_Call_CF_CList_InsertBack_AndSet_q_index_ToGiven_q */

/* end cf_move_transaction tests */

/* CF_CList_Remove_Ex tests */

void Test_CF_CList_Remove_Ex_AssertsBecause_q_size_Eq0(void)
{
    // /* Arrange */
    // channel_t           dummy_c;
    // channel_t*          arg_c = &dummy_c;
    // cf_queue_index_t    arg_index = Any_cf_queue_index_t();
    // clist_node          arg_node;
    // clist_node*         expected_remove_head;
    // clist_node          expected_remove_node;

    // CF_Clist_Remove_context_t context_clist_remove;
    // UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_clist_remove,
    //   sizeof(context_clist_remove), false);
    //
    // expected_remove_head = &arg_c->qs[arg_index];

    // CF_AppData.hk.channel_hk[arg_c-CF_AppData.engine.channels].q_size[arg_index] = 0;

    // /* Act */
    // CF_CList_Remove_Ex(arg_c, arg_index, arg_node);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.hk.channel_hk[c-CF_AppData.engine.channels].q_size[index]");
} /* end Test_CF_CList_Remove_Ex_AssertsBecause_q_size_Eq0 */

void Test_CF_CList_Remove_Ex_Call_CF_CList_Remove_AndDecrement_q_size(void)
{
    /* Arrange */
    channel_t       *arg_c     = &CF_AppData.engine.channels[Any_uint32_LessThan(CF_NUM_CHANNELS)];
    cf_queue_index_t arg_index = Any_cf_queue_index_t();
    clist_node_t     dummy_node;
    clist_node       arg_node = &dummy_node;
    clist_node      *expected_remove_head;
    clist_node       expected_remove_node;
    uint16           initial_q_size = Any_uint16_Except(0);

    CF_Clist_Remove_context_t context_clist_remove;
    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_clist_remove, sizeof(context_clist_remove), false);

    expected_remove_head = &arg_c->qs[arg_index];
    expected_remove_node = arg_node;

    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index] = initial_q_size;

    /* Act */
    CF_CList_Remove_Ex(arg_c, arg_index, arg_node);

    uint16 updated_q_size = CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index];

    /* Assert */
    UtAssert_STUB_COUNT(CF_HandleAssert, 0);
    UtAssert_STUB_COUNT(CF_CList_Remove, 1);
    UtAssert_ADDRESS_EQ(context_clist_remove.head, expected_remove_head);
    UtAssert_ADDRESS_EQ(context_clist_remove.node, expected_remove_node);
    UtAssert_True(updated_q_size == initial_q_size - 1, "q_size is %d and that is 1 less than initial value %d",
                  updated_q_size, initial_q_size);
} /* end Test_CF_CList_Remove_Ex_Call_CF_CList_Remove_AndDecrement_q_size */

/* end CF_CList_Remove_Ex tests */

/* CF_CList_InsertAfter_Ex tests */

void Test_CF_CList_InsertAfter_Ex_Call_CF_CList_InsertAfter_AndIncrement_q_size(void)
{
    /* Arrange */
    channel_t       *arg_c     = &CF_AppData.engine.channels[Any_uint32_LessThan(CF_NUM_CHANNELS)];
    cf_queue_index_t arg_index = Any_cf_queue_index_t();
    clist_node_t     dummy_start;
    clist_node       arg_start = &dummy_start;
    clist_node_t     dummy_after;
    clist_node       arg_after                  = &dummy_after;
    uint16           initial_q_size             = Any_uint16();
    clist_node      *expected_insert_after_head = &arg_c->qs[arg_index];

    CF_CList_InsertAfter_context_t context_CF_CList_InsertAfter;
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertAfter), &context_CF_CList_InsertAfter, sizeof(context_CF_CList_InsertAfter),
                     false);

    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index] = initial_q_size;

    /* Act */
    CF_CList_InsertAfter_Ex(arg_c, arg_index, arg_start, arg_after);

    uint16 updated_q_size = CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index];

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_InsertAfter, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.head, expected_insert_after_head);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.start, arg_start);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.after, arg_after);
    UtAssert_True(updated_q_size == (uint16)(initial_q_size + 1),
                  "q_size is %d and that is 1 more than initial value %d", updated_q_size, initial_q_size);

} /* end Test_CF_CList_InsertAfter_Ex_Call_CF_CList_InsertAfter_AndIncrement_q_size */

/* end CF_CList_InsertAfter_Ex tests */

/* CF_CList_InsertBack_Ex tests */

void Test_CF_CList_InsertBack_Ex_Call_CF_CList_InsertBack_AndIncrement_q_size(void)
{
    /* Arrange */
    channel_t       *arg_c     = &CF_AppData.engine.channels[Any_uint32_LessThan(CF_NUM_CHANNELS)];
    cf_queue_index_t arg_index = Any_cf_queue_index_t();
    clist_node_t     dummy_node;
    clist_node       arg_node       = &dummy_node;
    uint16           initial_q_size = Any_uint16();
    clist_node      *expected_insert_back_head;
    clist_node       expected_insert_back_node;

    CF_CList_InsertBack_context_t context_clist_insert_back;
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_clist_insert_back, sizeof(context_clist_insert_back), false);

    expected_insert_back_head = &arg_c->qs[arg_index];
    expected_insert_back_node = arg_node;

    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index] = initial_q_size;

    /* Act */
    CF_CList_InsertBack_Ex(arg_c, arg_index, arg_node);

    uint16 updated_q_size = CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[arg_index];

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.head, expected_insert_back_head);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.node, expected_insert_back_node);
    UtAssert_True(updated_q_size == (uint16)(initial_q_size + 1),
                  "q_size is %d and that is 1 more than initial value %d", updated_q_size, initial_q_size);

} /* end Test_CF_CList_InsertBack_Ex_Call_CF_CList_InsertBack_AndIncrement_q_size */

/* end CF_CList_InsertBack_Ex tests */

/* end cf_utils.h function tests */

/*******************************************************************************
**
**  CF_TraverseHistory tests
**
*******************************************************************************/

void Test_CF_TraverseHistory_AssertsBecause_h_dir_GreaterThan_CF_DIR_NUM(void)
{
    // /* Arrange */
    // history_t       dummy_h;
    // clist_node      arg_n = &dummy_h.cl_node;
    // trav_arg_t      dummy_context;
    // trav_arg_t*     arg_context = &dummy_context;
    //
    // dummy_h.src_eid = Any_uint8();
    // dummy_h.seq_num = Any_uint32();
    // dummy_h.dir = Any_direction_t();
    // dummy_h.peer_eid = Any_uint8();
    // dummy_h.cc = Any_condition_code_t();

    // /* Act */
    // CF_TraverseHistory(arg_n, arg_context);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - h->dir<CF_DIR_NUM");

} /* end Test_CF_TraverseHistory_AssertsBecause_h_dir_GreaterThan_CF_DIR_NUM */

void Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsFirstCallReturn_CLIST_EXIT(void)
{
    /* Arrange */
    history_t   dummy_h;
    clist_node  arg_n = &dummy_h.cl_node;
    trav_arg_t  dummy_context;
    trav_arg_t *arg_context      = &dummy_context;
    char        src_colon_str[6] = "SRC: "; /* duplicates function value */
    uint8       dummy_len;
    int         local_result;

    dummy_h.src_eid  = Any_uint8();
    dummy_h.seq_num  = Any_uint32();
    dummy_h.dir      = Any_direction_t();
    dummy_h.peer_eid = Any_uint8();
    dummy_h.cc       = Any_condition_code_t();

    /* ensures dummy_context.result change to 1 was done */
    dummy_context.result = 0;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange for CF_WrappedWrite in same file as CF_TraverseHistory */
    dummy_len = strlen(dummy_h.fnames.src_filename) + strlen(src_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), FIRST_CALL, Any_int_Except(dummy_len));

    /* Act */
    local_result = CF_TraverseHistory(arg_n, arg_context);

    /* Assert */
    UtAssert_True(EventID == CF_EID_ERR_CMD_WHIST_WRITE,
                  "CFE_EVS_SendEvent event id is 0x%04X and should be 0x%04X (CF_EID_ERR_CMD_WHIST_WRITE)", EventID,
                  CF_EID_ERR_CMD_WHIST_WRITE);
    UtAssert_True(arg_context->result == 1, "CF_TraverseHistory set context.result to %d and should be 1",
                  arg_context->result);
    UtAssert_True(local_result == CLIST_EXIT, "CF_TraverseHistory returned 0x%08X and should be 0x%08X (CLIST_EXIT)",
                  local_result, CLIST_EXIT);

} /* end Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsFirstCallReturn_CLIST_EXIT */

void Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsSecondCallReturn_CLIST_EXIT(void)
{
    /* Arrange */
    history_t   dummy_h;
    clist_node  arg_n = &dummy_h.cl_node;
    trav_arg_t  dummy_context;
    trav_arg_t *arg_context      = &dummy_context;
    char        src_colon_str[6] = "SRC: "; /* duplicates function value */
    char        dst_colon_str[6] = "DST: "; /* duplicates function value */
    uint8       dummy_len_src;
    uint8       dummy_len_dst;
    int         local_result;

    dummy_h.src_eid  = Any_uint8();
    dummy_h.seq_num  = Any_uint32();
    dummy_h.dir      = Any_direction_t();
    dummy_h.peer_eid = Any_uint8();
    dummy_h.cc       = Any_condition_code_t();

    /* ensures dummy_context.result change to 1 was done */
    dummy_context.result = 0;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange for CF_WrappedWrite in same file as CF_TraverseHistory */
    dummy_len_src = strlen(dummy_h.fnames.src_filename) + strlen(src_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), FIRST_CALL, dummy_len_src);
    dummy_len_dst = strlen(dummy_h.fnames.dst_filename) + strlen(dst_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), NEXT_CALL, Any_int_Except(dummy_len_dst));

    /* Act */
    local_result = CF_TraverseHistory(arg_n, arg_context);

    /* Assert */
    UtAssert_True(EventID == CF_EID_ERR_CMD_WHIST_WRITE,
                  "CFE_EVS_SendEvent event id is 0x%04X and should be 0x%04X (CF_EID_ERR_CMD_WHIST_WRITE)", EventID,
                  CF_EID_ERR_CMD_WHIST_WRITE);
    UtAssert_True(arg_context->result == 1, "CF_TraverseHistory set context.result to %d and should be 1",
                  arg_context->result);
    UtAssert_True(local_result == CLIST_EXIT, "CF_TraverseHistory returned 0x%08X and should be 0x%08X (CLIST_EXIT)",
                  local_result, CLIST_EXIT);

} /* end Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsSecondCallReturn_CLIST_EXIT */

void Test_CF_TraverseHistory_WhenBothWrappedWritesSuccessfulReturn_CLIST_CONT(void)
{
    /* Arrange */
    history_t   dummy_h;
    clist_node  arg_n = &dummy_h.cl_node;
    trav_arg_t  dummy_context;
    trav_arg_t *arg_context      = &dummy_context;
    char        src_colon_str[6] = "SRC: "; /* duplicates function value */
    char        dst_colon_str[6] = "DST: "; /* duplicates function value */
    uint8       dummy_len_src;
    uint8       dummy_len_dst;
    int         local_result;

    dummy_h.src_eid  = Any_uint8();
    dummy_h.seq_num  = Any_uint32();
    dummy_h.dir      = Any_direction_t();
    dummy_h.peer_eid = Any_uint8();
    dummy_h.cc       = Any_condition_code_t();

    /* ensures dummy_context.result change to 1 was done */
    dummy_context.result = 0;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Arrange for CF_WrappedWrite in same file as CF_TraverseHistory */
    dummy_len_src = strlen(dummy_h.fnames.src_filename) + strlen(src_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), FIRST_CALL, dummy_len_src);
    dummy_len_dst = strlen(dummy_h.fnames.dst_filename) + strlen(dst_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), NEXT_CALL, dummy_len_dst);

    /* Act */
    local_result = CF_TraverseHistory(arg_n, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_context->result == 0, "CF_TraverseHistory context.result is %d and should be 0",
                  arg_context->result);
    UtAssert_True(local_result == CLIST_CONT, "CF_TraverseHistory returned 0x%08X and should be 0x%08X (CLIST_CONT)",
                  local_result, CLIST_CONT);

} /* end Test_CF_TraverseHistory_WhenBothWrappedWritesSuccessfulReturn_CLIST_CONT */

/* end CF_TraverseHistory tests */

/*******************************************************************************
**
**  CF_TraverseTransactions tests
**
*******************************************************************************/

void Test_CF_TraverseTransactions_When_context_result_Is_1_Return_CLIST_EXIT(void)
{
    /* Arrange */
    history_t     dummy_history;
    transaction_t dummy_t;
    clist_node    arg_n = &dummy_t.cl_node;
    trav_arg_t    dummy_context;
    trav_arg_t   *arg_context = &dummy_context;
    int           local_result;

    /* Arrange for CF_TraverseHistory in same file as CF_TraverseTransactions */

    dummy_t.history           = &dummy_history;
    dummy_t.history->src_eid  = Any_uint8();
    dummy_t.history->seq_num  = Any_uint32();
    dummy_t.history->dir      = Any_direction_t();
    dummy_t.history->peer_eid = Any_uint8();
    dummy_t.history->cc       = Any_condition_code_t();

    /* Arrange for CF_WrappedWrite in same file as CF_TraverseHistor in same
     * file as CF_TraverseTransactions */
    char  src_colon_str[6] = "SRC: "; /* duplicates function value */
    uint8 dummy_len;

    dummy_len = strlen(dummy_t.history->fnames.src_filename) + strlen(src_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), FIRST_CALL, Any_int_Except(dummy_len));

    /* Act */
    local_result = CF_TraverseTransactions(arg_n, arg_context);

    /* Assert */
    UtAssert_True(local_result == CLIST_EXIT, "CF_TraverseTransactions returned %d and should be %d (CLIST_EXIT)",
                  local_result, CLIST_EXIT);
} /* end Test_CF_TraverseTransactions_When_context_result_Is_1_Return_CLIST_EXIT */

void Test_CF_TraverseTransactions_When_context_result_Is_0_Return_CLIST_CONT(void)
{
    /* Arrange */
    history_t     dummy_history;
    transaction_t dummy_t;
    clist_node    arg_n = &dummy_t.cl_node;
    trav_arg_t    dummy_context;
    trav_arg_t   *arg_context = &dummy_context;
    int           local_result;

    arg_context->result = 0; /* ensures arg_context->result starts at 0 */

    /* Arrange for CF_TraverseHistor in same file as CF_TraverseTransactions */
    dummy_t.history           = &dummy_history;
    dummy_t.history->src_eid  = Any_uint8();
    dummy_t.history->seq_num  = Any_uint32();
    dummy_t.history->dir      = Any_direction_t();
    dummy_t.history->peer_eid = Any_uint8();
    dummy_t.history->cc       = Any_condition_code_t();

    /* Arrange for CF_WrappedWrite in same file as CF_TraverseHistor in same
     * file as CF_TraverseTransactions */
    char  src_colon_str[6] = "SRC: "; /* duplicates function value */
    char  dst_colon_str[6] = "DST: "; /* duplicates function value */
    uint8 dummy_len_src;
    uint8 dummy_len_dst;

    dummy_len_src = strlen(dummy_t.history->fnames.src_filename) + strlen(src_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), FIRST_CALL, dummy_len_src);
    dummy_len_dst = strlen(dummy_t.history->fnames.dst_filename) + strlen(dst_colon_str);
    UT_SetDeferredRetcode(UT_KEY(OS_write), NEXT_CALL, dummy_len_dst);

    /* Act */
    local_result = CF_TraverseTransactions(arg_n, arg_context);

    /* Assert */
    UtAssert_True(local_result == CLIST_CONT, "CF_TraverseTransactions returned %d and should be %d (CLIST_CONT)",
                  local_result, CLIST_CONT);
} /* end Test_CF_TraverseTransactions_When_context_result_Is_0_Return_CLIST_CONT */

/* end CF_TraverseTransactions tests */

/*******************************************************************************
**
**  CF_WriteQueueDataToFile tests
**
*******************************************************************************/

void Test_CF_WriteQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result(void)
{
    /* Arrange */
    int32            arg_fd = Any_int32();
    channel_t        dummy_c;
    channel_t       *arg_c = &dummy_c;
    cf_queue_index_t arg_q = Any_cf_queue_index_t();
    clist_node_t     dummy_node;
    clist_node       expected_start = &dummy_node;
    clist_fn_t       expected_fn    = (clist_fn_t)CF_TraverseTransactions;

    CF_CList_Traverse_TRAV_ARG_T_context_t context_clist_traverse;

    dummy_c.qs[arg_q] = expected_start;

    context_clist_traverse.context_result = Any_int32();
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_clist_traverse, sizeof(context_clist_traverse), false);
    type_of_context_CF_CList_Traverse = TRAV_ARG_T;

    /* Act */
    result = CF_WriteQueueDataToFile(arg_fd, arg_c, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_clist_traverse.start, expected_start);
    UtAssert_True(context_clist_traverse.fn == expected_fn, "context_clist_traverse.fn ==  expected_fn");
    UtAssert_True(context_clist_traverse.context_fd == arg_fd,
                  "CF_WriteQueueDataToFile received context fd %d and should be %d (fd)",
                  context_clist_traverse.context_fd, arg_fd);
    UtAssert_True(result == context_clist_traverse.context_result,
                  "CF_WriteQueueDataToFile returned %d and should be %d (CF_CList_Traverse set arg.result)", result,
                  context_clist_traverse.context_result);
    /* NOTE: context_clist_traverse.counter is not checked because it is not altered */
} /* end Test_CF_WriteQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result */

/* end CF_WriteQueueDataToFile tests */

/*******************************************************************************
**
**  CF_WriteHistoryQueueDataToFile tests
**
*******************************************************************************/

void Test_CF_WriteHistoryQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result(void)
{
    /* Arrange */
    int32        arg_fd = Any_int32();
    channel_t    dummy_c;
    channel_t   *arg_c   = &dummy_c;
    direction_t  arg_dir = Any_direction_t();
    clist_node_t dummy_node;
    clist_node   expected_start = &dummy_node;
    clist_fn_t   expected_fn    = (clist_fn_t)CF_TraverseHistory;

    CF_CList_Traverse_TRAV_ARG_T_context_t context_clist_traverse;

    dummy_c.qs[CF_Q_HIST] = expected_start;

    context_clist_traverse.context_result = Any_int32();
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_clist_traverse, sizeof(context_clist_traverse), false);
    type_of_context_CF_CList_Traverse = TRAV_ARG_T;

    /* Act */
    result = CF_WriteHistoryQueueDataToFile(arg_fd, arg_c, arg_dir);

    /* Assert */
    /* NOTE: cannot test functions local setup of arg because it is passed as a void*
    ** Maybe this can be overcome, but need to know what all can be sent to CF_CList_Traverse */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_clist_traverse.start, expected_start);
    UtAssert_True(context_clist_traverse.fn == expected_fn, "context_clist_traverse.fn ==  expected_fn");
    UtAssert_True(context_clist_traverse.context_fd == arg_fd,
                  "CF_WriteQueueDataToFile received context fd %d and should be %d (fd)",
                  context_clist_traverse.context_fd, arg_fd);
    UtAssert_True(result == context_clist_traverse.context_result,
                  "CF_WriteQueueDataToFile returned %d and should be %d (CF_CList_Traverse set arg.result)", result,
                  context_clist_traverse.context_result);
    /* NOTE: context_clist_traverse.counter is not checked because it is not altered */
} /* end Test_CF_WriteHistoryQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result */

/* end CF_WriteHistoryQueueDataToFile tests */

/*******************************************************************************
**
**  CF_PrioSearch tests
**
*******************************************************************************/

void Test_CF_PrioSearch_When_t_PrioIsGreaterThanContextPrioReturn_CLIST_CONT(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    clist_node     arg_node = &dummy_t.cl_node;
    priority_arg_t dummy_p;
    void          *arg_context = (void *)&dummy_p;

    dummy_t.priority = Any_uint8_Except(0);
    dummy_p.priority = Any_uint8_LessThan(dummy_t.priority);

    /* Act */
    result = CF_PrioSearch(arg_node, arg_context);

    /* Assert */
    UtAssert_True(result == CLIST_CONT, "CF_PrioSearch returned %d and should be %d (CLIST_CONT)", result, CLIST_CONT);

} /* end Test_CF_PrioSearch_When_t_PrioIsGreaterThanContextPrioReturn_CLIST_CONT */

void Test_CF_PrioSearch_When_t_PrioIsEqToContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    clist_node     arg_node = &dummy_t.cl_node;
    priority_arg_t dummy_p;
    void          *arg_context = (void *)&dummy_p;

    /* NOTE: these are inverted from previous test! */
    dummy_t.priority = Any_uint8_Except(0);
    dummy_p.priority = dummy_t.priority;

    /* Act */
    result = CF_PrioSearch(arg_node, arg_context);

    /* Assert */
    UtAssert_True(result == CLIST_EXIT, "CF_PrioSearch returned %d and should be %d (CLIST_EXIT)", result, CLIST_EXIT);
    UtAssert_ADDRESS_EQ(dummy_p.t, &dummy_t);

} /* end Test_CF_PrioSearch_When_t_PrioIsEqToContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT */

void Test_CF_PrioSearch_When_t_PrioIsLessThanContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    clist_node     arg_node = &dummy_t.cl_node;
    priority_arg_t dummy_p;
    void          *arg_context = (void *)&dummy_p;

    /* NOTE: these are inverted from previous test! */
    dummy_p.priority = Any_uint8_Except(0);
    dummy_t.priority = Any_uint8_LessThan(dummy_p.priority);

    /* Act */
    result = CF_PrioSearch(arg_node, arg_context);

    /* Assert */
    UtAssert_True(result == CLIST_EXIT, "CF_PrioSearch returned %d and should be %d (CLIST_EXIT)", result, CLIST_EXIT);
    UtAssert_ADDRESS_EQ(dummy_p.t, &dummy_t);

} /* end Test_CF_PrioSearch_When_t_PrioIsLessThanContextPrio_Set_context_t_To_t_AndReturn_CLIST_EXIT */

/* end CF_PrioSearch tests */

/*******************************************************************************
**
**  CF_InsertSortPrio tests
**
*******************************************************************************/

void Test_CF_InsertSortPrio_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS(void)
{
    // /* Arrange */

    // /* Act */

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->chan_num<CF_NUM_CHANNELS");
} /* end Test_CF_InsertSortPrio_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS */

void Test_CF_InsertSortPrio_AssertsBecause_t_state_IsNot_CFDP_IDLE(void)
{
    // /* Arrange */

    // /* Act */

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state!=CFDP_IDLE");
} /* end Test_CF_InsertSortPrio_AssertsBecause_t_state_IsNot_CFDP_IDLE */

void Test_CF_InsertSortPrio_Call_CF_CList_InsertBack_Ex_ListIsEmpty_AndSet_q_index_To_q(void)
{
    /* Arrange */
    transaction_t    dummy_t;
    transaction_t   *arg_t = &dummy_t;
    cf_queue_index_t arg_q = Any_cf_queue_index_t();
    channel_t       *dummy_c;
    clist_node      *expected_insert_back_head;
    clist_node       expected_insert_back_node;

    /* dummy_t settings to bypass CF_Assert */
    dummy_t.chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    dummy_t.state    = Any_uint8_Except(CFDP_IDLE);

    CF_CList_InsertBack_context_t context_clist_insert_back;
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_clist_insert_back, sizeof(context_clist_insert_back), false);

    /* setting (&CF_AppData.engine.channels[arg_t->chan_num])->qs[arg_q] to NULL
     * makes the list empty */
    dummy_c            = &CF_AppData.engine.channels[arg_t->chan_num];
    dummy_c->qs[arg_q] = NULL;

    expected_insert_back_head = &dummy_c->qs[arg_q];
    expected_insert_back_node = &arg_t->cl_node;

    /* Act */
    CF_InsertSortPrio(arg_t, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.head, expected_insert_back_head);
    UtAssert_ADDRESS_EQ(context_clist_insert_back.node, expected_insert_back_node);
    UtAssert_True(arg_t->flags.com.q_index == arg_q,
                  "arg_t->flags.com.q_index set to %d and should be %d (cf_queue_index_t q)", arg_t->flags.com.q_index,
                  arg_q);
} /* end Test_CF_InsertSortPrio_Call_CF_CList_InsertBack_Ex_ListIsEmpty_AndSet_q_index_To_q */

void Test_CF_InsertSortPrio_Call_CF_CList_InsertAfter_Ex_AndSet_q_index_To_q(void)
{
    /* Arrange */
    transaction_t    dummy_p_t;
    transaction_t    dummy_t;
    transaction_t   *arg_t = &dummy_t;
    cf_queue_index_t arg_q = Any_cf_queue_index_t();
    clist_node       dummy_qs;
    channel_t       *dummy_c;
    clist_node       expected_end;
    clist_fn_t       expected_fn;
    clist_node      *expected_insert_after_head;
    clist_node      *expected_insert_after_start;
    clist_node      *expected_insert_after_after;

    CF_CList_Traverse_R_context_t context_cf_clist_traverse_r;
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse_R), &context_cf_clist_traverse_r, sizeof(context_cf_clist_traverse_r),
                     false);

    /* dummy_t settings to bypass CF_Assert */
    dummy_t.chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    dummy_t.state    = Any_uint8_Except(CFDP_IDLE);

    /* setting (&CF_AppData.engine.channels[arg_t->chan_num])->qs[arg_q] to
     * &dummy_qs makes the list NOT empty */
    dummy_c            = &CF_AppData.engine.channels[arg_t->chan_num];
    dummy_c->qs[arg_q] = (clist_node)&dummy_qs;

    /* setup priority_arg_t altered value */
    context_cf_clist_traverse_r.context_t = &dummy_p_t;

    /* Arrange for CF_CList_InsertAfter_Ex */
    CF_CList_InsertAfter_context_t context_CF_CList_InsertAfter;
    UT_SetDataBuffer(UT_KEY(CF_CList_InsertAfter), &context_CF_CList_InsertAfter, sizeof(context_CF_CList_InsertAfter),
                     false);

    /* set expected values */
    expected_end                = dummy_c->qs[arg_q];
    expected_fn                 = CF_PrioSearch;
    expected_insert_after_head  = (clist_node *)&dummy_c->qs[arg_q];
    expected_insert_after_start = (clist_node *)&dummy_p_t.cl_node;
    expected_insert_after_after = (clist_node *)&arg_t->cl_node;

    /* Act */
    CF_InsertSortPrio(arg_t, arg_q);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse_R, 1);
    UtAssert_ADDRESS_EQ(context_cf_clist_traverse_r.end, expected_end);
    UtAssert_True(context_cf_clist_traverse_r.fn == expected_fn, "context_cf_clist_traverse_r.fn ==  expected_fn");
    UtAssert_STUB_COUNT(CF_CList_InsertAfter, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.head, (clist_node *)expected_insert_after_head);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.start, (clist_node)expected_insert_after_start);
    UtAssert_ADDRESS_EQ(context_CF_CList_InsertAfter.after, (clist_node)expected_insert_after_after);
    UtAssert_True(arg_t->flags.com.q_index == arg_q, "t->flags.com.q_index is %u and should be %u (q)",
                  arg_t->flags.com.q_index, arg_q);

} /* end Test_CF_InsertSortPrio_Call_CF_CList_InsertAfter_Ex_AndSet_q_index_To_q */

void Test_CF_InsertSortPrio_When_p_t_Is_NULL_Call_CF_CList_InsertBack_Ex(void)
{
    /* Arrange */
    transaction_t    dummy_t;
    transaction_t   *arg_t = &dummy_t;
    cf_queue_index_t arg_q = Any_cf_queue_index_t();
    clist_node       dummy_qs;
    channel_t       *dummy_c;
    clist_node       expected_end;
    clist_fn_t       expected_fn;
    clist_node      *expected_insert_back_head;
    clist_node       expected_insert_back_node;

    CF_CList_Traverse_R_context_t context_cf_clist_traverse_r;
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse_R), &context_cf_clist_traverse_r, sizeof(context_cf_clist_traverse_r),
                     false);

    /* dummy_t settings to bypass CF_Assert */
    dummy_t.chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    dummy_t.state    = Any_uint8_Except(CFDP_IDLE);

    /* setting (&CF_AppData.engine.channels[arg_t->chan_num])->qs[arg_q] to
     * &dummy_qs makes the list NOT empty */
    dummy_c            = &CF_AppData.engine.channels[arg_t->chan_num];
    dummy_c->qs[arg_q] = (clist_node)&dummy_qs;

    /* setup priority_arg_t altered value */
    context_cf_clist_traverse_r.context_t = NULL;

    /* set expected values */
    expected_end              = dummy_c->qs[arg_q];
    expected_fn               = CF_PrioSearch;
    expected_insert_back_head = &dummy_c->qs[arg_q];
    expected_insert_back_node = &arg_t->cl_node;

    /* Arrange for CF_CList_InsertBack_Ex */
    CF_CList_InsertBack_context_t context_clist_insert_back;
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

} /* end Test_CF_InsertSortPrio_When_p_t_Is_NULL_Call_CF_CList_InsertBack_Ex */

/* end CF_InsertSortPrio tests */

/*******************************************************************************
**
**  CF_TraverseAllTransactions_ tests
**
*******************************************************************************/

void Test_CF_TraverseAllTransactions__GetContainer_t_Call_args_fn_AndAdd_1_ToCounter(void)
{
    /* Arrange */
    transaction_t        dummy_t;
    clist_node           arg_n = &dummy_t.cl_node;
    traverse_all_args_t  dummy_args;
    traverse_all_args_t *arg_args;
    int                  dummy_context_val;
    void                *dummy_context        = &dummy_context_val;
    int                  initial_args_counter = Any_int();
    transaction_t       *expected_t;
    void                *expected_context;

    dummy_args.fn      = DummyFunctionFor_CF_TraverseAllTransactions_;
    dummy_args.context = dummy_context;
    dummy_args.counter = initial_args_counter;

    arg_args = &dummy_args;

    /* set expected values */
    expected_t       = &dummy_t;
    expected_context = dummy_context;

    DummyFunctionFor_CF_TraverseAllTransactions__context_t func_ptr_context;
    UT_SetDataBuffer(UT_KEY(DummyFunctionFor_CF_TraverseAllTransactions_), &func_ptr_context, sizeof(func_ptr_context),
                     false);

    /* Act */
    result = CF_TraverseAllTransactions_(arg_n, arg_args);

    /* Assert */
    UtAssert_STUB_COUNT(DummyFunctionFor_CF_TraverseAllTransactions_, 1);
    UtAssert_ADDRESS_EQ(func_ptr_context.t, expected_t);
    UtAssert_ADDRESS_EQ(func_ptr_context.context, expected_context);
    UtAssert_True(arg_args->counter == initial_args_counter + 1,
                  "CF_TraverseAllTransactions_ set args->counter to %d which is 1 more than initial value %d",
                  arg_args->counter, initial_args_counter);
    UtAssert_True(result == CLIST_CONT, "CF_TraverseAllTransactions_ returned %d and should be %d (CLIST_CONT)", result,
                  CLIST_CONT);

} /* end Test_CF_TraverseAllTransactions__GetContainer_t_Call_args_fn_AndAdd_1_ToCounter */

/*******************************************************************************
**
**  CF_TraverseAllTransactions tests
**
*******************************************************************************/

void Test_CF_TraverseAllTransactions_CallOtherFunction_CF_Q_RX_TimesAndReturn_args_counter(void)
{
    /* Arrange */
    channel_t  dummy_c;
    channel_t *arg_c;
    int        dummy_context;
    void      *arg_context    = &dummy_context;
    uint8      expected_count = CF_Q_RX - CF_Q_PEND + 1;
    clist_node expected_qs_nodes[expected_count];

    CF_TraverseAllTransactions_fn_t arg_fn = DummyFunctionFor_CF_TraverseAllTransactions_;

    int i = 0;
    for (i = 0; i < expected_count; ++i)
    {
        dummy_c.qs[i] = (clist_node)&expected_qs_nodes[i];
    }

    /* set context */
    CF_CList_Traverse_TRAVERSE_ALL_ARGS_T_context_t contexts_cf_clist_traverse[expected_count];
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &contexts_cf_clist_traverse, sizeof(contexts_cf_clist_traverse), false);

    /* set correct context type for CF_CList_Traverse stub */
    type_of_context_CF_CList_Traverse = TRAVERSE_ALL_ARGS_T;

    /* finalize arguments */
    arg_c = &dummy_c;

    /* Act */
    result = CF_TraverseAllTransactions(arg_c, arg_fn, arg_context);

    /* Assert */
    for (i = 0; i < expected_count; ++i)
    {
        UtAssert_ADDRESS_EQ(contexts_cf_clist_traverse[i].start, (clist_node)&expected_qs_nodes[i]);
        UtAssert_True(contexts_cf_clist_traverse[i].fn == (clist_fn_t)CF_TraverseAllTransactions_,
                      "contexts_cf_clist_traverse[i].fn ==  (clist_fn_t) CF_TraverseAllTransactions_");
        UtAssert_True(contexts_cf_clist_traverse[i].context_fn == DummyFunctionFor_CF_TraverseAllTransactions_,
                      "contexts_cf_clist_traverse[i].context_fn ==  DummyFunctionFor_CF_TraverseAllTransactions_");
        UtAssert_ADDRESS_EQ(contexts_cf_clist_traverse[i].context_context, arg_context);
        /* NOTE: checking the context count really only verifies
        ** CF_CList_Traverse stub works properly -- this could be removed,
        ** should it? Although it is the only thing showing diff in the contexts */
        UtAssert_True(contexts_cf_clist_traverse[i].context_counter == i + 1,
                      "CF_CList_Traverse context_counter[%u] is %d and should be %d (+1 from previous)", i,
                      contexts_cf_clist_traverse[i].context_counter, i + 1);
    }
    UtAssert_True(result == expected_count,
                  "CF_TraverseAllTransactions returned %d and should be %d (CF_Q_RX - CF_Q_PEND + 1)", result,
                  expected_count);
} /* end Test_CF_TraverseAllTransactions_CallOtherFunction_CF_Q_RX_TimesAndReturn_args_counter */

/*******************************************************************************
**
**  CF_TraverseAllTransactions_All_Channels tests
**
*******************************************************************************/

void Test_CF_TraverseAllTransactions_All_Channels_ReturnTotalTraversals(void)
{
    /* Arrange */
    int   dummy_context;
    void *arg_context = &dummy_context;
    int   local_result;
    uint8 per_channel_count = CF_Q_RX - CF_Q_PEND + 1;
    int   expected_result   = per_channel_count * CF_NUM_CHANNELS;

    CF_TraverseAllTransactions_fn_t arg_fn = NULL;

    /* Arrange for CF_TraverseAllTransactions */
    /* set correct context type for CF_CList_Traverse stub */
    type_of_context_CF_CList_Traverse = TRAVERSE_ALL_ARGS_T;

    /* Act */
    local_result = CF_TraverseAllTransactions_All_Channels(arg_fn, arg_context);

    /* Assert */
    UtAssert_True(local_result == expected_result,
                  "CF_TraverseAllTransactions_All_Channels returned %d and should be %d (total transversals)",
                  local_result, expected_result);

} /* end Test_CF_TraverseAllTransactions_All_Channels_ReturnTotalTraversals */

/* end CF_TraverseAllTransactions_All_Channels tests */

/*******************************************************************************
**
**  CF_WrappedOpen tests
**
*******************************************************************************/

void Test_CF_WrappedOpen_Call_OS_OpenCreate_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    osal_id_t  dummy_fd;
    osal_id_t *arg_fd = &dummy_fd;
    char       dummy_fname;
    char      *arg_fname                   = &dummy_fname;
    int32      arg_flags                   = Any_uint32();
    int32      arg_access                  = Any_uint32();
    int32      forced_return_OS_OpenCreate = Any_int32();

    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), forced_return_OS_OpenCreate);

    /* Act */
    result = CF_WrappedOpenCreate(arg_fd, arg_fname, arg_flags, arg_access);

    // /* Assert */
    UtAssert_INT32_EQ(result, forced_return_OS_OpenCreate);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_OpenCreate, 1);
} /* end Test_CF_WrappedOpen_Call_OS_OpenCreate_WithGivenArgumentsAndReturnItsReturnValue */

/* end CF_WrappedOpen tests */

/*******************************************************************************
**
**  CF_WrappedClose tests
**
*******************************************************************************/

void Test_CF_WrappedClose_DoNotReceive_OS_SUCCESS_From_OS_close_EventSent(void)
{
    /* Arrange */
    int32 arg_fd = Any_uint32();

    UT_SetDefaultReturnValue(UT_KEY(OS_close), Any_int32_Except(OS_SUCCESS));

    /* Act */
    CF_WrappedClose(arg_fd);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_close, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_WrappedClose_DoNotReceive_OS_SUCCESS_From_OS_close_EventSent */

void Test_CF_WrappedClose_Receive_OS_SUCCESS_From_OS_close_NoEventSent(void)
{
    /* Arrange */
    int32 arg_fd = Any_uint32();

    UT_SetHandlerFunction(UT_KEY(OS_close), local_handler_OS_close, NULL);
    UT_SetDefaultReturnValue(UT_KEY(OS_close), OS_SUCCESS);

    /* Act */
    CF_WrappedClose(arg_fd);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_close, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_WrappedClose_Receive_OS_SUCCESS_From_OS_close_NoEventSent */

/* end CF_WrappedClose tests */

/*******************************************************************************
**
**  CF_WrappedRead tests
**
*******************************************************************************/

void Test_CF_WrappedRead_CallsOS_read_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    int32  arg_fd        = Any_int32();
    uint32 arg_read_size = Any_uint32_LessThan_or_EqualTo(10); // 10 is arbitrary to make test fast
    uint8  dummy_buf[10] = {0};                                // 10 to match max read size of 10 (arbitrary)
    void  *arg_buf       = &dummy_buf;
    int    local_result;

    UT_SetDefaultReturnValue(UT_KEY(OS_read), arg_read_size);

    /* Act */
    local_result = CF_WrappedRead(arg_fd, arg_buf, arg_read_size);

    /* Assert */
    UtAssert_True(local_result == arg_read_size,
                  "CF_WrappedRead returned %d which is the value returned from OS_read %d", local_result,
                  arg_read_size);
} /* end Test_CF_WrappedRead_CallsOS_read_WithGivenArgumentsAndReturnItsReturnValue */

/* end CF_WrappedRead tests */

/*******************************************************************************
**
**  CF_WrappedWrite tests
**
*******************************************************************************/

void Test_CF_WrappedWrite_Call_OS_write_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    int32  arg_fd = Any_int32();
    uint8  dummy_buf;
    void  *arg_buf         = &dummy_buf;
    uint32 test_write_size = Any_uint32();
    int    local_result;
    int32  expected_result = Any_int32();

    UT_SetDefaultReturnValue(UT_KEY(OS_write), expected_result);

    /* Act */
    local_result = CF_WrappedWrite(arg_fd, arg_buf, test_write_size);

    /* Assert */
    UtAssert_True(local_result == expected_result,
                  "CF_WrappedWrite returned %d which is the value returned from OS_write %d", local_result,
                  expected_result);
} /* end Test_CF_WrappedWrite_Call_OS_write_WithGivenArgumentsAndReturnItsReturnValue */

/* end CF_WrappedWrite tests */

/*******************************************************************************
**
**  CF_WrappedLseek tests
**
*******************************************************************************/

void Test_CF_WrappedLseek_Call_OS_lseek_WithGivenArgumentsAndReturnItsReturnValue(void)
{
    /* Arrange */
    int32  arg_fd      = Any_int32();
    uint32 test_offset = Any_uint32();
    int    test_mode   = Any_int();
    int    local_result;
    int32  expected_result = Any_int32();

    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), expected_result);

    /* Act */
    local_result = CF_WrappedLseek(arg_fd, test_offset, test_mode);

    /* Assert */
    UtAssert_True(local_result == expected_result,
                  "CF_WrappedLseek returned %d which is the value returned from OS_lseek %d", local_result,
                  expected_result);
} /* end Test_CF_WrappedLseek_Call_OS_lseek_WithGivenArgumentsAndReturnItsReturnValue */

/* end CF_WrappedLseek tests */

/*******************************************************************************
**
**  cf_utils_tests UtTest_Add groups
**
*******************************************************************************/

void add_cf_utils_h_tests(void)
{
    /* cf_dequeue_transaction tests */
    UtTest_Add(Test_cf_dequeue_transaction_AssertsBecause_t_IsNull, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_cf_dequeue_transaction_AssertsBecause_t_IsNull");
    UtTest_Add(Test_cf_dequeue_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS, cf_utils_tests_Setup,
               cf_utils_tests_Teardown,
               "Test_cf_dequeue_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS");
    UtTest_Add(Test_cf_dequeue_transaction_AssertsBecause_q_size_Eq0, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_cf_dequeue_transaction_AssertsBecause_q_size_Eq0");
    UtTest_Add(Test_cf_dequeue_transaction_Call_CF_CList_Remove_AndDecrement_q_size, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_cf_dequeue_transaction_Call_CF_CList_Remove_AndDecrement_q_size");
    /* end cf_dequeue_transaction tests */

    /* cf_move_transaction tests */
    UtTest_Add(Test_cf_move_transaction_AssertsBecause_t_IsNull, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_cf_move_transaction_AssertsBecause_t_IsNull");
    UtTest_Add(Test_cf_move_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_cf_move_transaction_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS");
    UtTest_Add(Test_cf_move_transaction_AssertsBecause_channel_hk_Has_q_size_Eq0, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_cf_move_transaction_AssertsBecause_channel_hk_Has_q_size_Eq0");
    UtTest_Add(Test_cf_move_transaction_Call_CF_CList_InsertBack_AndSet_q_index_ToGiven_q, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_cf_move_transaction_Call_CF_CList_InsertBack_AndSet_q_index_ToGiven_q");
    /* end cf_move_transaction tests */

    /* CF_CList_Remove_Ex tests */
    UtTest_Add(Test_CF_CList_Remove_Ex_AssertsBecause_q_size_Eq0, cf_utils_tests_Setup, cf_utils_tests_Teardown,
               "Test_CF_CList_Remove_Ex_AssertsBecause_q_size_Eq0");
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
}

void add_CF_TraverseHistory_tests(void)
{
    UtTest_Add(Test_CF_TraverseHistory_AssertsBecause_h_dir_GreaterThan_CF_DIR_NUM, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_TraverseHistory_AssertsBecause_h_dir_GreaterThan_CF_DIR_NUM");
    UtTest_Add(Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsFirstCallReturn_CLIST_EXIT, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsFirstCallReturn_CLIST_EXIT");
    UtTest_Add(Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsSecondCallReturn_CLIST_EXIT, cf_utils_tests_Setup,
               cf_utils_tests_Teardown,
               "Test_CF_TraverseHistory_When_CF_WrappedWrite_FailsSecondCallReturn_CLIST_EXIT");
    UtTest_Add(Test_CF_TraverseHistory_WhenBothWrappedWritesSuccessfulReturn_CLIST_CONT, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_TraverseHistory_WhenBothWrappedWritesSuccessfulReturn_CLIST_CONT");
}

void add_CF_TraverseTransactions_tests(void)
{
    UtTest_Add(Test_CF_TraverseTransactions_When_context_result_Is_1_Return_CLIST_EXIT, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_TraverseTransactions_When_context_result_Is_1_Return_CLIST_EXIT");
    UtTest_Add(Test_CF_TraverseTransactions_When_context_result_Is_0_Return_CLIST_CONT, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_TraverseTransactions_When_context_result_Is_0_Return_CLIST_CONT");
}

void add_CF_WriteQueueDataToFile_tests(void)
{
    UtTest_Add(Test_CF_WriteQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_WriteQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result");
}

void add_CF_WriteHistoryQueueDataToFile_tests(void)
{
    UtTest_Add(Test_CF_WriteHistoryQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result, cf_utils_tests_Setup,
               cf_utils_tests_Teardown,
               "Test_CF_WriteHistoryQueueDataToFile_Call_CF_CList_Traverse_AndReturn_arg_result");
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
    UtTest_Add(Test_CF_InsertSortPrio_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_InsertSortPrio_AssertsBecause_t_chan_num_LessThan_CF_NUM_CHANNELS");
    UtTest_Add(Test_CF_InsertSortPrio_AssertsBecause_t_state_IsNot_CFDP_IDLE, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_InsertSortPrio_AssertsBecause_t_state_IsNot_CFDP_IDLE");
    UtTest_Add(Test_CF_InsertSortPrio_Call_CF_CList_InsertBack_Ex_ListIsEmpty_AndSet_q_index_To_q, cf_utils_tests_Setup,
               cf_utils_tests_Teardown,
               "Test_CF_InsertSortPrio_Call_CF_CList_InsertBack_Ex_ListIsEmpty_AndSet_q_index_To_q");
    UtTest_Add(Test_CF_InsertSortPrio_Call_CF_CList_InsertAfter_Ex_AndSet_q_index_To_q, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_InsertSortPrio_Call_CF_CList_InsertAfter_Ex_AndSet_q_index_To_q");
    UtTest_Add(Test_CF_InsertSortPrio_When_p_t_Is_NULL_Call_CF_CList_InsertBack_Ex, cf_utils_tests_Setup,
               cf_utils_tests_Teardown, "Test_CF_InsertSortPrio_When_p_t_Is_NULL_Call_CF_CList_InsertBack_Ex");
}

void add_CF_TraverseAllTransactions__tests(void)
{
    UtTest_Add(Test_CF_TraverseAllTransactions__GetContainer_t_Call_args_fn_AndAdd_1_ToCounter, cf_utils_tests_Setup,
               cf_utils_tests_Teardown,
               "Test_CF_TraverseAllTransactions__GetContainer_t_Call_args_fn_AndAdd_1_ToCounter");
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

    add_CF_TraverseHistory_tests();

    add_CF_TraverseTransactions_tests();

    add_CF_WriteQueueDataToFile_tests();

    add_CF_WriteHistoryQueueDataToFile_tests();

    add_CF_PrioSearch_tests();

    add_CF_InsertSortPrio_tests();

    add_CF_TraverseAllTransactions__tests();

    add_CF_TraverseAllTransactions_tests();

    add_CF_TraverseAllTransactions_All_Channels_tests();

    add_CF_WrappedOpen_tests();

    add_CF_WrappedClose_tests();

    add_CF_WrappedRead_tests();

    add_CF_WrappedWrite_tests();

    add_CF_WrappedLseek_tests();

} /* end UtTest_Setup for cf_utils_tests.c */

/* end cf_utils_tests.c */
