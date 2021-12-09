/************************************************************************
** File: cf_timer.h
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
**  The CF Application timer header file
**
**
**
*************************************************************************/

#ifndef CF_TIMER_H
#define CF_TIMER_H

#include "cfe.h"

/* NOTE: We expect ticks to be 100/sec, so using uint32 for sec could have a bounds condition
 * with uint32. But, we don't expect to use more than 400,000,000 seconds for any reason so
 * let's just live with it. */
typedef uint32 CF_Timer_Ticks_t;
typedef uint32 CF_Timer_Seconds_t;

typedef struct CF_Timer
{
    CF_Timer_Ticks_t tick; /* expires when reaches 0 */
} CF_Timer_t;

/* initialize a timer
 *
 * If the abs_sec value is greater than current time, then the timer will
 * be immediately expired. */
extern void CF_Timer_InitRelSec(CF_Timer_t *c, CF_Timer_Seconds_t rel_sec);

extern void CF_Timer_UpdateTimebase(void);

/* returns 1 if expired */
extern int CF_Timer_Expired(const CF_Timer_t *t);

extern void CF_Timer_Tick(CF_Timer_t *t);

#endif /* !CF_TIMER_H */
