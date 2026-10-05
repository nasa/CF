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
 * This implements a compatibility layer to allow a smoother transition
 * between command definitions.  The goal is to employ consistent patterns
 * across all CMD/TLM where resources are identified by integers of a consistent
 * range and definition.  Specifically, the integer value of "0" will be reserved
 * to indicate "all" for a given resource, or considered invalid if the command
 * cannot be applied to all resources.
 *
 * Because this shifts values for what was previously considered valid (e.g. passing
 * 0 now means all where it previously meant channel 0), the code here implements
 * a translation layer for old command definitions.  These will rewrite the command
 * to be in the new format.
 *
 * This entire source module should be considered deprecated and will be removed
 * in an upcoming release.
 */

#ifndef CF_CMD_COMPAT_H
#define CF_CMD_COMPAT_H

#include "cfe.h"
#include "cf_app.h"
#include "cf_utils.h"

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
CFE_Status_t CF_ResetCountersCompatCmd(const CF_ResetCountersCompatCmd_t *msg);

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
CFE_Status_t CF_TxFileCompatCmd(const CF_TxFileCompatCmd_t *msg);

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
CFE_Status_t CF_PlaybackDirCompatCmd(const CF_PlaybackDirCompatCmd_t *msg);

/************************************************************************/
/** @brief Freeze a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_FreezeCompatCmd(const CF_FreezeCompatCmd_t *msg);

/************************************************************************/
/** @brief Thaw a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_ThawCompatCmd(const CF_ThawCompatCmd_t *msg);

/************************************************************************/
/** @brief Handle transaction suspend command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_SuspendCompatCmd(const CF_SuspendCompatCmd_t *msg);

/************************************************************************/
/** @brief Handle transaction resume command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_ResumeCompatCmd(const CF_ResumeCompatCmd_t *msg);

/************************************************************************/
/** @brief Handle a cancel ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_CancelCompatCmd(const CF_CancelCompatCmd_t *msg);

/************************************************************************/
/** @brief Handle an abandon ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_AbandonCompatCmd(const CF_AbandonCompatCmd_t *msg);

/************************************************************************/
/** @brief Handle an enable dequeue ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_EnableDequeueCompatCmd(const CF_EnableDequeueCompatCmd_t *msg);

/************************************************************************/
/** @brief Handle a disable dequeue ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_DisableDequeueCompatCmd(const CF_DisableDequeueCompatCmd_t *msg);

/************************************************************************/
/** @brief Enable a polling dir ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_EnableDirPollingCompatCmd(const CF_EnableDirPollingCompatCmd_t *msg);

/************************************************************************/
/** @brief Disable a polling dir ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_DisableDirPollingCompatCmd(const CF_DisableDirPollingCompatCmd_t *msg);

/************************************************************************/
/** @brief Ground command to purge either the history or pending queues.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_PurgeQueueCompatCmd(const CF_PurgeQueueCompatCmd_t *msg);

/************************************************************************/
/** @brief Ground command to write a file with queue information.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_WriteQueueCompatCmd(const CF_WriteQueueCompatCmd_t *msg);

/************************************************************************/
/** @brief Ground command to set a configuration parameter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_SetParamCompatCmd(const CF_SetParamCompatCmd_t *msg);

/************************************************************************/
/** @brief Ground command to get a configuration parameter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
CFE_Status_t CF_GetParamCompatCmd(const CF_GetParamCompatCmd_t *msg);

#endif
