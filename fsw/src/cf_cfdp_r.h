/************************************************************************
** File: cf_cfdp_r.h
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
**
*************************************************************************/

#ifndef CF_CFDP_R_H
#define CF_CFDP_R_H

#include "cf_cfdp.h"

typedef struct
{
    CF_Transaction_t    *t;
    CF_Logical_PduNak_t *nak;
} CF_GapComputeArgs_t;

void CF_CFDP_R1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R2_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R_Tick(CF_Transaction_t *t, int *cont);
void CF_CFDP_R_Cancel(CF_Transaction_t *t);
void CF_CFDP_R_Init(CF_Transaction_t *t);

void CF_CFDP_R2_SetCc(CF_Transaction_t *t, CF_CFDP_ConditionCode_t cc);
void CF_CFDP_R1_Reset(CF_Transaction_t *t);
void CF_CFDP_R2_Reset(CF_Transaction_t *t);
int  CF_CFDP_R_CheckCrc(CF_Transaction_t *t, uint32 expected_crc);
void CF_CFDP_R2_Complete(CF_Transaction_t *t, int ok_to_send_nak);
int  CF_CFDP_R_ProcessFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
int  CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R1_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R2_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R1_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R2_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *c, void *opaque);
int  CF_CFDP_R_SubstateSendNak(CF_Transaction_t *t);
int  CF_CFDP_R2_CalcCrcChunk(CF_Transaction_t *t);
int  CF_CFDP_R2_SubstateSendFin(CF_Transaction_t *t);
void CF_CFDP_R2_Recv_fin_ack(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R2_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_R_SendInactivityEvent(CF_Transaction_t *t);

#endif /* CF_CFDP_R_H */
