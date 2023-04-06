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
 *  @brief Stubs file for the CF Application main CFDP engine and PDU parsing file
 *
 *  This file contains two sets of functions. The first is what is needed
 *  to deal with CFDP PDUs. Specifically validating them for correctness
 *  and ensuring the byte-order is correct for the target. The second
 *  is incoming and outgoing CFDP PDUs pass through here. All receive
 *  CFDP PDU logic is performed here and the data is passed to the
 *  R (rx) and S (tx) logic.
 */

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include <string.h>
#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

#include "cf_test_utils.h"

/*----------------------------------------------------------------
 *
 * Default always returns NULL, an alt handler can be registered for other pointer returns
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_ConstructPduHeader(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_Logical_PduBuffer_t *retval;

    retval = NULL;

    UT_Stub_SetReturnValue(FuncKey, retval);
}

/*----------------------------------------------------------------
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_TxFile(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CFDP_TxFile_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CFDP_TxFile_context_t);
    const char *              ptr;

    if (ctxt)
    {
        ptr = UT_Hook_GetArgValueByName(Context, "src_filename", const char *);
        strncpy(ctxt->src_filename, ptr, sizeof(ctxt->src_filename));
        ptr = UT_Hook_GetArgValueByName(Context, "dst_filename", const char *);
        strncpy(ctxt->dst_filename, ptr, sizeof(ctxt->dst_filename));
        ctxt->cfdp_class = UT_Hook_GetArgValueByName(Context, "cfdp_class", CF_CFDP_Class_t);
        ctxt->keep       = UT_Hook_GetArgValueByName(Context, "keep", uint8);
        ctxt->chan       = UT_Hook_GetArgValueByName(Context, "chan", uint8);
        ctxt->priority   = UT_Hook_GetArgValueByName(Context, "priority", uint8);
        ctxt->dest_id    = UT_Hook_GetArgValueByName(Context, "dest_id", CF_EntityId_t);
    }
}

/*----------------------------------------------------------------
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_PlaybackDir(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CFDP_PlaybackDir_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CFDP_PlaybackDir_context_t);
    const char *                   ptr;

    if (ctxt)
    {
        ptr = UT_Hook_GetArgValueByName(Context, "src_filename", const char *);
        strncpy(ctxt->src_filename, ptr, sizeof(ctxt->src_filename));
        ptr = UT_Hook_GetArgValueByName(Context, "dst_filename", const char *);
        strncpy(ctxt->dst_filename, ptr, sizeof(ctxt->dst_filename));
        ctxt->cfdp_class = UT_Hook_GetArgValueByName(Context, "cfdp_class", CF_CFDP_Class_t);
        ctxt->keep       = UT_Hook_GetArgValueByName(Context, "keep", uint8);
        ctxt->chan       = UT_Hook_GetArgValueByName(Context, "chan", uint8);
        ctxt->priority   = UT_Hook_GetArgValueByName(Context, "priority", uint8);
        ctxt->dest_id    = UT_Hook_GetArgValueByName(Context, "dest_id", uint16);
    }
}

/*----------------------------------------------------------------
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_ResetTransaction(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CFDP_ResetTransaction_context_t *ctxt = UT_CF_GetContextBuffer(FuncKey, CF_CFDP_ResetTransaction_context_t);

    if (ctxt)
    {
        ctxt->t            = UT_Hook_GetArgValueByName(Context, "t", CF_Transaction_t *);
        ctxt->keep_history = UT_Hook_GetArgValueByName(Context, "keep_history", int);
    }
}

/*----------------------------------------------------------------
 *
 * For compatibility with other tests, this has a mechanism to save its
 * arguments to a test-provided context capture buffer.
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_CancelTransaction(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_Transaction_t **ctxt = UT_CF_GetContextBuffer(FuncKey, CF_Transaction_t *);

    if (ctxt)
    {
        *ctxt = UT_Hook_GetArgValueByName(Context, "t", CF_Transaction_t *);
    }
}
