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

static void UT_CFDP_R_SetupBasicRxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    /* placeholder, nothing for now in this module */
}

static void UT_CFDP_R_SetupBasicTxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    /* Make it so a call to CF_CFDP_ConstructPduBuffer returns the same PDU buffer */
    UT_SetHandlerFunction(UT_KEY(CF_CFDP_ConstructPduHeader), UT_AltHandler_GenericPointerReturn, pdu_buffer);
}

static void UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_t setup, CF_Logical_PduBuffer_t **pdu_buffer_p,
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
        UT_CFDP_R_SetupBasicTxState(&ut_pdu_buffer);
    }
    else if (setup == UT_CF_Setup_RX)
    {
        UT_CFDP_R_SetupBasicRxState(&ut_pdu_buffer);
    }

    /* reset the event ID capture between each sub-case */
    UT_CF_ResetEventCapture();

    /* Capture calls to CF_CFDP_SetTxnState() to capture transaction status */
    UT_SetHandlerFunction(UT_KEY(CF_CFDP_SetTxnStatus), UT_AltHandler_CaptureTransactionStatus, &ut_history.txn_stat);
}

/*******************************************************************************
**
**  cf_cfdp_r_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_r_tests_Setup(void)
{
    cf_tests_Setup();

    /* make sure global data is wiped between tests */
    memset(&CF_AppData, 0, sizeof(CF_AppData));
}

void cf_cfdp_r_tests_Teardown(void)
{
    cf_tests_Teardown();
}

/*******************************************************************************
**
**  Unit-specific test implementation
**
*******************************************************************************/

void Test_CF_CFDP_R1_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_R1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);

    UtAssert_VOIDCALL(CF_CFDP_R1_Recv(txn, ph));
}

void Test_CF_CFDP_R2_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_R2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal, ack timer not armed */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R2_Recv(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 0);

    /* When ack timer is armed, this should reset it */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_Recv(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
}

void Test_CF_CFDP_R_AckTimerTick(void)
{
    /* Test case for:
     * void CF_CFDP_R_AckTimerTick(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* no-op if not in R2 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode             = false;
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_R_AckTimerTick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);

    /* no-op if not armed */
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = false;
    UtAssert_VOIDCALL(CF_CFDP_R_AckTimerTick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);

    /* in R2 state, ack_timer_armed set but not expired */
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_R_AckTimerTick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* in R2 state, ack_timer_armed set, timer expires */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = true;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_AckTimerTick(txn));
    UtAssert_BOOL_FALSE(txn->flags.com.ack_timer_armed);
}

void Test_CF_CFDP_R_Tick(void)
{
    /* Test case for:
     * void CF_CFDP_R_Tick(CF_Transaction_t *txn, int *cont);
     */
    CF_Transaction_t *txn;

    /* nominal, holdover - just ticks */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->state = CF_TxnState_HOLD;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* nominal, active transaction */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode = false;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_STUB_COUNT(CF_CFDP_CompleteTick, 1);

    /* not in R2 state, timer expired */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->state = CF_TxnState_HOLD;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(txn));
    UtAssert_STUB_COUNT(CF_CFDP_RecycleTransaction, 1);

    /* nominal, in R2 state */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = true;
    txn->state_data.sub_state = CF_RxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* in HOLD state, timer expired now */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_TERMINATED);
    txn->state = CF_TxnState_HOLD;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(txn));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);

    /* in R2 state, timer expired now */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_ACTIVE);
    txn->reliable_mode = true;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.inactivity_fired);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_INACTIVITY_DETECTED);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);

    /* timer already expired */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode              = true;
    txn->flags.com.inactivity_fired = true;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.inactivity_fired);
    UtAssert_STUB_COUNT(CF_CFDP_CompleteTick, 1);
}

void Test_CF_CFDP_R_Tick_Maintenance(void)
{
    /* Test case for:
     * void CF_CFDP_R_Tick_Maintenance(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* in R1 state, nominal (does nothing, called for coverage) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode = false;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));

    /* in R1 state, send_fin set  */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode     = false;
    txn->flags.rx.send_fin = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));
    UtAssert_STUB_COUNT(CF_CFDP_SendFin, 1);

    /* in R2 state, send_ack set */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode              = true;
    txn->flags.rx.eof_count         = 1;
    txn->flags.rx.eof_ack_count     = 0;
    txn->flags.com.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_UINT8_EQ(txn->flags.rx.eof_ack_count, txn->flags.rx.eof_count);

    /* same as above, but SendAck fails */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendAck), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    txn->reliable_mode              = true;
    txn->flags.rx.eof_count         = 1;
    txn->flags.rx.eof_ack_count     = 0;
    txn->flags.com.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));
    UtAssert_UINT8_LT(txn->flags.rx.eof_ack_count, txn->flags.rx.eof_count);

    /* in R2 state, send_nak set */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode              = true;
    txn->flags.rx.send_nak          = true;
    txn->flags.com.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));
    UtAssert_BOOL_FALSE(txn->flags.rx.send_nak);

    /* same as above, but CF_CFDP_R_SendNak fails */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendNak), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    txn->reliable_mode              = true;
    txn->flags.rx.send_nak          = true;
    txn->flags.com.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));
    UtAssert_BOOL_TRUE(txn->flags.rx.send_nak);

    /* in R2 state, send_fin set */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode              = true;
    txn->flags.rx.send_fin          = true;
    txn->flags.com.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));
    UtAssert_BOOL_FALSE(txn->flags.rx.send_fin);

    /* same as above, but CF_CFDP_R2_SubstateSendFin fails */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendFin), 1, CF_SEND_PDU_NO_BUF_AVAIL_ERROR);
    txn->reliable_mode              = true;
    txn->flags.rx.send_fin          = true;
    txn->flags.com.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick_Maintenance(txn));
    UtAssert_BOOL_TRUE(txn->flags.rx.send_fin);
}

void Test_CF_CFDP_R_Init(void)
{
    /* Test case for:
     * void CF_CFDP_R_Init(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->state_data.fin_dc = -1;
    txn->state_data.fin_fs = -1;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(txn));
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_INVALID);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_INVALID);

    /* nominal, R2 state, creates tempfile */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(txn));
    UT_CF_AssertEventID(CF_CFDP_R_TEMP_FILE_INF_EID);

    /* failure of file open, class 1 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, -1);
    txn->reliable_mode = false;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(txn));
    UT_CF_AssertEventID(CF_CFDP_R_CREAT_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open, 1);

    /* failure of file open, class 2 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, -1);
    txn->reliable_mode = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(txn));
    UT_CF_AssertEventID(CF_CFDP_R_CREAT_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open, 2);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);
}

void Test_CF_CFDP_R_CheckCrc(void)
{
    /* Test case for:
     * int CF_CFDP_R_CheckCrc(CF_Transaction_t *txn, uint32 expected_crc);
     */
    CF_Transaction_t *txn;

    /* CRC mismatch, class 1 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode      = false;
    txn->crc.result         = 0xdeadbeef;
    txn->state_data.eof_crc = 0x1badc0de;
    UtAssert_INT32_EQ(CF_CFDP_R_CheckCrc(txn), CF_ERROR);
    UT_CF_AssertEventID(CF_CFDP_R_CRC_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.crc_mismatch, 1);

    /* CRC mismatch, class 2 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode      = true;
    txn->crc.result         = 0xdeadbeef;
    txn->state_data.eof_crc = 0x2badc0de;
    UtAssert_INT32_EQ(CF_CFDP_R_CheckCrc(txn), CF_ERROR);
    UT_CF_AssertEventID(CF_CFDP_R_CRC_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.crc_mismatch, 2);

    /* CRC match */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->crc.result         = 0xc0ffee;
    txn->state_data.eof_crc = 0xc0ffee;
    UtAssert_INT32_EQ(CF_CFDP_R_CheckCrc(txn), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void Test_CF_CFDP_R_ProcessFd(void)
{
    /* Test case for:
     * int CF_CFDP_R_ProcessFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *              txn;
    CF_Logical_PduBuffer_t *        ph;
    CF_Logical_PduFileDataHeader_t *fd;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    fd           = &ph->int_header.fd;
    fd->data_len = 100;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), fd->data_len);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(txn, ph), 0);
    UtAssert_UINT32_EQ(txn->state_data.cached_pos, 100);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.file_data_bytes, 100);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedWrite, 1);

    /* call again, but for something at a different offset */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    fd           = &ph->int_header.fd;
    fd->data_len = 100;
    fd->offset   = 200;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), fd->offset);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(txn, ph), 0);
    UtAssert_UINT32_EQ(txn->state_data.cached_pos, 300);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.file_data_bytes, 200);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
    UtAssert_STUB_COUNT(CF_WrappedWrite, 2);
    UtAssert_UINT32_EQ(txn->state_data.cached_pos, 300);

    /* call again, but with a failed write */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    fd                         = &ph->int_header.fd;
    fd->data_len               = 100;
    fd->offset                 = 300;
    txn->state_data.cached_pos = 300;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), -1);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(txn, ph), -1);
    UtAssert_UINT32_EQ(txn->state_data.cached_pos, 300);
    UT_CF_AssertEventID(CF_CFDP_R_WRITE_ERR_EID);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);

    /* call again, but with a failed lseek */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    fd                         = &ph->int_header.fd;
    fd->data_len               = 100;
    fd->offset                 = 200;
    txn->state_data.cached_pos = 300;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), -1);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(txn, ph), -1);
    UtAssert_UINT32_EQ(txn->state_data.cached_pos, 300);
    UT_CF_AssertEventID(CF_CFDP_R_SEEK_FD_ERR_EID);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILE_SIZE_ERROR);

    /* these stats should have been updated during the course of this test */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].counters.fault.file_write, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].counters.fault.file_seek, 1);
}

void Test_CF_CFDP_R_SubstateRecvEof(void)
{
    /* Test case for:
     * void CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_CheckAckNakCount), true);

    /* nominal, accept EOF */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.rx.eof_count  = 0;
    txn->state_data.eof_size = 0;
    ph->int_header.eof.size  = 10;
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvEof(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_CheckAckNakCount, 1);
    UtAssert_EQ(CF_FileSize_t, txn->state_data.eof_size, 10);

    /* repeat EOF */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.rx.eof_count  = 1;
    txn->state_data.eof_size = 10;
    ph->int_header.eof.size  = 20;
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvEof(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_CheckAckNakCount, 2);
    UtAssert_EQ(CF_FileSize_t, txn->state_data.eof_size, 10);

    /* At limit */
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_CheckAckNakCount), false);
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvEof(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_CheckAckNakCount, 3);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_POS_ACK_LIMIT_REACHED);

    /* with failure of CF_CFDP_RecvEof() */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), -1);
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvEof(txn, ph));
    UT_CF_AssertEventID(CF_CFDP_R_PDU_EOF_ERR_EID);

    /* these counters should have been updated during the test */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].counters.recv.error, 1);
}

void Test_CF_CFDP_R_SubstateRecvFileData(void)
{
    /* Test case for:
     * void CF_CFDP_R_SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->state_data.acknak_count = 1;
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvFileData(txn, ph));
    UtAssert_ZERO(txn->state_data.acknak_count); /* this resets the counter */
    UtAssert_STUB_COUNT(CF_ChunkListAdd, 1);     /* called */

    /* failure in CF_CFDP_RecvFd (bad packet) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->state_data.acknak_count = 1;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvFd), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvFileData(txn, ph));
    UtAssert_STUB_COUNT(CF_ChunkListAdd, 1);            /* NOT called */
    UtAssert_UINT8_EQ(txn->state_data.acknak_count, 1); /* NOT reset */

    /* failure in CF_CFDP_R_ProcessFd (via failure of CF_WrappedWrite) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->state_data.acknak_count = 1;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, false);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedWrite), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvFileData(txn, ph));
    UtAssert_UINT8_EQ(txn->state_data.acknak_count, 1); /* NOT reset */
}

void Test_CF_CFDP_R2_GapCompute(void)
{
    /* Test case for:
     * void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *chunk, void *opaque);
     */
    CF_ChunkList_t      chunks;
    CF_Chunk_t          chunk;
    CF_GapComputeArgs_t args;
    CF_Logical_PduNak_t nak;

    memset(&chunks, 0, sizeof(chunks));
    memset(&chunk, 0, sizeof(chunk));
    memset(&args, 0, sizeof(args));
    memset(&nak, 0, sizeof(nak));
    args.nak = &nak;

    /* nominal */
    chunk.offset    = 11000;
    chunk.size      = 100;
    nak.scope_start = 10000;
    nak.scope_end   = 20000;
    UtAssert_VOIDCALL(CF_CFDP_R2_GapCompute(&chunks, &chunk, &args));
    UtAssert_UINT32_EQ(nak.segment_list.num_segments, 1);

    /* the offset start/end should be normalized to the scope start/end */
    UtAssert_UINT32_EQ(nak.segment_list.segments[0].offset_start, 1000);
    UtAssert_UINT32_EQ(nak.segment_list.segments[0].offset_end, 1100);

    /* confirm that CF_PDU_MAX_SEGMENTS is not exceeded */
    nak.segment_list.num_segments = CF_PDU_MAX_SEGMENTS;
    UtAssert_VOIDCALL(CF_CFDP_R2_GapCompute(&chunks, &chunk, &args));
    UtAssert_UINT32_EQ(nak.segment_list.num_segments, CF_PDU_MAX_SEGMENTS);
}

void Test_CF_CFDP_R_SubstateSendNak(void)
{
    /* Test case for:
     * int CF_CFDP_R_SendNak(CF_Transaction_t *txn);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;
    CF_ChunkWrapper_t       chunks;

    memset(&chunks, 0, sizeof(chunks));

    /* no packet available */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_R_SendNak(txn), CF_SEND_PDU_NO_BUF_AVAIL_ERROR);

    /* with md_recv flag false, this should request one by sending a blank NAK */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_R_SendNak(txn), 0);
    UT_CF_AssertEventID(CF_CFDP_R_REQUEST_MD_INF_EID);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);

    /* with md_recv flag true, this should call gap compute to assemble the NAK */
    /* this requires the chunks list to be set up, and by default compute_gaps will
       return 0 (no gaps) so the transaction goes to complete */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.rx.md_recv    = true;
    txn->chunks              = &chunks;
    chunks.chunks.count      = 1;
    chunks.chunks.max_chunks = 2;
    UtAssert_INT32_EQ(CF_CFDP_R_SendNak(txn), 0);
    UtAssert_STUB_COUNT(CF_ChunkList_ComputeGaps, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1); /* did not increment */

    /* same, but return nonzero number of gaps */
    /* this also should use the max chunks instead of count */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_ChunkList_ComputeGaps), 1, 1);
    txn->flags.rx.md_recv    = true;
    txn->chunks              = &chunks;
    chunks.chunks.count      = 3;
    chunks.chunks.max_chunks = 2;
    UtAssert_INT32_EQ(CF_CFDP_R_SendNak(txn), 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 2);
}

void Test_CF_CFDP_R_CalcCrcChunk(void)
{
    /* Test case for:
     * int CF_CFDP_R_CalcCrcChunk(CF_Transaction_t *txn);
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;

    /* nominal with zero size file */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.crc_complete = false;
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcChunk(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);

    /* nominal with non zero size file, runs the loop */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    txn->flags.com.crc_complete          = false;
    config->rx_crc_calc_bytes_per_wakeup = 100;
    txn->fsize                           = 70;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, txn->fsize);
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcChunk(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);

    /* nominal with file larger than rx_crc_calc_bytes_per_wakeup */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    config->rx_crc_calc_bytes_per_wakeup = CF_R2_CRC_CHUNK_SIZE;
    txn->fsize                           = CF_R2_CRC_CHUNK_SIZE + 100;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, CF_R2_CRC_CHUNK_SIZE);
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcChunk(txn));
    UtAssert_BOOL_FALSE(txn->flags.com.crc_complete);

    /* nominal with file size larger than CF_R2_CRC_CHUNK_SIZE (this will do 2 reads) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    config->rx_crc_calc_bytes_per_wakeup = CF_R2_CRC_CHUNK_SIZE * 2;
    txn->fsize                           = CF_R2_CRC_CHUNK_SIZE + 100;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, CF_R2_CRC_CHUNK_SIZE);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, 100);
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcChunk(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);

    /* failure of read */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    config->rx_crc_calc_bytes_per_wakeup = 100;
    txn->fsize                           = 50;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcChunk(txn));
    UT_CF_AssertEventID(CF_CFDP_R_READ_ERR_EID);
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILE_SIZE_ERROR);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_read, 1);
}

void Test_CF_CFDP_R2_Recv_fin_ack(void)
{
    /* Test case for:
     * void CF_CFDP_R2_SubstateRecvFinAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    ph->int_header.ack.ack_directive_code = CF_CFDP_FileDirective_FIN;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFinAck(txn, ph));
    UtAssert_BOOL_TRUE(txn->flags.rx.finack_recv);

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.rx.finack_recv             = false;
    ph->int_header.ack.ack_directive_code = -1;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFinAck(txn, ph));
    UtAssert_BOOL_FALSE(txn->flags.rx.finack_recv);

    /* failure in CF_CFDP_RecvAck */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvAck), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFinAck(txn, ph));
    UT_CF_AssertEventID(CF_CFDP_R_PDU_FINACK_ERR_EID);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error, 2);
}

void Test_CF_CFDP_R_CheckComplete(void)
{
    /*
     * Test Case for:
     * bool CF_CFDP_R_CheckComplete(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    /* No flags set */
    UtAssert_BOOL_FALSE(CF_CFDP_R_CheckComplete(txn));

    /* Only recv_md, no eof */
    txn->flags.rx.md_recv = true;
    UtAssert_BOOL_FALSE(CF_CFDP_R_CheckComplete(txn));

    /* Only eof, no recv_md */
    txn->flags.rx.eof_count = 1;
    txn->flags.rx.md_recv   = false;
    UtAssert_BOOL_FALSE(CF_CFDP_R_CheckComplete(txn));

    /* Got EOF+MD but gaps in file */
    txn->flags.rx.eof_count = 1;
    txn->flags.rx.md_recv   = true;
    UT_SetDeferredRetcode(UT_KEY(CF_ChunkList_ComputeGaps), 1, 1);
    UtAssert_BOOL_FALSE(CF_CFDP_R_CheckComplete(txn));

    /* No gaps in file */
    UtAssert_BOOL_TRUE(CF_CFDP_R_CheckComplete(txn));

    /* NAK pending */
    txn->flags.rx.send_nak = true;
    UtAssert_BOOL_FALSE(CF_CFDP_R_CheckComplete(txn));
}

void Test_CF_CFDP_R_SendNak(void)
{
    /*
     * Test Case for:
     * CF_CFDP_R_SendNak()
     */
}

void Test_CF_CFDP_R_CalcCrcStart(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CalcCrcStart(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* Nominal success */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->fsize                 = 32;
    txn->state_data.cached_pos = txn->fsize;
    txn->state_data.eof_size   = txn->fsize;
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcStart(txn));
    UtAssert_STUB_COUNT(CF_CRC_Start, 1);
    UtAssert_BOOL_FALSE(txn->flags.com.crc_complete);
    UtAssert_ZERO(txn->state_data.cached_pos);

    /* Size mismatch */
    UT_ResetState(UT_KEY(CF_CRC_Start));
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->fsize                 = 32;
    txn->state_data.cached_pos = txn->fsize;
    txn->state_data.eof_size   = txn->fsize - 1;
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcStart(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);
    UT_CF_AssertEventID(CF_CFDP_R_SIZE_MISMATCH_ERR_EID);
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, -1);
    txn->fsize                 = 32;
    txn->state_data.cached_pos = txn->fsize;
    txn->state_data.eof_size   = txn->fsize;
    UtAssert_VOIDCALL(CF_CFDP_R_CalcCrcStart(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);
    UT_CF_AssertEventID(CF_CFDP_R_SEEK_CRC_ERR_EID);
    UtAssert_BOOL_TRUE(txn->flags.com.crc_complete);
    UtAssert_STUB_COUNT(CF_CRC_Start, 0);
}

void Test_CF_CFDP_R2_SubstateRecvFinAck(void)
{
    /*
     * Test Case for:
     * CF_CFDP_R2_SubstateRecvFinAck()
     */
}

void Test_CF_CFDP_R_SubstateRecvMd(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_SubstateRecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvMd(txn, ph));
    UtAssert_BOOL_TRUE(txn->flags.rx.md_recv);

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->flags.rx.md_recv = true;
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvMd(txn, ph));
    UtAssert_BOOL_TRUE(txn->flags.rx.md_recv);

    /* Failure in CF_CFDP_RecvMd */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvMd), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R_SubstateRecvMd(txn, ph));
    UtAssert_BOOL_FALSE(txn->flags.rx.md_recv);
}

void Test_CF_CFDP_R_HandleFileRetention(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_HandleFileRetention(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* with no FD or tempfile, does nothing, reports invalid */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R_HandleFileRetention(txn));
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_INVALID);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_INVALID);

    /* Same but with FD */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->fd = OS_ObjectIdFromInteger(1);
    UtAssert_VOIDCALL(CF_CFDP_R_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_INVALID);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_INVALID);

    /* Incomplete file */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.rx.tempfile_created = true;
    txn->flags.com.is_complete     = false;
    UtAssert_VOIDCALL(CF_CFDP_R_HandleFileRetention(txn));
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_INCOMPLETE);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_DISCARDED);

    /* Complete file, should move it (nominal success case) */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.rx.tempfile_created = true;
    txn->flags.com.is_complete     = true;
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    UtAssert_VOIDCALL(CF_CFDP_R_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 1);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_RETAINED);
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_COMPLETE);
    UT_CF_AssertEventID(CF_CFDP_R_FILE_RETAINED_EID);

    /* Complete file, CRC check fails */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->flags.rx.tempfile_created = true;
    txn->flags.com.is_complete     = true;
    txn->state_data.eof_crc        = 0xac0ffee;
    txn->crc.result                = 0xabadc0de;
    UtAssert_VOIDCALL(CF_CFDP_R_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_remove, 1);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_DISCARDED);
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_COMPLETE);
    UT_CF_AssertEventID(CF_CFDP_R_NOT_RETAINED_EID);

    /* with tempfile_created, move fails */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.rx.tempfile_created = true;
    txn->flags.com.is_complete     = true;
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    UT_SetDeferredRetcode(UT_KEY(OS_mv), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_R_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 1);
    UtAssert_STUB_COUNT(OS_remove, 1);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_DISCARDED_FILESTORE);
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_COMPLETE);
    UT_CF_AssertEventID(CF_CFDP_R_NOT_RETAINED_EID);

    /* transaction already in error state, not related to file storage (complete file but errored) */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.rx.tempfile_created = true;
    txn->flags.com.is_complete     = true;
    txn->history->txn_stat         = CF_TxnStatus_POS_ACK_LIMIT_REACHED;
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), false);
    UtAssert_VOIDCALL(CF_CFDP_R_HandleFileRetention(txn));
    UtAssert_STUB_COUNT(OS_mv, 0);
    UtAssert_STUB_COUNT(OS_remove, 1);
    UtAssert_UINT8_EQ(txn->state_data.fin_fs, CF_CFDP_FinFileStatus_DISCARDED);
    UtAssert_UINT8_EQ(txn->state_data.fin_dc, CF_CFDP_FinDeliveryCode_COMPLETE);
    UT_CF_AssertEventID(CF_CFDP_R_NOT_RETAINED_EID);
}

void Test_CF_CFDP_R_CheckState_NORMAL(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* DATA_NORMAL state, no EOF */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_RxSubState_DATA_NORMAL;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_DATA_NORMAL);

    /* DATA_NORMAL state, with EOF */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_RxSubState_DATA_NORMAL;
    txn->flags.rx.eof_count   = 1;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_DATA_EOF);

    /* DATA_NORMAL state, with error */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, false);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_RxSubState_DATA_NORMAL;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FILESTORE);

    /* DATA_NORMAL state, complete file (R2 nominal) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = true;
    txn->flags.rx.eof_count   = 1;
    txn->state_data.sub_state = CF_RxSubState_DATA_NORMAL;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_DATA_EOF);
    UtAssert_BOOL_TRUE(txn->flags.rx.send_nak);
}

void Test_CF_CFDP_R_CheckState_EOF(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* DATA_EOF state, complete file (R1 nominal) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = false;
    txn->flags.rx.eof_count    = 1;
    txn->flags.rx.md_recv      = true;
    txn->flags.com.is_complete = false;
    txn->state_data.sub_state  = CF_RxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_VALIDATE);
    UtAssert_BOOL_TRUE(txn->flags.com.is_complete);
    UtAssert_BOOL_FALSE(txn->flags.rx.send_nak);

    /* DATA_EOF state, incomplete in R1 (no retry, will keep spinning in EOF until inactive timer) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode        = false;
    txn->flags.rx.md_recv     = false;
    txn->state_data.sub_state = CF_RxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_DATA_EOF);

    /* DATA_EOF state, with error */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = false;
    txn->state_data.sub_state = CF_RxSubState_DATA_EOF;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, false);
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FILESTORE);

    /* DATA_EOF state, incomplete in R2, NAK pending */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = false;
    txn->flags.rx.send_nak         = true;
    txn->flags.rx.md_recv          = false;
    txn->state_data.sub_state      = CF_RxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_DATA_EOF);

    /* DATA_EOF state, incomplete in R2, NAK sent */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = true;
    txn->flags.rx.send_nak         = false;
    txn->flags.rx.md_recv          = false;
    txn->state_data.sub_state      = CF_RxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_DATA_EOF);

    /* DATA_EOF state, incomplete in R2, within nak limit, re-nak */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = false;
    txn->flags.rx.send_nak         = false;
    txn->flags.rx.md_recv          = false;
    txn->state_data.sub_state      = CF_RxSubState_DATA_EOF;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_CheckAckNakCount), 1, true);
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_DATA_EOF);
    UtAssert_BOOL_TRUE(txn->flags.rx.send_nak);

    /* DATA_EOF state, incomplete in R2, reached nak limit */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode             = true;
    txn->flags.com.ack_timer_armed = false;
    txn->flags.rx.send_nak         = false;
    txn->flags.rx.md_recv          = false;
    txn->state_data.sub_state      = CF_RxSubState_DATA_EOF;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_CheckAckNakCount), 1, false);
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FILESTORE);
    UtAssert_BOOL_FALSE(txn->flags.rx.send_nak);
}

void Test_CF_CFDP_R_CheckState_VALIDATE(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* VALIDATE state, nominal, not complete yet */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->fsize                 = 100;
    txn->state_data.cached_pos = 90;
    txn->state_data.sub_state  = CF_RxSubState_VALIDATE;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_VALIDATE);

    /* VALIDATE state, nominal, complete */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_TxnIsOK), true);
    txn->reliable_mode         = true;
    txn->fsize                 = 100;
    txn->state_data.cached_pos = 100;
    txn->state_data.sub_state  = CF_RxSubState_VALIDATE;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FILESTORE);

    /* VALIDATE state, nominal, error */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode         = true;
    txn->fsize                 = 200;
    txn->state_data.cached_pos = 100;
    txn->state_data.sub_state  = CF_RxSubState_VALIDATE;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_TxnIsOK), 1, false);
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FILESTORE);
}

void Test_CF_CFDP_R_CheckState_FILESTORE(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* FILESTORE state, R1, no fin requested */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = false;
    txn->flags.com.close_req  = false;
    txn->state_data.sub_state = CF_RxSubState_FILESTORE;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_COMPLETE);
    UtAssert_BOOL_FALSE(txn->flags.rx.send_fin);

    /* FILESTORE state, R1, with fin requested */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = false;
    txn->flags.com.close_req  = true;
    txn->state_data.sub_state = CF_RxSubState_FILESTORE;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FINACK);
    UtAssert_BOOL_TRUE(txn->flags.rx.send_fin);

    /* FILESTORE state, R2 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = true;
    txn->state_data.sub_state = CF_RxSubState_FILESTORE;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FINACK);
    UtAssert_BOOL_TRUE(txn->flags.rx.send_fin);
}

void Test_CF_CFDP_R_CheckState_FINACK(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* FINACK state, R1, fin pending */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = false;
    txn->flags.rx.send_fin    = true;
    txn->state_data.sub_state = CF_RxSubState_FINACK;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FINACK);

    /* FINACK state, R1, fin sent */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = false;
    txn->flags.rx.send_fin    = false;
    txn->state_data.sub_state = CF_RxSubState_FINACK;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_COMPLETE);
    UtAssert_STUB_COUNT(CF_CFDP_FinishTransaction, 1);

    /* FINACK state, R2, fin sent, acked */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode        = true;
    txn->flags.rx.finack_recv = true;
    txn->flags.rx.send_fin    = false;
    txn->state_data.sub_state = CF_RxSubState_FINACK;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_COMPLETE);
    UtAssert_STUB_COUNT(CF_CFDP_FinishTransaction, 1);

    /* FINACK state, R2, fin sent, not acked, timer active */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode             = true;
    txn->flags.rx.finack_recv      = false;
    txn->flags.rx.send_fin         = false;
    txn->flags.com.ack_timer_armed = true;
    txn->state_data.sub_state      = CF_RxSubState_FINACK;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FINACK);

    /* FINACK state, R2, fin sent, not acked, timer expired, under limit */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode             = true;
    txn->flags.rx.finack_recv      = false;
    txn->flags.rx.send_fin         = false;
    txn->flags.com.ack_timer_armed = false;
    txn->state_data.sub_state      = CF_RxSubState_FINACK;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_CheckAckNakCount), 1, true);
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_FINACK);
    UtAssert_BOOL_TRUE(txn->flags.rx.send_fin);

    /* FINACK state, R2, fin sent, not acked, timer expired, over limit */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode             = true;
    txn->flags.rx.finack_recv      = false;
    txn->flags.rx.send_fin         = false;
    txn->flags.com.ack_timer_armed = false;
    txn->state_data.sub_state      = CF_RxSubState_FINACK;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_CheckAckNakCount), 1, false);
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_COMPLETE);
    UtAssert_BOOL_FALSE(txn->flags.rx.send_fin);
    UtAssert_STUB_COUNT(CF_CFDP_FinishTransaction, 1);

    /* FINACK state, R2, fin sent, not acked, inactivity reached */
    UT_ResetState(0);
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->reliable_mode              = true;
    txn->flags.rx.finack_recv       = false;
    txn->flags.rx.send_fin          = false;
    txn->flags.com.inactivity_fired = true;
    txn->state_data.sub_state       = CF_RxSubState_FINACK;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_COMPLETE);
    UtAssert_BOOL_FALSE(txn->flags.rx.send_fin);
    UtAssert_STUB_COUNT(CF_CFDP_FinishTransaction, 1);
}

void Test_CF_CFDP_R_CheckState_COMPLETE(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* COMPLETE state */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->state_data.sub_state = CF_RxSubState_COMPLETE;
    UtAssert_VOIDCALL(CF_CFDP_R_CheckState(txn));
    UtAssert_UINT8_EQ(txn->state_data.sub_state, CF_RxSubState_COMPLETE);
}

void Test_CF_CFDP_R_CheckState(void)
{
    /*
     * Test Case for:
     * void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
     */

    /* This is a state machine and thus has all the if/else and case logic
     * centered in this routine, and therefore needs a lot of cases to exercise
     * all the if/else conditions.  To make the test cases more manageable, each
     * state gets a separate test routine. */

    UT_ResetState(0);
    Test_CF_CFDP_R_CheckState_NORMAL();
    UT_ResetState(0);
    Test_CF_CFDP_R_CheckState_EOF();
    UT_ResetState(0);
    Test_CF_CFDP_R_CheckState_VALIDATE();
    UT_ResetState(0);
    Test_CF_CFDP_R_CheckState_FILESTORE();
    UT_ResetState(0);
    Test_CF_CFDP_R_CheckState_FINACK();
    UT_ResetState(0);
    Test_CF_CFDP_R_CheckState_COMPLETE();
}

/*******************************************************************************
**
** cf_cfdp_r_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_CFDP_R1_Recv, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R1_Recv");
    UtTest_Add(Test_CF_CFDP_R2_Recv, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R2_Recv");
    UtTest_Add(Test_CF_CFDP_R_AckTimerTick, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_AckTimerTick");
    UtTest_Add(Test_CF_CFDP_R_Tick, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_Tick");
    UtTest_Add(Test_CF_CFDP_R_Tick_Maintenance, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "Test_CF_CFDP_R_Tick_Maintenance");
    UtTest_Add(Test_CF_CFDP_R_Init, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_Init");
    UtTest_Add(Test_CF_CFDP_R_CheckCrc, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_CheckCrc");
    UtTest_Add(Test_CF_CFDP_R_ProcessFd, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_ProcessFd");
    UtTest_Add(Test_CF_CFDP_R_SubstateRecvEof, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_SubstateRecvEof");
    UtTest_Add(Test_CF_CFDP_R2_GapCompute, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R2_GapCompute");
    UtTest_Add(Test_CF_CFDP_R_SubstateSendNak, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_SendNak");
    UtTest_Add(Test_CF_CFDP_R_CalcCrcChunk, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_CalcCrcChunk");
    UtTest_Add(Test_CF_CFDP_R2_Recv_fin_ack, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R2_SubstateRecvFinAck");
    UtTest_Add(Test_CF_CFDP_R_CheckComplete, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_CheckComplete");
    UtTest_Add(Test_CF_CFDP_R_SubstateRecvFileData, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_SubstateRecvFileData");
    UtTest_Add(Test_CF_CFDP_R_SendNak, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_SendNak");
    UtTest_Add(Test_CF_CFDP_R_CalcCrcStart, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_CalcCrcStart");
    UtTest_Add(Test_CF_CFDP_R2_SubstateRecvFinAck, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R2_SubstateRecvFinAck");
    UtTest_Add(Test_CF_CFDP_R_SubstateRecvMd, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_SubstateRecvMd");
    UtTest_Add(Test_CF_CFDP_R_HandleFileRetention, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_HandleFileRetention");
    UtTest_Add(Test_CF_CFDP_R_CheckState, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_CheckState");
}
