
/* cf testing includes */
#include "cf_crc.c"
#include "cf_test_utils.h"


/*******************************************************************************
**
**  cf_crc_tests local utility functions
**
*******************************************************************************/

uint8 Any_cf_crc_index(void)
{    
    uint8           possible_indexes[4] = {0, 1, 2, 3};
    
    return Any_uint8_FromThese(possible_indexes, sizeof(possible_indexes)/sizeof(possible_indexes[0]));
}

uint8 Any_cf_crc_index_except_3(void)
{
    uint8           possible_indexes[3] = {0, 1, 2};
    
    return Any_uint8_FromThese(possible_indexes, sizeof(possible_indexes)/sizeof(possible_indexes[0]));
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
void test_CF_CRC_Start_ReinitializesGiven_c_ToAllZeroValues(void)
{
    // TODO: would rather be able to -wrap and therefor stub memset to test this
    /* Arrange */
    cf_crc_t    dummy_c;
    cf_crc_t*   arg_c = &dummy_c;
    
    arg_c->working = Any_uint32_Except(0);
    arg_c->result  = Any_uint32_Except(0);
    arg_c->index   = Any_uint8_Except(0);
    
    /* Act */
    CF_CRC_Start(arg_c);
    
    /* Assert */
    UtAssert_True(arg_c->working == 0,
      "c->working was set to %d and should be 0", 
      arg_c->working);
    UtAssert_True(arg_c->result == 0,
      "c->result was set to %d and should be 0", 
      arg_c->result);
    UtAssert_True(arg_c->index == 0,
      "c->index was set to %d and should be 0", 
      arg_c->index);
    
} /* end test_CF_CRC_Start_ReinitializesGiven_c_ToAllZeroValues*/


/*******************************************************************************
**
**  CF_CRC_Digest tests
**
*******************************************************************************/

// TODO: not testing len > data's size, by contract this should not occur, 
// should it be an assumption in the doxygen comments?

void Test_CF_CRC_Digest_DoesNotAlter_c_working_or_c_result_or_c_index_When_len_Eq0(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8     arg_data[1] = {UT_UINT_8_DEFAULT};
    int32           arg_len = 0;
    uint32          initial_c_working = Any_uint32();
    uint32          initial_c_result = Any_uint32();
    uint8           initial_c_index = Any_cf_crc_index();
    
    arg_c->working = initial_c_working;
    arg_c->result = initial_c_result;
    arg_c->index = initial_c_index;
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->working == initial_c_working, 
      "c->working is %d which is unchanged from %d", 
      arg_c->working, initial_c_working);
    UtAssert_True(arg_c->result == initial_c_result, 
      "c->result is %d which is unchanged from %d", 
      arg_c->result, initial_c_result);
    UtAssert_True(arg_c->index == initial_c_index, 
      "c->index is %d which is unchanged from %d", 
      arg_c->index, initial_c_index);
   
} /* end Test_CF_CRC_Digest_DoesNotAlter_c_working_or_c_result_or_c_index_When_len_Eq0 */

void Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_When_len_Eq1(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8     arg_data[1] = {UT_UINT_8_DEFAULT};
    int32           arg_len = 1;
    int             i = 0;
    uint32          expected_c_working;

    arg_c->working = Any_uint32();
    arg_c->index = Any_cf_crc_index();
    
    expected_c_working = arg_c->working;
    expected_c_working  <<= 8;
    expected_c_working += arg_data[i];
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->working == expected_c_working, 
      "c->working is %u and should be %u", 
      arg_c->working, expected_c_working);
   
} /* end Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_When_len_Eq1 */

void Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_NumberOfTimesEqTo_len(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8*    arg_data;
    int32           arg_len;
    int             i = 0;
    uint32          expected_c_working;

    arg_c->working = Any_uint32();
    arg_c->index = Any_cf_crc_index();
    arg_len = Any_uint16_GreaterThan(1) / 2;  // TODO: change type if len changes,  Any_uint16_GreaterThan(1) / 2 roughly translates to a positive int that runs the test within a reasonable timeframe for size of len, this could change when more is learned about what len is supposed to be
    
    arg_data = malloc((size_t)arg_len);
    AnyBufferOf_uint8_WithSize((uint8*)arg_data, arg_len);
    
    expected_c_working = arg_c->working;
    for(i = 0; i < arg_len; ++i)
    {
        expected_c_working  <<= 8;
        expected_c_working  |= arg_data[i];
    }

    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->working == expected_c_working, 
      "c->working is %u and should be %u", 
      arg_c->working, expected_c_working);
      
    /* local Teardown */
    free((uint8*)arg_data);
   
} /* end Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_NumberOfTimesEqTo_len */

void Test_CF_CRC_Digest_DoesNotUpdate_c_result_WhenIndexIsNot3(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8     arg_data[1] = {UT_UINT_8_DEFAULT};
    int32           arg_len = 1;
    uint32          initial_c_result =  Any_uint32();

    arg_c->working = Any_uint32();
    arg_c->result = initial_c_result;
    arg_c->index = Any_cf_crc_index_except_3();
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->result == initial_c_result, 
      "c->result was not altered and is %u and should be %u", 
      arg_c->result, initial_c_result);
   
} /* end Test_CF_CRC_Digest_Updates_c_result_ */

void Test_CF_CRC_Digest_Updates_c_result_When_c_index_Is3(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8     arg_data[1] = {UT_UINT_8_DEFAULT};
    int32           arg_len;
    int             i = 0;
    uint32          expected_c_result;

    arg_c->working = Any_uint32();
    arg_c->index = 3;
    arg_c->result = Any_uint32();
    arg_len = 1;
    
    expected_c_result = arg_c->working;
    expected_c_result  <<= 8;
    expected_c_result += arg_data[i];
    expected_c_result += arg_c->result;
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->result == expected_c_result, 
      "c->result is %u and should be %u", 
      arg_c->result, expected_c_result);
   
} /* end Test_CF_CRC_Digest_Updates_c_result_When_c_index_Is3 */

void Test_CF_CRC_Digest_Updates_c_result_TheNumberOfTimesIndexReaches4(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8*    arg_data;
    int32           arg_len;
    uint32          dummy_c_working;
    int             i = 0;
    unsigned int    num_times_index_at_4;
    uint32          expected_c_result;

    arg_c->working = Any_uint32();
    arg_c->index = Any_cf_crc_index();
    arg_c->result = Any_uint32();
    arg_len = (int32) (Any_uint16() / 2); // TODO: change type if len changes,  Any_uint16_Except(0) / 2 roughly translates to a positive int that runs the test within a reasonable timeframe for size of len, this could change when more is learned about what len is supposed to be
    
    arg_data = malloc((size_t)arg_len);
    AnyBufferOf_uint8_WithSize((uint8*)arg_data, arg_len);
    
    dummy_c_working = arg_c->working;
    expected_c_result = arg_c->result;
    
    for (i = 0; i < (3 - arg_c->index); ++i) /* num shifts before first add */
    {
        dummy_c_working  <<= 8;
        dummy_c_working += arg_data[i];
    } /* end for */
    
    num_times_index_at_4 = (arg_len + arg_c->index) / 4;
    
    for (i = 0; i < num_times_index_at_4; ++i)
    {
        dummy_c_working  <<= 8;
        dummy_c_working += arg_data[(3 - arg_c->index) + (i * 4)];
        expected_c_result += dummy_c_working;
        
        if(i + 1 != num_times_index_at_4)
        {   
            int j;
            
            for(j = 1; j < 4; ++j)
            {
                dummy_c_working  <<= 8;
                dummy_c_working += arg_data[(3 - arg_c->index) + (i * 4) + j];
            } /* end for */
               
        } /* end if */
        
    } /* end for */
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->result == expected_c_result, 
      "c->result is %u and should be %u", 
      arg_c->result, expected_c_result);
      
    /* local Teardown */
    free((uint8*)arg_data);
   
} /* end Test_CF_CRC_Digest_Updates_c_result_TheNumberOfTimesIndexReaches4 */

void Test_CF_CRC_Digest_Updates_c_index_By1When_len_Eq1_And_c_index_LessThan3(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8     arg_data[1] = {UT_UINT_8_DEFAULT};
    int32           arg_len;
    uint8           expected_c_index;

    arg_c->working = Any_uint32();
    arg_c->index = Any_cf_crc_index_except_3();
    arg_c->result = Any_uint32();
    arg_len = 1;
    
    expected_c_index = arg_c->index + 1;
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->index == expected_c_index, 
      "c->index is %u and should be %u", 
      arg_c->index, expected_c_index);
   
} /* end Test_CF_CRC_Digest_Updates_c_index_By1When_len_Eq1_And_c_index_LessThan3 */

void Test_CF_CRC_Digest_Updates_c_index_To0When_len_Eq1_And_c_index_Is3(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8     arg_data[1] = {UT_UINT_8_DEFAULT};
    int32           arg_len;

    arg_c->working = Any_uint32();
    arg_c->index = 3;
    arg_c->result = Any_uint32();
    arg_len = 1;
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->index == 0, 
      "c->index is %u and should be 0", 
      arg_c->index);
   
} /* end Test_CF_CRC_Digest_Updates_c_index_To0When_len_Eq1_And_c_index_Is3 */

void Test_CF_CRC_Digest_Updates_c_index_CorrectlyDependingOn_c_index_And_len_Values(void)
{        
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    const uint8*    arg_data;
    int32           arg_len;
    uint8           expected_c_index;

    arg_c->working = Any_uint32();
    arg_c->index = Any_cf_crc_index();
    arg_c->result = Any_uint32();
    arg_len = (int32) (Any_uint16() / 2); // TODO: change type if len changes,  Any_uint16_Except(0) / 2 roughly translates to a positive int that runs the test within a reasonable timeframe for size of len, this could change when more is learned about what len is supposed to be
    
    arg_data = malloc((size_t)arg_len);
    AnyBufferOf_uint8_WithSize((uint8*)arg_data, arg_len);
    
    expected_c_index = (arg_len + arg_c->index) % 4;
    
    /* Act */
    CF_CRC_Digest(arg_c, arg_data, arg_len);
    
    /* Assert */
    UtAssert_True(arg_c->index == expected_c_index, 
      "c->index is %u and should be %u", 
      arg_c->index, expected_c_index);

    /* local Teardown */
    free((uint8*)arg_data);
   
} /* end Test_CF_CRC_Digest_Updates_c_index_To0When_len_Eq1_And_c_index_Is3 */

/* end CF_CRC_Digest tests */

/*******************************************************************************
**
**  CF_CRC_Finalize tests
**
*******************************************************************************/

void Test_CF_CRC_Finalize_DoesNothingWhenIndexIs0(void)
{
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    uint32          initial_c_working = Any_uint32();
    uint32          initial_c_result = Any_uint32();
    
    arg_c->working = initial_c_working;
    arg_c->result = initial_c_result;
    arg_c->index = 0;
    
    /* Act */ 
    CF_CRC_Finalize(arg_c);
    
    /* Assert */
    UtAssert_True(arg_c->working == initial_c_working,
      "c->working is %d which is unchanged from %d",
      arg_c->working, initial_c_working);
    UtAssert_True(arg_c->result == initial_c_result,
      "c->result is %d which is unchanged from %d",
      arg_c->result, initial_c_result);
    UtAssert_True(arg_c->index == 0,
      "c->index is %d which is unchanged from 0",
      arg_c->index);
} /* end Test_CF_CRC_Finalize_DoesNothingWhenIndexIs0 */

void Test_CF_CRC_Finalize_Getsexpected_resultAtIndex1(void)
{
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    uint32          working_shift = 256 * 256 * 256; /* 3 left shifts */
    uint32          expected_result;
        
    arg_c->index = 1;
    arg_c->working = Any_uint32();
    arg_c->result = Any_uint32();
    
    expected_result = arg_c->result + (arg_c->working * working_shift);
    
    /* Act */
    CF_CRC_Finalize(arg_c);
    
    /* Assert */
    UtAssert_True(arg_c->working == 0,
      "c->working is %u and it should be 0",
      arg_c->working);
    UtAssert_True(arg_c->result == expected_result,
      "c->result %u and it should be %u",
      arg_c->result, expected_result);
    UtAssert_True(arg_c->index == 0,
      "c->index is %u and it should be 0",
      arg_c->index);
} /* end Test_CF_CRC_Finalize_Getsexpected_resultAtIndex1 */

void Test_CF_CRC_Finalize_Getsexpected_resultAtIndex2(void)
{
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    uint8           dummy_index = 2;
    uint32          working_shift = 256 * 256; /* 2 left shifts */
    uint32          expected_result;
    
    arg_c->index = dummy_index;
    arg_c->working = Any_uint32();
    arg_c->result = Any_uint32();
    
    expected_result = arg_c->result + (arg_c->working * working_shift);
    
    /* Act */
    CF_CRC_Finalize(arg_c);
    
    /* Assert */
    UtAssert_True(arg_c->working == 0,
      "c->working is %u and it should be 0",
      arg_c->working);
    UtAssert_True(arg_c->result == expected_result,
      "c->result %u and it should be %u",
      arg_c->result, expected_result);
    UtAssert_True(arg_c->index == 0,
      "c->index is %u and it should be 0",
      arg_c->index);
} /* end Test_CF_CRC_Finalize_Getsexpected_resultAtIndex2 */

void Test_CF_CRC_Finalize_Getsexpected_resultAtIndex3(void)
{
    /* Arrange */
    cf_crc_t        dummy_c;
    cf_crc_t*       arg_c = &dummy_c;
    uint8           dummy_index = 3;
    uint32          working_shift = 256; /* 1 left shift */
    uint32          expected_result;
    
    arg_c->index = dummy_index;
    arg_c->working = Any_uint32();
    arg_c->result = Any_uint32();
    
    expected_result = arg_c->result + (arg_c->working * working_shift);
    
    /* Act */
    CF_CRC_Finalize(arg_c);
    
    /* Assert */
    UtAssert_True(arg_c->working == 0,
      "c->working is %u and it should be 0",
      arg_c->working);
    UtAssert_True(arg_c->result == expected_result,
      "c->result %u and it should be %u",
      arg_c->result, expected_result);
    UtAssert_True(arg_c->index == 0,
      "c->index is %u and it should be 0",
      arg_c->index);
} /* end Test_CF_CRC_Finalize_Getsexpected_resultAtIndex3 */

/* end CF_CRC_Finalize tests */



/*******************************************************************************
**
**  cf_crc_tests ADD TESTS functions
**
*******************************************************************************/

void add_CF_CRC_Start_tests(void)
{
    UtTest_Add(test_CF_CRC_Start_ReinitializesGiven_c_ToAllZeroValues, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "test_CF_CRC_Start_ReinitializesGiven_c_ToAllZeroValues");
}

void add_CF_CRC_Digest_tests(void)
{
    UtTest_Add(Test_CF_CRC_Digest_DoesNotAlter_c_working_or_c_result_or_c_index_When_len_Eq0, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_DoesNotAlter_c_working_or_c_result_or_c_index_When_len_Eq0");
    UtTest_Add(Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_When_len_Eq1, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_When_len_Eq1");
    UtTest_Add(Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_NumberOfTimesEqTo_len, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_PushesDataLeftOnto_c_working_NumberOfTimesEqTo_len");
    UtTest_Add(Test_CF_CRC_Digest_DoesNotUpdate_c_result_WhenIndexIsNot3, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_DoesNotUpdate_c_result_WhenIndexIsNot3");
    UtTest_Add(Test_CF_CRC_Digest_Updates_c_result_When_c_index_Is3, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_Updates_c_result_When_c_index_Is3");
    UtTest_Add(Test_CF_CRC_Digest_Updates_c_result_TheNumberOfTimesIndexReaches4, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_Updates_c_result_TheNumberOfTimesIndexReaches4");
    UtTest_Add(Test_CF_CRC_Digest_Updates_c_index_By1When_len_Eq1_And_c_index_LessThan3, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_Updates_c_index_By1When_len_Eq1_And_c_index_LessThan3");
    UtTest_Add(Test_CF_CRC_Digest_Updates_c_index_To0When_len_Eq1_And_c_index_Is3, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_Updates_c_index_To0When_len_Eq1_And_c_index_Is3");
    UtTest_Add(Test_CF_CRC_Digest_Updates_c_index_CorrectlyDependingOn_c_index_And_len_Values, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Digest_Updates_c_index_CorrectlyDependingOn_c_index_And_len_Values");
}

void add_CF_CRC_Finalize_tests(void)
{
    UtTest_Add(Test_CF_CRC_Finalize_DoesNothingWhenIndexIs0, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Finalize_DoesNothingWhenIndexIs0");
    UtTest_Add(Test_CF_CRC_Finalize_Getsexpected_resultAtIndex1, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Finalize_Getsexpected_resultAtIndex1");
    UtTest_Add(Test_CF_CRC_Finalize_Getsexpected_resultAtIndex2, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Finalize_Getsexpected_resultAtIndex2");
    UtTest_Add(Test_CF_CRC_Finalize_Getsexpected_resultAtIndex3, 
      cf_crc_tests_Setup, cf_crc_tests_Teardown, 
      "Test_CF_CRC_Finalize_Getsexpected_resultAtIndex3");
}


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