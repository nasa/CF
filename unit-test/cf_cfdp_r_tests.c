/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_cfdp.h"

/* cf_cfdp_helpers.h included for redef of cfdp_ldst_uint32 to take because cf_cfdp_r.c also includes it */
#include "cf_cfdp_helpers.h"

/* local stub defines for macros */
#undef FGV
#define FGV Stub_FGV
uint8 Stub_FGV(uint8 source, CF_FIELD_FIELD name);

#undef cfdp_ldst_uint32
#define cfdp_ldst_uint32(dst, src) (dst) = (src)

#include "cf_cfdp_r.c"

/*******************************************************************************
**
**  cf_cfdp_r_tests local dummy functions for CUT that uses function pointers
**
*******************************************************************************/

static void Dummy_fd_fn(transaction_t *t, const pdu_header_t *pdu)
{
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_fd_fn), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_fd_fn), &pdu, sizeof(pdu));

    UT_DEFAULT_IMPL(Dummy_fd_fn);
}

static void Dummy_fns(transaction_t *t, const pdu_header_t *pdu)
{
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_fns), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_fns), &pdu, sizeof(pdu));

    UT_DEFAULT_IMPL(Dummy_fns);
}

void Handler_CF_CRC_Finalize_SetResult(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    cf_crc_t *c             = UT_Hook_GetArgValueByName(Context, "c", cf_crc_t *);
    cf_crc_t *c_with_result = (cf_crc_t *)UserObj;

    c->result = c_with_result->result;
}

/*******************************************************************************
**
**  cf_cfdp_r_tests local stub functions for overrides
**
*******************************************************************************/

/* uint8 chosen for ease of Stub use, may need increased if larger values use this */
uint8 Stub_FGV(uint8 source, CF_FIELD_FIELD name)
{
    UT_Stub_CopyFromLocal(UT_KEY(FGV), &source, sizeof(source));
    UT_Stub_CopyFromLocal(UT_KEY(FGV), &name, sizeof(name));

    return UT_DEFAULT_IMPL(Stub_FGV);
}

/* end cf_cfdp_r_tests local utility functions */

/*******************************************************************************
**
**  cf_cfdp_r_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_r_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_cfdp_r_tests_Setup */

void cf_cfdp_r_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_r_tests_Teardown */

/* end cf_cfdp_r_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_CFDP_R2_SetCc tests - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R2_SetCc_StoreGivenConditionCodeAndSetFinFlag(void)
{
    /* Arrange */
    history_t        dummy_history;
    transaction_t    arg_t;
    condition_code_t arg_cc = CC_NO_ERROR; // TODO:Any_condition_code_t();

    arg_t.history           = &dummy_history;
    arg_t.history->cc       = CC_POS_ACK_LIMIT_REACHED; // TODO Any_condition_code_t_Except(arg_cc);
    arg_t.flags.rx.send_fin = 0;

    /* Act */
    CF_CFDP_R2_SetCc(&arg_t, arg_cc);

    /* Assert */
    UtAssert_True(arg_t.history->cc == arg_cc, "CF_CFDP_R2_SetCc set t->history->cc to %u and it should have been %u",
                  arg_t.history->cc, arg_cc);
    UtAssert_True(arg_t.flags.rx.send_fin == 1, "CF_CFDP_R2_SetCc set t->flags.rx.send_fin to %u and it should be 1",
                  arg_t.flags.rx.send_fin);
} /* end Test_CF_CFDP_R2_SetCc_StoreGivenConditionCodeAndSetFinFlag */

/* end CF_CFDP_R2_SetCc tests */

/*******************************************************************************
**
**  CF_CFDP_R1_Reset tests - full coverage - []
**
*******************************************************************************/

void Test_CFDP_R1_Reset_Call_CF_CFDP_ResetTransaction_With_1_For_keep_history(void)
{
    /* Arrange */
    transaction_t                      arg_t;
    CF_CFDP_ResetTransaction_context_t received_args;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &received_args, sizeof(CF_CFDP_ResetTransaction_context_t),
                     false);

    /* Act */
    CF_CFDP_R1_Reset(&arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(received_args.t, &arg_t);
    UtAssert_True(received_args.keep_history == 1, "CF_CFDP_ResetTransaction was sent %d and should be 1",
                  received_args.keep_history);
} /* end Test_CFDP_R1_Reset_Call_CF_CFDP_ResetTransaction_With_1_For_keep_history */

/* end CF_CFDP_R1_Reset tests */

/*******************************************************************************
**
**  CF_CFDP_R2_Reset tests - full coverage - []
**
*******************************************************************************/

void Test_CFDP_R2_Reset_WhenTransactionSubStateIs_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_R1_Reset(void)
{
    /* Arrange */
    transaction_t arg_t;
    history_t     dummy_history;

    arg_t.history                = &dummy_history;
    arg_t.state_data.r.sub_state = RECV_WAIT_FOR_FIN_ACK;
    arg_t.state_data.r.r2.eof_cc = CC_NO_ERROR;
    arg_t.history->cc            = CC_NO_ERROR;
    arg_t.flags.rx.canceled      = 0;
    arg_t.flags.rx.send_fin      = 0;

    /* Act */
    CF_CFDP_R2_Reset(&arg_t);

    /* Assert */
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_True(arg_t.flags.rx.send_fin == 0, "Transaction send_fin is %d and should be 0", arg_t.flags.rx.send_fin);

} /* end Test_CFDP_R2_Reset_WhenTransactionSubStateIs_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_R1_Reset */

void Test_CFDP_R2_Reset_When_r_r2_eof_cc_IsErrorConditionCall_CF_CFDP_R1_Reset(void)
{
    /* Arrange */
    transaction_t arg_t;
    history_t     dummy_history;

    arg_t.history                = &dummy_history;
    arg_t.state_data.r.sub_state = RECV_FILEDATA;            // TODO: Any_rx_sub_state_Except(RECV_WAIT_FOR_FIN_ACK);
    arg_t.state_data.r.r2.eof_cc = CC_POS_ACK_LIMIT_REACHED; // TODO: Any_condition_code_t_Except(CC_NO_ERROR);
    arg_t.history->cc            = CC_NO_ERROR;
    arg_t.flags.rx.canceled      = 0;
    arg_t.flags.rx.send_fin      = 0;

    /* Act */
    CF_CFDP_R2_Reset(&arg_t);

    /* Assert */
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_True(arg_t.flags.rx.send_fin == 0, "Transaction send_fin is %d and should be 0", arg_t.flags.rx.send_fin);

} /* end Test_CFDP_R2_Reset_When_r_r2_eof_cc_IsErrorConditionCall_CF_CFDP_R1_Reset */

void Test_CFDP_R2_Reset_When_t_history_cc_IsErrorConditionCall_CF_CFDP_R1_Reset(void)
{
    /* Arrange */
    transaction_t arg_t;
    history_t     dummy_history;

    arg_t.history                = &dummy_history;
    arg_t.state_data.r.sub_state = RECV_FILEDATA; // TODO: Any_rx_sub_state_Except(RECV_WAIT_FOR_FIN_ACK);
    arg_t.state_data.r.r2.eof_cc = CC_NO_ERROR;
    arg_t.history->cc            = CC_POS_ACK_LIMIT_REACHED; // TODO: Any_condition_code_t_Except(CC_NO_ERROR);;
    arg_t.flags.rx.canceled      = 0;
    arg_t.flags.rx.send_fin      = 0;

    /* Act */
    CF_CFDP_R2_Reset(&arg_t);

    /* Assert */
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_True(arg_t.flags.rx.send_fin == 0, "Transaction send_fin is %d and should be 0", arg_t.flags.rx.send_fin);

} /* end Test_CFDP_R2_Reset_When_t_history_cc_IsErrorConditionCall_CF_CFDP_R1_Reset */

void Test_CFDP_R2_Reset_When_t_flags_rx_cancelled_Is_1_Call_CF_CFDP_R1_Reset(void)
{
    /* Arrange */
    transaction_t arg_t;
    history_t     dummy_history;

    arg_t.history                = &dummy_history;
    arg_t.state_data.r.sub_state = RECV_FILEDATA; // TODO: Any_rx_sub_state_Except(RECV_WAIT_FOR_FIN_ACK);
    arg_t.state_data.r.r2.eof_cc = CC_NO_ERROR;
    arg_t.history->cc            = CC_NO_ERROR;
    arg_t.flags.rx.canceled      = 1; // TODO Any but 0?
    arg_t.flags.rx.send_fin      = 0;

    /* Act */
    CF_CFDP_R2_Reset(&arg_t);

    /* Assert */
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_True(arg_t.flags.rx.send_fin == 0, "Transaction send_fin is %d and should be 0", arg_t.flags.rx.send_fin);

} /* end Test_CFDP_R2_Reset_When_t_flags_rx_cancelled_Is_1_Call_CF_CFDP_R1_Reset */

void Test_CFDP_R2_Reset_Set_flags_rx_send_fin_To_1(void)
{
    /* Arrange */
    transaction_t arg_t;
    history_t     dummy_history;

    arg_t.history                = &dummy_history;
    arg_t.state_data.r.sub_state = RECV_FILEDATA; // TODO: Any_rx_sub_state_Except(RECV_WAIT_FOR_FIN_ACK);
    arg_t.state_data.r.r2.eof_cc = CC_NO_ERROR;
    arg_t.history->cc            = CC_NO_ERROR;
    arg_t.flags.rx.canceled      = 0;
    arg_t.flags.rx.send_fin      = 0;

    /* Act */
    CF_CFDP_R2_Reset(&arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
    UtAssert_True(arg_t.flags.rx.send_fin == 1, "CF_CFDP_R2_Reset set send_fin to %d and should be 1",
                  arg_t.flags.rx.send_fin);

} /* end Test_CFDP_R2_Reset_Set_flags_rx_send_fin_To_1 */

/* end CF_CFDP_R2_Reset tests */

/*******************************************************************************
**
**  CF_CFDP_R_CheckCrc tests - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R_CheckCrc_When_crc_NotEq_expected_crc_SendEventAndCountMismatch(void)
{
    /* Arrange */
    transaction_t        arg_t;
    cfdp_state_t         dummy_state;
    history_t            dummy_history;
    cf_entity_id_t       dummy_src_eid = Any_uint8();
    cf_transaction_seq_t dummy_seq_num = Any_uint32();
    cf_crc_t             dummy_crc;
    cf_crc_t            *received_crc;
    uint32               arg_expected_crc = Any_uint32();
    int                  expected_result  = 1;
    int                  local_result;
    hk_channel_data_t    dummy_channel_hk;
    uint16               initial_crc_mismatch = Any_uint16();
    uint16               expectedEventID      = CF_EID_ERR_CFDP_R_CRC;

    dummy_state = CFDP_R2;
    arg_t.state = dummy_state;

    dummy_history.src_eid = dummy_src_eid;
    dummy_history.seq_num = dummy_seq_num;
    arg_t.history         = &dummy_history;

    dummy_crc.result = Any_uint32_Except(arg_expected_crc);
    arg_t.crc        = dummy_crc;

    arg_t.chan_num                               = Any_uint32_LessThan_or_EqualTo(CF_NUM_CHANNELS);
    dummy_channel_hk.counters.fault.crc_mismatch = Any_uint16();
    CF_AppData.hk.channel_hk[arg_t.chan_num]     = dummy_channel_hk;
    CF_AppData.hk.channel_hk[arg_t.chan_num].counters.fault.crc_mismatch = initial_crc_mismatch;

    UT_SetDataBuffer(UT_KEY(CF_CRC_Finalize), &received_crc, sizeof(received_crc), false);
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    local_result = CF_CFDP_R_CheckCrc(&arg_t, arg_expected_crc);

    /* Assert */
    UtAssert_ADDRESS_EQ(received_crc, &arg_t.crc);
    UtAssert_True(local_result == expected_result, "CF_CFDP_R_CheckCrc returned %d and should be fail value of %d",
                  local_result, expected_result);
    UtAssert_True((uint16)EventID == expectedEventID, "CFE_EVS_SendEvent received %u and should have received %u",
                  (uint16)EventID, expectedEventID);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t.chan_num].counters.fault.crc_mismatch ==
                      (uint16)(initial_crc_mismatch + 1),
                  "crc_mismatch is %d and should be 1 more than %d",
                  CF_AppData.hk.channel_hk[arg_t.chan_num].counters.fault.crc_mismatch, initial_crc_mismatch);
} /* end Test_CF_CFDP_R_CheckCrc_When_crc_NotEq_expected_crc_SendEventAndCountMismatch */

void Test_CF_CFDP_R_CheckCrc_When_crc_calculated_IsEqTo_expected_crc_Return_0(void)
{
    /* Arrange */
    transaction_t arg_t;
    cf_crc_t      dummy_crc;
    cf_crc_t     *received_crc;
    uint32        arg_expected_crc = Any_uint32();
    int           expected_result  = 0;
    int           local_result;

    dummy_crc.result = arg_expected_crc;
    arg_t.crc        = dummy_crc;

    UT_SetDataBuffer(UT_KEY(CF_CRC_Finalize), &received_crc, sizeof(received_crc), false);

    /* Act */
    local_result = CF_CFDP_R_CheckCrc(&arg_t, arg_expected_crc);

    /* Assert */
    UtAssert_ADDRESS_EQ(received_crc, &arg_t.crc);
    UtAssert_True(local_result == expected_result, "CF_CFDP_R_CheckCrc returned %d and should be fail value of %d",
                  local_result, expected_result);
} /* end Test_CF_CFDP_R_CheckCrc_When_crc_calculated_IsEqTo_expected_crc_Return_0 */

/* end CF_CFDP_R_CheckCrc tests */

/*******************************************************************************
**
**  CF_CFDP_R2_Complete tests - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R2_Complete_Given_t_history_cc_IsNotEqTo_CC_NO_ERROR_DoNothing(void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    int            arg_ok_to_send_nak = Any_int();
    uint8          initial_sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* uint8 used for rx_sub_state because it is a small valuation right now, TODO:
                                            create any_cf_rx_sub_state */

    arg_t->history     = &dummy_history;
    arg_t->history->cc = Any_uint8_Except(CC_NO_ERROR); /* uint8 used for condition_code_t because it is a small
                                                           valuation right now, TODO: create any_cf_condition_code_t */
    arg_t->state_data.r.sub_state = initial_sub_state;

    /* Act */
    CF_CFDP_R2_Complete(arg_t, arg_ok_to_send_nak);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_ComputeGaps, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_t->state_data.r.sub_state == initial_sub_state,
                  "t->state_data.r.sub_state is %u and should be %u (unchanged)", arg_t->state_data.r.sub_state,
                  initial_sub_state);

} /* end Test_CF_CFDP_R2_Complete_Given_t_history_cc_IsNotEqTo_CC_NO_ERROR_DoNothing */

void Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_0_DoesNotSendNakButSet_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    int            arg_ok_to_send_nak = 0;
    uint8          initial_sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* uint8 used for rx_sub_state because it is a small valuation right now, TODO:
                                            create any_cf_rx_sub_state */

    arg_t->history                = &dummy_history;
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->state_data.r.sub_state = initial_sub_state;
    arg_t->flags.rx.md_recv       = 0;

    /* Act */
    CF_CFDP_R2_Complete(arg_t, arg_ok_to_send_nak);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_ComputeGaps, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);

} /* end
     Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_0_DoesNotSendNakButSet_sub_state_To_RECV_FILEDATA
   */

void Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_Reaches_nak_limit_SendEventSetLimitReachedInfoSet_send_fin_To_1_And_complete_To_1_And_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    int            arg_ok_to_send_nak = 1;
    uint8          initial_sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* uint8 used for rx_sub_state because it is a small valuation right now, TODO:
                                            create any_cf_rx_sub_state */
    cf_config_table_t dummy_config_table;

    arg_t->history                = &dummy_history;
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->state_data.r.sub_state = initial_sub_state;
    arg_t->flags.rx.md_recv       = 0;

    CF_AppData.config_table            = &dummy_config_table;
    CF_AppData.config_table->nak_limit = Any_uint8_Except(0);
    arg_t->state_data.r.r2.counter.nak = CF_AppData.config_table->nak_limit - 1;

    /* Act */
    CF_CFDP_R2_Complete(arg_t, arg_ok_to_send_nak);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_ComputeGaps, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);

} /* end
     Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_Reaches_nak_limit_SendEventSetLimitReachedInfoSet_send_fin_To_1_And_complete_To_1_And_sub_state_To_RECV_FILEDATA
   */

void Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_DoesNotReach_nak_limit_Sets_t_send_nak_To_1_And_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    int            arg_ok_to_send_nak = 1;
    uint8          initial_sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* uint8 used for rx_sub_state because it is a small valuation right now, TODO:
                                            create any_cf_rx_sub_state */
    cf_config_table_t dummy_config_table;

    arg_t->history                = &dummy_history;
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->state_data.r.sub_state = initial_sub_state;
    arg_t->flags.rx.md_recv       = 0;
    arg_t->flags.rx.send_nak      = 0;

    CF_AppData.config_table            = &dummy_config_table;
    CF_AppData.config_table->nak_limit = Any_uint8_GreaterThan(1);
    arg_t->state_data.r.r2.counter.nak = CF_AppData.config_table->nak_limit - 2;

    /* Act */
    CF_CFDP_R2_Complete(arg_t, arg_ok_to_send_nak);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_ComputeGaps, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_t->flags.rx.send_nak == 1, "t->flags.rx.send_nak is %u and should be 1",
                  arg_t->flags.rx.send_nak);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);

} /* end
     Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_DoesNotReach_nak_limit_Sets_t_send_nak_To_1_And_sub_state_To_RECV_FILEDATA
   */

void Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_1_Set_send_nak_To_1(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    int            arg_ok_to_send_nak = 1;
    uint8          initial_sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* uint8 used for rx_sub_state because it is a small valuation right now, TODO:
                                            create any_cf_rx_sub_state */
    cf_config_table_t dummy_config_table;

    arg_t->history                = &dummy_history;
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->state_data.r.sub_state = initial_sub_state;
    arg_t->flags.rx.md_recv       = 1;
    arg_t->flags.rx.send_nak      = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), Any_int_Except(0));

    CF_AppData.config_table            = &dummy_config_table;
    CF_AppData.config_table->nak_limit = Any_uint8_GreaterThan(1);
    arg_t->state_data.r.r2.counter.nak = CF_AppData.config_table->nak_limit - 2;

    /* Act */
    CF_CFDP_R2_Complete(arg_t, arg_ok_to_send_nak);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_ComputeGaps, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_t->flags.rx.send_nak == 1, "t->flags.rx.send_nak is %u and should be 1",
                  arg_t->flags.rx.send_nak);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);

} /* end
     Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_1_Set_send_nak_To_1
   */

void Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_And_t_eof_recv_Is_0_Set_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    int            arg_ok_to_send_nak = 0;
    uint8          initial_sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* uint8 used for rx_sub_state because it is a small valuation right now, TODO:
                                            create any_cf_rx_sub_state */
    cf_config_table_t dummy_config_table;

    arg_t->history                = &dummy_history;
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->state_data.r.sub_state = initial_sub_state;
    arg_t->flags.rx.md_recv       = 1;
    arg_t->flags.rx.send_nak      = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), 0);
    arg_t->flags.rx.eof_recv = 0;

    CF_AppData.config_table            = &dummy_config_table;
    CF_AppData.config_table->nak_limit = Any_uint8_GreaterThan(1);
    arg_t->state_data.r.r2.counter.nak = CF_AppData.config_table->nak_limit - 2;

    /* Act */
    CF_CFDP_R2_Complete(arg_t, arg_ok_to_send_nak);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_ComputeGaps, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_t->flags.rx.send_nak == 0, "t->flags.rx.send_nak is %u and should be 0",
                  arg_t->flags.rx.send_nak);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);

} /* end
     Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_And_t_eof_recv_Is_0_Set_sub_state_To_RECV_FILEDATA
   */

void Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_And_t_eof_recv_IsNonZero_Set_t_send_fin_To_1_And_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    int            arg_ok_to_send_nak = 0;
    uint8          initial_sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* uint8 used for rx_sub_state because it is a small valuation right now, TODO:
                                            create any_cf_rx_sub_state */
    cf_config_table_t dummy_config_table;

    arg_t->history                = &dummy_history;
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->state_data.r.sub_state = initial_sub_state;
    arg_t->flags.rx.md_recv       = 1;
    arg_t->flags.rx.send_nak      = 0;
    arg_t->flags.rx.send_fin      = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), 0);
    arg_t->flags.rx.eof_recv = 1;

    CF_AppData.config_table            = &dummy_config_table;
    CF_AppData.config_table->nak_limit = Any_uint8_GreaterThan(1);
    arg_t->state_data.r.r2.counter.nak = CF_AppData.config_table->nak_limit - 2;

    /* Act */
    CF_CFDP_R2_Complete(arg_t, arg_ok_to_send_nak);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_ComputeGaps, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_t->flags.rx.send_nak == 0, "t->flags.rx.send_nak is %u and should be 0",
                  arg_t->flags.rx.send_nak);
    UtAssert_True(arg_t->flags.rx.send_fin == 1, "t->flags.rx.send_fin is %u and should be 1",
                  arg_t->flags.rx.send_fin);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);

} /* end
     Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_And_t_eof_recv_IsNonZero_Set_t_send_fin_To_1_And_sub_state_To_RECV_FILEDATA
   */

/* end CF_CFDP_R2_Complete tests */

/*******************************************************************************
**
**  CF_CFDP_R_ProcessFd tests - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R_ProcessFd_NoCrcWhen_bytes_received_IsLessThan_size_of_pdu_file_data_header_t_Return_neg1(void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    transaction_t   dummy_t;
    transaction_t  *arg_t = &dummy_t;
    CFE_MSG_Size_t  dummy_bytes_received;
    CFE_MSG_Size_t *arg_bytes_received     = &dummy_bytes_received;
    CFE_MSG_Size_t  initial_bytes_received = Any_uint32_LessThan(sizeof(pdu_file_data_header_t));
    int             local_result;

    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_ph;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    arg_t->chan_num = Any_cf_chan_num();

    /* Act */
    local_result = CF_CFDP_R_ProcessFd(arg_t, arg_bytes_received);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R_ProcessFd returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(Stub_FGV, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

} /* end Test_CF_CFDP_R_ProcessFd_NoCrcWhen_bytes_received_IsLessThan_size_of_pdu_file_data_header_t_Return_neg1 */

void Test_CF_CFDP_R_ProcessFd_HasCrcBut_bytes_received_Minus_4_IsLessThan_size_of_pdu_file_data_header_t_Return_neg1(
    void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    transaction_t   dummy_t;
    transaction_t  *arg_t = &dummy_t;
    CFE_MSG_Size_t  dummy_bytes_received;
    CFE_MSG_Size_t *arg_bytes_received     = &dummy_bytes_received;
    CFE_MSG_Size_t  initial_bytes_received = Any_uint32_LessThan(sizeof(pdu_file_data_header_t)) + 4;
    int             local_result;

    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_ph;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);
    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    arg_t->chan_num = Any_cf_chan_num();

    /* Act */
    local_result = CF_CFDP_R_ProcessFd(arg_t, arg_bytes_received);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R_ProcessFd returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_R_ProcessFd_HasCrcBut_bytes_received_Minus_4_IsLessThan_size_of_pdu_file_data_header_t_Return_neg1
   */

void Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_And_fret_NotEqTo_offset_SendEventSetAndCountErrorReturn_neg1(
    void)
{
    /* Arrange */
    uint32          dummy_offset = Any_uint32();
    pdu_header_t   *dummy_ph     = NULL;
    pdu_fd_t       *dummy_fd     = NULL;
    history_t       dummy_history;
    transaction_t   dummy_t;
    transaction_t  *arg_t = &dummy_t;
    CFE_MSG_Size_t  dummy_bytes_received;
    CFE_MSG_Size_t *arg_bytes_received      = &dummy_bytes_received;
    CFE_MSG_Size_t  initial_bytes_received  = Any_uint32_GreaterThan(sizeof(
        pdu_file_data_header_t)); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an unsigned long */
    uint16          initial_fault_file_seek = Any_uint16();
    int             local_result;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_ph                       = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;
    dummy_fd                       = (pdu_fd_t *)dummy_ph;
    dummy_fd->fdh.offset           = dummy_offset;
    arg_t->state_data.r.cached_pos = Any_uint32_Except(dummy_offset);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), Any_uint32_Except(dummy_offset));
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                    = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek = initial_fault_file_seek;

    /* Act */
    local_result = CF_CFDP_R_ProcessFd(arg_t, arg_bytes_received);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R_ProcessFd returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 2); /* TODO: not yet stubbed STATIC_CAST is providing the hidden call */
    UtAssert_STUB_COUNT(Stub_FGV, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek ==
                      (uint16)(initial_fault_file_seek + 1),
                  "CF_AppData fault.file_seek is %u and should be 1 more than %u",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek, initial_fault_file_seek);
} /* end
     Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_And_fret_NotEqTo_offset_SendEventSetAndCountErrorReturn_neg1
   */

void Test_CF_CFDP_R_ProcessFd_NoCrc_fret_NotEqTo_bytes_received_Value_SendEventSetAndCountErrorReturn_neg1(void)
{
    /* Arrange */
    uint32          dummy_offset = Any_uint32();
    pdu_header_t   *dummy_ph     = NULL;
    pdu_fd_t       *dummy_fd     = NULL;
    history_t       dummy_history;
    transaction_t   dummy_t;
    transaction_t  *arg_t = &dummy_t;
    CFE_MSG_Size_t  dummy_bytes_received;
    CFE_MSG_Size_t *arg_bytes_received       = &dummy_bytes_received;
    CFE_MSG_Size_t  initial_bytes_received   = Any_uint32_GreaterThan(sizeof(
        pdu_file_data_header_t)); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an unsigned long */
    uint16          initial_fault_file_write = Any_uint16();
    int             local_result;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_ph                       = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;
    dummy_fd                       = (pdu_fd_t *)dummy_ph;
    dummy_fd->fdh.offset           = dummy_offset;
    arg_t->state_data.r.cached_pos = dummy_offset;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), dummy_offset);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                     = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_write = initial_fault_file_write;

    /* Act */
    local_result = CF_CFDP_R_ProcessFd(arg_t, arg_bytes_received);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R_ProcessFd returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 2); /* TODO: not yet stubbed STATIC_CAST is providing the hidden call */
    UtAssert_STUB_COUNT(Stub_FGV, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedWrite, 1);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_write ==
                      (uint16)(initial_fault_file_write + 1),
                  "CF_AppData fault.file_write is %u and should be 1 more than %u",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek, initial_fault_file_write);
} /* end Test_CF_CFDP_R_ProcessFd_NoCrc_fret_NotEqTo_bytes_received_Value_SendEventSetAndCountErrorReturn_neg1 */

void Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_Gets_bytes_received_Plus_offset_And_data_bytes_IncreasesBy_bytes_received_Return_0(
    void)
{
    /* Arrange */
    uint32          dummy_offset = Any_uint32();
    pdu_header_t   *dummy_ph     = NULL;
    pdu_fd_t       *dummy_fd     = NULL;
    history_t       dummy_history;
    transaction_t   dummy_t;
    transaction_t  *arg_t = &dummy_t;
    CFE_MSG_Size_t  dummy_bytes_received;
    CFE_MSG_Size_t *arg_bytes_received = &dummy_bytes_received;
    uint32          fake_bytes_received =
        Any_uint32_BetweenExcludeMax(sizeof(pdu_file_data_header_t) + sizeof(pdu_file_data_header_t),
                                     UINT16_MAX); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an
                                                     unsigned long , UINT16_MAX used for size constraint testability */
    CFE_MSG_Size_t initial_bytes_received  = fake_bytes_received;
    CFE_MSG_Size_t updated_bytes_received  = fake_bytes_received - sizeof(pdu_file_data_header_t);
    uint64         initial_file_data_bytes = Any_uint64();
    int            local_result;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_ph                       = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;
    dummy_fd                       = (pdu_fd_t *)dummy_ph;
    dummy_fd->fdh.offset           = dummy_offset;
    arg_t->state_data.r.cached_pos = dummy_offset;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), updated_bytes_received);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                         = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes = initial_file_data_bytes;

    /* Act */
    local_result = CF_CFDP_R_ProcessFd(arg_t, arg_bytes_received);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R_ProcessFd returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 2); /* TODO: not yet stubbed STATIC_CAST is providing the hidden call */
    UtAssert_STUB_COUNT(Stub_FGV, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedWrite, 1);
    UtAssert_UINT32_EQ(arg_t->state_data.r.cached_pos, updated_bytes_received + dummy_offset);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes ==
            (uint64)((uint64)updated_bytes_received + (uint64)initial_file_data_bytes),
        "CF_AppData recv.file_data_bytes is %lu and should be %lu (*bytes_received) more than %lu (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes, updated_bytes_received,
        initial_file_data_bytes);
} /* end
     Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_Gets_bytes_received_Plus_offset_And_data_bytes_IncreasesBy_bytes_received_Return_0
   */

void Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_But_fret_IsEqTo_offset_cached_pos_Gets_bytes_received_Plus_offset_And_data_bytes_IncreasesBy_bytes_received_Return_0(
    void)
{
    /* Arrange */
    uint32          dummy_offset = Any_uint32();
    pdu_header_t   *dummy_ph     = NULL;
    pdu_fd_t       *dummy_fd     = NULL;
    history_t       dummy_history;
    transaction_t   dummy_t;
    transaction_t  *arg_t = &dummy_t;
    CFE_MSG_Size_t  dummy_bytes_received;
    CFE_MSG_Size_t *arg_bytes_received = &dummy_bytes_received;
    uint32          fake_bytes_received =
        Any_uint32_BetweenExcludeMax(sizeof(pdu_file_data_header_t) + sizeof(pdu_file_data_header_t),
                                     UINT16_MAX); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an
                                                     unsigned long , UINT16_MAX used for size constraint testability */
    CFE_MSG_Size_t initial_bytes_received  = fake_bytes_received;
    CFE_MSG_Size_t updated_bytes_received  = fake_bytes_received - sizeof(pdu_file_data_header_t);
    uint64         initial_file_data_bytes = Any_uint64();
    int            local_result;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    dummy_ph                       = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;
    dummy_fd                       = (pdu_fd_t *)dummy_ph;
    dummy_fd->fdh.offset           = dummy_offset;
    arg_t->state_data.r.cached_pos = Any_uint32_Except(dummy_offset);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), dummy_offset);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), updated_bytes_received);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                         = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes = initial_file_data_bytes;

    /* Act */
    local_result = CF_CFDP_R_ProcessFd(arg_t, arg_bytes_received);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R_ProcessFd returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 2); /* TODO: not yet stubbed STATIC_CAST is providing the hidden call */
    UtAssert_STUB_COUNT(Stub_FGV, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
    UtAssert_STUB_COUNT(CF_WrappedWrite, 1);
    UtAssert_UINT32_EQ(arg_t->state_data.r.cached_pos, updated_bytes_received + dummy_offset);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes ==
            (uint64)((uint64)updated_bytes_received + (uint64)initial_file_data_bytes),
        "CF_AppData recv.file_data_bytes is %lu and should be %lu (*bytes_received) more than %lu (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes, updated_bytes_received,
        initial_file_data_bytes);
} /* end
     Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_But_fret_IsEqTo_offset_cached_pos_Gets_bytes_received_Plus_offset_And_data_bytes_IncreasesBy_bytes_received_Return_0
   */

/* end CF_CFDP_R_ProcessFd tests */

/*******************************************************************************
**
**  CF_CFDP_R_SubstateRecvEof tests - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R_SubstateRecvEof_CallTo_CF_CFDP_RecvEof_Returns_non0_SendEventIncrement_recv_error_Return_R_EOF_BAD_EOF(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    pdu_header_t  *arg_ph             = NULL;
    uint32         initial_recv_error = Any_uint32();
    int            local_result;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), Any_uint32_Except(0));

    arg_t->chan_num = Any_cf_chan_num();
    arg_t->history  = &dummy_history;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    /* Act */
    local_result = CF_CFDP_R_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(local_result == R_EOF_BAD_EOF,
                  "CF_CFDP_R_SubstateRecvEof returned %d and should be %d (R_EOF_BAD_EOF)", local_result,
                  R_EOF_BAD_EOF);
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, (uint32)(initial_recv_error + 1));
} /* end
     Test_CF_CFDP_R_SubstateRecvEof_CallTo_CF_CFDP_RecvEof_Returns_non0_SendEventIncrement_recv_error_Return_R_EOF_BAD_EOF
   */

void Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsNotEqTo_t_fsize_SendEventIncrement_file_size_mismatch_Return_R_EOF_FSIZE_MISMATCH(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph                     = (pdu_header_t *)&dummy_ph;
    uint16         initial_file_size_mismatch = Any_uint16();
    int            local_result;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), 0);

    arg_t->chan_num         = Any_cf_chan_num();
    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;
    arg_t->fsize            = Any_uint32();
    dummy_ph.size           = Any_uint32_Except(arg_t->fsize);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch = initial_file_size_mismatch;

    /* Act */
    local_result = CF_CFDP_R_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(local_result == R_EOF_FSIZE_MISMATCH,
                  "CF_CFDP_R_SubstateRecvEof returned %d and should be %d (R_EOF_FSIZE_MISMATCH)", local_result,
                  R_EOF_FSIZE_MISMATCH);
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch,
                      (uint16)(initial_file_size_mismatch + 1));
} /* end
     Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsNotEqTo_t_fsize_SendEventIncrement_file_size_mismatch_Return_R_EOF_FSIZE_MISMATCH
   */

void Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_0_And_ph_size_IsNotEqTo_t_fsize_Return_R_EOF_SUCCESS(void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph                     = (pdu_header_t *)&dummy_ph;
    uint16         initial_file_size_mismatch = Any_uint16();
    int            local_result;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), 0);

    arg_t->chan_num         = Any_cf_chan_num();
    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 0;
    arg_t->fsize            = Any_uint32();
    dummy_ph.size           = Any_uint32_Except(arg_t->fsize);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch = initial_file_size_mismatch;

    /* Act */
    local_result = CF_CFDP_R_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(local_result == R_EOF_SUCCESS,
                  "CF_CFDP_R_SubstateRecvEof returned %d and should be %d (R_EOF_SUCCESS)", local_result,
                  R_EOF_SUCCESS);
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_0_And_ph_size_IsNotEqTo_t_fsize_Return_R_EOF_SUCCESS */

void Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsEqTo_t_fsize_Return_R_EOF_SUCCESS(void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph                     = (pdu_header_t *)&dummy_ph;
    uint16         initial_file_size_mismatch = Any_uint16();
    int            local_result;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), 0);

    arg_t->chan_num         = Any_cf_chan_num();
    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;
    arg_t->fsize            = Any_uint32();
    dummy_ph.size           = arg_t->fsize;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch = initial_file_size_mismatch;

    /* Act */
    local_result = CF_CFDP_R_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(local_result == R_EOF_SUCCESS,
                  "CF_CFDP_R_SubstateRecvEof returned %d and should be %d (R_EOF_SUCCESS)", local_result,
                  R_EOF_SUCCESS);
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsEqTo_t_fsize_Return_R_EOF_SUCCESS */

/* end CF_CFDP_R_SubstateRecvEof tests */

/*******************************************************************************
**
**  CF_CFDP_R1_SubstateRecvEof tests (small) - full coverage - [unstubbables: CF_CFDP_R_SubstateRecvEof (medium),
*CF_CFDP_R_CheckCrc (small), CF_CFDP_R1_Reset (simple)]
**
*******************************************************************************/

void Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_NotEqTo_R_EOF_SUCCESS_DoNotSetGiven_t_keep_To_1(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph       = (pdu_header_t *)&dummy_ph;
    uint32         initial_keep = Any_uint8_Except(1);

    arg_t->keep = initial_keep;

    /* Arrange for CF_CFDP_R_SubstateRecvEof */
    history_t dummy_history;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), Any_uint32_Except(0));

    arg_t->chan_num = Any_cf_chan_num();
    arg_t->history  = &dummy_history;

    /* Act */
    CF_CFDP_R1_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(arg_t->keep == initial_keep,
                  "After call to CF_CFDP_R1_SubstateRecvEof t->keep is %u and should be %u (value before call)",
                  arg_t->keep, initial_keep);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_NotEqTo_R_EOF_SUCCESS_DoNotSetGiven_t_keep_To_1 */

void Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_CheckCrc_Returns_non0_DoNotSetGiven_t_keep_To_1(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph       = (pdu_header_t *)&dummy_ph;
    uint32         initial_keep = Any_uint8_Except(1);

    dummy_ph.crc = Any_uint32();

    arg_t->keep = initial_keep;

    /* Arrange for CF_CFDP_R_CheckCrc */
    arg_t->crc.result = Any_uint32_Except(dummy_ph.crc);

    /* Arrange for CF_CFDP_R_SubstateRecvEof */
    history_t dummy_history;
    uint16    initial_file_size_mismatch = Any_uint16();

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), 0);

    arg_t->chan_num         = Any_cf_chan_num();
    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 0;
    arg_t->fsize            = Any_uint32();
    dummy_ph.size           = Any_uint32_Except(arg_t->fsize);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch = initial_file_size_mismatch;

    /* Act */
    CF_CFDP_R1_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(arg_t->keep == initial_keep,
                  "After call to CF_CFDP_R1_SubstateRecvEof t->keep is %u and should be %u (value before call)",
                  arg_t->keep, initial_keep);
    /* Assert for CF_CFDP_R_CheckCrc */
    UtAssert_STUB_COUNT(CF_CRC_Finalize, 1);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_CheckCrc_Returns_non0_DoNotSetGiven_t_keep_To_1 */

void Test_CF_CFDP_R1_SubstateRecvEof_SetGiven_t_keep_To_1(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph       = (pdu_header_t *)&dummy_ph;
    uint32         initial_keep = Any_uint8_Except(1);

    dummy_ph.crc = Any_uint32();

    arg_t->keep = initial_keep;

    /* Arrange for CF_CFDP_R_CheckCrc */
    arg_t->crc.result = dummy_ph.crc;

    /* Arrange for CF_CFDP_R_SubstateRecvEof */
    history_t dummy_history;
    uint16    initial_file_size_mismatch = Any_uint16();

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), 0);

    arg_t->chan_num         = Any_cf_chan_num();
    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 0;
    arg_t->fsize            = Any_uint32();
    dummy_ph.size           = Any_uint32_Except(arg_t->fsize);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch = initial_file_size_mismatch;

    /* Act */
    CF_CFDP_R1_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(arg_t->keep == 1, "After call to CF_CFDP_R1_SubstateRecvEof t->keep is %u and should be 1",
                  arg_t->keep);
    /* Assert for CF_CFDP_R_CheckCrc */
    UtAssert_STUB_COUNT(CF_CRC_Finalize, 1);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_R1_SubstateRecvEof_SetGiven_t_keep_To_1 */

/* end CF_CFDP_R1_SubstateRecvEof tests */

/*******************************************************************************
**
**  CF_CFDP_R2_SubstateRecvEof tests (large) - full coverage - [unstubbables: CF_CFDP_R_SubstateRecvEof (medium),
*CF_CFDP_R2_Complete (large), CF_CFDP_R2_Reset (small), CF_CFDP_R2_SetCc (simple)]
**
*******************************************************************************/

void Test_CF_CFDP_R2_SubstateRecvEof_Given_t_flags_rx_eof_recv_Is_1_DoNothing(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t  = &dummy_t;
    pdu_header_t  *arg_ph = NULL;

    arg_t->flags.rx.eof_recv = 1;

    /* Act */
    CF_CFDP_R2_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    /* Assert for CF_CFDP_R_SubstateRecvEof - not run in this case! */
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 0);
} /* end Test_CF_CFDP_R2_SubstateRecvEof_Given_t_flags_rx_eof_recv_Is_1_DoNothing */

void Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_ReturnsAnyValueExcept_R_EOF_SUCCESS_Or_R_EOF_FSIZE_MISMATCH_Set_t_state_data_r_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t  = &dummy_t;
    pdu_header_t  *arg_ph = NULL;

    arg_t->flags.rx.eof_recv = 0;

    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_FILEDATA);

    /* Arrange for CF_CFDP_R_SubstateRecvEof */
    history_t dummy_history;

    UT_SetDefaultReturnValue(
        UT_KEY(CF_CFDP_RecvEof),
        Any_int32_Except(0)); /* TODO: int32 used because stub is not yet using new handler style */

    arg_t->history = &dummy_history;

    /* Act */
    CF_CFDP_R2_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);
    /* Assert for CF_CFDP_R_SubstateRecvEof */
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 1);
} /* end
     Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_ReturnsAnyValueExcept_R_EOF_SUCCESS_Or_R_EOF_FSIZE_MISMATCH_Set_t_state_data_r_sub_state_To_RECV_FILEDATA
   */

void Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_FSIZE_MISMATCH_Call_CF_CFDP_R2_SetCc(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph = (pdu_header_t *)&dummy_ph;

    arg_t->flags.rx.eof_recv = 0;

    /* Arrange for CF_CFDP_R_SubstateRecvEof */
    history_t dummy_history;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof),
                             0); /* TODO: int32 used because stub is not yet using new handler style */

    arg_t->flags.rx.md_recv     = 1;
    ((pdu_eof_t *)arg_ph)->size = Any_uint32();
    arg_t->fsize                = Any_uint32_Except(((pdu_eof_t *)arg_ph)->size);

    arg_t->history = &dummy_history;

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->history->cc = Any_uint8_Except(CC_FILE_SIZE_ERROR);

    /* Act */
    CF_CFDP_R2_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    /* Assert for CF_CFDP_R_SubstateRecvEof */
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 1);
    /* Assert for  CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->history->cc == CC_FILE_SIZE_ERROR,
                  "t->history->cc is %u and should be %u (CC_FILE_SIZE_ERROR)", arg_t->history->cc, CC_FILE_SIZE_ERROR);
} /* end
     Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_FSIZE_MISMATCH_Call_CF_CFDP_R2_SetCc
   */

void Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_eof_cc_IsEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Complete(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph = (pdu_header_t *)&dummy_ph;

    arg_t->flags.rx.eof_recv = 0;

    UT_SetDefaultReturnValue(UT_KEY(FGV), CC_NO_ERROR);

    /* Arrange for CF_CFDP_R_SubstateRecvEof */
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof),
                             0); /* TODO: int32 used because stub is not yet using new handler style */

    arg_t->flags.rx.md_recv     = 1;
    ((pdu_eof_t *)arg_ph)->size = Any_uint32();
    arg_t->fsize                = ((pdu_eof_t *)arg_ph)->size;

    /* Arrange for CF_CFDP_R2_Complete */
    history_t         dummy_history;
    cf_config_table_t dummy_config_table;

    arg_t->history                     = &dummy_history;
    arg_t->history->cc                 = CC_NO_ERROR;
    arg_t->flags.rx.md_recv            = 0;
    arg_t->state_data.r.r2.counter.nak = 0;
    CF_AppData.config_table            = &dummy_config_table;
    CF_AppData.config_table->nak_limit = UINT8_MAX;
    arg_t->state_data.r.sub_state      = Any_uint8_Except(RECV_FILEDATA);

    /* Act */
    CF_CFDP_R2_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    /* Assert for CF_CFDP_R_SubstateRecvEof */
    UtAssert_STUB_COUNT(CF_CFDP_RecvEof, 1);
    /* Assert for  CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);
} /* end
     Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_eof_cc_IsEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Complete
   */

void Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_eof_cc_IsNotEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Reset(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_eof_t      dummy_ph;
    pdu_header_t  *arg_ph = (pdu_header_t *)&dummy_ph;

    arg_t->flags.rx.eof_recv = 0;

    UT_SetDefaultReturnValue(UT_KEY(FGV), Any_uint8_Except(CC_NO_ERROR));

    /* Arrange for CF_CFDP_R_SubstateRecvEof */
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof),
                             0); /* TODO: int32 used because stub is not yet using new handler style */

    arg_t->flags.rx.md_recv     = 1;
    ((pdu_eof_t *)arg_ph)->size = Any_uint32();
    arg_t->fsize                = ((pdu_eof_t *)arg_ph)->size;

    /* Act */
    CF_CFDP_R2_SubstateRecvEof(arg_t, arg_ph);

    /* Assert */
    /* Assert for  CF_CFDP_R2_Reset via CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_eof_cc_IsNotEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Reset
   */

/* end CF_CFDP_R2_SubstateRecvEof tests */

/*******************************************************************************
**
**  CF_CFDP_R1_SubstateRecvFileData tests (small) - no coverage - [unstubbables: CF_CFDP_R_ProcessFd (large),
*CF_CFDP_R1_Reset (simple)]
**
*******************************************************************************/

void Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R1_Reset(void)
{
    /* Arrange */
    transaction_t *arg_t                        = NULL;
    pdu_header_t  *arg_ph                       = NULL;
    int            forced_return_CF_CFDP_RecvFd = Any_int_Except(0);

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    /* Act */
    CF_CFDP_R1_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    /* Assert for CF_CFDP_R_ProcessFd */
    UtAssert_STUB_COUNT(CF_HeaderSize, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R1_Reset */

void Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_0_Call_CF_CFDP_R1_Reset(
    void)
{
    /* Arrange */
    transaction_t *arg_t                        = NULL;
    pdu_header_t  *arg_ph                       = NULL;
    int            forced_return_CF_CFDP_RecvFd = 0;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    /* Arrange for CF_CFDP_R_ProcessFd */
    CF_AppData.engine.in.bytes_received = 0; /* 0 = recv dropped */

    /* Arrange for CF_CFDP_R2_Reset */
    transaction_t dummy_t;

    arg_t                         = &dummy_t;
    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Act */
    CF_CFDP_R1_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    /* Assert for CF_CFDP_R_ProcessFd */
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_0_Call_CF_CFDP_R1_Reset
   */

void Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_Call_CF_CRC_Digest(
    void)
{
    /* Arrange */
    transaction_t *arg_t                        = NULL;
    pdu_header_t  *arg_ph                       = NULL;
    int            forced_return_CF_CFDP_RecvFd = 0;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    /* Arrange for CF_CFDP_R_ProcessFd */
    transaction_t dummy_t;
    uint32        dummy_offset = Any_uint32();
    pdu_r_msg_t   dummy_msg;
    pdu_header_t *dummy_ph;
    history_t     dummy_history;
    uint32        fake_bytes_received =
        Any_uint32_BetweenExcludeMax(sizeof(pdu_file_data_header_t) + sizeof(pdu_file_data_header_t),
                                     UINT16_MAX); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an
                                                     unsigned long , UINT16_MAX used for size constraint testability */
    CFE_MSG_Size_t initial_bytes_received  = fake_bytes_received;
    CFE_MSG_Size_t updated_bytes_received  = fake_bytes_received - sizeof(pdu_file_data_header_t);
    uint64         initial_file_data_bytes = Any_uint64();

    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_msg;
    dummy_ph                 = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_fd_t *)dummy_ph)->fdh.offset = dummy_offset;
    arg_t                              = &dummy_t;
    arg_t->state_data.r.cached_pos     = dummy_offset;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), updated_bytes_received);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                         = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes = initial_file_data_bytes;

    /* Act */
    CF_CFDP_R1_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 1);
    /* Assert for CF_CFDP_R_ProcessFd - 3 calls, one in function two in STATIC_CAST */
    UtAssert_STUB_COUNT(CF_HeaderSize, 3);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_Call_CF_CRC_Digest
   */

/* end CF_CFDP_R1_SubstateRecvFileData tests */

/*******************************************************************************
**
**  CF_CFDP_R2_SubstateRecvFileData tests (medium) - no coverage - [unstubbables: CF_CFDP_R_ProcessFd (large),
*CF_CFDP_R2_Complete (large), CF_CFDP_R2_Reset (small)]
**
*******************************************************************************/

void Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R2_Reset(void)
{
    /* Arrange */
    transaction_t *arg_t                        = NULL;
    pdu_header_t  *arg_ph                       = NULL;
    int            forced_return_CF_CFDP_RecvFd = Any_int_Except(0);

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    /* Arrange for CF_CFDP_R2_Reset */
    transaction_t dummy_t;

    arg_t                         = &dummy_t;
    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Act */
    CF_CFDP_R2_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    UtAssert_STUB_COUNT(CF_Chunks_Add, 0);
    /* Assert for CF_CFDP_R_ProcessFd */
    UtAssert_STUB_COUNT(CF_HeaderSize, 0);
    /* Assert for CF_CFDP_R2_Reset via CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R2_Reset */

void Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_Call_CF_CFDP_R2_Reset(
    void)
{
    /* Arrange */
    transaction_t *arg_t                        = NULL;
    pdu_header_t  *arg_ph                       = NULL;
    int            forced_return_CF_CFDP_RecvFd = 0;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    /* Arrange for CF_CFDP_R_ProcessFd */
    CF_AppData.engine.in.bytes_received = 0; /* 0 = recv dropped */

    /* Arrange for CF_CFDP_R2_Reset */
    transaction_t dummy_t;

    arg_t                         = &dummy_t;
    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Act */
    CF_CFDP_R2_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    UtAssert_STUB_COUNT(CF_Chunks_Add, 0);
    /* Assert for CF_CFDP_R_ProcessFd */
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    /* Assert for CF_CFDP_R2_Reset via CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_Call_CF_CFDP_R2_Reset
   */

void Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_1_Set_t_state_data_r_r2_counter_nak_To_0(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_header_t   dummy_arg_ph;
    pdu_header_t  *arg_ph                       = &dummy_arg_ph;
    int            forced_return_CF_CFDP_RecvFd = 0;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    arg_t->state_data.r.r2.counter.nak = Any_uint8_Except(0);

    arg_t->flags.rx.fd_nak_sent = 0;

    arg_t->flags.rx.complete = 1;

    /* Arrange for CF_CFDP_R_ProcessFd */
    uint32        dummy_offset = Any_uint32();
    pdu_r_msg_t   dummy_msg;
    pdu_header_t *dummy_ph;
    history_t     dummy_history;
    uint32        fake_bytes_received =
        Any_uint32_BetweenExcludeMax(sizeof(pdu_file_data_header_t) + sizeof(pdu_file_data_header_t),
                                     UINT16_MAX); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an
                                                     unsigned long , UINT16_MAX used for size constraint testability */
    CFE_MSG_Size_t initial_bytes_received  = fake_bytes_received;
    CFE_MSG_Size_t updated_bytes_received  = fake_bytes_received - sizeof(pdu_file_data_header_t);
    uint64         initial_file_data_bytes = Any_uint64();

    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_msg;
    dummy_ph                 = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_fd_t *)dummy_ph)->fdh.offset = dummy_offset;
    arg_t->state_data.r.cached_pos     = dummy_offset;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), updated_bytes_received);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                         = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes = initial_file_data_bytes;

    /* Act */
    CF_CFDP_R2_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    UtAssert_STUB_COUNT(CF_Chunks_Add, 1);
    UtAssert_True(arg_t->state_data.r.r2.counter.nak == 0, "t->state_data.r.r2.counter.nak is %u and should be 0",
                  arg_t->state_data.r.r2.counter.nak);
    /* Assert for CF_CFDP_R_ProcessFd - 3 calls, one in function two in STATIC_CAST */
    UtAssert_STUB_COUNT(CF_HeaderSize, 3);
    /* Assert for CF_CFDP_R2_Reset via CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_1_Set_t_state_data_r_r2_counter_nak_To_0
   */

void Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_1_Call_CF_CFDP_R2_Complete_And_t_flags_rx_complete_Is_non0_Set_t_state_data_r_r2_counter_nak_To_0(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_header_t   dummy_arg_ph;
    pdu_header_t  *arg_ph                       = &dummy_arg_ph;
    int            forced_return_CF_CFDP_RecvFd = 0;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    arg_t->state_data.r.r2.counter.nak = Any_uint8_Except(0);

    arg_t->flags.rx.fd_nak_sent = 1;

    arg_t->flags.rx.complete = 1;

    /* Arrange for CF_CFDP_R_ProcessFd */
    uint32        dummy_offset = Any_uint32();
    pdu_r_msg_t   dummy_msg;
    pdu_header_t *dummy_ph;
    history_t     dummy_history;
    uint32        fake_bytes_received =
        Any_uint32_BetweenExcludeMax(sizeof(pdu_file_data_header_t) + sizeof(pdu_file_data_header_t),
                                     UINT16_MAX); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an
                                                     unsigned long , UINT16_MAX used for size constraint testability */
    CFE_MSG_Size_t initial_bytes_received  = fake_bytes_received;
    CFE_MSG_Size_t updated_bytes_received  = fake_bytes_received - sizeof(pdu_file_data_header_t);
    uint64         initial_file_data_bytes = Any_uint64();

    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_msg;
    dummy_ph                 = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_fd_t *)dummy_ph)->fdh.offset = dummy_offset;
    arg_t->state_data.r.cached_pos     = dummy_offset;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), updated_bytes_received);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                         = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes = initial_file_data_bytes;

    /* Arrange for CF_CFDP_R2_Complete */
    arg_t->history->cc = Any_uint8_Except(CC_NO_ERROR);

    /* Act */
    CF_CFDP_R2_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    UtAssert_STUB_COUNT(CF_Chunks_Add, 1);
    UtAssert_True(arg_t->state_data.r.r2.counter.nak == 0, "t->state_data.r.r2.counter.nak is %u and should be 0",
                  arg_t->state_data.r.r2.counter.nak);
    /* Assert for CF_CFDP_R_ProcessFd - 3 calls, one in function two in STATIC_CAST */
    UtAssert_STUB_COUNT(CF_HeaderSize, 3);
    /* Assert for CF_CFDP_R2_Reset via CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_1_Call_CF_CFDP_R2_Complete_And_t_flags_rx_complete_Is_non0_Set_t_state_data_r_r2_counter_nak_To_0
   */

void Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_0_Call_CF_CFDP_ArmAckTimer_Set_t_state_data_r_r2_counter_nak_To_0(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    pdu_header_t   dummy_arg_ph;
    pdu_header_t  *arg_ph                       = &dummy_arg_ph;
    int            forced_return_CF_CFDP_RecvFd = 0;

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_RecvFd), Handler_int_ForcedReturnOnly, &forced_return_CF_CFDP_RecvFd);

    arg_t->state_data.r.r2.counter.nak = Any_uint8_Except(0);

    arg_t->flags.rx.fd_nak_sent = 0;

    arg_t->flags.rx.complete = 0;

    /* Arrange for CF_CFDP_R_ProcessFd */
    uint32        dummy_offset = Any_uint32();
    pdu_r_msg_t   dummy_msg;
    pdu_header_t *dummy_ph = NULL;
    history_t     dummy_history;
    uint32        fake_bytes_received =
        Any_uint32_BetweenExcludeMax(sizeof(pdu_file_data_header_t) + sizeof(pdu_file_data_header_t),
                                     UINT16_MAX); /*TODO Any_uint32_GreaterThan runs test fine, but should really be an
                                                     unsigned long , UINT16_MAX used for size constraint testability */
    CFE_MSG_Size_t initial_bytes_received  = fake_bytes_received;
    CFE_MSG_Size_t updated_bytes_received  = fake_bytes_received - sizeof(pdu_file_data_header_t);
    uint64         initial_file_data_bytes = Any_uint64();

    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_msg;
    dummy_ph                 = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;

    CF_AppData.engine.in.bytes_received = initial_bytes_received;

    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), 0);
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_fd_t *)dummy_ph)->fdh.offset = dummy_offset;
    arg_t->state_data.r.cached_pos     = dummy_offset;

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), updated_bytes_received);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                         = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.file_data_bytes = initial_file_data_bytes;

    /* Act */
    CF_CFDP_R2_SubstateRecvFileData(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvFd, 1);
    UtAssert_STUB_COUNT(CF_Chunks_Add, 1);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_True(arg_t->state_data.r.r2.counter.nak == 0, "t->state_data.r.r2.counter.nak is %u and should be 0",
                  arg_t->state_data.r.r2.counter.nak);
    /* Assert for CF_CFDP_R_ProcessFd - 3 calls, one in function two in STATIC_CAST */
    UtAssert_STUB_COUNT(CF_HeaderSize, 3);
    /* Assert for CF_CFDP_R2_Reset via CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_0_Call_CF_CFDP_ArmAckTimer_Set_t_state_data_r_r2_counter_nak_To_0
   */

/* end CF_CFDP_R2_SubstateRecvFileData tests */

/*******************************************************************************
**
**  CF_CFDP_R2_GapCompute tests - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R2_GapCompute_AssertsWhenGiven_c_size_Is_0(void)
{
    // /* Arrange */
    // chunks_t*             arg_chunks;
    // chunk_t               dummy_c;
    // chunk_t*              arg_c = &dummy_c;
    // gap_compute_args_t    dummy_args;
    // void*                 arg_opaque = &dummy_args;

    // arg_c->size = 0;

    // /* Act */
    // CF_CFDP_R2_GapCompute(arg_chunks, arg_c, arg_opaque);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - c->size>0");
} /* end Test_CF_CFDP_R2_GapCompute_AssertsWhenGiven_c_size_Is_0 */

void Test_CF_CFDP_R2_GapCompute_WhenGiven_c_size_IsGreaterThan_0_Increment_gap_counter(void)
{
    /* Arrange */
    chunks_t           dummy_chunks;
    chunks_t          *arg_chunks = &dummy_chunks;
    chunk_t            dummy_c;
    chunk_t           *arg_c = &dummy_c;
    pdu_nak_t          dummy_nak;
    gap_compute_args_t dummy_args;
    void              *arg_opaque          = &dummy_args;
    uint32             initial_gap_counter = 0; /* there is only 1 segment in the nak */

    arg_c->size = 1;

    dummy_args.gap_counter = initial_gap_counter;
    dummy_args.ph          = (pdu_header_t *)&dummy_nak;

    /* Act */
    CF_CFDP_R2_GapCompute(arg_chunks, arg_c, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(dummy_args.gap_counter, (uint32)(initial_gap_counter + 1));
} /* end Test_CF_CFDP_R2_GapCompute_WhenGiven_c_size_IsGreaterThan_0_Increment_gap_counter */

/* end CF_CFDP_R2_GapCompute tests */

/*******************************************************************************
**
**  CF_CFDP_R_SubstateSendNak tests (large) - full coverage - [NOTE: CF_CFDP_R2_GapCompute is not unstubbable ONLY
*because it is passed as a value to a stubbable function]
**
*******************************************************************************/

void Test_CF_CFDP_R_SubstateSendNak_CallTo_CF_CFDP_ConstructPduHeader_Returns_NULL_Return_neg1(void)
{
    /* Arrange */
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    int                                  local_result;

    arg_t->history = &dummy_history;

    context_CF_CFDP_ConstructPduHeader.forced_return = NULL;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R_SubstateSendNak returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
} /* end Test_CF_CFDP_R_SubstateSendNak_CallTo_CF_CFDP_ConstructPduHeader_Returns_NULL_Return_neg1 */

void Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1(
    void)
{
    /* Arrange */
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    CF_CFDP_SendNak_context_t            context_CF_CFDP_SendNak;
    uint8                                exceptions[2] = {CF_SEND_ERROR, CF_SEND_SUCCESS};
    int                                  local_result;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 0;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    context_CF_CFDP_SendNak.forced_return = Any_uint8_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendNak), &context_CF_CFDP_SendNak, sizeof(context_CF_CFDP_SendNak), false);

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R_SubstateSendNak returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1
   */

void Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_CannotReturn_CF_SEND_ERROR_ButDoesAnyway(
    void)
{
    // /* Arrange */
    // history_t         dummy_history;
    // transaction_t     dummy_t;
    // transaction_t*    arg_t = &dummy_t;
    // pdu_nak_t         dummy_nak;
    // pdu_header_t*     dummy_ph = (pdu_header_t*)&dummy_nak;
    // CF_CFDP_ConstructPduHeader_context_t  context_CF_CFDP_ConstructPduHeader;
    // CF_CFDP_SendNak_context_t   context_CF_CFDP_SendNak;
    // int               local_result;

    // arg_t->history = &dummy_history;
    // arg_t->flags.rx.md_recv = 0;

    // context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    // UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
    //   sizeof(context_CF_CFDP_ConstructPduHeader), false);

    // context_CF_CFDP_SendNak.forced_return = CF_SEND_ERROR;
    // UT_SetDataBuffer(UT_KEY(CF_CFDP_SendNak), &context_CF_CFDP_SendNak,
    //   sizeof(context_CF_CFDP_SendNak), false);

    // /* Act */
    // local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - sret!=CF_SEND_ERROR");
    // UtAssert_True(local_result == -1,
    //   "CF_CFDP_R_SubstateSendNak returned %d and should be -1",
    //   local_result);
    // UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    // UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_CannotReturn_CF_SEND_ERROR_ButDoesAnyway
   */

void Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_Returns_CF_SEND_SUCCESS_Return_0(
    void)
{
    /* Arrange */
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    CF_CFDP_SendNak_context_t            context_CF_CFDP_SendNak;
    int                                  local_result;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 0;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    context_CF_CFDP_SendNak.forced_return = CF_SEND_SUCCESS;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendNak), &context_CF_CFDP_SendNak, sizeof(context_CF_CFDP_SendNak), false);

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R_SubstateSendNak returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_Returns_CF_SEND_SUCCESS_Return_0
   */

void Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1(
    void)
{
    /* Arrange */
    chunks_wrapper_t                     dummy_chunks;
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    CF_CFDP_SendNak_context_t            context_CF_CFDP_SendNak;
    uint8                                exceptions[2] = {CF_SEND_ERROR, CF_SEND_SUCCESS};
    int                                  local_result;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), Any_uint32_Except(0));

    context_CF_CFDP_SendNak.forced_return = Any_uint8_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendNak), &context_CF_CFDP_SendNak, sizeof(context_CF_CFDP_SendNak), false);

    arg_t->chunks = &dummy_chunks;

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R_SubstateSendNak returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1
   */

void Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_CannotReturn_CF_SEND_ERROR_ButDoesAnyway(
    void)
{
    // /* Arrange */
    // chunks_wrapper_t  dummy_chunks;
    // history_t         dummy_history;
    // transaction_t     dummy_t;
    // transaction_t*    arg_t = &dummy_t;
    // pdu_nak_t         dummy_nak;
    // pdu_header_t*     dummy_ph = (pdu_header_t*)&dummy_nak;
    // CF_CFDP_ConstructPduHeader_context_t  context_CF_CFDP_ConstructPduHeader;
    // CF_CFDP_SendNak_context_t   context_CF_CFDP_SendNak;

    // arg_t->history = &dummy_history;
    // arg_t->flags.rx.md_recv = 1;

    // context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    // UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
    //   sizeof(context_CF_CFDP_ConstructPduHeader), false);

    // UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), Any_uint32_Except(0));

    // context_CF_CFDP_SendNak.forced_return = CF_SEND_ERROR;
    // UT_SetDataBuffer(UT_KEY(CF_CFDP_SendNak), &context_CF_CFDP_SendNak,
    //   sizeof(context_CF_CFDP_SendNak), false);

    // arg_t->chunks = &dummy_chunks;

    // /* Act */
    // local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - sret!=CF_SEND_ERROR");
    // UtAssert_True(local_result == -1,
    //   "CF_CFDP_R_SubstateSendNak returned %d and should be -1",
    //   local_result);
    // UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    // UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_CannotReturn_CF_SEND_ERROR_ButDoesAnyway
   */

void Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_DoesCF_SEND_SUCCESS_Return_0(
    void)
{
    /* Arrange */
    chunks_wrapper_t                     dummy_chunks;
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    CF_CFDP_SendNak_context_t            context_CF_CFDP_SendNak;
    int                                  local_result;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), Any_uint32_Except(0));

    context_CF_CFDP_SendNak.forced_return = CF_SEND_SUCCESS;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendNak), &context_CF_CFDP_SendNak, sizeof(context_CF_CFDP_SendNak), false);

    arg_t->chunks = &dummy_chunks;

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R_SubstateSendNak returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_DoesCF_SEND_SUCCESS_Return_0
   */

void Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsLessThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0(
    void)
{
    /* Arrange */
    chunks_wrapper_t                     dummy_chunks;
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    int                                  local_result;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), 0);

    dummy_chunks.chunks.CF_max_chunks = Any_uint32_Except(0);
    dummy_chunks.chunks.count         = Any_uint32_LessThan(dummy_chunks.chunks.CF_max_chunks);
    arg_t->chunks                     = &dummy_chunks;

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R_SubstateSendNak returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 0);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsLessThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0
   */

void Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsEqTo_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0(
    void)
{
    /* Arrange */
    chunks_wrapper_t                     dummy_chunks;
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    int                                  local_result;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), 0);

    dummy_chunks.chunks.CF_max_chunks = Any_uint32_Except(0);
    dummy_chunks.chunks.count         = dummy_chunks.chunks.CF_max_chunks;
    arg_t->chunks                     = &dummy_chunks;

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R_SubstateSendNak returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 0);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsEqTo_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0
   */

void Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsGreaterThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0(
    void)
{
    /* Arrange */
    chunks_wrapper_t                     dummy_chunks;
    history_t                            dummy_history;
    transaction_t                        dummy_t;
    transaction_t                       *arg_t = &dummy_t;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    int                                  local_result;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), 0);

    dummy_chunks.chunks.CF_max_chunks = Any_uint32_Except(UINT32_MAX);
    dummy_chunks.chunks.count         = Any_uint32_GreaterThan(dummy_chunks.chunks.CF_max_chunks);
    arg_t->chunks                     = &dummy_chunks;

    /* Act */
    local_result = CF_CFDP_R_SubstateSendNak(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R_SubstateSendNak returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 0);
} /* end
     Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsGreaterThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0
   */

/* end CF_CFDP_R_SubstateSendNak tests */

/*******************************************************************************
**
**  CF_CFDP_R_Init tests - (small) - full coverage
**
*******************************************************************************/

void Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreate_ReturnsPositiveValueSet_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    history_t                      dummy_history;
    transaction_t                  dummy_t;
    transaction_t                 *arg_t = &dummy_t;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->state = Any_uint8_Except(CFDP_R2);

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);
    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    arg_t->state_data.r.sub_state =
        Any_uint8_Except(RECV_FILEDATA); /* setting sub_state is not required, but assists verification */

    /* Act */
    CF_CFDP_R_Init(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_True(context_CF_WrappedOpenCreate.access == OS_READ_WRITE,
                  "CF_WrappedCreate received access %u and should be %u (OS_READ_WRITE)",
                  context_CF_WrappedOpenCreate.access, OS_READ_WRITE);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);
} /* end
     Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreate_ReturnsPositiveValueSet_sub_state_To_RECV_FILEDATA
   */

void Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_open_AndCall_CFDP_R1_Reset(
    void)
{
    /* Arrange */
    history_t                      dummy_history;
    transaction_t                  dummy_t;
    transaction_t                 *arg_t             = &dummy_t;
    uint16                         initial_file_open = Any_uint16();
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->state = Any_uint8_Except(CFDP_R2);

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);
    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open = initial_file_open;

    /* Act */
    CF_CFDP_R_Init(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_True(context_CF_WrappedOpenCreate.access == OS_READ_WRITE,
                  "CF_WrappedCreate received access %u and should be %u (OS_READ_WRITE)",
                  context_CF_WrappedOpenCreate.access, OS_READ_WRITE);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_CREAT,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_CREAT)", EventID,
                  CF_EID_ERR_CFDP_R_CREAT);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open == (uint16)(initial_file_open + 1),
                  "file_open is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open, initial_file_open);
    /* Assert unstubbable - CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_open_AndCall_CFDP_R1_Reset
   */

void Test_CF_CFDP_R_Init_StateIs_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_open_But_t_state_IsEqTo_CFDP_R2_SoCall_CFDP_R2_SetCc(
    void)
{
    /* Arrange */
    history_t                      dummy_history;
    transaction_t                  dummy_t;
    transaction_t                 *arg_t             = &dummy_t;
    uint16                         initial_file_open = Any_uint16();
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.md_recv = 1;

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);
    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open = initial_file_open;

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->history->cc = Any_uint8_Except(CC_FILESTORE_REJECTION);

    /* Act */
    CF_CFDP_R_Init(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_True(context_CF_WrappedOpenCreate.access == OS_READ_WRITE,
                  "CF_WrappedCreate received access %u and should be %u (OS_READ_WRITE)",
                  context_CF_WrappedOpenCreate.access, OS_READ_WRITE);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_CREAT,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_CREAT)", EventID,
                  CF_EID_ERR_CFDP_R_CREAT);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open == (uint16)(initial_file_open + 1),
                  "file_open is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open, initial_file_open);
    /* Assert unstubbable - CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->history->cc == CC_FILESTORE_REJECTION,
                  "t->history->cc is %u and should be %u (CC_FILESTORE_REJECTION)", arg_t->history->cc,
                  CC_FILESTORE_REJECTION);
} /* end
     Test_CF_CFDP_R_Init_StateIs_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_open_But_t_state_IsEqTo_CFDP_R2_SoCall_CFDP_R2_SetCc
   */

void Test_CF_CFDP_R_Init_StateIs_CFDP_R2_And_t_flags_rx_md_recv_Is_0_SendEventThenCallTo_CF_WrappedCreat_Returned_0_Set_t_state_data_r_sub_state_To_RECV_FILEDATA(
    void)
{
    /* Arrange */
    cf_config_table_t              dummy_config_table;
    history_t                      dummy_history;
    transaction_t                  dummy_t;
    transaction_t                 *arg_t             = &dummy_t;
    uint16                         initial_file_open = Any_uint16();
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.md_recv = 0;

    arg_t->history          = &dummy_history;
    CF_AppData.config_table = &dummy_config_table;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    context_CF_WrappedOpenCreate.forced_return = 0;
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open = initial_file_open;

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->history->cc = Any_uint8_Except(CC_FILESTORE_REJECTION);

    /* Act */
    CF_CFDP_R_Init(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CFDP_R_TEMP_FILE,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_INF_CFDP_R_TEMP_FILE)", EventID,
                  CF_EID_INF_CFDP_R_TEMP_FILE);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_True(context_CF_WrappedOpenCreate.access == OS_READ_WRITE,
                  "CF_WrappedCreate received access %u and should be %u (OS_READ_WRITE)",
                  context_CF_WrappedOpenCreate.access, OS_READ_WRITE);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open == initial_file_open,
                  "file_open is %u and should be %u (unchanged, value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open, initial_file_open);
    /* Assert unstubbable - CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);
} /* end
     Test_CF_CFDP_R_Init_StateIs_CFDP_R2_And_t_flags_rx_md_recv_Is_0_SendEventThenCallTo_CF_WrappedCreat_Returned_0_Set_t_state_data_r_sub_state_To_RECV_FILEDATA
   */

/* end CF_CFDP_R_Init tests */

/*******************************************************************************
**
**  CF_CFDP_R2_CalcCrcChunk tests (large) - unstubbable: CF_CFDP_R_CheckCrc (small), CF_CFDP_R2_SetCc (simple)
**
*******************************************************************************/

void Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_count_bytes_IsGreaterThan_rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_neg1(
    void)
{
    /* Arrange */
    transaction_t     dummy_t;
    transaction_t    *arg_t = &dummy_t;
    cf_config_table_t dummy_config_table;
    int               local_result;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_Except(0);

    CF_AppData.config_table                               = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup = 0; /* forces count_bytes to never be less */

    arg_t->fsize = Any_uint32_Except(arg_t->state_data.r.r2.rx_crc_calc_bytes);

    /* Act */
    local_result = CF_CFDP_R2_CalcCrcChunk(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R2_SubstateSendFin returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);
} /* end
     Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_count_bytes_IsGreaterThan_rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_neg1
   */

void Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_0_Call_CF_CRC_Start_And_count_bytes_IsGreaterThan_rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_neg1(
    void)
{
    /* Arrange */
    transaction_t     dummy_t;
    transaction_t    *arg_t = &dummy_t;
    cf_config_table_t dummy_config_table;
    int               local_result;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = 0;

    CF_AppData.config_table                               = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup = 0; /* forces count_bytes to never be less */

    arg_t->fsize = Any_uint32_Except(arg_t->state_data.r.r2.rx_crc_calc_bytes);

    /* Act */
    local_result = CF_CFDP_R2_CalcCrcChunk(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R2_SubstateSendFin returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CRC_Start, 1);
} /* end
     Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_0_Call_CF_CRC_Start_And_count_bytes_IsGreaterThan_rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_neg1
   */

void Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_CFDP_R_CheckCrc_Return_Non0_Call_CF_CFDP_R2_SetCc_Set_t_flags_rx_crc_calc_To_1_Return_0(
    void)
{
    /* Arrange */
    transaction_t     dummy_t;
    transaction_t    *arg_t = &dummy_t;
    cf_config_table_t dummy_config_table;
    int               local_result;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_Except(0);

    CF_AppData.config_table = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup =
        1; /* forces count_bytes to be less because it starts at 0 */

    arg_t->fsize = arg_t->state_data.r.r2.rx_crc_calc_bytes;

    arg_t->flags.rx.crc_calc = 0;

    /* Arrange for CF_CFDP_R_CheckCrc */
    history_t dummy_history;

    arg_t->state_data.r.r2.eof_crc = Any_uint32();
    arg_t->crc.result              = Any_uint32_Except(arg_t->state_data.r.r2.eof_crc);

    arg_t->chan_num = Any_cf_chan_num();
    arg_t->history  = &dummy_history;

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->flags.rx.send_fin = 0;

    /* Act */
    local_result = CF_CFDP_R2_CalcCrcChunk(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R2_SubstateSendFin returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);
    UtAssert_True(arg_t->flags.rx.crc_calc == 1, "t->flags.rx.crc_calc is %u and should be 1",
                  arg_t->flags.rx.crc_calc);
    /* Assert for CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->flags.rx.send_fin == 1, "t->flags.rx.send_fin is %u and should be 1",
                  arg_t->flags.rx.send_fin);
} /* end
     Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_CFDP_R_CheckCrc_Return_Non0_Call_CF_CFDP_R2_SetCc_Set_t_flags_rx_crc_calc_To_1_Return_0
   */

void Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_CFDP_R_CheckCrc_Return_0_Set_t_keep_To_1_And_t_state_data_r_r2_cc_To_FIN_COMPLETE_And_t_state_data_r_r2_fs_To_FIN_RETAINED_t_flags_rx_crc_calc_To_1_Return_0(
    void)
{
    /* Arrange */
    transaction_t     dummy_t;
    transaction_t    *arg_t = &dummy_t;
    cf_config_table_t dummy_config_table;
    int               local_result;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_Except(0);

    CF_AppData.config_table = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup =
        1; /* forces count_bytes to be less because it starts at 0 */

    arg_t->fsize = arg_t->state_data.r.r2.rx_crc_calc_bytes;

    arg_t->keep = 0;

    arg_t->state_data.r.r2.dc = Any_uint8_Except(FIN_COMPLETE);
    arg_t->state_data.r.r2.fs = Any_uint8_Except(FIN_RETAINED);
    arg_t->flags.rx.crc_calc  = 0;

    /* Arrange for CF_CFDP_R_CheckCrc */
    history_t dummy_history;

    arg_t->state_data.r.r2.eof_crc = Any_uint32();
    arg_t->crc.result              = arg_t->state_data.r.r2.eof_crc;

    arg_t->chan_num = Any_cf_chan_num();
    arg_t->history  = &dummy_history;

    /* Act */
    local_result = CF_CFDP_R2_CalcCrcChunk(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R2_SubstateSendFin returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);
    UtAssert_True(arg_t->keep == 1, "t->keep is %u and should be 1", arg_t->keep);
    UtAssert_True(arg_t->state_data.r.r2.dc == FIN_COMPLETE,
                  "t->state_data.r.r2.dc is %u and should be %u (FIN_COMPLETE)", arg_t->state_data.r.r2.dc,
                  FIN_COMPLETE);
    UtAssert_True(arg_t->state_data.r.r2.fs == FIN_RETAINED,
                  "t->state_data.r.r2.fs is %u and should be %u (FIN_RETAINED)", arg_t->state_data.r.r2.fs,
                  FIN_RETAINED);
    UtAssert_True(arg_t->flags.rx.crc_calc == 1, "t->flags.rx.crc_calc is %u and should be 1",
                  arg_t->flags.rx.crc_calc);
} /* end
     Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_CFDP_R_CheckCrc_Return_0_Set_t_keep_To_1_And_t_state_data_r_r2_cc_To_FIN_COMPLETE_And_t_state_data_r_r2_fs_To_FIN_RETAINED_t_flags_rx_crc_calc_To_1_Return_0
   */

void Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueNotEqTo_RXC_SendEventAndSet_t_history_cc_To_CC_FILE_SIZE_ERROR_AndIncrement_fault_file_seek_Return_neg1(
    void)
{
    /* Arrange */
    history_t         dummy_history;
    transaction_t     dummy_t;
    transaction_t    *arg_t = &dummy_t;
    cf_config_table_t dummy_config_table;
    uint16            initial_file_seek = Any_uint16();
    int               local_result;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_BetweenExcludeMax(1, UINT32_MAX);

    CF_AppData.config_table = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup =
        1; /* forces count_bytes to be less because it starts at 0 */

    arg_t->fsize = arg_t->state_data.r.r2.rx_crc_calc_bytes + 1; /* + 1 forces less than */
    arg_t->state_data.r.cached_pos =
        Any_uint32_Except(arg_t->state_data.r.r2.rx_crc_calc_bytes); /* + 1 forces less than */

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), Any_uint32_Except(arg_t->state_data.r.r2.rx_crc_calc_bytes));

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history = &dummy_history;

    arg_t->history->cc = Any_uint8_Except(CC_FILE_SIZE_ERROR);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek = initial_file_seek;

    /* Act */
    local_result = CF_CFDP_R2_CalcCrcChunk(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R2_SubstateSendFin returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_SEEK_CRC,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_SEEK_CRC)", EventID,
                  CF_EID_ERR_CFDP_R_SEEK_CRC);
    UtAssert_True(arg_t->history->cc == CC_FILE_SIZE_ERROR,
                  "t->flags.rx.crc_calc is %u and should be %u (CC_FILE_SIZE_ERROR)", arg_t->history->cc,
                  CC_FILE_SIZE_ERROR);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek == (uint16)(initial_file_seek + 1),
                  "fault.file_seek is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_seek, initial_file_seek);
} /* end
     Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueNotEqTo_RXC_SendEventAndSet_t_history_cc_To_CC_FILE_SIZE_ERROR_AndIncrement_fault_file_seek_Return_neg1
   */

void Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueEqTo_RXC_crc_calc_bytes_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_read_size_WhichIsSetTo_t_fsize_Minus_RXC_SendEventAndSet_t_history_cc_To_CC_FILE_SIZE_ERROR_AndIncrement_fault_file_read_Return_neg1(
    void)
{
    /* Arrange */
    history_t         dummy_history;
    transaction_t     dummy_t;
    transaction_t    *arg_t = &dummy_t;
    cf_config_table_t dummy_config_table;
    uint16            initial_file_read = Any_uint16();
    int               local_result;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_BetweenExcludeMax(1, UINT32_MAX);

    CF_AppData.config_table = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup =
        1; /* forces count_bytes to be less because it starts at 0 */

    arg_t->fsize = arg_t->state_data.r.r2.rx_crc_calc_bytes + 1; /* + 1 forces less than */
    arg_t->state_data.r.cached_pos =
        Any_uint32_Except(arg_t->state_data.r.r2.rx_crc_calc_bytes); /* + 1 forces less than */

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), arg_t->state_data.r.r2.rx_crc_calc_bytes);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), arg_t->state_data.r.r2.rx_crc_calc_bytes);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history = &dummy_history;

    arg_t->history->cc = Any_uint8_Except(CC_FILE_SIZE_ERROR);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_read = initial_file_read;

    /* Act */
    local_result = CF_CFDP_R2_CalcCrcChunk(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R2_SubstateSendFin returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_READ,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_READ)", EventID,
                  CF_EID_ERR_CFDP_R_READ);
    UtAssert_True(arg_t->history->cc == CC_FILE_SIZE_ERROR,
                  "t->flags.rx.crc_calc is %u and should be %u (CC_FILE_SIZE_ERROR)", arg_t->history->cc,
                  CC_FILE_SIZE_ERROR);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_read == (uint16)(initial_file_read + 1),
                  "fault.file_seek is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_read, initial_file_read);
} /* end
     Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueEqTo_RXC_crc_calc_bytes_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_read_size_WhichIsSetTo_t_fsize_Minus_RXC_SendEventAndSet_t_history_cc_To_CC_FILE_SIZE_ERROR_AndIncrement_fault_file_read_Return_neg1
   */

void Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_cached_pos_IsEqTo_RXC_CallTo_CF_WrappedRead_ReturnsValueEqTo_read_size_WhichIsSetTo_sizeof_buf_Call_CF_CRC_Digest_ThenIncrease_RXC_ByAdding_read_size_AndSet_t_state_data_r_cached_pos_To_RXC(
    void)
{
    /* Arrange */
    history_t         dummy_history;
    transaction_t     dummy_t;
    transaction_t    *arg_t = &dummy_t;
    cf_config_table_t dummy_config_table;
    uint32            forced_return_CF_WrappedRead = (sizeof(uint8) * CF_R2_CRC_CHUNK_SIZE);
    uint32 initial_RXC = Any_uint32_BetweenExcludeMax(1, UINT8_MAX); /* UINT8_MAX used for reasonable test size */
    int    local_result;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = initial_RXC;

    CF_AppData.config_table = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup =
        1; /* forces count_bytes to be less because it starts at 0 */

    arg_t->fsize = arg_t->state_data.r.r2.rx_crc_calc_bytes + forced_return_CF_WrappedRead; /* - 1 forces less than */
    arg_t->state_data.r.cached_pos = arg_t->state_data.r.r2.rx_crc_calc_bytes;              /* + 1 forces less than */

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), arg_t->state_data.r.r2.rx_crc_calc_bytes);

    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedRead), forced_return_CF_WrappedRead);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history = &dummy_history;

    arg_t->history->cc = Any_uint8_Except(CC_FILE_SIZE_ERROR);

    /* Arrange for CF_CFDP_R_CheckCrc */
    cf_crc_t  dummy_crc;
    cf_crc_t *received_crc = &dummy_crc;

    received_crc->result = Any_uint32_Except(arg_t->state_data.r.r2.eof_crc);

    UT_SetHandlerFunction(UT_KEY(CF_CRC_Finalize), Handler_CF_CRC_Finalize_SetResult, received_crc);

    /* Act */
    local_result = CF_CFDP_R2_CalcCrcChunk(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R2_SubstateSendFin returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 1);
    /* Assert for CF_CFDP_R_CheckCrc */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_CRC,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_CRC)", EventID,
                  CF_EID_ERR_CFDP_R_CRC);
    UtAssert_UINT32_EQ(arg_t->state_data.r.r2.rx_crc_calc_bytes, (initial_RXC + forced_return_CF_WrappedRead));
    UtAssert_UINT32_EQ(arg_t->state_data.r.cached_pos, (initial_RXC + forced_return_CF_WrappedRead));
} /* end
     Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_cached_pos_IsEqTo_RXC_CallTo_CF_WrappedRead_ReturnsValueEqTo_read_size_WhichIsSetTo_sizeof_buf_Call_CF_CRC_Digest_ThenIncrease_RXC_ByAdding_read_size_AndSet_t_state_data_r_cached_pos_To_RXC
   */

/* end CF_CFDP_R2_CalcCrcChunk tests */

/*******************************************************************************
**
**  CF_CFDP_R2_SubstateSendFin tests (small) - full coverage - 1 CF_Assert - [unstubbables: CF_CFDP_R2_CalcCrcChunk
*(large)]
**
*******************************************************************************/

void Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_And_t_flags_rx_crc_calc_Is_0_CallTo_CF_CFDP_R2_CalcCrcChunk_Returns_Non0_Return_Neg1(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    int            local_result;

    arg_t->history           = &dummy_history;
    arg_t->history->cc       = CC_NO_ERROR;
    arg_t->flags.rx.crc_calc = 0;

    /* Arrange for CF_CFDP_R2_CalcCrcChunk */
    cf_config_table_t dummy_config_table;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_Except(0);

    CF_AppData.config_table                               = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup = 0; /* forces count_bytes to never be less */

    arg_t->fsize = Any_uint32_Except(arg_t->state_data.r.r2.rx_crc_calc_bytes);

    /* Act */
    local_result = CF_CFDP_R2_SubstateSendFin(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R2_SubstateSendFin returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 0);
} /* end
     Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_And_t_flags_rx_crc_calc_Is_0_CallTo_CF_CFDP_R2_CalcCrcChunk_Returns_Non0_Return_Neg1
   */

/* TODO: test CF_CFDP_R2_CalcCrcChunk success - deep setup */

void Test_CF_CFDP_R2_SubstateSendFin_AssertsBecauseCallTo_CF_CFDP_SendFin_Returns_CF_SEND_ERROR(void)
{
    // /* Arrange */
    // history_t         dummy_history;
    // transaction_t     dummy_t;
    // transaction_t*    arg_t = &dummy_t;
    // int               local_result;

    // arg_t->history = &dummy_history;
    // arg_t->history->cc = Any_uint8_Except(CC_NO_ERROR);
    // arg_t->flags.rx.crc_calc = 0;

    // UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendFin), CF_SEND_ERROR);

    // /* Act */
    // //local_result = CF_CFDP_R2_SubstateSendFin(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - sret!=CF_SEND_ERROR");
} /* end Test_CF_CFDP_R2_SubstateSendFin_AssertsBecauseCallTo_CF_CFDP_SendFin_Returns_CF_SEND_ERROR */

void Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_CallTo_CF_CFDP_SendFin_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t         = &dummy_t;
    uint8          exceptions[2] = {CF_SEND_ERROR, CF_SEND_SUCCESS};
    int            local_result;

    arg_t->history           = &dummy_history;
    arg_t->history->cc       = Any_uint8_Except(CC_NO_ERROR);
    arg_t->flags.rx.crc_calc = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendFin), Any_uint8_ExceptThese(exceptions, 2));

    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Act */
    local_result = CF_CFDP_R2_SubstateSendFin(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_R2_SubstateSendFin returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 1);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_WAIT_FOR_FIN_ACK,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_WAIT_FOR_FIN_ACK)",
                  arg_t->state_data.r.sub_state, RECV_WAIT_FOR_FIN_ACK);
} /* end
     Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_CallTo_CF_CFDP_SendFin_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1
   */

void Test_CF_CFDP_R2_SubstateSendFin_Given_t_flags_rx_crc_calc_Is_1_CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_Return_0(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    int            local_result;

    arg_t->history           = &dummy_history;
    arg_t->history->cc       = CC_NO_ERROR;
    arg_t->flags.rx.crc_calc = 1;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendFin), CF_SEND_SUCCESS);

    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Act */
    local_result = CF_CFDP_R2_SubstateSendFin(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R2_SubstateSendFin returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 1);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_WAIT_FOR_FIN_ACK,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_WAIT_FOR_FIN_ACK)",
                  arg_t->state_data.r.sub_state, RECV_WAIT_FOR_FIN_ACK);
} /* end
     Test_CF_CFDP_R2_SubstateSendFin_Given_t_flags_rx_crc_calc_Is_1_CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_Return_0
   */

void Test_CF_CFDP_R2_SubstateSendFin_CallTo_CF_CFDP_R2_CalcCrcChunk_Returns_0_Given_t_flags_rx_crc_calc_Is_1_CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_Return_0(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    int            local_result;

    arg_t->history           = &dummy_history;
    arg_t->history->cc       = CC_NO_ERROR;
    arg_t->flags.rx.crc_calc = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendFin), CF_SEND_SUCCESS);

    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Arrange for CF_CFDP_R2_CalcCrcChunk */
    cf_config_table_t dummy_config_table;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_Except(0);

    CF_AppData.config_table = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup =
        1; /* forces count_bytes to be less because it starts at 0 */

    arg_t->fsize = arg_t->state_data.r.r2.rx_crc_calc_bytes;

    arg_t->flags.rx.crc_calc = 0;

    /* Arrange for CF_CFDP_R_CheckCrc */
    arg_t->state_data.r.r2.eof_crc = Any_uint32();
    arg_t->crc.result              = Any_uint32_Except(arg_t->state_data.r.r2.eof_crc);

    arg_t->chan_num = Any_cf_chan_num();
    arg_t->history  = &dummy_history;

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->flags.rx.send_fin = 0;

    /* Act */
    local_result = CF_CFDP_R2_SubstateSendFin(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_R2_SubstateSendFin returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 1);
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_WAIT_FOR_FIN_ACK,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_WAIT_FOR_FIN_ACK)",
                  arg_t->state_data.r.sub_state, RECV_WAIT_FOR_FIN_ACK);
} /* end
     Test_CF_CFDP_R2_SubstateSendFin_CallTo_CF_CFDP_R2_CalcCrcChunk_Returns_0_Given_t_flags_rx_crc_calc_Is_1_CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_Return_0
   */

/* end CF_CFDP_R2_SubstateSendFin tests */

/*******************************************************************************
**
**  CF_CFDP_R2_Recv_fin_ack tests - full coverage - [unstubbables: CF_CFDP_R2_Reset]
**
*******************************************************************************/

void Test_CF_CFDP_R2_Recv_fin_ack_GetsInvalidFinAckFrom_CF_CFDP_RecvAck_SendEventAndIncrement_recv_error(void)
{
    /* Arrange */
    history_t           dummy_history;
    transaction_t       dummy_t;
    transaction_t      *arg_t              = &dummy_t;
    const pdu_header_t *arg_pdu            = NULL;
    uint32              initial_recv_error = Any_uint32();

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvAck), -1); /* -1 indicates error from CF_CFDP_RecvAck */

    arg_t->history = &dummy_history;
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    arg_t->chan_num                                               = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    /* Act */
    CF_CFDP_R2_Recv_fin_ack(arg_t, arg_pdu);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 1,
                  "recv error is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_PDU_FINACK,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_PDU_FINACK)", EventID,
                  CF_EID_ERR_CFDP_R_PDU_FINACK);
} /* end Test_CF_CFDP_R2_Recv_fin_ack_GetsInvalidFinAckFrom_CF_CFDP_RecvAck_SendEventAndIncrement_recv_error */

void Test_CF_CFDP_R2_Recv_fin_ack_GetsValidFinAckFrom_CF_CFDP_RecvAck_Calls_CFDP_R2_Reset(void)
{
    /* Arrange */
    history_t           dummy_history;
    transaction_t       dummy_t;
    transaction_t      *arg_t              = &dummy_t;
    const pdu_header_t *arg_pdu            = NULL;
    uint32              initial_recv_error = Any_uint32();

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvAck), 0); /* 0 indicates success from CF_CFDP_RecvAck */

    arg_t->history = &dummy_history;
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    arg_t->chan_num                                               = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    /* Assert unstubbable - CF_CFDP_R2_Reset setting to enter `else` block that sets send_fin */
    /* settings for arg_t show it was sent to CF_CFDP_R2_Reset */
    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);
    arg_t->state_data.r.r2.eof_cc = CC_NO_ERROR;
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->flags.rx.canceled      = 0;

    arg_t->flags.rx.send_fin = 0; /* set to see it turn to 1 in CF_CFDP_R2_Reset */

    /* Act */
    CF_CFDP_R2_Recv_fin_ack(arg_t, arg_pdu);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error,
                  "recv error is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert unstubbable - CF_CFDP_R2_Reset */
    UtAssert_True(arg_t->flags.rx.send_fin == 1, "send_fin is %u and should be 1", arg_t->flags.rx.send_fin);
} /* end Test_CF_CFDP_R2_Recv_fin_ack_GetsValidFinAckFrom_CF_CFDP_RecvAck_Calls_CFDP_R2_Reset */

/* end CF_CFDP_R2_Recv_fin_ack tests */

/*******************************************************************************
**
**  CF_CFDP_R2_RecvMd tests (large) - full coverage - [unstubbables: CF_CFDP_R2_SetCc (simple), CF_CFDP_R2_Complete
*(large)]
**
*******************************************************************************/

void Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_1_DoNothing(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t  = &dummy_t;
    pdu_header_t  *arg_ph = NULL;

    arg_t->flags.rx.md_recv = 1;

    /* Act */
    CF_CFDP_R2_RecvMd(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvMd, 0);
} /* end Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_1_DoNothing */

void Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_ButCallTo_CF_CFDP_RecvMd_Returns_Non_0_SendEventAndIncrease_recv_error(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t              = &dummy_t;
    pdu_header_t  *arg_ph             = NULL;
    uint32         initial_recv_error = Any_uint32();

    arg_t->flags.rx.md_recv = 0;

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvMd),
                             Any_int32_Except(0)); /* TODO: int32 is used for ease of stub use and should be updated */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    arg_t->chan_num                                               = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    /* Act */
    CF_CFDP_R2_RecvMd(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvMd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_PDU_MD,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_PDU_MD)", EventID,
                  CF_EID_ERR_CFDP_R_PDU_MD);
    UtAssert_INT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, (uint32)(initial_recv_error + 1));
} /* end
     Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_ButCallTo_CF_CFDP_RecvMd_Returns_Non_0_SendEventAndIncrease_recv_error
   */

void Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_But_t_state_data_r_r2_eof_size_IsNotEqTo_t_fsize_SendEventAndIncrease_file_size_mismatch_Call_CFDP_R2_SetCc(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t                      = &dummy_t;
    pdu_header_t  *arg_ph                     = NULL;
    uint16         initial_file_size_mismatch = Any_uint16();

    arg_t->flags.rx.md_recv = 0;

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvMd), 0);

    arg_t->flags.rx.eof_recv = 1;

    arg_t->fsize                    = Any_uint32();
    arg_t->state_data.r.r2.eof_size = Any_uint32_Except(arg_t->fsize);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    arg_t->chan_num                                                             = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch = initial_file_size_mismatch;

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->history->cc = Any_uint8_Except(CC_FILE_SIZE_ERROR);

    /* Act */
    CF_CFDP_R2_RecvMd(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvMd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_EOF_MD_SIZE,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_EOF_MD_SIZE)",
                  EventID, CF_EID_ERR_CFDP_R_EOF_MD_SIZE);
    UtAssert_INT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_size_mismatch,
                      (uint16)(initial_file_size_mismatch + 1));
    /* Assert for CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->history->cc == CC_FILE_SIZE_ERROR,
                  "t->history->cc is %u and should be %u (CC_FILE_SIZE_ERROR)", arg_t->history->cc, CC_FILE_SIZE_ERROR);
} /* end
     Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_But_t_state_data_r_r2_eof_size_IsNotEqTo_t_fsize_SendEventAndIncrease_file_size_mismatch_Call_CFDP_R2_SetCc
   */

void Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_DoesNotReturn_OS_SUCCESS_SendEventAndUndefine_t_fd_AndCAll_CF_CFDP_R2_SetCc_AndIncrement_fault_file_rename(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t               = &dummy_t;
    pdu_header_t  *arg_ph              = NULL;
    uint16         initial_file_rename = Any_uint16();

    arg_t->flags.rx.md_recv = 0;

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvMd), 0);

    arg_t->flags.rx.eof_recv = 1;

    arg_t->fsize                    = Any_uint32();
    arg_t->state_data.r.r2.eof_size = arg_t->fsize;

    UT_SetDefaultReturnValue(UT_KEY(OS_rename), Any_int32_Except(OS_SUCCESS));

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    arg_t->chan_num                                                      = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_rename = initial_file_rename;

    arg_t->fd = Any_uint32_Except(OS_OBJECT_ID_UNDEFINED);

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->history->cc = Any_uint8_Except(CC_FILESTORE_REJECTION);

    /* Act */
    CF_CFDP_R2_RecvMd(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvMd, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_rename, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_RENAME,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_RENAME)", EventID,
                  CF_EID_ERR_CFDP_R_RENAME);
    UtAssert_UINT32_EQ(arg_t->fd, OS_OBJECT_ID_UNDEFINED);
    UtAssert_INT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_rename,
                      (uint16)(initial_file_rename + 1));
    /* Assert for CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->history->cc == CC_FILESTORE_REJECTION,
                  "t->history->cc is %u and should be %u (CC_FILESTORE_REJECTION)", arg_t->history->cc,
                  CC_FILESTORE_REJECTION);
} /* end
     Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_DoesNotReturn_OS_SUCCESS_SendEventAndUndefine_t_fd_AndCAll_CF_CFDP_R2_SetCc_AndIncrement_fault_file_rename
   */

void Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_ButCallTo_CF_WrappedOpenCreate_ReturnsNegativeSoSendEventAndCall_CF_CFDP_R2_SetCc_AndIncrease_fault_file_open(
    void)
{
    /* Arrange */
    history_t                      dummy_history;
    transaction_t                  dummy_t;
    transaction_t                 *arg_t             = &dummy_t;
    pdu_header_t                  *arg_ph            = NULL;
    uint16                         initial_file_open = Any_uint16();
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->flags.rx.md_recv = 0;

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvMd), 0);

    arg_t->flags.rx.eof_recv = 1;

    arg_t->fsize                    = Any_uint32();
    arg_t->state_data.r.r2.eof_size = arg_t->fsize;

    UT_SetDefaultReturnValue(UT_KEY(OS_rename), OS_SUCCESS);

    context_CF_WrappedOpenCreate.forced_return = Any_int32_Negative();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    arg_t->chan_num                                                    = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open = initial_file_open;

    arg_t->fd = Any_uint32_Except(OS_OBJECT_ID_UNDEFINED);

    /* Arrange for CF_CFDP_R2_SetCc */
    arg_t->history->cc = Any_uint8_Except(CC_FILE_SIZE_ERROR);

    /* Act */
    CF_CFDP_R2_RecvMd(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvMd, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_rename, 1);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_OPEN,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_OPEN)", EventID,
                  CF_EID_ERR_CFDP_R_OPEN);
    UtAssert_INT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.file_open,
                      (uint16)(initial_file_open + 1));
    UtAssert_UINT32_EQ(arg_t->fd, OS_OBJECT_ID_UNDEFINED);
    /* Assert for CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->history->cc == CC_FILESTORE_REJECTION,
                  "t->history->cc is %u and should be %u (CC_FILESTORE_REJECTION)", arg_t->history->cc,
                  CC_FILESTORE_REJECTION);
} /* end
     Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_ButCallTo_CF_WrappedOpenCreate_ReturnsNegativeSoSendEventAndCall_CF_CFDP_R2_SetCc_AndIncrease_fault_file_open
   */

void Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_CallTo_CF_WrappedOpenCreate_ReturnsNotNegativeSoSet_md_recv_To_1_And_nak_To_0_Call_CF_CFDP_R2_Complete(
    void)
{
    /* Arrange */
    history_t                      dummy_history;
    transaction_t                  dummy_t;
    transaction_t                 *arg_t  = &dummy_t;
    pdu_header_t                  *arg_ph = NULL;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    arg_t->flags.rx.md_recv = 0;

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename,
                                         sizeof(arg_t->history->fnames.dst_filename) - 1);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvMd), 0);

    arg_t->flags.rx.eof_recv = 0;

    arg_t->fsize                    = Any_uint32();
    arg_t->state_data.r.r2.eof_size = arg_t->fsize;

    UT_SetDefaultReturnValue(UT_KEY(OS_rename), OS_SUCCESS);

    context_CF_WrappedOpenCreate.forced_return = Any_int32_ZeroOrPositive();
    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    arg_t->flags.rx.md_recv            = 0;
    arg_t->state_data.r.r2.counter.nak = Any_uint8_Except(0);

    /* Arrange for CF_CFDP_R2_Complete */
    arg_t->history->cc            = CC_NO_ERROR;
    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_FILEDATA);

    /* Act */
    CF_CFDP_R2_RecvMd(arg_t, arg_ph);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_RecvMd, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(OS_rename, 1);
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(arg_t->flags.rx.md_recv == 1, "t->flags.rx.md_recv is %u and should be 1", arg_t->flags.rx.md_recv);
    UtAssert_True(arg_t->state_data.r.r2.counter.nak == 0, "t->state_data.r.r2.counter.nak is %u and should be 0",
                  arg_t->state_data.r.r2.counter.nak);
    /* Assert for CF_CFDP_R2_Complete */
    UtAssert_True(arg_t->state_data.r.sub_state == RECV_FILEDATA,
                  "t->state_data.r.sub_state is %u and should be %u (RECV_FILEDATA)", arg_t->state_data.r.sub_state,
                  RECV_FILEDATA);
} /* end
     Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_CallTo_CF_WrappedOpenCreate_ReturnsNotNegativeSoSet_md_recv_To_1_And_nak_To_0_Call_CF_CFDP_R2_Complete
   */

/* end CF_CFDP_R2_RecvMd tests */

/*******************************************************************************
**
**  CF_CFDP_R_DispatchRecv tests - full coverage - 2 CF_Assert - []
**
*******************************************************************************/

void Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsEqTo_RECV_NUM_STATES(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state_data.r.sub_state<RECV_NUM_STATES");
} /* end Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsEqTo_RECV_NUM_STATES */

/* NOTE: Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsGreaterThan_RECV_NUM_STATES is not required for coverage
 * but desired for completeness */
// void Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsGreaterThan_RECV_NUM_STATES(void)
// {
//     /* Arrange */
//     /* Act */
//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state_data.r.sub_state<RECV_NUM_STATES");
// } /* end Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsGreaterThan_RECV_NUM_STATES */

void Test_CFDP_R_DispatchRecv_AssertsBecause_msg_in_Is_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.engine.in.msg");
} /* end Test_CFDP_R_DispatchRecv_AssertsBecause_msg_in_Is_NULL */

void Test_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped(void)
{
    /* Arrange */
    pdu_r_msg_t    dummy_msg;
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t                                                                                    = &dummy_t;
    static void (*const arg_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *) = {{NULL}};
    void (*const arg_fd_fn)(transaction_t *, const pdu_header_t *)                                          = {NULL};
    uint16 initial_dropped = Any_uint16();

    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_NUM_STATES);
    CF_AppData.engine.in.msg      = (CFE_SB_Buffer_t *)&dummy_msg;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);

    arg_t->history     = &dummy_history;
    arg_t->history->cc = Any_uint8_Except(CC_NO_ERROR);

    arg_t->chan_num                                                 = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped = initial_dropped;

    /* Act */
    CF_CFDP_R_DispatchRecv(arg_t, arg_fns, arg_fd_fn);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped == (uint16)(initial_dropped + 1),
                  "dropped is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped, initial_dropped);
} /* end Test_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped */

void Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_EqTo_CC_NO_ERROR_Calls_fd_fn_DispatchRecv_FlagsAreNotSetAnd_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped(
    void)
{
    /* Arrange */
    pdu_r_msg_t    dummy_msg;
    pdu_header_t  *dummy_ph = &dummy_msg.ph;
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t                                                                                    = &dummy_t;
    static void (*const arg_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *) = {{NULL}};
    void (*const arg_fd_fn)(transaction_t *, const pdu_header_t *) = Dummy_fd_fn;
    Dummy_fd_fn_context_t context_Dummy_fd_fn;

    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_NUM_STATES);
    CF_AppData.engine.in.msg      = (CFE_SB_Buffer_t *)&dummy_msg;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);

    arg_t->history     = &dummy_history;
    arg_t->history->cc = CC_NO_ERROR;

    UT_SetDataBuffer(UT_KEY(Dummy_fd_fn), &context_Dummy_fd_fn, sizeof(context_Dummy_fd_fn), false);

    /* Act */
    CF_CFDP_R_DispatchRecv(arg_t, arg_fns, arg_fd_fn);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_STUB_COUNT(Dummy_fd_fn, 1);
    UtAssert_True(context_Dummy_fd_fn.t == arg_t, "context_Dummy_fd_fn.t ==  arg_t");
    UtAssert_True(context_Dummy_fd_fn.pdu == dummy_ph, "context_Dummy_fd_fn.pdu ==  dummy_ph");
} /* end
     Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_EqTo_CC_NO_ERROR_Calls_fd_fn_DispatchRecv_FlagsAreNotSetAnd_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped
   */

void Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsEqTo_PDU_INVALID_MAX_Increment_spurious_AndSendEvent(
    void)
{
    /* Arrange */
    pdu_r_msg_t    dummy_msg;
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t                                                                                    = &dummy_t;
    static void (*const arg_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *) = {{NULL}};
    void (*const arg_fd_fn)(transaction_t *, const pdu_header_t *) = Dummy_fd_fn;
    uint16 initial_spurious                                        = Any_uint16();

    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_NUM_STATES);
    CF_AppData.engine.in.msg      = (CFE_SB_Buffer_t *)&dummy_msg;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_file_directive_header_t *)&dummy_msg.ph)->directive_code = PDU_INVALID_MAX;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious = initial_spurious;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history = &dummy_history;

    /* Act */
    CF_CFDP_R_DispatchRecv(arg_t, arg_fns, arg_fd_fn);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious == (uint16)(initial_spurious + 1),
                  "spurious is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_DC_INV,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_DC_INV)", EventID,
                  CF_EID_ERR_CFDP_R_DC_INV);
} /* end
     Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsEqTo_PDU_INVALID_MAX_Increment_spurious_AndSendEvent
   */

void Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsGreaterThan_PDU_INVALID_MAX_Increment_spurious_AndSendEvent(
    void)
{
    /* Arrange */
    pdu_r_msg_t    dummy_msg;
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t                                                                                    = &dummy_t;
    static void (*const arg_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *) = {{NULL}};
    void (*const arg_fd_fn)(transaction_t *, const pdu_header_t *) = Dummy_fd_fn;
    uint16 initial_spurious                                        = Any_uint16();

    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_NUM_STATES);
    CF_AppData.engine.in.msg      = (CFE_SB_Buffer_t *)&dummy_msg;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_file_directive_header_t *)&dummy_msg.ph)->directive_code = Any_uint8_GreaterThan(PDU_INVALID_MAX);

    arg_t->chan_num                                                  = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious = initial_spurious;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history = &dummy_history;

    /* Act */
    CF_CFDP_R_DispatchRecv(arg_t, arg_fns, arg_fd_fn);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious == (uint16)(initial_spurious + 1),
                  "spurious is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_DC_INV,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_DC_INV)", EventID,
                  CF_EID_ERR_CFDP_R_DC_INV);
} /* end
     Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsGreaterThan_PDU_INVALID_MAX_Increment_spurious_AndSendEvent
   */

void Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_directive_code_Is_NULL_DoNothing(
    void)
{
    /* Arrange */
    pdu_r_msg_t    dummy_msg;
    transaction_t  dummy_t;
    transaction_t *arg_t                                                                                    = &dummy_t;
    static void (*const arg_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *) = {{NULL}};
    void (*const arg_fd_fn)(transaction_t *, const pdu_header_t *) = Dummy_fd_fn;
    uint16 initial_dropped                                         = Any_uint16();
    uint16 initial_spurious                                        = Any_uint16();

    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_NUM_STATES);
    CF_AppData.engine.in.msg      = (CFE_SB_Buffer_t *)&dummy_msg;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_file_directive_header_t *)&dummy_msg.ph)->directive_code = Any_uint8_LessThan(PDU_INVALID_MAX);

    arg_t->chan_num                                                  = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped  = initial_dropped;
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious = initial_spurious;

    /* Act */
    CF_CFDP_R_DispatchRecv(arg_t, arg_fns, arg_fd_fn);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped == initial_dropped,
                  "dropped is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped, initial_dropped);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious == initial_spurious,
                  "spurious is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.spurious, initial_spurious);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end
     Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_directive_code_Is_NULL_DoNothing
   */

void Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_directive_code_IsAFunctionCallThatFunction(
    void)
{
    /* Arrange */
    pdu_r_msg_t    dummy_msg;
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;
    void (*const arg_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *);
    void (*const arg_fd_fn)(transaction_t *, const pdu_header_t *)        = Dummy_fd_fn;
    rx_sub_state dummy_state                                              = Any_uint8_LessThan(RECV_NUM_STATES);
    uint8        dummy_directive_code                                     = Any_uint8_LessThan(PDU_INVALID_MAX);
    void (*const fns_pointer)(transaction_t * t, const pdu_header_t *pdu) = Dummy_fns;
    Dummy_fns_context_t context_Dummy_fns;

    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_NUM_STATES);
    CF_AppData.engine.in.msg      = (CFE_SB_Buffer_t *)&dummy_msg;

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    ((pdu_file_directive_header_t *)&dummy_msg.ph)->directive_code = dummy_directive_code;

    arg_t->state_data.r.sub_state = dummy_state;
    // dummy_ph->fdh.directive_code = dummy_directive_code;
    memcpy((void *)&arg_fns[dummy_state][dummy_directive_code], &fns_pointer, sizeof(void *));

    UT_SetDataBuffer(UT_KEY(Dummy_fns), &context_Dummy_fns, sizeof(context_Dummy_fns), false);

    /* Act */
    CF_CFDP_R_DispatchRecv(arg_t, arg_fns, arg_fd_fn);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_fns, 1);
    UtAssert_True(context_Dummy_fns.t == arg_t, "context_Dummy_fns.t ==  arg_t");
} /* end
     Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_directive_code_IsAFunctionCallThatFunction
   */

/* end CF_CFDP_R_DispatchRecv tests */

/*******************************************************************************
**
**  CF_CFDP_R1_Recv tests - full coverage  - NOTE: only provides coverage, not adequately testing substance -
*[unstubbables: CF_CFDP_R2_SubstateRecvEof, CF_CFDP_R2_RecvMd, CF_CFDP_R2_Recv_fin_ack]
**
*******************************************************************************/

void Test_CF_CFDP_R1_Recv_Runs(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;

    /* Arrange unstubbable: CF_CFDP_R_DispatchRecv */
    pdu_r_msg_t dummy_msg;
    history_t   dummy_history;
    uint8       dummy_chan_num       = Any_cf_chan_num();
    uint16      initial_recv_dropped = Any_uint16();

    dummy_msg.ph.flags = 255;

    arg_t->state_data.r.sub_state =
        Any_uint8_LessThan(RECV_NUM_STATES); /* Any_uint8_LessThan used because small size of RECV_NUM_STATES*/
    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_msg;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = CC_NO_ERROR + 1; /* CC_NO_ERROR + 1 so it does not equal in method call */
    arg_t->chan_num    = dummy_chan_num;

    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.dropped = initial_recv_dropped;

    /* Arrange for CF_CFDP_R_DispatchRecv */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 1); /* 1 force else */

    /* Act */
    CF_CFDP_R1_Recv(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_R_DispatchRecv */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.dropped == (uint16)(initial_recv_dropped + 1),
                  "CF_AppData recv dropped is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.dropped, initial_recv_dropped);

} /* end Test_CF_CFDP_R1_Recv_Runs */

/* end CF_CFDP_R1_Recv tests */

/*******************************************************************************
**
**  CF_CFDP_R2_Recv tests - full coverage - [unstubbables: CF_CFDP_R1_Reset (simple)]
**
*******************************************************************************/

void Test_CF_CFDP_R2_Recv_Runs(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;

    /* Arrange unstubbable: CF_CFDP_R_DispatchRecv */
    pdu_r_msg_t dummy_msg;
    history_t   dummy_history;
    uint8       dummy_chan_num       = Any_cf_chan_num();
    uint16      initial_recv_dropped = Any_uint16();

    dummy_msg.ph.flags = 255;

    arg_t->state_data.r.sub_state =
        Any_uint8_LessThan(RECV_NUM_STATES); /* Any_uint8_LessThan used because small size of RECV_NUM_STATES*/
    CF_AppData.engine.in.msg = (CFE_SB_Buffer_t *)&dummy_msg;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = CC_NO_ERROR + 1; /* CC_NO_ERROR + 1 so it does not equal in method call */
    arg_t->chan_num    = dummy_chan_num;

    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.dropped = initial_recv_dropped;

    /* Arrange for CF_CFDP_R_DispatchRecv */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 1); /* 1 force else */

    /* Act */
    CF_CFDP_R2_Recv(arg_t);

    /* Assert */
    /* Assert for CF_CFDP_R_DispatchRecv */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.dropped == (uint16)(initial_recv_dropped + 1),
                  "CF_AppData recv dropped is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.dropped, initial_recv_dropped);

} /* end Test_CF_CFDP_R2_Recv_Runs */

/* end CF_CFDP_R2_Recv tests */

/*******************************************************************************
**
**  CF_CFDP_R_Cancel tests - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Set_send_fin_To_1(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;

    arg_t->state                  = CFDP_R2;
    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_WAIT_FOR_FIN_ACK);

    arg_t->flags.rx.send_fin = 0; /* setting send_fin to 0 is not required, but assistes verification */

    /* Act */
    CF_CFDP_R_Cancel(arg_t);

    /* Assert */
    UtAssert_True(arg_t->flags.rx.send_fin == 1, "send_fin is %u and should be 1", arg_t->flags.rx.send_fin);
} /* end
     Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Set_send_fin_To_1
   */

void Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;

    arg_t->state                  = Any_uint8_Except(CFDP_R2);
    arg_t->state_data.r.sub_state = Any_uint8_LessThan(RECV_WAIT_FOR_FIN_ACK);

    arg_t->flags.rx.send_fin = 0; /* setting send_fin to 0 is not required, but assistes verification */

    /* Arrange unstubbable - CF_CFDP_R1_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_R_Cancel(arg_t);

    /* Assert */
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "send_fin is %u and should be 0", arg_t->flags.rx.send_fin);
    /* Assert unstubbable - CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history);
} /* end
     Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t
   */

void Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;

    arg_t->state                  = CFDP_R2;
    arg_t->state_data.r.sub_state = RECV_WAIT_FOR_FIN_ACK;

    arg_t->flags.rx.send_fin = 0; /* setting send_fin to 0 is not required, but assistes verification */

    /* Arrange unstubbable - CF_CFDP_R1_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_R_Cancel(arg_t);

    /* Assert */
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "send_fin is %u and should be 0", arg_t->flags.rx.send_fin);
    /* Assert unstubbable - CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history);
} /* end
     Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t
   */

void Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;

    arg_t->state                  = CFDP_R2;
    arg_t->state_data.r.sub_state = Any_uint8_GreaterThan(RECV_WAIT_FOR_FIN_ACK);

    arg_t->flags.rx.send_fin = 0; /* setting send_fin to 0 is not required, but assistes verification */

    /* Arrange unstubbable - CF_CFDP_R1_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_R_Cancel(arg_t);

    /* Assert */
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "send_fin is %u and should be 0", arg_t->flags.rx.send_fin);
    /* Assert unstubbable - CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history);
} /* end
     Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t
   */

void Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t = &dummy_t;

    arg_t->state                  = Any_uint8_Except(CFDP_R2);
    arg_t->state_data.r.sub_state = Any_uint8_GreaterThan(RECV_WAIT_FOR_FIN_ACK);

    arg_t->flags.rx.send_fin = 0; /* setting send_fin to 0 is not required, but assistes verification */

    /* Arrange unstubbable - CF_CFDP_R1_Reset */
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CFDP_R_Cancel(arg_t);

    /* Assert */
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "send_fin is %u and should be 0", arg_t->flags.rx.send_fin);
    /* Assert unstubbable - CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 1,
                  "CF_CFDP_ResetTransaction received %d and should be 1",
                  context_CF_CFDP_ResetTransaction.keep_history);
} /* end
     Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t
   */

/* end CF_CFDP_R_Cancel tests */

/*******************************************************************************
**
**  CF_CFDP_R_SendInactivityEvent tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_R_SendInactivityEvent_SendEventAndIncrement_inactivity_timer(void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t                    = &dummy_t;
    uint16         initial_inactivity_timer = Any_uint16();

    arg_t->history = &dummy_history;
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    arg_t->chan_num                                                           = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer = initial_inactivity_timer;

    /* Act */
    CF_CFDP_R_SendInactivityEvent(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_INACT_TIMER,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_INACT_TIMER)",
                  EventID, CF_EID_ERR_CFDP_R_INACT_TIMER);
    /* TODO: Any uint16 problem if returns max value will cause check to fail because 0 != 65536 - left here on purpose
     * to check later by forcing Any_uint16 to return max value */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer ==
                      (uint16)(initial_inactivity_timer + 1),
                  "inactivity_timer is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.inactivity_timer, initial_inactivity_timer);
} /* end Test_CF_CFDP_R_SendInactivityEvent_SendEventAndIncrement_inactivity_timer */

/* end CF_CFDP_R_SendInactivityEvent tests */

/*******************************************************************************
**
**  CF_CFDP_R_Tick tests (very large) - full coverage - [unstubbables: CF_CFDP_R_SendInactivityEvent (simple),
*CF_CFDP_R2_SetCc (simple), CF_CFDP_R_SubstateSendNak (large), CF_CFDP_R2_SubstateSendFin (medium) - [unstubbables:
*CF_CFDP_R2_CalcCrcChunk(very large) - [unstubbables: CF_CFDP_R_CheckCrc (small), CF_CFDP_R2_SetCc (simple)]],
*CF_CFDP_R2_Complete (large), CF_CFDP_R2_Reset (small) - [unstubbables: CF_CFDP_R1_Reset (simple)],
*CF_CFDP_R_SendInactivityEvent (simple), CF_CFDP_R1_Reset (simple)]
**
*******************************************************************************/

void Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = Any_uint8_Except(CFDP_R2);

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 0); /* 0 = false */

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
} /* end Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick */

void Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_1_Call_CF_CFDP_R_SendInactivityEvent_And_CF_CFDP_R1_Reset(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = Any_uint8_Except(CFDP_R2);

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1);

    /* Arrange for CF_CFDP_R_SendInactivityEvent */
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    /* Assert for CF_CFDP_R_SendInactivityEvent */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_INACT_TIMER,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_INACT_TIMER)",
                  EventID, CF_EID_ERR_CFDP_R_INACT_TIMER);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_1_Call_CF_CFDP_R_SendInactivityEvent_And_CF_CFDP_R1_Reset
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_But_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_But_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 0); /* 0 = false */

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_1_Call_CF_CFDP_R_SendInactivityEvent_AndCAll_CF_CFDP_R2_SetCc_Set_inactivity_fired_To_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing(
    void)
{
    /* Arrange */
    history_t      dummy_history;
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1); /* 0 = false */

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Arrange for CF_CFDP_R_SendInactivityEvent */
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history  = &dummy_history;
    arg_t->chan_num = Any_cf_chan_num();

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_True(arg_t->flags.rx.inactivity_fired == 1, "t->flags.rx.inactivity_fired is %u and should be 1",
                  arg_t->flags.rx.inactivity_fired);
    /* Assert for CF_CFDP_R_SendInactivityEvent */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_INACT_TIMER,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_INACT_TIMER)",
                  EventID, CF_EID_ERR_CFDP_R_INACT_TIMER);
    /* Assert for CF_CFDP_R2_SetCc */
    UtAssert_True(arg_t->flags.rx.inactivity_fired == 1, "t->flags.rx.inactivity_fired is %u and should be 1",
                  arg_t->flags.rx.inactivity_fired);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_1_Call_CF_CFDP_R_SendInactivityEvent_AndCAll_CF_CFDP_R2_SetCc_Set_inactivity_fired_To_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_AssertsBecause_CF_CFDP_SendAck_Returns_CF_SEND_ERROR(
    void)
{
    // /* Arrange */
    // history_t           dummy_history;
    // transaction_t       dummy_t;
    // transaction_t*      arg_t = &dummy_t;
    // int                 arg_cont = Any_int();

    // arg_t->state = CFDP_R2;

    // arg_t->flags.rx.inactivity_fired = 0;

    // UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 1); /* 0 = false */

    // arg_t->flags.rx.send_ack = 0;

    // arg_t->flags.rx.send_nak = 0;

    // arg_t->flags.rx.send_fin = 0;

    // arg_t->flags.rx.ack_timer_armed = 0;

    // /* Arrange for CF_CFDP_R_SendInactivityEvent */
    // UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID,
    //   sizeof(EventID), false);
    // arg_t->history = &dummy_history;
    // arg_t->chan_num = Any_cf_chan_num();

    // /* Act */
    // CF_CFDP_R_Tick(arg_t, arg_cont);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - sret!=CF_SEND_ERROR");
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_AssertsBecause_CF_CFDP_SendAck_Returns_CF_SEND_ERROR
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_SendAck_Returns_NotCF_SEND_ERROR_NorCF_SEND_NO_MSG_Set_send_ack_To_0_And_ack_timer_armed_Is_0_DoNothingElse(
    void)
{
    /* Arrange */
    history_t                 dummy_history;
    transaction_t             dummy_t;
    transaction_t            *arg_t         = &dummy_t;
    uint8                     exceptions[2] = {CF_SEND_ERROR, CF_SEND_NO_MSG};
    int                       dummy_cont    = Any_int();
    int                      *arg_cont      = &dummy_cont;
    CF_CFDP_SendAck_context_t context_CF_CFDP_SendAck;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 1;

    context_CF_CFDP_SendAck.forced_return = Any_uint8_ExceptThese(exceptions, 2);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendAck), &context_CF_CFDP_SendAck, sizeof(context_CF_CFDP_SendAck), false);
    arg_t->history = &dummy_history;

    arg_t->flags.rx.send_nak = Any_0_or_1();

    arg_t->flags.rx.send_fin = Any_0_or_1();

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_True(arg_t->flags.rx.send_ack == 0, "t->flags.rx.send_ack is %u and should be 0",
                  arg_t->flags.rx.send_ack);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_SendAck_Returns_NotCF_SEND_ERROR_NorCF_SEND_NO_MSG_Set_send_ack_To_0_And_ack_timer_armed_Is_0_DoNothingElse
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_SendAck_Returns_CF_SEND_NO_MSG_DoNotSet_send_ack_And_ack_timer_armed_Is_0_DoNothingElse(
    void)
{
    /* Arrange */
    history_t                 dummy_history;
    transaction_t             dummy_t;
    transaction_t            *arg_t      = &dummy_t;
    int                       dummy_cont = Any_int();
    int                      *arg_cont   = &dummy_cont;
    CF_CFDP_SendAck_context_t context_CF_CFDP_SendAck;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 1;

    context_CF_CFDP_SendAck.forced_return = CF_SEND_NO_MSG;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendAck), &context_CF_CFDP_SendAck, sizeof(context_CF_CFDP_SendAck), false);
    arg_t->history = &dummy_history;

    arg_t->flags.rx.send_nak = Any_0_or_1();

    arg_t->flags.rx.send_fin = Any_0_or_1();

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_True(arg_t->flags.rx.send_ack == 1, "t->flags.rx.send_ack is %u and should be 1",
                  arg_t->flags.rx.send_ack);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_SendAck_Returns_CF_SEND_NO_MSG_DoNotSet_send_ack_And_ack_timer_armed_Is_0_DoNothingElse
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_0_Set_send_nak_To_0_And_ack_timer_armed_Is_0_DoNothingElse(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 1;

    arg_t->flags.rx.send_fin = Any_0_or_1();

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Arrange for CF_CFDP_R_SubstateSendNak */
    cf_config_table_t                    dummy_config_table;
    chunks_wrapper_t                     dummy_chunks;
    history_t                            dummy_history;
    pdu_nak_t                            dummy_nak;
    pdu_header_t                        *dummy_ph = (pdu_header_t *)&dummy_nak;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;
    CF_CFDP_SendNak_context_t            context_CF_CFDP_SendNak;

    arg_t->history          = &dummy_history;
    CF_AppData.config_table = &dummy_config_table;
    arg_t->flags.rx.md_recv = 1;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    UT_SetDefaultReturnValue(UT_KEY(CF_Chunks_ComputeGaps), Any_uint32_Except(0));

    context_CF_CFDP_SendNak.forced_return = CF_SEND_SUCCESS;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_SendNak), &context_CF_CFDP_SendNak, sizeof(context_CF_CFDP_SendNak), false);

    arg_t->chunks = &dummy_chunks;

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 0);
    UtAssert_True(arg_t->flags.rx.send_nak == 0, "t->flags.rx.send_ack is %u and should be 0",
                  arg_t->flags.rx.send_nak);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_0_Set_send_nak_To_0_And_ack_timer_armed_Is_0_DoNothingElse
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_non0_And_ack_timer_armed_Is_0_DoNothingElse(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 1;

    arg_t->flags.rx.send_fin = Any_0_or_1();

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Arrange for CF_CFDP_R_SubstateSendNak */
    history_t                            dummy_history;
    pdu_header_t                        *dummy_ph = NULL;
    CF_CFDP_ConstructPduHeader_context_t context_CF_CFDP_ConstructPduHeader;

    arg_t->history          = &dummy_history;
    arg_t->flags.rx.md_recv = 1;

    context_CF_CFDP_ConstructPduHeader.forced_return = dummy_ph;
    UT_SetDataBuffer(UT_KEY(CF_CFDP_ConstructPduHeader), &context_CF_CFDP_ConstructPduHeader,
                     sizeof(context_CF_CFDP_ConstructPduHeader), false);

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 0);
    UtAssert_True(arg_t->flags.rx.send_nak == 1, "t->flags.rx.send_nak is %u and should be 1",
                  arg_t->flags.rx.send_nak);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 1);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_non0_And_ack_timer_armed_Is_0_DoNothingElse
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_0_Set_send_fin_To_0_And_ack_timer_armed_Is_0_DoNothingElse(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 1;

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Arrange for CF_CFDP_R2_SubstateSendFin*/
    history_t dummy_history;

    arg_t->history           = &dummy_history;
    arg_t->history->cc       = CC_NO_ERROR;
    arg_t->flags.rx.crc_calc = 0;

    /* Arrange for CF_CFDP_R2_CalcCrcChunk */
    cf_config_table_t dummy_config_table;

    arg_t->state_data.r.r2.rx_crc_calc_bytes = Any_uint32_Except(0);

    CF_AppData.config_table                               = &dummy_config_table;
    CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup = 0; /* forces count_bytes to never be less */

    arg_t->fsize = Any_uint32_Except(arg_t->state_data.r.r2.rx_crc_calc_bytes);

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 0);
    UtAssert_True(arg_t->flags.rx.send_fin == 1, "t->flags.rx.send_fin is %u and should be 1",
                  arg_t->flags.rx.send_fin);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_0_Set_send_fin_To_0_And_ack_timer_armed_Is_0_DoNothingElse
   */

void Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_Non0_And_ack_timer_armed_Is_0_DoNothingElse(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 1;

    arg_t->flags.rx.ack_timer_armed = 0;

    /* Arrange for CF_CFDP_R2_SubstateSendFin*/
    history_t dummy_history;

    arg_t->history           = &dummy_history;
    arg_t->history->cc       = CC_NO_ERROR;
    arg_t->flags.rx.crc_calc = 1;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_SendFin), CF_SEND_SUCCESS);

    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 0);
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "t->flags.rx.send_fin is %u and should be 0",
                  arg_t->flags.rx.send_fin);
    /* Assert for CF_CFDP_R2_SubstateSendFin */
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 1);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_Non0_And_ack_timer_armed_Is_0_DoNothingElse
   */

void Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick(void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 1;

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), 0);

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "t->flags.rx.send_fin is %u and should be 0",
                  arg_t->flags.rx.send_fin);
    /* Assert for CF_CFDP_R2_SubstateSendFin */
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 0);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick */

void Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_t_state_data_r_sub_state_IsNotEqTo_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_ArmAckTimer(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 1;

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), Any_int_Except(0));

    arg_t->flags.rx.complete = 1;

    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "t->flags.rx.send_fin is %u and should be 0",
                  arg_t->flags.rx.send_fin);
    /* Assert for CF_CFDP_R2_SubstateSendFin */
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 0);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_t_state_data_r_sub_state_IsNotEqTo_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_ArmAckTimer
   */

void Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_0_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 1;

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), Any_int_Except(0));

    arg_t->flags.rx.complete = 0;

    arg_t->state_data.r.sub_state = Any_uint8_Except(RECV_WAIT_FOR_FIN_ACK);

    /* Arrange for CF_CFDP_R2_Complete */
    history_t dummy_history;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = Any_uint8_Except(CC_NO_ERROR);

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "t->flags.rx.send_fin is %u and should be 0",
                  arg_t->flags.rx.send_fin);
    /* Assert for CF_CFDP_R2_SubstateSendFin */
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 0);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_0_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer
   */

void Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsNotEqTo_ack_limit_Set_send_fin_To_1_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer(
    void)
{
    /* Arrange */
    transaction_t  dummy_t;
    transaction_t *arg_t      = &dummy_t;
    int            dummy_cont = Any_int();
    int           *arg_cont   = &dummy_cont;

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 1;

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), Any_int_Except(0));

    arg_t->flags.rx.complete = 1;

    arg_t->state_data.r.sub_state = RECV_WAIT_FOR_FIN_ACK;

    /* Arrange for CF_CFDP_R2_Complete */
    history_t dummy_history;

    arg_t->history     = &dummy_history;
    arg_t->history->cc = Any_uint8_Except(CC_NO_ERROR);

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_True(arg_t->flags.rx.send_fin == 1, "t->flags.rx.send_fin is %u and should be 1",
                  arg_t->flags.rx.send_fin);
    /* Assert for CF_CFDP_R2_SubstateSendFin */
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 0);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);
} /* end
     Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsNotEqTo_ack_limit_Set_send_fin_To_1_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer
   */

void Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsEqTo_ack_limit_SendEvent_Increment_fault_ack_limit_Call_CF_CFDP_R2_Reset(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    history_t         dummy_history;
    transaction_t     dummy_t;
    transaction_t    *arg_t            = &dummy_t;
    int               dummy_cont       = Any_int();
    int              *arg_cont         = &dummy_cont;
    uint16            inital_ack_limit = Any_uint16();

    arg_t->state = CFDP_R2;

    arg_t->flags.rx.inactivity_fired = 1;

    arg_t->flags.rx.send_ack = 0;

    arg_t->flags.rx.send_nak = 0;

    arg_t->flags.rx.send_fin = 0;

    arg_t->flags.rx.ack_timer_armed = 1;

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired), Any_int_Except(0));

    arg_t->flags.rx.complete = 1;

    arg_t->state_data.r.sub_state = RECV_WAIT_FOR_FIN_ACK;

    CF_AppData.config_table = &dummy_config_table;

    CF_AppData.config_table->ack_limit = Any_uint8_Except(0); /* Any_uint8 used for small value in test */

    arg_t->state_data.r.r2.counter.ack = CF_AppData.config_table->ack_limit - 1; /* - 1 code increases value */

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);
    arg_t->history = &dummy_history;

    arg_t->chan_num                                                    = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit = inital_ack_limit;

    /* Act */
    CF_CFDP_R_Tick(arg_t, arg_cont);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_R_ACK_LIMIT,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_R_ACK_LIMIT)", EventID,
                  CF_EID_ERR_CFDP_R_ACK_LIMIT);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit == (uint16)(inital_ack_limit + 1),
                  "fault.ack_limit is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.fault.ack_limit, inital_ack_limit);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);
    UtAssert_True(arg_t->flags.rx.send_fin == 0, "t->flags.rx.send_fin is %u and should be 0",
                  arg_t->flags.rx.send_fin);
    /* Assert for CF_CFDP_R2_SubstateSendFin */
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 0);
    /* Assert for CF_CFDP_R_SubstateSendNak */
    UtAssert_STUB_COUNT(CF_CFDP_ConstructPduHeader, 0);
    /* Assert for CF_CFDP_R1_Reset */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
} /* end
     Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsEqTo_ack_limit_SendEvent_Increment_fault_ack_limit_Call_CF_CFDP_R2_Reset
   */

/* end CF_CFDP_R_Tick tests */

/*******************************************************************************
**
**  cf_cfdp_r tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CFDP_R2_SetCc_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_SetCc_StoreGivenConditionCodeAndSetFinFlag, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R2_SetCc_StoreGivenConditionCodeAndSetFinFlag");
} /* end add_CF_CFDP_R2_SetCc_tests */

void add_CFDP_R1_Reset_tests(void)
{
    UtTest_Add(Test_CFDP_R1_Reset_Call_CF_CFDP_ResetTransaction_With_1_For_keep_history, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CFDP_R1_Reset_Call_CF_CFDP_ResetTransaction_With_1_For_keep_history");
} /* end add_CFDP_R1_Reset_tests */

void add_CFDP_R2_Reset_tests(void)
{
    UtTest_Add(Test_CFDP_R2_Reset_WhenTransactionSubStateIs_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_R1_Reset,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CFDP_R2_Reset_WhenTransactionSubStateIs_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_R1_Reset");
    UtTest_Add(Test_CFDP_R2_Reset_When_r_r2_eof_cc_IsErrorConditionCall_CF_CFDP_R1_Reset, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CFDP_R2_Reset_When_r_r2_eof_cc_IsErrorConditionCall_CF_CFDP_R1_Reset");
    UtTest_Add(Test_CFDP_R2_Reset_When_t_history_cc_IsErrorConditionCall_CF_CFDP_R1_Reset, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CFDP_R2_Reset_When_t_history_cc_IsErrorConditionCall_CF_CFDP_R1_Reset");
    UtTest_Add(Test_CFDP_R2_Reset_When_t_flags_rx_cancelled_Is_1_Call_CF_CFDP_R1_Reset, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CFDP_R2_Reset_When_t_flags_rx_cancelled_Is_1_Call_CF_CFDP_R1_Reset");
    UtTest_Add(Test_CFDP_R2_Reset_Set_flags_rx_send_fin_To_1, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CFDP_R2_Reset_Set_flags_rx_send_fin_To_1");
} /* end add_CFDP_R2_Reset_tests */

void add_CF_CFDP_R_CheckCrc_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R_CheckCrc_When_crc_NotEq_expected_crc_SendEventAndCountMismatch, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_CheckCrc_When_crc_NotEq_expected_crc_SendEventAndCountMismatch");
    UtTest_Add(Test_CF_CFDP_R_CheckCrc_When_crc_calculated_IsEqTo_expected_crc_Return_0, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R_CheckCrc_When_crc_calculated_IsEqTo_expected_crc_Return_0");
} /* end add_CF_CFDP_R_CheckCrc_tests */

void add_CF_CFDP_R2_Complete_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_Complete_Given_t_history_cc_IsNotEqTo_CC_NO_ERROR_DoNothing, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R2_Complete_Given_t_history_cc_IsNotEqTo_CC_NO_ERROR_DoNothing");
    UtTest_Add(
        Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_0_DoesNotSendNakButSet_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_0_DoesNotSendNakButSet_sub_state_"
        "To_RECV_FILEDATA");
    UtTest_Add(
        Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_Reaches_nak_limit_SendEventSetLimitReachedInfoSet_send_fin_To_1_And_complete_To_1_And_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_Reaches_nak_limit_"
        "SendEventSetLimitReachedInfoSet_send_fin_To_1_And_complete_To_1_And_sub_state_To_RECV_FILEDATA");
    UtTest_Add(
        Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_DoesNotReach_nak_limit_Sets_t_send_nak_To_1_And_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_Complete_Given_t_Sets_send_nak_To_1_Given_ok_to_send_nak_Is_1_DoesNotReach_nak_limit_Sets_t_"
        "send_nak_To_1_And_sub_state_To_RECV_FILEDATA");
    UtTest_Add(
        Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_1_Set_send_nak_To_1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_1_"
        "Set_send_nak_To_1");
    UtTest_Add(
        Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_And_t_eof_recv_Is_0_Set_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_"
        "And_t_eof_recv_Is_0_Set_sub_state_To_RECV_FILEDATA");
    UtTest_Add(
        Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_And_t_eof_recv_IsNonZero_Set_t_send_fin_To_1_And_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_Complete_Calls_CF_Chunks_ComputeGaps_Returns_non0_Set_send_nak_To_1_Given_ok_to_send_nak_Is_0_"
        "And_t_eof_recv_IsNonZero_Set_t_send_fin_To_1_And_sub_state_To_RECV_FILEDATA");
} /* end add_CF_CFDP_R2_Complete_tests */

void add_CF_CFDP_R_ProcessFd_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_R_ProcessFd_NoCrcWhen_bytes_received_IsLessThan_size_of_pdu_file_data_header_t_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_ProcessFd_NoCrcWhen_bytes_received_IsLessThan_size_of_pdu_file_data_header_t_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R_ProcessFd_HasCrcBut_bytes_received_Minus_4_IsLessThan_size_of_pdu_file_data_header_t_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_ProcessFd_HasCrcBut_bytes_received_Minus_4_IsLessThan_size_of_pdu_file_data_header_t_Return_"
        "neg1");
    UtTest_Add(
        Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_And_fret_NotEqTo_offset_SendEventSetAndCountErrorReturn_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_And_fret_NotEqTo_offset_"
        "SendEventSetAndCountErrorReturn_neg1");
    UtTest_Add(Test_CF_CFDP_R_ProcessFd_NoCrc_fret_NotEqTo_bytes_received_Value_SendEventSetAndCountErrorReturn_neg1,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_ProcessFd_NoCrc_fret_NotEqTo_bytes_received_Value_SendEventSetAndCountErrorReturn_neg1");
    UtTest_Add(
        Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_Gets_bytes_received_Plus_offset_And_data_bytes_IncreasesBy_bytes_received_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_Gets_bytes_received_Plus_offset_And_data_bytes_IncreasesBy_bytes_"
        "received_Return_0");
    UtTest_Add(
        Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_But_fret_IsEqTo_offset_cached_pos_Gets_bytes_received_Plus_offset_And_data_bytes_IncreasesBy_bytes_received_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_ProcessFd_NoCrc_cached_pos_NotEqTo_offset_But_fret_IsEqTo_offset_cached_pos_Gets_bytes_"
        "received_Plus_offset_And_data_bytes_IncreasesBy_bytes_received_Return_0");
} /* end add_CF_CFDP_R_ProcessFd_tests */

void add_CF_CFDP_R_SubstateRecvEof_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_R_SubstateRecvEof_CallTo_CF_CFDP_RecvEof_Returns_non0_SendEventIncrement_recv_error_Return_R_EOF_BAD_EOF,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateRecvEof_CallTo_CF_CFDP_RecvEof_Returns_non0_SendEventIncrement_recv_error_Return_R_EOF_"
        "BAD_EOF");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsNotEqTo_t_fsize_SendEventIncrement_file_size_mismatch_Return_R_EOF_FSIZE_MISMATCH,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsNotEqTo_t_fsize_SendEventIncrement_file_size_"
        "mismatch_Return_R_EOF_FSIZE_MISMATCH");
    UtTest_Add(Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_0_And_ph_size_IsNotEqTo_t_fsize_Return_R_EOF_SUCCESS,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_0_And_ph_size_IsNotEqTo_t_fsize_Return_R_EOF_SUCCESS");
    UtTest_Add(Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsEqTo_t_fsize_Return_R_EOF_SUCCESS,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_SubstateRecvEof_md_recv_IsEqTo_1_And_ph_size_IsEqTo_t_fsize_Return_R_EOF_SUCCESS");
} /* end add_CF_CFDP_R_SubstateRecvEof_tests */

void add_CF_CFDP_R1_SubstateRecvEof_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_NotEqTo_R_EOF_SUCCESS_DoNotSetGiven_t_keep_To_1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_NotEqTo_R_EOF_SUCCESS_DoNotSetGiven_t_keep_"
        "To_1");
    UtTest_Add(Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_CheckCrc_Returns_non0_DoNotSetGiven_t_keep_To_1,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R1_SubstateRecvEof_CallTo_CF_CFDP_R_CheckCrc_Returns_non0_DoNotSetGiven_t_keep_To_1");
    UtTest_Add(Test_CF_CFDP_R1_SubstateRecvEof_SetGiven_t_keep_To_1, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R1_SubstateRecvEof_SetGiven_t_keep_To_1");
} /* end add_CF_CFDP_R1_SubstateRecvEof_tests */

void add_CF_CFDP_R2_SubstateRecvEof_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_SubstateRecvEof_Given_t_flags_rx_eof_recv_Is_1_DoNothing, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R2_SubstateRecvEof_Given_t_flags_rx_eof_recv_Is_1_DoNothing");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_ReturnsAnyValueExcept_R_EOF_SUCCESS_Or_R_EOF_FSIZE_MISMATCH_Set_t_state_data_r_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_ReturnsAnyValueExcept_R_EOF_SUCCESS_Or_R_EOF_"
        "FSIZE_MISMATCH_Set_t_state_data_r_sub_state_To_RECV_FILEDATA");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_FSIZE_MISMATCH_Call_CF_CFDP_R2_SetCc,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_FSIZE_MISMATCH_Call_CF_CFDP_R2_"
        "SetCc");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_eof_cc_IsEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Complete,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_"
        "eof_cc_IsEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Complete");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_eof_cc_IsNotEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Reset,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvEof_CallTo_CF_CFDP_R_SubstateRecvEof_Returns_R_EOF_SUCCESS_And_t_state_data_r_r2_"
        "eof_cc_IsNotEqTo_CC_NO_ERROR_Call_CF_CFDP_R2_Reset");
} /* end add_CF_CFDP_R2_SubstateRecvEof_tests */

void add_CF_CFDP_R1_SubstateRecvFileData_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R1_Reset,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R1_Reset");
    UtTest_Add(
        Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_0_Call_CF_CFDP_R1_Reset,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_0_"
        "Call_CF_CFDP_R1_Reset");
    UtTest_Add(
        Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_Call_CF_CRC_Digest,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R1_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_"
        "Call_CF_CRC_Digest");
} /* end add_CF_CFDP_R1_SubstateRecvFileData_tests */

void add_CF_CFDP_R2_SubstateRecvFileData_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R2_Reset,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_non0_Call_CF_CFDP_R2_Reset");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_Call_CF_CFDP_R2_Reset,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvFileData_CallTo_CF_CFDP_RecvFd_Returns_0_CallTo_CF_CFDP_R_ProcessFd_Returns_non0_"
        "Call_CF_CFDP_R2_Reset");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_1_Set_t_state_data_r_r2_counter_nak_To_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_1_Set_t_state_"
        "data_r_r2_counter_nak_To_0");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_1_Call_CF_CFDP_R2_Complete_And_t_flags_rx_complete_Is_non0_Set_t_state_data_r_r2_counter_nak_To_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_1_Call_CF_CFDP_R2_Complete_And_t_flags_rx_"
        "complete_Is_non0_Set_t_state_data_r_r2_counter_nak_To_0");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_0_Call_CF_CFDP_ArmAckTimer_Set_t_state_data_r_r2_counter_nak_To_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateRecvFileData_t_flags_rx_fd_nak_sent_Is_0_And_t_flags_rx_complete_Is_0_Call_CF_CFDP_"
        "ArmAckTimer_Set_t_state_data_r_r2_counter_nak_To_0");
} /* end add_CF_CFDP_R2_SubstateRecvFileData_tests */

void add_CF_CFDP_R2_GapCompute_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_GapCompute_AssertsWhenGiven_c_size_Is_0, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R2_GapCompute_AssertsWhenGiven_c_size_Is_0");
    UtTest_Add(Test_CF_CFDP_R2_GapCompute_WhenGiven_c_size_IsGreaterThan_0_Increment_gap_counter, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R2_GapCompute_WhenGiven_c_size_IsGreaterThan_0_Increment_gap_counter");
} /* end add_CF_CFDP_R2_GapCompute_tests */

void add_CF_CFDP_R_SubstateSendNak_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R_SubstateSendNak_CallTo_CF_CFDP_ConstructPduHeader_Returns_NULL_Return_neg1,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_SubstateSendNak_CallTo_CF_CFDP_ConstructPduHeader_Returns_NULL_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_DoesNotReturn_CF_SEND_"
        "SUCCESS_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_CannotReturn_CF_SEND_ERROR_ButDoesAnyway,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_"
        "CannotReturn_CF_SEND_ERROR_ButDoesAnyway");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_Returns_CF_SEND_SUCCESS_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_0_SendEvent_CallTo_CF_CFDP_SendNak_Returns_CF_SEND_SUCCESS_"
        "Return_0");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_"
        "SendNak_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_CannotReturn_CF_SEND_ERROR_ButDoesAnyway,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_AssertsBecauseGiven_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_"
        "CallTo_CF_CFDP_SendNak_CannotReturn_CF_SEND_ERROR_ButDoesAnyway");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_SendNak_DoesCF_SEND_SUCCESS_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_Returns_non0_CallTo_CF_CFDP_"
        "SendNak_DoesCF_SEND_SUCCESS_Return_0");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsLessThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_"
        "IsLessThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsEqTo_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_"
        "IsEqTo_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0");
    UtTest_Add(
        Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_IsGreaterThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_SubstateSendNak_Given_t_md_recv_Is_1_CallTo_CF_Chunks_ComputeGaps_When_t_chunks_chunks_count_"
        "IsGreaterThan_t_chunks_chunks_CF_max_chunks_Returns_0_Return_0");
} /* end add_CF_CFDP_R_SubstateSendNak_tests */

void add_CF_CFDP_R_Init_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreate_ReturnsPositiveValueSet_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreate_ReturnsPositiveValueSet_sub_state_To_RECV_"
        "FILEDATA");
    UtTest_Add(
        Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_open_AndCall_CFDP_R1_Reset,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Init_StateIsNot_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_"
        "open_AndCall_CFDP_R1_Reset");
    UtTest_Add(
        Test_CF_CFDP_R_Init_StateIs_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_open_But_t_state_IsEqTo_CFDP_R2_SoCall_CFDP_R2_SetCc,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Init_StateIs_CFDP_R2_AndCallTo_CF_WrappedCreat_ReturnedNegativeSendEventThenIncrement_file_"
        "open_But_t_state_IsEqTo_CFDP_R2_SoCall_CFDP_R2_SetCc");
    UtTest_Add(
        Test_CF_CFDP_R_Init_StateIs_CFDP_R2_And_t_flags_rx_md_recv_Is_0_SendEventThenCallTo_CF_WrappedCreat_Returned_0_Set_t_state_data_r_sub_state_To_RECV_FILEDATA,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Init_StateIs_CFDP_R2_And_t_flags_rx_md_recv_Is_0_SendEventThenCallTo_CF_WrappedCreat_Returned_"
        "0_Set_t_state_data_r_sub_state_To_RECV_FILEDATA");
} /* end add_CF_CFDP_R_Init_tests */

void add_CF_CFDP_R2_CalcCrcChunk_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_count_bytes_IsGreaterThan_rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_count_bytes_IsGreaterThan_"
        "rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_0_Call_CF_CRC_Start_And_count_bytes_IsGreaterThan_rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_0_Call_CF_CRC_Start_And_count_bytes_"
        "IsGreaterThan_rx_crc_calc_bytes_per_wakeup_And_t_state_data_r_r2_rx_crc_calc_bytes_IsNotEqTo_t_fsize_Return_"
        "neg1");
    UtTest_Add(
        Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_CFDP_R_CheckCrc_Return_Non0_Call_CF_CFDP_R2_SetCc_Set_t_flags_rx_crc_calc_To_1_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_"
        "CFDP_R_CheckCrc_Return_Non0_Call_CF_CFDP_R2_SetCc_Set_t_flags_rx_crc_calc_To_1_Return_0");
    UtTest_Add(
        Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_CFDP_R_CheckCrc_Return_0_Set_t_keep_To_1_And_t_state_data_r_r2_cc_To_FIN_COMPLETE_And_t_state_data_r_r2_fs_To_FIN_RETAINED_t_flags_rx_crc_calc_To_1_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_r2_rx_crc_calc_bytes_Is_Non0_And_IsEqTo_t_fsize_CallTo_CF_"
        "CFDP_R_CheckCrc_Return_0_Set_t_keep_To_1_And_t_state_data_r_r2_cc_To_FIN_COMPLETE_And_t_state_data_r_r2_fs_To_"
        "FIN_RETAINED_t_flags_rx_crc_calc_To_1_Return_0");
    UtTest_Add(
        Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueNotEqTo_RXC_SendEventAndSet_t_history_cc_To_CC_FILE_SIZE_ERROR_AndIncrement_fault_file_seek_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueNotEqTo_RXC_SendEventAndSet_t_history_cc_To_"
        "CC_FILE_SIZE_ERROR_AndIncrement_fault_file_seek_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueEqTo_RXC_crc_calc_bytes_CallTo_CF_WrappedRead_ReturnsValueNotEqTo_read_size_WhichIsSetTo_t_fsize_Minus_RXC_SendEventAndSet_t_history_cc_To_CC_FILE_SIZE_ERROR_AndIncrement_fault_file_read_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_CalcCrcChunk_CAllTo_CF_WrappedLseek_ReturnsValueEqTo_RXC_crc_calc_bytes_CallTo_CF_WrappedRead_"
        "ReturnsValueNotEqTo_read_size_WhichIsSetTo_t_fsize_Minus_RXC_SendEventAndSet_t_history_cc_To_CC_FILE_SIZE_"
        "ERROR_AndIncrement_fault_file_read_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_cached_pos_IsEqTo_RXC_CallTo_CF_WrappedRead_ReturnsValueEqTo_read_size_WhichIsSetTo_sizeof_buf_Call_CF_CRC_Digest_ThenIncrease_RXC_ByAdding_read_size_AndSet_t_state_data_r_cached_pos_To_RXC,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_CalcCrcChunk_Given_t_state_data_r_cached_pos_IsEqTo_RXC_CallTo_CF_WrappedRead_"
        "ReturnsValueEqTo_read_size_WhichIsSetTo_sizeof_buf_Call_CF_CRC_Digest_ThenIncrease_RXC_ByAdding_read_size_"
        "AndSet_t_state_data_r_cached_pos_To_RXC");
} /* end add_CF_CFDP_R2_CalcCrcChunk_tests */

void add_CF_CFDP_R2_SubstateSendFin_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_And_t_flags_rx_crc_calc_Is_0_CallTo_CF_CFDP_R2_CalcCrcChunk_Returns_Non0_Return_Neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_And_t_flags_rx_crc_calc_Is_0_CallTo_CF_"
        "CFDP_R2_CalcCrcChunk_Returns_Non0_Return_Neg1");
    UtTest_Add(Test_CF_CFDP_R2_SubstateSendFin_AssertsBecauseCallTo_CF_CFDP_SendFin_Returns_CF_SEND_ERROR,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R2_SubstateSendFin_AssertsBecauseCallTo_CF_CFDP_SendFin_Returns_CF_SEND_ERROR");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_CallTo_CF_CFDP_SendFin_DoesNotReturn_CF_SEND_SUCCESS_Return_neg1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateSendFin_Given_t_history_cc_IsEqTo_CC_NO_ERROR_CallTo_CF_CFDP_SendFin_DoesNotReturn_CF_"
        "SEND_SUCCESS_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateSendFin_Given_t_flags_rx_crc_calc_Is_1_CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateSendFin_Given_t_flags_rx_crc_calc_Is_1_CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_"
        "Return_0");
    UtTest_Add(
        Test_CF_CFDP_R2_SubstateSendFin_CallTo_CF_CFDP_R2_CalcCrcChunk_Returns_0_Given_t_flags_rx_crc_calc_Is_1_CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_Return_0,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_SubstateSendFin_CallTo_CF_CFDP_R2_CalcCrcChunk_Returns_0_Given_t_flags_rx_crc_calc_Is_1_"
        "CallTo_CF_CFDP_SendFin_Returns_CF_SEND_SUCCESS_Return_0");
} /* end add_CF_CFDP_R2_SubstateSendFin_tests */

void add_CF_CFDP_R2_Recv_fin_ack_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_Recv_fin_ack_GetsInvalidFinAckFrom_CF_CFDP_RecvAck_SendEventAndIncrement_recv_error,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R2_Recv_fin_ack_GetsInvalidFinAckFrom_CF_CFDP_RecvAck_SendEventAndIncrement_recv_error");
    UtTest_Add(Test_CF_CFDP_R2_Recv_fin_ack_GetsValidFinAckFrom_CF_CFDP_RecvAck_Calls_CFDP_R2_Reset,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R2_Recv_fin_ack_GetsValidFinAckFrom_CF_CFDP_RecvAck_Calls_CFDP_R2_Reset");
} /* end add_CF_CFDP_R2_Recv_fin_ack_tests */

void add_CF_CFDP_R2_RecvMd_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_1_DoNothing, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_1_DoNothing");
    UtTest_Add(
        Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_ButCallTo_CF_CFDP_RecvMd_Returns_Non_0_SendEventAndIncrease_recv_error,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_ButCallTo_CF_CFDP_RecvMd_Returns_Non_0_"
        "SendEventAndIncrease_recv_error");
    UtTest_Add(
        Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_But_t_state_data_r_r2_eof_size_IsNotEqTo_t_fsize_SendEventAndIncrease_file_size_mismatch_Call_CFDP_R2_SetCc,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_"
        "Is_1_But_t_state_data_r_r2_eof_size_IsNotEqTo_t_fsize_SendEventAndIncrease_file_size_mismatch_Call_CFDP_R2_"
        "SetCc");
    UtTest_Add(
        Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_DoesNotReturn_OS_SUCCESS_SendEventAndUndefine_t_fd_AndCAll_CF_CFDP_R2_SetCc_AndIncrement_fault_file_rename,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_"
        "Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_DoesNotReturn_OS_SUCCESS_"
        "SendEventAndUndefine_t_fd_AndCAll_CF_CFDP_R2_SetCc_AndIncrement_fault_file_rename");
    UtTest_Add(
        Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_ButCallTo_CF_WrappedOpenCreate_ReturnsNegativeSoSendEventAndCall_CF_CFDP_R2_SetCc_AndIncrease_fault_file_open,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_"
        "Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_ButCallTo_CF_"
        "WrappedOpenCreate_ReturnsNegativeSoSendEventAndCall_CF_CFDP_R2_SetCc_AndIncrease_fault_file_open");
    UtTest_Add(
        Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_CallTo_CF_WrappedOpenCreate_ReturnsNotNegativeSoSet_md_recv_To_1_And_nak_To_0_Call_CF_CFDP_R2_Complete,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R2_RecvMd_Given_t_flags_rx_md_recv_Is_0_CallTo_CF_CFDP_RecvMd_Returns_0_And_t_flags_rx_eof_recv_"
        "Is_1_And_t_state_data_r_r2_eof_size_IsEqTo_t_fsize_CallTo_OS_rename_Returns_OS_SUCCESS_CallTo_CF_"
        "WrappedOpenCreate_ReturnsNotNegativeSoSet_md_recv_To_1_And_nak_To_0_Call_CF_CFDP_R2_Complete");
} /* end add_CF_CFDP_R2_RecvMd_tests */

void add_CFDP_R_DispatchRecv_tests(void)
{
    UtTest_Add(Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsEqTo_RECV_NUM_STATES, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsEqTo_RECV_NUM_STATES");
    // UtTest_Add(Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsGreaterThan_RECV_NUM_STATES,
    //   cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
    //   "Test_CFDP_R_DispatchRecv_AssertsBecause_sub_state_IsGreaterThan_RECV_NUM_STATES");
    UtTest_Add(Test_CFDP_R_DispatchRecv_AssertsBecause_msg_in_Is_NULL, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CFDP_R_DispatchRecv_AssertsBecause_msg_in_Is_NULL");
    UtTest_Add(
        Test_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped");
    UtTest_Add(
        Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_EqTo_CC_NO_ERROR_Calls_fd_fn_DispatchRecv_FlagsAreNotSetAnd_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreSetTo_PDU_HDR_FLAGS_TYPE_And_cc_EqTo_CC_NO_ERROR_Calls_fd_fn_"
        "DispatchRecv_FlagsAreNotSetAnd_cc_DoesNotEq_CC_NO_ERROR_Increment_dropped");
    UtTest_Add(
        Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsEqTo_PDU_INVALID_MAX_Increment_spurious_AndSendEvent,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsEqTo_PDU_INVALID_MAX_Increment_"
        "spurious_AndSendEvent");
    UtTest_Add(
        Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsGreaterThan_PDU_INVALID_MAX_Increment_spurious_AndSendEvent,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsGreaterThan_PDU_INVALID_MAX_Increment_"
        "spurious_AndSendEvent");
    UtTest_Add(
        Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_directive_code_Is_NULL_DoNothing,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_"
        "directive_code_Is_NULL_DoNothing");
    UtTest_Add(
        Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_directive_code_IsAFunctionCallThatFunction,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CFDP_RTest_CFDP_R_DispatchRecv_FlagsAreNotSetAnd_directive_code_IsLessThan_PDU_INVALID_MAX_And_fns_state_"
        "directive_code_IsAFunctionCallThatFunction");
} /* end add_CFDP_R_DispatchRecv_tests */

void add_CF_CFDP_R1_Recv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R1_Recv_Runs, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R1_Recv_Runs");
} /* end add_CF_CFDP_R1_Recv_tests */

void add_CF_CFDP_R2_Recv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R2_Recv_Runs, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R2_Recv_Runs");
} /* end add_CF_CFDP_R2_Recv_tests */

void add_CF_CFDP_R_Cancel_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Set_send_fin_To_1,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Set_send_fin_"
        "To_1");
    UtTest_Add(
        Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsLessThan_RECV_WAIT_FOR_FIN_ACK_Calls_"
        "CFDP_R1_Reset_With_t");
    UtTest_Add(
        Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_"
        "Reset_With_t");
    UtTest_Add(
        Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Cancel_When_t_state_IsEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_"
        "CFDP_R1_Reset_With_t");
    UtTest_Add(
        Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_CFDP_R1_Reset_With_t,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Cancel_When_t_state_IsNotEqTo_CFDP_R2_And_sub_state_IsGreaterThan_RECV_WAIT_FOR_FIN_ACK_Calls_"
        "CFDP_R1_Reset_With_t");
} /* end add_CF_CFDP_R_Cancel_tests */

void add_CF_CFDP_R_SendInactivityEvent_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R_SendInactivityEvent_SendEventAndIncrement_inactivity_timer, cf_cfdp_r_tests_Setup,
               cf_cfdp_r_tests_Teardown, "Test_CF_CFDP_R_SendInactivityEvent_SendEventAndIncrement_inactivity_timer");
} /* end add_CF_CFDP_R_SendInactivityEvent_tests */

void add_CF_CFDP_R_Tick_tests(void)
{
    UtTest_Add(Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick,
               cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_1_Call_CF_CFDP_R_SendInactivityEvent_And_CF_CFDP_R1_Reset,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsNotEqTo_CFDP_R2_And_CF_Timer_Expired_Returns_1_Call_CF_CFDP_R_"
        "SendInactivityEvent_And_CF_CFDP_R1_Reset");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_But_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_But_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_"
        "0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_0_"
        "Call_CF_Timer_Tick_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_"
        "DoNothing");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_1_Call_CF_CFDP_R_SendInactivityEvent_AndCAll_CF_CFDP_R2_SetCc_Set_inactivity_fired_To_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_0_And_CF_Timer_Expired_Returns_1_"
        "Call_CF_CFDP_R_SendInactivityEvent_AndCAll_CF_CFDP_R2_SetCc_Set_inactivity_fired_To_1_And_send_ack_Is_0_And_"
        "send_nak_Is_0_And_send_fin_Is_0_AndThen_ack_timer_armed_Is_0_DoNothing");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_AssertsBecause_CF_CFDP_SendAck_Returns_CF_SEND_ERROR,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_AssertsBecause_"
        "CF_CFDP_SendAck_Returns_CF_SEND_ERROR");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_SendAck_Returns_NotCF_SEND_ERROR_NorCF_SEND_NO_MSG_Set_send_ack_To_0_And_ack_timer_armed_Is_0_DoNothingElse,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_"
        "SendAck_Returns_NotCF_SEND_ERROR_NorCF_SEND_NO_MSG_Set_send_ack_To_0_And_ack_timer_armed_Is_0_DoNothingElse");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_SendAck_Returns_CF_SEND_NO_MSG_DoNotSet_send_ack_And_ack_timer_armed_Is_0_DoNothingElse,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_1_CallTo_CF_CFDP_"
        "SendAck_Returns_CF_SEND_NO_MSG_DoNotSet_send_ack_And_ack_timer_armed_Is_0_DoNothingElse");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_0_Set_send_nak_To_0_And_ack_timer_armed_Is_0_DoNothingElse,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_"
        "1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_0_Set_send_nak_To_0_And_ack_timer_armed_Is_0_DoNothingElse");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_non0_And_ack_timer_armed_Is_0_DoNothingElse,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_"
        "1_Call_To_CF_CFDP_R_SubstateSendNak_Returns_non0_And_ack_timer_armed_Is_0_DoNothingElse");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_0_Set_send_fin_To_0_And_ack_timer_armed_Is_0_DoNothingElse,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_"
        "0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_0_Set_send_fin_To_0_And_ack_timer_armed_Is_0_"
        "DoNothingElse");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_Non0_And_ack_timer_armed_Is_0_DoNothingElse,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_Given_t_state_IsEqTo_CFDP_R2_And_inactivity_fired_Is_1_And_send_ack_Is_0_And_send_nak_Is_"
        "0_And_send_fin_Is_1_CAllTo_CF_CFDP_R2_SubstateSendFin_Returns_Non0_And_ack_timer_armed_Is_0_DoNothingElse");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_0_Call_CF_Timer_Tick");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_t_state_data_r_sub_state_IsNotEqTo_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_ArmAckTimer,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_"
        "complete_Is_non0_t_state_data_r_sub_state_IsNotEqTo_RECV_WAIT_FOR_FIN_ACK_Call_CF_CFDP_ArmAckTimer");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_0_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_"
        "complete_Is_0_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsNotEqTo_ack_limit_Set_send_fin_To_1_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_"
        "complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsNotEqTo_ack_limit_Set_"
        "send_fin_To_1_Call_CF_CFDP_R2_Complete_Then_Call_CF_CFDP_ArmAckTimer");
    UtTest_Add(
        Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsEqTo_ack_limit_SendEvent_Increment_fault_ack_limit_Call_CF_CFDP_R2_Reset,
        cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
        "Test_CF_CFDP_R_Tick_NothingElseSet_ack_timer_armed_Is_1_CAllTo_CF_Timer_Expired_Returns_non0_t_flags_rx_"
        "complete_Is_non0_And_t_state_datat_r_sub_state_IsEqTo_RECV_WAIT_FOR_FIN_ACK_And_ack_IsEqTo_ack_limit_"
        "SendEvent_Increment_fault_ack_limit_Call_CF_CFDP_R2_Reset");
} /* end add_CF_CFDP_R_Tick_tests */

/* end cf_cfdp_r tests UtTest_Add groups */

/*******************************************************************************
**
** cf_cfdp_r_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_CFDP_R2_SetCc_tests();

    add_CFDP_R1_Reset_tests();

    add_CFDP_R2_Reset_tests();

    add_CF_CFDP_R_CheckCrc_tests();

    add_CF_CFDP_R2_Complete_tests();

    add_CF_CFDP_R_ProcessFd_tests();

    add_CF_CFDP_R_SubstateRecvEof_tests();

    add_CF_CFDP_R1_SubstateRecvEof_tests();

    add_CF_CFDP_R2_SubstateRecvEof_tests();

    add_CF_CFDP_R1_SubstateRecvFileData_tests();

    add_CF_CFDP_R2_SubstateRecvFileData_tests();

    add_CF_CFDP_R2_GapCompute_tests();

    add_CF_CFDP_R_SubstateSendNak_tests();

    add_CF_CFDP_R_Init_tests();

    add_CF_CFDP_R2_CalcCrcChunk_tests();

    add_CF_CFDP_R2_SubstateSendFin_tests();

    add_CF_CFDP_R2_Recv_fin_ack_tests();

    add_CF_CFDP_R2_RecvMd_tests();

    add_CFDP_R_DispatchRecv_tests();

    add_CF_CFDP_R1_Recv_tests();

    add_CF_CFDP_R2_Recv_tests();

    add_CF_CFDP_R_Cancel_tests();

    add_CF_CFDP_R_SendInactivityEvent_tests();

    add_CF_CFDP_R_Tick_tests();

} /* end UtTest_Setup for cf_cfdp_r_tests.c */

/* end cf_cfdp_r_tests.c */
