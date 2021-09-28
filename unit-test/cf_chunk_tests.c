
/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_chunk.c"



typedef struct {
    const chunks_t *cs;
    const chunk_t *c;
    void *opaque;
} CF_PACK Dummy_compute_gap_fn_t_context_t;
Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;


/*******************************************************************************
**
**  cf_chunk_tests Setup and Teardown
**
*******************************************************************************/

void cf_chunk_tests_Setup(void)
{
    cf_tests_Setup();

    context_Dummy_compute_gap_fn_t.cs = ut_default_ptr;
    context_Dummy_compute_gap_fn_t.c = ut_default_ptr;
    context_Dummy_compute_gap_fn_t.cs = ut_default_ptr;

} /* end cf_chunk_tests_Setup */

void cf_chunk_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_chunk_tests_Teardown */

/* end cf_chunk_tests Setup and Teardown */


/*******************************************************************************
**
**  cf_chunk_tests Local Utility Functions
**
*******************************************************************************/

index_t Any_index_t(void)
{
  return (index_t)Any_uint32();
}

void Dummy_compute_gap_fn_t(const chunks_t *cs, const chunk_t *c, void *opaque)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(Dummy_compute_gap_fn_t), cs);
    UT_Stub_RegisterContext(UT_KEY(Dummy_compute_gap_fn_t), c);
    UT_Stub_RegisterContextGenericArg(UT_KEY(Dummy_compute_gap_fn_t), opaque);
    
    UT_DEFAULT_IMPL(Dummy_compute_gap_fn_t);
}

/*******************************************************************************
**
**  macro tests
**
*******************************************************************************/

void Test_MAX_Returns_a_WhenItIsMaxValue(void)
{
    /* Arrange */
    int32 arg_a = Any_int32();
    int32 arg_b = Any_int32_LessThan(arg_a);
    int32 expectedResult = arg_a;
    
    /* Act */
    result = MAX(arg_a, arg_b);
    
    /* Assert */
    UtAssert_True(result == expectedResult,
      "\nMAX result was %d\n and should be %d\n    which is > %d",
      result, expectedResult, arg_b);
} /* end Test_MAX_Returns_a_WhenItIsMaxValue */

void Test_MAX_Returns_b_WhenItIsMaxValue(void)
{
    /* Arrange */
    int32 arg_b = Any_int32();
    int32 arg_a = Any_int32_LessThan(arg_b);
    int32 expectedResult = arg_b;
    
    /* Act */
    result = MAX(arg_a, arg_b);
    
    /* Assert */
    UtAssert_True(result == expectedResult,
      "\nMAX result was %d\n and should be %d\n    which is > %d",
      result, expectedResult, arg_a);
} /* end Test_MAX_Returns_b_WhenItIsMaxValue */

void Test_MAX_Returns_a_WhenItIsEqualTo_b(void)
{
    /* Arrange */
    int32 arg_a = Any_int32();
    int32 arg_b = arg_a;
    int32 expectedResult = arg_a;
    
    /* Act */
    result = MAX(arg_a, arg_b);
    
    /* Assert */
    UtAssert_True(result == expectedResult,
      "\nMAX result was %d\n and should be %d\n    which is = %d",
      result, expectedResult, arg_a);
} /* end Test_MAX_Returns_a_WhenItIsEqualTo_b */

/* end macro tests */

/*******************************************************************************
**
**  CF_Chunks_EraseRange tests
**
*******************************************************************************/

/* TODO: memmove could be stubbed, but proving problematic. 
 * CF_Chunks_EraseRange tests will use the memmove call for now */

void Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_start = Any_uint32_Except(UINT32_MAX);
    index_t       arg_end = Any_uint32_LessThan(arg_start);
    
    /* Act */
    //CF_Chunks_EraseRange(&Test_chunks, Test_start, Test_end);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue - end>=start");
} /* end Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start */

/* TODO Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start not required but desired */
// void Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start(void)
// {
//     /* Arrange */
//     chunks_t      dummy_chunks;
//     chunks_t*     arg_chunks = &dummy_chunks;
//     index_t       arg_start = Any_uint32_Except(UINT32_MAX);
//     index_t       arg_end = arg_start;
    
//     /* Act */
//     //CF_Chunks_EraseRange(&Test_chunks, Test_start, Test_end);
    
//     /* Assert */
//     UtAssert_Failed("CF_Assert issue - end>=start");
// } /* end Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start */

void Test_CF_Chunks_EraseRange_When_start_Is_OneMoreThan_end_MovesOneChunk(void)
{
    /* Arrange */
    chunk_t         dummy_chunk_array[2];
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_start = 0;
    index_t         arg_end = 1;
    chunk_offset_t  dummy_start_offset = Any_uint32();
    chunk_size_t    dummy_start_size = Any_uint32();
    chunk_offset_t  dummy_end_offset = Any_uint32_Except(dummy_start_offset);
    chunk_size_t    dummy_end_size = Any_uint32_Except(dummy_start_size);

    arg_chunks->count = 2;
    arg_chunks->chunks = dummy_chunk_array;
    
    arg_chunks->chunks[0].offset = dummy_start_offset;
    arg_chunks->chunks[0].size = dummy_start_size;
    arg_chunks->chunks[1].offset = dummy_end_offset;
    arg_chunks->chunks[1].size = dummy_end_size;
    
    /* Act */
    CF_Chunks_EraseRange(arg_chunks, arg_start, arg_end);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, dummy_end_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_end_size);
} /* end Test_CF_Chunks_EraseRange_When_start_Is_OneMoreThan_end_MovesOneChunk */

void Test_CF_Chunks_EraseRange_When_start_IsLessThan_chunks_count_DoesNotChangeStart(void)
{
    /* Arrange */
    chunk_t         dummy_chunk_array[2];
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_start = 2;
    index_t         arg_end = 3;
    chunk_offset_t  dummy_start_offset = Any_uint32();
    chunk_size_t    dummy_start_size = Any_uint32();
    chunk_offset_t  dummy_end_offset = Any_uint32_Except(dummy_start_offset);
    chunk_size_t    dummy_end_size = Any_uint32_Except(dummy_start_size);

    arg_chunks->count = 2;
    arg_chunks->chunks = dummy_chunk_array;
    
    arg_chunks->chunks[0].offset = dummy_start_offset;
    arg_chunks->chunks[0].size = dummy_start_size;
    arg_chunks->chunks[1].offset = dummy_end_offset;
    arg_chunks->chunks[1].size = dummy_end_size;
    
    /* Act */
    CF_Chunks_EraseRange(arg_chunks, arg_start, arg_end);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, dummy_start_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_start_size);
} /* end Test_CF_Chunks_EraseRange_When_start_IsLessThan_chunks_count_DoesNotChangeStart */

/* end CF_Chunks_EraseRange tests */

/*******************************************************************************
**
**  CF_Chunks_EraseChunk tests
**
*******************************************************************************/

/* TODO: memmove could be stubbed, but proving problematic. 
 * CF_Chunks_EraseChunk tests will use the memmove call for now */

void Test_CF_Chunks_EraseChunk_FailsAssert_Given_chunks_count_IsEqTo_0(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_erase_index = Any_index_t();
    
    arg_chunks->count = 0;

    /* Act */
    //CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_EraseChunk_FailsAssert_Given_chunks_count_IsEqTo_0 */

void Test_CF_Chunks_EraseChunk_FailsAssert_Given_erase_index_IsEqTo_chunks_count(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_erase_index;
    
    arg_chunks->count = 1;

    arg_erase_index = 2;

    /* Act */
    //CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_EraseChunk_FailsAssert_Given_erase_index_IsEqTo_chunks_count */

void Test_CF_Chunks_EraseChunk_ErasesOneChunkWhenThereIsOnlyOneAndDecrements_count_To_0(void)
{
    /* Arrange */
    chunk_t         dummy_start_chunk[1];
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_erase_index;

    arg_chunks->count = 1;
    arg_chunks->chunks = dummy_start_chunk;

    arg_erase_index = 0;
    
    /* Act */
    CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, 0);
} /* end Test_CF_Chunks_EraseChunk_ErasesOneChunkWhenThereIsOnlyOneAndDecrements_count_To_0 */

void Test_CF_Chunks_EraseChunk_ErasesOneChunkThatIsNotTheLastFrom_chunks_AndDecrements_count(void)
{
    /* Arrange */
    uint8           intial_count = Any_uint8_LessThan(10) + 2; /* 2-11, reasonably small count for testing and not its normal uint32 */
    chunk_t         dummy_chunk[11] = {0};
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_erase_index;
    uint8           i = 0;
    chunk_offset_t  offsets[11] = {0};
    chunk_size_t    sizes[11] = {0};
    chunk_offset_t  updated_offsets[10] = {0};
    chunk_size_t    updated_sizes[10] = {0};

    arg_chunks->count = intial_count;
    arg_chunks->chunks = dummy_chunk;

    arg_erase_index = Any_uint8_BetweenExcludeMax(0, intial_count - 1); /* -1 ensures not the last chunk */

    /* save chunk information by position for assertions later */
    for(i = 0; i < intial_count; ++i)
    {
      dummy_chunk[i].offset = Any_uint32();
      dummy_chunk[i].size = Any_uint32();

      /* save all offsets and sizes */
      offsets[i] = dummy_chunk[i].offset;
      sizes[i] = dummy_chunk[i].size;

      /* chunk past erased will take the place of erase_index, etc. */
      if (i >= arg_erase_index + 1)
      {
        updated_offsets[i - arg_erase_index - 1] = dummy_chunk[i].offset;
        updated_sizes[i - arg_erase_index - 1] = dummy_chunk[i].size;
      }

    }
    
    /* Act */
    CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, intial_count - 1);
    /* TODO: EXAMPLE: These for loops' output is probably a good indication of why these base outputs do not provide enough info */
    for(i = 0; i < arg_erase_index; ++i)
    {
        UtAssert_UINT32_EQ(dummy_chunk[i].offset, offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(dummy_chunk[i].size, sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */
    }

    for(i = 0; i < (intial_count - arg_erase_index - 1); ++i)
    {
        UtAssert_UINT32_EQ(dummy_chunk[i + arg_erase_index].offset, updated_offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(dummy_chunk[i + arg_erase_index].size, updated_sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */
    }

} /* end Test_CF_Chunks_EraseChunk_ErasesOneChunkWhenThereIsOnlyOneAndDecrements_count_To_0 */

void Test_CF_Chunks_EraseChunk_ErasesLastChunkFrom_chunks_AndDecrements_count(void)
{
    /* Arrange */
    uint8           initial_count = Any_uint8_LessThan(10) + 2; /* 2-11, reasonably small count for testing and not its normal uint32 */
    chunk_t         dummy_chunk[11] = {0};
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_erase_index = initial_count - 1;
    uint8           i = 0;
    chunk_offset_t  offsets[11] = {0};
    chunk_size_t    sizes[11] = {0};
    chunk_offset_t  updated_offsets[10] = {0};
    chunk_size_t    updated_sizes[10] = {0};

    arg_chunks->count = initial_count;
    arg_chunks->chunks = dummy_chunk;

    /* save chunk information by position for assertions later */
    for(i = 0; i < initial_count; ++i)
    {
      dummy_chunk[i].offset = Any_uint32();
      dummy_chunk[i].size = Any_uint32();

      /* save all offsets and sizes */
      offsets[i] = dummy_chunk[i].offset;
      sizes[i] = dummy_chunk[i].size;

      /* chunk past erased will take the place of erase_index, etc. */
      if (i >= arg_erase_index + 1)
      {
        updated_offsets[i - arg_erase_index - 1] = dummy_chunk[i].offset;
        updated_sizes[i - arg_erase_index - 1] = dummy_chunk[i].size;
      }

    }
    
    /* Act */
    CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, initial_count - 1);
    /* TODO: EXAMPLE: These for loops' output is probably a good indication of why these base outputs do not provide enough info */
    for(i = 0; i < arg_erase_index; ++i)
    {
        UtAssert_UINT32_EQ(dummy_chunk[i].offset, offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(dummy_chunk[i].size, sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */
    }

    for(i = 0; i < (initial_count - arg_erase_index - 1); ++i)
    {
      UtAssert_Failed("Test failed beacause there are remaining chunks - this output appears once for each.");  
    }

} /* end Test_CF_Chunks_EraseChunk_ErasesLastChunkFrom_chunks_AndDecrements_count */

/* end CF_Chunks_EraseChunk tests */

/*******************************************************************************
**
**  CF_Chunks_InsertChunk tests
**
*******************************************************************************/

/* TODO: memmove should be wrapped for stubbing, but proving problematic. 
 * CF_Chunks_InsertChunk tests will use the memmove call for now */

void Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsEqTo_chunks_CF_max_chunks(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_index_before = Any_index_t();
    chunk_t       arg_chunk = {0};
    
    arg_chunks->count = Any_uint32();
    arg_chunks->CF_max_chunks = arg_chunks->count;

    /* Act */
    //CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsEqTo_chunks_CF_max_chunks */

/* TODO Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsGreaterThan_chunks_CF_max_chunks not required but desired */
// void Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsGreaterThan_chunks_CF_max_chunks(void)
// {
//     /* Arrange */
//     chunks_t      dummy_chunks;
//     chunks_t*     arg_chunks = &dummy_chunks;
//     index_t       arg_index_before = Any_index_t();
//     chunk_t       dummy_chunk = {0};
//     chunk_t*      arg_chunk = &dummy_chunk;
    
//     arg_chunks->count = Any_uint32_Except(UINT32_MAX);
//     arg_chunks->CF_max_chunks = Any_uint32_GreaterThan(arg_chunks->count);

//     /* Act */
//     //CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);
    
//     /* Assert */
//     UtAssert_Failed("CF_Assert issue");
// } /* end Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsGreaterThan_chunks_CF_max_chunks */

void Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEmpty_chunks(void)
{
    /* Arrange */
    chunk_t         initial_chunks[1];
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_index_before = 0;
    chunk_t         dummy_chunk = {0};
    const chunk_t*  arg_chunk;
    
    arg_chunks->count = 0;
    arg_chunks->CF_max_chunks = UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size = Any_uint32();

    arg_chunk = &dummy_chunk;

    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, dummy_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->count, 1);
} /* end Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEmpty_chunks */

void Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoStartOfSingle_chunks(void)
{
    /* Arrange */
    chunk_t         initial_start_chunk;
    chunk_t         initial_chunks[2] = {0};
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_index_before = 0;
    chunk_t         dummy_chunk = {0};
    const chunk_t*  arg_chunk;
    
    initial_start_chunk.offset = Any_uint32();
    initial_start_chunk.size = Any_uint32();
    initial_chunks[0] = initial_start_chunk;
    
    arg_chunks->count = 1;
    arg_chunks->CF_max_chunks = UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size = Any_uint32();

    arg_chunk = &dummy_chunk;

    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, dummy_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].offset, initial_start_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].size, initial_start_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->count, 2);
} /* end Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoStartOfSingle_chunks */

void Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEndOfSingle_chunks(void)
{
    /* Arrange */
    chunk_t         initial_start_chunk;
    chunk_t         initial_chunks[2] = {0};
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_index_before = 1;
    chunk_t         dummy_chunk = {0};
    const chunk_t*  arg_chunk;
    
    initial_start_chunk.offset = Any_uint32();
    initial_start_chunk.size = Any_uint32();
    initial_chunks[0] = initial_start_chunk;
    
    arg_chunks->count = 1;
    arg_chunks->CF_max_chunks = UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size = Any_uint32();

    arg_chunk = &dummy_chunk;
    
    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, initial_start_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, initial_start_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].offset, dummy_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].size, dummy_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->count, 2);
} /* end Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEndOfSingle_chunks */

void Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoSome_chunks(void)
{
    /* Arrange */
    index_t         initial_count = Any_uint8_LessThan(10) + 1;/* 1-10, reasonably small count for testing and not its normal uint32 */
    chunk_t         initial_chunks[11] = {0};
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_index_before = 0;
    chunk_t         dummy_chunk = {0};
    const chunk_t*  arg_chunk;
    chunk_offset_t  updated_offsets[10] = {0};
    chunk_size_t    updated_sizes[10] = {0};
    uint8           i;

    for(i = 0; i < initial_count; ++i)
    {
      uint8 j = i;

      if (i >= arg_index_before)
      {
        ++j;
      }

      initial_chunks[i].offset = Any_uint32();
      initial_chunks[i].size = Any_uint32();

      /* save all offsets and sizes */
      updated_offsets[j] = initial_chunks[i].offset;
      updated_sizes[j] = initial_chunks[i].size;
    }
    
    arg_chunks->count = initial_count;
    arg_chunks->CF_max_chunks = UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size = Any_uint32();
    updated_offsets[0] = dummy_chunk.offset;
    updated_sizes[0] = dummy_chunk.size;

    arg_chunk = &dummy_chunk;
    
    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, initial_count + 1);

    for(i = 0; i < initial_count + 1; ++i)
    {
        UtAssert_UINT32_EQ(arg_chunks->chunks[i].offset, updated_offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(arg_chunks->chunks[i].size, updated_sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */  
    }
} /* end Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoSome_chunks */

/* end CF_Chunks_InsertChunk tests */

/*******************************************************************************
**
**  CF_Chunks_FindInsertPosition tests
**
*******************************************************************************/

void Test_CF_Chunks_FindInsertPosition_Given_chunks_IsEmptyReturns_0(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    arg_chunks->count = 0;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsEqTo_chunks_CF_max_chunks */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturns_0(void)
{
    /* Arrange */
    chunk_t       initial_chunks[1] = {0};
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    initial_chunks[0].offset = Any_uint32_Except(0);

    arg_chunk->offset = Any_uint32_LessThan(initial_chunks[0].offset);

    arg_chunks->count = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturns_0 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturns_0(void)
{
    /* Arrange */
    chunk_t       initial_chunks[1] = {0};
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    initial_chunks[0].offset = Any_uint32_Except(0);

    arg_chunk->offset = initial_chunks[0].offset;

    arg_chunks->count = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturns_0 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturns_1(void)
{
    /* Arrange */
    chunk_t       initial_chunks[1] = {0};
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    initial_chunks[0].offset = Any_uint32_Except(UINT32_MAX);

    arg_chunk->offset = Any_uint32_GreaterThan(initial_chunks[0].offset);

    arg_chunks->count = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturns_1 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturns_1(void)
{
    /* Arrange */
    chunk_t       initial_chunks[2] = {0};
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    initial_chunks[0].offset = Any_uint32_BetweenExcludeMax(0, UINT32_MAX - 1);
    initial_chunks[1].offset = Any_uint32_BetweenInclusive(initial_chunks[0].offset + 2, UINT32_MAX);

    arg_chunk->offset = Any_uint32_BetweenExcludeMax(initial_chunks[0].offset, initial_chunks[1].offset);

    arg_chunks->count = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturns_1 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnsCorrectExpectedPosition(void)
{
    /* Arrange */
    chunk_t       initial_chunks[20] = {0}; /* 20 used as a reasonably fast size for the test, but is still illustrative */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;
    uint8         num_chunks_before = Any_uint8_LessThan(10);
    uint8         num_chunks_after = Any_uint8_LessThan(10);
    uint8         i = 0;

    arg_chunk->offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    for(i = 0; i < num_chunks_before; ++i)
    {
       initial_chunks[i].offset = Any_uint32_LessThan(arg_chunk->offset);
    }

    for(i = 0; i < num_chunks_after; ++i)
    {
       initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(arg_chunk->offset);
    }

    arg_chunks->count = num_chunks_before + num_chunks_after;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, num_chunks_before);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnsCorrectExpectedPosition */

/* end CF_Chunks_FindInsertPosition tests */

/*******************************************************************************
**
**  CF_Chunks_CombinePrevious tests
**
*******************************************************************************/

void Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    arg_chunks->CF_max_chunks = Any_uint32_Except(0);
    arg_i = arg_chunks->CF_max_chunks;

    /* Act */
    //local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks */

/* TODO Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks not required but desired */
// void Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks(void)
// {
//     /* Arrange */
//     chunks_t      dummy_chunks;
//     chunks_t*     arg_chunks = &dummy_chunks;
//     index_t       arg_i;
//     chunk_t       dummy_chunk = {0};
//     chunk_t*      arg_chunk = &dummy_chunk;
//     index_t       local_result;

//     arg_chunks->CF_max_chunks = Any_uint32_Except(0);
//     arg_i = Any_uint32_GreaterThan(arg_chunks->CF_max_chunks);

//     /* Act */
//     //local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
//     /* Assert */
//     UtAssert_Failed("CF_Assert issue");
// } /* end Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks */

void Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);

    arg_chunks->CF_max_chunks = Any_uint32_Except(0);
    arg_i = 0;
    arg_chunks->count = Any_uint32_GreaterThan(0);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0 */

void Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);

    arg_chunks->CF_max_chunks = Any_uint32_Except(0);
    arg_i = Any_uint32_LessThan(arg_chunks->CF_max_chunks);
    arg_chunks->count = 0;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0 */

void Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);

    arg_chunks->CF_max_chunks = Any_uint32_Except(0);
    arg_i = 0;
    arg_chunks->count = 0;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0 */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_offset_Return_0(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i = Any_uint32_LessThan(dummy_chunks_count) + 1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    chunk_offset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    chunk_size_t   dummy_size = Any_uint32_LessThan(UINT32_MAX / 2);
    int           local_result = Any_int_Except(0);

    arg_chunks->CF_max_chunks = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count = dummy_chunks_count;
    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size = dummy_size;

    arg_chunk->offset = Any_uint32_GreaterThan(dummy_offset + dummy_size);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_offset_Return_0 */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i = Any_uint32_LessThan(dummy_chunks_count) + 1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    chunk_offset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    chunk_size_t   dummy_size = Any_uint32_LessThan(UINT32_MAX / 2);
    int           local_result = Any_int_Except(0);

    arg_chunks->CF_max_chunks = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count = dummy_chunks_count;
    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size = dummy_size;

    arg_chunk->offset = Any_uint32_LessThan(dummy_offset + dummy_size + 1);
    arg_chunk->size = Any_uint32_LessThan(dummy_offset + dummy_size - arg_chunk->offset);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0 */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i = Any_uint32_LessThan(dummy_chunks_count) + 1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    chunk_offset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    chunk_size_t   dummy_size = Any_uint32_LessThan(UINT32_MAX / 2);
    int           local_result = Any_int_Except(0);

    arg_chunks->CF_max_chunks = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count = dummy_chunks_count;
    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size = dummy_size;

    arg_chunk->offset = Any_uint32_LessThan(dummy_offset + dummy_size + 1);
    arg_chunk->size = Any_uint32_LessThan(dummy_offset + dummy_size - arg_chunk->offset);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0 */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButEqTo_chunk_end_Return_0(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i = Any_uint32_LessThan(dummy_chunks_count) + 1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    chunk_offset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    chunk_size_t   dummy_size = Any_uint32_LessThan(UINT32_MAX / 2);
    int           local_result = Any_int_Except(0);

    arg_chunks->CF_max_chunks = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count = dummy_chunks_count;
    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size = dummy_size;

    arg_chunk->offset = Any_uint32_LessThan(dummy_offset + dummy_size + 1);
    arg_chunk->size = dummy_offset + dummy_size - arg_chunk->offset;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButEqTo_chunk_end_Return_0 */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i = Any_uint32_LessThan(dummy_chunks_count) + 1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    chunk_offset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    chunk_size_t   initial_size = Any_uint32_LessThan(UINT32_MAX / 2);
    int           local_result = Any_int_Except(0);
    
    arg_chunks->CF_max_chunks = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count = dummy_chunks_count;
    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size = initial_size;

    arg_chunk->offset = 0; /* 0 guarantees that chunk->offset <= prev_end in CUT */
    arg_chunk->size = dummy_offset + initial_size - arg_chunk->offset + 1;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
    UtAssert_UINT32_EQ(arg_chunks->chunks[arg_i - 1].size, arg_chunk->offset + arg_chunk->size - arg_chunks->chunks[arg_i - 1].offset);
} /* end Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1 */

/* end CF_Chunks_CombinePrevious test */

/*******************************************************************************
**
**  CF_Chunks_CombineNext tests
**
*******************************************************************************/

void Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Returns_0(void)
{
    /* Arrange */    
    chunk_t       dummy_chunks_chunks; 
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk;
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);
    
    arg_chunks->count = Any_uint32();
    arg_i = arg_chunks->count;

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Returns_0 */

void Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_chunk_offset(void)
{
    /* Arrange */    
    chunk_t       dummy_chunks_chunks = {0}; 
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);

    arg_chunks->count = Any_uint32();
    arg_i = Any_uint32_Except(arg_chunks->count);

    /* Act */
    //local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_chunk_offset */

void Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_chunk_end_Returns_0(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i = Any_uint32_LessThan(arg_chunks->count);

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = Any_uint32_GreaterThan(arg_chunk->offset + arg_chunk->size);

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_chunk_end_Returns_0 */

void Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_CombinesChunksNotAtEndDoesNotEraseRangeReturns_1(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i = Any_uint32_LessThan(arg_chunks->count);

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = arg_chunk->offset + arg_chunk->size;
    arg_chunks->chunks[arg_i].size = 1; /* size = 1 moves existing end to greater than chunk_end */

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_EraseRange, 0);
    UtAssert_UINT32_EQ(local_result, 1);
} /* end Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_CombinesChunksNotAtEndDoesNotEraseRangeReturns_1 */

void Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    uint32        expected_num_chunks_combined;
    uint8         j = 0;
    int           local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i = Any_uint32_LessThan(arg_chunks->count - 2); /* -2 to argchunks->count to ensure at least 2 chunks till end */

    expected_num_chunks_combined = Any_uint32_BetweenExcludeMax(2, dummy_chunks_count - arg_i); /* 2 to 1 less than delta arg_i to dummy_chunks_count */

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = arg_chunk->offset + arg_chunk->size - expected_num_chunks_combined + 1;  /* +1 for itself */

    for(j = 1; j < expected_num_chunks_combined; ++j)
    {
      arg_chunks->chunks[arg_i + j].offset = 1;
    }

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1 */

void Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1(void)
{
    /* Arrange */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       arg_i;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    int           local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i = Any_uint32_LessThan(arg_chunks->count - 1); /* -1 to argchunks->count to ensure at least 2 chunks till end */

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = arg_chunk->offset + arg_chunk->size - 1;
    arg_chunks->chunks[arg_i].size = 1; /* size = 1 moves existing end to greater than chunk_end */

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1 */

/* end CF_Chunks_CombineNext tests */

/*******************************************************************************
**
**  CF_Chunks_FindSmallestSize tests
**
*******************************************************************************/

void Test_CF_Chunks_FindSmallestSize_Given_chunks_IsEmptyReturns_0(void)
{
    /* Arrange */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    arg_chunks->count = 0;

    /* Act */
    local_result = CF_Chunks_FindSmallestSize(arg_chunks);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsEqTo_chunks_CF_max_chunks */

void Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturns_0(void)
{
    /* Arrange */
    chunk_t       initial_chunks[1] = {0};
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    chunk_t       dummy_chunk = {0};
    chunk_t*      arg_chunk = &dummy_chunk;
    index_t       local_result;

    initial_chunks[0].size = Any_uint32();

    arg_chunks->count = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindSmallestSize(arg_chunks);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturns_0 */

void Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest(void)
{
    /* Arrange */
    chunk_t       initial_chunks[2] = {0};
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       expected_result;
    index_t       local_result;


    initial_chunks[0].size = Any_uint32_Except(0);
    initial_chunks[1].size = Any_uint32_Except(initial_chunks[0].size);

    if (initial_chunks[0].size < initial_chunks[1].size)
    {
      expected_result = 0;
    }
    else
    {
      expected_result = 1;
    }

    arg_chunks->count = 2;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindSmallestSize(arg_chunks);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, expected_result);
} /* end Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest */

void Test_CF_Chunks_FindSmallestSize_Given_chunks_HasManyChunksReturnsCorrectIndexOfSmallest(void)
{
    /* Arrange */
    uint8         dummy_count = 25;
    chunk_t       initial_chunks[25] = {0}; /* 25 for initial_chunks is arbitrary, small for speed */
    chunks_t      dummy_chunks;
    chunks_t*     arg_chunks = &dummy_chunks;
    index_t       expected_result = Any_uint8_LessThan(25);
    index_t       local_result;
    uint8         i = 0;

    for (i = 0; i < dummy_count; ++i)
    {
        if (i != expected_result)
        {
            initial_chunks[i].size = Any_uint32_GreaterThan(100); /* 100 is arbitrary, a number so we can select a smallest under it */
        }
        else
        {
            initial_chunks[i].size = Any_uint32_LessThan(100); /* 100 is arbitrary, a number so we can select a smallest under it */
        }

    }

    arg_chunks->count = dummy_count;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindSmallestSize(arg_chunks);
    
    /* Assert */
    UtAssert_UINT32_EQ(local_result, expected_result);
} /* end Test_CF_Chunks_FindSmallestSize_Given_chunks_HasManyChunksReturnsCorrectIndexOfSmallest */

/* end CF_Chunks_FindSmallestSize tests */

/*******************************************************************************
**
**  CF_Chunks_Insert tests
**  NOTE: Here is an example of a very difficult setup, the arguments to the CUT - arg_chunks, arg_i, arg_chunk
**        are all passed to two different unstubbable functions CF_Chunks_CombineNext and CF_Chunks_CombinePrevious.
**        If these could be stubbed, simple testing - just return the value required to see the behavior of the CUT
**        Because they cannot be stubbed - This puts constraints on the values that the arguments must be set to in
**        the Arrange phase and when a path takes it through both calls those constraints become more difficult to
**        ascertain as attempting to make one unstubbable function return a value required can easily cause the other
**        call to no longer work and vice versa.  Lack of isolation makes this testing time consuming, difficult and
**        a nightmare for maintenance.
**
*******************************************************************************/

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_DoNothing(void)
{
    /* Arrange */
    chunks_t*   arg_chunks;
    index_t     initial_i;
    index_t     arg_i;
    chunk_t     dummy_chunk = {0};
    chunk_t*    arg_chunk = &dummy_chunk;

    /* Arrange unstubbable: CF_Chunks_CombineNext to return 1 */
    /* Arrange unstubbable: CF_Chunks_CombinePrevious to return 0 */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;

    arg_chunks = &dummy_chunks;
    arg_chunks->CF_max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    initial_i = Any_uint32_LessThan(arg_chunks->count);
    arg_i = initial_i;
    
    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = arg_chunk->offset + arg_chunk->size;
    arg_chunks->chunks[arg_i].size = 0; /* size = 0 existing end equal chunk_end */

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    /* TODO: nothing good to verify, provides coverage only */
    UtAssert_True(arg_chunks->count == initial_i + 1,
      "chunks->count is %u and should be 1 more than %u (value of i at call)",
      arg_chunks->count, initial_i);
} /* end Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_DoNothing */

void Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk(void)
{
    /* Arrange */
    chunks_t*   arg_chunks;
    index_t     arg_i;
    chunk_t     dummy_chunk = {0};
    chunk_t*    arg_chunk = &dummy_chunk;

    /* Arrange unstubbable: CF_Chunks_CombineNext to return 1 */
    /* Arrange unstubbable: CF_Chunks_CombinePrevious to return 1 */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;

    arg_chunks = &dummy_chunks;
    arg_chunks->CF_max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    arg_i = Any_uint32_LessThan(arg_chunks->count - 1) + 1; /* -1 then +1 to ensure at least 1 */
    
    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = UINT32_MAX - 1;
    arg_chunks->chunks[arg_i].offset = 0;
    arg_chunks->chunks[arg_i].size = UINT32_MAX; /* size = 1 moves existing end to greater than chunk_end */

    /* Arrange unstubbable: CF_Chunks_EraseChunk - TODO: when able */

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_True(arg_chunks->count == dummy_chunks_count - 1,
      "chunks->count is %u and should be 1 less than %u (value before call)",
      arg_chunks->count, dummy_chunks_count);
} /* end Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_DoNothing(void)
{
    /* Arrange */
    chunks_t*   arg_chunks;
    index_t     arg_i;
    chunk_t     dummy_chunk = {0};
    chunk_t*    arg_chunk = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 1 */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;

    arg_chunks = &dummy_chunks;
    arg_chunks->CF_max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    arg_i = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size = 6; /* for chunk_end to be 11 = 5 + 6 */

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 9;
    arg_chunks->chunks[arg_i - 1].size = 1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10), but prev_end (10) < chunk_end (11) )

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    /* Assert for CF_Chunks_InsertChunk */
    UtAssert_True(arg_chunks->count == dummy_chunks_count,
      "chunks->count is %u and should be %u (value before call)",
      arg_chunks->count, dummy_chunks_count);
} /* end Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_DoNothing */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk(void)
{
    /* Arrange */
    chunks_t*   arg_chunks;
    index_t     arg_i;
    chunk_t     dummy_chunk = {0};
    chunk_t*    arg_chunk = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 0 */
    uint8         dummy_chunks_count = 10; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;

    arg_chunks = &dummy_chunks;
    arg_chunks->CF_max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    arg_i = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size = 5; /* for chunk_end to be 10 = 5 + 5 */

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 9;
    arg_chunks->chunks[arg_i - 1].size = 1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10) and  prev_end (10) = chunk_end (10) )

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    /* Assert for CF_Chunks_InsertChunk */
    UtAssert_True(arg_chunks->count == (uint16)(dummy_chunks_count + 1),
      "chunks->count is %u and should be 1 more than %u (value before call)",
      arg_chunks->count, dummy_chunks_count);
} /* end Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing(void)
{
    /* Arrange */
    chunks_t*   arg_chunks;
    index_t     arg_i;
    chunk_t     dummy_chunk = {0};
    chunk_t*    arg_chunk = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 0 */
    uint8         dummy_chunks_count = 3; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;

    arg_chunks = &dummy_chunks;
    arg_chunks->CF_max_chunks = 3;

    arg_chunks->count = dummy_chunks_count;
    arg_i = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size = 5; /* for chunk_end to be 10 = 5 + 5 */

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 4;
    arg_chunks->chunks[arg_i - 1].size = arg_chunk->size + 1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10) and  prev_end (10) = chunk_end (10) ) */

    /* Arrange for CF_Chunks_FindSmallestSize */
    arg_chunks->chunks[0].size = arg_chunk->size + 1;
    arg_chunks->chunks[1].size = arg_chunk->size + 1;

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_True(arg_chunks->count == dummy_chunks_count,
      "chunks->count is %u and should be %u (value before call)",
      arg_chunks->count, dummy_chunks_count);
} /* end Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_And_CF_Chunks_InsertChunk(void)
{
    /* Arrange */
    chunks_t*   arg_chunks;
    index_t     arg_i;
    chunk_t     dummy_chunk = {0};
    chunk_t*    arg_chunk = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 0 */
    uint8         dummy_chunks_count = 3; /* 10 for dummy_chunks_count is arbitrary, chosen for speed */   
    chunk_t       dummy_chunks_chunks[10] = {0}; /* 10 repeated for dummy_chunks for build ability */
    chunks_t      dummy_chunks;

    arg_chunks = &dummy_chunks;
    arg_chunks->CF_max_chunks = 3;

    arg_chunks->count = dummy_chunks_count;
    arg_i = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size = 5; /* for chunk_end to be 10 = 5 + 5 */

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 4;
    arg_chunks->chunks[arg_i - 1].size = arg_chunk->size + 1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10) and  prev_end (10) = chunk_end (10) ) */

    /* Arrange for CF_Chunks_FindSmallestSize */
    arg_chunks->chunks[0].size = 0;
    arg_chunks->chunks[1].size = 0;

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    /* Assert for CF_Chunks_EraseChunk and CF_Chunks_InsertChunk (-1/+1 thus no change? TODO: better way to prove these ran)*/
    UtAssert_True(arg_chunks->count == dummy_chunks_count,
      "chunks->count is %u and should be %u (value before call)",
      arg_chunks->count, dummy_chunks_count);
} /* end Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_And_CF_Chunks_InsertChunk */

/* end CF_Chunks_Insert tests */

/*******************************************************************************
**
**  CF_Chunks_Add tests
**
*******************************************************************************/
void Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset(void)
{
    /* Arrange */
    chunk_offset_t     arg_offset = UINT32_MAX;
    chunk_size_t       arg_size = 1;
    chunk_t            initial_chunks[20] = {0}; /* 20 used as a reasonably fast size for the test, but is still illustrative */
    chunks_t           dummy_chunks;
    chunks_t*          arg_chunks = &dummy_chunks;

    /* Arrange unstubbable: CF_Chunks_FindInsertPosition */
    chunk_t       dummy_chunk = {0};
    uint8         num_chunks_before = Any_uint8_LessThan(10);
    uint8         num_chunks_after = Any_uint8_LessThan(10);
    uint8         i = 0;

    dummy_chunk.offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    for(i = 0; i < num_chunks_before; ++i)
    {
       initial_chunks[i].offset = Any_uint32_LessThan(dummy_chunk.offset);
    }

    for(i = 0; i < num_chunks_after; ++i)
    {
       initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(dummy_chunk.offset);
    }

    arg_chunks->count = num_chunks_before + num_chunks_after;
    arg_chunks->chunks = initial_chunks;
    
    /* Act */
    // CF_Chunks_Add(arg_chunks, arg_offset, arg_size);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset */

void Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_IsNotLessThanOrEqToGiven_chunks_i_offset(void)
{
    /* Arrange */
    chunk_offset_t     arg_offset = 1;
    chunk_size_t       arg_size = 1;
    chunk_t            initial_chunks[20] = {0}; /* 20 used as a reasonably fast size for the test, but is still illustrative */
    chunks_t           dummy_chunks;
    chunks_t*          arg_chunks = &dummy_chunks;

    /* Arrange unstubbable: CF_Chunks_FindInsertPosition */
    chunk_t       dummy_chunk = {0};
    uint8         num_chunks_before = Any_uint8_LessThan(10);
    uint8         num_chunks_after = Any_uint8_LessThan(10);
    uint8         i = 0;

    dummy_chunk.offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    for(i = 0; i < num_chunks_before; ++i)
    {
       initial_chunks[i].offset = Any_uint32_LessThan(dummy_chunk.offset);
    }

    for(i = 0; i < num_chunks_after; ++i)
    {
       initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(dummy_chunk.offset);
    }

    arg_chunks->count = num_chunks_before + num_chunks_after;
    arg_chunks->chunks = initial_chunks;
    
    /* Act */
    // TODO: unsure how to set this up to hit the assert.
    // CF_Chunks_Add(arg_chunks, arg_offset, arg_size);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_IsNotLessThanOrEqToGiven_chunks_i_offset */

void Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenNotAtEnd(void)
{
    /* Arrange */
    chunk_offset_t     arg_offset = 0;
    chunk_size_t       arg_size = 0;
    chunk_t            initial_chunks[20] = {0}; /* 20 used as a reasonably fast size for the test, but is still illustrative */
    chunks_t           dummy_chunks;
    chunks_t*          arg_chunks = &dummy_chunks;

    /* Arrange unstubbable: CF_Chunks_FindInsertPosition */
    chunk_t       dummy_chunk = {0};
    uint8         num_chunks_before = Any_uint8_LessThan(10);
    uint8         num_chunks_after = Any_uint8_LessThan(10);
    uint8         i = 0;

    dummy_chunk.offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    for(i = 0; i < num_chunks_before; ++i)
    {
       initial_chunks[i].offset = Any_uint32_LessThan(dummy_chunk.offset);
    }

    for(i = 0; i < num_chunks_after; ++i)
    {
       initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(dummy_chunk.offset);
    }

    arg_chunks->count = num_chunks_before + num_chunks_after;
    arg_chunks->chunks = initial_chunks;
    
    /* Act */
    CF_Chunks_Add(arg_chunks, arg_offset, arg_size);
    
    /* Assert */
    /* TODO: find a way to verify results in this test */
    UtAssert_True(true, "Nothing is able to be asserted on here, success is measured by test not segfaulting and coverage check.");
} /* end Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenNotAtEnd */

void Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenAtEnd(void)
{
    /* Arrange */
    chunk_offset_t     arg_offset = 10;
    chunk_size_t       arg_size = 5;
    chunk_t            initial_chunks[1] = {0}; /* 20 used as a reasonably fast size for the test, but is still illustrative */
    chunks_t           dummy_chunks;
    chunks_t*          arg_chunks = &dummy_chunks;

    /* Arrange unstubbable: CF_Chunks_FindInsertPosition */
  
    initial_chunks->offset = 0;
    initial_chunks->size = 0;
    arg_chunks->count = 1;
    arg_chunks->chunks = initial_chunks;
    
    /* Act */
    CF_Chunks_Add(arg_chunks, arg_offset, arg_size);
    
    /* Assert */
    /* TODO: find a way to verify results in this test */
    UtAssert_True(true, "Nothing is able to be asserted on here, success is measured by test not segfaulting and coverage check.");
} /* end Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenAtEnd */

/* end CF_Chunks_Add tests */

/*******************************************************************************
**
**  CF_Chunks_RemoveFromFirst tests
**
*******************************************************************************/

void Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Calls_CF_Chunks_EraseChunk(void)
{
    /* Arrange */
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    chunk_offset_t  initial_chunks_chunks_offset = Any_uint32();
    chunk_size_t    arg_size = Any_uint32_Except(0);
    chunk_t         dummy_chunks_chunks;
    const chunk_t*  local_result;

    arg_chunks->count = 1;
    arg_chunks->chunks = &dummy_chunks_chunks;
    arg_chunks->chunks[0].offset = initial_chunks_chunks_offset;
    arg_chunks->chunks[0].size = arg_size;

    /* Act */
    CF_Chunks_RemoveFromFirst(arg_chunks, arg_size);
    
    /* Assert */
    /* Assert Unstubbable: CF_Chunks_EraseChunk */
    UtAssert_UINT32_EQ(arg_chunks->count, 0);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, initial_chunks_chunks_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, 0);
} /* end Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Calls_CF_Chunks_EraseChunk */

void Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Calls_CF_Chunks_EraseChunk(void)
{
    /* Arrange */
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    chunk_offset_t  initial_chunks_chunks_offset = Any_uint32();
    chunk_size_t    arg_size = Any_uint32_Except(UINT32_MAX - 1) + 2; /* from 2 to UINT_32_MAX */
    chunk_size_t    dummy_chunk_size = Any_uint32_LessThan(arg_size);
    chunk_t         dummy_chunks_chunks;
    const chunk_t*  local_result;

    arg_chunks->count = 1;
    arg_chunks->chunks = &dummy_chunks_chunks;
    arg_chunks->chunks[0].offset = initial_chunks_chunks_offset;
    arg_chunks->chunks[0].size = dummy_chunk_size;

    /* Act */
    CF_Chunks_RemoveFromFirst(arg_chunks, arg_size);
    
    /* Assert */
    /* Assert Unstubbable: CF_Chunks_EraseChunk */
    UtAssert_UINT32_EQ(arg_chunks->count, 0);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, initial_chunks_chunks_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, 0);
} /* end Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Calls_CF_Chunks_EraseChunk */

void Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddsGiven_size_To_chunks_chunks_0_offset_AndSets_chunks_chunks_size_To_0(void)
{
    /* Arrange */
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    chunk_offset_t  initial_chunks_chunks_offset = Any_uint32();
    chunk_size_t    arg_size = Any_uint32_Except(0); /* from 2 to UINT_32_MAX */
    chunk_size_t    dummy_chunk_size = Any_uint32_GreaterThan(arg_size);
    // chunk_size_t    arg_size = Any_uint32_Except(0);
    // chunk_size_t    dummy_chunk_size = Any_uint32_LessThan(arg_size);
    chunk_t         dummy_chunks_chunks;
    const chunk_t*  local_result;

    arg_chunks->count = 1;
    arg_chunks->chunks = &dummy_chunks_chunks;
    arg_chunks->chunks[0].offset = initial_chunks_chunks_offset;
    arg_chunks->chunks[0].size = dummy_chunk_size;

    /* Act */
    CF_Chunks_RemoveFromFirst(arg_chunks, arg_size);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, 1);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, initial_chunks_chunks_offset + arg_size);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_chunk_size - arg_size);
} /* end Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddsGiven_size_To_chunks_chunks_0_offset_AndSets_chunks_chunks_size_To_0 */

/* end CF_Chunks_RemoveFromFirst tests */

/*******************************************************************************
**
**  CF_Chunks_GetFirstChunk tests
**
*******************************************************************************/

void Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Returns_NULL(void)
{
    /* Arrange */
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    const chunk_t*  local_result;

    arg_chunks->count = 0;

    /* Act */
    local_result = CF_Chunks_GetFirstChunk(arg_chunks);
    
    /* Assert */
    UtAssert_NULL(local_result);
} /* end Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Returns_NULL */

void Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnsOnlyChunk(void)
{
    /* Arrange */
    chunks_t        dummy_chunks = {0};
    chunks_t*       arg_chunks = &dummy_chunks;
    chunk_t         only_chunk;
    const chunk_t*  local_result;

    arg_chunks->count = 1;
    arg_chunks->chunks = &only_chunk;

    /* Act */
    local_result = CF_Chunks_GetFirstChunk(arg_chunks);
    
    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &arg_chunks->chunks[0]);
    UtAssert_ADDRESS_EQ(local_result, &only_chunk);
} /* end Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnsOnlyChunk */

void Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_tReturnsFirstChunk(void)
{
    /* Arrange */
    chunks_t        dummy_chunks = {0};
    chunks_t*       arg_chunks = &dummy_chunks;
    chunk_t         first_chunk;
    const chunk_t*  local_result;

    arg_chunks->count = Any_index_t();
    arg_chunks->chunks = &first_chunk;

    /* Act */
    local_result = CF_Chunks_GetFirstChunk(arg_chunks);
    
    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &arg_chunks->chunks[0]);
    UtAssert_ADDRESS_EQ(local_result, &first_chunk);
} /* end Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_tReturnsFirstChunk */

/* CF_Chunks_GetFirstChunk tests */

/*******************************************************************************
**
**  CF_Chunks_Init tests
**
*******************************************************************************/

void Test_CF_Chunks_Init_AssertsBecauseGiven_CF_max_chunks_Is_0(void)
{
    /* Arrange */
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_CF_max_chunks = Any_uint16() + 2; /* 2-65537, uint16 is used instead of index_t to have a reasonably decent size for the test without being too large (segfault) */
    chunk_t*        arg_chunks_mem;

    /* Act */
    // CF_Chunks_Init(arg_chunks, arg_CF_max_chunks, arg_chunks_mem);
    
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_Init_AssertsBecauseGiven_CF_max_chunks_Is_0 */

void Test_CF_Chunks_Init_SetsGiven_chunks_CF_max_chunks_ToGiven_CF_max_chunks(void)
{
    /* Arrange */
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         arg_CF_max_chunks = Any_uint16() + 2; /* 2-65537, uint8 is used instead of index_t to have a reasonably decent size for the test without being too large (segfault) */
    chunk_t*        arg_chunks_mem[arg_CF_max_chunks];
    const chunk_t*  local_result;

    arg_chunks->count = 0;

    /* Act */
    CF_Chunks_Init(arg_chunks, arg_CF_max_chunks, arg_chunks_mem);
    
    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->CF_max_chunks, arg_CF_max_chunks);
    UtAssert_ADDRESS_EQ(arg_chunks->chunks, arg_chunks_mem);
    /* Assert Unstubbable - CF_ChunksReset */
    UtAssert_ZERO(arg_chunks->count);
    UtAssert_MemCmpValue(arg_chunks->chunks, 0x00, sizeof(*arg_chunks->chunks) * arg_CF_max_chunks,
      "The chunks, %u bytes (sizeof(*chunks->chunks)*chunks->CF_max_chunks), were all set to 0", sizeof(chunk_t)*arg_CF_max_chunks);
} /* end Test_CF_Chunks_Init_SetsGiven_chunks_CF_max_chunks_ToGiven_CF_max_chunks */

/* CF_Chunks_Init tests */

/*******************************************************************************
**
**  CF_ChunksReset tests
**
*******************************************************************************/

void Test_CF_ChunksReset_Sets_count_To_0_Keeps_CF_max_chunks_AndMemsets_chunks_ToAll_0(void)
{
    /* Arrange */
    chunks_t        dummy_chunks;
    chunks_t*       arg_chunks = &dummy_chunks;
    index_t         initial_CF_max_chunks = Any_uint16() + 2; /* 2-65537, uint8 is used instead of index_t to have a reasonably decent size for the test without being too large (segfault) */
    chunk_t         dummy_chunks_chunks[initial_CF_max_chunks];
    const chunk_t*  local_result;

    arg_chunks->count = Any_index_t();
    arg_chunks->CF_max_chunks = initial_CF_max_chunks;
    arg_chunks->chunks = dummy_chunks_chunks;

    /* Act */
    CF_ChunksReset(arg_chunks);
    
    /* Assert */
    UtAssert_ZERO(arg_chunks->count);
    UtAssert_UINT32_EQ(arg_chunks->CF_max_chunks, initial_CF_max_chunks);
    UtAssert_MemCmpValue(arg_chunks->chunks, 0x00, sizeof(chunk_t)*initial_CF_max_chunks,
      "The chunks, %u bytes (sizeof(chunk_t)*chunks->CF_max_chunks), were all set to 0", sizeof(chunk_t)*initial_CF_max_chunks);
} /* end Test_CF_ChunksReset_Sets_count_To_0_Keeps_CF_max_chunks_AndMemsets_chunks_ToAll_0 */

/* CF_ChunksReset tests */

/*******************************************************************************
**
**  CF_Chunks_ComputeGaps tests (large) - Full coverage - TODO: This is not a great example of good unit testing.  In the quest for speed and full coverage, liberties with values were taken to find ones that achieved the desired coverage in the CUT.  Values should be more robust and be more indicative of the kind of values that will actually be encountered during operation.  Not enough time could be spent on deciphering intent and the creator of that intent is no longer available.
**
*******************************************************************************/

void Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0(void)
{
    /* Arrange */
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = 0;
    chunk_offset_t    arg_start = Any_uint32();
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;

    /* Act */
    // CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0 */

void Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total(void)
{
    /* Arrange */
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX - 1) + 1; /* from 1 to (UINT32_MAX - 1) */
    chunk_offset_t    arg_start = Any_uint32_GreaterThan(arg_total);
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;

    /* Act */
    // CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total */

void Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsEqToGiven_total(void)
{
    /* Arrange */
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    chunk_offset_t    arg_start = arg_total;
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;

    /* Act */
    // CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_Return_1(void)
{
    /* Arrange */
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    chunk_offset_t    arg_start = arg_total - 1;
    compute_gap_fn_t  arg_compute_gap_fn = NULL;
    void*             arg_opaque = ut_default_ptr;

    arg_chunks->count = 0;

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 0);
    UtAssert_UINT32_EQ(result, 1);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_Return_1 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1(void)
{
    /* Arrange */
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    chunk_offset_t    arg_start = Any_uint32_LessThan(arg_total);
    compute_gap_fn_t  arg_compute_gap_fn = NULL;
    void*             arg_opaque = ut_default_ptr;

    arg_chunks->count = 0;

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 0);
    UtAssert_UINT32_EQ(result, 1);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_ThenReturn_1(void)
{
    /* Arrange */
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    chunk_offset_t    arg_start = Any_uint32_LessThan(arg_total);
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 0;

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 1);
    UtAssert_ADDRESS_EQ(context_Dummy_compute_gap_fn_t.cs, arg_chunks);
    UtPrintf("Stub count check is destroying the memory at context_Dummy_compute_gap_fn_t.c (ha ha, not a filename, 'c' is the variable name)");
    // TODO: values pointed to by contexts are out of scope and this is now an issue because any get stub count is killing access to values set by CUT and given to stubs via pointer (it may actually be in UT_GetStubEntry used by getting the stub count)
    // UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.c->offset, 0);
    // UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.c->size, arg_total);
    UtAssert_ADDRESS_EQ(context_Dummy_compute_gap_fn_t.opaque, arg_opaque);
    UtAssert_UINT32_EQ(result, 1);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_ThenReturn_1 */

void Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0(void)
{
    /* Arrange */
    chunks_t          dummy_chunks_chunks[1] = {0};
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = 0;
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    chunk_offset_t    arg_start = Any_uint32_LessThan(arg_total);
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 1;

    dummy_chunks.chunks = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint32_LessThan(arg_total);
    dummy_chunks.chunks[0].size = arg_total - dummy_chunks.chunks[0].offset;

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 0);
    UtAssert_UINT32_EQ(result, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0(void)
{
    /* Arrange */
    chunks_t          dummy_chunks_chunks[1] = {0};
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    chunk_offset_t    arg_start = Any_uint32_LessThan(arg_total);
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 1;
    
    dummy_chunks.chunks = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint32_LessThan(arg_total);
    dummy_chunks.chunks[0].size = arg_total - dummy_chunks.chunks[0].offset;

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 0);
    UtAssert_UINT32_EQ(result, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0(void)
{
    /* Arrange */
    chunks_t          dummy_chunks_chunks[1] = {0};
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = Any_uint32();
    chunk_size_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    chunk_offset_t    arg_start = Any_uint32_LessThan(arg_total);
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;
    chunk_offset_t    dummy_gap_start = Any_uint32_GreaterThan(arg_total);
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 1;
    dummy_chunks.chunks = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint32_LessThan(dummy_gap_start);
    dummy_chunks.chunks[0].size = dummy_gap_start - dummy_chunks.chunks[0].offset;

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 0);
    UtAssert_UINT32_EQ(result, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_Return_1(void)
{
    /* Arrange */
    chunks_t          dummy_chunks_chunks[1] = {0};
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    chunk_size_t      arg_total;
    chunk_offset_t    arg_start = 0;
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 1;

    dummy_chunks.chunks = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint8_GreaterThan(1); /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable size for test, larger may be difficult to handle */
    dummy_chunks.chunks[0].size = Any_uint8_GreaterThan(1); /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable size for test, larger may be difficult to handle */
    
    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + 1; /* + 1 to always be greater */

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 1);
    UtAssert_UINT32_EQ(result, 1);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_Return_1 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_NULL_Return_1(void)
{
    /* Arrange */
    chunks_t          dummy_chunks_chunks[1] = {0};
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    chunk_size_t      arg_total;
    chunk_offset_t    arg_start;
    compute_gap_fn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void*             arg_opaque = ut_default_ptr;
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 1;
    
    dummy_chunks.chunks = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint8_GreaterThan(1); /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable size for test, larger may be difficult to handle */
    dummy_chunks.chunks[0].size = Any_uint8_GreaterThan(1); /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable size for test, larger may be difficult to handle */
    
    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + 1; /* + 1 to always be greater */
    
    arg_start = arg_total - 1; /* - 1 for less than total, but forces c.offset+c.size)>=start to be true */ 

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 1);
    UtAssert_UINT32_EQ(result, 1);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_NULL_Return_1 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1(void)
{
    /* Arrange */
    chunks_t          dummy_chunks_chunks[2] = {0, 0};
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    chunk_size_t      arg_total;
    chunk_offset_t    arg_start;
    compute_gap_fn_t  arg_compute_gap_fn = NULL;
    void*             arg_opaque = ut_default_ptr;
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 2;
    
    dummy_chunks.chunks = dummy_chunks_chunks;
    dummy_chunks.chunks[1].offset = 1; /* 1 arbitrarily used for offset 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[1].size = 1; /* 1 arbitrarily used for size 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].offset = 1; /* 1 arbitrarily used for offset 0 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].size = 1; /* 1 arbitrarily used for size 0 because it exhibits desired coverage in CUT */
    
    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + 2; /* + 2 to always be greater */
    
    arg_start = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + 1; /* + 1 for less than total, but forces c.offset+c.size)>=start to be false */ 

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_UINT32_EQ(result, 1);
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButASubsequentLoop_started_Is_1_And_compute_gap_fn_Is_NULL_ReturnNumberOfLoops(void)
{
    /* Arrange */
    chunks_t          dummy_chunks_chunks[3] = {0, 0, 0};
    chunks_t          dummy_chunks;
    chunks_t*         arg_chunks = &dummy_chunks;
    index_t           arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    chunk_size_t      arg_total;
    chunk_offset_t    arg_start;
    compute_gap_fn_t  arg_compute_gap_fn = NULL;
    void*             arg_opaque = ut_default_ptr;
    Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

    arg_chunks->count = 3;
    
    dummy_chunks.chunks = dummy_chunks_chunks;
    dummy_chunks.chunks[2].offset = 5; /* 5 arbitrarily used for offset 2 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[2].size = 5; /* 5 arbitrarily used for size 2 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[1].offset = 1; /* 1 arbitrarily used for offset 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[1].size = 1; /* 1 arbitrarily used for size 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].offset = 1; /* 1 arbitrarily used for offset 0 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].size = 1; /* 1 arbitrarily used for size 0 because it exhibits desired coverage in CUT */

    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + dummy_chunks.chunks[1].offset + dummy_chunks.chunks[1].size + dummy_chunks.chunks[2].offset + dummy_chunks.chunks[2].size + 2; /* + 2 to always be greater */
    
    arg_start = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + dummy_chunks.chunks[1].offset + dummy_chunks.chunks[1].size + 1; /* forces c.offset+c.size)>=start to be false */ 

    UT_SetHookFunction(UT_KEY(Dummy_compute_gap_fn_t), stub_reporter_hook, &context_Dummy_compute_gap_fn_t);

    /* Act */
    result = CF_Chunks_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);
    
    /* Assert */
    UtAssert_UINT32_EQ(result, 2);
    UtAssert_STUB_COUNT(Dummy_compute_gap_fn_t, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1 */

/* end CF_Chunks_ComputeGaps tests */

/*******************************************************************************
**
**  Test Add functions
**
**  TODO: Tests commented out are edge case CF_Assert fail tests that are not
**        required to mark the fail because there is another test doing that.
**        These tests should be used if the CF_Assert issue is solved.
**
*******************************************************************************/

void add_macro_tests(void)
{
    UtTest_Add(Test_MAX_Returns_a_WhenItIsMaxValue, 
       cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
       "Test_MAX_Returns_a_WhenItIsMaxValue");     
    UtTest_Add(Test_MAX_Returns_b_WhenItIsMaxValue, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_MAX_Returns_b_WhenItIsMaxValue");     
    UtTest_Add(Test_MAX_Returns_a_WhenItIsEqualTo_b, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_MAX_Returns_a_WhenItIsEqualTo_b");
} /* end add_macro_tests */

void add_CF_Chunks_EraseRange_tests(void)
{
    UtTest_Add(Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start");    
    // UtTest_Add(Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start, 
    //   cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
    //   "Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start");
    UtTest_Add(Test_CF_Chunks_EraseRange_When_start_Is_OneMoreThan_end_MovesOneChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseRange_When_start_Is_OneMoreThan_end_MovesOneChunk"); 
    UtTest_Add(Test_CF_Chunks_EraseRange_When_start_IsLessThan_chunks_count_DoesNotChangeStart, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseRange_When_start_IsLessThan_chunks_count_DoesNotChangeStart");    
} /* end add_CF_Chunks_EraseRange_tests */

void add_CF_Chunks_EraseChunk_tests(void)
{
    UtTest_Add(Test_CF_Chunks_EraseChunk_FailsAssert_Given_chunks_count_IsEqTo_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseChunk_FailsAssert_Given_chunks_count_IsEqTo_0");
    UtTest_Add(Test_CF_Chunks_EraseChunk_FailsAssert_Given_erase_index_IsEqTo_chunks_count, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseChunk_FailsAssert_Given_erase_index_IsEqTo_chunks_count");
    UtTest_Add(Test_CF_Chunks_EraseChunk_ErasesOneChunkWhenThereIsOnlyOneAndDecrements_count_To_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseChunk_ErasesOneChunkWhenThereIsOnlyOneAndDecrements_count_To_0");
    UtTest_Add(Test_CF_Chunks_EraseChunk_ErasesOneChunkThatIsNotTheLastFrom_chunks_AndDecrements_count, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseChunk_ErasesOneChunkThatIsNotTheLastFrom_chunks_AndDecrements_count");
    UtTest_Add(Test_CF_Chunks_EraseChunk_ErasesLastChunkFrom_chunks_AndDecrements_count, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_EraseChunk_ErasesLastChunkFrom_chunks_AndDecrements_count");
} /* end add_CF_Chunks_EraseChunk_tests */

void add_CF_Chunks_InsertChunk_tests(void)
{
    UtTest_Add(Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsEqTo_chunks_CF_max_chunks, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsEqTo_chunks_CF_max_chunks");
    // UtTest_Add(Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsGreaterThan_chunks_CF_max_chunks, 
    //   cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
    //   "Test_CF_Chunks_InsertChunk_FailsAssert_Given_chunks_count_IsGreaterThan_chunks_CF_max_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEmpty_chunks, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEmpty_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoStartOfSingle_chunks, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoStartOfSingle_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEndOfSingle_chunks, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoEndOfSingle_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoSome_chunks, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_InsertChunk_PutsGiven_chunk_IntoSome_chunks");
} /* end add_CF_Chunks_InsertChunk_tests */

void add_CF_Chunks_FindInsertPosition_tests(void)
{
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_IsEmptyReturns_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindInsertPosition_Given_chunks_IsEmptyReturns_0");
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturns_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturns_0");
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturns_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturns_0");
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturns_1, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturns_1");
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturns_1, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturns_1");
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnsCorrectExpectedPosition, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnsCorrectExpectedPosition");
} /* end add_CF_Chunks_FindInsertPosition_tests */



void add_CF_Chunks_CombinePrevious_tests(void)
{
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks");
    // UtTest_Add(Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks, 
    //   cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
    //   "Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_offset_Return_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_offset_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButEqTo_chunk_end_Return_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButEqTo_chunk_end_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1");
} /* end add_CF_Chunks_CombinePrevious_tests */

void add_CF_Chunks_CombineNext_tests(void)
{
    UtTest_Add(Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Returns_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Returns_0");
    UtTest_Add(Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_chunk_offset, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_chunk_offset");
    UtTest_Add(Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_chunk_end_Returns_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_chunk_end_Returns_0");
    UtTest_Add(Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_CombinesChunksNotAtEndDoesNotEraseRangeReturns_1, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_CombinesChunksNotAtEndDoesNotEraseRangeReturns_1");
    UtTest_Add(Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1");
    UtTest_Add(Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplacesEverythingUntilEndAndCalls_CF_Chunks_EraseRange_ThenReturns_1");
} /* end add_CF_Chunks_CombineNext_tests */

void add_CF_Chunks_FindSmallestSize_tests(void)
{
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_IsEmptyReturns_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindSmallestSize_Given_chunks_IsEmptyReturns_0");
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturns_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturns_0");
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest");
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_HasManyChunksReturnsCorrectIndexOfSmallest, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_FindSmallestSize_Given_chunks_HasManyChunksReturnsCorrectIndexOfSmallest");
} /* end add_CF_Chunks_FindSmallestSize_tests */

void add_CF_Chunks_Insert_tests(void)
{
    UtTest_Add(Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_DoNothing, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_DoNothing");
    UtTest_Add(Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk");
    UtTest_Add(Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_DoNothing, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_DoNothing");
    UtTest_Add(Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk");
    UtTest_Add(Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing");
    UtTest_Add(Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_And_CF_Chunks_InsertChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_CF_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_And_CF_Chunks_InsertChunk");
} /* end add_CF_Chunks_Insert_tests */

void add_CF_Chunks_Add_tests(void)
{
    UtTest_Add(Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset");
    UtTest_Add(Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_IsNotLessThanOrEqToGiven_chunks_i_offset, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_IsNotLessThanOrEqToGiven_chunks_i_offset");
    UtTest_Add(Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenNotAtEnd, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenNotAtEnd");
    UtTest_Add(Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenAtEnd, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Add_Calls_CF_Chunks_Insert_WhenAtEnd");
} /* end add_CF_Chunks_Add_tests */

void add_CF_Chunks_RemoveFromFirst_tests(void)
{
    UtTest_Add(Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Calls_CF_Chunks_EraseChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Calls_CF_Chunks_EraseChunk");
    UtTest_Add(Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Calls_CF_Chunks_EraseChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Calls_CF_Chunks_EraseChunk");
    UtTest_Add(Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddsGiven_size_To_chunks_chunks_0_offset_AndSets_chunks_chunks_size_To_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddsGiven_size_To_chunks_chunks_0_offset_AndSets_chunks_chunks_size_To_0");
} /* end add_CF_Chunks_RemoveFromFirst_tests */

void add_CF_Chunks_GetFirstChunk_tests(void)
{
    UtTest_Add(Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Returns_NULL, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Returns_NULL");
    UtTest_Add(Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnsOnlyChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnsOnlyChunk");
    UtTest_Add(Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_tReturnsFirstChunk, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_tReturnsFirstChunk");
} /* end add_CF_Chunks_GetFirstChunk_tests */

void add_CF_Chunks_Init_tests(void)
{
    UtTest_Add(Test_CF_Chunks_Init_AssertsBecauseGiven_CF_max_chunks_Is_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Init_AssertsBecauseGiven_CF_max_chunks_Is_0");
    UtTest_Add(Test_CF_Chunks_Init_SetsGiven_chunks_CF_max_chunks_ToGiven_CF_max_chunks, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_Init_SetsGiven_chunks_CF_max_chunks_ToGiven_CF_max_chunks");
} /* end add_CF_Chunks_Init_tests */

void add_CF_ChunksReset_tests(void)
{
    UtTest_Add(Test_CF_ChunksReset_Sets_count_To_0_Keeps_CF_max_chunks_AndMemsets_chunks_ToAll_0, 
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_ChunksReset_Sets_count_To_0_Keeps_CF_max_chunks_AndMemsets_chunks_ToAll_0");
} /* end add_CF_ChunksReset_tests */

void add_CF_Chunks_ComputeGaps_tests(void)
{
    UtTest_Add(Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsEqToGiven_total,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsEqToGiven_total");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_Return_1,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_Return_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_ThenReturn_1,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_ThenReturn_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_Return_1,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_Return_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_NULL_Return_1,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_NULL_Return_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButASubsequentLoop_started_Is_1_And_compute_gap_fn_Is_NULL_ReturnNumberOfLoops,
      cf_chunk_tests_Setup, cf_chunk_tests_Teardown, 
      "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButASubsequentLoop_started_Is_1_And_compute_gap_fn_Is_NULL_ReturnNumberOfLoops");
} /* end add_CF_Chunks_ComputeGaps_tests */

/*******************************************************************************
**
**  cf_chunk_tests UtTest_Setup
**
*******************************************************************************/


void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();
    
    add_macro_tests();

    add_CF_Chunks_EraseRange_tests();

    add_CF_Chunks_EraseChunk_tests();
     
    add_CF_Chunks_InsertChunk_tests();

    add_CF_Chunks_FindInsertPosition_tests();

    add_CF_Chunks_CombinePrevious_tests();

    add_CF_Chunks_CombineNext_tests();

    add_CF_Chunks_FindSmallestSize_tests();

    add_CF_Chunks_Insert_tests();

    add_CF_Chunks_Add_tests();

    add_CF_Chunks_RemoveFromFirst_tests();

    add_CF_Chunks_GetFirstChunk_tests();

    add_CF_Chunks_Init_tests();

    add_CF_ChunksReset_tests();
    
    add_CF_Chunks_ComputeGaps_tests();
    
} /* end UtTest_Setup for cf_chunk_tests.c */


/* end cf_chunk_tests.c */