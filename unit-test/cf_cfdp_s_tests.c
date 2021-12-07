/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_cfdp.h"

/* stub redefines for unit testing */
#undef FGV
#define FGV Stub_FGV
int32 Stub_FGV(uint8 source, CF_FIELD_FIELD name);

#include "cf_cfdp_s.c"

/*******************************************************************************
**
**  cf_cfdp_s_tests local Any style functions
**
*******************************************************************************/

CF_SendRet_t Any_cfdp_send_ret_t(void)
{
    CF_SendRet_t random_val = Any_uint8_LessThan(CF_SendRet_FAILURE + 1);

    return random_val;
}

CF_SendRet_t Any_cfdp_send_ret_t_ExceptThese(CF_SendRet_t exceptions[], uint8 num_exceptions)
{
    uint8        i          = 0;
    CF_SendRet_t random_val = Any_uint8_LessThan(CF_SendRet_FAILURE + 1);

    while (i != num_exceptions)
    {
        /* TODO: technically this could go forever so it SHOULD have some
        ** sort of stop mechanism, but practically speaking this will
        ** never really happen - so it is not that important for test code */
        if (random_val == exceptions[i])
        {
            random_val = Any_uint8_LessThan(CF_SendRet_FAILURE + 1);
            i          = 0;
        }
        else
        {
            ++i;
        }
    }

    return random_val;
}

CF_SendRet_t Any_cfdp_send_ret_t_Except(CF_SendRet_t exception)
{
    CF_SendRet_t exceptions[1] = {exception};

    return Any_cfdp_send_ret_t_ExceptThese(exceptions, 1);
}

/* end cf_cfdp_s_tests local Any style functions */

/*******************************************************************************
**
**  cf_cfdp_s_tests local dummy functions for CUT that uses function pointers
**
*******************************************************************************/

/* NOTE: To use new handler type function for dummy stubs -- replacement is in comment
void Dummy_fns_CF_CFDP_S_DispatchRecv(CF_Transaction_t* t, const CF_CFDP_PduHeader_t* ph)
{
    UT_GenStub_AddParam(Stub_FGV, CF_Transaction_t*, t);
    UT_GenStub_AddParam(Stub_FGV, CF_CFDP_PduHeader_t*, ph);

    UT_GenStub_Execute(Stub_FGV, Basic, NULL);
} */

void Dummy_fns_CF_CFDP_S_DispatchRecv(CF_Transaction_t *t, const CF_CFDP_PduHeader_t *pdu)
{
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_fns_CF_CFDP_S_DispatchRecv), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_fns_CF_CFDP_S_DispatchRecv), &pdu, sizeof(pdu));

    UT_DEFAULT_IMPL(Dummy_fns_CF_CFDP_S_DispatchRecv);
}

/* end cf_cfdp_s_tests local dummy functions */

/*******************************************************************************
**
**  cf_cfdp_s_tests local stub functions for overrides
**
*******************************************************************************/

/* int32 selected only for ease of Stub use in this file */
int32 Stub_FGV(uint8 source, CF_FIELD_FIELD name)
{
    UT_GenStub_SetupReturnBuffer(Stub_FGV, int32);

    UT_GenStub_AddParam(Stub_FGV, uint8, source);
    UT_GenStub_AddParam(Stub_FGV, CF_FIELD_FIELD, name);

    UT_GenStub_Execute(Stub_FGV, Basic, NULL);

    return UT_GenStub_GetReturnValue(Stub_FGV, int32);
}

/* end cf_cfdp_s_tests local stub functions for overrides */

/*******************************************************************************
**
**  cf_cfdp_s_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_s_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_cfdp_s_tests_Setup */

void cf_cfdp_s_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_s_tests_Teardown */

/* end cf_cfdp_s_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_CFDP_S_Reset tests (simple) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_S_ResetCall_CF_CFDP_ResetTransaction_with_keep_history(void)
{
    /* Arrange */
    CF_Transaction_t                   arg_t;
    CF_CFDP_ResetTransaction_context_t received_args;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &received_args, sizeof(CF_CFDP_ResetTransaction_context_t),
                     false);

    /* Act */
    CF_CFDP_S_Reset(&arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(received_args.t, &arg_t);
    UtAssert_True(received_args.keep_history == 1,
                  "CF_CFDP_ResetTransaction was sent %d as keep_history and should be 1", received_args.keep_history);
} /* end  Test_CF_CFDP_S_ResetCall_CF_CFDP_ResetTransaction_with_keep_history */

/* end CF_CFDP_S_Reset tests */

/*******************************************************************************
**
**  CF_CFDP_S_SendEof tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CFDP_S_SendEof_When_flag_tx_crc_calc_Is_0_Call_CF_CRC_Finalize_AndSet_crc_calc_To_1_ReturnValueOfCallTo_CF_CFDP_SendEof(
    void)
{
    /* Arrange */
    CF_Transaction_t          dummy_t;
    CF_Transaction_t         *arg_t = &dummy_t;
    cf_crc_t                 *context_CF_CRC_Finalize;
    CF_SendRet_t              local_result;
    CF_SendRet_t              forced_return_CF_CFDP_SendEof = Any_cfdp_send_ret_t();
    CF_CFDP_SendEof_context_t context_CF_CFDP_SendEof;

    arg_t->flags.com.crc_calc = 0;

    UT_SetDataBuffer(UT_KEY(CF_CRC_Finalize), &context_CF_CRC_Finalize, sizeof(context_CF_CRC_Finalize), false);

    context_CF_CFDP_SendEof.forced_return = forced_return_CF_CFDP_SendEof;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof, sizeof(context_CF_CFDP_SendEof), false);

    /* Act */
    local_result = CF_CFDP_S_SendEof(arg_t); /* Return value not checked purposefully */

    /* Assert */
    UtAssert_True(local_result == forced_return_CF_CFDP_SendEof,
                  "CF_CFDP_S_SendEof returned %u and should be %u (value returned from CF_CFDP_SendEof)", local_result,
                  forced_return_CF_CFDP_SendEof);
    UtAssert_STUB_COUNT(CF_CRC_Finalize, 1);
    UtAssert_ADDRESS_EQ(context_CF_CRC_Finalize, &arg_t->crc);
    UtAssert_True(arg_t->flags.com.crc_calc == 1, "t->flags.com.crc_calc was changed to %d and should be 1",
                  arg_t->flags.com.crc_calc);
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendEof.t, arg_t);
} /* end
     Test_CFDP_S_SendEof_When_flag_tx_crc_calc_Is_0_Call_CF_CRC_Finalize_AndSet_crc_calc_To_1_ReturnValueOfCallTo_CF_CFDP_SendEof
   */

void Test_CFDP_S_SendEof_When_crc_calc_Is_1_DoNotCall_CF_CRC_Finalize_ReturnValueOfCallTo_CF_CFDP_SendEof(void)
{
    /* Arrange */
    CF_Transaction_t          dummy_t;
    CF_Transaction_t         *arg_t = &dummy_t;
    CF_SendRet_t              local_result;
    CF_SendRet_t              forced_return_CF_CFDP_SendEof = Any_cfdp_send_ret_t();
    CF_CFDP_SendEof_context_t context_CF_CFDP_SendEof;

    arg_t->flags.com.crc_calc = 1;

    context_CF_CFDP_SendEof.forced_return = forced_return_CF_CFDP_SendEof;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof, sizeof(context_CF_CFDP_SendEof), false);

    /* Act */
    local_result = CF_CFDP_S_SendEof(arg_t); /* Return value not checked purposefully */

    /* Assert */
    UtAssert_True(local_result == forced_return_CF_CFDP_SendEof,
                  "CF_CFDP_S_SendEof returned %u and should be %u (value returned from CF_CFDP_SendEof)", local_result,
                  forced_return_CF_CFDP_SendEof);
    UtAssert_STUB_COUNT(CF_CRC_Finalize, 0);
    UtAssert_True(arg_t->flags.com.crc_calc == 1, "t->flags.com.crc_calc is %d and should be 1 (unchanged)",
                  arg_t->flags.com.crc_calc);
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendEof.t, arg_t);
} /* end Test_CFDP_S_SendEof_When_crc_calc_Is_1_DoNotCall_CF_CRC_Finalize_ReturnValueOfCallTo_CF_CFDP_SendEof */

/* end CF_CFDP_S_SendEof tests */

/*******************************************************************************
**
**  CF_CFDP_S1_SubstateSendEof tests (simple) - full coverage - [unstubbables: CF_CFDP_S_SendEof, CF_CFDP_S_Reset]
**
*******************************************************************************/

void Test_CF_CFDP_S1_SubstateSendEof_When_S_SendEof_Is_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_Reset(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    /* Arrange unstubbable: CF_CFDP_S_SendEof */
    CF_SendRet_t              forced_return_CF_CFDP_SendEof = CF_SendRet_NO_MSG;
    CF_CFDP_SendEof_context_t context_CF_CFDP_SendEof;

    arg_t->flags.com.crc_calc = 0;

    context_CF_CFDP_SendEof.forced_return = forced_return_CF_CFDP_SendEof;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof, sizeof(context_CF_CFDP_SendEof), false);

    /* Act */
    CF_CFDP_S1_SubstateSendEof(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_S_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);
    /* Assert for CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);

} /* end Test_CF_CFDP_S1_SubstateSendEof_When_S_SendEof_Is_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_Reset */

void Test_CF_CFDP_S1_SubstateSendEof_Call_CF_CFDP_S_Reset_With_t_When_CFDP_S_SendEof_IsNot_CF_SEND_NO_MSG(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    /* Arrange unstubbable: CF_CFDP_S_SendEof */
    CF_SendRet_t              forced_return_CF_CFDP_SendEof = Any_cfdp_send_ret_t_Except(CF_SendRet_NO_MSG);
    CF_CFDP_SendEof_context_t context_CF_CFDP_SendEof;

    arg_t->flags.com.crc_calc = 0;

    context_CF_CFDP_SendEof.forced_return = forced_return_CF_CFDP_SendEof;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof, sizeof(context_CF_CFDP_SendEof), false);

    /* Act */
    CF_CFDP_S1_SubstateSendEof(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_S_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);
    /* Assert for CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_S1_SubstateSendEof_Call_CF_CFDP_S_Reset_With_t_When_CFDP_S_SendEof_IsNot_CF_SEND_NO_MSG */

/* end CF_CFDP_S1_SubstateSendEof tests */

/*******************************************************************************
**
**  CF_CFDP_S2_SubstateSendEof tests (small) - full coverage - [unstubbables: cf_dequeue_transaction]
**
*******************************************************************************/

void Test_CF_CFDP_S2_SubstateSendEof_TriggerTickProcessing(void)
{
    /* Arrange */
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_InsertSortPrio_context_t context_CF_InsertSortPrio;

    /* setting sub_state and ack_timer_armed not required but assists in verification */
    arg_t->state_data.s.sub_state    = Any_uint8_Except(CF_TxSubState_WAIT_FOR_EOF_ACK);
    arg_t->flags.com.ack_timer_armed = 0;

    UT_SetDataBuffer(UT_KEY(CF_InsertSortPrio), &context_CF_InsertSortPrio, sizeof(context_CF_InsertSortPrio), false);

    /* Arrange unstubbable: cf_dequeue_transaction */
    uint16                    initial_q_size_q_index = Any_uint16_GreaterThan(0);
    CF_CList_Remove_context_t context_CF_CList_Remove;

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.hk.channel_hk[arg_t->chan_num].q_size[arg_t->flags.com.q_index] = initial_q_size_q_index;

    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_CF_CList_Remove, sizeof(context_CF_CList_Remove), false);

    /* Act */
    CF_CFDP_S2_SubstateSendEof(arg_t);

    /* Assert */
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_WAIT_FOR_EOF_ACK,
                  "sub_state is %u and should be %u (CF_TxSubState_WAIT_FOR_EOF_ACK)", arg_t->state_data.s.sub_state,
                  CF_TxSubState_WAIT_FOR_EOF_ACK);
    UtAssert_True(arg_t->flags.com.ack_timer_armed == 1, "ack_timer_armed is %u and should be 1",
                  arg_t->flags.com.ack_timer_armed);
    UtAssert_STUB_COUNT(CF_InsertSortPrio, 1);
    UtAssert_ADDRESS_EQ(context_CF_InsertSortPrio.t, arg_t);
    UtAssert_True(context_CF_InsertSortPrio.q == CF_QueueIdx_TXW,
                  "CF_InsertSortPrio received q %u and should be %u (CF_QueueIdx_TXW)", context_CF_InsertSortPrio.q,
                  CF_QueueIdx_TXW);
    /* Assert for cf_dequeue_transaction */
    UtAssert_STUB_COUNT(CF_CList_Remove, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Remove.head,
                        &CF_AppData.engine.channels[arg_t->chan_num].qs[arg_t->flags.com.q_index]);
    UtAssert_ADDRESS_EQ(context_CF_CList_Remove.node, &arg_t->cl_node);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].q_size[arg_t->flags.com.q_index] ==
                      initial_q_size_q_index - 1,
                  "q_size[t->flags.com.q_index] is %u and should be 1 less than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].q_size[arg_t->flags.com.q_index], initial_q_size_q_index);
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_WAIT_FOR_EOF_ACK,
                  "t->state_data.s.sub_state is %d and should be %d (CF_TxSubState_WAIT_FOR_EOF_ACK)",
                  arg_t->state_data.s.sub_state, CF_TxSubState_WAIT_FOR_EOF_ACK);
    UtAssert_True(arg_t->flags.com.ack_timer_armed == 1, "t->flags.com.ack_timer_armed is %d and should be 1",
                  arg_t->flags.com.ack_timer_armed);
} /* end Test_CF_CFDP_S2_SubstateSendEof_TriggerTickProcessing */

/* end CF_CFDP_S2_SubstateSendEof tests */

/*******************************************************************************
**
** CF_CFDP_S_SendFileData tests (very large) - full coverage - []
**
*******************************************************************************/

void Test_CFDP_S_SendFileData_When_ph_Is_NULL_Return_0(void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t                                    = &dummy_t;
    uint32               arg_foffs                                = Any_uint32();
    uint32               arg_bytes_to_read                        = Any_uint32();
    uint8                arg_calc_crc                             = Any_uint8();
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = NULL;

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == 0, "NULL ph caused return of %d and should be 0", result);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
} /* end Test_CFDP_S_SendFileData_When_ph_Is_NULL_Return_0 */

// NOTE:Test_CFDP_S_SendFileData sets bytes to read to chunk size if greater than chunk size not required for coverage
// but desired for completeness

void Test_CFDP_S_SendFileData_WhenCachedPosNotEq_foffs_CallTo_CF_WrappedLseek_Returns_status_NotEqTo_foffs_SendEventReturn_neg1(
    void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t             = &dummy_t;
    uint32               arg_foffs         = Any_uint32();
    uint32               arg_bytes_to_read = Any_uint32();
    uint8                arg_calc_crc      = Any_uint8();
    CF_CFDP_PduHeader_t  dummy_ph;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    int32                forced_return_CF_WrappedLseek            = Any_uint32_Except(arg_foffs);
    const char          *expected_Spec = "CF S%d(%u:%u): error seeking to offset 0x%08x, got 0x%08x";

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = Any_uint32_Except(arg_foffs);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), forced_return_CF_WrappedLseek);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == -1, "Bad lseek caused return of %d and should be -1", result);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_SEEK_FD,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_SEEK_FD)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_SEEK_FD);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end
     Test_CFDP_S_SendFileData_WhenCachedPosNotEq_foffs_CallTo_CF_WrappedLseek_Returns_status_NotEqTo_foffs_SendEventReturn_neg1
   */

// TODO: Test_CFDP_S_SendFileData CF_WrappedLSeek status is == foffs
// TODO: Test_CFDP_S_SendFileData CF_WrappedRead status is == greater than 0

void Test_CFDP_S_SendFileData_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint32            arg_foffs         = Any_uint32();
    uint32            arg_bytes_to_read = Any_uint16(); /* Any_uint16_Except(0) used to limit to same type as
                                                           CF_AppData.config_table->outgoing_file_chunk_size */
    uint8                arg_calc_crc = Any_uint8();
    CF_CFDP_PduHeader_t  dummy_ph;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    int32                forced_return_CF_WrappedLseek            = arg_foffs;
    uint16               dummy_chunk_size                         = Any_uint16();
    uint32      expected_bytes_to_read = (arg_bytes_to_read < dummy_chunk_size ? arg_bytes_to_read : dummy_chunk_size);
    int32       forced_return_CF_WrappedRead = Any_int_Except(expected_bytes_to_read);
    const char *expected_Spec                = "CF S%d(%u:%u): error reading bytes: expected 0x%08x, got 0x%08x";

    dummy_config_table.outgoing_file_chunk_size =
        arg_bytes_to_read; /* forces bytes_to_read to be eq to CF_AppData.config_table->outgoing_file_chunk_size (don't
                              run if block) */
    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = Any_uint32_Except(arg_foffs);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), forced_return_CF_WrappedLseek);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == -1, "CF_TxnState_S_SendFileData returned %d and should be -1 (fail)", result);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
    UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_READ,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_READ)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_READ);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end Test_CFDP_S_SendFileData_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1 */

void Test_CFDP_S_SendFileData_Given_bytes_to_read_GreaterThan_outgoing_file_chunk_size_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint32            arg_foffs         = Any_uint32();
    uint32            arg_bytes_to_read = Any_uint16_Except(
                   0); /* Any_uint16_Except(0) used to limit to same type as CF_AppData.config_table->outgoing_file_chunk_size */
    uint8                arg_calc_crc = Any_uint8();
    CF_CFDP_PduHeader_t  dummy_ph;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    uint32               expected_bytes_to_read                   = arg_bytes_to_read;
    int32                forced_return_CF_WrappedRead             = expected_bytes_to_read;
    const char          *expected_Spec = "CF S%d(%u:%u): error reading bytes: expected 0x%08x, got 0x%08x";

    dummy_config_table.outgoing_file_chunk_size =
        arg_bytes_to_read - 1; /* forces bytes_to_read to be greater than
                                  CF_AppData.config_table->outgoing_file_chunk_size (run if block) */
    CF_AppData.config_table = &dummy_config_table;

    arg_t->history = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = arg_foffs;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == -1, "CF_TxnState_S_SendFileData returned %d and should be -1 (fail)", result);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_READ,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_READ)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_READ);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end
     Test_CFDP_S_SendFileData_Given_bytes_to_read_GreaterThan_outgoing_file_chunk_size_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1
   */

// TODO: Test_CFDP_S_SendFileData a test for where bytes to read IS 0

void Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_NO_MSG_Return_0(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint32            arg_foffs         = Any_uint32();
    uint32            arg_bytes_to_read = Any_uint16_Except(
                   0); /* Any_uint16_Except(0) used to limit to same type as CF_AppData.config_table->outgoing_file_chunk_size */
    uint8                    arg_calc_crc = Any_uint8();
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    uint32                   expected_bytes_to_read                   = arg_bytes_to_read;
    int32                    forced_return_CF_WrappedRead             = expected_bytes_to_read;
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_config_table.outgoing_file_chunk_size =
        arg_bytes_to_read; /* forces bytes_to_read to be greater than CF_AppData.config_table->outgoing_file_chunk_size
                              (run if block) */
    CF_AppData.config_table = &dummy_config_table;

    arg_t->history = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = arg_foffs;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    context_CF_CFDP_SendFd.forced_return = CF_SendRet_NO_MSG;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == 0, "CF_TxnState_S_SendFileData returned %d and should be 0 (no bytes processed)", result);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendFd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_NO_MSG_Return_0 */

void Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_ERROR_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint32            arg_foffs         = Any_uint32();
    uint32            arg_bytes_to_read = Any_uint16_Except(
                   0); /* Any_uint16_Except(0) used to limit to same type as CF_AppData.config_table->outgoing_file_chunk_size */
    uint8                    arg_calc_crc = Any_uint8();
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    uint32                   expected_bytes_to_read                   = arg_bytes_to_read;
    int32                    forced_return_CF_WrappedRead             = expected_bytes_to_read;
    const char              *expected_Spec                            = "CF S%d(%u:%u): error sending fd";
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_config_table.outgoing_file_chunk_size =
        arg_bytes_to_read; /* forces bytes_to_read to be greater than CF_AppData.config_table->outgoing_file_chunk_size
                              (run if block) */
    CF_AppData.config_table = &dummy_config_table;

    arg_t->history = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = arg_foffs;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    context_CF_CFDP_SendFd.forced_return = CF_SendRet_ERROR;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == -1, "CF_TxnState_S_SendFileData returned %d and should be -1 (fail)", result);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendFd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_SEND_FD,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_SEND_FD)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_SEND_FD);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_ERROR_SendEventAndReturn_neg1 */

void Test_CFDP_S_SendFileData_AssertsWhen_foffs_Plus_bytes_to_read_IsLessThanOrEqTo_t_fsize(void)
{
    /* Arrange */
    // cf_config_table_t     dummy_config_table;
    // CF_History_t             dummy_history;
    // CF_Transaction_t         dummy_t;
    // CF_Transaction_t*        arg_t = &dummy_t;
    // uint32                arg_foffs = Any_uint32();
    // uint32                arg_bytes_to_read = Any_uint16_Except(0); /* Any_uint16_Except(0) used to limit to same
    // type as CF_AppData.config_table->outgoing_file_chunk_size */ uint8                 arg_calc_crc = Any_uint8();
    // CF_CFDP_PduHeader_t          dummy_ph;
    // CF_CFDP_PduHeader_t*         forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    // uint32                expected_bytes_to_read = arg_bytes_to_read;
    // int32                 forced_return_CF_WrappedRead = expected_bytes_to_read;
    // CF_CFDP_SendFd_context_t    context_CF_CFDP_SendFd;

    // dummy_config_table.outgoing_file_chunk_size = arg_bytes_to_read; /* forces bytes_to_read to be greater than
    // CF_AppData.config_table->outgoing_file_chunk_size (run if block) */ CF_AppData.config_table =
    // &dummy_config_table;

    // arg_t->history = &dummy_history;

    // UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
    // &forced_return_CF_CFDP_ConstructPduHeader);

    // arg_t->state_data.s.cached_pos = arg_foffs;

    // UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    // UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    // context_CF_CFDP_SendFd.forced_return = CF_SendRet_ERROR;
    // UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd,
    //   sizeof(context_CF_CFDP_SendFd), false);

    // UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Act */
    // result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_Assert((foffs+bytes_to_read)<=t->fsize); /* sanity check */");
    // UtAssert_True(result == -1,
    //   "CF_TxnState_S_SendFileData returned %d and should be -1 (fail)",
    //   result);
    // UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    // UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    // UtAssert_STUB_COUNT(CF_CFDP_SendFd, 1);
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    // UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_SEND_FD,
    //   "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_SEND_FD)",
    //   context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_SEND_FD);
    // UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
    //   "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
    //   context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    // UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
    //   "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
    //   context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end Test_CFDP_S_SendFileData_AssertsWhen_foffs_Plus_bytes_to_read_IsLessThanOrEqTo_t_fsize */

// TODO: Test_CFDP_S_SendFileData a test for where foffs+bytes to read is more than fsize and Asserts
// TODO: Test_CFDP_S_SendFileData a test for where foffs+bytes to read is less than fsize

void Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareNoCallTo_CF_CRC_Digest_ReturnBytesToRead(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint32            arg_foffs         = Any_uint32();
    uint32            arg_bytes_to_read = Any_uint16_Except(
                   0); /* Any_uint16_Except(0) used to limit to same type as CF_AppData.config_table->outgoing_file_chunk_size */
    uint8                    arg_calc_crc = 0; /* 0 means no calc */
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    uint32                   expected_bytes_to_read                   = arg_bytes_to_read;
    int32                    forced_return_CF_WrappedRead             = expected_bytes_to_read;
    CF_SendRet_t             exceptions[2]                            = {CF_SendRet_NO_MSG, CF_SendRet_ERROR};
    uint64                   initial_file_data_bytes                  = Any_uint64();
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_config_table.outgoing_file_chunk_size =
        arg_bytes_to_read; /* forces bytes_to_read to be greater than CF_AppData.config_table->outgoing_file_chunk_size
                              (run if block) */
    CF_AppData.config_table = &dummy_config_table;

    arg_t->history = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = arg_foffs;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    context_CF_CFDP_SendFd.forced_return = Any_cfdp_send_ret_t_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes = initial_file_data_bytes;

    arg_t->fsize = arg_foffs + expected_bytes_to_read;

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == expected_bytes_to_read,
                  "CF_TxnState_S_SendFileData returned %d and should be %d (bytes_to_read)", result,
                  expected_bytes_to_read);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendFd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes ==
                      (uint64)(initial_file_data_bytes + (uint64)expected_bytes_to_read),
                  "file_read is %lu and should be %u (bytes_to_read) more than %lu (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes, expected_bytes_to_read,
                  initial_file_data_bytes);
} /* end Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareNoCallTo_CF_CRC_Digest_ReturnBytesToRead */

void Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareWithCallTo_CF_CRC_Digest_ReturnBytesToRead(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint32            arg_foffs         = Any_uint32();
    uint32            arg_bytes_to_read = Any_uint16_Except(
                   0); /* Any_uint16_Except(0) used to limit to same type as CF_AppData.config_table->outgoing_file_chunk_size */
    uint8                    arg_calc_crc = Any_uint8_Except(0); /* any but 0 means calc */
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    uint32                   expected_bytes_to_read                   = arg_bytes_to_read;
    int32                    forced_return_CF_WrappedRead             = expected_bytes_to_read;
    CF_SendRet_t             exceptions[2]                            = {CF_SendRet_NO_MSG, CF_SendRet_ERROR};
    uint64                   initial_file_data_bytes                  = Any_uint64();
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_config_table.outgoing_file_chunk_size =
        arg_bytes_to_read; /* forces bytes_to_read to be greater than CF_AppData.config_table->outgoing_file_chunk_size
                              (run if block) */
    CF_AppData.config_table = &dummy_config_table;

    arg_t->history = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = arg_foffs;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    context_CF_CFDP_SendFd.forced_return = Any_cfdp_send_ret_t_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes = initial_file_data_bytes;

    arg_t->fsize = arg_foffs + expected_bytes_to_read;

    /* Act */
    result = CF_CFDP_S_SendFileData(arg_t, arg_foffs, arg_bytes_to_read, arg_calc_crc);

    /* Assert */
    UtAssert_True(result == expected_bytes_to_read,
                  "CF_TxnState_S_SendFileData returned %d and should be %d (bytes_to_read)", result,
                  expected_bytes_to_read);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendFd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 1);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes ==
                      (uint64)(initial_file_data_bytes + (uint64)expected_bytes_to_read),
                  "file_read is %lu and should be %u (bytes_to_read) more than %lu (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes, expected_bytes_to_read,
                  initial_file_data_bytes);
} /* end Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareWithCallTo_CF_CRC_Digest_ReturnBytesToRead */

/* end CF_CFDP_S_SendFileData tests */

/*******************************************************************************
**
**  CF_CFDP_S_SubstateSendFileData tests - (medium) - full coverage - [unstubbables: CF_CFDP_S_SendFileData]
**
*******************************************************************************/

void Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_Returns_0_DontCareSoDoNothing(void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t         = &dummy_t;
    uint32            initial_foffs = Any_uint32();
    uint8             initial_sub_state =
        Any_uint8_Except(CF_TxSubState_EOF); /* TODO: any_tx_sub_state_except would be more proper */
    uint8 initial_cc = Any_uint8_Except(
        CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* TODO: any_condition_code_except would be more proper */

    arg_t->foffs = initial_foffs;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = initial_cc;

    arg_t->state_data.s.sub_state = initial_sub_state;

    /* Arrange unstubbable:CF_CFDP_S_SendFileData - pdu will be null err_out returns 0 */
    cf_config_table_t    dummy_config_table;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = NULL;

    CF_AppData.config_table = &dummy_config_table;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    /* Act */
    CF_CFDP_S_SubstateSendFileData(arg_t);

    /* Assert */
    UtAssert_True(arg_t->foffs == initial_foffs,
                  "t->foffs is %u and should not have changed from %u (value before call)", arg_t->foffs,
                  initial_foffs);
    UtAssert_True(arg_t->history->cc == initial_cc, "t->history->cc is %u and should be %u (unchanged)",
                  arg_t->history->cc, initial_cc);
    UtAssert_True(arg_t->state_data.s.sub_state == initial_sub_state,
                  "t->state_data.s.sub_state is %u and should be %u (unchanged)", arg_t->state_data.s.sub_state,
                  initial_sub_state);
    /* Assert unstubbable CF_CFDP_S_SendFileData */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_Returns_0_DontCareSoDoNothing */

void Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_NotEq_t_fsize_DoNotSet_sub_state(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t         = &dummy_t;
    uint32            initial_foffs = Any_uint32();
    uint8             initial_sub_state =
        Any_uint8_Except(CF_TxSubState_EOF); /* TODO: any_tx_sub_state_except would be more proper */
    uint8 initial_cc = Any_uint8_Except(
        CF_CFDP_ConditionCode_FILESTORE_REJECTION);        /* TODO: any_condition_code_except would be more proper */
    uint8 dummy_bytes_received = Any_uint8_GreaterThan(1); /* uint8 used for reasonably fast size for testing */

    arg_t->foffs = initial_foffs;
    arg_t->fsize = dummy_bytes_received + initial_foffs;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = initial_cc;

    arg_t->state_data.s.sub_state = initial_sub_state;

    /* Arrange unstubbable:CF_CFDP_S_SendFileData - pdu will be null err_out returns 0 */
    cf_config_table_t        dummy_config_table;
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    int32                    forced_return_CF_WrappedRead             = 1;
    CF_SendRet_t             exceptions[2]                            = {CF_SendRet_NO_MSG, CF_SendRet_ERROR};
    uint64                   initial_file_data_bytes                  = Any_uint64();
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_config_table.outgoing_file_chunk_size = 1; /* forces any bytes to read to be more than this value */
    CF_AppData.config_table                     = &dummy_config_table;

    arg_t->history = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = arg_t->foffs;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    context_CF_CFDP_SendFd.forced_return = Any_cfdp_send_ret_t_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes = initial_file_data_bytes;

    /* Act */
    CF_CFDP_S_SubstateSendFileData(arg_t);

    /* Assert */
    UtAssert_True(arg_t->foffs == 1 + initial_foffs, "t->foffs is %u and should be 1 more than %u (value before call)",
                  arg_t->foffs, initial_foffs);
    UtAssert_True(arg_t->history->cc == initial_cc, "t->history->cc is %u and should be %u (unchanged)",
                  arg_t->history->cc, initial_cc);
    UtAssert_True(arg_t->state_data.s.sub_state == initial_sub_state,
                  "t->state_data.s.sub_state is %u and should be %u (unchanged)", arg_t->state_data.s.sub_state,
                  initial_sub_state);
    /* Assert unstubbable CF_CFDP_S_SendFileData */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end
     Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_NotEq_t_fsize_DoNotSet_sub_state
   */

void Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_IsEqTo_t_fsize_Set_sub_state_To_SEND_EOF(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t         = &dummy_t;
    uint32            initial_foffs = Any_uint16(); /* uint16 used for reasonably fast testing */
    uint8             initial_sub_state =
        Any_uint8_Except(CF_TxSubState_EOF); /* TODO: any_tx_sub_state_except would be more proper */
    uint8 initial_cc = Any_uint8_Except(
        CF_CFDP_ConditionCode_FILESTORE_REJECTION);   /* TODO: any_condition_code_except would be more proper */
    uint8 dummy_bytes_received = Any_uint8_Except(0); /* uint8 used for reasonably fast size for testing */

    arg_t->foffs = initial_foffs;
    arg_t->fsize = dummy_bytes_received + initial_foffs;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = initial_cc;

    arg_t->state_data.s.sub_state = initial_sub_state;

    /* Arrange unstubbable:CF_CFDP_S_SendFileData - pdu will be null err_out returns 0 */
    cf_config_table_t        dummy_config_table;
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    int32                    forced_return_CF_WrappedRead             = arg_t->fsize - arg_t->foffs;
    CF_SendRet_t             exceptions[2]                            = {CF_SendRet_NO_MSG, CF_SendRet_ERROR};
    uint64                   initial_file_data_bytes                  = Any_uint64();
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_config_table.outgoing_file_chunk_size = UINT16_MAX; /* bytes to read should not exceed this */
    CF_AppData.config_table                     = &dummy_config_table;

    arg_t->history = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = arg_t->foffs;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    context_CF_CFDP_SendFd.forced_return = Any_cfdp_send_ret_t_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.sent.file_data_bytes = initial_file_data_bytes;

    /* Act */
    CF_CFDP_S_SubstateSendFileData(arg_t);

    /* Assert */
    UtAssert_True(arg_t->foffs == dummy_bytes_received + initial_foffs,
                  "t->foffs is %u and should be %u more than %u (value before call)", arg_t->foffs,
                  dummy_bytes_received, initial_foffs);
    UtAssert_True(arg_t->history->cc == initial_cc, "t->history->cc is %u and should be %u (unchanged)",
                  arg_t->history->cc, initial_cc);
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_EOF,
                  "t->state_data.s.sub_state is %u and should be %u (CF_TxSubState_EOF)", arg_t->state_data.s.sub_state,
                  CF_TxSubState_EOF);
    /* Assert unstubbable CF_CFDP_S_SendFileData */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end
     Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_IsEqTo_t_fsize_Set_sub_state_To_SEND_EOF
   */

void Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsLessThan_0_Set_cc_To_CC_FILESTORE_REJECTION_And_sub_state_To_SEND_EOF(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t         = &dummy_t;
    uint32            initial_foffs = Any_uint32();
    uint8             initial_sub_state =
        Any_uint8_Except(CF_TxSubState_EOF); /* TODO: any_tx_sub_state_except would be more proper */
    uint8 initial_cc = Any_uint8_Except(
        CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* TODO: any_condition_code_except would be more proper */

    arg_t->foffs = initial_foffs;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = initial_cc;

    arg_t->state_data.s.sub_state = initial_sub_state;

    /* Arrange for CF_CFDP_S_SendFileData - ph will be null err_out returns 0 */
    cf_config_table_t    dummy_config_table;
    CF_CFDP_PduHeader_t  dummy_ph;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    int32                forced_return_CF_WrappedLseek            = Any_uint32_Except(arg_t->foffs);

    CF_AppData.config_table = &dummy_config_table;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = Any_uint32_Except(arg_t->foffs);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), forced_return_CF_WrappedLseek);

    /* Act */
    CF_CFDP_S_SubstateSendFileData(arg_t);

    /* Assert */
    UtAssert_True(arg_t->foffs == initial_foffs,
                  "t->foffs is %u and should not have changed from %u (value before call)", arg_t->foffs,
                  initial_foffs);
    UtAssert_True(arg_t->history->cc == CF_CFDP_ConditionCode_FILESTORE_REJECTION,
                  "t->history->cc is %u and should be %u (CF_CFDP_ConditionCode_FILESTORE_REJECTION)",
                  arg_t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_EOF,
                  "t->state_data.s.sub_state is %u and should be %u (CF_TxSubState_EOF)", arg_t->state_data.s.sub_state,
                  CF_TxSubState_EOF);
    /* Assert unstubbable CF_CFDP_S_SendFileData */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end
     Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsLessThan_0_Set_cc_To_CC_FILESTORE_REJECTION_And_sub_state_To_SEND_EOF
   */

/* end CF_CFDP_S_SubstateSendFileData tests */

/*******************************************************************************
**
**  CF_CFDP_S_CheckAndRespondNak tests (large) - full coverage - [unstubbables: CF_CFDP_S_SendFileData, ]
**
*******************************************************************************/

void Test_CF_CFDP_S_CheckAndRespondNak_FailsBecause_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_Return_neg1(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    int               local_result;
    CF_Transaction_t *context_CF_CFDP_SendMd;

    arg_t->flags.tx.md_need_send = 1; /* 1 = true */

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendMd), &context_CF_CFDP_SendMd, sizeof(context_CF_CFDP_SendMd), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_ERROR);

    /* Act */
    local_result = CF_CFDP_S_CheckAndRespondNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_S_SendEof returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
} /* end Test_CF_CFDP_S_CheckAndRespondNak_FailsBecause_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_Return_neg1 */

void Test_CF_CFDP_S_CheckAndRespondNak_SuccessBecause_CF_CFDP_SendMd_Returns_CF_SEND_SUCCESS_Return_1(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    int               local_result;
    CF_Transaction_t *context_CF_CFDP_SendMd;

    arg_t->flags.tx.md_need_send = 1; /* 1 = true */

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendMd), &context_CF_CFDP_SendMd, sizeof(context_CF_CFDP_SendMd), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_SUCCESS);

    /* Act */
    local_result = CF_CFDP_S_CheckAndRespondNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_S_CheckAndRespondNak returned %d and should be 1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendMd, arg_t);
    UtAssert_True(arg_t->flags.tx.md_need_send == 0,
                  "CF_CFDP_S_CheckAndRespondNak set t->flags.tx.md_need_send to %d and should be 0",
                  arg_t->flags.tx.md_need_send);
} /* end Test_CF_CFDP_S_CheckAndRespondNak_SuccessBecause_CF_CFDP_SendMd_Returns_CF_SEND_SUCCESS_Return_1 */

void Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_CFDP_SendMd_DoesNotReturn_CF_SEND_ERROR_Or_CF_SEND_SUCCESS_Return_1(
    void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t         = &dummy_t;
    CF_SendRet_t      exceptions[2] = {CF_SendRet_ERROR, CF_SendRet_SUCCESS};
    int               local_result;
    CF_Transaction_t *context_CF_CFDP_SendMd;

    arg_t->flags.tx.md_need_send = 1; /* 1 = true */

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendMd), &context_CF_CFDP_SendMd, sizeof(context_CF_CFDP_SendMd), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), Any_cfdp_send_ret_t_ExceptThese(exceptions, 2));

    /* Act */
    local_result = CF_CFDP_S_CheckAndRespondNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_S_CheckAndRespondNak returned %d and should be 1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendMd, arg_t);
    UtAssert_True(arg_t->flags.tx.md_need_send == 1,
                  "CF_CFDP_S_CheckAndRespondNak did not set t->flags.tx.md_need_send %d and should be 1",
                  arg_t->flags.tx.md_need_send);
} /* end
     Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_CFDP_SendMd_DoesNotReturn_CF_SEND_ERROR_Or_CF_SEND_SUCCESS_Return_1
   */

void Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_Chunks_GetFirstChunk_Returns_NULL_Return_0(void)
{
    /* Arrange */
    CF_Transaction_t                  dummy_t;
    CF_Transaction_t                 *arg_t = &dummy_t;
    int                               local_result;
    CF_ChunkWrapper_t                 dummy_chunks;
    CF_Chunks_GetFirstChunk_context_t context_CF_Chunks_GetFirstChunk;

    arg_t->flags.tx.md_need_send = 0; /* 0 = false */
    arg_t->chunks                = &dummy_chunks;

    context_CF_Chunks_GetFirstChunk.forced_return = NULL;
    UT_SetDataBuffer(UT_KEY(CF_Chunks_GetFirstChunk), &context_CF_Chunks_GetFirstChunk,
                     sizeof(context_CF_Chunks_GetFirstChunk), false);

    /* Act */
    local_result = CF_CFDP_S_CheckAndRespondNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_S_SendEof returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 0);
    UtAssert_True(arg_t->flags.tx.md_need_send == 0,
                  "CF_CFDP_S_CheckAndRespondNak did not set t->flags.tx.md_need_send %d and should be 0",
                  arg_t->flags.tx.md_need_send);
    UtAssert_STUB_COUNT(CF_Chunks_GetFirstChunk, 1);
    UtAssert_ADDRESS_EQ(context_CF_Chunks_GetFirstChunk.chunks, &arg_t->chunks->chunks);
} /* end Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_Chunks_GetFirstChunk_Returns_NULL_Return_0 */

void Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_Returns_0_ThenReturn_0(
    void)
{
    /* Arrange */
    CF_Transaction_t                  dummy_t;
    CF_Transaction_t                 *arg_t = &dummy_t;
    int                               local_result;
    CF_ChunkWrapper_t                 dummy_chunks;
    chunk_t                           dummy_c_instance;
    chunk_t                          *dummy_c = &dummy_c_instance;
    CF_Chunks_GetFirstChunk_context_t context_CF_Chunks_GetFirstChunk;

    arg_t->flags.tx.md_need_send = 0; /* 0 = false */
    arg_t->chunks                = &dummy_chunks;

    context_CF_Chunks_GetFirstChunk.forced_return = dummy_c;
    UT_SetDataBuffer(UT_KEY(CF_Chunks_GetFirstChunk), &context_CF_Chunks_GetFirstChunk,
                     sizeof(context_CF_Chunks_GetFirstChunk), false);

    /* Arrange for CF_CFDP_S_SendFileData - ph will be null err_out returns 0 */
    cf_config_table_t    dummy_config_table;
    CF_History_t         dummy_history;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = NULL;

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    /* Act */
    local_result = CF_CFDP_S_CheckAndRespondNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_S_CheckAndRespondNak returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 0);
    UtAssert_STUB_COUNT(CF_Chunks_GetFirstChunk, 1);
    UtAssert_ADDRESS_EQ(context_CF_Chunks_GetFirstChunk.chunks, &arg_t->chunks->chunks);
    /* Assert unstubbable:CF_CFDP_S_SendFileData */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end
     Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_Returns_0_ThenReturn_0
   */

void Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_ReturnsLessThan_0_ThenReturn_neg1(
    void)
{
    /* Arrange */
    CF_Transaction_t                  dummy_t;
    CF_Transaction_t                 *arg_t = &dummy_t;
    int                               local_result;
    CF_ChunkWrapper_t                 dummy_chunks;
    chunk_t                           dummy_c_instance;
    chunk_t                          *dummy_c = &dummy_c_instance;
    CF_Chunks_GetFirstChunk_context_t context_CF_Chunks_GetFirstChunk;

    arg_t->flags.tx.md_need_send = 0; /* 0 = false */
    arg_t->chunks                = &dummy_chunks;

    context_CF_Chunks_GetFirstChunk.forced_return = dummy_c;
    UT_SetDataBuffer(UT_KEY(CF_Chunks_GetFirstChunk), &context_CF_Chunks_GetFirstChunk,
                     sizeof(context_CF_Chunks_GetFirstChunk), false);

    /* Arrange for CF_CFDP_S_SendFileData - fail return -1 */
    cf_config_table_t    dummy_config_table;
    CF_History_t         dummy_history;
    CF_CFDP_PduHeader_t  dummy_ph;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    int32                forced_return_CF_WrappedLseek            = Any_uint32_Except(arg_t->foffs);

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = Any_uint32_Except(arg_t->foffs);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), forced_return_CF_WrappedLseek);

    /* Act */
    local_result = CF_CFDP_S_CheckAndRespondNak(arg_t);

    // /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_S_CheckAndRespondNak returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 0);
    UtAssert_STUB_COUNT(CF_Chunks_GetFirstChunk, 1);
    UtAssert_ADDRESS_EQ(context_CF_Chunks_GetFirstChunk.chunks, &arg_t->chunks->chunks);
    /* Assert unstubbable:CF_CFDP_S_SendFileData */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
} /* end
     Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_ReturnsLessThan_0_ThenReturn_neg1
   */

void Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturned_nonNULL_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_ThenReturn_1(
    void)
{
    /* Arrange */
    CF_Transaction_t                  dummy_t;
    CF_Transaction_t                 *arg_t = &dummy_t;
    int                               local_result;
    CF_ChunkWrapper_t                 dummy_chunks;
    chunk_t                           dummy_c_instance;
    chunk_t                          *dummy_c = &dummy_c_instance;
    CF_Chunks_GetFirstChunk_context_t context_CF_Chunks_GetFirstChunk;

    arg_t->flags.tx.md_need_send = 0; /* 0 = false */
    arg_t->chunks                = &dummy_chunks;

    context_CF_Chunks_GetFirstChunk.forced_return = dummy_c;
    UT_SetDataBuffer(UT_KEY(CF_Chunks_GetFirstChunk), &context_CF_Chunks_GetFirstChunk,
                     sizeof(context_CF_Chunks_GetFirstChunk), false);

    dummy_c->size = Any_uint8_GreaterThan(0); /* uint8 used for small size during testing */

    /* Arrange for CF_CFDP_S_SendFileData - fail return -1 */
    cf_config_table_t        dummy_config_table;
    CF_History_t             dummy_history;
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    int32                    forced_return_CF_WrappedRead             = dummy_c->size;
    CF_SendRet_t             exceptions[2]                            = {CF_SendRet_NO_MSG, CF_SendRet_ERROR};
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_c->offset = 0; /* dummy_c->offset = 0 used so that (foffs+bytes_to_read)<=t->fsize is never false */

    dummy_config_table.outgoing_file_chunk_size = arg_t->foffs;
    CF_AppData.config_table                     = &dummy_config_table;
    arg_t->history                              = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    arg_t->state_data.s.cached_pos = dummy_c->offset;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    context_CF_CFDP_SendFd.forced_return = Any_cfdp_send_ret_t_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    /* Act */
    local_result = CF_CFDP_S_CheckAndRespondNak(arg_t);

    // /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_S_CheckAndRespondNak returned %d and should be 1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 0);
    UtAssert_STUB_COUNT(CF_Chunks_GetFirstChunk, 1);
    UtAssert_ADDRESS_EQ(context_CF_Chunks_GetFirstChunk.chunks, &arg_t->chunks->chunks);
    UtAssert_STUB_COUNT(CF_Chunks_RemoveFromFirst, 1);
    /* Assert unstubbable:CF_CFDP_S_SendFileData */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedRead, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendFd, 1);
} /* end
     Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturned_nonNULL_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_ThenReturn_1
   */

/* end CF_CFDP_S_CheckAndRespondNak tests */

/*******************************************************************************
**
**  CF_CFDP_S2_SubstateSendFileData tests (small) - no coverage (broken) - [unstubbables: CF_CFDP_S_CheckAndRespondNak,
*CF_CFDP_S_SubstateSendFileData, CF_CFDP_S_Reset]
**
*******************************************************************************/

/* NOTE: EXAMPLE: Here is the example of what the next test would look like IFF we could stub.
** compare it to the test that had items that could not be stubbed. */
// void
// Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_0_Call_CF_CFDP_S_SubstateSendFileData(void)
// {
//     //Arrange
//     CF_Transaction_t   dummy_t;
//     CF_Transaction_t*  arg_t = &dummy_t;

//     UT_SetDefaultReturnValue(CF_CFDP_S_CheckAndRespondNak, 0);

//     //Act
//     CF_CFDP_S2_SubstateSendFileData(arg_t);

//     //Assert
//     UtAssert_STUB_COUNT(CF_CFDP_S_SubstateSendFileData, 1);
//     UtAssert_STUB_COUNT(CF_CFDP_S_Reset, 0);
// } /* end
// Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_0_Call_CF_CFDP_S_SubstateSendFileData
// */

void Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_0_Call_CF_CFDP_S_SubstateSendFileData(
    void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    /* Arrange for CF_CFDP_S_CheckAndRespondNak (get it to return 0)*/
    CF_Chunks_GetFirstChunk_context_t context_CF_Chunks_GetFirstChunk;

    arg_t->flags.tx.md_need_send = 0; /* 0 = false */

    context_CF_Chunks_GetFirstChunk.forced_return = NULL;
    UT_SetDataBuffer(UT_KEY(CF_Chunks_GetFirstChunk), &context_CF_Chunks_GetFirstChunk,
                     sizeof(context_CF_Chunks_GetFirstChunk), false);

    /* Arrange for CF_CFDP_S_SubstateSendFileData */
    uint32 initial_foffs = Any_uint32();

    arg_t->foffs = initial_foffs;

    /* Arrange for CF_CFDP_S_SendFileData - ph will be null err_out returns 0 */
    cf_config_table_t    dummy_config_table;
    CF_History_t         dummy_history;
    CF_CFDP_PduHeader_t *forced_return_CF_CFDP_ConstructPduHeader = NULL;

    dummy_config_table.outgoing_file_chunk_size = arg_t->foffs;
    CF_AppData.config_table                     = &dummy_config_table;
    arg_t->history                              = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    /* Act */
    CF_CFDP_S2_SubstateSendFileData(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_S_CheckAndRespondNak */
    UtAssert_STUB_COUNT(CF_Chunks_GetFirstChunk, 1);
    /* Assert for CF_CFDP_S_SubstateSendFileData via CF_CFDP_S_SendFileData*/
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end
     Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_0_Call_CF_CFDP_S_SubstateSendFileData
   */

/* NOTE: EXAMPLE: Here is the example of what the next test would look like IFF we could stub.
** compare it to the test that had items that could not be stubbed. */
// void
// Test_CF_CFDP_S2_SubstateSendFileData_CF_CFDP_S_CheckAndRespondNak_ReturnsNegativeValue_Call_CF_CFDP_S_Reset(void)
// {
//     //Arrange
//     CF_Transaction_t   dummy_t;
//     CF_Transaction_t*  arg_t = &dummy_t;

//     UT_SetDefaultReturnValue(CF_CFDP_S_CheckAndRespondNak, Any_int_Negative());

//     //Act
//     CF_CFDP_S2_SubstateSendFileData(arg_t);

//     //Assert
//     UtAssert_STUB_COUNT(CF_CFDP_S_SubstateSendFileData, 0);
//     UtAssert_STUB_COUNT(CF_CFDP_S_Reset, 1);
// } /* end Test_CF_CFDP_S2_SubstateSendFileData_CF_CFDP_S_CheckAndRespondNak_ReturnsNegativeValue_Call_CF_CFDP_S_Reset
// */

void Test_CF_CFDP_S2_SubstateSendFileData_CF_CFDP_S_CheckAndRespondNak_ReturnsNegativeValue_Call_CF_CFDP_S_Reset(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    /* Arrange for CF_CFDP_S_CheckAndRespondNak (get it to return a negative value)*/
    arg_t->flags.tx.md_need_send = 1; /* 1 = true */

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_ERROR);

    /* Arrange for CF_CFDP_S_Reset - none required */

    /* Act */
    CF_CFDP_S2_SubstateSendFileData(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_S_CheckAndRespondNak */
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    /* Assert for CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_S2_SubstateSendFileData_CF_CFDP_S_CheckAndRespondNak_ReturnsNegativeValue_Call_CF_CFDP_S_Reset */

/* NOTE: EXAMPLE: Here is the example of what the next test would look like IFF we could stub.
** compare it to the test that had items that could not be stubbed. */
// void Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_ReturnsPositiveValueDoNothing(void)
// {
//     //Arrange
//     CF_Transaction_t   dummy_t;
//     CF_Transaction_t*  arg_t = &dummy_t;

//     UT_SetDefaultReturnValue(CF_CFDP_S_CheckAndRespondNak, Any_int_Positive());

//     //Act
//     CF_CFDP_S2_SubstateSendFileData(arg_t);

//     //Assert
//     UtAssert_STUB_COUNT(CF_CFDP_S_SubstateSendFileData, 0);
//     UtAssert_STUB_COUNT(CF_CFDP_S_Reset, 0);
// } /* end Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_ReturnsPositiveValueDoNothing */

void Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_ReturnsPositiveValueDoNothing(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    /* Arrange for CF_CFDP_S_CheckAndRespondNak (get it to return 0)*/
    chunk_t                           fake_c;
    chunk_t                          *dummy_c = &fake_c;
    CF_Chunks_GetFirstChunk_context_t context_CF_Chunks_GetFirstChunk;

    arg_t->flags.tx.md_need_send = 0; /* 0 = false */

    context_CF_Chunks_GetFirstChunk.forced_return = dummy_c;
    UT_SetDataBuffer(UT_KEY(CF_Chunks_GetFirstChunk), &context_CF_Chunks_GetFirstChunk,
                     sizeof(context_CF_Chunks_GetFirstChunk), false);

    /* Arrange for CF_CFDP_S_SendFileData */
    dummy_c->offset = 1;
    dummy_c->size   = 1;

    /* Arrange for CF_CFDP_S_SendFileData - ph will be null err_out returns 0 */
    cf_config_table_t        dummy_config_table;
    CF_History_t             dummy_history;
    CF_CFDP_PduHeader_t      dummy_ph;
    CF_CFDP_PduHeader_t     *forced_return_CF_CFDP_ConstructPduHeader = &dummy_ph;
    CF_CFDP_SendFd_context_t context_CF_CFDP_SendFd;

    dummy_config_table.outgoing_file_chunk_size = arg_t->foffs;
    CF_AppData.config_table                     = &dummy_config_table;
    arg_t->history                              = &dummy_history;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly,
                          &forced_return_CF_CFDP_ConstructPduHeader);

    CF_AppData.config_table->outgoing_file_chunk_size =
        2; /* bytes_to_read < CF_AppData.config_table->outgoing_file_chunk_size */

    arg_t->state_data.s.cached_pos = dummy_c->offset; /* no CF_WrappedLSeek */

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), dummy_c->size); /* status == bytes_to_read */

    context_CF_CFDP_SendFd.forced_return = CF_SendRet_SUCCESS; /* status == CF_SendRet_SUCCESS */

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendFd), &context_CF_CFDP_SendFd, sizeof(context_CF_CFDP_SendFd), false);

    /* Act */
    CF_CFDP_S2_SubstateSendFileData(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_S_CheckAndRespondNak */
    UtAssert_STUB_COUNT(CF_Chunks_GetFirstChunk, 1);
    /* Assert for CF_CFDP_S_SubstateSendFileData via CF_CFDP_S_SendFileData*/
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_ReturnsPositiveValueDoNothing */

/* end CF_CFDP_S2_SubstateSendFileData tests */

/*******************************************************************************
**
**  CF_CFDP_S_SubstateSendMetadata tests (large) - some coverage (1/2) (broken) - [unstubbables: CF_CFDP_S_Reset]
**
*******************************************************************************/

void Test_CF_CFDP_S_SubstateSendMetadata_CallTo_OS_FileOpenCheck_Returns_OS_SUCCESS_Increment_fault_file_open(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t                   = &dummy_t;
    uint16                      initial_fault_file_open = Any_uint16();
    const char                 *expected_Spec           = "CF S%d(%u:%u): file %s already open";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    arg_t->history = &dummy_history;
    arg_t->history->cc =
        Any_uint8_Except(CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* Setting arg_t->history->cc not required to set,
                                                                        but helps verify change */
    arg_t->fd = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), OS_SUCCESS);

    arg_t->chan_num                                                    = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open = initial_fault_file_open;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(OS_FileOpenCheck, 1);
    /* TODO stub issue: check filename sent to OS_FileOpenCheck, stub does not allow this */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_ALREADY_OPEN,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_ALREADY_OPEN)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_ALREADY_OPEN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open ==
                      (uint16)(initial_fault_file_open + 1),
                  "fault.file_open was incremented to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open, initial_fault_file_open);
    UtAssert_True(arg_t->history->cc == CF_CFDP_ConditionCode_FILESTORE_REJECTION,
                  "t->history->cc was set to %u and should be %u (CF_CFDP_ConditionCode_FILESTORE_REJECTION)",
                  arg_t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

} /* end Test_CF_CFDP_S_SubstateSendMetadata_CallTo_OS_FileOpenCheck_Returns_OS_SUCCESS_Increment_fault_file_open */

void Test_CF_CFDP_S_SubstateSendMetadata_CallTo_CF_WrappedOpen_Makes_fd_LessThan_0_Increment_fault_file_open(void)
{
    /* Arrange */
    CF_History_t                   dummy_history;
    CF_Transaction_t               dummy_t;
    CF_Transaction_t              *arg_t                   = &dummy_t;
    uint16                         initial_fault_file_open = Any_uint16();
    const char                    *expected_Spec           = "CF S%d(%u:%u): failed to open file %s, error=0x%08x";
    CFE_EVS_SendEvent_context_t    context_CFE_EVS_SendEvent;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->history = &dummy_history;
    arg_t->history->cc =
        Any_uint8_Except(CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* Setting arg_t->history->cc not required to set,
                                                                        but helps verify change */
    arg_t->fd = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), Any_int32_Except(OS_SUCCESS));

    arg_t->chan_num                                                    = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open = initial_fault_file_open;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    // /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    // /* Assert */
    UtAssert_STUB_COUNT(OS_FileOpenCheck, 1);
    /* TODO stub issue: check filename sent to OS_FileOpenCheck, stub does not allow this */
    UtAssert_ADDRESS_EQ(context_CF_WrappedOpenCreate.fname, arg_t->history->fnames.src_filename);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_OPEN,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_OPEN)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_OPEN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open ==
                      (uint16)(initial_fault_file_open + 1),
                  "fault.file_open was incremented to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open, initial_fault_file_open);
    UtAssert_True(arg_t->history->cc == CF_CFDP_ConditionCode_FILESTORE_REJECTION,
                  "t->history->cc was set to %u and should be %u (CF_CFDP_ConditionCode_FILESTORE_REJECTION)",
                  arg_t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

} /* end Test_CF_CFDP_S_SubstateSendMetadata_CallTo_CF_WrappedOpen_Makes_fd_LessThan_0_Increment_fault_file_open */

void Test_CF_CFDP_S_SubstateSendMetadata_FirstCallTo_CF_WrappedLseek_Sets_status_LessThan_0_Increment_fault_file_seek(
    void)
{
    /* Arrange */
    CF_History_t                   dummy_history;
    CF_Transaction_t               dummy_t;
    CF_Transaction_t              *arg_t                         = &dummy_t;
    uint16                         initial_fault_file_seek       = Any_uint16();
    int32                          forced_return_CF_WrappedLseek = Any_int_Negative();
    const char                    *expected_Spec = "CF S%d(%u:%u): failed to seek end file %s, error=0x%08x";
    CFE_EVS_SendEvent_context_t    context_CFE_EVS_SendEvent;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->history = &dummy_history;
    arg_t->history->cc =
        Any_uint8_Except(CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* Setting arg_t->history->cc not required to set,
                                                                        but helps verify change */
    arg_t->fd = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), Any_int32_Except(OS_SUCCESS));

    arg_t->chan_num                                                    = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek = initial_fault_file_seek;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), forced_return_CF_WrappedLseek);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(OS_FileOpenCheck, 1);
    /* TODO stub issue: check filename sent to OS_FileOpenCheck, stub does not allow this */
    UtAssert_ADDRESS_EQ(context_CF_WrappedOpenCreate.fname, arg_t->history->fnames.src_filename);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_SEEK_END,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_SEEK_END)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_SEEK_END);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek ==
                      (uint16)(initial_fault_file_seek + 1),
                  "fault.file_seek was incremented to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek, initial_fault_file_seek);
    UtAssert_True(arg_t->history->cc == CF_CFDP_ConditionCode_FILESTORE_REJECTION,
                  "t->history->cc was set to %u and should be %u (CF_CFDP_ConditionCode_FILESTORE_REJECTION)",
                  arg_t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_S_SubstateSendMetadata_FirstCallTo_CF_WrappedLseek_Sets_status_LessThan_0_Increment_fault_file_seek */

void Test_CF_CFDP_S_SubstateSendMetadata_SecondCallTo_CF_WrappedLseek_Sets_status_ToNot_0_Increment_fault_file_seek(
    void)
{
    /* Arrange */
    CF_History_t                   dummy_history;
    CF_Transaction_t               dummy_t;
    CF_Transaction_t              *arg_t                         = &dummy_t;
    uint16                         initial_fault_file_seek       = Any_uint16();
    int32                          forced_return_CF_WrappedLseek = Any_int_Negative();
    const char                    *expected_Spec = "CF S%d(%u:%u): failed to seek begin file %s, got 0x%08x";
    CFE_EVS_SendEvent_context_t    context_CFE_EVS_SendEvent;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->history = &dummy_history;
    arg_t->history->cc =
        Any_uint8_Except(CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* Setting arg_t->history->cc not required to set,
                                                                        but helps verify change */
    arg_t->fd = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), Any_int32_Except(OS_SUCCESS));

    arg_t->chan_num                                                    = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek = initial_fault_file_seek;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 2, forced_return_CF_WrappedLseek);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(OS_FileOpenCheck, 1);
    /* TODO stub issue: check filename sent to OS_FileOpenCheck, stub does not allow this */
    UtAssert_ADDRESS_EQ(context_CF_WrappedOpenCreate.fname, arg_t->history->fnames.src_filename);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_SEEK_BEG,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_SEEK_BEG)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_SEEK_BEG);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek ==
                      (uint16)(initial_fault_file_seek + 1),
                  "fault.file_seek was incremented to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek, initial_fault_file_seek);
    UtAssert_True(arg_t->history->cc == CF_CFDP_ConditionCode_FILESTORE_REJECTION,
                  "t->history->cc was set to %u and should be %u (CF_CFDP_ConditionCode_FILESTORE_REJECTION)",
                  arg_t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_S_SubstateSendMetadata_SecondCallTo_CF_WrappedLseek_Sets_status_ToNot_0_Increment_fault_file_seek
   */

void Test_CF_CFDP_S_SubstateSendMetadata_AllFileChecksPassCallTo_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_ThenErrorOut(void)
{
    /* Arrange */
    CF_History_t                   dummy_history;
    CF_Transaction_t               dummy_t;
    CF_Transaction_t              *arg_t         = &dummy_t;
    const char                    *expected_Spec = "CF S%d(%u:%u): failed to send md";
    CFE_EVS_SendEvent_context_t    context_CFE_EVS_SendEvent;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    CF_Transaction_t              *context_CF_CFDP_SendMd;

    arg_t->history = &dummy_history;
    arg_t->history->cc =
        Any_uint8_Except(CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* Setting arg_t->history->cc not required to set,
                                                                        but helps verify change */
    arg_t->fd = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), Any_int32_Except(OS_SUCCESS));

    arg_t->chan_num = Any_cf_chan_num();

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendMd), &context_CF_CFDP_SendMd, sizeof(context_CF_CFDP_SendMd), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_ERROR);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(OS_FileOpenCheck, 1);
    /* TODO stub issue: check filename sent to OS_FileOpenCheck, stub does not allow this */
    UtAssert_ADDRESS_EQ(context_CF_WrappedOpenCreate.fname, arg_t->history->fnames.src_filename);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 2);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendMd, arg_t);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_SEND_MD,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_SEND_MD)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_SEND_MD);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(arg_t->history->cc == CF_CFDP_ConditionCode_FILESTORE_REJECTION,
                  "t->history->cc was set to %u and should be %u (CF_CFDP_ConditionCode_FILESTORE_REJECTION)",
                  arg_t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_S_SubstateSendMetadata_AllFileChecksPassCallTo_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_ThenErrorOut
   */

void Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_ThenErrorOut(
    void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_Transaction_t           *context_CF_CFDP_SendMd;
    const char                 *expected_Spec = "CF S%d(%u:%u): failed to send md";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    arg_t->history = &dummy_history;
    arg_t->history->cc =
        Any_uint8_Except(CF_CFDP_ConditionCode_FILESTORE_REJECTION); /* Setting arg_t->history->cc not required to set,
                                                                        but helps verify change */
    arg_t->fd = Any_int32_Except(0);

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendMd), &context_CF_CFDP_SendMd, sizeof(context_CF_CFDP_SendMd), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_ERROR);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(OS_FileOpenCheck, 0);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 0);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendMd, arg_t);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_SEND_MD,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_SEND_MD)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_SEND_MD);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(arg_t->history->cc == CF_CFDP_ConditionCode_FILESTORE_REJECTION,
                  "t->history->cc was set to %u and should be %u (CF_CFDP_ConditionCode_FILESTORE_REJECTION)",
                  arg_t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_ThenErrorOut
   */

void Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_SUCCESS_Then_sub_state_SetTo_SEND_FILE_DATA(
    void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_Transaction_t *context_CF_CFDP_SendMd;

    arg_t->state_data.s.sub_state =
        Any_uint8_Except(CF_TxSubState_FILEDATA); /* setting sub_state is not required, but it helps verification */
    arg_t->fd = Any_int32_Except(0);

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendMd), &context_CF_CFDP_SendMd, sizeof(context_CF_CFDP_SendMd), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_SUCCESS);

    /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendMd, arg_t);
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_FILEDATA,
                  "sub_state set to %u and should be %u (CF_TxSubState_FILEDATA)", arg_t->state_data.s.sub_state,
                  CF_TxSubState_FILEDATA);

} /* end
     Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_SUCCESS_Then_sub_state_SetTo_SEND_FILE_DATA
   */

void Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_NO_MSG_DoNotSet_sub_state(
    void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint8             initial_sub_state = Any_uint8();
    CF_Transaction_t *context_CF_CFDP_SendMd;

    arg_t->state_data.s.sub_state =
        initial_sub_state; /* setting sub_state is not required, but it helps verification */
    arg_t->fd = Any_int32_Except(0);

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendMd), &context_CF_CFDP_SendMd, sizeof(context_CF_CFDP_SendMd), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_NO_MSG);

    /* Act */
    CF_CFDP_S_SubstateSendMetadata(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendMd, arg_t);
    UtAssert_True(arg_t->state_data.s.sub_state == initial_sub_state,
                  "sub_state set to %u and should be %u (value before call)", arg_t->state_data.s.sub_state,
                  initial_sub_state);

} /* end
     Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_NO_MSG_DoNotSet_sub_state
   */

/* end CF_CFDP_S_SubstateSendMetadata tests */

/*******************************************************************************
**
**  CF_CFDP_S_SubstateSendFinAck tests (simple) - full coverage - [unstubbables: CF_CFDP_S_Reset]
**
*******************************************************************************/

void Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_Returns_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_Reset_(
    void)
{
    /* Arrange */
    CF_History_t              dummy_history;
    CF_Transaction_t          dummy_t;
    CF_Transaction_t         *arg_t = &dummy_t;
    CF_CFDP_SendAck_context_t context_CF_CFDP_SendAck;

    arg_t->history                = &dummy_history;
    arg_t->state_data.s.s2.fin_cc = Any_uint8();
    arg_t->history->peer_eid      = Any_uint8();
    arg_t->history->seq_num       = Any_uint32();

    context_CF_CFDP_SendAck.forced_return = CF_SendRet_NO_MSG;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendAck), &context_CF_CFDP_SendAck, sizeof(context_CF_CFDP_SendAck), false);

    /* Arrange unstubbable: CF_CFDP_S_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_S_SubstateSendFinAck(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendAck.t, arg_t);
    UtAssert_True(context_CF_CFDP_SendAck.ts == CF_CFDP_AckTxnStatus_ACTIVE,
                  "CF_CFDP_SendAck received ts %u and should be %u (CF_CFDP_AckTxnStatus_ACTIVE)",
                  context_CF_CFDP_SendAck.ts, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_True(context_CF_CFDP_SendAck.dir_code == CF_CFDP_FileDirective_FIN,
                  "CF_CFDP_SendAck received dir_code %u and should be %u (CF_CFDP_FileDirective_FIN)",
                  context_CF_CFDP_SendAck.dir_code, CF_CFDP_FileDirective_FIN);
    UtAssert_True(context_CF_CFDP_SendAck.cc == arg_t->state_data.s.s2.fin_cc,
                  "CF_CFDP_SendAck received cc %u and should be %u (t->state_data.s.s2.fin_cc)",
                  context_CF_CFDP_SendAck.cc, arg_t->state_data.s.s2.fin_cc);
    UtAssert_True(context_CF_CFDP_SendAck.peer_eid == arg_t->history->peer_eid,
                  "CF_CFDP_SendAck received peer_eid %u and should be %u (t->history->peer_eid)",
                  context_CF_CFDP_SendAck.peer_eid, arg_t->history->peer_eid);
    UtAssert_True(context_CF_CFDP_SendAck.tsn == arg_t->history->seq_num,
                  "CF_CFDP_SendAck received tsn %u and should be %u (t->history->seq_num)", context_CF_CFDP_SendAck.tsn,
                  arg_t->history->seq_num);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);

} /* end Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_Returns_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_Reset_
   */

void Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_DoesNotReturn_CF_SEND_NO_MSG_Call_CF_CFDP_S_Reset_With_t_AsArgument(
    void)
{
    /* Arrange */
    CF_History_t              dummy_history;
    CF_Transaction_t          dummy_t;
    CF_Transaction_t         *arg_t = &dummy_t;
    CF_CFDP_SendAck_context_t context_CF_CFDP_SendAck;

    arg_t->history                = &dummy_history;
    arg_t->state_data.s.s2.fin_cc = Any_uint8();
    arg_t->history->peer_eid      = Any_uint8();
    arg_t->history->seq_num       = Any_uint32();

    context_CF_CFDP_SendAck.forced_return = Any_cfdp_send_ret_t_Except(CF_SendRet_NO_MSG);

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendAck), &context_CF_CFDP_SendAck, sizeof(context_CF_CFDP_SendAck), false);

    /* Arrange unstubbable: CF_CFDP_S_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_S_SubstateSendFinAck(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendAck.t, arg_t);
    UtAssert_True(context_CF_CFDP_SendAck.ts == CF_CFDP_AckTxnStatus_ACTIVE,
                  "CF_CFDP_SendAck received ts %u and should be %u (CF_CFDP_AckTxnStatus_ACTIVE)",
                  context_CF_CFDP_SendAck.ts, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_True(context_CF_CFDP_SendAck.dir_code == CF_CFDP_FileDirective_FIN,
                  "CF_CFDP_SendAck received dir_code %u and should be %u (CF_CFDP_FileDirective_FIN)",
                  context_CF_CFDP_SendAck.dir_code, CF_CFDP_FileDirective_FIN);
    UtAssert_True(context_CF_CFDP_SendAck.cc == arg_t->state_data.s.s2.fin_cc,
                  "CF_CFDP_SendAck received cc %u and should be %u (t->state_data.s.s2.fin_cc)",
                  context_CF_CFDP_SendAck.cc, arg_t->state_data.s.s2.fin_cc);
    UtAssert_True(context_CF_CFDP_SendAck.peer_eid == arg_t->history->peer_eid,
                  "CF_CFDP_SendAck received peer_eid %u and should be %u (t->history->peer_eid)",
                  context_CF_CFDP_SendAck.peer_eid, arg_t->history->peer_eid);
    UtAssert_True(context_CF_CFDP_SendAck.tsn == arg_t->history->seq_num,
                  "CF_CFDP_SendAck received tsn %u and should be %u (t->history->seq_num)", context_CF_CFDP_SendAck.tsn,
                  arg_t->history->seq_num);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received keep_history %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history == 1);

} /* end
     Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_DoesNotReturn_CF_SEND_NO_MSG_Call_CF_CFDP_S_Reset_With_t_AsArgument
   */

/* end CF_CFDP_S_SubstateSendFinAck tests */

/*******************************************************************************
**
**  CF_CFDP_S2_EarlyFin tests (simple) - full coverage - [unstubbables: CF_CFDP_S_Reset]
**
*******************************************************************************/

void Test_CF_CFDP_S2_EarlyFin_SendEventAndCallReset(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    const CF_CFDP_PduHeader_t   dummy_ph;
    const CF_CFDP_PduHeader_t  *arg_ph        = &dummy_ph;
    const char                 *expected_Spec = "CF S%d(%u:%u): got early fin -- cancelling";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    arg_t->history = &dummy_history;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CFDP_S_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_S2_EarlyFin(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_EARLY_FIN,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_EARLY_FIN)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_EARLY_FIN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received keep_history %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history == 1);
} /* end Test_CF_CFDP_S2_EarlyFin_SendEventAndCallReset */

/* end CF_CFDP_S2_EarlyFin tests */

/*******************************************************************************
**
**  CF_CFDP_S2_Fin tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_Returns_0_Set_fin_cc_And_sub_state(void)
{
    /* Arrange */
    CF_Transaction_t           dummy_t;
    CF_Transaction_t          *arg_t       = &dummy_t;
    uint8                      dummy_flags = Any_uint8();
    const CF_CFDP_PduHeader_t  dummy_ph;
    const CF_CFDP_PduHeader_t *arg_ph;
    uint8                      expected_fin_cc = FGV(dummy_flags, CF_CFDP_PduFin_FLAGS_CC);

    memcpy((void *)&dummy_ph.flags, &dummy_flags, 1);
    arg_ph = &dummy_ph;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvFin), 0);

    /* Act */
    CF_CFDP_S2_Fin(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFin, 1);
    UtAssert_True(arg_t->state_data.s.s2.fin_cc == expected_fin_cc,
                  "fin_cc is set to %u and should be %u (FGV(pdu->fin.flags, CF_CFDP_PduFin_FLAGS_CC))",
                  arg_t->state_data.s.s2.fin_cc, expected_fin_cc);
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_SEND_FIN_ACK,
                  "sub_state is set to %u and should be %u (CF_TxSubState_SEND_FIN_ACK)", arg_t->state_data.s.sub_state,
                  CF_TxSubState_SEND_FIN_ACK);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

} /* end Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_Returns_0_Set_fin_cc_And_sub_state */

void Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_DoesNotReturn_0_SendEventAndCountReceiveError(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    const CF_CFDP_PduHeader_t   dummy_ph;
    const CF_CFDP_PduHeader_t  *arg_ph             = &dummy_ph;
    uint32                      initial_recv_error = Any_uint32();
    const char                 *expected_Spec      = "CF S%d(%u:%u): received invalid fin pdu";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvFin), Any_int_Except(0));

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S2_Fin(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFin, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_PDU_FIN,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_PDU_FIN)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_PDU_FIN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 1,
                  "counter recv error set to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);

} /* end Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_DoesNotReturn_0_SendEventAndCountReceiveError */

/* end CF_CFDP_S2_Fin tests */

/*******************************************************************************
**
**  CF_CFDP_S2_Nak tests (large) - some coverage (1/4) (broken) - []
**
*******************************************************************************/

void Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_neg1_SendEventAndIncrement_recv_error(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_CFDP_PduNak_t            dummy_ph;
    CF_CFDP_PduHeader_t        *arg_ph             = (CF_CFDP_PduHeader_t *)&dummy_ph;
    uint32                      initial_recv_error = Any_uint32();
    const char                 *expected_Spec      = "CF S%d(%u:%u): received invalid nak pdu";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    int                         context_CF_CFDP_RecvNak_forced_num_sr = Any_int();

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), -1);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S2_Nak(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_PDU_NAK,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_PDU_NAK)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_PDU_NAK);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 1,
                  "counter recv error set to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
} /* end Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_neg1_SendEventAndIncrement_recv_error */

void Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_0_Set_num_sr_to_0_SendEventAndIncrement_recv_error(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_CFDP_PduNak_t            dummy_ph;
    CF_CFDP_PduHeader_t        *arg_ph             = (CF_CFDP_PduHeader_t *)&dummy_ph;
    uint32                      initial_recv_error = Any_uint32();
    const char                 *expected_Spec      = "CF S%d(%u:%u): received invalid nak pdu";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    int                         context_CF_CFDP_RecvNak_forced_num_sr = 0;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), 0);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S2_Nak(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_PDU_NAK,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_PDU_NAK)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_PDU_NAK);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 1,
                  "counter recv error set to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);

} /* end Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_0_Set_num_sr_to_0_SendEventAndIncrement_recv_error */

void Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Asserts_num_sr_IsGreaterThan_CF_NAK_MAX_SEGMENTS(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 issue");
} /* end Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Asserts_num_sr_IsGreaterThan_CF_NAK_MAX_SEGMENTS */

void Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Sets_md_need_send_To_1_When_offset_start_And_offset_end_Are_0_Add_num_sr_To_nak_segment_requests(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_CFDP_PduNak_t     dummy_ph;
    CF_CFDP_PduHeader_t *arg_ph                                = (CF_CFDP_PduHeader_t *)&dummy_ph;
    uint32               initial_nak_segment_requests          = Any_uint32();
    int                  context_CF_CFDP_RecvNak_forced_num_sr = 1;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests = initial_nak_segment_requests;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), 0);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    dummy_ph.segment_requests[0].offset_start = 0;
    dummy_ph.segment_requests[0].offset_end   = 0;

    arg_t->flags.tx.md_need_send = 0; /* setting md_need_send is not required, but assists in verification */

    /* Act */
    CF_CFDP_S2_Nak(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests ==
            (uint32)(initial_nak_segment_requests + context_CF_CFDP_RecvNak_forced_num_sr),
        "counter recv nak_segment_requests set to %u and should be %u (num_sr) more than %u (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests,
        context_CF_CFDP_RecvNak_forced_num_sr, initial_nak_segment_requests);
    UtAssert_True(arg_t->flags.tx.md_need_send == 1, "md_need_send set to %u and should be 1",
                  arg_t->flags.tx.md_need_send);
} /* end
     Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Sets_md_need_send_To_1_When_offset_start_And_offset_end_Are_0_Add_num_sr_To_nak_segment_requests
   */

void Test_CF_CFDP_S2_Nak_SendsEventBecause_offset_end_IsLessThan_offset_start(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_CFDP_PduNak_t            dummy_ph;
    CF_CFDP_PduHeader_t        *arg_ph = (CF_CFDP_PduHeader_t *)&dummy_ph;
    uint32                      dummy_offset_start;
    uint32                      dummy_offset_end;
    uint32                      initial_nak_segment_requests = Any_uint32();
    const char                 *expected_Spec = "CF S%d(%u:%u): received %d invalid nak segment requests";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    int                         context_CF_CFDP_RecvNak_forced_num_sr = 1;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();
    arg_t->fsize    = Any_uint32_BetweenExcludeMax(1, UINT16_MAX);

    dummy_offset_start = Any_uint32_GreaterThan(0);
    dummy_offset_end   = Any_uint32_LessThan(dummy_offset_start);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests = initial_nak_segment_requests;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), 0);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    dummy_ph.segment_requests[0].offset_start = dummy_offset_start;
    dummy_ph.segment_requests[0].offset_end   = dummy_offset_end;

    arg_t->flags.tx.md_need_send = 0; /* setting md_need_send is not required, but assists in verification */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S2_Nak(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests ==
            (uint32)(initial_nak_segment_requests + context_CF_CFDP_RecvNak_forced_num_sr),
        "counter recv nak_segment_requests set to %u and should be %u (num_sr) more than %u (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests,
        context_CF_CFDP_RecvNak_forced_num_sr, initial_nak_segment_requests);
    UtAssert_True(arg_t->flags.tx.md_need_send == 0, "md_need_send is %u and should not have changed from 0",
                  arg_t->flags.tx.md_need_send);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_INVALID_SR,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_INVALID_SR)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_INVALID_SR);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end Test_CF_CFDP_S2_Nak_SendsEventBecause_offset_end_IsLessThan_offset_start */

void Test_CF_CFDP_S2_Nak_SendsEventBecause_start_Plus_size_IsGreaterThanTransaction_fsize(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_CFDP_PduNak_t            dummy_ph;
    CF_CFDP_PduHeader_t        *arg_ph = (CF_CFDP_PduHeader_t *)&dummy_ph;
    uint32                      dummy_offset_start;
    uint32                      dummy_offset_end;
    uint32                      initial_nak_segment_requests = Any_uint32();
    const char                 *expected_Spec = "CF S%d(%u:%u): received %d invalid nak segment requests";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    int                         context_CF_CFDP_RecvNak_forced_num_sr = 1;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();
    arg_t->fsize    = Any_uint32_BetweenExcludeMax(1, UINT16_MAX);

    dummy_offset_end   = arg_t->fsize + 1;
    dummy_offset_start = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests = initial_nak_segment_requests;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), 0);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    dummy_ph.segment_requests[0].offset_start = dummy_offset_start;
    dummy_ph.segment_requests[0].offset_end   = dummy_offset_end;

    arg_t->flags.tx.md_need_send = 0; /* setting md_need_send is not required, but assists in verification */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S2_Nak(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests ==
            (uint32)(initial_nak_segment_requests + context_CF_CFDP_RecvNak_forced_num_sr),
        "counter recv nak_segment_requests set to %u and should be %u (num_sr) more than %u (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests,
        context_CF_CFDP_RecvNak_forced_num_sr, initial_nak_segment_requests);
    UtAssert_True(arg_t->flags.tx.md_need_send == 0, "md_need_send is %u and should not have changed from 0",
                  arg_t->flags.tx.md_need_send);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_INVALID_SR,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_INVALID_SR)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_INVALID_SR);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end Test_CF_CFDP_S2_Nak_SendsEventBecause_start_Plus_size_IsGreaterThanTransaction_fsize */

void Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsEqualToTransaction_fsize(void)
{
    /* Arrange */
    CF_History_t            dummy_history;
    CF_Transaction_t        dummy_t;
    CF_Transaction_t       *arg_t = &dummy_t;
    CF_CFDP_PduNak_t        dummy_ph;
    CF_CFDP_PduHeader_t    *arg_ph = (CF_CFDP_PduHeader_t *)&dummy_ph;
    uint32                  dummy_offset_start;
    uint32                  dummy_offset_end;
    uint32                  initial_nak_segment_requests = Any_uint32();
    CF_Chunks_Add_context_t context_CF_Chunks_Add;
    int                     context_CF_CFDP_RecvNak_forced_num_sr = 1;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();
    arg_t->fsize    = Any_uint32_BetweenExcludeMax(1, UINT16_MAX);

    dummy_offset_end   = arg_t->fsize;
    dummy_offset_start = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests = initial_nak_segment_requests;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), 0);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    dummy_ph.segment_requests[0].offset_start = dummy_offset_start;
    dummy_ph.segment_requests[0].offset_end   = dummy_offset_end;

    arg_t->flags.tx.md_need_send = 0; /* setting md_need_send is not required, but assists in verification */

    UT_SetDataBuffer(UT_KEY(CF_Chunks_Add), &context_CF_Chunks_Add, sizeof(context_CF_Chunks_Add), false);

    /* Act */
    CF_CFDP_S2_Nak(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests ==
            (uint32)(initial_nak_segment_requests + context_CF_CFDP_RecvNak_forced_num_sr),
        "counter recv nak_segment_requests set to %u and should be %u (num_sr) more than %u (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests,
        context_CF_CFDP_RecvNak_forced_num_sr, initial_nak_segment_requests);
    UtAssert_True(arg_t->flags.tx.md_need_send == 0, "md_need_send is %u and should not have changed from 0",
                  arg_t->flags.tx.md_need_send);
    UtAssert_STUB_COUNT(CF_Chunks_Add, 1);
    UtAssert_ADDRESS_EQ(context_CF_Chunks_Add.chunks, &arg_t->chunks->chunks);
    UtAssert_True(context_CF_Chunks_Add.offset == dummy_offset_start,
                  "CF_Chunks_Add received offset %u and should be %u (start)", context_CF_Chunks_Add.offset,
                  dummy_offset_start);
    UtAssert_True(context_CF_Chunks_Add.size == dummy_offset_end - dummy_offset_start,
                  "CF_Chunks_Add received size %u and should be %u (size)", context_CF_Chunks_Add.size,
                  dummy_offset_end - dummy_offset_start);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsEqualToTransaction_fsize */

void Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsLessThanTransaction_fsize(void)
{
    /* Arrange */
    CF_History_t            dummy_history;
    CF_Transaction_t        dummy_t;
    CF_Transaction_t       *arg_t = &dummy_t;
    CF_CFDP_PduNak_t        dummy_ph;
    CF_CFDP_PduHeader_t    *arg_ph = (CF_CFDP_PduHeader_t *)&dummy_ph;
    uint32                  dummy_offset_start;
    uint32                  dummy_offset_end;
    uint32                  initial_nak_segment_requests = Any_uint32();
    CF_Chunks_Add_context_t context_CF_Chunks_Add;
    int                     context_CF_CFDP_RecvNak_forced_num_sr = 1;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();
    arg_t->fsize    = Any_uint32_BetweenExcludeMax(1, UINT32_MAX);

    dummy_offset_end   = Any_uint32_LessThan(arg_t->fsize);
    dummy_offset_start = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests = initial_nak_segment_requests;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), 0);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    dummy_ph.segment_requests[0].offset_start = dummy_offset_start;
    dummy_ph.segment_requests[0].offset_end   = dummy_offset_end;

    arg_t->flags.tx.md_need_send = 0; /* setting md_need_send is not required, but assists in verification */

    UT_SetDataBuffer(UT_KEY(CF_Chunks_Add), &context_CF_Chunks_Add, sizeof(context_CF_Chunks_Add), false);

    /* Act */
    CF_CFDP_S2_Nak(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests ==
            (uint32)(initial_nak_segment_requests + context_CF_CFDP_RecvNak_forced_num_sr),
        "counter recv nak_segment_requests set to %u and should be %u (num_sr) more than %u (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.nak_segment_requests,
        context_CF_CFDP_RecvNak_forced_num_sr, initial_nak_segment_requests);
    UtAssert_True(arg_t->flags.tx.md_need_send == 0, "md_need_send is %u and should not have changed from 0",
                  arg_t->flags.tx.md_need_send);
    UtAssert_STUB_COUNT(CF_Chunks_Add, 1);
    UtAssert_ADDRESS_EQ(context_CF_Chunks_Add.chunks, &arg_t->chunks->chunks);
    UtAssert_True(context_CF_Chunks_Add.offset == dummy_offset_start,
                  "CF_Chunks_Add received offset %u and should be %u (start)", context_CF_Chunks_Add.offset,
                  dummy_offset_start);
    UtAssert_True(context_CF_Chunks_Add.size == dummy_offset_end - dummy_offset_start,
                  "CF_Chunks_Add received size %u and should be %u (size)", context_CF_Chunks_Add.size,
                  dummy_offset_end - dummy_offset_start);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsLessThanTransaction_fsize */

/* end CF_CFDP_S2_Nak tests */

/*******************************************************************************
**
**  CF_CFDP_S2_Nak_Arm tests (simple) - full coverage - [unstubbables: CF_CFDP_S2_Nak]
**
*******************************************************************************/

void Test_CF_CFDP_S2_Nak_Arm_Call_CF_CFDP_ArmAckTimer_And_CF_CFDP_S2_Nak(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    const CF_CFDP_PduHeader_t   dummy_ph;
    const CF_CFDP_PduHeader_t  *arg_ph                                = &dummy_ph;
    uint32                      initial_recv_error                    = Any_uint32();
    int                         context_CF_CFDP_RecvNak_forced_num_sr = Any_int();
    CF_Transaction_t           *context_CF_CFDP_ArmAckTimer;
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    /* Arrange unstubbable: CF_CFDP_S2_Nak */
    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ArmAckTimer), &context_CF_CFDP_ArmAckTimer, sizeof(context_CF_CFDP_ArmAckTimer),
                     false);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvNak), -1);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_RecvNak), &context_CF_CFDP_RecvNak_forced_num_sr,
                     sizeof(context_CF_CFDP_RecvNak_forced_num_sr), false);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_CFDP_S2_Nak_Arm(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ArmAckTimer, arg_t);
    /* Assert for CF_CFDP_S2_Nak */
    UtAssert_STUB_COUNT(CF_CFDP_RecvNak, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_PDU_NAK,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_PDU_NAK)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_PDU_NAK);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 1,
                  "counter recv error set to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
} /* end Test_CF_CFDP_S2_Nak_Arm_Call_CF_CFDP_ArmAckTimer_And_CF_CFDP_S2_Nak */

/* end CF_CFDP_S2_Nak_Arm tests */

/*******************************************************************************
**
**  CF_CFDP_S2_WaitForEofAck tests (medium) - full coverage - [unstubbables: CF_CFDP_S_Reset]
**
*******************************************************************************/

void Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_neg1_SendEventAndIncrement_recv_error(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    const CF_CFDP_PduHeader_t   dummy_ph;
    const CF_CFDP_PduHeader_t  *arg_ph             = &dummy_ph;
    uint32                      initial_recv_error = Any_uint32();
    const char                 *expected_Spec      = "CF S%d(%u:%u): received invalid eof pdu";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    arg_t->history = &dummy_history;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvAck), -1); /* -1 is fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    /* Act */
    CF_CFDP_S2_WaitForEofAck(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_PDU_EOF,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_PDU_EOF)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_PDU_EOF);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 1,
                  "counter recv error set to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);

} /* end Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_neg1_SendEventAndIncrement_recv_error */

void Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_EqTo_CC_NO_ERROR_Set_sub_state_To_SEND_WAIT_FOR_FIN_And_ack_time_armed_To_0(
    void)
{
    /* Arrange */
    CF_History_t               dummy_history;
    CF_Transaction_t           dummy_t;
    CF_Transaction_t          *arg_t = &dummy_t;
    const CF_CFDP_PduHeader_t  dummy_ph;
    const CF_CFDP_PduHeader_t *arg_ph = &dummy_ph;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = CF_CFDP_ConditionCode_NO_ERROR;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvAck), 0); /* 0 is pass */

    arg_t->state_data.s.sub_state =
        Any_uint8_Except(CF_TxSubState_WAIT_FOR_FIN); /* setting sub_state not required, but helps verification */
    arg_t->flags.com.ack_timer_armed = 1;             /* setting ack_timer_armed not required, but helps verification */

    /* Act */
    CF_CFDP_S2_WaitForEofAck(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_WAIT_FOR_FIN,
                  "sub_state was set to %u and should be %u (CF_TxSubState_WAIT_FOR_FIN)",
                  arg_t->state_data.s.sub_state, CF_TxSubState_WAIT_FOR_FIN);
    UtAssert_True(arg_t->flags.com.ack_timer_armed == 0, "sub_state was set to %u and should be 0",
                  arg_t->flags.com.ack_timer_armed);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end
     Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_EqTo_CC_NO_ERROR_Set_sub_state_To_SEND_WAIT_FOR_FIN_And_ack_time_armed_To_0
   */

void Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_NotEqTo_CC_NO_ERROR_Call_CF_CFDP_S_Reset(
    void)
{
    /* Arrange */
    CF_History_t               dummy_history;
    CF_Transaction_t           dummy_t;
    CF_Transaction_t          *arg_t = &dummy_t;
    const CF_CFDP_PduHeader_t  dummy_ph;
    const CF_CFDP_PduHeader_t *arg_ph = &dummy_ph;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = Any_uint8_Except(CF_CFDP_ConditionCode_NO_ERROR);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvAck), 0); /* 0 is pass */

    /* Act */
    CF_CFDP_S2_WaitForEofAck(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_NotEqTo_CC_NO_ERROR_Call_CF_CFDP_S_Reset
   */

/* end CF_CFDP_S2_WaitForEofAck tests */

/*******************************************************************************
**
**  CF_CFDP_S_DispatchRecv tests (medium) - no coverage (broken) - []
**
*******************************************************************************/

void Test_CF_CFDP_S_DispatchRecv_Asserts_sub_state_LessThan_SEND_NUM_STATES(void)
{
    /* Arrange */
    // CF_Transaction_t   dummy_t;
    // CF_Transaction_t*  arg_t = &dummy_t;
    // void *          arg_fns = NULL;

    // arg_t->state_data.s.sub_state = CF_TxSubState_NUM_STATES;

    /* Act */
    // CF_CFDP_S_DispatchRecv(arg_t, arg_fns);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 issue");
    // UtAssert_STUB_COUNT(CF_HandleAssert, 1);
} /* end Test_CF_CFDP_S_DispatchRecv_Asserts_sub_state_LessThan_SEND_NUM_STATES */

void Test_CF_CFDP_S_DispatchRecv_Asserts_msg_in_Is_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 issue");
} /* end Test_CF_CFDP_S_DispatchRecv_Asserts_msg_in_Is_NULL */

void Test_CF_CFDP_S_DispatchRecv_AlreadyHas_pdu_ph_flags_SetSoSendEvent(void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_UT_inmsg_buffer_t        dummy_msg;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t         = &dummy_t;
    const char                 *expected_Spec = "CF S%d(%u:%u): received non-file directive pdu";
    void                       *arg_fns       = NULL;
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->state_data.s.sub_state = Any_uint8_LessThan(CF_TxSubState_NUM_STATES);

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);
    arg_t->history = &dummy_history;

    /* Act */
    CF_CFDP_S_DispatchRecv(arg_t, arg_fns);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_NON_FD_PDU,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_NON_FD_PDU)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_NON_FD_PDU);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end Test_CF_CFDP_S_DispatchRecv_AlreadyHas_pdu_ph_flags_SetSoSendEvent */

void Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsEqTo_PDU_INVALID_MAX_SendEventAndIncrement_recv_spurious(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_UT_inmsg_buffer_t dummy_msg;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t                 = &dummy_t;
    uint8                dummy_chan_num        = Any_cf_chan_num();
    uint8                dummy_flags           = CF_CFDP_FileDirective_INVALID_MAX;
    uint16               initial_recv_spurious = Any_uint16();
    const char          *expected_Spec = "CF S%d(%u:%u): received pdu with invalid directive code %d for sub-state %d";
    void                *arg_fns       = NULL;
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->state_data.s.sub_state = Any_uint8_LessThan(CF_TxSubState_NUM_STATES);

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_msg.pdu_r_msg.ph.flags = dummy_flags;

    arg_t->chan_num                                                 = dummy_chan_num;
    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_recv_spurious;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);
    arg_t->history = &dummy_history;

    /* Act */
    CF_CFDP_S_DispatchRecv(arg_t, arg_fns);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious ==
                      (uint16)(initial_recv_spurious + 1),
                  "counter recv spurious set to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_recv_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_DC_INV,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_DC_INV)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_DC_INV);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end
     Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsEqTo_PDU_INVALID_MAX_SendEventAndIncrement_recv_spurious
   */

void Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsGreaterThan_PDU_INVALID_MAX_SendEventAndIncrement_recv_spurious(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_UT_inmsg_buffer_t dummy_msg;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t                 = &dummy_t;
    uint8                dummy_chan_num        = Any_cf_chan_num();
    uint8                dummy_flags           = Any_uint8_GreaterThan(CF_CFDP_FileDirective_INVALID_MAX);
    uint16               initial_recv_spurious = Any_uint16();
    const char          *expected_Spec = "CF S%d(%u:%u): received pdu with invalid directive code %d for sub-state %d";
    void                *arg_fns       = NULL;
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->state_data.s.sub_state = Any_uint8_LessThan(CF_TxSubState_NUM_STATES);

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_msg.pdu_r_msg.ph.flags = dummy_flags;

    arg_t->chan_num                                                 = dummy_chan_num;
    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_recv_spurious;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);
    arg_t->history = &dummy_history;

    /* Act */
    CF_CFDP_S_DispatchRecv(arg_t, arg_fns);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious ==
                      (uint16)(initial_recv_spurious + 1),
                  "counter recv spurious set to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_recv_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_DC_INV,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_DC_INV)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_DC_INV);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
} /* end
     Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsGreaterThan_PDU_INVALID_MAX_SendEventAndIncrement_recv_spurious
   */

void Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_Call_fns_sub_state_directive_code(
    void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t                 = &dummy_t;
    uint8                dummy_chan_num        = Any_cf_chan_num();
    uint8                dummy_sub_state       = 0; /* 0 = always choose Dummy_fns_CF_CFDP_S_DispatchRecv */
    uint8                dummy_flags           = 0; /* 0 = always choose Dummy_fns_CF_CFDP_S_DispatchRecv */
    uint16               initial_recv_spurious = Any_uint16();
    void (*const arg_fns[CF_TxSubState_NUM_STATES][CF_CFDP_FileDirective_INVALID_MAX])(CF_Transaction_t *,
                                                                                       const CF_CFDP_PduHeader_t *) = {
        {Dummy_fns_CF_CFDP_S_DispatchRecv, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
         NULL},                                                             /* CF_TxSubState_METADATA */
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, /* CF_TxSubState_FILEDATA */
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, /* CF_TxSubState_EOF */
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, /* CF_TxSubState_WAIT_FOR_EOF_ACK */
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, /* CF_TxSubState_WAIT_FOR_FIN */
        {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL}, /* CF_TxSubState_SEND_FIN_ACK */
    };

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_t->state_data.s.sub_state = dummy_sub_state;

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_msg.pdu_r_msg.ph.flags = dummy_flags;

    arg_t->chan_num                                                 = dummy_chan_num;
    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_recv_spurious;

    /* Act */
    CF_CFDP_S_DispatchRecv(arg_t, arg_fns);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_fns_CF_CFDP_S_DispatchRecv, 1);
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious == initial_recv_spurious,
                  "counter recv spurious set to %u and should be same as %u (unchanged, value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_recv_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end
     Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_Call_fns_sub_state_directive_code
   */

void Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_But_fns_NULL_DoNothing(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t                 = &dummy_t;
    uint8                dummy_chan_num        = Any_cf_chan_num();
    uint8                dummy_sub_state       = Any_uint8_LessThan(CF_TxSubState_NUM_STATES);
    uint8                dummy_flags           = Any_uint8_LessThan(CF_CFDP_FileDirective_INVALID_MAX);
    uint16               initial_recv_spurious = Any_uint16();
    void (*const arg_fns[CF_TxSubState_NUM_STATES][CF_CFDP_FileDirective_INVALID_MAX])(
        CF_Transaction_t *, const CF_CFDP_PduHeader_t *) = {{NULL}};

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->state_data.s.sub_state = dummy_sub_state;

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_msg.pdu_r_msg.ph.flags = dummy_flags;

    arg_t->chan_num                                                 = dummy_chan_num;
    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_recv_spurious;

    /* Act */
    CF_CFDP_S_DispatchRecv(arg_t, arg_fns);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious == initial_recv_spurious,
                  "counter recv spurious set to %u and should be same as %u (unchanged, value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_recv_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_But_fns_NULL_DoNothing */

/* end CF_CFDP_S_DispatchRecv tests */

/*******************************************************************************
**
**  CF_CFDP_S1_Recv tests (simple) - no coverage (broken) - [unstubbables: CF_CFDP_S_DispatchRecv]
**
*******************************************************************************/

void Test_CF_CFDP_S1_Recv_SendsAll_NULL_fns_To_CF_CFDP_S_DispatchRecv(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    /* Arrange for CF_CFDP_S_DispatchRecv */
    CF_UT_inmsg_buffer_t dummy_msg;
    uint8                dummy_chan_num        = Any_cf_chan_num();
    uint8                dummy_sub_state       = Any_uint8_LessThan(CF_TxSubState_NUM_STATES);
    uint8                dummy_flags           = Any_uint8_LessThan(CF_CFDP_FileDirective_INVALID_MAX);
    uint16               initial_recv_spurious = Any_uint16();

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->state_data.s.sub_state = dummy_sub_state;

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_msg.pdu_r_msg.ph.flags = dummy_flags;

    arg_t->chan_num                                                 = dummy_chan_num;
    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_recv_spurious;

    /* Act */
    CF_CFDP_S1_Recv(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_S_DispatchRecv */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious == initial_recv_spurious,
                  "counter recv spurious set to %u and should be same as %u (unchanged, value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_recv_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_S1_Recv_SendsAll_NULL_fns_To_CF_CFDP_S_DispatchRecv */

/* end CF_CFDP_S1_Recv tests */

/*******************************************************************************
**
**  CF_CFDP_S2_Recv tests (small) - full coverage - [unstubbables: CF_CFDP_S_DispatchRecv]
**  TODO:  Another example of how the lack of isolation and bad production code desing lead to unit testing nightmares
**         Currently (as of the writing of this comment), there is a double array created within the CF_CFDP_S2_Recv
**         method that is passed into CF_CFDP_S_DispatchRecv, an unstubbable method.  The size of the double
**         array is [CF_TxSubState_NUM_STATES][CF_CFDP_FileDirective_INVALID_MAX] and is [6][11] -- or 66 different
*possible values that may be
**         used in CF_CFDP_S_DispatchRecv.  Because it cannot be subbed, the only way to verify the array passed into
**         the method is to run 66 tests (!!!), using each array value and then to setup the test in the way that will
*run
**         the unstubbable function correctly to produce its flow to verify the value that came from the array!  This
**         creates 66 very large tests.  The current testing only provides coverage, with NO VERIFICATION of the array.
**         SOLUTION: make CF_CFDP_S_DispatchRecv stubbable
**                   make double array a constant that exists outside the method
**                   now there need be 1 test, that CF_CFDP_S1_Recv calls CF_CFDP_S_DispatchRecv
**                   with the expected t (a CF_Transaction_t*) and substate_fns (the double array)
**
*******************************************************************************/
/* TODO: CF_CFDP_S2_Recv tests should be verifying the array built in the method, but it cannot be done without an
 * extremely large amount of setup, thus there is 1 test that shows it calls CF_CFDP_S_DispatchRecv for coverage
 * purposes */
void Test_CF_CFDP_S2_Recv_Call_CF_CFDP_S_DispatchRecv(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    /* Arrange for CF_CFDP_S_DispatchRecv */
    CF_UT_inmsg_buffer_t dummy_msg;
    uint8                dummy_chan_num        = Any_cf_chan_num();
    uint8                dummy_sub_state       = 0; /* selects first row of array */
    uint8                dummy_flags           = 0; /* selects firs item of array which should be NULL */
    uint16               initial_recv_spurious = Any_uint16();

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->state_data.s.sub_state = dummy_sub_state;

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_msg.pdu_r_msg.ph.flags = dummy_flags;

    arg_t->chan_num                                                 = dummy_chan_num;
    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_recv_spurious;

    /* Act */
    CF_CFDP_S2_Recv(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_S_DispatchRecv */
    /* NOTE: the following 0 checks on method calls shows none of these that exist in the fns array were called */
    UtAssert_STUB_COUNT(CF_CFDP_S2_EarlyFin, 0);
    UtAssert_STUB_COUNT(CF_CFDP_S2_Nak, 0);
    UtAssert_STUB_COUNT(CF_CFDP_S2_Fin, 0);
    UtAssert_STUB_COUNT(CF_CFDP_S2_WaitForEofAck, 0);
    UtAssert_STUB_COUNT(CF_CFDP_S2_Nak_Arm, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious == initial_recv_spurious,
                  "counter recv spurious set to %u and should be same as %u (unchanged, value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_recv_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_S2_Recv_Call_CF_CFDP_S_DispatchRecv */

/* end CF_CFDP_S2_Recv tests */

/*******************************************************************************
**
**  CF_CFDP_S1_Tx tests (simple) - no coverage (broken) - [unstubbables: CF_CFDP_S_SubstateSendMetadata,
*CF_CFDP_S_SubstateSendFileData, CF_CFDP_S1_SubstateSendEof]
**  TODO: only 1 call is needed for coverage, but that does not fully test the behavior
*******************************************************************************/

void Test_CF_CFDP_S1_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 issue - t->state_data.s.sub_state<=CF_TxSubState_EOF");
} /* end Test_CF_CFDP_S1_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF */

void Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    arg_t->state_data.s.sub_state = 0;

    /* Arrange unstubbable: CF_CFDP_S_SubstateSendMetadata - no t->fd, send success */
    arg_t->fd = Any_int32_Except(0);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_SUCCESS);

    /* Act */
    CF_CFDP_S1_Tx(arg_t);

    /* Assert */
    /* Assert unstubbable: CF_CFDP_S_SubstateSendMetadata - check CF_CFDP_SendMd call */
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
} /* end Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata */

/* NOTE: Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData and
         Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S1_SubstateSendEof are commented
         out because they are not required for coverage.  These should be tested, but the setup
         is difficult and time consuming */

// void Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData(void)
// {
//     /* Arrange */
//     CF_Transaction_t   dummy_t;
//     CF_Transaction_t*  arg_t = &dummy_t;

//     arg_t->state_data.s.sub_state = 1;

//     /* Arrange unstubbable: CF_CFDP_S_SubstateSendFileData - callsCF_CFDP_S_SendFileData */
//     /* Arrange unstubbable:CF_CFDP_S_SendFileData -  pdu will be null err_out */
//     CF_CFDP_MsgOutGet_context_t   context_CF_CFDP_MsgOutGet;

//     context_CF_CFDP_MsgOutGet.forced_return = NULL;
//     UT_SetDataBuffer(UT_KEY(CF_CFDP_MsgOutGet), &context_CF_CFDP_MsgOutGet,
//       sizeof(context_CF_CFDP_MsgOutGet), false);

//     /* Act */
//     CF_CFDP_S1_Tx(arg_t);

//     /* Assert */
//     /* Assert unstubbable:CF_CFDP_S_SendFileData - check CF_CFDP_MsgOutGet call */
//     UtAssert_STUB_COUNT(CF_CFDP_MsgOutGet, 1);
// } /* end Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData */

// void Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S1_SubstateSendEof(void)
// {UtAssert_Failed("UPDATE FAIL: Uses new method CF_CFDP_EarlySendFd");
//     // /* Arrange */
//     // CF_Transaction_t   dummy_t;
//     // CF_Transaction_t*  arg_t = &dummy_t;

//     // arg_t->state_data.s.sub_state = 2;

//     // /* Arrange unstubbable: CF_CFDP_S1_SubstateSendEof - get back CF_SendRet_NO_MSG from CF_CFDP_S_SendEof*/
//     // /* Arrange unstubbable: CF_CFDP_S_SendEof -  bypass finalize just return CF_CFDP_SendEof */
//     // CF_CFDP_SendEof_context_t   context_CF_CFDP_SendEof;

//     // arg_t->flags.com.crc_calc = 1;

//     // context_CF_CFDP_SendEof.forced_return = Any_uint8_Except(CF_SendRet_NO_MSG);
//     // UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof,
//     //   sizeof(context_CF_CFDP_SendEof), false);

//     // /* Arrange unstubbable: CF_CFDP_S_Reset - confirm CF_CFDP_ResetTransaction call*/
//     // CF_CFDP_ResetTransaction_context_t   context_CF_CFDP_ResetTransaction;

//     // UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
//     //   sizeof(context_CF_CFDP_ResetTransaction), false);

//     // /* Act */
//     // CF_CFDP_S1_Tx(arg_t);

//     // /* Assert */
//     // /* Assert unstubbable: CF_CFDP_S_Reset - check CF_CFDP_SendMd call */
//     // UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
// } /* end Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S1_SubstateSendEof */

/* end CF_CFDP_S1_Tx tests */

/*******************************************************************************
**
**  CF_CFDP_S2_Tx tests (simple) - full coverage - [unstubbables: CF_CFDP_S_SubstateSendMetadata,
*CF_CFDP_S2_SubstateSendFileData, CF_CFDP_S2_SubstateSendEof]
**  TODO: only 1 call is needed for coverage, but that does not fully test the behavior
*******************************************************************************/

void Test_CF_CFDP_S2_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 issue");
} /* end Test_CF_CFDP_S2_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF */

void Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    arg_t->state_data.s.sub_state = 0;

    /* Arrange unstubbable: CF_CFDP_S_SubstateSendMetadata - no t->fd, send success */
    arg_t->fd = Any_int32_Except(0);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_SUCCESS);

    /* Act */
    CF_CFDP_S2_Tx(arg_t);

    /* Assert */
    /* Assert unstubbable: CF_CFDP_S_SubstateSendMetadata - check CF_CFDP_SendMd call */
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
} /* end Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata */

/* NOTE: Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData and
         Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S2_SubstateSendEof are commented
         out because they are not required for coverage.  These should be tested, but the setup
         is difficult and time consuming */

// void Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData(void)
// {
//     /* Arrange */
//     CF_Transaction_t   dummy_t;
//     CF_Transaction_t*  arg_t = &dummy_t;

//     arg_t->state_data.s.sub_state = 1;

//     /* Arrange unstubbable: CF_CFDP_S_SubstateSendFileData - callsCF_CFDP_S_SendFileData */
//     /* Arrange unstubbable:CF_CFDP_S_SendFileData -  pdu will be null err_out */
//     CF_CFDP_MsgOutGet_context_t   context_CF_CFDP_MsgOutGet;

//     context_CF_CFDP_MsgOutGet.forced_return = NULL;
//     UT_SetDataBuffer(UT_KEY(CF_CFDP_MsgOutGet), &context_CF_CFDP_MsgOutGet,
//       sizeof(context_CF_CFDP_MsgOutGet), false);

//     /* Act */
//     CF_CFDP_S2_Tx(arg_t);

//     /* Assert */
//     /* Assert unstubbable:CF_CFDP_S_SendFileData - check CF_CFDP_MsgOutGet call */
//     UtAssert_STUB_COUNT(CF_CFDP_MsgOutGet, 1);
// } /* end Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData */

// void Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S2_SubstateSendEof(void)
// {
//     /* Arrange */
//     CF_Transaction_t   dummy_t;
//     CF_Transaction_t*  arg_t = &dummy_t;

//     arg_t->state_data.s.sub_state = 2;

//     /* Arrange unstubbable: CF_CFDP_S2_SubstateSendEof - get back CF_SendRet_NO_MSG from CF_CFDP_S_SendEof*/
//     /* Arrange unstubbable: CF_CFDP_S_SendEof -  bypass finalize just return CF_CFDP_SendEof */
//     CF_CFDP_SendEof_context_t   context_CF_CFDP_SendEof;

//     arg_t->flags.com.crc_calc = 1;

//     context_CF_CFDP_SendEof.forced_return = Any_uint8_Except(CF_SendRet_NO_MSG);

//     /* Arrange unstubbable: CF_CFDP_S_Reset - confirm CF_CFDP_ResetTransaction call*/
//     CF_CFDP_ResetTransaction_context_t   context_CF_CFDP_ResetTransaction;

//     UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
//       sizeof(context_CF_CFDP_ResetTransaction), false);

//     /* Act */
//     CF_CFDP_S2_Tx(arg_t);

//     /* Assert */
//     /* Assert unstubbable: CF_CFDP_S_Reset - check CF_CFDP_SendMd call */
//     UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
// } /* end Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S2_SubstateSendEof */

/* end CF_CFDP_S2_Tx tests */

/*******************************************************************************
**
**  CF_CFDP_S_Cancel tests (simple) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_S_Cancel_When_t_sub_state_IsLessThan_SEND_EOF_SetTo_SEND_EOF(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint8             initial_sub_state = Any_uint8_LessThan(CF_TxSubState_EOF);

    arg_t->state_data.s.sub_state = initial_sub_state;

    /* Act */
    CF_CFDP_S_Cancel(arg_t);

    /* Assert */
    UtAssert_True(
        arg_t->state_data.s.sub_state == CF_TxSubState_EOF,
        "t->state_data.s.sub_state is %u and was changed to %u (CF_TxSubState_EOF) from %u (value before call)",
        arg_t->state_data.s.sub_state, CF_TxSubState_EOF, initial_sub_state);
} /* end Test_CF_CFDP_S_Cancel_When_t_sub_state_IsLessThan_SEND_EOF_SetTo_SEND_EOF */

void Test_CF_CFDP_S_Cancel_When_t_sub_state_IsEqTo_SEND_EOF_DoNothing(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;

    arg_t->state_data.s.sub_state = CF_TxSubState_EOF;

    /* Act */
    CF_CFDP_S_Cancel(arg_t);

    /* Assert */
    UtAssert_True(arg_t->state_data.s.sub_state == CF_TxSubState_EOF,
                  "t->state_data.s.sub_state is %u and is unchanged from %u (CF_TxSubState_EOF - value before call)",
                  arg_t->state_data.s.sub_state, CF_TxSubState_EOF);
} /* end Test_CF_CFDP_S_Cancel_When_t_sub_state_IsEqTo_SEND_EOF_DoNothing */

void Test_CF_CFDP_S_Cancel_When_t_sub_state_IsGreaterThan_SEND_EOF_DoNothing(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t             = &dummy_t;
    uint8             initial_sub_state = Any_uint8_GreaterThan(CF_TxSubState_EOF);

    arg_t->state_data.s.sub_state = initial_sub_state;

    /* Act */
    CF_CFDP_S_Cancel(arg_t);

    /* Assert */
    UtAssert_True(arg_t->state_data.s.sub_state == initial_sub_state,
                  "t->state_data.s.sub_state is %u and is unchanged from %u (value before call)",
                  arg_t->state_data.s.sub_state, initial_sub_state);
} /* end Test_CF_CFDP_S_Cancel_When_t_sub_state_IsGreaterThan_SEND_EOF_DoNothing */

/* end CF_CFDP_S_Cancel tests */

/*******************************************************************************
**
**  CF_CFDP_S_Tick tests (large) - full coverage - [unstubbables: CF_CFDP_S_Reset, CF_CFDP_S_SendEof,
*CF_CFDP_S_CheckAndRespondNak, CF_CFDP_S_SubstateSendFinAck]
**
*******************************************************************************/

void Test_CF_CFDP_S_Tick_When_t_state_IsNotEqTo_CFDP_S2_DoNothing(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t    = &dummy_t;
    int              *arg_cont = NULL;

    arg_t->state = Any_uint8_Except(CF_TxnState_S2);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
} /* end Test_CF_CFDP_S_Tick_When_t_state_IsNotEqTo_CFDP_S2_DoNothing */

void Test_CF_CFDP_S_Tick_CallTo_CF_TimerExpired_Returns_1_ThenSendsEventAndCalls_CF_CFDP_S_Reset_AndIncrements_inactivity_timer(
    void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t                    = &dummy_t;
    int                        *arg_cont                 = NULL;
    uint16                      initial_inactivity_timer = Any_uint16();
    cf_timer_t                 *context_CF_Timer_Expired;
    const char                 *expected_Spec = "CF S2(%u:%u): inactivity timer expired";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();
    arg_t->state    = CF_TxnState_S2;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), &context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;

    /* Arrange unstubbable: CF_CFDP_S_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired, &arg_t->inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_INACT_TIMER,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_INACT_TIMER)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_INACT_TIMER);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer ==
                      (uint16)(initial_inactivity_timer + 1),
                  "fault.inactivity_timer was incremented to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received keep_history %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history == 1);
} /* end
     Test_CF_CFDP_S_Tick_CallTo_CF_TimerExpired_Returns_1_ThenSendsEventAndCalls_CF_CFDP_S_Reset_AndIncrements_inactivity_timer
   */

void Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_EqTo_ack_limit_SendEventIncrement_ack_limit_ThenCalls_CF_CFDP_S_Reset(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    uint8             dummy_ack_limit          = Any_uint8();
    cf_timer_t       *context_CF_Timer_Expired[2];
    cf_timer_t       *context_CF_Timer_Tick;
    const char       *expected_Spec = "CF S2(%u:%u), ack limit reached, no eof-ack";

    arg_t->history                   = &dummy_history;
    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 1;
    arg_t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;

    arg_t->state_data.s.s2.counter.ack = dummy_ack_limit - 1;

    CF_AppData.config_table                                                   = &dummy_config_table;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;
    CF_AppData.config_table->ack_limit                                        = dummy_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), FIRST_CALL, 0);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), &context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CFDP_S_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 2);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[0], &arg_t->inactivity_timer);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[1], &arg_t->ack_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_S_ACK_LIMIT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_S_ACK_LIMIT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_S_ACK_LIMIT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit ==
                      (uint16)(initial_fault_ack_limit + 1),
                  "fault.ack_limit was incremented to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick, &arg_t->inactivity_timer);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received keep_history %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history == 1);
} /* end
     Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_EqTo_ack_limit_SendEventIncrement_ack_limit_ThenCalls_CF_CFDP_S_Reset
   */

void Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_Return_CF_SEND_NO_MSG_CausesErrOut(
    void)
{
    /* Arrange */

    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    uint8             dummy_ack_limit          = Any_uint8();
    cf_timer_t       *context_CF_Timer_Expired[2];
    cf_timer_t       *context_CF_Timer_Tick;

    arg_t->history                   = &dummy_history;
    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 1;
    arg_t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;

    arg_t->state_data.s.s2.counter.ack = Any_uint8_Except(dummy_ack_limit - 1);

    CF_AppData.config_table                                                   = &dummy_config_table;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;
    CF_AppData.config_table->ack_limit                                        = dummy_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), FIRST_CALL, 0);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), &context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    /* Arrange unstubbable: CF_CFDP_S_SendEof */
    CF_CFDP_SendEof_context_t context_CF_CFDP_SendEof;

    arg_t->flags.com.crc_calc = 1;

    context_CF_CFDP_SendEof.forced_return = CF_SendRet_NO_MSG;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof, sizeof(context_CF_CFDP_SendEof), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 2);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[0], &arg_t->inactivity_timer);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[1], &arg_t->ack_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == initial_fault_ack_limit,
                  "fault.ack_limit was incremented to %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick, &arg_t->inactivity_timer);
    /* Assert CF_CFDP_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendEof.t, arg_t);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_Return_CF_SEND_NO_MSG_CausesErrOut
   */

void Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_Returns_CF_SEND_ERROR_Call_CF_CFDP_S_Reset_AndErrOut(
    void)
{
    /* Arrange */

    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    uint8             dummy_ack_limit          = Any_uint8();
    cf_timer_t       *context_CF_Timer_Expired[2];
    cf_timer_t       *context_CF_Timer_Tick;

    arg_t->history                   = &dummy_history;
    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 1;
    arg_t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;

    arg_t->state_data.s.s2.counter.ack = Any_uint8_Except(dummy_ack_limit - 1);

    CF_AppData.config_table                                                   = &dummy_config_table;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;
    CF_AppData.config_table->ack_limit                                        = dummy_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), FIRST_CALL, 0);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), &context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    /* Arrange unstubbable: CF_CFDP_S_SendEof */
    CF_CFDP_SendEof_context_t context_CF_CFDP_SendEof;

    arg_t->flags.com.crc_calc = 1;

    context_CF_CFDP_SendEof.forced_return = CF_SendRet_ERROR;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof, sizeof(context_CF_CFDP_SendEof), false);

    /* Arrange unstubbable: CF_CFDP_S_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 2);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[0], &arg_t->inactivity_timer);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[1], &arg_t->ack_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == initial_fault_ack_limit,
                  "fault.ack_limit was incremented to %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick, &arg_t->inactivity_timer);
    /* Assert CF_CFDP_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendEof.t, arg_t);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received keep_history %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history == 1);
} /* end
     Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_Returns_CF_SEND_ERROR_Call_CF_CFDP_S_Reset_AndErrOut
   */

void Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_ReturnsAnythingExcept_CF_SEND_NO_MSG_Or_CF_SEND_ERROR_Call_CF_CFDP_ArmAckTimer(
    void)
{
    /* Arrange */

    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    uint8             dummy_ack_limit          = Any_uint8();
    cf_timer_t       *context_CF_Timer_Expired[2];
    cf_timer_t       *context_CF_Timer_Tick;
    CF_Transaction_t *context_CF_CFDP_ArmAckTimer;

    arg_t->history                   = &dummy_history;
    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 1;
    arg_t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;

    arg_t->state_data.s.s2.counter.ack = Any_uint8_Except(dummy_ack_limit - 1);

    CF_AppData.config_table                                                   = &dummy_config_table;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;
    CF_AppData.config_table->ack_limit                                        = dummy_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), FIRST_CALL, 0);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), &context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ArmAckTimer), &context_CF_CFDP_ArmAckTimer, sizeof(context_CF_CFDP_ArmAckTimer),
                     false);

    /* Arrange unstubbable: CF_CFDP_S_SendEof */
    CF_SendRet_t              exceptions[2] = {CF_SendRet_NO_MSG, CF_SendRet_ERROR};
    CF_CFDP_SendEof_context_t context_CF_CFDP_SendEof;

    arg_t->flags.com.crc_calc = 1;

    context_CF_CFDP_SendEof.forced_return = Any_cfdp_send_ret_t_ExceptThese(exceptions, 2);

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendEof), &context_CF_CFDP_SendEof, sizeof(context_CF_CFDP_SendEof), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 2);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[0], &arg_t->inactivity_timer);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[1], &arg_t->ack_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == initial_fault_ack_limit,
                  "fault.ack_limit was incremented to %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ArmAckTimer, arg_t);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick, &arg_t->inactivity_timer);
    /* Assert CF_CFDP_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendEof.t, arg_t);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_ReturnsAnythingExcept_CF_SEND_NO_MSG_Or_CF_SEND_ERROR_Call_CF_CFDP_ArmAckTimer
   */

void Test_CF_CFDP_S_Tick_ArmedTimerExpired_sub_state_NotEqTo_SEND_WAIT_FOR_EOF_ACK_DoNotArmAckOrTick(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    uint8             exceptions[2]            = {
        CF_TxSubState_WAIT_FOR_EOF_ACK,
        CF_TxSubState_SEND_FIN_ACK}; // CF_TxSubState_SEND_FIN_ACK only excepted for separate behavior 'if' block
    cf_timer_t *context_CF_Timer_Expired[2];
    cf_timer_t *context_CF_Timer_Tick;

    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 1;
    arg_t->state_data.s.sub_state    = Any_uint8_ExceptThese(exceptions, 2);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), FIRST_CALL, 0);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), &context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 2);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[0], &arg_t->inactivity_timer);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[1], &arg_t->ack_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == initial_fault_ack_limit,
                  "fault.ack_limit was incremented to %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick, &arg_t->inactivity_timer);
    /* Assert CF_CFDP_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 0);
    /* Assert CF_CFDP_S_SubstateSendFinAck */
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 0);
} /* end Test_CF_CFDP_S_Tick_ArmedTimerExpired_sub_state_NotEqTo_SEND_WAIT_FOR_EOF_ACK_DoNotArmAckOrTick */

void Test_CF_CFDP_S_Tick_ArmedTimerNotExpiredCall_CF_Timer_Tick(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    cf_timer_t       *context_CF_Timer_Expired[2];
    cf_timer_t       *context_CF_Timer_Tick[2];

    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 1;
    arg_t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), FIRST_CALL, 0);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 0);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 2);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[0], &arg_t->inactivity_timer);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired[1], &arg_t->ack_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == initial_fault_ack_limit,
                  "fault.ack_limit was incremented to %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 2);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick[0], &arg_t->inactivity_timer);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick[1], &arg_t->ack_timer);
    /* Assert CF_CFDP_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 0);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end Test_CF_CFDP_S_Tick_ArmedTimerNotExpiredCall_CF_Timer_Tick */

void Test_CF_CFDP_S_Tick_TimerNotArmedDoNotArmAckTimerOrDoTick(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    cf_timer_t       *context_CF_Timer_Expired;
    cf_timer_t       *context_CF_Timer_Tick;

    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 0;
    arg_t->state_data.s.sub_state    = Any_uint8_Except(CF_TxSubState_SEND_FIN_ACK);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), &context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 0);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), &context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired, &arg_t->inactivity_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == initial_fault_ack_limit,
                  "fault.ack_limit was incremented to %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick, &arg_t->inactivity_timer);
    /* Assert CF_CFDP_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 0);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end Test_CF_CFDP_S_Tick_TimerNotArmedDoNotArmAckTimerOrDoTick */

void Test_CF_CFDP_S_Tick_When_sub_state_IsEqTo_SEND_SEND_FIN_ACK_Call_CF_CFDP_S_SubstateSendFinAck(void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                    = &dummy_t;
    int              *arg_cont                 = NULL;
    uint16            initial_inactivity_timer = Any_uint16();
    uint16            initial_fault_ack_limit  = Any_uint16();
    cf_timer_t       *context_CF_Timer_Expired;
    cf_timer_t       *context_CF_Timer_Tick;

    arg_t->history                   = &dummy_history;
    arg_t->chan_num                  = Any_cf_chan_num();
    arg_t->state                     = CF_TxnState_S2;
    arg_t->flags.com.ack_timer_armed = 0;
    arg_t->state_data.s.sub_state    = CF_TxSubState_SEND_FIN_ACK;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit        = initial_fault_ack_limit;

    UT_SetDataBuffer(UT_KEY(CF_Timer_Expired), &context_CF_Timer_Expired, sizeof(context_CF_Timer_Expired), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 0);

    UT_SetDataBuffer(UT_KEY(CF_Timer_Tick), &context_CF_Timer_Tick, sizeof(context_CF_Timer_Tick), false);

    /* Arrange unstubbable: CF_CFDP_S_SubstateSendFinAck */
    CF_CFDP_SendAck_context_t context_CF_CFDP_SendAck;

    context_CF_CFDP_SendAck.forced_return = CF_SendRet_NO_MSG;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendAck), &context_CF_CFDP_SendAck, sizeof(context_CF_CFDP_SendAck), false);

    /* Act */
    CF_CFDP_S_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Expired, &arg_t->inactivity_timer);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer == initial_inactivity_timer,
                  "fault.inactivity_timer is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == initial_fault_ack_limit,
                  "fault.ack_limit was incremented to %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, initial_fault_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_Tick, &arg_t->inactivity_timer);
    /* Assert CF_CFDP_SendEof */
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 0);
    /* Assert CF_CFDP_S_SubstateSendFinAck */
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_SendAck.t, arg_t);
    UtAssert_True(context_CF_CFDP_SendAck.ts == CF_CFDP_AckTxnStatus_ACTIVE,
                  "CF_CFDP_SendAck received ts %u and should be %u (CF_CFDP_AckTxnStatus_ACTIVE)",
                  context_CF_CFDP_SendAck.ts, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_True(context_CF_CFDP_SendAck.dir_code == CF_CFDP_FileDirective_FIN,
                  "CF_CFDP_SendAck received dir_code %u and should be %u (CF_CFDP_FileDirective_FIN)",
                  context_CF_CFDP_SendAck.dir_code, CF_CFDP_FileDirective_FIN);
    UtAssert_True(context_CF_CFDP_SendAck.cc == arg_t->state_data.s.s2.fin_cc,
                  "CF_CFDP_SendAck received cc %u and should be %u (t->state_data.s.s2.fin_cc)",
                  context_CF_CFDP_SendAck.cc, arg_t->state_data.s.s2.fin_cc);
    UtAssert_True(context_CF_CFDP_SendAck.peer_eid == arg_t->history->peer_eid,
                  "CF_CFDP_SendAck received peer_eid %u and should be %u (t->history->peer_eid)",
                  context_CF_CFDP_SendAck.peer_eid, arg_t->history->peer_eid);
    UtAssert_True(context_CF_CFDP_SendAck.tsn == arg_t->history->seq_num,
                  "CF_CFDP_SendAck received tsn %u and should be %u (t->history->seq_num)", context_CF_CFDP_SendAck.tsn,
                  arg_t->history->seq_num);
    /* Assert CF_CFDP_S_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end Test_CF_CFDP_S_Tick_When_sub_state_IsEqTo_SEND_SEND_FIN_ACK_Call_CF_CFDP_S_SubstateSendFinAck */

/* end CF_CFDP_S_Tick tests */

/*******************************************************************************
**
**  CF_CFDP_S_Tick_Nak tests (small) - full coverage [unstubbables: CF_CFDP_S_CheckAndRespondNak (large)]
**
*******************************************************************************/

void Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_DoesNotReturn_1_Given_cont_ValueNotSet(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t        = &dummy_t;
    int               initial_cont = Any_int_Except(
                      1); /* Any_int_Except(1) is not required, but illustrative of the fact that 1 is the value that will be set. The
                             chance of 1 being selected randomly from all possible 'int's is practically nil. */
    int  dummy_cont = initial_cont;
    int *arg_cont   = &dummy_cont;

    /* Arrange unstubbable: CF_CFDP_S_CheckAndRespondNak */
    arg_t->flags.tx.md_need_send = 1; /* To enter first if */

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), CF_SendRet_ERROR); /* to err out */

    /* Act */
    CF_CFDP_S_Tick_Nak(arg_t, arg_cont);

    /* Assert */
    UtAssert_True(dummy_cont == initial_cont,
                  "After call to CF_CFDP_S_Tick_Nak value at cont is %d and should be %d (value unchanged)", dummy_cont,
                  initial_cont);
} /* end Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_DoesNotReturn_1_Given_cont_ValueNotSet */

void Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_1_Given_cont_ValueSetTo_1(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t        = &dummy_t;
    int               initial_cont = Any_int_Except(
                      1); /* Any_int_Except(1) is not required, but illustrative of the fact that 1 is the value that will be set. The
                             chance of 1 being selected randomly from all possible 'int's is practically nil. */
    int  dummy_cont = initial_cont;
    int *arg_cont   = &dummy_cont;

    /* Arrange unstubbable: CF_CFDP_S_CheckAndRespondNak */
    arg_t->flags.tx.md_need_send = 1; /* To enter first if */

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendMd), Any_uint8_Except(CF_SendRet_ERROR)); /* to NOT err out */

    /* Act */
    CF_CFDP_S_Tick_Nak(arg_t, arg_cont);

    /* Assert */
    UtAssert_True(dummy_cont == 1,
                  "After call to CF_CFDP_S_Tick_Nak value at cont is %d and should be 1 (set in call, before call %d)",
                  dummy_cont, initial_cont);
} /* end Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_1_Given_cont_ValueSetTo_1 */

/* end CF_CFDP_S_Tick_Nak tests */

/*******************************************************************************
**
**  cf_cfdp_s_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CFDP_S_Reset_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S_ResetCall_CF_CFDP_ResetTransaction_with_keep_history, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_ResetCall_CF_CFDP_ResetTransaction_with_keep_history");
} /* end add_CF_CFDP_S_Reset_tests */

void add_CFDP_S_SendEof_tests(void)
{
    UtTest_Add(
        Test_CFDP_S_SendEof_When_flag_tx_crc_calc_Is_0_Call_CF_CRC_Finalize_AndSet_crc_calc_To_1_ReturnValueOfCallTo_CF_CFDP_SendEof,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CFDP_S_SendEof_When_flag_tx_crc_calc_Is_0_Call_CF_CRC_Finalize_AndSet_crc_calc_To_1_ReturnValueOfCallTo_"
        "CF_CFDP_SendEof");
    UtTest_Add(Test_CFDP_S_SendEof_When_crc_calc_Is_1_DoNotCall_CF_CRC_Finalize_ReturnValueOfCallTo_CF_CFDP_SendEof,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CFDP_S_SendEof_When_crc_calc_Is_1_DoNotCall_CF_CRC_Finalize_ReturnValueOfCallTo_CF_CFDP_SendEof");
} /* end add_CFDP_S_SendEof_tests */

void add_CF_CFDP_S1_SubstateSendEof_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S1_SubstateSendEof_When_S_SendEof_Is_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_Reset,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S1_SubstateSendEof_When_S_SendEof_Is_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_Reset");
    UtTest_Add(Test_CF_CFDP_S1_SubstateSendEof_Call_CF_CFDP_S_Reset_With_t_When_CFDP_S_SendEof_IsNot_CF_SEND_NO_MSG,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S1_SubstateSendEof_Call_CF_CFDP_S_Reset_With_t_When_CFDP_S_SendEof_IsNot_CF_SEND_NO_MSG");
} /* end add_CF_CFDP_S1_SubstateSendEof_tests */

void add_CF_CFDP_S2_SubstateSendEof_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_SubstateSendEof_TriggerTickProcessing, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_SubstateSendEof_TriggerTickProcessing");
} /* end add_CF_CFDP_S2_SubstateSendEof_tests */

void add_CFDP_S_SendFileData_tests(void)
{
    UtTest_Add(Test_CFDP_S_SendFileData_When_ph_Is_NULL_Return_0, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CFDP_S_SendFileData_When_ph_Is_NULL_Return_0");
    UtTest_Add(
        Test_CFDP_S_SendFileData_WhenCachedPosNotEq_foffs_CallTo_CF_WrappedLseek_Returns_status_NotEqTo_foffs_SendEventReturn_neg1,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CFDP_S_SendFileData_WhenCachedPosNotEq_foffs_CallTo_CF_WrappedLseek_Returns_status_NotEqTo_foffs_"
        "SendEventReturn_neg1");
    UtTest_Add(
        Test_CFDP_S_SendFileData_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CFDP_S_SendFileData_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1");
    UtTest_Add(
        Test_CFDP_S_SendFileData_Given_bytes_to_read_GreaterThan_outgoing_file_chunk_size_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CFDP_S_SendFileData_Given_bytes_to_read_GreaterThan_outgoing_file_chunk_size_CallTo_CF_WrappedRead_"
        "ReturnsValueNotEqTo_bytes_to_read_SendEventAndReturn_neg1");
    UtTest_Add(Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_NO_MSG_Return_0, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown,
               "Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_NO_MSG_Return_0");
    UtTest_Add(Test_CFDP_S_SendFileData_AssertsWhen_foffs_Plus_bytes_to_read_IsLessThanOrEqTo_t_fsize,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CFDP_S_SendFileData_AssertsWhen_foffs_Plus_bytes_to_read_IsLessThanOrEqTo_t_fsize");
    UtTest_Add(Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_ERROR_SendEventAndReturn_neg1,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_Returns_CF_SEND_ERROR_SendEventAndReturn_neg1");
    UtTest_Add(
        Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareNoCallTo_CF_CRC_Digest_ReturnBytesToRead,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareNoCallTo_CF_CRC_Digest_ReturnBytesToRead");
    UtTest_Add(
        Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareWithCallTo_CF_CRC_Digest_ReturnBytesToRead,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CFDP_S_SendFileData_CallTo_CF_CFDP_SendFd_ReturnsDontCareWithCallTo_CF_CRC_Digest_ReturnBytesToRead");
} /* end add_CFDP_S_SendFileData_tests */

void add_CF_CFDP_S_SubstateSendFileData_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_Returns_0_DontCareSoDoNothing,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_Returns_0_DontCareSoDoNothing");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_NotEq_t_fsize_DoNotSet_sub_state,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_NotEq_t_"
        "fsize_DoNotSet_sub_state");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_IsEqTo_t_fsize_Set_sub_state_To_SEND_EOF,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_Update_t_foffs_IsEqTo_t_"
        "fsize_Set_sub_state_To_SEND_EOF");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsLessThan_0_Set_cc_To_CC_FILESTORE_REJECTION_And_sub_state_To_SEND_EOF,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendFileData_CallTo_CFDP_S_SendFileData_ReturnsLessThan_0_Set_cc_To_CC_FILESTORE_"
        "REJECTION_And_sub_state_To_SEND_EOF");
} /* end add_CF_CFDP_S_SubstateSendFileData_tests */

void add_CF_CFDP_S_CheckAndRespondNak_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S_CheckAndRespondNak_FailsBecause_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_Return_neg1,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_CheckAndRespondNak_FailsBecause_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_Return_neg1");
    UtTest_Add(Test_CF_CFDP_S_CheckAndRespondNak_SuccessBecause_CF_CFDP_SendMd_Returns_CF_SEND_SUCCESS_Return_1,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_CheckAndRespondNak_SuccessBecause_CF_CFDP_SendMd_Returns_CF_SEND_SUCCESS_Return_1");
    UtTest_Add(
        Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_CFDP_SendMd_DoesNotReturn_CF_SEND_ERROR_Or_CF_SEND_SUCCESS_Return_1,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_CFDP_SendMd_DoesNotReturn_CF_SEND_ERROR_Or_CF_SEND_"
        "SUCCESS_Return_1");
    UtTest_Add(Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_Chunks_GetFirstChunk_Returns_NULL_Return_0,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_CheckAndRespondNak_DoesNothingBecause_CF_Chunks_GetFirstChunk_Returns_NULL_Return_0");
    UtTest_Add(
        Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_Returns_0_ThenReturn_0,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_"
        "Returns_0_ThenReturn_0");
    UtTest_Add(
        Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_ReturnsLessThan_0_ThenReturn_neg1,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturns_nonNULL_CallTo_CFDP_S_SendFileData_"
        "ReturnsLessThan_0_ThenReturn_neg1");
    UtTest_Add(
        Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturned_nonNULL_CallTo_CFDP_S_SendFileData_ReturnsGreaterThan_0_ThenReturn_1,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_CheckAndRespondNak_CallTo_CF_Chunks_GetFirstChunkReturned_nonNULL_CallTo_CFDP_S_SendFileData_"
        "ReturnsGreaterThan_0_ThenReturn_1");
} /* end add_CF_CFDP_S_CheckAndRespondNak_tests */

void add_CF_CFDP_S2_SubstateSendFileData_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_0_Call_CF_CFDP_S_SubstateSendFileData,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_0_Call_CF_CFDP_S_"
        "SubstateSendFileData");
    UtTest_Add(
        Test_CF_CFDP_S2_SubstateSendFileData_CF_CFDP_S_CheckAndRespondNak_ReturnsNegativeValue_Call_CF_CFDP_S_Reset,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S2_SubstateSendFileData_CF_CFDP_S_CheckAndRespondNak_ReturnsNegativeValue_Call_CF_CFDP_S_Reset");
    UtTest_Add(
        Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_ReturnsPositiveValueDoNothing,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S2_SubstateSendFileData_CallTo_CF_CFDP_S_CheckAndRespondNak_ReturnsPositiveValueDoNothing");
} /* end add_CF_CFDP_S2_SubstateSendFileData_tests */

void add_CF_CFDP_S_SubstateSendMetadata_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_CallTo_OS_FileOpenCheck_Returns_OS_SUCCESS_Increment_fault_file_open,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_CallTo_OS_FileOpenCheck_Returns_OS_SUCCESS_Increment_fault_file_open");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_CallTo_CF_WrappedOpen_Makes_fd_LessThan_0_Increment_fault_file_open,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_CallTo_CF_WrappedOpen_Makes_fd_LessThan_0_Increment_fault_file_open");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_FirstCallTo_CF_WrappedLseek_Sets_status_LessThan_0_Increment_fault_file_seek,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_FirstCallTo_CF_WrappedLseek_Sets_status_LessThan_0_Increment_fault_file_"
        "seek");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_SecondCallTo_CF_WrappedLseek_Sets_status_ToNot_0_Increment_fault_file_seek,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_SecondCallTo_CF_WrappedLseek_Sets_status_ToNot_0_Increment_fault_file_"
        "seek");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_AllFileChecksPassCallTo_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_ThenErrorOut,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_AllFileChecksPassCallTo_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_"
        "ThenErrorOut");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_ThenErrorOut,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_"
        "ERROR_ThenErrorOut");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_SUCCESS_Then_sub_state_SetTo_SEND_FILE_DATA,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_"
        "SUCCESS_Then_sub_state_SetTo_SEND_FILE_DATA");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_NO_MSG_DoNotSet_sub_state,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendMetadata_BypassedFileChecks_t_fd_IsNot_0_CallTo_CF_CFDP_SendMd_Returns_CF_SEND_NO_"
        "MSG_DoNotSet_sub_state");
} /* end add_CF_CFDP_S_SubstateSendMetadata_tests */

void add_CF_CFDP_S_SubstateSendFinAck_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_Returns_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_Reset_,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_Returns_CF_SEND_NO_MSG_DoNotCall_CF_CFDP_S_"
        "Reset_");
    UtTest_Add(
        Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_DoesNotReturn_CF_SEND_NO_MSG_Call_CF_CFDP_S_Reset_With_t_AsArgument,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_SubstateSendFinAck_WhenCallTo_CF_CFDP_SendAck_DoesNotReturn_CF_SEND_NO_MSG_Call_CF_CFDP_S_"
        "Reset_With_t_AsArgument");
} /* end add_CF_CFDP_S_SubstateSendFinAck_tests */

void add_CF_CFDP_S2_EarlyFin_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_EarlyFin_SendEventAndCallReset, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_EarlyFin_SendEventAndCallReset");
} /* end add_CF_CFDP_S2_EarlyFin_tests */

void add_CF_CFDP_S2_Fin_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_Returns_0_Set_fin_cc_And_sub_state, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_Returns_0_Set_fin_cc_And_sub_state");
    UtTest_Add(Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_DoesNotReturn_0_SendEventAndCountReceiveError,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Fin_When_CF_CFDP_RecvFin_DoesNotReturn_0_SendEventAndCountReceiveError");
} /* end add_CF_CFDP_S2_Fin_tests */

void add_CF_CFDP_S2_Nak_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_neg1_SendEventAndIncrement_recv_error,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_neg1_SendEventAndIncrement_recv_error");
    UtTest_Add(Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_0_Set_num_sr_to_0_SendEventAndIncrement_recv_error,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_RecvNak_Returns_0_Set_num_sr_to_0_SendEventAndIncrement_recv_error");
    UtTest_Add(Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Asserts_num_sr_IsGreaterThan_CF_NAK_MAX_SEGMENTS,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Asserts_num_sr_IsGreaterThan_CF_NAK_MAX_SEGMENTS");
    UtTest_Add(
        Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Sets_md_need_send_To_1_When_offset_start_And_offset_end_Are_0_Add_num_sr_To_nak_segment_requests,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S2_Nak_CallTo_CF_CFDP_Sets_md_need_send_To_1_When_offset_start_And_offset_end_Are_0_Add_num_sr_"
        "To_nak_segment_requests");
    UtTest_Add(Test_CF_CFDP_S2_Nak_SendsEventBecause_offset_end_IsLessThan_offset_start, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S2_Nak_SendsEventBecause_offset_end_IsLessThan_offset_start");
    UtTest_Add(Test_CF_CFDP_S2_Nak_SendsEventBecause_start_Plus_size_IsGreaterThanTransaction_fsize,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Nak_SendsEventBecause_start_Plus_size_IsGreaterThanTransaction_fsize");
    UtTest_Add(Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsEqualToTransaction_fsize,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsEqualToTransaction_fsize");
    UtTest_Add(Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsLessThanTransaction_fsize,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Nak_Calls_CF_Chunks_Add_Because_start_Plus_size_IsLessThanTransaction_fsize");
} /* end add_CF_CFDP_S2_Nak_tests */

void add_CF_CFDP_S2_Nak_Arm_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_Nak_Arm_Call_CF_CFDP_ArmAckTimer_And_CF_CFDP_S2_Nak, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S2_Nak_Arm_Call_CF_CFDP_ArmAckTimer_And_CF_CFDP_S2_Nak");
} /* end add_CF_CFDP_S2_Nak_Arm_tests */

void add_CF_CFDP_S2_WaitForEofAck_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_neg1_SendEventAndIncrement_recv_error,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_neg1_SendEventAndIncrement_recv_error");
    UtTest_Add(
        Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_EqTo_CC_NO_ERROR_Set_sub_state_To_SEND_WAIT_FOR_FIN_And_ack_time_armed_To_0,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_EqTo_CC_NO_ERROR_Set_sub_"
        "state_To_SEND_WAIT_FOR_FIN_And_ack_time_armed_To_0");
    UtTest_Add(
        Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_NotEqTo_CC_NO_ERROR_Call_CF_CFDP_S_Reset,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S2_WaitForEofAck_CallTo_CF_CFDP_RecvAck_Returns_0_And_t_history_cc_NotEqTo_CC_NO_ERROR_Call_CF_"
        "CF_TxnState_S_Reset");
} /* end add_CF_CFDP_S2_WaitForEofAck_tests */

void add_CF_CFDP_S_DispatchRecv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S_DispatchRecv_Asserts_sub_state_LessThan_SEND_NUM_STATES, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_DispatchRecv_Asserts_sub_state_LessThan_SEND_NUM_STATES");
    UtTest_Add(Test_CF_CFDP_S_DispatchRecv_Asserts_msg_in_Is_NULL, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_DispatchRecv_Asserts_msg_in_Is_NULL");
    UtTest_Add(Test_CF_CFDP_S_DispatchRecv_AlreadyHas_pdu_ph_flags_SetSoSendEvent, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_DispatchRecv_AlreadyHas_pdu_ph_flags_SetSoSendEvent");
    UtTest_Add(
        Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsEqTo_PDU_INVALID_MAX_SendEventAndIncrement_recv_spurious,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsEqTo_PDU_INVALID_MAX_"
        "SendEventAndIncrement_recv_spurious");
    UtTest_Add(
        Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsGreaterThan_PDU_INVALID_MAX_SendEventAndIncrement_recv_spurious,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_DispatchRecv_DidNotHaveFlagsSetBut_fdh_directive_code_IsGreaterThan_PDU_INVALID_MAX_"
        "SendEventAndIncrement_recv_spurious");
    UtTest_Add(
        Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_Call_fns_sub_state_directive_code,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_Call_fns_sub_state_"
        "directive_code");
    UtTest_Add(
        Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_But_fns_NULL_DoNothing,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_DispatchRecv_Received_msg_ph_As_fdh_Has_flags_LessThan_PDU_INVALID_MAX_But_fns_NULL_DoNothing");
} /* end add_CF_CFDP_S_DispatchRecv_tests */

void add_CF_CFDP_S1_Recv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S1_Recv_SendsAll_NULL_fns_To_CF_CFDP_S_DispatchRecv, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S1_Recv_SendsAll_NULL_fns_To_CF_CFDP_S_DispatchRecv");
} /* end add_CF_CFDP_S1_Recv_tests */

void add_CF_CFDP_S2_Recv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_Recv_Call_CF_CFDP_S_DispatchRecv, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Recv_Call_CF_CFDP_S_DispatchRecv");
} /* end add_CF_CFDP_S2_Recv_tests */

void add_CF_CFDP_S1_Tx_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S1_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S1_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF");
    UtTest_Add(Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata");
    /* NOTE: comment tests back in when they are fixed, but coverage does not require
     * Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData or
     * Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S1_SubstateSendEof */
    // UtTest_Add(Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData,
    //   cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
    //   "Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData");
    // UtTest_Add(Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S1_SubstateSendEof,
    //   cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
    //   "Test_CF_CFDP_S1_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S1_SubstateSendEof");
} /* end add_CF_CFDP_S1_Tx_tests */

void add_CF_CFDP_S2_Tx_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S2_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S2_Tx_Asserts_t_sub_state_IsGreaterThan_SEND_EOF");
    UtTest_Add(Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_0_Call_CF_CFDP_S_SubstateSendMetadata");
    /* NOTE: comment tests back in when they are fixed, but coverage does not require
     * Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData or
     * Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S2_SubstateSendEof */
    // UtTest_Add(Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData,
    //   cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
    //   "Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_1_Call_CF_CFDP_S_SubstateSendFileData");
    // UtTest_Add(Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S2_SubstateSendEof,
    //   cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
    //   "Test_CF_CFDP_S2_Tx_When_t_sub_state_Is_2_Call_CF_CFDP_S2_SubstateSendEof");
} /* end add_CF_CFDP_S2_Tx_tests */

void add_CF_CFDP_S_Cancel_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S_Cancel_When_t_sub_state_IsLessThan_SEND_EOF_SetTo_SEND_EOF, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_Cancel_When_t_sub_state_IsLessThan_SEND_EOF_SetTo_SEND_EOF");
    UtTest_Add(Test_CF_CFDP_S_Cancel_When_t_sub_state_IsEqTo_SEND_EOF_DoNothing, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_Cancel_When_t_sub_state_IsEqTo_SEND_EOF_DoNothing");
    UtTest_Add(Test_CF_CFDP_S_Cancel_When_t_sub_state_IsGreaterThan_SEND_EOF_DoNothing, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_Cancel_When_t_sub_state_IsGreaterThan_SEND_EOF_DoNothing");
} /* end add_CF_CFDP_S_Cancel_tests */

void add_CF_CFDP_S_Tick_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S_Tick_When_t_state_IsNotEqTo_CFDP_S2_DoNothing, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_Tick_When_t_state_IsNotEqTo_CFDP_S2_DoNothing");
    UtTest_Add(
        Test_CF_CFDP_S_Tick_CallTo_CF_TimerExpired_Returns_1_ThenSendsEventAndCalls_CF_CFDP_S_Reset_AndIncrements_inactivity_timer,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_Tick_CallTo_CF_TimerExpired_Returns_1_ThenSendsEventAndCalls_CF_CFDP_S_Reset_AndIncrements_"
        "inactivity_timer");
    UtTest_Add(
        Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_EqTo_ack_limit_SendEventIncrement_ack_limit_ThenCalls_CF_CFDP_S_Reset,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_EqTo_ack_"
        "limit_SendEventIncrement_ack_limit_ThenCalls_CF_CFDP_S_Reset");
    UtTest_Add(
        Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_Return_CF_SEND_NO_MSG_CausesErrOut,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_"
        "limit_CallTo_CFDP_S_SendEof_Return_CF_SEND_NO_MSG_CausesErrOut");
    UtTest_Add(
        Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_Returns_CF_SEND_ERROR_Call_CF_CFDP_S_Reset_AndErrOut,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_"
        "limit_CallTo_CFDP_S_SendEof_Returns_CF_SEND_ERROR_Call_CF_CFDP_S_Reset_AndErrOut");
    UtTest_Add(
        Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_limit_CallTo_CFDP_S_SendEof_ReturnsAnythingExcept_CF_SEND_NO_MSG_Or_CF_SEND_ERROR_Call_CF_CFDP_ArmAckTimer,
        cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
        "Test_CF_CFDP_S_Tick_ArmedTimerExpiredAnd_sub_state_EqTo_SEND_WAIT_FOR_EOF_ACK_AndIncremented_ack_NotEqTo_ack_"
        "limit_CallTo_CFDP_S_SendEof_ReturnsAnythingExcept_CF_SEND_NO_MSG_Or_CF_SEND_ERROR_Call_CF_CFDP_ArmAckTimer");
    UtTest_Add(Test_CF_CFDP_S_Tick_ArmedTimerExpired_sub_state_NotEqTo_SEND_WAIT_FOR_EOF_ACK_DoNotArmAckOrTick,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_Tick_ArmedTimerExpired_sub_state_NotEqTo_SEND_WAIT_FOR_EOF_ACK_DoNotArmAckOrTick");
    UtTest_Add(Test_CF_CFDP_S_Tick_ArmedTimerNotExpiredCall_CF_Timer_Tick, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_Tick_ArmedTimerNotExpiredCall_CF_Timer_Tick");
    UtTest_Add(Test_CF_CFDP_S_Tick_TimerNotArmedDoNotArmAckTimerOrDoTick, cf_cfdp_s_tests_Setup,
               cf_cfdp_s_tests_Teardown, "Test_CF_CFDP_S_Tick_TimerNotArmedDoNotArmAckTimerOrDoTick");
    UtTest_Add(Test_CF_CFDP_S_Tick_When_sub_state_IsEqTo_SEND_SEND_FIN_ACK_Call_CF_CFDP_S_SubstateSendFinAck,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_Tick_When_sub_state_IsEqTo_SEND_SEND_FIN_ACK_Call_CF_CFDP_S_SubstateSendFinAck");
} /* end add_CF_CFDP_S_Tick_tests */

void add_CF_CFDP_S_Tick_Nak_tests(void)
{
    UtTest_Add(Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_DoesNotReturn_1_Given_cont_ValueNotSet,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_DoesNotReturn_1_Given_cont_ValueNotSet");
    UtTest_Add(Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_1_Given_cont_ValueSetTo_1,
               cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "Test_CF_CFDP_S_Tick_Nak_CallTo_CF_CFDP_S_CheckAndRespondNak_Returns_1_Given_cont_ValueSetTo_1");
} /* end add_CF_CFDP_S_Tick_Nak_tests */

/* end cf_cfdp_s_tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_cfdp_s_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_CFDP_S_Reset_tests();

    add_CFDP_S_SendEof_tests();

    add_CF_CFDP_S1_SubstateSendEof_tests();

    add_CF_CFDP_S2_SubstateSendEof_tests();

    add_CFDP_S_SendFileData_tests();

    add_CF_CFDP_S_SubstateSendFileData_tests();

    add_CF_CFDP_S_CheckAndRespondNak_tests();

    add_CF_CFDP_S2_SubstateSendFileData_tests();

    add_CF_CFDP_S_SubstateSendMetadata_tests();

    add_CF_CFDP_S_SubstateSendFinAck_tests();

    add_CF_CFDP_S2_EarlyFin_tests();

    add_CF_CFDP_S2_Fin_tests();

    add_CF_CFDP_S2_Nak_tests();

    add_CF_CFDP_S2_Nak_Arm_tests();

    add_CF_CFDP_S2_WaitForEofAck_tests();

    add_CF_CFDP_S_DispatchRecv_tests();

    add_CF_CFDP_S1_Recv_tests();

    add_CF_CFDP_S2_Recv_tests();

    add_CF_CFDP_S1_Tx_tests();

    add_CF_CFDP_S2_Tx_tests();

    add_CF_CFDP_S_Cancel_tests();

    add_CF_CFDP_S_Tick_tests();

    add_CF_CFDP_S_Tick_Nak_tests();

} /* end UtTest_Setup for cf_cfdp_s_tests.c */

/* end cf_cfdp_s_tests.c */
