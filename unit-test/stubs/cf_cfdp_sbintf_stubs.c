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
 * Auto-Generated stub implementations for functions defined in cf_cfdp_sbintf header
 */

#include "cf_cfdp_sbintf.h"
#include "utgenstub.h"

void UT_DefaultHandler_CF_CFDP_MsgOutGet(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_MsgOutGet()
 * ----------------------------------------------------
 */
CF_Logical_PduBuffer_t *CF_CFDP_MsgOutGet(const CF_Transaction_t *t, bool silent)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_MsgOutGet, CF_Logical_PduBuffer_t *);

    UT_GenStub_AddParam(CF_CFDP_MsgOutGet, const CF_Transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_MsgOutGet, bool, silent);

    UT_GenStub_Execute(CF_CFDP_MsgOutGet, Basic, UT_DefaultHandler_CF_CFDP_MsgOutGet);

    return UT_GenStub_GetReturnValue(CF_CFDP_MsgOutGet, CF_Logical_PduBuffer_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_ReceiveMessage()
 * ----------------------------------------------------
 */
void CF_CFDP_ReceiveMessage(CF_Channel_t *c)
{
    UT_GenStub_AddParam(CF_CFDP_ReceiveMessage, CF_Channel_t *, c);

    UT_GenStub_Execute(CF_CFDP_ReceiveMessage, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CFDP_Send()
 * ----------------------------------------------------
 */
void CF_CFDP_Send(uint8 chan_num, const CF_Logical_PduBuffer_t *ph)
{
    UT_GenStub_AddParam(CF_CFDP_Send, uint8, chan_num);
    UT_GenStub_AddParam(CF_CFDP_Send, const CF_Logical_PduBuffer_t *, ph);

    UT_GenStub_Execute(CF_CFDP_Send, Basic, NULL);
}
