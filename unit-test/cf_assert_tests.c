/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_events.h"
#include "cf_app.h"

/*******************************************************************************
**
**  cf_assert_tests Setup and Teardown
**
*******************************************************************************/

void cf_assert_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_assert_tests_Setup */

void cf_assert_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_assert_tests_Teardown */

/* end cf_assert_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_HandleAssert tests
**
*******************************************************************************/

void Test_CF_HandleAssert_SendEventAndExitWithNegativeOne(void)
{
    /* Arrange */
    const char *arg_file = AnyRandomStringOfLettersOfLength(10);
    int         arg_line = Any_int();
    uint32      context_CFE_ES_ExitApp_ExitStatus;

    UT_CF_ResetEventCapture(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDataBuffer(UT_KEY(CFE_ES_ExitApp), &context_CFE_ES_ExitApp_ExitStatus,
                     sizeof(context_CFE_ES_ExitApp_ExitStatus), false);

    /* Act */
    CF_HandleAssert(arg_file, arg_line);

    /* Assert */
    UT_CF_AssertEventID(CF_EID_ERR_ASSERT);
    UtAssert_True(context_CFE_ES_ExitApp_ExitStatus == -1, "CFE_ES_ExitApp received %d and should have recieved -1",
                  context_CFE_ES_ExitApp_ExitStatus);
} /* end Test_CF_HandleAssert_SendEventAndExitWithNegativeOne */

/* end CF_HandleAssert tests */

/*******************************************************************************
**
**  cf_assert_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_HandleAssert_tests(void)
{
    UtTest_Add(Test_CF_HandleAssert_SendEventAndExitWithNegativeOne, cf_assert_tests_Setup, cf_assert_tests_Teardown,
               "Test_CF_HandleAssert_SendEventAndExitWithNegativeOne");
} /* end add_CF_HandleAssert_tests */

/* end cf_assert_tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_assert_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_HandleAssert_tests();

} /* end UtTest_Setup for cf_assert_tests.c */

/* end cf_assert_tests.c */