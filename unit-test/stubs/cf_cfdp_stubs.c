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
 * Auto-Generated stub implementations for functions defined in cf_cfdp header
 */

#include "cf_cfdp.h"
#include "utgenstub.h"

void UT_DefaultHandler_CF_CFDP_CancelTransaction(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CFDP_ConstructPduHeader(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CFDP_PlaybackDir(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CFDP_ResetTransaction(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CFDP_TxFile(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_AppendTlv()
 * ----------------------------------------------------
 */
void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type)
{
    UT_GenStub_AddParam(CF_CFDP_AppendTlv, CF_Logical_TlvList_t *, ptlv_list);
    UT_GenStub_AddParam(CF_CFDP_AppendTlv, CF_CFDP_TlvType_t, tlv_type);

    UT_GenStub_Execute(CF_CFDP_AppendTlv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_ArmAckTimer()
 * ----------------------------------------------------
 */
void CF_CFDP_ArmAckTimer(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_ArmAckTimer, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_ArmAckTimer, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_CancelTransaction()
 * ----------------------------------------------------
 */
void CF_CFDP_CancelTransaction(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_CancelTransaction, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_CancelTransaction, Basic, UT_DefaultHandler_CF_CFDP_CancelTransaction);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_CloseFiles()
 * ----------------------------------------------------
 */
int CF_CFDP_CloseFiles(CF_CListNode_t *n, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_CloseFiles, int);

    UT_GenStub_AddParam(CF_CFDP_CloseFiles, CF_CListNode_t *, n);
    UT_GenStub_AddParam(CF_CFDP_CloseFiles, void *, context);

    UT_GenStub_Execute(CF_CFDP_CloseFiles, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_CloseFiles, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_ConstructPduHeader()
 * ----------------------------------------------------
 */
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *t, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_ConstructPduHeader, CF_Logical_PduBuffer_t *);

    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, const CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, CF_CFDP_FileDirective_t, directive_code);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, CF_EntityId_t, src_eid);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, CF_EntityId_t, dst_eid);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, bool, towards_sender);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, CF_TransactionSeq_t, tsn);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, bool, silent);

    UT_GenStub_Execute(CF_CFDP_ConstructPduHeader, Basic, UT_DefaultHandler_CF_CFDP_ConstructPduHeader);

    return UT_GenStub_GetReturnValue(CF_CFDP_ConstructPduHeader, CF_Logical_PduBuffer_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_CopyStringFromLV()
 * ----------------------------------------------------
 */
int CF_CFDP_CopyStringFromLV(char *buf, size_t buf_maxsz, const CF_Logical_Lv_t *src_lv)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_CopyStringFromLV, int);

    UT_GenStub_AddParam(CF_CFDP_CopyStringFromLV, char *, buf);
    UT_GenStub_AddParam(CF_CFDP_CopyStringFromLV, size_t, buf_maxsz);
    UT_GenStub_AddParam(CF_CFDP_CopyStringFromLV, const CF_Logical_Lv_t *, src_lv);

    UT_GenStub_Execute(CF_CFDP_CopyStringFromLV, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_CopyStringFromLV, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_CycleEngine()
 * ----------------------------------------------------
 */
void CF_CFDP_CycleEngine(void)
{

    UT_GenStub_Execute(CF_CFDP_CycleEngine, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_CycleTx()
 * ----------------------------------------------------
 */
void CF_CFDP_CycleTx(CF_Channel_t *c)
{
    UT_GenStub_AddParam(CF_CFDP_CycleTx, CF_Channel_t *, c);

    UT_GenStub_Execute(CF_CFDP_CycleTx, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_CycleTxFirstActive()
 * ----------------------------------------------------
 */
int CF_CFDP_CycleTxFirstActive(CF_CListNode_t *node, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_CycleTxFirstActive, int);

    UT_GenStub_AddParam(CF_CFDP_CycleTxFirstActive, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_CFDP_CycleTxFirstActive, void *, context);

    UT_GenStub_Execute(CF_CFDP_CycleTxFirstActive, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_CycleTxFirstActive, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DecodeStart()
 * ----------------------------------------------------
 */
void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, const void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size)
{
    UT_GenStub_AddParam(CF_CFDP_DecodeStart, CF_DecoderState_t *, pdec);
    UT_GenStub_AddParam(CF_CFDP_DecodeStart, const void *, msgbuf);
    UT_GenStub_AddParam(CF_CFDP_DecodeStart, CF_Logical_PduBuffer_t *, ph);
    UT_GenStub_AddParam(CF_CFDP_DecodeStart, size_t, encap_hdr_size);
    UT_GenStub_AddParam(CF_CFDP_DecodeStart, size_t, total_size);

    UT_GenStub_Execute(CF_CFDP_DecodeStart, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DisableEngine()
 * ----------------------------------------------------
 */
void CF_CFDP_DisableEngine(void)
{

    UT_GenStub_Execute(CF_CFDP_DisableEngine, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DispatchRecv()
 * ----------------------------------------------------
 */
void CF_CFDP_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_DispatchRecv, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_DispatchRecv, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_DispatchRecv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_DoTick()
 * ----------------------------------------------------
 */
int CF_CFDP_DoTick(CF_CListNode_t *node, void *context)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_DoTick, int);

    UT_GenStub_AddParam(CF_CFDP_DoTick, CF_CListNode_t *, node);
    UT_GenStub_AddParam(CF_CFDP_DoTick, void *, context);

    UT_GenStub_Execute(CF_CFDP_DoTick, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_DoTick, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_EncodeStart()
 * ----------------------------------------------------
 */
void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size)
{
    UT_GenStub_AddParam(CF_CFDP_EncodeStart, CF_EncoderState_t *, penc);
    UT_GenStub_AddParam(CF_CFDP_EncodeStart, void *, msgbuf);
    UT_GenStub_AddParam(CF_CFDP_EncodeStart, CF_Logical_PduBuffer_t *, ph);
    UT_GenStub_AddParam(CF_CFDP_EncodeStart, size_t, encap_hdr_size);
    UT_GenStub_AddParam(CF_CFDP_EncodeStart, size_t, total_size);

    UT_GenStub_Execute(CF_CFDP_EncodeStart, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_InitEngine()
 * ----------------------------------------------------
 */
int32 CF_CFDP_InitEngine(void)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_InitEngine, int32);

    UT_GenStub_Execute(CF_CFDP_InitEngine, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_InitEngine, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_InitTxnTxFile()
 * ----------------------------------------------------
 */
void CF_CFDP_InitTxnTxFile(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority)
{
    UT_GenStub_AddParam(CF_CFDP_InitTxnTxFile, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_InitTxnTxFile, CF_CFDP_Class_t, cfdp_class);
    UT_GenStub_AddParam(CF_CFDP_InitTxnTxFile, uint8, keep);
    UT_GenStub_AddParam(CF_CFDP_InitTxnTxFile, uint8, chan);
    UT_GenStub_AddParam(CF_CFDP_InitTxnTxFile, uint8, priority);

    UT_GenStub_Execute(CF_CFDP_InitTxnTxFile, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_PlaybackDir()
 * ----------------------------------------------------
 */
int32 CF_CFDP_PlaybackDir(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                          uint8 chan, uint8 priority, uint16 dest_id)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_PlaybackDir, int32);

    UT_GenStub_AddParam(CF_CFDP_PlaybackDir, const char *, src_filename);
    UT_GenStub_AddParam(CF_CFDP_PlaybackDir, const char *, dst_filename);
    UT_GenStub_AddParam(CF_CFDP_PlaybackDir, CF_CFDP_Class_t, cfdp_class);
    UT_GenStub_AddParam(CF_CFDP_PlaybackDir, uint8, keep);
    UT_GenStub_AddParam(CF_CFDP_PlaybackDir, uint8, chan);
    UT_GenStub_AddParam(CF_CFDP_PlaybackDir, uint8, priority);
    UT_GenStub_AddParam(CF_CFDP_PlaybackDir, uint16, dest_id);

    UT_GenStub_Execute(CF_CFDP_PlaybackDir, Basic, UT_DefaultHandler_CF_CFDP_PlaybackDir);

    return UT_GenStub_GetReturnValue(CF_CFDP_PlaybackDir, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_ProcessPlaybackDirectory()
 * ----------------------------------------------------
 */
void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *c, CF_Playback_t *p)
{
    UT_GenStub_AddParam(CF_CFDP_ProcessPlaybackDirectory, CF_Channel_t *, c);
    UT_GenStub_AddParam(CF_CFDP_ProcessPlaybackDirectory, CF_Playback_t *, p);

    UT_GenStub_Execute(CF_CFDP_ProcessPlaybackDirectory, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_ProcessPollingDirectories()
 * ----------------------------------------------------
 */
void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *c)
{
    UT_GenStub_AddParam(CF_CFDP_ProcessPollingDirectories, CF_Channel_t *, c);

    UT_GenStub_Execute(CF_CFDP_ProcessPollingDirectories, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvAck()
 * ----------------------------------------------------
 */
int CF_CFDP_RecvAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvAck, int);

    UT_GenStub_AddParam(CF_CFDP_RecvAck, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvAck, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvAck, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvAck, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvDrop()
 * ----------------------------------------------------
 */
void CF_CFDP_RecvDrop(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_RecvDrop, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvDrop, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvDrop, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvEof()
 * ----------------------------------------------------
 */
int CF_CFDP_RecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvEof, int);

    UT_GenStub_AddParam(CF_CFDP_RecvEof, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvEof, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvEof, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvEof, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvFd()
 * ----------------------------------------------------
 */
int CF_CFDP_RecvFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvFd, int);

    UT_GenStub_AddParam(CF_CFDP_RecvFd, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvFd, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvFd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvFd, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvFin()
 * ----------------------------------------------------
 */
int CF_CFDP_RecvFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvFin, int);

    UT_GenStub_AddParam(CF_CFDP_RecvFin, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvFin, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvFin, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvFin, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvIdle()
 * ----------------------------------------------------
 */
void CF_CFDP_RecvIdle(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_RecvIdle, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvIdle, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvIdle, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvMd()
 * ----------------------------------------------------
 */
int CF_CFDP_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvMd, int);

    UT_GenStub_AddParam(CF_CFDP_RecvMd, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvMd, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvMd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvMd, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvNak()
 * ----------------------------------------------------
 */
int CF_CFDP_RecvNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvNak, int);

    UT_GenStub_AddParam(CF_CFDP_RecvNak, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RecvNak, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvNak, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvNak, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RecvPh()
 * ----------------------------------------------------
 */
int CF_CFDP_RecvPh(uint8 chan_num, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvPh, int);

    UT_GenStub_AddParam(CF_CFDP_RecvPh, uint8, chan_num);
    UT_GenStub_AddParam(CF_CFDP_RecvPh, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_RecvPh, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvPh, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_ResetTransaction()
 * ----------------------------------------------------
 */
void CF_CFDP_ResetTransaction(CF_Transaction_t *t, int keep_history)
{
    UT_GenStub_AddParam(CF_CFDP_ResetTransaction, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_ResetTransaction, int, keep_history);

    UT_GenStub_Execute(CF_CFDP_ResetTransaction, Basic, UT_DefaultHandler_CF_CFDP_ResetTransaction);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_SendAck()
 * ----------------------------------------------------
 */
CF_SendRet_t CF_CFDP_SendAck(CF_Transaction_t *t, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_SendAck, CF_SendRet_t);

    UT_GenStub_AddParam(CF_CFDP_SendAck, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_SendAck, CF_CFDP_AckTxnStatus_t, ts);
    UT_GenStub_AddParam(CF_CFDP_SendAck, CF_CFDP_FileDirective_t, dir_code);
    UT_GenStub_AddParam(CF_CFDP_SendAck, CF_CFDP_ConditionCode_t, cc);
    UT_GenStub_AddParam(CF_CFDP_SendAck, CF_EntityId_t, peer_eid);
    UT_GenStub_AddParam(CF_CFDP_SendAck, CF_TransactionSeq_t, tsn);

    UT_GenStub_Execute(CF_CFDP_SendAck, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_SendAck, CF_SendRet_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_SendEof()
 * ----------------------------------------------------
 */
CF_SendRet_t CF_CFDP_SendEof(CF_Transaction_t *t)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_SendEof, CF_SendRet_t);

    UT_GenStub_AddParam(CF_CFDP_SendEof, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_SendEof, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_SendEof, CF_SendRet_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_SendFd()
 * ----------------------------------------------------
 */
CF_SendRet_t CF_CFDP_SendFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_SendFd, CF_SendRet_t);

    UT_GenStub_AddParam(CF_CFDP_SendFd, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_SendFd, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_SendFd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_SendFd, CF_SendRet_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_SendFin()
 * ----------------------------------------------------
 */
CF_SendRet_t CF_CFDP_SendFin(CF_Transaction_t *t, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_SendFin, CF_SendRet_t);

    UT_GenStub_AddParam(CF_CFDP_SendFin, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_SendFin, CF_CFDP_FinDeliveryCode_t, dc);
    UT_GenStub_AddParam(CF_CFDP_SendFin, CF_CFDP_FinFileStatus_t, fs);
    UT_GenStub_AddParam(CF_CFDP_SendFin, CF_CFDP_ConditionCode_t, cc);

    UT_GenStub_Execute(CF_CFDP_SendFin, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_SendFin, CF_SendRet_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_SendMd()
 * ----------------------------------------------------
 */
CF_SendRet_t CF_CFDP_SendMd(CF_Transaction_t *t)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_SendMd, CF_SendRet_t);

    UT_GenStub_AddParam(CF_CFDP_SendMd, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_SendMd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_SendMd, CF_SendRet_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_SendNak()
 * ----------------------------------------------------
 */
CF_SendRet_t CF_CFDP_SendNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_SendNak, CF_SendRet_t);

    UT_GenStub_AddParam(CF_CFDP_SendNak, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_SendNak, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_SendNak, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_SendNak, CF_SendRet_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_TickTransactions()
 * ----------------------------------------------------
 */
void CF_CFDP_TickTransactions(CF_Channel_t *c)
{
    UT_GenStub_AddParam(CF_CFDP_TickTransactions, CF_Channel_t *, c);

    UT_GenStub_Execute(CF_CFDP_TickTransactions, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_TxFile()
 * ----------------------------------------------------
 */
int32 CF_CFDP_TxFile(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                     uint8 chan, uint8 priority, CF_EntityId_t dest_id)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_TxFile, int32);

    UT_GenStub_AddParam(CF_CFDP_TxFile, const char *, src_filename);
    UT_GenStub_AddParam(CF_CFDP_TxFile, const char *, dst_filename);
    UT_GenStub_AddParam(CF_CFDP_TxFile, CF_CFDP_Class_t, cfdp_class);
    UT_GenStub_AddParam(CF_CFDP_TxFile, uint8, keep);
    UT_GenStub_AddParam(CF_CFDP_TxFile, uint8, chan);
    UT_GenStub_AddParam(CF_CFDP_TxFile, uint8, priority);
    UT_GenStub_AddParam(CF_CFDP_TxFile, CF_EntityId_t, dest_id);

    UT_GenStub_Execute(CF_CFDP_TxFile, Basic, UT_DefaultHandler_CF_CFDP_TxFile);

    return UT_GenStub_GetReturnValue(CF_CFDP_TxFile, int32);
}
