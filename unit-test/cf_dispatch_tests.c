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

/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_dispatch.h"
#include "cf_cmds.h"
#include "cf_msgids.h"
#include "cf_events.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/*******************************************************************************
**
**  cf_dispatch_tests Setup and Teardown
**
*******************************************************************************/

void cf_dispatch_tests_Setup(void)
{
    cf_tests_Setup();
}

void cf_dispatch_tests_Teardown(void)
{
    cf_tests_Teardown();
}

/*******************************************************************************
**
**  CF_ProcessGroundCommand tests
**
*******************************************************************************/

void Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 24;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    /* CFE_MSG_GetSize does not matter for Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent
     */

    /* Act */
    CF_ProcessGroundCommand(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CC_ERR_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
}

void Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 123;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    /* CFE_MSG_GetSize does not matter for Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent
     */

    /* Act */
    CF_ProcessGroundCommand(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CC_ERR_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
}

void Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndFailure(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = CF_NOOP_CC;
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize    = sizeof(CF_NoopCmd_t) + 1; /* Invalid size */

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);

    /* Act */
    CF_ProcessGroundCommand(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_CMD_LEN_ERR_EID);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
}

void Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_NoopCmd_With_msg(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = CF_NOOP_CC;
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize    = sizeof(CF_NoopCmd_t); /* Valid size */

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);

    /* Act */
    CF_ProcessGroundCommand(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
}

/* Hit a NULL entry to exercise that conditional and no action */
void Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   utbuf;
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 0x0C; /* 0x0C forces a null slot */
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize    = 0;

    memset(&utbuf, 0, sizeof(utbuf));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);

    /* Act */
    CF_ProcessGroundCommand(&utbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 0);
}

/*******************************************************************************
**
**  CF_AppPipe tests
**
*******************************************************************************/

void Test_CF_AppPipe_ProcessGroundCommand(void)
{
    /* Arrange */
    CFE_SB_Buffer_t   sbbuf;
    CFE_SB_MsgId_t    forced_MsgID                     = CFE_SB_ValueToMsgId(CF_CMD_MID);
    CFE_MSG_FcnCode_t forced_return_CFE_MSG_GetFcnCode = 0;
    CFE_MSG_Size_t    forced_return_CFE_MSG_GetSize    = 0; /* Invalid size */

    memset(&sbbuf, 0, sizeof(sbbuf));

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &forced_return_CFE_MSG_GetFcnCode,
                     sizeof(forced_return_CFE_MSG_GetFcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &forced_return_CFE_MSG_GetSize, sizeof(forced_return_CFE_MSG_GetSize),
                     false);

    /* Act */
    CF_AppPipe(&sbbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetFcnCode, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetSize, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);

    /* Assert for incremented counter */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.cmd, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
}

void Test_CF_AppPipe_WakeUp(void)
{
    /* Arrange */
    CFE_SB_Buffer_t sbbuf;
    CFE_SB_MsgId_t  forced_MsgID = CFE_SB_ValueToMsgId(CF_WAKE_UP_MID);

    memset(&sbbuf, 0, sizeof(sbbuf));

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_AppPipe(&sbbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_STUB_COUNT(CF_WakeupCmd, 1);
}

void Test_CF_AppPipe_SendHk(void)
{
    CFE_SB_Buffer_t sbbuf;
    CFE_SB_MsgId_t  forced_MsgID = CFE_SB_ValueToMsgId(CF_SEND_HK_MID);

    memset(&sbbuf, 0, sizeof(sbbuf));

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_AppPipe(&sbbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_STUB_COUNT(CF_SendHkCmd, 1);
}

void Test_CF_AppPipe_UnrecognizedCommandEnterDefaultPath(void)
{
    /* Arrange */
    CFE_SB_MsgId_t   forced_MsgID = CFE_SB_INVALID_MSG_ID;
    CFE_SB_Buffer_t *arg_msg      = NULL;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_AppPipe(arg_msg);

    /* Assert */
    UtAssert_UINT32_EQ(CF_AppData.hk.Payload.counters.err, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_MID_ERR_EID);
}

void add_CF_ProcessGroundCommand_tests(void)
{
    UtTest_Add(Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent, cf_dispatch_tests_Setup,
               cf_dispatch_tests_Teardown,
               "Test_CF_ProcessGroundCommand_When_cmd_EqTo_CF_NUM_COMMANDS_FailAndSendEvent");
    UtTest_Add(Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent,
               cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown,
               "Test_CF_ProcessGroundCommand_When_cmd_GreaterThan_CF_NUM_COMMANDS_FailAndSendEvent");
    UtTest_Add(
        Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndFailure,
        cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown,
        "Test_CF_ProcessGroundCommand_Receives_cmd_AndLengthDoesNotMatchExpectedForThatCommandSendEventAndFailure");
    UtTest_Add(Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_NoopCmd_With_msg, cf_dispatch_tests_Setup,
               cf_dispatch_tests_Teardown,
               "Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x00_AndCall_CF_NoopCmd_With_msg");
    UtTest_Add(Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL,
               cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown,
               "Test_CF_ProcessGroundCommand_ReceivesCmdCode_0x0C_AndDoNothingBecause_fns_12_Is_NULL");
}

void add_CF_AppPipe_tests(void)
{
    UtTest_Add(Test_CF_AppPipe_ProcessGroundCommand, cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown,
               "Test_CF_AppPipe_ProcessGroundCommand");
    UtTest_Add(Test_CF_AppPipe_WakeUp, cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown, "Test_CF_AppPipe_WakeUp");
    UtTest_Add(Test_CF_AppPipe_SendHk, cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown, "Test_CF_AppPipe_SendHk");
    UtTest_Add(Test_CF_AppPipe_UnrecognizedCommandEnterDefaultPath, cf_dispatch_tests_Setup, cf_dispatch_tests_Teardown,
               "Test_CF_AppPipe_UnrecognizedCommandEnterDefaultPath");
}

/*
 * Register the test cases to execute with the unit test tool
 */
void UtTest_Setup(void)
{
    add_CF_ProcessGroundCommand_tests();
    add_CF_AppPipe_tests();
}
