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
 *  The CF Application timer header file
 */

#ifndef CF_TIMER_H
#define CF_TIMER_H

#include "cfe.h"

/**
 * @brief Type for a timer tick count
 *
 * @note We expect ticks to be 100/sec, so using uint32 for sec could have a bounds condition
 * with uint32. But, we don't expect to use more than 400,000,000 seconds for any reason so
 * let's just live with it.
 */
typedef uint32 CF_Timer_Ticks_t;

/**
 * @brief Type for a timer number of seconds
 */
typedef uint32 CF_Timer_Seconds_t;

/**
 * @brief Basic CF timer object
 */
typedef struct CF_Timer
{
    CF_Timer_Ticks_t tick; /* expires when reaches 0 */
} CF_Timer_t;

/************************************************************************/
/** @brief Initialize a timer with a relative number of seconds.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t         Timer object to initialize
 * @param rel_sec   Relative number of seconds
 */
void CF_Timer_InitRelSec(CF_Timer_t *t, CF_Timer_Seconds_t rel_sec);

/************************************************************************/
/** @brief Check if a timer has expired.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t         Timer object to check
 *
 * @returns status code indicating whether timer has expired
 * @retval 1 if expired
 * @retval 0 if not expired
 */
int CF_Timer_Expired(const CF_Timer_t *t);

/************************************************************************/
/** @brief Notify a timer object a tick has occurred.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t         Timer object to tick
 */
void CF_Timer_Tick(CF_Timer_t *t);

/************************************************************************/
/** @brief Converts seconds into scheduler ticks.
 *
 * @par Assumptions, External Events, and Notes:
 *        sub-second resolution is not required
 *
 * @param sec        Number of seconds
 *
 * @returns Number of ticks for the given seconds.
 */
uint32 CF_Timer_Sec2Ticks(CF_Timer_Seconds_t sec);

#endif /* !CF_TIMER_H */
