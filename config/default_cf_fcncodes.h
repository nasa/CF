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
 *   Specification for the CFS CFDP (CF) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CF_FCNCODES_H
#define CF_FCNCODES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

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
     *       No Payload / Arguments
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_NOOP_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t where byte[0] specifies the counters type, byte[1-3] don't care:
     *       - 0 = all counters
     *       - 1 = command counters
     *       - 2 = fault counters
     *       - 3 = up counters
     *       - 4 = down counters
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_RESET_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid counter type, #CF_CMD_RESET_INVALID_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_TX_FILE_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid parameter, #CF_CMD_BAD_PARAM_ERR_EID
     *       - Transaction initialization failure, #CF_CMD_TX_FILE_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_PLAYBACK_DIR_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid parameter, #CF_CMD_BAD_PARAM_ERR_EID
     *       - Playback initialization failure, #CF_CMD_PLAYBACK_DIR_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_FREEZE_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid channel number, #CF_CMD_CHAN_PARAM_ERR_EID
     *       - Command processing failure, #CF_CMD_FREEZE_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_THAW_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid channel number, #CF_CMD_CHAN_PARAM_ERR_EID
     *       - Command processing failure, #CF_CMD_THAW_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_Transaction_Payload_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_SUSPRES_INF_EID
     *       - #CF_CMD_SUSPRES_SAME_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Transaction not found using compound key, #CF_CMD_TRANS_NOT_FOUND_ERR_EID
     *       - Invalid channel number, #CF_CMD_TSN_CHAN_INVALID_ERR_EID
     *       - No matching transaction, #CF_CMD_SUSPRES_CHAN_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_Transaction_Payload_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_SUSPRES_INF_EID
     *       - #CF_CMD_SUSPRES_SAME_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Transaction not found using compound key, #CF_CMD_TRANS_NOT_FOUND_ERR_EID
     *       - Invalid channel number, #CF_CMD_TSN_CHAN_INVALID_ERR_EID
     *       - No matching transaction, #CF_CMD_SUSPRES_CHAN_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_Transaction_Payload_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_CANCEL_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Transaction not found using compound key, #CF_CMD_TRANS_NOT_FOUND_ERR_EID
     *       - Invalid channel number, #CF_CMD_TSN_CHAN_INVALID_ERR_EID
     *       - No matching transaction, #CF_CMD_CANCEL_CHAN_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_Transaction_Payload_t
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_ABANDON_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Transaction not found using compound key, #CF_CMD_TRANS_NOT_FOUND_ERR_EID
     *       - Invalid channel number, #CF_CMD_TSN_CHAN_INVALID_ERR_EID
     *       - No matching transaction, #CF_CMD_ABANDON_CHAN_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_GETSET1_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid configuration parameter key, #CF_CMD_GETSET_PARAM_ERR_EID
     *       - Invalid channel number, #CF_CMD_GETSET_CHAN_ERR_EID
     *       - Parameter value failed validation, #CF_CMD_GETSET_VALIDATE_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_GET_PARAM_CC
     */
    CF_SET_PARAM_CC = 10,

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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_GETSET2_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid configuration parameter key, #CF_CMD_GETSET_PARAM_ERR_EID
     *       - Invalid channel number, #CF_CMD_GETSET_CHAN_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
     *
     *  \par Criticality
     *       None
     *
     *  \sa #CF_SET_PARAM_CC
     */
    CF_GET_PARAM_CC = 11,

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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_WQ_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid parameter combination, #CF_CMD_WQ_ARGS_ERR_EID
     *       - Invalid channel number, #CF_CMD_WQ_CHAN_ERR_EID
     *       - Open file to write failed, #CF_CMD_WQ_OPEN_ERR_EID
     *       - Write RX data failed, #CF_CMD_WQ_WRITEQ_RX_ERR_EID
     *       - Write RX history data failed, #CF_CMD_WQ_WRITEHIST_RX_ERR_EID
     *       - Write TX data failed, #CF_CMD_WQ_WRITEQ_TX_ERR_EID
     *       - Write TX history data failed, #CF_CMD_WQ_WRITEHIST_TX_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_ENABLE_DEQUEUE_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid channel number, #CF_CMD_CHAN_PARAM_ERR_EID
     *       - Enable dequeue failed, #CF_CMD_ENABLE_DEQUEUE_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t where byte[0] specifies the channel number or all channels
     *       - 255 = all channels
     *       - else = single channel
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_DISABLE_DEQUEUE_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid channel number, #CF_CMD_CHAN_PARAM_ERR_EID
     *       - Disable dequeue failed, #CF_CMD_DISABLE_DEQUEUE_INF_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t
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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_ENABLE_POLLDIR_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid channel number, #CF_CMD_CHAN_PARAM_ERR_EID
     *       - Invalid polling directory index, #CF_CMD_POLLDIR_INVALID_ERR_EID
     *       - Enable directory polling failed, #CF_CMD_ENABLE_POLLDIR_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t
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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_DISABLE_POLLDIR_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid channel number, #CF_CMD_CHAN_PARAM_ERR_EID
     *       - Invalid polling directory index, #CF_CMD_POLLDIR_INVALID_ERR_EID
     *       - Disable directory polling failed, #CF_CMD_DISABLE_POLLDIR_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       #CF_UnionArgs_Payload_t
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
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_PURGE_QUEUE_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Invalid channel number, #CF_CMD_CHAN_PARAM_ERR_EID
     *       - Invalid purge queue argument, #CF_CMD_PURGE_ARG_ERR_EID
     *       - Purge queue failed, #CF_CMD_PURGE_QUEUE_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       No Payload / Arguments
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_ENABLE_ENGINE_INF_EID
     *       - #CF_CMD_ENG_ALREADY_ENA_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *       - Engine initialization failed, #CF_CMD_ENABLE_ENGINE_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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
     *       re-enabled the new configuration will take effect.
     *
     *  \par Command Structure
     *       No Payload / Arguments
     *
     *  \par Command Verification
     *       Successful execution of this command may be verified with
     *       the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.cmd will increment
     *       - #CF_CMD_DISABLE_ENGINE_INF_EID
   *         - #CF_CMD_ENG_ALREADY_DIS_INF_EID
     *
     *  \par Error Conditions
     *       This command may fail for the following reason(s):
     *       - Command packet length not as expected, #CF_CMD_LEN_ERR_EID
     *
     *  \par Evidence of failure may be found in the following telemetry:
     *       - #CF_HkPacket_Payload_t.counters #CF_HkCmdCounters_t.err will increment
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

#endif
