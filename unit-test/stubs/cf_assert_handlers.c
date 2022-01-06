/************************************************************************
** File: cf_assert_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  File contains stubs for the CF Application CF_Assertion macro file
**
** Revision 1.0 2020/08/04 agibso1
**  Initial revision
*************************************************************************/

#include "cf_assert.h"
#include "cf_verify.h"
#include "cf_events.h"

/* UT includes */
#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_HandleAssert
 *
 * This registers a failure in logs.  Should not normally be invoked.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_HandleAssert(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UtAssert_Failed("CF_HandleAssert was called issue");
}
