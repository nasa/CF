/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_codec.h"

static void UT_CF_SetupEncodeState(CF_EncoderState_t *state, void *bytes, size_t sz)
{
    memset(state, 0, sizeof(*state));

    state->base = (uint8 *)bytes;
    CF_CFDP_CodecReset(&state->codec_state, sz);
}

static void UT_CF_SetupDecodeState(CF_DecoderState_t *state, const void *bytes, size_t sz)
{
    memset(state, 0, sizeof(*state));

    state->base = (const uint8 *)bytes;
    CF_CFDP_CodecReset(&state->codec_state, sz);
}

void Test_CF_CFDP_GetValueEncodedSize(void)
{
    /* Test for:
     * uint8 CF_CFDP_GetValueEncodedSize(uint64 Value)
     */

    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(0), 1);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(1), 1);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(126), 1);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(UINT8_MAX), 1);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(UINT8_MAX + 1), 2);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(UINT16_MAX), 2);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(UINT16_MAX + 1), 3);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(16777215), 3);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(16777216), 4);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(UINT32_MAX), 4);

    /*
     * This next case uses UINT64_C macro to force promotion so the +1 is done as 64-bit,
     * otherwise the UINT32_MAX is a 32-bit value and +1 results in 0.
     */
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(UINT32_MAX + UINT64_C(1)), 5);
    UtAssert_UINT32_EQ(CF_CFDP_GetValueEncodedSize(UINT64_MAX), 8);
}

void Test_CF_EncodeIntegerInSize(void)
{
    /* Test for:
     * void CF_EncodeIntegerInSize(CF_EncoderState_t *state, uint64 value, uint8 encode_size);
     */
    CF_EncoderState_t state;
    uint8             bytes[10];
    const uint8       expected_2[] = {0x12, 0x34};
    const uint8       expected_4[] = {0x00, 0x00, 0x12, 0x34};

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_EncodeIntegerInSize(&state, 0x1234, 2);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_EncodeIntegerInSize(&state, 0x1234, sizeof(expected_2));
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_2));
    UtAssert_MemCmp(bytes, expected_2, sizeof(expected_2), "Encoded Bytes 2");
    UtAssert_MemCmpValue(bytes + sizeof(expected_2), 0xEE, sizeof(bytes) - sizeof(expected_2), "Remainder unchanged");

    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_EncodeIntegerInSize(&state, 0x1234, sizeof(expected_4));
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_4));
    UtAssert_MemCmp(bytes, expected_4, sizeof(expected_4), "Encoded Bytes 4");
    UtAssert_MemCmpValue(bytes + sizeof(expected_4), 0xEE, sizeof(bytes) - sizeof(expected_4), "Remainder unchanged");
}

void Test_CF_CFDP_EncodeHeaderWithoutSize(void)
{
    /* Test for:
     * void CF_CFDP_EncodeHeaderWithoutSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh);
     */
    CF_EncoderState_t      state;
    CF_Logical_PduHeader_t in;
    uint8                  bytes[10];
    const uint8            expected[] = {0x3c, 0xEE, 0xEE, 0x00, 0x44, 0x55, 0x66};

    memset(&in, 0, sizeof(in));
    in.version         = 1;
    in.direction       = 1;
    in.pdu_type        = 1;
    in.txm_mode        = 1;
    in.txn_seq_length  = 1;
    in.eid_length      = 1;
    in.source_eid      = 0x44;
    in.sequence_num    = 0x55;
    in.destination_eid = 0x66;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeHeaderWithoutSize(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal, 1 byte EID/TSN */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeHeaderWithoutSize(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}

void Test_CF_CFDP_EncodeHeaderFinalSize(void)
{
    /* Test for:
     * void CF_CFDP_EncodeHeaderFinalSize(CF_EncoderState_t *state, CF_Logical_PduHeader_t *plh)
     */
    CF_EncoderState_t      state;
    CF_Logical_PduHeader_t in;
    uint8                  bytes[10];
    const uint8            expected[] = {0xEE, 0x12, 0x34, 0xEE};

    memset(&in, 0, sizeof(in));
    in.data_encoded_length = 0x1234;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeHeaderFinalSize(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal, set to indicate base header has been encoded */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    state.codec_state.next_offset = sizeof(CF_CFDP_PduHeader_t);
    CF_CFDP_EncodeHeaderFinalSize(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");

    /* also a noop, but gets full branch coverage */
    CF_CFDP_EncodeHeaderFinalSize(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}

void Test_CF_CFDP_EncodeFileDirectiveHeader(void)
{
    /* Test for:
     * void CF_CFDP_EncodeFileDirectiveHeader(CF_EncoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir);
     */
    CF_EncoderState_t                   state;
    CF_Logical_PduFileDirectiveHeader_t in;
    uint8                               bytes[10];
    const uint8                         expected[] = {0x07};

    memset(&in, 0, sizeof(in));
    in.directive_code = 7;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeFileDirectiveHeader(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeFileDirectiveHeader(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeLV(void)
{
    /* Test for:
     * void CF_CFDP_EncodeLV(CF_EncoderState_t *state, CF_Logical_Lv_t *pllv);
     */
    CF_EncoderState_t state;
    CF_Logical_Lv_t   in;
    uint8             bytes[10];
    const uint8       ref[]             = {0x45, 0x67, 0x89};
    const uint8       expected[]        = {0x03, 0x45, 0x67, 0x89};
    const uint8       expected_nodata[] = {0x00};

    memset(&in, 0, sizeof(in));
    in.length   = sizeof(ref);
    in.data_ptr = ref;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeLV(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeLV(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");

    /* setup where data does not fit */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(expected) - 1);
    CF_CFDP_EncodeLV(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));

    /* setup where caller did not include data but should have */
    in.data_ptr = NULL;
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeLV(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));

    /* setup where caller did not include data validly */
    in.length = 0;
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeLV(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_nodata));
    UtAssert_MemCmp(bytes, expected_nodata, sizeof(expected_nodata), "Encoded Bytes");
}
void Test_CF_CFDP_EncodeTLV(void)
{
    /* Test for:
     * void CF_CFDP_EncodeTLV(CF_EncoderState_t *state, CF_Logical_Tlv_t *pltlv);
     */
    CF_EncoderState_t state;
    CF_Logical_Tlv_t  in;
    uint8             bytes[10];
    const uint8       expected_tlv[]    = {0x06, 0x01, 0x77};
    const uint8       expected_other[]  = {0x01, 0x03, 'a', 'b', 'c'};
    const uint8       expected_nodata[] = {0x01, 0x00};

    memset(&in, 0, sizeof(in));
    in.type     = CF_CFDP_TLV_TYPE_ENTITY_ID;
    in.length   = 1;
    in.data.eid = 0x77;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeTLV(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeTLV(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_tlv));
    UtAssert_MemCmp(bytes, expected_tlv, sizeof(expected_tlv), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected_tlv), 0xEE, sizeof(bytes) - sizeof(expected_tlv),
                         "Remainder unchanged");

    /* setup non-EID */
    in.type          = 1;
    in.length        = 3;
    in.data.data_ptr = "abc";
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeTLV(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_other));
    UtAssert_MemCmp(bytes, expected_other, sizeof(expected_other), "Encoded Bytes");

    /* setup where data does not fit */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(expected_other) - 1);
    CF_CFDP_EncodeTLV(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));

    /* setup where caller did not include data but should have */
    in.data.data_ptr = NULL;
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeTLV(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));

    /* setup where caller did not include data validly */
    in.length = 0;
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeTLV(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_nodata));
    UtAssert_MemCmp(bytes, expected_nodata, sizeof(expected_nodata), "Encoded Bytes");
}
void Test_CF_CFDP_EncodeSegmentRequest(void)
{
    /* Test for:
     * void CF_CFDP_EncodeSegmentRequest(CF_EncoderState_t *state, CF_Logical_SegmentRequest_t *plseg);
     */
    CF_EncoderState_t           state;
    CF_Logical_SegmentRequest_t in;
    uint8                       bytes[10];
    const uint8                 expected[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    memset(&in, 0, sizeof(in));
    in.offset_start = 0x11223344;
    in.offset_end   = 0x55667788;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeSegmentRequest(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeSegmentRequest(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeAllTlv(void)
{
    /* Test for:
     * void CF_CFDP_EncodeAllTlv(CF_EncoderState_t *state, CF_Logical_TlvList_t *pltlv);
     */
    CF_EncoderState_t    state;
    CF_Logical_TlvList_t in;
    uint8                bytes[10];
    const uint8          expected[] = {0x06, 0x01, 0x88, 0x06, 0x01, 0x99};

    memset(&in, 0, sizeof(in));

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop.
     * this does not set the error because num is 0 - so it works. */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeAllTlv(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    in.num_tlv         = 2;
    in.tlv[0].type     = CF_CFDP_TLV_TYPE_ENTITY_ID;
    in.tlv[0].length   = 1;
    in.tlv[0].data.eid = 0x88;
    in.tlv[1].type     = CF_CFDP_TLV_TYPE_ENTITY_ID;
    in.tlv[1].length   = 1;
    in.tlv[1].data.eid = 0x99;
    CF_CFDP_EncodeAllTlv(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeAllTlv(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeAllSegments(void)
{
    /* Test for:
     * void CF_CFDP_EncodeAllSegments(CF_EncoderState_t *state, CF_Logical_SegmentList_t *plseg);
     */
    CF_EncoderState_t        state;
    CF_Logical_SegmentList_t in;
    uint8                    bytes[20];
    const uint8              expected[] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02,
                              0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04};

    memset(&in, 0, sizeof(in));

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop.
     * this does not set the error because num is 0 - so it works. */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeAllSegments(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    in.num_segments             = 2;
    in.segments[0].offset_start = 0x1;
    in.segments[0].offset_end   = 0x2;
    in.segments[1].offset_start = 0x3;
    in.segments[1].offset_end   = 0x4;
    CF_CFDP_EncodeAllSegments(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeAllSegments(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeMd(void)
{
    /* Test for:
     * void CF_CFDP_EncodeMd(CF_EncoderState_t *state, CF_Logical_PduMd_t *plmd);
     */
    CF_EncoderState_t  state;
    CF_Logical_PduMd_t in;
    uint8              bytes[20];
    const uint8        expected[] = {0x00, 0x00, 0x00, 0x12, 0x34, 0x03, 's', 'r', 'c', 0x04, 'd', 'e', 's', 't'};

    memset(&in, 0, sizeof(in));
    in.size                     = 0x1234;
    in.dest_filename.length     = 4;
    in.dest_filename.data_ptr   = "dest";
    in.source_filename.length   = 3;
    in.source_filename.data_ptr = "src";

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeMd(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeMd(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeFileDataHeader(void)
{
    /* Test for:
     * void CF_CFDP_EncodeFileDataHeader(CF_EncoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t
     * *plfd);
     */
    CF_EncoderState_t              state;
    CF_Logical_PduFileDataHeader_t in;
    uint8                          bytes[20];
    const uint8                    expected_basic[] = {0x00, 0x00, 0x00, 0x13};
    const uint8 expected_meta[] = {0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x13};

    memset(&in, 0, sizeof(in));
    in.offset   = 0x13;
    in.data_len = 4;
    in.data_ptr = "data";

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeFileDataHeader(&state, false, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal, no metadata */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeFileDataHeader(&state, false, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_basic));
    UtAssert_MemCmp(bytes, expected_basic, sizeof(expected_basic), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected_basic), 0xEE, sizeof(bytes) - sizeof(expected_basic),
                         "Remainder unchanged");

    /* setup nominal, with metadata */
    in.continuation_state                    = 1;
    in.segment_list.num_segments             = 1;
    in.segment_list.segments[0].offset_start = 0;
    in.segment_list.segments[0].offset_end   = 0x11;
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeFileDataHeader(&state, true, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected_meta));
    UtAssert_MemCmp(bytes, expected_meta, sizeof(expected_meta), "Encoded Bytes");
}
void Test_CF_CFDP_EncodeEof(void)
{
    /* Test for:
     * void CF_CFDP_EncodeEof(CF_EncoderState_t *state, CF_Logical_PduEof_t *pleof);
     */
    CF_EncoderState_t   state;
    CF_Logical_PduEof_t in;
    uint8               bytes[20];
    const uint8         expected[] = {0x10, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x45, 0x67, 0x06, 0x01, 0xaa};

    memset(&in, 0, sizeof(in));
    in.crc                      = 0x12345678;
    in.size                     = 0x4567;
    in.cc                       = 1;
    in.tlv_list.num_tlv         = 1;
    in.tlv_list.tlv[0].type     = CF_CFDP_TLV_TYPE_ENTITY_ID;
    in.tlv_list.tlv[0].length   = 1;
    in.tlv_list.tlv[0].data.eid = 0xaa;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeEof(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeEof(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeFin(void)
{
    /* Test for:
     * void CF_CFDP_EncodeFin(CF_EncoderState_t *state, CF_Logical_PduFin_t *plfin);
     */
    CF_EncoderState_t   state;
    CF_Logical_PduFin_t in;
    uint8               bytes[10];
    const uint8         expected[] = {0x16};

    memset(&in, 0, sizeof(in));
    in.cc            = 1;
    in.delivery_code = 1;
    in.file_status   = 2;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeFin(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeFin(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeAck(void)
{
    /* Test for:
     * void CF_CFDP_EncodeAck(CF_EncoderState_t *state, CF_Logical_PduAck_t *plack);
     */
    CF_EncoderState_t   state;
    CF_Logical_PduAck_t in;
    uint8               bytes[10];
    const uint8         expected[] = {0x51, 0x23};

    memset(&in, 0, sizeof(in));
    in.ack_directive_code = 5;
    in.ack_subtype_code   = 1;
    in.cc                 = 2;
    in.txn_status         = 3;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeAck(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeAck(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeNak(void)
{
    /* Test for:
     * void CF_CFDP_EncodeNak(CF_EncoderState_t *state, CF_Logical_PduNak_t *plnak);
     */
    CF_EncoderState_t   state;
    CF_Logical_PduNak_t in;
    uint8               bytes[30];
    const uint8         expected[] = {0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00, 0x05,
                              0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08};

    memset(&in, 0, sizeof(in));
    in.scope_start                           = 0x0102;
    in.scope_end                             = 0x0304;
    in.segment_list.num_segments             = 2;
    in.segment_list.segments[0].offset_start = 0x5;
    in.segment_list.segments[0].offset_end   = 0x6;
    in.segment_list.segments[1].offset_start = 0x7;
    in.segment_list.segments[1].offset_end   = 0x8;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeNak(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeNak(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_CFDP_EncodeCrc(void)
{
    /* Test for:
     * void CF_CFDP_EncodeCrc(CF_EncoderState_t *state, uint32 *pcrc);
     */
    CF_EncoderState_t state;
    uint32            in;
    uint8             bytes[10];
    const uint8       expected[] = {0xde, 0xad, 0xbe, 0xef};

    memset(&in, 0, sizeof(in));
    in = 0xdeadbeef;

    /* fill with nonzero bytes so it is evident what was set */
    memset(bytes, 0xEE, sizeof(bytes));

    /* call w/zero state should be noop */
    UT_CF_SetupEncodeState(&state, bytes, 0);
    CF_CFDP_EncodeCrc(&state, &in);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(bytes, 0xEE, sizeof(bytes), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupEncodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_EncodeCrc(&state, &in);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(expected));
    UtAssert_MemCmp(bytes, expected, sizeof(expected), "Encoded Bytes");
    UtAssert_MemCmpValue(bytes + sizeof(expected), 0xEE, sizeof(bytes) - sizeof(expected), "Remainder unchanged");
}
void Test_CF_DecodeIntegerInSize(void)
{
    /* Test for:
     * uint64 CF_DecodeIntegerInSize(CF_DecoderState_t *state, uint8 decode_size);
     */
    CF_DecoderState_t      state;
    CF_Logical_PduHeader_t out;
    const uint8            bytes_2[] = {0x12, 0x34};
    const uint8            bytes_4[] = {0x00, 0x56, 0x78, 0x9a};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes_2, 0);
    UtAssert_ZERO(CF_DecodeIntegerInSize(&state, 2));
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal, 2 byte int */
    UT_CF_SetupDecodeState(&state, bytes_2, sizeof(bytes_2));
    UtAssert_UINT32_EQ(CF_DecodeIntegerInSize(&state, sizeof(bytes_2)), 0x1234);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes_2));

    /* setup nominal, 4 byte int */
    UT_CF_SetupDecodeState(&state, bytes_4, sizeof(bytes_4));
    UtAssert_UINT32_EQ(CF_DecodeIntegerInSize(&state, sizeof(bytes_4)), 0x56789a);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes_4));
}
void Test_CF_CFDP_DecodeHeader(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeHeader(CF_DecoderState_t *state, CF_Logical_PduHeader_t *plh);
     */
    CF_DecoderState_t      state;
    CF_Logical_PduHeader_t out;
    const uint8            bytes[]     = {0x3c, 0x01, 0x02, 0x00, 0x44, 0x55, 0x66};
    const uint8            bad_input[] = {0x20, 0x01, 0x02, 0x33, 0x01, 0x02, 0x03};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeHeader(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeHeader(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.version, 1);
    UtAssert_UINT32_EQ(out.direction, 1);
    UtAssert_UINT32_EQ(out.pdu_type, 1);
    UtAssert_UINT32_EQ(out.txm_mode, 1);
    UtAssert_UINT32_EQ(out.eid_length, 1);
    UtAssert_UINT32_EQ(out.txn_seq_length, 1);
    UtAssert_UINT32_EQ(out.data_encoded_length, 0x0102);
    UtAssert_UINT32_EQ(out.source_eid, 0x44);
    UtAssert_UINT32_EQ(out.sequence_num, 0x55);
    UtAssert_UINT32_EQ(out.destination_eid, 0x66);
    UtAssert_UINT32_EQ(out.header_encoded_length, sizeof(bytes));

    /*
     * The bad input has large embedded EID/TSN lengths that would
     * cause it to read beyond the end of the buffer.  This is to
     * verify that the decode detects the problem and does not
     * read beyond the end.
     */
    UT_CF_SetupDecodeState(&state, bad_input, sizeof(bad_input));
    CF_CFDP_DecodeHeader(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeFileDirectiveHeader(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeFileDirectiveHeader(CF_DecoderState_t *state, CF_Logical_PduFileDirectiveHeader_t *pfdir);
     */
    CF_DecoderState_t                   state;
    CF_Logical_PduFileDirectiveHeader_t out;
    const uint8                         bytes[] = {0x08};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeFileDirectiveHeader(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeFileDirectiveHeader(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.directive_code, CF_CFDP_FileDirective_NAK);
}
void Test_CF_CFDP_DecodeLV(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeLV(CF_DecoderState_t *state, CF_Logical_Lv_t *pllv);
     */
    CF_DecoderState_t state;
    CF_Logical_Lv_t   out;
    const uint8       bytes[]     = {0x03, 0x45, 0x67, 0x89};
    const uint8       bad_input[] = {0x32, 0x45, 0x67, 0x89, 0xaa};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeLV(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeLV(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.length, 3);
    UtAssert_ADDRESS_EQ(out.data_ptr, &bytes[1]);

    /* The bad input has a long length that would go beyond the end */
    UT_CF_SetupDecodeState(&state, bad_input, sizeof(bad_input));
    CF_CFDP_DecodeLV(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeTLV(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeTLV(CF_DecoderState_t *state, CF_Logical_Tlv_t *pltlv);
     */
    CF_DecoderState_t state;
    CF_Logical_Tlv_t  out;
    const uint8       bytes_tlv[]   = {0x06, 0x01, 0x77};
    const uint8       bytes_other[] = {0x01, 0x02, 0x88, 0x99};
    const uint8       bad_input[]   = {0x06, 0x21, 0x88, 0x99};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes_tlv, 0);
    CF_CFDP_DecodeTLV(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes_tlv, sizeof(bytes_tlv));
    CF_CFDP_DecodeTLV(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes_tlv));
    UtAssert_UINT32_EQ(out.type, CF_CFDP_TLV_TYPE_ENTITY_ID);
    UtAssert_UINT32_EQ(out.length, 1);
    UtAssert_UINT32_EQ(out.data.eid, 0x77);

    /* non-EID TLVs are defined by the protocol but not yet used by CF */
    UT_CF_SetupDecodeState(&state, bytes_other, sizeof(bytes_other));
    CF_CFDP_DecodeTLV(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes_other));
    UtAssert_UINT32_EQ(out.type, 1);
    UtAssert_UINT32_EQ(out.length, 2);
    UtAssert_ADDRESS_EQ(out.data.data_ptr, &bytes_other[2]);

    /* The bad input has a long length that would go beyond the end */
    UT_CF_SetupDecodeState(&state, bad_input, sizeof(bad_input));
    CF_CFDP_DecodeTLV(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeSegmentRequest(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeSegmentRequest(CF_DecoderState_t *state, CF_Logical_SegmentRequest_t *plseg);
     */
    CF_DecoderState_t           state;
    CF_Logical_SegmentRequest_t out;
    const uint8                 bytes[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeSegmentRequest(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeSegmentRequest(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.offset_start, 0x11223344);
    UtAssert_UINT32_EQ(out.offset_end, 0x55667788);
}
void Test_CF_CFDP_DecodeAllTlv(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeAllTlv(CF_DecoderState_t *state, CF_Logical_TlvList_t *pltlv, uint8 limit);
     */
    CF_DecoderState_t    state;
    CF_Logical_TlvList_t out;
    const uint8          bytes[]                              = {0x06, 0x01, 0x88, 0x06, 0x01, 0x99};
    const uint8          bad_input[]                          = {0x06, 0x07, 0x88, 0x06, 0x03, 0x99, 0xaa};
    const uint8          long_input[2 * (CF_PDU_MAX_TLV + 1)] = {0};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop
     * as there are no fixed fields here, this succeeds. */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeAllTlv(&state, &out, CF_PDU_MAX_TLV);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_ZERO(out.num_tlv);

    /* setup nominal, but limit to single TLV */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeAllTlv(&state, &out, 1);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), 3);
    UtAssert_UINT32_EQ(CF_CODEC_GET_REMAIN(&state), 3);
    UtAssert_UINT32_EQ(out.num_tlv, 1);
    UtAssert_UINT32_EQ(out.tlv[0].type, CF_CFDP_TLV_TYPE_ENTITY_ID);
    UtAssert_UINT32_EQ(out.tlv[0].length, 1);
    UtAssert_UINT32_EQ(out.tlv[0].data.eid, 0x88);

    /* setup nominal, do all */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeAllTlv(&state, &out, CF_PDU_MAX_TLV);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.num_tlv, 2);
    UtAssert_UINT32_EQ(out.tlv[0].type, CF_CFDP_TLV_TYPE_ENTITY_ID);
    UtAssert_UINT32_EQ(out.tlv[0].length, 1);
    UtAssert_UINT32_EQ(out.tlv[0].data.eid, 0x88);
    UtAssert_UINT32_EQ(out.tlv[1].type, CF_CFDP_TLV_TYPE_ENTITY_ID);
    UtAssert_UINT32_EQ(out.tlv[1].length, 1);
    UtAssert_UINT32_EQ(out.tlv[1].data.eid, 0x99);

    /* The bad input has a long length that would go beyond the end */
    UT_CF_SetupDecodeState(&state, bad_input, sizeof(bad_input));
    CF_CFDP_DecodeAllTlv(&state, &out, CF_PDU_MAX_TLV);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));

    UT_CF_SetupDecodeState(&state, long_input, sizeof(long_input));
    CF_CFDP_DecodeAllTlv(&state, &out, 1 + CF_PDU_MAX_TLV);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeAllSegments(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeAllSegments(CF_DecoderState_t *state, CF_Logical_SegmentList_t *plseg, uint8 limit);
     */
    CF_DecoderState_t        state;
    CF_Logical_SegmentList_t out;
    const uint8              bytes[] = {0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02,
                           0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04};
    const uint8              long_input[8 * (CF_PDU_MAX_SEGMENTS + 1)] = {0};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop
     * as there are no fixed fields here, this succeeds. */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeAllSegments(&state, &out, CF_PDU_MAX_SEGMENTS);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_ZERO(out.num_segments);

    /* setup nominal, but limit to single segment */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeAllSegments(&state, &out, 1);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), 8);
    UtAssert_UINT32_EQ(CF_CODEC_GET_REMAIN(&state), 8);
    UtAssert_UINT32_EQ(out.num_segments, 1);
    UtAssert_UINT32_EQ(out.segments[0].offset_start, 0x1);
    UtAssert_UINT32_EQ(out.segments[0].offset_end, 0x2);

    /* setup nominal, do all */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeAllSegments(&state, &out, CF_PDU_MAX_SEGMENTS);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.num_segments, 2);
    UtAssert_UINT32_EQ(out.segments[0].offset_start, 0x1);
    UtAssert_UINT32_EQ(out.segments[0].offset_end, 0x2);
    UtAssert_UINT32_EQ(out.segments[1].offset_start, 0x3);
    UtAssert_UINT32_EQ(out.segments[1].offset_end, 0x4);

    UT_CF_SetupDecodeState(&state, long_input, sizeof(long_input));
    CF_CFDP_DecodeAllSegments(&state, &out, 1 + CF_PDU_MAX_SEGMENTS);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeMd(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeMd(CF_DecoderState_t *state, CF_Logical_PduMd_t *plmd);
     */
    CF_DecoderState_t  state;
    CF_Logical_PduMd_t out;
    const uint8        bytes[]     = {0x00, 0x00, 0x00, 0x12, 0x34, 0x03, 's', 'r', 'c', 0x04, 'd', 'e', 's', 't'};
    const uint8        bad_input[] = {0x00, 0x00, 0x00, 0x12, 0x34, 0x56, 's', 'r', 'c', 0x04, 'd', 'e', 's', 't'};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeMd(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeMd(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.size, 0x1234);
    UtAssert_ADDRESS_EQ(out.source_filename.data_ptr, &bytes[6]);
    UtAssert_UINT32_EQ(out.source_filename.length, 3);
    UtAssert_ADDRESS_EQ(out.dest_filename.data_ptr, &bytes[10]);
    UtAssert_UINT32_EQ(out.dest_filename.length, 4);

    /* The bad input has a long length that would go beyond the end */
    UT_CF_SetupDecodeState(&state, bad_input, sizeof(bad_input));
    CF_CFDP_DecodeMd(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeFileDataHeader(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeFileDataHeader(CF_DecoderState_t *state, bool with_meta, CF_Logical_PduFileDataHeader_t
     * *plfd);
     */
    CF_DecoderState_t              state;
    CF_Logical_PduFileDataHeader_t out;
    const uint8                    bytes_basic[] = {0x00, 0x00, 0x00, 0x13, 0xdd};
    const uint8 bytes_meta[]  = {0x41, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x13, 0xcc};
    const uint8 bad_input_1[] = {0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x00, 0x00, 0x00, 0x13, 0xcc};
    const uint8 bad_input_2[] = {0x41, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes_basic, 0);
    CF_CFDP_DecodeFileDataHeader(&state, false, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    /* Note - this will set the segment count to 0, so the bytes will be changed */

    /* setup nominal, no metadata */
    UT_CF_SetupDecodeState(&state, bytes_basic, sizeof(bytes_basic));
    CF_CFDP_DecodeFileDataHeader(&state, false, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes_basic));
    UtAssert_UINT32_EQ(out.offset, 0x13);
    UtAssert_UINT32_EQ(out.data_len, 1);
    UtAssert_ADDRESS_EQ(out.data_ptr, &bytes_basic[4]);

    /* setup nominal, with metadata */
    UT_CF_SetupDecodeState(&state, bytes_meta, sizeof(bytes_meta));
    CF_CFDP_DecodeFileDataHeader(&state, true, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes_meta));
    UtAssert_UINT32_EQ(out.continuation_state, 1);
    UtAssert_UINT32_EQ(out.segment_list.num_segments, 1);
    UtAssert_UINT32_EQ(out.segment_list.segments[0].offset_start, 0x01020304);
    UtAssert_UINT32_EQ(out.segment_list.segments[0].offset_end, 0x05060708);
    UtAssert_UINT32_EQ(out.offset, 0x13);
    UtAssert_UINT32_EQ(out.data_len, 1);
    UtAssert_ADDRESS_EQ(out.data_ptr, &bytes_meta[13]);

    /* The bad input has a long length that would go beyond the end */
    /* first has too many segments */
    UT_CF_SetupDecodeState(&state, bad_input_1, sizeof(bad_input_1));
    CF_CFDP_DecodeFileDataHeader(&state, true, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    /* next has a truncated segment */
    UT_CF_SetupDecodeState(&state, bad_input_2, sizeof(bad_input_2));
    CF_CFDP_DecodeFileDataHeader(&state, true, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeEof(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeEof(CF_DecoderState_t *state, CF_Logical_PduEof_t *pleof);
     */
    CF_DecoderState_t   state;
    CF_Logical_PduEof_t out;
    const uint8         bytes[]     = {0x10, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x45, 0x67, 0x06, 0x01, 0xaa};
    const uint8         bad_input[] = {0x10, 0x12, 0x34, 0x56, 0x78, 0x00, 0x00, 0x45, 0x67, 0x06, 0x06, 0xaa, 0xbb};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeEof(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeEof(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.crc, 0x12345678);
    UtAssert_UINT32_EQ(out.size, 0x4567);
    UtAssert_UINT32_EQ(out.cc, 1);
    UtAssert_UINT32_EQ(out.tlv_list.num_tlv, 1);
    UtAssert_UINT32_EQ(out.tlv_list.tlv[0].type, CF_CFDP_TLV_TYPE_ENTITY_ID);
    UtAssert_UINT32_EQ(out.tlv_list.tlv[0].length, 1);
    UtAssert_UINT32_EQ(out.tlv_list.tlv[0].data.eid, 0xaa);

    /* The bad input has a long length that would go beyond the end */
    UT_CF_SetupDecodeState(&state, bad_input, sizeof(bad_input));
    CF_CFDP_DecodeEof(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
}
void Test_CF_CFDP_DecodeFin(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeFin(CF_DecoderState_t *state, CF_Logical_PduFin_t *plfin);
     */
    CF_DecoderState_t   state;
    CF_Logical_PduFin_t out;
    const uint8         bytes[] = {0x16};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeFin(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeFin(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.cc, 1);
    UtAssert_UINT32_EQ(out.delivery_code, 1);
    UtAssert_UINT32_EQ(out.file_status, 2);
}
void Test_CF_CFDP_DecodeAck(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeAck(CF_DecoderState_t *state, CF_Logical_PduAck_t *plack);
     */
    CF_DecoderState_t   state;
    CF_Logical_PduAck_t out;
    const uint8         bytes[] = {0x51, 0x23};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeAck(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeAck(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.ack_directive_code, 5);
    UtAssert_UINT32_EQ(out.ack_subtype_code, 1);
    UtAssert_UINT32_EQ(out.cc, 2);
    UtAssert_UINT32_EQ(out.txn_status, 3);
}
void Test_CF_CFDP_DecodeNak(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeNak(CF_DecoderState_t *state, CF_Logical_PduNak_t *plnak);
     */
    CF_DecoderState_t   state;
    CF_Logical_PduNak_t out;
    const uint8         bytes[] = {0x00, 0x00, 0x01, 0x02, 0x00, 0x00, 0x03, 0x04, 0x00, 0x00, 0x00, 0x05,
                           0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x08};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, bytes, 0);
    CF_CFDP_DecodeNak(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeNak(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out.scope_start, 0x0102);
    UtAssert_UINT32_EQ(out.scope_end, 0x0304);
    UtAssert_UINT32_EQ(out.segment_list.num_segments, 2);
    UtAssert_UINT32_EQ(out.segment_list.segments[0].offset_start, 0x5);
    UtAssert_UINT32_EQ(out.segment_list.segments[0].offset_end, 0x6);
    UtAssert_UINT32_EQ(out.segment_list.segments[1].offset_start, 0x7);
    UtAssert_UINT32_EQ(out.segment_list.segments[1].offset_end, 0x8);
}
void Test_CF_CFDP_DecodeCrc(void)
{
    /* Test for:
     * void   CF_CFDP_DecodeCrc(CF_DecoderState_t *state, uint32 *pcrc);
     */
    CF_DecoderState_t state;
    uint32            out;
    const uint8       bytes[] = {0xde, 0xad, 0xbe, 0xef};

    /* fill with nonzero bytes so it is evident what was set */
    memset(&out, 0xEE, sizeof(out));

    /* call w/zero state should be noop */
    UT_CF_SetupDecodeState(&state, "", 0);
    CF_CFDP_DecodeCrc(&state, &out);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&state));
    UtAssert_MemCmpValue(&out, 0xEE, sizeof(out), "Bytes unchanged");

    /* setup nominal */
    UT_CF_SetupDecodeState(&state, bytes, sizeof(bytes));
    CF_CFDP_DecodeCrc(&state, &out);
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&state));
    UtAssert_UINT32_EQ(CF_CODEC_GET_POSITION(&state), sizeof(bytes));
    UtAssert_UINT32_EQ(out, 0xdeadbeef);
}

/*******************************************************************************
**
**  cf_codec_tests UtTest_Add groups
**
*******************************************************************************/

void Add_CF_Encode_tests(void)
{
    UtTest_Add(Test_CF_EncodeIntegerInSize, NULL, NULL, "CF_EncodeIntegerInSize");
    UtTest_Add(Test_CF_CFDP_EncodeHeaderWithoutSize, NULL, NULL, "CF_CFDP_EncodeHeaderWithoutSize");
    UtTest_Add(Test_CF_CFDP_EncodeHeaderFinalSize, NULL, NULL, "CF_CFDP_EncodeHeaderFinalSize");
    UtTest_Add(Test_CF_CFDP_EncodeFileDirectiveHeader, NULL, NULL, "CF_CFDP_EncodeFileDirectiveHeader");
    UtTest_Add(Test_CF_CFDP_EncodeLV, NULL, NULL, "CF_CFDP_EncodeLV");
    UtTest_Add(Test_CF_CFDP_EncodeTLV, NULL, NULL, "CF_CFDP_EncodeTLV");
    UtTest_Add(Test_CF_CFDP_EncodeSegmentRequest, NULL, NULL, "CF_CFDP_EncodeSegmentRequest");
    UtTest_Add(Test_CF_CFDP_EncodeAllTlv, NULL, NULL, "CF_CFDP_EncodeAllTlv");
    UtTest_Add(Test_CF_CFDP_EncodeAllSegments, NULL, NULL, "CF_CFDP_EncodeAllSegments");
    UtTest_Add(Test_CF_CFDP_EncodeMd, NULL, NULL, "CF_CFDP_EncodeMd");
    UtTest_Add(Test_CF_CFDP_EncodeFileDataHeader, NULL, NULL, "CF_CFDP_EncodeFileDataHeader");
    UtTest_Add(Test_CF_CFDP_EncodeEof, NULL, NULL, "CF_CFDP_EncodeEof");
    UtTest_Add(Test_CF_CFDP_EncodeFin, NULL, NULL, "CF_CFDP_EncodeFin");
    UtTest_Add(Test_CF_CFDP_EncodeAck, NULL, NULL, "CF_CFDP_EncodeAck");
    UtTest_Add(Test_CF_CFDP_EncodeNak, NULL, NULL, "CF_CFDP_EncodeNak");
    UtTest_Add(Test_CF_CFDP_EncodeCrc, NULL, NULL, "CF_CFDP_EncodeCrc");
}

void Add_CF_Decode_tests(void)
{
    UtTest_Add(Test_CF_DecodeIntegerInSize, NULL, NULL, "CF_DecodeIntegerInSize");
    UtTest_Add(Test_CF_CFDP_DecodeHeader, NULL, NULL, "CF_CFDP_DecodeHeader");
    UtTest_Add(Test_CF_CFDP_DecodeFileDirectiveHeader, NULL, NULL, "CF_CFDP_DecodeFileDirectiveHeader");
    UtTest_Add(Test_CF_CFDP_DecodeLV, NULL, NULL, "CF_CFDP_DecodeLV");
    UtTest_Add(Test_CF_CFDP_DecodeTLV, NULL, NULL, "CF_CFDP_DecodeTLV");
    UtTest_Add(Test_CF_CFDP_DecodeSegmentRequest, NULL, NULL, "CF_CFDP_DecodeSegmentRequest");
    UtTest_Add(Test_CF_CFDP_DecodeAllTlv, NULL, NULL, "CF_CFDP_DecodeAllTlv");
    UtTest_Add(Test_CF_CFDP_DecodeAllSegments, NULL, NULL, "CF_CFDP_DecodeAllSegments");
    UtTest_Add(Test_CF_CFDP_DecodeMd, NULL, NULL, "CF_CFDP_DecodeMd");
    UtTest_Add(Test_CF_CFDP_DecodeFileDataHeader, NULL, NULL, "CF_CFDP_DecodeFileDataHeader");
    UtTest_Add(Test_CF_CFDP_DecodeEof, NULL, NULL, "CF_CFDP_DecodeEof");
    UtTest_Add(Test_CF_CFDP_DecodeFin, NULL, NULL, "CF_CFDP_DecodeFin");
    UtTest_Add(Test_CF_CFDP_DecodeAck, NULL, NULL, "CF_CFDP_DecodeAck");
    UtTest_Add(Test_CF_CFDP_DecodeNak, NULL, NULL, "CF_CFDP_DecodeNak");
    UtTest_Add(Test_CF_CFDP_DecodeCrc, NULL, NULL, "CF_CFDP_DecodeCrc");
}

/*******************************************************************************
**
**  cf_codec_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_CFDP_GetValueEncodedSize, NULL, NULL, "CF_CFDP_GetValueEncodedSize");

    Add_CF_Encode_tests();
    Add_CF_Decode_tests();

} /* end UtTest_Setup for cf_codec_tests.c */

/* end cf_codec_tests.c */