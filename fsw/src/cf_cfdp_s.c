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
#include "cf_cfdp_helpers.h"

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
static inline void CF_CFDP_S_Reset(transaction_t *t)
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
**  \retcode CF_SEND_SUCCESS on success. \endcode
**  \retcode CF_SEND_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SEND_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
static cfdp_send_ret_t CF_CFDP_S_SendEof(transaction_t *t)
{
    if (!t->flags.tx.crc_calc)
    {
        CF_CRC_Finalize(&t->crc);
        t->flags.tx.crc_calc = 1;
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
static void CF_CFDP_S1_SubstateSendEof(transaction_t *t)
{
    /* this looks weird, but the idea is we want to reset the transaction if some error occurs while sending
     * and we want to reset the transaction if no error occurs. But, if we couldn't send because there are
     * no buffers, then we need to try and send again next time. */
    if (CF_CFDP_S_SendEof(t) != CF_SEND_NO_MSG)
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
static void CF_CFDP_S2_SubstateSendEof(transaction_t *t)
{
    t->state_data.s.sub_state   = SEND_WAIT_FOR_EOF_ACK;
    t->flags.tx.ack_timer_armed = 1; /* will cause tick to see ack_timer as expired, and act */

    /* no longer need to send file data PDU except in the case of NAK response */

    /* move this transaction off Q_PEND */
    cf_dequeue_transaction(t);
    CF_InsertSortPrio(t, CF_Q_TXW);
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
static int32 CF_CFDP_S_SendFileData(transaction_t *t, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc)
{
    int32         ret = -1;
    pdu_header_t *ph  = CF_CFDP_ConstructPduHeader(t, 0, CF_AppData.config_table->local_eid, t->history->peer_eid, 0,
                                                   t->history->seq_num, 1);
    pdu_fd_t     *fd;
    if (!ph)
    {
        ret = 0;
        goto err_out; /* couldn't get message, so no bytes sent. will try again next time */
    }
    int status;

    fd = STATIC_CAST(ph, pdu_fd_t);

    if (bytes_to_read > CF_AppData.config_table->outgoing_file_chunk_size)
    {
        bytes_to_read = CF_AppData.config_table->outgoing_file_chunk_size;
    }

    if (t->state_data.s.cached_pos != foffs)
    {
        status = CF_WrappedLseek(t->fd, foffs, OS_SEEK_SET);
        if (status != foffs)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_FD, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): error seeking to offset 0x%08x, got 0x%08x", (t->state == CFDP_S2),
                              t->history->src_eid, t->history->seq_num, foffs, status);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
            goto err_out;
        }
    }

    status = CF_WrappedRead(t->fd, fd->fdd.data, bytes_to_read);
    if (status != bytes_to_read)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_READ, CFE_EVS_EventType_ERROR,
                          "CF S%d(%u:%u): error reading bytes: expected 0x%08x, got 0x%08x", (t->state == CFDP_S2),
                          t->history->src_eid, t->history->seq_num, bytes_to_read, status);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_read;
        goto err_out;
    }

    t->state_data.s.cached_pos += status;
    status = CF_CFDP_SendFd(t, foffs, bytes_to_read);
    if (status == CF_SEND_NO_MSG)
    {
        ret = 0; /* no bytes were processed */
        goto err_out;
    }
    else if (status == CF_SEND_ERROR)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEND_FD, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): error sending fd",
                          (t->state == CFDP_S2), t->history->src_eid, t->history->seq_num);
        goto err_out;
    }
    else
    {
        /* don't care about other cases */
    }

    CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes += bytes_to_read;

    CF_Assert((foffs + bytes_to_read) <= t->fsize); /* sanity check */
    if (calc_crc)
    {
        CF_CRC_Digest(&t->crc, fd->fdd.data, bytes_to_read);
    }

    ret = bytes_to_read;

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
static void CF_CFDP_S_SubstateSendFileData(transaction_t *t)
{
    int32 bytes_processed = CF_CFDP_S_SendFileData(t, t->foffs, (t->fsize - t->foffs), 1);

    if (bytes_processed > 0)
    {
        t->foffs += bytes_processed;
        if (t->foffs == t->fsize)
        {
            /* file is done */
            t->state_data.s.sub_state = SEND_EOF;
        }
    }
    else if (bytes_processed < 0)
    {
        /* IO error -- change state and send EOF */
        t->history->cc            = CC_FILESTORE_REJECTION;
        t->state_data.s.sub_state = SEND_EOF;
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
static int CF_CFDP_S_CheckAndRespondNak(transaction_t *t)
{
    const chunk_t *c;
    int            ret = 0;

    if (t->flags.tx.md_need_send)
    {
        cfdp_send_ret_t sret = CF_CFDP_SendMd(t);
        if (sret == CF_SEND_ERROR)
        {
            ret = -1; /* error occurred */
            goto err_out;
        }
        else if (sret == CF_SEND_SUCCESS)
        {
            t->flags.tx.md_need_send = 0;
        }
        else
        {
            /* don't care about other cases */
        }

        /* unless CF_SEND_ERROR, return 1 to keep caller from sending file data */
        ret = 1; /* 1 means nak processed, so don't send filedata */
    }
    else if ((c = CF_Chunks_GetFirstChunk(&t->chunks->chunks)))
    {
        ret = CF_CFDP_S_SendFileData(t, c->offset, c->size, 0);
        if (ret > 0)
        {
            CF_Chunks_RemoveFromFirst(&t->chunks->chunks, ret);
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
static void CF_CFDP_S2_SubstateSendFileData(transaction_t *t)
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
static void CF_CFDP_S_SubstateSendMetadata(transaction_t *t)
{
    int             status;
    cfdp_send_ret_t sret;

    if (!OS_ObjectIdDefined(t->fd))
    {
        int32 ret;

        if (OS_FileOpenCheck(t->history->fnames.src_filename) == OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_ALREADY_OPEN, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): file %s already open", (t->state == CFDP_S2), t->history->src_eid,
                              t->history->seq_num, t->history->fnames.src_filename);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
            goto err_out;
        }

        ret = CF_WrappedOpenCreate(&t->fd, t->history->fnames.src_filename, OS_FILE_FLAG_NONE, OS_READ_ONLY);
        if (ret < 0)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_OPEN, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): failed to open file %s, error=0x%08x", (t->state == CFDP_S2),
                              t->history->src_eid, t->history->seq_num, t->history->fnames.src_filename, ret);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
            t->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
            goto err_out;
        }

        status = CF_WrappedLseek(t->fd, 0, OS_SEEK_END);
        if (status < 0)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_END, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): failed to seek end file %s, error=0x%08x", (t->state == CFDP_S2),
                              t->history->src_eid, t->history->seq_num, t->history->fnames.src_filename, status);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
            goto err_out;
        }

        t->fsize = status;

        status = CF_WrappedLseek(t->fd, 0, OS_SEEK_SET);
        if (status != 0)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_BEG, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): failed to seek begin file %s, got 0x%08x", (t->state == CFDP_S2),
                              t->history->src_eid, t->history->seq_num, t->history->fnames.src_filename, status);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
            goto err_out;
        }
    }

    sret = CF_CFDP_SendMd(t);
    if (sret == CF_SEND_ERROR)
    {
        /* failed to send md */
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEND_MD, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): failed to send md",
                          (t->state == CFDP_S2), t->history->src_eid, t->history->seq_num);
        goto err_out;
    }
    else if (sret == CF_SEND_SUCCESS)
    {
        /* once metadata is sent, switch to filedata mode */
        t->state_data.s.sub_state = SEND_FILEDATA;
    }
    else
    {
        /* if sret==CF_SEND_NO_MSG, then try to send md again next cycle */
    }

    /* don't need CF_CRC_Start() since taken care of by reset_cfdp() */
    /*CF_CRC_Start(&t->crc);*/
    return;

err_out:
    t->history->cc = CC_FILESTORE_REJECTION;
    CF_CFDP_S_Reset(t);
}

/************************************************************************/
/** \brief Send FIN-ACK packet for S2.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_S_SubstateSendFinAck(transaction_t *t)
{
    /* if send, or error, reset. if no message, try again next cycle */
    if (CF_CFDP_SendAck(t, ACK_TS_ACTIVE, PDU_FIN, t->state_data.s.s2.fin_cc, t->history->peer_eid,
                        t->history->seq_num) != CF_SEND_NO_MSG)
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
static void CF_CFDP_S2_EarlyFin(transaction_t *t, const pdu_header_t *ph)
{
    /* received early fin, so just cancel */
    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_EARLY_FIN, CFE_EVS_EventType_ERROR,
                      "CF S%d(%u:%u): got early fin -- cancelling", (t->state == CFDP_S2), t->history->src_eid,
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
static void CF_CFDP_S2_Fin(transaction_t *t, const pdu_header_t *ph)
{
    if (!CF_CFDP_RecvFin())
    {
        t->state_data.s.s2.fin_cc = FGV(STATIC_CAST(ph, pdu_fin_t)->flags, PDU_FLAGS_CC);
        t->state_data.s.sub_state = SEND_SEND_FIN_ACK;
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_PDU_FIN, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): received invalid fin pdu",
                          (t->state == CFDP_S2), t->history->src_eid, t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
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
static void CF_CFDP_S2_Nak(transaction_t *t, const pdu_header_t *ph)
{
    /* temporary function to respond to naks */
    int counter;
    int num_sr;
    int bad_sr = 0;

    if (!CF_CFDP_RecvNak(&num_sr) && num_sr)
    {
        pdu_nak_t *nak = STATIC_CAST(ph, pdu_nak_t);
        CF_Assert(num_sr <= CF_NAK_MAX_SEGMENTS); // sanity check

        for (counter = 0; counter < num_sr; ++counter)
        {
            uint32 offset_start, offset_end;
            cfdp_ldst_uint32(offset_start, nak->segment_requests[counter].offset_start);
            cfdp_ldst_uint32(offset_end, nak->segment_requests[counter].offset_end);
            if (!offset_start && !offset_end)
            {
                /* need to re-send metadata pdu */
                t->flags.tx.md_need_send = 1;
            }
            else
            {
                uint32 start, size;
                cfdp_ldst_uint32(start, /*nak->scope_start+*/ offset_start);
                if (offset_end < offset_start)
                {
                    ++bad_sr;
                    continue;
                }
                cfdp_ldst_uint32(size, offset_end - offset_start);

                /* overflow probably won't be an issue */
                if ((start + size) <= t->fsize)
                {
                    /* insert gap data in chunks */
                    CF_Chunks_Add(&t->chunks->chunks, start, size);
                }
                else
                {
                    ++bad_sr;
                    continue;
                }
            }
        }

        CF_AppData.hk.channel_hk[t->chan_num].counters.recv.nak_segment_requests += num_sr;
        if (bad_sr)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_INVALID_SR, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): received %d invalid nak segment requests", (t->state == CFDP_S2),
                              t->history->src_eid, t->history->seq_num, bad_sr);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_PDU_NAK, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): received invalid nak pdu",
                          (t->state == CFDP_S2), t->history->src_eid, t->history->seq_num);
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
static void CF_CFDP_S2_Nak_Arm(transaction_t *t, const pdu_header_t *ph)
{
    CF_CFDP_ArmAckTimer(t);
    CF_CFDP_S2_Nak(t, ph);
}

/************************************************************************/
/** \brief S2 received ack pdu in wait for EOF-ACK state.
**
**  \par Description
**       This function will trigger a state transition to SEND_WAIT_FOR_FIN,
**       which waits for a FIN pdu.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
static void CF_CFDP_S2_WaitForEofAck(transaction_t *t, const pdu_header_t *ph)
{
    if (!CF_CFDP_RecvAck())
    {
        /* don't send fin if error. Don't check the eof CC, just go with
         * the stored one we sent before */
        if (t->history->cc != CC_NO_ERROR)
        {
            CF_CFDP_S_Reset(t);
        }
        else
        {
            t->state_data.s.sub_state   = SEND_WAIT_FOR_FIN;
            t->flags.tx.ack_timer_armed = 0; /* just wait for fin now, nothing to re-send */
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_PDU_EOF, CFE_EVS_EventType_ERROR, "CF S%d(%u:%u): received invalid eof pdu",
                          (t->state == CFDP_S2), t->history->src_eid, t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
}

/************************************************************************/
/** \brief Dispatch function for all received packets.
**
**  \par Description
**       For either S1 or S2 this function handles common logic for
**       state processing based on current sub-state and the received
**       pdu type.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. fns must not be NULL.
**
*************************************************************************/
static void CF_CFDP_S_DispatchRecv(transaction_t *t,
                                   void (*const fns[SEND_NUM_STATES][PDU_INVALID_MAX])(transaction_t *,
                                                                                       const pdu_header_t *))
{
    CF_Assert(t->state_data.s.sub_state < SEND_NUM_STATES);
    CF_Assert(CF_AppData.engine.in.msg);

    /* at this point, pdu header for recv message is unmarshaled */
    pdu_header_t *ph = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;
    /* send state, so we only care about file directive PDU */
    if (!FGV(ph->flags, PDU_HDR_FLAGS_TYPE))
    {
        pdu_file_directive_header_t *fdh = STATIC_CAST(ph, pdu_file_directive_header_t);
        if (fdh->directive_code < PDU_INVALID_MAX)
        {
            /* check that there's a valid function pointer. if there isn't,
             * then silently ignore. We may want to discuss if it's worth
             * shutting down the whole transation if a PDU is received
             * that doesn't make sense to be received (For example,
             * class 1 CFDP receiving a NAK PDU) but for now, we silently
             * ignore the received packet and keep chugging along. */
            if (fns[t->state_data.s.sub_state][fdh->directive_code])
            {
                fns[t->state_data.s.sub_state][fdh->directive_code](t, ph);
            }
        }
        else
        {
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious;
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_DC_INV, CFE_EVS_EventType_ERROR,
                              "CF S%d(%u:%u): received pdu with invalid directive code %d for sub-state %d",
                              (t->state == CFDP_S2), t->history->src_eid, t->history->seq_num, fdh->directive_code,
                              t->state_data.s.sub_state);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_NON_FD_PDU, CFE_EVS_EventType_ERROR,
                          "CF S%d(%u:%u): received non-file directive pdu", (t->state == CFDP_S2), t->history->src_eid,
                          t->history->seq_num);
    }
}

/************************************************************************/
/** \brief S1 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S1_Recv(transaction_t *t)
{
    /* s1 doesn't need to receive anything */
    static void (*const substate_fns[SEND_NUM_STATES][PDU_INVALID_MAX])(transaction_t *,
                                                                        const pdu_header_t *) = {{NULL}};
    CF_CFDP_S_DispatchRecv(t, substate_fns);
}

/************************************************************************/
/** \brief S2 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Recv(transaction_t *t)
{
    static void (*const substate_fns[SEND_NUM_STATES][PDU_INVALID_MAX])(transaction_t *, const pdu_header_t *) = {
        {NULL, NULL, NULL, NULL, NULL, CF_CFDP_S2_EarlyFin, NULL, NULL, NULL, NULL, NULL},           /* SEND_METADATA */
        {NULL, NULL, NULL, NULL, NULL, CF_CFDP_S2_EarlyFin, NULL, NULL, CF_CFDP_S2_Nak, NULL, NULL}, /* SEND_FILEDATA */
        {NULL, NULL, NULL, NULL, NULL, CF_CFDP_S2_EarlyFin, NULL, NULL, CF_CFDP_S2_Nak, NULL, NULL}, /* SEND_EOF */
        {NULL, NULL, NULL, NULL, NULL, CF_CFDP_S2_Fin, CF_CFDP_S2_WaitForEofAck, NULL, CF_CFDP_S2_Nak_Arm, NULL,
         NULL}, /* SEND_WAIT_FOR_EOF_ACK */
        {NULL, NULL, NULL, NULL, NULL, CF_CFDP_S2_Fin, NULL, NULL, CF_CFDP_S2_Nak_Arm, NULL,
         NULL},                                                                       /* SEND_WAIT_FOR_FIN */
        {NULL, NULL, NULL, NULL, NULL, CF_CFDP_S2_Fin, NULL, NULL, NULL, NULL, NULL}, /* SEND_SEND_FIN_ACK */
    };
    CF_CFDP_S_DispatchRecv(t, substate_fns);
}

/************************************************************************/
/** \brief S1 dispatch function.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S1_Tx(transaction_t *t)
{
    static void (*const substate_fns[SEND_EOF + 1])(transaction_t * t) = {
        CF_CFDP_S_SubstateSendMetadata, CF_CFDP_S_SubstateSendFileData, CF_CFDP_S1_SubstateSendEof};

    CF_Assert(t->state_data.s.sub_state <= SEND_EOF);
    substate_fns[t->state_data.s.sub_state](t);
}

/************************************************************************/
/** \brief S2 dispatch function.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Tx(transaction_t *t)
{
    static void (*const substate_fns[SEND_EOF + 1])(transaction_t * t) = {
        CF_CFDP_S_SubstateSendMetadata, CF_CFDP_S2_SubstateSendFileData, CF_CFDP_S2_SubstateSendEof};

    CF_Assert(t->state_data.s.sub_state <= SEND_EOF);
    substate_fns[t->state_data.s.sub_state](t);
}

/************************************************************************/
/** \brief Cancel an S transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_Cancel(transaction_t *t)
{
    if (t->state_data.s.sub_state < SEND_EOF)
    {
        /* if state has not reached SEND_EOF, then set it to SEND_EOF now. */
        t->state_data.s.sub_state = SEND_EOF;
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
void CF_CFDP_S_Tick(transaction_t *t, int *cont /* unused */)
{
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */
    /* at each tick, various timers used by S are checked */
    /* first, check inactivity timer */
    if (t->state == CFDP_S2)
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

            if (t->flags.tx.ack_timer_armed)
            {
                if (CF_Timer_Expired(&t->ack_timer))
                {
                    if (t->state_data.s.sub_state == SEND_WAIT_FOR_EOF_ACK)
                    {
                        if (++t->state_data.s.s2.counter.ack == CF_AppData.config_table->ack_limit)
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
                            cfdp_send_ret_t sret = CF_CFDP_S_SendEof(t);
                            if (sret == CF_SEND_NO_MSG)
                            {
                                goto err_out; /* try next tick */
                            }
                            else if (sret == CF_SEND_ERROR)
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

            if (t->state_data.s.sub_state == SEND_SEND_FIN_ACK)
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
void CF_CFDP_S_Tick_Nak(transaction_t *t, int *cont)
{
    int ret = CF_CFDP_S_CheckAndRespondNak(t);

    if (ret == 1)
        *cont = 1; /* cause dispatcher to re-enter this wakeup */
}
