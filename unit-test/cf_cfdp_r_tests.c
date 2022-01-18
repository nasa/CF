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
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
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

} /* end cf_cfdp_r_tests_Setup */

void cf_cfdp_r_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_r_tests_Teardown */

/* end cf_cfdp_r_tests Setup and Teardown */

/*******************************************************************************
**
**  Unit-specific test implementation
**
*******************************************************************************/

void Test_CF_CFDP_R1_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_R1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);

    UtAssert_VOIDCALL(CF_CFDP_R1_Recv(t, ph));
}

void Test_CF_CFDP_R2_Recv(void)
{
    /* Test case for:
     * void CF_CFDP_R2_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);

    UtAssert_VOIDCALL(CF_CFDP_R2_Recv(t, ph));
}

void Test_CF_CFDP_R_Tick(void)
{
    /* Test case for:
     * void CF_CFDP_R_Tick(CF_Transaction_t *t, int *cont);
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;
    int               cont;

    /* nominal, not in R2 state - just ticks */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* not in R2 state, timer expired */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* nominal, in R2 state */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_R2;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 2);

    /* in R2 state, timer expired */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_R2;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_BOOL_TRUE(t->flags.rx.inactivity_fired);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_INACTIVITY_DETECTED);

    /* in R2 state, send_ack set */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state                     = CF_TxnState_R2;
    t->flags.rx.send_ack         = true;
    t->flags.rx.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_SendAck, 1);
    UtAssert_BOOL_FALSE(t->flags.rx.send_ack);

    /* same as above, but SendAck fails */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendAck), 1, CF_SendRet_NO_MSG);
    t->state                     = CF_TxnState_R2;
    t->flags.rx.send_ack         = true;
    t->flags.rx.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_BOOL_TRUE(t->flags.rx.send_ack);

    /* in R2 state, send_nak set */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state                     = CF_TxnState_R2;
    t->flags.rx.send_nak         = true;
    t->flags.rx.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_BOOL_FALSE(t->flags.rx.send_nak);

    /* same as above, but CF_CFDP_R_SubstateSendNak fails */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendNak), 1, CF_SendRet_NO_MSG);
    t->state                     = CF_TxnState_R2;
    t->flags.rx.send_nak         = true;
    t->flags.rx.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_BOOL_TRUE(t->flags.rx.send_nak);

    /* in R2 state, send_fin set */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state                     = CF_TxnState_R2;
    t->flags.rx.send_fin         = true;
    t->flags.rx.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_BOOL_FALSE(t->flags.rx.send_fin);

    /* same as above, but CF_CFDP_R2_SubstateSendFin fails */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendFin), 1, CF_SendRet_NO_MSG);
    t->state                     = CF_TxnState_R2;
    t->flags.rx.send_fin         = true;
    t->flags.rx.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);

    /* in R2 state, ack_timer_armed set */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state                     = CF_TxnState_R2;
    t->flags.com.ack_timer_armed = true;
    t->flags.rx.inactivity_fired = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_Timer_Tick, 3);

    /* in R2 state, ack_timer_armed set, timer expires */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state                     = CF_TxnState_R2;
    t->flags.com.ack_timer_armed = true;
    t->flags.rx.inactivity_fired = true;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UtAssert_BOOL_TRUE(t->flags.rx.complete);

    /* in R2 state, ack_timer_armed set, timer expires, finack substate */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->ack_limit            = 10;
    t->state                     = CF_TxnState_R2;
    t->flags.com.ack_timer_armed = true;
    t->flags.rx.inactivity_fired = true;
    t->flags.rx.complete         = true;
    t->state_data.r.sub_state    = CF_RxSubState_WAIT_FOR_FIN_ACK;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 2);
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);

    /* same as above, but acknak limit reached */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->ack_limit               = 10;
    t->state                        = CF_TxnState_R2;
    t->flags.com.ack_timer_armed    = true;
    t->flags.rx.inactivity_fired    = true;
    t->flags.rx.complete            = true;
    t->state_data.r.sub_state       = CF_RxSubState_WAIT_FOR_FIN_ACK;
    t->state_data.r.r2.acknak_count = 9;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 2);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.ack_limit, 1);

    /* in R2 state, ack_timer_armed set, timer expires, not in finack substate */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, &config);
    config->ack_limit            = 10;
    t->state                     = CF_TxnState_R2;
    t->flags.com.ack_timer_armed = true;
    t->flags.rx.inactivity_fired = true;
    t->flags.rx.complete         = true;
    t->state_data.r.sub_state    = CF_RxSubState_EOF;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R_Tick(t, &cont));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 3);
    UtAssert_BOOL_FALSE(t->flags.rx.send_fin);
}

void Test_CF_CFDP_R_Cancel(void)
{
    /* Test case for:
     * void CF_CFDP_R_Cancel(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal, calls reset */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R_Cancel(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* trigger send_fin on R2 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_R2;
    UtAssert_VOIDCALL(CF_CFDP_R_Cancel(t));
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);

    /* for coverage, this should also go to reset */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state                  = CF_TxnState_R2;
    t->state_data.r.sub_state = CF_RxSubState_WAIT_FOR_FIN_ACK;
    UtAssert_VOIDCALL(CF_CFDP_R_Cancel(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 2);
}

void Test_CF_CFDP_R_Init(void)
{
    /* Test case for:
     * void CF_CFDP_R_Init(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state_data.r.sub_state = CF_RxSubState_NUM_STATES; /* bogus; will get reset */
    UtAssert_VOIDCALL(CF_CFDP_R_Init(t));
    UtAssert_UINT32_EQ(t->state_data.r.sub_state, CF_RxSubState_FILEDATA);

    /* nominal, R2 state, no md_recv (creates tempfile) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_R2;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(t));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);
    UT_CF_AssertEventID(CF_EID_INF_CFDP_R_TEMP_FILE);

    /* nominal, R2 state, with md_recv (no tempfile) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state            = CF_TxnState_R2;
    t->flags.rx.md_recv = true;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(t));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 2);
    UT_CF_AssertEventID(0);

    /* failure of file open, class 1 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, -1);
    t->state = CF_TxnState_R1;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(t));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_CREAT);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open, 1);
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* failure of file open, class 2 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, -1);
    t->state = CF_TxnState_R2;
    UtAssert_VOIDCALL(CF_CFDP_R_Init(t));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_CREAT);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open, 2);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
}

void Test_CF_CFDP_R2_SetCc(void)
{
    /* Test case for:
     * void CF_CFDP_R2_SetCc(CF_Transaction_t *t, CF_CFDP_ConditionCode_t cc);
     */
    CF_Transaction_t *t;

    /* nominal, should save whatever cc is passed, and set "send_fin" */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE));
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE);
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);
}

void Test_CF_CFDP_R1_Reset(void)
{
    /* Test case for:
     * void CF_CFDP_R1_Reset(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal, this just resets */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R1_Reset(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);
}

void Test_CF_CFDP_R2_Reset(void)
{
    /* Test case for:
     * void CF_CFDP_R2_Reset(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal, sets "send_fin" to 1, does not reset */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R2_Reset(t));
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 0);

    /* test the various conditions that do cause reset */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state_data.r.sub_state = CF_RxSubState_WAIT_FOR_FIN_ACK;
    UtAssert_VOIDCALL(CF_CFDP_R2_Reset(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state_data.r.r2.eof_cc = CF_CFDP_ConditionCode_INVALID_TRANSMISSION_MODE; /* not NO_ERROR */
    UtAssert_VOIDCALL(CF_CFDP_R2_Reset(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 2);

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->history->cc = CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE; /* not NO_ERROR */
    UtAssert_VOIDCALL(CF_CFDP_R2_Reset(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 3);

    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->flags.com.canceled = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_Reset(t));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 4);
}

void Test_CF_CFDP_R_CheckCrc(void)
{
    /* Test case for:
     * int CF_CFDP_R_CheckCrc(CF_Transaction_t *t, uint32 expected_crc);
     */
    CF_Transaction_t *t;

    /* crc mismatch, class 1 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state      = CF_TxnState_R1;
    t->crc.result = 0xdeadbeef;
    UtAssert_INT32_EQ(CF_CFDP_R_CheckCrc(t, 0x1badc0de), 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_CRC);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.crc_mismatch, 1);

    /* crc mismatch, class 2 */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->state      = CF_TxnState_R2;
    t->crc.result = 0xdeadbeef;
    UtAssert_INT32_EQ(CF_CFDP_R_CheckCrc(t, 0x2badc0de), 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_CRC);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.crc_mismatch, 2);

    /* crc match */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->crc.result = 0xc0ffee;
    UtAssert_INT32_EQ(CF_CFDP_R_CheckCrc(t, 0xc0ffee), 0);
    UT_CF_AssertEventID(0);
}

void Test_CF_CFDP_R2_Complete(void)
{
    /* Test case for:
     * void CF_CFDP_R2_Complete(CF_Transaction_t *t, int ok_to_send_nak);
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R2_Complete(t, 0));
    UtAssert_UINT32_EQ(t->state_data.r.sub_state, CF_RxSubState_FILEDATA);

    /* test with error cc */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    t->history->cc = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
    UtAssert_VOIDCALL(CF_CFDP_R2_Complete(t, 0));

    /* nominal, send nak */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, &config);
    config->nak_limit = 2;
    UtAssert_VOIDCALL(CF_CFDP_R2_Complete(t, 1));
    UtAssert_BOOL_TRUE(t->flags.rx.send_nak);
    UtAssert_UINT32_EQ(t->state_data.r.sub_state, CF_RxSubState_FILEDATA);
    UtAssert_UINT32_EQ(t->state_data.r.r2.acknak_count, 1);

    /* same call again should trigger nak_limit */
    UtAssert_VOIDCALL(CF_CFDP_R2_Complete(t, 1));
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);
    UtAssert_BOOL_TRUE(t->flags.rx.complete);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_NAK_LIMIT);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.nak_limit, 1);

    /* test with md_recv - with no more setup this only sets filedata state */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, &config);
    config->nak_limit   = 2;
    t->flags.rx.md_recv = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_Complete(t, 0));
    UtAssert_UINT32_EQ(t->state_data.r.sub_state, CF_RxSubState_FILEDATA);

    /* with md_recv and eof_recv this should set send_fin */
    t->flags.rx.eof_recv = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_Complete(t, 1));
    UtAssert_BOOL_FALSE(t->flags.rx.send_nak);
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);
    UtAssert_BOOL_TRUE(t->flags.rx.complete);

    /* with gaps, this should send nak */
    UT_SetDeferredRetcode(UT_KEY(CF_ChunkList_ComputeGaps), 1, 1);
    UtAssert_VOIDCALL(CF_CFDP_R2_Complete(t, 1));
    UtAssert_BOOL_TRUE(t->flags.rx.send_nak);
    UtAssert_UINT32_EQ(t->state_data.r.sub_state, CF_RxSubState_FILEDATA);
    UtAssert_UINT32_EQ(t->state_data.r.r2.acknak_count, 1);
}

void Test_CF_CFDP_R_ProcessFd(void)
{
    /* Test case for:
     * int CF_CFDP_R_ProcessFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t               *t;
    CF_Logical_PduBuffer_t         *ph;
    CF_Logical_PduFileDataHeader_t *fd;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    fd           = &ph->int_header.fd;
    fd->data_len = 100;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), fd->data_len);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(t, ph), 0);
    UtAssert_UINT32_EQ(t->state_data.r.cached_pos, 100);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.file_data_bytes, 100);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 0);
    UtAssert_STUB_COUNT(CF_WrappedWrite, 1);

    /* call again, but for something at a different offset */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    fd           = &ph->int_header.fd;
    fd->data_len = 100;
    fd->offset   = 200;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), fd->offset);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(t, ph), 0);
    UtAssert_UINT32_EQ(t->state_data.r.cached_pos, 300);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.file_data_bytes, 200);
    UtAssert_STUB_COUNT(CF_WrappedLseek, 1);
    UtAssert_STUB_COUNT(CF_WrappedWrite, 2);
    UtAssert_UINT32_EQ(t->state_data.r.cached_pos, 300);

    /* call again, but with a failed write */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    fd                         = &ph->int_header.fd;
    fd->data_len               = 100;
    fd->offset                 = 300;
    t->state_data.r.cached_pos = 300;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedWrite), -1);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(t, ph), -1);
    UtAssert_UINT32_EQ(t->state_data.r.cached_pos, 300);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_WRITE);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);

    /* call again, but with a failed lseek */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    fd                         = &ph->int_header.fd;
    fd->data_len               = 100;
    fd->offset                 = 200;
    t->state_data.r.cached_pos = 300;
    UT_SetDefaultReturnValue(UT_KEY(CF_WrappedLseek), -1);
    UtAssert_INT32_EQ(CF_CFDP_R_ProcessFd(t, ph), -1);
    UtAssert_UINT32_EQ(t->state_data.r.cached_pos, 300);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_SEEK_FD);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILE_SIZE_ERROR);

    /* these stats should have been updated during the course of this test */
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].counters.fault.file_write, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].counters.fault.file_seek, 1);
}

void Test_CF_CFDP_R_SubstateRecvEof(void)
{
    /* Test case for:
     * int CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduEof_t    *eof;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateRecvEof(t, ph), 0);

    /* with md_recv and a matching size */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    eof                 = &ph->int_header.eof;
    t->flags.rx.md_recv = true;
    eof->size           = 200;
    t->fsize            = 200;
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateRecvEof(t, ph), 0);

    /* with md_recv and a different size */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    eof                 = &ph->int_header.eof;
    t->flags.rx.md_recv = true;
    eof->size           = 100;
    t->fsize            = 300;
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateRecvEof(t, ph), CF_RxEofRet_FSIZE_MISMATCH);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_SIZE_MISMATCH);

    /* with failure of CF_CFDP_RecvEof() */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_RecvEof), -1);
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateRecvEof(t, ph), CF_RxEofRet_BAD_EOF);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_PDU_EOF);

    /* these counters should have been updated during the test */
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].counters.fault.file_size_mismatch, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].counters.recv.error, 1);
}

void Test_CF_CFDP_R1_SubstateRecvEof(void)
{
    /* Test case for:
     * void CF_CFDP_R1_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduEof_t    *eof;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    eof           = &ph->int_header.eof;
    eof->crc      = 0xf007ba11;
    t->crc.result = eof->crc;
    eof->size     = 0xccc;
    t->fsize      = eof->size;
    UtAssert_VOIDCALL(CF_CFDP_R1_SubstateRecvEof(t, ph));
    UtAssert_BOOL_TRUE(t->keep);
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* failure in CF_CFDP_R_SubstateRecvEof - not a stub, but calls CF_CFDP_RecvEof, which is. */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvEof), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R1_SubstateRecvEof(t, ph));
    UtAssert_BOOL_FALSE(t->keep);

    /* failure in CF_CFDP_R_CheckCrc */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    eof           = &ph->int_header.eof;
    eof->crc      = 0xf007ba11;
    t->crc.result = ~eof->crc;
    UtAssert_VOIDCALL(CF_CFDP_R1_SubstateRecvEof(t, ph));
    UtAssert_BOOL_FALSE(t->keep);
}

void Test_CF_CFDP_R2_SubstateRecvEof(void)
{
    /* Test case for:
     * void CF_CFDP_R2_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduEof_t    *eof;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    eof           = &ph->int_header.eof;
    eof->crc      = 0xf007ba11;
    t->crc.result = eof->crc;
    eof->size     = 0xbbb;
    t->fsize      = 0xbbb;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvEof(t, ph));
    UtAssert_BOOL_TRUE(t->flags.rx.eof_recv);
    UtAssert_BOOL_TRUE(t->flags.rx.send_ack);
    UtAssert_UINT32_EQ(t->state_data.r.r2.eof_crc, eof->crc);
    UtAssert_UINT32_EQ(t->state_data.r.r2.eof_size, eof->size);
    UtAssert_UINT32_EQ(t->state_data.r.r2.eof_cc, eof->cc);

    /* non-success condition code - this resets the transaction */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    eof     = &ph->int_header.eof;
    eof->cc = CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvEof(t, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* eof already recvd - noop */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->flags.rx.eof_recv = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvEof(t, ph));
    UtAssert_BOOL_TRUE(t->flags.rx.eof_recv);         /* unchanged */
    UtAssert_BOOL_FALSE(t->flags.rx.send_ack);        /* unchanged */
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1); /* unchanged */

    /* failure in CF_CFDP_R_SubstateRecvEof - not a stub, but calls CF_CFDP_RecvEof, which is. */
    /* This will follow the CF_RxEofRet_BAD_EOF processing path, which just sets state to FILEDATA */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvEof), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvEof(t, ph));
    UtAssert_UINT32_EQ(t->state_data.r.sub_state, CF_RxSubState_FILEDATA);

    /* failure in CF_CFDP_R_SubstateRecvEof - set up for file size mismatch error */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    eof                 = &ph->int_header.eof;
    eof->size           = 0xddd;
    t->fsize            = 0xbbb;
    t->flags.rx.md_recv = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvEof(t, ph));
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILE_SIZE_ERROR);
}

void Test_CF_CFDP_R1_SubstateRecvFileData(void)
{
    /* Test case for:
     * void CF_CFDP_R1_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R1_SubstateRecvFileData(t, ph));
    UtAssert_STUB_COUNT(CF_CRC_Digest, 1);

    /* failure in CF_CFDP_RecvFd */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvFd), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R1_SubstateRecvFileData(t, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1);

    /* failure in CF_CFDP_R_ProcessFd (via failure of CF_WrappedWrite) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedWrite), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R1_SubstateRecvFileData(t, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 2);
}

void Test_CF_CFDP_R2_SubstateRecvFileData(void)
{
    /* Test case for:
     * void CF_CFDP_R2_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->state_data.r.r2.acknak_count = 1; /* make nonzero so it can be checked */
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFileData(t, ph));
    UtAssert_STUB_COUNT(CF_ChunkListAdd, 1);
    UtAssert_ZERO(t->state_data.r.r2.acknak_count); /* this resets the counter */
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 1);

    /* with fd_nak_sent flag */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->state_data.r.r2.acknak_count = 1; /* make nonzero so it can be checked */
    t->flags.rx.fd_nak_sent         = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFileData(t, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 2);
    UtAssert_ZERO(t->state_data.r.r2.acknak_count); /* this resets the counter */

    /* with rx.complete flag */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->state_data.r.r2.acknak_count = 1; /* make nonzero so it can be checked */
    t->flags.rx.complete            = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFileData(t, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ArmAckTimer, 2);    /* does NOT increment here */
    UtAssert_ZERO(t->state_data.r.r2.acknak_count); /* this resets the counter */

    /* failure in CF_CFDP_RecvFd (bad packet) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvFd), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFileData(t, ph));
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin); /* this just goes to FIN */

    /* failure in CF_CFDP_R_ProcessFd (via failure of CF_WrappedWrite) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedWrite), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_SubstateRecvFileData(t, ph));
    UtAssert_STUB_COUNT(CF_CFDP_ResetTransaction, 1); /* this resets the transaction */
}

void Test_CF_CFDP_R2_GapCompute(void)
{
    /* Test case for:
     * void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *c, void *opaque);
     */
    CF_ChunkList_t      chunks;
    CF_Chunk_t          c;
    CF_GapComputeArgs_t args;
    CF_Logical_PduNak_t nak;

    memset(&chunks, 0, sizeof(chunks));
    memset(&c, 0, sizeof(c));
    memset(&args, 0, sizeof(args));
    memset(&nak, 0, sizeof(nak));
    args.nak = &nak;

    /* nominal */
    c.offset        = 11000;
    c.size          = 100;
    nak.scope_start = 10000;
    nak.scope_end   = 20000;
    UtAssert_VOIDCALL(CF_CFDP_R2_GapCompute(&chunks, &c, &args));
    UtAssert_UINT32_EQ(nak.segment_list.num_segments, 1);

    /* the offset start/end should be normalized to the scope start/end */
    UtAssert_UINT32_EQ(nak.segment_list.segments[0].offset_start, 1000);
    UtAssert_UINT32_EQ(nak.segment_list.segments[0].offset_end, 1100);

    /* confirm that CF_PDU_MAX_SEGMENTS is not exceeded */
    nak.segment_list.num_segments = CF_PDU_MAX_SEGMENTS;
    UtAssert_VOIDCALL(CF_CFDP_R2_GapCompute(&chunks, &c, &args));
    UtAssert_UINT32_EQ(nak.segment_list.num_segments, CF_PDU_MAX_SEGMENTS);
}

void Test_CF_CFDP_R_SubstateSendNak(void)
{
    /* Test case for:
     * int CF_CFDP_R_SubstateSendNak(CF_Transaction_t *t);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_ChunkWrapper_t       chunks;

    memset(&chunks, 0, sizeof(chunks));

    /* no packet available */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateSendNak(t), -1);

    /* with md_recv flag false, this should request one by sending a blank NAK */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateSendNak(t), 0);
    UT_CF_AssertEventID(CF_EID_INF_CFDP_R_REQUEST_MD);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 1);

    /* same, but with failure of CF_CFDP_SendNak */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendNak), 1, CF_SendRet_NO_MSG);
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateSendNak(t), -1);
    UT_CF_AssertEventID(CF_EID_INF_CFDP_R_REQUEST_MD);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 2);

    /* with md_recv flag true, this should call gap compute to assemble the NAK */
    /* this requires the chunks list to be set up, and by default compute_gaps will
       return 0 (no gaps) so the transaction goes to complete */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    t->flags.rx.md_recv      = true;
    t->chunks                = &chunks;
    chunks.chunks.count      = 1;
    chunks.chunks.max_chunks = 2;
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateSendNak(t), 0);
    UtAssert_STUB_COUNT(CF_ChunkList_ComputeGaps, 1);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 2); /* did not increment */
    UtAssert_BOOL_TRUE(t->flags.rx.complete);

    /* same, but return nonzero number of gaps */
    /* this also should use the max chunks instead of count */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_ChunkList_ComputeGaps), 1, 1);
    t->flags.rx.md_recv      = true;
    t->chunks                = &chunks;
    chunks.chunks.count      = 3;
    chunks.chunks.max_chunks = 2;
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateSendNak(t), 0);
    UtAssert_STUB_COUNT(CF_CFDP_SendNak, 3);
    UtAssert_BOOL_TRUE(t->flags.rx.fd_nak_sent);

    /* same, nonzero number of gaps, but get failure in SendNak */
    /* this also should use the max chunks instead of count */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_ChunkList_ComputeGaps), 1, 1);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendNak), 1, CF_SendRet_NO_MSG);
    t->flags.rx.md_recv = true;
    t->chunks           = &chunks;
    UtAssert_INT32_EQ(CF_CFDP_R_SubstateSendNak(t), -1);
    UtAssert_BOOL_TRUE(t->flags.rx.fd_nak_sent); /* this flag is still set, even when it fails to send? */
}

void Test_CF_CFDP_R2_CalcCrcChunk(void)
{
    /* Test case for:
     * int CF_CFDP_R2_CalcCrcChunk(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;

    /* nominal with zero size file */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), 0);
    UtAssert_BOOL_TRUE(t->flags.com.crc_calc);

    /* nominal with non zero size file, runs the loop */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, &config);
    config->rx_crc_calc_bytes_per_wakeup = 100;
    t->fsize                             = 70;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, t->fsize);
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), 0);
    UtAssert_BOOL_TRUE(t->flags.com.crc_calc);

    /* force a CRC mismatch */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    t->crc.result              = 0xabadf00d;
    t->state_data.r.r2.eof_crc = 0xdeadbeef;
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), 0);
    UtAssert_BOOL_TRUE(t->flags.com.crc_calc);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE);

    /* nominal with file larger than rx_crc_calc_bytes_per_wakeup */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, &config);
    config->rx_crc_calc_bytes_per_wakeup = CF_R2_CRC_CHUNK_SIZE;
    t->fsize                             = CF_R2_CRC_CHUNK_SIZE + 100;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, CF_R2_CRC_CHUNK_SIZE);
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), -1); /*  -1 because its incomplete */
    UtAssert_BOOL_FALSE(t->flags.com.crc_calc);

    /* nominal with file size larger than CF_R2_CRC_CHUNK_SIZE (this will do 2 reads) */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, &config);
    config->rx_crc_calc_bytes_per_wakeup = CF_R2_CRC_CHUNK_SIZE * 2;
    t->fsize                             = CF_R2_CRC_CHUNK_SIZE + 100;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, CF_R2_CRC_CHUNK_SIZE);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, 100);
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), 0);
    UtAssert_BOOL_TRUE(t->flags.com.crc_calc);

    /* nominal with seek required */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, &config);
    t->state_data.r.r2.rx_crc_calc_bytes = 10;
    t->state_data.r.cached_pos           = 20;
    config->rx_crc_calc_bytes_per_wakeup = 100;
    t->fsize                             = 50;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, t->state_data.r.r2.rx_crc_calc_bytes);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, t->fsize - t->state_data.r.r2.rx_crc_calc_bytes);
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), 0);
    UtAssert_BOOL_TRUE(t->flags.com.crc_calc);

    /* failure of read */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, &config);
    config->rx_crc_calc_bytes_per_wakeup = 100;
    t->fsize                             = 50;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedRead), 1, -1);
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), -1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_READ);
    UtAssert_BOOL_FALSE(t->flags.com.crc_calc);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILE_SIZE_ERROR);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_read, 1);

    /* failure of lseek */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, &config);
    t->state_data.r.r2.rx_crc_calc_bytes = 20;
    t->state_data.r.cached_pos           = 10;
    config->rx_crc_calc_bytes_per_wakeup = 100;
    t->fsize                             = 50;
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedLseek), 1, -1);
    UtAssert_INT32_EQ(CF_CFDP_R2_CalcCrcChunk(t), -1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_SEEK_CRC);
    UtAssert_BOOL_FALSE(t->flags.com.crc_calc);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILE_SIZE_ERROR);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek, 1);
}

void Test_CF_CFDP_R2_SubstateSendFin(void)
{
    /* Test case for:
     * int CF_CFDP_R2_SubstateSendFin(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_R2_SubstateSendFin(t), 0);

    /* crc failure - can get this by having rx_crc_calc_bytes_per_wakeup less than fsize */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->fsize = 100;
    UtAssert_INT32_EQ(CF_CFDP_R2_SubstateSendFin(t), -1);

    /* failure in CF_CFDP_SendFin */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_SendFin), 1, CF_SendRet_NO_MSG);
    UtAssert_INT32_EQ(CF_CFDP_R2_SubstateSendFin(t), -1);

    /* non-success condition code */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->history->cc = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
    UtAssert_INT32_EQ(CF_CFDP_R2_SubstateSendFin(t), 0);

    /* already calculated crc */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.com.crc_calc = true;
    UtAssert_INT32_EQ(CF_CFDP_R2_SubstateSendFin(t), 0);
}

void Test_CF_CFDP_R2_Recv_fin_ack(void)
{
    /* Test case for:
     * void CF_CFDP_R2_Recv_fin_ack(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R2_Recv_fin_ack(t, ph));
    UtAssert_BOOL_TRUE(t->flags.rx.send_fin);

    /* failure in CF_CFDP_RecvAck */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvAck), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_Recv_fin_ack(t, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_PDU_FINACK);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error, 1);
}

void Test_CF_CFDP_R2_RecvMd(void)
{
    /* Test case for:
     * void CF_CFDP_R2_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R2_RecvMd(t, ph));

    /* md_recv already set */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->flags.rx.md_recv = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_RecvMd(t, ph));

    /* eof already received, file size match */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->fsize                    = 100;
    t->state_data.r.r2.eof_size = 100;
    t->flags.rx.eof_recv        = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_RecvMd(t, ph));

    /* eof already received, file size different */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->fsize                    = 100;
    t->state_data.r.r2.eof_size = 120;
    t->flags.rx.eof_recv        = true;
    UtAssert_VOIDCALL(CF_CFDP_R2_RecvMd(t, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_EOF_MD_SIZE);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILE_SIZE_ERROR);

    /* OS_rename failure */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(OS_rename), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_RecvMd(t, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_RENAME);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);

    /* reopen failure */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_WrappedOpenCreate), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_RecvMd(t, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_OPEN);
    UtAssert_UINT32_EQ(t->history->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);

    /* CF_CFDP_RecvMd failure */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_RecvMd), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_R2_RecvMd(t, ph));
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_PDU_MD);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error, 1);
}

void Test_CF_CFDP_R_SendInactivityEvent(void)
{
    /* Test case for:
     * void CF_CFDP_R_SendInactivityEvent(CF_Transaction_t *t);
     */
    CF_Transaction_t *t;

    /* nominal */
    UT_CFDP_R_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_R_SendInactivityEvent(t));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.fault.inactivity_timer, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_INACT_TIMER);
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
    UtTest_Add(Test_CF_CFDP_R_Tick, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_Tick");
    UtTest_Add(Test_CF_CFDP_R_Cancel, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_Cancel");
    UtTest_Add(Test_CF_CFDP_R_Init, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_Init");
    UtTest_Add(Test_CF_CFDP_R2_SetCc, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R2_SetCc");
    UtTest_Add(Test_CF_CFDP_R1_Reset, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R1_Reset");
    UtTest_Add(Test_CF_CFDP_R2_Reset, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R2_Reset");
    UtTest_Add(Test_CF_CFDP_R_CheckCrc, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_CheckCrc");
    UtTest_Add(Test_CF_CFDP_R2_Complete, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R2_Complete");
    UtTest_Add(Test_CF_CFDP_R_ProcessFd, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R_ProcessFd");
    UtTest_Add(Test_CF_CFDP_R_SubstateRecvEof, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_SubstateRecvEof");
    UtTest_Add(Test_CF_CFDP_R1_SubstateRecvEof, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R1_SubstateRecvEof");
    UtTest_Add(Test_CF_CFDP_R2_SubstateRecvEof, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R2_SubstateRecvEof");
    UtTest_Add(Test_CF_CFDP_R1_SubstateRecvFileData, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R1_SubstateRecvFileData");
    UtTest_Add(Test_CF_CFDP_R2_SubstateRecvFileData, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R2_SubstateRecvFileData");
    UtTest_Add(Test_CF_CFDP_R2_GapCompute, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R2_GapCompute");
    UtTest_Add(Test_CF_CFDP_R_SubstateSendNak, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_SubstateSendNak");
    UtTest_Add(Test_CF_CFDP_R2_CalcCrcChunk, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R2_CalcCrcChunk");
    UtTest_Add(Test_CF_CFDP_R2_SubstateSendFin, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R2_SubstateSendFin");
    UtTest_Add(Test_CF_CFDP_R2_Recv_fin_ack, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R2_Recv_fin_ack");
    UtTest_Add(Test_CF_CFDP_R2_RecvMd, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown, "CF_CFDP_R2_RecvMd");
    UtTest_Add(Test_CF_CFDP_R_SendInactivityEvent, cf_cfdp_r_tests_Setup, cf_cfdp_r_tests_Teardown,
               "CF_CFDP_R_SendInactivityEvent");

} /* end UtTest_Setup for cf_cfdp_r_tests.c */

/* end cf_cfdp_r_tests.c */
