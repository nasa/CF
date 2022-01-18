/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_cmd.h"
#include "cf_events.h"
#include "cf_test_alt_handler.h"

/*
 * In order to properly instantiate buffers to pass to functions that
 * accept a CFE_SB_Buffer_t, a union must be used to align the
 * data.
 */

typedef union
{
    CF_UnionArgsCmd_t ua;
    CFE_SB_Buffer_t   buf;
} CF_UT_cmd_unionargs_buf_t;

typedef union
{
    CF_SetParamCmd_t sp;
    CFE_SB_Buffer_t  buf;
} CF_UT_cmd_set_param_args_buf_t;

typedef union
{
    CF_GetParamCmd_t gp;
    CFE_SB_Buffer_t  buf;
} CF_UT_cmd_get_param_args_buf_t;

typedef union
{
    CF_TxFileCmd_t  tf;
    CFE_SB_Buffer_t buf;
} CF_UT_cmd_tx_file_buf_t;

typedef union
{
    CF_PlaybackDirCmd_t pd;
    CFE_SB_Buffer_t     buf;
} CF_UT_cmd_playback_dir_buf_t;

typedef union
{
    CF_TransactionCmd_t xact;
    CFE_SB_Buffer_t     buf;
} CF_UT_cmd_transaction_buf_t;

typedef union
{
    CF_WriteQueueCmd_t wq;
    CFE_SB_Buffer_t    buf;
} CF_UT_cmd_write_q_buf_t;

/*******************************************************************************
**
**  cf_cmd_tests Setup and Teardown
**
*******************************************************************************/

void cf_cmd_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_cmd_tests_Setup */

void cf_cmd_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cmd_tests_Teardown */

/* end cf_cmd_tests Setup and Teardown */

/*******************************************************************************
**
**  cf_cmd_tests specific Any functions  NOTE:Some of these may be better as global
**
*******************************************************************************/

CF_CFDP_Class_t Any_cfdp_class_t(void)
{
    return (CF_CFDP_Class_t)AnyCoinFlip();
}

CF_EntityId_t Any_CF_EntityId_t(void)
{
    return (CF_EntityId_t)Any_uint8();
}

/* uint8 used for Any_cf_channel likely there will never be that many channels */
uint8 Any_cf_channel(void)
{
    return Any_uint8_LessThan(CF_NUM_CHANNELS);
}

/* uint8 used for Any_cf_polldir likely there will never be that many polldirs */
uint8 Any_cf_polldir(void)
{
    return Any_uint8_LessThan(CF_MAX_POLLING_DIR_PER_CHAN);
}

/* bool_arg_t_barg should only be 0 or 1 (Boolean) */
uint8 Any_bool_arg_t_barg(void)
{
    return AnyCoinFlip();
}

uint8 Any_queue_Except_q_pend(void)
{
    /* q_pend = 0, q_active = 1, q_history = 2, q_all = 3 */
    return (rand() % 2) + 1; /* 0-2, + 1 -> 1-3 */
}

CF_TransactionSeq_t Any_CF_TransactionSeq_t(void)
{
    return (CF_TransactionSeq_t)Any_uint32();
}

/* end cf_cmd_tests specific Any functions */

/*******************************************************************************
**
**  cf_cmd_tests dummy test functions
**
*******************************************************************************/

typedef struct
{
    CF_Transaction_t *t;
    void             *context;
} Dummy_CF_TsnChanAction_fn_t_context_t;

int Dummy_chan_action_fn_t(uint8 chan_num, void *context)
{
    /* This one does not need to save its context, just call default so count works */
    return UT_DEFAULT_IMPL(Dummy_chan_action_fn_t);
}

void Dummy_CF_TsnChanAction_fn_t(CF_Transaction_t *t, void *context)
{
    Dummy_CF_TsnChanAction_fn_t_context_t *ctxt =
        UT_CF_GetContextBuffer(UT_KEY(Dummy_CF_TsnChanAction_fn_t), Dummy_CF_TsnChanAction_fn_t_context_t);

    if (ctxt)
    {
        ctxt->t       = t;
        ctxt->context = context;
    }

    UT_DEFAULT_IMPL(Dummy_CF_TsnChanAction_fn_t);
}

/*******************************************************************************
**
**  CF_CmdAcc tests
**
*******************************************************************************/

void Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter(void)
{
    /* Arrange */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdAcc();

    /* Assert */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter */

/* end CF_CmdAcc tests */

/*******************************************************************************
**
**  CF_CmdRej tests
**
*******************************************************************************/
void Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter(void)
{
    /* Arrange */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdRej();

    /* Assert */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter */

/* end CF_CmdRej tests */

/*******************************************************************************
**
**  CF_CmdCond tests
**
*******************************************************************************/

void Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc(void)
{
    /* Arrange */
    int arg_cond = 0;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdCond(arg_cond);

    /* Assert */
    /* Assert for CF-CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc */

void Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej(void)
{
    /* Arrange */
    int arg_cond = Any_int_Except(0);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdCond(arg_cond);

    /* Assert */
    /* Assert for CF-CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);

} /* Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej */

/* end CF_CmdCond tests */

/*******************************************************************************
**
**  CF_CmdNoop tests
**
*******************************************************************************/

void Test_CF_CmdNoop_SendNoopEventAndAcceptCommand(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdNoop(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_NOOP);
    /* Assert to show CF_CmdAcc was called */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* Test_CF_CmdNoop_SendNoopEventAndAcceptCommand */

/* end CF_CmdNoop tests */

/*******************************************************************************
**
**  CF_CmdReset tests
**
*******************************************************************************/

void Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 5; /* 5 is size of 'names' */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_RESET_INVALID);
    /* Assert to show CF_CmdRej was called */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = Any_uint8_GreaterThan(5); /* 5 is size of 'names' */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_RESET_INVALID);
    /* Assert to show CF_CmdRej was called */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 1; /* 1 is 'cmd' */

    CF_AppData.hk.counters.cmd = Any_uint16_Except(0);
    CF_AppData.hk.counters.err = Any_uint16_Except(0);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_RESET);
    UtAssert_ZERO(CF_AppData.hk.counters.cmd);
    UtAssert_ZERO(CF_AppData.hk.counters.err);
} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent */

void Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    int                       i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 2; /* 2 is 'fault' */

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.fault.file_open          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_read          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_seek          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_write         = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_rename        = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.directory_read     = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.crc_mismatch       = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_size_mismatch = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.nak_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.ack_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.inactivity_timer   = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.spare              = Any_uint16_Except(0);
    }

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_RESET);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.file_open);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.file_read);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.file_seek);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.file_write);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.file_rename);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.directory_read);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.crc_mismatch);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.file_size_mismatch);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.nak_limit);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.ack_limit);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.inactivity_timer);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.fault.spare);
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.fault, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.fault),
                             "fault channel %d was completely cleared to 0", i);
    }
    /* Assert to show CF_CmdAcc was called */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    int                       i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 3; /* 3 is 'up' */

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.recv.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.pdu                  = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.error                = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.spurious             = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.dropped              = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.nak_segment_requests = Any_uint32_Except(0);
    }

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_RESET);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.recv.file_data_bytes);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.recv.pdu);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.recv.error);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.recv.spurious);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.recv.pdu);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.recv.nak_segment_requests);
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.recv, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.recv),
                             "recv channel %d was completely cleared to 0", i);
    }
    /* Assert to show CF_CmdAcc was called */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand */

void Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    uint8                     i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 4; /* 4 is 'down' */

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.sent.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.nak_segment_requests = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.pdu                  = Any_uint32_Except(0);
    }

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_RESET);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.sent.file_data_bytes);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.sent.nak_segment_requests);
        UtAssert_ZERO(CF_AppData.hk.channel_hk[i].counters.sent.pdu);
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.sent, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.sent),
                             "sent channel %d was completely cleared to 0", i);
    }
    /* Assert to show CF_CmdAcc was called */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand */

void Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;
    int                       i         = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->data.byte[0] = 0; /* 0 is 'all' */

    CF_AppData.hk.counters.cmd = Any_uint16_Except(0);
    CF_AppData.hk.counters.err = Any_uint16_Except(0);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.fault.file_open          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_read          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_seek          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_write         = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_rename        = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.directory_read     = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.crc_mismatch       = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.file_size_mismatch = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.nak_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.ack_limit          = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.inactivity_timer   = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.fault.spare              = Any_uint16_Except(0);
    }

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.recv.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.pdu                  = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.error                = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.spurious             = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.dropped              = Any_uint16_Except(0);
        CF_AppData.hk.channel_hk[i].counters.recv.nak_segment_requests = Any_uint32_Except(0);
    }

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].counters.sent.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.nak_segment_requests = Any_uint32_Except(0);
        CF_AppData.hk.channel_hk[i].counters.sent.pdu                  = Any_uint32_Except(0);
    }

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    CF_CmdReset(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_RESET);

    UtAssert_ZERO(CF_AppData.hk.counters.cmd);
    UtAssert_ZERO(CF_AppData.hk.counters.err);
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.fault, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.fault),
                             "fault channel %d was completely cleared to 0", i);
    }
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.recv, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.recv),
                             "recv channel %d was completely cleared to 0", i);
    }
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[i].counters.sent, 0,
                             sizeof(&CF_AppData.hk.channel_hk[i].counters.sent),
                             "sent channel %d was completely cleared to 0", i);
    }
} /* end Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent */

/* end CF_CmdReset tests */

/*******************************************************************************
**
**  CF_CmdTxFile tests
**
*******************************************************************************/

void Test_CF_CmdTxFile(void)
{
    /* Test case for:
     * void CF_CmdTxFile(CFE_SB_Buffer_t *msg);
     */
    CF_UT_cmd_tx_file_buf_t utbuf;
    CF_TxFileCmd_t         *msg = &utbuf.tf;

    memset(&CF_AppData.hk.counters, 0, sizeof(CF_AppData.hk.counters));

    /* nominal, all zero should pass checks, just calls CF_CFDP_TxFile */
    memset(msg, 0, sizeof(*msg));
    UtAssert_VOIDCALL(CF_CmdTxFile(&utbuf.buf));
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, 1);

    /* out of range arguments: bad class */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = 10;
    UtAssert_VOIDCALL(CF_CmdTxFile(&utbuf.buf));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_BAD_PARAM);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 1);

    /* out of range arguments: bad channel */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = CF_NUM_CHANNELS;
    UtAssert_VOIDCALL(CF_CmdTxFile(&utbuf.buf));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_BAD_PARAM);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 2);

    /* out of range arguments: bad keep */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    memset(msg, 0, sizeof(*msg));
    msg->keep = 15;
    UtAssert_VOIDCALL(CF_CmdTxFile(&utbuf.buf));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_BAD_PARAM);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 3);
}

/* end CF_CmdTxFile tests */

/*******************************************************************************
**
**  CF_CmdPlaybackDir tests
**
*******************************************************************************/

void Test_CF_CmdPlaybackDir(void)
{
    /* Test case for:
     * void CF_CmdPlaybackDir(CFE_SB_Buffer_t *msg);
     */
    CF_UT_cmd_playback_dir_buf_t utbuf;
    CF_PlaybackDirCmd_t         *msg = &utbuf.pd;

    memset(&CF_AppData.hk.counters, 0, sizeof(CF_AppData.hk.counters));

    /* nominal, all zero should pass checks, just calls CF_CFDP_PlaybackDir */
    memset(msg, 0, sizeof(*msg));
    UtAssert_VOIDCALL(CF_CmdPlaybackDir(&utbuf.buf));
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, 1);

    /* out of range arguments: bad class */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = 10;
    UtAssert_VOIDCALL(CF_CmdPlaybackDir(&utbuf.buf));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_BAD_PARAM);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 1);

    /* out of range arguments: bad channel */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = CF_NUM_CHANNELS;
    UtAssert_VOIDCALL(CF_CmdPlaybackDir(&utbuf.buf));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_BAD_PARAM);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 2);

    /* out of range arguments: bad keep */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    memset(msg, 0, sizeof(*msg));
    msg->keep = 15;
    UtAssert_VOIDCALL(CF_CmdPlaybackDir(&utbuf.buf));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_BAD_PARAM);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 3);
}

/* end CF_CmdPlaybackDir tests */

/*******************************************************************************
**
**  CF_DoChanAction tests
**
*******************************************************************************/

void Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t         arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context    = &dummy_context;
    uint8                     random_fn_call = Any_uint8_LessThan(CF_NUM_CHANNELS) + 1;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = ALL_CHANNELS;

    UT_SetDeferredRetcode(UT_KEY(Dummy_chan_action_fn_t), random_fn_call, 1);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 1, "CF_DoChanAction returned %d and should be 1 (an fn returned 1)", local_result);

} /* end Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1 */

void Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t         arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = ALL_CHANNELS;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 1);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 1, "CF_DoChanAction returned %d and should be 1 (an fn returned 1)", local_result);

} /* end Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1 */

void Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t         arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = ALL_CHANNELS;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 0);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoChanAction returned %d and should be 0 (all fn returned 0)", local_result);

} /* end Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0 */

void Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t         arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = Any_cf_channel();

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 1);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 1, "CF_DoChanAction returned %d and should be 1 (fn returned 1)", local_result);

} /* end Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1 */

void Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t         arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = Any_cf_channel();

    UT_SetDefaultReturnValue(UT_KEY(Dummy_chan_action_fn_t), 0);

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoChanAction returned %d and should be 0 (fn returned 0)", local_result);

} /* end Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1 */

void Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t         arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[0] = CF_NUM_CHANNELS;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_CHAN_PARAM);

    UtAssert_True(local_result == -1,
                  "CF_DoChanAction returned %d and should be -1 (cmd->data.byte[0] >= CF_NUM_CHANNELS)", local_result);

} /* end Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_ */

void Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd    = &utbuf.ua;
    const char               *arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t         arg_fn     = &Dummy_chan_action_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    /* force ALL_CHANNELS to not be a selection possibility */
    arg_cmd->data.byte[0] = ALL_CHANNELS;
    int catastrophe_count = 0;
    while (arg_cmd->data.byte[0] == ALL_CHANNELS)
    {
        if (catastrophe_count == 10) // 10 is arbitrary
        {
            UtAssert_Message(UTASSERT_CASETYPE_ABORT, __FILE__, __LINE__,
                             "CANNOT make arg_cmd->data.byte[0] != ALL_CHANNELS in 10 tries");
        }

        arg_cmd->data.byte[0] = Any_uint8_GreaterThan_or_EqualTo(CF_NUM_CHANNELS);
        ++catastrophe_count;
    }

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    local_result = CF_DoChanAction(arg_cmd, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Dummy_chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_chan_action_fn_t, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_CHAN_PARAM);

    UtAssert_True(local_result == -1,
                  "CF_DoChanAction returned %d and should be -1 (cmd->data.byte[0] >= CF_NUM_CHANNELS)", local_result);

} /* end Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent */

/* end CF_DoChanAction tests */

/*******************************************************************************
**
**  CF_DoFreezeThaw tests
**
*******************************************************************************/

void Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0(void)
{
    /* Arrange */
    uint8                          arg_chan_num = Any_cf_channel();
    CF_ChanAction_BoolArg_t        dummy_context;
    const CF_ChanAction_BoolArg_t *arg_context;
    int                            local_result;

    dummy_context.barg = Any_bool_arg_t_barg();

    arg_context = &dummy_context;

    /* set frozen to opposite to ensure change was done - not required for test,
     * but it is helpful for verification that the function did the change */
    CF_AppData.hk.channel_hk[arg_chan_num].frozen = !dummy_context.barg;

    /* Act */
    local_result = CF_DoFreezeThaw(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_chan_num].frozen == dummy_context.barg,
                  "CF_DoFreezeThaw set frozen to %d and should be %d (context->barg))",
                  CF_AppData.hk.channel_hk[arg_chan_num].frozen, dummy_context.barg);
    UtAssert_True(local_result == 0, "CF_DoFreezeThaw returned %d and should be 0 (only returns 0)", local_result);

} /* end Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0 */

/* end CF_DoFreezeThaw tests */

/**************************************************************************
**
**  CF_CmdFreeze tests
**
*******************************************************************************/

void Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    uint8 dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdFreeze(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].frozen == 1,
                  "CF_DoFreezeThaw set frozen to %d and should be 1 (freeze = 1))",
                  CF_AppData.hk.channel_hk[dummy_chan_num].frozen);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand */

/*******************************************************************************
**
**  CF_CmdThaw tests
**
*******************************************************************************/

void Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    uint8 dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdThaw(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].frozen == 0,
                  "CF_DoFreezeThaw set frozen to %d and should be 0 (thaw = 0))",
                  CF_AppData.hk.channel_hk[dummy_chan_num].frozen);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);

} /* end Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand */

/*******************************************************************************
**
**  CF_FindTransactionBySequenceNumberAllChannels tests
**
*******************************************************************************/

void Test_CF_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL(void)
{
    /* Arrange */
    CF_TransactionSeq_t arg_ts  = Any_CF_TransactionSeq_t();
    CF_EntityId_t       arg_eid = Any_CF_EntityId_t();
    CF_Transaction_t   *local_result;
    CF_Transaction_t   *expected_result = NULL;

    CF_FindTransactionBySequenceNumber_context_t context_CF_CFDP_FTBSN;

    context_CF_CFDP_FTBSN.forced_return = NULL;

    UT_SetDataBuffer(UT_KEY(CF_FindTransactionBySequenceNumber), &context_CF_CFDP_FTBSN, sizeof(context_CF_CFDP_FTBSN),
                     false);

    /* Act */
    local_result = CF_FindTransactionBySequenceNumberAllChannels(arg_ts, arg_eid);

    UT_GetStubCount(UT_KEY(CF_FindTransactionBySequenceNumber));

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, CF_NUM_CHANNELS);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_FTBSN.c, CF_AppData.engine.channels);
    UtAssert_UINT32_EQ(context_CF_CFDP_FTBSN.transaction_sequence_number, arg_ts);
    UtAssert_UINT32_EQ(context_CF_CFDP_FTBSN.src_eid, arg_eid);
    UtAssert_ADDRESS_EQ(local_result, expected_result);

} /* end Test_CF_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL */

void Test_CF_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound(void)
{
    /* Arrange */
    CF_TransactionSeq_t arg_ts                   = Any_CF_TransactionSeq_t();
    CF_EntityId_t       arg_eid                  = Any_CF_EntityId_t();
    uint8               number_transaction_match = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_Transaction_t    dummy_return_value;
    CF_Transaction_t   *local_result;
    CF_Transaction_t   *expected_result = &dummy_return_value;

    CF_FindTransactionBySequenceNumber_context_t contexts_CF_CFDP_FTBSN[CF_NUM_CHANNELS];

    /* set non-matching transactions */
    int i = 0;
    for (i = 0; i < number_transaction_match; ++i)
    {
        contexts_CF_CFDP_FTBSN[i].forced_return = NULL;
    }
    /* set matching transaction */
    contexts_CF_CFDP_FTBSN[i].forced_return = &dummy_return_value;

    UT_SetDataBuffer(UT_KEY(CF_FindTransactionBySequenceNumber), &contexts_CF_CFDP_FTBSN,
                     sizeof(contexts_CF_CFDP_FTBSN), false);

    /* Act */
    local_result = CF_FindTransactionBySequenceNumberAllChannels(arg_ts, arg_eid);

    UT_GetStubCount(UT_KEY(CF_FindTransactionBySequenceNumber));

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, number_transaction_match + 1);
    for (i = 0; i < number_transaction_match; ++i)
    {
        UtAssert_ADDRESS_EQ(contexts_CF_CFDP_FTBSN[i].c, CF_AppData.engine.channels + i);
        UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].transaction_sequence_number, arg_ts);
        UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].src_eid, arg_eid);
    }
    UtAssert_ADDRESS_EQ(contexts_CF_CFDP_FTBSN[i].c, CF_AppData.engine.channels + i);
    UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].transaction_sequence_number, arg_ts);
    UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].src_eid, arg_eid);
    UtAssert_ADDRESS_EQ(local_result, expected_result);

} /* end Test_CF_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound */

/*******************************************************************************
**
**  CF_TsnChanAction tests
**
*******************************************************************************/

void Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd = &utbuf.xact;
    char                        dummy_cmdstr[10]; /* 10 is arbitrary */
    const char                  arg_cmdstr[10];   /* 10 is arbitrary */
    CF_TsnChanAction_fn_t       arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                         dummy_context;
    void                       *arg_context = &dummy_context;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(dummy_cmdstr, 10);
    memcpy((char *)arg_cmdstr, &dummy_cmdstr, 10);

    arg_cmd->chan = COMPOUND_KEY;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_FindTransactionBySequenceNumberAllChannels */
    CF_FindTransactionBySequenceNumber_context_t contexts_CF_CFDP_FTBSN[CF_NUM_CHANNELS];

    /* set non-matching transactions */
    int i = 0;
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        contexts_CF_CFDP_FTBSN[i].forced_return = NULL;
    }

    UT_SetDataBuffer(UT_KEY(CF_FindTransactionBySequenceNumber), &contexts_CF_CFDP_FTBSN,
                     sizeof(contexts_CF_CFDP_FTBSN), false);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), -1);

    UT_GetStubCount(UT_KEY(Dummy_CF_TsnChanAction_fn_t));

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_TRANS_NOT_FOUND);

    UtAssert_STUB_COUNT(Dummy_CF_TsnChanAction_fn_t, 0);
} /* end Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail */

void Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t           utbuf;
    CF_TransactionCmd_t                  *arg_cmd = &utbuf.xact;
    char                                  dummy_cmdstr[10]; /* 10 is arbitrary */
    const char                            arg_cmdstr[10];   /* 10 is arbitrary */
    CF_TsnChanAction_fn_t                 arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                                   dummy_context;
    void                                 *arg_context = &dummy_context;
    CF_Transaction_t                      dummy_t;
    Dummy_CF_TsnChanAction_fn_t_context_t context_Dummy_CF_TsnChanAction_fn_t;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(dummy_cmdstr, 10);
    memcpy((char *)arg_cmdstr, &dummy_cmdstr, 10);

    arg_cmd->chan = COMPOUND_KEY;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDataBuffer(UT_KEY(Dummy_CF_TsnChanAction_fn_t), &context_Dummy_CF_TsnChanAction_fn_t,
                     sizeof(context_Dummy_CF_TsnChanAction_fn_t), false);

    /* Arrange unstubbable: CF_FindTransactionBySequenceNumberAllChannels */
    CF_FindTransactionBySequenceNumber_context_t context_CF_CFDP_FTBSN;

    /* set matching transaction */
    context_CF_CFDP_FTBSN.forced_return = &dummy_t;

    UT_SetDataBuffer(UT_KEY(CF_FindTransactionBySequenceNumber), &context_CF_CFDP_FTBSN, sizeof(context_CF_CFDP_FTBSN),
                     false);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), 1);

    UT_GetStubCount(UT_KEY(Dummy_CF_TsnChanAction_fn_t));

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(Dummy_CF_TsnChanAction_fn_t, 1);
    UtAssert_ADDRESS_EQ(context_Dummy_CF_TsnChanAction_fn_t.t, &dummy_t);
    UtAssert_ADDRESS_EQ(context_Dummy_CF_TsnChanAction_fn_t.context, arg_context);
} /* end Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS */

void Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t                       utbuf;
    CF_TransactionCmd_t                              *arg_cmd = &utbuf.xact;
    char                                              dummy_cmdstr[10]; /* 10 is arbitrary */
    const char                                        arg_cmdstr[10];   /* 10 is arbitrary */
    CF_TsnChanAction_fn_t                             arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                                               dummy_context;
    void                                             *arg_context     = &dummy_context;
    int                                               expected_result = Any_int();
    CF_TraverseAllTransactions_All_Channels_context_t context_CF_TATAC;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(dummy_cmdstr, 10);
    memcpy((char *)arg_cmdstr, &dummy_cmdstr, 10);

    context_CF_TATAC.forced_return = expected_result;

    arg_cmd->chan = ALL_CHANNELS;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions_All_Channels), &context_CF_TATAC, sizeof(context_CF_TATAC),
                     false);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), expected_result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels */

void Test_CF_TsnChanAction_cmd_chan_IsASingleChannel(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t          utbuf;
    CF_TransactionCmd_t                 *arg_cmd       = &utbuf.xact;
    const char                           arg_cmdstr[1] = "";
    CF_TsnChanAction_fn_t                arg_fn        = &Dummy_CF_TsnChanAction_fn_t;
    int                                  dummy_context;
    void                                *arg_context     = &dummy_context;
    int                                  expected_result = Any_int();
    CF_TraverseAllTransactions_context_t context_CF_TraverseAllTransactions;

    memset(&utbuf, 0, sizeof(utbuf));
    arg_cmd->chan = Any_cf_channel();

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions), &context_CF_TraverseAllTransactions,
                     sizeof(context_CF_TraverseAllTransactions), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), expected_result);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), expected_result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* TODO: values for context_CF_TraverseAllTransactions need checked!! */
} /* end Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels */

void Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *arg_cmd       = &utbuf.xact;
    const char                  arg_cmdstr[1] = "";
    CF_TsnChanAction_fn_t       arg_fn        = &Dummy_CF_TsnChanAction_fn_t;
    int                         dummy_context;
    void                       *arg_context = &dummy_context;

    memset(&utbuf, 0, sizeof(utbuf));
    arg_cmd->chan = Any_uint8_BetweenExcludeMax(CF_NUM_CHANNELS, COMPOUND_KEY);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), -1);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_TSN_CHAN_INVALID);

} /* end Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid */

/*******************************************************************************
**
**  CF_DoSuspRes_Txn tests
**
*******************************************************************************/

void Test_CF_DoSuspRes_Txn_Asserts_t_Is_NULL(void)
{
    // /* Arrange */

    // /* Act */

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_DoSuspRes_Txn_Asserts_t_Is_NULL */

void Test_CF_DoSuspRes_Txn_Set_context_same_To_1_suspended_Eq_action(void)
{
    /* Arrange */
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_ChanAction_SuspResArg_t  dummy_context;
    CF_ChanAction_SuspResArg_t *arg_context = &dummy_context;

    /* set same to 0 to ensure change was done - not required for test,
     * but it is helpful for verification that the function did the change */
    arg_context->same   = 0;
    arg_context->action = AnyCoinFlip();

    arg_t->flags.com.suspended = arg_context->action;

    /* Act */
    CF_DoSuspRes_Txn(arg_t, arg_context);

    /* Assert */
    UtAssert_True(arg_context->same == 1, "CF_DoSuspRes_Txn set context->same to %d and should be 1 (direct set)",
                  arg_context->same);
} /* end Test_CF_DoSuspRes_Txn_Set_context_same_To_1_suspended_Eq_action */

void Test_CF_DoSuspRes_Txn_When_suspended_NotEqTo_action_Set_suspended_To_action(void)
{
    /* Arrange */
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_ChanAction_SuspResArg_t  dummy_context;
    CF_ChanAction_SuspResArg_t *arg_context = &dummy_context;

    /* set same to 0 to ensure change was done - not required for test,
     * but it is helpful for verification that the function did the change */
    arg_context->same   = 0;
    arg_context->action = AnyCoinFlip();

    arg_t->flags.com.suspended = !arg_context->action;

    /* Act */
    CF_DoSuspRes_Txn(arg_t, arg_context);

    /* Assert */
    UtAssert_True(arg_t->flags.com.suspended == arg_context->action,
                  "CF_DoSuspRes_Txn set arg_t->flags.com.suspended to %d and should be %d (context->action)",
                  arg_t->flags.com.suspended, arg_context->action);
} /* end Test_CF_DoSuspRes_Txn_When_suspended_NotEqTo_action_Set_suspended_To_action */

/*******************************************************************************
**
**  CF_DoSuspRes tests
**
*******************************************************************************/

static void UT_AltHandler_CF_TraverseAllTransactions_SetSuspResArg(void *UserObj, UT_EntryKey_t FuncKey,
                                                                   const UT_StubContext_t *Context)
{
    CF_ChanAction_SuspResArg_t *context = UT_Hook_GetArgValueByName(Context, "context", CF_ChanAction_SuspResArg_t *);
    CF_ChanAction_SuspResArg_t *utargs  = UserObj;

    if (context != NULL && utargs != NULL)
    {
        /* Update the caller-supplied context with the UT-supplied value */
        /* only "same" flag is an output, action is an input */
        context->same = utargs->same;
    }
}

void Test_CF_DoSuspRes(void)
{
    /* Test case for:
     * void CF_DoSuspRes(CF_TransactionCmd_t *cmd, uint8 action)
     */

    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *cmd = &utbuf.xact;
    CF_ChanAction_SuspResArg_t  utargs;

    memset(&CF_AppData.hk.counters, 0, sizeof(CF_AppData.hk.counters));
    memset(&utargs, 0, sizeof(utargs));
    memset(cmd, 0, sizeof(*cmd));

    /* nominal */
    /* With no setup, CF_TsnChanAction() invokes CF_TraverseAllTransactions stub, which returns 0 */
    /* this should increment the reject counter because it did not match any transactions */
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 0));
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 1);

    /* set up to match 1 transaction, should be accepted, but should not generate an event */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDeferredRetcode(UT_KEY(CF_TraverseAllTransactions), 1, 1);
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 1));
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, 1);

    /* Output the CF_ChanAction_SuspResArg_t back to the caller, to set the "same" flag to 1 */
    /* this gets the case where it attempts to set to the same value, and is rejected due to that */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDeferredRetcode(UT_KEY(CF_TraverseAllTransactions), 1, 1);
    utargs.same = 1;
    UT_SetHandlerFunction(UT_KEY(CF_TraverseAllTransactions), UT_AltHandler_CF_TraverseAllTransactions_SetSuspResArg,
                          &utargs);
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 0));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_SUSPRES_SAME);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 2);

    /* Output the CF_ChanAction_SuspResArg_t back to the caller, to set the "same" flag to 1 */
    /* however this time CF_TraverseAllTransactions reports it matched multiple transactions, so it should NOT reject it
     */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDeferredRetcode(UT_KEY(CF_TraverseAllTransactions), 1, 10);
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 1));
    UT_CF_AssertEventID(0);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, 2);
}

/*******************************************************************************
**
**  CF_CmdSuspend tests
**
*******************************************************************************/
/* TODO: SendEvent needs to be able to check all values before action = 1 can be verified in
 * Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1 */
void Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_cmd = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_cmd   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoSuspRes */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    dummy_cmd->chan = Any_uint8_BetweenExcludeMax(CF_NUM_CHANNELS, COMPOUND_KEY);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdSuspend(arg_cmd);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_TSN_CHAN_INVALID);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_SUSPRES_CHAN);

    /* Assert CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1 */

/* end CF_CmdSuspend tests */

/*******************************************************************************
**
**  CF_CmdResume tests
**
*******************************************************************************/
/* TODO: SendEvent needs to be able to check all values before action = 0 can be verified in
 * Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0 */
void Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_cmd = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_cmd   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoSuspRes */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_TsnChanAction  - causes areturn -1*/
    dummy_cmd->chan = Any_uint8_BetweenExcludeMax(CF_NUM_CHANNELS, COMPOUND_KEY);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdResume(arg_cmd);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_TSN_CHAN_INVALID);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_SUSPRES_CHAN);

    /* Assert CF_CmdRej */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0 */

/* end CF_CmdResume tests */

/*******************************************************************************
**
**  CF_CmdCancel_Txn tests
**
*******************************************************************************/

void Test_CF_CmdCancel_Txn_Call_CF_CFDP_CancelTransaction_WithGiven_t(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t       = &dummy_t;
    void             *arg_ignored = NULL;
    CF_Transaction_t *context_CF_CFDP_CancelTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_CancelTransaction), &context_CF_CFDP_CancelTransaction,
                     sizeof(context_CF_CFDP_CancelTransaction), false);

    /* Act */
    CF_CmdCancel_Txn(arg_t, arg_ignored);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_CancelTransaction, arg_t);

} /* end Test_CF_CmdCancel_Txn_Call_CF_CFDP_CancelTransaction_WithGiven_t */

/*******************************************************************************
**
**  CF_CmdCancel tests
**
*******************************************************************************/

void Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_msg = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_msg   = &utbuf.buf;

    // TODO: CF_TsnChanAction result is default with no setup; it should be controlled, but in the essence of time this
    // default value is used to show functionality of the code
    /* Arrange unstubbable: CF_TsnChanAction */
    CF_TraverseAllTransactions_context_t context_CF_TraverseAllTransactions;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->chan = Any_cf_chan_num();

    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions), &context_CF_TraverseAllTransactions,
                     sizeof(context_CF_TraverseAllTransactions), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), 0);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdCancel(arg_msg);

    /* Assert */
    /* Assert for CF_TsnChanAction */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.c, CF_AppData.engine.channels + dummy_msg->chan);
    UtAssert_True(context_CF_TraverseAllTransactions.fn == CF_CmdCancel_Txn,
                  "context_CF_TraverseAllTransactions.fn ==  CF_CmdCancel_Txn");
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.context, NULL);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction */

/*******************************************************************************
**
**  CF_CmdAbandon_Txn tests
**
*******************************************************************************/

void Test_CF_CmdAbandon_Txn_Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0(void)
{
    /* Arrange */
    CF_Transaction_t                   dummy_t;
    CF_Transaction_t                  *arg_t       = &dummy_t;
    void                              *arg_ignored = NULL;
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    CF_CmdAbandon_Txn(arg_t, arg_ignored);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, arg_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 0,
                  "CF_CFDP_CancelTransaction was called with int %d and should be 0 (constant in call)",
                  context_CF_CFDP_ResetTransaction.keep_history);

} /* end Test_CF_CmdAbandon_Txn_Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0 */

/*******************************************************************************
**
**  CF_CmdAbandon tests
**
*******************************************************************************/

void Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_transaction_buf_t utbuf;
    CF_TransactionCmd_t        *dummy_msg = &utbuf.xact;
    CFE_SB_Buffer_t            *arg_msg   = &utbuf.buf;

    // TODO: CF_TsnChanAction result is default with no setup; it should be controlled, but in the essence of time this
    // default value is used to show functionality of the code
    /* Arrange unstubbable: CF_TsnChanAction */
    CF_TraverseAllTransactions_context_t context_CF_TraverseAllTransactions;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->chan = Any_cf_chan_num();

    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions), &context_CF_TraverseAllTransactions,
                     sizeof(context_CF_TraverseAllTransactions), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), 0);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdAbandon(arg_msg);

    /* Assert */
    /* Assert for CF_TsnChanAction */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.c, CF_AppData.engine.channels + dummy_msg->chan);
    UtAssert_True(context_CF_TraverseAllTransactions.fn == CF_CmdAbandon_Txn,
                  "context_CF_TraverseAllTransactions.fn ==  CF_CmdAbandon_Txn");
    UtAssert_ADDRESS_EQ(context_CF_TraverseAllTransactions.context, NULL);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction */

/*******************************************************************************
**
**  CF_DoEnableDisableDequeue tests
**
*******************************************************************************/

void Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg(void)
{
    /* Arrange */
    CF_ConfigTable_t         dummy_config_table;
    uint8                    arg_chan_num = Any_cf_channel();
    CF_ChanAction_BoolArg_t  dummy_context;
    CF_ChanAction_BoolArg_t *arg_context = &dummy_context;

    CF_AppData.config_table = &dummy_config_table;
    dummy_context.barg      = Any_bool_arg_t_barg();

    /* Act */
    CF_DoEnableDisableDequeue(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].dequeue_enabled == dummy_context.barg,
                  "Channel %u dequeue_enabled is %u and should be %u (context->barg)", arg_chan_num,
                  CF_AppData.config_table->chan[arg_chan_num].dequeue_enabled, dummy_context.barg);

} /* end Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg */

/*******************************************************************************
**
**  CF_CmdEnableDequeue tests
**
*******************************************************************************/

void Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t dummy_config_table;
    uint8            dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdEnableDequeue(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled == 1,
                  "CF_CmdEnableDequeue set dequeue_enabled to %d and should be 1 (barg = 1))",
                  CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d\nACCEPTANCE OF COMMAND (+1) SHOULD BE "
                  "THE BEHAVIOR BUT IT IS NOT",
                  CF_AppData.hk.counters.cmd, initial_hk_cmd_counter);
} /* end Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction */

/*******************************************************************************
**
**  CF_CmdDisableDequeue tests
**
*******************************************************************************/

void Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction(void)
{
    /* Arrange */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t dummy_config_table;
    uint8            dummy_chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_chan_num;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdDisableDequeue(arg_msg);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled == 0,
                  "CF_CmdEnableDequeue set dequeue_enabled to %d and should be 0 (barg = 0))",
                  CF_AppData.config_table->chan[dummy_chan_num].dequeue_enabled);
    /* Assert for CF-CmdAcc */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction */

/*******************************************************************************
**
**  CF_DoEnableDisablePolldir tests
**
*******************************************************************************/

void Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg(void)
{
    /* Arrange */
    uint8                       arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t   utbuf;
    CF_UnionArgsCmd_t          *dummy_msg = &utbuf.ua;
    CF_ChanAction_BoolMsgArg_t  dummy_context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &dummy_context;
    CF_ConfigTable_t            dummy_config_table;
    uint8                       expected_enabled;
    int                         local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = ALL_CHANNELS;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();
    expected_enabled   = dummy_context.barg;

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    uint8 current_polldir = 0;

    for (current_polldir = 0; current_polldir < CF_MAX_POLLING_DIR_PER_CHAN; ++current_polldir)
    {
        UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].polldir[current_polldir].enabled == expected_enabled,
                      "Channel %u Polldir %u set to %u and should be %u (context->barg)", arg_chan_num, current_polldir,
                      CF_AppData.config_table->chan[arg_chan_num].polldir[current_polldir].enabled, expected_enabled);
    }
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoEnableDisablePolldir returned %d and should be 0", local_result);
} /* end Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg */

void Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg(void)
{
    /* Arrange */
    uint8                       arg_chan_num  = Any_cf_channel();
    uint8                       dummy_polldir = Any_cf_polldir();
    CF_UT_cmd_unionargs_buf_t   utbuf;
    CF_UnionArgsCmd_t          *dummy_msg = &utbuf.ua;
    CF_ChanAction_BoolMsgArg_t  dummy_context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &dummy_context;
    CF_ConfigTable_t            dummy_config_table;
    uint8                       expected_enabled;
    int                         local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = dummy_polldir;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();
    expected_enabled   = dummy_context.barg;

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].polldir[dummy_polldir].enabled == expected_enabled,
                  "Channel %u Polldir %u set to %u and should be %u (context->barg)", arg_chan_num, dummy_polldir,
                  CF_AppData.config_table->chan[arg_chan_num].polldir[dummy_polldir].enabled, expected_enabled);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_DoEnableDisablePolldir returned %d and should be 0", local_result);
} /* end Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg */

void Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent(void)
{
    /* Arrange */
    uint8                       arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t   utbuf;
    CF_UnionArgsCmd_t          *dummy_msg = &utbuf.ua;
    CF_ChanAction_BoolMsgArg_t  dummy_context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &dummy_context;
    CF_ConfigTable_t            dummy_config_table;
    int                         local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = CF_MAX_POLLING_DIR_PER_CHAN;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UT_CF_AssertEventID(CF_EID_ERR_CMD_POLLDIR_INVALID);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(local_result == -1, "CF_DoEnableDisablePolldir returned %d and should be -1", local_result);
} /* end Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent */

void Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent(void)
{
    /* Arrange */
    uint8                       arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t   utbuf;
    CF_UnionArgsCmd_t          *dummy_msg = &utbuf.ua;
    CF_ChanAction_BoolMsgArg_t  dummy_context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &dummy_context;
    CF_ConfigTable_t            dummy_config_table;
    int                         local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    dummy_msg->data.byte[1] = CF_MAX_POLLING_DIR_PER_CHAN;

    dummy_context.msg  = dummy_msg;
    dummy_context.barg = Any_bool_arg_t_barg();

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UT_CF_AssertEventID(CF_EID_ERR_CMD_POLLDIR_INVALID);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(local_result == -1, "CF_DoEnableDisablePolldir returned %d and should be -1", local_result);
} /* end Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent */

/*******************************************************************************
**
**  CF_CmdEnablePolldir tests
**
*******************************************************************************/

void Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ConfigTable_t          dummy_config_table;
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     dummy_polldir = Any_cf_polldir();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = dummy_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdEnablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled == 1,
                  "Channel %u Polldir %u set to %u and should be 1 (context->barg)", dummy_channel, dummy_polldir,
                  CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled);
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess */

void Test_CF_CmdEnablePolldir_FailWhenActionFail(void)
{
    /* Arrange */
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     error_polldir = Any_uint8_BetweenInclusive(CF_MAX_POLLING_DIR_PER_CHAN, ALL_CHANNELS - 1);
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = error_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdEnablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdEnablePolldir_FailWhenActionFail */

/* end */

/*******************************************************************************
**
**  CF_CmdDisablePolldir tests
**
*******************************************************************************/

void Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ConfigTable_t          dummy_config_table;
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     dummy_polldir = Any_cf_polldir();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = dummy_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdDisablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled == 0,
                  "Channel %u Polldir %u set to %u and should be 0 (context->barg)", dummy_channel, dummy_polldir,
                  CF_AppData.config_table->chan[dummy_channel].polldir[dummy_polldir].enabled);
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.counters.cmd is %d and should be 1 more than %d", CF_AppData.hk.counters.cmd,
                  initial_hk_cmd_counter);
} /* end Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess */

void Test_CF_CmdDisablePolldir_FailWhenActionFail(void)
{
    /* Arrange */
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     error_polldir = Any_uint8_BetweenInclusive(CF_MAX_POLLING_DIR_PER_CHAN, ALL_CHANNELS - 1);
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    dummy_msg->data.byte[1] = error_polldir;

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdDisablePolldir(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
} /* end Test_CF_CmdDisablePolldir_FailWhenActionFail */

/*******************************************************************************
**
**  CF_PurgeHistory tests
**
*******************************************************************************/

void Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT(void)
{
    /* Arrange */
    CF_History_t                   dummy_h;
    CF_CListNode_t                *arg_n = &dummy_h.cl_node;
    CF_Channel_t                   dummy_c;
    CF_Channel_t                  *arg_c = &dummy_c;
    int                            local_result;
    CF_CFDP_ResetHistory_context_t context_CF_CFDP_ResetHistory;

    UT_SetDataBuffer(UT_KEY(CF_ResetHistory), &context_CF_CFDP_ResetHistory, sizeof(context_CF_CFDP_ResetHistory),
                     false);

    /* Act */
    local_result = CF_PurgeHistory(arg_n, arg_c);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetHistory.c, arg_c);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetHistory.h, &dummy_h);
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_PurgeHistory returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);

} /* end Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT */

/*******************************************************************************
**
**  CF_PurgeTransaction tests
**
*******************************************************************************/

void Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT(void)
{
    /* Arrange */
    CF_Transaction_t                   dummy_t;
    CF_CListNode_t                    *arg_n = &dummy_t.cl_node;
    int                                dummy_ignored;
    void                              *arg_ignored = &dummy_ignored;
    int                                local_result;
    CF_CFDP_ResetTransaction_context_t context_CF_CFDP_ResetTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_ResetTransaction), &context_CF_CFDP_ResetTransaction,
                     sizeof(context_CF_CFDP_ResetTransaction), false);

    /* Act */
    local_result = CF_PurgeTransaction(arg_n, arg_ignored);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetTransaction.t, &dummy_t);
    UtAssert_True(context_CF_CFDP_ResetTransaction.keep_history == 0,
                  "CF_CFDP_ResetTransaction received keep_history %u and should be 0 (constant)",
                  context_CF_CFDP_ResetTransaction.keep_history);
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_PurgeHistory returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);

} /* end Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT */

/*******************************************************************************
**
**  CF_DoPurgeQueue tests
**
*******************************************************************************/

void Test_CF_DoPurgeQueue_PendOnly(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t           utbuf;
    CF_UnionArgsCmd_t                  *arg_cmd = &utbuf.ua;
    CF_Channel_t                       *dummy_c;
    CF_CListNode_t                      dummy_start;
    CF_CListNode_t                     *expected_start = &dummy_start;
    int                                 local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 0; /* pend */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_POINTER,
                          &context_CF_CList_Traverse);

    dummy_c                       = &CF_AppData.engine.channels[arg_chan_num];
    dummy_c->qs[CF_QueueIdx_PEND] = expected_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.start, expected_start);
    UtAssert_True(context_CF_CList_Traverse.fn == CF_PurgeTransaction,
                  "context_CF_CList_Traverse.fn ==  CF_PurgeTransaction");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.context, NULL);
    UtAssert_True(local_result == 0, "CF_DoPurgeQueue returned %d and should be 0", local_result);
} /* end Test_CF_DoPurgeQueue_PendOnly */

void Test_CF_DoPurgeQueue_HistoryOnly(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t           utbuf;
    CF_UnionArgsCmd_t                  *arg_cmd = &utbuf.ua;
    CF_Channel_t                       *dummy_c;
    CF_CListNode_t                      dummy_start;
    CF_CListNode_t                     *expected_start = &dummy_start;
    int                                 local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 1; /* history */

    /* set correct context type for CF_CList_Traverse stub */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_POINTER,
                          &context_CF_CList_Traverse);

    dummy_c                       = &CF_AppData.engine.channels[arg_chan_num];
    dummy_c->qs[CF_QueueIdx_HIST] = expected_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.start, expected_start);
    UtAssert_True(context_CF_CList_Traverse.fn == (CF_CListFn_t)CF_PurgeHistory,
                  "context_CF_CList_Traverse.fn ==  (CF_CListFn_t )CF_PurgeHistory");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.context, dummy_c);
    UtAssert_True(local_result == 0, "CF_DoPurgeQueue returned %d and should be 0", local_result);
} /* end Test_CF_DoPurgeQueue_HistoryOnly */

void Test_CF_DoPurgeQueue_Both(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t           utbuf;
    CF_UnionArgsCmd_t                  *arg_cmd = &utbuf.ua;
    CF_Channel_t                       *dummy_c;
    CF_CListNode_t                      dummy_pend_start;
    CF_CListNode_t                     *expected_pend_start = &dummy_pend_start;
    CF_CListNode_t                      dummy_history_start;
    CF_CListNode_t                     *expected_history_start = &dummy_history_start;
    int                                 local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse[2];

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 2; /* both */

    /* set correct context type for CF_CList_Traverse stub */
    /* this must use data buffer hack to pass multiple contexts */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_POINTER, NULL);
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    dummy_c                       = &CF_AppData.engine.channels[arg_chan_num];
    dummy_c->qs[CF_QueueIdx_PEND] = expected_pend_start;
    dummy_c->qs[CF_QueueIdx_HIST] = expected_history_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 2);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[0].start, expected_pend_start);
    UtAssert_True(context_CF_CList_Traverse[0].fn == CF_PurgeTransaction,
                  "context_CF_CList_Traverse[0].fn ==  CF_PurgeTransaction");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[0].context, NULL);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[1].start, expected_history_start);
    UtAssert_True(context_CF_CList_Traverse[1].fn == (CF_CListFn_t)CF_PurgeHistory,
                  "context_CF_CList_Traverse[1].fn ==  (CF_CListFn_t )CF_PurgeHistory");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[1].context, dummy_c);
    UtAssert_True(local_result == 0, "CF_DoPurgeQueue returned %d and should be 0", local_result);
} /* end Test_CF_DoPurgeQueue_Both */

void Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    uint8                     arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd = &utbuf.ua;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = 3; /* 3 is first default value */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_True(local_result == -1, "CF_DoPurgeQueue returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_PURGE_ARG);
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
} /* end Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1 */

void Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    uint8                     arg_chan_num = Any_cf_channel();
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *arg_cmd = &utbuf.ua;
    int                       local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    arg_cmd->data.byte[1] = Any_uint8_GreaterThan_or_EqualTo(3);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, arg_cmd);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_True(local_result == -1, "CF_DoPurgeQueue returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_PURGE_ARG);
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
} /* end Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1 */

/*******************************************************************************
**
**  CF_CmdPurgeQueue tests
**
*******************************************************************************/

void Test_CF_CmdPurgeQueue_FailWhenActionFail(void)
{
    /* Arrange */
    uint8                     dummy_channel = Any_cf_channel();
    uint8                     error_purge   = 3; /* Shortest return from CF_DoPurgeQueue */
    CF_UT_cmd_unionargs_buf_t utbuf;
    CF_UnionArgsCmd_t        *dummy_msg = &utbuf.ua;
    CFE_SB_Buffer_t          *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    dummy_msg->data.byte[0] = dummy_channel;

    /* Arrange unstubbable: CF_DoPurgeQueue */
    dummy_msg->data.byte[1] = error_purge;

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdPurgeQueue(arg_msg);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for CF_CmdCond */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);
}

/*******************************************************************************
**
**  CF_CmdWriteQueue tests
**
*******************************************************************************/

void Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* invalid channel */
    dummy_wq->chan = CF_NUM_CHANNELS;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_CHAN);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand */

void Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* invalid channel */
    dummy_wq->chan = Any_uint8_GreaterThan(CF_NUM_CHANNELS);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_CHAN);

    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand */

void Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* invalid combination up direction, pending queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* 0 is q_pend */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_ARGS);

    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand */

void Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination up direction, NOT pending queue */
    dummy_wq->type  = 1;                         /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = Any_queue_Except_q_pend(); /* 0 is q_pend */

    /* invalid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_INT32_EQ(context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_OPEN);

    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination down direction, pending queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* 0 is q_pend */

    /* invalid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_INT32_EQ(context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_OPEN);

    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = 0;

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_WRITEQ_RX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails
   */

void Test_CF_CmdWriteQueue_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_WRITEQ_RX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile), &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_WRITEHIST_RX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(void)
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_WRITEQ_TX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    int32                                forced_return_CF_WriteTxnQueueDataToFile_1st_call = 0;
    int32                                forced_return_CF_WriteTxnQueueDataToFile_2nd_call = Any_int32_Except(0);
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile[2];

    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile_1st_call);
    UT_SetDeferredRetcode(UT_KEY(CF_WriteTxnQueueDataToFile), SECOND_CALL,
                          forced_return_CF_WriteTxnQueueDataToFile_2nd_call);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_WRITEQ_TX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_WRITEQ_PEND);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(void)
   */

void Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;

    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile), &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* goto out_close */
    int32 context_CF_WrappedClose_fd;

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_WQ_WRITEHIST_TX);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
} /* end
     Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(void)
   */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 4);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Active(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 3);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 0; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_All(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    int32 forced_return_CF_WriteHistoryQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 1; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active */

/* Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Pend IS an error and is handled by a previous test */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_All(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 3; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 3);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_History(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 2; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 1; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active */

void Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend(void)
{
    /* Arrange */
    CF_UT_cmd_write_q_buf_t utbuf;
    CF_WriteQueueCmd_t     *dummy_wq = &utbuf.wq;
    CFE_SB_Buffer_t        *arg_msg  = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    dummy_wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    dummy_wq->type  = 2; /* type_all = 0; type_up = 1, type_down = 2 */
    dummy_wq->queue = 0; /* q_pend = 0; q_active = 1; q_history = 2; q_all = 3 */

    /* valid result from CF_WrappedCreat */
    strncpy(dummy_wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    int32 forced_return_CF_WriteTxnQueueDataToFile = 0;

    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdWriteQueue(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend */

/* end CF_CmdWriteQueue tests */

/*******************************************************************************
**
**  CF_CmdSendCfgParams tests
**
*******************************************************************************/

void Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg = &utbuf;
    CF_ConfigTable_t             dummy_config_table;
    CFE_TIME_SysTime_t           fake_time;
    CFE_SB_TransmitMsg_context_t context_CFE_SB_TransmitMsg;
    CFE_MSG_SetMsgTime_context_t context_CFE_MSG_SetMsgTime;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_config_table.ticks_per_second             = Any_uint32();
    dummy_config_table.rx_crc_calc_bytes_per_wakeup = Any_uint32();
    dummy_config_table.ack_timer_s                  = Any_uint32();
    dummy_config_table.nak_timer_s                  = Any_uint32();
    dummy_config_table.inactivity_timer_s           = Any_uint32();
    dummy_config_table.outgoing_file_chunk_size     = Any_uint16();
    dummy_config_table.ack_limit                    = Any_uint8();
    dummy_config_table.nak_limit                    = Any_uint8();
    dummy_config_table.local_eid                    = Any_uint8();

    CF_AppData.config_table = &dummy_config_table;

    Any_CFE_TIME_SysTime_Set(&fake_time);
    UT_SetDataBuffer(UT_KEY(CFE_TIME_GetTime), &fake_time, sizeof(fake_time), false);

    UT_SetHookFunction(UT_KEY(CFE_MSG_SetMsgTime), UT_Hook_CFE_MSG_SetMsgTime, &context_CFE_MSG_SetMsgTime);
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_Hook_CFE_SB_TransmitMsg, &context_CFE_SB_TransmitMsg);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdSendCfgParams(arg_msg);

    /* Assert */
    UtAssert_UINT32_EQ(CF_AppData.cfg.ticks_per_second, CF_AppData.config_table->ticks_per_second);
    UtAssert_UINT32_EQ(CF_AppData.cfg.rx_crc_calc_bytes_per_wakeup,
                       CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup);
    UtAssert_UINT32_EQ(CF_AppData.cfg.ack_timer_s, CF_AppData.config_table->ack_timer_s);
    UtAssert_UINT32_EQ(CF_AppData.cfg.nak_timer_s, CF_AppData.config_table->nak_timer_s);
    UtAssert_UINT32_EQ(CF_AppData.cfg.inactivity_timer_s, CF_AppData.config_table->inactivity_timer_s);
    UtAssert_UINT32_EQ(CF_AppData.cfg.outgoing_file_chunk_size, CF_AppData.config_table->outgoing_file_chunk_size);
    UtAssert_UINT32_EQ(CF_AppData.cfg.ack_limit, CF_AppData.config_table->ack_limit);
    UtAssert_UINT32_EQ(CF_AppData.cfg.nak_limit, CF_AppData.config_table->nak_limit);
    UtAssert_UINT32_EQ(CF_AppData.cfg.local_eid, CF_AppData.config_table->local_eid);
    UtAssert_STUB_COUNT(CFE_MSG_SetMsgTime, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_SetMsgTime.MsgPtr, &CF_AppData.cfg.tlm_header.Msg);
    UtAssert_UINT32_EQ(context_CFE_MSG_SetMsgTime.Time.Seconds, fake_time.Seconds);
    UtAssert_UINT32_EQ(context_CFE_MSG_SetMsgTime.Time.Subseconds, fake_time.Subseconds);
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
    UtAssert_ADDRESS_EQ(context_CFE_SB_TransmitMsg.MsgPtr, &CF_AppData.cfg.tlm_header.Msg);
    UtAssert_BOOL_TRUE(context_CFE_SB_TransmitMsg.IncrementSequenceCount);

    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand */

/*******************************************************************************
**
**  CF_CmdValidateChunkSize tests
**
*******************************************************************************/

void Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = sizeof(CF_CFDP_PduFileDataContent_t) + 1;
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CmdValidateChunkSize returned %d and should be 1 (failed)", local_result);

} /* end Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1 */

void Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = Any_uint32_GreaterThan(sizeof(CF_CFDP_PduFileDataContent_t));
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CmdValidateChunkSize returned %d and should be 1 (failed)", local_result);

} /* end Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1 */

void Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = sizeof(CF_CFDP_PduFileDataContent_t);
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateChunkSize returned %d and should be 0 (success)", local_result);

} /* end Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0 */

void Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0(void)
{
    /* Arrange */
    uint8  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32 arg_val      = Any_uint32_LessThan_or_EqualTo(sizeof(CF_CFDP_PduFileDataContent_t));
    int    local_result;

    /* Act */
    local_result = CF_CmdValidateChunkSize(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateChunkSize returned %d and should be 0 (success)", local_result);

} /* end Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0 */

/*******************************************************************************
**
**  CF_CmdValidateMaxOutgoing tests
**
*******************************************************************************/

void Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success(void)
{
    /* Arrange */
    uint32 arg_val      = Any_uint32_Except(0);
    uint8  arg_chan_num = Any_uint8(); /* Any_uint8() used here because it shows value does not matter in this test */
    int    local_result;

    // CF_AppData.config_table = &dummy_config_table;
    //  memcpy(CF_AppData.config_table->chan[arg_chan_num].sem_name, dummy_sem_name, 20);

    /* Act */
    local_result = CF_CmdValidateMaxOutgoing(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateMaxOutgoing returned %d and should be 0 (Success)", local_result);

} /* end Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success */

void Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success(void)
{
    /* Arrange */
    uint32 arg_val      = 0;
    uint8  arg_chan_num = Any_cf_chan_num(); /* Any_cf_chan_num used here because value matters to this test */
    CF_ConfigTable_t dummy_config_table;
    int              local_result;

    CF_AppData.config_table = &dummy_config_table;
    memset(CF_AppData.config_table->chan[arg_chan_num].sem_name, (char)Any_uint8_Except(0), 1);

    /* Act */
    local_result = CF_CmdValidateMaxOutgoing(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CmdValidateMaxOutgoing returned %d and should be 0 (Success)", local_result);

} /* end Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success */

void Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail(void)
{
    /* Arrange */
    uint32 arg_val      = 0;
    uint8  arg_chan_num = Any_cf_chan_num(); /* Any_cf_chan_num used here because value matters to this test */
    CF_ConfigTable_t dummy_config_table;
    int              local_result;

    CF_AppData.config_table = &dummy_config_table;
    memset(CF_AppData.config_table->chan[arg_chan_num].sem_name, (char)0, 1);

    /* Act */
    local_result = CF_CmdValidateMaxOutgoing(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CmdValidateMaxOutgoing returned %d and should be 1 (Success)", local_result);

} /* end Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail */

/* end CF_CmdValidateMaxOutgoing tests */

/*******************************************************************************
**
**  CF_CmdGetSetParam tests
**
*******************************************************************************/

void Test_CF_CmdGetSetParam(void)
{
    /* Test cases for:
     * void CF_CmdGetSetParam(uint8 is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num);
     */

    /* Arrange */
    CF_ConfigTable_t    ut_config_table;
    CF_GetSet_ValueID_t param_id;
    uint16              expected_count;

    memset(&ut_config_table, 0, sizeof(ut_config_table));
    memset(&CF_AppData.hk.counters, 0, sizeof(CF_AppData.hk.counters));
    CF_AppData.config_table = &ut_config_table;
    expected_count          = 0;

    /* Nominal: "set" for each parameter */
    for (param_id = 0; param_id < CF_GetSet_ValueID_MAX; ++param_id)
    {
        UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
        UtAssert_VOIDCALL(CF_CmdGetSetParam(1, param_id, 1 + param_id, UT_CFDP_CHANNEL));
        UT_CF_AssertEventID(CF_EID_INF_CMD_GETSET1);
        UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, ++expected_count);
    }

    /* each of the config parameters should have actually been set to a different value */
    UtAssert_UINT32_EQ(ut_config_table.ticks_per_second, 1);
    UtAssert_UINT32_EQ(ut_config_table.rx_crc_calc_bytes_per_wakeup, 2);
    UtAssert_UINT32_EQ(ut_config_table.ack_timer_s, 3);
    UtAssert_UINT32_EQ(ut_config_table.nak_timer_s, 4);
    UtAssert_UINT32_EQ(ut_config_table.inactivity_timer_s, 5);
    UtAssert_UINT32_EQ(ut_config_table.outgoing_file_chunk_size, 6);
    UtAssert_UINT32_EQ(ut_config_table.ack_limit, 7);
    UtAssert_UINT32_EQ(ut_config_table.nak_limit, 8);
    UtAssert_UINT32_EQ(ut_config_table.local_eid, 9);
    UtAssert_UINT32_EQ(ut_config_table.chan[UT_CFDP_CHANNEL].max_outgoing_messages_per_wakeup, 10);

    /* Nominal: "get" for each parameter */
    for (param_id = 0; param_id < CF_GetSet_ValueID_MAX; ++param_id)
    {
        UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
        UtAssert_VOIDCALL(CF_CmdGetSetParam(0, param_id, 1, UT_CFDP_CHANNEL));
        UT_CF_AssertEventID(CF_EID_INF_CMD_GETSET2);
        UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, ++expected_count);
    }

    /* Bad param ID */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_VOIDCALL(CF_CmdGetSetParam(0, CF_GetSet_ValueID_MAX, 0, UT_CFDP_CHANNEL));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_GETSET_PARAM);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 1);

    /* Bad channel ID */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_VOIDCALL(CF_CmdGetSetParam(0, 0, 0, CF_NUM_CHANNELS + 1));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_GETSET_CHAN);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 2);

    /* Validation fail */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UtAssert_VOIDCALL(CF_CmdGetSetParam(1, CF_GetSet_ValueID_outgoing_file_chunk_size,
                                        100 + sizeof(CF_CFDP_PduFileDataContent_t), UT_CFDP_CHANNEL));
    UT_CF_AssertEventID(CF_EID_ERR_CMD_GETSET_VALIDATE);
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 3);
}

/* end CF_CmdGetSetParam tests */

/*******************************************************************************
**
**  CF_CmdSetParam tests
**
*******************************************************************************/

/* TODO: Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value only uses 0 as key, there could be more
 * tests here, but only because CF_CmdGetSetParam is unstubbable, this test mimics
 * Test_CF_CmdGetSetParam_When_is_set_Is_1_And_param_id_Is_0_HasANull_fn_ThenCopy_value_To_ptr_SendEventAndAcceptCommand*/
void Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value(void)
{
    /* Arrange */
    CF_UT_cmd_set_param_args_buf_t utbuf;
    CF_SetParamCmd_t              *dummy_msg = &utbuf.sp;
    CFE_SB_Buffer_t               *arg_msg   = &utbuf.buf;

    /* Arrange unstubbable: CF_CmdGetSetParam */
    CF_ConfigTable_t dummy_config_table;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->key      = 0;
    dummy_msg->value    = Any_uint32();
    dummy_msg->chan_num = 0;

    CF_AppData.config_table = &dummy_config_table;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdSetParam(arg_msg);

    /* Assert */
    UtAssert_UINT32_EQ(CF_AppData.config_table->ticks_per_second, dummy_msg->value);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_GETSET1);
    /* Assert for CF_CmdAcc() */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value */

/* end CF_CmdSetParam tests */

/*******************************************************************************
**
**  CF_CmdGetParam tests
**
*******************************************************************************/

/* TODO: Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_And_0 only uses 0 as key, there could be more
 * tests here, but only because CF_CmdGetSetParam is unstubbable, this test mimics
 * Test_CF_CmdGetSetParam_When_is_set_Is_0_And_param_id_Is_0_MemCopySendEventAndAcceptCommand*/
void Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0(void)
{
    /* Arrange */
    CF_UT_cmd_get_param_args_buf_t utbuf;
    CF_GetParamCmd_t              *dummy_msg = &utbuf.gp;
    CFE_SB_Buffer_t               *arg_msg   = &utbuf.buf;

    memset(&utbuf, 0, sizeof(utbuf));

    dummy_msg->key      = 0;
    dummy_msg->chan_num = Any_cf_chan_num();

    /* Arrange unstubbable: CF_CmdGetSetParam */
    CF_ConfigTable_t dummy_config_table;

    CF_AppData.config_table = &dummy_config_table;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdGetParam(arg_msg);

    /* Assert */
    /* Assert for CF_CmdGetSetParam */
    /* TODO: CANNOT test memcpy because copies it to a local value (the arg value, but it was not passed by ref) and
     * then only used in the SendEvent function which does not track that value as of the writing of this comment. */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_GETSET2);
    /* Assert for CF_CmdAcc() */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0 */

/*******************************************************************************
**
**  CF_CmdEnableEngine tests
**
*******************************************************************************/

void Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg                          = NULL;
    uint32           forced_return_CF_CFDP_InitEngine = CFE_SUCCESS;

    CF_AppData.engine.enabled = 0; /* 0 is not enabled */

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdEnableEngine(arg_msg);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF-CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter */

void Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg                          = NULL;
    uint32           forced_return_CF_CFDP_InitEngine = Any_uint32_Except(CFE_SUCCESS);

    CF_AppData.engine.enabled = 0; /* 0 is not enabled */

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdEnableEngine(arg_msg);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_ENABLE_ENGINE);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter */

void Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    CF_AppData.engine.enabled = 1; /* 1 is enabled */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdEnableEngine(arg_msg);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_ENG_ALREADY_ENA);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter */

/*******************************************************************************
**
**  CF_CmdDisableEngine tests
**
*******************************************************************************/

void Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    CF_AppData.engine.enabled = 1; /* 1 is enabled */

    /* Arrange unstubbable: CF_CmdAcc */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_CmdDisableEngine(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter */

void Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail(void)
{
    /* Arrange */
    CFE_SB_Buffer_t *arg_msg = NULL;

    CF_AppData.engine.enabled = 0; /* 0 is not enabled */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_CmdDisableEngine(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_ENG_ALREADY_DIS);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.counters.err is %d and should be 1 more than %d", CF_AppData.hk.counters.err,
                  initial_hk_err_counter);

} /* end Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail */

/* end CF_CmdDisableEngine tests */

/*******************************************************************************
**
**  CF_ProcessGroundCommand tests
**
*******************************************************************************/

void Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t            forced_return_CFE_MSG_GetFcnCode = CF_NUM_COMMANDS;
    const char                  *expected_Spec                    = "CF: invalid ground command packet cmd_code=0x%02x";
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;
    CFE_EVS_SendEvent_context_t  context_CFE_EVS_SendEvent;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), UT_Hook_CFE_MSG_GetFcnCode, &context_CFE_MSG_GetFcnCode);
    /* CFE_MSG_GetSize does not matter for Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent
     */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Hook_CFE_EVS_SendEvent, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_GCMD_CC);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);

} /* end Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent */

void Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t            forced_return_CFE_MSG_GetFcnCode = Any_uint8_GreaterThan(CF_NUM_COMMANDS);
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;
    const char                  *expected_Spec = "CF: invalid ground command packet cmd_code=0x%02x";
    CFE_EVS_SendEvent_context_t  context_CFE_EVS_SendEvent;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), UT_Hook_CFE_MSG_GetFcnCode, &context_CFE_MSG_GetFcnCode);
    /* CFE_MSG_GetSize does not matter for Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent
     */

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Hook_CFE_EVS_SendEvent, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_GCMD_CC);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);

} /* end Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent */

void Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_CmdRej(
    void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_SB_Buffer_t  *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 0x00; /* 0x00 forces fns[0] which is CF_CmdNoop */
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize    = Any_uint16_Except(
              sizeof(CF_NoArgsCmd_t)); /* sizeof(CF_NoArgsCmd_t) is expected size of CF_CmdNoop, using uint16 as a
                                           reasonable size constraint here as size_t is at least 16 bit */
    const char *expected_Spec = "CF: invalid ground command length for command 0x%02x, expected %d got %zd";
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;
    CFE_MSG_GetSize_context_t    context_CFE_MSG_GetSize;
    CFE_EVS_SendEvent_context_t  context_CFE_EVS_SendEvent;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), UT_Hook_CFE_MSG_GetFcnCode, &context_CFE_MSG_GetFcnCode);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetSize), UT_Hook_CFE_MSG_GetSize, &context_CFE_MSG_GetSize);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Hook_CFE_EVS_SendEvent, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetSize.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CMD_GCMD_LEN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdRej */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);

} /* end Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_CmdRej
   */

void Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_SB_Buffer_t  *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 0x00; /* 0x00 forces fns[0] which is CF_CmdNoop */
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize =
        sizeof(CF_NoArgsCmd_t); /* sizeof(CF_NoArgsCmd_t) is expected size of CF_CmdNoop */
    const char                  *expected_Spec = "CF: No-Op received, Version %d.%d.%d";
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;
    CFE_MSG_GetSize_context_t    context_CFE_MSG_GetSize;
    CFE_EVS_SendEvent_context_t  context_CFE_EVS_SendEvent;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), UT_Hook_CFE_MSG_GetFcnCode, &context_CFE_MSG_GetFcnCode);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetSize), UT_Hook_CFE_MSG_GetSize, &context_CFE_MSG_GetSize);

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Hook_CFE_EVS_SendEvent, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CmdRej */
    uint16 initial_hk_cmd_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetSize.MsgPtr, &arg_msg->Msg);
    /* Assert for CF_CmdNoop */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_INF_CMD_NOOP);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_INFORMATION,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_INFORMATION)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CmdAcc */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);

} /* end Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg */

/* TODO: Not sure if it is even possible to have a NULL get to this check, but
** Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL is forcing it
** to happen.  The reason is, I'm not sure CFE_SB_GetTotalMsgLength can ever actually return 0. */
void Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL(void)
{
    /* Arrange */
    CFE_SB_Buffer_t              utbuf;
    CFE_SB_Buffer_t             *arg_msg                          = &utbuf;
    CFE_MSG_FcnCode_t            forced_return_CFE_MSG_GetFcnCode = 0x0C; /* 0x0C forces a null slot */
    CFE_MSG_Size_t               forced_return_CFE_MSG_GetSize    = 0;
    CFE_MSG_GetFcnCode_context_t context_CFE_MSG_GetFcnCode;
    CFE_MSG_GetSize_context_t    context_CFE_MSG_GetSize;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetFcnCode), UT_Hook_CFE_MSG_GetFcnCode, &context_CFE_MSG_GetFcnCode);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetSize), UT_Hook_CFE_MSG_GetSize, &context_CFE_MSG_GetSize);

    /* Arrange unstubbable: CF_CmdAcc, CF_CmdRej */ /* technically these are NOT called, but makes sense when looking at
                                                       other tests for CF_ProcessGroundCommand */
    uint16 initial_hk_cmd_counter = Any_uint16();
    uint16 initial_hk_err_counter = Any_uint16();

    CF_AppData.hk.counters.cmd = initial_hk_cmd_counter;
    CF_AppData.hk.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ProcessGroundCommand(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetFcnCode.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetSize.MsgPtr, &arg_msg->Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF_CmdAcc */ /* TODO: just to note because of other CF_ProcessGroundCommand tests note, no problem
                                  here with overflow because no values should have been altered */
    UtAssert_True(CF_AppData.hk.counters.cmd == initial_hk_cmd_counter,
                  "CF_AppData.hk.counters.cmd is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.counters.cmd, initial_hk_cmd_counter);
    UtAssert_True(CF_AppData.hk.counters.err == initial_hk_err_counter,
                  "CF_AppData.hk.counters.err is %u and should not have changed from %u (value before call)",
                  CF_AppData.hk.counters.err, initial_hk_err_counter);
} /* end Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL */

/* end CF_ProcessGroundCommand tests */

/*******************************************************************************
**
**  cf_cmd tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CmdAcc_tests(void)
{
    UtTest_Add(Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdAcc_Increment_CF_AppData_hk_cmd_counter");
} /* end add_CF_CmdAcc_tests */

void add_CF_CmdRej_tests(void)
{
    UtTest_Add(Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdRej_Increment_CF_AppData_hk_err_counter");
} /* end add_CF_CmdRej_tests */

void add_CF_CmdCond_tests(void)
{
    UtTest_Add(Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdCond_When_cond_Is_0_Call_CF_CmdAcc");
    UtTest_Add(Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdCond_When_cond_IsNot_0_Call_CF_CmdRej");
} /* end add_CF_CmdCond_tests */

void add_CF_CmdNoop_tests(void)
{
    UtTest_Add(Test_CF_CmdNoop_SendNoopEventAndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdNoop_SendNoopEventAndAcceptCommand");
} /* end add_CF_CmdNoop_tests */

void add_CF_CmdReset_tests(void)
{
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdReset_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdReset_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand");
    UtTest_Add(Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdReset_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent");
} /* end add_CF_CmdReset_tests */

void add_CF_CmdTxFile_tests(void)
{
    UtTest_Add(Test_CF_CmdTxFile, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_CmdTxFile");
} /* end add_CF_CmdTxFile_tests */

void add_CF_CmdPlaybackDir_tests(void)
{
    UtTest_Add(Test_CF_CmdPlaybackDir, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_CmdPlaybackDir");
} /* end add_CF_CmdPlaybackDir_tests */

void add_CF_DoChanAction_tests(void)
{
    UtTest_Add(Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_ALL_CHANNELS_WhenAll_fn_return_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0");
    UtTest_Add(Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1");
    UtTest_Add(Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_");
    UtTest_Add(Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent");
} /* end add_CF_DoChanAction_tests */

void add_CF_DoFreezeThaw_tests(void)
{
    UtTest_Add(Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0");
} /* end add_CF_DoFreezeThaw_tests */

void add_CF_CmdFreeze_tests(void)
{
    UtTest_Add(Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdFreeze_Set_frozen_To_1_AndAcceptCommand");
} /* end add_CF_CmdFreeze_tests */

void add_CF_CmdThaw_tests(void)
{
    UtTest_Add(Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdFThaw_Set_frozen_To_0_AndAcceptCommand");
} /* end add_CF_CmdThaw_tests */

void add_CF_FindTransactionBySequenceNumberAllChannels_tests(void)
{
    UtTest_Add(Test_CF_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL");
    UtTest_Add(Test_CF_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound");
} /* end add_CF_FindTransactionBySequenceNumberAllChannels_tests */

void add_CF_TsnChanAction_tests(void)
{
    UtTest_Add(Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_Eq_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_Eq_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_IsASingleChannel, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_IsASingleChannel");
    UtTest_Add(Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid");
} /* end add_CF_TsnChanAction_tests */

void add_CF_DoSuspRes_Txn_tests(void)
{
    UtTest_Add(Test_CF_DoSuspRes_Txn_Asserts_t_Is_NULL, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoSuspRes_Txn_Asserts_t_Is_NULL");
    UtTest_Add(Test_CF_DoSuspRes_Txn_Set_context_same_To_1_suspended_Eq_action, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoSuspRes_Txn_Set_context_same_To_1_suspended_Eq_action");
    UtTest_Add(Test_CF_DoSuspRes_Txn_When_suspended_NotEqTo_action_Set_suspended_To_action, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoSuspRes_Txn_When_suspended_NotEqTo_action_Set_suspended_To_action");
} /* end add_CF_DoSuspRes_Txn_tests */

void add_CF_DoSuspRes_tests(void)
{
    UtTest_Add(Test_CF_DoSuspRes, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_DoSuspRes");
} /* end add_CF_DoSuspRes_tests */

void add_CF_CmdSuspend_tests(void)
{
    UtTest_Add(Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdSuspend_Call_CF_DoSuspRes_WithGiven_msg_And_action_1");
} /* end add_CF_CmdSuspend_tests */

void add_CF_CmdResume_tests(void)
{
    UtTest_Add(Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdResume_Call_CF_DoSuspRes_WithGiven_msg_And_action_0");
} /* end add_CF_CmdResume_tests */

void add_CF_CmdCancel_Txn_tests(void)
{
    UtTest_Add(Test_CF_CmdCancel_Txn_Call_CF_CFDP_CancelTransaction_WithGiven_t, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdCancel_Txn_Call_CF_CFDP_CancelTransaction_WithGiven_t");
} /* end add_CF_CF_CmdCancel_Txn_tests */

void add_CF_CmdCancel_tests(void)
{
    UtTest_Add(Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdCancel_Call_CF_CmdCond_WithNotted_CF_TsnChanAction");
} /* end add_CF_CmdCancel_tests */

void add_CF_CmdAbandon_Txn_tests(void)
{
    UtTest_Add(Test_CF_CmdAbandon_Txn_Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdAbandon_Txn_Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0");
} /* end add_CF_CmdAbandon_Txn_tests */

void add_CF_CmdAbandon_tests(void)
{
    UtTest_Add(Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdAbandon_Call_CF_CmdCond_WithNotted_CF_TsnChanAction");
} /* end add_CF_CmdAbandon_Txn_tests */

void add_CF_DoEnableDisableDequeue_tests(void)
{
    UtTest_Add(Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg");
} /* end add_CF_DoEnableDisableDequeue_tests */

void add_CF_CmdEnableDequeue_tests(void)
{
    UtTest_Add(Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdEnableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction");
} /* end add_CF_CmdEnableDequeue_tests */

void add_CF_CmdDisableDequeue_tests(void)
{
    UtTest_Add(Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdDisableDequeue_Call_CmdCond_WithResultsOf_CF_DoChanAction");
} /* end add_CF_CmdDisableDequeue_tests */

void add_CF_DoEnableDisablePolldir_tests(void)
{
    UtTest_Add(Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_When_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg");
    UtTest_Add(
        Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg");
    UtTest_Add(Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent");
    UtTest_Add(Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent");
} /* end add_CF_DoEnableDisablePolldir_tests */

void add_CF_CmdEnablePolldir_tests(void)
{
    UtTest_Add(Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdEnablePolldir_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_CmdEnablePolldir_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdEnablePolldir_FailWhenActionFail");
} /* end add_CF_CmdEnablePolldir_tests */

void add_CF_CmdDisablePolldir_tests(void)
{
    UtTest_Add(Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdDisablePolldir_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_CmdDisablePolldir_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdDisablePolldir_FailWhenActionFail");
} /* end add_CF_CmdDisablePolldir_tests */

void add_CF_PurgeHistory_tests(void)
{
    UtTest_Add(Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT");
} /* end add_CF_PurgeHistory_tests */

void add_CF_PurgeTransaction_tests(void)
{
    UtTest_Add(Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT");
} /* end add_CF_PurgeTransaction_tests */

void add_CF_DoPurgeQueue_tests(void)
{
    UtTest_Add(Test_CF_DoPurgeQueue_PendOnly, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoPurgeQueue_PendOnly");
    UtTest_Add(Test_CF_DoPurgeQueue_HistoryOnly, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoPurgeQueue_HistoryOnly");
    UtTest_Add(Test_CF_DoPurgeQueue_Both, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_DoPurgeQueue_Both");
    UtTest_Add(Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1");
    UtTest_Add(Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1");
} /* end add_CF_DoPurgeQueue_tests */

void add_CF_CmdPurgeQueue_tests(void)
{
    UtTest_Add(Test_CF_CmdPurgeQueue_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdPurgeQueue_FailWhenActionFail");
} /* end add_CF_CmdPurgeQueue_tests */

void add_CF_CmdWriteQueue_tests(void)
{
    UtTest_Add(Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdWriteQueue_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_"
        "SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_"
        "CF_"
        "WrappedClose_SendEventClosesAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_"
        "IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_"
        "IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_CmdWriteQueue_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_All");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_History");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Active, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Active");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_AllAnd_q_Pend");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_All, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_All");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_History");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_UpAnd_q_Active");
    /* see Test_CF_CmdWriteQueue_WhenUpAndPendingQueueSendEventAndRejectCommand */
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_All, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_All");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_History, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_History");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Active");
    UtTest_Add(Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdWriteQueue_SuccessCall_CF_CmdAcc_type_DownAnd_q_Pend");
} /* end add_CF_CmdWriteQueue_tests */

void add_CF_CmdSendCfgParams_tests(void)
{
    UtTest_Add(Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdSendCfgParams_Set_cfg_TimeStampAndSendMsg_AcceptCommand");
} /* end add_CF_CmdSendCfgParams_tests */

void add_CF_CmdValidateChunkSize_tests(void)
{
    UtTest_Add(Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateChunkSize_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1");
    UtTest_Add(Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateChunkSize_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1");
    UtTest_Add(Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateChunkSize_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0");
    UtTest_Add(Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdValidateChunkSize_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0");
} /* end add_CF_CmdValidateChunkSize_tests */

void add_CF_CmdValidateMaxOutgoing_tests(void)
{
    UtTest_Add(Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_IsNot_0_Return_0_Success");
    UtTest_Add(Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success");
    UtTest_Add(Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdValidateMaxOutgoing_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail");
} /* end add_CF_CmdValidateMaxOutgoing_tests */

void add_CF_CmdGetSetParam_tests(void)
{
    UtTest_Add(Test_CF_CmdGetSetParam, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_CmdGetSetParam");
} /* end add_CF_CmdGetSetParam_tests */

void add_CF_CmdSetParam_tests(void)
{
    UtTest_Add(Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdSetParam_Call_CF_CmdGetSetParam_With_cmd_key_And_cmd_value");
} /* end add_CF_CmdSetParam_tests */

void add_CF_CmdGetParam_tests(void)
{
    UtTest_Add(Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdGetParam_Call_CF_CmdGetSetParam_With_cmd_data_byte_0_AndConstantValue_0");
} /* end add_CF_CmdGetParam_tests */

void add_CF_CmdEnableEngine_tests(void)
{
    UtTest_Add(Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdEnableEngine_WithEngineNotEnableInitSuccessAndIncrementCmdAccCounter");
    UtTest_Add(Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_CmdEnableEngine_WithEngineNotEnableFailsInitSendEventAndIncrementCmdRejCounter");
    UtTest_Add(Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdEnableEngine_WithEngineEnableFailsSendEventAndIncrementCmdRejCounter");
} /* end add_CF_CmdEnableEngine_tests */

void add_CF_CmdDisableEngine_tests(void)
{
    UtTest_Add(Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdAccCounter");
    UtTest_Add(Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdDisableEngine_WhenEngineDisabledAndIncrementCmdAccCounterThenFail");
} /* end add_CF_CmdDisableEngine_tests */

void add_CF_ProcessGroundCommand_tests(void)
{
    UtTest_Add(Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent");
    UtTest_Add(Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent");
    UtTest_Add(
        Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_CmdRej,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndCall_CF_"
        "CmdRej");
    UtTest_Add(Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_CmdNoop_With_msg");
    UtTest_Add(Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL");
} /* end add_CF_ProcessGroundCommand_tests */

/* end cf_cmd tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_cmd_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_CmdAcc_tests();

    add_CF_CmdRej_tests();

    add_CF_CmdCond_tests();

    add_CF_CmdNoop_tests();

    add_CF_CmdReset_tests();

    add_CF_CmdTxFile_tests();

    add_CF_CmdPlaybackDir_tests();

    add_CF_DoChanAction_tests();

    add_CF_DoFreezeThaw_tests();

    add_CF_CmdFreeze_tests();

    add_CF_CmdThaw_tests();

    add_CF_FindTransactionBySequenceNumberAllChannels_tests();

    add_CF_TsnChanAction_tests();

    add_CF_DoSuspRes_Txn_tests();

    add_CF_DoSuspRes_tests();

    add_CF_CmdSuspend_tests();

    add_CF_CmdResume_tests();

    add_CF_CmdCancel_Txn_tests();

    add_CF_CmdCancel_tests();

    add_CF_CmdAbandon_Txn_tests();

    add_CF_CmdAbandon_tests();

    add_CF_DoEnableDisableDequeue_tests();

    add_CF_CmdEnableDequeue_tests();

    add_CF_CmdDisableDequeue_tests();

    add_CF_DoEnableDisablePolldir_tests();

    add_CF_CmdEnablePolldir_tests();

    add_CF_CmdDisablePolldir_tests();

    add_CF_PurgeHistory_tests();

    add_CF_PurgeTransaction_tests();

    add_CF_DoPurgeQueue_tests();

    add_CF_CmdPurgeQueue_tests();

    add_CF_CmdWriteQueue_tests();

    add_CF_CmdSendCfgParams_tests();

    add_CF_CmdValidateChunkSize_tests();

    add_CF_CmdValidateMaxOutgoing_tests();

    add_CF_CmdGetSetParam_tests();

    add_CF_CmdSetParam_tests();

    add_CF_CmdGetParam_tests();

    add_CF_CmdEnableEngine_tests();

    add_CF_CmdDisableEngine_tests();

    add_CF_ProcessGroundCommand_tests();

} /* end UtTest_Setup for cf_cmd_tests.c */

/* end cf_cmd_tests.c */
