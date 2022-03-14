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

#include <string.h>

/* UT includes */
#include "ut_utils_cfe_hooks.h"
#include "cf_test_utils.h"
#include "utstubs.h"

int32 UT_Hook_CFE_EVS_SendEvent(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_EVS_SendEvent_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->EventID   = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
        ctxt->EventType = UT_Hook_GetArgValueByName(Context, "EventType", uint16);
        ctxt->Spec      = UT_Hook_GetArgValueByName(Context, "Spec", const char *);
    }

    return StubRetcode;
}

int32 UT_Hook_CFE_MSG_GetSize(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_MSG_GetSize_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", const CFE_MSG_Message_t *);
        ctxt->Size   = UT_Hook_GetArgValueByName(Context, "Size", CFE_MSG_Size_t *);
    }

    return StubRetcode;
}

int32 UT_Hook_CFE_MSG_SetSize(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_MSG_SetSize_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
        ctxt->Size   = UT_Hook_GetArgValueByName(Context, "Size", CFE_MSG_Size_t);
    }

    return StubRetcode;
}

int32 UT_Hook_CFE_MSG_SetMsgId(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_MSG_SetMsgId_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
        ctxt->MsgId  = UT_Hook_GetArgValueByName(Context, "MsgId", CFE_SB_MsgId_t);
    }

    return StubRetcode;
}

int32 UT_Hook_CFE_MSG_GetMsgId(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_MSG_GetMsgId_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->MsgPtr         = UT_Hook_GetArgValueByName(Context, "MsgPtr", const CFE_MSG_Message_t *);
        ctxt->returned_MsgId = UT_Hook_GetArgValueByName(Context, "MsgId", CFE_SB_MsgId_t *);
    }

    return StubRetcode;
}

int32 UT_Hook_CFE_MSG_GetFcnCode(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_MSG_GetFcnCode_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->MsgPtr  = UT_Hook_GetArgValueByName(Context, "MsgPtr", const CFE_MSG_Message_t *);
        ctxt->FcnCode = UT_Hook_GetArgValueByName(Context, "FcnCode", CFE_MSG_FcnCode_t *);
    }

    return StubRetcode;
}

int32 UT_Hook_CFE_MSG_SetMsgTime(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_MSG_SetMsgTime_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
        ctxt->Time   = UT_Hook_GetArgValueByName(Context, "NewTime", CFE_TIME_SysTime_t);
    }

    return StubRetcode;
}

int32 UT_Hook_CFE_SB_TransmitMsg(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_SB_TransmitMsg_context_t *ctxt = UserObj;

    if (ctxt)
    {
        ctxt->MsgPtr                 = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
        ctxt->IncrementSequenceCount = UT_Hook_GetArgValueByName(Context, "IncrementSequenceCount", bool);
    }

    return StubRetcode;
}
