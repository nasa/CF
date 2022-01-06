/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_test_alt_handler.h"
#include "cf_events.h"
#include "cf_cfdp_sbintf.h"
#include "cf_cfdp_pdu.h"

static union
{
    CF_PduRecvMsg_t cf_msg;
    CFE_SB_Buffer_t sb_buf;
    uint8           bytes[CF_MAX_PDU_SIZE];
} UT_r_msg;

static union
{
    CF_PduRecvMsg_t cf_msg;
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
    CFE_SB_Buffer_t         *bufptr;
    CFE_MSG_Size_t           sz;

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
    sz = sizeof(UT_r_msg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &sz, sizeof(sz), true);
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

    /* reset the event ID capture between each sub-case */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
}

/* end cf_cfdp_tests local utility functions */

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

} /* end cf_cfdp_tests_Setup */

void cf_cfdp_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_tests_Teardown */

/*******************************************************************************
**
**  Test cases
**
*******************************************************************************/

void Test_CF_CFDP_ReceiveMessage(void)
{
    /* Test case for:
     * void CF_CFDP_ReceiveMessage(CF_Channel_t *c);
     */
    CF_Channel_t           *c;
    CF_ConfigTable_t       *config;
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* no-config - the max per wakeup will be 0, and this is a noop */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &c, NULL, NULL, NULL);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));

    /* failure in CFE_SB_ReceiveBuffer */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &c, NULL, NULL, &config);
    config->chan[UT_CFDP_CHANNEL].rx_max_messages_per_wakeup = 1;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_NO_MESSAGE);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));

    /*
     *  - CF_CFDP_RecvPh() succeeds
     *  - CF_FindTransactionBySequenceNumber() returns NULL
     *  - CF_CFDP_FindUnusedTransaction() needs to return non-NULL
     */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, &c, NULL, &t, &config);
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, t);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));
    UtAssert_STUB_COUNT(CF_CFDP_DispatchRecv, 1); /* should be dispatched */
    UtAssert_UINT32_EQ(t->history->dir, CF_Direction_RX);
    UtAssert_UINT32_EQ(t->state_data.r.r2.dc, CF_CFDP_FinDeliveryCode_INCOMPLETE);
    UtAssert_UINT32_EQ(t->state_data.r.r2.fs, CF_CFDP_FinFileStatus_DISCARDED);

    /* failure in CF_CFDP_RecvPh - nothing really happens here */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, &c, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvPh), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));

    /*
     *  - CF_CFDP_RecvPh() succeeds
     *  - CF_FindTransactionBySequenceNumber() returns non-NULL
     */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, &c, NULL, &t, &config);
    t->state = CF_TxnState_R2;
    UT_SetHandlerFunction(UT_KEY(CF_FindTransactionBySequenceNumber), UT_AltHandler_GenericPointerReturn, t);
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));
    UtAssert_STUB_COUNT(CF_CFDP_DispatchRecv, 2);              /* should be dispatched */
    UT_ResetState(UT_KEY(CF_FindTransactionBySequenceNumber)); /* clears it */

    /* FIN handling special case */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &c, NULL, &t, &config);
    config->local_eid             = 123;
    ph->pdu_header.source_eid     = config->local_eid;
    ph->fdirective.directive_code = CF_CFDP_FileDirective_FIN;
    c->cur                        = t;
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_NULL(c->cur); /* cleared */

    /* FIN handling special case, but failure of CF_CFDP_RecvFin */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &c, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvFin), 1, -1);
    config->local_eid             = 123;
    ph->pdu_header.source_eid     = config->local_eid;
    ph->fdirective.directive_code = CF_CFDP_FileDirective_FIN;
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious, 1); /* no increment */
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);                                             /* no increment */
    UtAssert_NULL(c->cur);                                                               /* cleared */

    /* FIN handling special case, but failure of CF_CFDP_SendAck */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &c, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendAck), 1, CF_SendRet_NO_MSG);
    config->local_eid             = 123;
    ph->pdu_header.source_eid     = config->local_eid;
    ph->fdirective.directive_code = CF_CFDP_FileDirective_FIN;
    c->cur                        = t;
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious, 2); /* this does get increment */
    UtAssert_ADDRESS_EQ(c->cur, t);                                                      /* not changed */

    /* recv but not the correct destination_eid */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &c, NULL, &t, &config);
    config->local_eid              = 123;
    ph->pdu_header.destination_eid = ~config->local_eid;
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_INVALID_DST_EID);

    /* recv correct destination_eid but CF_MAX_SIMULTANEOUS_RX hit */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &c, NULL, &t, &config);
    CF_AppData.hk.channel_hk[t->chan_num].q_size[CF_QueueIdx_RX] = CF_MAX_SIMULTANEOUS_RX;
    config->local_eid                                            = 123;
    ph->pdu_header.destination_eid                               = config->local_eid;
    UtAssert_VOIDCALL(CF_CFDP_ReceiveMessage(c));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_RX_DROPPED);
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
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].counters.sent.pdu, 1);
    UtAssert_STUB_COUNT(CFE_MSG_SetSize, 1);
    UtAssert_STUB_COUNT(CFE_SB_TransmitBuffer, 1);
}

void Test_CF_CFDP_MsgOutGet(void)
{
    /* Test case for:
        CF_Logical_PduBuffer_t *CF_CFDP_MsgOutGet(const CF_Transaction_t *t, bool silent)
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;
    CF_Channel_t     *c;

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(t, false));
    UtAssert_STUB_COUNT(CFE_SB_ReleaseMessageBuffer, 0);
    UT_CF_AssertEventID(0);

    /* This should discard the old message, and get a new one */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(t, false));
    UtAssert_STUB_COUNT(CFE_SB_ReleaseMessageBuffer, 1);
    UT_CF_AssertEventID(0);

    /* test the various throttling mechanisms */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->chan[UT_CFDP_CHANNEL].max_outgoing_messages_per_wakeup = 3;
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(t, false));
    UtAssert_NULL(CF_CFDP_MsgOutGet(t, false));
    UT_CF_AssertEventID(0);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, NULL, &t, NULL);
    c->sem_id = OS_ObjectIdFromInteger(123);
    UtAssert_NOT_NULL(CF_CFDP_MsgOutGet(t, false));
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), OS_ERROR_TIMEOUT);
    UtAssert_NULL(CF_CFDP_MsgOutGet(t, false));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_NO_MSG);

    /* transaction is suspended */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.com.suspended = 1;
    UtAssert_NULL(CF_CFDP_MsgOutGet(t, false));
    UT_CF_AssertEventID(0);

    /* channel is frozen */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].frozen = 1;
    UtAssert_NULL(CF_CFDP_MsgOutGet(t, false));
    UT_CF_AssertEventID(0);
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].frozen = 0;

    /* no msg available from SB */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_NULL(CF_CFDP_MsgOutGet(t, false));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_NO_MSG);

    /* same, but the silent flag should supress the event */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_NULL(CF_CFDP_MsgOutGet(t, true));
    UT_CF_AssertEventID(0);
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

} /* end UtTest_Setup for cf_cfdp_tests.c */

/* end cf_cfdp_tests.c */
