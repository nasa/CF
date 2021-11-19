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

void CF_HandleAssert(const char *file, int line)
{
    /*
     * NOTE: These args are out of order so that Arg[0] and Arg[1] will
     * be the same as they are for other EVS calls.  This keeps it
     * compatible with old/existing UT hook routines.
     * Newly-implemented hooks should use the name-based argument
     * retrieval so it is independent of the order.
     */
    UtAssert_Failed("CF_HandleAssert was called issue");
    // UT_Stub_RegisterContext(UT_KEY(CF_HandleAssert), file);
    // UT_Stub_RegisterContextGenericArg(UT_KEY(CF_HandleAssert), line);

    // UT_DEFAULT_IMPL(CF_HandleAssert);
    //
    // if (status >= 0)
    // {
    //     UT_Stub_CopyFromLocal(UT_KEY(CF_HandleAssert), (uint8*)&EventID, sizeof(EventID));
    // }

    // UtAssert_EndTest();
}
