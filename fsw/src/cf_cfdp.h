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
 *  The CF Application cfdp engine and packet parsing header file
 */

#ifndef CF_CFDP_H
#define CF_CFDP_H

#include "cf_cfdp_types.h"

/**
 * @brief Structure for use with the CF_CFDP_CycleTx() function
 */
typedef struct CF_CFDP_CycleTx_args
{
    CF_Channel_t *c;       /**< channel structure */
    int           ran_one; /**< should be set to 1 if a transaction was cycled */
} CF_CFDP_CycleTx_args_t;

/**
 * @brief Structure for use with the CF_CFDP_DoTick() function
 */
typedef struct CF_CFDP_Tick_args
{
    CF_Channel_t *c;                       /* IN param */
    void (*fn)(CF_Transaction_t *, int *); /* IN param */
    int early_exit;                        /* OUT param */
    int cont;                              /* if 1, then re-traverse the list */
} CF_CFDP_Tick_args_t;

/********************************************************************************/
/**
 * @brief Initiate the process of encoding a new PDU to send
 *
 * This resets the encoder and PDU buffer to initial values, and prepares for encoding a new PDU
 * for sending to a remote entity.
 *
 * @param penc           Encoder state structure, will be reset/initialized by this call to point to msgbuf.
 * @param msgbuf         Pointer to encapsulation message, in this case a CFE software bus message
 * @param ph             Pointer to logical PDU buffer content, will be cleared to all zero by this call
 * @param encap_hdr_size Offset of first CFDP PDU octet within buffer
 * @param msgbuf_size    Allocated size of msgbuf encapsulation structure (encoding cannot exceed this)
 */
void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size);

/********************************************************************************/
/**
 * @brief Initiate the process of decoding a receieved PDU
 *
 * This resets the decoder and PDU buffer to initial values, and prepares for decoding a new PDU
 * that was received from a remote entity.
 *
 * @param pdec           Decoder state structure, will be reset/initialized by this call to point to msgbuf.
 * @param msgbuf         Pointer to encapsulation message, in this case a CFE software bus message
 * @param ph             Pointer to logical PDU buffer content, will be cleared to all zero by this call
 * @param encap_hdr_size Offset of first CFDP PDU octet within buffer
 * @param msgbuf_size    Total size of msgbuf encapsulation structure (decoding cannot exceed this)
 */
void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, const void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size);

/* engine execution functions */

/************************************************************************/
/** @brief Reset a transaction and all its internals to an unused state.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t  Pointer to the transaction object
 * @param keep_history Whether the transaction info should be preserved in history
 */
void CF_CFDP_ResetTransaction(CF_Transaction_t *t, int keep_history);

/************************************************************************/
/** @brief Initialization function for the cfdp engine
 *
 * @par Description
 *       Performs all initialization of the CFDP engine
 *
 * @par Assumptions, External Events, and Notes:
 *       Only called once.
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESSS
 * @returns anything else on error.
 *
 */
int32 CF_CFDP_InitEngine(void);

/************************************************************************/
/** @brief Cycle the engine. Called once per wakeup.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 */
void CF_CFDP_CycleEngine(void);

/************************************************************************/
/** @brief Disables the cfdp engine and resets all state in it.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 */
void CF_CFDP_DisableEngine(void);

/************************************************************************/
/** @brief Begin transmit of a file.
 *
 * @par Description
 *       This function sets up a transaction for and starts transmit of
 *       the given filename.
 *
 * @par Assumptions, External Events, and Notes:
 *       src_filename must not be NULL. dst_filename must not be NULL.
 *
 * @param src_filename  Local filename
 * @param dst_filename  Remote filename
 * @param cfdp_class    Whether to perform a class 1 or class 2 transfer
 * @param keep          Whether to keep or delete the local file after completion
 * @param chan          CF channel number to use
 * @param priority      CF priority level
 * @param dest_id       Entity ID of remote receiver
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESSS
 * @returns Anything else on error.
 */
int32 CF_CFDP_TxFile(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                     uint8 chan, uint8 priority, CF_EntityId_t dest_id);

/************************************************************************/
/** @brief Begin transmit of a directory.
 *
 * @par Description
 *       This function sets up CF_Playback_t structure with state so it can
 *       become part of the directory polling done at each engine cycle.
 *
 * @par Assumptions, External Events, and Notes:
 *       src_filename must not be NULL. dst_filename must not be NULL.
 *
 * @param src_filename  Local filename
 * @param dst_filename  Remote filename
 * @param cfdp_class    Whether to perform a class 1 or class 2 transfer
 * @param keep          Whether to keep or delete the local file after completion
 * @param chan          CF channel number to use
 * @param priority      CF priority level
 * @param dest_id       Entity ID of remote receiver
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESSS
 * @returns Anything else on error.
 */
int32 CF_CFDP_PlaybackDir(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                          uint8 chan, uint8 priority, uint16 dest_id);

/************************************************************************/
/** @brief Build the PDU header in the output buffer to prepare to send a packet.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t              Pointer to the transaction object
 * @param directive_code Code to use for file directive headers (set to 0 for data)
 * @param src_eid        Value to set in source entity ID field
 * @param dst_eid        Value to set in destination entity ID field
 * @param towards_sender Whether this is transmitting toward the sender entity
 * @param tsn            Transaction sequence number to put into PDU
 * @param silent         If true, supresses error event if no message buffer available
 *
 * @returns Pointer to PDU buffer which may be filled with additional data
 * @retval  NULL if no message buffer available
 */
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *t, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent);

/************************************************************************/
/** @brief Build a metadata PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t              Pointer to the transaction object
 *
 * @returns CF_SendRet_t status code
 * @retval CF_SendRet_SUCCESS on success.
 * @retval CF_SendRet_NO_MSG if message buffer cannot be obtained.
 * @retval CF_SendRet_ERROR if an error occurred while building the packet.
 */
CF_SendRet_t CF_CFDP_SendMd(CF_Transaction_t *t);

/************************************************************************/
/** @brief Send a previously-assembled filedata PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t   Pointer to the transaction object
 * @param ph  Pointer to logical PDU buffer content
 *
 * @note Unlike other "send" routines, the file data PDU must be acquired and
 * filled by the caller prior to invoking this routine.  This routine only
 * sends the PDU that was previously allocated and assembled.  As such, the
 * typical failure possibilies do not apply to this call.
 *
 * @returns CF_SendRet_t status code
 * @retval CF_SendRet_SUCCESS on success.
 */
CF_SendRet_t CF_CFDP_SendFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Build a eof PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t   Pointer to the transaction object
 *
 * @returns CF_SendRet_t status code
 * @retval CF_SendRet_SUCCESS on success.
 * @retval CF_SendRet_NO_MSG if message buffer cannot be obtained.
 * @retval CF_SendRet_ERROR if an error occurred while building the packet.
 */
CF_SendRet_t CF_CFDP_SendEof(CF_Transaction_t *t);

/************************************************************************/
/** @brief Build a ack PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @note CF_CFDP_SendAck() takes a CF_TransactionSeq_t instead of getting it from transaction history because
 * of the special case where a FIN-ACK must be sent for an unknown transaction. It's better for
 * long term maintenance to not build an incomplete CF_History_t for it.
 *
 * @param t        Pointer to the transaction object
 * @param ts       Transaction ACK status
 * @param dir_code File directive code being ACK'ed
 * @param cc       Condition code of transaction
 * @param peer_eid Remote entity ID
 * @param tsn      Transaction sequence number
 *
 * @returns CF_SendRet_t status code
 * @retval CF_SendRet_SUCCESS on success.
 * @retval CF_SendRet_NO_MSG if message buffer cannot be obtained.
 * @retval CF_SendRet_ERROR if an error occurred while building the packet.
 *
 */
CF_SendRet_t CF_CFDP_SendAck(CF_Transaction_t *t, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn);

/************************************************************************/
/** @brief Build a fin PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t     Pointer to the transaction object
 * @param dc    Final delivery status code (complete or incomplete)
 * @param fs    Final file status (retained or rejected, etc)
 * @param cc    Final CFDP condition code
 *
 * @returns CF_SendRet_t status code
 * @retval CF_SendRet_SUCCESS on success.
 * @retval CF_SendRet_NO_MSG if message buffer cannot be obtained.
 * @retval CF_SendRet_ERROR if an error occurred while building the packet.
 */
CF_SendRet_t CF_CFDP_SendFin(CF_Transaction_t *t, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc);

/************************************************************************/
/** @brief Send a previously-assembled nak PDU for transmit.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t   Pointer to the transaction object
 * @param ph  Pointer to logical PDU buffer content
 *
 * @note Unlike other "send" routines, the NAK PDU must be acquired and
 * filled by the caller prior to invoking this routine.  This routine only
 * encodes and sends the previously-assembled PDU buffer.  As such, the
 * typical failure possibilies do not apply to this call.
 *
 * @returns CF_SendRet_t status code
 * @retval CF_SendRet_SUCCESS on success.
 */
CF_SendRet_t CF_CFDP_SendNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Appends a single TLV value to the logical PDU data
 *
 * This function implements common functionality between SendEof and SendFin
 * which append a TLV value specifying the faulting entity ID.
 *
 * @par Assumptions, External Events, and Notes:
 *       ptlv_list must not be NULL.
 *       Only CF_CFDP_TLV_TYPE_ENTITY_ID type is currently implemented
 *
 * @param ptlv_list TLV list from current PDU buffer.
 * @param tlv_type  Type of TLV to append.  Currently must be CF_CFDP_TLV_TYPE_ENTITY_ID.
 */
void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type);

/************************************************************************/
/** @brief Unpack a basic PDU header from a received message.
 *
 * @par Description
 *       This interprets the common PDU header and the file directive header
 *       (if applicable) and populates the logical PDU buffer.
 *
 * @par Assumptions, External Events, and Notes:
 *       A new message has been received.
 *
 * @param chan_num The channel number for statistics purposes
 * @param ph       The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval 0 on success
 * @retval -1 on error
 *
 */
int CF_CFDP_RecvPh(uint8 chan_num, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack a metadata PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as a metadata PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval 0 on success
 * @retval -1 on error
 */
int CF_CFDP_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack a file data PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as a file data PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval 0 on success
 * @retval -1 on error
 *
 */
int CF_CFDP_RecvFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack an eof PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as an end of file PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval 0 on success
 * @retval -1 on error
 *
 */
int CF_CFDP_RecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack an ack PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as an acknowledgement PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval 0 on success
 * @retval -1 on error
 *
 */
int CF_CFDP_RecvAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack an fin PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as an final PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval 0 on success
 * @retval -1 on error
 *
 */
int CF_CFDP_RecvFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Unpack a nak PDU from a received message.
 *
 * This should only be invoked for buffers that have been identified
 * as an negative/non-acknowledgement PDU.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 * @returns integer status code
 * @retval 0 on success
 * @retval -1 on error
 *
 */
int CF_CFDP_RecvNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Dispatch received packet to its handler.
 *
 * This dispatches the PDU to the appropriate handler
 * based on the transaction state
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be null. It must be an initialized transaction.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 *
 */
void CF_CFDP_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Cancels a transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 *
 */
void CF_CFDP_CancelTransaction(CF_Transaction_t *t);

/************************************************************************/
/** @brief Helper function to set tx file state in a transaction.
 *
 * This sets various fields inside a newly-allocated transaction
 * structure appropriately for sending a file.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t          Pointer to the transaction state
 * @param cfdp_class Set to class 1 or class 2
 * @param keep       Whether to keep the local file
 * @param chan       CF channel number
 * @param priority   Priority of transfer
 *
 */
void CF_CFDP_InitTxnTxFile(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority);

/* functions to handle LVs (length-value, cfdp spec) */
/* returns number of bytes copied, or -1 on error */

/************************************************************************/
/** @brief Copy string data from a lv (length, value) pair.
 *
 * This copies a string value from an LV pair inside a PDU buffer.
 * In CF this is used for file names embedded within PDUs.
 *
 * @note This function assures that the output string is terminated
 * appropriately, such that it can be used as a normal C string.  As
 * such, the buffer size must be at least 1 byte larger than the maximum
 * string length.
 *
 * @par Assumptions, External Events, and Notes:
 *       src_lv must not be NULL. buf must not be NULL.
 *
 * @param buf        Pointer to buffer to store string
 * @param buf_maxsz  Total size of buffer pointer to by buf (usable size is 1 byte less, for termination)
 * @param src_lv     Pointer to LV pair from logical PDU buffer
 *
 * @returns The resulting string length, NOT including termination character
 * @retval -1 on error
 */
int CF_CFDP_CopyStringFromLV(char *buf, size_t buf_maxsz, const CF_Logical_Lv_t *src_lv);

/************************************************************************/
/** @brief Arm the ack timer
 *
 * @par Description
 *       Helper function to arm the ack timer and set the flag.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t          Pointer to the transaction state
 */
void CF_CFDP_ArmAckTimer(CF_Transaction_t *t);

/************************************************************************/
/** @brief Receive state function to ignore a packet.
 *
 * @par Description
 *       This function signature must match all receive state functions.
 *       The parameter t is ignored here.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 */
void CF_CFDP_RecvDrop(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief Receive state function to process new rx transaction.
 *
 * @par Description
 *       An idle transaction has never had message processing performed on it.
 *       Typically, the first packet received for a transaction would be
 *       the metadata pdu. There's a special case for R2 where the metadata
 *       pdu could be missed, and filedata comes in instead. In that case,
 *       an R2 transaction must still be started.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL. There must be a received message.
 *
 * @param t    Pointer to the transaction state
 * @param ph   The logical PDU buffer being received
 */
void CF_CFDP_RecvIdle(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

/************************************************************************/
/** @brief List traversal function to close all files in all active transactions.
 *
 * This helper is used in conjunction with CF_CList_Traverse().
 *
 * @par Assumptions, External Events, and Notes:
 *       n must not be NULL.
 *
 * @param n       List node pointer
 * @param context Opaque pointer, not used in this function
 *
 * @returns integer traversal code
 * @retval Always CF_LIST_CONT indicate list traversal should not exit early.
 */
int CF_CFDP_CloseFiles(CF_CListNode_t *n, void *context);

/************************************************************************/
/** @brief Cycle the current active tx or make a new one active.
 *
 * @par Description
 *       First traverses all tx transactions on the active queue. If at
 *       least one is found, then it stops. Otherwise it moves a
 *       transaction on the pending queue to the active queue and
 *       tries again to find an active one.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param c Channel to cycle
 */
void CF_CFDP_CycleTx(CF_Channel_t *c);

/************************************************************************/
/** @brief List traversal function that cycles the first active tx.
 *
 * This helper is used in conjunction with CF_CList_Traverse().
 *
 * @par Description
 *       There can only be one active tx transaction per engine cycle.
 *       This function finds the first active, and then sends file
 *       data pdus until there are no outgoing message buffers.
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL. context must not be NULL.
 *
 * @param node    Pointer to list node
 * @param context Pointer to CF_CFDP_CycleTx_args_t object (passed through)
 *
 * @returns integer traversal code
 * @retval CF_CLIST_EXIT when it's found, which terminates list traversal
 * @retval CF_CLIST_CONT when it's isn't found, which causes list traversal to continue
 */
int CF_CFDP_CycleTxFirstActive(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Call R and then S tick functions for all active transactions.
 *
 * @par Description
 *       Traverses all transactions in the RX and TXW queues, and calls
 *       their tick functions. Note that the TXW queue is used twice:
 *       once for regular tick processing, and one for NAK response.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL.
 *
 * @param c Channel to tick
 */
void CF_CFDP_TickTransactions(CF_Channel_t *c);

/************************************************************************/
/** @brief Step each active playback directory.
 *
 * @par Description
 *       Check if a playback directory needs iterated, and if so does, and
 *       if a valid file is found initiates playback on it.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL. p must not be NULL.
 *
 * @param c  The channel associated with the playback
 * @param p  The playback state
 */
void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *c, CF_Playback_t *p);

/************************************************************************/
/** @brief Kick the dir playback if timer elapsed.
 *
 * @par Description
 *       This function waits for the polling directory interval timer,
 *       and if it has expired, starts a playback in the polling directory.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL.
 *
 * @param c  The channel associated with the playback
 */
void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *c);

/************************************************************************/
/** @brief List traversal function that calls a r or s tick function.
 *
 * This helper is used in conjunction with CF_CList_Traverse().
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL. context must not be NULL.
 *
 * @param node    Pointer to list node
 * @param context Pointer to CF_CFDP_Tick_args_t object (passed through)
 *
 * @returns integer traversal code
 * @retval CF_CLIST_EXIT when it's found, which terminates list traversal
 * @retval CF_CLIST_CONT when it's isn't found, which causes list traversal to continue
 */
int CF_CFDP_DoTick(CF_CListNode_t *node, void *context);

#endif /* !CF_CFDP_H */
