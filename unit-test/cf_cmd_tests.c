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
#include "cf_cmd.h"
#include "cf_events.h"
#include "cf_test_alt_handler.h"

/*******************************************************************************
**
**  cf_cmd_tests Setup and Teardown
**
*******************************************************************************/

void cf_cmd_tests_Setup(void)
{
    cf_tests_Setup();
}

void cf_cmd_tests_Teardown(void)
{
    cf_tests_Teardown();
}

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

/*******************************************************************************
**
**  cf_cmd_tests test functions
**
*******************************************************************************/

typedef struct
{
    CF_Transaction_t *txn;
    void *            context;
} CF_TsnChanAction_fn_t_context_t;

CF_ChanAction_Status_t Chan_action_fn_t(uint8 chan_num, void *context)
{
    /* This one does not need to save its context, just call default so count works */
    return UT_DEFAULT_IMPL(Chan_action_fn_t);
}

void Dummy_CF_TsnChanAction_fn_t(CF_Transaction_t *txn, void *context)
{
    CF_TsnChanAction_fn_t_context_t *ctxt =
        UT_CF_GetContextBuffer(UT_KEY(Dummy_CF_TsnChanAction_fn_t), CF_TsnChanAction_fn_t_context_t);

    if (ctxt)
    {
        ctxt->txn     = txn;
        ctxt->context = context;
    }

    UT_DEFAULT_IMPL(Dummy_CF_TsnChanAction_fn_t);
}

/*******************************************************************************
**
**  CF_NoopCmd tests
**
*******************************************************************************/

void Test_CF_NoopCmd_SendNoopEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_NoopCmd_t utbuf;

    uint16 initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_NoopCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_NOOP_INF_EID);
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

/*******************************************************************************
**
**  CF_ResetCountersCmd tests
**
*******************************************************************************/

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t   utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = 5; /* 5 is size of 'names' */

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_RESET_INVALID_ERR_EID);
    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t   utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = Any_uint8_GreaterThan(5); /* 5 is size of 'names' */

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_RESET_INVALID_ERR_EID);
    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t   utbuf;
    CF_UnionArgs_Payload_t *data = &utbuf.Payload;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0]                      = CF_Reset_command;
    CF_AppData.hk.Payload.counters.cmd = Any_uint16_Except(0);
    CF_AppData.hk.Payload.counters.err = Any_uint16_Except(0);

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);
    UtAssert_ZERO(CF_AppData.hk.Payload.counters.cmd);
    UtAssert_ZERO(CF_AppData.hk.Payload.counters.err);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t   utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    int                     i                      = 0;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_Reset_fault;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_open          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_read          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_seek          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_write         = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_rename        = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.directory_read     = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.crc_mismatch       = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_size_mismatch = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.nak_limit          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.ack_limit          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.inactivity_timer   = Any_uint16_Except(0);
    }

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_open);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_read);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_seek);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_write);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_rename);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.directory_read);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.crc_mismatch);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_size_mismatch);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.nak_limit);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.ack_limit);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.fault.inactivity_timer);
        UtAssert_MemCmpValue(&CF_AppData.hk.Payload.channel_hk[i].counters.fault, 0,
                             sizeof(&CF_AppData.hk.Payload.channel_hk[i].counters.fault),
                             "fault channel %d was completely cleared to 0", i);
    }
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t   utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    int                     i                      = 0;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_Reset_up;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.pdu                  = Any_uint32_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.error                = Any_uint32_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.spurious             = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.dropped              = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.nak_segment_requests = Any_uint32_Except(0);
    }

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.recv.file_data_bytes);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.recv.pdu);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.recv.error);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.recv.spurious);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.recv.pdu);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.recv.nak_segment_requests);
        UtAssert_MemCmpValue(&CF_AppData.hk.Payload.channel_hk[i].counters.recv, 0,
                             sizeof(&CF_AppData.hk.Payload.channel_hk[i].counters.recv),
                             "recv channel %d was completely cleared to 0", i);
    }
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t   utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint8                   i                      = 0;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_Reset_down;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.Payload.channel_hk[i].counters.sent.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.sent.nak_segment_requests = Any_uint32_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.sent.pdu                  = Any_uint32_Except(0);
    }

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.sent.file_data_bytes);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.sent.nak_segment_requests);
        UtAssert_ZERO(CF_AppData.hk.Payload.channel_hk[i].counters.sent.pdu);
        UtAssert_MemCmpValue(&CF_AppData.hk.Payload.channel_hk[i].counters.sent, 0,
                             sizeof(&CF_AppData.hk.Payload.channel_hk[i].counters.sent),
                             "sent channel %d was completely cleared to 0", i);
    }
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t   utbuf;
    CF_UnionArgs_Payload_t *data = &utbuf.Payload;
    int                     i    = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_Reset_all;

    CF_AppData.hk.Payload.counters.cmd = Any_uint16_Except(0);
    CF_AppData.hk.Payload.counters.err = Any_uint16_Except(0);

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_open          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_read          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_seek          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_write         = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_rename        = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.directory_read     = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.crc_mismatch       = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.file_size_mismatch = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.nak_limit          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.ack_limit          = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.fault.inactivity_timer   = Any_uint16_Except(0);
    }

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.pdu                  = Any_uint32_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.error                = Any_uint32_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.spurious             = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.dropped              = Any_uint16_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.recv.nak_segment_requests = Any_uint32_Except(0);
    }

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.Payload.channel_hk[i].counters.sent.file_data_bytes      = Any_uint64_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.sent.nak_segment_requests = Any_uint32_Except(0);
        CF_AppData.hk.Payload.channel_hk[i].counters.sent.pdu                  = Any_uint32_Except(0);
    }

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    UtAssert_ZERO(CF_AppData.hk.Payload.counters.cmd);
    UtAssert_ZERO(CF_AppData.hk.Payload.counters.err);
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.Payload.channel_hk[i].counters.fault, 0,
                             sizeof(&CF_AppData.hk.Payload.channel_hk[i].counters.fault),
                             "fault channel %d was completely cleared to 0", i);
    }
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.Payload.channel_hk[i].counters.recv, 0,
                             sizeof(&CF_AppData.hk.Payload.channel_hk[i].counters.recv),
                             "recv channel %d was completely cleared to 0", i);
    }
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        UtAssert_MemCmpValue(&CF_AppData.hk.Payload.channel_hk[i].counters.sent, 0,
                             sizeof(&CF_AppData.hk.Payload.channel_hk[i].counters.sent),
                             "sent channel %d was completely cleared to 0", i);
    }
}

/*******************************************************************************
**
**  CF_TxFileCmd tests
**
*******************************************************************************/

void Test_CF_TxFileCmd(void)
{
    /* Test case for:
     * void CF_TxFileCmd(CFE_SB_Buffer_t *msg);
     */
    CF_TxFileCmd_t       utbuf;
    CF_TxFile_Payload_t *msg = &utbuf.Payload;

    memset(&CF_AppData.hk.Payload.counters, 0, sizeof(CF_AppData.hk.Payload.counters));

    /* nominal, all zero should pass checks, just calls CF_CFDP_TxFile */
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = CF_CFDP_CLASS_1;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_TX_FILE_INF_EID);

    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = CF_CFDP_CLASS_2;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_TX_FILE_INF_EID);

    /* out of range arguments: bad class */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = 10;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);

    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = -10;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 2);

    /* out of range arguments: bad channel */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = CF_NUM_CHANNELS;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 3);

    /* out of range arguments: bad keep */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->keep = 15;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 4);

    /* CF_CFDP_TxFile fails*/
    UT_CF_ResetEventCapture();
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxFile), -1);
    memset(msg, 0, sizeof(*msg));
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_TX_FILE_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 5);
}

/*******************************************************************************
**
**  CF_PlaybackDirCmd tests
**
*******************************************************************************/

void Test_CF_PlaybackDirCmd(void)
{
    /* Test case for:
     * void CF_PlaybackDirCmd(CFE_SB_Buffer_t *msg);
     */
    CF_PlaybackDirCmd_t  utbuf;
    CF_TxFile_Payload_t *msg = &utbuf.Payload;

    memset(&CF_AppData.hk.Payload.counters, 0, sizeof(CF_AppData.hk.Payload.counters));

    /* nominal, all zero should pass checks, just calls CF_CFDP_PlaybackDir */
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = CF_CFDP_CLASS_1;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);

    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = CF_CFDP_CLASS_2;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 2);

    /* out of range arguments: bad class */
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = 10;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);

    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->cfdp_class = -10;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 2);

    /* out of range arguments: bad channel */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = CF_NUM_CHANNELS;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 3);

    /* out of range arguments: bad keep */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->keep = 15;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 4);

    /* CF_CFDP_PlaybackDir fails*/
    UT_CF_ResetEventCapture();
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_PlaybackDir), -1);
    memset(msg, 0, sizeof(*msg));
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_PLAYBACK_DIR_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 5);
}

/*******************************************************************************
**
**  CF_DoChanAction tests
**
*******************************************************************************/

void Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data       = &utbuf;
    const char *            arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t       arg_fn     = &Chan_action_fn_t;
    int                     context;
    void *                  arg_context    = &context;
    uint8                   random_fn_call = Any_uint8_LessThan(CF_NUM_CHANNELS) + 1;
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_ALL_CHANNELS;

    UT_SetDeferredRetcode(UT_KEY(Chan_action_fn_t), random_fn_call, 1);

    /* Act */
    local_result = CF_DoChanAction(data, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS | 1);
}

void Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenAll_fn_return_1_Return_1(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data       = &utbuf;
    const char *            arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t       arg_fn     = &Chan_action_fn_t;
    int                     context;
    void *                  arg_context = &context;
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_ALL_CHANNELS;

    UT_SetDefaultReturnValue(UT_KEY(Chan_action_fn_t), 1);

    /* Act */
    local_result = CF_DoChanAction(data, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS | 1);
}

void Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data       = &utbuf;
    const char *            arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t       arg_fn     = &Chan_action_fn_t;
    int                     context;
    void *                  arg_context = &context;
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_ALL_CHANNELS;

    UT_SetDefaultReturnValue(UT_KEY(Chan_action_fn_t), 0);

    /* Act */
    local_result = CF_DoChanAction(data, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Chan_action_fn_t, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data       = &utbuf;
    const char *            arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t       arg_fn     = &Chan_action_fn_t;
    int                     context;
    void *                  arg_context = &context;
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = Any_cf_channel();

    UT_SetDefaultReturnValue(UT_KEY(Chan_action_fn_t), 1);

    /* Act */
    local_result = CF_DoChanAction(data, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Chan_action_fn_t, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(local_result, 1);
}

void Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data       = &utbuf;
    const char *            arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t       arg_fn     = &Chan_action_fn_t;
    int                     context;
    void *                  arg_context = &context;
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = Any_cf_channel();

    UT_SetDefaultReturnValue(UT_KEY(Chan_action_fn_t), 0);

    /* Act */
    local_result = CF_DoChanAction(data, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Chan_action_fn_t, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(local_result, 0);
}

void Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data       = &utbuf;
    const char *            arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t       arg_fn     = &Chan_action_fn_t;
    int                     context;
    void *                  arg_context = &context;
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[0] = CF_NUM_CHANNELS;

    /* Act */
    local_result = CF_DoChanAction(data, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Chan_action_fn_t, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);

    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
}

void Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data       = &utbuf;
    const char *            arg_errstr = "CANNOT TEST SENT TO SEND EVENT";
    CF_ChanActionFn_t       arg_fn     = &Chan_action_fn_t;
    int                     context;
    void *                  arg_context = &context;
    CF_ChanAction_Status_t  local_result;
    int                     catastrophe_count = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    /* force CF_ALL_CHANNELS to not be a selection possibility */
    data->byte[0] = CF_ALL_CHANNELS;
    while (data->byte[0] == CF_ALL_CHANNELS)
    {
        if (catastrophe_count == 10) /* 10 is arbitrary */
        {
            UtAssert_Message(UTASSERT_CASETYPE_ABORT, __FILE__, __LINE__,
                             "CANNOT make data->byte[0] != CF_ALL_CHANNELS in 10 tries");
        }

        data->byte[0] = Any_uint8_GreaterThan_or_EqualTo(CF_NUM_CHANNELS);
        ++catastrophe_count;
    }

    /* Act */
    local_result = CF_DoChanAction(data, arg_errstr, arg_fn, arg_context);

    UT_GetStubCount(UT_KEY(Chan_action_fn_t));
    UT_GetStubCount(UT_KEY(CFE_EVS_SendEvent));

    /* Assert */
    UtAssert_STUB_COUNT(Chan_action_fn_t, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);

    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
}

/*******************************************************************************
**
**  CF_DoFreezeThaw tests
**
*******************************************************************************/

void Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0(void)
{
    /* Arrange */
    uint8                   arg_chan_num = Any_cf_channel();
    CF_ChanAction_BoolArg_t context;
    CF_ChanAction_Status_t  local_result;

    context.barg = Any_bool_arg_t_barg();

    /* set frozen to opposite to ensure change was done - not required for test,
     * but it is helpful for verification that the function did the change */
    CF_AppData.hk.Payload.channel_hk[arg_chan_num].frozen = !context.barg;

    /* Act */
    local_result = CF_DoFreezeThaw(arg_chan_num, &context);

    /* Assert */
    UtAssert_True(CF_AppData.hk.Payload.channel_hk[arg_chan_num].frozen == context.barg,
                  "CF_DoFreezeThaw set frozen to %d and should be %d (context->barg))",
                  CF_AppData.hk.Payload.channel_hk[arg_chan_num].frozen, context.barg);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

/**************************************************************************
**
**  CF_FreezeCmd tests
**
*******************************************************************************/

void Test_CF_FreezeCmd_Set_frozen_To_1_AndAcceptCommand(void)
{
    /* Arrange */
    CF_FreezeCmd_t          utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    uint8 chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = chan_num;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_FreezeCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.hk.Payload.channel_hk[chan_num].frozen == 1,
                  "CF_DoFreezeThaw set frozen to %d and should be 1 (freeze = 1))",
                  CF_AppData.hk.Payload.channel_hk[chan_num].frozen);
    UtAssert_True(CF_AppData.hk.Payload.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.Payload.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.cmd, initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_FREEZE_INF_EID);
}

void Test_CF_FreezeCmd_Set_frozen_To_1_AndRejectCommand(void)
{
    /* Arrange */
    CF_FreezeCmd_t          utbuf;
    CF_UnionArgs_Payload_t *data = &utbuf.Payload;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = CF_NUM_CHANNELS + 1;

    CF_AppData.hk.Payload.counters.cmd = 0;

    /* Act */
    CF_FreezeCmd(&utbuf);

    /* Assert */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_FREEZE_ERR_EID);
}

/*******************************************************************************
**
**  CF_ThawCmd tests
**
*******************************************************************************/

void Test_CF_ThawCmd_Set_frozen_To_0_AndAcceptCommand(void)
{
    /* Arrange */
    CF_ThawCmd_t            utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    uint8 chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = chan_num;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ThawCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.hk.Payload.channel_hk[chan_num].frozen == 0,
                  "CF_DoFreezeThaw set frozen to %d and should be 0 (thaw = 0))",
                  CF_AppData.hk.Payload.channel_hk[chan_num].frozen);
    UtAssert_True(CF_AppData.hk.Payload.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.Payload.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.cmd, initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_THAW_INF_EID);
}

void Test_CF_ThawCmd_Set_frozen_To_0_AndRejectCommand(void)
{
    /* Arrange */
    CF_ThawCmd_t            utbuf;
    CF_UnionArgs_Payload_t *data = &utbuf.Payload;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = CF_NUM_CHANNELS + 1;

    CF_AppData.hk.Payload.counters.cmd = 0;

    /* Act */
    CF_ThawCmd(&utbuf);

    /* Assert */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_THAW_ERR_EID);
}

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
    CF_Transaction_t *  local_result;
    CF_Transaction_t *  expected_result = NULL;

    CF_FindTransactionBySequenceNumber_context_t context_CF_CFDP_FTBSN;

    context_CF_CFDP_FTBSN.forced_return = NULL;

    UT_SetDataBuffer(UT_KEY(CF_FindTransactionBySequenceNumber), &context_CF_CFDP_FTBSN, sizeof(context_CF_CFDP_FTBSN),
                     false);

    /* Act */
    local_result = CF_FindTransactionBySequenceNumberAllChannels(arg_ts, arg_eid);

    UT_GetStubCount(UT_KEY(CF_FindTransactionBySequenceNumber));

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, CF_NUM_CHANNELS);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_FTBSN.chan, CF_AppData.engine.channels);
    UtAssert_UINT32_EQ(context_CF_CFDP_FTBSN.transaction_sequence_number, arg_ts);
    UtAssert_UINT32_EQ(context_CF_CFDP_FTBSN.src_eid, arg_eid);
    UtAssert_ADDRESS_EQ(local_result, expected_result);
}

void Test_CF_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound(void)
{
    /* Arrange */
    CF_TransactionSeq_t arg_ts                   = Any_CF_TransactionSeq_t();
    CF_EntityId_t       arg_eid                  = Any_CF_EntityId_t();
    uint8               number_transaction_match = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_Transaction_t    return_value;
    CF_Transaction_t *  local_result;
    CF_Transaction_t *  expected_result = &return_value;
    int                 i               = 0;

    CF_FindTransactionBySequenceNumber_context_t contexts_CF_CFDP_FTBSN[CF_NUM_CHANNELS];

    /* set non-matching transactions */
    for (i = 0; i < number_transaction_match; ++i)
    {
        contexts_CF_CFDP_FTBSN[i].forced_return = NULL;
    }
    /* set matching transaction */
    contexts_CF_CFDP_FTBSN[i].forced_return = &return_value;

    UT_SetDataBuffer(UT_KEY(CF_FindTransactionBySequenceNumber), &contexts_CF_CFDP_FTBSN,
                     sizeof(contexts_CF_CFDP_FTBSN), false);

    /* Act */
    local_result = CF_FindTransactionBySequenceNumberAllChannels(arg_ts, arg_eid);

    UT_GetStubCount(UT_KEY(CF_FindTransactionBySequenceNumber));

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, number_transaction_match + 1);
    for (i = 0; i < number_transaction_match; ++i)
    {
        UtAssert_ADDRESS_EQ(contexts_CF_CFDP_FTBSN[i].chan, CF_AppData.engine.channels + i);
        UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].transaction_sequence_number, arg_ts);
        UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].src_eid, arg_eid);
    }
    UtAssert_ADDRESS_EQ(contexts_CF_CFDP_FTBSN[i].chan, CF_AppData.engine.channels + i);
    UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].transaction_sequence_number, arg_ts);
    UtAssert_UINT32_EQ(contexts_CF_CFDP_FTBSN[i].src_eid, arg_eid);
    UtAssert_ADDRESS_EQ(local_result, expected_result);
}

/*******************************************************************************
**
**  CF_TsnChanAction tests
**
*******************************************************************************/

void Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_CF_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail(void)
{
    /* Arrange */
    CF_Transaction_Payload_t  utbuf;
    CF_Transaction_Payload_t *arg_cmd = &utbuf;
    char                      cmdstr[10];     /* 10 is arbitrary */
    const char                arg_cmdstr[10]; /* 10 is arbitrary */
    CF_TsnChanAction_fn_t     arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                       context;
    void *                    arg_context = &context;
    int                       i           = 0;

    CF_FindTransactionBySequenceNumber_context_t contexts_CF_CFDP_FTBSN[CF_NUM_CHANNELS];

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(cmdstr, 10);
    memcpy((char *)arg_cmdstr, &cmdstr, 10);

    arg_cmd->chan = CF_COMPOUND_KEY;

    /* Arrange unstubbable: CF_FindTransactionBySequenceNumberAllChannels */
    /* set non-matching transactions */
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
    UT_CF_AssertEventID(CF_CMD_TRANS_NOT_FOUND_ERR_EID);

    UtAssert_STUB_COUNT(Dummy_CF_TsnChanAction_fn_t, 0);
}

void Test_CF_TsnChanAction_cmd_chan_Eq_CF_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS(void)
{
    /* Arrange */
    CF_Transaction_Payload_t        utbuf;
    CF_Transaction_Payload_t *      arg_cmd = &utbuf;
    char                            cmdstr[10];     /* 10 is arbitrary */
    const char                      arg_cmdstr[10]; /* 10 is arbitrary */
    CF_TsnChanAction_fn_t           arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                             context;
    void *                          arg_context = &context;
    CF_Transaction_t                txn;
    CF_TsnChanAction_fn_t_context_t context_CF_TsnChanAction_fn_t;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(cmdstr, 10);
    memcpy((char *)arg_cmdstr, &cmdstr, 10);

    arg_cmd->chan = CF_COMPOUND_KEY;

    UT_SetDataBuffer(UT_KEY(Dummy_CF_TsnChanAction_fn_t), &context_CF_TsnChanAction_fn_t,
                     sizeof(context_CF_TsnChanAction_fn_t), false);

    /* Arrange unstubbable: CF_FindTransactionBySequenceNumberAllChannels */
    CF_FindTransactionBySequenceNumber_context_t context_CF_CFDP_FTBSN;

    /* set matching transaction */
    context_CF_CFDP_FTBSN.forced_return = &txn;

    UT_SetDataBuffer(UT_KEY(CF_FindTransactionBySequenceNumber), &context_CF_CFDP_FTBSN, sizeof(context_CF_CFDP_FTBSN),
                     false);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), 1);

    UT_GetStubCount(UT_KEY(Dummy_CF_TsnChanAction_fn_t));

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(Dummy_CF_TsnChanAction_fn_t, 1);
    UtAssert_ADDRESS_EQ(context_CF_TsnChanAction_fn_t.txn, &txn);
    UtAssert_ADDRESS_EQ(context_CF_TsnChanAction_fn_t.context, arg_context);
}

void Test_CF_TsnChanAction_cmd_chan_Eq_CF_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels(void)
{
    /* Arrange */
    CF_Transaction_Payload_t                          utbuf;
    CF_Transaction_Payload_t *                        arg_cmd = &utbuf;
    char                                              cmdstr[10];     /* 10 is arbitrary */
    const char                                        arg_cmdstr[10]; /* 10 is arbitrary */
    CF_TsnChanAction_fn_t                             arg_fn = &Dummy_CF_TsnChanAction_fn_t;
    int                                               context;
    void *                                            arg_context     = &context;
    int                                               expected_result = Any_int();
    CF_TraverseAllTransactions_All_Channels_context_t context_CF_TATAC;

    memset(&utbuf, 0, sizeof(utbuf));
    AnyRandomStringOfLettersOfLengthCopy(cmdstr, 10);
    memcpy((char *)arg_cmdstr, &cmdstr, 10);

    context_CF_TATAC.forced_return = expected_result;

    arg_cmd->chan = CF_ALL_CHANNELS;

    UT_SetDataBuffer(UT_KEY(CF_TraverseAllTransactions_All_Channels), &context_CF_TATAC, sizeof(context_CF_TATAC),
                     false);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), expected_result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void Test_CF_TsnChanAction_cmd_chan_IsASingleChannel(void)
{
    /* Arrange */
    CF_Transaction_Payload_t cmd;
    int                      result = 1;

    memset(&cmd, 0, sizeof(cmd));

    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), result);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(&cmd, NULL, NULL, NULL), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
}

void Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid(void)
{
    /* Arrange */
    CF_Transaction_Payload_t  utbuf;
    CF_Transaction_Payload_t *arg_cmd       = &utbuf;
    const char                arg_cmdstr[1] = "";
    CF_TsnChanAction_fn_t     arg_fn        = &Dummy_CF_TsnChanAction_fn_t;
    int                       context;
    void *                    arg_context = &context;

    memset(&utbuf, 0, sizeof(utbuf));
    arg_cmd->chan = Any_uint8_BetweenExcludeMax(CF_NUM_CHANNELS, CF_COMPOUND_KEY);

    /* Act */
    UtAssert_INT32_EQ(CF_TsnChanAction(arg_cmd, arg_cmdstr, arg_fn, arg_context), -1);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_TSN_CHAN_INVALID_ERR_EID);
}

/*******************************************************************************
**
**  CF_DoSuspRes_Txn tests
**
*******************************************************************************/

void Test_CF_DoSuspRes_Txn_Set_context_same_To_1_suspended_Eq_action(void)
{
    /* Arrange */
    CF_Transaction_t            txn;
    CF_Transaction_t *          arg_t = &txn;
    CF_ChanAction_SuspResArg_t  context;
    CF_ChanAction_SuspResArg_t *arg_context = &context;

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
}

void Test_CF_DoSuspRes_Txn_When_suspended_NotEqTo_action_Set_suspended_To_action(void)
{
    /* Arrange */
    CF_Transaction_t            txn;
    CF_Transaction_t *          arg_t = &txn;
    CF_ChanAction_SuspResArg_t  context;
    CF_ChanAction_SuspResArg_t *arg_context = &context;

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
}

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
     * void CF_DoSuspRes(CF_Transaction_Payload_t *cmd, uint8 action)
     */

    CF_Transaction_Payload_t   utbuf;
    CF_Transaction_Payload_t * cmd = &utbuf;
    CF_ChanAction_SuspResArg_t utargs;

    memset(&CF_AppData.hk.Payload.counters, 0, sizeof(CF_AppData.hk.Payload.counters));
    memset(&utargs, 0, sizeof(utargs));
    memset(cmd, 0, sizeof(*cmd));

    /* nominal */
    /* With no setup, CF_TsnChanAction() invokes CF_TraverseAllTransactions stub, which returns 0 */
    /* this should increment the reject counter because it did not match any transactions */
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 0));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);

    /* set up to match 1 transaction, should be accepted, but should not generate an event */
    UT_CF_ResetEventCapture();
    UT_SetDeferredRetcode(UT_KEY(CF_TraverseAllTransactions), 1, 1);
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 1));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_SUSPRES_INF_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);

    /* Output the CF_ChanAction_SuspResArg_t back to the caller, to set the "same" flag to 1 */
    /* this gets the case where it attempts to set to the same value, and is rejected due to that */
    UT_CF_ResetEventCapture();
    UT_SetDeferredRetcode(UT_KEY(CF_TraverseAllTransactions), 1, 1);
    utargs.same = 1;
    UT_SetHandlerFunction(UT_KEY(CF_TraverseAllTransactions), UT_AltHandler_CF_TraverseAllTransactions_SetSuspResArg,
                          &utargs);
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 0));
    UT_CF_AssertEventID(CF_CMD_SUSPRES_SAME_INF_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 2);

    /* Output the CF_ChanAction_SuspResArg_t back to the caller, to set the "same" flag to 1 */
    /* however this time CF_TraverseAllTransactions reports it matched multiple transactions, so it should NOT reject it
     */
    UT_CF_ResetEventCapture();
    UT_SetDeferredRetcode(UT_KEY(CF_TraverseAllTransactions), 1, 10);
    UtAssert_VOIDCALL(CF_DoSuspRes(cmd, 1));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_SUSPRES_INF_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 3);
}

/*******************************************************************************
**
**  CF_SuspendCmd tests
**
*******************************************************************************/

/* Test_CF_SuspendCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_1 */
void Test_CF_SuspendCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_1(void)
{
    /* Arrange */
    CF_SuspendCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Set to invalid channel */
    utbuf.Payload.chan = CF_NUM_CHANNELS;

    /* Act */
    CF_SuspendCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[0], CF_CMD_TSN_CHAN_INVALID_ERR_EID);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[1], CF_CMD_SUSPRES_CHAN_ERR_EID);

    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
}

/*******************************************************************************
**
**  CF_ResumeCmd tests
**
*******************************************************************************/

/* Test_CF_ResumeCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_0 */
void Test_CF_ResumeCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_0(void)
{
    CF_ResumeCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Invalid channel */
    utbuf.Payload.chan = CF_NUM_CHANNELS;

    /* Act */
    CF_ResumeCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[0], CF_CMD_TSN_CHAN_INVALID_ERR_EID);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[1], CF_CMD_SUSPRES_CHAN_ERR_EID);

    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
}

/*******************************************************************************
**
**  CF_Cancel_TxnCmd tests
**
*******************************************************************************/

void Test_CF_Cancel_TxnCmd_Call_CF_CFDP_CancelTransaction_WithGiven_t(void)
{
    /* Arrange */
    CF_Transaction_t  txn;
    CF_Transaction_t *arg_t       = &txn;
    void *            arg_ignored = NULL;
    CF_Transaction_t *context_CF_CFDP_CancelTransaction;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_CancelTransaction), &context_CF_CFDP_CancelTransaction,
                     sizeof(context_CF_CFDP_CancelTransaction), false);

    /* Act */
    CF_Cancel_TxnCmd(arg_t, arg_ignored);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_CancelTransaction, arg_t);
}

/*******************************************************************************
**
**  CF_CancelCmd tests
**
*******************************************************************************/

void Test_CF_CancelCmd_Success(void)
{
    CF_CancelCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Nominally returns number of transactions affected, cause failure */
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), 1);

    /* Act */
    CF_CancelCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CANCEL_INF_EID);
}

void Test_CF_CancelCmd_Failure(void)
{
    CF_CancelCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Nominally returns number of transactions affected, cause failure */
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), 0);

    /* Act */
    CF_CancelCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_CANCEL_CHAN_ERR_EID);
}

/*******************************************************************************
**
**  CF_Abandon_TxnCmd tests
**
*******************************************************************************/

void Test_CF_Abandon_TxnCmd_Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0(void)
{
    /* Arrange */
    CF_Transaction_t  txn;
    CF_Transaction_t *arg_t       = &txn;
    void *            arg_ignored = NULL;

    /* Act */
    CF_Abandon_TxnCmd(arg_t, arg_ignored);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_FinishTransaction, 1);
}

/*******************************************************************************
**
**  CF_AbandonCmd tests
**
*******************************************************************************/

void Test_CF_AbandonCmd_Success(void)
{
    CF_AbandonCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Nominally returns number of transactions affected, cause failure */
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), 1);

    /* Act */
    CF_AbandonCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ABANDON_INF_EID);
}

void Test_CF_AbandonCmd_Failure(void)
{
    CF_AbandonCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Nominally returns number of transactions acted on, force failure */
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), 0);

    /* Act */
    CF_AbandonCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_ABANDON_CHAN_ERR_EID);
}

/*******************************************************************************
**
**  CF_DoEnableDisableDequeue tests
**
*******************************************************************************/

void Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg(void)
{
    /* Arrange */
    CF_ConfigTable_t         config_table;
    uint8                    arg_chan_num = Any_cf_channel();
    CF_ChanAction_BoolArg_t  context;
    CF_ChanAction_BoolArg_t *arg_context = &context;

    CF_AppData.config_table = &config_table;
    context.barg            = Any_bool_arg_t_barg();

    memset(&config_table, 0, sizeof(config_table));

    /* Act */
    CF_DoEnableDisableDequeue(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].dequeue_enabled == context.barg,
                  "Channel %u dequeue_enabled is %u and should be %u (context->barg)", arg_chan_num,
                  CF_AppData.config_table->chan[arg_chan_num].dequeue_enabled, context.barg);
}

/*******************************************************************************
**
**  CF_EnableDequeueCmd tests
**
*******************************************************************************/

void Test_CF_EnableDequeueCmd_Success(void)
{
    CF_EnableDequeueCmd_t utbuf;

    /* Arrange */
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t config_table;
    uint8            chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = chan_num;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.config_table->chan[chan_num].dequeue_enabled == 1,
                  "CF_EnableDequeueCmd set dequeue_enabled to %d and should be 1 (barg = 1))",
                  CF_AppData.config_table->chan[chan_num].dequeue_enabled);
    /* Assert for incremented counter */
    UtAssert_True(
        CF_AppData.hk.Payload.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
        "CF_AppData.hk.Payload.counters.cmd is %d and should be 1 more than %d\nACCEPTANCE OF COMMAND (+1) SHOULD BE "
        "THE BEHAVIOR BUT IT IS NOT",
        CF_AppData.hk.Payload.counters.cmd, initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_DEQUEUE_INF_EID);
}

void Test_CF_EnableDequeueCmd_Failure(void)
{
    /* Arrange */
    CF_EnableDequeueCmd_t utbuf;

    CF_UnionArgs_Payload_t *data = &utbuf.Payload;

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t config_table;

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = CF_NUM_CHANNELS + 1;

    CF_AppData.hk.Payload.counters.err = 0;

    /* Act */
    CF_EnableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_DEQUEUE_ERR_EID);
}

/*******************************************************************************
**
**  CF_DisableDequeueCmd tests
**
*******************************************************************************/

void Test_CF_DisableDequeueCmd_Success(void)
{
    CF_DisableDequeueCmd_t utbuf;

    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t config_table;
    uint8            chan_num = Any_cf_channel();

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = chan_num;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DisableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(CF_AppData.config_table->chan[chan_num].dequeue_enabled == 0,
                  "CF_EnableDequeueCmd set dequeue_enabled to %d and should be 0 (barg = 0))",
                  CF_AppData.config_table->chan[chan_num].dequeue_enabled);
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.hk.Payload.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.Payload.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.cmd, initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_DISABLE_DEQUEUE_INF_EID);
}

void Test_CF_DisableDequeueCmd_Failure(void)
{
    /* Arrange */
    CF_DisableDequeueCmd_t utbuf;

    CF_UnionArgs_Payload_t *data = &utbuf.Payload;

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ConfigTable_t config_table;

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = CF_NUM_CHANNELS + 1;

    CF_AppData.hk.Payload.counters.err = 0;

    /* Act */
    CF_DisableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_DISABLE_DEQUEUE_ERR_EID);
}

/*******************************************************************************
**
**  CF_DoEnableDisablePolldir tests
**
*******************************************************************************/

void Test_CF_DoEnableDisablePolldir_When_CF_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg(void)
{
    /* Arrange */
    CF_UnionArgs_Payload_t      utbuf;
    CF_UnionArgs_Payload_t *    data = &utbuf;
    CF_ChanAction_BoolMsgArg_t  context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &context;
    CF_ConfigTable_t            config_table;
    uint8                       arg_chan_num = Any_cf_channel();
    uint8                       expected_enabled;
    uint8                       current_polldir = 0;
    CF_ChanAction_Status_t      local_result;

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    data->byte[1] = CF_ALL_CHANNELS;

    context.data     = data;
    context.barg     = Any_bool_arg_t_barg();
    expected_enabled = context.barg;

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    for (current_polldir = 0; current_polldir < CF_MAX_POLLING_DIR_PER_CHAN; ++current_polldir)
    {
        UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].polldir[current_polldir].enabled == expected_enabled,
                      "Channel %u Polldir %u set to %u and should be %u (context->barg)", arg_chan_num, current_polldir,
                      CF_AppData.config_table->chan[arg_chan_num].polldir[current_polldir].enabled, expected_enabled);
    }
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg(void)
{
    /* Arrange */
    uint8                       arg_chan_num = Any_cf_channel();
    uint8                       polldir      = Any_cf_polldir();
    CF_UnionArgs_Payload_t      utbuf;
    CF_UnionArgs_Payload_t *    data = &utbuf;
    CF_ChanAction_BoolMsgArg_t  context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &context;
    CF_ConfigTable_t            config_table;
    uint8                       expected_enabled;
    CF_ChanAction_Status_t      local_result;

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    data->byte[1] = polldir;

    context.data     = data;
    context.barg     = Any_bool_arg_t_barg();
    expected_enabled = context.barg;

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UtAssert_True(CF_AppData.config_table->chan[arg_chan_num].polldir[polldir].enabled == expected_enabled,
                  "Channel %u Polldir %u set to %u and should be %u (context->barg)", arg_chan_num, polldir,
                  CF_AppData.config_table->chan[arg_chan_num].polldir[polldir].enabled, expected_enabled);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent(void)
{
    /* Arrange */
    uint8                       arg_chan_num = Any_cf_channel();
    CF_UnionArgs_Payload_t      utbuf;
    CF_UnionArgs_Payload_t *    data = &utbuf;
    CF_ChanAction_BoolMsgArg_t  context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &context;
    CF_ConfigTable_t            config_table;
    CF_ChanAction_Status_t      local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &config_table;

    data->byte[1] = CF_MAX_POLLING_DIR_PER_CHAN;

    context.data = data;
    context.barg = Any_bool_arg_t_barg();

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UT_CF_AssertEventID(CF_CMD_POLLDIR_INVALID_ERR_EID);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
}

void Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent(void)
{
    /* Arrange */
    uint8                       arg_chan_num = Any_cf_channel();
    CF_UnionArgs_Payload_t      utbuf;
    CF_UnionArgs_Payload_t *    data = &utbuf;
    CF_ChanAction_BoolMsgArg_t  context;
    CF_ChanAction_BoolMsgArg_t *arg_context = &context;
    CF_ConfigTable_t            config_table;
    CF_ChanAction_Status_t      local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.config_table = &config_table;

    data->byte[1] = CF_MAX_POLLING_DIR_PER_CHAN;

    context.data = data;
    context.barg = Any_bool_arg_t_barg();

    /* Act */
    local_result = CF_DoEnableDisablePolldir(arg_chan_num, arg_context);

    /* Assert */
    UT_CF_AssertEventID(CF_CMD_POLLDIR_INVALID_ERR_EID);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
}

/*******************************************************************************
**
**  CF_EnablePolldirCmd tests
**
*******************************************************************************/

void Test_CF_EnablePolldirCmd_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ConfigTable_t config_table;
    uint8            channel = Any_cf_channel();
    uint8            polldir = Any_cf_polldir();

    CF_EnableDirPollingCmd_t utbuf;

    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->byte[1] = polldir;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(CF_AppData.config_table->chan[channel].polldir[polldir].enabled == 1,
                  "Channel %u Polldir %u set to %u and should be 1 (context->barg)", channel, polldir,
                  CF_AppData.config_table->chan[channel].polldir[polldir].enabled);
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.hk.Payload.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.Payload.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.cmd, initial_hk_cmd_counter);
    UT_CF_AssertEventID(CF_CMD_ENABLE_POLLDIR_INF_EID);
}

void Test_CF_EnablePolldirCmd_FailWhenActionFail(void)
{
    /* Arrange */
    uint8 channel       = Any_cf_channel();
    uint8 error_polldir = Any_uint8_BetweenInclusive(CF_MAX_POLLING_DIR_PER_CHAN, CF_ALL_CHANNELS - 1);

    CF_EnableDirPollingCmd_t utbuf;
    CF_UnionArgs_Payload_t * data                   = &utbuf.Payload;
    uint16                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->byte[1]                      = error_polldir;
    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_EnableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.hk.Payload.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.Payload.counters.err is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.err, initial_hk_err_counter);
    UT_CF_AssertEventID(CF_CMD_ENABLE_POLLDIR_ERR_EID);
}

/*******************************************************************************
**
**  CF_DisablePolldirCmd tests
**
*******************************************************************************/

void Test_CF_DisablePolldirCmd_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ConfigTable_t config_table;
    uint8            channel = Any_cf_channel();
    uint8            polldir = Any_cf_polldir();

    CF_DisableDirPollingCmd_t utbuf;
    CF_UnionArgs_Payload_t *  data                   = &utbuf.Payload;
    uint16                    initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    CF_AppData.config_table = &config_table;

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->byte[1] = polldir;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DisableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(CF_AppData.config_table->chan[channel].polldir[polldir].enabled == 0,
                  "Channel %u Polldir %u set to %u and should be 0 (context->barg)", channel, polldir,
                  CF_AppData.config_table->chan[channel].polldir[polldir].enabled);
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.hk.Payload.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.hk.Payload.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.cmd, initial_hk_cmd_counter);
    UT_CF_AssertEventID(CF_CMD_DISABLE_POLLDIR_INF_EID);
}

void Test_CF_DisablePolldirCmd_FailWhenActionFail(void)
{
    /* Arrange */
    uint8 channel       = Any_cf_channel();
    uint8 error_polldir = Any_uint8_BetweenInclusive(CF_MAX_POLLING_DIR_PER_CHAN, CF_ALL_CHANNELS - 1);

    CF_DisableDirPollingCmd_t utbuf;
    CF_UnionArgs_Payload_t *  data                   = &utbuf.Payload;
    uint16                    initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = channel;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->byte[1] = error_polldir;

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_DisableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter*/
    UtAssert_True(CF_AppData.hk.Payload.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.Payload.counters.err is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.err, initial_hk_err_counter);
    UT_CF_AssertEventID(CF_CMD_DISABLE_POLLDIR_ERR_EID);
}

/*******************************************************************************
**
**  CF_PurgeHistory tests
**
*******************************************************************************/

void Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT(void)
{
    /* Arrange */
    CF_History_t                   history;
    CF_CListNode_t *               arg_n = &history.cl_node;
    CF_Channel_t                   chan;
    CF_Channel_t *                 arg_c = &chan;
    CF_CListTraverse_Status_t      local_result;
    CF_CFDP_ResetHistory_context_t context_CF_CFDP_ResetHistory;

    UT_SetDataBuffer(UT_KEY(CF_ResetHistory), &context_CF_CFDP_ResetHistory, sizeof(context_CF_CFDP_ResetHistory),
                     false);

    /* Act */
    local_result = CF_PurgeHistory(arg_n, arg_c);

    /* Assert */
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetHistory.chan, arg_c);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_ResetHistory.history, &history);
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_PurgeHistory returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);
}

/*******************************************************************************
**
**  CF_PurgeTransaction tests
**
*******************************************************************************/

void Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT(void)
{
    /* Arrange */
    CF_Transaction_t txn;
    CF_CListNode_t * arg_n = &txn.cl_node;
    int              ignored;
    void *           arg_ignored = &ignored;

    /* Act */
    UtAssert_INT32_EQ(CF_PurgeTransaction(arg_n, arg_ignored), CF_CLIST_CONT);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_FinishTransaction, 1);
}

/*******************************************************************************
**
**  CF_DoPurgeQueue tests
**
*******************************************************************************/

void Test_CF_DoPurgeQueue_PendOnly(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UnionArgs_Payload_t              utbuf;
    CF_UnionArgs_Payload_t *            data   = &utbuf;
    CF_ChanAction_MsgArg_t              msgarg = {data};
    CF_Channel_t *                      chan;
    CF_CListNode_t                      start;
    CF_CListNode_t *                    expected_start = &start;
    CF_ChanAction_Status_t              local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[1] = 0; /* pend */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_POINTER,
                          &context_CF_CList_Traverse);

    chan                       = &CF_AppData.engine.channels[arg_chan_num];
    chan->qs[CF_QueueIdx_PEND] = expected_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, &msgarg);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.start, expected_start);
    UtAssert_True(context_CF_CList_Traverse.fn == CF_PurgeTransaction,
                  "context_CF_CList_Traverse.fn ==  CF_PurgeTransaction");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.context, NULL);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_DoPurgeQueue_HistoryOnly(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UnionArgs_Payload_t              utbuf;
    CF_UnionArgs_Payload_t *            data   = &utbuf;
    CF_ChanAction_MsgArg_t              msgarg = {data};
    CF_Channel_t *                      chan;
    CF_CListNode_t                      start;
    CF_CListNode_t *                    expected_start = &start;
    CF_ChanAction_Status_t              local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[1] = 1; /* history */

    /* set correct context type for CF_CList_Traverse stub */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_POINTER,
                          &context_CF_CList_Traverse);

    chan                       = &CF_AppData.engine.channels[arg_chan_num];
    chan->qs[CF_QueueIdx_HIST] = expected_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, &msgarg);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.start, expected_start);
    UtAssert_True(context_CF_CList_Traverse.fn == (CF_CListFn_t)CF_PurgeHistory,
                  "context_CF_CList_Traverse.fn ==  (CF_CListFn_t )CF_PurgeHistory");
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse.context, chan);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_DoPurgeQueue_Both(void)
{
    /* Arrange */
    uint8                               arg_chan_num = Any_cf_channel();
    CF_UnionArgs_Payload_t              utbuf;
    CF_UnionArgs_Payload_t *            data   = &utbuf;
    CF_ChanAction_MsgArg_t              msgarg = {data};
    CF_Channel_t *                      chan;
    CF_CListNode_t                      pend_start;
    CF_CListNode_t *                    expected_pend_start = &pend_start;
    CF_CListNode_t                      history_start;
    CF_CListNode_t *                    expected_history_start = &history_start;
    CF_ChanAction_Status_t              local_result;
    CF_CList_Traverse_POINTER_context_t context_CF_CList_Traverse[2];

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[1] = 2; /* both */

    /* set correct context type for CF_CList_Traverse stub */
    /* this must use data buffer hack to pass multiple contexts */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_POINTER, NULL);
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    chan                       = &CF_AppData.engine.channels[arg_chan_num];
    chan->qs[CF_QueueIdx_PEND] = expected_pend_start;
    chan->qs[CF_QueueIdx_HIST] = expected_history_start;

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, &msgarg);

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
    UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[1].context, chan);
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_DoPurgeQueue_GivenBad_data_byte_1_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    uint8                   arg_chan_num = Any_cf_channel();
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data   = &utbuf;
    CF_ChanAction_MsgArg_t  msgarg = {data};
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[1] = 3; /* 3 is first default value */

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, &msgarg);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_PURGE_ARG_ERR_EID);
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
}

void Test_CF_DoPurgeQueue_AnyGivenBad_data_byte_1_SendEventAndReturn_neg1(void)
{
    /* Arrange */
    uint8                   arg_chan_num = Any_cf_channel();
    CF_UnionArgs_Payload_t  utbuf;
    CF_UnionArgs_Payload_t *data   = &utbuf;
    CF_ChanAction_MsgArg_t  msgarg = {data};
    CF_ChanAction_Status_t  local_result;

    memset(&utbuf, 0, sizeof(utbuf));

    data->byte[1] = Any_uint8_GreaterThan_or_EqualTo(3);

    /* Act */
    local_result = CF_DoPurgeQueue(arg_chan_num, &msgarg);

    UT_GetStubCount(UT_KEY(CF_CList_Traverse));

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_PURGE_ARG_ERR_EID);
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
}

/*******************************************************************************
**
**  CF_PurgeQueueCmd tests
**
*******************************************************************************/

void Test_CF_PurgeQueueCmd_FailWhenActionFail(void)
{
    /* Arrange */
    uint8                   channel     = Any_cf_channel();
    uint8                   error_purge = 3; /* Shortest return from CF_DoPurgeQueue */
    CF_PurgeQueueCmd_t      utbuf;
    CF_UnionArgs_Payload_t *data                   = &utbuf.Payload;
    uint16                  initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = channel;

    /* Arrange unstubbable: CF_DoPurgeQueue */
    data->byte[1] = error_purge;

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_PurgeQueueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.hk.Payload.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.hk.Payload.counters.err is %d and should be 1 more than %d",
                  CF_AppData.hk.Payload.counters.err, initial_hk_err_counter);
    UT_CF_AssertEventID(CF_CMD_PURGE_QUEUE_ERR_EID);
}

void Test_CF_PurgeQueueCmd_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    uint8                   channel = Any_cf_channel();
    CF_PurgeQueueCmd_t      utbuf;
    CF_UnionArgs_Payload_t *data = &utbuf.Payload;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->byte[0] = channel;

    CF_AppData.hk.Payload.counters.cmd = 0;

    /* Act */
    CF_PurgeQueueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_PURGE_QUEUE_INF_EID);
}

/*******************************************************************************
**
**  CF_WriteQueueCmd tests
**
*******************************************************************************/

void Test_CF_WriteQueueCmd_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq                     = &utbuf.Payload;
    uint16                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* invalid channel */
    wq->chan = CF_NUM_CHANNELS;

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_CHAN_ERR_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq                     = &utbuf.Payload;
    uint16                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* invalid channel */
    wq->chan = Any_uint8_GreaterThan(CF_NUM_CHANNELS);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_CHAN_ERR_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq                     = &utbuf.Payload;
    uint16                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* invalid combination up direction, pending queue */
    wq->type  = CF_Type_up;
    wq->queue = CF_Queue_pend;

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_ARGS_ERR_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t             utbuf;
    CF_WriteQueue_Payload_t *      wq = &utbuf.Payload;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    uint16                         initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination up direction, NOT pending queue */
    wq->type  = CF_Type_up;
    wq->queue = Any_queue_Except_q_pend(); /* 0 is q_pend */

    /* invalid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_INT32_EQ(context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_OPEN_ERR_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t             utbuf;
    CF_WriteQueue_Payload_t *      wq = &utbuf.Payload;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    uint16                         initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination down direction, pending queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_pend;

    /* invalid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_INT32_EQ(context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_OPEN_ERR_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t       context_CF_WrappedOpenCreate;
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    uint16                               initial_hk_err_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_all;
    wq->queue = CF_Queue_all;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = 0;

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_RX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t       context_CF_WrappedOpenCreate;
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    int32                                context_CF_WrappedClose_fd;
    uint16                               initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_up;
    wq->queue = CF_Queue_active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_RX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t           context_CF_WrappedOpenCreate;
    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    int32                                    context_CF_WrappedClose_fd;
    uint16                                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_up;
    wq->queue = CF_Queue_history;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile), &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEHIST_RX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t       context_CF_WrappedOpenCreate;
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    int32                                context_CF_WrappedClose_fd;
    uint16                               initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_TX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryDataToFile_FailsOnSecondCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t       context_CF_WrappedOpenCreate;
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile[2];
    int32                                forced_return_CF_WriteTxnQueueDataToFile_1st_call = 0;
    int32                                forced_return_CF_WriteTxnQueueDataToFile_2nd_call = Any_int32_Except(0);
    int32                                context_CF_WrappedClose_fd;
    uint16                               initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile_1st_call);
    UT_SetDeferredRetcode(UT_KEY(CF_WriteTxnQueueDataToFile), 2, forced_return_CF_WriteTxnQueueDataToFile_2nd_call);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_TX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t       context_CF_WrappedOpenCreate;
    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    int32                                context_CF_WrappedClose_fd;
    uint16                               initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_pend;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile), &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_PEND_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t           context_CF_WrappedOpenCreate;
    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    int32                                    context_CF_WrappedClose_fd;
    uint16                                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_history;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile), &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEHIST_TX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_All(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile     = 0;
    int32                          forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_all;
    wq->queue = CF_Queue_all;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 3);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_History(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_all;
    wq->queue = CF_Queue_history;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Active(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_all;
    wq->queue = CF_Queue_active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Pend(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_all;
    wq->queue = CF_Queue_pend;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_UpAnd_q_All(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile     = 0;
    int32                          forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_up;
    wq->queue = CF_Queue_all;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_UpAnd_q_History(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_up;
    wq->queue = CF_Queue_history;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Active(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_up;
    wq->queue = CF_Queue_active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

/* Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Pend IS an error and is handled by a previous test */

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_All(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_all;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_History(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_history;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Active(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Pend(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    int32                          forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16                         initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan = Any_uint8_LessThan(CF_NUM_CHANNELS);

    /* valid combination all direction, all queue */
    wq->type  = CF_Type_down;
    wq->queue = CF_Queue_pend;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Positive();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate), &context_CF_WrappedOpenCreate, sizeof(context_CF_WrappedOpenCreate),
                     false);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

/*******************************************************************************
**
**  CF_ValidateChunkSizeCmd tests
**
*******************************************************************************/

void Test_CF_ValidateChunkSizeCmd_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1(void)
{
    /* Arrange */
    uint8                  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    size_t                 arg_val      = sizeof(CF_CFDP_PduFileDataContent_t) + 1;
    CF_ChanAction_Status_t local_result;

    /* Act */
    local_result = CF_ValidateChunkSizeCmd(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
}

void Test_CF_ValidateChunkSizeCmd_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1(void)
{
    /* Arrange */
    uint8                  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32                 arg_val      = Any_uint32_GreaterThan(sizeof(CF_CFDP_PduFileDataContent_t));
    CF_ChanAction_Status_t local_result;

    /* Act */
    local_result = CF_ValidateChunkSizeCmd(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
}

void Test_CF_ValidateChunkSizeCmd_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0(void)
{
    /* Arrange */
    uint8                  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    size_t                 arg_val      = sizeof(CF_CFDP_PduFileDataContent_t);
    CF_ChanAction_Status_t local_result;

    /* Act */
    local_result = CF_ValidateChunkSizeCmd(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_ValidateChunkSizeCmd_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0(void)
{
    /* Arrange */
    uint8                  arg_chan_num = Any_uint8(); /* value labeled as 'ignored' in func def */
    uint32                 arg_val      = Any_uint32_LessThan_or_EqualTo(sizeof(CF_CFDP_PduFileDataContent_t));
    CF_ChanAction_Status_t local_result;

    /* Act */
    local_result = CF_ValidateChunkSizeCmd(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

/*******************************************************************************
**
**  CF_ValidateMaxOutgoingCmd tests
**
*******************************************************************************/

void Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_IsNot_0_Return_0_Success(void)
{
    /* Arrange */
    uint32 arg_val      = Any_uint32_Except(0);
    uint8  arg_chan_num = Any_uint8(); /* Any_uint8() used here because it shows value does not matter in this test */
    CF_ChanAction_Status_t local_result;

    /* Act */
    local_result = CF_ValidateMaxOutgoingCmd(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success(void)
{
    /* Arrange */
    uint32 arg_val      = 0;
    uint8  arg_chan_num = Any_cf_chan_num(); /* Any_cf_chan_num used here because value matters to this test */
    CF_ConfigTable_t       config_table;
    CF_ChanAction_Status_t local_result;

    CF_AppData.config_table = &config_table;
    memset(CF_AppData.config_table->chan[arg_chan_num].sem_name, (char)Any_uint8_Except(0), 1);

    /* Act */
    local_result = CF_ValidateMaxOutgoingCmd(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_SUCCESS);
}

void Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail(void)
{
    /* Arrange */
    uint32 arg_val      = 0;
    uint8  arg_chan_num = Any_cf_chan_num(); /* Any_cf_chan_num used here because value matters to this test */
    CF_ConfigTable_t       config_table;
    CF_ChanAction_Status_t local_result;

    CF_AppData.config_table = &config_table;
    memset(CF_AppData.config_table->chan[arg_chan_num].sem_name, (char)0, 1);

    /* Act */
    local_result = CF_ValidateMaxOutgoingCmd(arg_val, arg_chan_num);

    /* Assert */
    UtAssert_INT32_EQ(local_result, CF_ChanAction_Status_ERROR);
}

/*******************************************************************************
**
**  CF_GetSetParamCmd tests
**
*******************************************************************************/

void Test_CF_GetSetParamCmd(void)
{
    /* Test cases for:
     * void CF_GetSetParamCmd(bool is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num);
     */

    /* Arrange */
    CF_ConfigTable_t    ut_config_table;
    CF_GetSet_ValueID_t param_id;
    uint16              expected_count;

    memset(&ut_config_table, 0, sizeof(ut_config_table));
    memset(&CF_AppData.hk.Payload.counters, 0, sizeof(CF_AppData.hk.Payload.counters));
    CF_AppData.config_table = &ut_config_table;
    expected_count          = 0;

    /* Nominal: "set" for each parameter */
    for (param_id = 0; param_id < CF_GetSet_ValueID_MAX; ++param_id)
    {
        UT_CF_ResetEventCapture();
        UtAssert_VOIDCALL(CF_GetSetParamCmd(true, param_id, 1 + param_id, UT_CFDP_CHANNEL));
        UT_CF_AssertEventID(CF_CMD_GETSET1_INF_EID);
        UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, ++expected_count);
    }

    /* each of the config parameters should have actually been set to a different value */
    UtAssert_UINT32_EQ(ut_config_table.ticks_per_second, 1);
    UtAssert_UINT32_EQ(ut_config_table.rx_crc_calc_bytes_per_wakeup, 2);
    UtAssert_UINT32_EQ(ut_config_table.chan[UT_CFDP_CHANNEL].ack_timer_s, 3);
    UtAssert_UINT32_EQ(ut_config_table.chan[UT_CFDP_CHANNEL].nak_timer_s, 4);
    UtAssert_UINT32_EQ(ut_config_table.chan[UT_CFDP_CHANNEL].inactivity_timer_s, 5);
    UtAssert_UINT32_EQ(ut_config_table.outgoing_file_chunk_size, 6);
    UtAssert_UINT32_EQ(ut_config_table.chan[UT_CFDP_CHANNEL].ack_limit, 7);
    UtAssert_UINT32_EQ(ut_config_table.chan[UT_CFDP_CHANNEL].nak_limit, 8);
    UtAssert_UINT32_EQ(ut_config_table.local_eid, 9);
    UtAssert_UINT32_EQ(ut_config_table.chan[UT_CFDP_CHANNEL].max_outgoing_messages_per_wakeup, 10);

    /* Nominal: "get" for each parameter */
    for (param_id = 0; param_id < CF_GetSet_ValueID_MAX; ++param_id)
    {
        UT_CF_ResetEventCapture();
        UtAssert_VOIDCALL(CF_GetSetParamCmd(false, param_id, 1, UT_CFDP_CHANNEL));
        UT_CF_AssertEventID(CF_CMD_GETSET2_INF_EID);
        UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, ++expected_count);
    }

    /* Bad param ID */
    UT_CF_ResetEventCapture();
    UtAssert_VOIDCALL(CF_GetSetParamCmd(false, CF_GetSet_ValueID_MAX, 0, UT_CFDP_CHANNEL));
    UT_CF_AssertEventID(CF_CMD_GETSET_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);

    /* Bad channel ID */
    UT_CF_ResetEventCapture();
    UtAssert_VOIDCALL(CF_GetSetParamCmd(false, 0, 0, CF_NUM_CHANNELS + 1));
    UT_CF_AssertEventID(CF_CMD_GETSET_CHAN_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 2);

    /* Validation fail */
    UT_CF_ResetEventCapture();
    UtAssert_VOIDCALL(CF_GetSetParamCmd(true, CF_GetSet_ValueID_outgoing_file_chunk_size,
                                        100 + sizeof(CF_CFDP_PduFileDataContent_t), UT_CFDP_CHANNEL));
    UT_CF_AssertEventID(CF_CMD_GETSET_VALIDATE_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 3);
}

/*******************************************************************************
**
**  CF_SetParamCmd tests
**
*******************************************************************************/

void Test_CF_SetParamCmd_Call_CF_GetSetParamCmd_With_cmd_key_And_cmd_value(void)
{
    /* Arrange */
    CF_SetParamCmd_t utbuf;
    CF_ConfigTable_t config_table;

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    utbuf.Payload.key      = CF_GetSet_ValueID_ticks_per_second;
    utbuf.Payload.value    = 1;
    utbuf.Payload.chan_num = 0;

    CF_AppData.config_table = &config_table;

    /* Act */
    CF_SetParamCmd(&utbuf);

    /* Assert */
    UtAssert_UINT32_EQ(CF_AppData.config_table->ticks_per_second, utbuf.Payload.value);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[0], CF_CMD_GETSET1_INF_EID);
    /* Assert for incremented counter() */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);
}

/*******************************************************************************
**
**  CF_GetParamCmd tests
**
*******************************************************************************/

void Test_CF_GetParamCmd_Call_CF_GetSetParamCmd_With_cmd_data_byte_0_AndConstantValue_0(void)
{
    /* Arrange */
    CF_GetParamCmd_t utbuf;
    CF_ConfigTable_t config_table;

    memset(&utbuf, 0, sizeof(utbuf));
    memset(&config_table, 0, sizeof(config_table));

    utbuf.Payload.key      = CF_GetSet_ValueID_ticks_per_second;
    utbuf.Payload.chan_num = 0;

    CF_AppData.config_table = &config_table;

    /* Act */
    CF_GetParamCmd(&utbuf);

    /* Assert */
    /* Note actual value not tested, just flow */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[0], CF_CMD_GETSET2_INF_EID);
    /* Assert for incremented counter() */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 1);
}

/*******************************************************************************
**
**  CF_EnableEngineCmd tests
**
*******************************************************************************/

void Test_CF_EnableEngineCmd_WithEngineNotEnableInitSuccessAndIncrementCmdCounter(void)
{
    /* Arrange */
    CF_EnableEngineCmd_t utbuf;
    uint32               forced_return_CF_CFDP_InitEngine = CFE_SUCCESS;
    uint16               initial_hk_cmd_counter           = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.engine.enabled = false;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableEngineCmd(&utbuf);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_ENGINE_INF_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_EnableEngineCmd_WithEngineNotEnableFailsInitSendEventAndIncrementErrCounter(void)
{
    /* Arrange */
    CF_EnableEngineCmd_t utbuf;
    uint32               forced_return_CF_CFDP_InitEngine = Any_uint32_Except(CF_ChanAction_Status_SUCCESS);
    uint16               initial_hk_err_counter           = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.engine.enabled = false;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);

    CF_AppData.hk.Payload.counters.err = initial_hk_err_counter;

    /* Act */
    CF_EnableEngineCmd(&utbuf);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_ENGINE_ERR_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_EnableEngineCmd_WithEngineEnableFailsSendEventAndIncrementCmdCounter(void)
{
    /* Arrange */
    CF_EnableEngineCmd_t utbuf;
    uint16               initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.engine.enabled = true;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableEngineCmd(&utbuf);

    UT_GetStubCount(UT_KEY(CF_CFDP_InitEngine));

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENG_ALREADY_ENA_INF_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

/*******************************************************************************
**
**  CF_DisableEngineCmd tests
**
*******************************************************************************/

void Test_CF_DisableEngineCmd_SuccessWhenEngineEnabledAndIncrementCmdCounter(void)
{
    /* Arrange */
    CF_DisableEngineCmd_t utbuf;
    uint16                initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.engine.enabled = true;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DisableEngineCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_DISABLE_ENGINE_INF_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_DisableEngineCmd_WhenEngineDisabledAndIncrementCmdCounter(void)
{
    /* Arrange */
    CF_DisableEngineCmd_t utbuf;
    uint16                initial_hk_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    CF_AppData.engine.enabled = false;

    CF_AppData.hk.Payload.counters.cmd = initial_hk_counter;

    /* Act */
    CF_DisableEngineCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENG_ALREADY_DIS_INF_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, (initial_hk_counter + 1) & 0xFFFF);
}

/*******************************************************************************
**
**  CF_SendHkCmd tests - full coverage
**
*******************************************************************************/

void Test_CF_SendHkCmd(void)
{
    /* Act */
    CF_SendHkCmd(NULL);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_TimeStampMsg, 1);
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
}

/*******************************************************************************
**
**  CF_WakeupCmd tests
**
*******************************************************************************/

void Test_CF_WakeupCmd(void)
{
    /* Arrange */
    /* No Arrange Required */

    /* Act */
    CF_WakeupCmd(NULL);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_CycleEngine, 1);
}

/*******************************************************************************
**
**  cf_cmd tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_NoopCmd_tests(void)
{
    UtTest_Add(Test_CF_NoopCmd_SendNoopEventAndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_NoopCmd_SendNoopEventAndAcceptCommand");
}

void add_CF_ResetCountersCmd_tests(void)
{
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent");
}

void add_CF_TxFileCmd_tests(void)
{
    UtTest_Add(Test_CF_TxFileCmd, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_TxFileCmd");
}

void add_CF_PlaybackDirCmd_tests(void)
{
    UtTest_Add(Test_CF_PlaybackDirCmd, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_PlaybackDirCmd");
}

void add_CF_DoChanAction_tests(void)
{
    UtTest_Add(Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenAny_fn_returns_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenAll_fn_return_1_Return_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenAll_fn_return_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_CF_ALL_CHANNELS_WhenNo_fn_returns_0_Return_0");
    UtTest_Add(Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_1_Return_1");
    UtTest_Add(Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChannel_fn_ActionReturns_0_Return_1");
    UtTest_Add(Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DoChanAction_WhenChanNumberEq_CF_NUM_CHANNELS_Return_neg1_And_SendEvent_");
    UtTest_Add(Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoChanAction_WhenBadChannelNumber_Return_neg1_And_SendEvent");
}

void add_CF_DoFreezeThaw_tests(void)
{
    UtTest_Add(Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoFreezeThaw_Set_frozen_ToGiven_context_barg_AndReturn_0");
}

void add_CF_FreezeCmd_tests(void)
{
    UtTest_Add(Test_CF_FreezeCmd_Set_frozen_To_1_AndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_FreezeCmd_Set_frozen_To_1_AndAcceptCommand");
    UtTest_Add(Test_CF_FreezeCmd_Set_frozen_To_1_AndRejectCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_FreezeCmd_Set_frozen_To_1_AndRejectCommand");
}

void add_CF_ThawCmd_tests(void)
{
    UtTest_Add(Test_CF_ThawCmd_Set_frozen_To_0_AndAcceptCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ThawCmd_Set_frozen_To_0_AndAcceptCommand");
    UtTest_Add(Test_CF_ThawCmd_Set_frozen_To_0_AndRejectCommand, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ThawCmd_Set_frozen_To_0_AndRejectCommand");
}

void add_CF_FindTransactionBySequenceNumberAllChannels_tests(void)
{
    UtTest_Add(Test_CF_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_FindTransactionBySequenceNumberAllChannels_WhenNoTransactionFoundReturn_NULL");
    UtTest_Add(Test_CF_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_FindTransactionBySequenceNumberAllChannels_Return_TransactionFound");
}

void add_CF_TsnChanAction_tests(void)
{
    UtTest_Add(Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_CF_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_SendEvent_cmd_chan_Eq_CF_COMPOUND_KEY_TransactionNotFoundAndReturn_neg1_Fail");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_Eq_CF_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_Eq_CF_COMPOUND_KEY_TransactionFoundRun_fn_AndReturn_CFE_SUCCESS");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_Eq_CF_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_Eq_CF_ALL_CHANNELS_Return_CF_TraverseAllTransactions_All_Channels");
    UtTest_Add(Test_CF_TsnChanAction_cmd_chan_IsASingleChannel, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_chan_IsASingleChannel");
    UtTest_Add(Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_TsnChanAction_cmd_FailBecause_cmd_chan_IsInvalid");
}

void add_CF_DoSuspRes_Txn_tests(void)
{
    UtTest_Add(Test_CF_DoSuspRes_Txn_Set_context_same_To_1_suspended_Eq_action, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoSuspRes_Txn_Set_context_same_To_1_suspended_Eq_action");
    UtTest_Add(Test_CF_DoSuspRes_Txn_When_suspended_NotEqTo_action_Set_suspended_To_action, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoSuspRes_Txn_When_suspended_NotEqTo_action_Set_suspended_To_action");
}

void add_CF_DoSuspRes_tests(void)
{
    UtTest_Add(Test_CF_DoSuspRes, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_DoSuspRes");
}

void add_CF_SuspendCmd_tests(void)
{
    UtTest_Add(Test_CF_SuspendCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_SuspendCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_1");
}

void add_CF_ResumeCmd_tests(void)
{
    UtTest_Add(Test_CF_ResumeCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ResumeCmd_Call_CF_DoSuspRes_WithGiven_msg_And_action_0");
}

void add_CF_Cancel_TxnCmd_tests(void)
{
    UtTest_Add(Test_CF_Cancel_TxnCmd_Call_CF_CFDP_CancelTransaction_WithGiven_t, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_Cancel_TxnCmd_Call_CF_CFDP_CancelTransaction_WithGiven_t");
}

void add_CF_CancelCmd_tests(void)
{
    UtTest_Add(Test_CF_CancelCmd_Failure, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_CancelCmd_Failure");
    UtTest_Add(Test_CF_CancelCmd_Success, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_CancelCmd_Success");
}

void add_CF_Abandon_TxnCmd_tests(void)
{
    UtTest_Add(Test_CF_Abandon_TxnCmd_Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_Abandon_TxnCmd_Call_CF_CFDP_ResetTransaction_WithGiven_t_And_0");
}

void add_CF_AbandonCmd_tests(void)
{
    UtTest_Add(Test_CF_AbandonCmd_Failure, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_AbandonCmd_Failure");
    UtTest_Add(Test_CF_AbandonCmd_Success, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_AbandonCmd_Success");
}

void add_CF_DoEnableDisableDequeue_tests(void)
{
    UtTest_Add(Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DoEnableDisableDequeue_Set_chan_num_EnabledFlagTo_context_barg");
}

void add_CF_EnableDequeueCmd_tests(void)
{
    UtTest_Add(Test_CF_EnableDequeueCmd_Success, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_EnableDequeueCmd_Success");
    UtTest_Add(Test_CF_EnableDequeueCmd_Failure, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_EnableDequeueCmd_Failure");
}

void add_CF_DisableDequeueCmd_tests(void)
{
    UtTest_Add(Test_CF_DisableDequeueCmd_Success, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DisableDequeueCmd_Success");
    UtTest_Add(Test_CF_DisableDequeueCmd_Failure, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DisableDequeueCmd_Failure");
}

void add_CF_DoEnableDisablePolldir_tests(void)
{
    UtTest_Add(Test_CF_DoEnableDisablePolldir_When_CF_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_When_CF_ALL_CHANNELS_SetAllPolldirsInChannelEnabledTo_context_barg");
    UtTest_Add(
        Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_DoEnableDisablePolldir_WhenSetToSpecificPolldirSetPolldirFrom_context_ChannelEnabledTo_context_barg");
    UtTest_Add(Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_FailPolldirEq_CF_MAX_POLLING_DIR_PER_CHAN_AndSendEvent");
    UtTest_Add(Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DoEnableDisablePolldir_FailAnyBadPolldirSendEvent");
}

void add_CF_EnablePolldirCmd_tests(void)
{
    UtTest_Add(Test_CF_EnablePolldirCmd_SuccessWhenActionSuccess, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_EnablePolldirCmd_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_EnablePolldirCmd_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_EnablePolldirCmd_FailWhenActionFail");
}

void add_CF_DisablePolldirCmd_tests(void)
{
    UtTest_Add(Test_CF_DisablePolldirCmd_SuccessWhenActionSuccess, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DisablePolldirCmd_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_DisablePolldirCmd_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_DisablePolldirCmd_FailWhenActionFail");
}

void add_CF_PurgeHistory_tests(void)
{
    UtTest_Add(Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_PurgeHistory_Call_CF_CFDP_ResetHistory_AndReturn_CLIST_CONT");
}

void add_CF_PurgeTransaction_tests(void)
{
    UtTest_Add(Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_PurgeTransaction_Call_CF_CFDP_ResetTransaction_AndReturn_CLIST_CONT");
}

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
}

void add_CF_PurgeQueueCmd_tests(void)
{
    UtTest_Add(Test_CF_PurgeQueueCmd_FailWhenActionFail, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_PurgeQueueCmd_FailWhenActionFail");
    UtTest_Add(Test_CF_PurgeQueueCmd_SuccessWhenActionSuccess, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_PurgeQueueCmd_SuccessWhenActionSuccess");
}

void add_CF_WriteQueueCmd_tests(void)
{
    UtTest_Add(Test_CF_WriteQueueCmd_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_WriteQueueCmd_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_WriteQueueCmd_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_When_chan_GreaterThan_CF_NUM_CAHNNELS_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_"
        "SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_"
        "CF_"
        "WrappedClose_SendEventClosesAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_"
        "IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_All, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_All");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_History, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_History");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Active, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Active");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Pend, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Pend");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_UpAnd_q_All, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_UpAnd_q_All");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_UpAnd_q_History, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_UpAnd_q_History");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Active, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Active");
    /* see Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand */
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_All, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_All");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_History, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_History");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Active, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Active");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Pend, cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Pend");
}

void add_CF_ValidateChunkSizeCmd_tests(void)
{
    UtTest_Add(Test_CF_ValidateChunkSizeCmd_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ValidateChunkSizeCmd_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1");
    UtTest_Add(Test_CF_ValidateChunkSizeCmd_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ValidateChunkSizeCmd_Any_val_GreaterThan_pdu_fd_data_t_FailAndReturn_1");
    UtTest_Add(Test_CF_ValidateChunkSizeCmd_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ValidateChunkSizeCmd_val_SizeOf_pdu_fd_data_t_SuccessAndReturn_0");
    UtTest_Add(Test_CF_ValidateChunkSizeCmd_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ValidateChunkSizeCmd_val_LessThanOrEqSizeOf_pdu_fd_data_t_SuccessAndReturn_0");
}

void add_CF_ValidateMaxOutgoingCmd_tests(void)
{
    UtTest_Add(Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_IsNot_0_Return_0_Success, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_IsNot_0_Return_0_Success");
    UtTest_Add(Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success,
               cf_cmd_tests_Setup, cf_cmd_tests_Teardown,
               "Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_Is_0_But_sem_name_IsNot_NULL_Return_0_Success");
    UtTest_Add(Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ValidateMaxOutgoingCmd_WhenGiven_val_Is_0_And_sem_name_Is_NULL_Return_1_Fail");
}

void add_CF_GetSetParamCmd_tests(void)
{
    UtTest_Add(Test_CF_GetSetParamCmd, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "CF_GetSetParamCmd");
}

void add_CF_SetParamCmd_tests(void)
{
    UtTest_Add(Test_CF_SetParamCmd_Call_CF_GetSetParamCmd_With_cmd_key_And_cmd_value, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_SetParamCmd_Call_CF_GetSetParamCmd_With_cmd_key_And_cmd_value");
}

void add_CF_GetParamCmd_tests(void)
{
    UtTest_Add(Test_CF_GetParamCmd_Call_CF_GetSetParamCmd_With_cmd_data_byte_0_AndConstantValue_0, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_GetParamCmd_Call_CF_GetSetParamCmd_With_cmd_data_byte_0_AndConstantValue_0");
}

void add_CF_EnableEngineCmd_tests(void)
{
    UtTest_Add(Test_CF_EnableEngineCmd_WithEngineNotEnableInitSuccessAndIncrementCmdCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_EnableEngineCmd_WithEngineNotEnableInitSuccessAndIncrementCmdCounter");
    UtTest_Add(Test_CF_EnableEngineCmd_WithEngineNotEnableFailsInitSendEventAndIncrementErrCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnableEngineCmd_WithEngineNotEnableFailsInitSendEventAndIncrementErrCounter");
    UtTest_Add(Test_CF_EnableEngineCmd_WithEngineEnableFailsSendEventAndIncrementCmdCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_EnableEngineCmd_WithEngineEnableFailsSendEventAndIncrementCmdCounter");
}

void add_CF_DisableEngineCmd_tests(void)
{
    UtTest_Add(Test_CF_DisableEngineCmd_SuccessWhenEngineEnabledAndIncrementCmdCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdCounter");
    UtTest_Add(Test_CF_DisableEngineCmd_WhenEngineDisabledAndIncrementCmdCounter, cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown, "Test_CF_DisableEngineCmd_WhenEngineDisabledAndIncrementCmdCounter");
}

void add_CF_SendHkCmd_tests(void)
{
    UtTest_Add(Test_CF_SendHkCmd, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_SendHkCmd");
}

void add_CF_WakeupCmd_tests(void)
{
    UtTest_Add(Test_CF_WakeupCmd, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_WakeupCmd");
}

/*******************************************************************************
**
**  cf_cmd_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_NoopCmd_tests();

    add_CF_ResetCountersCmd_tests();

    add_CF_TxFileCmd_tests();

    add_CF_PlaybackDirCmd_tests();

    add_CF_DoChanAction_tests();

    add_CF_DoFreezeThaw_tests();

    add_CF_FreezeCmd_tests();

    add_CF_ThawCmd_tests();

    add_CF_FindTransactionBySequenceNumberAllChannels_tests();

    add_CF_TsnChanAction_tests();

    add_CF_DoSuspRes_Txn_tests();

    add_CF_DoSuspRes_tests();

    add_CF_SuspendCmd_tests();

    add_CF_ResumeCmd_tests();

    add_CF_Cancel_TxnCmd_tests();

    add_CF_CancelCmd_tests();

    add_CF_Abandon_TxnCmd_tests();

    add_CF_AbandonCmd_tests();

    add_CF_DoEnableDisableDequeue_tests();

    add_CF_EnableDequeueCmd_tests();

    add_CF_DisableDequeueCmd_tests();

    add_CF_DoEnableDisablePolldir_tests();

    add_CF_EnablePolldirCmd_tests();

    add_CF_DisablePolldirCmd_tests();

    add_CF_PurgeHistory_tests();

    add_CF_PurgeTransaction_tests();

    add_CF_DoPurgeQueue_tests();

    add_CF_PurgeQueueCmd_tests();

    add_CF_WriteQueueCmd_tests();

    add_CF_ValidateChunkSizeCmd_tests();

    add_CF_ValidateMaxOutgoingCmd_tests();

    add_CF_GetSetParamCmd_tests();

    add_CF_SetParamCmd_tests();

    add_CF_GetParamCmd_tests();

    add_CF_EnableEngineCmd_tests();

    add_CF_DisableEngineCmd_tests();

    add_CF_SendHkCmd_tests();

    add_CF_WakeupCmd_tests();
}
