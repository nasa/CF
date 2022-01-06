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
