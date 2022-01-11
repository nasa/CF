#include <stdlib.h>
#include <math.h>
#include <time.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

#include "cf_test_utils.h"
#include "cf_test_alt_handler.h"
#include "cf_utils.h"

/*----------------------------------------------------------------
 *
 * Function: UT_AltHandler_CF_CList_Traverse_TRAVERSE_ALL_ARGS_T
 *
 * A handler for CF_CList_Traverse which saves its arguments
 * including the opaque context pointer as a CF_TraverseAll_Arg_t object.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_CList_Traverse_TRAVERSE_ALL_ARGS_T(void *UserObj, UT_EntryKey_t FuncKey,
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
 * Function: UT_AltHandler_CF_CList_Traverse_POINTER
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
 * Function: UT_AltHandler_CF_CList_Traverse_R_PRIO
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
            arg->t = ctxt->context_t;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_AltHandler_CF_TraverseAllTransactions_All_Channels_Set_Context
 *
 * A handler for CF_TraverseAllTransactions which _sets_ the opaque context
 * pointer as a int* object.  The value is taken from the UserObj opaque pointer.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_CF_TraverseAllTransactions_All_Channels_Set_Context(void *UserObj, UT_EntryKey_t FuncKey,
                                                                       const UT_StubContext_t *Context)
{
    int *call_context = UT_Hook_GetArgValueByName(Context, "context", int *);
    int *req_context  = UserObj;
    int  forced_return;

    *call_context = *req_context;
    forced_return = -1;

    UT_Stub_SetReturnValue(FuncKey, forced_return);
}

/*----------------------------------------------------------------
 *
 * Function: UT_AltHandler_GenericPointerReturn
 *
 * A simple handler that can be used for any stub that returns a pointer.
 * it just forces the return value to be the object passed in as UserObj.
 *
 *-----------------------------------------------------------------*/
void UT_AltHandler_GenericPointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}
