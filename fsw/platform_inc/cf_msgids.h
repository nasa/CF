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
 *  The CF Application Message IDs header file
 */

#ifndef CF_MSGIDS_H
#define CF_MSGIDS_H

#include "cfe_msgids.h"
#include "cfe_mission_cfg.h"

/*
 * Define a set of backup msg offsets, in case the user has not added these
 * to the global mission config header.  This also facilitates the bundle build.
 * However normally one should set a value for these in cfe_mission_cfg.h.
 */
#ifndef CFE_MISSION_CF_CMD_MSG
#define CFE_MISSION_CF_CMD_MSG 0xB3
#endif

#ifndef CFE_MISSION_CF_SEND_HK_MSG
#define CFE_MISSION_CF_SEND_HK_MSG 0xB4
#endif

#ifndef CFE_MISSION_CF_WAKE_UP_MSG
#define CFE_MISSION_CF_WAKE_UP_MSG 0xB5
#endif

#ifndef CFE_MISSION_CF_HK_TLM_MSG
#define CFE_MISSION_CF_HK_TLM_MSG 0xB0
#endif

#ifndef CFE_MISSION_CF_CONFIG_TLM_MSG
#define CFE_MISSION_CF_CONFIG_TLM_MSG 0xB2
#endif

/**
 * \defgroup cfscfcmdmid CFS CFDP Command Message IDs
 * \{
 */

#define CF_CMD_MID CFE_PLATFORM_CMD_MID_BASE + CFE_MISSION_CF_CMD_MSG /**< \brief Message ID for commands */
#define CF_SEND_HK_MID \
    CFE_PLATFORM_CMD_MID_BASE + CFE_MISSION_CF_SEND_HK_MSG /**< \brief Message ID to request housekeeping telemetry */
#define CF_WAKE_UP_MID                                                                                               \
    CFE_PLATFORM_CMD_MID_BASE + CFE_MISSION_CF_WAKE_UP_MSG /**< \brief Message ID for waking up the processing cycle \
                                                            */

/**\}*/

/**
 * \defgroup cfscftlmmid CFS CFDP Telemetry Message IDs
 * \{
 */

#define CF_HK_TLM_MID \
    CFE_PLATFORM_TLM_MID_BASE + CFE_MISSION_CF_HK_TLM_MSG /**< \brief Message ID for housekeeping telemetry */
#define CF_CONFIG_TLM_MID \
    CFE_PLATFORM_TLM_MID_BASE + CFE_MISSION_CF_CONFIG_TLM_MSG /**< \brief Message ID for configuration telemetry */

/**\}*/

#endif /* !CF_MSGIDS_H */
