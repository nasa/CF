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
 *   Specification for the CFS CFDP (CF) command and telemetry
 *   message data types.
 *
 * @note
 *   Constants and enumerated types related to these message structures
 *   are defined in cf_msgdefs.h.
 */
#ifndef CF_MSGSTRUCT_H
#define CF_MSGSTRUCT_H

/************************************************************************
 * Includes
 ************************************************************************/
#include "cf_msgdefs.h"
#include "cf_mission_cfg.h"
#include "cfe_msg_hdr.h"

/**
 * \defgroup cfscftlm CFS CFDP Telemetry
 * \{
 */

/**
 * \brief Housekeeping packet
 */
typedef struct CF_HkPacket
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief Telemetry header */
    CF_HkPacket_Payload_t     Payload;
} CF_HkPacket_t;

/**
 * \brief End of transaction packet
 */
typedef struct CF_EotPacket
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader; /**< \brief Telemetry header */
    CF_EotPacket_Payload_t    Payload;
} CF_EotPacket_t;

/**\}*/

/**
 * \defgroup cfscfcmdstructs CFS CFDP Command Structures
 * \{
 */

/**
 * \brief Noop command structure
 *
 * For command details see #CF_NOOP_CC
 */
typedef struct CF_NoopCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CF_NoopCmd_t;

/**
 * \brief EnableEngine command structure
 *
 * For command details see #CF_ENABLE_ENGINE_CC
 */
typedef struct CF_EnableEngineCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CF_EnableEngineCmd_t;

/**
 * \brief DisableEngine command structure
 *
 * For command details see #CF_DISABLE_ENGINE_CC
 */
typedef struct CF_DisableEngineCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CF_DisableEngineCmd_t;

/**
 * \brief Reset command structure
 *
 * For command details see #CF_RESET_CC
 */
typedef struct CF_ResetCountersCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_ResetCountersCmd_t;

/**
 * \brief Freeze command structure
 *
 * For command details see #CF_FREEZE_CC
 */
typedef struct CF_FreezeCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_FreezeCmd_t;

/**
 * \brief Thaw command structure
 *
 * For command details see #CF_THAW_CC
 */
typedef struct CF_ThawCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_ThawCmd_t;

/**
 * \brief EnableDequeue command structure
 *
 * For command details see #CF_ENABLE_DEQUEUE_CC
 */
typedef struct CF_EnableDequeueCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_EnableDequeueCmd_t;

/**
 * \brief DisableDequeue command structure
 *
 * For command details see #CF_DISABLE_DEQUEUE_CC
 */
typedef struct CF_DisableDequeueCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_DisableDequeueCmd_t;

/**
 * \brief EnableDirPolling command structure
 *
 * For command details see #CF_ENABLE_DIR_POLLING_CC
 */
typedef struct CF_EnableDirPollingCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_EnableDirPollingCmd_t;

/**
 * \brief DisableDirPolling command structure
 *
 * For command details see #CF_DISABLE_DIR_POLLING_CC
 */
typedef struct CF_DisableDirPollingCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_DisableDirPollingCmd_t;

/**
 * \brief PurgeQueue command structure
 *
 * For command details see #CF_PURGE_QUEUE_CC
 */
typedef struct CF_PurgeQueueCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_UnionArgs_Payload_t  Payload;       /**< \brief Generic command arguments */
} CF_PurgeQueueCmd_t;

/**
 * \brief Get parameter command structure
 *
 * For command details see #CF_GET_PARAM_CC
 */
typedef struct CF_GetParamCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_GetParam_Payload_t   Payload;
} CF_GetParamCmd_t;

/**
 * \brief Set parameter command structure
 *
 * For command details see #CF_SET_PARAM_CC
 */
typedef struct CF_SetParamCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_SetParam_Payload_t   Payload;
} CF_SetParamCmd_t;

/**
 * \brief Transmit file command structure
 *
 * For command details see #CF_TX_FILE_CC
 */
typedef struct CF_TxFileCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_TxFile_Payload_t     Payload;
} CF_TxFileCmd_t;

/**
 * \brief Write Queue command structure
 *
 * For command details see #CF_WRITE_QUEUE_CC
 */
typedef struct CF_WriteQueueCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_WriteQueue_Payload_t Payload;
} CF_WriteQueueCmd_t;

/**
 * \brief Playback directory command structure
 *
 * For command details see #CF_PLAYBACK_DIR_CC
 */
typedef struct CF_PlaybackDirCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
    CF_TxFile_Payload_t     Payload;
} CF_PlaybackDirCmd_t;

/**
 * \brief Suspend command structure
 *
 * For command details see #CF_SUSPEND_CC
 */
typedef struct CF_SuspendCmd
{
    CFE_MSG_CommandHeader_t  CommandHeader; /**< \brief Command header */
    CF_Transaction_Payload_t Payload;
} CF_SuspendCmd_t;

/**
 * \brief Resume command structure
 *
 * For command details see #CF_RESUME_CC
 */
typedef struct CF_ResumeCmd
{
    CFE_MSG_CommandHeader_t  CommandHeader; /**< \brief Command header */
    CF_Transaction_Payload_t Payload;
} CF_ResumeCmd_t;

/**
 * \brief Cancel command structure
 *
 * For command details see #CF_CANCEL_CC
 */
typedef struct CF_CancelCmd
{
    CFE_MSG_CommandHeader_t  CommandHeader; /**< \brief Command header */
    CF_Transaction_Payload_t Payload;
} CF_CancelCmd_t;

/**
 * \brief Abandon command structure
 *
 * For command details see #CF_ABANDON_CC
 */
typedef struct CF_AbandonCmd
{
    CFE_MSG_CommandHeader_t  CommandHeader; /**< \brief Command header */
    CF_Transaction_Payload_t Payload;
} CF_AbandonCmd_t;

/**
 * \brief Send Housekeeping Command
 *
 * Internal notification from SCH with no payload
 */
typedef struct CF_SendHkCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CF_SendHkCmd_t;

/**
 * \brief Wake Up Command
 *
 * Internal notification from SCH with no payload
 */
typedef struct CF_WakeupCmd
{
    CFE_MSG_CommandHeader_t CommandHeader; /**< \brief Command header */
} CF_WakeupCmd_t;

/**\}*/

#endif
