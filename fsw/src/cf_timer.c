/************************************************************************
** File: cf_timer.c
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
**  The CF Application timer source file
**
**  A timer in CF is really just a structure that holds a counter that
**  indicates the timer expired when it reaches 0. The goal is that
**  any timer is driven by the scheduler ticks. There is no reason
**  we need any finer grained resolution than this for CF.
**
** 
** 
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_timer.h"
#include "cf_app.h"
#include "cf_assert.h"

/* NOTE: sub-second resolution is not required */

/************************************************************************/
/** \brief Converts seconds into scheduler ticks.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt Number of ticks for the given seconds. \endcode
**  \endreturns
**
*************************************************************************/
static inline uint32 CF_Timer_Sec2Ticks(cf_timer_sec_t sec)
{
    return sec*CF_AppData.config_table->ticks_per_second;
}

/************************************************************************/
/** \brief Initialize a timer with a relative number of seconds.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_Timer_InitRelSec(cf_timer_t *t, uint32 rel_sec)
{
    t->tick = CF_Timer_Sec2Ticks(rel_sec);
}

/************************************************************************/
/** \brief Check if a timer has expired.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 1 if expired; otherwise 0. \endcode
**  \endreturns
**
*************************************************************************/
int CF_Timer_Expired(const cf_timer_t *t)
{
    return !t->tick;
}

/************************************************************************/
/** \brief Notify a timer object a tick has occurred.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_Timer_Tick(cf_timer_t *t)
{
    CF_Assert(t->tick);
    --t->tick;
}
