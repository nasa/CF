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
 * Auto-Generated stub implementations for functions defined in cf_cfdp_s header
 */

#include "cf_cfdp_s.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S1_Recv()
 * ----------------------------------------------------
 */
void CF_CFDP_S1_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_S1_Recv, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_S1_Recv, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_S1_Recv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S2_SubstateEofAck()
 * ----------------------------------------------------
 */
void CF_CFDP_S2_SubstateEofAck(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_S2_SubstateEofAck, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_S2_SubstateEofAck, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_S2_SubstateEofAck, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S2_SubstateNak()
 * ----------------------------------------------------
 */
void CF_CFDP_S2_SubstateNak(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_S2_SubstateNak, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_S2_SubstateNak, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_S2_SubstateNak, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S2_Recv()
 * ----------------------------------------------------
 */
void CF_CFDP_S2_Recv(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_S2_Recv, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_S2_Recv, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_S2_Recv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_AckTimerTick()
 * ----------------------------------------------------
 */
void CF_CFDP_S_AckTimerTick(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_AckTimerTick, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_AckTimerTick, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_CheckState()
 * ----------------------------------------------------
 */
void CF_CFDP_S_CheckState(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_CheckState, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_CheckState, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_SubstateEarlyFin()
 * ----------------------------------------------------
 */
void CF_CFDP_S_SubstateEarlyFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_S_SubstateEarlyFin, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_S_SubstateEarlyFin, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_S_SubstateEarlyFin, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_SubstateRecvFin()
 * ----------------------------------------------------
 */
void CF_CFDP_S_SubstateRecvFin(CF_Transaction_t *txn, CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_S_SubstateRecvFin, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_S_SubstateRecvFin, CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_S_SubstateRecvFin, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_HandleFileRetention()
 * ----------------------------------------------------
 */
void CF_CFDP_S_HandleFileRetention(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_HandleFileRetention, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_HandleFileRetention, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_Init()
 * ----------------------------------------------------
 */
void CF_CFDP_S_Init(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_Init, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_Init, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_SendFileData()
 * ----------------------------------------------------
 */
CFE_Status_t CF_CFDP_S_SendFileData(CF_Transaction_t *txn, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_S_SendFileData, CFE_Status_t);

    UT_GenStub_AddParam(CF_CFDP_S_SendFileData, CF_Transaction_t *, txn);
    UT_GenStub_AddParam(CF_CFDP_S_SendFileData, uint32, foffs);
    UT_GenStub_AddParam(CF_CFDP_S_SendFileData, uint32, bytes_to_read);
    UT_GenStub_AddParam(CF_CFDP_S_SendFileData, uint8, calc_crc);

    UT_GenStub_Execute(CF_CFDP_S_SendFileData, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_S_SendFileData, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_SubstateSendFileData()
 * ----------------------------------------------------
 */
void CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_SubstateSendFileData, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_SubstateSendFileData, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_Tick()
 * ----------------------------------------------------
 */
void CF_CFDP_S_Tick(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_Tick, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_Tick, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_Tick_Maintenance()
 * ----------------------------------------------------
 */
void CF_CFDP_S_Tick_Maintenance(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_Tick_Maintenance, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_Tick_Maintenance, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_Tick_Nak()
 * ----------------------------------------------------
 */
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *txn)
{
    UT_GenStub_AddParam(CF_CFDP_S_Tick_Nak, CF_Transaction_t *, txn);

    UT_GenStub_Execute(CF_CFDP_S_Tick_Nak, Basic, NULL);
}
