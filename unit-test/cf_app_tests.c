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
#include "cf_events.h"
#include "cf_app.h"
#include "cf_cmd.h"

/*******************************************************************************
**
**  cf_app_tests Setup and Teardown
**
*******************************************************************************/

void cf_app_tests_Setup(void)
{
    cf_tests_Setup();
}

void CF_App_Tests_Teardown(void)
{
    cf_tests_Teardown();
}

/*******************************************************************************
**
**  cf_app_tests helpers
**
*******************************************************************************/

/* NOTE: UT_UpdatedDefaultHandler_CFE_SB_ReceiveBuffer is an update to the provided version that can be made to
 * correctly mimic the behavior needed.
 * In this case, values not equal to CFE_SUCCESS should return NULL.  Provided CFE_SB stub as of caelum-rc3 tag does
 * not provide this ability
 * see https://github.com/nasa/cFE/issues/1617 (closed as wontfix label)
 */
void UT_UpdatedDefaultHandler_CFE_SB_ReceiveBuffer(void *UserObj, UT_EntryKey_t FuncKey,
                                                   const UT_StubContext_t *Context)
{
    CFE_SB_Buffer_t **BufPtr = UT_Hook_GetArgValueByName(Context, "BufPtr", CFE_SB_Buffer_t **);

    UT_Stub_CopyToLocal(UT_KEY(CFE_SB_ReceiveBuffer), BufPtr, sizeof(*BufPtr));
}

/*******************************************************************************
**
**  CF_HkCmd tests - full coverage
**
*******************************************************************************/

void Test_CF_HkCmd(void)
{
    /* Act */
    CF_HkCmd();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_SetMsgTime, 1);
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
    UtAssert_STUB_COUNT(CFE_TIME_GetTime, 1);
}

/*******************************************************************************
**
**  CF_CheckTables tests - full coverage
**
*******************************************************************************/

void Test_CF_CheckTables_DoNotReleaseAddressBecauseEngineIsEnabled(void)
{
    /* Arrange */
    CF_AppData.engine.enabled = 1;

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 0);
}

void Test_CF_CheckTables_CallTo_CFE_TBL_ReleaseAddress_ReturnsNot_CFE_SUCCESS_SendEvent(void)
{
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_ReleaseAddress), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void Test_CF_CheckTables_CallTo_CFE_TBL_Manage_ReturnsNot_CFE_SUCCESS_SendEvent(void)
{
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Manage), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_ReturnsNot_CFE_SUCCESS_Or_CFE_TBL_INFO_UPDATED_SendEvent(void)
{
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_SUCCESS(void)
{
    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED(void)
{
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

/*******************************************************************************
**
**  CF_ValidateConfigTable tests - full coverage
**
*******************************************************************************/

/* CF_ValidateConfigTable tests specific items */

/* CF_ValidateConfigTable tests specific global variables */
CF_ConfigTable_t dummy_table;

/* CF_ValidateConfigTable tests specific functions */
void cf_config_table_tests_set_dummy_table_to_nominal(void)
{
    /* all values for dummy_table.ticks_per_second nominal except 0 */
    dummy_table.ticks_per_second = Any_uint32_Except(0);
    /* all values (except 0) & 3ff == 0 are nominal (1024 byte aligned) */
    dummy_table.rx_crc_calc_bytes_per_wakeup = Any_uint32_Except(0) << 10;
    /* all values less than sizeof(CF_CFDP_PduFileDataContent_t) are nominal */
    dummy_table.outgoing_file_chunk_size = Any_uint16_LessThan(sizeof(CF_CFDP_PduFileDataContent_t));
}

void Setup_cf_config_table_tests(void)
{
    cf_app_tests_Setup();
    cf_config_table_tests_set_dummy_table_to_nominal();
}

/* end CF_ValidateConfigTable tests specific items */

void Test_CF_ValidateConfigTable_FailBecauseTableTicksPerSecondIs0(void)
{
    /* Arrange */
    CF_ConfigTable_t *arg_table = &dummy_table;
    int32             result;

    arg_table->ticks_per_second = 0;

    /* Act */
    result = CF_ValidateConfigTable(arg_table);

    /* Assert */
    UtAssert_INT32_EQ(result, -1);
}

void Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIs0(void)
{
    /* Arrange */
    CF_ConfigTable_t *arg_table = &dummy_table;
    int32             result;

    arg_table->ticks_per_second             = 1;
    arg_table->rx_crc_calc_bytes_per_wakeup = 0;

    /* Act */
    result = CF_ValidateConfigTable(arg_table);

    /* Assert */
    UtAssert_INT32_EQ(result, -2);
}

void Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIsNot1024ByteAligned(void)
{
    /* Arrange */
    CF_ConfigTable_t *arg_table = &dummy_table;
    int32             result;

    arg_table->ticks_per_second = 1;
    arg_table->rx_crc_calc_bytes_per_wakeup =
        (Any_uint32() << 10) +
        (Any_uint32_LessThan_or_EqualTo(0x3FE) +
         1); /* Any_uint32_LessThan_or_EqualTo(0x3FE) + 1 is 0x001 to 0x3FF, forcing no 1024 byte alignment */

    /* Act */
    result = CF_ValidateConfigTable(arg_table);

    /* Assert */
    UtAssert_INT32_EQ(result, -2);
}

void Test_CF_ValidateConfigTable_FailBecauseOutgoingFileChunkSmallerThanDataArray(void)
{
    /* Arrange */
    CF_ConfigTable_t *arg_table = &dummy_table;
    int32             result;

    /* outgoing_file_chunk_size set to greater than sizeof(CF_CFDP_PduFileDataContent_t) */
    arg_table->ticks_per_second             = 1;
    arg_table->rx_crc_calc_bytes_per_wakeup = 0x0400; /* 1024 aligned */
    arg_table->outgoing_file_chunk_size     = sizeof(CF_CFDP_PduFileDataContent_t) + 1;

    /* Act */
    result = CF_ValidateConfigTable(arg_table);

    /* Assert */
    UtAssert_INT32_EQ(result, -3);
}

void Test_CF_ValidateConfigTable_Success(void)
{
    /* Arange */
    CF_ConfigTable_t *arg_table = &dummy_table;
    int32             result;

    arg_table->ticks_per_second             = 1;
    arg_table->rx_crc_calc_bytes_per_wakeup = 0x0400; /* 1024 aligned */
    arg_table->outgoing_file_chunk_size     = sizeof(CF_CFDP_PduFileDataContent_t);

    /* Act */
    result = CF_ValidateConfigTable(arg_table);

    /* Assert */
    UtAssert_INT32_EQ(result, CFE_SUCCESS);
}

/*******************************************************************************
**
**  CF_TableInit tests - full coverage
**
*******************************************************************************/

void Test_CF_TableInit_FailBecause_CFE_TBL_Register_DidNotReturnSuccess(void)
{
    int32 result = -1;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), result);

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_REG);
}

void Test_CF_TableInit_FailBecause_CFE_TBL_Load_DidNotReturnSuccess(void)
{
    int32 result = -1;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), result);

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_LOAD);
}

void Test_CF_TableInit_FailBecause_CFE_TBL_Manage_DidNotReturnSuccess(void)
{
    int32 result = -1;

    /* Arrange */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Manage), result);

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_MANAGE);
}

void Test_CF_TableInit_FailBecause_CFE_TBL_GetAddress_DidNotReturnSuccess(void)
{
    int32 result = -1;

    /* Arrange */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), result);

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_GETADDR);
}

void Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_SUCCESS_SuccessAndDoNotSendEvent(void)
{
    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), CFE_SUCCESS);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

void Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED_SuccessAndDoNotSendEvent(void)
{
    /* Arrange */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), CFE_SUCCESS);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
}

/*******************************************************************************
**
**  CF_Init tests - full coverage
**
*******************************************************************************/

void Test_CF_Init_CallTo_CFE_EVS_Register_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    int32 result = -1;

    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_Register), result);

    /* Act */
    UtAssert_INT32_EQ(CF_Init(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
}

void Test_CF_Init_CallTo_CFE_SB_CreatePipe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 result = -1;

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), result);

    /* Act */
    UtAssert_INT32_EQ(CF_Init(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
}

void Test_CF_Init_FirstCallTo_CFE_SB_Subscribe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 result = -1;

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_Subscribe), result);

    /* Act */
    UtAssert_INT32_EQ(CF_Init(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 1);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
}

/* NOTE: multi call test for CFE_SB_Subscribe would be helpful but not necessary for coverage */

void Test_CF_Init_CallTo_CF_TableInit_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 result = -1;

    /* Arrange unstubbable: CF_TableInit */
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), result);

    /* Act */
    UtAssert_INT32_EQ(CF_Init(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 3);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void Test_CF_Init_CallTo_CF_CFDP_InitEngine_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 result = -1;

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), result);

    /* Act */
    UtAssert_INT32_EQ(CF_Init(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 3);
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
}

void Test_CF_Init_CallTo_CFE_EVS_SendEvent_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 result = -1;

    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_SendEvent), result);

    /* Act */
    UtAssert_INT32_EQ(CF_Init(), result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 3);
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
}

void Test_CF_Init_Success(void)
{
    /* Act */
    UtAssert_INT32_EQ(CF_Init(), CFE_SUCCESS);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
}

/*******************************************************************************
**
**  CF_WakeUp tests
**
*******************************************************************************/

void Test_CF_WakeUp(void)
{
    /* Arrange */
    /* No Arrange Required */

    /* Act */
    CF_WakeUp();

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_CycleEngine, 1);
}

/*******************************************************************************
**
**  CF_ProcessMsg tests
**
*******************************************************************************/

void Test_CF_ProcessMsg_ProcessGroundCommand(void)
{
    /* Arrange */
    CFE_SB_Buffer_t sbbuf;
    CFE_SB_MsgId_t  forced_MsgID = CFE_SB_ValueToMsgId(CF_CMD_MID);

    memset(&sbbuf, 0, sizeof(sbbuf));

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_ProcessMsg(&sbbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_STUB_COUNT(CF_ProcessGroundCommand, 1);
}

void Test_CF_ProcessMsg_WakeUp(void)
{
    /* Arrange */
    CFE_SB_Buffer_t sbbuf;
    CFE_SB_MsgId_t  forced_MsgID = CFE_SB_ValueToMsgId(CF_WAKE_UP_MID);

    memset(&sbbuf, 0, sizeof(sbbuf));

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_ProcessMsg(&sbbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_STUB_COUNT(CF_CFDP_CycleEngine, 1);
}

void Test_CF_ProcessMsg_SendHk(void)
{
    CFE_SB_Buffer_t sbbuf;
    CFE_SB_MsgId_t  forced_MsgID = CFE_SB_ValueToMsgId(CF_SEND_HK_MID);

    memset(&sbbuf, 0, sizeof(sbbuf));

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_ProcessMsg(&sbbuf);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_MSG_SetMsgTime, 1); /* Confirms CF_HkCmd path was taken */
}

void Test_CF_ProcessMsg_UnrecognizedCommandEnterDefaultPath(void)
{
    /* Arrange */
    CFE_SB_MsgId_t   forced_MsgID = CFE_SB_INVALID_MSG_ID;
    CFE_SB_Buffer_t *arg_msg      = NULL;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_ProcessMsg(arg_msg);

    /* Assert */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_CMD_LENGTH);
}

/*******************************************************************************
**
**  CF_AppMain tests
**
*******************************************************************************/

void Test_CF_AppMain_CallTo_CF_Init_DoNotReturn_CFE_SUCCESS_Set_CF_AppData_run_status_To_CFE_ES_RunStatus_APP_ERROR(
    void)
{
    /* Arrange */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_Register), -1);

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(CF_AppData.run_status, CFE_ES_RunStatus_APP_ERROR);
}

void Test_CF_AppMain_CFE_SB_ReceiveBuffer_Cases(void)
{
    CFE_SB_Buffer_t  sbbuf;
    CFE_SB_Buffer_t *sbbufptr = NULL;

    memset(&sbbuf, 0, sizeof(sbbuf));

    /* Run loop once */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    /* Unit under test does not use the buffer in this case */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &sbbufptr, sizeof(sbbufptr), false);

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, -1);

    /* Act */
    UtAssert_VOIDCALL(CF_AppMain());

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 4);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 2);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);

    /* Event from CF_Init and CF_AppMain */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[0], CF_EID_INF_INIT);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[1], CF_EID_ERR_INIT_MSG_RECV);

    /* Reset, return CFE_SUCCESS from CFE_SB_ReceiveBuffer and buffer NULL */
    UT_CF_ResetEventCapture();
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &sbbufptr, sizeof(sbbufptr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);

    UtAssert_VOIDCALL(CF_AppMain());

    /* Event from CF_Init and CF_AppMain */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[0], CF_EID_INF_INIT);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[1], CF_EID_ERR_INIT_MSG_RECV);

    /* Reset, return non-error codes and non-NULL buffer */
    UT_CF_ResetEventCapture();
    UT_ResetState(UT_KEY(CFE_ES_RunLoop));
    sbbufptr = &sbbuf;
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &sbbufptr, sizeof(sbbufptr), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_TIME_OUT);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_NO_MESSAGE);

    UtAssert_VOIDCALL(CF_AppMain());

    /* Event from CF_Init */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT32_EQ(UT_CF_CapturedEventIDs[0], CF_EID_INF_INIT);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 3);
}

void Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_AndValid_msg_Call_CF_ProcessMsg(void)
{
    /* Arrange */
    CFE_SB_MsgId_t   forced_MsgID = CFE_SB_INVALID_MSG_ID;
    CFE_SB_Buffer_t  fake_msg;
    CFE_SB_Buffer_t *dummy_msg = &fake_msg;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    /* Actual data not used, just address is needed */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &dummy_msg, sizeof(dummy_msg), false);

    /* Arrange unstubbable: CF_ProcessMsg, invalid ID */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 4);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 2);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
    /* Assert for CF_Init call and CF_ProcessMsg */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    /* Assert for CF_ProcessMsg */
    UtAssert_UINT32_EQ(CF_AppData.hk.counters.err, 1);
}

/*******************************************************************************
**
**  cf_app_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_HkCmd_tests(void)
{
    UtTest_Add(Test_CF_HkCmd, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_HkCmd");
}

void add_CF_CheckTables_tests(void)
{
    UtTest_Add(Test_CF_CheckTables_DoNotReleaseAddressBecauseEngineIsEnabled, Setup_cf_config_table_tests,
               CF_App_Tests_Teardown, "Test_CF_CheckTables_DoNotReleaseAddressBecauseEngineIsEnabled");
    UtTest_Add(Test_CF_CheckTables_CallTo_CFE_TBL_ReleaseAddress_ReturnsNot_CFE_SUCCESS_SendEvent,
               Setup_cf_config_table_tests, CF_App_Tests_Teardown,
               "Test_CF_CheckTables_CallTo_CFE_TBL_ReleaseAddress_ReturnsNot_CFE_SUCCESS_SendEvent");
    UtTest_Add(Test_CF_CheckTables_CallTo_CFE_TBL_Manage_ReturnsNot_CFE_SUCCESS_SendEvent, Setup_cf_config_table_tests,
               CF_App_Tests_Teardown, "Test_CF_CheckTables_CallTo_CFE_TBL_Manage_ReturnsNot_CFE_SUCCESS_SendEvent");
    UtTest_Add(
        Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_ReturnsNot_CFE_SUCCESS_Or_CFE_TBL_INFO_UPDATED_SendEvent,
        Setup_cf_config_table_tests, CF_App_Tests_Teardown,
        "Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_ReturnsNot_CFE_SUCCESS_Or_CFE_TBL_INFO_UPDATED_SendEvent");
    UtTest_Add(Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_SUCCESS, Setup_cf_config_table_tests,
               CF_App_Tests_Teardown, "Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_SUCCESS");
    UtTest_Add(Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED, Setup_cf_config_table_tests,
               CF_App_Tests_Teardown, "Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED");
}

void add_CF_ValidateConfigTable_tests(void)
{
    UtTest_Add(Test_CF_ValidateConfigTable_FailBecauseTableTicksPerSecondIs0, Setup_cf_config_table_tests,
               CF_App_Tests_Teardown, "Test_CF_ValidateConfigTable_FailBecauseTableTicksPerSecondIs0");
    UtTest_Add(Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIs0, Setup_cf_config_table_tests,
               CF_App_Tests_Teardown, "Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIs0");
    UtTest_Add(Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIsNot1024ByteAligned,
               Setup_cf_config_table_tests, CF_App_Tests_Teardown,
               "Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIsNot1024ByteAligned");
    UtTest_Add(Test_CF_ValidateConfigTable_FailBecauseOutgoingFileChunkSmallerThanDataArray,
               Setup_cf_config_table_tests, CF_App_Tests_Teardown,
               "Test_CF_ValidateConfigTable_FailBecauseOutgoingFileChunkSmallerThanDataArray");
    UtTest_Add(Test_CF_ValidateConfigTable_Success, Setup_cf_config_table_tests, CF_App_Tests_Teardown,
               "Test_CF_ValidateConfigTable_Success");
}

void add_CF_TableInit_tests(void)
{
    UtTest_Add(Test_CF_TableInit_FailBecause_CFE_TBL_Register_DidNotReturnSuccess, cf_app_tests_Setup,
               CF_App_Tests_Teardown, "Test_CF_TableInit_FailBecause_CFE_TBL_Register_DidNotReturnSuccess");
    UtTest_Add(Test_CF_TableInit_FailBecause_CFE_TBL_Load_DidNotReturnSuccess, cf_app_tests_Setup,
               CF_App_Tests_Teardown, "Test_CF_TableInit_FailBecause_CFE_TBL_Load_DidNotReturnSuccess");
    UtTest_Add(Test_CF_TableInit_FailBecause_CFE_TBL_Manage_DidNotReturnSuccess, cf_app_tests_Setup,
               CF_App_Tests_Teardown, "Test_CF_TableInit_FailBecause_CFE_TBL_Manage_DidNotReturnSuccess");
    UtTest_Add(Test_CF_TableInit_FailBecause_CFE_TBL_GetAddress_DidNotReturnSuccess, cf_app_tests_Setup,
               CF_App_Tests_Teardown, "Test_CF_TableInit_FailBecause_CFE_TBL_GetAddress_DidNotReturnSuccess");
    UtTest_Add(Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_SUCCESS_SuccessAndDoNotSendEvent,
               cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_SUCCESS_SuccessAndDoNotSendEvent");
    UtTest_Add(Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED_SuccessAndDoNotSendEvent,
               cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED_SuccessAndDoNotSendEvent");
}

void add_CF_Init_tests(void)
{
    UtTest_Add(
        Test_CF_Init_CallTo_CFE_EVS_Register_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_Init_CallTo_CFE_EVS_Register_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus");
    UtTest_Add(
        Test_CF_Init_CallTo_CFE_SB_CreatePipe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_Init_CallTo_CFE_SB_CreatePipe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus");
    UtTest_Add(
        Test_CF_Init_FirstCallTo_CFE_SB_Subscribe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_Init_FirstCallTo_CFE_SB_Subscribe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus");
    UtTest_Add(Test_CF_Init_CallTo_CF_TableInit_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus, cf_app_tests_Setup,
               CF_App_Tests_Teardown, "Test_CF_Init_CallTo_CF_TableInit_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus");
    UtTest_Add(Test_CF_Init_CallTo_CF_CFDP_InitEngine_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus, cf_app_tests_Setup,
               CF_App_Tests_Teardown,
               "Test_CF_Init_CallTo_CF_CFDP_InitEngine_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus");
    UtTest_Add(
        Test_CF_Init_CallTo_CFE_EVS_SendEvent_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_Init_CallTo_CFE_EVS_SendEvent_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus");
    UtTest_Add(Test_CF_Init_Success, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_Init_Success");
}

void add_CF_WakeUp_tests(void)
{
    UtTest_Add(Test_CF_WakeUp, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_WakeUp");
}

void add_CF_ProcessMsg_tests(void)
{
    UtTest_Add(Test_CF_ProcessMsg_ProcessGroundCommand, cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_ProcessMsg_ProcessGroundCommand");
    UtTest_Add(Test_CF_ProcessMsg_WakeUp, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_ProcessMsg_WakeUp");
    UtTest_Add(Test_CF_ProcessMsg_SendHk, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_ProcessMsg_SendHk");
    UtTest_Add(Test_CF_ProcessMsg_UnrecognizedCommandEnterDefaultPath, cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_ProcessMsg_UnrecognizedCommandEnterDefaultPath");
}

void add_CF_AppMain_tests(void)
{
    UtTest_Add(
        Test_CF_AppMain_CallTo_CF_Init_DoNotReturn_CFE_SUCCESS_Set_CF_AppData_run_status_To_CFE_ES_RunStatus_APP_ERROR,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_AppMain_CallTo_CF_Init_DoNotReturn_CFE_SUCCESS_Set_CF_AppData_run_status_To_CFE_ES_RunStatus_APP_"
        "ERROR");
    UtTest_Add(Test_CF_AppMain_CFE_SB_ReceiveBuffer_Cases, cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_AppMain_CFE_SB_ReceiveBuffer_Cases");
    UtTest_Add(
        Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_AndValid_msg_Call_CF_ProcessMsg,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_AndValid_msg_Call_CF_ProcessMsg");
}

/*******************************************************************************
**
**  cf_app_tests test UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_HkCmd_tests();

    add_CF_CheckTables_tests();

    add_CF_ValidateConfigTable_tests();

    add_CF_TableInit_tests();

    add_CF_Init_tests();

    add_CF_WakeUp_tests();

    add_CF_ProcessMsg_tests();

    add_CF_AppMain_tests();
}