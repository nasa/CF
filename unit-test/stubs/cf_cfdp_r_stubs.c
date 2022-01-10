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
 *
 ************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cf_cfdp_r header
 */

#include "cf_cfdp_r.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R1_Recv()
 * ----------------------------------------------------
 */
void CF_CFDP_R1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R1_Recv, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R1_Recv, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R1_Recv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R1_Reset()
 * ----------------------------------------------------
 */
void CF_CFDP_R1_Reset(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_R1_Reset, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R1_Reset, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R1_SubstateRecvEof()
 * ----------------------------------------------------
 */
void CF_CFDP_R1_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R1_SubstateRecvEof, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R1_SubstateRecvEof, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R1_SubstateRecvEof, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R1_SubstateRecvFileData()
 * ----------------------------------------------------
 */
void CF_CFDP_R1_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R1_SubstateRecvFileData, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R1_SubstateRecvFileData, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R1_SubstateRecvFileData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_CalcCrcChunk()
 * ----------------------------------------------------
 */
int CF_CFDP_R2_CalcCrcChunk(CF_Transaction_t *t)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R2_CalcCrcChunk, int);

    UT_GenStub_AddParam(CF_CFDP_R2_CalcCrcChunk, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R2_CalcCrcChunk, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R2_CalcCrcChunk, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_Complete()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_Complete(CF_Transaction_t *t, int ok_to_send_nak)
{
    UT_GenStub_AddParam(CF_CFDP_R2_Complete, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R2_Complete, int, ok_to_send_nak);

    UT_GenStub_Execute(CF_CFDP_R2_Complete, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_GapCompute()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *c, void *opaque)
{
    UT_GenStub_AddParam(CF_CFDP_R2_GapCompute, const CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_CFDP_R2_GapCompute, const CF_Chunk_t *, c);
    UT_GenStub_AddParam(CF_CFDP_R2_GapCompute, void *, opaque);

    UT_GenStub_Execute(CF_CFDP_R2_GapCompute, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_Recv()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R2_Recv, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R2_Recv, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R2_Recv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_RecvMd()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_RecvMd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R2_RecvMd, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R2_RecvMd, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R2_RecvMd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_Recv_fin_ack()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_Recv_fin_ack(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R2_Recv_fin_ack, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R2_Recv_fin_ack, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R2_Recv_fin_ack, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_Reset()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_Reset(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_R2_Reset, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R2_Reset, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_SetCc()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_SetCc(CF_Transaction_t *t, CF_CFDP_ConditionCode_t cc)
{
    UT_GenStub_AddParam(CF_CFDP_R2_SetCc, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R2_SetCc, CF_CFDP_ConditionCode_t, cc);

    UT_GenStub_Execute(CF_CFDP_R2_SetCc, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_SubstateRecvEof()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R2_SubstateRecvEof, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R2_SubstateRecvEof, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R2_SubstateRecvEof, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_SubstateRecvFileData()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_SubstateRecvFileData(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R2_SubstateRecvFileData, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R2_SubstateRecvFileData, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R2_SubstateRecvFileData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_SubstateSendFin()
 * ----------------------------------------------------
 */
int CF_CFDP_R2_SubstateSendFin(CF_Transaction_t *t)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R2_SubstateSendFin, int);

    UT_GenStub_AddParam(CF_CFDP_R2_SubstateSendFin, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R2_SubstateSendFin, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R2_SubstateSendFin, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_Cancel()
 * ----------------------------------------------------
 */
void CF_CFDP_R_Cancel(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_R_Cancel, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R_Cancel, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_CheckCrc()
 * ----------------------------------------------------
 */
int CF_CFDP_R_CheckCrc(CF_Transaction_t *t, uint32 expected_crc)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_CheckCrc, int);

    UT_GenStub_AddParam(CF_CFDP_R_CheckCrc, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R_CheckCrc, uint32, expected_crc);

    UT_GenStub_Execute(CF_CFDP_R_CheckCrc, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_CheckCrc, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_Init()
 * ----------------------------------------------------
 */
void CF_CFDP_R_Init(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_R_Init, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R_Init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_ProcessFd()
 * ----------------------------------------------------
 */
int CF_CFDP_R_ProcessFd(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_ProcessFd, int);

    UT_GenStub_AddParam(CF_CFDP_R_ProcessFd, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R_ProcessFd, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R_ProcessFd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_ProcessFd, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_SendInactivityEvent()
 * ----------------------------------------------------
 */
void CF_CFDP_R_SendInactivityEvent(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_R_SendInactivityEvent, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R_SendInactivityEvent, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_SubstateRecvEof()
 * ----------------------------------------------------
 */
int CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_SubstateRecvEof, int);

    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvEof, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvEof, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R_SubstateRecvEof, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_SubstateRecvEof, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_SubstateSendNak()
 * ----------------------------------------------------
 */
int CF_CFDP_R_SubstateSendNak(CF_Transaction_t *t)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_SubstateSendNak, int);

    UT_GenStub_AddParam(CF_CFDP_R_SubstateSendNak, CF_Transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R_SubstateSendNak, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_SubstateSendNak, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_Tick()
 * ----------------------------------------------------
 */
void CF_CFDP_R_Tick(CF_Transaction_t *t, int *cont)
{
    UT_GenStub_AddParam(CF_CFDP_R_Tick, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R_Tick, int *, cont);

    UT_GenStub_Execute(CF_CFDP_R_Tick, Basic, NULL);
}
