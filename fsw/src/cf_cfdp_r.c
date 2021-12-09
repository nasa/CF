/************************************************************************
** File: cf_cfdp_r.c
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
**  The CF Application CFDP receive logic source file
**
**  Handles all CFDP engine functionality specific to RX transactions.
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
#include "cf_cfdp_helpers.h"
#include "cf_utils.h"

#include <stdio.h>
#include <string.h>
#include "cf_assert.h"

typedef struct
{
    transaction_t *t;
    pdu_header_t  *ph;
    uint32         gap_counter;
} gap_compute_args_t;

/************************************************************************/
/** \brief Helper function to store condition code set send_fin flag.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_SetCc(transaction_t *t, condition_code_t cc)
{
    t->history->cc       = cc;
    t->flags.rx.send_fin = 1;
}

/************************************************************************/
/** \brief CFDP R1 transaction reset function.
**
**  \par Description
**       All R transactions use this call to indicate the transaction
**       state can be returned to the system. While this function currently
**       only calls CF_CFDP_ResetTransaction(), it is here as a placeholder.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static inline void CF_CFDP_R1_Reset(transaction_t *t)
{
    CF_CFDP_ResetTransaction(t, 1);
}

/************************************************************************/
/** \brief CFDP R2 transaction reset function.
**
**  \par Description
**       Handles reset logic for R2, then calls R1 reset logic.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_Reset(transaction_t *t)
{
    if ((t->state_data.r.sub_state == RECV_WAIT_FOR_FIN_ACK) || (t->state_data.r.r2.eof_cc != CC_NO_ERROR) ||
        (t->history->cc != CC_NO_ERROR) || t->flags.com.canceled)
    {
        CF_CFDP_R1_Reset(t); /* it's done */
    }
    else
    {
        /* not waiting for fin ack, so trigger send fin */
        t->flags.rx.send_fin = 1;
    }
}

/************************************************************************/
/** \brief Checks that the transaction file's CRC matches expected.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on CRC match, otherwise error. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_CheckCrc(transaction_t *t, uint32 expected_crc)
{
    int ret = 0;
    CF_CRC_Finalize(&t->crc);
    if (t->crc.result != expected_crc)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_CRC, CFE_EVS_EventType_ERROR,
                          "CF R%d(%u:%u): crc mismatch for R trans. got 0x%x expected 0x%x", (t->state == CFDP_R2),
                          t->history->src_eid, t->history->seq_num, t->crc.result, expected_crc);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.crc_mismatch;
        ret = 1;
    }

    return ret;
}

/************************************************************************/
/** \brief Checks R2 transaction state for transaction completion status.
**
**  \par Description
**       This function is called anywhere there's a desire to know if the
**       transaction has completed. It may trigger other actions by setting
**       flags to be handled during tick processing. In order for a
**       transaction to be complete, it must have had its meta-data PDU
**       received, the EOF must have been received, and there must be
**       no gaps in the file. EOF is not checked in this function, because
**       it's only called from functions after EOF is received.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_Complete(transaction_t *t, int ok_to_send_nak)
{
    int send_nak = 0;
    int send_fin = 0;
    /* checking if r2 is complete. check nak list, and send NAK if appropriate */
    /* if all data is present, then there will be no gaps in the chunk */

    if (t->history->cc != CC_NO_ERROR)
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
        uint32 ret = CF_Chunks_ComputeGaps(&t->chunks->chunks, 1, t->fsize, 0, NULL, NULL);

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
        if (++t->state_data.r.r2.counter.nak == CF_AppData.config_table->nak_limit)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_NAK_LIMIT, CFE_EVS_EventType_ERROR, "CF R%d(%u:%u): nak limited reach",
                              (t->state == CFDP_R2), t->history->src_eid, t->history->seq_num);
            send_fin = 1;
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.nak_limit;
            t->history->cc =
                CC_NAK_LIMIT_REACHED; /* don't use CF_CFDP_R2_SetCc because many places in this function set send_fin */
            t->state_data.r.r2.counter.nak = 0; /* reset for fin/ack */
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

    /* always go to RECV_FILEDATA, and let tick change state */
    t->state_data.r.sub_state = RECV_FILEDATA;

err_out:;
}

/************************************************************************/
/** \brief Process a filedata PDU on a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. bytes_received must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_ProcessFd(transaction_t *t, CFE_MSG_Size_t *bytes_received)
{
    pdu_header_t *ph = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;
    *bytes_received  = CF_AppData.engine.in.bytes_received;

    int ret = -1;

    /* take out the variable PDU header size */
    *bytes_received -= CF_HeaderSize(ph);

    /* if crc field is present in the pdu header, subtract that from bytes_received */
    if (FGV(ph->flags, PDU_HDR_FLAGS_CRC))
    {
        *bytes_received -= 4;
    }

    /* bytes_received now contains the number of bytes of file data in the pdu */
    if (*bytes_received > sizeof(pdu_file_data_header_t))
    {
        pdu_fd_t *fd = STATIC_CAST(ph, pdu_fd_t);
        int       fret;
        uint32    offset;
        cfdp_ldst_uint32(offset, fd->fdh.offset);
        if (t->state_data.r.cached_pos != offset)
        {
            fret = CF_WrappedLseek(t->fd, offset, OS_SEEK_SET);
            if (fret != offset)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_SEEK_FD, CFE_EVS_EventType_ERROR,
                                  "CF R%d(%u:%u): failed to seek offset %u, got 0x%08x", (t->state == CFDP_R2),
                                  t->history->src_eid, t->history->seq_num, offset, fret);
                t->history->cc = CC_FILE_SIZE_ERROR;
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
                goto err_out; /* connection will reset in caller */
            }
        }

        *bytes_received -= sizeof(pdu_file_data_header_t);
        fret = CF_WrappedWrite(t->fd, fd->fdd.data, *bytes_received);
        if (fret != *bytes_received)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_WRITE, CFE_EVS_EventType_ERROR,
                              "CF R%d(%u:%u): OS_write returned 0x%08x, got 0x%08x", (t->state == CFDP_R2),
                              t->history->src_eid, t->history->seq_num, offset, fret);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_write;
            t->history->cc = CC_FILESTORE_REJECTION;
            goto err_out; /* connection will reset in caller */
        }

        t->state_data.r.cached_pos = (*bytes_received + offset);
        CF_AppData.hk.channel_hk[t->chan_num].counters.recv.file_data_bytes += *bytes_received;
        ret = 0;
    }
    else
    {
        /* file data PDU has 0 bytes -- so drop it */
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped;
    }

err_out:
    return ret;
}

/************************************************************************/
/** \brief Processing receive EOF common functionality for R1/R2.
**
**  \par Description
**       This function is used for both R1 and R2 eof receive. It calls
**       the unmarshaling function and then checks known transaction
**       data against the PDU.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_SubstateRecvEof(transaction_t *t, const pdu_header_t *ph)
{
    int ret = R_EOF_SUCCESS;

    if (!CF_CFDP_RecvEof())
    {
        uint32 size;

        cfdp_ldst_uint32(size, STATIC_CAST(ph, pdu_eof_t)->size);
        /* only check size if MD received, otherwise it's still OK */
        if (t->flags.rx.md_recv && (size != t->fsize))
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_SIZE_MISMATCH, CFE_EVS_EventType_ERROR,
                              "CF R%d(%u:%u): eof file size mismatch: got %u expected %u", (t->state == CFDP_R2),
                              t->history->src_eid, t->history->seq_num, size, t->fsize);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_size_mismatch;
            ret = R_EOF_FSIZE_MISMATCH;
            goto err_out;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_PDU_EOF, CFE_EVS_EventType_ERROR, "CF R%d(%u:%u): invalid eof packet",
                          (t->state == CFDP_R2), t->history->src_eid, t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
        ret = R_EOF_BAD_EOF;
        goto err_out;
    }

err_out:
    return ret;
}

/************************************************************************/
/** \brief Process receive EOF for R1.
**
**  \par Description
**       Only need to confirm crc for R1.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_R1_SubstateRecvEof(transaction_t *t, const pdu_header_t *ph)
{
    int    ret = CF_CFDP_R_SubstateRecvEof(t, ph);
    uint32 crc;

    cfdp_ldst_uint32(crc, STATIC_CAST(ph, pdu_eof_t)->crc);
    if ((ret == R_EOF_SUCCESS) && !CF_CFDP_R_CheckCrc(t, crc))
    {
        /* successfully processed the file */
        t->keep = 1; /* save the file */
    }
    /* if file failed to process, there's nothing to do. CF_CFDP_R_CheckCrc() generates an event on failure */

    /* after exit, always reset since we are done */
    /* reset even if the eof failed -- class 1, so it won't come again! */
    CF_CFDP_R1_Reset(t);
}

/************************************************************************/
/** \brief Process receive EOF for R2.
**
**  \par Description
**       For R2, need to trigger the send of EOF-ACK and then call the
**       check complete function which will either send NAK or FIN.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_R2_SubstateRecvEof(transaction_t *t, const pdu_header_t *ph)
{
    if (!t->flags.rx.eof_recv)
    {
        int ret = CF_CFDP_R_SubstateRecvEof(t, ph);

        /* did receiving eof succeed? */
        if (ret == R_EOF_SUCCESS)
        {
            pdu_eof_t *eof       = STATIC_CAST(ph, pdu_eof_t);
            t->flags.rx.eof_recv = 1;

            /* need to remember the eof crc for later */
            cfdp_ldst_uint32(t->state_data.r.r2.eof_crc, eof->crc);
            cfdp_ldst_uint32(t->state_data.r.r2.eof_size, eof->size);

            /* always ack the EOF, even if we're not done */
            t->state_data.r.r2.eof_cc = FGV(eof->cc, PDU_FLAGS_CC);
            t->flags.rx.send_ack      = 1; /* defer sending ack to tick handling */

            /* only check for complete if EOF with no errors */
            if (t->state_data.r.r2.eof_cc == CC_NO_ERROR)
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
            if (ret == R_EOF_FSIZE_MISMATCH)
            {
                CF_CFDP_R2_SetCc(t, CC_FILE_SIZE_ERROR);
            }
            else
            {
                /* can't do anything with this bad EOF, so return to FILEDATA */
                t->state_data.r.sub_state = RECV_FILEDATA;
            }
        }
    }
}

/************************************************************************/
/** \brief Process received file data for R1.
**
**  \par Description
**       For R1, only need to digest the CRC.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R1_SubstateRecvFileData(transaction_t *t, const pdu_header_t *ph)
{
    CFE_MSG_Size_t bytes_received; /* initialized in CF_CFDP_R_ProcessFd() */

    /* got file data pdu? */
    if (CF_CFDP_RecvFd(t) || CF_CFDP_R_ProcessFd(t, &bytes_received))
    {
        goto err_out;
    }

    /* class 1 digests crc */
    CF_CRC_Digest(&t->crc, STATIC_CAST(ph, pdu_fd_t)->fdd.data, (uint32)bytes_received);

    return;

err_out:
    CF_CFDP_R1_Reset(t);
}

/************************************************************************/
/** \brief Process received file data for R2.
**
**  \par Description
**       For R2, the CRC is checked after the whole file is received
**       since there may be gaps. Instead, insert file received range
**       data into chunks. Once NAK has been received, this function
**       always checks for completion. This function also re-arms
**       the ack timer.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_SubstateRecvFileData(transaction_t *t, const pdu_header_t *ph)
{
    CFE_MSG_Size_t bytes_received; /* initialized in CF_CFDP_R_ProcessFd() */
    uint32         offset;

    /* got file data pdu? */
    if (CF_CFDP_RecvFd(t) || CF_CFDP_R_ProcessFd(t, &bytes_received))
    {
        goto err_out;
    }

    cfdp_ldst_uint32(offset, STATIC_CAST(ph, pdu_fd_t)->fdh.offset);
    /* class 2 does crc at FIN, but track gaps */
    CF_Chunks_Add(&t->chunks->chunks, offset, (uint32)bytes_received);

    if (t->flags.rx.fd_nak_sent)
    {
        CF_CFDP_R2_Complete(t, 0); /* once nak-retransmit received, start checking for completion at each fd */
    }

    if (!t->flags.rx.complete)
    {
        CF_CFDP_ArmAckTimer(t); /* re-arm ack timer, since we got data */
    }

    t->state_data.r.r2.counter.nak = 0;

    return;

err_out:
    CF_CFDP_R2_Reset(t);
}

/************************************************************************/
/** \brief Loads a single NAK segment request.
**
**  \par Description
**       This is a function callback from cf_chunks_compuete_gaps().
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. c must not be NULL. opaque must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_R2_GapCompute(const chunks_t *chunks, const chunk_t *c, void *opaque)
{
    gap_compute_args_t *args = (gap_compute_args_t *)opaque;
    pdu_nak_t          *nak  = STATIC_CAST(args->ph, pdu_nak_t);

    CF_Assert(c->size > 0);

    /* it seems that scope in the old engine is not used the way I read it in the spec, so
     * leave this code here for now for future reference */

    cfdp_ldst_uint32(nak->segment_requests[args->gap_counter].offset_start, (c->offset - nak->scope_start));
    cfdp_ldst_uint32(nak->segment_requests[args->gap_counter].offset_end,
                     (nak->segment_requests[args->gap_counter].offset_start + c->size));
    ++args->gap_counter;
}

/************************************************************************/
/** \brief Send a NAK pdu for R2.
**
**  \par Description
**       NAK pdu is sent when there are gaps in the received data. The
**       chunks class tracks this and generates the nak pdu by calculating
**       gaps internally and calling CF_CFDP_R2_GapCompute(). There is a special
**       case where if a metadata pdu has not been received, then a nak
**       packet will be sent to request another.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_SubstateSendNak(transaction_t *t)
{
    pdu_header_t *ph  = CF_CFDP_ConstructPduHeader(t, PDU_NAK, t->history->peer_eid, CF_AppData.config_table->local_eid,
                                                   1, t->history->seq_num, 1);
    pdu_nak_t    *nak = STATIC_CAST(ph, pdu_nak_t);
    cfdp_send_ret_t sret;

    int ret = -1;

    if (ph)
    {
        if (t->flags.rx.md_recv)
        {
            /* we have metadata, so send valid nak */
            gap_compute_args_t args = {t, ph, 0};
            uint32             cret;

            cfdp_ldst_uint32(nak->scope_start, 0);
            cret = CF_Chunks_ComputeGaps(&t->chunks->chunks,
                                         (t->chunks->chunks.count < t->chunks->chunks.CF_max_chunks)
                                             ? t->chunks->chunks.CF_max_chunks
                                             : (t->chunks->chunks.CF_max_chunks - 1),
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
                cfdp_ldst_uint32(nak->scope_end, 0);
                sret                    = CF_CFDP_SendNak(t, cret);
                t->flags.rx.fd_nak_sent = 1;      /* latch that at least one nak has been sent requesting filedata */
                CF_Assert(sret != CF_SEND_ERROR); /* NOTE: this CF_Assert is here because CF_CFDP_SendNak() does not
                                                     return CF_SEND_ERROR, so if it's ever added to that function we
                                                     need to test handling it here */
                if (sret == CF_SEND_SUCCESS)
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
                              "CF R%d(%u:%u): requesting MD", (t->state == CFDP_R2), t->history->src_eid,
                              t->history->seq_num);
            /* scope start/end, and sr[0] start/end == 0 special value to request metadata */
            cfdp_ldst_uint32(nak->scope_start, 0);
            cfdp_ldst_uint32(nak->scope_end, 0);
            cfdp_ldst_uint32(nak->segment_requests[0].offset_start, 0);
            cfdp_ldst_uint32(nak->segment_requests[0].offset_end, 0);
            sret = CF_CFDP_SendNak(t, 1);
            CF_Assert(
                sret !=
                CF_SEND_ERROR); /* this CF_Assert is here because CF_CFDP_SendNak() does not return CF_SEND_ERROR */
            if (sret == CF_SEND_SUCCESS)
            {
                ret = 0;
            }
        }
    }

    return ret;
}

/************************************************************************/
/** \brief Initialize a transaction structure for R.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R_Init(transaction_t *t)
{
    int32 ret;

    if (t->state == CFDP_R2)
    {
        if (!t->flags.rx.md_recv)
        {
            /* we need to make a temp file and then do a NAK for md pdu */
            /* the transaction already has a history, and that has a buffer that we can use to
             * hold the temp filename */
            /* the -1 below is to make room for the slash */
            snprintf(t->history->fnames.dst_filename, sizeof(t->history->fnames.dst_filename) - 1, "%.*s/%d.tmp",
                     CF_FILENAME_MAX_PATH - 1, CF_AppData.config_table->tmp_dir, t->history->seq_num);
            CFE_EVS_SendEvent(CF_EID_INF_CFDP_R_TEMP_FILE, CFE_EVS_EventType_INFORMATION,
                              "CF R%d(%u:%u): making temp file %s for transaction without MD", (t->state == CFDP_R2),
                              t->history->src_eid, t->history->seq_num, t->history->fnames.dst_filename);
        }

        CF_CFDP_ArmAckTimer(t);
    }

    ret = CF_WrappedOpenCreate(&t->fd, t->history->fnames.dst_filename, OS_FILE_FLAG_CREATE, OS_READ_WRITE);
    if (ret < 0)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_CREAT, CFE_EVS_EventType_ERROR,
                          "CF R%d(%u:%u): failed to create file %s for writing, error=0x%08x", (t->state == CFDP_R2),
                          t->history->src_eid, t->history->seq_num, t->history->fnames.dst_filename, ret);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
        t->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
        if (t->state == CFDP_R2)
        {
            CF_CFDP_R2_SetCc(t, CC_FILESTORE_REJECTION);
        }
        else
        {
            CF_CFDP_R1_Reset(t);
        }
    }
    else
    {
        t->state_data.r.sub_state = RECV_FILEDATA;
    }
}

/************************************************************************/
/** \brief Calculate up to the configured amount of bytes of CRC.
**
**  \par Description
**       The configuration table has a number of bytes to calculate per
**       transaction per wakeup. At each wakeup, the file is read and
**       this number of bytes are calculated. This function will set
**       the checksum error condition code if the final crc does not match.
**
**  \par PTFO
**       Increase throughput by consuming all crc bytes per wakeup in
**       transaction-order. This would require a change to the meaning
**       of the value in the configuration table.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on completion, and -1 on non-completion. Error status is stored in condition code. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R2_CalcCrcChunk(transaction_t *t)
{
    int ret = -1;

    uint8  buf[CF_R2_CRC_CHUNK_SIZE];
    uint32 count_bytes = 0;
#define RXC t->state_data.r.r2.rx_crc_calc_bytes
    if (!RXC)
    {
        CF_CRC_Start(&t->crc);
    }

    while ((count_bytes < CF_AppData.config_table->rx_crc_calc_bytes_per_wakeup) && (RXC < t->fsize))
    {
        const uint32 want_offs_size = (RXC + sizeof(buf));
        const uint32 read_size      = (want_offs_size > t->fsize ? (t->fsize - RXC) : sizeof(buf));
        int          fret;

        if (t->state_data.r.cached_pos != RXC)
        {
            fret = CF_WrappedLseek(t->fd, RXC, OS_SEEK_SET);
            if (fret != RXC)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_SEEK_CRC, CFE_EVS_EventType_ERROR,
                                  "CF R%d(%u:%u): failed to seek offset %u, got 0x%08x", (t->state == CFDP_R2),
                                  t->history->src_eid, t->history->seq_num, RXC, fret);
                t->history->cc = CC_FILE_SIZE_ERROR; /* should be ok to use this one */
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_seek;
                goto err_out;
            }
        }

        fret = CF_WrappedRead(t->fd, buf, read_size);
        if (fret != read_size)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_READ, CFE_EVS_EventType_ERROR,
                              "CF R%d(%u:%u): failed to read file expected %u, got 0x%08x", (t->state == CFDP_R2),
                              t->history->src_eid, t->history->seq_num, read_size, fret);
            t->history->cc = CC_FILE_SIZE_ERROR; /* should be ok to use this one */
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_read;
            goto err_out;
        }

        CF_CRC_Digest(&t->crc, buf, read_size);
        RXC += read_size;
        t->state_data.r.cached_pos = RXC; /* should only call Lseek once */
    }

    if (RXC == t->fsize)
    {
        /* all bytes calculated, so now check */
        if (!CF_CFDP_R_CheckCrc(t, t->state_data.r.r2.eof_crc))
        {
            /* crc matched! we are happy */
            t->keep = 1; /* save the file */

            /* set fin pdu status */
            t->state_data.r.r2.dc = FIN_COMPLETE;
            t->state_data.r.r2.fs = FIN_RETAINED;
        }
        else
        {
            CF_CFDP_R2_SetCc(t, CC_FILE_CHECKSUM_FAILURE);
        }

        t->flags.com.crc_calc = 1;

        ret = 0;
    }

err_out:
    return ret;
}

/************************************************************************/
/** \brief Send a FIN pdu.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R2_SubstateSendFin(transaction_t *t)
{
    cfdp_send_ret_t sret;
    int             ret = -1;

    if (t->history->cc == CC_NO_ERROR && !t->flags.com.crc_calc)
    {
        /* no error, and haven't checked crc -- so start checking it */
        if (CF_CFDP_R2_CalcCrcChunk(t))
        {
            goto err_out; /* signal to caller to re-enter next tick */
        }
    }

    sret = CF_CFDP_SendFin(t, t->state_data.r.r2.dc, t->state_data.r.r2.fs, t->history->cc);
    CF_Assert(sret != CF_SEND_ERROR);                  /* CF_CFDP_SendFin does not return CF_SEND_ERROR */
    t->state_data.r.sub_state = RECV_WAIT_FOR_FIN_ACK; /* whether or not fin send successful, ok to transition state */
    if (sret == CF_SEND_SUCCESS)
    {
        ret = 0;
        goto err_out;
    }

    /* if no message, then try again next time */

err_out:
    return ret;
}

/************************************************************************/
/** \brief Process receive FIN-ACK pdu.
**
**  \par Description
**       This is the end of an R2 transaction. Simply reset the transaction
**       state.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_Recv_fin_ack(transaction_t *t, const pdu_header_t *ph)
{
    if (!CF_CFDP_RecvAck())
    {
        /* got fin ack, so time to close the state */
        CF_CFDP_R2_Reset(t);
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_PDU_FINACK, CFE_EVS_EventType_ERROR, "CF R%d(%u:%u): invalid fin-ack",
                          (t->state == CFDP_R2), t->history->src_eid, t->history->seq_num);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    }
}

/************************************************************************/
/** \brief Process receive metadata pdu for R2.
**
**  \par Description
**       It's possible that metadata PDU was missed in cf_cfdp.c, or that
**       it was re-sent. This function checks if it was already processed,
**       and if not, handles it. If there was a temp file opened due to
**       missed metadata pdu, it will move the file to the correct
**       destination according to the metadata pdu.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. ph must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_RecvMd(transaction_t *t, const pdu_header_t *ph)
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
        status = CF_CFDP_RecvMd(t);
        if (!status)
        {
            /* successfully obtained md pdu */
            if (t->flags.rx.eof_recv)
            {
                /* eof was received, so check that md and eof sizes match */
                if (t->state_data.r.r2.eof_size != t->fsize)
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_EOF_MD_SIZE, CFE_EVS_EventType_ERROR,
                                      "CF R%d(%u:%u): eof/md size mismatch md: %d, eof: %d", (t->state == CFDP_R2),
                                      t->history->src_eid, t->history->seq_num, t->fsize, t->state_data.r.r2.eof_size);
                    ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_size_mismatch;
                    CF_CFDP_R2_SetCc(t, CC_FILE_SIZE_ERROR);
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
                                  "CF R%d(%u:%u): failed to rename file in R2, error=0x%08x", (t->state == CFDP_R2),
                                  t->history->src_eid, t->history->seq_num, status);
                t->fd = OS_OBJECT_ID_UNDEFINED;
                CF_CFDP_R2_SetCc(t, CC_FILESTORE_REJECTION);
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
                                      "CF R%d(%u:%u): failed to open renamed file in R2, error=0x%08x",
                                      (t->state == CFDP_R2), t->history->src_eid, t->history->seq_num, ret);
                    CF_CFDP_R2_SetCc(t, CC_FILESTORE_REJECTION);
                    ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.file_open;
                    t->fd = OS_OBJECT_ID_UNDEFINED; /* just in case */
                    goto err_out;
                }
            }

            t->flags.rx.md_recv            = 1;
            t->state_data.r.r2.counter.nak = 0; /* in case part of nak */
            CF_CFDP_R2_Complete(t, 1);          /* check for completion now that md is received */
        }
        else
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_PDU_MD, CFE_EVS_EventType_ERROR, "CF R%d(%u:%u): invalid md received",
                              (t->state == CFDP_R2), t->history->src_eid, t->history->seq_num);
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
            /* do nothing here, since it will be nak'd again later */
        }
    }

err_out:;
}

/************************************************************************/
/** \brief Dispatch function for all received packets.
**
**  \par Description
**       For either R1 or R2 this function handles common logic for
**       state processing based on current sub-state and the received
**       pdu type.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. fns must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R_DispatchRecv(transaction_t *t,
                                   void (*const fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t *,
                                                                                       const pdu_header_t *),
                                   void (*const fd_fn)(transaction_t *, const pdu_header_t *))
{
    CF_Assert(t->state_data.r.sub_state < RECV_NUM_STATES);
    CF_Assert(CF_AppData.engine.in.msg);

    pdu_header_t *ph = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;

    /* the 2d jump table is only used with file directive pdu */
    if (!FGV(ph->flags, PDU_HDR_FLAGS_TYPE))
    {
        pdu_file_directive_header_t *fdh = STATIC_CAST(ph, pdu_file_directive_header_t);
        if (fdh->directive_code < PDU_INVALID_MAX)
        {
            if (fns[t->state_data.r.sub_state][fdh->directive_code])
            {
                fns[t->state_data.r.sub_state][fdh->directive_code](t, ph);
            }
        }
        else
        {
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious;
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_DC_INV, CFE_EVS_EventType_ERROR,
                              "CF R%d(%u:%u): received pdu with invalid directive code %d for sub-state %d",
                              (t->state == CFDP_R2), t->history->src_eid, t->history->seq_num, fdh->directive_code,
                              t->state_data.r.sub_state);
        }
    }
    else
    {
        if (t->history->cc == CC_NO_ERROR)
        {
            fd_fn(t, ph); /* if history shows error, drop filedata pdu on the floor */
        }
        else
        {
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped;
        }
    }
}

/************************************************************************/
/** \brief R1 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R1_Recv(transaction_t *t)
{
    static void (*const substate_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *) = {
        {NULL, NULL, NULL, NULL, CF_CFDP_R1_SubstateRecvEof, NULL, NULL, NULL, NULL, NULL, NULL}, /* RECV_FILEDATA */
        {NULL, NULL, NULL, NULL, CF_CFDP_R1_SubstateRecvEof, NULL, NULL, NULL, NULL, NULL, NULL}, /* RECV_EOF */
        {NULL, NULL, NULL, NULL, CF_CFDP_R1_SubstateRecvEof, NULL, NULL, NULL, NULL, NULL,
         NULL}, /* RECV_WAIT_FOR_FIN_ACK */
    };

    CF_CFDP_R_DispatchRecv(t, substate_fns, CF_CFDP_R1_SubstateRecvFileData);
}

/************************************************************************/
/** \brief R2 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R2_Recv(transaction_t *t)
{
    static void (*const substate_fns[RECV_NUM_STATES][PDU_INVALID_MAX])(transaction_t * t, const pdu_header_t *) = {
        {NULL, NULL, NULL, NULL, CF_CFDP_R2_SubstateRecvEof, NULL, NULL, CF_CFDP_R2_RecvMd, NULL, NULL,
         NULL}, /* RECV_FILEDATA */
        {NULL, NULL, NULL, NULL, CF_CFDP_R2_SubstateRecvEof, NULL, NULL, CF_CFDP_R2_RecvMd, NULL, NULL,
         NULL}, /* RECV_EOF */
        {NULL, NULL, NULL, NULL, CF_CFDP_R2_SubstateRecvEof, NULL, CF_CFDP_R2_Recv_fin_ack, NULL, NULL, NULL,
         NULL}, /* RECV_WAIT_FOR_FIN_ACK */
    };

    CF_CFDP_R_DispatchRecv(t, substate_fns, CF_CFDP_R2_SubstateRecvFileData);
}

/************************************************************************/
/** \brief Cancel an R transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R_Cancel(transaction_t *t)
{
    /* for cancel, only need to send FIN if R2 */
    if ((t->state == CFDP_R2) && (t->state_data.r.sub_state < RECV_WAIT_FOR_FIN_ACK))
    {
        t->flags.rx.send_fin = 1;
    }
    else
    {
        CF_CFDP_R1_Reset(t); /* if R1, just call it quits */
    }
}

/************************************************************************/
/** \brief Sends an inactivity timer expired event to EVS.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static inline void CF_CFDP_R_SendInactivityEvent(transaction_t *t)
{
    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_INACT_TIMER, CFE_EVS_EventType_ERROR, "CF R%d(%u:%u): inactivity timer expired",
                      (t->state == CFDP_R2), t->history->src_eid, t->history->seq_num);
    ++CF_AppData.hk.channel_hk[t->chan_num].counters.fault.inactivity_timer;
}

/************************************************************************/
/** \brief Perform tick (time-based) processing for R transactions.
**
**  \par Description
**       This function is called on every transaction by the engine on
**       every CF wakeup. This is where flags are checked to send ACK,
**       NAK, and FIN. It checks for inactivity timer and processes the
**       ack timer. The ack timer is what triggers re-sends of PDUs
**       that require acknowledgment.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. cont is unused, so may be NULL
**
*************************************************************************/
void CF_CFDP_R_Tick(transaction_t *t, int *cont /* unused */)
{
    /* Steven is not real happy with this function. There should be a better way to separate out
     * the logic by state so that it isn't a bunch of if statements for different flags
     */
    /* at each tick, various timers used by R are checked */
    /* first, check inactivity timer */
    if (t->state == CFDP_R2)
    {
        if (!t->flags.rx.inactivity_fired)
        {
            if (CF_Timer_Expired(&t->inactivity_timer))
            {
                CF_CFDP_R_SendInactivityEvent(t);

                CF_CFDP_R2_SetCc(t, CC_INACTIVITY_DETECTED);
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
            cfdp_send_ret_t sret = CF_CFDP_SendAck(t, ACK_TS_ACTIVE, PDU_EOF, t->state_data.r.r2.eof_cc,
                                                   t->history->peer_eid, t->history->seq_num);
            CF_Assert(sret != CF_SEND_ERROR);

            /* if CF_SEND_SUCCESS, then move on in the state machine. CF_CFDP_SendAck does not return CF_SEND_ERROR */
            if (sret != CF_SEND_NO_MSG)
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
                else if (t->state_data.r.sub_state == RECV_WAIT_FOR_FIN_ACK)
                {
                    if (++t->state_data.r.r2.counter.ack == CF_AppData.config_table->ack_limit)
                    {
                        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_ACK_LIMIT, CFE_EVS_EventType_ERROR,
                                          "CF R2(%u:%u): ack limit reached, no fin-ack", t->history->src_eid,
                                          t->history->seq_num);
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
