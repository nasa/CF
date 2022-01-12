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
 *  The CF Application timer source file
 *
 *  A timer in CF is really just a structure that holds a counter that
 *  indicates the timer expired when it reaches 0. The goal is that
 *  any timer is driven by the scheduler ticks. There is no reason
 *  we need any finer grained resolution than this for CF.
 */

#include "cfe.h"
#include "cf_verify.h"
#include "cf_timer.h"
#include "cf_app.h"
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Function: CF_Timer_Sec2Ticks
 *
 * Application-scope internal function
 * See description in cf_timer.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CF_Timer_Sec2Ticks(CF_Timer_Seconds_t sec)
{
    return sec * CF_AppData.config_table->ticks_per_second;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Timer_InitRelSec
 *
 * Application-scope internal function
 * See description in cf_timer.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_Timer_InitRelSec(CF_Timer_t *t, uint32 rel_sec)
{
    t->tick = CF_Timer_Sec2Ticks(rel_sec);
}

/*----------------------------------------------------------------
 *
 * Function: CF_Timer_Expired
 *
 * Application-scope internal function
 * See description in cf_timer.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_Timer_Expired(const CF_Timer_t *t)
{
    return !t->tick;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Timer_Tick
 *
 * Application-scope internal function
 * See description in cf_timer.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_Timer_Tick(CF_Timer_t *t)
{
    CF_Assert(t->tick);
    --t->tick;
}
