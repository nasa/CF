/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_chunk.h"

/* Gap function test function and context */
#define TEST_CF_MAX_GAPS 3
typedef struct
{
    uint32     count;
    CF_Chunk_t chunks[TEST_CF_MAX_GAPS];
} Test_CF_compute_gap_context_t;

Test_CF_compute_gap_context_t Test_CF_compute_gap_context;

void Test_CF_compute_gap_fn(const CF_ChunkList_t *CList, const CF_Chunk_t *Chunk, void *Opaque)
{
    UtAssert_ADDRESS_EQ(CList, Opaque);

    if (Test_CF_compute_gap_context.count < TEST_CF_MAX_GAPS)
    {
        Test_CF_compute_gap_context.chunks[Test_CF_compute_gap_context.count].size   = Chunk->size;
        Test_CF_compute_gap_context.chunks[Test_CF_compute_gap_context.count].offset = Chunk->offset;
    }
    ++Test_CF_compute_gap_context.count;
}

/* Fill a chunk list */
void UT_CF_Chunk_SetupFull(CF_ChunkList_t *CList)
{
    CF_ChunkIdx_t cidx;

    /*
     * Set up nonzero values for size and calculate a "realistic" offsets w/ size
     *   Size: just set to index+1 so it's uniquely identifiable
     *   Offset: calculated using size and gap of 10
     *
     *  1    2     3     4     5
     * 0-1 11-13 23-26 36-40 50-55
     */
    CList->chunks[0].offset = 0;
    for (cidx = 0; cidx < CList->max_chunks; cidx++)
    {
        CList->chunks[cidx].size = cidx + 1;

        if (cidx > 0)
        {
            CList->chunks[cidx].offset = CList->chunks[cidx - 1].offset + CList->chunks[cidx - 1].size + 10;
        }
    }

    /* Set count to max since list is now full */
    CList->count = CList->max_chunks;
}

/* Print the chunk list to the UT log (test debug helper) */
void UT_CF_Chunk_Print(CF_ChunkList_t *CList)
{

    CF_ChunkIdx_t cidx;

    UtPrintf("Chunk list: index{offset, size}");
    for (cidx = 0; cidx < CList->count; cidx++)
    {
        UtPrintf("%u{%u, %u}", (unsigned int)cidx, (unsigned int)CList->chunks[cidx].offset,
                 (unsigned int)CList->chunks[cidx].size);
    }

    UtPrintf("Chunk list: index{start-end}");
    for (cidx = 0; cidx < CList->count; cidx++)
    {
        UtPrintf("%u{%u-%u}", (unsigned int)cidx, (unsigned int)CList->chunks[cidx].offset,
                 (unsigned int)(CList->chunks[cidx].offset + CList->chunks[cidx].size));
    }
}

/*
 * Test routines
 */

/*
 * Cover nominal create (which resets), add cases (empty, front, end, replace smallest)
 * Note out of order sizes are to fully cover CF_Chunks_FindSmallestSize
 */
void Test_CF_Chunk_CreateAddReset(void)
{
    CF_ChunkList_t clist;
    CF_Chunk_t     chunks[3];

    /* Set nonzero values and test CF_ChunkListInit */
    memset(&clist, 0xFF, sizeof(clist));
    memset(chunks, 0xFF, sizeof(chunks));
    UtAssert_VOIDCALL(CF_ChunkListInit(&clist, sizeof(chunks) / sizeof(chunks[0]), chunks));
    UtAssert_UINT32_EQ(clist.count, 0);
    UtAssert_UINT32_EQ(clist.max_chunks, sizeof(chunks) / sizeof(chunks[0]));
    /* Spot check chunks clear */
    UtAssert_UINT32_EQ(chunks[1].size, 0);
    UtAssert_UINT32_EQ(chunks[1].offset, 0);

    /* Add to empty list */
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 5, 1));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 5);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 1);
    UtAssert_UINT32_EQ(clist.count, 1);

    /* Add to end of list */
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 10, 1));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 5);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 1);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 10);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 1);
    UtAssert_UINT32_EQ(clist.count, 2);

    /* Add to front of list */
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 0, 2));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 2);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 5);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 1);
    UtAssert_UINT32_EQ(clist.chunks[2].offset, 10);
    UtAssert_UINT32_EQ(clist.chunks[2].size, 1);
    UtAssert_UINT32_EQ(clist.count, 3);

    /* Force 1 to drop (first smallest), with new at the end */
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 20, 2));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 2);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 10);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 1);
    UtAssert_UINT32_EQ(clist.chunks[2].offset, 20);
    UtAssert_UINT32_EQ(clist.chunks[2].size, 2);
    UtAssert_UINT32_EQ(clist.count, 3);

    /* Nominal combine previous (no overlap, at the end) */
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 22, 2));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 2);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 10);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 1);
    UtAssert_UINT32_EQ(clist.chunks[2].offset, 20);
    UtAssert_UINT32_EQ(clist.chunks[2].size, 4);
    UtAssert_UINT32_EQ(clist.count, 3);
}

/* Cover combination cases */
void Test_CF_Chunk_Combine(void)
{
    CF_ChunkList_t clist;
    CF_Chunk_t     chunks[5];

    /* Initialize list (note already tested) */
    CF_ChunkListInit(&clist, sizeof(chunks) / sizeof(chunks[0]), chunks);

    UtPrintf("Initial chunk list state for reference");
    UT_CF_Chunk_SetupFull(&clist);
    UT_CF_Chunk_Print(&clist);

    UtPrintf("Add chunk that won't add since list full and new chunk is smallest");
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 2, 1)); /* 2-3 */
    UT_CF_Chunk_Print(&clist);
    /* Confirm 0 and 1 didn't change */
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 1);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 11);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 2);
    UtAssert_UINT32_EQ(clist.count, 5);

    UT_CF_Chunk_SetupFull(&clist);
    UtPrintf("Add chunk that replaces chunk 0 as the smallest chunk");
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 2, 2)); /* 2-4 */
    UT_CF_Chunk_Print(&clist);
    /* Confirm 0 replaced and 1 didn't change */
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 2);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 2);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 11);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 2);
    UtAssert_UINT32_EQ(clist.count, 5);

    UT_CF_Chunk_SetupFull(&clist);
    UtPrintf("Add chunk that combines with chunk 1 w/ no overlap");
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 10, 1)); /* 10-11 */
    UT_CF_Chunk_Print(&clist);
    /* 0 and 2 unchanged, 1 combined */
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 1);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 10);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 3);
    UtAssert_UINT32_EQ(clist.chunks[2].offset, 23);
    UtAssert_UINT32_EQ(clist.chunks[2].size, 3);
    UtAssert_UINT32_EQ(clist.count, 5);

    UT_CF_Chunk_SetupFull(&clist);
    UtPrintf("Add chunk that should completely replace chunk 2 and 3, both as Next");
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 20, 21)); /* 20-41 */
    UT_CF_Chunk_Print(&clist);
    /* 1 unchanged, 2 combined, 4 in slot 3 */
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 11);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 2);
    UtAssert_UINT32_EQ(clist.chunks[2].offset, 20);
    UtAssert_UINT32_EQ(clist.chunks[2].size, 21);
    UtAssert_UINT32_EQ(clist.chunks[3].offset, 50);
    UtAssert_UINT32_EQ(clist.chunks[3].size, 5);
    UtAssert_UINT32_EQ(clist.count, 4);

    UT_CF_Chunk_SetupFull(&clist);
    UtPrintf("Add chunk that combines with chunk 1, 2 and 3, (prev, next, next)");
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 12, 25)); /* 12-37 */
    UT_CF_Chunk_Print(&clist);
    /* 0 unchanged, 1 combined, 4 in slot 2 */
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 1);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 11);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 29);
    UtAssert_UINT32_EQ(clist.chunks[2].offset, 50);
    UtAssert_UINT32_EQ(clist.chunks[2].size, 5);
    UtAssert_UINT32_EQ(clist.count, 3);

    UT_CF_Chunk_SetupFull(&clist);
    UtPrintf("Add chunk that is a subset of 3 (should just drop)");
    UtAssert_VOIDCALL(CF_ChunkListAdd(&clist, 37, 2)); /* 37-39 */
    UT_CF_Chunk_Print(&clist);
    /* 1, 2, and 3 unchanged */
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 11);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 2);
    UtAssert_UINT32_EQ(clist.chunks[2].offset, 23);
    UtAssert_UINT32_EQ(clist.chunks[2].size, 3);
    UtAssert_UINT32_EQ(clist.chunks[3].offset, 36);
    UtAssert_UINT32_EQ(clist.chunks[3].size, 4);
    UtAssert_UINT32_EQ(clist.count, 5);
}

void Test_CF_Chunk_GetRmFirst(void)
{
    CF_ChunkList_t clist;
    CF_Chunk_t     chunks[2];

    /* Initialize list (note already tested) */
    CF_ChunkListInit(&clist, sizeof(chunks) / sizeof(chunks[0]), chunks);

    /* Get first with empty list */
    UtAssert_ADDRESS_EQ(CF_ChunkList_GetFirstChunk(&clist), NULL);

    /* Note CF_ChunkList_RemoveFromFirst can not be called on empty list (as documented) */

    /* Add two (already tested) */
    CF_ChunkListAdd(&clist, 0, 10);
    CF_ChunkListAdd(&clist, 20, 10);

    /* Get first with non-empty list */
    UtAssert_ADDRESS_EQ(CF_ChunkList_GetFirstChunk(&clist), chunks);

    /* Remove part from first non-empty list */
    UtAssert_VOIDCALL(CF_ChunkList_RemoveFromFirst(&clist, 5));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 5);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 5);
    UtAssert_UINT32_EQ(clist.chunks[1].offset, 20);
    UtAssert_UINT32_EQ(clist.chunks[1].size, 10);
    UtAssert_UINT32_EQ(clist.count, 2);

    /* Remove the rest of first from non-empty list */
    UtAssert_VOIDCALL(CF_ChunkList_RemoveFromFirst(&clist, 5));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 20);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 10);
    UtAssert_UINT32_EQ(clist.count, 1);

    /* Add back in, do large remove, confirm only first chunk removed */
    CF_ChunkListAdd(&clist, 0, 10);
    UtAssert_VOIDCALL(CF_ChunkList_RemoveFromFirst(&clist, 50));
    UtAssert_UINT32_EQ(clist.chunks[0].offset, 20);
    UtAssert_UINT32_EQ(clist.chunks[0].size, 10);
    UtAssert_UINT32_EQ(clist.count, 1);
}

void Test_CF_Chunk_ComputeGaps(void)
{
    CF_ChunkList_t clist;
    CF_Chunk_t     chunks[5];
    CF_ChunkSize_t total;

    /* Initialize list (note already tested) */
    CF_ChunkListInit(&clist, sizeof(chunks) / sizeof(chunks[0]), chunks);

    /* Empty list with function callback */
    total = 10;
    memset(&Test_CF_compute_gap_context, 0, sizeof(Test_CF_compute_gap_context));
    UtAssert_UINT32_EQ(CF_ChunkList_ComputeGaps(&clist, TEST_CF_MAX_GAPS, total, 0, Test_CF_compute_gap_fn, &clist), 1);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].size, total);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.count, 1);

    /* Empty list no callback */
    memset(&Test_CF_compute_gap_context, 0, sizeof(Test_CF_compute_gap_context));
    UtAssert_UINT32_EQ(CF_ChunkList_ComputeGaps(&clist, TEST_CF_MAX_GAPS, total, 0, NULL, NULL), 1);

    /* Add three with gaps 0-4, 10-19, 30-49 */
    CF_ChunkListAdd(&clist, 5, 5);
    CF_ChunkListAdd(&clist, 20, 10);
    CF_ChunkListAdd(&clist, 50, 10);

    /* Check 0-45, reports 3 gaps and breaks on total limit */
    memset(&Test_CF_compute_gap_context, 0, sizeof(Test_CF_compute_gap_context));
    UtAssert_UINT32_EQ(CF_ChunkList_ComputeGaps(&clist, TEST_CF_MAX_GAPS, 25, 0, Test_CF_compute_gap_fn, &clist), 2);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].size, 5);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[1].size, 10);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[1].offset, 10);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.count, 2);

    /* Same with no callback */
    memset(&Test_CF_compute_gap_context, 0, sizeof(Test_CF_compute_gap_context));
    UtAssert_UINT32_EQ(CF_ChunkList_ComputeGaps(&clist, TEST_CF_MAX_GAPS, 45, 0, NULL, NULL), 3);

    /* Check 25-75, end while loop at end of chunk list */
    memset(&Test_CF_compute_gap_context, 0, sizeof(Test_CF_compute_gap_context));
    UtAssert_UINT32_EQ(CF_ChunkList_ComputeGaps(&clist, TEST_CF_MAX_GAPS, 75, 25, Test_CF_compute_gap_fn, &clist), 2);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].size, 20);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].offset, 30);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[1].size, 15);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[1].offset, 60);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.count, 2);

    /* Check 0-75, limit by TEST_CF_MAX_GAPS */
    memset(&Test_CF_compute_gap_context, 0, sizeof(Test_CF_compute_gap_context));
    UtAssert_UINT32_EQ(CF_ChunkList_ComputeGaps(&clist, TEST_CF_MAX_GAPS, 75, 0, Test_CF_compute_gap_fn, &clist), 3);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].size, 5);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[0].offset, 0);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[1].size, 10);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[1].offset, 10);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[2].size, 20);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.chunks[2].offset, 30);
    UtAssert_UINT32_EQ(Test_CF_compute_gap_context.count, 3);
}

/* Add tests */
void UtTest_Setup(void)
{
    /* Full coverage with just this section of tests */
    TEST_CF_ADD(Test_CF_Chunk_CreateAddReset);
    TEST_CF_ADD(Test_CF_Chunk_Combine);
    TEST_CF_ADD(Test_CF_Chunk_GetRmFirst);
    TEST_CF_ADD(Test_CF_Chunk_ComputeGaps);
}
