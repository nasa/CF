/************************************************************************
** File: CF_Assert.h
**
** NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
** Application version 3.0.0”
** Copyright © 2019 United States Government as represented by the
** Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
** Licensed under the Apache License, Version 2.0 (the "License"); you may
** not use this file except in compliance with the License. You may obtain
** a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**
** Purpose:
**  The CF Application CF_Assert macro
**
** Revision 1.0 2020/07/15 sseeger
**
*************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cf_assert header
 */

#include "cf_assert.h"
#include "utgenstub.h"

extern void UT_DefaultHandler_CF_HandleAssert(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_HandleAssert()
 * ----------------------------------------------------
 */
void CF_HandleAssert(const char *file, int line)
{
    UT_GenStub_AddParam(CF_HandleAssert, const char *, file);
    UT_GenStub_AddParam(CF_HandleAssert, int, line);

    UT_GenStub_Execute(CF_HandleAssert, Basic, UT_DefaultHandler_CF_HandleAssert);
}
