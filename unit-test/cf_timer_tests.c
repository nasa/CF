/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_timer.c"

/*******************************************************************************
**
**  cf_timer_tests Setup and Teardown
**
*******************************************************************************/

void cf_timer_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_timer_tests_Setup */

void cf_timer_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_timer_tests_Teardown */

/* end cf_timer_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_Timer_Sec2Ticks tests
**
*******************************************************************************/

void Test_CF_Timer_Sec2Ticks_ReturnExpectedValue(void)
{
    /* Arrange */
    cf_timer_sec_t    arg_sec                = Any_uint32();
    uint32            dummy_ticks_per_second = Any_uint32();
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table                   = &dummy_config_table;
    CF_AppData.config_table->ticks_per_second = dummy_ticks_per_second;

    /* Act */
    result = CF_Timer_Sec2Ticks(arg_sec);

    /* Assert */
    UtAssert_True(result == arg_sec * dummy_ticks_per_second, "Result was %u and should be %u", result,
                  arg_sec * dummy_ticks_per_second);
} /* end Test_CF_Timer_Sec2Ticks_ReturnExpectedValue */

/* end CF_Timer_Sec2Ticks tests */

/*******************************************************************************
**
**  CF_Timer_InitRelSec tests
**
*******************************************************************************/

void Test_CF_Timer_InitRelSec_ReceiveExpectedValue(void)
{
    /* Arrange */
    uint32      arg_rel_sec = Any_uint32();
    cf_timer_t  dummy_timer;
    cf_timer_t *arg_t = &dummy_timer;

    /* Arrange unstubbalbe: CF_Timer_Sec2Ticks in same file */
    uint32            dummy_ticks_per_second = Any_uint32();
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table                   = &dummy_config_table;
    CF_AppData.config_table->ticks_per_second = dummy_ticks_per_second;

    arg_t->tick = dummy_ticks_per_second;

    /* Act */
    CF_Timer_InitRelSec(arg_t, arg_rel_sec);

    /* Assert */
    UtAssert_True(arg_t->tick == arg_rel_sec * dummy_ticks_per_second,
                  "Timer ticks are %u and should be %u (return from CF_Timer_Sec2Ticks)", arg_t->tick,
                  arg_rel_sec * dummy_ticks_per_second);
} /* end Test_CF_Timer_InitRelSec_ReceiveExpectedValue */

/* end CF_Timer_InitRelSec tests */

/*******************************************************************************
**
**  CF_Timer_Expired tests
**
*******************************************************************************/

void Test_CF_Timer_Expired_When_t_tick_Is_0_Return_1(void)
{
    /* Arrange */
    cf_timer_t dummy_timer;
    dummy_timer.tick        = 0;
    const cf_timer_t *arg_t = &dummy_timer;
    int               local_result;
    int               expected_result = 1;

    /* Act */
    local_result = CF_Timer_Expired(arg_t);

    /* Assert */
    UtAssert_True(local_result == expected_result, "Result was %u and should be %u", local_result, expected_result);
} /* end Test_CF_Timer_Expired_When_t_tick_Is_0_Return_1 */

void Test_CF_Timer_Expired_When_t_tick_Is_1_Return_0(void)
{
    /* Arrange */
    cf_timer_t dummy_timer;
    dummy_timer.tick        = 1;
    const cf_timer_t *arg_t = &dummy_timer;
    int               local_result;
    int               expected_result = 0;

    /* Act */
    local_result = CF_Timer_Expired(arg_t);

    /* Assert */
    UtAssert_True(local_result == expected_result, "Result was %u and should be %u", local_result, expected_result);
} /* end Test_CF_Timer_Expired_When_t_tick_Is_1_Return_0 */

void Test_CF_Timer_Expired_When_t_tick_IsAnyIntegerExcept_0_Return_0(void)
{
    /* Arrange */
    cf_timer_t dummy_timer;
    dummy_timer.tick        = Any_int_Except(0);
    const cf_timer_t *arg_t = &dummy_timer;
    int               local_result;
    int               expected_result = 0;

    /* Act */
    local_result = CF_Timer_Expired(arg_t);

    /* Assert */
    UtAssert_True(local_result == expected_result, "Result was %u and should be %u", local_result, expected_result);
} /* end Test_CF_Timer_Expired_When_t_tick_IsAnyIntegerExcept_0_Return_0 */

/* end CF_Timer_Expired tests */

/*******************************************************************************
**
**  CF_Timer_Tick tests
**
*******************************************************************************/

void Test_CF_Timer_TickAssertsBecause_t_tick_Is_0(void)
{
    // /* Arrange */

    // /* Act */

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->tick");
} /* Test_CF_Timer_TickAssertsBecause_t_tick_Is_0 */

void Test_CF_Timer_Tick_When_t_tick_Is_non0_Decrement_t_tick(void)
{
    /* Arrange */
    uint32      initial_tick = Any_uint32_Except(0);
    cf_timer_t  dummy_t;
    cf_timer_t *arg_t = &dummy_t;

    arg_t->tick = initial_tick;

    /* Act */
    CF_Timer_Tick(arg_t);

    /* Assert */
    UtAssert_True(arg_t->tick == initial_tick - 1, "tick is %d and should have decreased by 1 from %d", arg_t->tick,
                  initial_tick);

} /* Test_CF_Timer_Tick_When_t_tick_Is_non0_Decrement_t_tick */

/* end CF_Timer_Tick tests */

/*******************************************************************************
**
**  cf_timer_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_Timer_Sec2Ticks_tests(void)
{
    UtTest_Add(Test_CF_Timer_Sec2Ticks_ReturnExpectedValue, cf_timer_tests_Setup, cf_timer_tests_Teardown,
               "Test_CF_Timer_Sec2Ticks_ReturnExpectedValue");
}

void add_CF_Timer_InitRelSec_tests(void)
{
    UtTest_Add(Test_CF_Timer_InitRelSec_ReceiveExpectedValue, cf_timer_tests_Setup, cf_timer_tests_Teardown,
               "Test_CF_Timer_InitRelSec_ReceiveExpectedValue");
}

void add_CF_Timer_Expired_tests(void)
{
    UtTest_Add(Test_CF_Timer_Expired_When_t_tick_Is_0_Return_1, cf_timer_tests_Setup, cf_timer_tests_Teardown,
               "Test_CF_Timer_Expired_When_t_tick_Is_0_Return_1");

    UtTest_Add(Test_CF_Timer_Expired_When_t_tick_Is_1_Return_0, cf_timer_tests_Setup, cf_timer_tests_Teardown,
               "Test_CF_Timer_Expired_When_t_tick_Is_1_Return_0");

    UtTest_Add(Test_CF_Timer_Expired_When_t_tick_IsAnyIntegerExcept_0_Return_0, cf_timer_tests_Setup,
               cf_timer_tests_Teardown, "Test_CF_Timer_Expired_When_t_tick_IsAnyIntegerExcept_0_Return_0");
}

void add_CF_Timer_Tick_tests(void)
{
    UtTest_Add(Test_CF_Timer_TickAssertsBecause_t_tick_Is_0, cf_timer_tests_Setup, cf_timer_tests_Teardown,
               "Test_CF_Timer_TickAssertsBecause_t_tick_Is_0");
    UtTest_Add(Test_CF_Timer_Tick_When_t_tick_Is_non0_Decrement_t_tick, cf_timer_tests_Setup, cf_timer_tests_Teardown,
               "Test_CF_Timer_Tick_When_t_tick_Is_non0_Decrement_t_tick");
}

/* end cf_timer_tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_timer_tests test UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_Timer_Sec2Ticks_tests();

    add_CF_Timer_InitRelSec_tests();

    add_CF_Timer_Expired_tests();

    add_CF_Timer_Tick_tests();

} /* end UtTest_Setup cf_timer_tests.c */

/* end cf_timer_tests.c */