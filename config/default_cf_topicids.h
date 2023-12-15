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
 *   CFDP (CF) Application Topic IDs
 */
#ifndef CF_TOPICIDS_H
#define CF_TOPICIDS_H


/*
 * The following topic IDs are for the software bus control interface
 * These are for the normal CF app commands and telemtry
 */

#define CFE_MISSION_CF_CMD_TOPICID     0xB3 /**< \brief Message ID for commands */
#define CFE_MISSION_CF_SEND_HK_TOPICID 0xB4 /**< \brief Message ID to request housekeeping telemetry */
#define CFE_MISSION_CF_WAKE_UP_TOPICID 0xB5 /**< \brief Message ID for waking up the processing cycle */
#define CFE_MISSION_CF_HK_TLM_TOPICID  0xB0 /**< \brief Message ID for housekeeping telemetry */
#define CFE_MISSION_CF_EOT_TLM_TOPICID 0xB3 /**< \brief Message ID for end of transaction telemetry */

/*
 * The following topic IDs are for the data interface (PDUs)
 * These are for the actual CF file transfer activity
 *
 * Note that the TX-direction (CF output) side is a "telemetry" topic and
 * the RX-direction (CF input) is a "command" topic.  In reality they are
 * just plain data in both directions, but they are encapsulated into a
 * CMD or TLM message to be sent across the software bus.
 */
#define CFE_MISSION_CF_CH0_TX_TOPICID 0xB4
#define CFE_MISSION_CF_CH1_TX_TOPICID 0xB5
#define CFE_MISSION_CF_CH0_RX_TOPICID 0xB6
#define CFE_MISSION_CF_CH1_RX_TOPICID 0xB7

#endif
