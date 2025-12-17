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
 *  The CF Application CFDP receive logic source file
 *
 *  Handles all CFDP engine functionality specific to RX transactions.
 */
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
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_R_CheckCrc(CF_Transaction_t *txn)
{
    CFE_Status_t ret;

    if (txn->crc.result != txn->state_data.eof_crc)
    {
        CFE_EVS_SendEvent(CF_CFDP_R_CRC_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF R%d(%lu:%lu): CRC mismatch for R trans. got 0x%08lx expected 0x%08lx",
                          CF_CFDP_GetPrintClass(txn), (unsigned long)txn->history->src_eid,
                          (unsigned long)txn->history->seq_num, (unsigned long)txn->crc.result,
                          (unsigned long)txn->state_data.eof_crc);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.crc_mismatch;
        ret = CF_ERROR;
    }
    else
    {
        ret = CFE_SUCCESS;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CF_CFDP_R_CheckComplete(CF_Transaction_t *txn)
{
    uint32 ret;

    /*
     * "completeness" of an active, non-errored R transaction is judged as:
     *  - having received MD PDU
     *  - having received all filedata PDUs (no gaps)
     *  - having received the EOF PDU
     */

    /* Active transaction with no errors: The MD and EOF flags must be set with no pending NAKs */
    if (txn->flags.rx.send_nak || txn->flags.rx.eof_count == 0 || !txn->flags.rx.md_recv)
    {
        return false; /* not complete yet due to missing PDU of some type */
    }

    /* Finally if all other state seems OK check for gaps in the file data */
    /* only look for 1 gap, since the goal here is just to know that there are gaps */
    ret = CF_ChunkList_ComputeGaps(&txn->chunks->chunks, 1, txn->fsize, 0, NULL, NULL);

    /* The file is complete if there are no gaps */
    return (ret == 0);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_R_ProcessFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduFileDataHeader_t *fd;
    int32                                 fret;
    CFE_Status_t                          ret;

    /* this function is only entered for data PDUs */
    fd  = &ph->int_header.fd;
    ret = CFE_SUCCESS;

    /*
     * NOTE: The decode routine should have left a direct pointer to the data and actual data length
     * within the PDU.  The length has already been verified, too.  Should not need to make any
     * adjustments here, just write it.
     */

    if (txn->state_data.cached_pos != fd->offset)
    {
        fret = CF_WrappedLseek(txn->fd, fd->offset, OS_SEEK_SET);
        if (fret != fd->offset)
        {
            CFE_EVS_SendEvent(CF_CFDP_R_SEEK_FD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): failed to seek offset %ld, got %ld", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              (long)fd->offset, (long)fret);
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
            ret = CF_ERROR; /* connection will reset in caller */
        }
        else
        {
            txn->state_data.cached_pos = fd->offset;
        }
    }

    if (ret != CF_ERROR)
    {
        fret = CF_WrappedWrite(txn->fd, fd->data_ptr, fd->data_len);
        if (fret != fd->data_len)
        {
            CFE_EVS_SendEvent(CF_CFDP_R_WRITE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): OS_write expected %ld, got %ld", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              (long)fd->data_len, (long)fret);
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_write;
            ret = CF_ERROR; /* connection will reset in caller */
        }
        else
        {
            txn->state_data.cached_pos = fd->data_len + fd->offset;
            CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.file_data_bytes += fd->data_len;

            /* insert gap data in chunks */
            CF_ChunkListAdd(&txn->chunks->chunks, fd->offset, fd->data_len);
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t               ret = CFE_SUCCESS;
    const CF_Logical_PduEof_t *eof;

    /* this function is only entered for PDUs identified as EOF type */
    ret = CF_CFDP_RecvEof(txn, ph);
    if (ret == CFE_SUCCESS)
    {
        eof = &ph->int_header.eof;

        /* only accept the first EOF, ignore dupes */
        if (txn->flags.rx.eof_count == 0)
        {
            /* need to remember the EOF CRC for later */
            txn->state_data.eof_crc  = eof->crc;
            txn->state_data.eof_size = eof->size;

            txn->state_data.peer_cc = eof->cc;

            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_From_ConditionCode(eof->cc));
        }

        if (!CF_CFDP_CheckAckNakCount(txn, &txn->flags.rx.eof_count))
        {
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_POS_ACK_LIMIT_REACHED);
        }

        CF_TRACE("%s(): Recv EOF, crc=%08lx, size=%lu, cc=%d\n", __func__, (unsigned long)eof->crc,
                 (unsigned long)eof->size, (int)eof->cc);
    }
    else
    {
        CFE_EVS_SendEvent(CF_CFDP_R_PDU_EOF_ERR_EID, CFE_EVS_EventType_ERROR, "CF R%d(%lu:%lu): invalid EOF packet",
                          CF_CFDP_GetPrintClass(txn), (unsigned long)txn->history->src_eid,
                          (unsigned long)txn->history->seq_num);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    int ret;

    /* got file data PDU? */
    ret = CF_CFDP_RecvFd(txn, ph);
    if (ret == CFE_SUCCESS)
    {
        ret = CF_CFDP_R_ProcessFd(txn, ph);
    }

    if (ret == CFE_SUCCESS)
    {
        /* All new file data will reset the NAK counter.  if we are post-EOF
         * then all additional PDUs are presumed to be the result of a NAK.
         * If we are pre-EOF then it is irrelevant, this is benign because
         * nothing is ack/nak'd until after EOF anyway. Note it would be more
         * correct/sophisticated to check if the ChunkListAdd above actually
         * filled a hole in the file.  But this is good enough. */
        txn->state_data.acknak_count = 0;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *chunk, void *opaque)
{
    CF_GapComputeArgs_t *        args = (CF_GapComputeArgs_t *)opaque;
    CF_Logical_SegmentRequest_t *pseg;
    CF_Logical_SegmentList_t *   pseglist;
    CF_Logical_PduNak_t *        nak;

    /* This function is only invoked for NAK types */
    nak      = args->nak;
    pseglist = &nak->segment_list;
    CF_Assert(chunk->size > 0);

    /* it seems that scope in the old engine is not used the way I read it in the spec, so
     * leave this code here for now for future reference */

    if (pseglist->num_segments < CF_PDU_MAX_SEGMENTS)
    {
        pseg = &pseglist->segments[pseglist->num_segments];

        pseg->offset_start = chunk->offset - nak->scope_start;
        pseg->offset_end   = pseg->offset_start + chunk->size;

        ++pseglist->num_segments;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CFDP_R_SendNak(CF_Transaction_t *txn)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_NAK, txn->history->peer_eid,
                                   CF_AppData.config_table->local_eid, 1, txn->history->seq_num, 1);
    CF_Logical_PduNak_t *nak;
    uint32               cret;
    CFE_Status_t         ret;

    if (ph == NULL)
    {
        ret = CF_SEND_PDU_NO_BUF_AVAIL_ERROR;
    }
    else
    {
        ret = CFE_SUCCESS; /* no other failure possibility */
        nak = &ph->int_header.nak;

        /* this impl always sends scope as 0 */
        nak->scope_start = 0;
        nak->scope_end   = 0;

        if (txn->flags.rx.md_recv)
        {
            /* we have metadata, so send valid NAK */
            CF_GapComputeArgs_t args = {txn, nak};

            cret = CF_ChunkList_ComputeGaps(&txn->chunks->chunks,
                                            (txn->chunks->chunks.count < txn->chunks->chunks.max_chunks)
                                                ? txn->chunks->chunks.max_chunks
                                                : (txn->chunks->chunks.max_chunks - 1),

                                            txn->fsize, 0, CF_CFDP_R2_GapCompute, &args);

            /*
             * NOTE: If this returns 0 then it means the file is complete and there is nothing to NAK.
             *
             * This is not expected to be 0 because this is only called after something set
             * send_nak = true in the txn state, which means something was identified as missing.  The
             * possibility is that a gap was previously identified but then the missing piece was
             * received before we got into this function.
             *
             * In that case, we still return CFE_SUCCESS so the tick processor will clear the send_nak
             * flag and re-check for completeness and (presumably) determine that nothing is missing.
             */
            if (cret != 0)
            {
                CF_CFDP_SendNak(txn, ph);

                CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.nak_segment_requests += cret;
            }
        }
        else
        {
            /* need to send simple NAK packet to request metadata PDU again */
            /* after doing so, transition to recv md state */
            CFE_EVS_SendEvent(CF_CFDP_R_REQUEST_MD_INF_EID, CFE_EVS_EventType_INFORMATION,
                              "CF R%d(%lu:%lu): requesting MD", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);

            /* scope start/end, and sr[0] start/end == 0 special value to request metadata */
            nak->scope_start                           = 0;
            nak->scope_end                             = 0;
            nak->segment_list.segments[0].offset_start = 0;
            nak->segment_list.segments[0].offset_end   = 0;
            nak->segment_list.num_segments             = 1;

            CF_CFDP_SendNak(txn, ph);
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_Init(CF_Transaction_t *txn)
{
    int32 ret;
    char  TempName[CFE_MISSION_MAX_FILE_LEN];

    /* set default FIN status */
    txn->state_data.fin_dc = CF_CFDP_FinDeliveryCode_INVALID;
    txn->state_data.fin_fs = CF_CFDP_FinFileStatus_INVALID;

    /* make a temp file to hold the data */
    CF_CFDP_GetTempName(txn->history, TempName, sizeof(TempName));

    ret = CF_WrappedOpenCreate(&txn->fd, TempName, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_READ_WRITE);
    if (ret < 0)
    {
        CFE_EVS_SendEvent(CF_CFDP_R_CREAT_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF R%d(%lu:%lu): failed to create file %s for writing, error=%ld",
                          CF_CFDP_GetPrintClass(txn), (unsigned long)txn->history->src_eid,
                          (unsigned long)txn->history->seq_num, TempName, (long)ret);

        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
        txn->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */

        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
    }
    else
    {
        CFE_EVS_SendEvent(CF_CFDP_R_TEMP_FILE_INF_EID, CFE_EVS_EventType_INFORMATION,
                          "CF R%d(%lu:%lu): starting transaction using temp file \'%s\'", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num, TempName);

        txn->flags.rx.tempfile_created = true;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_CalcCrcStart(CF_Transaction_t *txn)
{
    int32 OsStatus;

    if (txn->fsize != txn->state_data.eof_size)
    {
        CFE_EVS_SendEvent(CF_CFDP_R_SIZE_MISMATCH_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF R%d(%lu:%lu): file size mismatch, md=%lu eof=%lu", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                          (unsigned long)txn->fsize, (unsigned long)txn->state_data.eof_size);

        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
        txn->flags.com.crc_complete = true;
    }
    else
    {
        OsStatus = CF_WrappedLseek(txn->fd, 0, OS_SEEK_SET);
        if (OsStatus != 0)
        {
            CFE_EVS_SendEvent(CF_CFDP_R_SEEK_CRC_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): failed to seek offset 0, got %ld", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              (long)OsStatus);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;

            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
            txn->flags.com.crc_complete = true;
        }
        else
        {
            CF_CRC_Start(&txn->crc);
            txn->state_data.cached_pos = 0;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_CalcCrcChunk(CF_Transaction_t *txn)
{
    uint8  buf[CF_R2_CRC_CHUNK_SIZE];
    size_t count_bytes;
    size_t want_offs_size;
    size_t read_size;
    int    fret;
    bool   success = true;

    memset(buf, 0, sizeof(buf));

    count_bytes = 0;

    while ((count_bytes < CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup) &&
           (txn->state_data.cached_pos < txn->fsize))
    {
        want_offs_size = txn->state_data.cached_pos + sizeof(buf);

        if (want_offs_size > txn->fsize)
        {
            read_size = txn->fsize - txn->state_data.cached_pos;
        }
        else
        {
            read_size = sizeof(buf);
        }

        fret = CF_WrappedRead(txn->fd, buf, read_size);
        if (fret != read_size)
        {
            CFE_EVS_SendEvent(CF_CFDP_R_READ_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): failed to read file expected %lu, got %ld", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              (unsigned long)read_size, (long)fret);
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_SIZE_ERROR);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_read;
            success = false;
            break;
        }

        CF_CRC_Digest(&txn->crc, buf, read_size);
        txn->state_data.cached_pos += read_size;
        count_bytes += read_size;
    }

    /* Mark CRC as completed if we either reached the end of the file (normal) or
     * if a read error occurred (abnormal) */
    if (!success)
    {
        /* an error occurred, stop trying to calculate the crc */
        txn->flags.com.crc_complete = true;
    }
    else if (txn->state_data.cached_pos == txn->fsize)
    {
        /* reached the end of the file, so finalize the CRC */
        CF_CRC_Finalize(&txn->crc);

        /* this also means to stop calculating it */
        txn->flags.com.crc_complete = true;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_SubstateRecvFinAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t         status;
    CF_Logical_PduAck_t *ack;

    /* receipt of fin-ack is only valid if we had previously sent
     * a fin and it decodes as expected */
    status = CF_CFDP_RecvAck(txn, ph);
    if (status == CFE_SUCCESS)
    {
        ack = &ph->int_header.ack;

        if (ack->ack_directive_code == CF_CFDP_FileDirective_FIN)
        {
            /* seems OK so note its reception */
            txn->flags.rx.finack_recv = true;
        }

        CF_TRACE("%s(): Recv ACK, dir_code=%d, cc=%d, st=%d\n", __func__, (int)ack->ack_directive_code, (int)ack->cc,
                 (int)ack->txn_status);
    }

    /* Nothing to do with a fin-ack that does not check out.  Just ignore it and let the
     * state machine do its other integrity checks.  If the txn went wrong then something else
     * will be in a bad state too (i.e. lack of other PDUs).  But if nothing is wrong then
     * do not make a problem where there was none. */
    if (status != CFE_SUCCESS || !txn->flags.rx.finack_recv)
    {
        CFE_EVS_SendEvent(CF_CFDP_R_PDU_FINACK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF R%d(%lu:%lu): received invalid FIN-ACK PDU", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_SubstateRecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t status;

    if (!txn->flags.rx.md_recv)
    {
        status = CF_CFDP_RecvMd(txn, ph);
        if (status == CFE_SUCCESS)
        {
            txn->flags.rx.md_recv = true;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t r1_fdir_handlers = {
        .fdirective = {[CF_CFDP_FileDirective_EOF]      = CF_CFDP_R_SubstateRecvEof,
                       [CF_CFDP_FileDirective_METADATA] = CF_CFDP_R_SubstateRecvMd}};
    static const CF_CFDP_R_SubstateDispatchTable_t substate_fns = {
        .state = {[CF_RxSubState_DATA_NORMAL] = &r1_fdir_handlers, [CF_RxSubState_DATA_EOF] = &r1_fdir_handlers}};

    CF_CFDP_R_DispatchRecv(txn, ph, &substate_fns, CF_CFDP_R_SubstateRecvFileData);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_normal = {
        .fdirective = {[CF_CFDP_FileDirective_EOF]      = CF_CFDP_R_SubstateRecvEof,
                       [CF_CFDP_FileDirective_METADATA] = CF_CFDP_R_SubstateRecvMd}};
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_done = {
        .fdirective = {
            [CF_CFDP_FileDirective_EOF] =
                CF_CFDP_R_SubstateRecvEof, /* sender may re-send EOF if it did not get our EOF-ACK */
        }};
    static const CF_CFDP_FileDirectiveDispatchTable_t r2_fdir_handlers_finack = {
        .fdirective = {
            [CF_CFDP_FileDirective_EOF] = CF_CFDP_R_SubstateRecvEof,
            [CF_CFDP_FileDirective_ACK] = CF_CFDP_R2_SubstateRecvFinAck,
        }};
    static const CF_CFDP_R_SubstateDispatchTable_t substate_fns = {
        .state = {[CF_RxSubState_DATA_NORMAL] = &r2_fdir_handlers_normal,
                  [CF_RxSubState_DATA_EOF]    = &r2_fdir_handlers_normal,
                  [CF_RxSubState_VALIDATE]    = &r2_fdir_handlers_done,
                  [CF_RxSubState_FILESTORE]   = &r2_fdir_handlers_done,
                  [CF_RxSubState_FINACK]      = &r2_fdir_handlers_finack,
                  [CF_RxSubState_COMPLETE]    = &r2_fdir_handlers_finack}};

    CF_CFDP_R_DispatchRecv(txn, ph, &substate_fns, CF_CFDP_R_SubstateRecvFileData);

    /* Any sort of recv activity should defer sending additional NAKs.
     * PDUs might be coming across an unreliable network so if we are getting
     * anything at all, whatever might have been NAK'ed might still be in a queue
     * yet to be delivered.  Only after reception goes quiet for a period should we
     * re-evaluate if something still needs [another] NAK.  */
    if (txn->flags.com.ack_timer_armed)
    {
        CF_CFDP_ArmAckTimer(txn);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_HandleFileRetention(CF_Transaction_t *txn)
{
    /*
     * For a receiver:
     *   - If the transfer was successful then the temp file is put into the final
     *     location under the indicated name.
     *   - If the file transfer is unsuccessful then the temp file is deleted.
     */
    char        TempName[CFE_MISSION_MAX_PATH_LEN];
    const char *SubjectFile;
    const char *MoveDest;
    int32       OsStatus;

    CF_CFDP_FinFileStatus_t   PendingFs;
    CF_CFDP_FinDeliveryCode_t PendingDc;

    PendingFs = CF_CFDP_FinFileStatus_INVALID; /* until proven otherwise .... */

    /* If temp file is still opened, close it now */
    if (OS_ObjectIdDefined(txn->fd))
    {
        CF_WrappedClose(txn->fd);
        txn->fd = OS_OBJECT_ID_UNDEFINED;
    }

    /* If the temp file exists, then move it now */
    if (txn->flags.rx.tempfile_created)
    {
        MoveDest = txn->history->fnames.dst_filename;

        /* Receiver-side, data is always in a temp file that needs disposition */
        CF_CFDP_GetTempName(txn->history, TempName, sizeof(TempName));
        SubjectFile = TempName;
    }
    else
    {
        MoveDest    = NULL;
        SubjectFile = NULL;
    }

    if (SubjectFile == NULL)
    {
        /* This means a filesystem error occurred and we could not even create the temp file */
        PendingDc = CF_CFDP_FinDeliveryCode_INVALID;
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
    }
    else if (!txn->flags.com.is_complete)
    {
        /* mark it as incomplete (although the dc should have been set to this
         * already at init) and set a non-success status, just in case one was
         * not already set - this will not overwrite an existing error). */
        PendingDc = CF_CFDP_FinDeliveryCode_INCOMPLETE;
        CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_INVALID_FILE_STRUCTURE);
    }
    else
    {
        /* we got a complete file, but might not be a good file */
        PendingDc = CF_CFDP_FinDeliveryCode_COMPLETE;

        if (!CF_CFDP_TxnIsOK(txn) || CF_CFDP_R_CheckCrc(txn) != CFE_SUCCESS)
        {
            /* mark it with checksum failure (this will not overwrite an existing error status) */
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILE_CHECKSUM_FAILURE);
        }
        else
        {
            OsStatus = OS_mv(SubjectFile, MoveDest);
            if (OsStatus == OS_SUCCESS)
            {
                /* we also put the good file into the correct spot */
                CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_NO_ERROR);
                PendingFs = CF_CFDP_FinFileStatus_RETAINED;

                CFE_EVS_SendEvent(CF_CFDP_R_FILE_RETAINED_EID, CFE_EVS_EventType_INFORMATION,
                                  "CF R%d(%lu:%lu): successfully retained file as %s", CF_CFDP_GetPrintClass(txn),
                                  (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num, MoveDest);

                SubjectFile = NULL; /* its already gone */
            }
            else
            {
                /* the file was good but we cannot store it */
                CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);

                CFE_EVS_SendEvent(CF_CFDP_R_RENAME_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF R%d(%lu:%lu): cannot move file to %s, error=%d", CF_CFDP_GetPrintClass(txn),
                                  (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num, MoveDest,
                                  (int)OsStatus);
            }
        }
    }

    /* If we still have a temp file then remove it */
    if (SubjectFile != NULL)
    {
        OsStatus = OS_remove(SubjectFile);

        CFE_EVS_SendEvent(CF_CFDP_R_NOT_RETAINED_EID, CFE_EVS_EventType_INFORMATION,
                          "CF R%d(%lu:%lu): removed temp file %s, status=%d, txn_stat=%d", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num, SubjectFile,
                          (int)OsStatus, (int)txn->history->txn_stat);

        /* Indicate that the file was discarded */
        /* If this is being discared due to filestore error, there is a special FS for that */
        if (txn->history->txn_stat == CF_TxnStatus_FILESTORE_REJECTION)
        {
            PendingFs = CF_CFDP_FinFileStatus_DISCARDED_FILESTORE;
        }
        else
        {
            PendingFs = CF_CFDP_FinFileStatus_DISCARDED;
        }
    }

    /* Save the DC/FS for reporting to the peer */
    txn->state_data.fin_dc = PendingDc;
    txn->state_data.fin_fs = PendingFs;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_AckTimerTick(CF_Transaction_t *txn)
{
    /* note: the ack timer is only ever armed on class 2 */
    if (!txn->reliable_mode || !txn->flags.com.ack_timer_armed)
    {
        /* nothing to do */
        return;
    }

    if (!CF_Timer_Expired(&txn->ack_timer))
    {
        CF_Timer_Tick(&txn->ack_timer);
    }
    else
    {
        /* disarm the timer */
        txn->flags.com.ack_timer_armed = false;
    }
}

/*----------------------------------------------------------------
 *
 * Local Helper function for R state machine
 *
 *-----------------------------------------------------------------*/
CF_RxSubState_t CF_CFDP_R_CheckState_DATA_NORMAL(CF_Transaction_t *txn)
{
    CF_RxSubState_t next_state = txn->state_data.sub_state;

    /* The "normal" data state just waits for PDUs to come in and collects them, nothing
     * really happens until we get the EOF from the remote */
    if (txn->flags.rx.eof_count != 0)
    {
        /* we got EOF, do gap check tasks */
        next_state = CF_RxSubState_DATA_EOF;
    }
    else if (!CF_CFDP_TxnIsOK(txn))
    {
        /* do file storage tasks (it will be deleted) */
        next_state = CF_RxSubState_FILESTORE;
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for R state machine
 *
 *-----------------------------------------------------------------*/
CF_RxSubState_t CF_CFDP_R_CheckState_DATA_EOF(CF_Transaction_t *txn)
{
    CF_RxSubState_t next_state = txn->state_data.sub_state;

    /* The "eof" data state is basically the same thing, except that we check for gaps
     * and will now send NAKs for anything we do not have.  Note in R1 the only way
     * out of this state is that CheckComplete returns true OR the transaction is canceled.
     * Otherwise eventually the inactivty timer will fire and it will be closed. */
    if (!CF_CFDP_TxnIsOK(txn))
    {
        /* do file storage tasks (it will probably be deleted) */
        next_state = CF_RxSubState_FILESTORE;
    }
    else if (CF_CFDP_R_CheckComplete(txn))
    {
        /* no gaps, do integrity check tasks */
        txn->flags.com.is_complete = true;
        next_state                 = CF_RxSubState_VALIDATE;
    }
    else if (txn->reliable_mode && !txn->flags.rx.send_nak && !txn->flags.com.ack_timer_armed)
    {
        if (CF_CFDP_CheckAckNakCount(txn, &txn->state_data.acknak_count))
        {
            /* ok to NAK again */
            txn->flags.rx.send_nak = true;
        }
        else
        {
            /* failed to fill the gaps */
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_NAK_LIMIT_REACHED);
            next_state = CF_RxSubState_FILESTORE;
        }
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for R state machine
 *
 *-----------------------------------------------------------------*/
CF_RxSubState_t CF_CFDP_R_CheckState_VALIDATE(CF_Transaction_t *txn)
{
    CF_RxSubState_t next_state = txn->state_data.sub_state;

    /* locally computing the CRC */
    if (!CF_CFDP_TxnIsOK(txn) || txn->state_data.cached_pos == txn->fsize)
    {
        next_state = CF_RxSubState_FILESTORE;
    }
    else
    {
        CF_CFDP_R_CalcCrcChunk(txn);
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for R state machine
 *
 *-----------------------------------------------------------------*/
CF_RxSubState_t CF_CFDP_R_CheckState_FILESTORE(CF_Transaction_t *txn)
{
    CF_RxSubState_t next_state = txn->state_data.sub_state;

    /* This applies the retention policy based on whether the transaction was successful */
    /* It sets flags in the txn state if it fails (which will then be sent in the FIN) */
    CF_CFDP_R_HandleFileRetention(txn);

    /* Check if we need to do the FIN.  This is always the case in R2, only by request in R1 */
    if (txn->reliable_mode || txn->flags.com.close_req)
    {
        next_state = CF_RxSubState_FINACK;
    }
    else
    {
        next_state = CF_RxSubState_COMPLETE;
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for R state machine
 *
 *-----------------------------------------------------------------*/
CF_RxSubState_t CF_CFDP_R_CheckState_FINACK(CF_Transaction_t *txn)
{
    CF_RxSubState_t next_state = txn->state_data.sub_state;

    /* just send fin here */
    if (!txn->flags.rx.send_fin)
    {
        /* R1 does not send fin-ack, just move based on sending the FIN */
        if (!txn->reliable_mode || txn->flags.rx.finack_recv || txn->flags.com.inactivity_fired)
        {
            /* We are done */
            next_state = CF_RxSubState_COMPLETE;
        }
        else if (!txn->flags.com.ack_timer_armed)
        {
            if (CF_CFDP_CheckAckNakCount(txn, &txn->state_data.acknak_count))
            {
                /* fin-ack still missing, send FIN again */
                txn->flags.rx.send_fin = true;
            }
            else
            {
                /* this is also considered done, the fin-ack is not strictly needed */
                next_state = CF_RxSubState_COMPLETE;
            }
        }
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
{
    CF_RxSubState_t next_state = txn->state_data.sub_state;

    /* State transitions are done here */
    switch (txn->state_data.sub_state)
    {
        /* The "normal" data state just waits for PDUs to come in and collects them, nothing
         * really happens until we get the EOF from the remote */
        case CF_RxSubState_DATA_NORMAL:
            next_state = CF_CFDP_R_CheckState_DATA_NORMAL(txn);
            break;

        /* The "eof" data state is basically the same thing, except that we check for gaps
         * and will now send NAKs for anything we do not have.  Note in R1 the only way
         * out of this state is that CheckComplete returns true OR the transaction is canceled.
         * Otherwise eventually the inactivty timer will fire and it will be closed. */
        case CF_RxSubState_DATA_EOF:
            next_state = CF_CFDP_R_CheckState_DATA_EOF(txn);
            break;

        case CF_RxSubState_VALIDATE: /* locally computing the CRC */
            next_state = CF_CFDP_R_CheckState_VALIDATE(txn);
            break;

        case CF_RxSubState_FILESTORE:
            next_state = CF_CFDP_R_CheckState_FILESTORE(txn);
            break;

        case CF_RxSubState_FINACK:
            next_state = CF_CFDP_R_CheckState_FINACK(txn);
            break;

        default:
            next_state = CF_RxSubState_COMPLETE;
            break;
    }

    /* did we enter a new state? Do one-time entry things */
    if (next_state != txn->state_data.sub_state)
    {
        CF_TRACE("%s(): State %d -> %d\n", __func__, txn->state_data.sub_state, next_state);

        txn->state_data.sub_state      = next_state;
        txn->flags.com.ack_timer_armed = false;

        switch (next_state)
        {
            case CF_RxSubState_DATA_EOF:
                txn->state_data.acknak_count = 0;
                /* make an initial pass through the NAK check (will be no-op if nothing is missing) */
                /* this is only valid for R2, as R1 does not NAK */
                if (txn->reliable_mode)
                {
                    txn->flags.rx.send_nak = true;
                }
                break;
            case CF_RxSubState_VALIDATE:
                CF_CFDP_R_CalcCrcStart(txn);
                break;
            case CF_RxSubState_FINACK:
                txn->state_data.acknak_count = 0;
                /* Send an initial FIN (note R1 may request a FIN, this is the only time R1 has a reply) */
                txn->flags.rx.send_fin = true;
                break;
            case CF_RxSubState_COMPLETE:
                /* This changes the txn state such that this function is no longer called. */
                CF_CFDP_FinishTransaction(txn, true);
                break;
            default:
                break;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_Tick_Maintenance(CF_Transaction_t *txn)
{
    CFE_Status_t sret;

    /* rx maintenance: possibly process send_eof_ack, send_nak or send_fin */
    if (txn->reliable_mode && txn->flags.rx.eof_ack_count != txn->flags.rx.eof_count)
    {
        sret = CF_CFDP_SendAck(txn, CF_CFDP_FileDirective_EOF);
        if (sret == CFE_SUCCESS)
        {
            /* in the event that EOFs got bunched and we are more than 1 behind,
             * just send one ACK to cover them all.  We do not need to be 1:1. */
            txn->flags.rx.eof_ack_count = txn->flags.rx.eof_count;
        }
    }
    else if (txn->flags.rx.send_nak)
    {
        sret = CF_CFDP_R_SendNak(txn);
        if (sret == CFE_SUCCESS)
        {
            txn->flags.rx.send_nak = false;
        }
    }
    else if (txn->flags.rx.send_fin)
    {
        sret = CF_CFDP_SendFin(txn);
        if (sret == CFE_SUCCESS)
        {
            txn->flags.rx.send_fin = false;

            /* In R2, the FIN needs a FIN-ACK response, so arm the timer. */
            if (txn->reliable_mode)
            {
                /* arm the timer for receipt of the fin-ack response */
                CF_CFDP_ArmAckTimer(txn);
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_r.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_Tick(CF_Transaction_t *txn)
{
    CF_CFDP_AckTxnStatus_t CurrStatus;

    CurrStatus = CF_CFDP_GetAckTxnStatus(txn);

    if (!txn->flags.com.inactivity_fired)
    {
        if (!CF_Timer_Expired(&txn->inactivity_timer))
        {
            CF_Timer_Tick(&txn->inactivity_timer);
        }
        else
        {
            txn->flags.com.inactivity_fired = true;

            /* HOLD state is the normal path to recycle transaction objects, not an error */
            /* inactivity is abnormal in any active state */
            if (CurrStatus == CF_CFDP_AckTxnStatus_ACTIVE)
            {
                CFE_EVS_SendEvent(CF_CFDP_R_INACT_TIMER_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF R%d(%lu:%lu): inactivity timer expired", CF_CFDP_GetPrintClass(txn),
                                  (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer;

                CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_INACTIVITY_DETECTED);
            }
        }
    }

    /* If this is still active (not in hold or drop) then run the state check/change routine */
    if (CurrStatus == CF_CFDP_AckTxnStatus_ACTIVE)
    {
        CF_CFDP_R_AckTimerTick(txn);
        CF_CFDP_R_CheckState(txn);
    }

    CF_CFDP_R_Tick_Maintenance(txn);

    /* if the inactivity timer ran out, then there is no sense
     * pending for responses for anything. */
    if (txn->flags.com.inactivity_fired && txn->state == CF_TxnState_HOLD)
    {
        /* the transaction is now recycleable - this means we will
         * no longer have a record of this transaction seq.  If the sender
         * wakes up or if the network delivers severely delayed PDUs at
         * some future point, then they will be seen as spurious.  They
         * will no longer be associable with this transaction at all */
        CF_CFDP_RecycleTransaction(txn);

        /* NOTE: this must be the last thing in here.  Do not use txn after this */
    }
    else
    {
        CF_CFDP_CompleteTick(txn);
    }
}
