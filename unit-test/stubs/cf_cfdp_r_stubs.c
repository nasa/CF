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
void CF_CFDP_R1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R1_Recv, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_R1_Recv, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R1_Recv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_GapCompute()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_GapCompute(const CF_ChunkList_t *chunks, const CF_Chunk_t *chunk, void *opaque)
{
    UT_GenStub_AddParam(CF_CFDP_R2_GapCompute, const CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_CFDP_R2_GapCompute, const CF_Chunk_t *, chunk);
    UT_GenStub_AddParam(CF_CFDP_R2_GapCompute, void *, opaque);

    UT_GenStub_Execute(CF_CFDP_R2_GapCompute, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_Recv()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R2_Recv, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_R2_Recv, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R2_Recv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R2_SubstateRecvFinAck()
 * ----------------------------------------------------
 */
void CF_CFDP_R2_SubstateRecvFinAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R2_SubstateRecvFinAck, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_R2_SubstateRecvFinAck, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R2_SubstateRecvFinAck, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_AckTimerTick()
 * ----------------------------------------------------
 */
void CF_CFDP_R_AckTimerTick(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_AckTimerTick, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_AckTimerTick, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_CalcCrcChunk()
 * ----------------------------------------------------
 */
void CF_CFDP_R_CalcCrcChunk(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_CalcCrcChunk, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_CalcCrcChunk, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_CalcCrcStart()
 * ----------------------------------------------------
 */
void CF_CFDP_R_CalcCrcStart(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_CalcCrcStart, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_CalcCrcStart, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_CheckComplete()
 * ----------------------------------------------------
 */
bool CF_CFDP_R_CheckComplete(CF_Transaction_t *txn)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_CheckComplete, bool);

    UT_GenStub_AddParam(CF_CFDP_R_CheckComplete, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_CheckComplete, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_CheckComplete, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_CheckCrc()
 * ----------------------------------------------------
 */
CFE_Status_t CF_CFDP_R_CheckCrc(CF_Transaction_t *txn)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_CheckCrc, CFE_Status_t);

    UT_GenStub_AddParam(CF_CFDP_R_CheckCrc, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_CheckCrc, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_CheckCrc, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_CheckState()
 * ----------------------------------------------------
 */
void CF_CFDP_R_CheckState(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_CheckState, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_CheckState, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_HandleFileRetention()
 * ----------------------------------------------------
 */
void CF_CFDP_R_HandleFileRetention(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_HandleFileRetention, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_HandleFileRetention, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_Init()
 * ----------------------------------------------------
 */
void CF_CFDP_R_Init(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_Init, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_Init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_ProcessFd()
 * ----------------------------------------------------
 */
CFE_Status_t CF_CFDP_R_ProcessFd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_ProcessFd, CFE_Status_t);

    UT_GenStub_AddParam(CF_CFDP_R_ProcessFd, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_R_ProcessFd, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R_ProcessFd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_ProcessFd, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_SendNak()
 * ----------------------------------------------------
 */
CFE_Status_t CF_CFDP_R_SendNak(CF_Transaction_t *txn)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_SendNak, CFE_Status_t);

    UT_GenStub_AddParam(CF_CFDP_R_SendNak, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_SendNak, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_SendNak, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_SubstateRecvEof()
 * ----------------------------------------------------
 */
void CF_CFDP_R_SubstateRecvEof(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvEof, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvEof, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R_SubstateRecvEof, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_SubstateRecvFileData()
 * ----------------------------------------------------
 */
void CF_CFDP_R_SubstateRecvFileData(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvFileData, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvFileData, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R_SubstateRecvFileData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_SubstateRecvMd()
 * ----------------------------------------------------
 */
void CF_CFDP_R_SubstateRecvMd(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvMd, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_R_SubstateRecvMd, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_R_SubstateRecvMd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_Tick()
 * ----------------------------------------------------
 */
void CF_CFDP_R_Tick(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_Tick, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_Tick, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_Tick_Maintenance()
 * ----------------------------------------------------
 */
void CF_CFDP_R_Tick_Maintenance(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_R_Tick_Maintenance, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_R_Tick_Maintenance, Basic, NULL);
}
