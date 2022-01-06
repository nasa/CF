/************************************************************************
** File: cf_chunk_handlers.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  Stubs file for the CF Application main cfdp engine and pdu parsing file
**
**  This file contains two sets of functions. The first is what is needed
**  to deal with CFDP PDUs. Specifically validating them for correctness
**  and ensuring the byte-order is correct for the target. The second
**  is incoming and outgoing CFDP PDUs pass through here. All receive
**  CFDP PDU logic is performed here and the data is passed to the
**  R (rx) and S (tx) logic.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cf_chunk.h"

#include <string.h>

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

#include "cf_test_utils.h"

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_ChunkList_GetFirstChunk
 *
 * Default always returns NULL, an alt handler can be registered for other pointer returns
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_ChunkList_GetFirstChunk(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_Chunk_t *c = NULL;
    UT_Stub_SetReturnValue(FuncKey, c);
}
