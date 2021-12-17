/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_codec.h"

/*******************************************************************************
**
**  cf_codec_tests Setup and Teardown
**
*******************************************************************************/

void cf_codec_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_codec_tests_Setup */

void cf_codec_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_codec_tests_Teardown */

/* end cf_codec_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_Codec tests
**
*******************************************************************************/
void Test_cf_codec_1(void)
{
}

void Test_cf_codec_2(void)
{
}

/*******************************************************************************
**
**  cf_codec_tests UtTest_Add groups
**
*******************************************************************************/

void Add_CF_Encode_tests(void)
{
    UtTest_Add(Test_cf_codec_1, cf_codec_tests_Setup, cf_codec_tests_Teardown,
               "Test_cf_codec_1");
} /* end add_CF_Codec_Start_tests */

void Add_CF_Decode_tests(void)
{
    UtTest_Add(Test_cf_codec_2, cf_codec_tests_Setup,
               cf_codec_tests_Teardown, "Test_cf_codec_2");
} /* end add_CF_Codec_Digest_tests */

/*******************************************************************************
**
**  cf_codec_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    Add_CF_Encode_tests();
    Add_CF_Decode_tests();

} /* end UtTest_Setup for cf_codec_tests.c */

/* end cf_codec_tests.c */