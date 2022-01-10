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
 * Auto-Generated stub implementations for functions defined in cf_codec header
 */

#include "cf_codec.h"
#include "utgenstub.h"

void UT_DefaultHandler_CF_CFDP_CodecCheckSize(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CFDP_DoDecodeChunk(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CFDP_DoEncodeChunk(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CFDP_GetValueEncodedSize(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_DecodeIntegerInSize(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_CodecCheckSize()
 * ----------------------------------------------------
 */
bool CF_CFDP_CodecCheckSize(CF_CodecState_t *state, size_t chunksize)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_CodecCheckSize, bool);

    UT_GenStub_AddParam(CF_CFDP_CodecCheckSize, CF_CodecState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_CodecCheckSize, size_t, chunksize);

    UT_GenStub_Execute(CF_CFDP_CodecCheckSize, Basic, UT_DefaultHandler_CF_CFDP_CodecCheckSize);

    return UT_GenStub_GetReturnValue(CF_CFDP_CodecCheckSize, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeAck()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeAck(CF_DecoderState_t *state, CF_Logical_PduAck_t *plack)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeAck, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeAck, CF_Logical_PduAck_t *, plack);

    UT_GenStub_Execute(CF_CFDP_DecodeAck, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeAllSegments()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeAllSegments(CF_DecoderState_t *state, CF_Logical_SegmentList_t *plseg, uint8 limit)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeAllSegments, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeAllSegments, CF_Logical_SegmentList_t *, plseg);
    UT_GenStub_AddParam(CF_CFDP_DecodeAllSegments, uint8, limit);

    UT_GenStub_Execute(CF_CFDP_DecodeAllSegments, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeAllTlv()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeAllTlv(CF_DecoderState_t *state, CF_Logical_TlvList_t *pltlv, uint8 limit)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeAllTlv, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeAllTlv, CF_Logical_TlvList_t *, pltlv);
    UT_GenStub_AddParam(CF_CFDP_DecodeAllTlv, uint8, limit);

    UT_GenStub_Execute(CF_CFDP_DecodeAllTlv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeCrc()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeCrc(CF_DecoderState_t *state, uint32 *pcrc)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeCrc, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeCrc, uint32 *, pcrc);

    UT_GenStub_Execute(CF_CFDP_DecodeCrc, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeEof()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeEof(CF_DecoderState_t *state, CF_Logical_PduEof_t *pleof)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeEof, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeEof, CF_Logical_PduEof_t *, pleof);

    UT_GenStub_Execute(CF_CFDP_DecodeEof, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeFileDataHeader()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeFileDataHeader(CF_DecoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeFileDataHeader, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeFileDataHeader, bool, with_meta);
    UT_GenStub_AddParam(CF_CFDP_DecodeFileDataHeader, CF_Logical_PduFileDataHeader_t *, plfd);

    UT_GenStub_Execute(CF_CFDP_DecodeFileDataHeader, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeFileDirectiveHeader()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeFileDirectiveHeader(CF_DecoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeFileDirectiveHeader, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeFileDirectiveHeader, CF_Logical_PduFileDirectiveHeader_t *, pfdir);

    UT_GenStub_Execute(CF_CFDP_DecodeFileDirectiveHeader, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeFin()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeFin(CF_DecoderState_t *state, CF_Logical_PduFin_t *plfin)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeFin, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeFin, CF_Logical_PduFin_t *, plfin);

    UT_GenStub_Execute(CF_CFDP_DecodeFin, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeHeader()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeHeader(CF_DecoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeHeader, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeHeader, CF_Logical_PduHeader_t *, plh);

    UT_GenStub_Execute(CF_CFDP_DecodeHeader, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeLV()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeLV(CF_DecoderState_t *state, CF_Logical_Lv_t *pllv)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeLV, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeLV, CF_Logical_Lv_t *, pllv);

    UT_GenStub_Execute(CF_CFDP_DecodeLV, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeMd()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeMd(CF_DecoderState_t *state, CF_Logical_PduMd_t *plmd)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeMd, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeMd, CF_Logical_PduMd_t *, plmd);

    UT_GenStub_Execute(CF_CFDP_DecodeMd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeNak()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeNak(CF_DecoderState_t *state, CF_Logical_PduNak_t *plnak)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeNak, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeNak, CF_Logical_PduNak_t *, plnak);

    UT_GenStub_Execute(CF_CFDP_DecodeNak, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeSegmentRequest()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeSegmentRequest(CF_DecoderState_t *state, CF_Logical_SegmentRequest_t *plseg)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeSegmentRequest, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeSegmentRequest, CF_Logical_SegmentRequest_t *, plseg);

    UT_GenStub_Execute(CF_CFDP_DecodeSegmentRequest, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeTLV()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeTLV(CF_DecoderState_t *state, CF_Logical_Tlv_t *pltlv)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeTLV, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DecodeTLV, CF_Logical_Tlv_t *, pltlv);

    UT_GenStub_Execute(CF_CFDP_DecodeTLV, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DoDecodeChunk()
 * ----------------------------------------------------
 */
const void *CF_CFDP_DoDecodeChunk(CF_DecoderState_t *state, size_t chunksize)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_DoDecodeChunk, const void *);

    UT_GenStub_AddParam(CF_CFDP_DoDecodeChunk, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DoDecodeChunk, size_t, chunksize);

    UT_GenStub_Execute(CF_CFDP_DoDecodeChunk, Basic, UT_DefaultHandler_CF_CFDP_DoDecodeChunk);

    return UT_GenStub_GetReturnValue(CF_CFDP_DoDecodeChunk, const void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DoEncodeChunk()
 * ----------------------------------------------------
 */
void *CF_CFDP_DoEncodeChunk(CF_EncoderState_t *state, size_t chunksize)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_DoEncodeChunk, void *);

    UT_GenStub_AddParam(CF_CFDP_DoEncodeChunk, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_DoEncodeChunk, size_t, chunksize);

    UT_GenStub_Execute(CF_CFDP_DoEncodeChunk, Basic, UT_DefaultHandler_CF_CFDP_DoEncodeChunk);

    return UT_GenStub_GetReturnValue(CF_CFDP_DoEncodeChunk, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeAck()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeAck(CF_EncoderState_t *state, CF_Logical_PduAck_t *plack)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeAck, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeAck, CF_Logical_PduAck_t *, plack);

    UT_GenStub_Execute(CF_CFDP_EncodeAck, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeAllSegments()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeAllSegments(CF_EncoderState_t *state, CF_Logical_SegmentList_t *plseg)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeAllSegments, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeAllSegments, CF_Logical_SegmentList_t *, plseg);

    UT_GenStub_Execute(CF_CFDP_EncodeAllSegments, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeAllTlv()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeAllTlv(CF_EncoderState_t *state, CF_Logical_TlvList_t *pltlv)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeAllTlv, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeAllTlv, CF_Logical_TlvList_t *, pltlv);

    UT_GenStub_Execute(CF_CFDP_EncodeAllTlv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeCrc()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeCrc(CF_EncoderState_t *state, uint32 *plcrc)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeCrc, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeCrc, uint32 *, plcrc);

    UT_GenStub_Execute(CF_CFDP_EncodeCrc, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeEof()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeEof(CF_EncoderState_t *state, CF_Logical_PduEof_t *pleof)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeEof, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeEof, CF_Logical_PduEof_t *, pleof);

    UT_GenStub_Execute(CF_CFDP_EncodeEof, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeFileDataHeader()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeFileDataHeader(CF_EncoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t *plfd)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeFileDataHeader, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeFileDataHeader, bool, with_meta);
    UT_GenStub_AddParam(CF_CFDP_EncodeFileDataHeader, CF_Logical_PduFileDataHeader_t *, plfd);

    UT_GenStub_Execute(CF_CFDP_EncodeFileDataHeader, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeFileDirectiveHeader()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeFileDirectiveHeader(CF_EncoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeFileDirectiveHeader, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeFileDirectiveHeader, CF_Logical_PduFileDirectiveHeader_t *, pfdir);

    UT_GenStub_Execute(CF_CFDP_EncodeFileDirectiveHeader, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeFin()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeFin(CF_EncoderState_t *state, CF_Logical_PduFin_t *plfin)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeFin, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeFin, CF_Logical_PduFin_t *, plfin);

    UT_GenStub_Execute(CF_CFDP_EncodeFin, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeHeaderFinalSize()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeHeaderFinalSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeHeaderFinalSize, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeHeaderFinalSize, CF_Logical_PduHeader_t *, plh);

    UT_GenStub_Execute(CF_CFDP_EncodeHeaderFinalSize, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeHeaderWithoutSize()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeHeaderWithoutSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeHeaderWithoutSize, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeHeaderWithoutSize, CF_Logical_PduHeader_t *, plh);

    UT_GenStub_Execute(CF_CFDP_EncodeHeaderWithoutSize, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeLV()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeLV(CF_EncoderState_t *state, CF_Logical_Lv_t *pllv)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeLV, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeLV, CF_Logical_Lv_t *, pllv);

    UT_GenStub_Execute(CF_CFDP_EncodeLV, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeMd()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeMd(CF_EncoderState_t *state, CF_Logical_PduMd_t *plmd)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeMd, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeMd, CF_Logical_PduMd_t *, plmd);

    UT_GenStub_Execute(CF_CFDP_EncodeMd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeNak()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeNak(CF_EncoderState_t *state, CF_Logical_PduNak_t *plnak)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeNak, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeNak, CF_Logical_PduNak_t *, plnak);

    UT_GenStub_Execute(CF_CFDP_EncodeNak, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeSegmentRequest()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeSegmentRequest(CF_EncoderState_t *state, CF_Logical_SegmentRequest_t *plseg)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeSegmentRequest, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeSegmentRequest, CF_Logical_SegmentRequest_t *, plseg);

    UT_GenStub_Execute(CF_CFDP_EncodeSegmentRequest, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeTLV()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeTLV(CF_EncoderState_t *state, CF_Logical_Tlv_t *pltlv)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeTLV, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_CFDP_EncodeTLV, CF_Logical_Tlv_t *, pltlv);

    UT_GenStub_Execute(CF_CFDP_EncodeTLV, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_GetValueEncodedSize()
 * ----------------------------------------------------
 */
uint8 CF_CFDP_GetValueEncodedSize(uint64 Value)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_GetValueEncodedSize, uint8);

    UT_GenStub_AddParam(CF_CFDP_GetValueEncodedSize, uint64, Value);

    UT_GenStub_Execute(CF_CFDP_GetValueEncodedSize, Basic, UT_DefaultHandler_CF_CFDP_GetValueEncodedSize);

    return UT_GenStub_GetReturnValue(CF_CFDP_GetValueEncodedSize, uint8);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_DecodeIntegerInSize()
 * ----------------------------------------------------
 */
uint64 CF_DecodeIntegerInSize(CF_DecoderState_t *state, uint8 decode_size)
{
    UT_GenStub_SetupReturnBuffer(CF_DecodeIntegerInSize, uint64);

    UT_GenStub_AddParam(CF_DecodeIntegerInSize, CF_DecoderState_t *, state);
    UT_GenStub_AddParam(CF_DecodeIntegerInSize, uint8, decode_size);

    UT_GenStub_Execute(CF_DecodeIntegerInSize, Basic, UT_DefaultHandler_CF_DecodeIntegerInSize);

    return UT_GenStub_GetReturnValue(CF_DecodeIntegerInSize, uint64);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_EncodeIntegerInSize()
 * ----------------------------------------------------
 */
void CF_EncodeIntegerInSize(CF_EncoderState_t *state, uint64 value, uint8 encode_size)
{
    UT_GenStub_AddParam(CF_EncodeIntegerInSize, CF_EncoderState_t *, state);
    UT_GenStub_AddParam(CF_EncodeIntegerInSize, uint64, value);
    UT_GenStub_AddParam(CF_EncodeIntegerInSize, uint8, encode_size);

    UT_GenStub_Execute(CF_EncodeIntegerInSize, Basic, NULL);
}
