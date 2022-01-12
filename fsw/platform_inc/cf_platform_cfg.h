/************************************************************************
 *
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 * Copyright © 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************/

/**
 * @file
 *
 *  CF application platform configuration.
 *  These options are used to configure application behavior.
 */

#ifndef CF_PLATFORM_CFG_H
#define CF_PLATFORM_CFG_H

/*************************************************************************
 * Macro definitions
 *************************************************************************/

/**
 * @cfcfg Entity id size
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
 *        CF_TransactionCmd_t, any command that selects a transaction based on EID
 *
 * @par Limits
 *         Must be one of uint8, uint16, uint32, uint64.
 */
typedef uint32 CF_EntityId_t;

/**
 * @cfcfg transaction sequence number size
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
 *        CF_TransactionCmd_t, any command that selects a transaction based on TSN
 *
 * @par Limits
 *         Must be one of uint8, uint16, uint32, uint64.
 */
typedef uint32 CF_TransactionSeq_t;

/**
 *  @cfcfg Application Pipe Depth
 *
 *  @par Description:
 *       Dictates the pipe depth of the cf command pipe.
 *
 *  @par Limits:
 *		 The minimum size of this paramater is 1
 *       The maximum size dictated by cFE platform configuration
 *		 parameter is CFE_SB_MAX_PIPE_DEPTH
 */
#define CF_PIPE_DEPTH 32

/**
 *  @cfcfg Number of channels
 *
 *  @par Description:
 *       The number of chanenls in the engine. Changing this
 *       value changes the configuration table for the application.
 *
 *  @par Limits:
 *       Must be less <= 200. Obviously it will be smaller than that.
 */
#define CF_NUM_CHANNELS 2

/**
 *  @cfcfg Max NAK segments supported in a NAK pdu
 *
 *  @par Description:
 *       When a NAK pdu is sent or received, this is the max number of
 *       segment requests supported. This number should match the ground
 *       cfdp engine configuration as well.
 *
 *  @par Limits:
 *
 */
#define CF_NAK_MAX_SEGMENTS 58 /* max number of NAK segments CF supports (leave room for overhead */

/* CHUNKS -
 * A chunk is a representatino of a range range (offset, size) of data received by a receiver.
 *
 * Class 2 CFDP deals with NAK, so received data must be tracked for receivers in order to generate
 * the NAK. The sender must also keep track of NAK requests and send new file data PDUs as a result.
 * (array size must be CF_NUM_CHANNELS)
 * CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION is an array for each channel indicating the number of chunks per transaction
 * CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION is an array for each channel indciate the number of chunks to keep track of
 *   NAK requests from the receiver per transaction*/
/**
 *  @cfcfg RX chunks per transaction (per channel)
 *
 *  @par Description:
 *       Number of chunks per transaction per channel (RX).
 *
 *  @par Limits:
 *
 */
#define CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION \
    {                                            \
        CF_NAK_MAX_SEGMENTS, CF_NAK_MAX_SEGMENTS \
    }

/**
 *  @cfcfg TX chunks per transaction (per channel)
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
 *  @cfcfg Total number of chunks (tx, rx, all channels)
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

/* definitions that affect file queuing */
/**
 *  @cfcfg Number of max commanded playback files per chan.
 *
 *  @par Description:
 *       This is the max number of outstanding ground commanded file transmits per channel.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN 10

/**
 *  @cfcfg Max number of simultaneous file receives.
 *
 *  @par Description:
 *       Each channel can support this number of file receive transactions at a time.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_SIMULTANEOUS_RX 5

/* definitions that affect execution */

/**
 *  @cfcfg Max number of commanded playback directories per channel.
 *
 *  @par Description:
 *       Each channel can support this number of groudn commanded directory playbacks.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN 2

/**
 *  @cfcfg Max number of polling directories per channel.
 *
 *  @par Description:
 *       This affects the configuration table. There must be an entry (can
 *       be empty) for each of these polling directories per channel.
 *
 *  @par Limits:
 *
 */
#define CF_MAX_POLLING_DIR_PER_CHAN 5

/**
 *  @cfcfg Number of transactions per playback directoriy.
 *
 *  @par Description:
 *       Each playback/polling directory operation will be able to have this
 *       many active transfers at a time pending or active.
 *
 *  @par Limits:
 *
 */
#define CF_NUM_TRANSACTIONS_PER_PLAYBACK 5

/**
 *  @cfcfg Number of histories per channel
 *
 *  @par Description:
 *       Each channel can support this number of file receive transactions at a time.
 *
 *  @par Limits:
 *       65536 is the current max.
 */
#define CF_NUM_HISTORIES_PER_CHANNEL 256

/**
 *  @cfcfg Max PDU size.
 *
 *  @par Description:
 *       The max PDU size across all channels in the system. Keep in mind that
 *       the max filedata pdu will be smaller than this. This size includes
 *       the PDU headers and everything. While this is the max value for all
 *       channels, the outgoing_file_chunk_size in the configuration table
 *       is different for each channel so a smaller size can be used.
 *
 *  @par Limits:
 *
 */
/* CF_MAX_PDU_SIZE must be the max possible PDU for any channel. Channels can be configured with a smaller max. */
#define CF_MAX_PDU_SIZE 512

/**
 *  @cfcfg Name of the CF Configuration Table
 *
 *  @par Description:
 *       This parameter defines the name of the CF Configuration Table.
 *
 *  @par Limits
 *       The length of this string, including the NULL terminator cannot exceed
 *       the #OS_MAX_PATH_LEN value.
 */
#define CF_CONFIG_TABLE_NAME "config_table"

/**
 *  @cfcfg CF Configuration Table Filename
 *
 *  @par Description:
 *       The value of this constant defines the filename of the CF Config Table
 *
 *  @par Limits
 *       The length of this string, including the NULL terminator cannot exceed
 *       the #OS_MAX_PATH_LEN value.
 */
#define CF_CONFIG_TABLE_FILENAME "/cf/cf_def_config.tbl"

/**
 *  @cfcfg Maximum file name length.
 *
 *  @par Limits:
 *
 */
#define CF_FILENAME_MAX_NAME OS_MAX_FILE_NAME

/**
 *  @cfcfg Maximum file path (not including file name)
 *
 *  @par Limits:
 *
 */
#define CF_FILENAME_MAX_PATH (OS_MAX_PATH_LEN - OS_MAX_FILE_NAME)

/**
 *  @cfcfg Max filename and path length.
 *
 *  @par Limits:
 *
 */
#define CF_FILENAME_MAX_LEN (CF_FILENAME_MAX_NAME + CF_FILENAME_MAX_PATH)

/**
 *  @cfcfg R2 crc calc chunk size
 *
 *  @par Description
 *       R2 performs crc calculation upon file completion in chunks. This is the size
 *       of the buffer. The larger the size the more stack will be used, but
 *       the faster it can go. The overall number of bytes calculated per wakeup
 *       is set in the configuration table.
 *
 *  @par Limits:
 *
 */
#define CF_R2_CRC_CHUNK_SIZE 1024

#if CF_FILENAME_MAX_LEN > OS_MAX_PATH_LEN
#error CF_FILENAME_MAX_LEN must be <= OS_MAX_PATH_LEN
#endif

/**
 *  @cfcfg Number of milliseconds to wait for a SB message
 *
 *  @par Limits:
 *
 */
#define CF_RCVMSG_TIMEOUT 100

#endif /* !CF_PLATFORM_CFG_H */
