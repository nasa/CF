/************************************************************************
** File: cf_cfdp_s.c
**
** NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
** Application version 3.0.0”
** Copyright © 2019 United States Government as represented by the
** Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
** Licensed under the Apache License, Version 2.0 (the "License"); you may
** not use this file except in compliance with the License. You may obtain
** a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**
** Purpose:
**  The CF Application CFDP send logic source file
**
**  Handles all CFDP engine functionality specific to TX transactions.
**
**
**
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include "cf_cfdp_s.h"
#include "cf_cfdp_dispatch.h"

#include <stdio.h>
#include <string.h>
#include "cf_assert.h"

/************************************************************************/
/** \brief CFDP S1 transaction reset function.
**
**  \par Description
**       All S transactions use this call to indicate the transaction
**       state can be returned to the system. While this function currently
**       only calls CF_CFDP_ResetTransaction(), it is here as a placeholder.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static inline void CF_CFDP_S_Reset(CF_Transaction_t *t)
{
    CF_CFDP_ResetTransaction(t, 1);
}

/************************************************************************/
/** \brief Send an eof pdu.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SendRet_SUCCESS on success. \endcode
**  \retcode CF_SendRet_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SendRet_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
CF_SendRet_t CF_CFDP_S_SendEof(CF_Transaction_t *t)
{
    if (!t->flags.com.crc_calc)
    {
        CF_CRC_Finalize(&t->crc);
        t->flags.com.crc_calc = 1;
    }
    return CF_CFDP_SendEof(t);
}

/************************************************************************/
/** \brief Sends an eof for S1.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S1_SubstateSendEof(CF_Transaction_t *t)
{
    /* this looks weird, but the idea is we want to reset the transaction if some error occurs while sending
     * and we want to reset the transaction if no error occurs. But, if we couldn't send because there are
     * no buffers, then we need to try and send again next time. */
    if (CF_CFDP_S_SendEof(t) != CF_SendRet_NO_MSG)
    {
        CF_CFDP_S_Reset(t); /* all done, so clean up */
    }
}

/************************************************************************/
/** \brief Triggers tick processing to send an EOF and wait for EOF-ACK for S2
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_SubstateSendEof(CF_Transaction_t *t)
{
    t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;
    t->flags.com.ack_timer_armed = 1; /* will cause tick to see ack_timer as expired, and act */

    /* no longer need to send file data PDU except in the case of NAK response */

    /* move this transaction off Q_PEND */
    CF_DequeueTransaction(t);
    CF_InsertSortPrio(t, CF_QueueIdx_TXW);
}

/************************************************************************/
/** \brief Helper function to populate the pdu with file data and send it.
**
**  \par Description
**       This function checks the file offset cache and if the desired
**       location is where the file offset is, it can skip a seek() call.
**       The file is read into the filedata pdu and then the pdu is sent.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt The number of bytes sent in the file data PDU. \endcode
**  \endreturns
**
*************************************************************************/
/* if bytes_to_read is 0, then read max possible */
int32 CF_CFDP_S_SendFileData(CF_Transaction_t *t, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc)
{
    int32                           ret = -1;
    CF_Logical_PduBuffer_t         *ph  = CF_CFDP_ConstructPduHeader(t, 0, CF_AppData.config_table->local_eid,
                                                                     t->history->peer_eid, 0, t->history->seq_num, 1);
    CF_Logical_PduFileDataHeader_t *fd;
    size_t                          actual_bytes;
    void                           *data_ptr;

    if (!ph)
    {
        ret = 0;
        goto err_out; /* couldn't get message, so no bytes sent. will try again next time */
    }
    int status;

    fd = &ph->int_header.fd;

    /* need to encode data header up to this point to figure out where data needs to get copied to */
    fd->offset = foffs;
    CF_CFDP_EncodeFileDataHeader(ph->penc, ph->pdu_header.segment_meta_flag, fd);

    /*
     * the actual bytes to read is the smallest of these:
     *  - passed-in size
     *  - outgoing_file_chunk_size from configuration
     *  - amount of space actually available in the PDU after encoding the headers
     */
    actual_bytes = CF_CODEC_GET_REMAIN(ph->penc);
    if (actual_bytes > bytes_to_read)
    {
        actual_bytes = bytes_to_read;
    }
    if (actual_bytes > CF_AppData.config_table->outgoing_file_chunk_size)
    {
        actual_bytes = CF_AppData.config_table->outgoing_file_chunk_size;
    }

    /*
     * The call to CF_CFDP_DoEncodeChunk() should never fail because actual_bytes is
     * guaranteed to be less than or equal to the remaining space in the encode buffer.
     */
    data_ptr = CF_CFDP_DoEncodeChunk(ph->penc, actual_bytes);

    /*
     * save off a pointer to the data for future reference.
     * This isn't encoded into the output PDU, but it allows a future step (such as CRC)
     * to easily find and read the data blob in this PDU.
     */
    fd->data_len = actual_bytes;
    fd->data_ptr = data_ptr;

    if (t->state_data.s.cached_pos != foffs)
    {
        status = CF_WrappedLseek(t->fd, foffs, OS_SEEK_SET);
        if (status != foffs)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_FD, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): error seeking to offset %ld, got %ld", (t->state == CF_TxnState_S2),
                              (unsigned int)t->history->src_eid, (unsigned int)t->history->seq_num, (long)foffs,
                              (long)status);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
            goto err_out;
        }
    }

    status = CF_WrappedRead(t->fd, data_ptr, actual_bytes);
    if (status != actual_bytes)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_READ, CFE_EVS_EventType_ERROR,
                          "CF S%d(%u:%u): error reading bytes: expected %ld, got %ld", (t->state == CF_TxnState_S2),
                          (unsigned int)t->history->src_eid, (unsigned int)t->history->seq_num, (long)actual_bytes,
                          (long)status);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_read;
        goto err_out;
    }

    t->state_data.s.cached_pos += status;
    status = CF_CFDP_SendFd(t, ph);
    if (status == CF_SendRet_NO_MSG)
    {
        ret = 0; /* no bytes were processed */
        goto err_out;
    }
    else if (status == CF_SendRet_ERROR)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEND_FD, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): error sending fd",
                          (t->state == CF_TxnState_S2), t->history->src_eid, t->history->seq_num);
        goto err_out;
    }
    else
    {
        /* don't care about other cases */
    }

    CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes += actual_bytes;

    CF_Assert((foffs + actual_bytes) <= t->fsize); /* sanity check */
    if (calc_crc)
    {
        CF_CRC_Digest(&t->crc, fd->data_ptr, fd->data_len);
    }

    ret = actual_bytes;

err_out:
    return ret;
}

/************************************************************************/
/** \brief Standard state function to send the next file data PDU for active transaction.
**
**  \par Description
**       During the transfer of active transaction file data pdus, the file
**       offset is saved. This function sends the next chunk of data. If
**       the file offset equals the file size, then transition to the EOF
**       state.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
/* regular filedata send
 * based on t->foffs for current offset
 * checks for EOF and changes state if necessary */
void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *t)
{
    int32 bytes_processed = CF_CFDP_S_SendFileData(t, t->foffs, (t->fsize - t->foffs), 1);

    if (bytes_processed > 0)
    {
        t->foffs += bytes_processed;
        if (t->foffs == t->fsize)
        {
            /* file is done */
            t->state_data.s.sub_state = CF_TxSubState_EOF;
        }
    }
    else if (bytes_processed < 0)
    {
        /* IO error -- change state and send EOF */
        t->history->cc            = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
        t->state_data.s.sub_state = CF_TxSubState_EOF;
    }
    else
    {
        /* don't care about other cases */
    }
}

/************************************************************************/
/** \brief Respond to a nak by sending filedata pdus as response.
**
**  \par Description
**       Checks to see if a metadata pdu or filedata re-transmits must
**       occur.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 if no NAK processed. 1 if NAK processed. <0 if error. \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_S_CheckAndRespondNak(CF_Transaction_t *t)
{
    const CF_Chunk_t *c;
    int               ret = 0;

    if (t->flags.tx.md_need_send)
    {
        CF_SendRet_t sret = CF_CFDP_SendMd(t);
        if (sret == CF_SendRet_ERROR)
        {
            ret = -1; /* error occurred */
            goto err_out;
        }
        else if (sret == CF_SendRet_SUCCESS)
        {
            t->flags.tx.md_need_send = 0;
        }
        else
        {
            /* don't care about other cases */
        }

        /* unless CF_SendRet_ERROR, return 1 to keep caller from sending file data */
        ret = 1; /* 1 means nak processed, so don't send filedata */
    }
    else if ((c = CF_ChunkList_GetFirstChunk(&t->chunks->chunks)))
    {
        ret = CF_CFDP_S_SendFileData(t, c->offset, c->size, 0);
        if (ret > 0)
        {
            CF_ChunkList_RemoveFromFirst(&t->chunks->chunks, ret);
            ret = 1; /* processed nak, so caller doesn't send file data */
        }
        else if (ret < 0)
        {
            ret = -1; /* error occurred */
        }
        else
        {
            /* nothing to do if ret==0, since nothing was sent */
        }
    }

err_out:
    return ret;
}

/************************************************************************/
/** \brief Send filedata handling for S2.
**
**  \par Description
**       S2 will either respond to a NAK by sending retransmits, or in
**       absence of a NAK, it will send more of the original file data.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_SubstateSendFileData(CF_Transaction_t *t)
{
    int ret = CF_CFDP_S_CheckAndRespondNak(t);

    if (!ret)
    {
        CF_CFDP_S_SubstateSendFileData(t);
    }
    else if (ret < 0)
    {
        CF_CFDP_S_Reset(t);
    }
    else
    {
        /* don't care about other cases */
    }
}

/************************************************************************/
/** \brief Send metadata PDU.
**
**  \par Description
**       Construct and send a metadata PDU. This function determines the
**       size of the file to put in the metadata PDU.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_SubstateSendMetadata(CF_Transaction_t *t)
{
    int          status;
    CF_SendRet_t sret;

    if (!OS_ObjectIdDefined(t->fd))
    {
        int32 ret;

        if (OS_FileOpenCheck(t->history->fnames.src_filename) == OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_ALREADY_OPEN, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): file %s already open", (t->state == CF_TxnState_S2), t->history->src_eid,
                              t->history->seq_num, t->history->fnames.src_filename);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
            goto err_out;
        }

        ret = CF_WrappedOpenCreate(&t->fd, t->history->fnames.src_filename, OS_FILE_FLAG_NONE, OS_READ_ONLY);
        if (ret < 0)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_OPEN, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): failed to open file %s, error=0x%08x", (t->state == CF_TxnState_S2),
                              t->history->src_eid, t->history->seq_num, t->history->fnames.src_filename, ret);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
            t->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
            goto err_out;
        }

        status = CF_WrappedLseek(t->fd, 0, OS_SEEK_END);
        if (status < 0)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_END, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): failed to seek end file %s, error=0x%08x", (t->state == CF_TxnState_S2),
                              t->history->src_eid, t->history->seq_num, t->history->fnames.src_filename, status);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
            goto err_out;
        }

        t->fsize = status;

        status = CF_WrappedLseek(t->fd, 0, OS_SEEK_SET);
        if (status != 0)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_BEG, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): failed to seek begin file %s, got 0x%08x", (t->state == CF_TxnState_S2),
                              t->history->src_eid, t->history->seq_num, t->history->fnames.src_filename, status);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
            goto err_out;
        }
    }

    sret = CF_CFDP_SendMd(t);
    if (sret == CF_SendRet_ERROR)
    {
        /* failed to send md */
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEND_MD, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): failed to send md",
                          (t->state == CF_TxnState_S2), t->history->src_eid, t->history->seq_num);
        goto err_out;
    }
    else if (sret == CF_SendRet_SUCCESS)
    {
        /* once metadata is sent, switch to filedata mode */
        t->state_data.s.sub_state = CF_TxSubState_FILEDATA;
    }
    else
    {
        /* if sret==CF_SendRet_NO_MSG, then try to send md again next cycle */
    }

    /* don't need CF_CRC_Start() since taken care of by reset_cfdp() */
    /*CF_CRC_Start(&t->crc);*/
    return;

err_out:
    t->history->cc = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
    CF_CFDP_S_Reset(t);
}

/************************************************************************/
/** \brief Send FIN-ACK packet for S2.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_SubstateSendFinAck(CF_Transaction_t *t)
{
    /* if send, or error, reset. if no message, try again next cycle */
    if (CF_CFDP_SendAck(t, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_FIN, t->state_data.s.s2.fin_cc,
                        t->history->peer_eid, t->history->seq_num) != CF_SendRet_NO_MSG)
    {
        CF_CFDP_S_Reset(t);
    }
}

/************************************************************************/
/** \brief A fin was received before file complete, so abandon the transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_EarlyFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* received early fin, so just cancel */
    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_EARLY_FIN, CFE_EVS_EventType_ERROR,
                      "CF S%d(%u:%u): got early fin -- cancelling", (t->state == CF_TxnState_S2), t->history->src_eid,
                      t->history->seq_num);
    CF_CFDP_S_Reset(t);
}

/************************************************************************/
/** \brief S2 received FIN, so set flag to send FIN-ACK.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Fin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    t->state_data.s.s2.fin_cc = ph->int_header.fin.cc;
    t->state_data.s.sub_state = CF_TxSubState_SEND_FIN_ACK;
}

/************************************************************************/
/** \brief S2 NAK pdu received handling.
**
**  \par Description
**       Stores the segment requests from the NAK packet in the chunks
**       structure. These can be used to generate re-transmit filedata
**       PDUs.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Nak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_SegmentRequest_t *sr;
    const CF_Logical_PduNak_t         *nak;
    uint8                              counter;
    uint8                              bad_sr;

    bad_sr = 0;

    /* this function is only invoked for NAK PDU types */
    nak = &ph->int_header.nak;

    if (nak->segment_list.num_segments > 0)
    {
        for (counter = 0; counter < nak->segment_list.num_segments; ++counter)
        {
            sr = &nak->segment_list.segments[counter];

            if (sr->offset_start == 0 && sr->offset_end == 0)
            {
                /* need to re-send metadata pdu */
                t->flags.tx.md_need_send = 1;
            }
            else
            {
                if (sr->offset_end < sr->offset_start)
                {
                    ++bad_sr;
                    continue;
                }

                /* overflow probably won't be an issue */
                if (sr->offset_end > t->fsize)
                {
                    ++bad_sr;
                    continue;
                }

                /* insert gap data in chunks */
                CF_ChunkListAdd(&t->chunks->chunks, sr->offset_start, sr->offset_end - sr->offset_start);
            }
        }

        CF_AppData.hk.channel_hk[t->chan_num].counters.recv.nak_segment_requests += nak->segment_list.num_segments;
        if (bad_sr)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_INVALID_SR, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): received %d invalid nak segment requests", (t->state == CF_TxnState_S2),
                              t->history->src_eid, t->history->seq_num, bad_sr);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_PDU_NAK, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): received invalid nak pdu",
                          (t->state == CF_TxnState_S2), t->history->src_eid, t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
}

/************************************************************************/
/** \brief S2 NAK handling but with arming the NAK timer.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Nak_Arm(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    CF_CFDP_ArmAckTimer(t);
    CF_CFDP_S2_Nak(t, ph);
}

/************************************************************************/
/** \brief S2 received ack pdu in wait for EOF-ACK state.
**
**  \par Description
**       This function will trigger a state transition to CF_TxSubState_WAIT_FOR_FIN,
**       which waits for a FIN pdu.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_WaitForEofAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    if (!CF_CFDP_RecvAck(t, ph))
    {
        /* don't send fin if error. Don't check the eof CC, just go with
         * the stored one we sent before */
        if (t->history->cc != CF_CFDP_ConditionCode_NO_ERROR)
        {
            CF_CFDP_S_Reset(t);
        }
        else
        {
            t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_FIN;
            t->flags.com.ack_timer_armed = 0; /* just wait for fin now, nothing to re-send */
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_PDU_EOF, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): received invalid eof pdu",
                          (t->state == CF_TxnState_S2), t->history->src_eid, t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
}

/************************************************************************/
/** \brief S1 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* s1 doesn't need to receive anything */
    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {{NULL}};
    CF_CFDP_S_DispatchRecv(t, ph, &substate_fns);
}

/************************************************************************/
/** \brief S2 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_meta      = {.fdirective = {
                                                                     [CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_EarlyFin,
                                                                 }};
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_fd_or_eof = {
        .fdirective = {
            [CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_EarlyFin, [CF_CFDP_FileDirective_NAK] = CF_CFDP_S2_Nak}};
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_wait_eof_ack = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_Fin,
                       [CF_CFDP_FileDirective_ACK] = CF_CFDP_S2_WaitForEofAck,
                       [CF_CFDP_FileDirective_NAK] = CF_CFDP_S2_Nak_Arm}};
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_wait_fin = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_Fin, [CF_CFDP_FileDirective_NAK] = CF_CFDP_S2_Nak_Arm}};
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_fin_ack = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S2_Fin}};

    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {
        .substate = {
            [CF_TxSubState_METADATA]         = &s2_meta,
            [CF_TxSubState_FILEDATA]         = &s2_fd_or_eof,
            [CF_TxSubState_EOF]              = &s2_fd_or_eof,
            [CF_TxSubState_WAIT_FOR_EOF_ACK] = &s2_wait_eof_ack,
            [CF_TxSubState_WAIT_FOR_FIN]     = &s2_wait_fin,
            [CF_TxSubState_SEND_FIN_ACK]     = &s2_fin_ack,
        }};

    CF_CFDP_S_DispatchRecv(t, ph, &substate_fns);
}

/************************************************************************/
/** \brief S1 dispatch function.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S1_Tx(CF_Transaction_t *t)
{
    static const CF_CFDP_S_SubstateSendDispatchTable_t substate_fns = {
        .substate = {
            [CF_TxSubState_METADATA] = CF_CFDP_S_SubstateSendMetadata,
            [CF_TxSubState_FILEDATA] = CF_CFDP_S_SubstateSendFileData,
            [CF_TxSubState_EOF]      = CF_CFDP_S1_SubstateSendEof,
        }};

    CF_CFDP_S_DispatchTransmit(t, &substate_fns);
}

/************************************************************************/
/** \brief S2 dispatch function.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Tx(CF_Transaction_t *t)
{
    static const CF_CFDP_S_SubstateSendDispatchTable_t substate_fns = {
        .substate = {
            [CF_TxSubState_METADATA] = CF_CFDP_S_SubstateSendMetadata,
            [CF_TxSubState_FILEDATA] = CF_CFDP_S2_SubstateSendFileData,
            [CF_TxSubState_EOF]      = CF_CFDP_S2_SubstateSendEof,
        }};

    CF_CFDP_S_DispatchTransmit(t, &substate_fns);
}

/************************************************************************/
/** \brief Cancel an S transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_Cancel(CF_Transaction_t *t)
{
    if (t->state_data.s.sub_state < CF_TxSubState_EOF)
    {
        /* if state has not reached CF_TxSubState_EOF, then set it to CF_TxSubState_EOF now. */
        t->state_data.s.sub_state = CF_TxSubState_EOF;
    }
}

/************************************************************************/
/** \brief Perform tick (time-based) processing for S transactions.
**
**  \par Description
**       This function is called on every transaction by the engine on
**       every CF wakeup. This is where flags are checked to send EOF or
**       FIN-ACK. If nothing else is sent, it checks to see if a NAK
**       retransmit must occur.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. cont is unused, so may be NULL
**
*************************************************************************/
void CF_CFDP_S_Tick(CF_Transaction_t *t, int *cont /* unused */)
{
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */
    /* at each tick, various timers used by S are checked */
    /* first, check inactivity timer */
    if (t->state == CF_TxnState_S2)
    {
        if (CF_Timer_Expired(&t->inactivity_timer))
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_INACT_TIMER, CFE_EVS_EventType_ERROR,
                              "CF S2(%u:%u): inactivity timer expired", t->history->src_eid, t->history->seq_num);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.inactivity_timer;
            CF_CFDP_S_Reset(t);
        }
        else
        {
            CF_Timer_Tick(&t->inactivity_timer);

            if (t->flags.com.ack_timer_armed)
            {
                if (CF_Timer_Expired(&t->ack_timer))
                {
                    if (t->state_data.s.sub_state == CF_TxSubState_WAIT_FOR_EOF_ACK)
                    {
                        if (++t->state_data.s.s2.acknak_count >= CF_AppData.config_table->ack_limit)
                        {
                            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_ACK_LIMIT, CFE_EVS_EventType_ERROR,
                                              "CF S2(%u:%u), ack limit reached, no eof-ack", t->history->src_eid,
                                              t->history->seq_num);
                            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.ack_limit;

                            /* no reason to reset this timer, as it isn't used again */
                            CF_CFDP_S_Reset(t);
                            goto err_out; /* must exit after reset */
                        }
                        else
                        {
                            CF_SendRet_t sret = CF_CFDP_S_SendEof(t);
                            if (sret == CF_SendRet_NO_MSG)
                            {
                                goto err_out; /* try next tick */
                            }
                            else if (sret == CF_SendRet_ERROR)
                            {
                                CF_CFDP_S_Reset(t); /* can't go on, error occurred */
                                goto err_out;       /* must exit after reset */
                            }

                            CF_CFDP_ArmAckTimer(t); /* re-arm ack timer */
                        }
                    }
                }
                else
                {
                    CF_Timer_Tick(&t->ack_timer);
                }
            }

            if (t->state_data.s.sub_state == CF_TxSubState_SEND_FIN_ACK)
            {
                CF_CFDP_S_SubstateSendFinAck(t);
            }
        }
    }

err_out:;
}

/************************************************************************/
/** \brief Perform NAK response for TX transactions
**
**  \par Description
**       This function is called at tick processing time to send pending
**       NAK responses. It indicates "cont" is 1 if there are more responses
**       left to send.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. cont must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *t, int *cont)
{
    int ret = CF_CFDP_S_CheckAndRespondNak(t);

    if (ret == 1)
        *cont = 1; /* cause dispatcher to re-enter this wakeup */
}
