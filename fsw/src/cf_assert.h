/************************************************************************
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
 *  The CF Application CF_Assert macro
 */

#ifndef CF_ASSERT_H
#define CF_ASSERT_H

#include "cfe.h"

/*
 * CF_Assert statements within the code are primarily informational for developers,
 * as the conditions within them should always be true.  Barring any unforeseen
 * bugs in the code, they should never get triggered.  However, if the code is
 * modified, these conditions could happen, so it is still worthwhile to keep
 * these statements in the source code, so they can be enabled if necessary.
 */

#ifdef CF_DEBUG_BUILD

/*
 * Debug build:
 * Translate CF_Assert to the system assert.  Note that asserts may still get disabled
 * if building with NDEBUG flag set, even if CF_DEBUG_BUILD flag is enabled.
 */
#include <assert.h>
#define CF_Assert(x) assert(x)

#else                /* CF_DEBUG_BUILD */

/*
 * Normal build:
 * It should be impossible to get any conditions which are asserted, so it should
 * be safe to turn these off.  This is the configuration that the code should be
 * normally tested and verified in.
 */
#define CF_Assert(x) /* no-op */

#endif /* CF_DEBUG_BUILD */
#endif /* !CF_ASSERT_H */
