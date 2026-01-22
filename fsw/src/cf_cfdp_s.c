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
#include "cf_eventids.h"
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
CFE_Status_t CF_CFDP_S_SendFileData(CF_Transaction_t *txn, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc)
{
    int32                           status = 0;
    CFE_Status_t                    ret;
    CF_Logical_PduBuffer_t *        ph = CF_CFDP_ConstructPduHeader(txn, 0, CF_AppData.config_table->local_eid,
                                                            txn->history->peer_eid, 0, txn->history->seq_num, 1);
    CF_Logical_PduFileDataHeader_t *fd;
    size_t                          actual_bytes;
    void *                          data_ptr;

    if (!ph)
    {
        ret = CF_SEND_PDU_NO_BUF_AVAIL_ERROR; /* couldn't get message, so no bytes sent. Will try again next time */
    }
    else
    {
        ret = CFE_SUCCESS;

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

        if (txn->state_data.cached_pos != foffs)
        {
            status = CF_WrappedLseek(txn->fd, foffs, OS_SEEK_SET);
            if (status != foffs)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_SEEK_FD_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): error seeking to offset %ld, got %ld", CF_CFDP_GetPrintClass(txn),
                                  (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                                  (long)foffs, (long)status);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
                ret = CF_ERROR;
            }
            else
            {
                txn->state_data.cached_pos = foffs;
            }
        }

        if (ret == CFE_SUCCESS)
        {
            status = CF_WrappedRead(txn->fd, data_ptr, actual_bytes);
            if (status != actual_bytes)
            {
                CFE_EVS_SendEvent(CF_CFDP_S_READ_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S%d(%lu:%lu): error reading bytes: expected %ld, got %ld",
                                  CF_CFDP_GetPrintClass(txn), (unsigned long)txn->history->src_eid,
                                  (unsigned long)txn->history->seq_num, (long)actual_bytes, (long)status);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_read;
                ret = CF_ERROR;
            }
            else
            {
                txn->state_data.cached_pos += actual_bytes;
            }
        }

        if (ret == CFE_SUCCESS)
        {
            CF_CFDP_SendFd(txn, ph); /* CF_CFDP_SendFd only returns CFE_SUCCESS */

            CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.sent.file_data_bytes += actual_bytes;
            if (calc_crc)
            {
                CF_CRC_Digest(&txn->crc, fd->data_ptr, fd->data_len);
            }

            /* The return code needs to pass back the actual chunk size that was sent */
            ret = actual_bytes;
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
void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *txn)
{
    int32 sret;

    if (txn->foffs < txn->fsize)
    {
        /* this returns the actual size of the chunk that was sent, or an error code */
        /* no buffer available is benign; just do nothing and come back next tick */
        sret = CF_CFDP_S_SendFileData(txn, txn->foffs, txn->fsize - txn->foffs, 1);
        if (sret > 0)
        {
            txn->foffs += sret;
        }
        else if (sret != CF_SEND_PDU_NO_BUF_AVAIL_ERROR)
        {
            /* anything other than lack of buffers is an error that will stop the txn */
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_READ_FAILURE);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_SubstateEarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* received early fin, so just cancel */
    CFE_EVS_SendEvent(CF_CFDP_S_EARLY_FIN_ERR_EID, CFE_EVS_EventType_ERROR,
                      "CF S%d(%lu:%lu): got early FIN -- cancelling", CF_CFDP_GetPrintClass(txn),
                      (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);

    /* Set our own status code to remember in history that this was canceled due to early FIN. */
    /* Note this is not a CFDP defined CC.  It will translate to a CANCEL_REQUEST in an ack. */
    CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_EARLY_FIN);

    /* otherwise do normal fin processing */
    CF_CFDP_S_SubstateRecvFin(txn, ph);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_SubstateRecvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CF_Logical_PduFin_t *fin;

    if (CF_CFDP_RecvFin(txn, ph) == CFE_SUCCESS)
    {
        fin = &ph->int_header.fin;

        /* set the CC only on the first time we get the FIN.  If this is a dupe
         * then count and re-ack but otherwise ignore it */
        if (txn->flags.tx.fin_count == 0)
        {
            txn->state_data.peer_cc = fin->cc;
            txn->state_data.fin_dc  = fin->delivery_code;
            txn->state_data.fin_fs  = fin->file_status;

            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_From_ConditionCode(fin->cc));
        }

        if (!CF_CFDP_CheckAckNakCount(txn, &txn->flags.tx.fin_count))
        {
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_POS_ACK_LIMIT_REACHED);
        }

        CF_TRACE("%s(): Recv FIN, cc=%d, dc=%d, st=%d\n", __func__, (int)fin->cc, (int)fin->delivery_code,
                 (int)fin->file_status);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_SubstateNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_SegmentRequest_t *sr;
    const CF_Logical_PduNak_t *        nak;
    uint8                              counter;
    uint8                              bad_sr;

    bad_sr = 0;

    /* this function is only invoked for NAK PDU types */
    if (CF_CFDP_RecvNak(txn, ph) == CFE_SUCCESS)
    {
        nak = &ph->int_header.nak;
        CF_TRACE("%s(): Recv NAK, segment_count=%d\n", __func__, (int)nak->segment_list.num_segments);

        for (counter = 0; counter < nak->segment_list.num_segments; ++counter)
        {
            sr = &nak->segment_list.segments[counter];

            if (sr->offset_start == 0 && sr->offset_end == 0)
            {
                /* need to re-send metadata PDU */
                txn->flags.tx.send_md = true;
                CF_TRACE("%s(): NAK for MD\n", __func__);
            }
            else
            {
                if (sr->offset_end < sr->offset_start)
                {
                    ++bad_sr;
                    continue;
                }

                /* overflow probably won't be an issue */
                if (sr->offset_end > txn->fsize)
                {
                    ++bad_sr;
                    continue;
                }

                /* insert gap data in chunks */
                CF_ChunkListAdd(&txn->chunks->chunks, sr->offset_start, sr->offset_end - sr->offset_start);

                /* note the fact that this has received a NAK of some nature */
                txn->flags.tx.fd_nak_pending = true;
                CF_TRACE("%s(): NAK for FD, offset=%lu->%lu\n", __func__, (unsigned long)sr->offset_start,
                         (unsigned long)sr->offset_end);
            }
        }

        CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.nak_segment_requests +=
            nak->segment_list.num_segments;
        if (bad_sr)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_INVALID_SR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): received %d invalid NAK segment requests", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num, bad_sr);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_CFDP_S_PDU_NAK_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF S%d(%lu:%lu): received invalid NAK PDU", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_SubstateEofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    CFE_Status_t         ret;
    CF_Logical_PduAck_t *ack;

    ret = CF_CFDP_RecvAck(txn, ph);
    if (ret == CFE_SUCCESS)
    {
        ack = &ph->int_header.ack;

        if (ack->ack_directive_code == CF_CFDP_FileDirective_EOF)
        {
            txn->flags.tx.eof_ack_recv = true;
        }

        CF_TRACE("%s(): Recv ACK, dir_code=%d, cc=%d, st=%d\n", __func__, (int)ack->ack_directive_code, (int)ack->cc,
                 (int)ack->txn_status);
    }

    if (ret != CFE_SUCCESS || !txn->flags.tx.eof_ack_recv)
    {
        CFE_EVS_SendEvent(CF_CFDP_S_PDU_EOF_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF S%d(%lu:%lu): received invalid EOF-ACK PDU", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num);
        ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.recv.error;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    /* s1 generally doesn't receive except a possible FIN */
    static const CF_CFDP_FileDirectiveDispatchTable_t s1_normal = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S_SubstateEarlyFin}};
    static const CF_CFDP_FileDirectiveDispatchTable_t s1_eof = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S_SubstateRecvFin}};

    /* In S1 there is no retransmit so only accept during the actual transaction */
    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {
        .substate = {[CF_TxSubState_DATA_NORMAL] = &s1_normal, [CF_TxSubState_DATA_EOF] = &s1_eof}};

    CF_CFDP_S_DispatchRecv(txn, ph, &substate_fns);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_normal = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S_SubstateEarlyFin,
                       [CF_CFDP_FileDirective_NAK] = CF_CFDP_S2_SubstateNak}};
    static const CF_CFDP_FileDirectiveDispatchTable_t s2_eof = {
        .fdirective = {[CF_CFDP_FileDirective_FIN] = CF_CFDP_S_SubstateRecvFin,
                       [CF_CFDP_FileDirective_ACK] = CF_CFDP_S2_SubstateEofAck,
                       [CF_CFDP_FileDirective_NAK] = CF_CFDP_S2_SubstateNak}};

    /* In S2, we must continue allowing all directives until full completion,
     * as the peer may be retransmitting something if it missed our ACK */
    static const CF_CFDP_S_SubstateRecvDispatchTable_t substate_fns = {
        .substate = {[CF_TxSubState_DATA_NORMAL] = &s2_normal,
                     [CF_TxSubState_DATA_EOF]    = &s2_eof,
                     [CF_TxSubState_FILESTORE]   = &s2_eof,
                     [CF_TxSubState_COMPLETE]    = &s2_eof}};

    CF_CFDP_S_DispatchRecv(txn, ph, &substate_fns);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Init(CF_Transaction_t *txn)
{
    int32     OsStatus;
    osal_id_t PendingFd;

    PendingFd = OS_OBJECT_ID_UNDEFINED;

    /* This is a race condition (toctou), but only intended to catch operational mistakes
     * because CFDP does not work well with files that are potentially changing. */
    if (CF_CFDP_TxnIsOK(txn))
    {
        OsStatus = OS_FileOpenCheck(txn->history->fnames.src_filename);
        if (OsStatus == OS_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_ALREADY_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): file %s already open", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              txn->history->fnames.src_filename);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        }
    }

    if (CF_CFDP_TxnIsOK(txn))
    {
        OsStatus = CF_WrappedOpenCreate(&PendingFd, txn->history->fnames.src_filename, OS_FILE_FLAG_NONE, OS_READ_ONLY);
        if (OsStatus < 0)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_OPEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): failed to open file %s, error=%ld", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              txn->history->fnames.src_filename, (long)OsStatus);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_open;
            PendingFd = OS_OBJECT_ID_UNDEFINED; /* just in case */
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        }
    }

    if (CF_CFDP_TxnIsOK(txn))
    {
        OsStatus = CF_WrappedLseek(PendingFd, 0, OS_SEEK_END);
        if (OsStatus < 0)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_SEEK_END_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): failed to seek end file %s, error=%ld", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              txn->history->fnames.src_filename, (long)OsStatus);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        }
        else
        {
            txn->fsize = OsStatus;
        }
    }

    if (CF_CFDP_TxnIsOK(txn))
    {
        OsStatus = CF_WrappedLseek(PendingFd, 0, OS_SEEK_SET);
        if (OsStatus < 0)
        {
            CFE_EVS_SendEvent(CF_CFDP_S_SEEK_BEG_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): failed to seek begin file %s, got %ld", CF_CFDP_GetPrintClass(txn),
                              (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num,
                              txn->history->fnames.src_filename, (long)OsStatus);
            ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.file_seek;
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION);
        }
    }

    if (CF_CFDP_TxnIsOK(txn))
    {
        /* allow the state machine to use the file */
        txn->fd = PendingFd;

        CF_CRC_Start(&txn->crc);

        /* initial MD needs to be sent */
        txn->flags.tx.send_md = true;
    }
    else if (OS_ObjectIdDefined(PendingFd))
    {
        /* Clean up, if an error occurred in here then close the file */
        CF_WrappedClose(PendingFd);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_HandleFileRetention(CF_Transaction_t *txn)
{
    /*
     * For a sender:
     *   - If the transfer is successful and the "keep" flag is false, then it applies
     *     the local file deletion policy, either delete directly or move to recycle dir
     *   - If the transfer is not successful or the "keep" flag is true, then do nothing
     */
    char                      TempName[CFE_MISSION_MAX_PATH_LEN];
    const char *              SubjectFile;
    const char *              MoveDest;
    const CF_ChannelConfig_t *config;
    int32                     OsStatus;
    bool                      AllowLocalRemove;

    config           = &CF_AppData.config_table->chan[txn->chan_num];
    SubjectFile      = txn->history->fnames.src_filename;
    MoveDest         = NULL;
    AllowLocalRemove = false;

    if (!CF_CFDP_TxnIsOK(txn) || !txn->flags.com.is_complete)
    {
        /* Nominally do nothing, we generally should not remove files that did not transfer, UNLESS this
         * was an automatically triggered transfer (i.e. not invoked by command).  In that case doing nothing
         * would cause the file to be transferred again in a loop and the problem may repeat itself in a loop.
         * The "fail dir" is intended to address this loop risk. Moving the file there retains it locally
         * while making it so the automatic transfer will not re-trigger on the same file. */
        if (!txn->flags.tx.cmd_tx)
        {
            MoveDest =
                CF_CFDP_GetMoveTarget(CF_AppData.config_table->fail_dir, SubjectFile, TempName, sizeof(TempName));
        }
    }
    else if (!txn->keep)
    {
        /* It is a candidate for removal if the remote said it retained it */
        if (!txn->reliable_mode)
        {
            /* In class 1, there is no FIN so the status will be UNREPORTED.  This is risky,
             * but if the user said keep=0 with a class 1, then it is assumed they know the risk. */
            AllowLocalRemove = true;
        }
        else
        {
            /* In class 2, check that the peer FIN said it had a completed and retained file */
            AllowLocalRemove = (txn->state_data.fin_fs == CF_CFDP_FinFileStatus_RETAINED &&
                                txn->state_data.fin_dc == CF_CFDP_FinDeliveryCode_COMPLETE);
        }

        if (AllowLocalRemove)
        {
            /* Everything checked out, If move directory is defined attempt move */
            MoveDest = CF_CFDP_GetMoveTarget(config->move_dir, SubjectFile, TempName, sizeof(TempName));
        }
    }

    if (MoveDest != NULL)
    {
        OsStatus = OS_mv(SubjectFile, MoveDest);
        CFE_EVS_SendEvent(CF_CFDP_S_FILE_MOVED_EID, CFE_EVS_EventType_INFORMATION,
                          "CF R%d(%lu:%lu): moved %s -> %s, status=%d", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num, SubjectFile,
                          MoveDest, (int)OsStatus);
    }
    else if (AllowLocalRemove)
    {
        /* no move dir configured, so remove it (because keep == 0 and txn was success) */
        OsStatus = OS_remove(SubjectFile);
        CFE_EVS_SendEvent(CF_CFDP_S_FILE_REMOVED_EID, CFE_EVS_EventType_INFORMATION,
                          "CF R%d(%lu:%lu): removed source file %s, status=%d", CF_CFDP_GetPrintClass(txn),
                          (unsigned long)txn->history->src_eid, (unsigned long)txn->history->seq_num, SubjectFile,
                          (int)OsStatus);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_AckTimerTick(CF_Transaction_t *txn)
{
    /* note: the ack timer is only ever relevant on class 2 */
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
 * Local Helper function for S state machine
 *
 *-----------------------------------------------------------------*/
CF_TxSubState_t CF_CFDP_S_CheckState_DATA_NORMAL(CF_Transaction_t *txn)
{
    CF_TxSubState_t next_state = txn->state_data.sub_state;

    /* The "normal" data state just pumps out PDUs, which is done by a separate sub-tick
     * after all control messages are done.  At this stage we just check if its done sending PDUs. */
    if (txn->foffs >= txn->fsize)
    {
        /* we reached EOF, do maintenance tasks */
        next_state = CF_TxSubState_DATA_EOF;
    }
    else if (!CF_CFDP_TxnIsOK(txn) || txn->flags.tx.fin_count != 0)
    {
        /* do file storage tasks */
        next_state = CF_TxSubState_FILESTORE;
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for S1 state machine
 *
 *-----------------------------------------------------------------*/
CF_TxSubState_t CF_CFDP_S1_CheckState_DATA_EOF(CF_Transaction_t *txn)
{
    CF_TxSubState_t next_state = txn->state_data.sub_state;

    if (!txn->flags.com.close_req)
    {
        /* In S1 when close is not requested, the only thing we need
         * to do is send the EOF, so this means we are done.  There is
         * no fin, no ack, no timer. */
        txn->state_data.fin_dc     = CF_CFDP_FinFileStatus_UNREPORTED;
        txn->flags.com.is_complete = true;
        next_state                 = CF_TxSubState_FILESTORE;
    }
    else if (txn->flags.tx.fin_count != 0)
    {
        /* In S1 when close is requested, got the fin.
         * This should use the DC from the fin (i.e. its not unreported in this case) */
        txn->flags.com.is_complete = true;
        next_state                 = CF_TxSubState_FILESTORE;
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for S state machine
 *
 *-----------------------------------------------------------------*/
CF_TxSubState_t CF_CFDP_S2_CheckState_DATA_EOF(CF_Transaction_t *txn)
{
    CF_TxSubState_t next_state = txn->state_data.sub_state;

    if (txn->flags.tx.fin_count != txn->flags.tx.fin_ack_count)
    {
        /* This means we need to send a FIN-ACK PDU but have not yet sent it (S2 only) */
        /* Do nothing here, let it be sent, come back next time. */
    }
    else if (txn->flags.tx.eof_ack_recv)
    {
        if (txn->flags.tx.fin_count != 0)
        {
            /* Receipt of these means we are done (normal S2 completion) */
            txn->flags.com.is_complete = true;
            next_state                 = CF_TxSubState_FILESTORE;
        }
    }
    else if (!txn->flags.com.ack_timer_armed)
    {
        if (txn->flags.tx.fin_count != 0)
        {
            /* this would mean we are S2 and got the FIN but not the EOF-ACK */
            /* There could be many reasons:
             *  - the peer is sending an "Early" FIN to stop the ttxn->flags.tx.eof_ack_recvransaction
             *    (even though we sent EOF, peer might not be there yet)
             *  - The EOF-ACK was lost in transit and we only got the FIN
             *
             * Receiving a FIN (whether early or at the expected time) always
             * means to stop the transaction, so handle it the same.  The only
             * difference is that we do not set the is_complete flag, because
             * it means that not all protocol requirements were met. */
            next_state = CF_TxSubState_FILESTORE;
        }
        else if (CF_CFDP_CheckAckNakCount(txn, &txn->state_data.acknak_count))
        {
            /* ok to send EOF again, expecting EOF-ACK */
            txn->flags.tx.send_eof = true;
        }
        else
        {
            /* hit limit */
            CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_NAK_LIMIT_REACHED);
            next_state = CF_TxSubState_FILESTORE;
        }
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for S state machine
 *
 *-----------------------------------------------------------------*/
CF_TxSubState_t CF_CFDP_S_CheckState_DATA_EOF(CF_Transaction_t *txn)
{
    CF_TxSubState_t next_state = txn->state_data.sub_state;

    /* The "eof" data state just responds to receipt of control PDUs from the peer.  This may
     * include data NAKs, which are also responded to in a separate sub-tick.  This just needs
     * to check for completeness criteria */
    if (!CF_CFDP_TxnIsOK(txn))
    {
        /* These conditions means we are done (abnormal) */
        next_state = CF_TxSubState_FILESTORE;
    }
    else if (txn->flags.tx.send_eof)
    {
        /* This means we need to send an EOF PDU but have not yet sent it */
        /* Do nothing here, let it be sent, come back next time. */
    }
    else if (!txn->reliable_mode)
    {
        next_state = CF_CFDP_S1_CheckState_DATA_EOF(txn);
    }
    else
    {
        next_state = CF_CFDP_S2_CheckState_DATA_EOF(txn);
    }

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Local Helper function for S state machine
 *
 *-----------------------------------------------------------------*/
CF_TxSubState_t CF_CFDP_S_CheckState_FILESTORE(CF_Transaction_t *txn)
{
    CF_TxSubState_t next_state = txn->state_data.sub_state;

    /* This applies the retention policy based on whether the transaction was successful */
    /* It uses data received in the FIN */
    CF_CFDP_S_HandleFileRetention(txn);
    next_state = CF_TxSubState_COMPLETE;

    return next_state;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_CheckState(CF_Transaction_t *txn)
{
    CF_TxSubState_t next_state = txn->state_data.sub_state;

    /* State transitions are done here */
    switch (txn->state_data.sub_state)
    {
        /* The "normal" data state just pumps out PDUs, which is done by a separate sub-tick
         * after all control messages are done.  At this stage we just check if its done sending PDUs. */
        case CF_TxSubState_DATA_NORMAL:
            next_state = CF_CFDP_S_CheckState_DATA_NORMAL(txn);
            break;

        /* The "eof" data state just responds to receipt of control PDUs from the peer.  This may
         * include data NAKs, which are also responded to in a separate sub-tick.  This just needs
         * to check for completeness criteria */
        case CF_TxSubState_DATA_EOF:
            next_state = CF_CFDP_S_CheckState_DATA_EOF(txn);
            break;

        case CF_TxSubState_FILESTORE:
            next_state = CF_CFDP_S_CheckState_FILESTORE(txn);
            break;

        default:
            next_state = CF_TxSubState_COMPLETE;
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
            case CF_TxSubState_DATA_EOF:
                txn->state_data.acknak_count = 0;
                CF_CRC_Finalize(&txn->crc);
                txn->flags.com.crc_complete = true;
                /* send the initial EOF */
                txn->flags.tx.send_eof = true;
                break;
            case CF_TxSubState_COMPLETE:
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
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Tick(CF_Transaction_t *txn)
{
    CF_CFDP_AckTxnStatus_t CurrStatus;

    CurrStatus = CF_CFDP_GetAckTxnStatus(txn);

    /* at each tick, various timers used by S are checked */
    /* first, check inactivity timer */
    /* Note that the CFDP protocol does not expect any response from the peer until after EOF */
    if (!txn->flags.com.inactivity_fired && txn->state_data.sub_state != CF_TxSubState_DATA_NORMAL)
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
                CFE_EVS_SendEvent(CF_CFDP_S_INACT_TIMER_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF S(%lu:%lu): inactivity timer expired", (unsigned long)txn->history->src_eid,
                                  (unsigned long)txn->history->seq_num);
                ++CF_AppData.hk.Payload.channel_hk[txn->chan_num].counters.fault.inactivity_timer;

                CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_INACTIVITY_DETECTED);
            }
        }
    }

    if (CurrStatus == CF_CFDP_AckTxnStatus_ACTIVE)
    {
        CF_CFDP_S_AckTimerTick(txn);
        CF_CFDP_S_CheckState(txn);
    }

    CF_CFDP_S_Tick_Maintenance(txn);

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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Tick_Maintenance(CF_Transaction_t *txn)
{
    CFE_Status_t sret;

    /* tx maintenance: possibly process send_md, send_eof, or send_fin_ack */
    if (txn->flags.tx.send_md)
    {
        /* this will clear the flag when the MD is sent */
        sret = CF_CFDP_SendMd(txn);
        if (sret == CFE_SUCCESS)
        {
            /* track that MD was sent.  */
            /* Note that MD is not ack'ed in the CFDP protocol, so
             * no timer set here.  If lost, it is only NAK'ed after we
             * send data, otherwise we assume it got there. */
            txn->flags.tx.send_md = false;
        }
    }
    else if (txn->flags.tx.send_eof)
    {
        /* this will clear the flag when the EOF is sent */
        sret = CF_CFDP_SendEof(txn);
        if (sret == CFE_SUCCESS)
        {
            txn->flags.tx.send_eof = false;

            /* In S2, the EOF needs an EOF-ACK response, so arm the timer. */
            if (txn->reliable_mode)
            {
                CF_CFDP_ArmAckTimer(txn);
            }
        }
    }
    else if (txn->reliable_mode && txn->flags.tx.fin_ack_count != txn->flags.tx.fin_count)
    {
        /* this will clear the flag when the FINACK is sent */
        sret = CF_CFDP_SendAck(txn, CF_CFDP_FileDirective_FIN);
        if (sret == CFE_SUCCESS)
        {
            txn->flags.tx.fin_ack_count = txn->flags.tx.fin_count;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_s.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *txn)
{
    const CF_Chunk_t *chunk;
    CFE_Status_t      sret;

    if (txn->flags.tx.fd_nak_pending)
    {
        /* Get first chunk and process if available */
        chunk = CF_ChunkList_GetFirstChunk(&txn->chunks->chunks);
        if (chunk == NULL)
        {
            /* no more chunks, clear the flag */
            txn->flags.tx.fd_nak_pending = false;
        }
        else
        {
            sret = CF_CFDP_S_SendFileData(txn, chunk->offset, chunk->size, 0);
            if (sret > 0)
            {
                /* Something was sent, so take the chunk out of the list */
                CF_ChunkList_RemoveFromFirst(&txn->chunks->chunks, sret);
            }
        }
    }
}
