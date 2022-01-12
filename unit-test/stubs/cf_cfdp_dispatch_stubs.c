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
 * Auto-Generated stub implementations for functions defined in cf_cfdp_dispatch header
 */

#include "cf_cfdp_dispatch.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_R_DispatchRecv()
 * ----------------------------------------------------
 */
void CF_CFDP_R_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph,
                            const CF_CFDP_R_SubstateDispatchTable_t *dispatch, CF_CFDP_StateRecvFunc_t fd_fn)
{
    UT_GenStub_AddParam(CF_CFDP_R_DispatchRecv, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_R_DispatchRecv, CF_Logical_PduBuffer_t *, ph);
    UT_GenStub_AddParam(CF_CFDP_R_DispatchRecv, const CF_CFDP_R_SubstateDispatchTable_t *, dispatch);
    UT_GenStub_AddParam(CF_CFDP_R_DispatchRecv, CF_CFDP_StateRecvFunc_t, fd_fn);

    UT_GenStub_Execute(CF_CFDP_R_DispatchRecv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_RxStateDispatch()
 * ----------------------------------------------------
 */
void CF_CFDP_RxStateDispatch(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph,
                             const CF_CFDP_TxnRecvDispatchTable_t *dispatch)
{
    UT_GenStub_AddParam(CF_CFDP_RxStateDispatch, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_RxStateDispatch, CF_Logical_PduBuffer_t *, ph);
    UT_GenStub_AddParam(CF_CFDP_RxStateDispatch, const CF_CFDP_TxnRecvDispatchTable_t *, dispatch);

    UT_GenStub_Execute(CF_CFDP_RxStateDispatch, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_DispatchRecv()
 * ----------------------------------------------------
 */
void CF_CFDP_S_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph,
                            const CF_CFDP_S_SubstateRecvDispatchTable_t *dispatch)
{
    UT_GenStub_AddParam(CF_CFDP_S_DispatchRecv, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_S_DispatchRecv, CF_Logical_PduBuffer_t *, ph);
    UT_GenStub_AddParam(CF_CFDP_S_DispatchRecv, const CF_CFDP_S_SubstateRecvDispatchTable_t *, dispatch);

    UT_GenStub_Execute(CF_CFDP_S_DispatchRecv, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_S_DispatchTransmit()
 * ----------------------------------------------------
 */
void CF_CFDP_S_DispatchTransmit(CF_Transaction_t *t, const CF_CFDP_S_SubstateSendDispatchTable_t *dispatch)
{
    UT_GenStub_AddParam(CF_CFDP_S_DispatchTransmit, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_S_DispatchTransmit, const CF_CFDP_S_SubstateSendDispatchTable_t *, dispatch);

    UT_GenStub_Execute(CF_CFDP_S_DispatchTransmit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_TxStateDispatch()
 * ----------------------------------------------------
 */
void CF_CFDP_TxStateDispatch(CF_Transaction_t *t, const CF_CFDP_TxnSendDispatchTable_t *dispatch)
{
    UT_GenStub_AddParam(CF_CFDP_TxStateDispatch, CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_TxStateDispatch, const CF_CFDP_TxnSendDispatchTable_t *, dispatch);

    UT_GenStub_Execute(CF_CFDP_TxStateDispatch, Basic, NULL);
}
