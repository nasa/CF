#ifndef CF_TEST_ALT_HANDLER_H
#define CF_TEST_ALT_HANDLER_H

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"
#include "utgenstub.h"

/* Alternate handlers for CF_CList_Traverse -
 * these differ in the type of context they save */
void UT_AltHandler_CF_CList_Traverse_TRAVERSE_ALL_ARGS_T(void *UserObj, UT_EntryKey_t FuncKey,
                                                         const UT_StubContext_t *Context);
void UT_AltHandler_CF_CList_Traverse_POINTER(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void UT_AltHandler_CF_CList_Traverse_R_PRIO(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

void UT_AltHandler_CF_TraverseAllTransactions_All_Channels_Set_Context(void *UserObj, UT_EntryKey_t FuncKey,
                                                                       const UT_StubContext_t *Context);

void UT_AltHandler_GenericPointerReturn(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

#endif /* CF_TEST_ALT_HANDLER_H */
