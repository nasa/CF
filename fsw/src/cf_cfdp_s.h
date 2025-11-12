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
 * Implementation related to CFDP Send File transactions
 *
 * This file contains various state handling routines for
 * transactions which are sending a file.
 */

#ifndef CF_CFDP_S_H
#define CF_CFDP_S_H

#include "cf_cfdp_types.h"

/************************************************************************/
/** @brief S1 receive PDU processing.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 receive PDU processing.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Perform acknowledgement timer tick (time-based) processing for S transactions.
 *
 * @par Description
 *       This is invoked as part of overall timer tick processing if the transaction
 *       has some sort of acknowledgement pending from the remote.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL
 *
 * @param txn  Pointer to the transaction object
 *
 */
void CF_CFDP_S_AckTimerTick(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Perform tick (time-based) processing for S transactions.
 *
 * @par Description
 *       This function is called on every transaction by the engine on
 *       every CF wakeup. This is where flags are checked to send EOF or
 *       FIN-ACK. If nothing else is sent, it checks to see if a NAK
 *       retransmit must occur.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. cont is unused, so may be NULL
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S_Tick(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Generate protocol messages for TX transactions
 *
 * @par Description
 *       This function is called at tick processing time to send pending
 *       protocol messages such as MD, EOF, and FIN-ACK
 *
 *       These messages are considered higher priority than any file data
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S_Tick_Maintenance(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Generate file data PDUs from NAKs
 *
 * @par Description
 *      Generates file data PDUs for chunks that the peer has NAKed
 *
 *      Responding to NAK is considered higher priority than sending
 *      new file data, but lower priority than maintenance PDUs.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Initialize a transaction structure for S.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S_Init(CF_Transaction_t *txn);

/***********************************************************************
 *
 * Handler routines for send-file transactions
 * These are not called from outside this module, but are declared here so they can be unit tested
 *
 ************************************************************************/

/************************************************************************/
/** @brief Helper function to populate the PDU with file data and send it.
 *
 * @par Description
 *       This function checks the file offset cache and if the desired
 *       location is where the file offset is, it can skip a seek() call.
 *       The file is read into the filedata PDU and then the PDU is sent.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @returns The number of bytes sent in the file data PDU (CFE_SUCCESS,
 *          i.e. 0, if no bytes were processed), or CF_ERROR on error
 *
 * @param txn     Pointer to the transaction object
 * @param foffs Position in file to send data from
 * @param bytes_to_read Number of bytes to send (maximum)
 * @param calc_crc Enable CRC/Checksum calculation
 *
 */
CFE_Status_t CF_CFDP_S_SendFileData(CF_Transaction_t *txn, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc);

/************************************************************************/
/** @brief Standard state function to send the next file data PDU for active transaction.
 *
 * @par Description
 *       During the transfer of active transaction file data PDUs, the file
 *       offset is saved. This function sends the next chunk of data. If
 *       the file offset equals the file size, then transition to the EOF
 *       state.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn     Pointer to the transaction object
 */
void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *txn);

/************************************************************************/
/** @brief A FIN was received before file complete, so abandon the transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S_SubstateEarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 received FIN, so set flag to send FIN-ACK.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S_SubstateRecvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 NAK PDU received handling.
 *
 * @par Description
 *       Stores the segment requests from the NAK packet in the chunks
 *       structure. These can be used to generate re-transmit filedata
 *       PDUs.
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_SubstateNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief S2 received ACK PDU.
 *
 * @par Description
 *       Handles reception of an ACK PDU
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL. ph must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 * @param ph Pointer to the PDU information
 */
void CF_CFDP_S2_SubstateEofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Run TX state machine for the transaction
 *
 * @par Description
 *      Checks flags on the transaction and execute state transitions
 *
 * @par Assumptions, External Events, and Notes:
 *       txn must not be NULL.
 *
 * @param txn  Pointer to the transaction object
 */
void CF_CFDP_S_CheckState(CF_Transaction_t *txn);

/************************************************************************/
/** @brief Remove/Move file after transaction
 *
 * Determines disposition of local file after file transfer completion.
 *
 * For a sender:
 *   - If the transfer is successful and the "keep" flag is false, then it applies
 *     the local file deletion policy (either delete directly or move to recycle dir)
 *   - If the transfer is not successful or the "keep" flag is true, then do nothing
 *
 * @par Assumptions, External Events, and Notes:
 *
 * @param txn Transaction object pointer
 *
 */
void CF_CFDP_S_HandleFileRetention(CF_Transaction_t *txn);

#endif /* !CF_CFDP_S_H */
