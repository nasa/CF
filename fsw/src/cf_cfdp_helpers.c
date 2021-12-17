/************************************************************************
** File: cf_cfdp_helper.c
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
**  The CF Application CFDP helper functions
**
** Revision 1.0 2021/04/21 sseeger
**
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_cfdp_helpers.h"
#include "cf_utils.h"

#include <string.h>
#include "cf_assert.h"

uint8 CF_GetNumberMinSize(uint64 Value)
{
    uint8  MinSize;
    uint64 Limit = 0x100;

    Limit = 0x100;
    for (MinSize = 1; MinSize < 8 && Value >= Limit; ++MinSize)
    {
        Limit <<= 8;
    }

    return MinSize;
}
