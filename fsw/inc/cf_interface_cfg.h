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
 *   CFS CFDP (CF) Application Mission Configuration Header File
 *
 * @note This file may be overridden/superceded by mission-provided defintions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CF_INTERFACE_CFG_H
#define CF_INTERFACE_CFG_H

#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "cf_interface_cfg_values.h"

/**
 * \defgroup cfscfplatformcfg CFS CFDP Platform Configuration
 * \{
 */

/**
 *  @brief Number of channels
 *
 *  @par Description:
 *       The number of channels in the engine. Changing this
 *       value changes the configuration table for the application.
 *
 *  @par Limits:
 *       Must be less <= 200. Obviously it will be smaller than that.
 */
#define CF_NUM_CHANNELS         CF_INTERFACE_CFGVAL(NUM_CHANNELS)
#define DEFAULT_CF_NUM_CHANNELS 2

/**
 *  @brief Max NAK segments supported in a NAK PDU
 *
 *  @par Description:
 *       When a NAK PDU is sent or received, this is the max number of
 *       segment requests supported. This number should match the ground
 *       CFDP engine configuration as well.
 *
 *  @par Limits:
 *
 */
#define CF_NAK_MAX_SEGMENTS         CF_INTERFACE_CFGVAL(NAK_MAX_SEGMENTS)
#define DEFAULT_CF_NAK_MAX_SEGMENTS 58

/**
 *  @brief Max number of polling directories per channel.
 *
 *  @par Description:
 *       This affects the configuration table. There must be an entry (can
 *       be empty) for each of these polling directories per channel.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_POLLING_DIR_PER_CHAN         CF_INTERFACE_CFGVAL(MAX_POLLING_DIR_PER_CHAN)
#define DEFAULT_CF_MAX_POLLING_DIR_PER_CHAN 5

/**
 *  @brief Max PDU size.
 *
 *  @par Description:
 *       Limits the maximum possible Tx PDU size. Note the resulting CCSDS packet
 *       also includes a CCSDS header and CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES.
 *       The outgoing file data chunk size is also limited from the table configuration
 *       or by set parameter command, which is checked against this value
 *       (+ smallest possible PDU header).
 *
 *  @par Note:
 *       This does NOT limit Rx PDUs, since the file data is written from
 *       the transport packet to the file.
 *
 *  @par Limits:
 *       Since PDUs are wrapped in CCSDS packets, need to respect any
 *       CCSDS packet size limits on the system.
 *
 */
#define CF_MAX_PDU_SIZE         CF_INTERFACE_CFGVAL(MAX_PDU_SIZE)
#define DEFAULT_CF_MAX_PDU_SIZE 512

/**
 *  @brief Maximum file name length.
 *
 *  @par Limits:
 *
 */
#define CF_FILENAME_MAX_NAME         CF_INTERFACE_CFGVAL(FILENAME_MAX_NAME)
#define DEFAULT_CF_FILENAME_MAX_NAME CFE_MISSION_MAX_FILE_LEN

/**
 *  @brief Max filename and path length.
 *
 *  @par Limits:    
 *
 */
#define CF_FILENAME_MAX_LEN         CF_INTERFACE_CFGVAL(FILENAME_MAX_LEN)
#define DEFAULT_CF_FILENAME_MAX_LEN CFE_MISSION_MAX_PATH_LEN

/**
 * @brief Number of trailing bytes to add to CFDP PDU
 *
 * @par Description
 *      Additional padding bytes to be appended to the tail of CFDP PDUs
 *      This reserves extra space to the software bus encapsulation buffer for every
 *      CFDP PDU such that platform-specific trailer information may be added.  This
 *      includes, but is not limited to a separate CRC or error control field in addition
 *      to the error control field(s) within the the nominal CFDP protocol.
 *
 *      These extra bytes are added at the software bus encapsulation layer, they are not
 *      part of the CFDP PDU itself.
 *
 *      Set to 0 to disable this feature, such that the software bus buffer
 *      encapsulates only the CFDP PDU and no extra bytes are added.
 *
 * @par Limits:
 *       Maximum value is the difference between the maximum size of a CFDP PDU and the
 *       maximum size of an SB message.
 */
#define CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES         CF_INTERFACE_CFGVAL(PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES)
#define DEFAULT_CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES 0

/**\}*/

#endif
