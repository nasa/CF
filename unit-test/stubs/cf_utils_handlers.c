/************************************************************************
** File: cf_utils_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application general utility functions stubs file
**
**  Various odds and ends are put here.
**
** Revision 1.0 2020/05/01 sseeger
**  Initial revision
*************************************************************************/

#include "cf_app.h"
#include "cf_verify.h"
#include "cf_cfdp.h"
#include "cf_utils.h"
#include "cf_events.h"
#include "cf_perfids.h"

#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

#include "cf_test_utils.h"

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_ResetHistory
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_ResetHistory(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CFDP_ResetHistory_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CFDP_ResetHistory_context_t);

    if (ctxt)
    {
        ctxt->c = UT_Hook_GetArgValueByName(Context, "c", CF_Channel_t *);
        ctxt->h = UT_Hook_GetArgValueByName(Context, "h", CF_History_t *);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_FindTransactionBySequenceNumber
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_FindTransactionBySequenceNumber(void *UserObj, UT_EntryKey_t FuncKey,
                                                          const UT_StubContext_t *Context)
{
    CF_FindTransactionBySequenceNumber_context_t *ctxt =
        UT_CF_GetContextBuffer(FuncKey, CF_FindTransactionBySequenceNumber_context_t);
    CF_Transaction_t *forced_return;

    if (ctxt)
    {
        ctxt->c = UT_Hook_GetArgValueByName(Context, "c", CF_Channel_t *);
        ctxt->transaction_sequence_number =
            UT_Hook_GetArgValueByName(Context, "transaction_sequence_number", CF_TransactionSeq_t);
        ctxt->src_eid = UT_Hook_GetArgValueByName(Context, "src_eid", CF_EntityId_t);

        forced_return = ctxt->forced_return;
    }
    else
    {
        forced_return = NULL;
    }

    UT_Stub_SetReturnValue(FuncKey, forced_return);
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_FindUnusedTransaction
 *
 * Default always returns NULL, an alt handler can be registered for other pointer returns
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_FindUnusedTransaction(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_Transaction_t *forced_return;

    forced_return = NULL;

    UT_Stub_SetReturnValue(FuncKey, forced_return);
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_WriteTxnQueueDataToFile
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_WriteTxnQueueDataToFile(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_WriteTxnQueueDataToFile_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_WriteTxnQueueDataToFile_context_t);

    if (ctxt)
    {
        ctxt->fd = UT_Hook_GetArgValueByName(Context, "fd", int32);
        ctxt->c  = UT_Hook_GetArgValueByName(Context, "c", CF_Channel_t *);
        ctxt->q  = UT_Hook_GetArgValueByName(Context, "q", CF_QueueIdx_t);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_WriteHistoryQueueDataToFile
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_WriteHistoryQueueDataToFile(void *UserObj, UT_EntryKey_t FuncKey,
                                                      const UT_StubContext_t *Context)
{
    CF_WriteHistoryQueueDataToFile_context_t *ctxt =
        UT_CF_GetContextBuffer(FuncKey, CF_WriteHistoryQueueDataToFile_context_t);

    if (ctxt)
    {
        ctxt->fd  = UT_Hook_GetArgValueByName(Context, "fd", int32);
        ctxt->c   = UT_Hook_GetArgValueByName(Context, "c", CF_Channel_t *);
        ctxt->dir = UT_Hook_GetArgValueByName(Context, "dir", CF_Direction_t);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_TraverseAllTransactions
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_TraverseAllTransactions(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_TraverseAllTransactions_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_TraverseAllTransactions_context_t);

    if (ctxt)
    {
        ctxt->c       = UT_Hook_GetArgValueByName(Context, "c", CF_Channel_t *);
        ctxt->fn      = UT_Hook_GetArgValueByName(Context, "fn", CF_TraverseAllTransactions_fn_t);
        ctxt->context = UT_Hook_GetArgValueByName(Context, "context", void *);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_TraverseAllTransactions_All_Channels
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_TraverseAllTransactions_All_Channels(void *UserObj, UT_EntryKey_t FuncKey,
                                                               const UT_StubContext_t *Context)
{
    CF_TraverseAllTransactions_All_Channels_context_t *ctxt =
        UT_CF_GetContextBuffer(FuncKey, CF_TraverseAllTransactions_All_Channels_context_t);

    if (ctxt)
    {
        ctxt->fn      = UT_Hook_GetArgValueByName(Context, "fn", CF_TraverseAllTransactions_fn_t);
        ctxt->context = UT_Hook_GetArgValueByName(Context, "context", void *);

        UT_Stub_SetReturnValue(FuncKey, ctxt->forced_return);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_WrappedOpenCreate
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_WrappedOpenCreate(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_WrappedOpenCreate_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_WrappedOpenCreate_context_t);

    if (ctxt)
    {
        ctxt->fd     = UT_Hook_GetArgValueByName(Context, "fd", osal_id_t *);
        ctxt->fname  = UT_Hook_GetArgValueByName(Context, "fname", const char *);
        ctxt->flags  = UT_Hook_GetArgValueByName(Context, "flags", int32);
        ctxt->access = UT_Hook_GetArgValueByName(Context, "access", int32);

        UT_Stub_SetReturnValue(FuncKey, ctxt->forced_return);
    }
}
