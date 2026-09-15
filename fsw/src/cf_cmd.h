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
 *
 * CF command processing function declarations
 */

#ifndef CF_CMD_H
#define CF_CMD_H

#include "cfe.h"
#include "cf_app.h"
#include "cf_utils.h"

/**
 * @brief Generic action base struct
 *
 * A common context struct that is used when looping through
 * many channels/transactions/queues/directions etc.  This
 * is just basic counters.  The struct should be extended
 * for additional context info (this should appear first in extensions)
 */
typedef struct
{
    CF_Engine_t *engine;       /* The engine being operated on (currently always the same singleton) */
    int32        TotalCount;   /* total number of callbacks, including no-ops */
    int32        SuccessCount; /* count of callbacks where return status is CFE_SUCCESS */
    int32        ValidCount;   /* General purpose counter - Incremented in the callback based on selected conditions */
    int32        ErrorCount;   /* General purpose counter - Incremented in the callback based on selected conditions */
} CF_GenericAction_Context_t;

/**
 * @brief Callback function for generic actions
 *
 * This will be invoked for every channel match
 *
 * @param ga      pointer to the (base) context struct
 * @param chan    CFDP channel - passed through to function
 */
typedef CFE_Status_t (*CF_GenericAction_Func_t)(CF_GenericAction_Context_t *ga, CF_Channel_t *chan);

/************************************************************************/
/** @brief Send CF housekeeping packet
 *
 * @par Description
 *      The command to send the CF housekeeping packet
 *
 * @par Assumptions, External Events, and Notes:
 *      None
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_SendHkCmd(const CF_SendHkCmd_t *msg);

/************************************************************************/
/** @brief CF wakeup function
 *
 * @par Description
 *       Performs a single engine cycle for each wakeup
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_WakeupCmd(const CF_WakeupCmd_t *msg);

/************************************************************************/
/** @brief The no-operation command.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       This function simply prints an event message.
 *       Increments the command accept counter.
 *       The msg parameter is ignored in this one.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_NoopCmd(const CF_NoopCmd_t *msg);

/************************************************************************/
/** @brief The reset counters command.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       Resets the given counter, or all.
 *       Increments the command accept or reject counter. If the command
 *       counters are reset, then there is no increment.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_ResetCountersCmd(const CF_ResetCountersCmd_t *msg);

/************************************************************************/
/** @brief Ground command to start a file transfer.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       Increments the command accept or reject counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 *
 */
CFE_Status_t CF_TxFileCmd(const CF_TxFileCmd_t *msg);

/************************************************************************/
/** @brief Ground command to start directory playback.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       Increments the command accept or reject counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_PlaybackDirCmd(const CF_PlaybackDirCmd_t *msg);

/************************************************************************/
/** @brief Freeze a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_FreezeCmd(const CF_FreezeCmd_t *msg);

/************************************************************************/
/** @brief Thaw a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_ThawCmd(const CF_ThawCmd_t *msg);

/************************************************************************/
/** @brief Handle transaction suspend command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_SuspendCmd(const CF_SuspendCmd_t *msg);

/************************************************************************/
/** @brief Handle transaction resume command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_ResumeCmd(const CF_ResumeCmd_t *msg);

/************************************************************************/
/** @brief Handle a cancel ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_CancelCmd(const CF_CancelCmd_t *msg);

/************************************************************************/
/** @brief Handle an abandon ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_AbandonCmd(const CF_AbandonCmd_t *msg);

/************************************************************************/
/** @brief Handle an enable dequeue ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_EnableDequeueCmd(const CF_EnableDequeueCmd_t *msg);

/************************************************************************/
/** @brief Handle a disable dequeue ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_DisableDequeueCmd(const CF_DisableDequeueCmd_t *msg);

/************************************************************************/
/** @brief Enable a polling dir ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_EnableDirPollingCmd(const CF_EnableDirPollingCmd_t *msg);

/************************************************************************/
/** @brief Disable a polling dir ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_DisableDirPollingCmd(const CF_DisableDirPollingCmd_t *msg);

/************************************************************************/
/** @brief Ground command to purge either the history or pending queues.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_PurgeQueueCmd(const CF_PurgeQueueCmd_t *msg);

/************************************************************************/
/** @brief Ground command to write a file with queue information.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_WriteQueueCmd(const CF_WriteQueueCmd_t *msg);

/************************************************************************/
/** @brief Ground command to set a configuration parameter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_SetParamCmd(const CF_SetParamCmd_t *msg);

/************************************************************************/
/** @brief Ground command to get a configuration parameter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_GetParamCmd(const CF_GetParamCmd_t *msg);

/************************************************************************/
/** @brief Ground command enable engine.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_EnableEngineCmd(const CF_EnableEngineCmd_t *msg);

/************************************************************************/
/** @brief Ground command disable engine.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_DisableEngineCmd(const CF_DisableEngineCmd_t *msg);

#endif
