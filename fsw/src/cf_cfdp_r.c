/************************************************************************
 * File: cf_cfdp_r.c
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
 *
 * Purpose:
 *  The CF Application CFDP receive logic source file
 *
 *  Handles all CFDP engine functionality specific to RX transactions.
 *
 *
 *
 ************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include "cf_cfdp_r.h"
#include "cf_cfdp_dispatch.h"

#include <stdio.h>
#include <string.h>
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_SetCc
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_SetCc(CF_Transaction_t *t, CF_CFDP_ConditionCode_t cc)
{
    t->history->cc       = cc;
    t->flags.rx.send_fin = 1;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R1_Reset
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R1_Reset(CF_Transaction_t *t)
{
    CF_CFDP_ResetTransaction(t, 1);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_Reset
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_Reset(CF_Transaction_t *t)
{
    if ((t->state_data.r.sub_state == CF_RxSubState_WAIT_FOR_FIN_ACK) ||
        (t->state_data.r.r2.eof_cc != CF_CFDP_ConditionCode_NO_ERROR) ||
        (t->history->cc != CF_CFDP_ConditionCode_NO_ERROR) || t->flags.com.canceled)
    {
        CF_CFDP_R1_Reset(t); /* it's done */
    }
    else
    {
        /* not waiting for fin ack, so trigger send fin */
        t->flags.rx.send_fin = 1;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_CheckCrc
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_R_CheckCrc(CF_Transaction_t *t, uint32 expected_crc)
{
    int ret = 0;
    CF_CRC_Finalize(&t->crc);
    if (t->crc.result != expected_crc)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_CRC, CFE_EVS_EventType_ERROR,
                          "CF R%d(%lu:%lu): crc mismatch for R trans. got 0x%08lx expected 0x%08lx",
                          (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                          (unsigned long)t->history->seq_num, (unsigned long)t->crc.result,
                          (unsigned long)expected_crc);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.crc_mismatch;
        ret = 1;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_Complete
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_Complete(CF_Transaction_t *t, int ok_to_send_nak)
{
    int send_nak = 0;
    int send_fin = 0;
    /* checking if r2 is complete. check nak list, and send NAK if appropriate */
    /* if all data is present, then there will be no gaps in the chunk */

    if (t->history->cc != CF_CFDP_ConditionCode_NO_ERROR)
    {
        goto err_out; /* nothing to do here if error cc is set */
    }

    /* first, check if md is received. if not, send specialized nak */
    if (!t->flags.rx.md_recv)
    {
        send_nak = 1;
    }
    else
    {
        /* only look for 1 gap, since the goal here is just to know that there are gaps */
        uint32 ret = CF_ChunkList_ComputeGaps(&t->chunks->chunks, 1, t->fsize, 0, NULL, NULL);

        if (ret)
        {
            /* there is at least 1 gap, so send a nak */
            send_nak = 1;
        }
        else if (t->flags.rx.eof_recv)
        {
            /* the eof was received, and there are no NAKs -- process completion in send fin state */
            send_fin = 1;
        }
    }

    if (send_nak && ok_to_send_nak)
    {
        if (++t->state_data.r.r2.acknak_count >= CF_AppData.config_table->nak_limit)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_NAK_LIMIT, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): nak limited reach", (t->state == CF_TxnState_R2),
                              (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
            send_fin = 1;
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.nak_limit;
            t->history->cc = CF_CFDP_ConditionCode_NAK_LIMIT_REACHED; /* don't use CF_CFDP_R2_SetCc because many places
                                                                         in this function set send_fin */
            t->state_data.r.r2.acknak_count = 0;                      /* reset for fin/ack */
        }
        else
        {
            t->flags.rx.send_nak = 1;
        }
    }

    if (send_fin)
    {
        t->flags.rx.send_fin = 1;
        t->flags.rx.complete = 1; /* latch completeness, since send_fin is cleared later */
    }

    /* always go to CF_RxSubState_FILEDATA, and let tick change state */
    t->state_data.r.sub_state = CF_RxSubState_FILEDATA;

err_out:;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_ProcessFd
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_R_ProcessFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduFileDataHeader_t *fd;
    int32                                 fret;
    int                                   ret;

    /* this function is only entered for data PDUs */
    fd  = &ph->int_header.fd;
    ret = -1;

    /*
     * NOTE: The decode routine should have left a direct pointer to the data and actual data length
     * within the PDU.  The length has already been verified, too.  Should not need to make any
     * adjustments here, just write it.
     */

    if (t->state_data.r.cached_pos != fd->offset)
    {
        fret = CF_WrappedLseek(t->fd, fd->offset, OS_SEEK_SET);
        if (fret != fd->offset)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_SEEK_FD, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): failed to seek offset %ld, got %ld", (t->state == CF_TxnState_R2),
                              (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num, (long)fd->offset,
                              (long)fret);
            t->history->cc = CF_CFDP_ConditionCode_FILE_SIZE_ERROR;
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
            goto err_out; /* connection will reset in caller */
        }
    }

    fret = CF_WrappedWrite(t->fd, fd->data_ptr, fd->data_len);
    if (fret != fd->data_len)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_WRITE, CFE_EVS_EventType_ERROR,
                          "CF R%d(%lu:%lu): OS_write expected %ld, got %ld", (t->state == CF_TxnState_R2),
                          (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num, (long)fd->data_len,
                          (long)fret);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_write;
        t->history->cc = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
        goto err_out; /* connection will reset in caller */
    }

    t->state_data.r.cached_pos = fd->data_len + fd->offset;
    CF_AppData.hk.channel_hk[t->chan_num].counters.recv.file_data_bytes += fd->data_len;
    ret = 0;

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_SubstateRecvEof
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    int                        ret = CF_RxEofRet_SUCCESS;
    const CF_Logical_PduEof_t *eof;

    if (!CF_CFDP_RecvEof(t, ph))
    {
        /* this function is only entered for PDUs identified as EOF type */
        eof = &ph->int_header.eof;

        /* only check size if MD received, otherwise it's still OK */
        if (t->flags.rx.md_recv && (eof->size != t->fsize))
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_SIZE_MISMATCH, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): eof file size mismatch: got %lu expected %lu",
                              (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num, (unsigned long)eof->size, (unsigned long)t->fsize);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_size_mismatch;
            ret = CF_RxEofRet_FSIZE_MISMATCH;
            goto err_out;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_PDU_EOF, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid eof packet",
                          (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                          (unsigned long)t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
        ret = CF_RxEofRet_BAD_EOF;
        goto err_out;
    }

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R1_SubstateRecvEof
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R1_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    int                        ret = CF_CFDP_R_SubstateRecvEof(t, ph);
    uint32                     crc;
    const CF_Logical_PduEof_t *eof;

    /* this function is only entered for PDUs identified as EOF type */
    eof = &ph->int_header.eof;
    crc = eof->crc;

    if ((ret == CF_RxEofRet_SUCCESS) && !CF_CFDP_R_CheckCrc(t, crc))
    {
        /* successfully processed the file */
        t->keep = 1; /* save the file */
    }
    /* if file failed to process, there's nothing to do. CF_CFDP_R_CheckCrc() generates an event on failure */

    /* after exit, always reset since we are done */
    /* reset even if the eof failed -- class 1, so it won't come again! */
    CF_CFDP_R1_Reset(t);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_SubstateRecvEof
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduEof_t *eof;
    int                        ret;

    if (!t->flags.rx.eof_recv)
    {
        ret = CF_CFDP_R_SubstateRecvEof(t, ph);

        /* did receiving eof succeed? */
        if (ret == CF_RxEofRet_SUCCESS)
        {
            eof = &ph->int_header.eof;

            t->flags.rx.eof_recv = 1;

            /* need to remember the eof crc for later */
            t->state_data.r.r2.eof_crc  = eof->crc;
            t->state_data.r.r2.eof_size = eof->size;

            /* always ack the EOF, even if we're not done */
            t->state_data.r.r2.eof_cc = eof->cc;
            t->flags.rx.send_ack      = 1; /* defer sending ack to tick handling */

            /* only check for complete if EOF with no errors */
            if (t->state_data.r.r2.eof_cc == CF_CFDP_ConditionCode_NO_ERROR)
            {
                CF_CFDP_R2_Complete(t, 1); /* CF_CFDP_R2_Complete() will change state */
            }
            else
            {
                CF_CFDP_R2_Reset(t);
            }
        }
        else
        {
            /* bad eof sent? */
            if (ret == CF_RxEofRet_FSIZE_MISMATCH)
            {
                CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_FILE_SIZE_ERROR);
            }
            else
            {
                /* can't do anything with this bad EOF, so return to FILEDATA */
                t->state_data.r.sub_state = CF_RxSubState_FILEDATA;
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R1_SubstateRecvFileData
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R1_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* got file data pdu? */
    if (CF_CFDP_RecvFd(t, ph) || CF_CFDP_R_ProcessFd(t, ph))
    {
        goto err_out;
    }

    /* class 1 digests crc */
    CF_CRC_Digest(&t->crc, ph->int_header.fd.data_ptr, ph->int_header.fd.data_len);

    return;

err_out:
    CF_CFDP_R1_Reset(t);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_SubstateRecvFileData
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduFileDataHeader_t *fd;

    /* this function is only entered for data PDUs */
    fd = &ph->int_header.fd;

    /* got file data pdu? */
    if (CF_CFDP_RecvFd(t, ph) || CF_CFDP_R_ProcessFd(t, ph))
    {
        goto err_out;
    }

    /* class 2 does crc at FIN, but track gaps */
    CF_ChunkListAdd(&t->chunks->chunks, fd->offset, fd->data_len);

    if (t->flags.rx.fd_nak_sent)
    {
        CF_CFDP_R2_Complete(t, 0); /* once nak-retransmit received, start checking for completion at each fd */
    }

    if (!t->flags.rx.complete)
    {
        CF_CFDP_ArmAckTimer(t); /* re-arm ack timer, since we got data */
    }

    t->state_data.r.r2.acknak_count = 0;

    return;

err_out:
    CF_CFDP_R2_Reset(t);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_GapCompute
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *c, void *opaque)
{
    CF_GapComputeArgs_t         *args = (CF_GapComputeArgs_t *)opaque;
    CF_Logical_SegmentRequest_t *pseg;
    CF_Logical_SegmentList_t    *pseglist;
    CF_Logical_PduNak_t         *nak;

    /* This function is only invoked for NAK types */
    nak      = args->nak;
    pseglist = &nak->segment_list;
    CF_Assert(c->size > 0);

    /* it seems that scope in the old engine is not used the way I read it in the spec, so
     * leave this code here for now for future reference */

    if (pseglist->num_segments < CF_PDU_MAX_SEGMENTS)
    {
        pseg = &pseglist->segments[pseglist->num_segments];

        pseg->offset_start = c->offset - nak->scope_start;
        pseg->offset_end   = pseg->offset_start + c->size;

        ++pseglist->num_segments;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_SubstateSendNak
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_R_SubstateSendNak(CF_Transaction_t *t)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_NAK, t->history->peer_eid,
                                   CF_AppData.config_table->local_eid, 1, t->history->seq_num, 1);
    CF_Logical_PduNak_t *nak;
    CF_SendRet_t         sret;

    int ret = -1;

    if (ph)
    {
        nak = &ph->int_header.nak;

        if (t->flags.rx.md_recv)
        {
            /* we have metadata, so send valid nak */
            CF_GapComputeArgs_t args = {t, nak};
            uint32              cret;

            nak->scope_start = 0;
            cret             = CF_ChunkList_ComputeGaps(&t->chunks->chunks,
                                            (t->chunks->chunks.count < t->chunks->chunks.max_chunks)
                                                            ? t->chunks->chunks.max_chunks
                                                            : (t->chunks->chunks.max_chunks - 1),
                                                        t->fsize, 0, CF_CFDP_R2_GapCompute, &args);

            if (!cret)
            {
                /* no gaps left, so go ahead and check for completion */
                t->flags.rx.complete = 1; /* we know md was received, and there's no gaps -- it's complete */
                ret                  = 0;
            }
            else
            {
                /* gaps are present, so let's send the nak pdu */
                nak->scope_end          = 0;
                sret                    = CF_CFDP_SendNak(t, ph);
                t->flags.rx.fd_nak_sent = 1;         /* latch that at least one nak has been sent requesting filedata */
                CF_Assert(sret != CF_SendRet_ERROR); /* NOTE: this CF_Assert is here because CF_CFDP_SendNak() does not
                                                     return CF_SendRet_ERROR, so if it's ever added to that function we
                                                     need to test handling it here */
                if (sret == CF_SendRet_SUCCESS)
                {
                    CF_AppData.hk.channel_hk[t->chan_num].counters.sent.nak_segment_requests += cret;
                    ret = 0;
                }
            }
        }
        else
        {
            /* need to send simple nak packet to request metadata pdu again */
            /* after doing so, transition to recv md state */
            CFE_EVS_SendEvent(CF_EID_INF_CFDP_R_REQUEST_MD, CFE_EVS_EventType_INFORMATION,
                              "CF R%d(%lu:%lu): requesting MD", (t->state == CF_TxnState_R2),
                              (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
            /* scope start/end, and sr[0] start/end == 0 special value to request metadata */
            nak->scope_start                           = 0;
            nak->scope_end                             = 0;
            nak->segment_list.segments[0].offset_start = 0;
            nak->segment_list.segments[0].offset_end   = 0;
            nak->segment_list.num_segments             = 1;

            sret = CF_CFDP_SendNak(t, ph);
            CF_Assert(sret != CF_SendRet_ERROR); /* this CF_Assert is here because CF_CFDP_SendNak() does not return
                                                    CF_SendRet_ERROR */
            if (sret == CF_SendRet_SUCCESS)
            {
                ret = 0;
            }
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_Init
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_Init(CF_Transaction_t *t)
{
    int32 ret;

    if (t->state == CF_TxnState_R2)
    {
        if (!t->flags.rx.md_recv)
        {
            /* we need to make a temp file and then do a NAK for md pdu */
            /* the transaction already has a history, and that has a buffer that we can use to
             * hold the temp filename */
            /* the -1 below is to make room for the slash */
            snprintf(t->history->fnames.dst_filename, sizeof(t->history->fnames.dst_filename) - 1, "%.*s/%lu.tmp",
                     CF_FILENAME_MAX_PATH - 1, CF_AppData.config_table->tmp_dir, (unsigned long)t->history->seq_num);
            CFE_EVS_SendEvent(CF_EID_INF_CFDP_R_TEMP_FILE, CFE_EVS_EventType_INFORMATION,
                              "CF R%d(%lu:%lu): making temp file %s for transaction without MD",
                              (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num, t->history->fnames.dst_filename);
        }

        CF_CFDP_ArmAckTimer(t);
    }

    ret = CF_WrappedOpenCreate(&t->fd, t->history->fnames.dst_filename, OS_FILE_FLAG_CREATE, OS_READ_WRITE);
    if (ret < 0)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_CREAT, CFE_EVS_EventType_ERROR,
                          "CF R%d(%lu:%lu): failed to create file %s for writing, error=%ld",
                          (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                          (unsigned long)t->history->seq_num, t->history->fnames.dst_filename, (long)ret);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
        t->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
        if (t->state == CF_TxnState_R2)
        {
            CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
        }
        else
        {
            CF_CFDP_R1_Reset(t);
        }
    }
    else
    {
        t->state_data.r.sub_state = CF_RxSubState_FILEDATA;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_CalcCrcChunk
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_R2_CalcCrcChunk(CF_Transaction_t *t)
{
    uint8  buf[CF_R2_CRC_CHUNK_SIZE];
    size_t count_bytes;
    size_t want_offs_size;
    size_t read_size;
    int    fret;
    int    ret;

    count_bytes = 0;
    ret         = -1;

    if (t->state_data.r.r2.rx_crc_calc_bytes == 0)
    {
        CF_CRC_Start(&t->crc);
    }

    while ((count_bytes < CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup) &&
           (t->state_data.r.r2.rx_crc_calc_bytes < t->fsize))
    {
        want_offs_size = t->state_data.r.r2.rx_crc_calc_bytes + sizeof(buf);

        if (want_offs_size > t->fsize)
        {
            read_size = t->fsize - t->state_data.r.r2.rx_crc_calc_bytes;
        }
        else
        {
            read_size = sizeof(buf);
        }

        if (t->state_data.r.cached_pos != t->state_data.r.r2.rx_crc_calc_bytes)
        {
            fret = CF_WrappedLseek(t->fd, t->state_data.r.r2.rx_crc_calc_bytes, OS_SEEK_SET);
            if (fret != t->state_data.r.r2.rx_crc_calc_bytes)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_SEEK_CRC, CFE_EVS_EventType_ERROR,
                                  "CF R%d(%lu:%lu): failed to seek offset %lu, got %ld", (t->state == CF_TxnState_R2),
                                  (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num,
                                  (unsigned long)t->state_data.r.r2.rx_crc_calc_bytes, (long)fret);
                t->history->cc = CF_CFDP_ConditionCode_FILE_SIZE_ERROR; /* should be ok to use this one */
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
                goto err_out;
            }
        }

        fret = CF_WrappedRead(t->fd, buf, read_size);
        if (fret != read_size)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_READ, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): failed to read file expected %lu, got %ld",
                              (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num, (unsigned long)read_size, (long)fret);
            t->history->cc = CF_CFDP_ConditionCode_FILE_SIZE_ERROR; /* should be ok to use this one */
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_read;
            goto err_out;
        }

        CF_CRC_Digest(&t->crc, buf, read_size);
        t->state_data.r.r2.rx_crc_calc_bytes += read_size;
        t->state_data.r.cached_pos = t->state_data.r.r2.rx_crc_calc_bytes;
        count_bytes += read_size;
    }

    if (t->state_data.r.r2.rx_crc_calc_bytes == t->fsize)
    {
        /* all bytes calculated, so now check */
        if (!CF_CFDP_R_CheckCrc(t, t->state_data.r.r2.eof_crc))
        {
            /* crc matched! we are happy */
            t->keep = 1; /* save the file */

            /* set fin pdu status */
            t->state_data.r.r2.dc = CF_CFDP_FinDeliveryCode_COMPLETE;
            t->state_data.r.r2.fs = CF_CFDP_FinFileStatus_RETAINED;
        }
        else
        {
            CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE);
        }

        t->flags.com.crc_calc = 1;

        ret = 0;
    }

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_SubstateSendFin
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_R2_SubstateSendFin(CF_Transaction_t *t)
{
    CF_SendRet_t sret;
    int          ret = -1;

    if (t->history->cc == CF_CFDP_ConditionCode_NO_ERROR && !t->flags.com.crc_calc)
    {
        /* no error, and haven't checked crc -- so start checking it */
        if (CF_CFDP_R2_CalcCrcChunk(t))
        {
            goto err_out; /* signal to caller to re-enter next tick */
        }
    }

    sret = CF_CFDP_SendFin(t, t->state_data.r.r2.dc, t->state_data.r.r2.fs, t->history->cc);
    CF_Assert(sret != CF_SendRet_ERROR); /* CF_CFDP_SendFin does not return CF_SendRet_ERROR */
    t->state_data.r.sub_state =
        CF_RxSubState_WAIT_FOR_FIN_ACK; /* whether or not fin send successful, ok to transition state */
    if (sret == CF_SendRet_SUCCESS)
    {
        ret = 0;
        goto err_out;
    }

    /* if no message, then try again next time */

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_Recv_fin_ack
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_Recv_fin_ack(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    if (!CF_CFDP_RecvAck(t, ph))
    {
        /* got fin ack, so time to close the state */
        CF_CFDP_R2_Reset(t);
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_PDU_FINACK, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid fin-ack",
                          (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                          (unsigned long)t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_RecvMd
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* it isn't an error to get another MD pdu, right? */
    if (!t->flags.rx.md_recv)
    {
        /* NOTE: t->flags.rx.md_recv always 1 in R1, so this is R2 only */
        /* parse the md pdu. this will overwrite the transaction's history, which contains our filename. so let's
         * save the filename in a local buffer so it can be used with OS_rename upon successful parsing of
         * the md pdu */
        char fname[CF_FILENAME_MAX_LEN];
        int  status;

        strcpy(
            fname,
            t->history->fnames.dst_filename); /* strcpy is ok, since fname is CF_FILENAME_MAX_LEN like dst_filename */
        status = CF_CFDP_RecvMd(t, ph);
        if (!status)
        {
            /* successfully obtained md pdu */
            if (t->flags.rx.eof_recv)
            {
                /* eof was received, so check that md and eof sizes match */
                if (t->state_data.r.r2.eof_size != t->fsize)
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_EOF_MD_SIZE, CFE_EVS_EventType_ERROR,
                                      "CF R%d(%lu:%lu): eof/md size mismatch md: %lu, eof: %lu",
                                      (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                                      (unsigned long)t->history->seq_num, (unsigned long)t->fsize,
                                      (unsigned long)t->state_data.r.r2.eof_size);
                    ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_size_mismatch;
                    CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_FILE_SIZE_ERROR);
                    goto err_out;
                }
            }

            /* close and rename file */
            CF_WrappedClose(t->fd);
            CFE_ES_PerfLogEntry(CF_PERF_ID_RENAME);
            status = OS_rename(fname, t->history->fnames.dst_filename);
            CFE_ES_PerfLogExit(CF_PERF_ID_RENAME);
            if (status != OS_SUCCESS)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_RENAME, CFE_EVS_EventType_ERROR,
                                  "CF R%d(%lu:%lu): failed to rename file in R2, error=%ld",
                                  (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                                  (unsigned long)t->history->seq_num, (long)status);
                t->fd = OS_OBJECT_ID_UNDEFINED;
                CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_rename;
                goto err_out;
            }
            else
            {
                int32 ret =
                    CF_WrappedOpenCreate(&t->fd, t->history->fnames.dst_filename, OS_FILE_FLAG_NONE, OS_READ_WRITE);
                if (ret < 0)
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_OPEN, CFE_EVS_EventType_ERROR,
                                      "CF R%d(%lu:%lu): failed to open renamed file in R2, error=%ld",
                                      (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                                      (unsigned long)t->history->seq_num, (long)ret);
                    CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
                    ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
                    t->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
                    goto err_out;
                }
            }

            t->flags.rx.md_recv             = 1;
            t->state_data.r.r2.acknak_count = 0; /* in case part of nak */
            CF_CFDP_R2_Complete(t, 1);           /* check for completion now that md is received */
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_PDU_MD, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid md received",
                              (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
            /* do nothing here, since it will be nak'd again later */
        }
    }

err_out:;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R1_Recv
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t r1_fdir_handlers = {
        .fdirective = {[CF_CFDP_FileDirective_EOF] = CF_CFDP_R1_SubstateRecvEof}};
    static const CF_CFDP_R_SubstateDispatchTable_t substate_fns = {
        .state = {[CF_RxSubState_FILEDATA]         = &r1_fdir_handlers,
                  [CF_RxSubState_EOF]              = &r1_fdir_handlers,
                  [CF_RxSubState_WAIT_FOR_FIN_ACK] = &r1_fdir_handlers}};

    CF_CFDP_R_DispatchRecv(t, ph, &substate_fns, CF_CFDP_R1_SubstateRecvFileData);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R2_Recv
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_normal = {
        .fdirective = {
            [CF_CFDP_FileDirective_EOF]      = CF_CFDP_R2_SubstateRecvEof,
            [CF_CFDP_FileDirective_METADATA] = CF_CFDP_R2_RecvMd,
        }};
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_finack = {
        .fdirective = {
            [CF_CFDP_FileDirective_EOF] = CF_CFDP_R2_SubstateRecvEof,
            [CF_CFDP_FileDirective_ACK] = CF_CFDP_R2_Recv_fin_ack,
        }};
    static const CF_CFDP_R_SubstateDispatchTable_t substate_fns = {
        .state = {[CF_RxSubState_FILEDATA]         = &r2_fdir_handlers_normal,
                  [CF_RxSubState_EOF]              = &r2_fdir_handlers_normal,
                  [CF_RxSubState_WAIT_FOR_FIN_ACK] = &r2_fdir_handlers_finack}};

    CF_CFDP_R_DispatchRecv(t, ph, &substate_fns, CF_CFDP_R2_SubstateRecvFileData);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_Cancel
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_Cancel(CF_Transaction_t *t)
{
    /* for cancel, only need to send FIN if R2 */
    if ((t->state == CF_TxnState_R2) && (t->state_data.r.sub_state < CF_RxSubState_WAIT_FOR_FIN_ACK))
    {
        t->flags.rx.send_fin = 1;
    }
    else
    {
        CF_CFDP_R1_Reset(t); /* if R1, just call it quits */
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_SendInactivityEvent
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_SendInactivityEvent(CF_Transaction_t *t)
{
    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_INACT_TIMER, CFE_EVS_EventType_ERROR,
                      "CF R%d(%lu:%lu): inactivity timer expired", (t->state == CF_TxnState_R2),
                      (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
    ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.inactivity_timer;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_Tick
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_Tick(CF_Transaction_t *t, int *cont /* unused */)
{
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */
    /* at each tick, various timers used by R are checked */
    /* first, check inactivity timer */
    if (t->state == CF_TxnState_R2)
    {
        if (!t->flags.rx.inactivity_fired)
        {
            if (CF_Timer_Expired(&t->inactivity_timer))
            {
                CF_CFDP_R_SendInactivityEvent(t);

                CF_CFDP_R2_SetCc(t, CF_CFDP_ConditionCode_INACTIVITY_DETECTED);
                t->flags.rx.inactivity_fired = 1;
            }
            else
            {
                CF_Timer_Tick(&t->inactivity_timer);
            }
        }

        /* rx maintenance: possibly process send_eof_ack, send_nak or send_fin */
        if (t->flags.rx.send_ack)
        {
            CF_SendRet_t sret = CF_CFDP_SendAck(t, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF,
                                                t->state_data.r.r2.eof_cc, t->history->peer_eid, t->history->seq_num);
            CF_Assert(sret != CF_SendRet_ERROR);

            /* if CF_SendRet_SUCCESS, then move on in the state machine. CF_CFDP_SendAck does not return
             * CF_SendRet_ERROR */
            if (sret != CF_SendRet_NO_MSG)
            {
                t->flags.rx.send_ack = 0;
            }
        }
        else if (t->flags.rx.send_nak)
        {
            if (!CF_CFDP_R_SubstateSendNak(t))
            {
                t->flags.rx.send_nak = 0; /* will re-enter on error */
            }
        }
        else if (t->flags.rx.send_fin)
        {
            if (!CF_CFDP_R2_SubstateSendFin(t))
            {
                t->flags.rx.send_fin = 0; /* will re-enter on error */
            }
        }
        else
        {
            /* don't care about any other cases */
        }

        if (t->flags.com.ack_timer_armed)
        {
            if (CF_Timer_Expired(&t->ack_timer))
            {
                /* ack timer expired, so check for completion */
                if (!t->flags.rx.complete)
                {
                    CF_CFDP_R2_Complete(t, 1);
                }
                else if (t->state_data.r.sub_state == CF_RxSubState_WAIT_FOR_FIN_ACK)
                {
                    if (++t->state_data.r.r2.acknak_count >= CF_AppData.config_table->ack_limit)
                    {
                        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_ACK_LIMIT, CFE_EVS_EventType_ERROR,
                                          "CF R2(%lu:%lu): ack limit reached, no fin-ack",
                                          (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
                        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.ack_limit;
                        CF_CFDP_R2_Reset(t);
                        goto err_out; /* must return after reset */
                    }
                    else
                    {
                        t->flags.rx.send_fin = 1;
                    }
                }

                CF_CFDP_ArmAckTimer(t); /* whether sending fin or waiting for more filedata, need ack timer armed */
            }
            else
            {
                CF_Timer_Tick(&t->ack_timer);
            }
        }
    }
    else
    {
        if (CF_Timer_Expired(&t->inactivity_timer))
        {
            CF_CFDP_R_SendInactivityEvent(t);
            CF_CFDP_R1_Reset(t);
        }
        else
        {
            CF_Timer_Tick(&t->inactivity_timer);
        }
    }

err_out:;
}
