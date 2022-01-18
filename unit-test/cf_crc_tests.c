/* cf testing includes */
#include "cf_test_utils.h"

/*******************************************************************************
**
**  cf_crc_tests local utility functions
**
*******************************************************************************/

uint8 Any_cf_crc_index(void)
{
    uint8 possible_indexes[4] = {0, 1, 2, 3};

    return Any_uint8_FromThese(possible_indexes, sizeof(possible_indexes) / sizeof(possible_indexes[0]));
}

uint8 Any_cf_crc_index_except_3(void)
{
    uint8 possible_indexes[3] = {0, 1, 2};

    return Any_uint8_FromThese(possible_indexes, sizeof(possible_indexes) / sizeof(possible_indexes[0]));
}
/* end cf_crc_tests local utility functions */

/*******************************************************************************
**
**  cf_crc_tests Setup and Teardown
**
*******************************************************************************/

void cf_crc_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_crc_tests_Setup */

void cf_crc_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_crc_tests_Teardown */

/* end cf_crc_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_CRC_Start tests
**
*******************************************************************************/
void test_CF_CRC_Start_ReinitializeGiven_c_ToAllZeroValues(void)
{
    /* Arrange */
    CF_Crc_t  dummy_c;
    CF_Crc_t *arg_c = &dummy_c;

    arg_c->working = Any_uint32_Except(0);
    arg_c->result  = Any_uint32_Except(0);
    arg_c->index   = Any_uint8_Except(0);

    /* Act */
    CF_CRC_Start(arg_c);

    /* Assert */
    UtAssert_ZERO(arg_c->working);
    UtAssert_ZERO(arg_c->result);
    UtAssert_ZERO(arg_c->index);

} /* end test_CF_CRC_Start_ReinitializeGiven_c_ToAllZeroValues*/

/* end CF_CRC_Start tests */

/*******************************************************************************
**
**  CF_CRC_Digest tests
**
*******************************************************************************/

/* NOTE: not testing len > data's size, by contract this should not occur,
** should it be an assumption in the doxygen comments? */

void Test_CF_CRC_Digest_When_len_Is_0_DoNotAlter_c_working_or_c_result_or_c_index(void)
{
    /* Arrange */
    CF_Crc_t    dummy_c;
    CF_Crc_t   *arg_c             = &dummy_c;
    const uint8 arg_data[1]       = {UT_UINT_8_DEFAULT};
    int32       arg_len           = 0;
    uint32      initial_c_working = Any_uint32();
    uint32      initial_c_result  = Any_uint32();
    uint8       initial_c_index   = Any_cf_crc_index();

    arg_c->working = initial_c_working;
    arg_c->result  = initial_c_result;
    arg_c->index   = initial_c_index;

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->working, initial_c_working);
    UtAssert_UINT32_EQ(arg_c->result, initial_c_result);
    UtAssert_UINT32_EQ(arg_c->index, initial_c_index);

} /* end Test_CF_CRC_Digest_When_len_Is_0_DoNotAlter_c_working_or_c_result_or_c_index */

void Test_CF_CRC_Digest_When_len_Eq_1_PushDataLeftOnto_c_working(void)
{
    /* Arrange */
    CF_Crc_t    dummy_c;
    CF_Crc_t   *arg_c       = &dummy_c;
    const uint8 arg_data[1] = {UT_UINT_8_DEFAULT};
    int32       arg_len     = 1;
    int         i           = 0;
    uint32      expected_c_working;

    arg_c->working = Any_uint32();
    arg_c->index   = Any_cf_crc_index();

    expected_c_working = arg_c->working;
    expected_c_working <<= 8;
    expected_c_working += arg_data[i];

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->working, expected_c_working);

} /* end Test_CF_CRC_Digest_When_len_Eq_1_PushDataLeftOnto_c_working */

void Test_CF_CRC_Digest_PushDataLeftOnto_c_working_NumberOfTimesEqTo_len(void)
{
    /* Arrange */
    CF_Crc_t     dummy_c;
    CF_Crc_t    *arg_c = &dummy_c;
    const uint8 *arg_data;
    int32        arg_len;
    int          i = 0;
    uint32       expected_c_working;

    arg_c->working = Any_uint32();
    arg_c->index   = Any_cf_crc_index();
    arg_len        = Any_uint16_GreaterThan(1) /
              2; // NOTE: change type if len changes,  Any_uint16_GreaterThan(1) / 2 roughly translates to a positive
                 // int that runs the test within a reasonable timeframe for size of len, this could change when more is
                 // learned about what len is supposed to be

    arg_data = malloc((size_t)arg_len);
    AnyBufferOf_uint8_WithSize((uint8 *)arg_data, arg_len);

    expected_c_working = arg_c->working;
    for (i = 0; i < arg_len; ++i)
    {
        expected_c_working <<= 8;
        expected_c_working |= arg_data[i];
    }

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->working, expected_c_working);

    /* local Teardown */
    free((uint8 *)arg_data);

} /* end Test_CF_CRC_Digest_PushDataLeftOnto_c_working_NumberOfTimesEqTo_len */

void Test_CF_CRC_Digest_When_index_IsNot_3_DoNotUpdate_c_result(void)
{
    /* Arrange */
    CF_Crc_t    dummy_c;
    CF_Crc_t   *arg_c            = &dummy_c;
    const uint8 arg_data[1]      = {UT_UINT_8_DEFAULT};
    int32       arg_len          = 1;
    uint32      initial_c_result = Any_uint32();

    arg_c->working = Any_uint32();
    arg_c->result  = initial_c_result;
    arg_c->index   = Any_cf_crc_index_except_3();

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->result, initial_c_result);

} /* end Test_CF_CRC_Digest_When_index_IsNot_3_DoNotUpdate_c_result */

void Test_CF_CRC_Digest_When_c_index_Is_3_Update_c_result(void)
{
    /* Arrange */
    CF_Crc_t    dummy_c;
    CF_Crc_t   *arg_c       = &dummy_c;
    const uint8 arg_data[1] = {UT_UINT_8_DEFAULT};
    int32       arg_len;
    int         i = 0;
    uint32      expected_c_result;

    arg_c->working = Any_uint32();
    arg_c->index   = 3;
    arg_c->result  = Any_uint32();
    arg_len        = 1;

    expected_c_result = arg_c->working;
    expected_c_result <<= 8;
    expected_c_result += arg_data[i];
    expected_c_result += arg_c->result;

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->result, expected_c_result);

} /* end Test_CF_CRC_Digest_When_c_index_Is_3_Update_c_result */

void Test_CF_CRC_Digest_Update_c_result_TheNumberOfTimes_index_Reaches4(void)
{
    /* Arrange */
    CF_Crc_t     dummy_c;
    CF_Crc_t    *arg_c = &dummy_c;
    const uint8 *arg_data;
    int32        arg_len;
    uint32       dummy_c_working;
    int          i = 0;
    unsigned int num_times_index_at_4;
    uint32       expected_c_result;

    arg_c->working = Any_uint32();
    arg_c->index   = Any_cf_crc_index();
    arg_c->result  = Any_uint32();
    arg_len        = (int32)(Any_uint16_GreaterThan(1) /
                      2); // NOTE: change type if len changes,  Any_uint16_GreaterThan(1) / 2 roughly translates to a
                          // positive int that runs the test within a reasonable timeframe for size of len, this could
                          // change when more is learned about what len is supposed to be

    arg_data = malloc((size_t)arg_len);
    AnyBufferOf_uint8_WithSize((uint8 *)arg_data, arg_len);

    dummy_c_working   = arg_c->working;
    expected_c_result = arg_c->result;

    for (i = 0; i < (3 - arg_c->index); ++i) /* num shifts before first add */
    {
        dummy_c_working <<= 8;
        dummy_c_working += arg_data[i];
    } /* end for */

    num_times_index_at_4 = (arg_len + arg_c->index) / 4;

    for (i = 0; i < num_times_index_at_4; ++i)
    {
        dummy_c_working <<= 8;
        dummy_c_working += arg_data[(3 - arg_c->index) + (i * 4)];
        expected_c_result += dummy_c_working;

        if (i + 1 != num_times_index_at_4)
        {
            int j;

            for (j = 1; j < 4; ++j)
            {
                dummy_c_working <<= 8;
                dummy_c_working += arg_data[(3 - arg_c->index) + (i * 4) + j];
            } /* end for */

        } /* end if */

    } /* end for */

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->result, expected_c_result);

    /* local Teardown */
    free((uint8 *)arg_data);

} /* end Test_CF_CRC_Digest_Update_c_result_TheNumberOfTimes_index_Reaches4 */

void Test_CF_CRC_Digest_When_len_Eq1_And_c_index_LessThan_3_Update_c_index_By_1(void)
{
    /* Arrange */
    CF_Crc_t    dummy_c;
    CF_Crc_t   *arg_c       = &dummy_c;
    const uint8 arg_data[1] = {UT_UINT_8_DEFAULT};
    int32       arg_len;
    uint8       expected_c_index;

    arg_c->working = Any_uint32();
    arg_c->index   = Any_cf_crc_index_except_3();
    arg_c->result  = Any_uint32();
    arg_len        = 1;

    expected_c_index = arg_c->index + 1;

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->index, expected_c_index);

} /* end Test_CF_CRC_Digest_When_len_Eq1_And_c_index_LessThan_3_Update_c_index_By_1 */

void Test_CF_CRC_Digest_When_len_Eq1_And_c_index_Is_3_Update_c_index_To_0(void)
{
    /* Arrange */
    CF_Crc_t    dummy_c;
    CF_Crc_t   *arg_c       = &dummy_c;
    const uint8 arg_data[1] = {UT_UINT_8_DEFAULT};
    int32       arg_len;

    arg_c->working = Any_uint32();
    arg_c->index   = 3;
    arg_c->result  = Any_uint32();
    arg_len        = 1;

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_ZERO(arg_c->index);

} /* end Test_CF_CRC_Digest_When_len_Eq1_And_c_index_Is_3_Update_c_index_To_0 */

void Test_CF_CRC_Digest_Update_c_index_CorrectlyDependingOn_c_index_And_len_Values(void)
{
    /* Arrange */
    CF_Crc_t     dummy_c;
    CF_Crc_t    *arg_c = &dummy_c;
    const uint8 *arg_data;
    int32        arg_len;
    uint8        expected_c_index;

    arg_c->working = Any_uint32();
    arg_c->index   = Any_cf_crc_index();
    arg_c->result  = Any_uint32();
    arg_len        = (int32)(Any_uint16_GreaterThan(1) /
                      2); // NOTE: change type if len changes,  Any_uint16_GreaterThan(1) / 2 roughly translates to a
                          // positive int that runs the test within a reasonable timeframe for size of len, this could
                          // change when more is learned about what len is supposed to be

    arg_data = malloc((size_t)arg_len);
    AnyBufferOf_uint8_WithSize((uint8 *)arg_data, arg_len);

    expected_c_index = (arg_len + arg_c->index) % 4;

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->index, expected_c_index);

    /* local Teardown */
    free((uint8 *)arg_data);

} /* end Test_CF_CRC_Digest_Update_c_index_CorrectlyDependingOn_c_index_And_len_Values */

/* end CF_CRC_Digest tests */

/*******************************************************************************
**
**  CF_CRC_Finalize tests
**
*******************************************************************************/

void Test_CF_CRC_Finalize_When_index_Is_0_DoNothing(void)
{
    /* Arrange */
    CF_Crc_t  dummy_c;
    CF_Crc_t *arg_c             = &dummy_c;
    uint32    initial_c_working = Any_uint32();
    uint32    initial_c_result  = Any_uint32();

    arg_c->working = initial_c_working;
    arg_c->result  = initial_c_result;
    arg_c->index   = 0;

    /* Act */
    CF_CRC_Finalize(arg_c);

    /* Assert */
    UtAssert_UINT32_EQ(arg_c->working, initial_c_working);
    UtAssert_UINT32_EQ(arg_c->result, initial_c_result);
    UtAssert_ZERO(arg_c->index);
} /* end Test_CF_CRC_Finalize_When_index_Is_0_DoNothing */

void Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_1(void)
{
    /* Arrange */
    CF_Crc_t  dummy_c;
    CF_Crc_t *arg_c         = &dummy_c;
    uint32    working_shift = 256 * 256 * 256; /* 3 left shifts */
    uint32    expected_result;

    arg_c->index   = 1;
    arg_c->working = Any_uint32();
    arg_c->result  = Any_uint32();

    expected_result = arg_c->result + (arg_c->working * working_shift);

    /* Act */
    CF_CRC_Finalize(arg_c);

    /* Assert */
    UtAssert_ZERO(arg_c->working);
    UtAssert_UINT32_EQ(arg_c->result, expected_result);
    UtAssert_ZERO(arg_c->index);
} /* end Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_1 */

void Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_2(void)
{
    /* Arrange */
    CF_Crc_t  dummy_c;
    CF_Crc_t *arg_c         = &dummy_c;
    uint8     dummy_index   = 2;
    uint32    working_shift = 256 * 256; /* 2 left shifts */
    uint32    expected_result;

    arg_c->index   = dummy_index;
    arg_c->working = Any_uint32();
    arg_c->result  = Any_uint32();

    expected_result = arg_c->result + (arg_c->working * working_shift);

    /* Act */
    CF_CRC_Finalize(arg_c);

    /* Assert */
    UtAssert_ZERO(arg_c->working);
    UtAssert_UINT32_EQ(arg_c->result, expected_result);
    UtAssert_ZERO(arg_c->index);
} /* end Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_2 */

void Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_3(void)
{
    /* Arrange */
    CF_Crc_t  dummy_c;
    CF_Crc_t *arg_c         = &dummy_c;
    uint8     dummy_index   = 3;
    uint32    working_shift = 256; /* 1 left shift */
    uint32    expected_result;

    arg_c->index   = dummy_index;
    arg_c->working = Any_uint32();
    arg_c->result  = Any_uint32();

    expected_result = arg_c->result + (arg_c->working * working_shift);

    /* Act */
    CF_CRC_Finalize(arg_c);

    /* Assert */
    UtAssert_ZERO(arg_c->working);
    UtAssert_UINT32_EQ(arg_c->result, expected_result);
    UtAssert_ZERO(arg_c->index);
} /* end Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_3 */

/* end CF_CRC_Finalize tests */

/*******************************************************************************
**
**  cf_crc_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CRC_Start_tests(void)
{
    UtTest_Add(test_CF_CRC_Start_ReinitializeGiven_c_ToAllZeroValues, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "test_CF_CRC_Start_ReinitializeGiven_c_ToAllZeroValues");
} /* end add_CF_CRC_Start_tests */

void add_CF_CRC_Digest_tests(void)
{
    UtTest_Add(Test_CF_CRC_Digest_When_len_Is_0_DoNotAlter_c_working_or_c_result_or_c_index, cf_crc_tests_Setup,
               cf_crc_tests_Teardown, "Test_CF_CRC_Digest_When_len_Is_0_DoNotAlter_c_working_or_c_result_or_c_index");
    UtTest_Add(Test_CF_CRC_Digest_When_len_Eq_1_PushDataLeftOnto_c_working, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "Test_CF_CRC_Digest_When_len_Eq_1_PushDataLeftOnto_c_working");
    UtTest_Add(Test_CF_CRC_Digest_PushDataLeftOnto_c_working_NumberOfTimesEqTo_len, cf_crc_tests_Setup,
               cf_crc_tests_Teardown, "Test_CF_CRC_Digest_PushDataLeftOnto_c_working_NumberOfTimesEqTo_len");
    UtTest_Add(Test_CF_CRC_Digest_When_index_IsNot_3_DoNotUpdate_c_result, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "Test_CF_CRC_Digest_When_index_IsNot_3_DoNotUpdate_c_result");
    UtTest_Add(Test_CF_CRC_Digest_When_c_index_Is_3_Update_c_result, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "Test_CF_CRC_Digest_When_c_index_Is_3_Update_c_result");
    UtTest_Add(Test_CF_CRC_Digest_Update_c_result_TheNumberOfTimes_index_Reaches4, cf_crc_tests_Setup,
               cf_crc_tests_Teardown, "Test_CF_CRC_Digest_Update_c_result_TheNumberOfTimes_index_Reaches4");
    UtTest_Add(Test_CF_CRC_Digest_When_len_Eq1_And_c_index_LessThan_3_Update_c_index_By_1, cf_crc_tests_Setup,
               cf_crc_tests_Teardown, "Test_CF_CRC_Digest_When_len_Eq1_And_c_index_LessThan_3_Update_c_index_By_1");
    UtTest_Add(Test_CF_CRC_Digest_When_len_Eq1_And_c_index_Is_3_Update_c_index_To_0, cf_crc_tests_Setup,
               cf_crc_tests_Teardown, "Test_CF_CRC_Digest_When_len_Eq1_And_c_index_Is_3_Update_c_index_To_0");
    UtTest_Add(Test_CF_CRC_Digest_Update_c_index_CorrectlyDependingOn_c_index_And_len_Values, cf_crc_tests_Setup,
               cf_crc_tests_Teardown, "Test_CF_CRC_Digest_Update_c_index_CorrectlyDependingOn_c_index_And_len_Values");
} /* end add_CF_CRC_Digest_tests */

void add_CF_CRC_Finalize_tests(void)
{
    UtTest_Add(Test_CF_CRC_Finalize_When_index_Is_0_DoNothing, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "Test_CF_CRC_Finalize_When_index_Is_0_DoNothing");
    UtTest_Add(Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_1, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_1");
    UtTest_Add(Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_2, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_2");
    UtTest_Add(Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_3, cf_crc_tests_Setup, cf_crc_tests_Teardown,
               "Test_CF_CRC_Finalize_ReceiveExpectedResultAt_index_3");
} /* end add_CF_CRC_Finalize_tests */

/* end cf_crc_tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_crc_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_CRC_Start_tests();

    add_CF_CRC_Digest_tests();

    add_CF_CRC_Finalize_tests();
} /* end UtTest_Setup for cf_crc_tests.c */

/* end cf_crc_tests.c */
