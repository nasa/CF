/************************************************************************
** File: cf_codec.c
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
*************************************************************************/

#define CF_DO_DECLARE_FIELDS

#include "cf_cfdp_pdu.h"
#include "cf_codec.h"
#include "cf_events.h"

/* NOTE: get/set will handle endianess */
/*
 * ALSO NOTE: These store/set inline functions/macros are used with
 * literal integers as well as variables.  So they operate on value, where
 * the load/get functions operate by reference
 */
static inline void CF_Codec_Store_uint8(CF_CFDP_uint8_t *pdst, uint8 val)
{
    pdst->octets[0] = val;
}
#define cfdp_set_uint8(dst, src) CF_Codec_Store_uint8(&(dst), src)

static inline void CF_Codec_Store_uint16(CF_CFDP_uint16_t *pdst, uint16 val)
{
    pdst->octets[1] = val & 0xFF;
    val >>= 8;
    pdst->octets[0] = val & 0xFF;
}
#define cfdp_set_uint16(dst, src) CF_Codec_Store_uint16(&(dst), src)

static inline void CF_Codec_Store_uint32(CF_CFDP_uint32_t *pdst, uint32 val)
{
    pdst->octets[3] = val & 0xFF;
    val >>= 8;
    pdst->octets[2] = val & 0xFF;
    val >>= 8;
    pdst->octets[1] = val & 0xFF;
    val >>= 8;
    pdst->octets[0] = val & 0xFF;
}
#define cfdp_set_uint32(dst, src) CF_Codec_Store_uint32(&(dst), src)

static inline void CF_Codec_Store_uint64(CF_CFDP_uint64_t *pdst, uint64 val)
{
    pdst->octets[7] = val & 0xFF;
    val >>= 8;
    pdst->octets[6] = val & 0xFF;
    val >>= 8;
    pdst->octets[5] = val & 0xFF;
    val >>= 8;
    pdst->octets[4] = val & 0xFF;
    val >>= 8;
    pdst->octets[3] = val & 0xFF;
    val >>= 8;
    pdst->octets[2] = val & 0xFF;
    val >>= 8;
    pdst->octets[1] = val & 0xFF;
    val >>= 8;
    pdst->octets[0] = val & 0xFF;
}
#define cfdp_set_uint64(dst, src) CF_Codec_Store_uint64(&(dst), src)

static inline void CF_Codec_Load_uint8(uint8 *pdst, const CF_CFDP_uint8_t *psrc)
{
    *pdst = psrc->octets[0];
}
#define cfdp_get_uint8(dst, src) CF_Codec_Load_uint8(&(dst), &(src))

static inline void CF_Codec_Load_uint16(uint16 *pdst, const CF_CFDP_uint16_t *psrc)
{
    uint16 val = 0;

    val |= psrc->octets[0];
    val <<= 8;
    val |= psrc->octets[1];

    *pdst = val;
}
#define cfdp_get_uint16(dst, src) CF_Codec_Load_uint16(&(dst), &(src))

static inline void CF_Codec_Load_uint32(uint32 *pdst, const CF_CFDP_uint32_t *psrc)
{
    uint32 val = 0;

    val |= psrc->octets[0];
    val <<= 8;
    val |= psrc->octets[1];
    val <<= 8;
    val |= psrc->octets[2];
    val <<= 8;
    val |= psrc->octets[3];

    *pdst = val;
}
#define cfdp_get_uint32(dst, src) CF_Codec_Load_uint32(&(dst), &(src))

static inline void CF_Codec_Load_uint64(uint64 *pdst, const CF_CFDP_uint64_t *psrc)
{
    uint64 val = 0;

    val |= psrc->octets[0];
    val <<= 8;
    val |= psrc->octets[1];
    val <<= 8;
    val |= psrc->octets[2];
    val <<= 8;
    val |= psrc->octets[3];
    val <<= 8;
    val |= psrc->octets[4];
    val <<= 8;
    val |= psrc->octets[5];
    val <<= 8;
    val |= psrc->octets[6];
    val <<= 8;
    val |= psrc->octets[7];

    *pdst = val;
}
#define cfdp_get_uint64(dst, src) CF_Codec_Load_uint64(&(dst), &(src))

bool CF_CFDP_CodecCheckSize(CF_CodecState_t *state, size_t chunksize)
{
    size_t next_offset = state->next_offset + chunksize;

    if (next_offset > state->max_size)
    {
        CF_CFDP_CodecSetDone(state);
    }
    else
    {
        state->next_offset = next_offset;
    }

    return CF_CFDP_CodecIsOK(state);
}

void *CF_CFDP_DoEncodeChunk(CF_EncoderState_t *state, size_t chunksize)
{
    uint8 *buf = state->base + CF_CFDP_CodecGetPosition(&state->codec_state);

    if (!CF_CFDP_CodecCheckSize(&state->codec_state, chunksize))
    {
        buf = NULL;
    }

    return buf;
}

const void *CF_CFDP_DoDecodeChunk(CF_DecoderState_t *state, size_t chunksize)
{
    const uint8 *buf = state->base + CF_CFDP_CodecGetPosition(&state->codec_state);

    if (!CF_CFDP_CodecCheckSize(&state->codec_state, chunksize))
    {
        buf = NULL;
    }

    return buf;
}

uint8 CF_CFDP_GetValueEncodedSize(uint64 Value)
{
    uint8  MinSize;
    uint64 Limit = 0x100;

    Limit = 0x100;
    for (MinSize = 1; MinSize < 8 && Value >= Limit; ++MinSize)
    {
        Limit <<= 8;
    }

    return MinSize;
}

void CF_EncodeIntegerInSize(CF_EncoderState_t *state, uint64 value, uint8 encode_size)
{
    uint8 *dptr;

    dptr = CF_CFDP_DoEncodeChunk(state, encode_size);
    if (dptr != NULL)
    {
        /* this writes from LSB to MSB, in reverse (so the result will be in network order) */
        dptr += encode_size;
        while (encode_size > 0)
        {
            --encode_size;
            --dptr;
            *dptr = value & 0xFF;
            value >>= 8;
        }
    }
}

/*
 * On transmit side, the common/base header must be encoded in two parts, to deal
 * with the "total_size" field.  The initial encoding of the the basic fields is
 * done as soon as it is known that a PDU of this type needs to be sent, but the
 * total size may not be yet known, as it depends on the remainder of encoding
 * and any additional data that might get added to the variable length sections.
 *
 * This function encodes all base header fields _except_ total length.  There is a
 * separate function later to update the total_length to the correct value once the
 * remainder of encoding is done.  Luckily, the total_length is in the first fixed
 * position binary blob so it is easy to update later.
 */
void CF_CFDP_EncodeHeaderWithoutSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    CF_CFDP_PduHeader_t *peh; /* for encoding fixed sized fields */

    peh = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduHeader_t);
    if (peh != NULL)
    {
        cfdp_set_uint8(peh->flags, 0);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_VERSION, plh->version);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_DIR, plh->direction);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_TYPE, plh->pdu_type);
        FSV(peh->flags, CF_CFDP_PduHeader_FLAGS_MODE, plh->txm_mode);

        /* The eid+tsn lengths are encoded as -1 */
        cfdp_set_uint8(peh->eid_tsn_lengths, 0);
        FSV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_ENTITY, plh->eid_length - 1);
        FSV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_TRANSACTION_SEQUENCE, plh->txn_seq_length - 1);

        /* NOTE: peh->length is NOT set here, as it depends on future encoding */

        /* Now copy variable-length fields */
        CF_EncodeIntegerInSize(state, plh->source_eid, plh->eid_length);
        CF_EncodeIntegerInSize(state, plh->sequence_num, plh->txn_seq_length);
        CF_EncodeIntegerInSize(state, plh->destination_eid, plh->eid_length);

        /* The position now reflects the length of the basic header */
        plh->header_encoded_length = CF_CODEC_GET_POSITION(state);
    }
}

void CF_CFDP_EncodeHeaderFinalSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    CF_CFDP_PduHeader_t *peh;

    /*
     * This is different as it is updating a block that was already encoded,
     * so it cannot use CF_ENCODE_FIXED_CHUNK because this adds an entity to the tail.
     *
     * The PDU header that needs update is the very first entity in the packet, and
     * this should never be NULL.
     */
    if (CF_CODEC_IS_OK(state) && CF_CODEC_GET_POSITION(state) >= sizeof(CF_CFDP_PduHeader_t))
    {
        peh = (CF_CFDP_PduHeader_t *)state->base;

        /* Total length is a simple 16-bit quantity */
        cfdp_set_uint16(peh->length, plh->data_encoded_length);
    }

    /* This "closes" the packet so nothing else can be added to this EncoderState,
     * it is not indicative of an error */
    CF_CODEC_SET_DONE(state);
}

void CF_CFDP_EncodeFileDirectiveHeader(CF_EncoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir)
{
    CF_CFDP_PduFileDirectiveHeader_t *peh; /* for encoding fixed sized fields */
    uint8                             value = pfdir->directive_code;

    peh = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduFileDirectiveHeader_t);
    if (peh != NULL)
    {
        cfdp_set_uint8(peh->directive_code, value);
    }
}

void CF_CFDP_EncodeLV(CF_EncoderState_t *state, CF_Logical_Lv_t *pllv)
{
    CF_CFDP_lv_t *lv; /* for encoding fixed sized fields */
    void         *data_ptr;

    lv = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_lv_t);
    if (lv != NULL)
    {
        cfdp_set_uint8(lv->length, pllv->length);
        if (pllv->length > 0)
        {
            data_ptr = CF_CFDP_DoEncodeChunk(state, pllv->length);
            if (data_ptr != NULL && pllv->data_ptr != NULL)
            {
                memcpy(data_ptr, pllv->data_ptr, pllv->length);
            }
            else
            {
                CF_CODEC_SET_DONE(state);
            }
        }
    }
}

void CF_CFDP_EncodeTLV(CF_EncoderState_t *state, CF_Logical_Tlv_t *pltlv)
{
    CF_CFDP_tlv_t *tlv; /* for encoding fixed sized fields */
    void          *data_ptr;

    tlv = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_tlv_t);
    if (tlv != NULL)
    {
        cfdp_set_uint8(tlv->type, pltlv->type);
        cfdp_set_uint8(tlv->length, pltlv->length);

        /* the only TLV type currently implemented is entity id */
        if (pltlv->type == CF_CFDP_TLV_TYPE_ENTITY_ID)
        {
            CF_EncodeIntegerInSize(state, pltlv->data.eid, pltlv->length);
        }
        else if (pltlv->length > 0)
        {
            /* Copy the other data in (feature not used in CF yet, but should be handled) */
            data_ptr = CF_CFDP_DoEncodeChunk(state, pltlv->length);
            if (data_ptr != NULL && pltlv->data.data_ptr != NULL)
            {
                memcpy(data_ptr, pltlv->data.data_ptr, pltlv->length);
            }
            else
            {
                CF_CODEC_SET_DONE(state);
            }
        }
    }
}

void CF_CFDP_EncodeSegmentRequest(CF_EncoderState_t *state, CF_Logical_SegmentRequest_t *plseg)
{
    CF_CFDP_SegmentRequest_t *sr; /* for encoding fixed sized fields */

    sr = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_SegmentRequest_t);
    if (sr != NULL)
    {
        cfdp_set_uint32(sr->offset_start, plseg->offset_start);
        cfdp_set_uint32(sr->offset_end, plseg->offset_end);
    }
}

void CF_CFDP_EncodeAllTlv(CF_EncoderState_t *state, CF_Logical_TlvList_t *pltlv)
{
    uint8 i;

    for (i = 0; CF_CODEC_IS_OK(state) && i < pltlv->num_tlv; ++i)
    {
        CF_CFDP_EncodeTLV(state, &pltlv->tlv[i]);
    }
}

void CF_CFDP_EncodeAllSegments(CF_EncoderState_t *state, CF_Logical_SegmentList_t *plseg)
{
    uint8 i;

    for (i = 0; CF_CODEC_IS_OK(state) && i < plseg->num_segments; ++i)
    {
        CF_CFDP_EncodeSegmentRequest(state, &plseg->segments[i]);
    }
}

void CF_CFDP_EncodeMd(CF_EncoderState_t *state, CF_Logical_PduMd_t *plmd)
{
    CF_CFDP_PduMd_t *md; /* for encoding fixed sized fields */

    md = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduMd_t);
    if (md != NULL)
    {
        cfdp_set_uint8(md->segmentation_control, 0);
        FSV(md->segmentation_control, CF_CFDP_PduMd_CLOSURE_REQUESTED, plmd->close_req);
        FSV(md->segmentation_control, CF_CFDP_PduMd_CHECKSUM_TYPE, plmd->checksum_type);
        cfdp_set_uint32(md->size, plmd->size);

        /* Add in LV for src/dest */
        CF_CFDP_EncodeLV(state, &plmd->source_filename);
        CF_CFDP_EncodeLV(state, &plmd->dest_filename);
    }
}

void CF_CFDP_EncodeFileDataHeader(CF_EncoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd)
{
    CF_CFDP_PduFileDataHeader_t *fd;
    CF_CFDP_uint8_t             *optional_fields;

    /* in this packet, the optional fields actually come first */
    if (with_meta)
    {
        optional_fields = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_uint8_t);
    }
    else
    {
        optional_fields = NULL;
    }

    if (optional_fields != NULL)
    {
        cfdp_set_uint8(*optional_fields, 0);
        FSV(*optional_fields, CF_CFDP_PduFileData_RECORD_CONTINUATION_STATE, plfd->continuation_state);
        FSV(*optional_fields, CF_CFDP_PduFileData_SEGMENT_METADATA_LENGTH, plfd->segment_list.num_segments);

        CF_CFDP_EncodeAllSegments(state, &plfd->segment_list);
    }

    fd = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduFileDataHeader_t);
    if (fd != NULL)
    {
        cfdp_set_uint32(fd->offset, plfd->offset);
    }
}

void CF_CFDP_EncodeEof(CF_EncoderState_t *state, CF_Logical_PduEof_t *pleof)
{
    CF_CFDP_PduEof_t *eof; /* for encoding fixed sized fields */

    eof = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduEof_t);
    if (eof != NULL)
    {
        cfdp_set_uint8(eof->cc, 0);
        FSV(eof->cc, CF_CFDP_PduEof_FLAGS_CC, pleof->cc);
        cfdp_set_uint32(eof->crc, pleof->crc);
        cfdp_set_uint32(eof->size, pleof->size);

        CF_CFDP_EncodeAllTlv(state, &pleof->tlv_list);
    }
}

void CF_CFDP_EncodeFin(CF_EncoderState_t *state, CF_Logical_PduFin_t *plfin)
{
    CF_CFDP_PduFin_t *fin; /* for encoding fixed sized fields */

    fin = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduFin_t);
    if (fin != NULL)
    {
        cfdp_set_uint8(fin->flags, 0);
        FSV(fin->flags, CF_CFDP_PduFin_FLAGS_CC, plfin->cc);
        FSV(fin->flags, CF_CFDP_PduFin_FLAGS_DELIVERY_CODE, plfin->delivery_code);
        FSV(fin->flags, CF_CFDP_PduFin_FLAGS_FILE_STATUS, plfin->file_status);

        CF_CFDP_EncodeAllTlv(state, &plfin->tlv_list);
    }
}

void CF_CFDP_EncodeAck(CF_EncoderState_t *state, CF_Logical_PduAck_t *plack)
{
    CF_CFDP_PduAck_t *ack; /* for encoding fixed sized fields */

    ack = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduAck_t);
    if (ack != NULL)
    {
        cfdp_set_uint8(ack->directive_and_subtype_code, 0);
        FSV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_CODE, plack->ack_directive_code);
        FSV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_SUBTYPE_CODE, plack->ack_subtype_code);

        cfdp_set_uint8(ack->cc_and_transaction_status, 0);
        FSV(ack->cc_and_transaction_status, CF_CFDP_PduAck_CC, plack->cc);
        FSV(ack->cc_and_transaction_status, CF_CFDP_PduAck_TRANSACTION_STATUS, plack->txn_status);
    }
}

void CF_CFDP_EncodeNak(CF_EncoderState_t *state, CF_Logical_PduNak_t *plnak)
{
    CF_CFDP_PduNak_t *nak; /* for encoding fixed sized fields */

    nak = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_PduNak_t);
    if (nak != NULL)
    {
        cfdp_set_uint32(nak->scope_start, plnak->scope_start);
        cfdp_set_uint32(nak->scope_end, plnak->scope_end);

        CF_CFDP_EncodeAllSegments(state, &plnak->segment_list);
    }
}

void CF_CFDP_EncodeCrc(CF_EncoderState_t *state, uint32 *plcrc)
{
    CF_CFDP_uint32_t *pecrc; /* CFDP CRC values are 32-bit only, per blue book */

    pecrc = CF_ENCODE_FIXED_CHUNK(state, CF_CFDP_uint32_t);
    if (pecrc != NULL)
    {
        cfdp_set_uint32(*pecrc, *plcrc);
    }
}

uint64 CF_DecodeIntegerInSize(CF_DecoderState_t *state, uint8 decode_size)
{
    const uint8 *sptr;
    uint64       temp_val;

    temp_val = 0;
    sptr     = CF_CFDP_DoDecodeChunk(state, decode_size);
    if (sptr != NULL)
    {
        /* this reads from MSB to LSB, so the result will be in native order */
        while (decode_size > 0)
        {
            temp_val <<= 8;
            temp_val |= *sptr & 0xFF;
            ++sptr;
            --decode_size;
        }
    }

    return temp_val;
}

void CF_CFDP_DecodeHeader(CF_DecoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    const CF_CFDP_PduHeader_t *peh; /* for decoding fixed sized fields */

    /* decode the standard PDU header */
    peh = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduHeader_t);
    if (peh != NULL)
    {
        plh->version   = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_VERSION);
        plh->direction = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_DIR);
        plh->pdu_type  = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_TYPE);
        plh->txm_mode  = FGV(peh->flags, CF_CFDP_PduHeader_FLAGS_MODE);

        /* The eid+tsn lengths are encoded as -1 */
        plh->eid_length     = FGV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_ENTITY) + 1;
        plh->txn_seq_length = FGV(peh->eid_tsn_lengths, CF_CFDP_PduHeader_LENGTHS_TRANSACTION_SEQUENCE) + 1;

        /* Length is a simple 16-bit quantity and refers to the content after this header */
        cfdp_get_uint16(plh->data_encoded_length, peh->length);

        /* Now copy variable-length fields */
        plh->source_eid      = CF_DecodeIntegerInSize(state, plh->eid_length);
        plh->sequence_num    = CF_DecodeIntegerInSize(state, plh->txn_seq_length);
        plh->destination_eid = CF_DecodeIntegerInSize(state, plh->eid_length);

        /* The header length is where decoding ended at this point */
        plh->header_encoded_length = CF_CODEC_GET_POSITION(state);
    }
}

void CF_CFDP_DecodeFileDirectiveHeader(CF_DecoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir)
{
    const CF_CFDP_PduFileDirectiveHeader_t *peh;
    uint8                                   packet_val;

    /* decode the standard PDU header */
    peh = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduFileDirectiveHeader_t);
    if (peh != NULL)
    {
        cfdp_get_uint8(packet_val, peh->directive_code);
        pfdir->directive_code = packet_val;
    }
}

void CF_CFDP_DecodeLV(CF_DecoderState_t *state, CF_Logical_Lv_t *pllv)
{
    const CF_CFDP_lv_t *lv;

    lv = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_lv_t);
    if (lv != NULL)
    {
        cfdp_get_uint8(pllv->length, lv->length);
        pllv->data_ptr = CF_CFDP_DoDecodeChunk(state, pllv->length);
    }
}

void CF_CFDP_DecodeTLV(CF_DecoderState_t *state, CF_Logical_Tlv_t *pltlv)
{
    const CF_CFDP_tlv_t *tlv;
    uint8                type_val;

    tlv = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_tlv_t);
    if (tlv != NULL)
    {
        cfdp_get_uint8(type_val, tlv->type);
        cfdp_get_uint8(pltlv->length, tlv->length);

        /* the only TLV type currently implemented is entity id */
        pltlv->type = type_val;
        if (pltlv->type == CF_CFDP_TLV_TYPE_ENTITY_ID)
        {
            pltlv->data.eid = CF_DecodeIntegerInSize(state, pltlv->length);
        }
        else
        {
            /* not implemented, but must not send random data */
            pltlv->data.data_ptr = CF_CFDP_DoDecodeChunk(state, pltlv->length);
            ;
        }
    }
}

void CF_CFDP_DecodeSegmentRequest(CF_DecoderState_t *state, CF_Logical_SegmentRequest_t *plseg)
{
    const CF_CFDP_SegmentRequest_t *sr; /* for decoding fixed sized fields */

    sr = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_SegmentRequest_t);
    if (sr != NULL)
    {
        cfdp_get_uint32(plseg->offset_start, sr->offset_start);
        cfdp_get_uint32(plseg->offset_end, sr->offset_end);
    }
}

void CF_CFDP_DecodeMd(CF_DecoderState_t *state, CF_Logical_PduMd_t *plmd)
{
    const CF_CFDP_PduMd_t *md; /* for decoding fixed sized fields */

    md = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduMd_t);
    if (md != NULL)
    {
        plmd->close_req     = FGV(md->segmentation_control, CF_CFDP_PduMd_CLOSURE_REQUESTED);
        plmd->checksum_type = FGV(md->segmentation_control, CF_CFDP_PduMd_CHECKSUM_TYPE);
        cfdp_get_uint32(plmd->size, md->size);

        /* Add in LV for src/dest */
        CF_CFDP_DecodeLV(state, &plmd->source_filename);
        CF_CFDP_DecodeLV(state, &plmd->dest_filename);
    }
}

void CF_CFDP_DecodeFileDataHeader(CF_DecoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd)
{
    const CF_CFDP_PduFileDataHeader_t *fd;
    const CF_CFDP_uint8_t             *optional_fields;
    uint8                              field_count;

    plfd->continuation_state        = 0;
    plfd->segment_list.num_segments = 0;

    /* in this packet, the optional fields actually come first */
    if (with_meta)
    {
        optional_fields = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_uint8_t);
    }
    else
    {
        optional_fields = NULL;
    }

    if (optional_fields != NULL)
    {
        plfd->continuation_state = FGV(*optional_fields, CF_CFDP_PduFileData_RECORD_CONTINUATION_STATE);
        field_count              = FGV(*optional_fields, CF_CFDP_PduFileData_SEGMENT_METADATA_LENGTH);
        if (field_count > CF_PDU_MAX_SEGMENTS)
        {
            /* do not overfill */
            CF_CODEC_SET_DONE(state);
            field_count = 0;
        }

        while (field_count > 0)
        {
            --field_count;

            /* append decoded segment info */
            CF_CFDP_DecodeSegmentRequest(state, &plfd->segment_list.segments[plfd->segment_list.num_segments]);
            if (!CF_CODEC_IS_OK(state))
            {
                break;
            }

            /* only increment if successful */
            ++plfd->segment_list.num_segments;
        }
    }

    fd = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduFileDataHeader_t);
    if (fd != NULL)
    {
        cfdp_get_uint32(plfd->offset, fd->offset);

        plfd->data_len = CF_CODEC_GET_REMAIN(state);
        plfd->data_ptr = CF_CFDP_DoDecodeChunk(state, plfd->data_len);
    }
}

void CF_CFDP_DecodeCrc(CF_DecoderState_t *state, uint32 *plcrc)
{
    const CF_CFDP_uint32_t *pecrc; /* CFDP CRC values are 32-bit only, per blue book */

    pecrc = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_uint32_t);
    if (pecrc != NULL)
    {
        cfdp_get_uint32(*plcrc, *pecrc);
    }
}

void CF_CFDP_DecodeEof(CF_DecoderState_t *state, CF_Logical_PduEof_t *pleof)
{
    const CF_CFDP_PduEof_t *eof; /* for decoding fixed sized fields */

    eof = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduEof_t);
    if (eof != NULL)
    {
        pleof->cc = FGV(eof->cc, CF_CFDP_PduEof_FLAGS_CC);
        cfdp_get_uint32(pleof->crc, eof->crc);
        cfdp_get_uint32(pleof->size, eof->size);

        CF_CFDP_DecodeAllTlv(state, &pleof->tlv_list, CF_PDU_MAX_TLV);
    }
}

void CF_CFDP_DecodeFin(CF_DecoderState_t *state, CF_Logical_PduFin_t *plfin)
{
    const CF_CFDP_PduFin_t *fin; /* for decoding fixed sized fields */

    fin = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduFin_t);
    if (fin != NULL)
    {
        plfin->cc            = FGV(fin->flags, CF_CFDP_PduFin_FLAGS_CC);
        plfin->delivery_code = FGV(fin->flags, CF_CFDP_PduFin_FLAGS_DELIVERY_CODE);
        plfin->file_status   = FGV(fin->flags, CF_CFDP_PduFin_FLAGS_FILE_STATUS);

        CF_CFDP_DecodeAllTlv(state, &plfin->tlv_list, CF_PDU_MAX_TLV);
    }
}

void CF_CFDP_DecodeAck(CF_DecoderState_t *state, CF_Logical_PduAck_t *plack)
{
    const CF_CFDP_PduAck_t *ack; /* for decoding fixed sized fields */

    ack = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduAck_t);
    if (ack != NULL)
    {
        plack->ack_directive_code = FGV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_CODE);
        plack->ack_subtype_code   = FGV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_SUBTYPE_CODE);

        plack->cc         = FGV(ack->cc_and_transaction_status, CF_CFDP_PduAck_CC);
        plack->txn_status = FGV(ack->cc_and_transaction_status, CF_CFDP_PduAck_TRANSACTION_STATUS);
    }
}

void CF_CFDP_DecodeNak(CF_DecoderState_t *state, CF_Logical_PduNak_t *plnak)
{
    const CF_CFDP_PduNak_t *nak; /* for encoding fixed sized fields */

    nak = CF_DECODE_FIXED_CHUNK(state, CF_CFDP_PduNak_t);
    if (nak != NULL)
    {
        cfdp_get_uint32(plnak->scope_start, nak->scope_start);
        cfdp_get_uint32(plnak->scope_end, nak->scope_end);

        CF_CFDP_DecodeAllSegments(state, &plnak->segment_list, CF_PDU_MAX_SEGMENTS);
    }
}

void CF_CFDP_DecodeAllTlv(CF_DecoderState_t *state, CF_Logical_TlvList_t *pltlv, uint8 limit)
{
    pltlv->num_tlv = 0;

    /* The set of TLV data may exactly consume the rest of the PDU, this is OK */
    while (limit > 0 && CF_CODEC_GET_REMAIN(state) != 0)
    {
        --limit;

        if (pltlv->num_tlv >= CF_PDU_MAX_TLV)
        {
            /* too many */
            CF_CODEC_SET_DONE(state);
        }
        else
        {
            CF_CFDP_DecodeTLV(state, &pltlv->tlv[pltlv->num_tlv]);
        }

        if (!CF_CODEC_IS_OK(state))
        {
            break;
        }

        /* only increment if above was successful */
        ++pltlv->num_tlv;
    }
}

void CF_CFDP_DecodeAllSegments(CF_DecoderState_t *state, CF_Logical_SegmentList_t *plseg, uint8 limit)
{
    plseg->num_segments = 0;

    /* The set of SegmentRequest data may exactly consume the rest of the PDU, this is OK */
    while (limit > 0 && CF_CODEC_GET_REMAIN(state) != 0)
    {
        --limit;

        if (plseg->num_segments >= CF_PDU_MAX_TLV)
        {
            /* too many */
            CF_CODEC_SET_DONE(state);
        }
        else
        {
            CF_CFDP_DecodeSegmentRequest(state, &plseg->segments[plseg->num_segments]);
        }

        if (!CF_CODEC_IS_OK(state))
        {
            break;
        }

        /* only increment if above was successful */
        ++plseg->num_segments;
    }
}
