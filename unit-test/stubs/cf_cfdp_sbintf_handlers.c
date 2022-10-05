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
 *  @brief Stubs file for the CF Application main cfdp engine and pdu parsing file
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
#include "cf_cfdp_sbintf.h"
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
void UT_DefaultHandler_CF_CFDP_MsgOutGet(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_Logical_PduBuffer_t *retval;

    retval = NULL;

    UT_Stub_SetReturnValue(FuncKey, retval);
}
