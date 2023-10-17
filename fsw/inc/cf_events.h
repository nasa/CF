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
 *  The CF Application event id definition header file
 */

#ifndef CF_EVENTS_H
#define CF_EVENTS_H

/**
 * \defgroup cfscfevents CFS CFDP Event IDs
 * \{
 */

/**************************************************************************
 * CF_INIT event IDs - Initialization
 */

/**
 * \brief CF Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Successful completion of application initialization
 */
#define CF_EID_INF_INIT (20)

/**
 * \brief CF Check Table Release Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from release table address call during periodic table check
 */
#define CF_EID_ERR_INIT_TBL_CHECK_REL (21)

/**
 * \brief CF Check Table Manage Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from manage table call during periodic table check
 */
#define CF_EID_ERR_INIT_TBL_CHECK_MAN (22)

/**
 * \brief CF Check Table Get Address Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from get table call during periodic table check
 */
#define CF_EID_ERR_INIT_TBL_CHECK_GA (23)

/**
 * \brief CF Table Registration At Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from table register call during application initialization
 */
#define CF_EID_ERR_INIT_TBL_REG (24)

/**
 * \brief CF Table Load At Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from table load call during application initialization
 */
#define CF_EID_ERR_INIT_TBL_LOAD (25)

/**
 * \brief CF Table Manage At Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from table manage call during application initialization
 */
#define CF_EID_ERR_INIT_TBL_MANAGE (26)

/**
 * \brief CF Table Get Address At Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from table get address call during application initialization
 */
#define CF_EID_ERR_INIT_TBL_GETADDR (27)

/**
 * \brief CF Message ID Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid message ID received on the software bus pipe
 */
#define CF_EID_ERR_INIT_CMD_LENGTH (28)

/**
 * \brief CF SB Receive Buffer Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from SB Receive Buffer call in application run loop
 */
#define CF_EID_ERR_INIT_MSG_RECV (29)

/**
 * \brief CF Channel Semaphore Initialization Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from get semaphore by name call during engine channel initialization,
 *  semaphore needs to exist before engine is initialized.
 */
#define CF_EID_ERR_INIT_SEM (30)

/**
 * \brief CF Channel Create Pipe Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from create pipe call during engine channel initialization
 */
#define CF_EID_ERR_INIT_PIPE (31)

/**
 * \brief CF Channel Message Subscription Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from message subscription call during engine channel initialization
 */
#define CF_EID_ERR_INIT_SUB (32)

/**
 * \brief CF Ticks Per Second Config Table Validation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Configuration table ticks per second set to zero
 */
#define CF_EID_ERR_INIT_TPS (33)

/**
 * \brief CF CRC Bytes Per Wakeup Config Table Validation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Configuration table CRC bytes per wakeup not aligned or zero
 */
#define CF_EID_ERR_INIT_CRC_ALIGN (34)

/**
 * \brief CF Outgoing Chunk Size Config Table Validation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Configuration table outgoing chunk size larger than PDU data size
 */
#define CF_EID_ERR_INIT_OUTGOING_SIZE (35)

/**************************************************************************
 * CF_PDU event IDs - Protocol data unit
 */

/**
 * \brief CF Metadata PDU Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Successful processing of metadata PDU
 */
#define CF_EID_INF_PDU_MD_RECVD (40)

/**
 * \brief CF PDU Header Too Short Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing PDU header
 */
#define CF_EID_ERR_PDU_SHORT_HEADER (41)

/**
 * \brief CF Metadata PDU Too Short Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing metadata PDU
 */
#define CF_EID_ERR_PDU_MD_SHORT (43)

/**
 * \brief CF Metadata PDU Source Filename Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Metadata PDU source filename length exceeds buffer size
 */
#define CF_EID_ERR_PDU_INVALID_SRC_LEN (44)

/**
 * \brief CF Metadata PDU Destination Filename Length Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Metadata PDU destination filename length exceeds buffer size
 */
#define CF_EID_ERR_PDU_INVALID_DST_LEN (45)

/**
 * \brief CF File Data PDU Too Short Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing file data PDU
 */
#define CF_EID_ERR_PDU_FD_SHORT (46)

/**
 * \brief CF End-Of-File PDU Too Short Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing end-of-file PDU
 */
#define CF_EID_ERR_PDU_EOF_SHORT (47)

/**
 * \brief CF Acknowledgment PDU Too Short Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing acknowledgment PDU
 */
#define CF_EID_ERR_PDU_ACK_SHORT (48)

/**
 * \brief CF Finished PDU Too Short Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing finished PDU
 */
#define CF_EID_ERR_PDU_FIN_SHORT (49)

/**
 * \brief CF Negative Acknowledgment PDU Too Short Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing negative acknowledgment PDU
 */
#define CF_EID_ERR_PDU_NAK_SHORT (50)

/**
 * \brief CF File Data PDU Unsupported Option Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  File Data PDU received with the segment metadata flag set
 */
#define CF_EID_ERR_PDU_FD_UNSUPPORTED (54)

/**
 * \brief CF PDU Header Large File Flag Set Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  PDU Header received with the unsupported large file flag set
 */
#define CF_EID_ERR_PDU_LARGE_FILE (55)

/**
 * \brief CF PDU Header Field Truncation
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  PDU Header received with fields that would be truncated with the cf configuration
 */
#define CF_EID_ERR_PDU_TRUNCATION (56)

/**************************************************************************
 * CF_CFDP event IDs - Engine
 */

/**
 * \brief Attempt to reset a transaction that has already been freed
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  Can be induced via various off-nominal conditions - such as sending a META-data PDU
 *  with an invalid file destination.
 *  
 */
#define CF_EID_DBG_RESET_FREED_XACT (59)

/**
 * \brief CF PDU Received Without Existing Transaction, Dropped Due To Max RX Reached Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  PDU without a matching/existing transaction received when channel receive queue is already
 *  handling the maximum number of concurrent receive transactions
 */
#define CF_EID_ERR_CFDP_RX_DROPPED (60)

/**
 * \brief CF PDU Received With Invalid Destination Entity ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  PDU without a matching/existing transaction received with an entity ID that doesn't
 *  match the receiving channel's entity ID
 */
#define CF_EID_ERR_CFDP_INVALID_DST_EID (61)

/**
 * \brief CF Invalid Metadata PDU Received On Idle Transaction Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Metadata PDU received for an idle transaction failed decoding
 */
#define CF_EID_ERR_CFDP_IDLE_MD (62)

/**
 * \brief CF Non-metadata File Directive PDU Received On Idle Transaction Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  File Directive PDU received without the metadata directive code on an idle transaction
 */
#define CF_EID_ERR_CFDP_FD_UNHANDLED (63)

/**
 * \brief CF Transmission Request Rejected Due To Max Commanded TX Reached Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Command request to transmit a file received when channel is already
 *  handling the maximum number of concurrent command transmit transactions
 */
#define CF_EID_ERR_CFDP_MAX_CMD_TX (64)

/**
 * \brief CF Playback/Polling Directory Open Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure opening directory during playback or polling initialization
 */
#define CF_EID_ERR_CFDP_OPENDIR (65)

/**
 * \brief CF Playback Request Rejected Due to Max Playback Directories Reached Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Command request to playback a directory received when channel is already
 *  handling the maximum number of concurrent playback directories
 */
#define CF_EID_ERR_CFDP_DIR_SLOT (66)

/**
 * \brief CF No Message Buffer Available Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from SB allocate message buffer call when constructing PDU
 */
#define CF_EID_ERR_CFDP_NO_MSG (67)

/**
 * \brief CF Close File Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from file close call
 */
#define CF_EID_ERR_CFDP_CLOSE_ERR (68)

/**************************************************************************
 * CF_CFDP_R event IDs - Engine receive
 */

/**
 * \brief CF Requesting RX Metadata Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  RX transaction missing metadata which results in a NAK being sent to
 *  request a metadata PDU for the transaction
 */
#define CF_EID_INF_CFDP_R_REQUEST_MD (70)

/**
 * \brief CF Creating Temp File For RX Transaction Without Metadata PDU
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  RX transaction missing metadata causing creation of a temporary
 *  filename to store the data
 */
#define CF_EID_INF_CFDP_R_TEMP_FILE (71)

/**
 * \brief CF RX Transaction NAK Limit Reached Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Condition that triggers a NAK occurred that would meet or exceed the NAK limit
 */
#define CF_EID_ERR_CFDP_R_NAK_LIMIT (72)

/**
 * \brief CF RX Transaction ACK Limit Reached Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Condition that triggers an ACK occurred that would meet or exceed the ACK limit
 */
#define CF_EID_ERR_CFDP_R_ACK_LIMIT (73)

/**
 * \brief CF RX Transaction CRC Mismatch Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  RX Transaction final CRC mismatch
 */
#define CF_EID_ERR_CFDP_R_CRC (74)

/**
 * \brief CF RX File Data PDU Seek Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of lseek call when processing out of order file data PDUs
 */
#define CF_EID_ERR_CFDP_R_SEEK_FD (75)

/**
 * \brief CF RX Class 2 CRC Seek Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of lseek call when calculating CRC from the file at
 *  the end of a Class 2 RX transaction
 */
#define CF_EID_ERR_CFDP_R_SEEK_CRC (76)

/**
 * \brief CF RX File Data PDU Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 * Failure of write to file call when processing file data PDUs
 */
#define CF_EID_ERR_CFDP_R_WRITE (77)

/**
 * \brief CF RX End-Of-File PDU File Size Mismatch Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  End-of-file PDU file size does not match transaction expected file size
 */
#define CF_EID_ERR_CFDP_R_SIZE_MISMATCH (78)

/**
 * \brief CF Invalid End-Of-File PDU Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  End-of-file PDU failed decoding
 */
#define CF_EID_ERR_CFDP_R_PDU_EOF (79)

/**
 * \brief CF RX Transaction File Create Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure in opencreate file call for an RX transaction
 */
#define CF_EID_ERR_CFDP_R_CREAT (80)

/**
 * \brief CF Class 2 RX Transaction Invalid FIN-ACK PDU Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  ACK PDU failed decoding during Class 2 RX Transaction
 */
#define CF_EID_ERR_CFDP_R_PDU_FINACK (81)

/**
 * \brief CF RX Class 2 Metadata PDU Size Mismatch Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Out-of-order RX Class 2 Metadata PDU received with file size that doesn't
 *  match already received EOF PDU file size
 */
#define CF_EID_ERR_CFDP_R_EOF_MD_SIZE (82)

/**
 * \brief CF RX Class 2 Metadata PDU File Rename Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from file rename call after reception of an out-of-order RX
 *  Class 2 Metadata PDU
 */
#define CF_EID_ERR_CFDP_R_RENAME (83)

/**
 * \brief CF RX Class 2 Metadata PDU File Open Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from file open call after reception of an out-of-order RX
 *  Class 2 Metadata PDU
 */
#define CF_EID_ERR_CFDP_R_OPEN (84)

/**
 * \brief CF Invalid Out-of-order Metadata PDU Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure to decode out-of-order metadata PDU
 */
#define CF_EID_ERR_CFDP_R_PDU_MD (85)

/**
 * \brief CF Class 2 CRC Read From File Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure from file read call during RX Class 2 CRC calculation
 */
#define CF_EID_ERR_CFDP_R_READ (86)

/**
 * \brief CF RX Invalid File Directive PDU Code Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Unrecognized file directive PDU directive code received for
 *  a current transaction
 */
#define CF_EID_ERR_CFDP_R_DC_INV (87)

/**
 * \brief CF RX Inactivity Timer Expired Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Expiration of the RX inactivity timer
 */
#define CF_EID_ERR_CFDP_R_INACT_TIMER (88)

/**************************************************************************
 * CF_CFDP_S event IDs - Engine send
 */

/**
 * \brief CF TX Initiated Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  File TX transaction initiated
 */
#define CF_EID_INF_CFDP_S_START_SEND (90)

/**
 * \brief CF TX File Data PDU Seek Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of lseek call when preparing to send file data PDU
 */
#define CF_EID_ERR_CFDP_S_SEEK_FD (91)

/**
 * \brief CF TX File Data PDU Read Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of read file call when preparing to send file data PDU
 */
#define CF_EID_ERR_CFDP_S_READ (92)

/**
 * \brief CF TX File Data PDU Send Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure to send the file data PDU
 */
#define CF_EID_ERR_CFDP_S_SEND_FD (93)

/**
 * \brief CF TX Metadata PDU File Already Open Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure to send metadata PDU due to file already being open
 */
#define CF_EID_ERR_CFDP_S_ALREADY_OPEN (94)

/**
 * \brief CF TX Metadata PDU File Open Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure in file open call when preparing to send metadata PDU
 */
#define CF_EID_ERR_CFDP_S_OPEN (95)

/**
 * \brief CF TX Metadata PDU File Seek End Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure in file lseek to end of file call when preparing
 *  to send metadata PDU
 */
#define CF_EID_ERR_CFDP_S_SEEK_END (96)

/**
 * \brief CF TX Metadata PDU File Seek Beginning Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure in file lseek to beginning of file call when
 *  preparing to send metadata PDU
 */
#define CF_EID_ERR_CFDP_S_SEEK_BEG (97)

/**
 * \brief CF TX Metadata PDU Send Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure to send the metadata PDU
 */
#define CF_EID_ERR_CFDP_S_SEND_MD (98)

/**
 * \brief CF TX Received NAK PDU Bad Segment Request Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Bad segment request values in received NAK PDU relating
 *  to a current transaction
 */
#define CF_EID_ERR_CFDP_S_INVALID_SR (100)

/**
 * \brief CF TX Received NAK PDU Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing received NAK PDU relating
 *  to a current transaction
 */
#define CF_EID_ERR_CFDP_S_PDU_NAK (101)

/**
 * \brief CF TX Received EOF ACK PDU Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure processing received ACK PDU relating
 *  to a current transaction
 */
#define CF_EID_ERR_CFDP_S_PDU_EOF (102)

/**
 * \brief CF TX Received Early FIN PDU Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Early FIN PDU received prior to completion of a current transaction
 */
#define CF_EID_ERR_CFDP_S_EARLY_FIN (103)

/**
 * \brief CF Invalid TX File Directive PDU Code Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Unrecognized file directive PDU directive code received for
 *  a current transaction
 */
#define CF_EID_ERR_CFDP_S_DC_INV (104)

/**
 * \brief CF Received TX Non-File Directive PDU Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Received a non-file directive PDU on a send transaction
 */
#define CF_EID_ERR_CFDP_S_NON_FD_PDU (105)

/**
 * \brief CF TX EOF PDU Send Limit Reached Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Timed out the limit number of times waiting for an ACK PDU for the EOF PDU on a
 *  current transaction
 */
#define CF_EID_ERR_CFDP_S_ACK_LIMIT (106)

/**
 * \brief CF TX Inactivity Timer Expired Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Send transaction activity timeout expired
 */
#define CF_EID_ERR_CFDP_S_INACT_TIMER (107)

/**************************************************************************
 * CF_CMD event IDs - Command processing
 */

/**
 * \brief CF NOOP Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt of NOOP command
 */
#define CF_EID_INF_CMD_NOOP (110)

/**
 * \brief CF Reset Counters Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of reset counters command
 */
#define CF_EID_INF_CMD_RESET (111)

/**
 * \brief CF Set Parameter Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of set parameter command
 */
#define CF_EID_INF_CMD_GETSET1 (112)

/**
 * \brief CF Get Parameter Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of get parameter command
 */
#define CF_EID_INF_CMD_GETSET2 (113)

/**
 * \brief CF Suspend/Resume Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of suspend/resume command
 */
#define CF_EID_INF_CMD_SUSPRES (114)

/**
 * \brief CF Write Queue Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of write queue command
 */
#define CF_EID_INF_CMD_WQ (115)

/**
 * \brief CF Enable Engine Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of enable engine command
 */
#define CF_EID_INF_CMD_ENABLE_ENGINE (116)

/**
 * \brief CF Disable Engine Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of disable engine command
 */
#define CF_EID_INF_CMD_DISABLE_ENGINE (117)

/**
 * \brief CF Transfer File Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of transfer file command
 */
#define CF_EID_INF_CMD_TX_FILE (118)

/**
 * \brief CF Playback Directory Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of playback directory command
 */
#define CF_EID_INF_CMD_PLAYBACK_DIR (119)

/**
 * \brief CF Freeze Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of freeze command
 */
#define CF_EID_INF_CMD_FREEZE (120)

/**
 * \brief CF Thaw Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of thaw command
 */
#define CF_EID_INF_CMD_THAW (121)

/**
 * \brief CF Cancel Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of cancel command
 */
#define CF_EID_INF_CMD_CANCEL (122)

/**
 * \brief CF Abandon Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of abandon command
 */
#define CF_EID_INF_CMD_ABANDON (123)

/**
 * \brief CF Enable Dequeue Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of enable dequeue command
 */
#define CF_EID_INF_CMD_ENABLE_DEQUEUE (124)

/**
 * \brief CF Disable Dequeue Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of disable dequeue command
 */
#define CF_EID_INF_CMD_DISABLE_DEQUEUE (125)

/**
 * \brief CF Enable Polldir Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of enable polldir command
 */
#define CF_EID_INF_CMD_ENABLE_POLLDIR (126)

/**
 * \brief CF Disable Polldir Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of disable polldir command
 */
#define CF_EID_INF_CMD_DISABLE_POLLDIR (127)

/**
 * \brief CF Purge Queue Command Received Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Receipt and successful processing of purge queue command
 */
#define CF_EID_INF_CMD_PURGE_QUEUE (128)

/**
 * \brief CF Reset Counters Command Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Reset counters command received with invalid parameter
 */
#define CF_EID_ERR_CMD_RESET_INVALID (129)

/**
 * \brief CF Command Channel Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Command received with channel parameter out of range
 */
#define CF_EID_ERR_CMD_CHAN_PARAM (130)

/**
 * \brief CF Command Transaction Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Command received without a matching transaction
 */
#define CF_EID_ERR_CMD_TRANS_NOT_FOUND (131)

/**
 * \brief CF Command All Transaction Channel Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Command received to act on all transactions with invalid channel
 */
#define CF_EID_ERR_CMD_TSN_CHAN_INVALID (132)

/**
 * \brief CF Suspend/Resume Command For Single Transaction State Unchanged Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Suspend/resume command received affecting single transaction already set to that state
 */
#define CF_EID_ERR_CMD_SUSPRES_SAME (133)

/**
 * \brief CF Suspend/Resume Command No Matching Transaction Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Suspend/resume command received without a matching transaction
 */
#define CF_EID_ERR_CMD_SUSPRES_CHAN (134)

/**
 * \brief CF Enable/Disable Polling Directory Command Invalid Polling Directory Index Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Enable/disable polling directory command received with invalid poling directory index
 */
#define CF_EID_ERR_CMD_POLLDIR_INVALID (135)

/**
 * \brief CF Purge Queue Command Invalid Argument Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Purge Queue command received with invalid queue argument
 */
#define CF_EID_ERR_CMD_PURGE_ARG (136)

/**
 * \brief CF Write Queue Command Invalid Channel Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Write Queue command received with invalid channel argument
 */
#define CF_EID_ERR_CMD_WQ_CHAN (137)

/**
 * \brief CF Write Queue Command Invalid Queue Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Write Queue command received with invalid queue selection arguments
 */
#define CF_EID_ERR_CMD_WQ_ARGS (138)

/**
 * \brief CF Write Queue Command File Open Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of open file call during processing of write queue command
 */
#define CF_EID_ERR_CMD_WQ_OPEN (139)

/**
 * \brief CF Write Queue Command RX Active File Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of file write call for RX active transactions during processing of write queue command
 */
#define CF_EID_ERR_CMD_WQ_WRITEQ_RX (140)

/**
 * \brief CF Write Queue Command RX History File Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of file write call for RX history during processing of write queue command
 */
#define CF_EID_ERR_CMD_WQ_WRITEHIST_RX (141)

/**
 * \brief CF Write Queue Command TX Active File Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of file write call for TX active transactions during processing of write queue command
 */
#define CF_EID_ERR_CMD_WQ_WRITEQ_TX (142)

/**
 * \brief CF Write Queue Command TX Pending File Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of file write call for TX pending transactions during processing of write queue command
 */
#define CF_EID_ERR_CMD_WQ_WRITEQ_PEND (143)

/**
 * \brief CF Write Queue Command TX History File Write Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failure of file write call for TX history during processing of write queue command
 */
#define CF_EID_ERR_CMD_WQ_WRITEHIST_TX (144)

/**
 * \brief CF Set Parameter Command Parameter Validation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Parameter validation failed during processing of set parameter command
 */
#define CF_EID_ERR_CMD_GETSET_VALIDATE (145)

/**
 * \brief CF Set/Get Parameter Command Invalid Parameter ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid parameter id value received in set or get parameter command
 */
#define CF_EID_ERR_CMD_GETSET_PARAM (146)

/**
 * \brief CF Set/Get Parameter Command Invalid Channel Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid channel value received in set or get parameter command
 */
#define CF_EID_ERR_CMD_GETSET_CHAN (147)

/**
 * \brief CF Enable Engine Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Failed to initialize engine when processing engine enable command
 */
#define CF_EID_ERR_CMD_ENABLE_ENGINE (148)

/**
 * \brief CF Enable Engine Command Engine Already Enabled Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Enable engine command received while engine is already enabled
 */
#define CF_EID_ERR_CMD_ENG_ALREADY_ENA (149)

/**
 * \brief CF Disable Engine Command Engine Already Disabled Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Disable engine command received while engine is already disabled
 */
#define CF_EID_ERR_CMD_ENG_ALREADY_DIS (150)

/**
 * \brief CF Command Length Verification Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Received command length verification failure
 */
#define CF_EID_ERR_CMD_GCMD_LEN (151)

/**
 * \brief CF Command Code Invalid Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Received command code unrecognized
 */
#define CF_EID_ERR_CMD_GCMD_CC (152)

/**
 * \brief CF Write Entry To File Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Write entry to file did not match expected length
 */
#define CF_EID_ERR_CMD_WHIST_WRITE (153)

/**
 * \brief CF Playback Dir Or TX File Command Bad Parameter Event ID
 *
 *  \par Type:  ERROR
 *
 *  \par Cause:
 *
 *  Bad parameter received in playback directory or transfer file command
 */
#define CF_EID_ERR_CMD_BAD_PARAM (154)

/**
 * \brief CF Cancel Command No Matching Transaction Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Cancel command received without a matching transaction
 */
#define CF_EID_ERR_CMD_CANCEL_CHAN (155)

/**
 * \brief CF Abandon Command No Matching Transaction Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Abandon command received without a matching transaction
 */
#define CF_EID_ERR_CMD_ABANDON_CHAN (156)

/**
 * \brief CF Transfer File Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Transfer file command was unsuccessful
 */
#define CF_EID_ERR_CMD_TX_FILE (157)

/**
 * \brief CF Playback Directory Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Playback directory command was unsuccessful
 */
#define CF_EID_ERR_CMD_PLAYBACK_DIR (158)

/**
 * \brief CF Freeze Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Freeze command was unsuccessful
 */
#define CF_EID_ERR_CMD_FREEZE (159)

/**
 * \brief CF Thaw Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Thaw command was unsuccessful
 */
#define CF_EID_ERR_CMD_THAW (160)

/**
 * \brief CF Enable Dequeue Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Enable Dequeue command was unsuccessful
 */
#define CF_EID_ERR_CMD_ENABLE_DEQUEUE (161)

/**
 * \brief CF Disable Dequeue Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Disable dequeue command was unsuccessful
 */
#define CF_EID_ERR_CMD_DISABLE_DEQUEUE (162)

/**
 * \brief CF Enable Polldir Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Enable polldir command was unsuccessful
 */
#define CF_EID_ERR_CMD_ENABLE_POLLDIR (163)

/**
 * \brief CF Disable Polldir Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Disable polldir command was unsuccessful
 */
#define CF_EID_ERR_CMD_DISABLE_POLLDIR (164)

/**
 * \brief CF Purge Queue Command Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Purge queue command was unsuccessful
 */
#define CF_EID_ERR_CMD_PURGE_QUEUE (165)

/**\}*/

#endif /* !CF_EVENTS_H */
