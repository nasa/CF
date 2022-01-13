/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_test_alt_handler.h"
#include "cf_events.h"
#include "cf_cfdp.h"
#include "cf_cfdp_s.h"
#include "cf_cfdp_pdu.h"
#include "cf_cfdp_sbintf.h"
#include "cf_cfdp_dispatch.h"

/*******************************************************************************
**
**  cf_cfdp_tests local utility functions
**
*******************************************************************************/

static void UT_CFDP_SetupBasicRxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    static CF_DecoderState_t ut_decoder;
    static uint8             bytes[CF_CFDP_MAX_HEADER_SIZE];

    memset(bytes, 0, sizeof(bytes));

    ut_decoder.base                    = bytes;
    ut_decoder.codec_state.is_valid    = true;
    ut_decoder.codec_state.max_size    = sizeof(bytes);
    ut_decoder.codec_state.next_offset = 0;

    pdu_buffer->pdec = &ut_decoder;
}

static void UT_CFDP_SetupBasicTxState(CF_Logical_PduBuffer_t *pdu_buffer)
{
    static CF_EncoderState_t ut_encoder;
    static uint8             bytes[CF_CFDP_MAX_HEADER_SIZE];

    memset(bytes, 0, sizeof(bytes));

    ut_encoder.base                    = bytes;
    ut_encoder.codec_state.is_valid    = true;
    ut_encoder.codec_state.max_size    = sizeof(bytes);
    ut_encoder.codec_state.next_offset = 0;

    pdu_buffer->penc = &ut_encoder;
}

static void UT_CFDP_SetupBasicTestState(UT_CF_Setup_t setup, CF_Logical_PduBuffer_t **pdu_buffer_p,
                                        CF_Channel_t **channel_p, CF_History_t **history_p, CF_Transaction_t **txn_p,
                                        CF_ConfigTable_t **config_table_p)
{
    /*
     * fake objects used to pass into CF app during unit tests.
     * These are declared static so they can be returned
     */
    static CF_Logical_PduBuffer_t ut_pdu_buffer;
    static CF_History_t           ut_history;
    static CF_Transaction_t       ut_transaction;
    static CF_ConfigTable_t       ut_config_table;

    /*
     * always clear all objects, regardless of what was asked for.
     * this helps ensure that a test does not depend on preexisting data
     * in the buffer (each test should set up its buffers in full)
     */
    memset(&ut_pdu_buffer, 0, sizeof(ut_pdu_buffer));
    memset(&ut_history, 0, sizeof(ut_history));
    memset(&ut_transaction, 0, sizeof(ut_transaction));
    memset(&ut_config_table, 0, sizeof(ut_config_table));

    /* certain pointers should be connected even if they were not asked for,
     * as internal code may assume these are set (test cases may un-set) */
    ut_transaction.history  = &ut_history;
    CF_AppData.config_table = &ut_config_table;

    if (pdu_buffer_p)
    {
        if (setup == UT_CF_Setup_TX || setup == UT_CF_Setup_RX)
        {
            *pdu_buffer_p = &ut_pdu_buffer;
        }
        else
        {
            *pdu_buffer_p = NULL;
        }
    }
    if (channel_p)
    {
        /*
         * note that for channels, many CF app functions assume
         * that when channel is passed as a pointer, that it is a member
         * of the array within CF_AppData, and the channel number can
         * be obtained by pointer arithmetic.
         * this arithmetic will break if the pointer is not actually
         * a member of that array, so for now it must be so.
         * This always uses the same channel for now.
         */
        *channel_p = &CF_AppData.engine.channels[UT_CFDP_CHANNEL];
    }
    if (history_p)
    {
        *history_p = &ut_history;
    }
    if (txn_p)
    {
        *txn_p = &ut_transaction;
    }
    if (config_table_p)
    {
        *config_table_p = &ut_config_table;
    }

    if (setup == UT_CF_Setup_TX)
    {
        UT_CFDP_SetupBasicTxState(&ut_pdu_buffer);

        /* make sure that a potential call to CF_CFDP_MsgOutGet() returns this buffer */
        UT_SetHandlerFunction(UT_KEY(CF_CFDP_MsgOutGet), UT_AltHandler_GenericPointerReturn, &ut_pdu_buffer);
    }
    else if (setup == UT_CF_Setup_RX)
    {
        UT_CFDP_SetupBasicRxState(&ut_pdu_buffer);
    }

    /* reset the event ID capture between each sub-case */
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
}

/* end cf_cfdp_tests local utility functions */

/*******************************************************************************
**
**  cf_cfdp_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_tests_Setup(void)
{
    cf_tests_Setup();

    /*
     * Also clear the app global. No test case should depend on data
     * previously left in here.
     */
    memset(&CF_AppData, 0, sizeof(CF_AppData));

} /* end cf_cfdp_tests_Setup */

void cf_cfdp_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_tests_Teardown */

/* end cf_cfdp_tests Setup and Teardown */

/*******************************************************************************
**
**  cf_cfdp_tests Implementation-specific tests
**
*******************************************************************************/

void Test_CF_CFDP_CF_CFDP_EncodeStart(void)
{
    /* Test case for:
     * void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, CFE_SB_Buffer_t *msgbuf, CF_Logical_PduBuffer_t *ph,
     *                size_t msgbuf_size)
     */
    CF_EncoderState_t enc;
    struct
    {
        uint32 hdr;
        uint32 data;
    } msg;
    CF_Logical_PduBuffer_t pdubuf;

    memset(&msg, 0xEE, sizeof(msg));

    /* nominal */
    /* This should deduct for the size of the SB header, but
     * do not want to hardcode the specific number here */

    CF_CFDP_EncodeStart(&enc, &msg, &pdubuf, sizeof(msg.hdr), sizeof(msg));
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&enc));
    UtAssert_UINT32_LT(CF_CODEC_GET_REMAIN(&enc), sizeof(msg));
    UtAssert_ZERO(CF_CODEC_GET_POSITION(&enc));
    UtAssert_UINT32_EQ(CF_CODEC_GET_SIZE(&enc), CF_CODEC_GET_REMAIN(&enc));

    /* too small */
    CF_CFDP_EncodeStart(&enc, &msg, &pdubuf, sizeof(msg.hdr), sizeof(msg.hdr) - 1);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&enc));
}

void Test_CF_CFDP_CF_CFDP_DecodeStart(void)
{
    /* Test case for:
     * void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, CFE_SB_Buffer_t *msgbuf, CF_Logical_PduBuffer_t *ph,
     *                size_t msgbuf_size)
     */
    CF_DecoderState_t dec;
    struct
    {
        uint32 hdr;
        uint32 data;
    } msg;
    CF_Logical_PduBuffer_t pdubuf;

    memset(&msg, 0xEE, sizeof(msg));

    /* nominal */
    /* This should deduct for the size of the SB header, but
     * do not want to hardcode the specific number here */

    CF_CFDP_DecodeStart(&dec, &msg, &pdubuf, sizeof(msg.hdr), sizeof(msg));
    UtAssert_BOOL_TRUE(CF_CODEC_IS_OK(&dec));
    UtAssert_UINT32_LT(CF_CODEC_GET_REMAIN(&dec), sizeof(msg));
    UtAssert_ZERO(CF_CODEC_GET_POSITION(&dec));
    UtAssert_UINT32_EQ(CF_CODEC_GET_SIZE(&dec), CF_CODEC_GET_REMAIN(&dec));

    /* too small */
    CF_CFDP_DecodeStart(&dec, &msg, &pdubuf, sizeof(msg.hdr), sizeof(msg.hdr) - 1);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(&dec));
}

void Test_CF_CFDP_ArmAckTimer(void)
{
    /* Test case for:
     * void CF_CFDP_ArmAckTimer(CF_Transaction_t *t)
     */
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, &config);

    /* nominal call */
    UtAssert_VOIDCALL(CF_CFDP_ArmAckTimer(t));
}

void Test_CF_CFDP_RecvPh(void)
{
    /* Test case for:
     * int CF_CFDP_RecvPh(uint8 chan_num, const CFE_SB_Buffer_t *msgbuf, CF_Logical_PduBuffer_t **pph)
     */
    CF_Logical_PduBuffer_t *ph;

    /* nominal, file directive */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, NULL, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvPh(UT_CFDP_CHANNEL, ph), 0);

    /* nominal, file data */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, NULL, NULL);
    ph->pdu_header.pdu_type = 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvPh(UT_CFDP_CHANNEL, ph), 0);

    /* decode error, fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, NULL, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvPh(UT_CFDP_CHANNEL, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_SHORT_HEADER);

    /* decode error, large file bit set */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, NULL, NULL);
    ph->pdu_header.large_flag = true;
    UtAssert_INT32_EQ(CF_CFDP_RecvPh(UT_CFDP_CHANNEL, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_LARGE_FILE);
}

void Test_CF_CFDP_RecvMd(void)
{
    /* Test case for:
     * int CF_CFDP_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t       *t;
    CF_History_t           *h;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduMd_t     *md;
    const char              src[]  = "mds";
    const char              dest[] = "mdd";

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &h, &t, NULL);
    md                           = &ph->int_header.md;
    md->size                     = 10;
    md->dest_filename.length     = sizeof(dest) - 1;
    md->dest_filename.data_ptr   = dest;
    md->source_filename.length   = sizeof(src) - 1;
    md->source_filename.data_ptr = src;
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(t, ph), 0);
    UtAssert_UINT32_EQ(t->fsize, md->size);
    UtAssert_STRINGBUF_EQ(md->dest_filename.data_ptr, md->dest_filename.length, h->fnames.dst_filename,
                          sizeof(h->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(md->source_filename.data_ptr, md->source_filename.length, h->fnames.src_filename,
                          sizeof(h->fnames.src_filename));

    /* deode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_MD_SHORT);

    /* decode errors: LV dest filename too long */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    md                       = &ph->int_header.md;
    md->dest_filename.length = CF_FILENAME_MAX_LEN + 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_INVALID_DST_LEN);

    /* decode errors: LV source filename too long */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    md                         = &ph->int_header.md;
    md->source_filename.length = CF_FILENAME_MAX_LEN + 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_INVALID_SRC_LEN);
}

void Test_CF_CFDP_RecvFd(void)
{
    /* Test case for:
     * int CF_CFDP_RecvFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call, no crc */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(t, ph), 0);

    /* nominal call, with crc */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.crc_flag    = 1;
    ph->int_header.fd.data_len = 10 + sizeof(CF_CFDP_uint32_t);
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(t, ph), 0);
    UtAssert_UINT32_EQ(ph->int_header.fd.data_len, 10);

    /* deode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_FD_SHORT);

    /* deode errors: crc part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.crc_flag    = 1;
    ph->int_header.fd.data_len = sizeof(CF_CFDP_uint32_t) - 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(t, ph), -1);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(ph->pdec));

    /* with segment metadata (unimplemented) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    ph->pdu_header.segment_meta_flag = 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_FD_UNSUPPORTED);
}

void Test_CF_CFDP_RecvEof(void)
{
    /* Test case for:
     * int CF_CFDP_RecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
     */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvEof(t, ph), 0);

    /* deode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvEof(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_EOF_SHORT);
}

void Test_CF_CFDP_RecvAck(void)
{
    /* Test case for:
     * int CF_CFDP_RecvAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvAck(t, ph), 0);

    /* deode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvAck(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_ACK_SHORT);
}

void Test_CF_CFDP_RecvFin(void)
{
    /* Test case for:
     * int CF_CFDP_RecvFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
     */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvFin(t, ph), 0);

    /* deode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvFin(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_FIN_SHORT);
}

void Test_CF_CFDP_RecvNak(void)
{
    /* Test case for:
     * int CF_CFDP_RecvNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
     */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvNak(t, ph), 0);

    /* deode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvNak(t, ph), -1);
    UT_CF_AssertEventID(CF_EID_ERR_PDU_NAK_SHORT);
}

void Test_CF_CFDP_RecvDrop(void)
{
    /* Test case for:
     * void CF_CFDP_RecvDrop(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &t, NULL);
    UtAssert_VOIDCALL(CF_CFDP_RecvDrop(t, ph));
}
void Test_CF_CFDP_RecvIdle(void)
{
    /* Test case for:
     * void CF_CFDP_RecvIdle(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_History_t           *h;
    CF_Logical_PduBuffer_t *ph;
    CF_ChunkWrapper_t       ut_unused_chunks;

    /* setup for FindUnusedChunks */
    memset(&ut_unused_chunks, 0, sizeof(ut_unused_chunks));
    CF_AppData.engine.channels[UT_CFDP_CHANNEL].cs[CF_Direction_RX] = &ut_unused_chunks.cl_node;
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].q_size[0]             = 4;
    UT_SetHandlerFunction(UT_KEY(CF_CList_Pop), UT_AltHandler_GenericPointerReturn, &ut_unused_chunks.cl_node);

    /* nominal call, file data, class 1 */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &h, &t, NULL);
    ph->pdu_header.pdu_type = 1; /* follow file data path */
    ph->pdu_header.txm_mode = 1; /* class 1 */
    UtAssert_VOIDCALL(CF_CFDP_RecvIdle(t, ph));
    UtAssert_INT32_EQ(t->state, CF_TxnState_DROP);

    /* nominal call, file data, class 2 */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &h, &t, NULL);
    ph->pdu_header.pdu_type = 1; /* follow file data path */
    ph->pdu_header.txm_mode = 0; /* class 2 */
    UtAssert_VOIDCALL(CF_CFDP_RecvIdle(t, ph));
    UtAssert_INT32_EQ(t->state, CF_TxnState_R2);

    /* nominal call, file metadata, class 1 */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &h, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_METADATA;
    ph->pdu_header.txm_mode       = 1; /* class 1 */
    UtAssert_VOIDCALL(CF_CFDP_RecvIdle(t, ph));
    UtAssert_INT32_EQ(t->state, CF_TxnState_R1);

    /* nominal call, file metadata, class 2 */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &h, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_METADATA;
    UtAssert_VOIDCALL(CF_CFDP_RecvIdle(t, ph));
    UtAssert_INT32_EQ(t->state, CF_TxnState_R2);

    /* decode error in RecvMd */
    /* This will proceed to call CF_CFDP_ResetTransaction() which needs
     * the q_size to be nonzero */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &h, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_METADATA;
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_VOIDCALL(CF_CFDP_RecvIdle(t, ph));
    UtAssert_INT32_EQ(t->state, CF_TxnState_IDLE);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_IDLE_MD);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &h, &t, NULL);
    ph->fdirective.directive_code = CF_CFDP_FileDirective_INVALID_MIN;
    UtAssert_VOIDCALL(CF_CFDP_RecvIdle(t, ph));
    UtAssert_INT32_EQ(t->state, CF_TxnState_IDLE);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_FD_UNHANDLED);
}

void Test_CF_CFDP_CopyStringFromLV(void)
{
    /* Test case for:
     * int CF_CFDP_CopyStringFromLV(char *buf, size_t buf_maxsz, const CF_Logical_Lv_t *src_lv)
     */
    char            buf[20];
    const char      refstr[] = "refstr";
    CF_Logical_Lv_t input;

    input.data_ptr = refstr;
    input.length   = sizeof(refstr) - 1;

    /* nominal call */
    UtAssert_INT32_EQ(CF_CFDP_CopyStringFromLV(buf, sizeof(buf), &input), input.length);
}

void Test_CF_CFDP_ConstructPduHeader(void)
{
    /* Test case for:
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *t, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent);
*/
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduHeader_t *hdr;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_NULL(CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_ACK, 3, 2, true, 42, false));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_NULL(CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_ACK, 3, 2, true, 42, true));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_S1;
    UtAssert_NOT_NULL(CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_ACK, 3, 2, true, 42, false));
    hdr = &ph->pdu_header;
    UtAssert_UINT32_EQ(hdr->version, 1);
    UtAssert_UINT32_EQ(hdr->pdu_type, 0);
    UtAssert_UINT32_EQ(hdr->direction, 1);
    UtAssert_UINT32_EQ(hdr->txm_mode, 1);
    UtAssert_UINT32_EQ(hdr->eid_length, 1);
    UtAssert_UINT32_EQ(hdr->txn_seq_length, 1);
    UtAssert_UINT32_EQ(hdr->source_eid, 3);
    UtAssert_UINT32_EQ(hdr->destination_eid, 2);
    UtAssert_UINT32_EQ(hdr->sequence_num, 42);
    UtAssert_UINT32_EQ(ph->fdirective.directive_code, CF_CFDP_FileDirective_ACK);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_GetValueEncodedSize), 5);
    t->state = CF_TxnState_S2;
    UtAssert_NOT_NULL(CF_CFDP_ConstructPduHeader(t, 0, 7, 6, false, 44, false));
    hdr = &ph->pdu_header;
    UtAssert_UINT32_EQ(hdr->version, 1);
    UtAssert_UINT32_EQ(hdr->pdu_type, 1);
    UtAssert_UINT32_EQ(hdr->direction, 0);
    UtAssert_UINT32_EQ(hdr->txm_mode, 0);
    UtAssert_UINT32_EQ(hdr->eid_length, 5);
    UtAssert_UINT32_EQ(hdr->txn_seq_length, 5);
    UtAssert_UINT32_EQ(hdr->source_eid, 7);
    UtAssert_UINT32_EQ(hdr->destination_eid, 6);
    UtAssert_UINT32_EQ(hdr->sequence_num, 44);
}

void Test_CF_CFDP_SendMd(void)
{
    /* Test case for:
        CF_SendRet_t     CF_CFDP_SendMd(CF_Transaction_t *t);
     */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_History_t           *h;
    CF_Logical_PduMd_t     *md;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendMd(t), CF_SendRet_NO_MSG);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, &h, &t, NULL);
    md = &ph->int_header.md;
    strncpy(h->fnames.dst_filename, "dst1", sizeof(h->fnames.dst_filename));
    strncpy(h->fnames.src_filename, "src1", sizeof(h->fnames.src_filename));
    t->state = CF_TxnState_S1;
    t->fsize = 1234;
    UtAssert_INT32_EQ(CF_CFDP_SendMd(t), CF_SendRet_SUCCESS);
    UtAssert_UINT32_EQ(md->size, t->fsize);
    UtAssert_STRINGBUF_EQ(md->dest_filename.data_ptr, md->dest_filename.length, h->fnames.dst_filename,
                          sizeof(h->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(md->source_filename.data_ptr, md->source_filename.length, h->fnames.src_filename,
                          sizeof(h->fnames.src_filename));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, &h, &t, NULL);
    md = &ph->int_header.md;
    strncpy(h->fnames.dst_filename, "dst2", sizeof(h->fnames.dst_filename));
    strncpy(h->fnames.src_filename, "src2", sizeof(h->fnames.src_filename));
    t->state = CF_TxnState_S2;
    t->fsize = 5678;
    UtAssert_INT32_EQ(CF_CFDP_SendMd(t), CF_SendRet_SUCCESS);
    UtAssert_UINT32_EQ(md->size, t->fsize);
    UtAssert_STRINGBUF_EQ(md->dest_filename.data_ptr, md->dest_filename.length, h->fnames.dst_filename,
                          sizeof(h->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(md->source_filename.data_ptr, md->source_filename.length, h->fnames.src_filename,
                          sizeof(h->fnames.src_filename));
}
void Test_CF_CFDP_SendFd(void)
{
    /* Test case for:
        CF_SendRet_t     CF_CFDP_SendFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
    */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendFd(t, ph), CF_SendRet_SUCCESS);
}

void Test_CF_CFDP_SendEof(void)
{
    /* Test case for:
        CF_SendRet_t     CF_CFDP_SendEof(CF_Transaction_t *t);
     */

    CF_Transaction_t       *t;
    CF_History_t           *h;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduEof_t    *eof;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendEof(t), CF_SendRet_NO_MSG);

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    eof = &ph->int_header.eof;
    UtAssert_INT32_EQ(CF_CFDP_SendEof(t), CF_SendRet_SUCCESS);
    UtAssert_ZERO(eof->tlv_list.num_tlv);

    /* test with an alternate condition code, which should append a TLV */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, &h, &t, NULL);
    eof   = &ph->int_header.eof;
    h->cc = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
    UtAssert_INT32_EQ(CF_CFDP_SendEof(t), CF_SendRet_SUCCESS);
    UtAssert_UINT32_EQ(eof->tlv_list.num_tlv, 1);
    UtAssert_STUB_COUNT(CF_CFDP_Send, 2);
}

void Test_CF_CFDP_SendAck(void)
{
    /* Test case for:
        CF_SendRet_t CF_CFDP_SendAck(CF_Transaction_t *t, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                                    CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn);
     */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduAck_t    *ack;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendAck(t, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF,
                                      CF_CFDP_ConditionCode_NO_ERROR, 1, 42),
                      CF_SendRet_NO_MSG);

    /* nominal as receiver */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    ack      = &ph->int_header.ack;
    t->state = CF_TxnState_R2;
    UtAssert_INT32_EQ(CF_CFDP_SendAck(t, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF,
                                      CF_CFDP_ConditionCode_NO_ERROR, 1, 42),
                      CF_SendRet_SUCCESS);
    UtAssert_UINT32_EQ(ack->ack_directive_code, CF_CFDP_FileDirective_EOF);
    UtAssert_UINT32_EQ(ack->ack_subtype_code, 1);
    UtAssert_UINT32_EQ(ack->txn_status, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_UINT32_EQ(ack->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* nominal as sender */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    ack      = &ph->int_header.ack;
    t->state = CF_TxnState_S2;
    UtAssert_INT32_EQ(CF_CFDP_SendAck(t, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF,
                                      CF_CFDP_ConditionCode_NO_ERROR, 1, 42),
                      CF_SendRet_SUCCESS);
    UtAssert_UINT32_EQ(ack->ack_directive_code, CF_CFDP_FileDirective_EOF);
    UtAssert_UINT32_EQ(ack->ack_subtype_code, 1);
    UtAssert_UINT32_EQ(ack->txn_status, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_UINT32_EQ(ack->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* still success path but with non-nominal values */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    ack      = &ph->int_header.ack;
    t->state = CF_TxnState_R2;
    UtAssert_INT32_EQ(CF_CFDP_SendAck(t, CF_CFDP_AckTxnStatus_TERMINATED, CF_CFDP_FileDirective_FIN,
                                      CF_CFDP_ConditionCode_FILESTORE_REJECTION, 1, 42),
                      CF_SendRet_SUCCESS);
    UtAssert_UINT32_EQ(ack->ack_directive_code, CF_CFDP_FileDirective_FIN);
    UtAssert_UINT32_EQ(ack->ack_subtype_code, 1);
    UtAssert_UINT32_EQ(ack->txn_status, CF_CFDP_AckTxnStatus_TERMINATED);
    UtAssert_UINT32_EQ(ack->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
}

void Test_CF_CFDP_SendFin(void)
{
    /* Test case for:
        CF_SendRet_t CF_CFDP_SendFin(CF_Transaction_t *t, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                                    CF_CFDP_ConditionCode_t cc);
     */

    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduFin_t    *fin;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendFin(t, CF_CFDP_FinDeliveryCode_COMPLETE, CF_CFDP_FinFileStatus_RETAINED,
                                      CF_CFDP_ConditionCode_NO_ERROR),
                      CF_SendRet_NO_MSG);

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    fin = &ph->int_header.fin;
    UtAssert_INT32_EQ(CF_CFDP_SendFin(t, CF_CFDP_FinDeliveryCode_COMPLETE, CF_CFDP_FinFileStatus_RETAINED,
                                      CF_CFDP_ConditionCode_NO_ERROR),
                      CF_SendRet_SUCCESS);
    UtAssert_ZERO(fin->tlv_list.num_tlv);
    UtAssert_UINT32_EQ(fin->delivery_code, CF_CFDP_FinDeliveryCode_COMPLETE);
    UtAssert_UINT32_EQ(fin->file_status, CF_CFDP_FinFileStatus_RETAINED);
    UtAssert_UINT32_EQ(fin->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* test with an alternate condition code, which should append a TLV */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    fin = &ph->int_header.fin;
    UtAssert_INT32_EQ(CF_CFDP_SendFin(t, CF_CFDP_FinDeliveryCode_INCOMPLETE, CF_CFDP_FinFileStatus_DISCARDED,
                                      CF_CFDP_ConditionCode_FILESTORE_REJECTION),
                      CF_SendRet_SUCCESS);
    UtAssert_UINT32_EQ(fin->delivery_code, CF_CFDP_FinDeliveryCode_INCOMPLETE);
    UtAssert_UINT32_EQ(fin->file_status, CF_CFDP_FinFileStatus_DISCARDED);
    UtAssert_UINT32_EQ(fin->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    UtAssert_UINT32_EQ(fin->tlv_list.num_tlv, 1);
    UtAssert_STUB_COUNT(CF_CFDP_Send, 2);
}

void Test_CF_CFDP_SendNak(void)
{
    /* Test case for:
        CF_SendRet_t CF_CFDP_SendNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendNak(t, ph), CF_SendRet_NO_MSG);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_S2;
    UtAssert_INT32_EQ(CF_CFDP_SendNak(t, ph), CF_SendRet_SUCCESS);

    UtAssert_STUB_COUNT(CF_CFDP_Send, 1);
}

void Test_CF_CFDP_AppendTlv(void)
{
    /* Test case for:
        void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type)

        This helper is used by EOF and FIN to set TLV values
    */
    CF_Logical_PduBuffer_t *ph;
    CF_ConfigTable_t       *config;
    CF_Logical_TlvList_t   *tlv_list;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, NULL, &config);
    config->local_eid = 123;
    tlv_list          = &ph->int_header.eof.tlv_list;

    UtAssert_VOIDCALL(CF_CFDP_AppendTlv(tlv_list, 1));
    UtAssert_NULL(tlv_list->tlv[0].data.data_ptr);
    UtAssert_UINT32_EQ(tlv_list->tlv[0].length, 0);
    UtAssert_UINT32_EQ(tlv_list->num_tlv, 1);

    UtAssert_VOIDCALL(CF_CFDP_AppendTlv(tlv_list, CF_CFDP_TLV_TYPE_ENTITY_ID));
    UtAssert_UINT32_EQ(tlv_list->tlv[1].data.eid, config->local_eid);
    UtAssert_UINT32_EQ(tlv_list->num_tlv, 2);

    /* call w/max should be no-op */
    tlv_list->num_tlv = CF_PDU_MAX_TLV;
    UtAssert_VOIDCALL(CF_CFDP_AppendTlv(tlv_list, 1));
    UtAssert_UINT32_EQ(tlv_list->num_tlv, CF_PDU_MAX_TLV);
}

void Test_CF_CFDP_FindUnusedTransaction(void)
{
    /* Test case for:
        CF_Transaction_t *CF_CFDP_FindUnusedTransaction(CF_Channel_t *c)
     */
    CF_Transaction_t       *t;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, &ph, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendNak(t, ph), CF_SendRet_NO_MSG);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &t, NULL);
    t->state = CF_TxnState_S2;
    UtAssert_INT32_EQ(CF_CFDP_SendNak(t, ph), CF_SendRet_SUCCESS);

    UtAssert_STUB_COUNT(CF_CFDP_Send, 1);
}

void Test_CF_CFDP_InitEngine(void)
{
    /* Test case for:
     * int32 CF_CFDP_InitEngine(void)
     */
    CF_ConfigTable_t *config;

    /* nominal call, no sem */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), 0);
    UtAssert_BOOL_TRUE(CF_AppData.engine.enabled);

    /* nominal call, with sem */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    config->chan[0].sem_name[0] = 'u';
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), 0);
    UtAssert_BOOL_TRUE(CF_AppData.engine.enabled);

    /* failure of OS_CountSemGetIdByName */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    config->chan[0].sem_name[0] = 'u';
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemGetIdByName), OS_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), OS_ERROR);
    UtAssert_BOOL_FALSE(CF_AppData.engine.enabled);

    /* failure of CFE_SB_CreatePipe */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_BOOL_FALSE(CF_AppData.engine.enabled);

    /* failure of CFE_SB_SubscribeLocal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_SubscribeLocal), 1, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_BOOL_FALSE(CF_AppData.engine.enabled);
}

void Test_CF_CFDP_TxFile(void)
{
    /* Test case for:
     * int32 CF_CFDP_TxFile(const char *src_filename, const char *dst_filename,
                            CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, CF_EntityId_t dest_id);

     */
    const char        src[]  = "tsrc";
    const char        dest[] = "tdest";
    CF_History_t     *h;
    CF_Transaction_t *t;
    CF_Channel_t     *c;
    CF_ChunkWrapper_t chunk_wrap;

    memset(&chunk_wrap, 0, sizeof(chunk_wrap));

    /* nominal call */
    /* make sure call to CF_FindUnusedTransaction() returns this buffer */
    /* Also need to set up for call to CF_CFDP_FindUnusedChunks which calls CF_CList_Pop */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, &h, &t, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, t);
    UT_SetHandlerFunction(UT_KEY(CF_CList_Pop), UT_AltHandler_GenericPointerReturn, &chunk_wrap.cl_node);
    c->cs[CF_Direction_TX] = &chunk_wrap.cl_node;
    UtAssert_INT32_EQ(CF_CFDP_TxFile(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), 0);
    UtAssert_STRINGBUF_EQ(dest, -1, h->fnames.dst_filename, sizeof(h->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(src, -1, h->fnames.src_filename, sizeof(h->fnames.src_filename));
    UtAssert_UINT32_EQ(c->num_cmd_tx, 1);
    UT_CF_AssertEventID(CF_EID_INF_CFDP_S_START_SEND);

    /* same but for class 2 (for branch coverage) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, &h, &t, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, t);
    UT_SetHandlerFunction(UT_KEY(CF_CList_Pop), UT_AltHandler_GenericPointerReturn, &chunk_wrap.cl_node);
    c->cs[CF_Direction_TX] = &chunk_wrap.cl_node;
    UtAssert_INT32_EQ(CF_CFDP_TxFile(src, dest, CF_CFDP_CLASS_2, 1, UT_CFDP_CHANNEL, 0, 1), 0);
    UtAssert_STRINGBUF_EQ(dest, -1, h->fnames.dst_filename, sizeof(h->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(src, -1, h->fnames.src_filename, sizeof(h->fnames.src_filename));
    UtAssert_UINT32_EQ(c->num_cmd_tx, 2);
    UT_CF_AssertEventID(CF_EID_INF_CFDP_S_START_SEND);

    /* max TX */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, &h, &t, NULL);
    c->num_cmd_tx = CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN;
    UtAssert_INT32_EQ(CF_CFDP_TxFile(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), -1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_MAX_CMD_TX);
}

void Test_CF_CFDP_PlaybackDir(void)
{
    /* Test case for:
     * int32 CF_CFDP_PlaybackDir(const char *src_filename,
                                 const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                                 uint8 chan, uint8 priority, uint16 dest_id);
     */
    const char     src[]  = "psrc";
    const char     dest[] = "pdest";
    CF_Playback_t *pb;
    CF_Channel_t  *c;
    uint8          i;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &c, NULL, NULL, NULL);
    pb = &c->playback[0];
    memset(pb, 0, sizeof(*pb));
    UtAssert_INT32_EQ(CF_CFDP_PlaybackDir(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), 0);
    UtAssert_STRINGBUF_EQ(dest, -1, pb->fnames.dst_filename, sizeof(pb->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(src, -1, pb->fnames.src_filename, sizeof(pb->fnames.src_filename));
    UtAssert_BOOL_TRUE(pb->diropen);
    UtAssert_BOOL_TRUE(pb->busy);

    /* OS_DirectoryOpen fail */
    memset(pb, 0, sizeof(*pb));
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryOpen), 1, OS_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_PlaybackDir(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), -1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_OPENDIR);

    /* no non-busy entries */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &c, NULL, NULL, NULL);
    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        pb       = &c->playback[i];
        pb->busy = 1;
    }
    UtAssert_INT32_EQ(CF_CFDP_PlaybackDir(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), -1);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_DIR_SLOT);
}

static int32 Ut_Hook_CycleTx_SetRanOne(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                       const UT_StubContext_t *Context)
{
    CF_CFDP_CycleTx_args_t *args = UT_Hook_GetArgValueByName(Context, "context", CF_CFDP_CycleTx_args_t *);

    if (CallCount == 1)
    {
        args->ran_one = 1;
    }

    return StubRetcode;
}

void Test_CF_CFDP_CycleTx(void)
{
    /* Test case for:
     * void CF_CFDP_CycleTx(CF_Channel_t *c)
     */
    CF_Channel_t     *c;
    CF_Transaction_t *t;
    CF_ConfigTable_t *config;
    CF_Transaction_t  t2;

    memset(&t2, 0, sizeof(t2));

    /* need to set dequeue_enabled so it enters the actual logic */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, NULL, &t, &config);
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].q_size[0] = 10;
    CF_AppData.engine.enabled                           = 1;
    config->chan[UT_CFDP_CHANNEL].dequeue_enabled       = 1;

    /* nominal call, w/c->cur non-null */
    c->cur = t;
    UtAssert_VOIDCALL(CF_CFDP_CycleTx(c));
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);

    /* nominal call, w/c->cur null, but queue empty */
    UtAssert_VOIDCALL(CF_CFDP_CycleTx(c));
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);

    /* nominal call, w/c->cur null, queue not empty */
    UT_ResetState(UT_KEY(CF_CList_Traverse));
    UT_SetHookFunction(UT_KEY(CF_CList_Traverse), Ut_Hook_CycleTx_SetRanOne, false);
    c->qs[CF_QueueIdx_PEND] = &t2.cl_node;
    UtAssert_VOIDCALL(CF_CFDP_CycleTx(c));
    UtAssert_STUB_COUNT(CF_CList_Traverse, 2);
}

static int32 Ut_Hook_StateHandler_SetQIndex(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                            const UT_StubContext_t *Context)
{
    CF_Transaction_t *t  = UT_Hook_GetArgValueByName(Context, "t", CF_Transaction_t *);
    t->flags.com.q_index = 0;
    return StubRetcode;
}

void Test_CF_CFDP_CycleTxFirstActive(void)
{
    /* Test case for:
       int CF_CFDP_CycleTxFirstActive(CF_CListNode_t *node, void *context);
     */
    CF_CFDP_CycleTx_args_t args;
    CF_Transaction_t      *t;

    memset(&args, 0, sizeof(args));

    /* suspended, should return 0 */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.com.suspended = 1;
    UtAssert_INT32_EQ(CF_CFDP_CycleTxFirstActive(&t->cl_node, &args), 0);

    /* nominal, with c->cur set non-null, should skip loop and return 1 */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.c, NULL, &t, NULL);
    t->flags.com.q_index = CF_QueueIdx_TXA; /* must be this */
    args.c->cur          = t;
    UtAssert_INT32_EQ(CF_CFDP_CycleTxFirstActive(&t->cl_node, &args), 1);
    UtAssert_BOOL_TRUE(args.ran_one);

    /* nominal, with c->cur set null, should do loop and return 1 */
    /* will call the handler for this state, which is a stub */
    /* need to use a hook function or else this is infinite loop */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->state             = CF_TxnState_S1;
    t->flags.com.q_index = CF_QueueIdx_TXA; /* must be this */
    args.c->cur          = NULL;
    UT_SetHookFunction(UT_KEY(CF_CFDP_TxStateDispatch), Ut_Hook_StateHandler_SetQIndex, NULL);
    UtAssert_INT32_EQ(CF_CFDP_CycleTxFirstActive(&t->cl_node, &args), 1);
}

static void DoTickFnClearCont(CF_Transaction_t *t, int *cont)
{
    *cont = 0;
}

static void DoTickFnSetCur(CF_Transaction_t *t, int *cont)
{
    CF_AppData.engine.channels[t->chan_num].cur = t;
}

void Test_CF_CFDP_DoTick(void)
{
    /* Test case for:
     * int CF_CFDP_DoTick(CF_CListNode_t *node, void *context);
     */
    CF_Transaction_t   *t;
    CF_Transaction_t    t2;
    CF_CFDP_Tick_args_t args;

    memset(&args, 0, sizeof(args));
    memset(&t2, 0, sizeof(t2));
    args.fn = DoTickFnClearCont;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.c, NULL, &t, NULL);
    args.c->cur = &t2;
    args.cont   = true;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&t->cl_node, &args), CF_CLIST_CONT);
    UtAssert_BOOL_TRUE(args.cont);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.c, NULL, &t, NULL);
    args.c->cur = t;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&t->cl_node, &args), CF_CLIST_CONT);
    UtAssert_BOOL_FALSE(args.cont);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.c, NULL, &t, NULL);
    t->flags.com.suspended = 1;
    args.cont              = true;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&t->cl_node, &args), CF_CLIST_CONT);
    UtAssert_BOOL_TRUE(args.cont);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.c, NULL, &t, NULL);
    args.fn = DoTickFnSetCur;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&t->cl_node, &args), CF_CLIST_EXIT);
    UtAssert_BOOL_TRUE(args.early_exit);
}

void Test_CF_CFDP_ProcessPollingDirectories(void)
{
    /* Test case for:
     * void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *c)
     */
    CF_Channel_t     *c;
    CF_ConfigTable_t *config;
    CF_PollDir_t     *pdcfg;
    CF_Poll_t        *poll;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, NULL, NULL, &config);
    pdcfg = &config->chan[UT_CFDP_CHANNEL].polldir[0];
    poll  = &c->poll[0];

    /* nominal call, w/engine disabled (noop) */
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].poll_counter, 0);

    /* nominal call, w/engine enabled, polldir enabled but interval_sec == 0 */
    CF_AppData.engine.enabled = 1;
    pdcfg->enabled            = 1;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].poll_counter, 0);

    /* with interval_sec nonzero the timer should get set, but not tick */
    pdcfg->interval_sec = 1;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_BOOL_TRUE(poll->timer_set);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].poll_counter, 1);

    /* call again should tick */
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_BOOL_TRUE(poll->timer_set);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* call again timer should expire and start a playback */
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, true);
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_BOOL_FALSE(poll->timer_set);
    UtAssert_BOOL_TRUE(poll->pb.busy);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].poll_counter, 1);

    /* make an error occur in CF_CFDP_PlaybackDir_Initiate() */
    poll->pb.busy   = false; /* above would have set it true */
    poll->timer_set = true;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, true);
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryOpen), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_BOOL_TRUE(poll->timer_set);
    UT_CF_AssertEventID(CF_EID_ERR_CFDP_OPENDIR);

    /* Test case where the impl calls through to CF_CFDP_ProcessPlaybackDirectory()
     *
     * NOTE: with diropen set false, this will not attempt
     * to start a new transaction here (that case is covered in another test)
     */
    poll->pb.busy    = false;
    poll->pb.diropen = false;
    poll->pb.num_ts  = 1;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_BOOL_FALSE(poll->pb.busy);

    poll->pb.busy   = true;
    poll->pb.num_ts = 0;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_BOOL_FALSE(poll->pb.busy); /* because num_ts == 0 */

    /* test that call to CF_CFDP_UpdatePollPbCounted will decrement back to 0 again */
    pdcfg->enabled = 0;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(c));
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].poll_counter, 0);
}

void Test_CF_CFDP_ProcessPlaybackDirectory(void)
{
    /* Test case for:
     * void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *c, CF_Playback_t *p)
     */
    CF_Transaction_t *t;
    CF_History_t     *h;
    CF_Channel_t     *c;
    CF_ConfigTable_t *config;
    CF_Playback_t     pb;
    os_dirent_t       dirent[3];
    CF_ChunkWrapper_t chunk_wrap;

    memset(&chunk_wrap, 0, sizeof(chunk_wrap));
    memset(&pb, 0, sizeof(pb));
    memset(dirent, 0, sizeof(dirent));
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &c, &h, &t, &config);
    CF_AppData.engine.enabled = 1;

    /* diropen is true but num_ts is high so operations are restricted */
    pb.busy    = 1;
    pb.num_ts  = CF_NUM_TRANSACTIONS_PER_PLAYBACK + 1;
    pb.diropen = true;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPlaybackDirectory(c, &pb));
    UtAssert_BOOL_TRUE(pb.busy);
    UtAssert_BOOL_TRUE(pb.diropen);

    /*
     * enter the loop, but error calling OS_DirectoryRead().
     * This should end up calling OS_DirectoryClose().
     */
    pb.busy    = 1;
    pb.diropen = true;
    pb.num_ts  = 0;
    OS_DirectoryOpen(&pb.dir_id, "ut");
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_ProcessPlaybackDirectory(c, &pb));
    UtAssert_STUB_COUNT(OS_DirectoryClose, 1);
    UtAssert_BOOL_FALSE(pb.busy);
    UtAssert_BOOL_FALSE(pb.diropen);

    /* nominal, but path is "." or ".." ...
     * this initially does not start a new transaction - those files are ignored.
     * note that this does a while loop here, so have to prepare all the entries at once.
     * After bypassing . and .. this will start a new transaction on pass 3.
     *  - this calls CF_FindUnusedTransaction() so that must return non-NULL.
     *  - this also calls CF_CFDP_FindUnusedChunks() and that pops an entry
     */
    pb.busy    = 1;
    pb.diropen = true;
    pb.num_ts  = 0;
    strcpy(dirent[0].FileName, ".");  /* ignored */
    strcpy(dirent[1].FileName, ".."); /* ignored */
    strcpy(dirent[2].FileName, "ut"); /* valid file */
    OS_DirectoryOpen(&pb.dir_id, "ut");
    UT_SetDataBuffer(UT_KEY(OS_DirectoryRead), dirent, sizeof(dirent), false);
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 4, OS_ERROR); /* end of dir */
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, t);
    UT_SetHandlerFunction(UT_KEY(CF_CList_Pop), UT_AltHandler_GenericPointerReturn, &chunk_wrap.cl_node);
    c->cs[CF_Direction_TX] = &chunk_wrap.cl_node;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPlaybackDirectory(c, &pb));
    UtAssert_BOOL_TRUE(pb.busy);
    UtAssert_BOOL_FALSE(pb.diropen);
    UtAssert_STRINGBUF_EQ(h->fnames.src_filename, sizeof(h->fnames.src_filename), "/ut", -1);
    UtAssert_STRINGBUF_EQ(h->fnames.dst_filename, sizeof(h->fnames.dst_filename), "/ut", -1);
    UT_CF_AssertEventID(CF_EID_INF_CFDP_S_START_SEND);
}

static int32 Ut_Hook_TickTransactions_SetEarlyExit(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                   const UT_StubContext_t *Context)
{
    CF_CFDP_Tick_args_t *args = UT_Hook_GetArgValueByName(Context, "context", CF_CFDP_Tick_args_t *);

    /* set flag on the second call */
    if ((CallCount & 1) == 1)
    {
        args->early_exit = 1;
    }

    return StubRetcode;
}

static int32 Ut_Hook_TickTransactions_SetCont(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                              const UT_StubContext_t *Context)
{
    CF_CFDP_Tick_args_t *args = UT_Hook_GetArgValueByName(Context, "context", CF_CFDP_Tick_args_t *);

    /* every other call do not set "cont" flag */
    if ((CallCount & 1) == 0)
    {
        args->cont = 1;
    }

    return StubRetcode;
}

void Test_CF_CFDP_TickTransactions(void)
{
    /* Test case for:
        void CF_CFDP_TickTransactions(CF_Channel_t *c);
     */

    CF_Channel_t *c;

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, NULL, NULL, NULL);
    UtAssert_VOIDCALL(CF_CFDP_TickTransactions(c));
    UtAssert_STUB_COUNT(CF_CList_Traverse, CF_TickType_NUM_TYPES);
    UtAssert_UINT32_EQ(c->tick_type, CF_TickType_RX);

    /* invoke "early exit" block via hook */
    /* The flag is set on the second call, so this should increment tick_type */
    UT_ResetState(UT_KEY(CF_CList_Traverse));
    UT_SetHookFunction(UT_KEY(CF_CList_Traverse), Ut_Hook_TickTransactions_SetEarlyExit, NULL);
    UtAssert_VOIDCALL(CF_CFDP_TickTransactions(c));
    UtAssert_UINT32_EQ(c->tick_type, CF_TickType_TXW_NORM);

    /* this should resume where it left from the last call,
     * and then reset the tick_type  */
    UtAssert_VOIDCALL(CF_CFDP_TickTransactions(c));
    UtAssert_UINT32_EQ(c->tick_type, CF_TickType_RX);

    UT_ResetState(UT_KEY(CF_CList_Traverse));
    UT_SetHookFunction(UT_KEY(CF_CList_Traverse), Ut_Hook_TickTransactions_SetCont, NULL);
    UtAssert_VOIDCALL(CF_CFDP_TickTransactions(c));
    UtAssert_UINT32_EQ(c->tick_type, CF_TickType_RX);
}

void Test_CF_CFDP_CycleEngine(void)
{
    /* Test case for:
     * void CF_CFDP_CycleEngine(void)
     */
    CF_Channel_t *c;

    /* nominal with engine disabled, noop */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &c, NULL, NULL, NULL);
    UtAssert_VOIDCALL(CF_CFDP_CycleEngine());

    /* enabled but frozen */
    CF_AppData.engine.enabled                        = 1;
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].frozen = 1;
    UtAssert_VOIDCALL(CF_CFDP_CycleEngine());

    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].frozen = 0;
    UtAssert_VOIDCALL(CF_CFDP_CycleEngine());
}

void Test_CF_CFDP_ResetTransaction(void)
{
    /* Test case for:
     * void CF_CFDP_ResetTransaction(CF_Transaction_t *t, int keep_history)
     */

    CF_Transaction_t *t;
    CF_History_t     *h;
    CF_Channel_t     *c;
    CF_Playback_t     pb;

    memset(&pb, 0, sizeof(pb));

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &t, NULL);
    CF_AppData.hk.channel_hk[UT_CFDP_CHANNEL].q_size[t->flags.com.q_index] = 10;
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 1));
    UtAssert_STUB_COUNT(CF_FreeTransaction, 1);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, &h, &t, NULL);
    t->fd    = OS_ObjectIdFromInteger(1);
    h->dir   = CF_Direction_TX;
    t->state = CF_TxnState_S1;
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 1));
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 0));
    UtAssert_STUB_COUNT(CF_FreeTransaction, 3);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, &h, &t, NULL);
    t->fd    = OS_ObjectIdFromInteger(1);
    h->dir   = CF_Direction_RX;
    t->state = CF_TxnState_R1;
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 1));
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 0));
    UtAssert_STUB_COUNT(CF_FreeTransaction, 5);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->fd    = OS_ObjectIdFromInteger(1);
    h->dir   = CF_Direction_TX;
    t->keep  = 1;
    t->state = CF_TxnState_S1;
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 1));
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 0));
    UtAssert_STUB_COUNT(CF_FreeTransaction, 7);

    /* coverage completeness:
     * test decrement of c->num_cmd_tx
     * test decrement of playback num_ts
     * test reset of "cur" pointer
     */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &c, &h, &t, NULL);
    pb.num_ts          = 10;
    t->p               = &pb;
    c->cur             = t;
    t->flags.tx.cmd_tx = 5;
    c->num_cmd_tx      = 8;
    h->dir             = CF_Direction_TX;
    t->state           = CF_TxnState_S1;
    UtAssert_VOIDCALL(CF_CFDP_ResetTransaction(t, 1));
    UtAssert_NULL(c->cur);
    UtAssert_UINT32_EQ(pb.num_ts, 9);
    UtAssert_UINT32_EQ(c->num_cmd_tx, 7);
    UtAssert_STUB_COUNT(CF_FreeTransaction, 8);
}

void Test_CF_CFDP_DisableEngine(void)
{
    /* Test case for:
     * void CF_CFDP_DisableEngine(void)
     */

    /* nominal call */
    CF_AppData.engine.enabled = 1;
    UtAssert_VOIDCALL(CF_CFDP_DisableEngine());
    UtAssert_STUB_COUNT(CFE_SB_DeletePipe, CF_NUM_CHANNELS);
    UtAssert_BOOL_FALSE(CF_AppData.engine.enabled);

    /* nominal call with playbacks and polls active */
    CF_AppData.engine.channels[UT_CFDP_CHANNEL].playback[0].busy = 1;
    OS_DirectoryOpen(&CF_AppData.engine.channels[UT_CFDP_CHANNEL].playback[0].dir_id, "ut");
    CF_AppData.engine.channels[UT_CFDP_CHANNEL].poll[0].pb.busy = 1;
    OS_DirectoryOpen(&CF_AppData.engine.channels[UT_CFDP_CHANNEL].poll[0].pb.dir_id, "ut");
    UtAssert_VOIDCALL(CF_CFDP_DisableEngine());
    UtAssert_STUB_COUNT(OS_DirectoryClose, 2);
}

void Test_CF_CFDP_CloseFiles(void)
{
    /* Test case for:
     * int CF_CFDP_CloseFiles(CF_CListNode_t *n, void *context)
     */
    CF_Transaction_t *t;

    /* nominal call, no file */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    UtAssert_INT32_EQ(CF_CFDP_CloseFiles(&t->cl_node, NULL), CF_CLIST_CONT);

    /* nominal call, w/ file */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->fd = OS_ObjectIdFromInteger(1);
    UtAssert_INT32_EQ(CF_CFDP_CloseFiles(&t->cl_node, NULL), CF_CLIST_CONT);
}

void Test_CF_CFDP_CancelTransaction(void)
{
    /* Test case for:
     * void CF_CFDP_CancelTransaction(CF_Transaction_t *t)
     */
    CF_Transaction_t *t;

    /* nominal; cover both "flags.com.canceled" branches in here */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.com.canceled = 1;
    UtAssert_VOIDCALL(CF_CFDP_CancelTransaction(t));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &t, NULL);
    t->flags.com.canceled = 0;
    UtAssert_VOIDCALL(CF_CFDP_CancelTransaction(t));
}

/*******************************************************************************
**
**  cf_cfdp_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_CFDP_InitEngine, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_InitEngine");
    UtTest_Add(Test_CF_CFDP_CycleEngine, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_CycleEngine");
    UtTest_Add(Test_CF_CFDP_ProcessPlaybackDirectory, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ProcessPlaybackDirectory");
    UtTest_Add(Test_CF_CFDP_ProcessPollingDirectories, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ProcessPollingDirectories");
    UtTest_Add(Test_CF_CFDP_CycleTx, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "Test_CF_CFDP_CycleTx");
    UtTest_Add(Test_CF_CFDP_CycleTxFirstActive, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CycleTxFirstActive");
    UtTest_Add(Test_CF_CFDP_DoTick, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_DoTick");
    UtTest_Add(Test_CF_CFDP_TickTransactions, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_TickTransactions");
    UtTest_Add(Test_CF_CFDP_ResetTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_ResetTransaction");
    UtTest_Add(Test_CF_CFDP_CancelTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CancelTransaction");
    UtTest_Add(Test_CF_CFDP_DisableEngine, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_DisableEngine");
    UtTest_Add(Test_CF_CFDP_CloseFiles, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_CloseFiles");
    UtTest_Add(Test_CF_CFDP_CancelTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CancelTransaction");
    UtTest_Add(Test_CF_CFDP_TxFile, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_TxFile");
    UtTest_Add(Test_CF_CFDP_PlaybackDir, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_PlaybackDir");
    UtTest_Add(Test_CF_CFDP_ArmAckTimer, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_ArmAckTimer");

    UtTest_Add(Test_CF_CFDP_CF_CFDP_EncodeStart, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CF_CFDP_EncodeStart");
    UtTest_Add(Test_CF_CFDP_CF_CFDP_DecodeStart, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CF_CFDP_DecodeStart");

    UtTest_Add(Test_CF_CFDP_RecvDrop, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvDrop");
    UtTest_Add(Test_CF_CFDP_RecvIdle, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvIdle");
    UtTest_Add(Test_CF_CFDP_RecvPh, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvPh");
    UtTest_Add(Test_CF_CFDP_RecvMd, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvMd");
    UtTest_Add(Test_CF_CFDP_RecvFd, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvFd");
    UtTest_Add(Test_CF_CFDP_RecvEof, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvEof");
    UtTest_Add(Test_CF_CFDP_RecvAck, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvAck");
    UtTest_Add(Test_CF_CFDP_RecvFin, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvFin");
    UtTest_Add(Test_CF_CFDP_RecvNak, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvNak");
    UtTest_Add(Test_CF_CFDP_CopyStringFromLV, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_CopyStringFromLV");

    UtTest_Add(Test_CF_CFDP_ConstructPduHeader, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_ConstructPduHeader");
    UtTest_Add(Test_CF_CFDP_SendMd, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SendMd");
    UtTest_Add(Test_CF_CFDP_SendFd, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SendFd");
    UtTest_Add(Test_CF_CFDP_SendEof, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SendEof");
    UtTest_Add(Test_CF_CFDP_SendAck, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SendAck");
    UtTest_Add(Test_CF_CFDP_SendFin, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SendFin");
    UtTest_Add(Test_CF_CFDP_SendNak, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SendNak");
    UtTest_Add(Test_CF_CFDP_AppendTlv, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_AppendTlv");

} /* end UtTest_Setup for cf_cfdp_tests.c */

/* end cf_cfdp_tests.c */
