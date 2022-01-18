/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_chunk.h"

typedef struct
{
    uint32                count;
    const CF_ChunkList_t *cs;
    const CF_Chunk_t     *c;
    void                 *opaque;
} Dummy_compute_gap_fn_t_context_t;

Dummy_compute_gap_fn_t_context_t context_Dummy_compute_gap_fn_t;

/*******************************************************************************
**
**  cf_chunk_tests Setup and Teardown
**
*******************************************************************************/

void cf_chunk_tests_Setup(void)
{
    cf_tests_Setup();

    memset(&context_Dummy_compute_gap_fn_t, 0, sizeof(context_Dummy_compute_gap_fn_t));

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

CF_ChunkIdx_t Any_index_t(void)
{
    return (CF_ChunkIdx_t)Any_uint32();
}

void Dummy_compute_gap_fn_t(const CF_ChunkList_t *cs, const CF_Chunk_t *c, void *opaque)
{
    ++context_Dummy_compute_gap_fn_t.count;
    context_Dummy_compute_gap_fn_t.cs     = cs;
    context_Dummy_compute_gap_fn_t.c      = c;
    context_Dummy_compute_gap_fn_t.opaque = opaque;
}

/*******************************************************************************
**
**  macro tests
**
*******************************************************************************/

void Test_MAX_WhenItIsMaxValue_Return_a(void)
{
    UtAssert_UINT32_EQ(CF_Chunk_MAX(5555, 4444), 5555);
} /* end Test_MAX_WhenItIsMaxValue_Return_a */

void Test_MAX_WhenItIsMaxValue_Return_b(void)
{
    UtAssert_UINT32_EQ(CF_Chunk_MAX(5555, 6666), 6666);
} /* end Test_MAX_WhenItIsMaxValue_Return_b */

void Test_MAX_WhenItIsEqualTo_b_Returns_a(void)
{
    UtAssert_UINT32_EQ(CF_Chunk_MAX(7777, 7777), 7777);
} /* end Test_MAX_WhenItIsEqualTo_b_Returns_a */

/* end macro tests */

/*******************************************************************************
**
**  CF_Chunks_EraseRange tests
**
*******************************************************************************/

/* NOTE: memmove could be stubbed, but proving problematic.
 * CF_Chunks_EraseRange tests will use the memmove call for now */

void Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start(void)
{
    // /* Arrange */
    // CF_ChunkList_t      dummy_chunks;
    // CF_ChunkList_t*     arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t       arg_start = Any_uint32_Except(UINT32_MAX);
    // CF_ChunkIdx_t       arg_end = Any_uint32_LessThan(arg_start);

    // /* Act */
    // CF_Chunks_EraseRange(&Test_chunks, Test_start, Test_end);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - end>=start");
} /* end Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start */

/* NOTE: Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start not required but desired */
// void Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start(void)
// {
//     /* Arrange */
//     CF_ChunkList_t      dummy_chunks;
//     CF_ChunkList_t*     arg_chunks = &dummy_chunks;
//     CF_ChunkIdx_t       arg_start = Any_uint32_Except(UINT32_MAX);
//     CF_ChunkIdx_t       arg_end = arg_start;

//     /* Act */
//     //CF_Chunks_EraseRange(&Test_chunks, Test_start, Test_end);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - end>=start");
// } /* end Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start */

void Test_CF_Chunks_When_start_Is_OneMoreThan_end_MovesOneChunk_EraseRange(void)
{
    /* Arrange */
    CF_Chunk_t       dummy_chunk_array[2];
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t    arg_start          = 0;
    CF_ChunkIdx_t    arg_end            = 1;
    CF_ChunkOffset_t dummy_start_offset = Any_uint32();
    CF_ChunkSize_t   dummy_start_size   = Any_uint32();
    CF_ChunkOffset_t dummy_end_offset   = Any_uint32_Except(dummy_start_offset);
    CF_ChunkSize_t   dummy_end_size     = Any_uint32_Except(dummy_start_size);

    arg_chunks->count  = 2;
    arg_chunks->chunks = dummy_chunk_array;

    arg_chunks->chunks[0].offset = dummy_start_offset;
    arg_chunks->chunks[0].size   = dummy_start_size;
    arg_chunks->chunks[1].offset = dummy_end_offset;
    arg_chunks->chunks[1].size   = dummy_end_size;

    /* Act */
    CF_Chunks_EraseRange(arg_chunks, arg_start, arg_end);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, dummy_end_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_end_size);
} /* end Test_CF_Chunks_When_start_Is_OneMoreThan_end_MovesOneChunk_EraseRange */

void Test_CF_Chunks_When_start_IsLessThan_chunks_count_DoesNotChangeStart_EraseRange(void)
{
    /* Arrange */
    CF_Chunk_t       dummy_chunk_array[2];
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t    arg_start          = 2;
    CF_ChunkIdx_t    arg_end            = 3;
    CF_ChunkOffset_t dummy_start_offset = Any_uint32();
    CF_ChunkSize_t   dummy_start_size   = Any_uint32();
    CF_ChunkOffset_t dummy_end_offset   = Any_uint32_Except(dummy_start_offset);
    CF_ChunkSize_t   dummy_end_size     = Any_uint32_Except(dummy_start_size);

    arg_chunks->count  = 2;
    arg_chunks->chunks = dummy_chunk_array;

    arg_chunks->chunks[0].offset = dummy_start_offset;
    arg_chunks->chunks[0].size   = dummy_start_size;
    arg_chunks->chunks[1].offset = dummy_end_offset;
    arg_chunks->chunks[1].size   = dummy_end_size;

    /* Act */
    CF_Chunks_EraseRange(arg_chunks, arg_start, arg_end);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, dummy_start_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_start_size);
} /* end Test_CF_Chunks_When_start_IsLessThan_chunks_count_DoesNotChangeStart_EraseRange */

/* end CF_Chunks_EraseRange tests */

/*******************************************************************************
**
**  CF_Chunks_EraseChunk tests
**
*******************************************************************************/

/* NOTE: memmove could be stubbed, but proving problematic.
 * CF_Chunks_EraseChunk tests will use the memmove call for now */

void Test_CF_Chunks_EraseChunk_AssertsBecause_Given_chunks_count_IsEqTo_0(void)
{
    // /* Arrange */
    // CF_ChunkList_t      dummy_chunks;
    // CF_ChunkList_t*     arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t       arg_erase_index = Any_index_t();

    // arg_chunks->count = 0;

    // /* Act */
    // CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_EraseChunk_AssertsBecause_Given_chunks_count_IsEqTo_0 */

void Test_CF_Chunks_EraseChunk_AssertsBecause_Given_erase_index_IsEqTo_chunks_count(void)
{
    // /* Arrange */
    // CF_ChunkList_t      dummy_chunks;
    // CF_ChunkList_t*     arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t       arg_erase_index;

    // arg_chunks->count = 1;

    // arg_erase_index = 2;

    // /* Act */
    // CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_EraseChunk_AssertsBecause_Given_erase_index_IsEqTo_chunks_count */

void Test_CF_Chunks_EraseChunk_WhenThereIsOnlyOneEraseOneChunkAndDecrement_count_To_0(void)
{
    /* Arrange */
    CF_Chunk_t      dummy_start_chunk[1];
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_erase_index;

    arg_chunks->count  = 1;
    arg_chunks->chunks = dummy_start_chunk;

    arg_erase_index = 0;

    /* Act */
    CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, 0);
} /* end Test_CF_Chunks_EraseChunk_WhenThereIsOnlyOneEraseOneChunkAndDecrement_count_To_0 */

void Test_CF_Chunks_EraseChunk_EraseOneChunkThatIsNotTheLastFrom_chunks_AndDecrement_count(void)
{
    /* Arrange */
    uint8 intial_count =
        Any_uint8_LessThan(10) + 2; /* 2-11, reasonably small count for testing and not its normal uint32 */
    CF_Chunk_t       dummy_chunk[11] = {{0}};
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t    arg_erase_index;
    uint8            i                   = 0;
    CF_ChunkOffset_t offsets[11]         = {0};
    CF_ChunkSize_t   sizes[11]           = {0};
    CF_ChunkOffset_t updated_offsets[10] = {0};
    CF_ChunkSize_t   updated_sizes[10]   = {0};

    arg_chunks->count  = intial_count;
    arg_chunks->chunks = dummy_chunk;

    arg_erase_index = Any_uint8_BetweenExcludeMax(0, intial_count - 1); /* -1 ensures not the last chunk */

    /* save chunk information by position for assertions later */
    for (i = 0; i < intial_count; ++i)
    {
        dummy_chunk[i].offset = Any_uint32();
        dummy_chunk[i].size   = Any_uint32();

        /* save all offsets and sizes */
        offsets[i] = dummy_chunk[i].offset;
        sizes[i]   = dummy_chunk[i].size;

        /* chunk past erased will take the place of erase_index, etc. */
        if (i >= arg_erase_index + 1)
        {
            updated_offsets[i - arg_erase_index - 1] = dummy_chunk[i].offset;
            updated_sizes[i - arg_erase_index - 1]   = dummy_chunk[i].size;
        }
    }

    /* Act */
    CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, intial_count - 1);
    /* NOTE: EXAMPLE: These for loops' output is probably a good indication of why these base outputs do not provide
     * enough info */
    for (i = 0; i < arg_erase_index; ++i)
    {
        UtAssert_UINT32_EQ(dummy_chunk[i].offset,
                           offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(dummy_chunk[i].size,
                           sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */
    }

    for (i = 0; i < (intial_count - arg_erase_index - 1); ++i)
    {
        UtAssert_UINT32_EQ(dummy_chunk[i + arg_erase_index].offset,
                           updated_offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(dummy_chunk[i + arg_erase_index].size,
                           updated_sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */
    }

} /* end Test_CF_Chunks_EraseChunk_EraseOneChunkThatIsNotTheLastFrom_chunks_AndDecrement_count */

void Test_CF_Chunks_EraseChunk_ErasesLastChunkFrom_chunks_AndDecrements_count(void)
{
    /* Arrange */
    uint8 initial_count =
        Any_uint8_LessThan(10) + 2; /* 2-11, reasonably small count for testing and not its normal uint32 */
    CF_Chunk_t       dummy_chunk[11] = {{0}};
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks      = &dummy_chunks;
    CF_ChunkIdx_t    arg_erase_index = initial_count - 1;
    uint8            i               = 0;
    CF_ChunkOffset_t offsets[11]     = {0};
    CF_ChunkSize_t   sizes[11]       = {0};

    arg_chunks->count  = initial_count;
    arg_chunks->chunks = dummy_chunk;

    /* save chunk information by position for assertions later */
    for (i = 0; i < initial_count; ++i)
    {
        dummy_chunk[i].offset = Any_uint32();
        dummy_chunk[i].size   = Any_uint32();

        /* save all offsets and sizes */
        offsets[i] = dummy_chunk[i].offset;
        sizes[i]   = dummy_chunk[i].size;
    }

    /* Act */
    CF_Chunks_EraseChunk(arg_chunks, arg_erase_index);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, initial_count - 1);
    /* NOTE: EXAMPLE: These for loops' output is probably a good indication of why these base outputs do not provide
     * enough info */
    for (i = 0; i < arg_erase_index; ++i)
    {
        UtAssert_UINT32_EQ(dummy_chunk[i].offset,
                           offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(dummy_chunk[i].size,
                           sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */
    }

    for (i = 0; i < (initial_count - arg_erase_index - 1); ++i)
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

/* NOTE: memmove should be wrapped for stubbing, but proving problematic.
 * CF_Chunks_InsertChunk tests will use the memmove call for now */

void Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsEqTo_chunks_max_chunks(void)
{
    // /* Arrange */
    // CF_ChunkList_t      dummy_chunks;
    // CF_ChunkList_t*     arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t       arg_index_before = Any_index_t();
    // CF_Chunk_t       arg_chunk = {0};

    // arg_chunks->count = Any_uint32();
    // arg_chunks->max_chunks = arg_chunks->count;

    // /* Act */
    // CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsEqTo_chunks_max_chunks */

/* NOTE: Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsGreaterThan_chunks_max_chunks not required
 * but desired */
// void Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsGreaterThan_chunks_max_chunks(void)
// {
//     /* Arrange */
//     CF_ChunkList_t      dummy_chunks;
//     CF_ChunkList_t*     arg_chunks = &dummy_chunks;
//     CF_ChunkIdx_t       arg_index_before = Any_index_t();
//     CF_Chunk_t       dummy_chunk = {0};
//     CF_Chunk_t*      arg_chunk = &dummy_chunk;

//     arg_chunks->count = Any_uint32_Except(UINT32_MAX);
//     arg_chunks->max_chunks = Any_uint32_GreaterThan(arg_chunks->count);

//     /* Act */
//     //CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsGreaterThan_chunks_max_chunks */

void Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEmpty_chunks(void)
{
    /* Arrange */
    CF_Chunk_t        initial_chunks[1];
    CF_ChunkList_t    dummy_chunks;
    CF_ChunkList_t   *arg_chunks       = &dummy_chunks;
    CF_ChunkIdx_t     arg_index_before = 0;
    CF_Chunk_t        dummy_chunk      = {0};
    const CF_Chunk_t *arg_chunk;

    arg_chunks->count = 0;
    arg_chunks->max_chunks =
        UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size   = Any_uint32();

    arg_chunk = &dummy_chunk;

    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset,
                       dummy_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size,
                       dummy_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->count, 1);
} /* end Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEmpty_chunks */

void Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoStartOfSingle_chunks(void)
{
    /* Arrange */
    CF_Chunk_t        initial_start_chunk;
    CF_Chunk_t        initial_chunks[2] = {{0}};
    CF_ChunkList_t    dummy_chunks;
    CF_ChunkList_t   *arg_chunks       = &dummy_chunks;
    CF_ChunkIdx_t     arg_index_before = 0;
    CF_Chunk_t        dummy_chunk      = {0};
    const CF_Chunk_t *arg_chunk;

    initial_start_chunk.offset = Any_uint32();
    initial_start_chunk.size   = Any_uint32();
    initial_chunks[0]          = initial_start_chunk;

    arg_chunks->count = 1;
    arg_chunks->max_chunks =
        UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size   = Any_uint32();

    arg_chunk = &dummy_chunk;

    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset,
                       dummy_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size,
                       dummy_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].offset,
                       initial_start_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].size,
                       initial_start_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->count, 2);
} /* end Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoStartOfSingle_chunks */

void Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEndOfSingle_chunks(void)
{
    /* Arrange */
    CF_Chunk_t        initial_start_chunk;
    CF_Chunk_t        initial_chunks[2] = {{0}};
    CF_ChunkList_t    dummy_chunks;
    CF_ChunkList_t   *arg_chunks       = &dummy_chunks;
    CF_ChunkIdx_t     arg_index_before = 1;
    CF_Chunk_t        dummy_chunk      = {0};
    const CF_Chunk_t *arg_chunk;

    initial_start_chunk.offset = Any_uint32();
    initial_start_chunk.size   = Any_uint32();
    initial_chunks[0]          = initial_start_chunk;

    arg_chunks->count = 1;
    arg_chunks->max_chunks =
        UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size   = Any_uint32();

    arg_chunk = &dummy_chunk;

    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset,
                       initial_start_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size,
                       initial_start_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].offset,
                       dummy_chunk.offset); /* NOTE: using UINT32_EQ because that is offset's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->chunks[1].size,
                       dummy_chunk.size); /* NOTE: using UINT32_EQ because that is size's underlying type */
    UtAssert_UINT32_EQ(arg_chunks->count, 2);
} /* end Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEndOfSingle_chunks */

void Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoSome_chunks(void)
{
    /* Arrange */
    CF_ChunkIdx_t initial_count =
        Any_uint8_LessThan(10) + 1; /* 1-10, reasonably small count for testing and not its normal uint32 */
    CF_Chunk_t        initial_chunks[11] = {{0}};
    CF_ChunkList_t    dummy_chunks;
    CF_ChunkList_t   *arg_chunks       = &dummy_chunks;
    CF_ChunkIdx_t     arg_index_before = 0;
    CF_Chunk_t        dummy_chunk      = {0};
    const CF_Chunk_t *arg_chunk;
    CF_ChunkOffset_t  updated_offsets[10] = {0};
    CF_ChunkSize_t    updated_sizes[10]   = {0};
    uint8             i;

    for (i = 0; i < initial_count; ++i)
    {
        uint8 j = i;

        if (i >= arg_index_before)
        {
            ++j;
        }

        initial_chunks[i].offset = Any_uint32();
        initial_chunks[i].size   = Any_uint32();

        /* save all offsets and sizes */
        updated_offsets[j] = initial_chunks[i].offset;
        updated_sizes[j]   = initial_chunks[i].size;
    }

    arg_chunks->count = initial_count;
    arg_chunks->max_chunks =
        UINT32_MAX; /* UINT32_MAX maybe unresonable but keeps it out of the way with the CF_Assert */
    arg_chunks->chunks = initial_chunks;

    dummy_chunk.offset = Any_uint32();
    dummy_chunk.size   = Any_uint32();
    updated_offsets[0] = dummy_chunk.offset;
    updated_sizes[0]   = dummy_chunk.size;

    arg_chunk = &dummy_chunk;

    /* Act */
    CF_Chunks_InsertChunk(arg_chunks, arg_index_before, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, initial_count + 1);

    for (i = 0; i < initial_count + 1; ++i)
    {
        UtAssert_UINT32_EQ(arg_chunks->chunks[i].offset,
                           updated_offsets[i]); /* NOTE: using UINT32_EQ because that is offset's underlying type */
        UtAssert_UINT32_EQ(arg_chunks->chunks[i].size,
                           updated_sizes[i]); /* NOTE: using UINT32_EQ because that is size's underlying type */
    }
} /* end Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoSome_chunks */

/* end CF_Chunks_InsertChunk tests */

/*******************************************************************************
**
**  CF_Chunks_FindInsertPosition tests
**
*******************************************************************************/

void Test_CF_Chunks_FindInsertPosition_Given_chunks_IsEmptyReturn_0(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks  = &dummy_chunks;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;
    CF_ChunkIdx_t   local_result;

    arg_chunks->count = 0;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_IsEmptyReturn_0 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturn_0(void)
{
    /* Arrange */
    CF_Chunk_t      initial_chunks[1] = {{0}};
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks  = &dummy_chunks;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;
    CF_ChunkIdx_t   local_result;

    initial_chunks[0].offset = Any_uint32_Except(0);

    arg_chunk->offset = Any_uint32_LessThan(initial_chunks[0].offset);

    arg_chunks->count  = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturn_0 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturn_0(void)
{
    /* Arrange */
    CF_Chunk_t      initial_chunks[1] = {{0}};
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks  = &dummy_chunks;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;
    CF_ChunkIdx_t   local_result;

    initial_chunks[0].offset = Any_uint32_Except(0);

    arg_chunk->offset = initial_chunks[0].offset;

    arg_chunks->count  = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturn_0 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturn_1(void)
{
    /* Arrange */
    CF_Chunk_t      initial_chunks[1] = {{0}};
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks  = &dummy_chunks;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;
    CF_ChunkIdx_t   local_result;

    initial_chunks[0].offset = Any_uint32_Except(UINT32_MAX);

    arg_chunk->offset = Any_uint32_GreaterThan(initial_chunks[0].offset);

    arg_chunks->count  = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturn_1 */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturn_1(
    void)
{
    /* Arrange */
    CF_Chunk_t      initial_chunks[2] = {{0}};
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks  = &dummy_chunks;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;
    CF_ChunkIdx_t   local_result;

    initial_chunks[0].offset = Any_uint32_BetweenExcludeMax(0, UINT32_MAX - 1);
    initial_chunks[1].offset = Any_uint32_BetweenInclusive(initial_chunks[0].offset + 2, UINT32_MAX);

    arg_chunk->offset = Any_uint32_BetweenExcludeMax(initial_chunks[0].offset, initial_chunks[1].offset);

    arg_chunks->count  = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturn_1
   */

void Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnCorrectExpectedPosition(void)
{
    /* Arrange */
    CF_Chunk_t initial_chunks[20] = {
        {0}}; /* 20 used as a reasonably fast size for the test, but is still illustrative */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks  = &dummy_chunks;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;
    CF_ChunkIdx_t   local_result;
    uint8           num_chunks_before = Any_uint8_LessThan(10);
    uint8           num_chunks_after  = Any_uint8_LessThan(10);
    uint8           i                 = 0;

    arg_chunk->offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    for (i = 0; i < num_chunks_before; ++i)
    {
        initial_chunks[i].offset = Any_uint32_LessThan(arg_chunk->offset);
    }

    for (i = 0; i < num_chunks_after; ++i)
    {
        initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(arg_chunk->offset);
    }

    arg_chunks->count  = num_chunks_before + num_chunks_after;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindInsertPosition(arg_chunks, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, num_chunks_before);
} /* end Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnCorrectExpectedPosition */

/* end CF_Chunks_FindInsertPosition tests */

/*******************************************************************************
**
**  CF_Chunks_CombinePrevious tests
**
*******************************************************************************/

void Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks(void)
{
    // /* Arrange */
    // CF_ChunkList_t      dummy_chunks;
    // CF_ChunkList_t*     arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t       arg_i;
    // CF_Chunk_t       dummy_chunk = {0};
    // CF_Chunk_t*      arg_chunk = &dummy_chunk;
    // CF_ChunkIdx_t       local_result;

    // arg_chunks->max_chunks = Any_uint32_Except(0);
    // arg_i = arg_chunks->max_chunks;

    // /* Act */
    // local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks */

/* NOTE: Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks not required but desired */
// void Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks(void)
// {
//     /* Arrange */
//     CF_ChunkList_t      dummy_chunks;
//     CF_ChunkList_t*     arg_chunks = &dummy_chunks;
//     CF_ChunkIdx_t       arg_i;
//     CF_Chunk_t       dummy_chunk = {0};
//     CF_Chunk_t*      arg_chunk = &dummy_chunk;
//     CF_ChunkIdx_t       local_result;

//     arg_chunks->max_chunks = Any_uint32_Except(0);
//     arg_i = Any_uint32_GreaterThan(arg_chunks->max_chunks);

//     /* Act */
//     //local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks */

void Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk  = {0};
    CF_Chunk_t     *arg_chunk    = &dummy_chunk;
    int             local_result = Any_int_Except(0);

    arg_chunks->max_chunks = Any_uint32_Except(0);
    arg_i                  = 0;
    arg_chunks->count      = Any_uint32_GreaterThan(0);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0 */

void Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk  = {0};
    CF_Chunk_t     *arg_chunk    = &dummy_chunk;
    int             local_result = Any_int_Except(0);

    arg_chunks->max_chunks = Any_uint32_Except(0);
    arg_i                  = Any_uint32_LessThan(arg_chunks->max_chunks);
    arg_chunks->count      = 0;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0 */

void Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk  = {0};
    CF_Chunk_t     *arg_chunk    = &dummy_chunk;
    int             local_result = Any_int_Except(0);

    arg_chunks->max_chunks = Any_uint32_Except(0);
    arg_i                  = 0;
    arg_chunks->count      = 0;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0 */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_offset_Return_0(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i =
        Any_uint32_LessThan(dummy_chunks_count) +
        1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    CF_Chunk_t       dummy_chunk  = {0};
    CF_Chunk_t      *arg_chunk    = &dummy_chunk;
    CF_ChunkOffset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    CF_ChunkSize_t   dummy_size   = Any_uint32_LessThan(UINT32_MAX / 2);
    int              local_result = Any_int_Except(0);

    arg_chunks->max_chunks               = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count                    = dummy_chunks_count;
    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size   = dummy_size;

    arg_chunk->offset = Any_uint32_GreaterThan(dummy_offset + dummy_size);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end
     Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_offset_Return_0
   */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i =
        Any_uint32_LessThan(dummy_chunks_count) +
        1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    CF_Chunk_t       dummy_chunk  = {0};
    CF_Chunk_t      *arg_chunk    = &dummy_chunk;
    CF_ChunkOffset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    CF_ChunkSize_t   dummy_size   = Any_uint32_LessThan(UINT32_MAX / 2);
    int              local_result = Any_int_Except(0);

    arg_chunks->max_chunks               = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count                    = dummy_chunks_count;
    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size   = dummy_size;

    arg_chunk->offset = Any_uint32_LessThan(dummy_offset + dummy_size + 1);
    arg_chunk->size   = Any_uint32_LessThan(dummy_offset + dummy_size - arg_chunk->offset);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end
     Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0
   */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i =
        Any_uint32_LessThan(dummy_chunks_count) +
        1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    CF_Chunk_t       dummy_chunk  = {0};
    CF_Chunk_t      *arg_chunk    = &dummy_chunk;
    CF_ChunkOffset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    CF_ChunkSize_t   dummy_size   = Any_uint32_LessThan(UINT32_MAX / 2);
    int              local_result = Any_int_Except(0);

    arg_chunks->max_chunks               = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count                    = dummy_chunks_count;
    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size   = dummy_size;

    arg_chunk->offset = Any_uint32_LessThan(dummy_offset + dummy_size + 1);
    arg_chunk->size   = Any_uint32_LessThan(dummy_offset + dummy_size - arg_chunk->offset);

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end
     Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0
   */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButEqTo_chunk_end_Return_0(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i =
        Any_uint32_LessThan(dummy_chunks_count) +
        1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    CF_Chunk_t       dummy_chunk  = {0};
    CF_Chunk_t      *arg_chunk    = &dummy_chunk;
    CF_ChunkOffset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    CF_ChunkSize_t   dummy_size   = Any_uint32_LessThan(UINT32_MAX / 2);
    int              local_result = Any_int_Except(0);

    arg_chunks->max_chunks               = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count                    = dummy_chunks_count;
    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size   = dummy_size;

    arg_chunk->offset = Any_uint32_LessThan(dummy_offset + dummy_size + 1);
    arg_chunk->size   = dummy_offset + dummy_size - arg_chunk->offset;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end
     Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButEqTo_chunk_end_Return_0
   */

void Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i =
        Any_uint32_LessThan(dummy_chunks_count) +
        1; /* arg_i gets +1 because the index is 0 based, but the CUT expects an i of min 1 which is the 0 element */
    CF_Chunk_t       dummy_chunk  = {0};
    CF_Chunk_t      *arg_chunk    = &dummy_chunk;
    CF_ChunkOffset_t dummy_offset = Any_uint32_LessThan(UINT32_MAX / 2);
    CF_ChunkSize_t   initial_size = Any_uint32_LessThan(UINT32_MAX / 2);
    int              local_result = Any_int_Except(0);

    arg_chunks->max_chunks               = Any_uint32_GreaterThan(dummy_chunks_count);
    arg_chunks->count                    = dummy_chunks_count;
    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = dummy_offset;
    arg_chunks->chunks[arg_i - 1].size   = initial_size;

    arg_chunk->offset = 0; /* 0 guarantees that chunk->offset <= prev_end in CUT */
    arg_chunk->size   = dummy_offset + initial_size - arg_chunk->offset + 1;

    /* Act */
    local_result = CF_Chunks_CombinePrevious(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
    UtAssert_UINT32_EQ(arg_chunks->chunks[arg_i - 1].size,
                       arg_chunk->offset + arg_chunk->size - arg_chunks->chunks[arg_i - 1].offset);
} /* end
     Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1
   */

/* end CF_Chunks_CombinePrevious test */

/*******************************************************************************
**
**  CF_Chunks_CombineNext tests
**
*******************************************************************************/

void Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Return_0(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk;
    CF_Chunk_t     *arg_chunk    = &dummy_chunk;
    int             local_result = Any_int_Except(0);

    arg_chunks->count = Any_uint32();
    arg_i             = arg_chunks->count;

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Return_0 */

void Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_chunk_offset(
    void)
{
    // /* Arrange */
    // CF_Chunk_t       dummy_chunks_chunks = {0};
    // CF_ChunkList_t      dummy_chunks;
    // CF_ChunkList_t*     arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t       arg_i;
    // CF_Chunk_t       dummy_chunk = {0};
    // CF_Chunk_t*      arg_chunk = &dummy_chunk;
    // int           local_result = Any_int_Except(0);

    // arg_chunks->count = Any_uint32();
    // arg_i = Any_uint32_Except(arg_chunks->count);

    // /* Act */
    // local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end
     Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_chunk_offset
   */

void Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_chunk_end_Return_0(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk  = {0};
    CF_Chunk_t     *arg_chunk    = &dummy_chunk;
    int             local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i             = Any_uint32_LessThan(arg_chunks->count);

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size   = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks               = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = Any_uint32_GreaterThan(arg_chunk->offset + arg_chunk->size);

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end
     Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_chunk_end_Return_0
   */

void Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_CombinesChunksNotAtEndDoesNotEraseRangeReturn_1(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk  = {0};
    CF_Chunk_t     *arg_chunk    = &dummy_chunk;
    int             local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i             = Any_uint32_LessThan(arg_chunks->count);

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size   = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks               = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = arg_chunk->offset + arg_chunk->size;
    arg_chunks->chunks[arg_i].size   = 1; /* size = 1 moves existing end to greater than chunk_end */

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Chunks_EraseRange, 0);
    UtAssert_UINT32_EQ(local_result, 1);
} /* end
     Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_CombinesChunksNotAtEndDoesNotEraseRangeReturn_1
   */

void Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplaceEverythingUntilEndAndCall_CF_Chunks_EraseRange_ThenReturn_1(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;
    uint32          expected_num_chunks_combined;
    uint8           j            = 0;
    int             local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i =
        Any_uint32_LessThan(arg_chunks->count - 2); /* -2 to argchunks->count to ensure at least 2 chunks till end */

    expected_num_chunks_combined = Any_uint32_BetweenExcludeMax(
        2, dummy_chunks_count - arg_i); /* 2 to 1 less than delta arg_i to dummy_chunks_count */

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size   = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset =
        arg_chunk->offset + arg_chunk->size - expected_num_chunks_combined + 1; /* +1 for itself */

    for (j = 1; j < expected_num_chunks_combined; ++j)
    {
        arg_chunks->chunks[arg_i + j].offset = 1;
    }

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end
     Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplaceEverythingUntilEndAndCall_CF_Chunks_EraseRange_ThenReturn_1
   */

void Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplaceEverythingUntilEndAndCall_CF_Chunks_EraseRange_ThenReturn_1(
    void)
{
    /* Arrange */
    uint8           dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t      dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk  = {0};
    CF_Chunk_t     *arg_chunk    = &dummy_chunk;
    int             local_result = Any_int_Except(0);

    arg_chunks->count = dummy_chunks_count;
    arg_i =
        Any_uint32_LessThan(arg_chunks->count - 1); /* -1 to argchunks->count to ensure at least 2 chunks till end */

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size   = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks               = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = arg_chunk->offset + arg_chunk->size - 1;
    arg_chunks->chunks[arg_i].size   = 1; /* size = 1 moves existing end to greater than chunk_end */

    /* Act */
    local_result = CF_Chunks_CombineNext(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 1);
} /* end
     Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplaceEverythingUntilEndAndCall_CF_Chunks_EraseRange_ThenReturn_1
   */

/* end CF_Chunks_CombineNext tests */

/*******************************************************************************
**
**  CF_Chunks_FindSmallestSize tests
**
*******************************************************************************/

void Test_CF_Chunks_FindSmallestSize_Given_chunks_IsEmptyReturn_0(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   local_result;

    arg_chunks->count = 0;

    /* Act */
    local_result = CF_Chunks_FindSmallestSize(arg_chunks);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindSmallestSize_Given_chunks_IsEmptyReturn_0 */

void Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturn_0(void)
{
    /* Arrange */
    CF_Chunk_t      initial_chunks[1] = {{0}};
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   local_result;

    initial_chunks[0].size = Any_uint32();

    arg_chunks->count  = 1;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindSmallestSize(arg_chunks);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, 0);
} /* end Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturn_0 */

void Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest(void)
{
    /* Arrange */
    CF_Chunk_t      initial_chunks[2] = {{0}};
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;
    CF_ChunkIdx_t   expected_result;
    CF_ChunkIdx_t   local_result;

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

    arg_chunks->count  = 2;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    local_result = CF_Chunks_FindSmallestSize(arg_chunks);

    /* Assert */
    UtAssert_UINT32_EQ(local_result, expected_result);
} /* end Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest */

void Test_CF_Chunks_FindSmallestSize_Given_chunks_HasManyChunksReturnsCorrectIndexOfSmallest(void)
{
    /* Arrange */
    uint8           dummy_count        = 25;
    CF_Chunk_t      initial_chunks[25] = {{0}}; /* 25 for initial_chunks is arbitrary, small for speed */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks      = &dummy_chunks;
    CF_ChunkIdx_t   expected_result = Any_uint8_LessThan(25);
    CF_ChunkIdx_t   local_result;
    uint8           i = 0;

    for (i = 0; i < dummy_count; ++i)
    {
        if (i != expected_result)
        {
            initial_chunks[i].size =
                Any_uint32_GreaterThan(100); /* 100 is arbitrary, a number so we can select a smallest under it */
        }
        else
        {
            initial_chunks[i].size =
                Any_uint32_LessThan(100); /* 100 is arbitrary, a number so we can select a smallest under it */
        }
    }

    arg_chunks->count  = dummy_count;
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

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_DoNothing(
    void)
{
    /* Arrange */
    CF_ChunkList_t *arg_chunks;
    CF_ChunkIdx_t   initial_i;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 1 */
    /* Arrange for CF_Chunks_CombinePrevious to return 0 */
    uint8          dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t     dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t dummy_chunks;

    arg_chunks             = &dummy_chunks;
    arg_chunks->max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    initial_i         = Any_uint32_LessThan(arg_chunks->count);
    arg_i             = initial_i;

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size   = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks               = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i].offset = arg_chunk->offset + arg_chunk->size;
    arg_chunks->chunks[arg_i].size   = 0; /* size = 0 existing end equal chunk_end */

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, initial_i + 1);
} /* end
     Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_DoNothing
   */

void Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk(void)
{
    /* Arrange */
    CF_ChunkList_t *arg_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 1 */
    /* Arrange for CF_Chunks_CombinePrevious to return 1 */
    uint8          dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t     dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t dummy_chunks;

    arg_chunks             = &dummy_chunks;
    arg_chunks->max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    arg_i             = Any_uint32_LessThan(arg_chunks->count - 1) + 1; /* -1 then +1 to ensure at least 1 */

    arg_chunk->offset = Any_uint32_LessThan(UINT32_MAX / 2);
    arg_chunk->size   = Any_uint32_LessThan(UINT32_MAX / 2);

    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = UINT32_MAX - 1;
    arg_chunks->chunks[arg_i].offset     = 0;
    arg_chunks->chunks[arg_i].size       = UINT32_MAX; /* size = 1 moves existing end to greater than chunk_end */

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, dummy_chunks_count - 1);
} /* end Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_DoNothing(
    void)
{
    /* Arrange */
    CF_ChunkList_t *arg_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 1 */
    uint8          dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t     dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t dummy_chunks;

    arg_chunks             = &dummy_chunks;
    arg_chunks->max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    arg_i             = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size   = 6; /* for chunk_end to be 11 = 5 + 6 */

    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 9;
    arg_chunks->chunks[arg_i - 1].size =
        1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10), but prev_end (10) < chunk_end (11) ) */

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    /* Assert for CF_Chunks_InsertChunk */
    UtAssert_UINT32_EQ(arg_chunks->count, dummy_chunks_count);
} /* end
     Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_DoNothing
   */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk(
    void)
{
    /* Arrange */
    CF_ChunkList_t *arg_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 0 */
    CF_ChunkIdx_t  dummy_chunks_count      = 10;    /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t     dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t dummy_chunks;

    arg_chunks             = &dummy_chunks;
    arg_chunks->max_chunks = UINT32_MAX;

    arg_chunks->count = dummy_chunks_count;
    arg_i             = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size   = 5; /* for chunk_end to be 10 = 5 + 5 */

    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 9;
    arg_chunks->chunks[arg_i - 1].size =
        1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10) and  prev_end (10) = chunk_end (10) ) */

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    /* Assert for CF_Chunks_InsertChunk */
    UtAssert_UINT32_EQ(arg_chunks->count, dummy_chunks_count + 1);
} /* end
     Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk
   */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing(
    void)
{
    /* Arrange */
    CF_ChunkList_t *arg_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 0 */
    CF_ChunkIdx_t  dummy_chunks_count      = 3;     /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t     dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t dummy_chunks;

    arg_chunks             = &dummy_chunks;
    arg_chunks->max_chunks = 3;

    arg_chunks->count = dummy_chunks_count;
    arg_i             = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size   = 5; /* for chunk_end to be 10 = 5 + 5 */

    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 4;
    arg_chunks->chunks[arg_i - 1].size =
        arg_chunk->size +
        1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10) and  prev_end (10) = chunk_end (10) ) */

    /* Arrange for CF_Chunks_FindSmallestSize */
    arg_chunks->chunks[0].size = arg_chunk->size + 1;
    arg_chunks->chunks[1].size = arg_chunk->size + 1;

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, dummy_chunks_count);
} /* end
     Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing
   */

void Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_And_CF_Chunks_InsertChunk(
    void)
{
    /* Arrange */
    CF_ChunkList_t *arg_chunks;
    CF_ChunkIdx_t   arg_i;
    CF_Chunk_t      dummy_chunk = {0};
    CF_Chunk_t     *arg_chunk   = &dummy_chunk;

    /* Arrange for CF_Chunks_CombineNext to return 0 and CF_Chunks_CombinePrevious to return 0 */
    CF_ChunkIdx_t  dummy_chunks_count      = 3;     /* 10 for dummy_chunks_count is arbitrary, chosen for speed */
    CF_Chunk_t     dummy_chunks_chunks[10] = {{0}}; /* 10 repeated for dummy_chunks for build ability */
    CF_ChunkList_t dummy_chunks;

    arg_chunks             = &dummy_chunks;
    arg_chunks->max_chunks = 3;

    arg_chunks->count = dummy_chunks_count;
    arg_i             = arg_chunks->count;

    arg_chunk->offset = 5; /* for chunk_offset (5) < prev_end (10)*/
    arg_chunk->size   = 5; /* for chunk_end to be 10 = 5 + 5 */

    arg_chunks->chunks                   = dummy_chunks_chunks;
    arg_chunks->chunks[arg_i - 1].offset = 4;
    arg_chunks->chunks[arg_i - 1].size =
        arg_chunk->size +
        1; /* for prev_end to be 10, chunk_offset (5) < prev_end (10) and  prev_end (10) = chunk_end (10) ) */

    /* Arrange for CF_Chunks_FindSmallestSize */
    arg_chunks->chunks[0].size = 0;
    arg_chunks->chunks[1].size = 0;

    /* Act */
    CF_Chunks_Insert(arg_chunks, arg_i, arg_chunk);

    /* Assert */
    /* Assert for CF_Chunks_EraseChunk and CF_Chunks_InsertChunk (-1/+1 thus no change? NOTE: better way to prove these
     * ran)*/
    UtAssert_UINT32_EQ(arg_chunks->count, dummy_chunks_count);
} /* end
     Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_And_CF_Chunks_InsertChunk
   */

/* end CF_Chunks_Insert tests */

/*******************************************************************************
**
**  CF_ChunkListAdd tests
**
*******************************************************************************/
void Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset(void)
{
    // /* Arrange */
    // CF_ChunkOffset_t     arg_offset = UINT32_MAX;
    // CF_ChunkSize_t       arg_size = 1;
    // CF_Chunk_t            initial_chunks[20] = { {0} }; /* 20 used as a reasonably fast size for the test, but is
    // still illustrative */ CF_ChunkList_t           dummy_chunks; CF_ChunkList_t*          arg_chunks = &dummy_chunks;

    // /* Arrange for CF_Chunks_FindInsertPosition */
    // CF_Chunk_t       dummy_chunk = {0};
    // uint8         num_chunks_before = Any_uint8_LessThan(10);
    // uint8         num_chunks_after = Any_uint8_LessThan(10);
    // uint8         i = 0;

    // dummy_chunk.offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    // for(i = 0; i < num_chunks_before; ++i)
    // {
    //    initial_chunks[i].offset = Any_uint32_LessThan(dummy_chunk.offset);
    // }

    // for(i = 0; i < num_chunks_after; ++i)
    // {
    //    initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(dummy_chunk.offset);
    // }

    // arg_chunks->count = num_chunks_before + num_chunks_after;
    // arg_chunks->chunks = initial_chunks;

    // /* Act */
    // CF_ChunkListAdd(arg_chunks, arg_offset, arg_size);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset */

void Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_IsNotLessThanOrEqToGiven_chunks_i_offset(
    void)
{
    // /* Arrange */
    // CF_ChunkOffset_t     arg_offset = 1;
    // CF_ChunkSize_t       arg_size = 1;
    // CF_Chunk_t            initial_chunks[20] = { {0} }; /* 20 used as a reasonably fast size for the test, but is
    // still illustrative */ CF_ChunkList_t           dummy_chunks; CF_ChunkList_t*          arg_chunks = &dummy_chunks;

    // /* Arrange for CF_Chunks_FindInsertPosition */
    // CF_Chunk_t       dummy_chunk = {0};
    // uint8         num_chunks_before = Any_uint8_LessThan(10);
    // uint8         num_chunks_after = Any_uint8_LessThan(10);
    // uint8         i = 0;

    // dummy_chunk.offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    // for(i = 0; i < num_chunks_before; ++i)
    // {
    //    initial_chunks[i].offset = Any_uint32_LessThan(dummy_chunk.offset);
    // }

    // for(i = 0; i < num_chunks_after; ++i)
    // {
    //    initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(dummy_chunk.offset);
    // }

    // arg_chunks->count = num_chunks_before + num_chunks_after;
    // arg_chunks->chunks = initial_chunks;

    // /* Act */
    // /* NOTE: unsure how to set this up to hit the assert. */
    // CF_ChunkListAdd(arg_chunks, arg_offset, arg_size);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end
     Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_IsNotLessThanOrEqToGiven_chunks_i_offset
   */

void Test_CF_Chunks_Add_WhenNotAtEndCall_CF_Chunks_Insert(void)
{
    /* Arrange */
    CF_ChunkOffset_t arg_offset         = 0;
    CF_ChunkSize_t   arg_size           = 0;
    CF_Chunk_t       initial_chunks[20] = {
        {0}}; /* 20 used as a reasonably fast size for the test, but is still illustrative */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks = &dummy_chunks;

    /* Arrange for CF_Chunks_FindInsertPosition */
    CF_Chunk_t dummy_chunk       = {0};
    uint8      num_chunks_before = Any_uint8_LessThan(10);
    uint8      num_chunks_after  = Any_uint8_LessThan(10);
    uint8      i                 = 0;

    dummy_chunk.offset = Any_uint32_BetweenInclusive(1, UINT32_MAX - 1);

    for (i = 0; i < num_chunks_before; ++i)
    {
        initial_chunks[i].offset = Any_uint32_LessThan(dummy_chunk.offset);
    }

    for (i = 0; i < num_chunks_after; ++i)
    {
        initial_chunks[i + num_chunks_before].offset = Any_uint32_GreaterThan(dummy_chunk.offset);
    }

    arg_chunks->count  = num_chunks_before + num_chunks_after;
    arg_chunks->chunks = initial_chunks;

    /* Act */
    CF_ChunkListAdd(arg_chunks, arg_offset, arg_size);

    /* Assert */
    /* NOTE: find a way to verify results in this test */
    UtAssert_True(
        true,
        "Nothing is able to be asserted on here, success is measured by test not segfaulting and coverage check.");
} /* end Test_CF_Chunks_Add_WhenNotAtEndCall_CF_Chunks_Insert */

void Test_CF_Chunks_Add_WhenAtEndCall_CF_Chunks_Insert(void)
{
    /* Arrange */
    CF_ChunkOffset_t arg_offset        = 10;
    CF_ChunkSize_t   arg_size          = 5;
    CF_Chunk_t       initial_chunks[2] = {{0}};
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks = &dummy_chunks;

    /* Arrange for CF_Chunks_FindInsertPosition */

    initial_chunks->offset = 0;
    initial_chunks->size   = 0;
    arg_chunks->count      = 1;
    arg_chunks->chunks     = initial_chunks;

    /* Act */
    CF_ChunkListAdd(arg_chunks, arg_offset, arg_size);

    /* Assert */
    /* NOTE: find a way to verify results in this test */
    UtAssert_True(
        true,
        "Nothing is able to be asserted on here, success is measured by test not segfaulting and coverage check.");
} /* end Test_CF_Chunks_Add_WhenAtEndCall_CF_Chunks_Insert */

/* end CF_ChunkListAdd tests */

/*******************************************************************************
**
**  CF_ChunkList_RemoveFromFirst tests
**
*******************************************************************************/

void Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Call_CF_Chunks_EraseChunk(
    void)
{
    /* Arrange */
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks                   = &dummy_chunks;
    CF_ChunkOffset_t initial_chunks_chunks_offset = Any_uint32();
    CF_ChunkSize_t   arg_size                     = Any_uint32_Except(0);
    CF_Chunk_t       dummy_chunks_chunks;

    arg_chunks->count            = 1;
    arg_chunks->chunks           = &dummy_chunks_chunks;
    arg_chunks->chunks[0].offset = initial_chunks_chunks_offset;
    arg_chunks->chunks[0].size   = arg_size;

    /* Act */
    CF_ChunkList_RemoveFromFirst(arg_chunks, arg_size);

    /* Assert */
    /* Assert Unstubbable: CF_Chunks_EraseChunk */
    UtAssert_UINT32_EQ(arg_chunks->count, 0);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, initial_chunks_chunks_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, 0);
} /* end
     Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Call_CF_Chunks_EraseChunk */

void Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Call_CF_Chunks_EraseChunk(
    void)
{
    /* Arrange */
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks                   = &dummy_chunks;
    CF_ChunkOffset_t initial_chunks_chunks_offset = Any_uint32();
    CF_ChunkSize_t   arg_size                     = Any_uint32_Except(UINT32_MAX - 1) + 2; /* from 2 to UINT_32_MAX */
    CF_ChunkSize_t   dummy_chunk_size             = Any_uint32_LessThan(arg_size);
    CF_Chunk_t       dummy_chunks_chunks;

    arg_chunks->count            = 1;
    arg_chunks->chunks           = &dummy_chunks_chunks;
    arg_chunks->chunks[0].offset = initial_chunks_chunks_offset;
    arg_chunks->chunks[0].size   = dummy_chunk_size;

    /* Act */
    CF_ChunkList_RemoveFromFirst(arg_chunks, arg_size);

    /* Assert */
    /* Assert Unstubbable: CF_Chunks_EraseChunk */
    UtAssert_UINT32_EQ(arg_chunks->count, 0);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, initial_chunks_chunks_offset);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, 0);
} /* end
     Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Call_CF_Chunks_EraseChunk
   */

void Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddGiven_size_To_chunks_chunks_0_offset_AndSet_chunks_chunks_size_To_0(
    void)
{
    /* Arrange */
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks                   = &dummy_chunks;
    CF_ChunkOffset_t initial_chunks_chunks_offset = Any_uint32();
    CF_ChunkSize_t   arg_size                     = Any_uint32_Except(0); /* from 2 to UINT_32_MAX */
    CF_ChunkSize_t   dummy_chunk_size             = Any_uint32_GreaterThan(arg_size);
    CF_Chunk_t       dummy_chunks_chunks;

    arg_chunks->count            = 1;
    arg_chunks->chunks           = &dummy_chunks_chunks;
    arg_chunks->chunks[0].offset = initial_chunks_chunks_offset;
    arg_chunks->chunks[0].size   = dummy_chunk_size;

    /* Act */
    CF_ChunkList_RemoveFromFirst(arg_chunks, arg_size);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->count, 1);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].offset, initial_chunks_chunks_offset + arg_size);
    UtAssert_UINT32_EQ(arg_chunks->chunks[0].size, dummy_chunk_size - arg_size);
} /* end
     Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddGiven_size_To_chunks_chunks_0_offset_AndSet_chunks_chunks_size_To_0
   */

/* end CF_ChunkList_RemoveFromFirst tests */

/*******************************************************************************
**
**  CF_ChunkList_GetFirstChunk tests
**
*******************************************************************************/

void Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Return_NULL(void)
{
    /* Arrange */
    CF_ChunkList_t    dummy_chunks;
    CF_ChunkList_t   *arg_chunks = &dummy_chunks;
    const CF_Chunk_t *local_result;

    arg_chunks->count = 0;

    /* Act */
    local_result = CF_ChunkList_GetFirstChunk(arg_chunks);

    /* Assert */
    UtAssert_NULL(local_result);
} /* end Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Return_NULL */

void Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnOnlyChunk(void)
{
    /* Arrange */
    CF_ChunkList_t    dummy_chunks = {0};
    CF_ChunkList_t   *arg_chunks   = &dummy_chunks;
    CF_Chunk_t        only_chunk;
    const CF_Chunk_t *local_result;

    arg_chunks->count  = 1;
    arg_chunks->chunks = &only_chunk;

    /* Act */
    local_result = CF_ChunkList_GetFirstChunk(arg_chunks);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &arg_chunks->chunks[0]);
    UtAssert_ADDRESS_EQ(local_result, &only_chunk);
} /* end Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnOnlyChunk */

void Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_t_ReturnFirstChunk(void)
{
    /* Arrange */
    CF_ChunkList_t    dummy_chunks = {0};
    CF_ChunkList_t   *arg_chunks   = &dummy_chunks;
    CF_Chunk_t        first_chunk;
    const CF_Chunk_t *local_result;

    arg_chunks->count  = Any_index_t();
    arg_chunks->chunks = &first_chunk;

    /* Act */
    local_result = CF_ChunkList_GetFirstChunk(arg_chunks);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &arg_chunks->chunks[0]);
    UtAssert_ADDRESS_EQ(local_result, &first_chunk);
} /* end Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_t_ReturnFirstChunk */

/* CF_ChunkList_GetFirstChunk tests */

/*******************************************************************************
**
**  CF_ChunkListInit tests
**
*******************************************************************************/

void Test_CF_Chunks_Init_AssertsBecauseGiven_max_chunks_Is_0(void)
{
    // /* Arrange */
    // CF_ChunkList_t        dummy_chunks;
    // CF_ChunkList_t*       arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t         arg_max_chunks = Any_uint16() + 2; /* 2-65537, uint16 is used instead of CF_ChunkIdx_t
    // to have a reasonably decent size for the test without being too large (segfault) */ CF_Chunk_t* arg_chunks_mem;

    // /* Act */
    // CF_ChunkListInit(arg_chunks, arg_max_chunks, arg_chunks_mem);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_Init_AssertsBecauseGiven_max_chunks_Is_0 */

void Test_CF_Chunks_Init_SetGiven_chunks_max_chunks_ToGiven_max_chunks(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks     = &dummy_chunks;
    CF_ChunkIdx_t   arg_max_chunks = 14;
    CF_Chunk_t      arg_chunks_mem[14];

    arg_chunks->count = 0;

    /* Act */
    CF_ChunkListInit(arg_chunks, arg_max_chunks, arg_chunks_mem);

    /* Assert */
    UtAssert_UINT32_EQ(arg_chunks->max_chunks, arg_max_chunks);
    UtAssert_ADDRESS_EQ(arg_chunks->chunks, arg_chunks_mem);
    /* Assert Unstubbable - CF_ChunkListReset */
    UtAssert_ZERO(arg_chunks->count);
    UtAssert_MemCmpValue(arg_chunks->chunks, 0x00, sizeof(*arg_chunks->chunks) * arg_max_chunks,
                         "The chunks, %lu bytes (sizeof(*chunks->chunks)*chunks->max_chunks), were all set to 0",
                         (unsigned long)sizeof(CF_Chunk_t) * arg_max_chunks);
} /* end Test_CF_Chunks_Init_SetGiven_chunks_max_chunks_ToGiven_max_chunks */

/* CF_ChunkListInit tests */

/*******************************************************************************
**
**  CF_ChunkListReset tests
**
*******************************************************************************/

void Test_CF_ChunksReset_Set_count_To_0_Keeps_max_chunks_AndMemsets_chunks_ToAll_0(void)
{
    /* Arrange */
    CF_ChunkList_t  dummy_chunks;
    CF_ChunkList_t *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t   initial_max_chunks = 17;
    CF_Chunk_t      dummy_chunks_chunks[17];

    arg_chunks->count      = Any_index_t();
    arg_chunks->max_chunks = initial_max_chunks;
    arg_chunks->chunks     = dummy_chunks_chunks;

    /* Act */
    CF_ChunkListReset(arg_chunks);

    /* Assert */
    UtAssert_ZERO(arg_chunks->count);
    UtAssert_UINT32_EQ(arg_chunks->max_chunks, initial_max_chunks);
    UtAssert_MemCmpValue(arg_chunks->chunks, 0x00, sizeof(CF_Chunk_t) * initial_max_chunks,
                         "The chunks, %lu bytes (sizeof(CF_Chunk_t)*chunks->max_chunks), were all set to 0",
                         (unsigned long)sizeof(CF_Chunk_t) * initial_max_chunks);
} /* end Test_CF_ChunksReset_Set_count_To_0_Keeps_max_chunks_AndMemsets_chunks_ToAll_0 */

/* CF_ChunkListReset tests */

/*******************************************************************************
**
**  CF_ChunkList_ComputeGaps tests (large) - Full coverage - NOTE: This is not a great example of good unit testing.  In
*the quest for speed and full coverage, liberties with values were taken to find ones that achieved the desired coverage
*in the CUT.  Values should be more robust and be more indicative of the kind of values that will actually be
*encountered during operation.  Not enough time could be spent on deciphering intent and the creator of that intent is
*no longer available.
**
*******************************************************************************/

void Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0(void)
{
    // /* Arrange */
    // CF_ChunkList_t          dummy_chunks;
    // CF_ChunkList_t*         arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t           arg_max_gaps = Any_uint32();
    // CF_ChunkSize_t      arg_total = 0;
    // CF_ChunkOffset_t    arg_start = Any_uint32();
    // CF_ChunkList_ComputeGapFn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    // void*             arg_opaque = NULL;

    // /* Act */
    // CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0 */

void Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total(void)
{
    // /* Arrange */
    // CF_ChunkList_t          dummy_chunks;
    // CF_ChunkList_t*         arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t           arg_max_gaps = Any_uint32();
    // CF_ChunkSize_t      arg_total = Any_uint32_Except(UINT32_MAX - 1) + 1; /* from 1 to (UINT32_MAX - 1) */
    // CF_ChunkOffset_t    arg_start = Any_uint32_GreaterThan(arg_total);
    // CF_ChunkList_ComputeGapFn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    // void*             arg_opaque = NULL;

    // /* Act */
    // CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total */

void Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsEqToGiven_total(void)
{
    // /* Arrange */
    // CF_ChunkList_t          dummy_chunks;
    // CF_ChunkList_t*         arg_chunks = &dummy_chunks;
    // CF_ChunkIdx_t           arg_max_gaps = Any_uint32();
    // CF_ChunkSize_t      arg_total = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    // CF_ChunkOffset_t    arg_start = arg_total;
    // CF_ChunkList_ComputeGapFn_t  arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    // void*             arg_opaque = NULL;

    // /* Act */
    // CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_Return_1(
    void)
{
    /* Arrange */
    CF_ChunkList_t              dummy_chunks;
    CF_ChunkList_t             *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t               arg_max_gaps       = Any_uint32();
    CF_ChunkSize_t              arg_total          = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    CF_ChunkOffset_t            arg_start          = arg_total - 1;
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = NULL;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 0;

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 0);
    UtAssert_UINT32_EQ(result, 1);
} /* end
     Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_Return_1
   */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1(void)
{
    /* Arrange */
    CF_ChunkList_t              dummy_chunks;
    CF_ChunkList_t             *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t               arg_max_gaps       = Any_uint32();
    CF_ChunkSize_t              arg_total          = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    CF_ChunkOffset_t            arg_start          = Any_uint32_LessThan(arg_total);
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = NULL;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 0;

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 0);
    UtAssert_UINT32_EQ(result, 1);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_ThenReturn_1(
    void)
{
    /* Arrange */
    CF_ChunkList_t              dummy_chunks;
    CF_ChunkList_t             *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t               arg_max_gaps       = Any_uint32();
    CF_ChunkSize_t              arg_total          = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    CF_ChunkOffset_t            arg_start          = Any_uint32_LessThan(arg_total);
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 0;

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_True(context_Dummy_compute_gap_fn_t.cs == arg_chunks, "context_Dummy_compute_gap_fn_t.cs ==  arg_chunks");
    UtPrintf("Stub count check is destroying the memory at context_Dummy_compute_gap_fn_t.c (ha ha, not a filename, "
             "'c' is the variable name)");
    // NOTE: values pointed to by contexts are out of scope and this is now an issue because any get stub count is
    // killing access to values set by CUT and given to stubs via pointer (it may actually be in UT_GetStubEntry used by
    // getting the stub count) UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.c->offset, 0);
    // UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.c->size, arg_total);
    UtAssert_True(context_Dummy_compute_gap_fn_t.opaque == arg_opaque,
                  "context_Dummy_compute_gap_fn_t.opaque ==  arg_opaque");
    UtAssert_UINT32_EQ(result, 1);
} /* end
     Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_ThenReturn_1
   */

void Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0(void)
{
    /* Arrange */
    CF_Chunk_t                  dummy_chunks_chunks[1] = {{0}};
    CF_ChunkList_t              dummy_chunks;
    CF_ChunkList_t             *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t               arg_max_gaps       = 0;
    CF_ChunkSize_t              arg_total          = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    CF_ChunkOffset_t            arg_start          = Any_uint32_LessThan(arg_total);
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 1;

    dummy_chunks.chunks           = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint32_LessThan(arg_total);
    dummy_chunks.chunks[0].size   = arg_total - dummy_chunks.chunks[0].offset;

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 0);
    UtAssert_UINT32_EQ(result, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0(void)
{
    /* Arrange */
    CF_Chunk_t                  dummy_chunks_chunks[1] = {{0}};
    CF_ChunkList_t              dummy_chunks;
    CF_ChunkList_t             *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t               arg_max_gaps       = Any_uint32();
    CF_ChunkSize_t              arg_total          = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    CF_ChunkOffset_t            arg_start          = Any_uint32_LessThan(arg_total);
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 1;

    dummy_chunks.chunks           = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint32_LessThan(arg_total);
    dummy_chunks.chunks[0].size   = arg_total - dummy_chunks.chunks[0].offset;

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 0);
    UtAssert_UINT32_EQ(result, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0(void)
{
    /* Arrange */
    CF_Chunk_t                  dummy_chunks_chunks[1] = {{0}};
    CF_ChunkList_t              dummy_chunks;
    CF_ChunkList_t             *arg_chunks         = &dummy_chunks;
    CF_ChunkIdx_t               arg_max_gaps       = Any_uint32();
    CF_ChunkSize_t              arg_total          = Any_uint32_Except(UINT32_MAX) + 1; /* from 1 to UINT32_MAX */
    CF_ChunkOffset_t            arg_start          = Any_uint32_LessThan(arg_total);
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void                       *arg_opaque         = NULL;
    CF_ChunkOffset_t            dummy_gap_start    = Any_uint32_GreaterThan(arg_total);
    int32                       result;

    arg_chunks->count             = 1;
    dummy_chunks.chunks           = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint32_LessThan(dummy_gap_start);
    dummy_chunks.chunks[0].size   = dummy_gap_start - dummy_chunks.chunks[0].offset;

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 0);
    UtAssert_UINT32_EQ(result, 0);
} /* end Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0 */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_Return_1(
    void)
{
    /* Arrange */
    CF_Chunk_t       dummy_chunks_chunks[1] = {{0}};
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks   = &dummy_chunks;
    CF_ChunkIdx_t    arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    CF_ChunkSize_t   arg_total;
    CF_ChunkOffset_t arg_start                     = 0;
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 1;

    dummy_chunks.chunks           = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint8_GreaterThan(1); /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable
                                                                 size for test, larger may be difficult to handle */
    dummy_chunks.chunks[0].size = Any_uint8_GreaterThan(1);   /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable
                                                                 size for test, larger may be difficult to handle */

    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + 1; /* + 1 to always be greater */

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 1);
    UtAssert_UINT32_EQ(result, 1);
} /* end
     Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_Return_1
   */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_NULL_Return_1(
    void)
{
    /* Arrange */
    CF_Chunk_t       dummy_chunks_chunks[1] = {{0}};
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks   = &dummy_chunks;
    CF_ChunkIdx_t    arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    CF_ChunkSize_t   arg_total;
    CF_ChunkOffset_t arg_start;
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = Dummy_compute_gap_fn_t;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 1;

    dummy_chunks.chunks           = dummy_chunks_chunks;
    dummy_chunks.chunks[0].offset = Any_uint8_GreaterThan(1); /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable
                                                                 size for test, larger may be difficult to handle */
    dummy_chunks.chunks[0].size = Any_uint8_GreaterThan(1);   /* Any_uint8_GreaterThan(1) arbitrarily used as reasonable
                                                                 size for test, larger may be difficult to handle */

    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + 1; /* + 1 to always be greater */

    arg_start = arg_total - 1; /* - 1 for less than total, but forces c.offset+c.size)>=start to be true */

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 1);
    UtAssert_UINT32_EQ(result, 1);
} /* end
     Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_NULL_Return_1
   */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1(
    void)
{
    /* Arrange */
    CF_Chunk_t       dummy_chunks_chunks[2] = {{0}};
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks   = &dummy_chunks;
    CF_ChunkIdx_t    arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    CF_ChunkSize_t   arg_total;
    CF_ChunkOffset_t arg_start;
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = NULL;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 2;

    dummy_chunks.chunks           = dummy_chunks_chunks;
    dummy_chunks.chunks[1].offset = 1; /* 1 arbitrarily used for offset 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[1].size   = 1; /* 1 arbitrarily used for size 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].offset = 1; /* 1 arbitrarily used for offset 0 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].size   = 1; /* 1 arbitrarily used for size 0 because it exhibits desired coverage in CUT */

    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + 2; /* + 2 to always be greater */

    arg_start = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size +
                1; /* + 1 for less than total, but forces c.offset+c.size)>=start to be false */

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(result, 1);
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 0);
} /* end
     Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1
   */

void Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButASubsequentLoop_started_Is_1_And_compute_gap_fn_Is_NULL_ReturnNumberOfLoops(
    void)
{
    /* Arrange */
    CF_Chunk_t       dummy_chunks_chunks[3] = {{0}};
    CF_ChunkList_t   dummy_chunks;
    CF_ChunkList_t  *arg_chunks   = &dummy_chunks;
    CF_ChunkIdx_t    arg_max_gaps = UINT32_MAX; /* UINT32_MAX used to show this will not come into play for this test */
    CF_ChunkSize_t   arg_total;
    CF_ChunkOffset_t arg_start;
    CF_ChunkList_ComputeGapFn_t arg_compute_gap_fn = NULL;
    void                       *arg_opaque         = NULL;
    int32                       result;

    arg_chunks->count = 3;

    dummy_chunks.chunks           = dummy_chunks_chunks;
    dummy_chunks.chunks[2].offset = 5; /* 5 arbitrarily used for offset 2 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[2].size   = 5; /* 5 arbitrarily used for size 2 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[1].offset = 1; /* 1 arbitrarily used for offset 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[1].size   = 1; /* 1 arbitrarily used for size 1 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].offset = 1; /* 1 arbitrarily used for offset 0 because it exhibits desired coverage in CUT */
    dummy_chunks.chunks[0].size   = 1; /* 1 arbitrarily used for size 0 because it exhibits desired coverage in CUT */

    arg_total = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + dummy_chunks.chunks[1].offset +
                dummy_chunks.chunks[1].size + dummy_chunks.chunks[2].offset + dummy_chunks.chunks[2].size +
                2; /* + 2 to always be greater */

    arg_start = dummy_chunks.chunks[0].offset + dummy_chunks.chunks[0].size + dummy_chunks.chunks[1].offset +
                dummy_chunks.chunks[1].size + 1; /* forces c.offset+c.size)>=start to be false */

    /* Act */
    result = CF_ChunkList_ComputeGaps(arg_chunks, arg_max_gaps, arg_total, arg_start, arg_compute_gap_fn, arg_opaque);

    /* Assert */
    UtAssert_UINT32_EQ(result, 2);
    UtAssert_UINT32_EQ(context_Dummy_compute_gap_fn_t.count, 0);
} /* end
     Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1
   */

/* end CF_ChunkList_ComputeGaps tests */

/*******************************************************************************
**
**  cf_chunk_tests UtTest_Add groups
**
**  NOTE: Tests commented out are edge case CF_Assert fail tests that are not
**        required to mark the fail because there is another test doing that.
**        These tests should be used if the CF_Assert issue is solved.
**
*******************************************************************************/

void add_macro_tests(void)
{
    UtTest_Add(Test_MAX_WhenItIsMaxValue_Return_a, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_MAX_WhenItIsMaxValue_Return_a");
    UtTest_Add(Test_MAX_WhenItIsMaxValue_Return_b, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_MAX_WhenItIsMaxValue_Return_b");
    UtTest_Add(Test_MAX_WhenItIsEqualTo_b_Returns_a, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_MAX_WhenItIsEqualTo_b_Returns_a");
} /* end add_macro_tests */

void add_CF_Chunks_EraseRange_tests(void)
{
    UtTest_Add(Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_EraseRange_AssertsBecause_end_IsLessThan_start");
    // UtTest_Add(Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start,
    //   cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
    //   "Test_CF_Chunks_EraseRange_AssertsBecause_end_EqTo_start");
    UtTest_Add(Test_CF_Chunks_When_start_Is_OneMoreThan_end_MovesOneChunk_EraseRange, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_When_start_Is_OneMoreThan_end_MovesOneChunk_EraseRange");
    UtTest_Add(Test_CF_Chunks_When_start_IsLessThan_chunks_count_DoesNotChangeStart_EraseRange, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown,
               "Test_CF_Chunks_When_start_IsLessThan_chunks_count_DoesNotChangeStart_EraseRange");
} /* end add_CF_Chunks_EraseRange_tests */

void add_CF_Chunks_EraseChunk_tests(void)
{
    UtTest_Add(Test_CF_Chunks_EraseChunk_AssertsBecause_Given_chunks_count_IsEqTo_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_EraseChunk_AssertsBecause_Given_chunks_count_IsEqTo_0");
    UtTest_Add(Test_CF_Chunks_EraseChunk_AssertsBecause_Given_erase_index_IsEqTo_chunks_count, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown,
               "Test_CF_Chunks_EraseChunk_AssertsBecause_Given_erase_index_IsEqTo_chunks_count");
    UtTest_Add(Test_CF_Chunks_EraseChunk_WhenThereIsOnlyOneEraseOneChunkAndDecrement_count_To_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown,
               "Test_CF_Chunks_EraseChunk_WhenThereIsOnlyOneEraseOneChunkAndDecrement_count_To_0");
    UtTest_Add(Test_CF_Chunks_EraseChunk_EraseOneChunkThatIsNotTheLastFrom_chunks_AndDecrement_count,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_EraseChunk_EraseOneChunkThatIsNotTheLastFrom_chunks_AndDecrement_count");
    UtTest_Add(Test_CF_Chunks_EraseChunk_ErasesLastChunkFrom_chunks_AndDecrements_count, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_EraseChunk_ErasesLastChunkFrom_chunks_AndDecrements_count");
} /* end add_CF_Chunks_EraseChunk_tests */

void add_CF_Chunks_InsertChunk_tests(void)
{
    UtTest_Add(Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsEqTo_chunks_max_chunks,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsEqTo_chunks_max_chunks");
    // UtTest_Add(Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsGreaterThan_chunks_max_chunks,
    //   cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
    //   "Test_CF_Chunks_InsertChunk_AssertsBecause_Given_chunks_count_IsGreaterThan_chunks_max_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEmpty_chunks, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEmpty_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoStartOfSingle_chunks, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoStartOfSingle_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEndOfSingle_chunks, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoEndOfSingle_chunks");
    UtTest_Add(Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoSome_chunks, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_InsertChunk_PutGiven_chunk_IntoSome_chunks");
} /* end add_CF_Chunks_InsertChunk_tests */

void add_CF_Chunks_FindInsertPosition_tests(void)
{
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_IsEmptyReturn_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_FindInsertPosition_Given_chunks_IsEmptyReturn_0");
    UtTest_Add(
        Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturn_0,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_LessThanItemOffsetReturn_0");
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturn_0,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_EqToItemOffsetReturn_0");
    UtTest_Add(
        Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturn_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasOneItemAndGiven_chunk_offset_GreaterThanOffsetReturn_1");
    UtTest_Add(
        Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturn_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasTwoItemsAndGiven_chunk_offset_IsInBetweenTheirOffsetsReturn_"
        "1");
    UtTest_Add(Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnCorrectExpectedPosition,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_FindInsertPosition_Given_chunks_HasAnyItemsAndReturnCorrectExpectedPosition");
} /* end add_CF_Chunks_FindInsertPosition_tests */

void add_CF_Chunks_CombinePrevious_tests(void)
{
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsEqTo_chunks_CF_max_Chunks");
    // UtTest_Add(Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks,
    //   cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
    //   "Test_CF_Chunks_CombinePrevious_Asserts_i_GivenIsGreaterThan_chunks_CF_max_Chunks");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_CombinePrevious_Given_i_Is_0_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_CombinePrevious_Given_chunks_count_Is_0_Return_0");
    UtTest_Add(Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_CombinePrevious_Given_i_Is_0_And_chunks_count_Is_0_Return_0");
    UtTest_Add(
        Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_offset_Return_0,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_IsGreaterThanGiven_chunk_"
        "offset_Return_0");
    UtTest_Add(
        Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_EqToGiven_chunk_offset_"
        "ButNotLessThan_chunk_end_Return_0");
    UtTest_Add(
        Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButNotLessThan_chunk_end_Return_0,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_"
        "ButNotLessThan_chunk_end_Return_0");
    UtTest_Add(
        Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_ButEqTo_chunk_end_Return_0,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanGiven_chunk_offset_"
        "ButEqTo_chunk_end_Return_0");
    UtTest_Add(
        Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombinePrevious_GivenIndexMinusOne_chunks_chunks_ValueFor_prev_end_LessThanOrEqToGiven_chunk_"
        "offset_AndLessThan_chunk_end_Alters_prev_size_AndReturns_1");
} /* end add_CF_Chunks_CombinePrevious_tests */

void add_CF_Chunks_CombineNext_tests(void)
{
    UtTest_Add(Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Return_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_CombineNext_Given_i_IsEqTo_Given_chunks_count_Return_0");
    UtTest_Add(
        Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_chunk_offset,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_AssertsBecauseGiven_chunk_end_LessThanGiven_"
        "chunk_offset");
    UtTest_Add(
        Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_chunk_end_Return_0,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombineNext_Given_i_IsNotEqTo_Given_chunks_count_Given_chunks_chunks_i_offset_IsGreaterThan_"
        "chunk_end_Return_0");
    UtTest_Add(
        Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_CombinesChunksNotAtEndDoesNotEraseRangeReturn_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombineNext_Given_chunks_chunks_offset_IsLessThan_chunk_end_"
        "CombinesChunksNotAtEndDoesNotEraseRangeReturn_1");
    UtTest_Add(
        Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplaceEverythingUntilEndAndCall_CF_Chunks_EraseRange_ThenReturn_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_BreaksBReplaceEverythingUntilEndAndCall_CF_Chunks_"
        "EraseRange_ThenReturn_1");
    UtTest_Add(
        Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplaceEverythingUntilEndAndCall_CF_Chunks_EraseRange_ThenReturn_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_CombineNext_Given_i_IsEqToGiven_chunks_count_ReplaceEverythingUntilEndAndCall_CF_Chunks_"
        "EraseRange_ThenReturn_1");
} /* end add_CF_Chunks_CombineNext_tests */

void add_CF_Chunks_FindSmallestSize_tests(void)
{
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_IsEmptyReturn_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_FindSmallestSize_Given_chunks_IsEmptyReturn_0");
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturn_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_FindSmallestSize_Given_chunks_HasOneChunkReturn_0");
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_FindSmallestSize_Given_chunks_HasTwoChunksReturnsCorrectIndexOfSmallest");
    UtTest_Add(Test_CF_Chunks_FindSmallestSize_Given_chunks_HasManyChunksReturnsCorrectIndexOfSmallest,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_FindSmallestSize_Given_chunks_HasManyChunksReturnsCorrectIndexOfSmallest");
} /* end add_CF_Chunks_FindSmallestSize_tests */

void add_CF_Chunks_Insert_tests(void)
{
    UtTest_Add(
        Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_DoNothing,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_non0_CallTo_CF_Chunks_CombinePrevious_Returns_0_"
        "DoNothing");
    UtTest_Add(Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_Insert_CombinesNextSuccessButCombinePreviousSuccessCalls_CF_Chunks_EraseChunk");
    UtTest_Add(
        Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_DoNothing,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_1_"
        "DoNothing");
    UtTest_Add(
        Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_"
        "chunks_count_IsLessThan_chunks_CF_max_Chunks_Call_CF_Chunks_InsertChunk");
    UtTest_Add(
        Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_"
        "chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsGreaterThan_chunk_size_DoNothing");
    UtTest_Add(
        Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_And_CF_Chunks_InsertChunk,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_Insert_CallTo_CF_Chunks_CombineNext_Returns_0_CallTo_CF_Chunks_CombinePrevious_Returns_0_And_"
        "chunks_count_IsGreaterThan_max_chunks_And_smallest_c_size_IsLessThan_chunk_size_Call_CF_Chunks_EraseChunk_"
        "And_CF_Chunks_InsertChunk");
} /* end add_CF_Chunks_Insert_tests */

void add_CF_Chunks_Add_tests(void)
{
    UtTest_Add(Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_Add_Asserts_WhenGiven_offset_Plus_size_IsLessThanGiven_offset");
    UtTest_Add(
        Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_IsNotLessThanOrEqToGiven_chunks_i_offset,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_Add_Asserts_WhenInsertPositionIsNotEqualToGiven_chunks_count_And_chunk_offset_"
        "IsNotLessThanOrEqToGiven_chunks_i_offset");
    UtTest_Add(Test_CF_Chunks_Add_WhenNotAtEndCall_CF_Chunks_Insert, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_Add_WhenNotAtEndCall_CF_Chunks_Insert");
    UtTest_Add(Test_CF_Chunks_Add_WhenAtEndCall_CF_Chunks_Insert, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_Add_WhenAtEndCall_CF_Chunks_Insert");
} /* end add_CF_Chunks_Add_tests */

void add_CF_Chunks_RemoveFromFirst_tests(void)
{
    UtTest_Add(
        Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Call_CF_Chunks_EraseChunk,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsEqToGiven_size_Call_CF_Chunks_"
        "EraseChunk");
    UtTest_Add(
        Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Call_CF_Chunks_EraseChunk,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_RemoveFromFirst_When_chunks_HasOnlyOneChunkAndIts_size_IsSmallerThanGiven_size_Call_CF_Chunks_"
        "EraseChunk");
    UtTest_Add(
        Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddGiven_size_To_chunks_chunks_0_offset_AndSet_chunks_chunks_size_To_0,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_RemoveFromFirst_When_chunks_size_IsGreaterThanGiven_size_AddGiven_size_To_chunks_chunks_0_"
        "offset_AndSet_chunks_chunks_size_To_0");
} /* end add_CF_Chunks_RemoveFromFirst_tests */

void add_CF_Chunks_GetFirstChunk_tests(void)
{
    UtTest_Add(Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Return_NULL, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_0_Return_NULL");
    UtTest_Add(Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnOnlyChunk, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_1_ReturnOnlyChunk");
    UtTest_Add(Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_t_ReturnFirstChunk,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_GetFirstChunk_WhenGiven_chunks_count_Is_Any_index_t_ReturnFirstChunk");
} /* end add_CF_Chunks_GetFirstChunk_tests */

void add_CF_Chunks_Init_tests(void)
{
    UtTest_Add(Test_CF_Chunks_Init_AssertsBecauseGiven_max_chunks_Is_0, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_Init_AssertsBecauseGiven_max_chunks_Is_0");
    UtTest_Add(Test_CF_Chunks_Init_SetGiven_chunks_max_chunks_ToGiven_max_chunks, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_Init_SetGiven_chunks_max_chunks_ToGiven_max_chunks");
} /* end add_CF_Chunks_Init_tests */

void add_CF_ChunksReset_tests(void)
{
    UtTest_Add(Test_CF_ChunksReset_Set_count_To_0_Keeps_max_chunks_AndMemsets_chunks_ToAll_0, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown,
               "Test_CF_ChunksReset_Set_count_To_0_Keeps_max_chunks_AndMemsets_chunks_ToAll_0");
} /* end add_CF_ChunksReset_tests */

void add_CF_Chunks_ComputeGaps_tests(void)
{
    UtTest_Add(Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_total_Is_0");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown,
               "Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsGreaterThanGiven_total");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsEqToGiven_total, cf_chunk_tests_Setup,
               cf_chunk_tests_Teardown, "Test_CF_Chunks_ComputeGaps_AssertsBecauseGiven_start_IsEqToGiven_total");
    UtTest_Add(
        Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_Return_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_And_start_IsOneLessThan_total_"
        "Return_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_Is_NULL_Return_1");
    UtTest_Add(
        Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_ThenReturn_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_0_And_compute_gap_fn_IsNot_NULL_Call_compute_gap_fn_"
        "ThenReturn_1");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0, cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_ComputeGaps_Given_max_gaps_Is_0_Return_0");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsEqTo_total_BreakAndReturn_0");
    UtTest_Add(Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0,
               cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
               "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsGreaterThan_total_BreakAndReturn_0");
    UtTest_Add(
        Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_Return_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_gap_start_IsLessThan_total_And_compute_gap_fn_Is_NULL_"
        "Return_1");
    UtTest_Add(
        Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_NULL_Return_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsEqToStart_And_compute_gap_fn_Is_"
        "NULL_Return_1");
    UtTest_Add(
        Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_"
        "ButNextLoop_started_Is_1_And_compute_gap_fn_Is_NULL_Return_1");
    UtTest_Add(
        Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_ButASubsequentLoop_started_Is_1_And_compute_gap_fn_Is_NULL_ReturnNumberOfLoops,
        cf_chunk_tests_Setup, cf_chunk_tests_Teardown,
        "Test_CF_Chunks_ComputeGaps_Given_chunks_count_Is_1_And_c_offset_Plus_c_size_IsLessThanStart_And_started_Is_0_"
        "ButASubsequentLoop_started_Is_1_And_compute_gap_fn_Is_NULL_ReturnNumberOfLoops");
} /* end add_CF_Chunks_ComputeGaps_tests */

/* end cf_chunk_tests UtTest_Add groups */

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