/************************************************************************
** File: cf_cmd_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application command handling stubs file
**
**  All ground commands are processed in this file. All supporting functions
**  necessary to process the commands are also here.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cf_test_utils.h"
#include "cf_app.h"

#include <string.h>

/* UT includes */
#include "uttest.h"
#include "utstubs.h"

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_ProcessGroundCommand
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_ProcessGroundCommand(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_Buffer_t **ctxt = UT_CF_GetContextBuffer(FuncKey, CFE_SB_Buffer_t *);

    if (ctxt)
    {
        *ctxt = UT_Hook_GetArgValueByName(Context, "msg", CFE_SB_Buffer_t *);
    }
}
