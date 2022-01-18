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
} /* end cf_app_tests_Setup */

void CF_App_Tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end CF_App_Tests_Teardown */

/* end cf_app_tests Setup and Teardown */

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

/* end cf_app_tests helpers */

/*******************************************************************************
**
**  CF_HkCmd tests - full coverage
**
*******************************************************************************/

void Test_CF_HkCmd_TimestampAndSendMessageWith_CF_AppData_hk(void)
{
    /* Arrange */
    CFE_TIME_SysTime_t           fake_time;
    CFE_SB_TransmitMsg_context_t context_CFE_SB_TransmitMsg;
    CFE_MSG_SetMsgTime_context_t context_CFE_MSG_SetMsgTime;

    Any_CFE_TIME_SysTime_Set(&fake_time);

    UT_SetDataBuffer(UT_KEY(CFE_TIME_GetTime), &fake_time, sizeof(fake_time), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_SetMsgTime), UT_Hook_CFE_MSG_SetMsgTime, &context_CFE_MSG_SetMsgTime);
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_Hook_CFE_SB_TransmitMsg, &context_CFE_SB_TransmitMsg);

    /* Act */
    CF_HkCmd();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_SetMsgTime, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_SetMsgTime.MsgPtr, &CF_AppData.hk.tlm_header.Msg);
    UtAssert_True(context_CFE_MSG_SetMsgTime.Time.Seconds == fake_time.Seconds,
                  "CFE_MSG_SetMsgTime received Time.Seconds %lu and should be %lu (call to CFE_TIME_GetTime Seconds)",
                  (unsigned long)context_CFE_MSG_SetMsgTime.Time.Seconds, (unsigned long)fake_time.Seconds);
    UtAssert_True(
        context_CFE_MSG_SetMsgTime.Time.Subseconds == fake_time.Subseconds,
        "CFE_MSG_SetMsgTime received Time.Subseconds %lu and should be %lu (call to CFE_TIME_GetTime Subseconds)",
        (unsigned long)context_CFE_MSG_SetMsgTime.Time.Subseconds, (unsigned long)fake_time.Subseconds);
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
    UtAssert_ADDRESS_EQ(context_CFE_SB_TransmitMsg.MsgPtr, &CF_AppData.hk.tlm_header.Msg);
    UtAssert_True(context_CFE_SB_TransmitMsg.IncrementSequenceCount == true,
                  "CFE_SB_TransmitMsg received IncrementSequenceCount '%s' and should be 'true'",
                  context_CFE_SB_TransmitMsg.IncrementSequenceCount ? "true" : "false");
} /* end Test_CF_HkCmd_TimestampAndSendMessageWith_CF_AppData_hk */

/* end CF_HkCmd tests */

/*******************************************************************************
**
**  CF_CheckTables tests - full coverage
**
*******************************************************************************/

void Test_CF_CheckTables_DoNotReleaseAddressBecauseEngineIsEnabled(void)
{
    /* Arrange */
    /* CF_AppData.engine.enabled not being 0 bypasses all code in CUT */
    CF_AppData.engine.enabled = Any_uint8_Except(0);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 0);
} /* end Test_CF_CheckTables_DoNotReleaseAddressBecauseEngineIsEnabled */

void Test_CF_CheckTables_CallTo_CFE_TBL_ReleaseAddress_ReturnsNot_CFE_SUCCESS_SendEvent(void)
{
    /* Arrange */
    int32 forced_return_CFE_TBL_ReleaseAddress = CFE_STATUS_EXTERNAL_RESOURCE_FAIL; /* any generic failure */

    /* CF_AppData.engine.enabled being 0 runs code in CUT */
    CF_AppData.engine.enabled = 0;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_ReleaseAddress), forced_return_CFE_TBL_ReleaseAddress);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_CheckTables_CallTo_CFE_TBL_ReleaseAddress_ReturnsNot_CFE_SUCCESS_SendEvent */

void Test_CF_CheckTables_CallTo_CFE_TBL_Manage_ReturnsNot_CFE_SUCCESS_SendEvent(void)
{
    /* Arrange */
    int32 forced_return_CFE_TBL_Manage = CFE_STATUS_EXTERNAL_RESOURCE_FAIL; /* any generic failure */

    /* CF_AppData.engine.enabled being 0 runs code in CUT */
    CF_AppData.engine.enabled = 0;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Manage), forced_return_CFE_TBL_Manage);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_CheckTables_CallTo_CFE_TBL_Manage_ReturnsNot_CFE_SUCCESS_SendEvent */

void Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_ReturnsNot_CFE_SUCCESS_Or_CFE_TBL_INFO_UPDATED_SendEvent(void)
{
    /* Arrange */
    int32 forced_return_CFE_TBL_GetAddress = CFE_STATUS_EXTERNAL_RESOURCE_FAIL; /* any generic failure */

    /* CF_AppData.engine.enabled being 0 runs code in CUT */
    CF_AppData.engine.enabled = 0;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), forced_return_CFE_TBL_GetAddress);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_ReturnsNot_CFE_SUCCESS_Or_CFE_TBL_INFO_UPDATED_SendEvent */

void Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_SUCCESS(void)
{
    /* Arrange */
    /* CF_AppData.engine.enabled being 0 runs code in CUT */
    CF_AppData.engine.enabled = 0;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_SUCCESS */

void Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED(void)
{
    /* Arrange */
    /* CF_AppData.engine.enabled being 0 runs code in CUT */
    CF_AppData.engine.enabled = 0;

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Act */
    CF_CheckTables();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_TBL_ReleaseAddress, 1);
    UtAssert_STUB_COUNT(CFE_TBL_Manage, 1);
    UtAssert_STUB_COUNT(CFE_TBL_GetAddress, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CheckTables_CallTo_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED */

/* end CF_CheckTables tests */

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
    // all values for dummy_table.ticks_per_second nominal except 0
    dummy_table.ticks_per_second = Any_uint32_Except(0);
    // all values (except 0) & 3ff == 0 are nominal (1024 byte aligned)
    dummy_table.rx_crc_calc_bytes_per_wakeup = Any_uint32_Except(0) << 10;
    // all values less than sizeof(CF_CFDP_PduFileDataContent_t) are nominal
    dummy_table.outgoing_file_chunk_size = Any_uint16_LessThan(sizeof(CF_CFDP_PduFileDataContent_t));
} /* end cf_config_table_tests_set_dummy_table_to_nominal */

void Setup_cf_config_table_tests(void)
{
    cf_app_tests_Setup();
    cf_config_table_tests_set_dummy_table_to_nominal();
} /* end Setup_cf_config_table_tests */

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
} /* end Test_CF_ValidateConfigTable_FailBecauseTableTicksPerSecondIs0 */

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
} /* end Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIs0 */

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
} /* end Test_CF_ValidateConfigTable_FailBecauseCalcBytesPerWakeupIsNot1024ByteAligned */

void Test_CF_ValidateConfigTable_FailBecauseOutgoingFileChunkSmallerThanDataArray(void)
{
    /* Arrange */
    CF_ConfigTable_t *arg_table = &dummy_table;
    int32             result;

    // outgoing_file_chunk_size set to greater than sizeof(CF_CFDP_PduFileDataContent_t)
    arg_table->ticks_per_second             = 1;
    arg_table->rx_crc_calc_bytes_per_wakeup = 0x0400; /* 1024 aligned */
    arg_table->outgoing_file_chunk_size     = sizeof(CF_CFDP_PduFileDataContent_t) + 1;

    /* Act */
    result = CF_ValidateConfigTable(arg_table);

    /* Assert */
    UtAssert_INT32_EQ(result, -3);
} /* end Test_CF_ValidateConfigTable_FailBecauseOutgoingFileChunkSmallerThanDataArray */

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
} /* end Test_CF_ValidateConfigTable_Success */

/* end CF_ValidateConfigTable tests */

/*******************************************************************************
**
**  CF_TableInit tests - full coverage
**
*******************************************************************************/

void Test_CF_TableInit_FailBecause_CFE_TBL_Register_DidNotReturnSuccess(void)
{
    /* Arrange */
    int32 expected_result = Any_int32_Except(CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), expected_result);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), expected_result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_REG);
    /* TODO: CFE_EVS_SendEvent needs to check all context values! */
} /* end Test_CF_TableInit_FailBecause_CFE_TBL_Register_DidNotReturnSuccess */

void Test_CF_TableInit_FailBecause_CFE_TBL_Load_DidNotReturnSuccess(void)
{
    /* Arrange */
    int32 expected_result = Any_int32_Except(CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Load), expected_result);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), expected_result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_LOAD);
    /* TODO: CFE_EVS_SendEvent needs to check all context values! */
} /* end Test_CF_TableInit_FailBecause_CFE_TBL_Load_DidNotReturnSuccess */

void Test_CF_TableInit_FailBecause_CFE_TBL_Manage_DidNotReturnSuccess(void)
{
    /* Arrange */
    int32 expected_result = Any_int32_Except(CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Manage), expected_result);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), expected_result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_MANAGE);
    /* TODO: CFE_EVS_SendEvent needs to check all context values! */
} /* end Test_CF_TableInit_FailBecause_CFE_TBL_Manage_DidNotReturnSuccess */

void Test_CF_TableInit_FailBecause_CFE_TBL_GetAddress_DidNotReturnSuccess(void)
{
    /* Arrange */
    int32 possible_success_results[2] = {CFE_SUCCESS, CFE_TBL_INFO_UPDATED};
    int32 expected_result = Any_int32_ExceptThese(possible_success_results, sizeof(possible_success_results) /
                                                                                sizeof(possible_success_results[0]));

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), expected_result);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), expected_result);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_TBL_GETADDR);
    /* TODO: CFE_EVS_SendEvent needs to check all context values! */
} /* end Test_CF_TableInit_FailBecause_CFE_TBL_GetAddress_DidNotReturnSuccess */

void Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_SUCCESS_SuccessAndDoNotSendEvent(void)
{
    /* Arrange */

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_SUCCESS);

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), CFE_SUCCESS);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_SUCCESS_SuccessAndDoNotSendEvent */

void Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED_SuccessAndDoNotSendEvent(void)
{
    /* Arrange */

    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), CFE_TBL_INFO_UPDATED);

    /* Act */
    UtAssert_INT32_EQ(CF_TableInit(), CFE_SUCCESS);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_TableInit_When_CFE_TBL_GetAddress_Returns_CFE_TBL_INFO_UPDATED_SuccessAndDoNotSendEvent */

/* end CF_TableInit tests */

/*******************************************************************************
**
**  CF_Init tests - full coverage
**
*******************************************************************************/

void Test_CF_Init_CallTo_CFE_EVS_Register_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 forced_return_CFE_MSG_Init = Any_int32_Except(CFE_SUCCESS);
    int32 result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_Register), forced_return_CFE_MSG_Init);

    /* Act */
    result = CF_Init();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 2);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
    UtAssert_INT32_EQ(result, forced_return_CFE_MSG_Init);
} /* end Test_CF_Init_CallTo_CFE_EVS_Register_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus */

void Test_CF_Init_CallTo_CFE_SB_CreatePipe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 forced_return_CFE_SB_CreatePipe = Any_int32_Except(CFE_SUCCESS);
    int32 result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), forced_return_CFE_SB_CreatePipe);

    /* Act */
    result = CF_Init();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 2);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
    UtAssert_INT32_EQ(result, forced_return_CFE_SB_CreatePipe);
} /* end Test_CF_Init_CallTo_CFE_SB_CreatePipe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus */

void Test_CF_Init_FirstCallTo_CFE_SB_Subscribe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 forced_return_CFE_SB_Subscribe = Any_int32_Except(CFE_SUCCESS);
    int32 result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_Subscribe), forced_return_CFE_SB_Subscribe);

    /* Act */
    result = CF_Init();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 2);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 1);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
    UtAssert_INT32_EQ(result, forced_return_CFE_SB_Subscribe);
} /* end Test_CF_Init_FirstCallTo_CFE_SB_Subscribe_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus */

/* NOTE: multi call test for CFE_SB_Subscribe would be helpful but not necessary for coverage */

void Test_CF_Init_CallTo_CF_TableInit_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 result;

    /* Arrange unstubbable: CF_TableInit */
    int32 forced_return_CFE_TBL_Register = Any_int32_Except(CFE_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(CFE_TBL_Register), forced_return_CFE_TBL_Register);

    /* Act */
    result = CF_Init();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 2);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 3);
    /* Assert for CF_TableInit */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(result, forced_return_CFE_TBL_Register);
} /* end Test_CF_Init_CallTo_CF_TableInit_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus */

void Test_CF_Init_CallTo_CF_CFDP_InitEngine_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 result;
    int32 forced_return_CF_CFDP_InitEngine = Any_int32_Except(CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CF_CFDP_InitEngine), forced_return_CF_CFDP_InitEngine);

    /* Act */
    result = CF_Init();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 2);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 3);
    // UtAssert_STUB_COUNT(CF_TableInit, 1);
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_INT32_EQ(result, forced_return_CF_CFDP_InitEngine);
} /* end Test_CF_Init_CallTo_CF_CFDP_InitEngine_ReturnsNot_CFE_SUCCESS_ReturnErrorStatus */

void Test_CF_Init_CallTo_CFE_EVS_SendEvent_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus(void)
{
    /* Arrange */
    int32 force_return_CFE_EVS_SendEvent = Any_int32_Except(CFE_SUCCESS);
    int32 result;

    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_SendEvent), force_return_CFE_EVS_SendEvent);

    /* Act */
    result = CF_Init();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 2);
    UtAssert_STUB_COUNT(CFE_EVS_Register, 1);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_Subscribe, 3);
    // UtAssert_STUB_COUNT(CF_TableInit, 1);
    UtAssert_STUB_COUNT(CF_CFDP_InitEngine, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_INT32_EQ(result, force_return_CFE_EVS_SendEvent);
} /* end Test_CF_Init_CallTo_CFE_EVS_SendEvent_ReturnsNot_CFE_SUCCESS_Call_CFE_ES_WriteToSysLog_ReturnErrorStatus */

void Test_CF_Init_Success(void)
{
    /* Arrange */
    int32 result;

    /* Act */
    result = CF_Init();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_Init, 2);
    UtAssert_INT32_EQ(result, CFE_SUCCESS);
} /* end Test_CF_Init_Success */

/* end CF_Init tests */

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
} /* end Test_CF_WakeUp */

/*******************************************************************************
**
**  CF_ProcessMsg tests
**
*******************************************************************************/

void Test_CF_ProcessMsg_ProcessGroundCommand(void)
{
    /* Arrange */
    CFE_SB_Buffer_t            dummy_msg;
    CFE_SB_Buffer_t           *arg_msg      = &dummy_msg;
    CFE_SB_MsgId_t             forced_MsgID = CFE_SB_ValueToMsgId(CF_CMD_MID);
    CFE_SB_Buffer_t           *context_CF_ProcessGroundCommand_msg;
    CFE_MSG_GetMsgId_context_t context_CFE_MSG_GetMsgId;

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetMsgId), UT_Hook_CFE_MSG_GetMsgId, &context_CFE_MSG_GetMsgId);

    UT_SetDataBuffer(UT_KEY(CF_ProcessGroundCommand), &context_CF_ProcessGroundCommand_msg,
                     sizeof(context_CF_ProcessGroundCommand_msg), false);

    /* Act */
    CF_ProcessMsg(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetMsgId.MsgPtr, (CFE_MSG_Message_t *)arg_msg);
    UtAssert_STUB_COUNT(CF_ProcessGroundCommand, 1);
    UtAssert_ADDRESS_EQ(context_CF_ProcessGroundCommand_msg, arg_msg);
} /* end Test_CF_ProcessMsg_ProcessGroundCommand */

void Test_CF_ProcessMsg_WakeUp(void)
{
    /* Arrange */
    CFE_SB_Buffer_t            dummy_msg;
    CFE_SB_Buffer_t           *arg_msg      = &dummy_msg;
    CFE_SB_MsgId_t             forced_MsgID = CFE_SB_ValueToMsgId(CF_WAKE_UP_MID);
    CFE_MSG_GetMsgId_context_t context_CFE_MSG_GetMsgId;

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetMsgId), UT_Hook_CFE_MSG_GetMsgId, &context_CFE_MSG_GetMsgId);

    /* Act */
    CF_ProcessMsg(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_GetMsgId.MsgPtr, &arg_msg->Msg);
    /* Assert Unstubbable - CF_WakeUp */
    UtAssert_STUB_COUNT(CF_CFDP_CycleEngine, 1);
} /* end Test_CF_ProcessMsg_WakeUp */

void Test_CF_ProcessMsg_SendHk(void)
{
    /* Arrange */
    /* TODO: figure out if message is required here - not used at time of writing this */
    // CFE_MSG_Message_t   dummy_Msg;
    // CFE_SB_Buffer_t     dummy_msg;
    CFE_SB_Buffer_t           *arg_msg      = NULL;
    CFE_SB_MsgId_t             forced_MsgID = CFE_SB_ValueToMsgId(CF_SEND_HK_MID);
    CFE_MSG_GetMsgId_context_t context_CFE_MSG_GetMsgId;

    /* CFE_MSG_GetMsgId uses return by ref */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetMsgId), UT_Hook_CFE_MSG_GetMsgId, &context_CFE_MSG_GetMsgId);
    /* CFE_MSG_GetMsgId return value ignored in code! Therefore no setup of return value requried */

    /* Arrange unstubbable: CF_HkCmd exists in same source file */
    CFE_SB_TransmitMsg_context_t context_CFE_SB_TransmitMsg;

    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_Hook_CFE_SB_TransmitMsg, &context_CFE_SB_TransmitMsg);

    /* Act */
    CF_ProcessMsg(arg_msg);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_GetMsgId, 1);
    /* Assert for CF_HkCmd*/
    UtAssert_ADDRESS_EQ(context_CFE_SB_TransmitMsg.MsgPtr, &CF_AppData.hk.tlm_header.Msg);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_ProcessMsg_SendHk */

void Test_CF_ProcessMsg_UnrecognizedCommandEnterDefaultPath(void)
{
    /* Arrange */
    uint16                      initial_err_count = CF_AppData.hk.counters.err;
    CFE_SB_MsgId_Atom_t         midval;
    CFE_SB_MsgId_t              forced_MsgID;
    CFE_SB_Buffer_t            *arg_msg = NULL;
    CFE_MSG_GetMsgId_context_t  context_CFE_MSG_GetMsgId;
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    /* do not use one of the three real MIDs.
     * As this depends on configuration, should not hardcode values here
     */
    midval = 1;
    while (midval == CF_CMD_MID || midval == CF_WAKE_UP_MID || midval == CF_SEND_HK_MID)
    {
        ++midval;
    }

    forced_MsgID = CFE_SB_ValueToMsgId(midval);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &forced_MsgID, sizeof(forced_MsgID), false);
    UT_SetHookFunction(UT_KEY(CFE_MSG_GetMsgId), UT_Hook_CFE_MSG_GetMsgId, &context_CFE_MSG_GetMsgId);
    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_Hook_CFE_EVS_SendEvent, &context_CFE_EVS_SendEvent);

    /* Act */
    CF_ProcessMsg(arg_msg);

    /* Assert */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_err_count + 1),
                  "CF_AppData.hk.counters.err is %d which is 1 more than %d", CF_AppData.hk.counters.err,
                  initial_err_count);
    /* TODO: event sent not checked, does it need to be? found 1/19/21 */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UT_CF_AssertEventID(CF_EID_ERR_INIT_CMD_LENGTH);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);

} /* end Test_CF_ProcessMsg_UnrecognizedCommandEnterDefaultPath */

/* end CF_ProcessMsg tests */

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

    /* Arrange unstubbable: CF_Init */
    CFE_Status_t forced_return_CFE_EVS_Register = Any_CFE_Status_t_Except(CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_EVS_Register), forced_return_CFE_EVS_Register);

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(CF_AppData.run_status, CFE_ES_RunStatus_APP_ERROR);
} /* end Test_CF_AppMain_CallTo_CF_Init_DoNotReturn_CFE_SUCCESS_Set_CF_AppData_run_status_To_CFE_ES_RunStatus_APP_ERROR
   */

void Test_CF_AppMain_CallTo_CFE_ES_RunLoop_Returns_false_AppExit(void)
{
    /* Arrange */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 1);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
    /* Assert for CF_Init call */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_AppMain_CallTo_CFE_ES_RunLoop_Returns_false_AppExit */

void Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_ReturnsNot_CFE_SUCCESS_AndNot_CFE_SB_TIME_OUT_SendEvent(void)
{
    /* Arrange */
    int32 forced_return_CFE_SB_ReceiveBuffer =
        Any_int32_Negative(); /* TODO: has to be negative unless a handler is used */
    CFE_SB_Buffer_t *dummy_msg = NULL;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    /* TODO: setting the data buffer only removes a TSF, otherwise in this test it does nothing */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &dummy_msg, sizeof(dummy_msg), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), forced_return_CFE_SB_ReceiveBuffer);

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 4);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 2);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
    /* Assert for CF_Init call and CF_AppMain */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
} /* end Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_ReturnsNot_CFE_SUCCESS_AndNot_CFE_SB_TIME_OUT_SendEvent */

void Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_And_msg_Is_NULL_SendEvent(void)
{
    /* Arrange */
    int32            forced_return_CFE_SB_ReceiveBuffer = CFE_SUCCESS;
    CFE_SB_Buffer_t *dummy_msg                          = NULL;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    /* TODO: setting the data buffer only removes a TSF, otherwise in this test it does nothing */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &dummy_msg, sizeof(dummy_msg), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), forced_return_CFE_SB_ReceiveBuffer);

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 4);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 2);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
    /* Assert for CF_Init call and CF_AppMain */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
} /* end Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_And_msg_Is_NULL_SendEvent */

void Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SB_TIME_OUT_And_msg_Is_NULL_DoNothingDuringLoop(
    void)
{
    /* Arrange */
    int32            forced_return_CFE_SB_ReceiveBuffer = CFE_SB_TIME_OUT;
    CFE_SB_Buffer_t *dummy_BufPtr                       = NULL;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    /* TODO: setting the data buffer only removes a TSF, otherwise in this test it does nothing */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &dummy_BufPtr, sizeof(dummy_BufPtr), false);
    UT_SetHandlerFunction(UT_KEY(CFE_SB_ReceiveBuffer), UT_UpdatedDefaultHandler_CFE_SB_ReceiveBuffer, NULL);
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), forced_return_CFE_SB_ReceiveBuffer);

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 4);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 2);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
    /* Assert for CF_Init call, but CF_AppMain did not send event */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SB_TIME_OUT_And_msg_Is_NULL_DoNothingDuringLoop
   */

void Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_AndValid_msg_Call_CF_ProcessMsg(void)
{
    /* Arrange */
    int32            forced_return_CFE_SB_ReceiveBuffer = CFE_SUCCESS;
    CFE_SB_Buffer_t  fake_msg;
    CFE_SB_Buffer_t *dummy_msg               = &fake_msg;
    uint16           initial_hk_counters_err = Any_uint16();

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RunLoop), 1, true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_RunLoop), false);

    /* TODO: setting the data buffer only removes a TSF, otherwise in this test it does nothing */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &dummy_msg, sizeof(dummy_msg), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), forced_return_CFE_SB_ReceiveBuffer);

    /* Arrange unstubbable: CF_ProcessMsg */
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetMsgId),
                             UINT32_MAX); /* UINT32_MAX selected because it should error out */
    CF_AppData.hk.counters.err = initial_hk_counters_err;

    /* Act */
    CF_AppMain();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 4);
    UtAssert_STUB_COUNT(CFE_ES_RunLoop, 2);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
    /* Assert for CF_Init call and CF_ProcessMsg */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    /* Assert for CF_ProcessMsg */
    UtAssert_True(CF_AppData.hk.counters.err == (uint16)(initial_hk_counters_err + 1),
                  "CF_AppData.hk.counters.err is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.counters.err, initial_hk_counters_err);
} /* end Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_AndValid_msg_Call_CF_ProcessMsg */

/* end CF_AppMain tests */

/*******************************************************************************
**
**  cf_app_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_HkCmd_tests(void)
{
    UtTest_Add(Test_CF_HkCmd_TimestampAndSendMessageWith_CF_AppData_hk, cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_HkCmd_TimestampAndSendMessageWith_CF_AppData_hk");
} /* end add_CF_HkCmd_tests */

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
} /* end add_CF_CheckTables_tests */

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
} /* end add_CF_ValidateConfigTable_tests */

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
} /* end add_CF_TableInit_tests */

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
} /* end add_CF_Init_tests */

void add_CF_WakeUp_tests(void)
{
    UtTest_Add(Test_CF_WakeUp, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_WakeUp");
} /* end add_CF_WakeUp_tests */

void add_CF_ProcessMsg_tests(void)
{
    UtTest_Add(Test_CF_ProcessMsg_ProcessGroundCommand, cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_ProcessMsg_ProcessGroundCommand");
    UtTest_Add(Test_CF_ProcessMsg_WakeUp, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_ProcessMsg_WakeUp");
    UtTest_Add(Test_CF_ProcessMsg_SendHk, cf_app_tests_Setup, CF_App_Tests_Teardown, "Test_CF_ProcessMsg_SendHk");
    UtTest_Add(Test_CF_ProcessMsg_UnrecognizedCommandEnterDefaultPath, cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_ProcessMsg_UnrecognizedCommandEnterDefaultPath");
} /* end add_CF_ProcessMsg_tests */

void add_CF_AppMain_tests(void)
{
    UtTest_Add(
        Test_CF_AppMain_CallTo_CF_Init_DoNotReturn_CFE_SUCCESS_Set_CF_AppData_run_status_To_CFE_ES_RunStatus_APP_ERROR,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_AppMain_CallTo_CF_Init_DoNotReturn_CFE_SUCCESS_Set_CF_AppData_run_status_To_CFE_ES_RunStatus_APP_"
        "ERROR");
    UtTest_Add(Test_CF_AppMain_CallTo_CFE_ES_RunLoop_Returns_false_AppExit, cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_AppMain_CallTo_CFE_ES_RunLoop_Returns_false_AppExit");
    UtTest_Add(
        Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_ReturnsNot_CFE_SUCCESS_AndNot_CFE_SB_TIME_OUT_SendEvent,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_ReturnsNot_CFE_SUCCESS_AndNot_CFE_SB_TIME_OUT_SendEvent");
    UtTest_Add(Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_And_msg_Is_NULL_SendEvent,
               cf_app_tests_Setup, CF_App_Tests_Teardown,
               "Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_And_msg_Is_NULL_SendEvent");
    UtTest_Add(
        Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SB_TIME_OUT_And_msg_Is_NULL_DoNothingDuringLoop,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SB_TIME_OUT_And_msg_Is_NULL_"
        "DoNothingDuringLoop");
    UtTest_Add(
        Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_AndValid_msg_Call_CF_ProcessMsg,
        cf_app_tests_Setup, CF_App_Tests_Teardown,
        "Test_CF_AppMain_RunLoopCallTo_CFE_SB_ReceiveBuffer_Returns_CFE_SUCCESS_AndValid_msg_Call_CF_ProcessMsg");
} /* end add_CF_AppMain_tests */

/* end cf_app_tests UtTest_Add groups */

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
/* end cf_app_tests.c */
