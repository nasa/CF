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
#include "cf_eventids.h"
#include "cf_cfdp_sbintf.h"
#include "cf_cfdp_pdu.h"

static union
{
    CF_PduCmdMsg_t  cf_msg;
    CFE_SB_Buffer_t sb_buf;
    uint8           bytes[CF_MAX_PDU_SIZE];
} UT_r_msg;

static union
{
    CF_PduTlmMsg_t  cf_msg;
    CFE_SB_Buffer_t sb_buf;
    uint8           bytes[CF_MAX_PDU_SIZE];
} UT_s_msg;

/*******************************************************************************
**
**  cf_cfdp_tests local utility functions
**
*******************************************************************************/

static void UT_CFDP_SetupBasicRxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    static CF_DecoderState_t ut_decoder;
    static uint8             bytes[CF_CFDP_MAX_HEADER_SIZE];
    CFE_SB_Buffer_t *        bufptr;
    CFE_MSG_Size_t           sz;
    CFE_MSG_Type_t           msg_type = CFE_MSG_Type_Cmd;

    memset(pdu_buffer, 0, sizeof(*pdu_buffer));
    memset(bytes, 0, sizeof(bytes));

    ut_decoder.base                    = bytes;
    ut_decoder.codec_state.is_valid    = true;
    ut_decoder.codec_state.max_size    = sizeof(bytes);
    ut_decoder.codec_state.next_offset = 0;

    pdu_buffer->pdec = &ut_decoder;

    /* setup for a potential call to CFE_SB_ReceiveBuffer() */
    bufptr = &UT_r_msg.sb_buf;
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &bufptr, sizeof(bufptr), true);

    /* setup for a potential call to CFE_MSG_GetSize() */
    sz = sizeof(UT_r_msg) + CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &sz, sizeof(sz), true);

    /* setup for a potential call to CFE_MSG_GetType() */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &msg_type, sizeof(msg_type), false);
}

static void UT_CFDP_SetupBasicTxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    static CF_EncoderState_t ut_encoder;
    static uint8             bytes[CF_CFDP_MAX_HEADER_SIZE];

    CFE_SB_Buffer_t *bufptr;

    memset(pdu_buffer, 0, sizeof(*pdu_buffer));
    memset(bytes, 0, sizeof(bytes));

    ut_encoder.base                    = bytes;
    ut_encoder.codec_state.is_valid    = true;
    ut_encoder.codec_state.max_size    = sizeof(bytes);
    ut_encoder.codec_state.next_offset = 0;

    pdu_buffer->penc = &ut_encoder;

    /* setup for a potential call to CFE_SB_AllocateMessageBuffer() */
    bufptr = &UT_s_msg.sb_buf;
    UT_SetDataBuffer(UT_KEY(CFE_SB_AllocateMessageBuffer), &bufptr, sizeof(bufptr), true);
}

static void UT_CFDP_SetupBasicTestState(UT_CF_Setup_t setup, CF_Logical_PduBuffer_t **pdu_buffer_p,
                                        CF_Channel_t **channel_p, CF_History_t **history_p, CF_Transaction_t **txn_p,
                                        CF_ConfigTable_t **config_table_p)
{
    /*
     * fake objects used to pass into CF app during unit tests.
     * These are declared static so they can be returned
     */
    static CF_History_t     ut_history;
    static CF_Transaction_t ut_transaction;
    static CF_ConfigTable_t ut_config_table;

    /*
     * always clear all objects, regardless of what was asked for.
     * this helps ensure that a test does not depend on preexisting data
     * in the buffer (each test should set up its buffers in full)
     */
    memset(&ut_history, 0, sizeof(ut_history));
    memset(&ut_transaction, 0, sizeof(ut_transaction));
    memset(&ut_config_table, 0, sizeof(ut_config_table));

    /* certain pointers should be connected even if they were not asked for,
     * as internal code may assume these are set (test cases may un-set) */
    ut_transaction.history  = &ut_history;
    CF_AppData.config_table = &ut_config_table;

    if (pdu_buffer_p)
    {
        if (setup == UT_CF_Setup_TX)
        {
            *pdu_buffer_p = &CF_AppData.engine.out.tx_pdudata;
        }
        else if (setup == UT_CF_Setup_RX)
        {
            *pdu_buffer_p = &CF_AppData.engine.in.rx_pdudata;
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
        /* transmit is likely to invoke CF_CFDP_ConstructPduHeader()
            which in turn requires MsgOutGet to work */
        UT_CFDP_SetupBasicTxState(&CF_AppData.engine.out.tx_pdudata);
    }
    else if (setup == UT_CF_Setup_RX)
    {
        /* most calls on the RX side will do some sort of decode, so set up for that. */
        UT_CFDP_SetupBasicRxState(&CF_AppData.engine.in.rx_pdudata);
        ut_config_table.chan[UT_CFDP_CHANNEL].rx_max_messages_per_wakeup = 1;
    }

    UT_SetDefaultReturnValue(UT_KEY(CF_GetChannelFromTxn),
                             (UT_IntReturn_t)&CF_AppData.engine.channels[UT_CFDP_CHANNEL]);

    /* reset the event ID capture between each sub-case */
    UT_CF_ResetEventCapture();
}

/*******************************************************************************
**
**  cf_cfdp_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_tests_Setup(void)
{
    cf_tests_Setup();

    /*
     * Also clear the app global. No test case should depend on data
     * previously left in here.
     */
    memset(&CF_AppData, 0, sizeof(CF_AppData));
}

void cf_cfdp_tests_Teardown(void)
{
    cf_tests_Teardown();
}

/*******************************************************************************
**
**  Test cases
**
*******************************************************************************/

void Test_CF_CFDP_ReceiveMessage(void)
{
    /* Test case for:
     * void CF_CFDP_ReceiveMessage(CF_Channel_t *chan);
     */
    CF_Channel_t *    chan;
    CF_ConfigTable_t *config;
    CF_Transaction_t *txn;
    CFE_MSG_Type_t    msg_type;
    CFE_MSG_Size_t    msg_size_buf;

    /* no-config - the max per wakeup will be 0, and this is a noop */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, NULL, NULL);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(chan));

    /* failure in CFE_SB_ReceiveBuffer */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, NULL, &config);
    config->chan[UT_CFDP_CHANNEL].rx_max_messages_per_wakeup = 1;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_NO_MESSAGE);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(chan));

    /* Set up with a zero size input message */
    msg_size_buf = 0;
    msg_type     = CFE_MSG_Type_Tlm;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &msg_size_buf, sizeof(msg_size_buf), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &msg_type, sizeof(msg_type), false);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(chan));
    UtAssert_STUB_COUNT(CF_CFDP_ReceivePdu, 1); /* should be dispatched, this function checks size */
    UT_ResetState(UT_KEY(CFE_MSG_GetSize));
    UT_ResetState(UT_KEY(CFE_MSG_GetType));

    /*
     *  Nonzero size, Cmd framing
     */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, &chan, NULL, &txn, &config);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(chan));
    UtAssert_STUB_COUNT(CF_CFDP_ReceivePdu, 2); /* should be dispatched */
}

void Test_CF_CFDP_Send(void)
{
    /* Test case for:
     * void CF_CFDP_Send(uint8 chan_num, const CF_Logical_PduBuffer_t *ph)
     */
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, NULL, NULL);
    UtAssert_VOIDCALL(CF_CFDP_Send(UT_CFDP_CHANNEL, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].counters.sent.pdu, 1);
    UtAssert_STUB_COUNT(CFE_MSG_SetSize, 1);
    UtAssert_STUB_COUNT(CFE_SB_TransmitBuffer, 1);
}

void Test_CF_CFDP_MsgOutGet(void)
{
    /* Test case for:
        CF_Logical_PduBuffer_t *CF_CFDP_MsgOutGet(const CF_Transaction_t *txn, bool silent)
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;
    CF_Channel_t *    chan;

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(txn, false));
    UtAssert_STUB_COUNT(CFE_SB_ReleaseMessageBuffer, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* This should discard the old message, and get a new one */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(txn, false));
    UtAssert_STUB_COUNT(CFE_SB_ReleaseMessageBuffer, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* test the various throttling mechanisms */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, &config);
    config->chan[UT_CFDP_CHANNEL].max_outgoing_messages_per_wakeup = 3;
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(txn, false));
    UtAssert_NULL(CF_CFDP_MsgOutGet(txn, false));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, NULL, &txn, NULL);
    chan->sem_id = OS_ObjectIdFromInteger(123);
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(txn, false));
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemTimedWait), 1, OS_ERROR_TIMEOUT);
    UtAssert_NULL(CF_CFDP_MsgOutGet(txn, false));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* transaction is suspended */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.suspended = true;
    UtAssert_NULL(CF_CFDP_MsgOutGet(txn, false));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);

    /* channel is frozen */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].frozen = 1;
    UtAssert_NULL(CF_CFDP_MsgOutGet(txn, false));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].frozen = 0;

    /* no msg available from SB */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_NULL(CF_CFDP_MsgOutGet(txn, false));
    UT_CF_AssertEventID(CF_CFDP_NO_MSG_ERR_EID);

    /* same, but the silent flag should suppress the event */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_NULL(CF_CFDP_MsgOutGet(txn, true));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

/*******************************************************************************
**
**  cf_cfdp_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_CFDP_ReceiveMessage, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_ReceiveMessage");

    UtTest_Add(Test_CF_CFDP_MsgOutGet, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_MsgOutGet");
    UtTest_Add(Test_CF_CFDP_Send, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_Send");
}
