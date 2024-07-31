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
#include "cf_events.h"

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

void Test_CF_CFDP_S1_Tx(void)
{
    /* Test case for:
     * void CF_CFDP_S1_Tx(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S1_Tx(txn));
}

void Test_CF_CFDP_S2_Tx(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Tx(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Tx(txn));
}

void Test_CF_CFDP_S_Tick(void)
{
    /* Test case for:
     * void CF_CFDP_S_Tick(CF_Transaction_t *txn, int *cont);
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;
    int               cont;

    cont = 0;

    /* nominal, not in CF_TxnState_S2 (noop) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));

    /* nominal, in CF_TxnState_S2, no timer expiry */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->state = CF_TxnState_S2;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* nominal, in CF_TxnState_S2, with timer expiry */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    txn->state = CF_TxnState_S2;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_INACT_TIMER);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer, 1);
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* in CF_TxnState_S2, ack_timer_armed */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->state                     = CF_TxnState_S2;
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 3); /* called twice! */

    /* in CF_TxnState_S2, ack_timer_armed + expiry */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    txn->state                     = CF_TxnState_S2;
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 4);

    /* in CF_TxnState_S2, ack_timer_armed + expiry + finack substate */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    config->chan[txn->chan_num].ack_limit = 10;
    txn->state                            = CF_TxnState_S2;
    txn->flags.com.ack_timer_armed        = true;
    txn->state_data.send.sub_state        = CF_TxSubState_WAIT_FOR_EOF_ACK;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);

    /* same, with acklimit reached */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    config->chan[txn->chan_num].ack_limit = 10;
    txn->state                            = CF_TxnState_S2;
    txn->flags.com.ack_timer_armed        = true;
    txn->state_data.send.sub_state        = CF_TxSubState_WAIT_FOR_EOF_ACK;
    txn->state_data.send.s2.acknak_count  = 9;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_ACK_LIMIT);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.ack_limit, 1);
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 2);

    /* same, with CF_CFDP_S_SendEof no message */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    config->chan[txn->chan_num].ack_limit = 10;
    txn->state                            = CF_TxnState_S2;
    txn->flags.com.ack_timer_armed        = true;
    txn->state_data.send.sub_state        = CF_TxSubState_WAIT_FOR_EOF_ACK;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->state                     = CF_TxnState_S2;
    txn->state_data.send.sub_state = CF_TxSubState_SEND_FIN_ACK;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(txn, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 3);
}

void Test_CF_CFDP_S_Tick_Nak(void)
{
    /* Test case for:
     * void CF_CFDP_S_Tick_Nak(CF_Transaction_t *txn, int *cont);
     */
    CF_Transaction_t *txn;
    int               cont;

    cont = 0;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(txn, &cont));
    UtAssert_ZERO(cont);

    /* CF_CFDP_S_CheckAndRespondNak returns 1 */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.md_need_send = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(txn, &cont));
    UtAssert_NONZERO(cont);
}

void Test_CF_CFDP_S_Cancel(void)
{
    /* Test case for:
     * void CF_CFDP_S_Cancel(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Cancel(txn));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_EOF);

    /* already EOF state */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->state_data.send.sub_state = CF_TxSubState_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_Cancel(txn));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_EOF);
}

void Test_CF_CFDP_S_SendEof(void)
{
    /* Test case for:
     * CFE_Status_t CF_CFDP_S_SendEof(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_S_SendEof(txn), CFE_SUCCESS);

    /* with CRC calc */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.crc_calc = true;
    UtAssert_INT32_EQ(CF_CFDP_S_SendEof(txn), CFE_SUCCESS);

    /* confirm retcode from CF_CFDP_SendEof is carried through */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_S_SendEof(txn), CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
}

void Test_CF_CFDP_S1_SubstateSendEof(void)
{
    /* Test case for:
     * void CF_CFDP_S1_SubstateSendEof(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal, should reset */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S1_SubstateSendEof(txn));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* should not reset transaction if error */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S1_SubstateSendEof(txn));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1); /* no increment */
}

void Test_CF_CFDP_S2_SubstateSendEof(void)
{
    /* Test case for:
     * void CF_CFDP_S2_SubstateSendEof(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal, this dequeues a transaction so q_size must be nonzero */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index] = 10;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendEof(txn));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_WAIT_FOR_EOF_ACK);
    UtAssert_BOOL_TRUE(txn->flags.com.ack_timer_armed);
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
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(txn, offset, read_size, true), 0);
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
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_READ);

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
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEEK_FD);
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
    txn->state_data.send.sub_state   = CF_TxSubState_FILEDATA;
    txn->fsize                       = CF_MAX_PDU_SIZE / 2;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, txn->fsize);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_EOF);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_UNDEFINED);

    /* nominal, less than whole file at once */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE / 2;
    txn->state_data.send.sub_state   = CF_TxSubState_FILEDATA;
    txn->fsize                       = CF_MAX_PDU_SIZE;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, config->outgoing_file_chunk_size);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_FILEDATA);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_UNDEFINED);

    /* error during read */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(txn));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_EOF);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);
}

void Test_CF_CFDP_S_CheckAndRespondNak(void)
{
    /* Test case for:
     * int CF_CFDP_S_CheckAndRespondNak(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;
    CF_ChunkWrapper_t chunks;
    CF_Chunk_t        ut_chunk;
    CF_ConfigTable_t *config;

    memset(&chunks, 0, sizeof(chunks));
    memset(&ut_chunk, 0, sizeof(ut_chunk));

    ut_chunk.offset = 0;
    ut_chunk.size   = CF_MAX_PDU_SIZE / 2;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(txn), 0);

    /* with md_need_send flag set */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.md_need_send = true;
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(txn), 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_BOOL_FALSE(txn->flags.tx.md_need_send);

    /* with md_need_send flag set, but failed */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.md_need_send = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SEND_PDU_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(txn), -1);
    UtAssert_BOOL_TRUE(txn->flags.tx.md_need_send); /* still set */

    /* with md_need_send flag set, but no message */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.md_need_send = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(txn), 1);
    UtAssert_BOOL_TRUE(txn->flags.tx.md_need_send); /* still set */

    /* with chunklist - this will send file data, which needs to be set up for */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE;
    txn->fsize                       = ut_chunk.size;
    txn->chunks                      = &chunks;
    UT_SetHandlerFunction(UT_KEY(CF_ChunkList_GetFirstChunk), UT_AltHandler_GenericPointerReturn, &ut_chunk);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, ut_chunk.size);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(txn), 1);

    /* with chunklist - failure to send file data */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE;
    txn->fsize                       = ut_chunk.size;
    txn->chunks                      = &chunks;
    UT_SetHandlerFunction(UT_KEY(CF_ChunkList_GetFirstChunk), UT_AltHandler_GenericPointerReturn, &ut_chunk);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(txn), -1);

    /* with chunklist but CF_CFDP_S_SendFileData returning 0 (nothing to send) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE;
    txn->fsize                       = ut_chunk.size;
    txn->chunks                      = &chunks;
    UT_SetHandlerFunction(UT_KEY(CF_ChunkList_GetFirstChunk), UT_AltHandler_GenericPointerReturn, &ut_chunk);
    UT_ResetState(UT_KEY(CF_CFDP_ConstructPduHeader)); /* Returns NULL by default */
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(txn), 0);
}

void Test_CF_CFDP_S2_SubstateSendFileData(void)
{
    /* Test case for:
     * void CF_CFDP_S2_SubstateSendFileData(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal, just invokes CF_CFDP_S_SubstateSendFileData */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendFileData(txn));

    /* If CF_CFDP_S_CheckAndRespondNak returns > 0 */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.md_need_send = true;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendFileData(txn));

    /* failure in CF_CFDP_S_CheckAndRespondNak, resets transaction */
    /* easiest way to trigger is via SendMd failure */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.tx.md_need_send = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SEND_PDU_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendFileData(txn));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
}

void Test_CF_CFDP_S_SubstateSendMetadata(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateSendMetadata(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* with no setup, OS_FileOpenCheck returns SUCCESS (true) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_ALREADY_OPEN);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open, 1);

    /* file already open */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    OS_OpenCreate(&txn->fd, "ut", 0, 0); /* sets fd */
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_FILEDATA);

    /* this retval is sticky and applies for the rest of the test cases */
    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), OS_ERROR);

    /* OS_FileOpenCheck does not succeed, then WrappedOpenCreate fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_OPEN);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open, 2);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);

    /* first CF_WrappedLseek fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEEK_END);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek, 1);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);

    /* second CF_WrappedLseek fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 2, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEEK_BEG);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek, 2);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);

    /* CF_CFDP_SendMd fails w/ ERROR */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SEND_PDU_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEND_MD);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);

    /* CF_CFDP_SendMd fails w/ NO_MSG (no event here) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_UNDEFINED);

    /* everything works */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(txn));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_FILEDATA);
}

void Test_CF_CFDP_S_SubstateSendFinAck(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateSendFinAck(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFinAck(txn));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* CF_SEND_PDU_NO_BUF_AVAIL_ERROR status */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendAck), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFinAck(txn));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1); /* not incremented */
}

void Test_CF_CFDP_S2_EarlyFin(void)
{
    /* Test case for:
     * void CF_CFDP_S2_EarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_EarlyFin(txn, ph));
}

void Test_CF_CFDP_S2_Fin(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Fin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Fin(txn, ph));
}

void Test_CF_CFDP_S2_Nak(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Nak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduNak_t *   nak;

    /* no segments */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(txn, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_PDU_NAK);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 1);

    /* nominal, re-send md request (0,0) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 1;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {0, 0};
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(txn, ph));
    UtAssert_BOOL_TRUE(txn->flags.tx.md_need_send);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests, 1);

    /* nominal, nonzero offsets */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 2;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {0, 200};
    nak->segment_list.segments[1]  = (CF_Logical_SegmentRequest_t) {200, 300};
    txn->fsize                     = 300;
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(txn, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests, 3);

    /* bad segments */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 3;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {200, 100};
    nak->segment_list.segments[1]  = (CF_Logical_SegmentRequest_t) {100, 400};
    nak->segment_list.segments[2]  = (CF_Logical_SegmentRequest_t) {400, 0};
    txn->fsize                     = 300;
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(txn, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests, 6);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_INVALID_SR);

    /* bad decode */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvNak), 1, -1);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 1;
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(txn, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 2);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_PDU_NAK);
}

void Test_CF_CFDP_S2_Nak_Arm(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Nak_Arm(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak_Arm(txn, ph));
}

void Test_CF_CFDP_S2_WaitForEofAck(void)
{
    /* Test case for:
     * void CF_CFDP_S2_WaitForEofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_WaitForEofAck(txn, ph));
    UtAssert_UINT32_EQ(txn->state_data.send.sub_state, CF_TxSubState_WAIT_FOR_FIN);
    UtAssert_BOOL_FALSE(txn->flags.com.ack_timer_armed);

    /* failure of CF_CFDP_RecvAck */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvAck), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S2_WaitForEofAck(txn, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_PDU_EOF);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 1);

    /* with error status */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_TxnStatus_IsError), true);
    UtAssert_VOIDCALL(CF_CFDP_S2_WaitForEofAck(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
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
    UtTest_Add(Test_CF_CFDP_S1_Tx, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S1_Tx");
    UtTest_Add(Test_CF_CFDP_S2_Tx, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S2_Tx");
    UtTest_Add(Test_CF_CFDP_S_Tick, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_Tick");
    UtTest_Add(Test_CF_CFDP_S_Tick_Nak, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_Tick_Nak");
    UtTest_Add(Test_CF_CFDP_S_Cancel, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_Cancel");
    UtTest_Add(Test_CF_CFDP_S_SendEof, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_SendEof");
    UtTest_Add(Test_CF_CFDP_S1_SubstateSendEof, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S1_SubstateSendEof");
    UtTest_Add(Test_CF_CFDP_S2_SubstateSendEof, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S2_SubstateSendEof");
    UtTest_Add(Test_CF_CFDP_S_SendFileData, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S_SendFileData");
    UtTest_Add(Test_CF_CFDP_S_SubstateSendFileData, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_SubstateSendFileData");
    UtTest_Add(Test_CF_CFDP_S_CheckAndRespondNak, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_CheckAndRespondNak");
    UtTest_Add(Test_CF_CFDP_S2_SubstateSendFileData, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S2_SubstateSendFileData");
    UtTest_Add(Test_CF_CFDP_S_SubstateSendMetadata, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_SubstateSendMetadata");
    UtTest_Add(Test_CF_CFDP_S_SubstateSendFinAck, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S_SubstateSendFinAck");
    UtTest_Add(Test_CF_CFDP_S2_EarlyFin, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S2_EarlyFin");
    UtTest_Add(Test_CF_CFDP_S2_Fin, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S2_Fin");
    UtTest_Add(Test_CF_CFDP_S2_Nak, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S2_Nak");
    UtTest_Add(Test_CF_CFDP_S2_Nak_Arm, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown, "CF_CFDP_S2_Nak_Arm");
    UtTest_Add(Test_CF_CFDP_S2_WaitForEofAck, cf_cfdp_s_tests_Setup, cf_cfdp_s_tests_Teardown,
               "CF_CFDP_S2_WaitForEofAck");
}
