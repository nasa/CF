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
#ifndef CF_MSGIDS_H
#define CF_MSGIDS_H

#include "cfe_core_api_base_msgids.h"
#include "cf_topicids.h"

/**
 * \defgroup cfscfcmdmid CFS CFDP Command Message IDs
 * \{
 */

/** \brief Message ID for commands */
#define CF_CMD_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_CF_CMD_TOPICID)

/** \brief Message ID to request housekeeping telemetry */
#define CF_SEND_HK_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_CF_SEND_HK_TOPICID)

/** \brief Message ID for waking up the processing cycle */
#define CF_WAKE_UP_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_CF_WAKE_UP_TOPICID)

/**\}*/

/**
 * \defgroup cfscftlmmid CFS CFDP Telemetry Message IDs
 * \{
 */

/** \brief Message ID for housekeeping telemetry */
#define CF_HK_TLM_MID  CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_CF_HK_TLM_TOPICID)

/** \brief Message ID for end of transaction telemetry */
#define CF_EOT_TLM_MID CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_CF_EOT_TLM_TOPICID)

/**\}*/

/**
 * \defgroup cfscfdatamid CFS CFDP Data Interface Message IDs
 * \{
 */

#define CF_CH0_TX_MID CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_CF_CH0_TX_TOPICID)
#define CF_CH1_TX_MID CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_CF_CH1_TX_TOPICID)
#define CF_CH0_RX_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_CF_CH0_RX_TOPICID)
#define CF_CH1_RX_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_CF_CH1_RX_TOPICID)

/**\}*/

#endif
