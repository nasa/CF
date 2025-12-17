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

/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_test_alt_handler.h"
#include "cf_events.h"
#include "cf_cfdp.h"
#include "cf_cfdp_s.h"
#include "cf_cfdp_r.h"
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
        ut_history.dir = CF_Direction_TX;
        UT_CFDP_SetupBasicTxState(&ut_pdu_buffer);

        /* make sure that a potential call to CF_CFDP_MsgOutGet() returns this buffer */
        UT_SetHandlerFunction(UT_KEY(CF_CFDP_MsgOutGet), UT_AltHandler_GenericPointerReturn, &ut_pdu_buffer);
    }
    else if (setup == UT_CF_Setup_RX)
    {
        ut_history.dir = CF_Direction_RX;
        UT_CFDP_SetupBasicRxState(&ut_pdu_buffer);
    }

    UT_SetDefaultReturnValue(UT_KEY(CF_GetChannelFromTxn),
                             (UT_IntReturn_t)&CF_AppData.engine.channels[UT_CFDP_CHANNEL]);
    UT_SetDefaultReturnValue(UT_KEY(CF_GetChunkListHead),
                             (UT_IntReturn_t)&CF_AppData.engine.channels[UT_CFDP_CHANNEL].cs[ut_history.dir]);

    /* reset the event ID capture between each sub-case */
    UT_CF_ResetEventCapture();
}

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
}

void cf_cfdp_tests_Teardown(void)
{
    cf_tests_Teardown();
}

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
    memset(&enc, 0, sizeof(enc));

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
    memset(&dec, 0, sizeof(dec));

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
     * void CF_CFDP_ArmAckTimer(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);

    /* nominal call */
    UtAssert_VOIDCALL(CF_CFDP_ArmAckTimer(txn));
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
    UtAssert_INT32_EQ(CF_CFDP_RecvPh(UT_CFDP_CHANNEL, ph), CF_SHORT_PDU_ERROR);
    UT_CF_AssertEventID(CF_PDU_SHORT_HEADER_ERR_EID);

    /* decode error, large file bit set */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, NULL, NULL);
    ph->pdu_header.large_flag = true;
    UtAssert_INT32_EQ(CF_CFDP_RecvPh(UT_CFDP_CHANNEL, ph), CF_ERROR);
    UT_CF_AssertEventID(CF_PDU_LARGE_FILE_ERR_EID);

    /* decode error, insufficient storage for EID or seq num */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_DecodeHeader), 1, CF_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_RecvPh(UT_CFDP_CHANNEL, ph), CF_ERROR);
    UT_CF_AssertEventID(CF_PDU_TRUNCATION_ERR_EID);
}

void Test_CF_CFDP_RecvMd(void)
{
    /* Test case for:
     * int CF_CFDP_RecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t *      txn;
    CF_History_t *          history;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduMd_t *    md;
    const char              src[]  = "mds";
    const char              dest[] = "mdd";

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &history, &txn, NULL);
    md                           = &ph->int_header.md;
    md->size                     = 10;
    md->dest_filename.length     = sizeof(dest) - 1;
    md->dest_filename.data_ptr   = dest;
    md->source_filename.length   = sizeof(src) - 1;
    md->source_filename.data_ptr = src;
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(txn, ph), 0);
    UtAssert_UINT32_EQ(txn->fsize, md->size);
    UtAssert_STRINGBUF_EQ(md->dest_filename.data_ptr, md->dest_filename.length, history->fnames.dst_filename,
                          sizeof(history->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(md->source_filename.data_ptr, md->source_filename.length, history->fnames.src_filename,
                          sizeof(history->fnames.src_filename));

    /* decode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(txn, ph), CF_PDU_METADATA_ERROR);
    UT_CF_AssertEventID(CF_PDU_MD_SHORT_ERR_EID);

    /* decode errors: LV dest filename too long */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    md                       = &ph->int_header.md;
    md->dest_filename.length = CF_FILENAME_MAX_LEN + 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(txn, ph), CF_PDU_METADATA_ERROR);
    UT_CF_AssertEventID(CF_PDU_INVALID_DST_LEN_ERR_EID);

    /* decode errors: LV source filename too long */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    md                         = &ph->int_header.md;
    md->source_filename.length = CF_FILENAME_MAX_LEN + 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvMd(txn, ph), CF_PDU_METADATA_ERROR);
    UT_CF_AssertEventID(CF_PDU_INVALID_SRC_LEN_ERR_EID);
}

void Test_CF_CFDP_RecvFd(void)
{
    /* Test case for:
     * int CF_CFDP_RecvFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call, no CRC */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(txn, ph), 0);

    /* nominal call, with CRC */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    ph->pdu_header.crc_flag    = 1;
    ph->int_header.fd.data_len = 10 + sizeof(CF_CFDP_uint32_t);
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(txn, ph), 0);
    UtAssert_UINT32_EQ(ph->int_header.fd.data_len, 10);

    /* decode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(txn, ph), CF_SHORT_PDU_ERROR);
    UT_CF_AssertEventID(CF_PDU_FD_SHORT_ERR_EID);

    /* decode errors: CRC part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    ph->pdu_header.crc_flag    = 1;
    ph->int_header.fd.data_len = sizeof(CF_CFDP_uint32_t) - 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(txn, ph), CF_SHORT_PDU_ERROR);
    UtAssert_BOOL_FALSE(CF_CODEC_IS_OK(ph->pdec));

    /* with segment metadata (unimplemented) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    ph->pdu_header.segment_meta_flag = 1;
    UtAssert_INT32_EQ(CF_CFDP_RecvFd(txn, ph), CF_ERROR);
    UT_CF_AssertEventID(CF_PDU_FD_UNSUPPORTED_ERR_EID);
}

void Test_CF_CFDP_RecvEof(void)
{
    /* Test case for:
     * int CF_CFDP_RecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvEof(txn, ph), 0);

    /* decode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvEof(txn, ph), CF_SHORT_PDU_ERROR);
    UT_CF_AssertEventID(CF_PDU_EOF_SHORT_ERR_EID);
}

void Test_CF_CFDP_RecvAck(void)
{
    /* Test case for:
     * int CF_CFDP_RecvAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvAck(txn, ph), 0);

    /* decode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvAck(txn, ph), CF_SHORT_PDU_ERROR);
    UT_CF_AssertEventID(CF_PDU_ACK_SHORT_ERR_EID);
}

void Test_CF_CFDP_RecvFin(void)
{
    /* Test case for:
     * int CF_CFDP_RecvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvFin(txn, ph), 0);

    /* decode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvFin(txn, ph), CF_SHORT_PDU_ERROR);
    UT_CF_AssertEventID(CF_PDU_FIN_SHORT_ERR_EID);
}

void Test_CF_CFDP_RecvNak(void)
{
    /* Test case for:
     * int CF_CFDP_RecvNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_RecvNak(txn, ph), 0);

    /* decode errors: fixed part */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    CF_CODEC_SET_DONE(ph->pdec);
    UtAssert_INT32_EQ(CF_CFDP_RecvNak(txn, ph), CF_SHORT_PDU_ERROR);
    UT_CF_AssertEventID(CF_PDU_NAK_SHORT_ERR_EID);
}

void Test_CF_CFDP_RecvDrop(void)
{
    /* Test case for:
     * void CF_CFDP_RecvDrop(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_RecvDrop(txn, ph));
}

void Test_CF_CFDP_RecvHold(void)
{
    /* Test case for:
     * void CF_CFDP_RecvHold(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    /* nominal call, fin in TX */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->history->dir             = CF_Direction_TX;
    ph->fdirective.directive_code = CF_CFDP_FileDirective_FIN;
    UtAssert_VOIDCALL(CF_CFDP_RecvHold(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_S_SubstateRecvFin, 1);

    /* nominal call, eof in RX */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->history->dir             = CF_Direction_RX;
    ph->fdirective.directive_code = CF_CFDP_FileDirective_EOF;
    UtAssert_VOIDCALL(CF_CFDP_RecvHold(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_R_SubstateRecvEof, 1);

    /* Not a FIN in TX */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->history->dir             = CF_Direction_TX;
    ph->fdirective.directive_code = CF_CFDP_FileDirective_INVALID_MAX;
    UtAssert_VOIDCALL(CF_CFDP_RecvHold(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_S_SubstateRecvFin, 1);

    /* Not a EOF in RX */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, NULL, &txn, NULL);
    txn->history->dir             = CF_Direction_RX;
    ph->fdirective.directive_code = CF_CFDP_FileDirective_INVALID_MAX;
    UtAssert_VOIDCALL(CF_CFDP_RecvHold(txn, ph));
    UtAssert_STUB_COUNT(CF_CFDP_R_SubstateRecvEof, 1);
}

void Test_CF_CFDP_RecvInit(void)
{
    /* Test case for:
     * void CF_CFDP_RecvInit(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_History_t *          history;
    CF_Logical_PduBuffer_t *ph;

    /* when FindUnusedChunks returns NULL (default setup for stub) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, NULL, &history, &txn, NULL);
    txn->state = CF_TxnState_INIT;
    UtAssert_VOIDCALL(CF_CFDP_RecvInit(txn, ph));
    UtAssert_INT32_EQ(txn->state, CF_TxnState_HOLD);
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
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *txn, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent);
*/
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduHeader_t *hdr;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_NULL(CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_ACK, 3, 2, true, 42, false));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_NULL(CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_ACK, 3, 2, true, 42, true));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    txn->state = CF_TxnState_S1;
    UtAssert_NOT_NULL(CF_CFDP_ConstructPduHeader(txn, CF_CFDP_FileDirective_ACK, 3, 2, true, 42, false));
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

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_GetValueEncodedSize), 5);
    txn->state = CF_TxnState_S2;
    UtAssert_NOT_NULL(CF_CFDP_ConstructPduHeader(txn, 0, 7, 6, false, 44, false));
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
        CFE_Status_t     CF_CFDP_SendMd(CF_Transaction_t *txn);
     */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;
    CF_History_t *          history;
    CF_Logical_PduMd_t *    md;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendMd(txn), CF_SEND_PDU_NO_BUF_AVAIL_ERROR);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, &history, &txn, NULL);
    md = &ph->int_header.md;
    strncpy(history->fnames.dst_filename, "dst1", sizeof(history->fnames.dst_filename));
    strncpy(history->fnames.src_filename, "src1", sizeof(history->fnames.src_filename));
    txn->state = CF_TxnState_S1;
    txn->fsize = 1234;
    UtAssert_INT32_EQ(CF_CFDP_SendMd(txn), CFE_SUCCESS);
    UtAssert_UINT32_EQ(md->size, txn->fsize);
    UtAssert_STRINGBUF_EQ(md->dest_filename.data_ptr, md->dest_filename.length, history->fnames.dst_filename,
                          sizeof(history->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(md->source_filename.data_ptr, md->source_filename.length, history->fnames.src_filename,
                          sizeof(history->fnames.src_filename));

    /* Class 2, also hit maximum string length */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, &history, &txn, NULL);
    md = &ph->int_header.md;
    memset(history->fnames.dst_filename, 0xFF, sizeof(history->fnames.dst_filename));
    strncpy(history->fnames.src_filename, "src2", sizeof(history->fnames.src_filename));
    txn->state = CF_TxnState_S2;
    txn->fsize = 5678;
    UtAssert_INT32_EQ(CF_CFDP_SendMd(txn), CFE_SUCCESS);
    UtAssert_UINT32_EQ(md->size, txn->fsize);
    UtAssert_UINT32_EQ(md->dest_filename.length, sizeof(history->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(md->source_filename.data_ptr, md->source_filename.length, history->fnames.src_filename,
                          sizeof(history->fnames.src_filename));
}

void Test_CF_CFDP_SendFd(void)
{
    /* Test case for:
        CFE_Status_t     CF_CFDP_SendFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
    */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendFd(txn, ph), CFE_SUCCESS);

    /* Hit CF_CFDP_SetPduLength condition where final_pos < the header_encoded_length */
    ph->pdu_header.header_encoded_length = CF_CODEC_GET_POSITION(ph->penc) + 1;
    ph->pdu_header.data_encoded_length   = 0;

    UtAssert_INT32_EQ(CF_CFDP_SendFd(txn, ph), CFE_SUCCESS);

    UtAssert_UINT32_EQ(ph->pdu_header.data_encoded_length, 0);
}

void Test_CF_CFDP_SendEof(void)
{
    /* Test case for:
        CFE_Status_t     CF_CFDP_SendEof(CF_Transaction_t *txn);
     */

    CF_Transaction_t *      txn;
    CF_History_t *          history;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduEof_t *   eof;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_SendEof(txn), CF_SEND_PDU_NO_BUF_AVAIL_ERROR);

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    eof = &ph->int_header.eof;
    UtAssert_INT32_EQ(CF_CFDP_SendEof(txn), CFE_SUCCESS);
    UtAssert_ZERO(eof->tlv_list.num_tlv);

    /* test with a transaction error status, which should append a TLV */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, &history, &txn, NULL);
    eof = &ph->int_header.eof;
    UT_SetDefaultReturnValue(UT_KEY(CF_TxnStatus_To_ConditionCode), CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    UtAssert_INT32_EQ(CF_CFDP_SendEof(txn), CFE_SUCCESS);
    UtAssert_UINT32_EQ(eof->tlv_list.num_tlv, 1);
    UtAssert_STUB_COUNT(CF_CFDP_Send, 2);
}

void Test_CF_CFDP_SendAck(void)
{
    /* Test case for:
        CFE_Status_t CF_CFDP_SendAck(CF_Transaction_t *txn, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                                    CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn);
     */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduAck_t *   ack;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    // CF_CFDP_SendAck(txn, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF, CF_CFDP_ConditionCode_NO_ERROR, 1,
    // 42);
    UtAssert_INT32_EQ(CF_CFDP_SendAck(txn, CF_CFDP_FileDirective_EOF), CF_SEND_PDU_NO_BUF_AVAIL_ERROR);

    /* nominal as receiver */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_ACTIVE);
    txn->state = CF_TxnState_R2;
    // CF_CFDP_SendAck(txn, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF, CF_CFDP_ConditionCode_NO_ERROR, 1,
    // 42)
    UtAssert_INT32_EQ(CF_CFDP_SendAck(txn, CF_CFDP_FileDirective_EOF), CFE_SUCCESS);
    ack = &ph->int_header.ack;
    UtAssert_UINT32_EQ(ack->ack_directive_code, CF_CFDP_FileDirective_EOF);
    UtAssert_UINT32_EQ(ack->ack_subtype_code, 0);
    UtAssert_UINT32_EQ(ack->txn_status, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_UINT32_EQ(ack->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* nominal as sender */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_ACTIVE);
    txn->state = CF_TxnState_S2;
    // CF_CFDP_SendAck(txn, CF_CFDP_AckTxnStatus_ACTIVE, CF_CFDP_FileDirective_EOF, CF_CFDP_ConditionCode_NO_ERROR, 1,
    // 42),
    UtAssert_INT32_EQ(CF_CFDP_SendAck(txn, CF_CFDP_FileDirective_EOF), CFE_SUCCESS);
    ack = &ph->int_header.ack;
    UtAssert_UINT32_EQ(ack->ack_directive_code, CF_CFDP_FileDirective_EOF);
    UtAssert_UINT32_EQ(ack->ack_subtype_code, 0);
    UtAssert_UINT32_EQ(ack->txn_status, CF_CFDP_AckTxnStatus_ACTIVE);
    UtAssert_UINT32_EQ(ack->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* still success path but with non-nominal values */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_GetAckTxnStatus), 1, CF_CFDP_AckTxnStatus_TERMINATED);
    txn->state              = CF_TxnState_R2;
    txn->state_data.peer_cc = CF_CFDP_ConditionCode_FILESTORE_REJECTION;
    // CF_CFDP_SendAck(txn, CF_CFDP_AckTxnStatus_TERMINATED, CF_CFDP_FileDirective_FIN,
    // CF_CFDP_ConditionCode_FILESTORE_REJECTION, 1, 42)
    UtAssert_INT32_EQ(CF_CFDP_SendAck(txn, CF_CFDP_FileDirective_FIN), CFE_SUCCESS);
    ack = &ph->int_header.ack;
    UtAssert_UINT32_EQ(ack->ack_directive_code, CF_CFDP_FileDirective_FIN);
    UtAssert_UINT32_EQ(ack->ack_subtype_code, 1);
    UtAssert_UINT32_EQ(ack->txn_status, CF_CFDP_AckTxnStatus_TERMINATED);
    UtAssert_UINT32_EQ(ack->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
}

void Test_CF_CFDP_SendFin(void)
{
    /* Test case for:
        CFE_Status_t CF_CFDP_SendFin(CF_Transaction_t *txn, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                                    CF_CFDP_ConditionCode_t cc);
     */

    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;
    CF_Logical_PduFin_t *   fin;

    /* setup without a tx message */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    // CF_CFDP_SendFin(txn, CF_CFDP_FinDeliveryCode_COMPLETE, CF_CFDP_FinFileStatus_RETAINED,
    // CF_CFDP_ConditionCode_NO_ERROR),
    UtAssert_INT32_EQ(CF_CFDP_SendFin(txn), CF_SEND_PDU_NO_BUF_AVAIL_ERROR);

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    txn->state_data.fin_dc = CF_CFDP_FinDeliveryCode_COMPLETE;
    txn->state_data.fin_fs = CF_CFDP_FinFileStatus_RETAINED;
    // CF_CFDP_SendFin(txn, CF_CFDP_FinDeliveryCode_COMPLETE, CF_CFDP_FinFileStatus_RETAINED,
    // CF_CFDP_ConditionCode_NO_ERROR),
    UtAssert_INT32_EQ(CF_CFDP_SendFin(txn), CFE_SUCCESS);
    fin = &ph->int_header.fin;
    UtAssert_ZERO(fin->tlv_list.num_tlv);
    UtAssert_UINT32_EQ(fin->delivery_code, CF_CFDP_FinDeliveryCode_COMPLETE);
    UtAssert_UINT32_EQ(fin->file_status, CF_CFDP_FinFileStatus_RETAINED);
    UtAssert_UINT32_EQ(fin->cc, CF_CFDP_ConditionCode_NO_ERROR);

    /* test with an alternate condition code, which should append a TLV */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    txn->state_data.fin_dc = CF_CFDP_FinDeliveryCode_INCOMPLETE;
    txn->state_data.fin_fs = CF_CFDP_FinFileStatus_DISCARDED;
    UT_SetDeferredRetcode(UT_KEY(CF_TxnStatus_To_ConditionCode), 1, CF_TxnStatus_FILESTORE_REJECTION);
    // CF_CFDP_SendFin(txn, CF_CFDP_FinDeliveryCode_INCOMPLETE, CF_CFDP_FinFileStatus_DISCARDED,
    // CF_CFDP_ConditionCode_FILESTORE_REJECTION),
    UtAssert_INT32_EQ(CF_CFDP_SendFin(txn), CFE_SUCCESS);
    fin = &ph->int_header.fin;
    UtAssert_UINT32_EQ(fin->delivery_code, CF_CFDP_FinDeliveryCode_INCOMPLETE);
    UtAssert_UINT32_EQ(fin->file_status, CF_CFDP_FinFileStatus_DISCARDED);
    UtAssert_UINT32_EQ(fin->cc, CF_CFDP_ConditionCode_FILESTORE_REJECTION);
    UtAssert_UINT32_EQ(fin->tlv_list.num_tlv, 1);
    UtAssert_STUB_COUNT(CF_CFDP_Send, 2);
}

void Test_CF_CFDP_SendNak(void)
{
    /* Test case for:
        CFE_Status_t CF_CFDP_SendNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph);
     */
    CF_Transaction_t *      txn;
    CF_Logical_PduBuffer_t *ph;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, &ph, NULL, NULL, &txn, NULL);
    txn->state = CF_TxnState_S2;
    UtAssert_VOIDCALL(CF_CFDP_SendNak(txn, ph));
}

void Test_CF_CFDP_AppendTlv(void)
{
    /* Test case for:
        void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type)

        This helper is used by EOF and FIN to set TLV values
    */
    CF_Logical_PduBuffer_t *ph;
    CF_ConfigTable_t *      config;
    CF_Logical_TlvList_t *  tlv_list;

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
    UtAssert_STUB_COUNT(CF_FreeTransaction, CF_NUM_TRANSACTIONS_PER_CHANNEL * CF_NUM_CHANNELS);

    /* nominal call, with sem */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    config->chan[0].sem_name[0] = 'u';
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), 0);
    UtAssert_BOOL_TRUE(CF_AppData.engine.enabled);

    /* failure of OS_CountSemGetIdByName for non-name reason */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    config->chan[0].sem_name[0] = 'u';
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemGetIdByName), OS_ERROR);
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), OS_ERROR);
    UtAssert_BOOL_FALSE(CF_AppData.engine.enabled);
    UT_CF_AssertEventID(CF_INIT_SEM_ERR_EID);

    /* Max retries of OS_CountSemGetIdByName - sem was never created at all  */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    config->chan[0].sem_name[0] = 'u';
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemGetIdByName), OS_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), OS_ERR_NAME_NOT_FOUND);
    UtAssert_BOOL_FALSE(CF_AppData.engine.enabled);
    UT_CF_AssertEventID(CF_INIT_SEM_ERR_EID);

    /* Retry of OS_CountSemGetIdByName, when sem was created late, and thus
     * got return OS_ERR_NAME_NOT_FOUND followed by OS_SUCCESS */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, NULL, &config);
    config->chan[0].sem_name[0] = 'u';
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemGetIdByName), OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CF_CFDP_InitEngine(), 0);
    UtAssert_BOOL_TRUE(CF_AppData.engine.enabled);

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
    CF_History_t *    history;
    CF_Transaction_t *txn;
    CF_Channel_t *    chan;
    CF_ChunkWrapper_t chunk_wrap;

    memset(&chunk_wrap, 0, sizeof(chunk_wrap));

    /* nominal call */
    /* make sure call to CF_FindUnusedTransaction() returns this buffer */
    /* Also need to set up for call to CF_CFDP_FindUnusedChunks which calls CF_CList_Pop */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, &history, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, txn);
    UT_SetHandlerFunction(UT_KEY(CF_CList_Pop), UT_AltHandler_GenericPointerReturn, &chunk_wrap.cl_node);
    chan->cs[CF_Direction_TX] = &chunk_wrap.cl_node;
    UtAssert_INT32_EQ(CF_CFDP_TxFile(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), 0);
    UtAssert_STRINGBUF_EQ(dest, -1, history->fnames.dst_filename, sizeof(history->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(src, -1, history->fnames.src_filename, sizeof(history->fnames.src_filename));
    UtAssert_UINT32_EQ(chan->num_cmd_tx, 1);
    UT_CF_AssertEventID(CF_CFDP_S_START_SEND_INF_EID);

    /* same but for class 2 (for branch coverage) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, &history, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, txn);
    UT_SetHandlerFunction(UT_KEY(CF_CList_Pop), UT_AltHandler_GenericPointerReturn, &chunk_wrap.cl_node);
    chan->cs[CF_Direction_TX] = &chunk_wrap.cl_node;
    UtAssert_INT32_EQ(CF_CFDP_TxFile(src, dest, CF_CFDP_CLASS_2, 1, UT_CFDP_CHANNEL, 0, 1), 0);
    UtAssert_STRINGBUF_EQ(dest, -1, history->fnames.dst_filename, sizeof(history->fnames.dst_filename));
    UtAssert_STRINGBUF_EQ(src, -1, history->fnames.src_filename, sizeof(history->fnames.src_filename));
    UtAssert_UINT32_EQ(chan->num_cmd_tx, 2);
    UT_CF_AssertEventID(CF_CFDP_S_START_SEND_INF_EID);

    /* max TX */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, &history, &txn, NULL);
    chan->num_cmd_tx = CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN;
    UtAssert_INT32_EQ(CF_CFDP_TxFile(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), -1);
    UT_CF_AssertEventID(CF_CFDP_MAX_CMD_TX_ERR_EID);
}

void Test_CF_CFDP_StartRxTransaction(void)
{
    /* Test case for:
     * CF_Transaction_t *CF_CFDP_StartRxTransaction(uint8 chan_num);
     */
    CF_Transaction_t *txn;

    /* Reject at limit */
    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_RX] = CF_MAX_SIMULTANEOUS_RX;
    UtAssert_NULL(CF_CFDP_StartRxTransaction(UT_CFDP_CHANNEL));

    /* Under limit but FindUnusedTransaction still fails */
    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_RX] = CF_MAX_SIMULTANEOUS_RX - 1;
    UtAssert_NULL(CF_CFDP_StartRxTransaction(UT_CFDP_CHANNEL));

    /* Set so FindUnusedTransaction returns non-null (nominal) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, txn);
    UtAssert_ADDRESS_EQ(CF_CFDP_StartRxTransaction(UT_CFDP_CHANNEL), txn);
    UtAssert_UINT8_EQ(txn->flags.com.q_index, CF_QueueIdx_RX);
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
    CF_Channel_t * chan;
    uint8          i;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, NULL, NULL);
    pb = &chan->playback[0];
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
    UT_CF_AssertEventID(CF_CFDP_OPENDIR_ERR_EID);

    /* no non-busy entries */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, NULL, NULL);
    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        pb       = &chan->playback[i];
        pb->busy = true;
    }
    UtAssert_INT32_EQ(CF_CFDP_PlaybackDir(src, dest, CF_CFDP_CLASS_1, 1, UT_CFDP_CHANNEL, 0, 1), -1);
    UT_CF_AssertEventID(CF_CFDP_DIR_SLOT_ERR_EID);
}

static int32 Ut_Hook_StateHandler_SetCount(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                           const UT_StubContext_t *Context)
{
    if (CallCount < 3)
    {
        ++CF_AppData.engine.channels[UT_CFDP_CHANNEL].outgoing_counter;
    }
    return StubRetcode;
}

void Test_CF_CFDP_S_Tick_NewData(void)
{
    /* Test case for:
       int CF_CFDP_S_Tick_NewData(CF_CListNode_t *node, void *context);
     */
    CF_Transaction_t *txn;
    CF_Channel_t *    chan;

    /* suspended (do nothing) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.suspended = true;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_NewData(txn));

    /* nominal, not in normal data state (do nothing) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.suspended  = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_EOF;
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_NewData(txn));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, NULL, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_GetChannelFromTxn), UT_AltHandler_GenericPointerReturn, chan);
    txn->flags.com.suspended  = false;
    txn->state_data.sub_state = CF_TxSubState_DATA_NORMAL;
    UT_SetHookFunction(UT_KEY(CF_CFDP_S_SubstateSendFileData), Ut_Hook_StateHandler_SetCount, NULL);
    UtAssert_VOIDCALL(CF_CFDP_S_Tick_NewData(txn));
}

static void DoTickFnSetBlocked(CF_Transaction_t *txn)
{
    CF_AppData.engine.channels[txn->chan_num].tx_blocked = true;
}

static void DoTickNoop(CF_Transaction_t *txn)
{
    UT_DEFAULT_IMPL(DoTickNoop);
}

void Test_CF_CFDP_DoTick(void)
{
    /* Test case for:
     * int CF_CFDP_DoTick(CF_CListNode_t *node, void *context);
     */
    CF_Transaction_t *  txn;
    CF_Transaction_t    txn2;
    CF_CFDP_Tick_args_t args;

    memset(&args, 0, sizeof(args));
    memset(&txn2, 0, sizeof(txn2));
    args.fn = DoTickNoop;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.chan, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&txn->cl_node, &args), CF_CLIST_CONT);
    UtAssert_STUB_COUNT(DoTickNoop, 1);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.chan, NULL, &txn, NULL);
    args.resume_point = &txn2;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&txn->cl_node, &args), CF_CLIST_CONT);
    UtAssert_STUB_COUNT(DoTickNoop, 1);
    UtAssert_ADDRESS_EQ(args.resume_point, &txn2);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.chan, NULL, &txn, NULL);
    args.resume_point = txn;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&txn->cl_node, &args), CF_CLIST_CONT);
    UtAssert_STUB_COUNT(DoTickNoop, 2);
    UtAssert_NULL(args.resume_point);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.chan, NULL, &txn, NULL);
    txn->flags.com.suspended = true;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&txn->cl_node, &args), CF_CLIST_CONT);
    UtAssert_STUB_COUNT(DoTickNoop, 2);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &args.chan, NULL, &txn, NULL);
    args.fn = DoTickFnSetBlocked;
    UtAssert_INT32_EQ(CF_CFDP_DoTick(&txn->cl_node, &args), CF_CLIST_EXIT);
}

void Test_CF_CFDP_ProcessPollingDirectories(void)
{
    /* Test case for:
     * void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *chan)
     */
    CF_Channel_t *    chan;
    CF_ConfigTable_t *config;
    CF_PollDir_t *    pdcfg;
    CF_Poll_t *       poll;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, NULL, NULL, &config);
    pdcfg = &config->chan[UT_CFDP_CHANNEL].polldir[0];
    poll  = &chan->poll[0];

    /* nominal call, polldir disabled (noop) */
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].poll_counter, 0);

    /* nominal call, polldir enabled but interval_sec == 0 */
    /* Will tick because CF_Timer_Expired stub returns 0 by default (not expired) */
    pdcfg->enabled = 1;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_BOOL_FALSE(poll->timer_set);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].poll_counter, 1);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);

    /* with interval_sec nonzero the timer should get set, but not tick */
    pdcfg->interval_sec = 1;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_BOOL_TRUE(poll->timer_set);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].poll_counter, 1);

    /* call again should tick */
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_BOOL_TRUE(poll->timer_set);
    UtAssert_STUB_COUNT(CF_Timer_Tick, 2);

    /* call again timer should expire and start a playback */
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, true);
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_BOOL_FALSE(poll->timer_set);
    UtAssert_BOOL_TRUE(poll->pb.busy);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].poll_counter, 1);

    /* make an error occur in CF_CFDP_PlaybackDir_Initiate() */
    poll->pb.busy   = false; /* above would have set it true */
    poll->timer_set = true;
    UT_SetDeferredRetcode(UT_KEY(CF_Timer_Expired), 1, true);
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryOpen), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_BOOL_TRUE(poll->timer_set);
    UT_CF_AssertEventID(CF_CFDP_OPENDIR_ERR_EID);

    /* Test case where the impl calls through to CF_CFDP_ProcessPlaybackDirectory()
     *
     * NOTE: with diropen set false, this will not attempt
     * to start a new transaction here (that case is covered in another test)
     */
    poll->pb.busy    = false;
    poll->pb.diropen = false;
    poll->pb.num_ts  = 1;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_BOOL_FALSE(poll->pb.busy);

    poll->pb.busy   = true;
    poll->pb.num_ts = 0;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_BOOL_FALSE(poll->pb.busy); /* because num_ts == 0 */

    /* test that call to CF_CFDP_UpdatePollPbCounted will decrement back to 0 again */
    pdcfg->enabled = 0;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPollingDirectories(chan));
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].poll_counter, 0);
}

void Test_CF_CFDP_ProcessPlaybackDirectory(void)
{
    /* Test case for:
     * void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *chan, CF_Playback_t *pb)
     */
    CF_Transaction_t *txn;
    CF_History_t *    history;
    CF_Channel_t *    chan;
    CF_ConfigTable_t *config;
    CF_Playback_t     pb;
    os_dirent_t       dirent[3];
    CF_ChunkWrapper_t chunk_wrap;

    memset(&chunk_wrap, 0, sizeof(chunk_wrap));
    memset(&pb, 0, sizeof(pb));
    memset(dirent, 0, sizeof(dirent));
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, &history, &txn, &config);
    CF_AppData.engine.enabled = true;

    /* diropen is true but num_ts is high so operations are restricted */
    pb.busy    = true;
    pb.num_ts  = CF_NUM_TRANSACTIONS_PER_PLAYBACK + 1;
    pb.diropen = true;
    UtAssert_VOIDCALL(CF_CFDP_ProcessPlaybackDirectory(chan, &pb));
    UtAssert_BOOL_TRUE(pb.busy);
    UtAssert_BOOL_TRUE(pb.diropen);

    /*
     * enter the loop, but error calling OS_DirectoryRead().
     * This should end up calling OS_DirectoryClose().
     */
    pb.busy    = true;
    pb.diropen = true;
    pb.num_ts  = 0;
    OS_DirectoryOpen(&pb.dir_id, "ut");
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 1, OS_ERROR);
    UtAssert_VOIDCALL(CF_CFDP_ProcessPlaybackDirectory(chan, &pb));
    UtAssert_STUB_COUNT(OS_DirectoryClose, 1);
    UtAssert_BOOL_FALSE(pb.busy);
    UtAssert_BOOL_FALSE(pb.diropen);

    /* nominal, but path is "." or ".." ...
     * this initially does not start a new transaction - those files are ignored.
     * note that this does a while loop here, so have to prepare all the entries at once.
     * After bypassing . and .. this will try to start a new transaction on pass 3.
     * this calls CF_FindUnusedTransaction() and if that returns NULL it stops there.
     */
    pb.busy    = true;
    pb.diropen = true;
    pb.num_ts  = 0;
    strcpy(dirent[0].FileName, ".");  /* ignored */
    strcpy(dirent[1].FileName, ".."); /* ignored */
    strcpy(dirent[2].FileName, "ut"); /* valid file */
    OS_DirectoryOpen(&pb.dir_id, "ut");
    UT_SetDataBuffer(UT_KEY(OS_DirectoryRead), dirent, sizeof(dirent), false);
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 4, OS_ERROR); /* end of dir */
    UtAssert_VOIDCALL(CF_CFDP_ProcessPlaybackDirectory(chan, &pb));
    UtAssert_BOOL_TRUE(pb.busy);
    UtAssert_BOOL_TRUE(pb.diropen);
    UtAssert_ZERO(pb.num_ts);
    UtAssert_STRINGBUF_EQ(pb.pending_file, sizeof(pb.pending_file), "ut", -1);

    /* Now enter again but this time FindUnusedTransaction gets non-NULL (resumes playback) */
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, txn);
    UtAssert_VOIDCALL(CF_CFDP_ProcessPlaybackDirectory(chan, &pb));
    UtAssert_BOOL_TRUE(pb.busy);
    UtAssert_BOOL_FALSE(pb.diropen);
    UtAssert_UINT16_EQ(pb.num_ts, 1);
    UtAssert_STRINGBUF_EQ(history->fnames.src_filename, sizeof(history->fnames.src_filename), "/ut", -1);
    UtAssert_STRINGBUF_EQ(history->fnames.dst_filename, sizeof(history->fnames.dst_filename), "/ut", -1);
    UtAssert_STRINGBUF_EQ(pb.pending_file, sizeof(pb.pending_file), "", -1);
    UT_CF_AssertEventID(CF_CFDP_S_START_SEND_INF_EID);
}

static int32 Ut_Hook_TickTransactions_SetBlocked(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                 const UT_StubContext_t *Context)
{
    CF_CFDP_Tick_args_t *args = UT_Hook_GetArgValueByName(Context, "context", CF_CFDP_Tick_args_t *);

    if (CallCount != 0)
    {
        args->chan->tx_blocked = true;
    }

    return StubRetcode;
}

static int32 Ut_Hook_TickTransactions_UpdateCount(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                                  const UT_StubContext_t *Context)
{
    if (CallCount < 5)
    {
        ++CF_AppData.engine.channels[UT_CFDP_CHANNEL].outgoing_counter;
    }

    return StubRetcode;
}

void Test_CF_CFDP_TickTransactions(void)
{
    /* Test case for:
        void CF_CFDP_TickTransactions(CF_Channel_t *chan);
     */
    CF_Channel_t *chan;

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, NULL, NULL, NULL);
    UtAssert_VOIDCALL(CF_CFDP_TickTransactions(chan));
    UtAssert_STUB_COUNT(CF_CList_Traverse, 4);

    UT_ResetState(UT_KEY(CF_CList_Traverse));
    UT_SetHookFunction(UT_KEY(CF_CList_Traverse), Ut_Hook_TickTransactions_SetBlocked, NULL);
    UtAssert_VOIDCALL(CF_CFDP_TickTransactions(chan));
    UtAssert_STUB_COUNT(CF_CList_Traverse, 2);

    UT_ResetState(UT_KEY(CF_CList_Traverse));
    UT_SetHookFunction(UT_KEY(CF_CList_Traverse), Ut_Hook_TickTransactions_UpdateCount, NULL);
    chan->tx_blocked = false;
    UtAssert_VOIDCALL(CF_CFDP_TickTransactions(chan));
    UtAssert_STUB_COUNT(CF_CList_Traverse, 7); /* because NAK is repeated several times  */
}

void Test_CF_CFDP_CycleEngine(void)
{
    /* Test case for:
     * void CF_CFDP_CycleEngine(void)
     */
    CF_Channel_t *chan;

    /* nominal with engine disabled, noop */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, NULL, NULL);
    UtAssert_VOIDCALL(CF_CFDP_CycleEngine());

    /* enabled but frozen */
    CF_AppData.engine.enabled                                = true;
    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].frozen = 1;
    UtAssert_VOIDCALL(CF_CFDP_CycleEngine());

    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].frozen = 0;
    UtAssert_VOIDCALL(CF_CFDP_CycleEngine());
}

void Test_CF_CFDP_FinishTransaction(void)
{
    /* Test case for:
     * void CF_CFDP_FinishTransaction(CF_Transaction_t *txn, int keep_history)
     */

    CF_Transaction_t *txn;
    CF_History_t *    history;
    CF_Channel_t *    chan;
    CF_Playback_t     pb;

    memset(&pb, 0, sizeof(pb));

    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_TX] = 10;

    /* Attempt to reset a transaction that has already been freed*/
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.q_index = CF_QueueIdx_FREE;
    UtAssert_VOIDCALL(CF_CFDP_FinishTransaction(txn, false));

    /* nominal call - when txn is on TXA this also moves it to TXW
     * (this is needed so the TX tick processor stops trying to service this) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_GetAckTxnStatus), CF_CFDP_AckTxnStatus_ACTIVE);
    txn->flags.com.q_index = CF_QueueIdx_TX;

    UtAssert_VOIDCALL(CF_CFDP_FinishTransaction(txn, true));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, &history, &txn, NULL);
    txn->flags.com.q_index      = CF_QueueIdx_TX;
    txn->fd                     = OS_ObjectIdFromInteger(1);
    history->dir                = CF_Direction_TX;
    txn->state                  = CF_TxnState_S1;
    txn->flags.com.keep_history = true;
    UtAssert_VOIDCALL(CF_CFDP_FinishTransaction(txn, false));
    UtAssert_BOOL_FALSE(txn->flags.com.keep_history);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, &history, &txn, NULL);
    txn->flags.com.q_index      = CF_QueueIdx_RX;
    txn->fd                     = OS_ObjectIdFromInteger(1);
    history->dir                = CF_Direction_RX;
    txn->state                  = CF_TxnState_R1;
    txn->flags.com.keep_history = false;
    UtAssert_VOIDCALL(CF_CFDP_FinishTransaction(txn, true));
    UtAssert_BOOL_TRUE(txn->flags.com.keep_history);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->fd                     = OS_ObjectIdFromInteger(1);
    history->dir                = CF_Direction_TX;
    txn->keep                   = 1;
    txn->state                  = CF_TxnState_S1;
    txn->flags.com.keep_history = false;
    UtAssert_VOIDCALL(CF_CFDP_FinishTransaction(txn, true));
    UtAssert_BOOL_TRUE(txn->flags.com.keep_history);

    /* coverage completeness:
     * test decrement of chan->num_cmd_tx
     * test decrement of playback num_ts
     */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, &chan, &history, &txn, NULL);
    pb.num_ts            = 10;
    txn->pb              = &pb;
    txn->flags.tx.cmd_tx = 5;
    chan->num_cmd_tx     = 8;
    history->dir         = CF_Direction_TX;
    txn->state           = CF_TxnState_S1;
    UtAssert_VOIDCALL(CF_CFDP_FinishTransaction(txn, true));
    UtAssert_UINT32_EQ(pb.num_ts, 9);
    UtAssert_UINT32_EQ(chan->num_cmd_tx, 7);

    /* coverage completeness: history is NULL (should never happen in real use) */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, &history, &txn, NULL);
    txn->history = NULL;
    txn->state   = CF_TxnState_S1;
    UtAssert_VOIDCALL(CF_CFDP_FinishTransaction(txn, true));
}

void Test_CF_CFDP_RecycleTransaction(void)
{
    /* Test case for:
     * void CF_CFDP_RecycleTransaction(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;
    CF_ChunkWrapper_t cl;

    CF_AppData.hk.Payload.channel_hk[UT_CFDP_CHANNEL].q_size[CF_QueueIdx_TX] = 10;

    /* nominal */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.q_index      = CF_QueueIdx_TX;
    txn->flags.com.keep_history = false;
    UtAssert_VOIDCALL(CF_CFDP_RecycleTransaction(txn));
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);

    /* cleanup dangling file handle */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.q_index      = CF_QueueIdx_TX;
    txn->flags.com.keep_history = true;
    OS_OpenCreate(&txn->fd, "ut", 0, 0);
    UtAssert_VOIDCALL(CF_CFDP_RecycleTransaction(txn));
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_BOOL_FALSE(OS_ObjectIdDefined(txn->fd));

    /* cleanup chunklist */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->flags.com.q_index = CF_QueueIdx_TX;
    txn->chunks            = &cl;
    UtAssert_VOIDCALL(CF_CFDP_RecycleTransaction(txn));
    UtAssert_NULL(txn->chunks);

    /* repeat but fail to get list head */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UT_ResetState(UT_KEY(CF_GetChunkListHead));
    txn->flags.com.q_index = CF_QueueIdx_TX;
    txn->chunks            = &cl;
    UtAssert_VOIDCALL(CF_CFDP_RecycleTransaction(txn));
    UtAssert_ADDRESS_EQ(txn->chunks, &cl); /* not cleaned */

    /* Dupe call: history already freed */
    UtAssert_VOIDCALL(CF_CFDP_RecycleTransaction(txn));

    /* Invalid - channel not known */
    UT_ResetState(UT_KEY(CF_GetChannelFromTxn));
    UtAssert_VOIDCALL(CF_CFDP_RecycleTransaction(txn));
}

void Test_CF_CFDP_SetTxnStatus(void)
{
    /* Test case for:
     * void CF_CFDP_SetTxnStatus(CF_Transaction_t *txn, CF_TxnStatus_t txn_stat)
     */

    CF_Transaction_t *txn;

    /* nominal call */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_NO_ERROR));
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_NO_ERROR);

    /* set an error */
    UtAssert_VOIDCALL(CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_FILESTORE_REJECTION));
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);

    /* confirm errors are "sticky"  */
    UtAssert_VOIDCALL(CF_CFDP_SetTxnStatus(txn, CF_TxnStatus_NO_ERROR));
    UtAssert_INT32_EQ(txn->history->txn_stat, CF_TxnStatus_FILESTORE_REJECTION);
}

void Test_CF_CFDP_SendEotPkt(void)
{
    CF_EotPacket_t  PktBuf;
    CF_EotPacket_t *PktBufPtr;

    CF_Transaction_t *txn;
    CF_Playback_t     pb;

    memset(&pb, 0, sizeof(pb));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);

    /* Test case where CF_EotPktBuf_t is NULL */
    UtAssert_VOIDCALL(CF_CFDP_SendEotPkt(txn));

    /* Verify results */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 0);
    UtAssert_STUB_COUNT(CFE_SB_TimeStampMsg, 0);
    UtAssert_STUB_COUNT(CFE_SB_TransmitBuffer, 0);

    /* setup for a call to CFE_SB_AllocateMessageBuffer() */
    PktBufPtr = &PktBuf;
    memset(PktBufPtr, 0, sizeof(*PktBufPtr));
    UT_SetDataBuffer(UT_KEY(CFE_SB_AllocateMessageBuffer), &PktBufPtr, sizeof(PktBufPtr), true);

    /* Execute the function being tested */
    /* nominal call */
    UtAssert_VOIDCALL(CF_CFDP_SendEotPkt(txn));

    /* Verify results */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_SB_TimeStampMsg, 1);
    UtAssert_STUB_COUNT(CFE_SB_TransmitBuffer, 1);
}

void Test_CF_CFDP_DisableEngine(void)
{
    /* Test case for:
     * void CF_CFDP_DisableEngine(void)
     */

    /* nominal call */
    CF_AppData.engine.enabled = true;
    UtAssert_VOIDCALL(CF_CFDP_DisableEngine());
    UtAssert_STUB_COUNT(CFE_SB_DeletePipe, CF_NUM_CHANNELS);
    UtAssert_BOOL_FALSE(CF_AppData.engine.enabled);

    /* nominal call with playbacks and polls active */
    CF_AppData.engine.channels[UT_CFDP_CHANNEL].playback[0].busy = true;
    OS_DirectoryOpen(&CF_AppData.engine.channels[UT_CFDP_CHANNEL].playback[0].dir_id, "ut");
    CF_AppData.engine.channels[UT_CFDP_CHANNEL].poll[0].pb.busy = true;
    OS_DirectoryOpen(&CF_AppData.engine.channels[UT_CFDP_CHANNEL].poll[0].pb.dir_id, "ut");
    UtAssert_VOIDCALL(CF_CFDP_DisableEngine());
    UtAssert_STUB_COUNT(OS_DirectoryClose, 2);
}

void Test_CF_CFDP_CloseFiles(void)
{
    /* Test case for:
     * int CF_CFDP_CloseFiles(CF_CListNode_t *node, void *context)
     */
    CF_Transaction_t *txn;

    /* nominal call, no file */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    UtAssert_INT32_EQ(CF_CFDP_CloseFiles(&txn->cl_node, NULL), CF_CLIST_CONT);

    /* nominal call, w/ file */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->fd = OS_ObjectIdFromInteger(1);
    UtAssert_INT32_EQ(CF_CFDP_CloseFiles(&txn->cl_node, NULL), CF_CLIST_CONT);
}

void Test_CF_CFDP_CancelTransaction(void)
{
    /* Test case for:
     * void CF_CFDP_CancelTransaction(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    /* nominal; cover both "flags.com.canceled" branches in here */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->history->dir       = CF_Direction_TX;
    txn->flags.com.canceled = true;
    UtAssert_VOIDCALL(CF_CFDP_CancelTransaction(txn));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_TX, NULL, NULL, NULL, &txn, NULL);
    txn->history->dir       = CF_Direction_TX;
    txn->flags.com.canceled = false;
    UtAssert_VOIDCALL(CF_CFDP_CancelTransaction(txn));

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, NULL, NULL, NULL, &txn, NULL);
    txn->history->dir       = CF_Direction_RX;
    txn->flags.com.canceled = false;
    UtAssert_VOIDCALL(CF_CFDP_CancelTransaction(txn));

    /* invalid direction */
    txn->history->dir       = CF_Direction_NUM;
    txn->flags.com.canceled = false;
    UtAssert_VOIDCALL(CF_CFDP_CancelTransaction(txn));
}

void Test_CF_CFDP_ArmInactTimer(void)
{
    /* Test Case For:
     * void CF_CFDP_ArmInactTimer(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, NULL);
    UtAssert_VOIDCALL(CF_CFDP_ArmAckTimer(txn));
    UtAssert_BOOL_TRUE(txn->flags.com.ack_timer_armed);
}

void Test_CF_CFDP_CheckAckNakCount(void)
{
    /* Test Case For:
     * bool CF_CFDP_CheckAckNakCount(CF_Transaction_t *txn, uint8 *counter)
     */
    CF_Transaction_t *txn;
    CF_ConfigTable_t *config;
    uint8             counter;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    config->chan[txn->chan_num].ack_limit = 10;

    counter = 9;
    UtAssert_BOOL_TRUE(CF_CFDP_CheckAckNakCount(txn, &counter));
    UtAssert_UINT8_EQ(counter, 10);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    counter           = config->chan[txn->chan_num].ack_limit;
    txn->history->dir = CF_Direction_RX;
    UtAssert_BOOL_FALSE(CF_CFDP_CheckAckNakCount(txn, &counter));
    UtAssert_UINT8_EQ(counter, config->chan[txn->chan_num].ack_limit);
    UT_CF_AssertEventID(CF_CFDP_R_ACK_LIMIT_ERR_EID);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, NULL, NULL, &txn, &config);
    counter           = config->chan[txn->chan_num].ack_limit;
    txn->history->dir = CF_Direction_TX;
    UtAssert_BOOL_FALSE(CF_CFDP_CheckAckNakCount(txn, &counter));
    UtAssert_UINT8_EQ(counter, config->chan[txn->chan_num].ack_limit);
    UT_CF_AssertEventID(CF_CFDP_S_ACK_LIMIT_ERR_EID);
}

void Test_CF_CFDP_DispatchRecv(void)
{
    /* Test Case For:
     * CF_CFDP_DispatchRecv()
     */
}

void Test_CF_CFDP_AllocChunkList(void)
{
    /* Test Case For:
     * void CF_CFDP_AllocChunkList(CF_Transaction_t *txn)
     */
    CF_Transaction_t  txn;
    CF_History_t      hist;
    CF_ChunkWrapper_t list_node;
    CF_CListNode_t *  list_ptr;

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    memset(&list_node, 0, sizeof(list_node));

    list_ptr = NULL;
    UT_SetHandlerFunction(UT_KEY(CF_GetChunkListHead), UT_AltHandler_GenericPointerReturn, &list_ptr);
    UT_SetHandlerFunction(UT_KEY(CF_CList_Pop), UT_AltHandler_GenericPointerReturn, &list_node.cl_node);

    txn.history = &hist;
    hist.dir    = CF_Direction_TX;

    UtAssert_VOIDCALL(CF_CFDP_AllocChunkList(&txn));

    UtAssert_NULL(txn.chunks);
    UT_CF_AssertEventID(CF_CFDP_NO_CHUNKLIST_AVAIL_EID);
    UtAssert_UINT8_EQ(hist.txn_stat, CF_TxnStatus_NO_RESOURCE);

    list_ptr = &list_node.cl_node;
    UtAssert_VOIDCALL(CF_CFDP_AllocChunkList(&txn));
    UtAssert_ADDRESS_EQ(txn.chunks, &list_node);
}

void Test_CF_CFDP_SetupTxTransaction(void)
{
    /* Test Case For:
     * void CF_CFDP_SetupTxTransaction(CF_Transaction_t *txn)
     */
    CF_Transaction_t  txn;
    CF_History_t      hist;
    CF_ConfigTable_t  config;
    CF_CListNode_t *  list_ptr;
    CF_ChunkWrapper_t chunks;

    memset(&config, 0, sizeof(config));
    memset(&chunks, 0, sizeof(chunks));

    CF_AppData.config_table = &config;
    list_ptr                = NULL;
    UT_SetHandlerFunction(UT_KEY(CF_GetChunkListHead), UT_AltHandler_GenericPointerReturn, &list_ptr);

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    txn.history = &hist;

    CF_CFDP_SetTxnStatus(&txn, CF_TxnStatus_PROTOCOL_ERROR);
    CF_AppData.hk.Payload.channel_hk[txn.chan_num].q_size[txn.flags.com.q_index] = 1;
    UtAssert_VOIDCALL(CF_CFDP_SetupTxTransaction(&txn));
    UtAssert_STUB_COUNT(CF_CFDP_S_Init, 0);

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    txn.chunks  = &chunks;
    txn.history = &hist;

    CF_AppData.hk.Payload.channel_hk[txn.chan_num].q_size[txn.flags.com.q_index] = 1;
    UtAssert_VOIDCALL(CF_CFDP_SetupTxTransaction(&txn));
    UtAssert_STUB_COUNT(CF_CFDP_S_Init, 1);
}

void Test_CF_CFDP_TxnStatus(void)
{
    CF_Transaction_t txn;

    /* Check that it is safe to call these routines on a wiped txn */
    memset(&txn, 0, sizeof(txn));
    UtAssert_VOIDCALL(CF_CFDP_GetTxnStatus(&txn));
    UtAssert_VOIDCALL(CF_CFDP_SetTxnStatus(&txn, CF_TxnStatus_ACK_LIMIT_NO_EOF));
}

void Test_CF_CFDP_SetupRxTransaction(void)
{
    /* Test Case For:
     * void CF_CFDP_SetupRxTransaction(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
     */
    CF_Transaction_t       txn;
    CF_Logical_PduBuffer_t ph;
    CF_History_t           hist;
    CF_ChunkWrapper_t      chunks;

    CF_ConfigTable_t config;
    CF_CListNode_t * list_ptr;

    memset(&config, 0, sizeof(config));
    memset(&ph, 0, sizeof(ph));
    memset(&chunks, 0, sizeof(chunks));

    CF_AppData.config_table = &config;
    list_ptr                = NULL;
    UT_SetHandlerFunction(UT_KEY(CF_GetChunkListHead), UT_AltHandler_GenericPointerReturn, &list_ptr);

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    txn.history = &hist;

    CF_CFDP_SetTxnStatus(&txn, CF_TxnStatus_PROTOCOL_ERROR);
    CF_AppData.hk.Payload.channel_hk[txn.chan_num].q_size[txn.flags.com.q_index] = 1;
    UtAssert_VOIDCALL(CF_CFDP_SetupRxTransaction(&txn, &ph));
    UtAssert_NULL(txn.chunks);
    UtAssert_UINT32_EQ(txn.state, CF_TxnState_HOLD);

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    txn.history = &hist;
    txn.chunks  = &chunks;

    CF_AppData.hk.Payload.channel_hk[txn.chan_num].q_size[txn.flags.com.q_index] = 1;
    ph.pdu_header.txm_mode                                                       = 1;
    UtAssert_VOIDCALL(CF_CFDP_SetupRxTransaction(&txn, &ph));
    UtAssert_UINT32_EQ(txn.state, CF_TxnState_R1);
    UtAssert_STUB_COUNT(CF_CFDP_R_Init, 1);

    memset(&txn, 0, sizeof(txn));
    memset(&hist, 0, sizeof(hist));
    txn.history = &hist;
    txn.chunks  = &chunks;

    CF_AppData.hk.Payload.channel_hk[txn.chan_num].q_size[txn.flags.com.q_index] = 1;
    ph.pdu_header.txm_mode                                                       = 0;
    UtAssert_VOIDCALL(CF_CFDP_SetupRxTransaction(&txn, &ph));
    UtAssert_UINT32_EQ(txn.state, CF_TxnState_R2);
    UtAssert_STUB_COUNT(CF_CFDP_R_Init, 2);
}

void Test_CF_CFDP_ReceivePdu(void)
{
    /* Test Case For:
     * void CF_CFDP_ReceivePdu(CF_Channel_t *chan, CF_Logical_PduBuffer_t *ph)
     */
    CF_Channel_t *          chan;
    CF_Logical_PduBuffer_t *ph;
    CF_Transaction_t *      txn;
    CF_ConfigTable_t *      config;
    CF_ChunkWrapper_t       chunk_wrap;

    memset(&chunk_wrap, 0, sizeof(chunk_wrap));

    /*
     *  - CF_CFDP_RecvPh() succeeds
     *  - CF_FindTransactionBySequenceNumber() returns non-NULL
     */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &chan, NULL, &txn, &config);
    txn->state = CF_TxnState_R2;
    UT_SetHandlerFunction(UT_KEY(CF_FindTransactionBySequenceNumber), UT_AltHandler_GenericPointerReturn, txn);
    UtAssert_VOIDCALL(CF_CFDP_ReceivePdu(chan, ph));
    UtAssert_STUB_COUNT(CF_CFDP_RxStateDispatch, 1);           /* should be dispatched */
    UT_ResetState(UT_KEY(CF_FindTransactionBySequenceNumber)); /* clears it */

    /* recv but not the correct destination_eid */
    UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &chan, NULL, &txn, &config);
    txn->state                     = CF_TxnState_R2;
    config->local_eid              = 123;
    ph->pdu_header.destination_eid = ~config->local_eid;
    UtAssert_VOIDCALL(CF_CFDP_ReceivePdu(chan, ph));
    UT_CF_AssertEventID(CF_CFDP_INVALID_DST_ERR_EID);

    /* recv correct destination_eid  */
    UT_ResetState(UT_KEY(CF_CFDP_R_Init)), UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &chan, NULL, &txn, &config);
    UT_SetHandlerFunction(UT_KEY(CF_FindUnusedTransaction), UT_AltHandler_GenericPointerReturn, txn);
    txn->chunks                    = &chunk_wrap;
    txn->state                     = CF_TxnState_R1;
    config->local_eid              = 123;
    ph->pdu_header.destination_eid = config->local_eid;
    UtAssert_VOIDCALL(CF_CFDP_ReceivePdu(chan, ph));
    UtAssert_STUB_COUNT(CF_CFDP_R_Init, 1);

    /* failure in CF_CFDP_StartRxTransaction */
    UT_ResetState(UT_KEY(CF_FindUnusedTransaction)),
        UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &chan, NULL, &txn, &config);
    txn->state                     = CF_TxnState_R1;
    config->local_eid              = 123;
    ph->pdu_header.destination_eid = config->local_eid;
    UtAssert_VOIDCALL(CF_CFDP_ReceivePdu(chan, ph));
    UT_CF_AssertEventID(CF_CFDP_RX_DROPPED_ERR_EID);

    /* failure in RecvPh */
    UT_ResetState(UT_KEY(CF_FindTransactionBySequenceNumber)),
        UT_CFDP_SetupBasicTestState(UT_CF_Setup_RX, &ph, &chan, NULL, &txn, &config);
    UT_SetDeferredRetcode(UT_KEY(CF_CFDP_DecodeHeader), 1, -1);
    UtAssert_VOIDCALL(CF_CFDP_ReceivePdu(chan, ph));
    UtAssert_STUB_COUNT(CF_FindTransactionBySequenceNumber, 0); /* should NOT be called */
}

void Test_CF_CFDP_StartFirstPending(void)
{
    /* Test Case For:
     * bool CF_CFDP_StartFirstPending(CF_Channel_t *chan)
     */
    CF_Channel_t *    chan;
    CF_Transaction_t *txn;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, &txn, NULL);

    /* nothing pending */
    UtAssert_BOOL_FALSE(CF_CFDP_StartFirstPending(chan));

    txn->flags.com.q_index     = CF_QueueIdx_PEND;
    chan->qs[CF_QueueIdx_PEND] = &txn->cl_node;

    /* this dequeues */
    CF_AppData.hk.Payload.channel_hk[txn->chan_num].q_size[txn->flags.com.q_index] = 1;

    UtAssert_BOOL_TRUE(CF_CFDP_StartFirstPending(chan));
}

void Test_CF_CFDP_CompleteTick(void)
{
    /* Test Case For:
     * void CF_CFDP_CompleteTick(CF_Transaction_t *txn)
     */
    CF_Transaction_t *txn;
    CF_Transaction_t  txn2;
    CF_Channel_t *    chan;

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_GetChannelFromTxn), UT_AltHandler_GenericPointerReturn, chan);
    chan->tx_blocked  = true;
    chan->tick_resume = NULL;
    UtAssert_VOIDCALL(CF_CFDP_CompleteTick(txn));
    UtAssert_ADDRESS_EQ(chan->tick_resume, txn);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_GetChannelFromTxn), UT_AltHandler_GenericPointerReturn, chan);
    chan->tx_blocked  = true;
    chan->tick_resume = &txn2;
    UtAssert_VOIDCALL(CF_CFDP_CompleteTick(txn));
    UtAssert_ADDRESS_EQ(chan->tick_resume, &txn2);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, &txn, NULL);
    UT_SetHandlerFunction(UT_KEY(CF_GetChannelFromTxn), UT_AltHandler_GenericPointerReturn, chan);
    chan->tx_blocked  = false;
    chan->tick_resume = NULL;
    UtAssert_VOIDCALL(CF_CFDP_CompleteTick(txn));
    UtAssert_NULL(chan->tick_resume);

    UT_CFDP_SetupBasicTestState(UT_CF_Setup_NONE, NULL, &chan, NULL, &txn, NULL);
    UT_ResetState(UT_KEY(CF_GetChannelFromTxn));
    chan->tx_blocked  = false;
    chan->tick_resume = NULL;
    UtAssert_VOIDCALL(CF_CFDP_CompleteTick(txn));
    UtAssert_NULL(chan->tick_resume);
}

void Test_CF_CFDP_GetTempName(void)
{
    /* Test Case For:
     * void CF_CFDP_GetTempName(const CF_History_t *hist, char *FileNameBuf, size_t FileNameSize)
     */
    CF_History_t     hist;
    CF_ConfigTable_t config;
    char             FileNameBuf[12];

    memset(&hist, 0, sizeof(hist));
    memset(&config, 0, sizeof(config));

    CF_AppData.config_table = &config;
    strcpy(config.tmp_dir, "ut");
    hist.src_eid = 3;
    hist.seq_num = 4;
    UtAssert_VOIDCALL(CF_CFDP_GetTempName(&hist, FileNameBuf, sizeof(FileNameBuf)));
    UtAssert_STRINGBUF_EQ(FileNameBuf, sizeof(FileNameBuf), "ut/3_4.tmp", -1);
}

void Test_CF_CFDP_GetMoveTarget(void)
{
    /* Test Case For:
     * const char *CF_CFDP_GetMoveTarget(const char *dest_dir, const char *subject_file, char *dest_buf, size_t
     * dest_size)
     */
    char FileNameBuf[6];

    /* nominal, no dest dir */
    UtAssert_NULL(CF_CFDP_GetMoveTarget(NULL, "ut", FileNameBuf, sizeof(FileNameBuf)));
    UtAssert_NULL(CF_CFDP_GetMoveTarget("", "ut", FileNameBuf, sizeof(FileNameBuf)));

    /* nominal with dest dir */
    UtAssert_NOT_NULL(CF_CFDP_GetMoveTarget("d", "ut", FileNameBuf, sizeof(FileNameBuf)));
    UtAssert_STRINGBUF_EQ(FileNameBuf, sizeof(FileNameBuf), "d/ut", -1);

    /* nominal with dest dir */
    UtAssert_NOT_NULL(CF_CFDP_GetMoveTarget("d", "b/ut", FileNameBuf, sizeof(FileNameBuf)));
    UtAssert_STRINGBUF_EQ(FileNameBuf, sizeof(FileNameBuf), "d/ut", -1);

    /* truncation */
    UtAssert_NOT_NULL(CF_CFDP_GetMoveTarget("d", "longname", FileNameBuf, sizeof(FileNameBuf)));
    UtAssert_STRINGBUF_EQ(FileNameBuf, sizeof(FileNameBuf), "d/lo$", -1);

    /* truncation */
    UtAssert_NOT_NULL(CF_CFDP_GetMoveTarget("d", "longname", FileNameBuf, 2));
    UtAssert_STRINGBUF_EQ(FileNameBuf, sizeof(FileNameBuf), "d", -1);
}

/*******************************************************************************
**
**  cf_cfdp_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    UtTest_Add(Test_CF_CFDP_TxnStatus, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_TxnStatus");
    UtTest_Add(Test_CF_CFDP_InitEngine, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_InitEngine");
    UtTest_Add(Test_CF_CFDP_CycleEngine, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_CycleEngine");
    UtTest_Add(Test_CF_CFDP_ProcessPlaybackDirectory, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ProcessPlaybackDirectory");
    UtTest_Add(Test_CF_CFDP_ProcessPollingDirectories, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ProcessPollingDirectories");
    UtTest_Add(Test_CF_CFDP_S_Tick_NewData, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "Test_CF_CFDP_S_Tick_NewData");
    UtTest_Add(Test_CF_CFDP_DoTick, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_DoTick");
    UtTest_Add(Test_CF_CFDP_TickTransactions, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_TickTransactions");
    UtTest_Add(Test_CF_CFDP_FinishTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_FinishTransaction");
    UtTest_Add(Test_CF_CFDP_RecycleTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_RecycleTransaction");
    UtTest_Add(Test_CF_CFDP_SetTxnStatus, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SetTxnStatus");
    UtTest_Add(Test_CF_CFDP_SendEotPkt, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "Test_CF_CFDP_SendEotPkt");
    UtTest_Add(Test_CF_CFDP_CancelTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CancelTransaction");
    UtTest_Add(Test_CF_CFDP_DisableEngine, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_DisableEngine");
    UtTest_Add(Test_CF_CFDP_CloseFiles, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_CloseFiles");
    UtTest_Add(Test_CF_CFDP_CancelTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CancelTransaction");
    UtTest_Add(Test_CF_CFDP_TxFile, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_TxFile");
    UtTest_Add(Test_CF_CFDP_StartRxTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_StartRxTransaction");
    UtTest_Add(Test_CF_CFDP_PlaybackDir, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_PlaybackDir");
    UtTest_Add(Test_CF_CFDP_ArmAckTimer, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_ArmAckTimer");

    UtTest_Add(Test_CF_CFDP_CF_CFDP_EncodeStart, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CF_CFDP_EncodeStart");
    UtTest_Add(Test_CF_CFDP_CF_CFDP_DecodeStart, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_CF_CFDP_DecodeStart");

    UtTest_Add(Test_CF_CFDP_RecvDrop, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvDrop");
    UtTest_Add(Test_CF_CFDP_RecvHold, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvHold");
    UtTest_Add(Test_CF_CFDP_RecvInit, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_RecvInit");
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
    UtTest_Add(Test_CF_CFDP_AppendTlv, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_AppendTlv");
    UtTest_Add(Test_CF_CFDP_SendNak, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_SendNak");

    UtTest_Add(Test_CF_CFDP_ArmInactTimer, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_ArmInactTimer");
    UtTest_Add(Test_CF_CFDP_CheckAckNakCount, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_CheckAckNakCount");
    UtTest_Add(Test_CF_CFDP_DispatchRecv, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_DispatchRecv");
    UtTest_Add(Test_CF_CFDP_AllocChunkList, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_AllocChunkList");
    UtTest_Add(Test_CF_CFDP_SetupTxTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_SetupTxTransaction");
    UtTest_Add(Test_CF_CFDP_SetupRxTransaction, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_SetupRxTransaction");
    UtTest_Add(Test_CF_CFDP_ReceivePdu, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_ReceivePdu");
    UtTest_Add(Test_CF_CFDP_StartFirstPending, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "CF_CFDP_StartFirstPending");
    UtTest_Add(Test_CF_CFDP_CompleteTick, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_CompleteTick");
    UtTest_Add(Test_CF_CFDP_GetTempName, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_GetTempName");
    UtTest_Add(Test_CF_CFDP_GetMoveTarget, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown, "CF_CFDP_GetMoveTarget");
}
