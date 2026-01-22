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
 *   Specification for the CFS CFDP (CF) table related
 *   constant definitions.
 *
 * @note
 *   These Macro definitions have been put in this file (instead of
 *   cf_tbl.h). DO NOT PUT ANY TYPEDEFS OR
 *   STRUCTURE DEFINITIONS IN THIS FILE!
 *   ADD THEM TO cf_tbl.h IF NEEDED!
 */
#ifndef DEFAULT_CF_TBLDEFS_H
#define DEFAULT_CF_TBLDEFS_H

#include "cf_mission_cfg.h"
#include "cf_extern_typedefs.h"

/**
 * \brief Configuration entry for directory polling
 */
typedef struct CF_PollDir
{
    uint32 interval_sec; /**< \brief number of seconds to wait before trying a new directory */

    uint8           priority;   /**< \brief priority to use when placing transactions on the pending queue */
    CF_CFDP_Class_t cfdp_class; /**< \brief the CFDP class to send */
    CF_EntityId_t   dest_eid;   /**< \brief destination entity id */

    char src_dir[CF_FILENAME_MAX_PATH]; /**< \brief path to source dir */
    char dst_dir[CF_FILENAME_MAX_PATH]; /**< \brief path to destination dir */

    uint8 enabled; /**< \brief Enabled flag */
} CF_PollDir_t;

/**
 * \brief Configuration entry for CFDP channel
 */
typedef struct CF_ChannelConfig
{
    uint32 max_outgoing_messages_per_wakeup; /**< \brief max number of messages to send per wakeup (0 - unlimited) */
    uint32 rx_max_messages_per_wakeup;       /**< \brief max number of rx messages to process per wakeup */

    uint32 ack_timer_s;        /**< \brief Acknowledge timer in seconds */
    uint32 nak_timer_s;        /**< \brief Non-acknowledge timer in seconds */
    uint32 inactivity_timer_s; /**< \brief Inactivity timer in seconds */

    uint8 ack_limit; /**< number of times to retry ACK (for ex, send FIN and wait for fin-ack) */
    uint8 nak_limit; /**< number of times to retry NAK before giving up (resets on a single response */

    CFE_SB_MsgId_Atom_t mid_input;  /**< \brief msgid integer value for incoming messages */
    CFE_SB_MsgId_Atom_t mid_output; /**< \brief msgid integer value for outgoing messages */

    uint16 pipe_depth_input; /**< \brief depth of pipe to receive incoming PDU */

    CF_PollDir_t polldir[CF_MAX_POLLING_DIR_PER_CHAN]; /**< \brief Configuration for polled directories */

    char  sem_name[OS_MAX_API_NAME]; /**< \brief name of throttling semaphore in TO */
    uint8 dequeue_enabled;           /**< \brief if 1, then the channel will make pending transactions active */
    char  move_dir[OS_MAX_PATH_LEN]; /**< \brief Move directory if not empty */
} CF_ChannelConfig_t;

/*
 * Previously, the entire definition of the CF table was in this file, now it is split.
 * For backward compatibility with existing CF table definitions, include the other part here.
 */
#ifndef CF_OMIT_DEPRECATED
#include "cf_tblstruct.h"
#endif

#endif
