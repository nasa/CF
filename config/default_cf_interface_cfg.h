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
 *   CFS CFDP (CF) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 *
 * @note This file may be overridden/superceded by mission-provided defintions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CF_INTERFACE_CFG_H
#define CF_INTERFACE_CFG_H

#include "common_types.h"
#include "cfe_mission_cfg.h"

/**
 * \defgroup cfscfplatformcfg CFS CFDP Platform Configuration
 * \{
 */

/**
 * @brief Entity id size
 *
 * @par Description:
 *      The maximum size of the entity id as expected for all CFDP packets.
 *      CF supports the spec's variable size of EID, where the actual size is
 *      selected at runtime, and therefore the size in CFDP PDUs may be smaller
 *      than the size specified here.  This type only establishes the maximum
 *      size (and therefore maximum value) that an EID may be.
 *
 * @note This type is used in several CF commands, and so changing the size
 *       of this type will affect the following structs:
 *        CF_ConfigTable_t, configuration table - will change size of file
 *        CF_ConfigPacket_t, set config params command
 *        CF_TxFileCmd_t, transmit file command
 *        CF_PlaybackDirCmd_t, equivalent to above
 *        CF_Transaction_Payload_t, any command that selects a transaction based on EID
 *
 * @par Limits
 *         Must be one of uint8, uint16, uint32, uint64.
 */
typedef uint32 CF_EntityId_t;

/**
 * @brief transaction sequence number size
 *
 * @par Description:
 *      The max size of the transaction sequence number as expected for all CFDP packets.
 *      CF supports the spec's variable size of TSN, where the actual size is
 *      selected at runtime, and therefore the size in CFDP PDUs may be smaller
 *      than the size specified here.  This type only establishes the maximum
 *      size (and therefore maximum value) that a TSN may be.
 *
 * @note This type is used in several CF commands, and so changing the size
 *       of this type will affect the following structure:
 *        CF_Transaction_Payload_t, any command that selects a transaction based on TSN
 *
 * @par Limits
 *         Must be one of uint8, uint16, uint32, uint64.
 */
typedef uint32 CF_TransactionSeq_t;

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
#define CF_NUM_CHANNELS (2)

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
#define CF_NAK_MAX_SEGMENTS (58)

/**
 *  @brief Total number of chunks (tx, rx, all channels)
 *
 *  @par Description:
 *       Must be equal to the sum of all values input in CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION
 *       and CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION.
 *
 *  @par Limits:
 *
 */
/* CF_TOTAL_CHUNKS must be equal to the total number of chunks per rx/tx transactions per channel */
/* (in other words, the summation of all elements in CF_CHANNEL_NUM_R/TX_CHUNKS_PER_TRANSACTION */
#define CF_TOTAL_CHUNKS (CF_NAK_MAX_SEGMENTS * 4)

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
#define CF_MAX_POLLING_DIR_PER_CHAN (5)

/**
 *  @brief Max PDU size.
 *
 *  @par Description:
 *       The max PDU size across all channels in the system. Keep in mind that
 *       the max filedata PDU will be smaller than this. This size includes
 *       the PDU headers and everything. While this is the max value for all
 *       channels, the outgoing_file_chunk_size in the configuration table
 *       is different for each channel so a smaller size can be used.
 *
 *  @par Limits:
 *
 */
/* CF_MAX_PDU_SIZE must be the max possible PDU for any channel. Channels can be configured with a smaller max. */
#define CF_MAX_PDU_SIZE (512)

/**
 *  @brief Maximum file name length.
 *
 *  @par Limits:
 *
 */
#define CF_FILENAME_MAX_NAME CFE_MISSION_MAX_FILE_LEN

/**
 *  @brief Maximum file path (not including file name)
 *
 *  @par Limits:
 *
 */
#define CF_FILENAME_MAX_PATH (CFE_MISSION_MAX_PATH_LEN - CFE_MISSION_MAX_FILE_LEN)

/**
 *  @brief Max filename and path length.
 *
 *  @par Limits:
 *
 */
#define CF_FILENAME_MAX_LEN CFE_MISSION_MAX_PATH_LEN

/**
 * @brief Number of trailing bytes to add to CFDP PDU
 *
 * @par Description
 *      Additional padding bytes to be appended to the tail of CFDP PDUs
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
 *  @par Limits:
 *       Maximum value is the difference between the maximum size of a CFDP PDU and the
 *       maximum size of an SB message.
 */
#define CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES 0

/**
 * \brief Mission specific version number
 *
 *  \par Description:
 *       An application version number consists of four parts:
 *       major version number, minor version number, revision
 *       number and mission specific revision number. The mission
 *       specific revision number is defined here such
 *       that missions can manage as a configuration definition
 *
 *  \par Limits:
 *       Must be defined as a numeric value that is greater than
 *       or equal to zero.
 */
#define CF_MISSION_REV 0

/**\}*/

#endif
