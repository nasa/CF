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
#include "cf_eventids.h"
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

CF_EntityId_t Any_CF_EntityId_t(void)
{
    return (CF_EntityId_t)Any_uint8();
}

/* uint8 used for Any_cf_polldir likely there will never be that many polldirs */
CF_PollIdxSelect_t Any_cf_polldir(void)
{
    return 1 + Any_uint8_LessThan(CF_MAX_POLLING_DIR_PER_CHAN);
}

/* bool_arg_t_barg should only be 0 or 1 (Boolean) */
uint8 Any_bool_arg_t_barg(void)
{
    return AnyCoinFlip();
}

CF_QueueSelect_Enum_t Any_queue_Except_q_pend(void)
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
    void             *context;
} CF_TsnChanAction_fn_t_context_t;

CFE_Status_t Chan_action_fn_t(CF_Channel_t *chan_ptr, void *context)
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
    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_NoopCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_NOOP_INF_EID);
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

/*******************************************************************************
**
**  CF_ResetCountersCmd tests
**
*******************************************************************************/

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t          utbuf;
    CF_ResetCountersCmd_Payload_t *data                   = &utbuf.Payload;
    uint16                         initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->ResetType = 5; /* 5 is size of 'names' */

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_RESET_INVALID_ERR_EID);
    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t          utbuf;
    CF_ResetCountersCmd_Payload_t *data                   = &utbuf.Payload;
    uint16                         initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->ResetType = Any_uint8_GreaterThan(5); /* 5 is size of 'names' */

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_RESET_INVALID_ERR_EID);
    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t          utbuf;
    CF_ResetCountersCmd_Payload_t *data = &utbuf.Payload;

    memset(&utbuf, 0, sizeof(utbuf));

    data->ResetType         = CF_Reset_command;
    CF_AppData.counters.cmd = Any_uint16_Except(0);
    CF_AppData.counters.err = Any_uint16_Except(0);

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);
    UtAssert_ZERO(CF_AppData.counters.cmd);
    UtAssert_ZERO(CF_AppData.counters.err);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t          utbuf;
    CF_ResetCountersCmd_Payload_t *data                   = &utbuf.Payload;
    uint16                         initial_hk_cmd_counter = Any_uint16();
    CF_Channel_t                  *chan                   = UT_CFDP_CHANNEL_PTR;

    memset(&utbuf, 0, sizeof(utbuf));

    data->ResetType = CF_Reset_fault;

    chan->stat.counters.fault.file_open          = Any_uint16_Except(0);
    chan->stat.counters.fault.file_read          = Any_uint16_Except(0);
    chan->stat.counters.fault.file_seek          = Any_uint16_Except(0);
    chan->stat.counters.fault.file_write         = Any_uint16_Except(0);
    chan->stat.counters.fault.file_rename        = Any_uint16_Except(0);
    chan->stat.counters.fault.directory_read     = Any_uint16_Except(0);
    chan->stat.counters.fault.crc_mismatch       = Any_uint16_Except(0);
    chan->stat.counters.fault.file_size_mismatch = Any_uint16_Except(0);
    chan->stat.counters.fault.nak_limit          = Any_uint16_Except(0);
    chan->stat.counters.fault.ack_limit          = Any_uint16_Except(0);
    chan->stat.counters.fault.inactivity_timer   = Any_uint16_Except(0);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_ForEachChannel, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    UtAssert_ZERO(chan->stat.counters.fault.file_open);
    UtAssert_ZERO(chan->stat.counters.fault.file_read);
    UtAssert_ZERO(chan->stat.counters.fault.file_seek);
    UtAssert_ZERO(chan->stat.counters.fault.file_write);
    UtAssert_ZERO(chan->stat.counters.fault.file_rename);
    UtAssert_ZERO(chan->stat.counters.fault.directory_read);
    UtAssert_ZERO(chan->stat.counters.fault.crc_mismatch);
    UtAssert_ZERO(chan->stat.counters.fault.file_size_mismatch);
    UtAssert_ZERO(chan->stat.counters.fault.nak_limit);
    UtAssert_ZERO(chan->stat.counters.fault.ack_limit);
    UtAssert_ZERO(chan->stat.counters.fault.inactivity_timer);
    UtAssert_MemCmpValue(&chan->stat.counters.fault,
                         0,
                         sizeof(chan->stat.counters.fault),
                         "fault channel was completely cleared to 0");
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t          utbuf;
    CF_ResetCountersCmd_Payload_t *data                   = &utbuf.Payload;
    uint16                         initial_hk_cmd_counter = Any_uint16();
    CF_Channel_t                  *chan                   = UT_CFDP_CHANNEL_PTR;

    memset(&utbuf, 0, sizeof(utbuf));

    data->ResetType = CF_Reset_up;

    chan->stat.counters.recv.file_data_bytes      = Any_uint64_Except(0);
    chan->stat.counters.recv.pdu                  = Any_uint32_Except(0);
    chan->stat.counters.recv.error                = Any_uint32_Except(0);
    chan->stat.counters.recv.spurious             = Any_uint16_Except(0);
    chan->stat.counters.recv.dropped              = Any_uint16_Except(0);
    chan->stat.counters.recv.nak_segment_requests = Any_uint32_Except(0);
    chan->stat.counters.recv.files_recv           = Any_uint32_Except(0);
    chan->stat.counters.recv.files_started        = Any_uint32_Except(0);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_ForEachChannel, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    UtAssert_ZERO(chan->stat.counters.recv.file_data_bytes);
    UtAssert_ZERO(chan->stat.counters.recv.pdu);
    UtAssert_ZERO(chan->stat.counters.recv.error);
    UtAssert_ZERO(chan->stat.counters.recv.spurious);
    UtAssert_ZERO(chan->stat.counters.recv.pdu);
    UtAssert_ZERO(chan->stat.counters.recv.nak_segment_requests);
    UtAssert_ZERO(chan->stat.counters.recv.files_recv);
    UtAssert_ZERO(chan->stat.counters.recv.files_started);
    UtAssert_MemCmpValue(&chan->stat.counters.recv,
                         0,
                         sizeof(chan->stat.counters.recv),
                         "recv channel was completely cleared to 0");
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t          utbuf;
    CF_ResetCountersCmd_Payload_t *data                   = &utbuf.Payload;
    uint16                         initial_hk_cmd_counter = Any_uint16();
    CF_Channel_t                  *chan                   = UT_CFDP_CHANNEL_PTR;

    memset(&utbuf, 0, sizeof(utbuf));

    data->ResetType = CF_Reset_down;

    chan->stat.counters.sent.file_data_bytes      = Any_uint64_Except(0);
    chan->stat.counters.sent.nak_segment_requests = Any_uint32_Except(0);
    chan->stat.counters.sent.pdu                  = Any_uint32_Except(0);
    chan->stat.counters.sent.files_sent           = Any_uint32_Except(0);
    chan->stat.counters.sent.files_started        = Any_uint32_Except(0);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_ForEachChannel, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    UtAssert_ZERO(chan->stat.counters.sent.file_data_bytes);
    UtAssert_ZERO(chan->stat.counters.sent.nak_segment_requests);
    UtAssert_ZERO(chan->stat.counters.sent.pdu);
    UtAssert_ZERO(chan->stat.counters.sent.files_sent);
    UtAssert_ZERO(chan->stat.counters.sent.files_started);
    UtAssert_MemCmpValue(&chan->stat.counters.sent,
                         0,
                         sizeof(chan->stat.counters.sent),
                         "sent channel was completely cleared to 0");
    /* Assert to show counter incremented */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent(void)
{
    /* Arrange */
    CF_ResetCountersCmd_t          utbuf;
    CF_ResetCountersCmd_Payload_t *data = &utbuf.Payload;
    CF_Channel_t                  *chan = UT_CFDP_CHANNEL_PTR;

    memset(&utbuf, 0, sizeof(utbuf));

    data->ResetType = CF_Reset_all;

    CF_AppData.counters.cmd = Any_uint16_Except(0);
    CF_AppData.counters.err = Any_uint16_Except(0);

    chan->stat.counters.fault.file_open          = Any_uint16_Except(0);
    chan->stat.counters.fault.file_read          = Any_uint16_Except(0);
    chan->stat.counters.fault.file_seek          = Any_uint16_Except(0);
    chan->stat.counters.fault.file_write         = Any_uint16_Except(0);
    chan->stat.counters.fault.file_rename        = Any_uint16_Except(0);
    chan->stat.counters.fault.directory_read     = Any_uint16_Except(0);
    chan->stat.counters.fault.crc_mismatch       = Any_uint16_Except(0);
    chan->stat.counters.fault.file_size_mismatch = Any_uint16_Except(0);
    chan->stat.counters.fault.nak_limit          = Any_uint16_Except(0);
    chan->stat.counters.fault.ack_limit          = Any_uint16_Except(0);
    chan->stat.counters.fault.inactivity_timer   = Any_uint16_Except(0);

    chan->stat.counters.recv.file_data_bytes      = Any_uint64_Except(0);
    chan->stat.counters.recv.pdu                  = Any_uint32_Except(0);
    chan->stat.counters.recv.error                = Any_uint32_Except(0);
    chan->stat.counters.recv.spurious             = Any_uint16_Except(0);
    chan->stat.counters.recv.dropped              = Any_uint16_Except(0);
    chan->stat.counters.recv.nak_segment_requests = Any_uint32_Except(0);
    chan->stat.counters.recv.files_recv           = Any_uint32_Except(0);
    chan->stat.counters.recv.files_started        = Any_uint32_Except(0);

    chan->stat.counters.sent.file_data_bytes      = Any_uint64_Except(0);
    chan->stat.counters.sent.nak_segment_requests = Any_uint32_Except(0);
    chan->stat.counters.sent.pdu                  = Any_uint32_Except(0);
    chan->stat.counters.sent.files_sent           = Any_uint32_Except(0);
    chan->stat.counters.sent.files_started        = Any_uint32_Except(0);

    /* Act */
    CF_ResetCountersCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_ForEachChannel, 1);
    UT_CF_AssertEventID(CF_RESET_INF_EID);

    UtAssert_ZERO(CF_AppData.counters.cmd);
    UtAssert_ZERO(CF_AppData.counters.err);
    UtAssert_MemCmpValue(&chan->stat.counters.fault,
                         0,
                         sizeof(chan->stat.counters.fault),
                         "fault channel was completely cleared to 0");
    UtAssert_MemCmpValue(&chan->stat.counters.recv,
                         0,
                         sizeof(chan->stat.counters.recv),
                         "recv channel was completely cleared to 0");
    UtAssert_MemCmpValue(&chan->stat.counters.sent,
                         0,
                         sizeof(chan->stat.counters.sent),
                         "sent channel was completely cleared to 0");
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

    memset(&CF_AppData.counters, 0, sizeof(CF_AppData.counters));

    /* nominal, all zero should pass checks, just calls CF_CFDP_TxFile */
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = CF_CFDP_Class_1;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_TX_FILE_INF_EID);

    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = CF_CFDP_Class_2;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_TX_FILE_INF_EID);

    /* out of range arguments: bad class */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = 10;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);

    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = -10;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 2);

    /* out of range arguments: bad channel (nonzero) */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = CF_ChannelSelect_FromInt(CF_NUM_CHANNELS + 1);
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 3);

    /* out of range arguments: bad channel (0) */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = CF_ChannelSelect_FromInt(0);
    msg->cfdp_class = CF_CFDP_Class_1;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 4);

    /* out of range arguments: bad keep */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = UT_CFDP_CHANNEL;
    msg->keep     = 15;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 5);

    /* CF_CFDP_TxFile fails*/
    UT_CF_ResetEventCapture();
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxFile), -1);
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = UT_CFDP_CHANNEL;
    UtAssert_VOIDCALL(CF_TxFileCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_TX_FILE_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 6);
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

    memset(&CF_AppData.counters, 0, sizeof(CF_AppData.counters));

    /* nominal, all zero should pass checks, just calls CF_CFDP_PlaybackDir */
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = CF_CFDP_Class_1;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);

    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = CF_CFDP_Class_2;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 2);

    /* out of range arguments: bad class */
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = 10;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);

    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num   = UT_CFDP_CHANNEL;
    msg->cfdp_class = -10;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 2);

    /* out of range arguments: bad channel */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = CF_ChannelSelect_FromInt(CF_NUM_CHANNELS + 1);
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 3);

    /* out of range arguments: bad keep */
    UT_CF_ResetEventCapture();
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = UT_CFDP_CHANNEL;
    msg->keep     = 15;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_BAD_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 4);

    /* CF_CFDP_PlaybackDir fails*/
    UT_CF_ResetEventCapture();
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_PlaybackDir), -1);
    memset(msg, 0, sizeof(*msg));
    msg->chan_num = UT_CFDP_CHANNEL;
    UtAssert_VOIDCALL(CF_PlaybackDirCmd(&utbuf));
    UT_CF_AssertEventID(CF_CMD_PLAYBACK_DIR_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 5);
}

/**************************************************************************
**
**  CF_FreezeCmd tests
**
*******************************************************************************/

void Test_CF_FreezeCmd_Set_frozen_To_1_AndAcceptCommand(void)
{
    /* Arrange */
    CF_FreezeCmd_t utbuf;
    uint16         initial_hk_cmd_counter = Any_uint16();
    CF_Channel_t  *chan;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    chan              = UT_CFDP_CHANNEL_PTR;
    chan->stat.frozen = false;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_FreezeCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(chan->stat.frozen == 1,
                  "CF_DoFreezeThaw set frozen to %d and should be 1 (freeze = 1))",
                  chan->stat.frozen);
    UtAssert_True(CF_AppData.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.counters.cmd,
                  initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_FREEZE_INF_EID);
}

void Test_CF_FreezeCmd_Set_frozen_To_1_AndRejectCommand(void)
{
    /* Arrange */
    CF_FreezeCmd_t              utbuf;
    CF_ChannelSelect_Payload_t *data = &utbuf.Payload;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = UT_CFDP_INVALID_CHANNEL;

    CF_AppData.counters.cmd = 0;

    /* Act */
    CF_FreezeCmd(&utbuf);

    /* Assert */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
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
    CF_ThawCmd_t                utbuf;
    CF_ChannelSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_cmd_counter = Any_uint16();

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    CF_Channel_t *chan;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = Any_cf_chan_num();
    chan                = CF_GetChannelPtr(data->ChannelSelect);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_ThawCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(chan->stat.frozen == 0,
                  "CF_DoFreezeThaw set frozen to %d and should be 0 (thaw = 0))",
                  chan->stat.frozen);
    UtAssert_True(CF_AppData.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.counters.cmd,
                  initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_THAW_INF_EID);
}

void Test_CF_ThawCmd_Set_frozen_To_0_AndRejectCommand(void)
{
    /* Arrange */
    CF_ThawCmd_t                utbuf;
    CF_ChannelSelect_Payload_t *data = &utbuf.Payload;

    /* Arrange unstubbable: CF_DoFreezeThaw via CF_DoChanAction */
    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = UT_CFDP_INVALID_CHANNEL;

    CF_AppData.counters.cmd = 0;

    /* Act */
    CF_ThawCmd(&utbuf);

    /* Assert */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_THAW_ERR_EID);
}

/*******************************************************************************
**
**  CF_SuspendCmd tests
**
*******************************************************************************/

void Test_CF_SuspendCmd_InvalidChannel(void)
{
    /* Arrange */
    CF_SuspendCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Set to invalid channel */
    utbuf.Payload.chan_num = UT_CFDP_INVALID_CHANNEL;

    /* Act */
    CF_SuspendCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_SUSPRES_CHAN_ERR_EID);

    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
}

void Test_CF_SuspendCmd_Success(void)
{
    CF_SuspendCmd_t  utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    /* This needs to invoke the callback thru CF_TraverseAllTransactions */
    UT_SetHandlerFunction(UT_KEY(CF_TraverseAllTransactions), UT_AltHandler_CF_TraverseAllTransactions_InvokeCb, &txn);

    /* Act */
    CF_SuspendCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_SUSPRES_INF_EID);
}

/*******************************************************************************
**
**  CF_ResumeCmd tests
**
*******************************************************************************/

void Test_CF_ResumeCmd_InvalidChannel(void)
{
    CF_ResumeCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Invalid channel */
    utbuf.Payload.chan_num = UT_CFDP_INVALID_CHANNEL;

    /* Act */
    CF_ResumeCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_SUSPRES_CHAN_ERR_EID);

    /* Assert incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
}

void Test_CF_ResumeCmd_SuccessSame(void)
{
    CF_ResumeCmd_t   utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    /* This needs to invoke the callback thru CF_TraverseAllTransactions */
    UT_SetHandlerFunction(UT_KEY(CF_TraverseAllTransactions), UT_AltHandler_CF_TraverseAllTransactions_InvokeCb, &txn);

    /* Act */
    CF_ResumeCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_SUSPRES_SAME_INF_EID);
}

/*******************************************************************************
**
**  CF_CancelCmd tests
**
*******************************************************************************/

void Test_CF_CancelCmd_Success(void)
{
    CF_CancelCmd_t   utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    /* This needs to invoke the callback thru CF_TraverseAllTransactions */
    UT_SetHandlerFunction(UT_KEY(CF_TraverseAllTransactions), UT_AltHandler_CF_TraverseAllTransactions_InvokeCb, &txn);

    /* Act */
    CF_CancelCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CANCEL_INF_EID);
}

void Test_CF_CancelCmd_SuccessSingleTxn(void)
{
    CF_CancelCmd_t   utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num   = UT_CFDP_CHANNEL;
    utbuf.Payload.use_ts_eid = true;

    /* This needs to return a valid pointer from CF_FindTransactionBySequenceNumber */
    UT_SetHandlerFunction(UT_KEY(CF_FindTransactionBySequenceNumber),
                          UT_AltHandler_CF_FindTransactionBySequenceNumber,
                          &txn);

    /* Act */
    CF_CancelCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CANCEL_INF_EID);
}

void Test_CF_CancelCmd_Failure(void)
{
    CF_CancelCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    /* Act */
    CF_CancelCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_CANCEL_CHAN_ERR_EID);
}

void Test_CF_CancelCmd_FailSingleTxn(void)
{
    CF_CancelCmd_t   utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num   = UT_CFDP_CHANNEL;
    utbuf.Payload.use_ts_eid = true;

    /* Note: default behavior of CF_FindTransactionBySequenceNumber() is to return NULL */

    /* Act */
    CF_CancelCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_TRANS_NOT_FOUND_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_CANCEL_CHAN_ERR_EID);
}

/*******************************************************************************
**
**  CF_AbandonCmd tests
**
*******************************************************************************/

void Test_CF_AbandonCmd_Success(void)
{
    CF_AbandonCmd_t  utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    /* This needs to invoke the callback thru CF_TraverseAllTransactions */
    UT_SetHandlerFunction(UT_KEY(CF_TraverseAllTransactions), UT_AltHandler_CF_TraverseAllTransactions_InvokeCb, &txn);

    /* Act */
    CF_AbandonCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ABANDON_INF_EID);
}

void Test_CF_AbandonCmd_SuccessSingleTxn(void)
{
    CF_AbandonCmd_t  utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num   = UT_CFDP_CHANNEL;
    utbuf.Payload.use_ts_eid = true;

    /* This needs to return a valid pointer from CF_FindTransactionBySequenceNumber */
    UT_SetHandlerFunction(UT_KEY(CF_FindTransactionBySequenceNumber),
                          UT_AltHandler_CF_FindTransactionBySequenceNumber,
                          &txn);

    /* Act */
    CF_AbandonCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ABANDON_INF_EID);
}

void Test_CF_AbandonCmd_Failure(void)
{
    CF_AbandonCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    /* Nominally returns number of transactions acted on, force failure */
    UT_SetDefaultReturnValue(UT_KEY(CF_TraverseAllTransactions), 0);

    /* Act */
    CF_AbandonCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_TraverseAllTransactions, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_ABANDON_CHAN_ERR_EID);
}

void Test_CF_AbandonCmd_FailSingleTxn(void)
{
    CF_AbandonCmd_t  utbuf;
    CF_Transaction_t txn;

    memset(&txn, 0, sizeof(txn));
    memset(&utbuf, 0, sizeof(utbuf));
    utbuf.Payload.chan_num   = UT_CFDP_CHANNEL;
    utbuf.Payload.use_ts_eid = true;

    /* Note: default behavior of CF_FindTransactionBySequenceNumber() is to return NULL */

    /* Act */
    CF_AbandonCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_TRANS_NOT_FOUND_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_ABANDON_CHAN_ERR_EID);
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
    CF_ChannelSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_cmd_counter = Any_uint16();

    /* Arrange unstubbable: CF_DoEnableDisableDequeue via CF_DoChanAction */
    CF_ChannelSelect_t chan_num = Any_cf_chan_num();
    CF_Channel_t      *chan     = CF_GetChannelPtr(chan_num);

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(chan->config.dequeue_enabled == 1,
                  "CF_EnableDequeueCmd set dequeue_enabled to %d and should be 1 (barg = 1))",
                  chan->config.dequeue_enabled);
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.counters.cmd is %d and should be 1 more than %d\nACCEPTANCE OF COMMAND (+1) SHOULD BE "
                  "THE BEHAVIOR BUT IT IS NOT",
                  CF_AppData.counters.cmd,
                  initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_DEQUEUE_INF_EID);
}

void Test_CF_EnableDequeueCmd_Failure(void)
{
    /* Arrange */
    CF_EnableDequeueCmd_t utbuf;

    CF_ChannelSelect_Payload_t *data = &utbuf.Payload;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = UT_CFDP_INVALID_CHANNEL;

    CF_AppData.counters.err = 0;

    /* Act */
    CF_EnableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
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

    CF_ChannelSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_cmd_counter = Any_uint16();

    CF_ChannelSelect_t chan_num = Any_cf_chan_num();
    CF_Channel_t      *chan     = CF_GetChannelPtr(chan_num);

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DisableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_True(chan->config.dequeue_enabled == 0,
                  "CF_EnableDequeueCmd set dequeue_enabled to %d and should be 0 (barg = 0))",
                  chan->config.dequeue_enabled);
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.counters.cmd,
                  initial_hk_cmd_counter);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_DISABLE_DEQUEUE_INF_EID);
}

void Test_CF_DisableDequeueCmd_Failure(void)
{
    /* Arrange */
    CF_DisableDequeueCmd_t utbuf;

    CF_ChannelSelect_Payload_t *data = &utbuf.Payload;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = UT_CFDP_INVALID_CHANNEL;

    CF_AppData.counters.err = 0;

    /* Act */
    CF_DisableDequeueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoFreezeThaw */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
    UT_CF_AssertEventID(CF_CMD_DISABLE_DEQUEUE_ERR_EID);
}

/*******************************************************************************
**
**  CF_EnablePolldirCmd tests
**
*******************************************************************************/

void Test_CF_EnablePolldirCmd_InvalidChannel(void)
{
    /* Arrange */
    CF_Channel_t       *chan = UT_CFDP_CHANNEL_PTR;
    CF_LocalPdConfig_t *PdConfig;

    CF_EnableDirPollingCmd_t utbuf;

    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    PdConfig = &chan->config.polldir[CF_MAX_POLLING_DIR_PER_CHAN - 1];

    PdConfig->enabled   = false;
    data->ChannelSelect = UT_CFDP_INVALID_CHANNEL;
    data->PollDirIndx   = CF_ALL_POLLDIRS;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_EnableDirPollingCmd(&utbuf);

    /* Assert */
    UtAssert_BOOL_FALSE(PdConfig->enabled);
    UtAssert_UINT16_EQ(CF_AppData.counters.err, initial_hk_err_counter + 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_ENABLE_POLLDIR_ERR_EID);
}

void Test_CF_EnablePolldirCmd_AllPollDirs(void)
{
    /* Arrange */
    CF_Channel_t       *chan = UT_CFDP_CHANNEL_PTR;
    CF_LocalPdConfig_t *PdConfig;

    CF_EnableDirPollingCmd_t utbuf;

    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    PdConfig = &chan->config.polldir[CF_MAX_POLLING_DIR_PER_CHAN - 1];

    PdConfig->enabled   = false;
    data->ChannelSelect = UT_CFDP_CHANNEL;
    data->PollDirIndx   = CF_ALL_POLLDIRS;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableDirPollingCmd(&utbuf);

    /* Assert */
    UtAssert_BOOL_TRUE(PdConfig->enabled);
    UtAssert_UINT16_EQ(CF_AppData.counters.cmd, initial_hk_cmd_counter + 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_POLLDIR_INF_EID);
}

void Test_CF_EnablePolldirCmd_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ChannelSelect_t  chan_num = Any_cf_chan_num();
    uint8               polldir  = Any_cf_polldir();
    CF_Channel_t       *chan     = CF_GetChannelPtr(chan_num);
    CF_LocalPdConfig_t *PdConfig;

    CF_EnableDirPollingCmd_t utbuf;

    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    PdConfig = &chan->config.polldir[polldir - 1];

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->PollDirIndx = polldir;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(PdConfig->enabled == 1,
                  "Channel %u Polldir %u set to %u and should be 1 (context->barg)",
                  CF_ChannelSelect_AsInt(chan_num),
                  polldir,
                  PdConfig->enabled);
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.counters.cmd,
                  initial_hk_cmd_counter);
    UT_CF_AssertEventID(CF_CMD_ENABLE_POLLDIR_INF_EID);
}

void Test_CF_EnablePolldirCmd_FailWhenActionFail(void)
{
    /* Arrange */
    CF_ChannelSelect_t chan_num      = Any_cf_chan_num();
    uint8              error_polldir = 1 + CF_MAX_POLLING_DIR_PER_CHAN;

    CF_EnableDirPollingCmd_t    utbuf;
    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->PollDirIndx = error_polldir;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_EnableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.counters.err is %d and should be 1 more than %d",
                  CF_AppData.counters.err,
                  initial_hk_err_counter);
    UT_CF_AssertEventID(CF_CMD_ENABLE_POLLDIR_ERR_EID);
}

/*******************************************************************************
**
**  CF_DisablePolldirCmd tests
**
*******************************************************************************/

void Test_CF_DisablePolldirCmd_InvalidChannel(void)
{
    /* Arrange */
    CF_Channel_t       *chan = UT_CFDP_CHANNEL_PTR;
    CF_LocalPdConfig_t *PdConfig;

    CF_DisableDirPollingCmd_t utbuf;

    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    PdConfig = &chan->config.polldir[CF_MAX_POLLING_DIR_PER_CHAN - 1];

    PdConfig->enabled   = true;
    data->ChannelSelect = UT_CFDP_INVALID_CHANNEL;
    data->PollDirIndx   = CF_ALL_POLLDIRS;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_DisableDirPollingCmd(&utbuf);

    /* Assert */
    UtAssert_BOOL_TRUE(PdConfig->enabled);
    UtAssert_UINT16_EQ(CF_AppData.counters.err, initial_hk_err_counter + 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_DISABLE_POLLDIR_ERR_EID);
}

void Test_CF_DisablePolldirCmd_AllPollDirs(void)
{
    /* Arrange */
    CF_Channel_t       *chan = UT_CFDP_CHANNEL_PTR;
    CF_LocalPdConfig_t *PdConfig;

    CF_DisableDirPollingCmd_t utbuf;

    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_cmd_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));
    PdConfig = &chan->config.polldir[CF_MAX_POLLING_DIR_PER_CHAN - 1];

    PdConfig->enabled   = true;
    data->ChannelSelect = UT_CFDP_CHANNEL;
    data->PollDirIndx   = CF_ALL_POLLDIRS;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DisableDirPollingCmd(&utbuf);

    /* Assert */
    UtAssert_BOOL_FALSE(PdConfig->enabled);
    UtAssert_UINT16_EQ(CF_AppData.counters.cmd, initial_hk_cmd_counter + 1);

    UT_CF_AssertEventID(CF_CMD_DISABLE_POLLDIR_INF_EID);
}

void Test_CF_DisablePolldirCmd_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ChannelSelect_t chan_num = Any_cf_chan_num();
    uint8              polldir  = Any_cf_polldir();

    CF_DisableDirPollingCmd_t   utbuf;
    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_cmd_counter = Any_uint16();
    CF_Channel_t               *chan                   = CF_GetChannelPtr(chan_num);

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->PollDirIndx = polldir;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DisableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    UtAssert_True(chan->config.polldir[polldir].enabled == 0,
                  "Channel %u Polldir %u set to %u and should be 0 (context->barg)",
                  CF_ChannelSelect_AsInt(chan_num),
                  polldir,
                  chan->config.polldir[polldir].enabled);
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.counters.cmd == (uint16)(initial_hk_cmd_counter + 1),
                  "CF_AppData.counters.cmd is %d and should be 1 more than %d",
                  CF_AppData.counters.cmd,
                  initial_hk_cmd_counter);
    UT_CF_AssertEventID(CF_CMD_DISABLE_POLLDIR_INF_EID);
}

void Test_CF_DisablePolldirCmd_FailWhenActionFail(void)
{
    /* Arrange */
    CF_ChannelSelect_t chan_num      = Any_cf_chan_num();
    uint8              error_polldir = 1 + CF_MAX_POLLING_DIR_PER_CHAN;

    CF_DisableDirPollingCmd_t   utbuf;
    CF_PollDirSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                      initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    /* Arrange unstubbable: CF_DoEnableDisablePolldir */
    data->PollDirIndx = error_polldir;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_DisableDirPollingCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter*/
    UtAssert_True(CF_AppData.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.counters.err is %d and should be 1 more than %d",
                  CF_AppData.counters.err,
                  initial_hk_err_counter);
    UT_CF_AssertEventID(CF_CMD_DISABLE_POLLDIR_ERR_EID);
}

/*******************************************************************************
**
**  CF_PurgeQueueCmd tests
**
*******************************************************************************/

void Test_CF_PurgeQueueCmd_InvalidChannel(void)
{
    /* Arrange */
    CF_PurgeQueueCmd_t        utbuf;
    CF_QueueSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                    initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    data->ChannelSelect = UT_CFDP_INVALID_CHANNEL;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_PurgeQueueCmd(&utbuf);

    /* Assert */
    UtAssert_UINT16_EQ(CF_AppData.counters.err, initial_hk_err_counter + 1);

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_PURGE_QUEUE_ERR_EID);
}

void Test_CF_PurgeQueueCmd_FailWhenActionFail(void)
{
    /* Arrange */
    CF_ChannelSelect_t        chan_num    = Any_cf_chan_num();
    uint8                     error_purge = 0xAA; /* Shortest return from CF_DoPurgeQueue */
    CF_PurgeQueueCmd_t        utbuf;
    CF_QueueSelect_Payload_t *data                   = &utbuf.Payload;
    uint16                    initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    /* Arrange unstubbable: CF_DoPurgeQueue */
    data->QueueSelect = error_purge;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_PurgeQueueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter */
    UtAssert_True(CF_AppData.counters.err == (uint16)(initial_hk_err_counter + 1),
                  "CF_AppData.counters.err is %d and should be 1 more than %d",
                  CF_AppData.counters.err,
                  initial_hk_err_counter);
    UT_CF_AssertEventID(CF_CMD_PURGE_ARG_ERR_EID);
    UT_CF_AssertEventID(CF_CMD_PURGE_QUEUE_ERR_EID);
}

void Test_CF_PurgeQueueCmd_SuccessWhenActionSuccess(void)
{
    /* Arrange */
    CF_ChannelSelect_t        chan_num = Any_cf_chan_num();
    CF_PurgeQueueCmd_t        utbuf;
    CF_QueueSelect_Payload_t *data = &utbuf.Payload;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Arrange unstubbable: CF_DoChanAction */
    data->ChannelSelect = chan_num;

    CF_AppData.counters.cmd = 0;

    /* this causes the callback to be invoked */
    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), UT_AltHandler_CF_CList_Traverse_InvokeCb, NULL);

    /* Act */
    CF_PurgeQueueCmd(&utbuf);

    /* Assert */
    /* Assert for CF_DoEnableDisablePolldir */
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
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
    wq->chan_num = UT_CFDP_INVALID_CHANNEL;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq                     = &utbuf.Payload;
    uint16                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* invalid combination up direction, pending queue */
    wq->dir_type = CF_DirectionType_up;
    wq->queue    = CF_QueueSelect_Pending;

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_ARGS_ERR_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t             utbuf;
    CF_WriteQueue_Payload_t       *wq = &utbuf.Payload;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    uint16                         initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination up direction, NOT pending queue */
    wq->dir_type = CF_DirectionType_up;
    wq->queue    = CF_QueueSelect_Active;

    /* invalid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate),
                     &context_CF_WrappedOpenCreate,
                     sizeof(context_CF_WrappedOpenCreate),
                     false);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_INT32_EQ(context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_OPEN_ERR_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t             utbuf;
    CF_WriteQueue_Payload_t       *wq = &utbuf.Payload;
    CF_WrappedOpenCreate_context_t context_CF_WrappedOpenCreate;
    uint16                         initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination down direction, pending queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_Pending;

    /* invalid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    context_CF_WrappedOpenCreate.forced_return = Any_int_Negative();

    UT_SetDataBuffer(UT_KEY(CF_WrappedOpenCreate),
                     &context_CF_WrappedOpenCreate,
                     sizeof(context_CF_WrappedOpenCreate),
                     false);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WrappedOpenCreate, 1);
    UtAssert_INT32_EQ(context_CF_WrappedOpenCreate.access, OS_WRITE_ONLY);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_OPEN_ERR_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    uint16                               initial_hk_err_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_all;
    wq->queue    = CF_QueueSelect_All;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile),
                     &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile),
                     false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_RX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    int32                                context_CF_WrappedClose_fd;
    uint16                               initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_up;
    wq->queue    = CF_QueueSelect_Active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile),
                     &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile),
                     false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_RX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    int32                                    context_CF_WrappedClose_fd;
    uint16                                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_up;
    wq->queue    = CF_QueueSelect_History;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile),
                     &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile),
                     false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEHIST_RX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    int32                                context_CF_WrappedClose_fd;
    uint16                               initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_Active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile),
                     &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile),
                     false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_TX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WriteTxnQueueDataToFile_context_t context_CF_WriteTxnQueueDataToFile;
    int32                                forced_return_CF_WriteTxnQueueDataToFile = Any_int32_Except(0);
    int32                                context_CF_WrappedClose_fd;
    uint16                               initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_Pending;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* invalid result from CF_WriteTxnQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteTxnQueueDataToFile),
                     &context_CF_WriteTxnQueueDataToFile,
                     sizeof(context_CF_WriteTxnQueueDataToFile),
                     false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEQ_PEND_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand(
    void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    CF_WriteHistoryQueueDataToFile_context_t context_CF_WriteHistoryQueueDataToFile;
    int32                                    forced_return_CF_WriteHistoryQueueDataToFile = Any_int32_Except(0);
    int32                                    context_CF_WrappedClose_fd;
    uint16                                   initial_hk_err_counter = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_History;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* invalid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDataBuffer(UT_KEY(CF_WriteHistoryQueueDataToFile),
                     &context_CF_WriteHistoryQueueDataToFile,
                     sizeof(context_CF_WriteHistoryQueueDataToFile),
                     false);
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_WrappedClose_fd, sizeof(context_CF_WrappedClose_fd), false);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_WRITEHIST_TX_ERR_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_All(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile     = 0;
    int32  forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_all;
    wq->queue    = CF_QueueSelect_All;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 3);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_History(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_all;
    wq->queue    = CF_QueueSelect_History;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Active(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_all;
    wq->queue    = CF_QueueSelect_Active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Pend(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_all;
    wq->queue    = CF_QueueSelect_Pending;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_UpAnd_q_All(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile     = 0;
    int32  forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_up;
    wq->queue    = CF_QueueSelect_All;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_UpAnd_q_History(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteHistoryQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                       = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_up;
    wq->queue    = CF_QueueSelect_History;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteHistoryQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteHistoryQueueDataToFile), forced_return_CF_WriteHistoryQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Active(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_up;
    wq->queue    = CF_QueueSelect_Active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

/* Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Pend IS an error and is handled by a previous test */

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_All(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_All;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 2);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_History(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_History;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Active(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_Active;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Pend(void)
{
    /* Arrange */
    CF_WriteQueueCmd_t       utbuf;
    CF_WriteQueue_Payload_t *wq = &utbuf.Payload;

    int32  forced_return_CF_WriteTxnQueueDataToFile = 0;
    uint16 initial_hk_cmd_counter                   = Any_uint16();

    memset(&utbuf, 0, sizeof(utbuf));

    /* valid channel */
    wq->chan_num = Any_cf_chan_num();

    /* valid combination all direction, all queue */
    wq->dir_type = CF_DirectionType_down;
    wq->queue    = CF_QueueSelect_Pending;

    /* valid result from CF_WrappedCreat */
    strncpy(wq->filename, AnyRandomStringOfLettersOfLength(10), 10);

    /* valid result from CF_WriteTxnQueueDataToFile */
    UT_SetDefaultReturnValue(UT_KEY(CF_WriteTxnQueueDataToFile), forced_return_CF_WriteTxnQueueDataToFile);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_WriteQueueCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_WriteTxnQueueDataToFile, 1);
    UtAssert_STUB_COUNT(CF_WriteHistoryQueueDataToFile, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_WQ_INF_EID);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

/*******************************************************************************
**
**  CF_SetParamCmd tests
**
*******************************************************************************/

void Test_CF_SetParamCmd_Nominal(void)
{
    CF_SetParamCmd_t    utbuf;
    CF_GetSet_ValueID_t param_id;
    uint16              expected_count;
    CF_Engine_t        *engine_ptr = CF_GetEngine();
    CF_Channel_t       *chan       = UT_CFDP_CHANNEL_PTR;

    memset(&utbuf, 0, sizeof(utbuf));
    expected_count = 0;

    /* Nominal: "set" for each parameter.  0 is reserved, valid values start at 1. */
    for (param_id = 1; (int)param_id < CF_GetSet_ValueID_MAX; ++param_id)
    {
        UT_CF_ResetEventCapture();
        utbuf.Payload.key      = param_id;
        utbuf.Payload.value    = 1 + param_id;
        utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

        /* run the function under test */
        UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

        UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
        UT_CF_AssertEventID(CF_CMD_GETSET1_INF_EID);
        UtAssert_UINT32_EQ(CF_AppData.counters.cmd, ++expected_count);
    }

    /* each of the config parameters should have actually been set to a different value */
    UtAssert_UINT32_EQ(engine_ptr->config.ticks_per_second, 1 + CF_GetSet_ValueID_ticks_per_second);
    UtAssert_UINT32_EQ(engine_ptr->config.rx_crc_calc_bytes_per_wakeup,
                       1 + CF_GetSet_ValueID_rx_crc_calc_bytes_per_wakeup);
    UtAssert_UINT32_EQ(chan->config.ack_timer_s, 1 + CF_GetSet_ValueID_ack_timer_s);
    UtAssert_UINT32_EQ(chan->config.nak_timer_s, 1 + CF_GetSet_ValueID_nak_timer_s);
    UtAssert_UINT32_EQ(chan->config.inactivity_timer_s, 1 + CF_GetSet_ValueID_inactivity_timer_s);
    UtAssert_UINT32_EQ(engine_ptr->config.outgoing_file_chunk_size, 1 + CF_GetSet_ValueID_outgoing_file_chunk_size);
    UtAssert_UINT32_EQ(chan->config.ack_limit, 1 + CF_GetSet_ValueID_ack_limit);
    UtAssert_UINT32_EQ(chan->config.nak_limit, 1 + CF_GetSet_ValueID_nak_limit);
    UtAssert_UINT32_EQ(engine_ptr->config.local_eid, 1 + CF_GetSet_ValueID_local_eid);
    UtAssert_UINT32_EQ(chan->config.max_outgoing_messages_per_wakeup,
                       1 + CF_GetSet_ValueID_chan_max_outgoing_messages_per_wakeup);
}

void Test_CF_SetParamCmd_BadParam(void)
{
    CF_SetParamCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Bad param ID (0)*/
    utbuf.Payload.key      = 0;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UT_CF_AssertEventID(CF_CMD_GETSET_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);

    /* Bad param ID (high)*/
    UT_CF_ResetEventCapture();
    utbuf.Payload.key      = CF_GetSet_ValueID_MAX;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_GETSET_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 2);

    /* Bad channel ID */
    UT_CF_ResetEventCapture();
    utbuf.Payload.key      = 1;
    utbuf.Payload.chan_num = UT_CFDP_INVALID_CHANNEL;

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 3);
}

void Test_CF_SetParamCmd_ValidateChunkSize(void)
{
    CF_SetParamCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Validation fail, chunk size 0 */
    utbuf.Payload.key      = CF_GetSet_ValueID_outgoing_file_chunk_size;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;
    utbuf.Payload.value    = 0;

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_GETSET_VALIDATE_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 0);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);

    /* Validation fail, chunk size too high */
    UT_CF_ResetEventCapture();
    utbuf.Payload.key      = CF_GetSet_ValueID_outgoing_file_chunk_size;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;
    utbuf.Payload.value    = 100 + sizeof(CF_CFDP_PduFileDataContent_t);

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_GETSET_VALIDATE_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 0);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 2);

    /* Validation success, chunk size max */
    UT_CF_ResetEventCapture();
    utbuf.Payload.key      = CF_GetSet_ValueID_outgoing_file_chunk_size;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;
    utbuf.Payload.value    = sizeof(CF_CFDP_PduFileDataContent_t);

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_GETSET1_INF_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 2);
}

void Test_CF_SetParamCmd_ValidateMaxMsgs(void)
{
    /* this test case specifically targets the additional validation for chan_max_outgoing_messages_per_wakeup */
    CF_SetParamCmd_t utbuf;
    CF_Channel_t    *chan = UT_CFDP_CHANNEL_PTR;

    memset(&utbuf, 0, sizeof(utbuf));

    utbuf.Payload.key      = CF_GetSet_ValueID_chan_max_outgoing_messages_per_wakeup;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    /* Validation fail: unlimited msgs with no sem */
    chan->config.sem_name[0] = 0;

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UT_CF_AssertEventID(CF_CMD_GETSET_VALIDATE_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 0);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);

    /* Validation success: unlimited msgs with sem */
    UT_CF_ResetEventCapture();
    chan->config.sem_name[0] = 'a';

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UT_CF_AssertEventID(CF_CMD_GETSET1_INF_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 1);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);

    /* Validation success: limited msgs with sem */
    UT_CF_ResetEventCapture();
    chan->config.sem_name[0] = 'a';
    utbuf.Payload.value      = 10;

    UtAssert_VOIDCALL(CF_SetParamCmd(&utbuf));

    UT_CF_AssertEventID(CF_CMD_GETSET1_INF_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, 2);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);
}

/*******************************************************************************
**
**  CF_GetParamCmd tests
**
*******************************************************************************/

void Test_CF_GetParamCmd_Nominal(void)
{
    CF_GetParamCmd_t    utbuf;
    CF_GetSet_ValueID_t param_id;
    uint16              expected_count;

    memset(&utbuf, 0, sizeof(utbuf));
    expected_count = 0;

    /* Nominal: "get" for each parameter */
    for (param_id = 1; (int)param_id < CF_GetSet_ValueID_MAX; ++param_id)
    {
        UT_CF_ResetEventCapture();

        utbuf.Payload.key      = param_id;
        utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

        UtAssert_VOIDCALL(CF_GetParamCmd(&utbuf));

        UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
        UT_CF_AssertEventID(CF_CMD_GETSET2_INF_EID);
        UtAssert_UINT32_EQ(CF_AppData.counters.cmd, ++expected_count);
    }
}

void Test_CF_GetParamCmd_BadParam(void)
{
    CF_GetParamCmd_t utbuf;

    memset(&utbuf, 0, sizeof(utbuf));

    /* Bad param ID (0)*/
    utbuf.Payload.key      = 0;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    UtAssert_VOIDCALL(CF_GetParamCmd(&utbuf));

    UT_CF_AssertEventID(CF_CMD_GETSET_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 1);

    /* Bad param ID (high)*/
    UT_CF_ResetEventCapture();
    utbuf.Payload.key      = CF_GetSet_ValueID_MAX;
    utbuf.Payload.chan_num = UT_CFDP_CHANNEL;

    UtAssert_VOIDCALL(CF_GetParamCmd(&utbuf));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_GETSET_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 2);

    /* Bad channel ID */
    UT_CF_ResetEventCapture();
    utbuf.Payload.key      = 1;
    utbuf.Payload.chan_num = UT_CFDP_INVALID_CHANNEL;

    UtAssert_VOIDCALL(CF_GetParamCmd(&utbuf));

    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_CHAN_PARAM_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.counters.err, 3);
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
    CF_Engine_t         *engine_ptr                       = CF_GetEngine();

    memset(&utbuf, 0, sizeof(utbuf));

    engine_ptr->enabled = false;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableEngineCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_ENGINE_INF_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_EnableEngineCmd_WithEngineNotEnableFailsInitSendEventAndIncrementErrCounter(void)
{
    /* Arrange */
    CF_EnableEngineCmd_t utbuf;
    uint16               initial_hk_err_counter = Any_uint16();
    CF_Engine_t         *engine_ptr             = CF_GetEngine();

    memset(&utbuf, 0, sizeof(utbuf));

    engine_ptr->enabled = false;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    CF_AppData.counters.err = initial_hk_err_counter;

    /* Act */
    CF_EnableEngineCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENABLE_ENGINE_ERR_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.err, (initial_hk_err_counter + 1) & 0xFFFF);
}

void Test_CF_EnableEngineCmd_WithEngineEnableFailsSendEventAndIncrementCmdCounter(void)
{
    /* Arrange */
    CF_EnableEngineCmd_t utbuf;
    uint16               initial_hk_cmd_counter = Any_uint16();
    CF_Engine_t         *engine_ptr             = CF_GetEngine();

    memset(&utbuf, 0, sizeof(utbuf));

    engine_ptr->enabled = true;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_EnableEngineCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENG_ALREADY_ENA_INF_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
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
    CF_Engine_t          *engine_ptr             = CF_GetEngine();

    memset(&utbuf, 0, sizeof(utbuf));

    engine_ptr->enabled = true;

    CF_AppData.counters.cmd = initial_hk_cmd_counter;

    /* Act */
    CF_DisableEngineCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_DISABLE_ENGINE_INF_EID);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_cmd_counter + 1) & 0xFFFF);
}

void Test_CF_DisableEngineCmd_WhenEngineDisabledAndIncrementCmdCounter(void)
{
    /* Arrange */
    CF_DisableEngineCmd_t utbuf;
    uint16                initial_hk_counter = Any_uint16();
    CF_Engine_t          *engine_ptr         = CF_GetEngine();

    memset(&utbuf, 0, sizeof(utbuf));

    engine_ptr->enabled = false;

    CF_AppData.counters.cmd = initial_hk_counter;

    /* Act */
    CF_DisableEngineCmd(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_DisableEngine, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_ENG_ALREADY_DIS_INF_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.counters.cmd, (initial_hk_counter + 1) & 0xFFFF);
}

/*******************************************************************************
**
**  CF_SendHkCmd tests - full coverage
**
*******************************************************************************/

void Test_CF_SendHkCmd_Nominal(void)
{
    union
    {
        CFE_SB_Buffer_t sb;
        CF_HkPacket_t   hk;
    } buf;
    CFE_SB_Buffer_t *sb_buf = &buf.sb;

    memset(&buf, 0xFF, sizeof(buf));
    CF_AppData.counters.cmd = 10;
    CF_AppData.counters.err = 20;
    UT_SetDataBuffer(UT_KEY(CFE_SB_AllocateMessageBuffer), &sb_buf, sizeof(sb_buf), false);

    /* Act */
    CF_SendHkCmd(NULL);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_SB_TransmitBuffer, 1);
    UtAssert_UINT8_EQ(buf.hk.Payload.counters.cmd, CF_AppData.counters.cmd);
    UtAssert_UINT8_EQ(buf.hk.Payload.counters.err, CF_AppData.counters.err);
}

void Test_CF_SendHkCmd_NoBuffers(void)
{
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_AllocateMessageBuffer), -1);

    /* Act */
    CF_SendHkCmd(NULL);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 0);
    UtAssert_STUB_COUNT(CFE_SB_TransmitBuffer, 0);
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
    UtTest_Add(Test_CF_NoopCmd_SendNoopEventAndAcceptCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_NoopCmd_SendNoopEventAndAcceptCommand");
}

void add_CF_ResetCountersCmd_tests(void)
{
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIsEqTo_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIsGreaterThan_5_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_command_AndResetHkCmdAndErrCountSendEvent");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_fault_ResetAllHkFaultCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_up_AndResetAllHkRecvCountSendEventAndAcceptCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResetCountersCmd_tests_SWhenCommandByteIs_down_AndResetAllHkSentCountendEventAcceptCommand");
    UtTest_Add(Test_CF_ResetCountersCmd_tests_WhenCommandByteIs_all_AndResetAllMemValuesSendEvent,
               cf_cmd_tests_Setup,
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

void add_CF_FreezeCmd_tests(void)
{
    UtTest_Add(Test_CF_FreezeCmd_Set_frozen_To_1_AndAcceptCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_FreezeCmd_Set_frozen_To_1_AndAcceptCommand");
    UtTest_Add(Test_CF_FreezeCmd_Set_frozen_To_1_AndRejectCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_FreezeCmd_Set_frozen_To_1_AndRejectCommand");
}

void add_CF_ThawCmd_tests(void)
{
    UtTest_Add(Test_CF_ThawCmd_Set_frozen_To_0_AndAcceptCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ThawCmd_Set_frozen_To_0_AndAcceptCommand");
    UtTest_Add(Test_CF_ThawCmd_Set_frozen_To_0_AndRejectCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ThawCmd_Set_frozen_To_0_AndRejectCommand");
}

void add_CF_SuspendCmd_tests(void)
{
    UtTest_Add(Test_CF_SuspendCmd_InvalidChannel,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_SuspendCmd_InvalidChannel");
    UtTest_Add(Test_CF_SuspendCmd_Success, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_SuspendCmd_Success");
}

void add_CF_ResumeCmd_tests(void)
{
    UtTest_Add(Test_CF_ResumeCmd_InvalidChannel,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResumeCmd_InvalidChannel");
    UtTest_Add(Test_CF_ResumeCmd_SuccessSame,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_ResumeCmd_SuccessSame");
}

void add_CF_CancelCmd_tests(void)
{
    UtTest_Add(Test_CF_CancelCmd_Failure, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_CancelCmd_Failure");
    UtTest_Add(Test_CF_CancelCmd_Success, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_CancelCmd_Success");
    UtTest_Add(Test_CF_CancelCmd_SuccessSingleTxn,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CancelCmd_SuccessSingleTxn");
    UtTest_Add(Test_CF_CancelCmd_FailSingleTxn,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CancelCmd_FailSingleTxn");
}

void add_CF_AbandonCmd_tests(void)
{
    UtTest_Add(Test_CF_AbandonCmd_Failure, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_AbandonCmd_Failure");
    UtTest_Add(Test_CF_AbandonCmd_Success, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_AbandonCmd_Success");
    UtTest_Add(Test_CF_AbandonCmd_SuccessSingleTxn,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_AbandonCmd_SuccessSingleTxn");
    UtTest_Add(Test_CF_AbandonCmd_FailSingleTxn,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_AbandonCmd_FailSingleTxn");
}

void add_CF_EnableDequeueCmd_tests(void)
{
    UtTest_Add(Test_CF_EnableDequeueCmd_Success,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnableDequeueCmd_Success");
    UtTest_Add(Test_CF_EnableDequeueCmd_Failure,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnableDequeueCmd_Failure");
}

void add_CF_DisableDequeueCmd_tests(void)
{
    UtTest_Add(Test_CF_DisableDequeueCmd_Success,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DisableDequeueCmd_Success");
    UtTest_Add(Test_CF_DisableDequeueCmd_Failure,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DisableDequeueCmd_Failure");
}

void add_CF_EnablePolldirCmd_tests(void)
{
    UtTest_Add(Test_CF_EnablePolldirCmd_SuccessWhenActionSuccess,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnablePolldirCmd_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_EnablePolldirCmd_FailWhenActionFail,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnablePolldirCmd_FailWhenActionFail");
    UtTest_Add(Test_CF_EnablePolldirCmd_AllPollDirs,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnablePolldirCmd_AllPollDirs");
    UtTest_Add(Test_CF_EnablePolldirCmd_InvalidChannel,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnablePolldirCmd_InvalidChannel");
}

void add_CF_DisablePolldirCmd_tests(void)
{
    UtTest_Add(Test_CF_DisablePolldirCmd_SuccessWhenActionSuccess,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DisablePolldirCmd_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_DisablePolldirCmd_FailWhenActionFail,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DisablePolldirCmd_FailWhenActionFail");
    UtTest_Add(Test_CF_DisablePolldirCmd_AllPollDirs,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DisablePolldirCmd_AllPollDirs");
    UtTest_Add(Test_CF_DisablePolldirCmd_InvalidChannel,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DisablePolldirCmd_InvalidChannel");
}

void add_CF_PurgeQueueCmd_tests(void)
{
    UtTest_Add(Test_CF_PurgeQueueCmd_FailWhenActionFail,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_PurgeQueueCmd_FailWhenActionFail");
    UtTest_Add(Test_CF_PurgeQueueCmd_SuccessWhenActionSuccess,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_PurgeQueueCmd_SuccessWhenActionSuccess");
    UtTest_Add(Test_CF_PurgeQueueCmd_InvalidChannel,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_PurgeQueueCmd_InvalidChannel");
}

void add_CF_WriteQueueCmd_tests(void)
{
    UtTest_Add(Test_CF_WriteQueueCmd_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_When_chan_Eq_CF_NUM_CAHNNELS_SendEventAndRejectCommand");
    UtTest_Add(Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_Is_type_up_And_queue_IsNot_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_SendEventAndRejectCommand,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WrappedCreat_Fails_type_IsNot_type_up_And_queue_Is_q_pend_"
        "SendEventAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_wq_IsAllAnd_queue_IsAll_fd_Is_0_Call_CF_WrappedClose_"
        "SendEventCloseAndRejectCommandWhen_CF_WriteTxnQueueDataToFile_Fails");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventClosesAndRejectCommand,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteTxnQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsActive_fd_IsPositive_Call_"
        "CF_"
        "WrappedClose_SendEventClosesAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsUpAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryDataToFile_FailsOnFirstCallAnd_wq_IsDownAnd_queue_IsActive_fd_"
        "IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsPend_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(
        Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_Call_CF_WrappedClose_SendEventCloseAndRejectCommand,
        cf_cmd_tests_Setup,
        cf_cmd_tests_Teardown,
        "Test_CF_WriteQueueCmd_When_CF_WriteHistoryQueueDataToFile_FailsAnd_wq_IsDownAnd_queue_IsHistory_fd_IsPositive_"
        "Call_CF_WrappedClose_SendEventCloseAndRejectCommand");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_All,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_All");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_History,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_History");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Active,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Active");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Pend,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_AllAnd_q_Pend");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_UpAnd_q_All,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_UpAnd_q_All");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_UpAnd_q_History,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_UpAnd_q_History");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Active,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_UpAnd_q_Active");
    /* see Test_CF_WriteQueueCmd_WhenUpAndPendingQueueSendEventAndRejectCommand */
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_All,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_All");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_History,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_History");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Active,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Active");
    UtTest_Add(Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Pend,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_WriteQueueCmd_Success_type_DownAnd_q_Pend");
}

void add_CF_SetParamCmd_tests(void)
{
    UtTest_Add(Test_CF_SetParamCmd_Nominal, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_SetParamCmd_Nominal");
    UtTest_Add(Test_CF_SetParamCmd_BadParam, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_SetParamCmd_BadParam");
    UtTest_Add(Test_CF_SetParamCmd_ValidateChunkSize,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_SetParamCmd_ValidateChunkSize");
    UtTest_Add(Test_CF_SetParamCmd_ValidateMaxMsgs,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_SetParamCmd_ValidateMaxMsgs");
}

void add_CF_GetParamCmd_tests(void)
{
    UtTest_Add(Test_CF_GetParamCmd_Nominal, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_GetParamCmd_Nominal");

    UtTest_Add(Test_CF_GetParamCmd_BadParam, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_GetParamCmd_BadParam");
}

void add_CF_EnableEngineCmd_tests(void)
{
    UtTest_Add(Test_CF_EnableEngineCmd_WithEngineNotEnableInitSuccessAndIncrementCmdCounter,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnableEngineCmd_WithEngineNotEnableInitSuccessAndIncrementCmdCounter");
    UtTest_Add(Test_CF_EnableEngineCmd_WithEngineNotEnableFailsInitSendEventAndIncrementErrCounter,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnableEngineCmd_WithEngineNotEnableFailsInitSendEventAndIncrementErrCounter");
    UtTest_Add(Test_CF_EnableEngineCmd_WithEngineEnableFailsSendEventAndIncrementCmdCounter,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_EnableEngineCmd_WithEngineEnableFailsSendEventAndIncrementCmdCounter");
}

void add_CF_DisableEngineCmd_tests(void)
{
    UtTest_Add(Test_CF_DisableEngineCmd_SuccessWhenEngineEnabledAndIncrementCmdCounter,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_CmdDisableEngine_SuccessWhenEngineEnabledAndIncrementCmdCounter");
    UtTest_Add(Test_CF_DisableEngineCmd_WhenEngineDisabledAndIncrementCmdCounter,
               cf_cmd_tests_Setup,
               cf_cmd_tests_Teardown,
               "Test_CF_DisableEngineCmd_WhenEngineDisabledAndIncrementCmdCounter");
}

void add_CF_SendHkCmd_tests(void)
{
    UtTest_Add(Test_CF_SendHkCmd_Nominal, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_SendHkCmd_Nominal");
    UtTest_Add(Test_CF_SendHkCmd_NoBuffers, cf_cmd_tests_Setup, cf_cmd_tests_Teardown, "Test_CF_SendHkCmd_NoBuffers");
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

    add_CF_FreezeCmd_tests();

    add_CF_ThawCmd_tests();

    add_CF_SuspendCmd_tests();

    add_CF_ResumeCmd_tests();

    add_CF_CancelCmd_tests();

    add_CF_AbandonCmd_tests();

    add_CF_EnableDequeueCmd_tests();

    add_CF_DisableDequeueCmd_tests();

    add_CF_EnablePolldirCmd_tests();

    add_CF_DisablePolldirCmd_tests();

    add_CF_PurgeQueueCmd_tests();

    add_CF_WriteQueueCmd_tests();

    add_CF_SetParamCmd_tests();

    add_CF_GetParamCmd_tests();

    add_CF_EnableEngineCmd_tests();

    add_CF_DisableEngineCmd_tests();

    add_CF_SendHkCmd_tests();

    add_CF_WakeupCmd_tests();
}
