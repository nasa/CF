/************************************************************************
** File: CF_Assert.c
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
**  The CF Application CF_Assertion macro
**
** Revision 1.0 2020/07/15 sseeger
**
*************************************************************************/

#include "cf_assert.h"
#include "cf_verify.h"
#include "cf_events.h"

void CF_HandleAssert(const char *file, int line)
{
    CFE_EVS_SendEvent(CF_EID_ERR_ASSERT, CFE_EVS_EventType_ERROR, "CF: failed CF_Assertion (%s:%d)", file, line);
    CFE_ES_ExitApp(-1);
}
