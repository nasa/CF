/* library includes */
#include <time.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* cf testing includes */
#include "source_file.c"
#include "cf_test_utils.h"

/* source_file_tests globals */

uint8 expectedCfeEvsSendEventCallCount;

/*******************************************************************************
**
**  source_file_tests local utility functions
**
*******************************************************************************/

void reset_source_file_tests_globals(void)
{
    result                           = UT_INT_32_DEFAULT;
    EventID                          = UT_INT_16_DEFAULT;
    expectedCfeEvsSendEventCallCount = 0;
} /* end reset_source_file_tests_globals */

/* end source_file_tests local utility functions */

/*******************************************************************************
**
**  source_file_tests Setup and Teardown
**
*******************************************************************************/

void source_file_tests_Setup(void)
{
    cf_tests_Setup();
    reset_source_file_tests_globals();
} /* end source_file_tests_Setup */

void source_file_tests_Teardown(void)
{
    /* No Operation */
} /* end source_file_tests_Teardown */

/* end source_file_tests Setup and Teardown */

/*******************************************************************************
**
**  source_file tests
**
*******************************************************************************/

void test_source_file_default(void)
{
    /* Arrange */

    /* Act */

    /* Assert */
    UtAssert_Failed("Template default test always fails and needs removed");
} /* end test_source_file_default */

/*******************************************************************************
**
**  source_file_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    UtTest_Add(test_source_file_default, source_file_tests_Setup, source_file_tests_Teardown,
               "test_source_file_default");
} /* end UtTest_Setup for source_file_tests.c */

/* end source_file_tests.c */