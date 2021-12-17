/************************************************************************
** File: cf_cfdp.h
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
**  The CF Application cfdp engine and packet parsing header file
**
**
**
*************************************************************************/

#ifndef CF_CODEC_H
#define CF_CODEC_H

#include "cfe.h"
#include "cf_cfdp_pdu.h"
#include "cf_logical_pdu.h"

typedef struct CF_CodecState
{
    bool   is_valid;
    size_t next_offset;
    size_t max_size;
} CF_CodecState_t;

typedef struct CF_EncoderState
{
    CF_CodecState_t codec_state;
    uint8          *base;
} CF_EncoderState_t;

typedef struct CF_DecoderState
{
    CF_CodecState_t codec_state;
    const uint8    *base;
} CF_DecoderState_t;

static inline bool CF_CFDP_CodecIsOK(const CF_CodecState_t *state)
{
    return state->is_valid;
}

static inline void CF_CFDP_CodecSetDone(CF_CodecState_t *state)
{
    state->is_valid = false;
}

static inline size_t CF_CFDP_CodecGetPosition(const CF_CodecState_t *state)
{
    return state->next_offset;
}

static inline size_t CF_CFDP_CodecGetSize(const CF_CodecState_t *state)
{
    return state->max_size;
}

static inline size_t CF_CFDP_CodecGetRemain(const CF_CodecState_t *state)
{
    return (state->max_size - state->next_offset);
}

static inline void CF_CFDP_CodecReset(CF_CodecState_t *state, size_t max_size)
{
    state->is_valid    = true;
    state->next_offset = 0;
    state->max_size    = max_size;
}

bool        CF_CFDP_CodecCheckSize(CF_CodecState_t *state, size_t chunksize);
void       *CF_CFDP_DoEncodeChunk(CF_EncoderState_t *state, size_t chunksize);
const void *CF_CFDP_DoDecodeChunk(CF_DecoderState_t *state, size_t chunksize);

uint8 CF_CFDP_GetValueEncodedSize(uint64 Value);

#define CF_ENCODE_FIXED_CHUNK(state, type) ((type *)CF_CFDP_DoEncodeChunk(state, sizeof(type)))
#define CF_DECODE_FIXED_CHUNK(state, type) ((const type *)CF_CFDP_DoDecodeChunk(state, sizeof(type)))

#define CF_CODEC_IS_OK(s)        CF_CFDP_CodecIsOK(&((s)->codec_state))
#define CF_CODEC_SET_DONE(s)     CF_CFDP_CodecSetDone(&((s)->codec_state))
#define CF_CODEC_GET_POSITION(s) CF_CFDP_CodecGetPosition(&((s)->codec_state))
#define CF_CODEC_GET_REMAIN(s)   CF_CFDP_CodecGetRemain(&((s)->codec_state))
#define CF_CODEC_GET_SIZE(s)     CF_CFDP_CodecGetSize(&((s)->codec_state))

void CF_EncodeIntegerInSize(CF_EncoderState_t *state, uint64 value, uint8 encode_size);
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
void CF_CFDP_EncodeHeaderWithoutSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh);
void CF_CFDP_EncodeHeaderFinalSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh);
void CF_CFDP_EncodeFileDirectiveHeader(CF_EncoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir);
void CF_CFDP_EncodeLV(CF_EncoderState_t *state, CF_Logical_Lv_t *pllv);
void CF_CFDP_EncodeTLV(CF_EncoderState_t *state, CF_Logical_Tlv_t *pltlv);
void CF_CFDP_EncodeSegmentRequest(CF_EncoderState_t *state, CF_Logical_SegmentRequest_t *plseg);
void CF_CFDP_EncodeAllTlv(CF_EncoderState_t *state, CF_Logical_TlvList_t *pltlv);
void CF_CFDP_EncodeAllSegments(CF_EncoderState_t *state, CF_Logical_SegmentList_t *plseg);
void CF_CFDP_EncodeMd(CF_EncoderState_t *state, CF_Logical_PduMd_t *plmd);
void CF_CFDP_EncodeFileDataHeader(CF_EncoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd);
void CF_CFDP_EncodeEof(CF_EncoderState_t *state, CF_Logical_PduEof_t *pleof);
void CF_CFDP_EncodeFin(CF_EncoderState_t *state, CF_Logical_PduFin_t *plfin);
void CF_CFDP_EncodeAck(CF_EncoderState_t *state, CF_Logical_PduAck_t *plack);
void CF_CFDP_EncodeNak(CF_EncoderState_t *state, CF_Logical_PduNak_t *plnak);
void CF_CFDP_EncodeCrc(CF_EncoderState_t *state, uint32 *plcrc);

uint64 CF_DecodeIntegerInSize(CF_DecoderState_t *state, uint8 decode_size);
void   CF_CFDP_DecodeHeader(CF_DecoderState_t *state, CF_Logical_PduHeader_t *plh);
void   CF_CFDP_DecodeFileDirectiveHeader(CF_DecoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir);
void   CF_CFDP_DecodeLV(CF_DecoderState_t *state, CF_Logical_Lv_t *pllv);
void   CF_CFDP_DecodeTLV(CF_DecoderState_t *state, CF_Logical_Tlv_t *pltlv);
void   CF_CFDP_DecodeSegmentRequest(CF_DecoderState_t *state, CF_Logical_SegmentRequest_t *plseg);
void   CF_CFDP_DecodeAllTlv(CF_DecoderState_t *state, CF_Logical_TlvList_t *pltlv, uint8 limit);
void   CF_CFDP_DecodeAllSegments(CF_DecoderState_t *state, CF_Logical_SegmentList_t *plseg, uint8 limit);
void   CF_CFDP_DecodeMd(CF_DecoderState_t *state, CF_Logical_PduMd_t *plmd);
void   CF_CFDP_DecodeFileDataHeader(CF_DecoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd);
void   CF_CFDP_DecodeEof(CF_DecoderState_t *state, CF_Logical_PduEof_t *pleof);
void   CF_CFDP_DecodeFin(CF_DecoderState_t *state, CF_Logical_PduFin_t *plfin);
void   CF_CFDP_DecodeAck(CF_DecoderState_t *state, CF_Logical_PduAck_t *plack);
void   CF_CFDP_DecodeNak(CF_DecoderState_t *state, CF_Logical_PduNak_t *plnak);
void   CF_CFDP_DecodeCrc(CF_DecoderState_t *state, uint32 *pcrc);

#endif /* !CF_CODEC_H */
