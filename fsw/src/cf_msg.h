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
 *  The CF Application message definitions header file
 */

#ifndef CF_MSG_H
#define CF_MSG_H

#include "cfe.h"
#include "cf_platform_cfg.h"
#include "cf_cfdp.h"
#include "cf_tbldefs.h"

#define CF_ALL_CHANNELS (255)
#define CF_ALL_POLLDIRS (CF_ALL_CHANNELS)
#define CF_COMPOUND_KEY (254)

/**
 * \defgroup cfscftlm CFS CFDP Telemetry
 * \{
 */

/**
 * \brief Housekeeping command counters
 */
typedef struct CF_HkCmdCounters
{
    uint16 cmd; /**< \brief Command success counter */
    uint16 err; /**< \brief Command error counter */
} CF_HkCmdCounters_t;

/**
 * \brief Housekeeping sent counters
 */
typedef struct CF_HkSent
{
    uint64 file_data_bytes;      /**< \brief Sent File data bytes */
    uint32 pdu;                  /**< \brief Sent PDUs counter */
    uint32 nak_segment_requests; /**< \brief Sent NAK segment requests counter */
} CF_HkSent_t;

/**
 * \brief Housekeeping received counters
 */
typedef struct CF_HkRecv
{
    uint64 file_data_bytes;      /**< \brief Received File data bytes */
    uint32 pdu;                  /**< \brief Received PDUs with valid header counter */
    uint32 error;                /**< \brief Received PDUs with error counter, see related event for cause */
    uint16 spurious;             /**< \brief Received PDUs with invalid directive code for current context or
                                  *          file directive FIN without matching active transaction counter,
                                  *          see related event for cause
                                  */
    uint16 dropped;              /**< \brief Received PDUs dropped due to a transaction error */
    uint32 nak_segment_requests; /**< \brief Received NAK segment requests counter */
} CF_HkRecv_t;

/**
 * \brief Housekeeping fault counters
 */
typedef struct CF_HkFault
{
    uint16 file_open;          /**< \brief File open fault counter */
    uint16 file_read;          /**< \brief File read fault counter */
    uint16 file_seek;          /**< \brief File seek fault counter */
    uint16 file_write;         /**< \brief File write fault counter */
    uint16 file_rename;        /**< \brief File rename fault counter */
    uint16 directory_read;     /**< \brief Directory read fault counter */
    uint16 crc_mismatch;       /**< \brief CRC mismatch fault counter */
    uint16 file_size_mismatch; /**< \brief File size mismatch fault counter */
    uint16 nak_limit;          /**< \brief NAK limit exceeded fault counter */
    uint16 ack_limit;          /**< \brief ACK limit exceeded fault counter */
    uint16 inactivity_timer;   /**< \brief Inactivity timer exceeded counter */
    uint16 spare;              /**< \brief Alignment spare to avoid implicit padding */
} CF_HkFault_t;

/**
 * \brief Housekeeping counters
 */
typedef struct CF_HkCounters
{
    CF_HkSent_t  sent;  /**< \brief Sent counters */
    CF_HkRecv_t  recv;  /**< \brief Received counters */
    CF_HkFault_t fault; /**< \brief Fault counters */
} CF_HkCounters_t;

/**
 * \brief Housekeeping channel data
 */
typedef struct CF_HkChannel_Data
{
    CF_HkCounters_t counters;                /**< \brief Counters */
    uint16          q_size[CF_QueueIdx_NUM]; /**< \brief Queue sizes */
    uint8           poll_counter;            /**< \brief Number of active polling directories */
    uint8           playback_counter;        /**< \brief Number of active playback directories */
    uint8           frozen;                  /**< \brief Frozen state: 0 == not frozen, else frozen */
    uint8           spare[7];                /**< \brief Alignment spare (uint64 values in the counters) */
} CF_HkChannel_Data_t;

/**
 * \brief Housekeeping packet
 */
typedef struct CF_HkPacket
{
    CFE_MSG_TelemetryHeader_t tlm_header; /**< \brief Telemetry header */
    CF_HkCmdCounters_t        counters;   /**< \brief Command counters */
    uint8                     spare[4];   /**< \brief Alignment spare (CF_HkCmdCounters_t is 4 bytes) */

    CF_HkChannel_Data_t channel_hk[CF_NUM_CHANNELS]; /**< \brief Per channel housekeeping data */
} CF_HkPacket_t;

/**
 * \brief End of transaction packet
 */
typedef struct CF_EotPacket
{
    CFE_MSG_TelemetryHeader_t tlm_header; /**< \brief Telemetry header */
    CF_TransactionSeq_t       seq_num;    /**< \brief transaction identifier, stays constant for entire transfer */
    uint32                    channel;    /**< \brief Channel number */
    uint32                    direction;  /**< \brief direction of this transaction */
    uint32                    state;      /**< \brief Transaction state */
    uint32                    cc;         /**< \brief final condition code of operation */
    CF_EntityId_t             src_eid;    /**< \brief the source eid of the transaction */
    CF_EntityId_t             peer_eid;   /**< \brief peer_eid is always the "other guy", same src_eid for RX */
    uint32                    fsize;      /**< \brief File size */
    uint32                    crc_result; /**< \brief CRC result */
    CF_TxnFilenames_t         fnames;     /**< \brief file names associated with this transaction */
} CF_EotPacket_t;

/**
 * \brief Single application file info packet buffer
 *
 * This typedef supports CFE_SB_AllocateMessageBuffer use with a CF_EotPkt_t
 * that compiles with the alignment constraints of a CFE_SB_Buffer_t
 */
typedef union
{
    CFE_SB_Buffer_t SBBuf; /**< \brief Message buffer for alignment */
    CF_EotPacket_t  eot;   /**< \brief Single end of transaction info packet */
} CF_EotPktBuf_t;

/**\}*/

/**
 * \defgroup cfscfcmdcodes CFS CFDP Command Codes
 * \{
 */
typedef enum
{

    /**
     * \brief No-op
     *
     *  \par Description
     *       No-operation command for aliveness verification and version reporting
     *
     *  \par Command Structure
     *       #CF_NoArgsCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_NOOP
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     */
    CF_NOOP_CC = 0,

    /**
     * \brief Reset counters
     *
     *  \par Description
     *       Resets the requested housekeeping counters
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t where byte[0] specifies the counters type, byte[1-3] don't care:
     *       - 0 = all counters
     *       - 1 = command counters
     *       - 2 = fault counters
     *       - 3 = up counters
     *       - 4 = down counters
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_RESET
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid counter type, #CF_EID_ERR_CMD_RESET_INVALID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     */
    CF_RESET_CC = 1,

    /**
     * \brief Transmit file
     *
     *  \par Description
     *       Requests transmission of a file
     *
     *  \par Command Structure
     *       #CF_TxFileCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_TX_FILE
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid parameter, #CF_EID_ERR_CMD_BAD_PARAM
     *       - Transaction initialization failure, #CF_EID_ERR_CMD_TX_FILE
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_PLAYBACK_DIR_CC
     */
    CF_TX_FILE_CC = 2,

    /**
     * \brief Playback a directory
     *
     *  \par Description
     *       Transmits all the files in a directory
     *
     *  \par Command Structure
     *       #CF_PlaybackDirCmd_t - note it's currently a typedef of CF_TxFileCmd_t, where
     *       the source filename and destination filename are directories
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_PLAYBACK_DIR
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid parameter, #CF_EID_ERR_CMD_BAD_PARAM
     *       - Playback initialization failure, #CF_EID_ERR_CMD_PLAYBACK_DIR
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_TX_FILE_CC
     */
    CF_PLAYBACK_DIR_CC = 3,

    /**
     * \brief Freeze a channel
     *
     *  \par Description
     *       Disables the transmission of all PDUs and disables tick processing (timeouts, ACK/NAK, etc)
     *       for the specified channel, will still consume all received messages.  Note
     *       this could cause failures for class 2 transactions in progress.
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_FREEZE
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid channel number, #CF_EID_ERR_CMD_CHAN_PARAM
     *       - Command processing failure, #CF_EID_ERR_CMD_FREEZE
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_THAW_CC
     */
    CF_FREEZE_CC = 4,

    /**
     * \brief Thaw a channel
     *
     *  \par Description
     *       Enables the transmission of all PDUs and resumes tick processing (timeouts, ACK/NAK, etc)
     *       for the specified channel, note received messages are consumed either way.
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_THAW
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid channel number, #CF_EID_ERR_CMD_CHAN_PARAM
     *       - Command processing failure, #CF_EID_ERR_CMD_THAW
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_FREEZE_CC
     */
    CF_THAW_CC = 5,

    /**
     * \brief Suspend a transaction
     *
     *  \par Description
     *       Disables the transmission of all PDUs and disables tick processing (timeouts, ACK/NAK, etc) on a single
     *       transaction, all channels and transactions, or all transactions on a specific channel.
     *       Will still consume all received messages. Note suspension is tracked per transaction,
     *       whereas freeze/thaw are tracked per channel.
     *
     *  \par Command Structure
     *       #CF_TransactionCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_SUSPRES
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Transaction not found using compound key, #CF_EID_ERR_CMD_TRANS_NOT_FOUND
     *       - Invalid channel number, #CF_EID_ERR_CMD_TSN_CHAN_INVALID
     *       - Already in requested state, #CF_EID_ERR_CMD_SUSPRES_SAME
     *       - No matching transaction, #CF_EID_ERR_CMD_SUSPRES_CHAN
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_RESUME_CC, #CF_CANCEL_CC, #CF_ABANDON_CC
     */
    CF_SUSPEND_CC = 6,

    /**
     * \brief Resume a transaction
     *
     *  \par Description
     *       Enables the transmission of all PDUs and resumes tick processing (timeouts, ACK/NAK, etc) on a single
     *       transaction, all channels and transactions, or all transactions on a specific channel.
     *       Note a suspended transaction still consume all received messages. Note suspension is tracked per
     *       transaction, whereas freeze/thaw are tracked per channel.
     *
     *  \par Command Structure
     *       #CF_TransactionCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_SUSPRES
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Transaction not found using compound key, #CF_EID_ERR_CMD_TRANS_NOT_FOUND
     *       - Invalid channel number, #CF_EID_ERR_CMD_TSN_CHAN_INVALID
     *       - Already in requested state, #CF_EID_ERR_CMD_SUSPRES_SAME
     *       - No matching transaction, #CF_EID_ERR_CMD_SUSPRES_CHAN
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_SUSPEND_CC, #CF_CANCEL_CC, #CF_ABANDON_CC
     */
    CF_RESUME_CC = 7,

    /**
     * \brief Cancel a transaction
     *
     *  \par Description
     *       Cancel transaction processing by taking steps to close out cleanly (based on transaction
     *       type and direction) for a single transaction, all channels and transactions,
     *       or all transactions on a specific channel.
     *
     *  \par Command Structure
     *       #CF_TransactionCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_CANCEL
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Transaction not found using compound key, #CF_EID_ERR_CMD_TRANS_NOT_FOUND
     *       - Invalid channel number, #CF_EID_ERR_CMD_TSN_CHAN_INVALID
     *       - No matching transaction, #CF_EID_ERR_CMD_CANCEL_CHAN
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_SUSPEND_CC, #CF_RESUME_CC, #CF_ABANDON_CC
     */
    CF_CANCEL_CC = 8,

    /**
     * \brief Abandon a transaction
     *
     *  \par Description
     *       Abandon transaction processing with an immediate reset (no close out attempted)
     *       for a single transaction, all channels and transactions,
     *       or all transactions on a specific channel.
     *
     *  \par Command Structure
     *       #CF_TransactionCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_ABANDON
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Transaction not found using compound key, #CF_EID_ERR_CMD_TRANS_NOT_FOUND
     *       - Invalid channel number, #CF_EID_ERR_CMD_TSN_CHAN_INVALID
     *       - No matching transaction, #CF_EID_ERR_CMD_ABANDON_CHAN
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_SUSPEND_CC, #CF_RESUME_CC, #CF_CANCEL_CC
     */
    CF_ABANDON_CC = 9,

    /**
     * \brief Set parameter
     *
     *  \par Description
     *       Sets a configuration parameter
     *
     *  \par Command Structure
     *       #CF_SetParamCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_GETSET1
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid configuration parameter key, #CF_EID_ERR_CMD_GETSET_PARAM
     *       - Invalid channel number, #CF_EID_ERR_CMD_GETSET_CHAN
     *       - Parameter value failed validation, #CF_EID_ERR_CMD_GETSET_VALIDATE
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_GET_MIB_PARAM_CC
     */
    CF_SET_MIB_PARAM_CC = 10,

    /**
     * \brief Get parameter
     *
     *  \par Description
     *       Gets a configuration parameter
     *
     *  \par Command Structure
     *       #CF_GetParamCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_GETSET2
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid configuration parameter key, #CF_EID_ERR_CMD_GETSET_PARAM
     *       - Invalid channel number, #CF_EID_ERR_CMD_GETSET_CHAN
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_SET_MIB_PARAM_CC
     */
    CF_GET_MIB_PARAM_CC = 11,

    /**
     * \brief Write queue
     *
     *  \par Description
     *       Writes requested queue(s) to a file
     *
     *  \par Command Structure
     *       #CF_WriteQueueCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_WQ
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid parameter combination, #CF_EID_ERR_CMD_WQ_ARGS
     *       - Invalid channel number, #CF_EID_ERR_CMD_WQ_CHAN
     *       - Open file to write failed, #CF_EID_ERR_CMD_WQ_OPEN
     *       - Write RX data failed, #CF_EID_ERR_CMD_WQ_WRITEQ_RX
     *       - Write RX history data failed, #CF_EID_ERR_CMD_WQ_WRITEHIST_RX
     *       - Write TX data failed, #CF_EID_ERR_CMD_WQ_WRITEQ_TX
     *       - Write TX history data failed, #CF_EID_ERR_CMD_WQ_WRITEHIST_TX
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_PURGE_QUEUE_CC
     */
    CF_WRITE_QUEUE_CC = 15,

    /**
     * \brief Enable dequeue
     *
     *  \par Description
     *       Enables the sending of file data PDUs.
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_ENABLE_DEQUEUE
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid channel number, #CF_EID_ERR_CMD_CHAN_PARAM
     *       - Enable dequeue failed, #CF_EID_ERR_CMD_ENABLE_DEQUEUE
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_DISABLE_DEQUEUE_CC
     */
    CF_ENABLE_DEQUEUE_CC = 16,

    /**
     * \brief Disable dequeue
     *
     *  \par Description
     *       Disables the sending of file data PDUs.
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_DISABLE_DEQUEUE
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid channel number, #CF_EID_ERR_CMD_CHAN_PARAM
     *       - Disable dequeue failed, #CF_EID_INF_CMD_DISABLE_DEQUEUE
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_ENABLE_DEQUEUE_CC
     */
    CF_DISABLE_DEQUEUE_CC = 17,

    /**
     * \brief Enable directory polling
     *
     *  \par Description
     *       Enables the processing of polling directories
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t
     *
     *       byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *       byte[1] specifies the polling directory index
     *       - 255 = all polling directories
     *       - else = single polling directory index
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_ENABLE_POLLDIR
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid channel number, #CF_EID_ERR_CMD_CHAN_PARAM
     *       - Invalid polling directory index, #CF_EID_ERR_CMD_POLLDIR_INVALID
     *       - Enable directory polling failed, #CF_EID_ERR_CMD_ENABLE_POLLDIR
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_DISABLE_DIR_POLLING_CC
     */
    CF_ENABLE_DIR_POLLING_CC = 18,

    /**
     * \brief Disable directory polling
     *
     *  \par Description
     *       Disable the processing of polling directories
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t
     *
     *       byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *       byte[1] specifies the polling directory index
     *       - 255 = all polling directories
     *       - else = single polling directory index
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_DISABLE_POLLDIR
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid channel number, #CF_EID_ERR_CMD_CHAN_PARAM
     *       - Invalid polling directory index, #CF_EID_ERR_CMD_POLLDIR_INVALID
     *       - Disable directory polling failed, #CF_EID_ERR_CMD_DISABLE_POLLDIR
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_ENABLE_DIR_POLLING_CC
     */
    CF_DISABLE_DIR_POLLING_CC = 19,

    /**
     * \brief Purge queue
     *
     *  \par Description
     *       Purge the requested queue
     *
     *  \par Command Structure
     *       #CF_UnionArgsCmd_t
     *
     *       byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *       byte[1] specifies the queue
     *       - 0 = Pending queue
     *       - 1 = History queue
     *       - 2 = Both pending and history queue
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_PURGE_QUEUE
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Invalid channel number, #CF_EID_ERR_CMD_CHAN_PARAM
     *       - Invalid purge queue argument, #CF_EID_ERR_CMD_PURGE_ARG
     *       - Purge queue failed, #CF_EID_ERR_CMD_PURGE_QUEUE
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_WRITE_QUEUE_CC
     */
    CF_PURGE_QUEUE_CC = 21,

    /**
     * \brief Enable engine
     *
     *  \par Description
     *       Reinitialize engine and enable processing.  Note configuration table updates
     *       are not processed while the engine is enabled.
     *
     *  \par Command Structure
     *       #CF_NoArgsCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_ENABLE_ENGINE
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Engine initialization failed, #CF_EID_ERR_CMD_ENABLE_ENGINE
     *       - Engine already enabled, #CF_EID_ERR_CMD_ENG_ALREADY_ENA
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_DISABLE_ENGINE_CC
     */
    CF_ENABLE_ENGINE_CC = 22,

    /**
     * \brief Disable engine
     *
     *  \par Description
     *       Disable engine processing.  Note configuration table updates
     *       can be performed while the engine is disabled, and when the engine is
     *       re-enabled the new configuration will take affect.
     *
     *  \par Command Structure
     *       #CF_NoArgsCmd_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_EID_INF_CMD_DISABLE_ENGINE
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_EID_ERR_CMD_GCMD_LEN
     *       - Engine already disabled, #CF_EID_ERR_CMD_ENG_ALREADY_DIS
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_DISABLE_ENGINE_CC
     */
    CF_DISABLE_ENGINE_CC = 23,

    /** \brief Command code limit used for validity check and array sizing */
    CF_NUM_COMMANDS = 24,
} CF_CMDS;

/**\}*/

/**
 * \defgroup cfscfcmdstructs CFS CFDP Command Structures
 * \{
 */

/**
 * \brief No arguments command structure
 *
 * For command details see #CF_NOOP_CC, #CF_ENABLE_ENGINE_CC, #CF_DISABLE_ENGINE_CC
 */
typedef struct CF_NoArgsCmd
{
    CFE_MSG_CommandHeader_t cmd_header; /**< \brief Command header */
} CF_NoArgsCmd_t;

/**
 * \brief Command payload argument union to support 4 uint8's, 2 uint16's or 1 uint32
 */
typedef union CF_UnionArgs_Payload
{
    uint32 dword;    /**< \brief Generic uint32 argument */
    uint16 hword[2]; /**< \brief Generic uint16 array of arguments */
    uint8  byte[4];  /**< \brief Generic uint8 array of arguments */
} CF_UnionArgs_Payload_t;

/**
 * \brief Generic command structure with arguments supports common handling on multiple command types
 *
 * For command details see #CF_RESET_CC, #CF_FREEZE_CC, #CF_THAW_CC, #CF_ENABLE_DEQUEUE_CC,
 * #CF_DISABLE_DEQUEUE_CC, #CF_ENABLE_DIR_POLLING_CC, #CF_ENABLE_DIR_POLLING_CC, #CF_PURGE_QUEUE_CC
 */
typedef struct
{
    CFE_MSG_CommandHeader_t cmd_header; /**< \brief Command header */
    CF_UnionArgs_Payload_t  data;       /**< \brief Generic command arguments */
} CF_UnionArgsCmd_t;

/**
 * \brief IDs for use for Reset cmd
 */
typedef enum
{
    CF_Reset_all     = 0, /**< \brief Reset all */
    CF_Reset_command = 1, /**< \brief Reset command */
    CF_Reset_fault   = 2, /**< \brief Reset fault */
    CF_Reset_up      = 3, /**< \brief Reset up */
    CF_Reset_down    = 4  /**< \brief Reset down */
} CF_Reset_t;

/**
 * \brief Type IDs for use for Write Queue cmd
 */
typedef enum
{
    CF_Type_all  = 0, /**< \brief Type all */
    CF_Type_up   = 1, /**< \brief Type up */
    CF_Type_down = 2  /**< \brief Type down */
} CF_Type_t;

/**
 * \brief Queue IDs for use for Write Queue cmd
 */
typedef enum
{
    CF_Queue_pend    = 0, /**< \brief Queue pending */
    CF_Queue_active  = 1, /**< \brief Queue active */
    CF_Queue_history = 2, /**< \brief Queue history */
    CF_Queue_all     = 3  /**< \brief Queue all */
} CF_Queue_t;

/**
 * \brief Parameter IDs for use with Get/Set parameter messages
 *
 * Specifically these are used for the "key" field within CF_GetParamCmd_t and
 * CF_SetParamCmd_t message structures.
 */
typedef enum
{
    CF_GetSet_ValueID_ticks_per_second,                      /**< \brief Ticks per second key */
    CF_GetSet_ValueID_rx_crc_calc_bytes_per_wakeup,          /**< \brief Receive CRC calculated bytes per wake-up key */
    CF_GetSet_ValueID_ack_timer_s,                           /**< \brief ACK timer in seconds key */
    CF_GetSet_ValueID_nak_timer_s,                           /**< \brief NAK timer in seconds key */
    CF_GetSet_ValueID_inactivity_timer_s,                    /**< \brief Inactivity timer in seconds key */
    CF_GetSet_ValueID_outgoing_file_chunk_size,              /**< \brief Outgoing file chunk size key */
    CF_GetSet_ValueID_ack_limit,                             /**< \brief ACK retry limit key */
    CF_GetSet_ValueID_nak_limit,                             /**< \brief NAK retry limit key */
    CF_GetSet_ValueID_local_eid,                             /**< \brief Local entity id key */
    CF_GetSet_ValueID_chan_max_outgoing_messages_per_wakeup, /**< \brief Max outgoing messages per wake-up key */
    CF_GetSet_ValueID_MAX                                    /**< \brief Key limit used for validity check */
} CF_GetSet_ValueID_t;

/**
 * \brief Get parameter command structure
 *
 * For command details see #CF_GET_MIB_PARAM_CC
 */
typedef struct CF_GetParamCmd
{
    CFE_MSG_CommandHeader_t cmd_header; /**< \brief Command header */
    uint8                   key;        /**< \brief Parameter key, see #CF_GetSet_ValueID_t */
    uint8                   chan_num;   /**< \brief Channel number */
} CF_GetParamCmd_t;

/**
 * \brief Set parameter command structure
 *
 * For command details see #CF_SET_MIB_PARAM_CC
 */
typedef struct CF_SetParamCmd
{
    CFE_MSG_CommandHeader_t cmd_header; /**< \brief Command header */
    uint32                  value;      /**< \brief Parameter value to set */
    uint8                   key;        /**< \brief Parameter key, see #CF_GetSet_ValueID_t */
    uint8                   chan_num;   /**< \brief Channel number */
    uint8                   spare[2];   /**< \brief Alignment spare, uint32 multiple */
} CF_SetParamCmd_t;

/**
 * \brief Transmit file command structure
 *
 * For command details see #CF_TX_FILE_CC
 */
typedef struct CF_TxFileCmd
{
    CFE_MSG_CommandHeader_t cmd_header;                        /**< \brief Command header */
    uint8                   cfdp_class;                        /**< \brief CFDP class: 0=class 1, 1=class 2 */
    uint8                   keep;                              /**< \brief Keep file flag: 1=keep, else delete */
    uint8                   chan_num;                          /**< \brief Channel number */
    uint8                   priority;                          /**< \brief Priority: 0=highest priority */
    CF_EntityId_t           dest_id;                           /**< \brief Destination entity id */
    char                    src_filename[CF_FILENAME_MAX_LEN]; /**< \brief Source file/directory name */
    char                    dst_filename[CF_FILENAME_MAX_LEN]; /**< \brief Destination file/directory name */
} CF_TxFileCmd_t;

/**
 * \brief Write Queue command structure
 *
 * For command details see #CF_WRITE_QUEUE_CC
 */
typedef struct CF_WriteQueueCmd
{
    CFE_MSG_CommandHeader_t cmd_header; /**< \brief Command header */
    uint8                   type;       /**< \brief Transaction direction: all=0, up=1, down=2 */
    uint8                   chan;       /**< \brief Channel number */
    uint8                   queue;      /**< \brief Queue type: 0=pending, 1=active, 2=history, 3=all */
    uint8                   spare;      /**< \brief Alignment spare, puts filename on 32-bit boundary */

    char filename[CF_FILENAME_MAX_LEN]; /**< \brief Filename written to */
} CF_WriteQueueCmd_t;

/**
 * \brief Playback directory command structure
 *
 * For command details see #CF_PLAYBACK_DIR_CC
 */
typedef CF_TxFileCmd_t CF_PlaybackDirCmd_t;

/**
 * \brief Transaction command structure
 *
 * For command details see #CF_SUSPEND_CC, #CF_RESUME_CC, #CF_CANCEL_CC, #CF_ABANDON_CC
 */
typedef struct CF_TransactionCmd
{
    CFE_MSG_CommandHeader_t cmd_header; /**< \brief Command header */
    CF_TransactionSeq_t     ts;         /**< \brief Transaction sequence number */
    CF_EntityId_t           eid;        /**< \brief Entity id */
    uint8                   chan;       /**< \brief Channel number: 254=use ts, 255=all channels, else channel */
    uint8                   spare[3];   /**< \brief Alignment spare for 32-bit multiple */
} CF_TransactionCmd_t;

/**\}*/

#endif /* !CF_MSG_H */
