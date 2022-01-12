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
 *  The CF Application configuration verification header
 */

#ifndef CF_VERIFY_H
#define CF_VERIFY_H

#include "cfe.h"
#include "cf_platform_cfg.h"
#include "cf_perfids.h"

/* limit number of channels to a reasonable amount for special values for some commands */
#if CF_NUM_CHANNELS > 250
#error That's a lot of channels. I salute you, but it's too many.
#endif

#if CF_NUM_CHANNELS == 0
#error Must have at least one channel.
#endif

#if CF_NUM_HISTORIES > 65535
#error refactor code for 32 bit CF_NUM_HISTORIES
#endif

#if (CF_PERF_ID_PDURCVD(CF_NUM_CHANNELS - 1) >= CF_PERF_ID_PDUSENT(0))
#error Collision between CF_PERF_ID_PDURCVD and CF_PERF_ID_PDUSENT given number of channels
#endif

#endif /* !CF_VERIFY_H */
