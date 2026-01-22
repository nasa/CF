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
#include "cf_cfdp.h"
#include "cf_app.h"
#include "cf_eventids.h"

#include "cf_cfdp_r.h"
#include "cf_cfdp_s.h"

static void UT_CFDP_S_SetupBasicRxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    static CF_DecoderState_t ut_decoder;
    static uint8             bytes[CF_CFDP_MAX_HEADER_SIZE];

    memset(bytes, 0, sizeof(bytes));

    ut_decoder.base                    = bytes;
    ut_decoder.codec_state.is_valid    = true;
    ut_decoder.codec_state.max_size    = sizeof(bytes);
    ut_decoder.codec_state.next_offset = 0;

    pdu_buffer->pdec = &ut_decoder;
}

static void UT_CFDP_S_SetupBasicTxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    /* to support generating file data PDUs, not just headers, this must have a bigger buffer */
    static CF_EncoderState_t ut_encoder;
    static uint8             bytes[CF_MAX_PDU_SIZE];

    memset(bytes, 0, sizeof(bytes));

    ut_encoder.base                    = bytes;
    ut_encoder.codec_state.is_valid    = true;
    ut_encoder.codec_state.max_size    = sizeof(bytes);
    ut_encoder.codec_state.next_offset = 0;

    pdu_buffer->penc = &ut_encoder;

    /* Make it so a call to CF_CFDP_ConstructPduBuffer returns the same PDU buffer */
    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), UT_AltHandler_GenericPointerReturn, pdu_buffer);
}

static void UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_t setup, CF_Logical_PduBuffer_t **pdu_buffer_p,
                                          CF_Channel_t **channel_p, CF_History_t **history_p, CF_Transaction_t **txn_p,
                                          CF_ConfigTable_t **config_table_p)
{
    /*
     * fake objects used to pass into CF app during unit tests.
     * These are declared static so they can be returned
     */
    static CF_Logical_PduBuffer_t ut_pdu_buffer;
    static CF_History_t           ut_history;
    static CF_Transaction_t       ut_transaction;
    static CF_ConfigTable_t       ut_config_table;

    /*
     * always clear all objects, regardless of what was asked for.
     * this helps ensure that a test does not depend on preexisting data
     * in the buffer (each test should set up its buffers in full)
     */
    memset(&ut_pdu_buffer, 0, sizeof(ut_pdu_buffer));
    memset(&ut_history, 0, sizeof(ut_history));
    memset(&ut_transaction, 0, sizeof(ut_transaction));
    memset(&ut_config_table, 0, sizeof(ut_config_table));

    /* certain pointers should be connected even if they were not asked for,
     * as internal code may assume these are set (test cases may un-set) */
    ut_transaction.history  = &ut_history;
    ut_history.txn_stat     = CF_TxnStatus_UNDEFINED;
    CF_AppData.config_table = &ut_config_table;

    if (pdu_buffer_p)
    {
        if (setup == UT_CF_Setup_TX || setup == UT_CF_Setup_RX)
        {
            *pdu_buffer_p = &ut_pdu_buffer;
        }
        else
        {
            *pdu_buffer_p = NULL;
        }
    }
    if (channel_p)
    {
        /*
         * note that for channels, many CF app functions assume
         * that when channel is passed as a pointer, that it is a member
         * of the array within CF_AppData, and the channel number can
         * be obtained by pointer arithmetic.
         * this arithmetic will break if the pointer is not actually
         * a member of that array, so for now it must be so.
         * This always uses the same channel for now.
         */
        *channel_p = &CF_AppData.engine.channels[UT_CFDP_CHANNEL];
    }
    if (history_p)
    {
        *history_p = &ut_history;
    }
    if (txn_p)
    {
        *txn_p = &ut_transaction;
    }
    if (config_table_p)
    {
        *config_table_p = &ut_config_table;
    }

    if (setup == UT_CF_Setup_TX)
    {
        UT_CFDP_S_SetupBasicTxState(&ut_pdu_buffer);
    }
    else if (setup == UT_CF_Setup_RX)
    {
        UT_CFDP_S_SetupBasicRxState(&ut_pdu_buffer);
    }

    /* reset the event ID capture between each sub-case */
    UT_CF_ResetEventCapture();

    /* Capture calls to CF_CFDP_SetTxnState() to capture transaction status */
    UT_SetHandlerFunction(UT_KEY(CF_CFDP_SetTxnStatus), UT_AltHandler_CaptureTransactionStatus, &ut_history.txn_stat);
}

/*******************************************************************************
**
**  cf_cfdp_s_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_s_tests_Setup(void)
{
    cf_tests_Setup();

    /* make sure global data is wiped between tests */
    memset(&CF_AppData, 0, sizeof(CF_AppData));
}

void cf_cfdp_s_tests_Teardown(void)
{
    cf_tests_Teardown();
}

/*******************************************************************************
**
**  Unit-specific test implementation
**
*******************************************************************************/

void Test_CF_CFDP_S1_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_S1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* class 1 recv is really a noop, it basically drops all packets.
       nothing to verify, just call for coverage */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S1_Recv(txn, ph));
}

void Test_CF_CFDP_S2_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* class 2 recv just invokes a dispatcher to functions that should be
       tested separately.  nothing to verify here. */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Recv(txn, ph));
}

void Test_CF_CFDP_S_AckTimerTick(void)
{
    /* Test case for:
     * void CF_CFDP_S_AckTimerTick(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* no-op if not in R2 */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode             = false;
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_S_AckTimerTick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);

    /* no-op if not armed */
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = false;
    UtAssert_VOIDCALL(CF_CFDP_S_AckTimerTick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);

    /* in CF_TxnState_S2, ack_timer_armed */
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_S_AckTimerTick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* in CF_TxnState_S2, ack_timer_armed + expiry */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_S_AckTimerTick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
}

void Test_CF_CFDP_S_Tick(void)
{
    /* Test case for:
     * void CF_CFDP_S_Tick(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal, not in CF_TxSubState_DATA_NORMAL */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.inactivity_fired = false;
    txn->reliable_mode              = false;
    txn->state_data.sub_state       = CF_TxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_STUB_COUNT(CF_CFDP_CompleteTick, 1);

    /* nominal, with timer expiry */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_ACTIVE);
    txn->flags.com.inactivity_fired = false;
    txn->reliable_mode              = false;
    txn->state_data.sub_state       = CF_TxSubState_DATA_EOF;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.inactivity_fired);
    UtAssert_STUB_COUNT(CF_CFDP_RecycleTransaction, 0);
    UtAssert_STUB_COUNT(CF_CFDP_CompleteTick, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer, 1);

    /* nominal, in CF_TxnState_HOLD, with timer expiry */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_TERMINATED);
    txn->flags.com.inactivity_fired = false;
    txn->state                      = CF_TxnState_HOLD;
    txn->state_data.sub_state       = CF_TxSubState_COMPLETE;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.inactivity_fired);
    UtAssert_STUB_COUNT(CF_CFDP_RecycleTransaction, 1);

    /* nominal, in CF_TxnState_S2, with timer expiry */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_ACTIVE);
    txn->flags.com.inactivity_fired = false;
    txn->reliable_mode              = true;
    txn->state_data.sub_state       = CF_TxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UT_CF_AssertEventID(CF_CFDP_S_INACT_TIMER_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer, 2);
    UtAssert_BOOL_TRUE(txn->flags.com.inactivity_fired);
    UtAssert_STUB_COUNT(CF_CFDP_RecycleTransaction, 0);

    /* nominal, active transaction */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_STUB_COUNT(CF_CFDP_CompleteTick, 1);

    /* inactive transaction */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->state                      = CF_TxnState_HOLD;
    txn->flags.com.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn));
    UtAssert_STUB_COUNT(CF_CFDP_RecycleTransaction, 1);
}

void Test_CF_CFDP_S_Tick_Maintenance(void)
{
    /* Test case for:
     * void CF_CFDP_S_Tick_Maintenance(CF_Transaction_t *txn, int *cont);
     */
    CF_Transaction_t *txn;

    /* nominal, nothing pending */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));

    /* If send_md is pending but failed to send */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode    = true;
    txn->flags.tx.send_md = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_BOOL_TRUE(txn->flags.tx.send_md); /* remains pending */

    /* second time it does send, clears the flag */
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_BOOL_FALSE(txn->flags.tx.send_md);

    /* If send_eof is pending but failed to send */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode     = false;
    txn->flags.tx.send_eof = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_BOOL_TRUE(txn->flags.tx.send_eof); /* remains pending */

    /* second time it does send, clears the flag, S1 does not arm timer */
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_BOOL_FALSE(txn->flags.tx.send_eof);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);

    /* In S2 it does arm the ack timer */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.send_eof = true;
    txn->reliable_mode     = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_BOOL_FALSE(txn->flags.tx.send_eof);
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);

    /* If fin_count is pending but failed to send */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode          = true;
    txn->flags.tx.fin_count     = 1;
    txn->flags.tx.fin_ack_count = 0;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendAck), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_ZERO(txn->flags.tx.fin_ack_count); /* remains pending */

    /* second time it does send, clears the flag */
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_UINT8_EQ(txn->flags.tx.fin_ack_count, txn->flags.tx.fin_count);

    /* If fin_count is pending in S1 (ignored) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode          = false;
    txn->flags.tx.fin_count     = 1;
    txn->flags.tx.fin_ack_count = 0;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Maintenance(txn));
    UtAssert_ZERO(txn->flags.tx.fin_ack_count); /* remains pending */
}

void Test_CF_CFDP_S_SendFileData(void)
{
    /* Test case for:
     * int32 CF_CFDP_S_SendFileData(CF_Transaction_t *txn, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc);
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;
    uint32            cumulative_read;
    uint32            read_size;
    uint32            offset;

    cumulative_read = 0;
    offset          = 0;
    read_size       = 100;

    /* failure of CF_CFDP_ConstructPduHeader */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size, true), CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* nominal, smaller than chunk, no CRC */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = 150;
    txn->fsize                       = 300;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size, false), read_size);
    cumulative_read += read_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* nominal, larger than PDU, no CRC */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE * 2;
    txn->fsize                       = CF_MAX_PDU_SIZE * 2;
    read_size                        = CF_MAX_PDU_SIZE;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size * 2, false), read_size);
    cumulative_read += read_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 0);

    /* nominal, larger than chunk, with CRC */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = 50;
    read_size                        = 100;
    txn->fsize                       = 300;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, config->outgoing_file_chunk_size);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size, true), config->outgoing_file_chunk_size);
    cumulative_read += config->outgoing_file_chunk_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 1);

    /* read w/failure */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    config->outgoing_file_chunk_size = read_size;
    txn->fsize                       = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size, true), -1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_read, 1);
    UT_CF_AssertEventID(CF_CFDP_S_READ_ERR_EID);

    /* require lseek */
    offset = 25;
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, offset);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    config->outgoing_file_chunk_size = read_size;
    txn->fsize                       = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size, true), read_size);
    cumulative_read += read_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* lseek w/failure */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, -1);
    config->outgoing_file_chunk_size = read_size;
    txn->fsize                       = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size, true), -1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek, 1);
    UT_CF_AssertEventID(CF_CFDP_S_SEEK_FD_ERR_EID);
}

void Test_CF_CFDP_S_SubstateSendFileData(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;

    /* nominal, zero bytes processed */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));

    /* nominal, whole file at once */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE;
    txn->state_data.sub_state        = CF_TxSubState_DATA_NORMAL;
    txn->fsize                       = CF_MAX_PDU_SIZE / 2;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, txn->fsize);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));
    UtAssert_EQ(CF_FileSize_t, txn->foffs, txn->fsize);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_UNDEFINED);

    /* nominal, less than whole file at once */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE / 2;
    txn->state_data.sub_state        = CF_TxSubState_DATA_NORMAL;
    txn->fsize                       = CF_MAX_PDU_SIZE;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, config->outgoing_file_chunk_size);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));
    UtAssert_UINT32_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_NORMAL);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_UNDEFINED);

    /* error during read */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    txn->foffs                = 0;
    txn->fsize                = CF_MAX_PDU_SIZE;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_READ_FAILURE);

    /* No buffers available */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    txn->foffs                = 0;
    txn->fsize                = CF_MAX_PDU_SIZE;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));
    UtAssert_ZERO(txn->foffs);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_UNDEFINED);
}

void Test_CF_CFDP_S_SubstateEarlyFin(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateEarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateEarlyFin(txn, ph));
}

void Test_CF_CFDP_S_SubstateRecvFin(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateRecvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_CheckAckNakCount), true);

    /* nominal, first FIN recv */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.tx.fin_count = 0;
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateRecvFin(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_CheckAckNakCount, 1);

    /* call again, should reject as dupe but still ack */
    txn->flags.tx.fin_count = 1;
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateRecvFin(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_CheckAckNakCount, 2);

    /* call again, at ack/nak limit */
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_NO_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_CheckAckNakCount), false);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateRecvFin(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_CheckAckNakCount, 3);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_POS_ACK_LIMIT_REACHED);

    /* fail to decode */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvFin), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateRecvFin(txn, ph));
}

void Test_CF_CFDP_S2_SubstateNak(void)
{
    /* Test case for:
     * void CF_CFDP_S2_SubstateNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduNak_t *   nak;

    /* no segments */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvNak), 1, CF_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateNak(txn, ph));
    UT_CF_AssertEventID(CF_CFDP_S_PDU_NAK_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 1);

    /* nominal, re-send md request (0,0) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 1;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {0, 0};
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateNak(txn, ph));
    UtAssert_BOOL_TRUE(txn->flags.tx.send_md);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests, 1);

    /* nominal, nonzero offsets */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 2;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {0, 200};
    nak->segment_list.segments[1]  = (CF_Logical_SegmentRequest_t) {200, 300};
    txn->fsize                     = 300;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateNak(txn, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests, 3);

    /* bad segments */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 3;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {200, 100};
    nak->segment_list.segments[1]  = (CF_Logical_SegmentRequest_t) {100, 400};
    nak->segment_list.segments[2]  = (CF_Logical_SegmentRequest_t) {400, 0};
    txn->fsize                     = 300;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateNak(txn, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests, 6);
    UT_CF_AssertEventID(CF_CFDP_S_INVALID_SR_ERR_EID);

    /* bad decode */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvNak), 1, -1);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 1;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateNak(txn, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 2);
    UT_CF_AssertEventID(CF_CFDP_S_PDU_NAK_ERR_EID);
}

void Test_CF_CFDP_S2_SubstateEofAck(void)
{
    /* Test case for:
     * void CF_CFDP_S2_SubstateEofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.tx.eof_ack_recv            = false;
    ph->int_header.ack.ack_directive_code = CF_CFDP_FileDirective_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateEofAck(txn, ph));
    UtAssert_BOOL_TRUE(txn->flags.tx.eof_ack_recv);
    UtAssert_BOOL_FALSE(txn->flags.com.ack_timer_armed);

    /* failure of CF_CFDP_RecvAck */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    ph->int_header.ack.ack_directive_code = CF_CFDP_FileDirective_EOF;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvAck), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateEofAck(txn, ph));
    UT_CF_AssertEventID(CF_CFDP_S_PDU_EOF_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 1);

    /* Ack not for EOF */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    ph->int_header.ack.ack_directive_code = CF_CFDP_FileDirective_FIN;
    txn->flags.tx.eof_ack_recv            = false;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateEofAck(txn, ph));
    UT_CF_AssertEventID(CF_CFDP_S_PDU_EOF_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 2);
}

/* Hook function to set FD output on opencreate call */
static void UT_AltHandler_CF_WrappedOpenCreate(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    osal_id_t *fd = UT_Hook_GetArgValueByName(Context, "fd", osal_id_t *);
    int32      status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status == 0)
    {
        *fd = OS_ObjectIdFromInteger(1);
    }
}

void Test_CF_CFDP_S_Init(void)
{
    /* Test Case For:
     * void CF_CFDP_S_Init(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), OS_ERROR); /* this is the "good" code */
    UT_SetHandlerFunction(UT_KEY(CF_WrappedOpenCreate), UT_AltHandler_CF_WrappedOpenCreate,
                          NULL); /* set FD on output */

    /* Nominal case, everything success */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    UtAssert_VOIDCALL(CF_CFDP_S_Init(txn));
    UtAssert_STUB_COUNT(CF_CRC_Start, 1);
    UtAssert_BOOL_TRUE(txn->flags.tx.send_md);

    /* From here on is error checks */
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), false);

    /* Error condition set before call */
    /* This is really a no-op */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Init(txn));

    /* First status check passes, open check fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, true);
    UT_SetDeferredRetcode(UT_KEY(OS_FileOpenCheck), 1, OS_SUCCESS); /* this is the "bad" code */
    UtAssert_VOIDCALL(CF_CFDP_S_Init(txn));
    UT_CF_AssertEventID(CF_CFDP_S_ALREADY_OPEN_ERR_EID);

    /* Second status check passes, open succeeds - confirms close of FD */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 2, true);
    UtAssert_VOIDCALL(CF_CFDP_S_Init(txn));
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_BOOL_FALSE(OS_ObjectIdDefined(txn->fd));

    /* Second status check passes, open create fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 2, true);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_Init(txn));
    UT_CF_AssertEventID(CF_CFDP_S_OPEN_ERR_EID);

    /* Third status check passes, seek fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 3, true);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_Init(txn));
    UT_CF_AssertEventID(CF_CFDP_S_SEEK_END_ERR_EID);

    /* Forth status check passes, seek fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 4, true);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_Init(txn));
    UT_CF_AssertEventID(CF_CFDP_S_SEEK_BEG_ERR_EID);

    UtAssert_STUB_COUNT(CF_CRC_Start, 1); /* nothing but the first nominal case should have invoked this */
}

void Test_CF_CFDP_S_HandleFileRetention(void)
{
    /* Test Case For:
     * void CF_CFDP_S_HandleFileRetention(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;
    char              faildir[] = "f";

    /* Nominal, keep flag set - nothing done */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, true);
    txn->keep                  = true;
    txn->flags.com.is_complete = true;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 0);

    /* Failed transfer, nothing done */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, true);
    txn->flags.com.is_complete = false;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 0);

    /* Failed transfer, nothing done */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, false);
    txn->flags.com.is_complete = true;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 0);

    /* Successful transfer, keep flag unset, S1 */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = false;
    txn->keep                  = false;
    txn->flags.com.is_complete = true;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 1);
    UT_CF_AssertEventID(CF_CFDP_S_FILE_REMOVED_EID);

    /* Successful transfer, keep flag unset, S2, FIN from peer says not retained */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->keep                  = false;
    txn->flags.com.is_complete = true;
    txn->state_data.fin_fs     = CF_CFDP_FinFileStatus_UNREPORTED;
    txn->state_data.fin_dc     = CF_CFDP_FinDeliveryCode_COMPLETE;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 0);

    /* Successful transfer, keep flag unset, S2, FIN from peer says not complete */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->keep                  = false;
    txn->flags.com.is_complete = true;
    txn->state_data.fin_fs     = CF_CFDP_FinFileStatus_RETAINED;
    txn->state_data.fin_dc     = CF_CFDP_FinDeliveryCode_INCOMPLETE;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 0);

    /* Successful transfer, keep flag unset, S2, FIN from peer says complete + retained */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->keep                  = false;
    txn->flags.com.is_complete = true;
    txn->state_data.fin_fs     = CF_CFDP_FinFileStatus_RETAINED;
    txn->state_data.fin_dc     = CF_CFDP_FinDeliveryCode_COMPLETE;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 1);
    UT_CF_AssertEventID(CF_CFDP_S_FILE_REMOVED_EID);

    /* Unsuccessful transfer, keep flag unset, not commanded (e.g. polling dir), but no fail_dir set */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->keep                  = false;
    txn->flags.com.is_complete = false;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 0);
    UtAssert_STUB_COUNT(CF_CFDP_GetMoveTarget, 1);

    /* Unsuccessful transfer, keep flag unset, not commanded (e.g. polling dir), fail_dir set */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_CFDP_GetMoveTarget), UT_AltHandler_GenericPointerReturn, faildir);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->keep                  = false;
    txn->flags.com.is_complete = false;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 1);
    UtAssert_STUB_COUNT(OS_remove, 0);
    UtAssert_STUB_COUNT(CF_CFDP_GetMoveTarget, 1);
    UT_CF_AssertEventID(CF_CFDP_S_FILE_MOVED_EID);

    /* Unsuccessful transfer, keep flag unset, commanded */
    UT_ResetState(0);
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->keep                  = false;
    txn->flags.com.is_complete = false;
    txn->flags.tx.cmd_tx       = true;
    UtAssert_VOIDCALL(CF_CFDP_S_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 0);
    UtAssert_STUB_COUNT(CF_CFDP_GetMoveTarget, 0);
}

void Test_CF_CFDP_S_CheckState_NORMAL(void)
{
    /* Test Case For:
     * void CF_CFDP_S_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* incomplete file, nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->foffs                = 10;
    txn->fsize                = 20;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_NORMAL);

    /* complete file */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->foffs                = 20;
    txn->fsize                = 20;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);

    /* incomplete file with error state */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, false);
    txn->reliable_mode        = false;
    txn->foffs                = 0;
    txn->fsize                = 20;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);

    /* incomplete file with early FIN received */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->flags.tx.fin_count   = 1;
    txn->foffs                = 0;
    txn->fsize                = 20;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);
}

void Test_CF_CFDP_S_CheckState_EOF(void)
{
    /* Test Case For:
     * void CF_CFDP_S_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* nominal, send_eof still pending */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_EOF;
    txn->flags.tx.send_eof    = true;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);

    /* An error is pending */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, false);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);
    UtAssert_BOOL_FALSE(txn->flags.com.is_complete);

    /* nominal, S1, send_eof done, no closure request */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_EOF;
    txn->flags.tx.send_eof    = false;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);
    UtAssert_BOOL_TRUE(txn->flags.com.is_complete);
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinFileStatus_UNREPORTED);

    /* nominal, S1, send_eof done, closure request still pending */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_EOF;
    txn->flags.tx.send_eof    = false;
    txn->flags.com.close_req  = true;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);
    UtAssert_BOOL_FALSE(txn->flags.com.is_complete);

    /* nominal, S1, send_eof done, closure request and FIN recv */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_EOF;
    txn->flags.tx.send_eof    = false;
    txn->flags.com.close_req  = true;
    txn->flags.tx.fin_count   = 1;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);
    UtAssert_BOOL_TRUE(txn->flags.com.is_complete);

    /* nominal, S2, send_eof done, waiting on ack, timer still active */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_CheckAckNakCount), 1, true);
    txn->reliable_mode             = true;
    txn->state_data.sub_state      = CF_TxSubState_DATA_EOF;
    txn->flags.com.ack_timer_armed = true;
    txn->flags.tx.send_eof         = false;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);

    /* nominal, S2, send_eof done, waiting on ack, timer expired, under ack limit */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_CheckAckNakCount), 1, true);
    txn->reliable_mode             = true;
    txn->state_data.sub_state      = CF_TxSubState_DATA_EOF;
    txn->flags.com.ack_timer_armed = false;
    txn->flags.tx.send_eof         = false;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);
    UtAssert_BOOL_TRUE(txn->flags.tx.send_eof);

    /* nominal, S2, send_eof done, waiting on ack, timer expired, over ack limit */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_CheckAckNakCount), 1, false);
    txn->reliable_mode             = true;
    txn->state_data.sub_state      = CF_TxSubState_DATA_EOF;
    txn->flags.com.ack_timer_armed = false;
    txn->flags.tx.send_eof         = false;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);
    UtAssert_BOOL_FALSE(txn->flags.tx.send_eof);
    UtAssert_BOOL_FALSE(txn->flags.com.is_complete);

    /* nominal, S2, send_eof done, got eof-ack but no FIN, under ack limit */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->state_data.sub_state  = CF_TxSubState_DATA_EOF;
    txn->flags.tx.eof_ack_recv = true;
    txn->flags.tx.send_eof     = false;
    txn->flags.tx.fin_count    = 0;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);

    /* nominal, S2, send_eof done, got eof-ack and fin, but fin-ack not sent yet */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode          = true;
    txn->state_data.sub_state   = CF_TxSubState_DATA_EOF;
    txn->flags.tx.eof_ack_recv  = true;
    txn->flags.tx.send_eof      = false;
    txn->flags.tx.fin_count     = 1;
    txn->flags.tx.fin_ack_count = 0;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);

    /* nominal, S2, send_eof done, got eof-ack and fin, and fin-ack sent */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode          = true;
    txn->state_data.sub_state   = CF_TxSubState_DATA_EOF;
    txn->flags.tx.eof_ack_recv  = true;
    txn->flags.tx.send_eof      = false;
    txn->flags.tx.fin_count     = 1;
    txn->flags.tx.fin_ack_count = 1;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);
    UtAssert_BOOL_TRUE(txn->flags.com.is_complete);

    /* nominal, got fin but no eof-ack, timer still active */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode             = true;
    txn->state_data.sub_state      = CF_TxSubState_DATA_EOF;
    txn->flags.tx.eof_ack_recv     = false;
    txn->flags.com.ack_timer_armed = true;
    txn->flags.tx.fin_count        = 1;
    txn->flags.tx.fin_ack_count    = 1;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_DATA_EOF);

    /* nominal, got fin but no eof-ack, timer expired */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode             = true;
    txn->state_data.sub_state      = CF_TxSubState_DATA_EOF;
    txn->flags.tx.eof_ack_recv     = false;
    txn->flags.com.ack_timer_armed = false;
    txn->flags.tx.fin_count        = 1;
    txn->flags.tx.fin_ack_count    = 1;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_FILESTORE);
    UtAssert_BOOL_FALSE(txn->flags.com.is_complete);
}

void Test_CF_CFDP_S_CheckState_FILESTORE(void)
{
    /* Test Case For:
     * void CF_CFDP_S_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->state_data.sub_state = CF_TxSubState_FILESTORE;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_COMPLETE);
    UtAssert_STUB_COUNT(CF_CFDP_FinishTransaction, 1);
}

void Test_CF_CFDP_S_CheckState_COMPLETE(void)
{
    /* Test Case For:
     * void CF_CFDP_S_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->state_data.sub_state = CF_TxSubState_COMPLETE;
    UtAssert_VOIDCALL(CF_CFDP_S_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_TxSubState_COMPLETE);
}

void Test_CF_CFDP_S_CheckState(void)
{
    /* Test Case For:
     * void CF_CFDP_S_CheckState(CF_Transaction_t *txn)
     */

    UT_ResetState(0);
    Test_CF_CFDP_S_CheckState_NORMAL();
    UT_ResetState(0);
    Test_CF_CFDP_S_CheckState_EOF();
    UT_ResetState(0);
    Test_CF_CFDP_S_CheckState_FILESTORE();
    UT_ResetState(0);
    Test_CF_CFDP_S_CheckState_COMPLETE();
}

void Test_CF_CFDP_S_Tick_Nak(void)
{
    /* Test Case For:
     * void CF_CFDP_S_Tick_Nak(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;
    CF_Chunk_t        chunk;

    memset(&chunk, 0, sizeof(chunk));
    chunk.size = 100;

    /* Nominal, nothing to do */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.fd_nak_pending = false;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(txn));
    UtAssert_STUB_COUNT(CF_ChunkList_GetFirstChunk, 0);

    /* Nominal, chunk list empty */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.fd_nak_pending = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(txn));
    UtAssert_STUB_COUNT(CF_ChunkList_GetFirstChunk, 1);
    UtAssert_BOOL_FALSE(txn->flags.tx.fd_nak_pending);

    /* chunk list not empty, SendFileData fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_ChunkList_GetFirstChunk), UT_AltHandler_GenericPointerReturn, &chunk);
    txn->flags.tx.fd_nak_pending = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(txn));
    UtAssert_STUB_COUNT(CF_ChunkList_RemoveFromFirst, 0);
    UtAssert_BOOL_TRUE(txn->flags.tx.fd_nak_pending);

    /* Nominal, chunk list not empty, SendFileData works */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetHandlerFunction(UT_KEY(CF_ChunkList_GetFirstChunk), UT_AltHandler_GenericPointerReturn, &chunk);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, chunk.size);
    config->outgoing_file_chunk_size = chunk.size;
    txn->flags.tx.fd_nak_pending     = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(txn));
    UtAssert_STUB_COUNT(CF_ChunkList_RemoveFromFirst, 1);
    UtAssert_BOOL_TRUE(txn->flags.tx.fd_nak_pending);
}

/*******************************************************************************
**
** cf_cfdp_s_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_CFDP_S1_Recv, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S1_Recv");
    UtTest_Add(Test_CF_CFDP_S2_Recv, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S2_Recv");
    UtTest_Add(Test_CF_CFDP_S_AckTimerTick, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_AckTimerTick");
    UtTest_Add(Test_CF_CFDP_S_Tick, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_Tick");
    UtTest_Add(Test_CF_CFDP_S_Tick_Maintenance, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_Tick_Maintenance");
    UtTest_Add(Test_CF_CFDP_S_SendFileData, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_SendFileData");
    UtTest_Add(Test_CF_CFDP_S_SubstateSendFileData, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_SubstateSendFileData");
    UtTest_Add(Test_CF_CFDP_S_SubstateEarlyFin, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_SubstateEarlyFin");
    UtTest_Add(Test_CF_CFDP_S_SubstateRecvFin, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_SubstateRecvFin");
    UtTest_Add(Test_CF_CFDP_S2_SubstateNak, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S2_SubstateNak");
    UtTest_Add(Test_CF_CFDP_S2_SubstateEofAck, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S2_SubstateEofAck");

    UtTest_Add(Test_CF_CFDP_S_Init, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_Init");
    UtTest_Add(Test_CF_CFDP_S_HandleFileRetention, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_HandleFileRetention");
    UtTest_Add(Test_CF_CFDP_S_CheckState, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_CheckState");
    UtTest_Add(Test_CF_CFDP_S_Tick_Nak, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_Tick_Nak");
}
