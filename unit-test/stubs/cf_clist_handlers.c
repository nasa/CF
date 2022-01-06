/************************************************************************
** File: cf_clist_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application circular list definition stubs file
**
**  This is a circular doubly-linked list implementation. It is used for
**  all data structures in CF.
**
**  This file is intended to be a generic class that can be used in other apps.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cf_verify.h"
#include "cf_clist.h"
#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

#include "cf_test_utils.h"
#include "cf_cfdp.h"

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CList_InitNode
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CList_InitNode(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CListNode_t **ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CListNode_t *);

    if (ctxt)
    {
        *ctxt = UT_Hook_GetArgValueByName(Context, "node", CF_CListNode_t *);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CList_InsertBack
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CList_InsertBack(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CList_InsertBack_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_InsertBack_context_t);

    if (ctxt)
    {
        ctxt->head = UT_Hook_GetArgValueByName(Context, "head", CF_CListNode_t **);
        ctxt->node = UT_Hook_GetArgValueByName(Context, "node", CF_CListNode_t *);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CList_CF_CList_Pop
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CList_CF_CList_Pop(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CList_Pop_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_Pop_context_t);

    if (ctxt)
    {
        ctxt->head = UT_Hook_GetArgValueByName(Context, "head", CF_CListNode_t **);
        UT_Stub_SetReturnValue(FuncKey, ctxt->forced_return);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CList_Remove
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CList_Remove(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CList_Remove_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_Remove_context_t);

    if (ctxt)
    {
        ctxt->head = UT_Hook_GetArgValueByName(Context, "head", CF_CListNode_t **);
        ctxt->node = UT_Hook_GetArgValueByName(Context, "node", CF_CListNode_t *);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CList_InsertAfter
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CList_InsertAfter(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CList_InsertAfter_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_InsertAfter_context_t);

    if (ctxt)
    {
        ctxt->head  = UT_Hook_GetArgValueByName(Context, "head", CF_CListNode_t **);
        ctxt->start = UT_Hook_GetArgValueByName(Context, "start", CF_CListNode_t *);
        ctxt->after = UT_Hook_GetArgValueByName(Context, "after", CF_CListNode_t *);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CList_Traverse
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CList_Traverse(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void **ctxt = UT_CF_GetContextBuffer(FuncKey, void *);
    if (ctxt)
    {
        *ctxt = UT_Hook_GetArgValueByName(Context, "context", void *);
    }
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CList_Traverse_R
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CList_Traverse_R(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CList_Traverse_R_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CList_Traverse_R_context_t);

    if (ctxt)
    {
        ctxt->end       = UT_Hook_GetArgValueByName(Context, "end", CF_CListNode_t *);
        ctxt->fn        = UT_Hook_GetArgValueByName(Context, "fn", CF_CListFn_t);
        ctxt->context_t = UT_Hook_GetArgValueByName(Context, "context", CF_Transaction_t *);
    }
}
