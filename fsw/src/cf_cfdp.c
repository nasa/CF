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
 *  The CF Application main cfdp engine and pdu parsing implementation
 *
 *  This file contains two sets of functions. The first is what is needed
 *  to deal with CFDP PDUs. Specifically validating them for correctness
 *  and ensuring the byte-order is correct for the target. The second
 *  is incoming and outgoing CFDP PDUs pass through here. All receive
 *  CFDP PDU logic is performed here and the data is passed to the
 *  R (rx) and S (tx) logic.
 */

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include "cf_cfdp_r.h"
#include "cf_cfdp_s.h"
#include "cf_cfdp_dispatch.h"
#include "cf_cfdp_sbintf.h"

#include <string.h>
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_EncodeStart
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size)
{
    /* Clear the PDU buffer structure to start */
    memset(ph, 0, sizeof(*ph));

    /* attach encoder object to PDU buffer which is attached to SB (encapsulation) buffer */
    penc->base = (uint8 *)msgbuf;
    ph->penc   = penc;

    CF_CFDP_CodecReset(&penc->codec_state, total_size);

    /*
     * adjust so that the base points to the actual PDU Header, this makes the offset
     * refer to the real offset within the CFDP PDU, rather than the offset of the SB
     * msg container.
     */
    if (total_size > encap_hdr_size)
    {
        penc->codec_state.max_size -= encap_hdr_size;
        penc->base += encap_hdr_size;
    }
    else
    {
        CF_CFDP_CodecSetDone(&penc->codec_state);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_DecodeStart
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, const void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size)
{
    /* Clear the PDU buffer structure to start */
    memset(ph, 0, sizeof(*ph));

    /* attach decoder object to PDU buffer which is attached to SB (encapsulation) buffer */
    pdec->base = (const uint8 *)msgbuf;
    ph->pdec   = pdec;

    CF_CFDP_CodecReset(&pdec->codec_state, total_size);

    /*
     * adjust so that the base points to the actual PDU Header, this makes the offset
     * refer to the real offset within the CFDP PDU, rather than the offset of the SB
     * msg container.
     */
    if (total_size > encap_hdr_size)
    {
        pdec->codec_state.max_size -= encap_hdr_size;
        pdec->base += encap_hdr_size;
    }
    else
    {
        CF_CFDP_CodecSetDone(&pdec->codec_state);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ArmAckTimer
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ArmAckTimer(CF_Transaction_t *t)
{
    CF_Timer_InitRelSec(&t->ack_timer, CF_AppData.config_table->ack_timer_s);
    t->flags.com.ack_timer_armed = 1;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_GetClass
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline CF_CFDP_Class_t CF_CFDP_GetClass(const CF_Transaction_t *ti)
{
    CF_Assert(ti->flags.com.q_index != CF_QueueIdx_FREE);
    return !!((ti->state == CF_TxnState_S2) || (ti->state == CF_TxnState_R2));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_IsSender
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline int CF_CFDP_IsSender(CF_Transaction_t *ti)
{
    CF_Assert(ti->flags.com.q_index != CF_QueueIdx_FREE);
    /* the state could actually be CF_TxnState_IDLE, which is still not a sender. This would
     * be an unused transaction in the RX (CF_CFDP_ReceiveMessage) path. */
    return !!((ti->state == CF_TxnState_S1) || (ti->state == CF_TxnState_S2));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ArmInactTimer
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline void CF_CFDP_ArmInactTimer(CF_Transaction_t *t)
{
    CF_Timer_InitRelSec(&t->inactivity_timer, CF_AppData.config_table->inactivity_timer_s);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_DispatchRecv
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    static const CF_CFDP_TxnRecvDispatchTable_t state_fns = {.rx = {[CF_TxnState_IDLE] = CF_CFDP_RecvIdle,
                                                                    [CF_TxnState_R1]   = CF_CFDP_R1_Recv,
                                                                    [CF_TxnState_S1]   = CF_CFDP_S1_Recv,
                                                                    [CF_TxnState_R2]   = CF_CFDP_R2_Recv,
                                                                    [CF_TxnState_S2]   = CF_CFDP_S2_Recv,
                                                                    [CF_TxnState_DROP] = CF_CFDP_RecvDrop}};

    CF_CFDP_RxStateDispatch(t, ph, &state_fns);
    CF_CFDP_ArmInactTimer(t); /* whenever a packet was received by the other size, always arm its inactivity timer */
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_DispatchTx
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_DispatchTx(CF_Transaction_t *t)
{
    static const CF_CFDP_TxnSendDispatchTable_t state_fns = {
        .tx = {[CF_TxnState_S1] = CF_CFDP_S1_Tx, [CF_TxnState_S2] = CF_CFDP_S2_Tx}};

    CF_CFDP_TxStateDispatch(t, &state_fns);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_FindUnusedChunks
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static CF_ChunkWrapper_t *CF_CFDP_FindUnusedChunks(CF_Channel_t *c, CF_Direction_t dir)
{
    CF_ChunkWrapper_t *ret;

    CF_Assert(dir < CF_Direction_NUM);
    CF_Assert(c->cs[dir]);

    ret = container_of(CF_CList_Pop(&c->cs[dir]), CF_ChunkWrapper_t, cl_node);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_SetPduLength
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_SetPduLength(CF_Logical_PduBuffer_t *ph)
{
    uint16 final_pos;

    /* final position of the encoder state should reflect the entire PDU length */
    final_pos = CF_CODEC_GET_POSITION(ph->penc);

    if (final_pos >= ph->pdu_header.header_encoded_length)
    {
        /* the value that goes into the packet is length _after_ header */
        ph->pdu_header.data_encoded_length = final_pos - ph->pdu_header.header_encoded_length;
    }

    CF_CFDP_EncodeHeaderFinalSize(ph->penc, &ph->pdu_header);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ConstructPduHeader
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *t, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent)
{
    /* directive_code == 0 if file data */
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduHeader_t *hdr;
    uint8                   eid_len;

    ph = CF_CFDP_MsgOutGet(t, silent);

    if (ph)
    {
        hdr = &ph->pdu_header;

        hdr->version   = 1;
        hdr->pdu_type  = (directive_code == 0); /* set to '1' for file data pdu, '0' for a directive pdu */
        hdr->direction = (towards_sender != 0); /* set to '1' for toward sender, '0' for toward recevier */
        hdr->txm_mode  = (CF_CFDP_GetClass(t) == CF_CFDP_CLASS_1); /* set to '1' for class 1 data, '0' for class 2 */

        /* choose the larger of the two EIDs to determine size */
        if (src_eid > dst_eid)
        {
            eid_len = CF_CFDP_GetValueEncodedSize(src_eid);
        }
        else
        {
            eid_len = CF_CFDP_GetValueEncodedSize(dst_eid);
        }

        /*
         * This struct holds the "real" length - when assembled into the final packet
         * this is encoded as 1 less than this value
         */
        hdr->eid_length     = eid_len;
        hdr->txn_seq_length = CF_CFDP_GetValueEncodedSize(tsn);

        hdr->source_eid      = src_eid;
        hdr->destination_eid = dst_eid;
        hdr->sequence_num    = tsn;

        /*
         * encode the known parts so far.  total_size field cannot be
         * included yet because its value is not known, but the basic
         * encoding of the other stuff needs to be done so the position
         * of any data fields can be determined.
         */
        CF_CFDP_EncodeHeaderWithoutSize(ph->penc, hdr);

        /* If directive code is zero, the pdu is a file data pdu which has no directive code field.
         * So only set if non-zero, otherwise it will write a 0 to a byte in a file data pdu where we
         * don't necessarily want a 0. */
        if (directive_code)
        {
            /* set values which can be determined at this time */
            ph->fdirective.directive_code = directive_code;

            CF_CFDP_EncodeFileDirectiveHeader(ph->penc, &ph->fdirective);
        }
    }

    return ph;
}

/*----------------------------------------------------------------
 *
 * Function: CF_strnlen
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static inline size_t CF_strnlen(const char *s, size_t maxlen)
{
    const char *end = memchr(s, 0, maxlen);
    if (end != NULL)
    {
        /* actual length of string is difference */
        maxlen = end - s;
    }
    return maxlen;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_SendMd
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_SendRet_t CF_CFDP_SendMd(CF_Transaction_t *t)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_METADATA, CF_AppData.config_table->local_eid,
                                   t->history->peer_eid, 0, t->history->seq_num, 0);
    CF_Logical_PduMd_t *md;
    CF_SendRet_t        sret;

    sret = CF_SendRet_SUCCESS;

    if (!ph)
    {
        sret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    md = &ph->int_header.md;

    CF_Assert((t->state == CF_TxnState_S1) || (t->state == CF_TxnState_S2));

    md->size = t->fsize;

    /* at this point, need to append filenames into md packet */
    /* this does not actually copy here - that is done during encode */
    md->source_filename.length = CF_strnlen(t->history->fnames.src_filename, sizeof(t->history->fnames.src_filename));
    ;
    md->source_filename.data_ptr = t->history->fnames.src_filename;
    md->dest_filename.length     = CF_strnlen(t->history->fnames.dst_filename, sizeof(t->history->fnames.dst_filename));
    ;
    md->dest_filename.data_ptr = t->history->fnames.dst_filename;

    CF_CFDP_EncodeMd(ph->penc, md);
    CF_CFDP_SetPduLength(ph);
    CF_CFDP_Send(t->chan_num, ph);

err_out:
    return sret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_SendFd
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_SendRet_t CF_CFDP_SendFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* NOTE: SendFd does not need a call to CF_CFDP_MsgOutGet, as the caller already has it */
    CF_SendRet_t ret = CF_SendRet_SUCCESS;

    /* this should check if any encoding error occurred */

    /* update pdu length */
    CF_CFDP_SetPduLength(ph);
    CF_CFDP_Send(t->chan_num, ph);

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_AppendTlv
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type)
{
    CF_Logical_Tlv_t *ptlv;

    if (ptlv_list->num_tlv < CF_PDU_MAX_TLV)
    {
        ptlv = &ptlv_list->tlv[ptlv_list->num_tlv];
        ++ptlv_list->num_tlv;
    }
    else
    {
        ptlv = NULL;
    }

    if (ptlv)
    {
        ptlv->type = tlv_type;

        if (tlv_type == CF_CFDP_TLV_TYPE_ENTITY_ID)
        {
            ptlv->data.eid = CF_AppData.config_table->local_eid;
            ptlv->length   = CF_CFDP_GetValueEncodedSize(ptlv->data.eid);
        }
        else
        {
            ptlv->data.data_ptr = NULL;
            ptlv->length        = 0;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_SendEof
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_SendRet_t CF_CFDP_SendEof(CF_Transaction_t *t)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_EOF, CF_AppData.config_table->local_eid,
                                   t->history->peer_eid, 0, t->history->seq_num, 0);
    CF_Logical_PduEof_t *eof;
    CF_SendRet_t         ret = CF_SendRet_SUCCESS;

    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    eof = &ph->int_header.eof;

    eof->cc   = t->history->cc;
    eof->crc  = t->crc.result;
    eof->size = t->fsize;

    if (t->history->cc != CF_CFDP_ConditionCode_NO_ERROR)
    {
        CF_CFDP_AppendTlv(&eof->tlv_list, CF_CFDP_TLV_TYPE_ENTITY_ID);
    }

    CF_CFDP_EncodeEof(ph->penc, eof);
    CF_CFDP_SetPduLength(ph);
    CF_CFDP_Send(t->chan_num, ph);

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_SendAck
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_SendRet_t CF_CFDP_SendAck(CF_Transaction_t *t, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn)
{
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduAck_t    *ack;
    CF_SendRet_t            ret = CF_SendRet_SUCCESS;
    CF_EntityId_t           src_eid;
    CF_EntityId_t           dst_eid;

    CF_Assert((dir_code == CF_CFDP_FileDirective_EOF) || (dir_code == CF_CFDP_FileDirective_FIN));

    if (CF_CFDP_IsSender(t))
    {
        src_eid = CF_AppData.config_table->local_eid;
        dst_eid = peer_eid;
    }
    else
    {
        src_eid = peer_eid;
        dst_eid = CF_AppData.config_table->local_eid;
    }

    ph = CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_ACK, src_eid, dst_eid,
                                    (dir_code == CF_CFDP_FileDirective_EOF), tsn, 0);
    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    ack = &ph->int_header.ack;

    ack->ack_directive_code = dir_code;
    ack->ack_subtype_code   = 1; /* looks like always 1 if not extended features */
    ack->cc                 = cc;
    ack->txn_status         = ts;

    CF_CFDP_EncodeAck(ph->penc, ack);
    CF_CFDP_SetPduLength(ph);
    CF_CFDP_Send(t->chan_num, ph);
err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_SendFin
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_SendRet_t CF_CFDP_SendFin(CF_Transaction_t *t, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc)
{
    CF_Logical_PduBuffer_t *ph =
        CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_FIN, t->history->peer_eid,
                                   CF_AppData.config_table->local_eid, 1, t->history->seq_num, 0);
    CF_Logical_PduFin_t *fin;
    CF_SendRet_t         ret = CF_SendRet_SUCCESS;

    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    fin = &ph->int_header.fin;

    fin->cc            = cc;
    fin->delivery_code = dc;
    fin->file_status   = fs;

    if (cc != CF_CFDP_ConditionCode_NO_ERROR)
    {
        CF_CFDP_AppendTlv(&fin->tlv_list, CF_CFDP_TLV_TYPE_ENTITY_ID);
    }

    CF_CFDP_EncodeFin(ph->penc, fin);
    CF_CFDP_SetPduLength(ph);
    CF_CFDP_Send(t->chan_num, ph);

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_SendNak
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_SendRet_t CF_CFDP_SendNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    CF_Logical_PduNak_t *nak;
    CF_SendRet_t         ret = CF_SendRet_SUCCESS;

    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    CF_Assert(CF_CFDP_GetClass(t) == CF_CFDP_CLASS_2);

    nak = &ph->int_header.nak;

    /*
     * NOTE: the caller should have already initialized all the fields.
     * This does not need to add anything more to the NAK here
     */

    CF_CFDP_EncodeNak(ph->penc, nak);
    CF_CFDP_SetPduLength(ph);
    CF_CFDP_Send(t->chan_num, ph);

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvPh
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_RecvPh(uint8 chan_num, CF_Logical_PduBuffer_t *ph)
{
    CF_Assert(chan_num < CF_NUM_CHANNELS);

    CF_CFDP_DecodeHeader(ph->pdec, &ph->pdu_header);

    /*
     * The "large file" flag is not supported by this implementation yet.
     * This means file sizes and offsets will be 64 bits, so codec routines
     * will need to be updated to understand this.  OSAL also doesn't support
     * 64-bit file access yet.
     */
    if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.large_flag)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_LARGE_FILE, CFE_EVS_EventType_ERROR,
                          "CF: pdu with large file bit received (unsupported)");
        goto err_out;
    }

    if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.pdu_type == 0)
    {
        CF_CFDP_DecodeFileDirectiveHeader(ph->pdec, &ph->fdirective);
    }

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_SHORT_HEADER, CFE_EVS_EventType_ERROR, "CF: pdu too short (%lu received)",
                          (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        goto err_out;
    }

    /* pdu is ok, so continue processing */
    ++CF_AppData.hk.channel_hk[chan_num].counters.recv.pdu;
    /* going to leave two return statements in, since they are at the end of the function.
     * one is normal exit path, and one is error exit path */
    return 0;

err_out:
    ++CF_AppData.hk.channel_hk[chan_num].counters.recv.error;
    return -1;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvMd
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    const CF_Logical_PduMd_t *md = &ph->int_header.md;
    int                       lv_ret;

    CF_CFDP_DecodeMd(ph->pdec, &ph->int_header.md);
    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_MD_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: metadata packet too short: %lu bytes received",
                          (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        goto err_out;
    }

    /* store the expected file size in transaction */
    t->fsize = md->size;

    /*
     * store the filenames in transaction.
     *
     * NOTE: The "CF_CFDP_CopyStringFromLV()" now knows that the data is supposed to be a C string,
     * and ensures that the output content is properly terminated, so this only needs to check that
     * it worked.
     */
    lv_ret = CF_CFDP_CopyStringFromLV(t->history->fnames.src_filename, sizeof(t->history->fnames.src_filename),
                                      &md->source_filename);
    if (lv_ret < 0)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_INVALID_SRC_LEN, CFE_EVS_EventType_ERROR,
                          "CF: metadata pdu rejected due to invalid length in source filename of 0x%02x",
                          md->source_filename.length);
        goto err_out;
    }
    lv_ret = CF_CFDP_CopyStringFromLV(t->history->fnames.dst_filename, sizeof(t->history->fnames.dst_filename),
                                      &md->dest_filename);
    if (lv_ret < 0)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_INVALID_DST_LEN, CFE_EVS_EventType_ERROR,
                          "CF: metadata pdu rejected due to invalid length in dest filename of 0x%02x",
                          md->dest_filename.length);
        goto err_out;
    }

    CFE_EVS_SendEvent(CF_EID_INF_PDU_MD_RECVD, CFE_EVS_EventType_INFORMATION,
                      "CF: md received for source: %s, dest: %s", t->history->fnames.src_filename,
                      t->history->fnames.dst_filename);

    /* normal return path */
    return 0;

err_out:
    ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    /* error return path */
    return -1;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvFd
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_RecvFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    int ret = 0;

    CF_CFDP_DecodeFileDataHeader(ph->pdec, ph->pdu_header.segment_meta_flag, &ph->int_header.fd);

    /* if the CRC flag is set, need to deduct the size of the CRC from the data - always 32 bits */
    if (CF_CODEC_IS_OK(ph->pdec) && ph->pdu_header.crc_flag)
    {
        if (ph->int_header.fd.data_len < sizeof(CF_CFDP_uint32_t))
        {
            CF_CODEC_SET_DONE(ph->pdec);
        }
        else
        {
            ph->int_header.fd.data_len -= sizeof(CF_CFDP_uint32_t);
        }
    }

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_FD_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: filedata pdu too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
        ret = -1;
    }
    else if (ph->pdu_header.segment_meta_flag)
    {
        /* If recv PDU has the "segment_meta_flag" set, this is not currently handled in CF. */
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_FD_UNSUPPORTED, CFE_EVS_EventType_ERROR,
                          "CF: filedata pdu with segment metadata received");
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
        ret = -1;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvEof
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_RecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    int ret = 0;

    CF_CFDP_DecodeEof(ph->pdec, &ph->int_header.eof);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_EOF_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: eof pdu too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = -1;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvAck
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_RecvAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    int ret = 0;

    CF_CFDP_DecodeAck(ph->pdec, &ph->int_header.ack);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_ACK_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: ack pdu too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = -1;
    }

    /* nothing to do for this one, as all fields are bytes */
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvFin
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_RecvFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    int ret = 0;

    CF_CFDP_DecodeFin(ph->pdec, &ph->int_header.fin);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_FIN_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: fin pdu too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = -1;
    }

    /* NOTE: right now we don't care about the fault location */
    /* nothing to do for this one. all fields are bytes */
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvNak
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_RecvNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    int ret = 0;

    CF_CFDP_DecodeNak(ph->pdec, &ph->int_header.nak);

    if (!CF_CODEC_IS_OK(ph->pdec))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_NAK_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: nak pdu too short: %lu bytes received", (unsigned long)CF_CODEC_GET_SIZE(ph->pdec));
        ret = -1;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvDrop
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_RecvDrop(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RecvIdle
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_RecvIdle(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    CF_Logical_PduFileDirectiveHeader_t *fdh;
    int                                  status;

    /* only RX transactions dare tread here */
    t->history->seq_num = ph->pdu_header.sequence_num;

    /* peer_eid is always the remote partner. src_eid is always the transaction source.
     * in this case, they are the same */
    t->history->peer_eid = ph->pdu_header.source_eid;
    t->history->src_eid  = ph->pdu_header.source_eid;

    t->chunks = CF_CFDP_FindUnusedChunks(&CF_AppData.engine.channels[t->chan_num], CF_Direction_RX);

    /* this is an idle transaction, so see if there's a received packet that can
     * be bound to the transaction */
    if (ph->pdu_header.pdu_type)
    {
        /* file data PDU */
        /* being idle and receiving a file data PDU means that no active transaction knew
         * about the transaction in progress, so most likely PDUs were missed. */

        /* if class 2, switch into R2 state and let it handle */
        /* don't forget to bind the transaction */
        if (ph->pdu_header.txm_mode)
        {
            /* R1, can't do anything without metadata first */
            t->state = CF_TxnState_DROP; /* drop all incoming */
            /* use inactivity timer to ultimately free the state */
        }
        else
        {
            /* R2 can handle missing metadata, so go ahead and create a temp file */
            t->state = CF_TxnState_R2;
            CF_CFDP_R_Init(t);
            CF_CFDP_DispatchRecv(t, ph); /* re-dispatch to enter r2 */
        }
    }
    else
    {
        fdh = &ph->fdirective;

        /* file directive PDU, but we are in an idle state. It only makes sense right now to accept metadata PDU. */
        switch (fdh->directive_code)
        {
            case CF_CFDP_FileDirective_METADATA:
                status = CF_CFDP_RecvMd(t, ph);
                if (!status)
                {
                    /* NOTE: whether or not class 1 or 2, get a free chunks. it's cheap, and simplifies cleanup path */
                    t->state            = ph->pdu_header.txm_mode ? CF_TxnState_R1 : CF_TxnState_R2;
                    t->flags.rx.md_recv = 1;
                    CF_CFDP_R_Init(t); /* initialize R */
                }
                else
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_IDLE_MD, CFE_EVS_EventType_ERROR,
                                      "CF: got invalid md pdu -- abandoning transaction");
                    ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
                    /* leave state as idle, which will reset below */
                }
                break;
            default:
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_FD_UNHANDLED, CFE_EVS_EventType_ERROR,
                                  "CF: unhandled file directive code 0x%02x in idle state", fdh->directive_code);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
                break;
        }
    }

    if (t->state == CF_TxnState_IDLE)
    {
        /* state was not changed, so free the transaction */
        CF_CFDP_ResetTransaction(t, 0);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_InitEngine
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_CFDP_InitEngine(void)
{
    /* initialize all transaction nodes */
    int                i, j;
    int                chunk_mem_offset = 0;
    CF_Transaction_t  *t                = CF_AppData.engine.transactions;
    CF_ChunkWrapper_t *c                = CF_AppData.engine.chunks;
    int32              ret              = CFE_SUCCESS;

    static const int CF_DIR_MAX_CHUNKS[CF_Direction_NUM][CF_NUM_CHANNELS] = {CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION,
                                                                             CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION};

    memset(&CF_AppData.engine, 0, sizeof(CF_AppData.engine));

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        char nbuf[64];
        snprintf(nbuf, sizeof(nbuf) - 1, "%s%d", CF_CHANNEL_PIPE_PREFIX, i);
        if ((ret = CFE_SB_CreatePipe(&CF_AppData.engine.channels[i].pipe,
                                     CF_AppData.config_table->chan[i].pipe_depth_input, nbuf)) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_PIPE, CFE_EVS_EventType_ERROR,
                              "CF: failed to create pipe %s, returned 0x%08lx", nbuf, (unsigned long)ret);
            goto err_out;
        }

        if ((ret = CFE_SB_SubscribeLocal(CFE_SB_ValueToMsgId(CF_AppData.config_table->chan[i].mid_input),
                                         CF_AppData.engine.channels[i].pipe,
                                         CF_AppData.config_table->chan[i].pipe_depth_input)) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_SUB, CFE_EVS_EventType_ERROR,
                              "CF: failed to subscribe to MID 0x%lx, returned 0x%08lx",
                              (unsigned long)CF_AppData.config_table->chan[i].mid_input, (unsigned long)ret);
            goto err_out;
        }

        if (CF_AppData.config_table->chan[i].sem_name[0])
        {
            ret = OS_CountSemGetIdByName(&CF_AppData.engine.channels[i].sem_id,
                                         CF_AppData.config_table->chan[i].sem_name);
            if (ret != OS_SUCCESS)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_INIT_SEM, CFE_EVS_EventType_ERROR,
                                  "CF: failed to get sem id for name %s, error=%ld",
                                  CF_AppData.config_table->chan[i].sem_name, (long)ret);
                goto err_out;
            }
        }

        for (j = 0; j < CF_NUM_TRANSACTIONS_PER_CHANNEL; ++j, ++t)
        {
            int k;

            t->chan_num = i;
            CF_FreeTransaction(t);

            for (k = 0; k < CF_Direction_NUM; ++k, ++c)
            {
                CF_Assert((chunk_mem_offset + CF_DIR_MAX_CHUNKS[k][i]) <= CF_NUM_CHUNKS_ALL_CHANNELS);
                CF_ChunkListInit(&c->chunks, CF_DIR_MAX_CHUNKS[k][i], &CF_AppData.engine.chunk_mem[chunk_mem_offset]);
                chunk_mem_offset += CF_DIR_MAX_CHUNKS[k][i];
                CF_CList_InitNode(&c->cl_node);
                CF_CList_InsertBack(&CF_AppData.engine.channels[i].cs[k], &c->cl_node);
            }
        }

        for (j = 0; j < CF_NUM_HISTORIES_PER_CHANNEL; ++j)
        {
            CF_History_t *h = &CF_AppData.engine.histories[(i * CF_NUM_HISTORIES_PER_CHANNEL) + j];
            CF_CList_InitNode(&h->cl_node);
            CF_CList_InsertBack_Ex(&CF_AppData.engine.channels[i], CF_QueueIdx_HIST_FREE, &h->cl_node);
        }
    }

    CF_AppData.engine.enabled = 1;

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_CycleTxFirstActive
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_CycleTxFirstActive(CF_CListNode_t *node, void *context)
{
    CF_CFDP_CycleTx_args_t *args = (CF_CFDP_CycleTx_args_t *)context;
    CF_Transaction_t       *t    = container_of(node, CF_Transaction_t, cl_node);
    int                     ret  = 1; /* default option is exit traversal */

    if (t->flags.com.suspended)
    {
        ret = 0; /* suspended, so move on to next */
        goto err_out;
    }

    CF_Assert(t->flags.com.q_index == CF_QueueIdx_TXA); /* huh? */

    /* if no more messages, then c->cur will be set.
     * If the transaction sent the last filedata pdu and eof, it will move itself
     * off the active queue. Run until either of these occur. */
    while (!args->c->cur && t->flags.com.q_index == CF_QueueIdx_TXA)
    {
        CFE_ES_PerfLogEntry(CF_PERF_ID_PDUSENT(t->chan_num));
        CF_CFDP_DispatchTx(t);
        CFE_ES_PerfLogExit(CF_PERF_ID_PDUSENT(t->chan_num));
    }

    args->ran_one = 1;

err_out:
    return ret; /* exit traversal */
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_CycleTx
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_CycleTx(CF_Channel_t *c)
{
    CF_Transaction_t      *t;
    CF_CFDP_CycleTx_args_t args;

    if (CF_AppData.config_table->chan[(c - CF_AppData.engine.channels)].dequeue_enabled)
    {
        args = (CF_CFDP_CycleTx_args_t) {c, 0};

        /* loop through as long as there are pending transactions, and a message buffer to send their pdus on */

        /* NOTE: tick processesing is higher priority than sending new filedata pdus, so only send however many
         * PDUs that can be sent once we get to here */
        if (!c->cur)
        { /* don't enter if cur is set, since we need to pick up where we left off on tick processing next wakeup */

            while (true)
            {
                /* Attempt to run something on TXA */
                CF_CList_Traverse(c->qs[CF_QueueIdx_TXA], CF_CFDP_CycleTxFirstActive, &args);

                /* Keep going until CF_QueueIdx_PEND is empty or something is run */
                if (args.ran_one || c->qs[CF_QueueIdx_PEND] == NULL)
                {
                    break;
                }

                t = container_of(c->qs[CF_QueueIdx_PEND], CF_Transaction_t, cl_node);
                CF_MoveTransaction(t, CF_QueueIdx_TXA);
            }
        }

        /* in case the loop exited due to no message buffers, clear it and start from the top next time */
        c->cur = NULL;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_DoTick
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_DoTick(CF_CListNode_t *node, void *context)
{
    int                  ret  = CF_CLIST_CONT; /* CF_CLIST_CONT means don't tick one, keep looking for cur */
    CF_CFDP_Tick_args_t *args = (CF_CFDP_Tick_args_t *)context;
    CF_Transaction_t    *t    = container_of(node, CF_Transaction_t, cl_node);
    if (!args->c->cur || (args->c->cur == t))
    {
        /* found where we left off, so clear that and move on */
        args->c->cur = NULL;
        if (!t->flags.com.suspended)
        {
            args->fn(t, &args->cont);
        }

        /* if args->c->cur was set to not-NULL above, then exit early */
        /* NOTE: if channel is frozen, then tick processing won't have been entered.
         *     so there is no need to check it here */
        if (args->c->cur)
        {
            ret              = CF_CLIST_EXIT;
            args->early_exit = 1;
        }
    }

    return ret; /* don't tick one, keep looking for cur */
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_TickTransactions
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_TickTransactions(CF_Channel_t *c)
{
    void (*fns[CF_TickType_NUM_TYPES])(CF_Transaction_t *, int *) = {CF_CFDP_R_Tick, CF_CFDP_S_Tick,
                                                                     CF_CFDP_S_Tick_Nak};
    int qs[CF_TickType_NUM_TYPES]                                 = {CF_QueueIdx_RX, CF_QueueIdx_TXW, CF_QueueIdx_TXW};

    CF_Assert(c->tick_type < CF_TickType_NUM_TYPES);

    for (; c->tick_type < CF_TickType_NUM_TYPES; ++c->tick_type)
    {
        CF_CFDP_Tick_args_t args = {c, fns[c->tick_type], 0, 0};

        do
        {
            args.cont = 0;
            CF_CList_Traverse(c->qs[qs[c->tick_type]], CF_CFDP_DoTick, &args);
            if (args.early_exit)
            {
                /* early exit means we ran out of availalbe outgoing messages this wakeup.
                 * If current tick type is nak response, then reset tick type. It would be
                 * bad to let NAK response starve out RX or TXW ticks on the next cycle.
                 *
                 * If RX ticks use up all availalee messages, then we pick up where whe left
                 * off on the next cycle. (This causes some RX tick counts to be missed,
                 * but that's ok. Precise timing isn't required.)
                 *
                 * This scheme allows the following priority for use of outgoing messages:
                 *
                 * RX state messages
                 * TXW state messages
                 * NAK response (could be many)
                 *
                 * New file data on TXA
                 */
                if (c->tick_type == CF_TickType_TXW_NAK)
                    break; /* triggers tick type reset below */
                else
                    goto early_exit;
            }
        } while (args.cont);
    }

    c->tick_type = CF_TickType_RX; /* reset tick type */

early_exit:;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_InitTxnTxFile
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_InitTxnTxFile(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority)
{
    t->chan_num = chan;
    t->priority = priority;
    t->keep     = keep;
    t->state    = cfdp_class ? CF_TxnState_S2 : CF_TxnState_S1;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_TxFile_Initiate
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_TxFile_Initiate(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan,
                                    uint8 priority, CF_EntityId_t dest_id)
{
    CFE_EVS_SendEvent(CF_EID_INF_CFDP_S_START_SEND, CFE_EVS_EventType_INFORMATION,
                      "CF: start class %d tx of file %lu:%.*s -> %lu:%.*s", cfdp_class + 1,
                      (unsigned long)CF_AppData.config_table->local_eid, CF_FILENAME_MAX_LEN,
                      t->history->fnames.src_filename, (unsigned long)dest_id, CF_FILENAME_MAX_LEN,
                      t->history->fnames.dst_filename);

    CF_CFDP_InitTxnTxFile(t, cfdp_class, keep, chan, priority);

    t->history->dir      = CF_Direction_TX;
    t->history->seq_num  = ++CF_AppData.engine.seq_num;
    t->history->src_eid  = CF_AppData.config_table->local_eid;
    t->history->peer_eid = dest_id;
    CF_CFDP_ArmInactTimer(t);

    /* NOTE: whether or not class 1 or 2, get a free chunks. it's cheap, and simplifies cleanup path */
    t->chunks = CF_CFDP_FindUnusedChunks(&CF_AppData.engine.channels[chan], CF_Direction_TX);
    CF_InsertSortPrio(t, CF_QueueIdx_PEND);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_TxFile
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_CFDP_TxFile(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                     uint8 chan, uint8 priority, CF_EntityId_t dest_id)
{
    CF_Transaction_t *t;
    CF_Channel_t     *c = &CF_AppData.engine.channels[chan];
    CF_Assert(chan < CF_NUM_CHANNELS);

    int32 ret = CFE_SUCCESS;

    if (c->num_cmd_tx == CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_MAX_CMD_TX, CFE_EVS_EventType_ERROR,
                          "CF: max number of commanded files reached");
        ret = -1;
        goto err_out;
    }

    t = CF_FindUnusedTransaction(&CF_AppData.engine.channels[chan]);
    CF_Assert(t); /* should always have a free transaction at this point */

    CF_Assert(t->state == CF_TxnState_IDLE);

    /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */
    strncpy(t->history->fnames.src_filename, src_filename, sizeof(t->history->fnames.src_filename) - 1);
    t->history->fnames.src_filename[sizeof(t->history->fnames.src_filename) - 1] = 0;
    strncpy(t->history->fnames.dst_filename, dst_filename, sizeof(t->history->fnames.dst_filename) - 1);
    t->history->fnames.dst_filename[sizeof(t->history->fnames.dst_filename) - 1] = 0;
    CF_CFDP_TxFile_Initiate(t, cfdp_class, keep, chan, priority, dest_id);

    ++c->num_cmd_tx;
    t->flags.tx.cmd_tx = 1;

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_PlaybackDir_Initiate
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static int32 CF_CFDP_PlaybackDir_Initiate(CF_Playback_t *p, const char *src_filename, const char *dst_filename,
                                          CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority,
                                          CF_EntityId_t dest_id)
{
    int32 ret = CFE_SUCCESS;

    /* make sure the directory can be open */
    ret = OS_DirectoryOpen(&p->dir_id, src_filename);
    if (ret != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_OPENDIR, CFE_EVS_EventType_ERROR,
                          "CF: failed to open playback directory %s, error=%ld", src_filename, (long)ret);
        ++CF_AppData.hk.channel_hk[chan].counters.fault.directory_read;
        goto err_out;
    }

    p->diropen    = 1;
    p->busy       = 1;
    p->keep       = keep;
    p->priority   = priority;
    p->dest_id    = dest_id;
    p->cfdp_class = cfdp_class;

    /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */
    strncpy(p->fnames.src_filename, src_filename, sizeof(p->fnames.src_filename) - 1);
    p->fnames.src_filename[sizeof(p->fnames.src_filename) - 1] = 0;
    strncpy(p->fnames.dst_filename, dst_filename, sizeof(p->fnames.dst_filename) - 1);
    p->fnames.dst_filename[sizeof(p->fnames.dst_filename) - 1] = 0;

    /* the executor will start the transfer next cycle */

err_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_PlaybackDir
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_CFDP_PlaybackDir(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                          uint8 chan, uint8 priority, uint16 dest_id)
{
    int            i;
    CF_Playback_t *p;

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        p = &CF_AppData.engine.channels[chan].playback[i];
        if (!p->busy)
        {
            break;
        }
    }

    if (i == CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_DIR_SLOT, CFE_EVS_EventType_ERROR, "CF: no playback dir slot available");
        return -1;
    }

    return CF_CFDP_PlaybackDir_Initiate(p, src_filename, dst_filename, cfdp_class, keep, chan, priority, dest_id);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ProcessPlaybackDirectory
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *c, CF_Playback_t *p)
{
    os_dirent_t dirent;
    /* either there's no transaction (first one) or the last one was finished, so check for a new one */

    while (p->diropen && (p->num_ts < CF_NUM_TRANSACTIONS_PER_PLAYBACK))
    {
        int32 status;

        CFE_ES_PerfLogEntry(CF_PERF_ID_DIRREAD);
        status = OS_DirectoryRead(p->dir_id, &dirent);
        CFE_ES_PerfLogExit(CF_PERF_ID_DIRREAD);

        if (status == CFE_SUCCESS)
        {
            if (!strcmp(dirent.FileName, ".") || !strcmp(dirent.FileName, ".."))
            {
                continue;
            }

            {
                CF_Transaction_t *pt = CF_FindUnusedTransaction(c);
                CF_Assert(pt); /* should be impossible not to have one because there are limits on the number of uses of
                                  them */

                /* the -1 below is to make room for the slash */
                snprintf(pt->history->fnames.src_filename, sizeof(pt->history->fnames.src_filename), "%.*s/%.*s",
                         CF_FILENAME_MAX_PATH - 1, p->fnames.src_filename, CF_FILENAME_MAX_NAME - 1, dirent.FileName);
                snprintf(pt->history->fnames.dst_filename, sizeof(pt->history->fnames.dst_filename), "%.*s/%.*s",
                         CF_FILENAME_MAX_PATH - 1, p->fnames.dst_filename, CF_FILENAME_MAX_NAME - 1, dirent.FileName);

                /* in case snprintf didn't have room for NULL terminator */
                pt->history->fnames.src_filename[CF_FILENAME_MAX_LEN - 1] = 0;
                pt->history->fnames.dst_filename[CF_FILENAME_MAX_LEN - 1] = 0;

                CF_CFDP_TxFile_Initiate(pt, p->cfdp_class, p->keep, (c - CF_AppData.engine.channels), p->priority,
                                        p->dest_id);

                pt->p = p;
                ++p->num_ts;
            }
        }
        else
        {
            /* PFTO: can we figure out the difference between "end of dir" and an error? */
            OS_DirectoryClose(p->dir_id);
            p->diropen = 0;
        }
    }

    if (!p->diropen && !p->num_ts)
    {
        /* the directory has been exhausted, and there are no more active transactions
         * for this playback -- so mark it as not busy */
        p->busy = 0;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_UpdatePollPbCounted
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_UpdatePollPbCounted(CF_Playback_t *pb, int up, uint8 *counter)
{
    if (pb->counted != up)
    {
        /* only handle on state change */
        pb->counted = !!up; /* !! ensure 0 or 1, should be optimized out */

        if (up)
        {
            ++*counter;
        }
        else
        {
            CF_Assert(*counter); /* sanity check it isn't zero */
            --*counter;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ProcessPlaybackDirectories
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
static void CF_CFDP_ProcessPlaybackDirectories(CF_Channel_t *c)
{
    int       i;
    const int chan_index = (c - CF_AppData.engine.channels);

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        CF_CFDP_ProcessPlaybackDirectory(c, &c->playback[i]);
        CF_CFDP_UpdatePollPbCounted(&c->playback[i], c->playback[i].busy,
                                    &CF_AppData.hk.channel_hk[chan_index].playback_counter);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ProcessPollingDirectories
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *c)
{
    int i;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        CF_Poll_t          *p           = &c->poll[i];
        int                 chan_index  = (c - CF_AppData.engine.channels);
        CF_ChannelConfig_t *cc          = &CF_AppData.config_table->chan[chan_index];
        CF_PollDir_t       *pd          = &cc->polldir[i];
        int                 count_check = 0;

        if (pd->enabled && pd->interval_sec)
        {
            /* only handle polling for polldirs configured with a non-zero interval */
            if (!p->pb.busy && !p->pb.num_ts)
            {
                if (!p->timer_set)
                {
                    /* timer was not set, so set it now */
                    CF_Timer_InitRelSec(&p->interval_timer, pd->interval_sec);
                    p->timer_set = 1;
                }
                else if (CF_Timer_Expired(&p->interval_timer))
                {
                    /* the timer has expired */
                    int ret = CF_CFDP_PlaybackDir_Initiate(&p->pb, pd->src_dir, pd->dst_dir, pd->cfdp_class, 0,
                                                           chan_index, pd->priority, pd->dest_eid);
                    if (!ret)
                    {
                        p->timer_set = 0;
                    }
                    else
                    {
                        /* error occured in playback directory, so reset the timer */
                        /* an event is sent in CF_CFDP_PlaybackDir_Initiate so there is no reason to
                         * to have another here */
                        CF_Timer_InitRelSec(&p->interval_timer, pd->interval_sec);
                    }
                }
                else
                    CF_Timer_Tick(&p->interval_timer);
            }
            else
            {
                /* playback is active, so step it */
                CF_CFDP_ProcessPlaybackDirectory(c, &p->pb);
            }

            count_check = 1;
        }

        CF_CFDP_UpdatePollPbCounted(&p->pb, count_check, &CF_AppData.hk.channel_hk[chan_index].poll_counter);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_CycleEngine
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_CycleEngine(void)
{
    int i;

    if (CF_AppData.engine.enabled)
    {
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
        {
            CF_Channel_t *c                    = &CF_AppData.engine.channels[i];
            CF_AppData.engine.outgoing_counter = 0;

            /* consume all received messages, even if channel is frozen */
            CF_CFDP_ReceiveMessage(c);

            if (!CF_AppData.hk.channel_hk[i].frozen)
            {
                /* handle ticks before tx cycle. Do this because there may be a limited number of TX messages available
                 * this cycle, and it's important to respond to class 2 ACK/NAK more than it is to send new filedata
                 * PDUs. */

                /* cycle all transactions (tick) */
                CF_CFDP_TickTransactions(c);

                /* cycle the current tx transaction */
                CF_CFDP_CycleTx(c);

                CF_CFDP_ProcessPlaybackDirectories(c);
                CF_CFDP_ProcessPollingDirectories(c);
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ResetTransaction
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ResetTransaction(CF_Transaction_t *t, int keep_history)
{
    CF_Channel_t *c = &CF_AppData.engine.channels[t->chan_num];
    CF_Assert(t->chan_num < CF_NUM_CHANNELS);

    CF_DequeueTransaction(t);

    if (OS_ObjectIdDefined(t->fd))
    {
        CF_WrappedClose(t->fd);
        if (!t->keep)
        {
            if (CF_CFDP_IsSender(t))
            {
                OS_remove(t->history->fnames.src_filename);
            }
            else
            {
                OS_remove(t->history->fnames.dst_filename);
            }
        }
    }

    /* extra bookkeeping for tx direction only */
    if (t->history->dir == CF_Direction_TX)
    {
        if (t->flags.tx.cmd_tx)
        {
            CF_Assert(c->num_cmd_tx); /* sanity check */
            --c->num_cmd_tx;
        }

        if (t->p)
        {
            /* a playback's transaction is now done. decrement the playback counter */
            CF_Assert(t->p->num_ts);
            --t->p->num_ts;
        }
    }

    /* bookkeeping for all transactions */
    /* move transaction history to history queue */
    if (keep_history)
    {
        CF_CList_InsertBack_Ex(c, CF_QueueIdx_HIST, &t->history->cl_node);
    }
    else
    {
        CF_CList_InsertBack_Ex(c, CF_QueueIdx_HIST_FREE, &t->history->cl_node);
    }

    CF_CList_InsertBack(&c->cs[!!CF_CFDP_IsSender(t)], &t->chunks->cl_node);

    if (c->cur == t)
    {
        c->cur = NULL; /* this transaction couldn't get a message previously, so clear it here to avoid problems */
    }
    CF_FreeTransaction(t);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_CopyStringFromLV
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_CopyStringFromLV(char *buf, size_t buf_maxsz, const CF_Logical_Lv_t *src_lv)
{
    if (src_lv->length < buf_maxsz)
    {
        memcpy(buf, src_lv->data_ptr, src_lv->length);
        buf[src_lv->length] = 0;
        return src_lv->length;
    }

    /* ensure output is empty */
    buf[0] = 0;
    return -1; /* invalid len in lv? */
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_CancelTransaction
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_CancelTransaction(CF_Transaction_t *t)
{
    void (*fns[2])(CF_Transaction_t * t) = {CF_CFDP_R_Cancel, CF_CFDP_S_Cancel};
    if (!t->flags.com.canceled)
    {
        t->flags.com.canceled = 1;
        t->history->cc        = CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED;
        fns[!!CF_CFDP_IsSender(t)](t);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_CloseFiles
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_CFDP_CloseFiles(CF_CListNode_t *n, void *context)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);
    if (OS_ObjectIdDefined(t->fd))
    {
        CF_WrappedClose(t->fd);
    }
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_DisableEngine
 *
 * Application-scope internal function
 * See description in cf_cfdp.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_DisableEngine(void)
{
    int                        i, j;
    static const CF_QueueIdx_t CLOSE_QUEUES[] = {CF_QueueIdx_RX, CF_QueueIdx_TXA, CF_QueueIdx_TXW};
    CF_Channel_t              *c;

    CF_AppData.engine.enabled = 0;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        c = &CF_AppData.engine.channels[i];

        /* first, close all active files */
        for (j = 0; j < (sizeof(CLOSE_QUEUES) / sizeof(CLOSE_QUEUES[0])); ++j)
        {
            CF_CList_Traverse(c->qs[CLOSE_QUEUES[j]], CF_CFDP_CloseFiles, NULL);
        }

        /* any playback directories need to have their dirents closed */
        for (j = 0; j < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++j)
        {
            if (c->playback[j].busy)
            {
                OS_DirectoryClose(c->playback[j].dir_id);
            }
        }

        for (j = 0; j < CF_MAX_POLLING_DIR_PER_CHAN; ++j)
        {
            if (c->poll[j].pb.busy)
            {
                OS_DirectoryClose(c->poll[j].pb.dir_id);
            }
        }

        /* finally all queue counters must be reset */
        memset(&CF_AppData.hk.channel_hk[i].q_size, 0, sizeof(CF_AppData.hk.channel_hk[i].q_size));

        CFE_SB_DeletePipe(c->pipe);
    }
}
