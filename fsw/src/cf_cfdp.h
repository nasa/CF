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

#ifndef CF_CFDP_H
#define CF_CFDP_H

#include "cf_cfdp_types.h"

typedef struct CF_CFDP_CycleTx_args
{
    CF_Channel_t *c;
    int           ran_one;
} CF_CFDP_CycleTx_args_t;

typedef struct CF_CFDP_Tick_args
{
    CF_Channel_t *c;                       /* IN param */
    void (*fn)(CF_Transaction_t *, int *); /* IN param */
    int early_exit;                        /* OUT param */
    int cont;                              /* if 1, then re-traverse the list */
} CF_CFDP_Tick_args_t;

void CF_CFDP_EncodeStart(CF_EncoderState_t *penc, void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size);
void CF_CFDP_DecodeStart(CF_DecoderState_t *pdec, const void *msgbuf, CF_Logical_PduBuffer_t *ph, size_t encap_hdr_size,
                         size_t total_size);

/* engine execution functions */
void  CF_CFDP_ResetTransaction(CF_Transaction_t *t, int keep_history);
int32 CF_CFDP_InitEngine(void);
void  CF_CFDP_CycleEngine(void);
void  CF_CFDP_DisableEngine(void);

/* ground commands into the engine */
/* returns NULL on err */
int32 CF_CFDP_TxFile(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                     uint8 chan, uint8 priority, CF_EntityId_t dest_id);
int32 CF_CFDP_PlaybackDir(const char *src_filename, const char *dst_filename, CF_CFDP_Class_t cfdp_class, uint8 keep,
                          uint8 chan, uint8 priority, uint16 dest_id);

/* PDU send functions */
/* CF_CFDP_ConstructPduHeader sets length of 0. Must set it after building packet */
CF_Logical_PduBuffer_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *t, CF_CFDP_FileDirective_t directive_code,
                                                   CF_EntityId_t src_eid, CF_EntityId_t dst_eid, bool towards_sender,
                                                   CF_TransactionSeq_t tsn, bool silent);
CF_SendRet_t            CF_CFDP_SendMd(CF_Transaction_t *t);
CF_SendRet_t            CF_CFDP_SendFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
CF_SendRet_t            CF_CFDP_SendEof(CF_Transaction_t *t);
/* NOTE: CF_CFDP_SendAck() takes a CF_TransactionSeq_t instead of getting it from transaction history because
 * of the special case where a FIN-ACK must be sent for an unknown transaction. It's better for
 * long term maintenance to not build an incomplete CF_History_t for it.
 */
CF_SendRet_t CF_CFDP_SendAck(CF_Transaction_t *t, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn);
CF_SendRet_t CF_CFDP_SendFin(CF_Transaction_t *t, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc);
CF_SendRet_t CF_CFDP_SendNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

void CF_CFDP_AppendTlv(CF_Logical_TlvList_t *ptlv_list, CF_CFDP_TlvType_t tlv_type);

/* PDU receive functions */
/* returns 0 on success */
int CF_CFDP_RecvPh(uint8 chan_num, CF_Logical_PduBuffer_t *ph);
int CF_CFDP_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
int CF_CFDP_RecvFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
int CF_CFDP_RecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
int CF_CFDP_RecvAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
int CF_CFDP_RecvFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
int CF_CFDP_RecvNak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

void CF_CFDP_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

void CF_CFDP_CancelTransaction(CF_Transaction_t *t);
void CF_CFDP_InitTxnTxFile(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority);

/* functions to handle LVs (length-value, cfdp spec) */
/* returns number of bytes copied, or -1 on error */
extern int CF_CFDP_CopyStringFromLV(char *buf, size_t buf_maxsz, const CF_Logical_Lv_t *src_lv);

extern void CF_CFDP_ArmAckTimer(CF_Transaction_t *t);

void CF_CFDP_RecvDrop(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_RecvIdle(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

int CF_CFDP_CloseFiles(CF_CListNode_t *n, void *context);

void CF_CFDP_CycleTx(CF_Channel_t *c);
int  CF_CFDP_CycleTxFirstActive(CF_CListNode_t *node, void *context);
void CF_CFDP_TickTransactions(CF_Channel_t *c);
void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *c, CF_Playback_t *p);
void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *c);
int  CF_CFDP_DoTick(CF_CListNode_t *node, void *context);

#endif /* !CF_CFDP_H */
