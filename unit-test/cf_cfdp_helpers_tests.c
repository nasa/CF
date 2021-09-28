/* cf testing includes */
#include "cf_test_utils.h"
/* test header to add standard lib stub overrides */
#include "cf_cfdp_helpers_tests.h"

/* stub redefines for unit testing */
#undef FGV
#define FGV  Stub_FGV
int32 Stub_FGV(uint8 source, CF_FIELD_FIELD name);
#undef FSV
#define FSV  Stub_FSV
int32 Stub_FSV(uint8 source, CF_FIELD_FIELD name, int value);

#undef cfdp_get_uint8
#define cfdp_get_uint8(dst, src) (dst) = (src)

/* cf source file include */
#include "cf_cfdp_helpers.c"

/*******************************************************************************
**
**  cf macro overrides
**
*******************************************************************************/

/* int32 selected only for ease of Stub use */
int32 Stub_FGV(uint8 source, CF_FIELD_FIELD name)
{
    UT_GenStub_SetupReturnBuffer(Stub_FGV, int32);

    UT_GenStub_AddParam(Stub_FGV, uint8, source);
    UT_GenStub_AddParam(Stub_FGV, CF_FIELD_FIELD, name);

    UT_GenStub_Execute(Stub_FGV, Basic, NULL);

    return UT_GenStub_GetReturnValue(Stub_FGV, int32);
}

int32 Stub_FSV(uint8 source, CF_FIELD_FIELD name, int value)
{
    UT_GenStub_SetupReturnBuffer(Stub_FSV, int32);

    UT_GenStub_AddParam(Stub_FSV, uint8, source);
    UT_GenStub_AddParam(Stub_FSV, CF_FIELD_FIELD, name);
    UT_GenStub_AddParam(Stub_FSV, int, value);

    UT_GenStub_Execute(Stub_FSV, Basic, NULL);

    return UT_GenStub_GetReturnValue(Stub_FSV, int32);
}

/*******************************************************************************
**
**  cf standard lib overrides (CSLO) - defined in cf_cfdp_helpers_tests.h
**
*******************************************************************************/

typedef struct
{
  void *__s;
  int __c;
  int __n;
} CF_PACK context_memset_t;

context_memset_t memset_context;


void *CSLO_memset(void *__s, int __c, size_t __n)
{
  UT_Stub_CopyFromLocal(UT_KEY(CSLO_memset), &__s, sizeof(__s));
  UT_Stub_CopyFromLocal(UT_KEY(CSLO_memset), &__c, sizeof(__c));
  UT_Stub_CopyFromLocal(UT_KEY(CSLO_memset), &__n, sizeof(__n));

  UT_DEFAULT_IMPL(CSLO_memset);
}

/* end cf standard lib overrides */

/*******************************************************************************
**
**  cf_cfdp_helpers_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_helpers_tests_Setup(void)
{
    cf_tests_Setup();

    memset_context.__s = ut_default_ptr;
    memset_context.__c = UT_INT_32_DEFAULT;
    memset_context.__n = UT_INT_32_DEFAULT;

} /* end cf_cfdp_helpers_tests_Setup */

void cf_cfdp_helpers_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_helpers_tests_Teardown */

/* end cf_cfdp_helpers_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_GetMemcpySize ENDIAN==_EL tests - full coverage
**
*******************************************************************************/
#if ENDIAN==_EL
void Test_CF_GetMemcpySize_WhenGivenSizeIs_0_Given_num_ArrayValuesDoNotMatterAlwaysReturn_0(void)
{
    /* Arrange */
    uint8_t   arg_num[200]; /* 200 for arg_num just an arbitrary value for definition in this test */
    int       arg_size = 0; /* 0 for arg_size because of behavior being tested */
    int       local_result;
    int       i = 0;

    for(i = 0; i < 200; ++i)
    {
      if (Any_bool())  /* 50/50 chance to be 0 or random number to show 0s in Array do not affect result */
      {
        arg_num[i] = 0;
      }
      else
      {
        arg_num[i] = Any_uint8();
      }
    }

    /* Act */
    local_result = CF_GetMemcpySize(arg_num, arg_size);

    /* Assert */
    UtAssert_True(local_result == 0,
      "CF_GetMemcpySize returned %d and should be 0",
      local_result);
} /* end Test_CF_GetMemcpySize_WhenGivenSizeIs_0_Given_num_ArrayValuesDoNotMatterAlwaysReturn_0 */

void Test_CF_GetMemcpySize_WhenGivenSizeIs_1_AndGiven_num_Uint8Is_0_Return_1(void)
{
    /* Arrange */
    uint8_t   arg_num[1]; /* 1 for arg_num to match arg_size */
    int       arg_size = 1; /* 1 for arg_size because of behavior being tested */
    int       local_result;

    arg_num[0] = 0;

    /* Act */
    local_result = CF_GetMemcpySize(arg_num, arg_size);

    /* Assert */
    UtAssert_True(local_result == 1,
      "CF_GetMemcpySize returned %d and should be 1",
      local_result);
} /* end Test_CF_GetMemcpySize_WhenGivenSizeIs_1_AndGiven_num_Uint8Is_0_Return_1 */

void Test_CF_GetMemcpySize_WhenGivenSizeIsAny_int_AndNumsAreAll_0_Return_1(void)
{
    /* Arrange */
    uint8_t   arg_num[200] = {0}; /* 200 for arg_num just an arbitrary value for speed of testing */
    int       arg_size = Any_int_ZeroOrPositiveLessThan(200) + 1; /* Any_int_ZeroOrPositiveLessThan(200) + 1 gives 1 to 200 to be under or eq arg_num defined size, but at least 1*/
    int       local_result;

    /* Act */
    local_result = CF_GetMemcpySize(arg_num, arg_size);

    /* Assert */
    UtAssert_True(local_result == 1,
      "CF_GetMemcpySize returned %d and should be 1",
      local_result);
} /* end Test_CF_GetMemcpySize_WhenGivenSizeIsAny_int_AndNumsAreAll_0_Return_1 */

void Test_CF_GetMemcpySize_WhenGiven_num_ElementAtGiven_size_Minus_1_IsNot_0_ReturnValueOf_size(void)
{
    /* Arrange */
    uint8_t   arg_num[200] = {0}; /* 200 for arg_num just an arbitrary value for speed of testing */
    int       arg_size = Any_int_ZeroOrPositiveLessThan(200) + 1; /* Any_int_ZeroOrPositiveLessThan(200) + 1 gives 1 to 200 to be under or eq arg_num defined size, but at least 1*/
    int       local_result;

    arg_num[arg_size - 1] = Any_uint8_Except(0);

    /* Act */
    local_result = CF_GetMemcpySize(arg_num, arg_size);

    /* Assert */
    UtAssert_True(local_result == arg_size,
      "CF_GetMemcpySize returned %d and should be %d (size)",
      local_result, arg_size);
} /* end Test_CF_GetMemcpySize_WhenGiven_num_ElementAtGiven_size_Minus_1_IsNot_0_ReturnValueOf_size */

void Test_CF_GetMemcpySize_WhenGiven_num_ElementsAre_0_FromGiven_size_UntilAnotherElementInArrayIsNonZeroReturnThatElementsIndexPlus_1(void)
{
    /* Arrange */
    uint8_t   arg_num[200] = {0}; /* 200 for arg_num just an arbitrary value for speed of testing */
    int       arg_size = Any_int_ZeroOrPositiveLessThan(199) + 2; /* Any_int_ZeroOrPositiveLessThan(199) + 2 gives 2 to 200 to be under or eq arg_num defined size, but at least 2 because of behavior being tested */
    int       local_result;
    int       dummy_num_element_index = Any_int_ZeroOrPositiveLessThan(arg_size - 1);
    int       i;

    arg_num[arg_size - 1] = 0;

    for(i = arg_size - 2; i >= 0; --i)
    {
        if (i == dummy_num_element_index)
        {
          break;
        }

        arg_num[i] = 0;
    }

    arg_num[i] = Any_uint8_Except(0);

    /* Act */
    local_result = CF_GetMemcpySize(arg_num, arg_size);

    /* Assert */
    UtAssert_True(local_result == dummy_num_element_index + 1,
      "CF_GetMemcpySize returned %d and should be %d (location of first non 0 element)",
      local_result, dummy_num_element_index + 1);
} /* end Test_CF_GetMemcpySize_WhenGiven_num_ElementsAre_0_FromGiven_size_UntilAnotherElementInArrayIsNonZeroReturnThatElementsIndexPlus_1 */
#endif

/* end CF_GetMemcpySize ENDIAN==_ELtests */

/*******************************************************************************
**
**  CF_MemcpyToBE ENDIAN==_EL tests - full coverage
**
*******************************************************************************/

void Test_CF_MemcpyToBE_Asserts_src_size_EqTo_0(void)
{
    /* Arrange */
    uint8 *     arg_dst;
    uint8 *     arg_src;
    int         arg_src_size = 0;
    int         arg_dst_size = Any_int_Positive();

    /* Act */
    //CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (src_size>0)&&(dst_size>0)");
} /* end Test_CF_MemcpyToBE_Asserts_src_size_EqTo_0 */

/* NOTE: Test_CF_MemcpyToBE_Asserts_src_size_LessThan_0 not required for coverage but desired for completeness */
// void Test_CF_MemcpyToBE_Asserts_src_size_LessThan_0(void)
// {
//     /* Arrange */
//     uint8 *     arg_dst;
//     uint8 *     arg_src;
//     int         arg_src_size = Any_int_Negative();
//     int         arg_dst_size = Any_int_Positive();

//     /* Act */
//     //CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (src_size>0)&&(dst_size>0)");
// } /* end Test_CF_MemcpyToBE_Asserts_src_size_LessThan_0 */

void Test_CF_MemcpyToBE_Asserts_dst_size_EqTo_0(void)
{
    /* Arrange */
    uint8 *     arg_dst;
    uint8 *     arg_src;
    int         arg_src_size = Any_int_Positive();
    int         arg_dst_size = 0;

    /* Act */
    //CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (src_size>0)&&(dst_size>0)");
} /* end Test_CF_MemcpyToBE_Asserts_dst_size_EqTo_0 */

/* NOTE:Test_CF_MemcpyToBE_Asserts_dst_size_LessThan_0 not require but desired */
// void Test_CF_MemcpyToBE_Asserts_dst_size_LessThan_0(void)
// {
//     /* Arrange */
//     uint8 *     arg_dst;
//     uint8 *     arg_src;
//     int         arg_src_size = Any_int_Positive();
//     int         arg_dst_size = Any_int_Negative();

//     /* Act */
//     //CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_MemcpyToBE_Asserts_dst_size_LessThan_0 */

void Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_Are_0(void)
{
    /* Arrange */
    uint8 *     arg_dst;
    uint8 *     arg_src;
    int         arg_src_size = 0;
    int         arg_dst_size = 0;

    /* Act */
    //CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - src_size>=dst_size");
} /* end Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_Are_0 */

/* NOTE:Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_AreNegative not required for coverage but desired for completeness */
// void Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_AreNegative(void)
// {
//     /* Arrange */
//     uint8 *     arg_dst;
//     uint8 *     arg_src;
//     int         arg_src_size = Any_int_Negative();
//     int         arg_dst_size = Any_int_Negative();

//     /* Act */
//     //CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_AreNegative */

/* NOTE:Test_CF_MemcpyToBE_Asserts_src_size_IsLessThan_dst_size not required for coverage but desired for completeness */
// void Test_CF_MemcpyToBE_Asserts_src_size_IsLessThan_dst_size(void)
// {
//     /* Arrange */
//     uint8 *     arg_dst;
//     uint8 *     arg_src;
//     int         arg_src_size = Any_int_ZeroOrPositiveLessThan(INT32_MAX - 2) + 1; /* -2 then +1 for 1 to INT32_MAX - 1 */
//     int         arg_dst_size = arg_src_size + 1; /* TODO change to any int greater than? DNE at time of writing */

//     /* Act */
//     //CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_MemcpyToBE_Asserts_src_size_IsLessThan_dst_size */

void Test_CF_MemcpyToBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value(void)
{
    /* Arrange */
    uint8       arg_dst[1] = {Any_uint8()};
    uint8       arg_src[1]  = {Any_uint8_Except(arg_dst[0])};
    int         arg_src_size = 1;
    int         arg_dst_size = arg_src_size;
    
    /* Act */
    CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_True(arg_dst[0] == arg_src[0],
      "One value copied from arg_src to arg_dst");
} /* end Test_CF_MemcpyToBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value */

void Test_CF_MemcpyToBE_When_src_size_GreaterThan_1_And_dst_size_Is_1_CopyOne_src_Value(void)
{
    /* Arrange */
    uint8       arg_dst[1] = {Any_uint8()}; 
    uint8       arg_src[256]; /* 256 is arbitrary for arg_src's size for speed of testing */
    int         arg_src_size = Any_uint8_GreaterThan(0) + 1; /* Any_uint8_GreaterThan(0) + 1 gives value from 2 to 256 */
    int         arg_dst_size = 1; 

    arg_src[0] = Any_uint8_Except(arg_dst[0]);

    /* Act */
    CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_True(arg_dst[0] == arg_src[0],
      "One value copied from arg_src to arg_dst");
} /* end Test_CF_MemcpyToBE_When_src_size_GreaterThan_1_And_dst_size_Is_1_CopyOne_src_Value */

void Test_CF_MemcpyToBE_When_dst_size_IsLessThan_src_size_CopyAllRequisite_src_ValuesInto_dst(void)
{
    /* Arrange */
    uint8       arg_dst[12]; /* 12 is arbitrary for arg_dst's size for speed of testing */
    uint8       arg_src[12]; /* 12 is arbitrary for arg_src's size for speed of testing */
    int         arg_src_size = Any_uint8_LessThan(10) + 2; /* Any_uint8_LessThan(10) + 2 gives value from 2 to 11 for speed */
    int         arg_dst_size = Any_uint8_LessThan(arg_src_size - 1) + 1;  /* Any_uint8_LessThan(arg_src_size - 1) + 1 give min of 1, max of 1 less than arg_src_size */ 
    uint8       i;
    
    /* load all destination */
    for(i = 0; i < 12; ++i)
    {
      arg_dst[i] = Any_uint8();
    }

    /* load only used values in source */
    for(i = 0; i < arg_src_size; ++i)
    {
      arg_src[i] = Any_uint8_Except(arg_dst[arg_src_size - i - 1]);
    }
  
    /* Act */
    CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    for (i = 0; i < arg_dst_size; ++i)
    {
      UtAssert_True(arg_dst[arg_dst_size - i - 1] == arg_src[i],
        "Correct value copied from arg_src[%u] (%u) to arg_dst[%u] (%u)",
        i, arg_src[i], arg_dst_size - i - 1, arg_dst[arg_dst_size - i - 1]);
    }
} /* end Test_CF_MemcpyToBE_When_dst_size_IsLessThan_src_size_CopyAllRequisite_src_ValuesInto_dst */

void Test_CF_MemcpyToBE_CopyAllRequisite_src_ValuesInto_dst_When_dst_size_IsEqTo_src_size(void)
{
    /* Arrange */
    uint8       arg_dst[12]; /* 12 is arbitrary for arg_dst's size for speed of testing */
    uint8       arg_src[12]; /* 12 is arbitrary for arg_src's size for speed of testing */
    int         arg_src_size = Any_uint8_LessThan(10) + 2; /* Any_uint8_LessThan(10) + 2 gives value from 2 to 11 for speed */
    int         arg_dst_size = arg_src_size;
    uint8       i;
    
    /* load all destination */
    for(i = 0; i < 12; ++i)
    {
      arg_dst[i] = Any_uint8();
    }

    /* load only used values in source */
    for(i = 0; i < arg_src_size; ++i)
    {
      arg_src[i] = Any_uint8_Except(arg_dst[arg_src_size - i - 1]);
    }
  
    /* Act */
    CF_MemcpyToBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    for (i = 0; i < arg_dst_size; ++i)
    {
      UtAssert_True(arg_dst[arg_dst_size - i - 1] == arg_src[i],
        "Correct value copied from arg_src[%u] (%u) to arg_dst[%u] (%u)",
        i, arg_src[i], arg_dst_size - i - 1, arg_dst[arg_dst_size - i - 1]);
    }
} /* end Test_CF_MemcpyToBE_CopyAllRequisite_src_ValuesInto_dst_When_dst_size_IsEqTo_src_size */

/* end CF_MemcpyToBE ENDIAN==_EL tests */


/*******************************************************************************
**
**  CF_MemcpyToBE ENDIAN==_EL tests - full coverage
**
*******************************************************************************/

void Test_CF_MemcpyFromBE_Asserts_src_size_EqTo_0(void)
{
    /* Arrange */
    uint8 *     arg_dst;
    uint8 *     arg_src;
    int         arg_src_size = 0;
    int         arg_dst_size = Any_int_Positive();

    /* Act */
    //CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (src_size>0)&&(dst_size>0)");
} /* end Test_CF_MemcpyFromBE_Asserts_src_size_EqTo_0 */

// NOTE: Test_CF_MemcpyFromBE_Asserts_src_size_LessThan_0 not required for coverage but desired for completeness */
// void Test_CF_MemcpyFromBE_Asserts_src_size_LessThan_0(void)
// {
//     /* Arrange */
//     uint8 *     arg_dst;
//     uint8 *     arg_src;
//     int         arg_src_size = Any_int_Negative();
//     int         arg_dst_size = Any_int_Positive();

//     /* Act */
//     //CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_MemcpyFromBE_Asserts_src_size_LessThan_0 */

void Test_CF_MemcpyFromBE_Asserts_dst_size_EqTo_0(void)
{
    /* Arrange */
    uint8 *     arg_dst;
    uint8 *     arg_src;
    int         arg_src_size = Any_int_Positive();
    int         arg_dst_size = 0;

    /* Act */
    //CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (src_size>0)&&(dst_size>0)");
} /* end Test_CF_MemcpyFromBE_Asserts_dst_size_EqTo_0 */

/* NOTE:Test_CF_MemcpyFromBE_Asserts_dst_size_LessThan_0 not required for coverage but desired for completeness */
// void Test_CF_MemcpyFromBE_Asserts_dst_size_LessThan_0(void)
// {
//     /* Arrange */
//     uint8 *     arg_dst;
//     uint8 *     arg_src;
//     int         arg_src_size = Any_int_Positive();
//     int         arg_dst_size = Any_int_Negative();

//     /* Act */
//     //CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_MemcpyFromBE_Asserts_dst_size_LessThan_0 */

void Test_CF_MemcpyToBE_Asserts_dst_size_IsLessThan_src_size(void)
{
    /* Arrange */
    uint8 *     arg_dst;
    uint8 *     arg_src;
    int         arg_dst_size = Any_int_ZeroOrPositiveLessThan(INT32_MAX - 2) + 1; /* -2 then +1 for 1 to INT32_MAX - 1 */
    int         arg_src_size = arg_dst_size + 1; /* TODO change to any int greater than? DNE at time of writing */

    /* Act */
    //CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - dst_size>=src_size");
} /* end Test_CF_MemcpyToBE_Asserts_dst_size_IsLessThan_src_size */

void Test_CF_MemcpyFromBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value(void)
{
    /* Arrange */
    uint8       arg_dst[1] = {Any_uint8()};
    uint8       arg_src[1]  = {Any_uint8_Except(arg_dst[0])};
    int         arg_src_size = 1;
    int         arg_dst_size = 1;

    UT_SetDataBuffer(UT_KEY(CSLO_memset), &memset_context, sizeof(memset_context), false);
    
    /* Act */
    CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_STUB_COUNT(CSLO_memset, 1);
    UtAssert_ADDRESS_EQ(memset_context.__s, arg_dst);
    UtAssert_True(memset_context.__c == 0,
      "memset __c is %d and should be 0",
      memset_context.__c);
    UtAssert_True(memset_context.__n == arg_dst_size,
      "memset __n is %d and should be %d (dst_size)",
      memset_context.__n, arg_dst_size);
    UtAssert_True(arg_dst[0] == arg_src[0],
      "One value copied from arg_src to arg_dst");
} /* end Test_CF_MemcpyFromBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value */

void Test_CF_MemcpyFromBE_When_src_size_IsLessThan_dst_size_CopyAllRequisite_src_ValuesInto_dst(void)
{
    /* Arrange */
    uint8       arg_dst[12]; /* 12 is arbitrary for arg_dst's size for speed of testing */
    uint8       arg_src[12]; /* 12 is arbitrary for arg_src's size for speed of testing */
    int         arg_dst_size = Any_uint8_LessThan(10) + 2; /* Any_uint8_LessThan(10) + 2 gives value from 2 to 11 for speed */
    int         arg_src_size = Any_uint8_LessThan(arg_dst_size - 1) + 1;  /* Any_uint8_LessThan(arg_src_size - 1) + 1 give min of 1, max of 1 less than arg_src_size */ 
    uint8       i;

    UT_SetDataBuffer(UT_KEY(CSLO_memset), &memset_context, sizeof(memset_context), false);
    
    /* load all destination */
    for(i = 0; i < 12; ++i)
    {
      arg_dst[i] = Any_uint8();
    }

    /* load only used values in source */
    for(i = 0; i < arg_src_size; ++i)
    {
      arg_src[i] = Any_uint8_Except(arg_dst[arg_src_size - i - 1]);
    }
  
    /* Act */
    CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_STUB_COUNT(CSLO_memset, 1);
    UtAssert_ADDRESS_EQ(memset_context.__s, arg_dst);
    UtAssert_True(memset_context.__c == 0,
      "memset __c is %d and should be 0",
      memset_context.__c);
    UtAssert_True(memset_context.__n == arg_dst_size,
      "memset __n is %d and should be %d (dst_size)",
      memset_context.__n, arg_dst_size);
    for (i = 0; i < arg_src_size; ++i)
    {
      UtAssert_True(arg_dst[arg_src_size - i - 1] == arg_src[i],
        "Correct value copied from arg_src[%u] (%u) to arg_dst[%u] (%u)",
        i, arg_src[i], arg_src_size - i - 1, arg_dst[arg_src_size - i - 1]);
    }
} /* end Test_CF_MemcpyFromBE_When_src_size_IsLessThan_dst_size_CopyAllRequisite_src_ValuesInto_dst */

void Test_CF_MemcpyFromBE_When_src_size_IsEqTo_dst_size_CopyAllRequisite_src_ValuesInto_dst(void)
{
    /* Arrange */
    uint8       arg_dst[12]; /* 12 is arbitrary for arg_dst's size for speed of testing */
    uint8       arg_src[12]; /* 12 is arbitrary for arg_src's size for speed of testing */
    int         arg_dst_size = Any_uint8_LessThan(10) + 2; /* Any_uint8_LessThan(10) + 2 gives value from 2 to 11 for speed */
    int         arg_src_size = arg_dst_size;
    uint8       i;

    UT_SetDataBuffer(UT_KEY(CSLO_memset), &memset_context, sizeof(memset_context), false);
    
    /* load all destination */
    for(i = 0; i < 12; ++i)
    {
      arg_dst[i] = Any_uint8();
    }

    /* load only used values in source */
    for(i = 0; i < arg_src_size; ++i)
    {
      arg_src[i] = Any_uint8_Except(arg_dst[arg_src_size - i - 1]);
    }
  
    /* Act */
    CF_MemcpyFromBE(arg_dst, arg_src, arg_src_size, arg_dst_size);

    /* Assert */
    UtAssert_STUB_COUNT(CSLO_memset, 1);
    UtAssert_ADDRESS_EQ(memset_context.__s, arg_dst);
    UtAssert_True(memset_context.__c == 0,
      "memset __c is %d and should be 0",
      memset_context.__c);
    UtAssert_True(memset_context.__n == arg_dst_size,
      "memset __n is %d and should be %d (dst_size)",
      memset_context.__n, arg_dst_size);
    for (i = 0; i < arg_src_size; ++i)
    {
      UtAssert_True(arg_dst[arg_src_size - i - 1] == arg_src[i],
        "Correct value copied from arg_src[%u] (%u) to arg_dst[%u] (%u)",
        i, arg_src[i], arg_src_size - i - 1, arg_dst[arg_src_size - i - 1]);
    }
} /* end Test_CF_MemcpyFromBE_When_src_size_IsEqTo_dst_size_CopyAllRequisite_src_ValuesInto_dst */
  
/* end CF_MemcpyFromBE ENDIAN==_EL tests */

/*******************************************************************************
**
**  CF_GetTSNSize tests - full coverage
**
*******************************************************************************/

void Test_CF_GetTSNSize_When_ret_IsGreaterThan_size_of_cf_transaction_seq_t_ReturnThatValue(void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    pdu_header_t*   arg_ph = &dummy_ph;
    uint8           forced_return_FGV = sizeof(cf_transaction_seq_t); /* CUT adds +1 to this value making it greater than */
    int             local_result;
    
    UT_SetDefaultReturnValue(UT_KEY(FGV), forced_return_FGV); 

    /* Act */
    local_result = CF_GetTSNSize(arg_ph);

    /* Assert */
    UtAssert_True(local_result == -1,
      "CF_GetTSNSize returned %d and should be -1",
      local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_GetTSNSize_When_ret_IsGreaterThan_size_of_cf_transaction_seq_t_ReturnThatValue */

void Test_CF_GetTSNSize_When_ret_Is_1_LessThan_size_of_cf_transaction_seq_t_Add_1_MakingItEqToThatSizeReturnThatValue(void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    pdu_header_t*   arg_ph = &dummy_ph;
    uint8           forced_return_FGV = sizeof(cf_transaction_seq_t) - 1; /* CUT adds +1 to this value making it equal to */
    int             expected_result = forced_return_FGV + 1;
    int             local_result;
    
    UT_SetDefaultReturnValue(UT_KEY(FGV), forced_return_FGV); 

    /* Act */
    local_result = CF_GetTSNSize(arg_ph);

    /* Assert */
    UtAssert_True(local_result == expected_result,
      "CF_GetTSNSize returned %d and should be %d (sizeof(cf_transaction_seq_t))",
      local_result, sizeof(cf_transaction_seq_t));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_GetTSNSize_When_ret_Is_1_LessThan_size_of_cf_transaction_seq_t_Add_1_MakingItEqToThatSizeReturnThatValue */

void Test_CF_GetTSNSize_When_ret_Is_LessThan_size_of_cf_transaction_seq_t_Add_1_AndReturnThatValue(void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    pdu_header_t*   arg_ph = &dummy_ph;
    uint8           forced_return_FGV = Any_uint8_LessThan(sizeof(cf_transaction_seq_t)); /* CUT adds +1 to this value */
    int             expected_result = forced_return_FGV + 1;
    int             local_result;
    
    UT_SetDefaultReturnValue(UT_KEY(FGV), forced_return_FGV); 

    /* Act */
    local_result = CF_GetTSNSize(arg_ph);

    /* Assert */
    UtAssert_True(local_result == forced_return_FGV + 1,
      "CF_GetTSNSize returned %d and should be 1 more than %d (FGV)",
      local_result, forced_return_FGV);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_GetTSNSize_When_ret_Is_LessThan_size_of_cf_transaction_seq_t_Add_1_AndReturnThatValue */

/* end CF_GetTSNSize tests */

/*******************************************************************************
**
**  CF_GetEIDSize tests - full coverage
**
*******************************************************************************/

void Test_CF_GetEIDSize_When_ret_IsGreaterThan_size_of_cf_entity_id_t_ReturnThatValue(void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    pdu_header_t*   arg_ph = &dummy_ph;
    uint8           forced_return_FGV = sizeof(cf_entity_id_t); /* CUT adds +1 to this value making it greater than */
    int             local_result;
    
    UT_SetDefaultReturnValue(UT_KEY(FGV), forced_return_FGV); 

    /* Act */
    local_result = CF_GetEIDSize(arg_ph);

    /* Assert */
    UtAssert_True(local_result == -1,
      "CF_GetEIDSize returned %d and should be -1",
      local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_GetEIDSize_When_ret_IsGreaterThan_size_of_cf_entity_id_t_ReturnThatValue */

void Test_CF_GetEIDSize_When_ret_Is_1_LessThan_size_of_cf_entity_id_t_Add_1_MakingItEqToThatSizeReturnThatValue(void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    pdu_header_t*   arg_ph = &dummy_ph;
    uint8           forced_return_FGV = sizeof(cf_entity_id_t) - 1; /* CUT adds +1 to this value making it equal to */
    int             expected_result = forced_return_FGV + 1;
    int             local_result;
    
    UT_SetDefaultReturnValue(UT_KEY(FGV), forced_return_FGV); 

    /* Act */
    local_result = CF_GetEIDSize(arg_ph);

    /* Assert */
    UtAssert_True(local_result == expected_result,
      "CF_GetEIDSize returned %d and should be %d (sizeof(cf_entity_id_t))",
      local_result, sizeof(cf_entity_id_t));
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_GetEIDSize_When_ret_Is_1_LessThan_size_of_cf_entity_id_t_Add_1_MakingItEqToThatSizeReturnThatValue */

void Test_CF_GetEIDSize_When_ret_Is_LessThan_size_of_cf_entity_id_t_Add_1_AndReturnThatValue(void)
{
    /* Arrange */
    pdu_header_t    dummy_ph;
    pdu_header_t*   arg_ph = &dummy_ph;
    uint8           forced_return_FGV = Any_uint8_LessThan(sizeof(cf_entity_id_t)); /* CUT adds +1 to this value */
    int             expected_result = forced_return_FGV + 1;
    int             local_result;
    
    UT_SetDefaultReturnValue(UT_KEY(FGV), forced_return_FGV); 

    /* Act */
    local_result = CF_GetEIDSize(arg_ph);

    /* Assert */
    UtAssert_True(local_result == forced_return_FGV + 1,
      "CF_GetEIDSize returned %d and should be 1 more than %d (FGV)",
      local_result, forced_return_FGV);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_GetEIDSize_When_ret_Is_LessThan_size_of_cf_entity_id_t_Add_1_AndReturnThatValue */

/* end CF_GetEIDSize tests */

/*******************************************************************************
**
**  CF_GetVariableHeader tests (small) - full coverage - [unstubbables: CF_GetEIDSize, CF_GetEIDSize, CF_MemcpyFromBE]
**
*******************************************************************************/

void Test_CF_GetVariableHeader_When_eid_l_AND_tsn_l_AreNotGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1(void)
{
    /* Arrange */
    pdu_r_msg_t     dummy_ph;
    int             local_result;

    CF_AppData.engine.in.msg = &dummy_ph;

    /* Arrange for CF_GetEIDSize */
    uint32           forced_return_FGV_from_EID = sizeof(cf_entity_id_t); /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_EID); /* FGV + 1 > sizeof(cf_entity_id_t) causes ret to be -1  */

    /* Arrange for CF_GetTSNSize */
    uint32           forced_return_FGV_from_TSN = sizeof(cf_transaction_seq_t); /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_TSN); /* FGV + 1 > sizeof(cf_transaction_seq_t) causes ret to be -1 */

    /* Act */
    local_result = CF_GetVariableHeader();

    /* Assert */
    UtAssert_True(local_result == -1,
      "CF_GetVariableHeader returned %d and should be -1 (fail)",
      local_result);
    /* Assert for CF_GetEIDSize, CF_GetTSNSize */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
} /* end Test_CF_GetVariableHeader_When_eid_l_AND_tsn_l_AreNotGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1 */

void Test_CF_GetVariableHeader_WhenOnly_eid_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1(void)
{
    /* Arrange */
    pdu_r_msg_t     dummy_ph;
    int             local_result;

    CF_AppData.engine.in.msg = &dummy_ph;

    /* Arrange for CF_GetEIDSize */
    uint32           forced_return_FGV_from_EID = sizeof(cf_entity_id_t) - 1; /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_EID); /* Call to FGV returns sizeof(cf_entity_id_t) - 1 + 1 = sizeof(cf_entity_id_t) causes ret to be sizeof(cf_entity_id_t) */

    /* Arrange for CF_GetTSNSize */
    uint32           forced_return_FGV_from_TSN = sizeof(cf_transaction_seq_t); /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_TSN); /* FGV + 1 > sizeof(cf_transaction_seq_t) causes ret to be -1  */

    /* Act */
    local_result = CF_GetVariableHeader();

    /* Assert */
    UtAssert_True(local_result == -1,
      "CF_GetVariableHeader returned %d and should be -1 (fail)",
      local_result);
    /* Assert for CF_GetTSNSize */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_GetVariableHeader_WhenOnly_eid_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1 */

void Test_CF_GetVariableHeader_WhenOnly_tsn_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1(void)
{
    /* Arrange */
    pdu_r_msg_t     dummy_ph;
    int             local_result;

    CF_AppData.engine.in.msg = &dummy_ph;

    /* Arrange for CF_GetEIDSize */
    uint32           forced_return_FGV_from_EID = sizeof(cf_entity_id_t); /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_EID); /* FGV + 1 > sizeof(cf_entity_id_t) causes ret to be -1  */

    /* Arrange for CF_GetTSNSize */
    uint32           forced_return_FGV_from_TSN = sizeof(cf_transaction_seq_t) - 1; /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_TSN); /* Call to FGV returns sizeof(cf_transaction_seq_t) - 1 + 1 = sizeof(cf_transaction_seq_t) causes ret to be sizeof(cf_transaction_seq_t) */

    /* Act */
    local_result = CF_GetVariableHeader();

    /* Assert */
    UtAssert_True(local_result == -1,
      "CF_GetVariableHeader returned %d and should be -1 (fail)",
      local_result);
    /* Assert for CF_GetEIDSize */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_GetVariableHeader_WhenOnly_tsn_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1 */

void Test_CF_GetVariableHeader_GetsAllThreeVariableLengthItemsOutOfHeaderAndReturn_0(void)
{
    /* Arrange */
    pdu_r_msg_t     dummy_ph;
    int             local_result;

    CF_AppData.engine.in.msg = &dummy_ph;

    /* Arrange for CF_GetEIDSize */
    uint32           forced_return_FGV_from_EID = Any_uint32_LessThan(sizeof(cf_entity_id_t)); /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_EID); /* FGV + 1 <= sizeof(cf_entity_id_t) */

    /* Arrange for CF_GetTSNSize */
    uint32           forced_return_FGV_from_TSN = Any_uint32_LessThan(sizeof(cf_transaction_seq_t)); /* unstubbable code adds +1 to this value */

    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_from_TSN); /* FGV + 1 <= sizeof(cf_transaction_seq_t) */

    /* Act */
    local_result = CF_GetVariableHeader();

    /* Assert */
    UtAssert_True(local_result == 0,
      "CF_GetVariableHeader returned %d and should be 0 (success)",
      local_result);
    /* Assert for CF_GetEIDSize, CF_GetTSNSize */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_GetVariableHeader_GetsAllThreeVariableLengthItemsOutOfHeaderAndReturn_0 */

/* end CF_GetVariableHeader tests */

/*******************************************************************************
**
**  CF_SetVariableHeader tests (small) - full coverage  - [unstubbables: CF_GetMemcpySize, CF_MemcpyToBE]
**
*******************************************************************************/

void Test_CF_SetVariableHeader_Call_FSV_Twice(void)
{
    /* Arrange */
    cf_entity_id_t          arg_src_eid;
    cf_entity_id_t          arg_dst_eid;
    cf_transaction_seq_t    arg_tsn;
    pdu_s_msg_t             dummy_msg;

    CF_AppData.engine.out.msg = &dummy_msg;

    /* Act */
    CF_SetVariableHeader(arg_src_eid, arg_dst_eid, arg_tsn);

    /* Assert */
    UtAssert_STUB_COUNT(FSV, 2);
} /* end Test_CF_SetVariableHeader_Call_FSV_Twice */

/* end CF_SetVariableHeader tests */

/*******************************************************************************
**
**  CF_HeaderSize tests (small) - full coverage  - [unstubbables: CF_GetMemcpySize, CF_MemcpyToBE]
**
*******************************************************************************/

void Test_CF_HeaderSize_AssertsWhen_eid_l_IsNotGreaterThan_0(void)
{
    /* Arrange */
    pdu_header_t      dummy_ph;
    pdu_header_t*     arg_ph = &dummy_ph;
    int32             forced_return_FGV_for_eid_l = Any_int32_Negative(); /* negative forces error */
    int32             forced_return_FGV_for_tsn_l = Any_uint8(); /* uint8 used arbitrarily for small size */
    int               local_result;
    
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_eid_l);
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_tsn_l);

    /* Act */
    //local_result = CF_HeaderSize(arg_ph);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (eid_l>0)&&(tsn_l>0)");
} /* end Test_CF_HeaderSize_AssertsWhen_eid_l_IsNotGreaterThan_0 */

void Test_CF_HeaderSize_AssertsWhen_tsn_l_IsNotGreaterThan_0(void)
{
    /* Arrange */
    pdu_header_t      dummy_ph;
    pdu_header_t*     arg_ph = &dummy_ph;
    int32             forced_return_FGV_for_eid_l = Any_uint8(); /* uint8 used arbitrarily for small size */
    int32             forced_return_FGV_for_tsn_l = Any_int32_Negative(); /* negative forces error */
    int               local_result;
    
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_eid_l);
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_tsn_l);

    /* Act */
    //local_result = CF_HeaderSize(arg_ph);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (eid_l>0)&&(tsn_l>0)");
} /* end Test_CF_HeaderSize_AssertsWhen_tsn_l_IsNotGreaterThan_0 */

void Test_CF_HeaderSize_Return_sizeof_pdu_header_t_Plus_2_Times_eid_l_Plus_tsn_l(void)
{
    /* Arrange */
    pdu_header_t      dummy_ph;
    pdu_header_t*     arg_ph = &dummy_ph;
    int32             forced_return_FGV_for_eid_l = Any_uint8(); /* uint8 used arbitrarily for small size */
    int32             forced_return_FGV_for_tsn_l = Any_uint8(); /* uint8 used arbitrarily for small size */
    int               expected_result = sizeof(pdu_header_t) + (2 * (forced_return_FGV_for_eid_l + 1)) + (forced_return_FGV_for_tsn_l + 1); /* each +1 added by CUT */
    int               local_result;
    
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_eid_l);
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_tsn_l);

    /* Act */
    local_result = CF_HeaderSize(arg_ph);

    /* Assert */
    UtAssert_True(local_result == expected_result,
      "CF_GetVariableHeader returned %d and should be %d (sizeof(pdu_header_t) + (2 * (%u + 1))) + (%u + 1))",
      local_result, expected_result, forced_return_FGV_for_eid_l, forced_return_FGV_for_tsn_l);
    UtAssert_STUB_COUNT(FGV, 2);
} /* end Test_CF_HeaderSize_Return_sizeof_pdu_header_t_Plus_2_Times_eid_l_Plus_tsn_l */

/* end CF_SetVariableHeader tests */

/*******************************************************************************
**
**  CF_EndOfHeaderPtr tests (simple) - full coverage - [unstubbables: CF_HeaderSize]
**
*******************************************************************************/

void Test_CF_EndOfHeaderPtr_Return_ph_PointerPlus_CF_HeaderSize_ph(void)
{
    /* Arrange */
    pdu_header_t      dummy_ph;
    pdu_header_t*     arg_ph = &dummy_ph;
    int32             forced_return_FGV_for_eid_l = Any_uint8_LessThan(sizeof(cf_entity_id_t)); /*  Any_uint8_LessThan(sizeof(cf_entity_id_t)) used for reasonable size */
    int32             forced_return_FGV_for_tsn_l = Any_uint8_LessThan(sizeof(cf_transaction_seq_t)); /*  Any_uint8_LessThan(sizeof(cf_transaction_seq_t)) used for reasonable size */
    void*             expected_result = (uint8*)arg_ph + sizeof(pdu_header_t) + (2 * (forced_return_FGV_for_eid_l + 1)) + (forced_return_FGV_for_tsn_l + 1); /* each +1 added by CUT */
    void*             local_result;
    
    /* Arrange for CF_HeaderSize */
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_eid_l);
    UT_SetDeferredRetcode(UT_KEY(FGV), 1, forced_return_FGV_for_tsn_l);

    /* Act */
    local_result = CF_EndOfHeaderPtr(arg_ph);

    /* Assert */
    UtAssert_True(local_result == expected_result,
      "CF_GetVariableHeader returned %p and should be %p ((void*)(((uint8*)ph)+CF_HeaderSize(ph)))",
      local_result, expected_result);
    /* Assert for CF_HeaderSize */
    UtAssert_STUB_COUNT(FGV, 2);
} /* end Test_CF_EndOfHeaderPtr_Return_ph_PointerPlus_CF_HeaderSize_ph */

/* end CF_EndOfHeaderPtr tests */

/*******************************************************************************
**
**  cf_cfdp_helpers_tests UtTest_Add groups
**
*******************************************************************************/
void add_CF_GetMemcpySize_tests(void)
{
    UtTest_Add(Test_CF_GetMemcpySize_WhenGivenSizeIs_0_Given_num_ArrayValuesDoNotMatterAlwaysReturn_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetMemcpySize_WhenGivenSizeIs_0_Given_num_ArrayValuesDoNotMatterAlwaysReturn_0");
    UtTest_Add(Test_CF_GetMemcpySize_WhenGivenSizeIs_1_AndGiven_num_Uint8Is_0_Return_1, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetMemcpySize_WhenGivenSizeIs_1_AndGiven_num_Uint8Is_0_Return_1");
    UtTest_Add(Test_CF_GetMemcpySize_WhenGivenSizeIsAny_int_AndNumsAreAll_0_Return_1, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetMemcpySize_WhenGivenSizeIsAny_int_AndNumsAreAll_0_Return_1");
    UtTest_Add(Test_CF_GetMemcpySize_WhenGiven_num_ElementAtGiven_size_Minus_1_IsNot_0_ReturnValueOf_size, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetMemcpySize_WhenGiven_num_ElementAtGiven_size_Minus_1_IsNot_0_ReturnValueOf_size");
    UtTest_Add(Test_CF_GetMemcpySize_WhenGiven_num_ElementsAre_0_FromGiven_size_UntilAnotherElementInArrayIsNonZeroReturnThatElementsIndexPlus_1, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetMemcpySize_WhenGiven_num_ElementsAre_0_FromGiven_size_UntilAnotherElementInArrayIsNonZeroReturnThatElementsIndexPlus_1");
} /* end add_CF_GetMemcpySize_tests */

void add_CF_MemcpyToBE_tests(void)
{
    /* NOTE: commented out tests should be added if they are able to be - see the tests */
    UtTest_Add(Test_CF_MemcpyToBE_Asserts_src_size_EqTo_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_Asserts_src_size_EqTo_0");
    // UtTest_Add(Test_CF_MemcpyToBE_Asserts_src_size_LessThan_0, 
    //      cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
    //      "Test_CF_MemcpyToBE_Asserts_src_size_LessThan_0");
    UtTest_Add(Test_CF_MemcpyToBE_Asserts_dst_size_EqTo_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_Asserts_dst_size_EqTo_0");
    // UtTest_Add(Test_CF_MemcpyToBE_Asserts_dst_size_LessThan_0, 
    //      cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
    //      "Test_CF_MemcpyToBE_Asserts_dst_size_LessThan_0");
    UtTest_Add(Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_Are_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_Are_0");
    // UtTest_Add(Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_AreNegative, 
    //      cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
    //      "Test_CF_MemcpyToBE_Asserts_both_src_size_And_dst_size_AreNegative");
    // UtTest_Add(Test_CF_MemcpyToBE_Asserts_src_size_IsLessThan_dst_size, 
    //      cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
    //      "Test_CF_MemcpyToBE_Asserts_src_size_IsLessThan_dst_size");
    UtTest_Add(Test_CF_MemcpyToBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value");
    UtTest_Add(Test_CF_MemcpyToBE_When_src_size_GreaterThan_1_And_dst_size_Is_1_CopyOne_src_Value, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_When_src_size_GreaterThan_1_And_dst_size_Is_1_CopyOne_src_Value");
    UtTest_Add(Test_CF_MemcpyToBE_When_dst_size_IsLessThan_src_size_CopyAllRequisite_src_ValuesInto_dst, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_When_dst_size_IsLessThan_src_size_CopyAllRequisite_src_ValuesInto_dst");
    UtTest_Add(Test_CF_MemcpyToBE_CopyAllRequisite_src_ValuesInto_dst_When_dst_size_IsEqTo_src_size, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_CopyAllRequisite_src_ValuesInto_dst_When_dst_size_IsEqTo_src_size");
} /* end add_CF_MemcpyToBE_tests */

void add_CF_MemcpyFromBE_tests(void)
{
    /* NOTE: commented out tests should be added if they are able to be - see the tests */
    UtTest_Add(Test_CF_MemcpyFromBE_Asserts_src_size_EqTo_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyFromBE_Asserts_src_size_EqTo_0");
    // UtTest_Add(Test_CF_MemcpyFromBE_Asserts_src_size_LessThan_0, 
    //      cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
    //      "Test_CF_MemcpyFromBE_Asserts_src_size_LessThan_0");
    UtTest_Add(Test_CF_MemcpyFromBE_Asserts_dst_size_EqTo_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyFromBE_Asserts_dst_size_EqTo_0");
    // UtTest_Add(Test_CF_MemcpyFromBE_Asserts_dst_size_LessThan_0, 
    //      cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
    //      "Test_CF_MemcpyFromBE_Asserts_dst_size_LessThan_0");
    UtTest_Add(Test_CF_MemcpyToBE_Asserts_dst_size_IsLessThan_src_size, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyToBE_Asserts_dst_size_IsLessThan_src_size");
    UtTest_Add(Test_CF_MemcpyFromBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyFromBE_WhenBoth_src_size_And_dst_size_Eq_1_CopyOne_src_Value");
    UtTest_Add(Test_CF_MemcpyFromBE_When_src_size_IsLessThan_dst_size_CopyAllRequisite_src_ValuesInto_dst, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyFromBE_When_src_size_IsLessThan_dst_size_CopyAllRequisite_src_ValuesInto_dst");
    UtTest_Add(Test_CF_MemcpyFromBE_When_src_size_IsEqTo_dst_size_CopyAllRequisite_src_ValuesInto_dst, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_MemcpyFromBE_When_src_size_IsEqTo_dst_size_CopyAllRequisite_src_ValuesInto_dst");
} /* end add_CF_MemcpyFromBE_tests */

void add_CF_GetTSNSize_tests(void)
{
    UtTest_Add(Test_CF_GetTSNSize_When_ret_IsGreaterThan_size_of_cf_transaction_seq_t_ReturnThatValue, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetTSNSize_When_ret_IsGreaterThan_size_of_cf_transaction_seq_t_ReturnThatValue");
    UtTest_Add(Test_CF_GetTSNSize_When_ret_Is_1_LessThan_size_of_cf_transaction_seq_t_Add_1_MakingItEqToThatSizeReturnThatValue, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetTSNSize_When_ret_Is_1_LessThan_size_of_cf_transaction_seq_t_Add_1_MakingItEqToThatSizeReturnThatValue");
    UtTest_Add(Test_CF_GetTSNSize_When_ret_Is_LessThan_size_of_cf_transaction_seq_t_Add_1_AndReturnThatValue, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetTSNSize_When_ret_Is_LessThan_size_of_cf_transaction_seq_t_Add_1_AndReturnThatValue");
} /* end add_CF_GetTSNSize_tests */

void add_CF_GetEIDSize_tests(void)
{
    UtTest_Add(Test_CF_GetEIDSize_When_ret_IsGreaterThan_size_of_cf_entity_id_t_ReturnThatValue, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetEIDSize_When_ret_IsGreaterThan_size_of_cf_entity_id_t_ReturnThatValue");
    UtTest_Add(Test_CF_GetEIDSize_When_ret_Is_1_LessThan_size_of_cf_entity_id_t_Add_1_MakingItEqToThatSizeReturnThatValue, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetEIDSize_When_ret_Is_1_LessThan_size_of_cf_entity_id_t_Add_1_MakingItEqToThatSizeReturnThatValue");
    UtTest_Add(Test_CF_GetEIDSize_When_ret_Is_LessThan_size_of_cf_entity_id_t_Add_1_AndReturnThatValue, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetEIDSize_When_ret_Is_LessThan_size_of_cf_entity_id_t_Add_1_AndReturnThatValue");
} /* end add_CF_GetEIDSize_tests */

void add_CF_GetVariableHeader_tests(void)
{
    UtTest_Add(Test_CF_GetVariableHeader_When_eid_l_AND_tsn_l_AreNotGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetVariableHeader_When_eid_l_AND_tsn_l_AreNotGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1");
    UtTest_Add(Test_CF_GetVariableHeader_WhenOnly_eid_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetVariableHeader_WhenOnly_eid_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1");
    UtTest_Add(Test_CF_GetVariableHeader_WhenOnly_tsn_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetVariableHeader_WhenOnly_tsn_l_IsGreaterThan_0_DoesNotCallAnyMemCopiesReturn_neg1");
    UtTest_Add(Test_CF_GetVariableHeader_GetsAllThreeVariableLengthItemsOutOfHeaderAndReturn_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_GetVariableHeader_GetsAllThreeVariableLengthItemsOutOfHeaderAndReturn_0");
} /* end add_CF_GetVariableHeader_tests */

void add_CF_SetVariableHeader_tests(void)
{
    UtTest_Add(Test_CF_SetVariableHeader_Call_FSV_Twice, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_SetVariableHeader_Call_FSV_Twice");
} /* end add_CF_SetVariableHeader_tests */

void add_CF_HeaderSize_tests(void)
{
    UtTest_Add(Test_CF_HeaderSize_AssertsWhen_eid_l_IsNotGreaterThan_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_HeaderSize_AssertsWhen_eid_l_IsNotGreaterThan_0");
    UtTest_Add(Test_CF_HeaderSize_AssertsWhen_tsn_l_IsNotGreaterThan_0, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_HeaderSize_AssertsWhen_tsn_l_IsNotGreaterThan_0");
    UtTest_Add(Test_CF_HeaderSize_Return_sizeof_pdu_header_t_Plus_2_Times_eid_l_Plus_tsn_l, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_HeaderSize_Return_sizeof_pdu_header_t_Plus_2_Times_eid_l_Plus_tsn_l");
} /* end add_CF_HeaderSize_tests */

void add_CF_EndOfHeaderPtr_tests(void)
{
    UtTest_Add(Test_CF_EndOfHeaderPtr_Return_ph_PointerPlus_CF_HeaderSize_ph, 
         cf_cfdp_helpers_tests_Setup, cf_cfdp_helpers_tests_Teardown, 
         "Test_CF_EndOfHeaderPtr_Return_ph_PointerPlus_CF_HeaderSize_ph");
} /* end add_CF_EndOfHeaderPtr_tests */

/* end cf_cfdp_helpers_tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_cfdp_helpers_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();
    
    add_CF_GetMemcpySize_tests();

    add_CF_MemcpyToBE_tests();

    add_CF_MemcpyFromBE_tests();

    add_CF_GetTSNSize_tests();

    add_CF_GetEIDSize_tests();

    add_CF_GetVariableHeader_tests();

    add_CF_SetVariableHeader_tests();

    add_CF_HeaderSize_tests();

    add_CF_EndOfHeaderPtr_tests();

} /* end UtTest_Setup for cf_cfdp_helpers_tests.c */

/* end cf_cfdp_helpers_tests.c */