/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *   CFS CFDP (CF) Application Message IDs
 */
#ifndef DEFAULT_CF_MSGID_VALUES_H
#define DEFAULT_CF_MSGID_VALUES_H

#include "cfe_core_api_base_msgids.h"
#include "cf_topicids.h"

#define CFE_PLATFORM_CF_CMD_MIDVAL(x) CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_CF_##x##_TOPICID)
#define CFE_PLATFORM_CF_TLM_MIDVAL(x) CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_CF_##x##_TOPICID)

#endif