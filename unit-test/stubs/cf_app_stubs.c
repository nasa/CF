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
 * Auto-Generated stub implementations for functions defined in cf_app header
 */

#include "cf_app.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_AppMain()
 * ----------------------------------------------------
 */
void CF_AppMain(void)
{

    UT_GenStub_Execute(CF_AppMain, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CheckTables()
 * ----------------------------------------------------
 */
void CF_CheckTables(void)
{

    UT_GenStub_Execute(CF_CheckTables, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_HkCmd()
 * ----------------------------------------------------
 */
void CF_HkCmd(void)
{

    UT_GenStub_Execute(CF_HkCmd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Init()
 * ----------------------------------------------------
 */
CFE_Status_t CF_Init(void)
{
    UT_GenStub_SetupReturnBuffer(CF_Init, CFE_Status_t);

    UT_GenStub_Execute(CF_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Init, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ProcessMsg()
 * ----------------------------------------------------
 */
void CF_ProcessMsg(CFE_SB_Buffer_t *msg)
{
    UT_GenStub_AddParam(CF_ProcessMsg, CFE_SB_Buffer_t *, msg);

    UT_GenStub_Execute(CF_ProcessMsg, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_TableInit()
 * ----------------------------------------------------
 */
CFE_Status_t CF_TableInit(void)
{
    UT_GenStub_SetupReturnBuffer(CF_TableInit, CFE_Status_t);

    UT_GenStub_Execute(CF_TableInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_TableInit, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ValidateConfigTable()
 * ----------------------------------------------------
 */
CFE_Status_t CF_ValidateConfigTable(void *tbl_ptr)
{
    UT_GenStub_SetupReturnBuffer(CF_ValidateConfigTable, CFE_Status_t);

    UT_GenStub_AddParam(CF_ValidateConfigTable, void *, tbl_ptr);

    UT_GenStub_Execute(CF_ValidateConfigTable, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_ValidateConfigTable, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_WakeUp()
 * ----------------------------------------------------
 */
void CF_WakeUp(void)
{

    UT_GenStub_Execute(CF_WakeUp, Basic, NULL);
}
