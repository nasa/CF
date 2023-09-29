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

/**
 * @file
 *  @brief The CF Application general utility functions stubs file
 *
 *  Various odds and ends are put here.
 */

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
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_ResetHistory(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CFDP_ResetHistory_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CFDP_ResetHistory_context_t);

    if (ctxt)
    {
        ctxt->chan    = UT_Hook_GetArgValueByName(Context, "chan", CF_Channel_t *);
        ctxt->history = UT_Hook_GetArgValueByName(Context, "history", CF_History_t *);
    }
}

/*----------------------------------------------------------------
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
        ctxt->chan = UT_Hook_GetArgValueByName(Context, "chan", CF_Channel_t *);
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
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_WriteTxnQueueDataToFile(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_WriteTxnQueueDataToFile_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_WriteTxnQueueDataToFile_context_t);

    if (ctxt)
    {
        ctxt->fd    = UT_Hook_GetArgValueByName(Context, "fd", int32);
        ctxt->chan  = UT_Hook_GetArgValueByName(Context, "chan", CF_Channel_t *);
        ctxt->queue = UT_Hook_GetArgValueByName(Context, "queue", CF_QueueIdx_t);
    }
}

/*----------------------------------------------------------------
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
        ctxt->fd   = UT_Hook_GetArgValueByName(Context, "fd", int32);
        ctxt->chan = UT_Hook_GetArgValueByName(Context, "chan", CF_Channel_t *);
        ctxt->dir  = UT_Hook_GetArgValueByName(Context, "dir", CF_Direction_t);
    }
}

/*----------------------------------------------------------------
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
        ctxt->chan    = UT_Hook_GetArgValueByName(Context, "chan", CF_Channel_t *);
        ctxt->fn      = UT_Hook_GetArgValueByName(Context, "fn", CF_TraverseAllTransactions_fn_t);
        ctxt->context = UT_Hook_GetArgValueByName(Context, "context", void *);
    }
}

/*----------------------------------------------------------------
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

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_TxnStatus_IsError
 *
 * Translate the return value to a "bool"
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_TxnStatus_IsError(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool result;

    result = (bool)Context->Int32StatusCode;

    UT_Stub_SetReturnValue(FuncKey, result);
}
