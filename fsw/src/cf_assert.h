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

#ifndef CF_ASSERT__H
#define CF_ASSERT__H

#include "cfe.h"

extern void CF_HandleAssert(const char *file, int line);

/*
 * Note that in some cases, code in CF may compute or store a value for the
 * sole purpose of checking it with a CF_Assert().  If CF_Assert is then entirely
 * compiled out with NDEBUG, the compiler may see that as an unused value and
 * trigger a warning.
 *
 * To avoid this, a no-op inline function is used, such that the value in the
 * CF_Assert call is still evaluated, but the result is ignored.
 */

#ifdef NDEBUG
/* this is release mode */
static inline void CF_NoAssert(bool cond)
{
    /* no-op to avoid unused value warning */
}
#define CF_Assert(x) CF_NoAssert(x)
#else /* NDEBUG */
#include <assert.h>
#define CF_Assert(x) assert(x)
#endif /* !NDEBUG */
#endif /* !CF_ASSERT__H */
