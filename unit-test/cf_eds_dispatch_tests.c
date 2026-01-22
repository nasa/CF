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

/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_dispatch.h"
#include "cf_eds_dispatcher.h"
#include "cf_cmd.h"
#include "cf_msgids.h"
#include "cf_eventids.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

void cf_dispatch_tests_Setup(void)
{
    cf_tests_Setup();
}

void cf_dispatch_tests_Teardown(void)
{
    cf_tests_Teardown();
}

/*
**********************************************************************************
**          TEST CASE FUNCTIONS
**********************************************************************************
*/

void Test_CF_AppPipe(void)
{
    /*
     * Test Case For:
     * void CF_AppPipe
     */
    CFE_SB_Buffer_t UtBuf;

    UT_SetDeferredRetcode(UT_KEY(CFE_EDSMSG_Dispatch), 1, CFE_SUCCESS);

    memset(&UtBuf, 0, sizeof(UtBuf));
    UtAssert_VOIDCALL(CF_AppPipe(&UtBuf));
}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_AppPipe, cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown, "Test_CF_AppPipe");
}
