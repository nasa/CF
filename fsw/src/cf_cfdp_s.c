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
 *  The CF Application CFDP send logic source file
 *
 *  Handles all CFDP engine functionality specific to TX transactions.
 */

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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
static inline void CF_CFDP_S_Reset(CF_Transaction_t *t)
{
    CF_CFDP_ResetTransaction(t, 1);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_S_SendEof(CF_Transaction_t *t)
{
    if (!t->flags.com.crc_calc)
    {
        CF_CRC_Finalize(&t->crc);
        t->flags.com.crc_calc = 1;
    }
    return CF_CFDP_SendEof(t);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S1_SubstateSendEof(CF_Transaction_t *t)
{
    /* this looks weird, but the idea is we want to reset the transaction if some error occurs while sending
     * and we want to reset the transaction if no error occurs. But, if we couldn't send because there are
     * no buffers, then we need to try and send again next time. */
    if (CF_CFDP_S_SendEof(t) != CF_SEND_PDU_NO_BUF_AVAIL_ERROR)
    {
        CF_CFDP_S_Reset(t); /* all done, so clean up */
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_SubstateSendEof(CF_Transaction_t *t)
{
    t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_EOF_ACK;
    t->flags.com.ack_timer_armed = 1; /* will cause tick to see ack_timer as expired, and act */

    /* no longer need to send file data PDU except in the case of NAK response */

    /* move this transaction off Q_PEND */
    CF_DequeueTransaction(t);
    CF_InsertSortPrio(t, CF_QueueIdx_TXW);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_S_SendFileData(CF_Transaction_t *t, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc)
{
    bool                            success = true;
    int                             status  = 0;
    CFE_Status_t                    ret     = CF_ERROR;
    CF_Logical_PduBuffer_t *        ph      = CF_CFDP_ConstructPduHeader(t, 0, CF_AppData.config_table->local_eid,
                                                            t->history->peer_eid, 0, t->history->seq_num, 1);
    CF_Logical_PduFileDataHeader_t *fd;
    size_t                          actual_bytes;
    void *                          data_ptr;

    if (!ph)
    {
        ret     = CFE_SUCCESS; /* couldn't get message, so no bytes sent. Will try again next time */
        success = false;
    }
    else
    {
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
                                  "CF S%d(%lu:%lu): error seeking to offset %ld, got %ld", (t->state == CF_TxnState_S2),
                                  (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num, (long)foffs,
                                  (long)status);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
                success = false;
            }
        }

        if (success)
        {
            status = CF_WrappedRead(t->fd, data_ptr, actual_bytes);
            if (status != actual_bytes)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_READ, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): error reading bytes: expected %ld, got %ld",
                                  (t->state == CF_TxnState_S2), (unsigned long)t->history->src_eid,
                                  (unsigned long)t->history->seq_num, (long)actual_bytes, (long)status);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_read;
                success = false;
            }
        }

        if (success)
        {
            t->state_data.s.cached_pos += status;
            status = CF_CFDP_SendFd(t, ph);
            if (status == CF_SEND_PDU_NO_BUF_AVAIL_ERROR)
            {
                ret = CFE_SUCCESS; /* no bytes were processed */
            }
            else if (status == CF_SEND_PDU_ERROR)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEND_FD, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): error sending fd", (t->state == CF_TxnState_S2),
                                  (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
                ret = CF_ERROR;
            }
            else
            {
                CF_AppData.hk.channel_hk[t->chan_num].counters.sent.file_data_bytes += actual_bytes;

                CF_Assert((foffs + actual_bytes) <= t->fsize); /* sanity check */
                if (calc_crc)
                {
                    CF_CRC_Digest(&t->crc, fd->data_ptr, fd->data_len);
                }

                ret = actual_bytes;
            }
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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
        CF_CFDP_SetTxnStatus(t, CF_TxnStatus_FILESTORE_REJECTION);
        t->state_data.s.sub_state = CF_TxSubState_EOF;
    }
    else
    {
        /* don't care about other cases */
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_S_CheckAndRespondNak(CF_Transaction_t *t)
{
    const CF_Chunk_t *c;
    CFE_Status_t      sret;
    CFE_Status_t      ret = 0;

    if (t->flags.tx.md_need_send)
    {
        sret = CF_CFDP_SendMd(t);
        if (sret == CF_SEND_PDU_ERROR)
        {
            ret = CF_ERROR; /* error occurred */
        }
        else
        {
            if (sret == CFE_SUCCESS)
            {
                t->flags.tx.md_need_send = 0;
            }
            /* unless CF_SEND_PDU_ERROR, return 1 to keep caller from sending file data */
            ret = 1; /* 1 means nak processed, so don't send filedata */
        }
    }
    else
    {
        /* Get first chunk and process if available */
        c = CF_ChunkList_GetFirstChunk(&t->chunks->chunks);
        if (c != NULL)
        {
            ret = CF_CFDP_S_SendFileData(t, c->offset, c->size, 0);
            if (ret > 0)
            {
                CF_ChunkList_RemoveFromFirst(&t->chunks->chunks, ret);
                ret = 1; /* processed nak, so caller doesn't send file data */
            }
            else if (ret < 0)
            {
                ret = CF_ERROR; /* error occurred */
            }
            else
            {
                /* nothing to do if ret==0, since nothing was sent */
            }
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_SubstateSendFileData(CF_Transaction_t *t)
{
    int ret = CF_CFDP_S_CheckAndRespondNak(t);

    if (!ret)
    {
        CF_CFDP_S_SubstateSendFileData(t);
    }
    else if (ret < 0)
    {
        CF_CFDP_SetTxnStatus(t, CF_TxnStatus_NAK_RESPONSE_ERROR);
        CF_CFDP_S_Reset(t);
    }
    else
    {
        /* don't care about other cases */
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_SubstateSendMetadata(CF_Transaction_t *t)
{
    CFE_Status_t sret;
    int32        ret;
    int          status  = 0;
    bool         success = true;

    if (!OS_ObjectIdDefined(t->fd))
    {
        if (OS_FileOpenCheck(t->history->fnames.src_filename) == OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_ALREADY_OPEN, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): file %s already open", (t->state == CF_TxnState_S2),
                              (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num,
                              t->history->fnames.src_filename);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
            success = false;
        }

        if (success)
        {
            ret = CF_WrappedOpenCreate(&t->fd, t->history->fnames.src_filename, OS_FILE_FLAG_NONE, OS_READ_ONLY);
            if (ret < 0)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_OPEN, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): failed to open file %s, error=%ld", (t->state == CF_TxnState_S2),
                                  (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num,
                                  t->history->fnames.src_filename, (long)ret);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
                t->fd   = OS_OBJECT_ID_UNDEFINED; /* just in case */
                success = false;
            }
        }

        if (success)
        {
            status = CF_WrappedLseek(t->fd, 0, OS_SEEK_END);
            if (status < 0)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_END, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): failed to seek end file %s, error=%ld",
                                  (t->state == CF_TxnState_S2), (unsigned long)t->history->src_eid,
                                  (unsigned long)t->history->seq_num, t->history->fnames.src_filename, (long)status);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
                success = false;
            }
        }

        if (success)
        {
            t->fsize = status;

            status = CF_WrappedLseek(t->fd, 0, OS_SEEK_SET);
            if (status != 0)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEEK_BEG, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): failed to seek begin file %s, got %ld",
                                  (t->state == CF_TxnState_S2), (unsigned long)t->history->src_eid,
                                  (unsigned long)t->history->seq_num, t->history->fnames.src_filename, (long)status);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
                success = false;
            }
        }
    }

    if (success)
    {
        sret = CF_CFDP_SendMd(t);
        if (sret == CF_SEND_PDU_ERROR)
        {
            /* failed to send md */
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_SEND_MD, CFE_EVS_EventType_ERROR, "CF S%d(%lu:%lu): failed to send md",
                              (t->state == CF_TxnState_S2), (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num);
            success = false;
        }
        else if (sret == CFE_SUCCESS)
        {
            /* once metadata is sent, switch to filedata mode */
            t->state_data.s.sub_state = CF_TxSubState_FILEDATA;
        }
        /* if sret==CF_SEND_PDU_NO_BUF_AVAIL_ERROR, then try to send md again next cycle */
    }

    if (!success)
    {
        CF_CFDP_SetTxnStatus(t, CF_TxnStatus_FILESTORE_REJECTION);
        CF_CFDP_S_Reset(t);
    }

    /* don't need CF_CRC_Start() since taken care of by reset_cfdp() */
    /*CF_CRC_Start(&t->crc);*/
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_SubstateSendFinAck(CF_Transaction_t *t)
{
    /* if send, or error, reset. if no message, try again next cycle */
    if (CF_CFDP_SendAck(t, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_FIN, t->state_data.s.s2.fin_cc,
                        t->history->peer_eid, t->history->seq_num) != CF_SEND_PDU_NO_BUF_AVAIL_ERROR)
    {
        CF_CFDP_SetTxnStatus(t, CF_TxnStatus_NO_ERROR);
        CF_CFDP_S_Reset(t);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_EarlyFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* received early fin, so just cancel */
    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_EARLY_FIN, CFE_EVS_EventType_ERROR,
                      "CF S%d(%lu:%lu): got early FIN -- cancelling", (t->state == CF_TxnState_S2),
                      (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
    CF_CFDP_SetTxnStatus(t, CF_TxnStatus_EARLY_FIN);
    CF_CFDP_S_Reset(t);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Fin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    t->state_data.s.s2.fin_cc = ph->int_header.fin.cc;
    t->state_data.s.sub_state = CF_TxSubState_SEND_FIN_ACK;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Nak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_SegmentRequest_t *sr;
    const CF_Logical_PduNak_t *        nak;
    uint8                              counter;
    uint8                              bad_sr;

    bad_sr = 0;

    /* this function is only invoked for NAK PDU types */
    nak = &ph->int_header.nak;

    if (CF_CFDP_RecvNak(t, ph) == 0 && nak->segment_list.num_segments > 0)
    {
        for (counter = 0; counter < nak->segment_list.num_segments; ++counter)
        {
            sr = &nak->segment_list.segments[counter];

            if (sr->offset_start == 0 && sr->offset_end == 0)
            {
                /* need to re-send metadata PDU */
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
                              "CF S%d(%lu:%lu): received %d invalid NAK segment requests", (t->state == CF_TxnState_S2),
                              (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num, bad_sr);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_PDU_NAK, CFE_EVS_EventType_ERROR,
                          "CF S%d(%lu:%lu): received invalid NAK PDU", (t->state == CF_TxnState_S2),
                          (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Nak_Arm(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    CF_CFDP_ArmAckTimer(t);
    CF_CFDP_S2_Nak(t, ph);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_WaitForEofAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    if (!CF_CFDP_RecvAck(t, ph))
    {
        /* don't send FIN if error. Don't check the EOF CC, just go with
         * the stored one we sent before */
        if (CF_TxnStatus_IsError(t->history->txn_stat))
        {
            CF_CFDP_S_Reset(t);
        }
        else
        {
            t->state_data.s.sub_state    = CF_TxSubState_WAIT_FOR_FIN;
            t->flags.com.ack_timer_armed = 0; /* just wait for FIN now, nothing to re-send */
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_PDU_EOF, CFE_EVS_EventType_ERROR,
                          "CF S%d(%lu:%lu): received invalid EOF PDU", (t->state == CF_TxnState_S2),
                          (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* s1 doesn't need to receive anything */
    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {{NULL}};
    CF_CFDP_S_DispatchRecv(t, ph, &substate_fns);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Cancel(CF_Transaction_t *t)
{
    if (t->state_data.s.sub_state < CF_TxSubState_EOF)
    {
        /* if state has not reached CF_TxSubState_EOF, then set it to CF_TxSubState_EOF now. */
        t->state_data.s.sub_state = CF_TxSubState_EOF;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Tick(CF_Transaction_t *t, int *cont /* unused */)
{
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */
    CFE_Status_t sret;
    bool         early_exit = false;

    /* at each tick, various timers used by S are checked */
    /* first, check inactivity timer */
    if (t->state == CF_TxnState_S2)
    {
        if (CF_Timer_Expired(&t->inactivity_timer))
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_INACT_TIMER, CFE_EVS_EventType_ERROR,
                              "CF S2(%lu:%lu): inactivity timer expired", (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num);
            CF_CFDP_SetTxnStatus(t, CF_TxnStatus_INACTIVITY_DETECTED);

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
                        /* Increment acknak counter */
                        ++t->state_data.s.s2.acknak_count;

                        /* Check limit and handle if needed */
                        if (t->state_data.s.s2.acknak_count >= CF_AppData.config_table->chan[t->chan_num].ack_limit)
                        {
                            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_ACK_LIMIT, CFE_EVS_EventType_ERROR,
                                              "CF S2(%lu:%lu), ack limit reached, no eof-ack",
                                              (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
                            CF_CFDP_SetTxnStatus(t, CF_TxnStatus_ACK_LIMIT_NO_EOF);
                            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.ack_limit;

                            /* no reason to reset this timer, as it isn't used again */
                            CF_CFDP_S_Reset(t);
                            early_exit = true; /* must exit after reset */
                        }
                        else
                        {
                            sret = CF_CFDP_S_SendEof(t);
                            if (sret == CF_SEND_PDU_NO_BUF_AVAIL_ERROR)
                            {
                                early_exit = true;
                            }
                            else if (sret == CF_SEND_PDU_ERROR)
                            {
                                CF_CFDP_SetTxnStatus(t, CF_TxnStatus_SEND_EOF_FAILURE);
                                CF_CFDP_S_Reset(t); /* can't go on, error occurred */
                                early_exit = true;
                            }

                            if (!early_exit)
                            {
                                CF_CFDP_ArmAckTimer(t); /* re-arm ack timer */
                            }
                        }
                    }
                }
                else
                {
                    CF_Timer_Tick(&t->ack_timer);
                }
            }

            if (!early_exit && t->state_data.s.sub_state == CF_TxSubState_SEND_FIN_ACK)
            {
                CF_CFDP_S_SubstateSendFinAck(t);
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *t, int *cont)
{
    int ret = CF_CFDP_S_CheckAndRespondNak(t);

    if (ret == 1)
        *cont = 1; /* cause dispatcher to re-enter this wakeup */
}
