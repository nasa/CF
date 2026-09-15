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

#include <stdlib.h>
#include <math.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include "cf_test_utils.h"
#include "cf_test_alt_handler.h"
#include "cf_utils.h"

/*----------------------------------------------------------------
 *
 * A handler for CF_CList_Traverse which saves its arguments
 * including the opaque context pointer as a CF_TraverseAll_Arg_t object.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_CList_Traverse_TRAVERSE_ALL_ARGS_T(void                   *UserObj,
                                                         UT_EntryKey_t           FuncKey,
                                                         const UT_StubContext_t *Context)
{
    CF_CList_Traverse_TRAVERSE_ALL_ARGS_T_context_t *ctxt;
    CF_TraverseAll_Arg_t *arg = UT_Hook_GetArgValueByName(Context, "context", CF_TraverseAll_Arg_t *);

    if (UserObj)
    {
        ctxt = UserObj;
    }
    else
    {
        ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_Traverse_TRAVERSE_ALL_ARGS_T_context_t);
    }

    /* the counter seems to be an output */
    if (arg)
    {
        ++arg->counter;
    }

    if (ctxt)
    {
        ctxt->start = UT_Hook_GetArgValueByName(Context, "start", CF_CListNode_t *);
        ctxt->fn    = UT_Hook_GetArgValueByName(Context, "fn", CF_CListFn_t);
        if (arg)
        {
            ctxt->context_fn      = arg->fn;
            ctxt->context_counter = arg->counter;
            ctxt->context_context = arg->context;
        }
    }
}

/*----------------------------------------------------------------
 *
 * A handler for CF_CList_Traverse which saves its arguments
 * to a CF_CList_Traverse_POINTER_context_t object.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_CList_Traverse_POINTER(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CList_Traverse_POINTER_context_t *ctxt;

    if (UserObj)
    {
        ctxt = UserObj;
    }
    else
    {
        ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_Traverse_POINTER_context_t);
    }

    if (ctxt)
    {
        ctxt->start   = UT_Hook_GetArgValueByName(Context, "start", CF_CListNode_t *);
        ctxt->fn      = UT_Hook_GetArgValueByName(Context, "fn", CF_CListFn_t);
        ctxt->context = UT_Hook_GetArgValueByName(Context, "context", void *);
    }
}

/*----------------------------------------------------------------
 *
 * A handler for CF_CList_Traverse which saves its arguments
 * including the opaque context pointer as a CF_Traverse_PriorityArg_t object.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_CList_Traverse_R_PRIO(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CList_Traverse_R_context_t *ctxt;
    CF_Traverse_PriorityArg_t     *arg = UT_Hook_GetArgValueByName(Context, "context", CF_Traverse_PriorityArg_t *);

    if (UserObj)
    {
        ctxt = UserObj;
    }
    else
    {
        ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_Traverse_R_context_t);
    }

    if (ctxt)
    {
        ctxt->end = UT_Hook_GetArgValueByName(Context, "end", CF_CListNode_t *);
        ctxt->fn  = UT_Hook_GetArgValueByName(Context, "fn", CF_CListFn_t);

        /* This handler is a little different in that it sets the output to the caller */
        if (arg)
        {
            arg->txn = ctxt->context_t;
        }
    }
}

/*----------------------------------------------------------------
 *
 * A simple handler that can be used for any stub that returns a pointer.
 * it just forces the return value to be the object passed in as UserObj.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_GenericPointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

/*----------------------------------------------------------------
 *
 * Function: UT_AltHandler_CaptureTransactionStatus
 *
 * A handler for CF_CFDP_SetTxnStatus() and similar that captures the CF_TxnStatus_t
 * value to the supplied storage location.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CaptureTransactionStatus(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_TxnStatus_t *p_txn_stat = UserObj;
    CF_TxnStatus_t  in_stat    = UT_Hook_GetArgValueByName(Context, "txn_stat", CF_TxnStatus_t);

    *p_txn_stat = in_stat;
}

/*----------------------------------------------------------------
 *
 * Function: UT_AltHandler_CF_TraverseAllTransactions_InvokeCb
 *
 * A handler for CF_TraverseAllTransactions() which invokes the callback
 * with the given UserObj for the transaction
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_TraverseAllTransactions_InvokeCb(void                   *UserObj,
                                                       UT_EntryKey_t           FuncKey,
                                                       const UT_StubContext_t *Context)
{
    CF_Channel_t                   *chan    = UT_Hook_GetArgValueByName(Context, "chan", CF_Channel_t *);
    CF_TraverseAllTransactions_fn_t fn      = UT_Hook_GetArgValueByName(Context, "fn", CF_TraverseAllTransactions_fn_t);
    void                           *context = UT_Hook_GetArgValueByName(Context, "context", void *);
    CF_Transaction_t               *txn     = UserObj;
    int32                           RetVal;

    if (fn == NULL || txn == NULL)
    {
        RetVal = 0;
    }
    else
    {
        txn->chan_ptr = chan;
        fn(UserObj, context);
        RetVal = 1;
    }

    UT_Stub_SetReturnValue(FuncKey, RetVal);
}

/*----------------------------------------------------------------
 *
 * Function: UT_AltHandler_CF_FindTransactionBySequenceNumber
 *
 * Similar to the basic pointer return but ensures the chan_ptr
 * in the transaction points to a valid channel
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_FindTransactionBySequenceNumber(void                   *UserObj,
                                                      UT_EntryKey_t           FuncKey,
                                                      const UT_StubContext_t *Context)
{
    CF_Channel_t     *chan          = UT_Hook_GetArgValueByName(Context, "chan", CF_Channel_t *);
    CF_Transaction_t *forced_return = UserObj;

    if (forced_return != NULL)
    {
        forced_return->chan_ptr = chan;
    }

    UT_Stub_SetReturnValue(FuncKey, forced_return);
}

/*----------------------------------------------------------------
 *
 * Function: UT_AltHandler_CF_CList_Traverse_InvokeCb
 *
 * Similar to the basic pointer return but ensures the chan_ptr
 * in the transaction points to a valid channel
 *
 * A handler for CF_CList_Traverse() which invokes the callback
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_CList_Traverse_InvokeCb(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CListNode_t *start   = UT_Hook_GetArgValueByName(Context, "start", CF_CListNode_t *);
    CF_CListFn_t    fn      = UT_Hook_GetArgValueByName(Context, "fn", CF_CListFn_t);
    void           *context = UT_Hook_GetArgValueByName(Context, "context", void *);

    if (fn != NULL)
    {
        fn(start, context);
    }
}
