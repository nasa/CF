/************************************************************************
 *
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 * Copyright © 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cf_timer header
 */

#include "cf_timer.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Timer_Expired()
 * ----------------------------------------------------
 */
int CF_Timer_Expired(const CF_Timer_t *t)
{
    UT_GenStub_SetupReturnBuffer(CF_Timer_Expired, int);

    UT_GenStub_AddParam(CF_Timer_Expired, const CF_Timer_t *, t);

    UT_GenStub_Execute(CF_Timer_Expired, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Timer_Expired, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Timer_InitRelSec()
 * ----------------------------------------------------
 */
void CF_Timer_InitRelSec(CF_Timer_t *t, CF_Timer_Seconds_t rel_sec)
{
    UT_GenStub_AddParam(CF_Timer_InitRelSec, CF_Timer_t *, t);
    UT_GenStub_AddParam(CF_Timer_InitRelSec, CF_Timer_Seconds_t, rel_sec);

    UT_GenStub_Execute(CF_Timer_InitRelSec, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Timer_Sec2Ticks()
 * ----------------------------------------------------
 */
uint32 CF_Timer_Sec2Ticks(CF_Timer_Seconds_t sec)
{
    UT_GenStub_SetupReturnBuffer(CF_Timer_Sec2Ticks, uint32);

    UT_GenStub_AddParam(CF_Timer_Sec2Ticks, CF_Timer_Seconds_t, sec);

    UT_GenStub_Execute(CF_Timer_Sec2Ticks, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Timer_Sec2Ticks, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Timer_Tick()
 * ----------------------------------------------------
 */
void CF_Timer_Tick(CF_Timer_t *t)
{
    UT_GenStub_AddParam(CF_Timer_Tick, CF_Timer_t *, t);

    UT_GenStub_Execute(CF_Timer_Tick, Basic, NULL);
}
