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
/************************************************************************
 * NASA Docket No. GSC-18,920-1, and identified as “Core Flight
 * System (cFS) Health & Safety (CF) Application version 2.4.1”
 *
 * Copyright (c) 2021 United States Government as represented by the
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
 *   CFS CFDP (CF) Application Private Config Definitions
 *
 * This provides default values for configurable items that are internal
 * to this module and do NOT affect the interface(s) of this module.  Changes
 * to items in this file only affect the local module and will be transparent
 * to external entities that are using the public interface(s).
 *
 * @note This file may be overridden/superceded by mission-provided defintions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CF_INTERNAL_CFG_H
#define CF_INTERNAL_CFG_H

/**
 * \defgroup cfscfplatformcfg CFS CFDP Platform Configuration
 * \{
 */

/**
 *  @brief RX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (RX).
 *
 * CHUNKS -
 * A chunk is a representation of a range (offset, size) of data received by a receiver.
 *
 * Class 2 CFDP deals with NAK, so received data must be tracked for receivers in order to generate
 * the NAK. The sender must also keep track of NAK requests and send new file data PDUs as a result.
 * (array size must be CF_NUM_CHANNELS)
 * CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION is an array for each channel indicating the number of chunks per transaction
 * CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION is an array for each channel indicating the number of chunks to keep track
 * of NAK requests from the receiver per transaction
 *
 *  @par Limits:
 *
 */
#define CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CF_NAK_MAX_SEGMENTS, CF_NAK_MAX_SEGMENTS \
    }

/**
 *  @brief TX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (TX).
 *
 *  @par Limits:
 *
 */
#define CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CF_NAK_MAX_SEGMENTS, CF_NAK_MAX_SEGMENTS \
    }

/**
 *  @brief Application Pipe Depth
 *
 *  @par Description:
 *       Dictates the pipe depth of the cf command pipe.
 *
 *  @par Limits:
 *		 The minimum size of this parameter is 1
 *       The maximum size dictated by cFE platform configuration
 *		 parameter is OS_QUEUE_MAX_DEPTH
 */
#define CF_PIPE_DEPTH (32)

/**
 *  @brief Number of max commanded playback files per chan.
 *
 *  @par Description:
 *       This is the max number of outstanding ground commanded file transmits per channel.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN (10)

/**
 *  @brief Max number of simultaneous file receives.
 *
 *  @par Description:
 *       Each channel can support this number of file receive transactions at a time.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_SIMULTANEOUS_RX (5)

/* definitions that affect execution */

/**
 *  @brief Max number of commanded playback directories per channel.
 *
 *  @par Description:
 *       Each channel can support this number of ground commanded directory playbacks.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN (2)

/**
 *  @brief Number of histories per channel
 *
 *  @par Description:
 *       Each channel can support this number of file receive transactions at a time.
 *
 *  @par Limits:
 *       65536 is the current max.
 */
#define CF_NUM_HISTORIES_PER_CHANNEL (256)

/**
 *  @brief Number of transactions per playback directory.
 *
 *  @par Description:
 *       Each playback/polling directory operation will be able to have this
 *       many active transfers at a time pending or active.
 *
 *  @par Limits:
 *
 */
#define CF_NUM_TRANSACTIONS_PER_PLAYBACK (5)

/**
 *  @brief Name of the CF Configuration Table
 *
 *  @par Description:
 *       This parameter defines the name of the CF Configuration Table.
 *
 *  @par Limits
 *       The length of this string, including the NULL terminator cannot exceed
 *       the #OS_MAX_PATH_LEN value.
 */
#define CF_CONFIG_TABLE_NAME ("config_table")

/**
 *  @brief CF Configuration Table Filename
 *
 *  @par Description:
 *       The value of this constant defines the filename of the CF Config Table
 *
 *  @par Limits
 *       The length of this string, including the NULL terminator cannot exceed
 *       the #OS_MAX_PATH_LEN value.
 */
#define CF_CONFIG_TABLE_FILENAME ("/cf/cf_def_config.tbl")

/**
 *  @brief R2 CRC calc chunk size
 *
 *  @par Description
 *       R2 performs CRC calculation upon file completion in chunks. This is the size
 *       of the buffer. The larger the size the more stack will be used, but
 *       the faster it can go. The overall number of bytes calculated per wakeup
 *       is set in the configuration table.
 *
 *  @par Limits:
 *
 */
#define CF_R2_CRC_CHUNK_SIZE (1024)

/**
 *  @brief Number of milliseconds to wait for a SB message
 */
#define CF_RCVMSG_TIMEOUT (100)

/**
 * @brief Limits the number of retries to obtain the CF throttle sem
 *
 * @par Description
 *      If the CF throttle sem is not available during CF startup, the initialization
 *      will retry after a short delay.
 *
 * @sa CF_STARTUP_SEM_TASK_DELAY
 */
#define CF_STARTUP_SEM_MAX_RETRIES 25

/**
 * @brief Number of milliseconds to wait if CF throttle sem is not available
 *
 * @par Description
 *      If the CF throttle sem is not available during CF startup, the initialization
 *      will delay for this period of time before trying again
 *
 * @sa CF_STARTUP_SEM_MAX_RETRIES
 */
#define CF_STARTUP_SEM_TASK_DELAY 100

/**\}*/

#endif
