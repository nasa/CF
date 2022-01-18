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
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
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

} /* end cf_cfdp_s_tests_Setup */

void cf_cfdp_s_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_s_tests_Teardown */

/* end cf_cfdp_s_tests Setup and Teardown */

/*******************************************************************************
**
**  Unit-specific test implementation
**
*******************************************************************************/

void Test_CF_CFDP_S1_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_S1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* class 1 recv is really a noop, it basically drops all packets.
       nothing to verify, just call for coverage */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S1_Recv(t, ph));
}

void Test_CF_CFDP_S2_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* class 2 recv just invokes a dispatcher to functions that should be
       tested separately.  nothing to verify here. */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Recv(t, ph));
}

void Test_CF_CFDP_S1_Tx(void)
{
    /* Test case for:
     * void CF_CFDP_S1_Tx(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S1_Tx(t));
}

void Test_CF_CFDP_S2_Tx(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Tx(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Tx(t));
}

void Test_CF_CFDP_S_Tick(void)
{
    /* Test case for:
     * void CF_CFDP_S_Tick(CF_Transaction_t *t, int *cont);
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;
    int               cont;

    cont = 0;

    /* nominal, not in CF_TxnState_S2 (noop) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));

    /* nominal, in CF_TxnState_S2, no timer expiry */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_S2;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* nominal, in CF_TxnState_S2, with timer expiry */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    t->state = CF_TxnState_S2;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_INACT_TIMER);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.inactivity_timer, 1);
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* in CF_TxnState_S2, ack_timer_armed */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state                     = CF_TxnState_S2;
    t->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 3); /* called twice! */

    /* in CF_TxnState_S2, ack_timer_armed + expiry */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    t->state                     = CF_TxnState_S2;
    t->flags.com.ack_timer_armed = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 4);

    /* in CF_TxnState_S2, ack_timer_armed + expiry + finack substate */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    config->ack_limit            = 10;
    t->state                     = CF_TxnState_S2;
    t->flags.com.ack_timer_armed = true;
    t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_SendEof, 1);

    /* same, with acklimit reached */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    config->ack_limit               = 10;
    t->state                        = CF_TxnState_S2;
    t->flags.com.ack_timer_armed    = true;
    t->state_data.s.sub_state       = CF_TxSubState_WAIT_FOR_EOF_ACK;
    t->state_data.s.s2.acknak_count = 9;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_ACK_LIMIT);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.ack_limit, 1);
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 2);

    /* same, with CF_CFDP_S_SendEof no message */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    config->ack_limit            = 10;
    t->state                     = CF_TxnState_S2;
    t->flags.com.ack_timer_armed = true;
    t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, CF_SendRet_NO_MSG);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));

    /* same, with CF_CFDP_S_SendEof Error */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 2, 1);
    config->ack_limit            = 10;
    t->state                     = CF_TxnState_S2;
    t->flags.com.ack_timer_armed = true;
    t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, CF_SendRet_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 3);

    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state                  = CF_TxnState_S2;
    t->state_data.s.sub_state = CF_TxSubState_SEND_FIN_ACK;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 4);
}

void Test_CF_CFDP_S_Tick_Nak(void)
{
    /* Test case for:
     * void CF_CFDP_S_Tick_Nak(CF_Transaction_t *t, int *cont);
     */
    CF_Transaction_t *t;
    int               cont;

    cont = 0;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(t, &cont));
    UtAssert_ZERO(cont);

    /* CF_CFDP_S_CheckAndRespondNak returns 1 */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.tx.md_need_send = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_Nak(t, &cont));
    UtAssert_NONZERO(cont);
}

void Test_CF_CFDP_S_Cancel(void)
{
    /* Test case for:
     * void CF_CFDP_S_Cancel(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Cancel(t));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_EOF);

    /* already EOF state */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state_data.s.sub_state = CF_TxSubState_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_Cancel(t));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_EOF);
}

void Test_CF_CFDP_S_SendEof(void)
{
    /* Test case for:
     * CF_SendRet_t CF_CFDP_S_SendEof(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_S_SendEof(t), CF_SendRet_SUCCESS);

    /* with CRC calc */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.com.crc_calc = true;
    UtAssert_INT32_EQ(CF_CFDP_S_SendEof(t), CF_SendRet_SUCCESS);

    /* confirm retcode from CF_CFDP_SendEof is carried through */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, CF_SendRet_NO_MSG);
    UtAssert_INT32_EQ(CF_CFDP_S_SendEof(t), CF_SendRet_NO_MSG);
}

void Test_CF_CFDP_S1_SubstateSendEof(void)
{
    /* Test case for:
     * void CF_CFDP_S1_SubstateSendEof(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal, should reset */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S1_SubstateSendEof(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* should not reset transaction if error */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendEof), 1, CF_SendRet_NO_MSG);
    UtAssert_VOIDCALL(CF_CFDP_S1_SubstateSendEof(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1); /* no increment */
}

void Test_CF_CFDP_S2_SubstateSendEof(void)
{
    /* Test case for:
     * void CF_CFDP_S2_SubstateSendEof(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal, this dequeues a transaction so q_size must be nonzero */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index] = 10;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendEof(t));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_WAIT_FOR_EOF_ACK);
    UtAssert_BOOL_TRUE(t->flags.com.ack_timer_armed);
}

void Test_CF_CFDP_S_SendFileData(void)
{
    /* Test case for:
     * int32 CF_CFDP_S_SendFileData(CF_Transaction_t *t, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc);
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;
    uint32            cumulative_read;
    uint32            read_size;
    uint32            offset;

    cumulative_read = 0;
    offset          = 0;
    read_size       = 100;

    /* failure of CF_CFDP_ConstructPduHeader */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, true), 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* nominal, smaller than chunk, no crc */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->outgoing_file_chunk_size = 150;
    t->fsize                         = 300;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, false), read_size);
    cumulative_read += read_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* nominal, larger than PDU, no crc */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE * 2;
    t->fsize                         = CF_MAX_PDU_SIZE * 2;
    read_size                        = CF_MAX_PDU_SIZE;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size * 2, false), read_size);
    cumulative_read += read_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 0);

    /* nominal, larger than chunk, with crc */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->outgoing_file_chunk_size = 50;
    read_size                        = 100;
    t->fsize                         = 300;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, config->outgoing_file_chunk_size);
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, true), config->outgoing_file_chunk_size);
    cumulative_read += config->outgoing_file_chunk_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_STUB_COUNT(CF_CRC_Digest, 1);

    /* no message available */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendFd), 1, CF_SendRet_NO_MSG);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    config->outgoing_file_chunk_size = read_size;
    t->fsize                         = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, true), 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* other send error */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendFd), 1, CF_SendRet_ERROR);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    config->outgoing_file_chunk_size = read_size;
    t->fsize                         = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, true), -1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEND_FD);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* read w/failure */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    config->outgoing_file_chunk_size = read_size;
    t->fsize                         = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, true), -1);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_read, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_READ);

    /* require lseek */
    offset = 25;
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, offset);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, read_size);
    config->outgoing_file_chunk_size = read_size;
    t->fsize                         = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, true), read_size);
    cumulative_read += read_size;
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);

    /* lseek w/failure */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, -1);
    config->outgoing_file_chunk_size = read_size;
    t->fsize                         = 300;
    UtAssert_INT32_EQ(CF_CFDP_S_SendFileData(t, offset, read_size, true), -1);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes, cumulative_read);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEEK_FD);
}

void Test_CF_CFDP_S_SubstateSendFileData(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;

    /* nominal, zero bytes processed */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(t));

    /* nominal, whole file at once */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE;
    t->state_data.s.sub_state        = CF_TxSubState_FILEDATA;
    t->fsize                         = CF_MAX_PDU_SIZE / 2;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, t->fsize);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(t));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_EOF);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* nominal, less than whole file at once */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE / 2;
    t->state_data.s.sub_state        = CF_TxSubState_FILEDATA;
    t->fsize                         = CF_MAX_PDU_SIZE;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, config->outgoing_file_chunk_size);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(t));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_FILEDATA);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* error during read */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFileData(t));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_EOF);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
}

void Test_CF_CFDP_S_CheckAndRespondNak(void)
{
    /* Test case for:
     * int CF_CFDP_S_CheckAndRespondNak(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;
    CF_ChunkWrapper_t chunks;
    CF_Chunk_t        ut_chunk;
    CF_ConfigTable_t *config;

    memset(&chunks, 0, sizeof(chunks));
    memset(&ut_chunk, 0, sizeof(ut_chunk));

    ut_chunk.offset = 0;
    ut_chunk.size   = CF_MAX_PDU_SIZE / 2;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(t), 0);

    /* with md_need_send flag set */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.tx.md_need_send = true;
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(t), 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendMd, 1);
    UtAssert_BOOL_FALSE(t->flags.tx.md_need_send);

    /* with md_need_send flag set, but failed */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.tx.md_need_send = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SendRet_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(t), -1);
    UtAssert_BOOL_TRUE(t->flags.tx.md_need_send); /* still set */

    /* with md_need_send flag set, but no message */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.tx.md_need_send = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SendRet_NO_MSG);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(t), 1);
    UtAssert_BOOL_TRUE(t->flags.tx.md_need_send); /* still set */

    /* with chunklist - this will send file data, which needs to be set up for */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE;
    t->fsize                         = ut_chunk.size;
    t->chunks                        = &chunks;
    UT_SetHandlerFunction(UT_KEY(CF_ChunkList_GetFirstChunk), UT_AltHandler_GenericPointerReturn, &ut_chunk);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, ut_chunk.size);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(t), 1);

    /* with chunklist - failure to send file data */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->outgoing_file_chunk_size = CF_MAX_PDU_SIZE;
    t->fsize                         = ut_chunk.size;
    t->chunks                        = &chunks;
    UT_SetHandlerFunction(UT_KEY(CF_ChunkList_GetFirstChunk), UT_AltHandler_GenericPointerReturn, &ut_chunk);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_INT32_EQ(CF_CFDP_S_CheckAndRespondNak(t), -1);
}

void Test_CF_CFDP_S2_SubstateSendFileData(void)
{
    /* Test case for:
     * void CF_CFDP_S2_SubstateSendFileData(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal, just invokes CF_CFDP_S_SubstateSendFileData */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendFileData(t));

    /* If CF_CFDP_S_CheckAndRespondNak returns > 0 */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.tx.md_need_send = true;
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendFileData(t));

    /* failure in CF_CFDP_S_CheckAndRespondNak, resets transaction */
    /* easiest way to trigger is via SendMd failure */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.tx.md_need_send = true;
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SendRet_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S2_SubstateSendFileData(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
}

void Test_CF_CFDP_S_SubstateSendMetadata(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateSendMetadata(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* with no setup, OS_FileOpenCheck returns SUCCESS (true) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_ALREADY_OPEN);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open, 1);

    /* file already open */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    OS_OpenCreate(&t->fd, "ut", 0, 0); /* sets fd */
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UT_CF_AssertEventID(0);
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_FILEDATA);

    /* this retval is sticky and applies for the rest of the test cases */
    UT_SetDefaultReturnValue(UT_KEY(OS_FileOpenCheck), OS_ERROR);

    /* OS_FileOpenCheck does not succeed, then WrappedOpenCreate fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_OPEN);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open, 2);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);

    /* first CF_WrappedLseek fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEEK_END);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek, 1);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);

    /* second CF_WrappedLseek fails */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 2, -1);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEEK_BEG);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek, 2);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);

    /* CF_CFDP_SendMd fails w/ ERROR */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SendRet_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_SEND_MD);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);

    /* CF_CFDP_SendMd fails w/ NO_MSG (no event here) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendMd), 1, CF_SendRet_NO_MSG);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UT_CF_AssertEventID(0);
    UtAssert_UINT32_EQ(t->history->cc, 0);

    /* everything works */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendMetadata(t));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_FILEDATA);
}

void Test_CF_CFDP_S_SubstateSendFinAck(void)
{
    /* Test case for:
     * void CF_CFDP_S_SubstateSendFinAck(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFinAck(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* CF_SendRet_NO_MSG status */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendAck), 1, CF_SendRet_NO_MSG);
    UtAssert_VOIDCALL(CF_CFDP_S_SubstateSendFinAck(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1); /* not incremented */
}

void Test_CF_CFDP_S2_EarlyFin(void)
{
    /* Test case for:
     * void CF_CFDP_S2_EarlyFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_EarlyFin(t, ph));
}

void Test_CF_CFDP_S2_Fin(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Fin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Fin(t, ph));
}

void Test_CF_CFDP_S2_Nak(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Nak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduNak_t    *nak;

    /* no segments */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(t, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_PDU_NAK);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error, 1);

    /* nominal, re-send md request (0,0) */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 1;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {0, 0};
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(t, ph));
    UtAssert_BOOL_TRUE(t->flags.tx.md_need_send);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.nak_segment_requests, 1);

    /* nominal, nonzero offsets */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 2;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {0, 200};
    nak->segment_list.segments[1]  = (CF_Logical_SegmentRequest_t) {200, 300};
    t->fsize                       = 300;
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(t, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.nak_segment_requests, 3);

    /* bad segments */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 3;
    nak->segment_list.segments[0]  = (CF_Logical_SegmentRequest_t) {200, 100};
    nak->segment_list.segments[1]  = (CF_Logical_SegmentRequest_t) {100, 400};
    nak->segment_list.segments[2]  = (CF_Logical_SegmentRequest_t) {400, 0};
    t->fsize                       = 300;
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(t, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.nak_segment_requests, 6);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_INVALID_SR);

    /* bad decode */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvNak), 1, -1);
    nak                            = &ph->int_header.nak;
    nak->segment_list.num_segments = 1;
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak(t, ph));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error, 2);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_PDU_NAK);
}

void Test_CF_CFDP_S2_Nak_Arm(void)
{
    /* Test case for:
     * void CF_CFDP_S2_Nak_Arm(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_Nak_Arm(t, ph));
}

void Test_CF_CFDP_S2_WaitForEofAck(void)
{
    /* Test case for:
     * void CF_CFDP_S2_WaitForEofAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S2_WaitForEofAck(t, ph));
    UtAssert_UINT32_EQ(t->state_data.s.sub_state, CF_TxSubState_WAIT_FOR_FIN);
    UtAssert_BOOL_FALSE(t->flags.com.ack_timer_armed);

    /* failure of CF_CFDP_RecvAck */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvAck), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_S2_WaitForEofAck(t, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_PDU_EOF);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error, 1);

    /* with condition code */
    UT_CFDP_S_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->history->cc = CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE;
    UtAssert_VOIDCALL(CF_CFDP_S2_WaitForEofAck(t, ph));
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

} /* end UtTest_Setup for cf_cfdp_s_tests.c */

/* end cf_cfdp_s_tests.c */
