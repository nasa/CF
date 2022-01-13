/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_test_alt_handler.h"
#include "cf_cfdp.h"
#include "cf_app.h"
#include "cf_events.h"

#include "cf_cfdp_r.h"
#include "cf_cfdp_s.h"
#include "cf_cfdp_dispatch.h"

static void UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_t setup, CF_Logical_PduBuffer_t **pdu_buffer_p,
                                                 CF_Channel_t **channel_p, CF_History_t **history_p,
                                                 CF_Transaction_t **txn_p, CF_ConfigTable_t **config_table_p)
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

    /* reset the event ID capture between each sub-case */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
}

/*******************************************************************************
**
**  cf_cfdp_dispatch_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_dispatch_tests_Setup(void)
{
    cf_tests_Setup();

    /* make sure global data is wiped between tests */
    memset(&CF_AppData, 0, sizeof(CF_AppData));

} /* end cf_cfdp_dispatch_tests_Setup */

void cf_cfdp_dispatch_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_dispatch_tests_Teardown */

/* end cf_cfdp_dispatch_tests Setup and Teardown */

/*******************************************************************************
**
**  Unit-specific test implementation
**
*******************************************************************************/

void Test_CF_CFDP_R_DispatchRecv(void)
{
    /* Test case for:
     * void CF_CFDP_R_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph, const
     * CF_CFDP_R_SubstateDispatchTable_t *dispatch, CF_CFDP_StateRecvFunc_t fd_fn);
     */
    CF_Transaction_t                    *t;
    CF_Logical_PduBuffer_t              *ph;
    CF_CFDP_R_SubstateDispatchTable_t    dispatch;
    CF_CFDP_FileDirectiveDispatchTable_t fddt;

    memset(&dispatch, 0, sizeof(dispatch));
    memset(&fddt, 0, sizeof(fddt));

    /* just using CF_CFDP_R[12]_Recv here because they are stubs in this context */
    fddt.fdirective[CF_CFDP_FileDirective_METADATA] = CF_CFDP_R1_Recv;
    dispatch.state[CF_RxSubState_EOF]               = &fddt;

    /* nominal (file directive) */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.pdu_type = 0;
    UtAssert_VOIDCALL(CF_CFDP_R_DispatchRecv(t, ph, &dispatch, NULL));

    /* nominal (file data) */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.pdu_type = 1;
    UtAssert_VOIDCALL(CF_CFDP_R_DispatchRecv(t, ph, &dispatch, NULL));

    /* directive code beyond range */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_INVALID_MAX;
    UtAssert_VOIDCALL(CF_CFDP_R_DispatchRecv(t, ph, &dispatch, NULL));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_R_DC_INV);

    /* file data with error */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.pdu_type = 1;
    t->history->cc          = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
    UtAssert_VOIDCALL(CF_CFDP_R_DispatchRecv(t, ph, &dispatch, NULL));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped, 1);

    /* test actual dispatch */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_METADATA;
    t->state_data.r.sub_state     = CF_RxSubState_EOF;
    UtAssert_VOIDCALL(CF_CFDP_R_DispatchRecv(t, ph, &dispatch, CF_CFDP_R2_Recv));
    UtAssert_STUB_COUNT(CF_CFDP_R1_Recv, 1);
    UtAssert_STUB_COUNT(CF_CFDP_R2_Recv, 0);

    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.pdu_type   = 1;
    t->state_data.r.sub_state = CF_RxSubState_EOF;
    UtAssert_VOIDCALL(CF_CFDP_R_DispatchRecv(t, ph, &dispatch, CF_CFDP_R2_Recv));
    UtAssert_STUB_COUNT(CF_CFDP_R1_Recv, 1);
    UtAssert_STUB_COUNT(CF_CFDP_R2_Recv, 1);
}

void Test_CF_CFDP_S_DispatchRecv(void)
{
    /* Test case for:
     * void CF_CFDP_S_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph,
                                   const CF_CFDP_S_SubstateRecvDispatchTable_t *dispatch)
     */
    CF_Transaction_t                     *t;
    CF_Logical_PduBuffer_t               *ph;
    CF_CFDP_S_SubstateRecvDispatchTable_t dispatch;
    CF_CFDP_FileDirectiveDispatchTable_t  fddt;

    memset(&dispatch, 0, sizeof(dispatch));
    memset(&fddt, 0, sizeof(fddt));

    /* just using CF_CFDP_S[12]_Recv here because they are stubs in this context */
    fddt.fdirective[CF_CFDP_FileDirective_METADATA] = CF_CFDP_S1_Recv;
    dispatch.substate[CF_TxSubState_EOF]            = &fddt;

    /* nominal, no handler */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_DispatchRecv(t, ph, &dispatch));

    /* directive code beyond range */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_INVALID_MAX;
    UtAssert_VOIDCALL(CF_CFDP_S_DispatchRecv(t, ph, &dispatch));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious, 1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_S_DC_INV);

    /* file data PDU, not expected in this type of txn */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.pdu_type = 1;
    UtAssert_VOIDCALL(CF_CFDP_S_DispatchRecv(t, ph, &dispatch));

    /* test actual dispatch */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_METADATA;
    t->state_data.s.sub_state     = CF_TxSubState_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_DispatchRecv(t, ph, &dispatch));
    UtAssert_STUB_COUNT(CF_CFDP_S1_Recv, 1);
}

void Test_CF_CFDP_S_DispatchTransmit(void)
{
    /* Test case for:
     * void CF_CFDP_S_DispatchTransmit(CF_Transaction_t *t, const CF_CFDP_S_SubstateSendDispatchTable_t *dispatch);
     */
    CF_Transaction_t                     *t;
    CF_CFDP_S_SubstateSendDispatchTable_t dispatch;

    /* The CF_CFDP_S2_Tx is just used as a convenient stub to target */
    memset(&dispatch, 0, sizeof(dispatch));
    dispatch.substate[CF_TxSubState_EOF] = CF_CFDP_S2_Tx;

    /* nominal, no handler */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_DispatchTransmit(t, &dispatch));

    /* test actual dispatch */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state_data.s.sub_state = CF_TxSubState_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_DispatchTransmit(t, &dispatch));
    UtAssert_STUB_COUNT(CF_CFDP_S2_Tx, 1);
}

void Test_CF_CFDP_TxStateDispatch(void)
{
    /* Test case for:
     * void CF_CFDP_TxStateDispatch(CF_Transaction_t *t, const CF_CFDP_TxnSendDispatchTable_t *dispatch);
     */
    CF_Transaction_t              *t;
    CF_CFDP_TxnSendDispatchTable_t dispatch;

    memset(&dispatch, 0, sizeof(dispatch));

    dispatch.tx[CF_TxnState_S1] = CF_CFDP_S1_Tx;

    /* nominal, no handler */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_TxStateDispatch(t, &dispatch));

    /* nominal, with handler */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_S1;
    UtAssert_VOIDCALL(CF_CFDP_TxStateDispatch(t, &dispatch));
    UtAssert_STUB_COUNT(CF_CFDP_S1_Tx, 1);
}

void Test_CF_CFDP_RxStateDispatch(void)
{
    /* Test case for:
     * void CF_CFDP_RxStateDispatch(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph, const
     * CF_CFDP_TxnRecvDispatchTable_t *dispatch);
     */
    CF_Transaction_t              *t;
    CF_Logical_PduBuffer_t        *ph;
    CF_CFDP_TxnRecvDispatchTable_t dispatch;

    memset(&dispatch, 0, sizeof(dispatch));

    dispatch.rx[CF_TxnState_R1] = CF_CFDP_R1_Recv;

    /* nominal, no handler */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_RxStateDispatch(t, ph, &dispatch));

    /* nominal, with handler */
    UT_CFDP_Dispatch_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_R1;
    UtAssert_VOIDCALL(CF_CFDP_RxStateDispatch(t, ph, &dispatch));
    UtAssert_STUB_COUNT(CF_CFDP_R1_Recv, 1);
}

/*******************************************************************************
**
** cf_cfdp_dispatch_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_CFDP_R_DispatchRecv, cf_cfdp_dispatch_tests_Setup, cf_cfdp_dispatch_tests_Teardown,
               "CF_CFDP_R_DispatchRecv");

    UtTest_Add(Test_CF_CFDP_S_DispatchRecv, cf_cfdp_dispatch_tests_Setup, cf_cfdp_dispatch_tests_Teardown,
               "CF_CFDP_S_DispatchRecv");
    UtTest_Add(Test_CF_CFDP_S_DispatchTransmit, cf_cfdp_dispatch_tests_Setup, cf_cfdp_dispatch_tests_Teardown,
               "CF_CFDP_S_DispatchTransmit");

    UtTest_Add(Test_CF_CFDP_TxStateDispatch, cf_cfdp_dispatch_tests_Setup, cf_cfdp_dispatch_tests_Teardown,
               "CF_CFDP_TxStateDispatch");
    UtTest_Add(Test_CF_CFDP_RxStateDispatch, cf_cfdp_dispatch_tests_Setup, cf_cfdp_dispatch_tests_Teardown,
               "CF_CFDP_RxStateDispatch");

} /* end UtTest_Setup for cf_cfdp_dispatch_tests.c */

/* end cf_cfdp_dispatch_tests.c */
