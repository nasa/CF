/************************************************************************
 *
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 * Copyright © 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cf_chunk header
 */

#include "cf_chunk.h"
#include "utgenstub.h"

void UT_DefaultHandler_CF_ChunkList_GetFirstChunk(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ChunkListAdd()
 * ----------------------------------------------------
 */
void CF_ChunkListAdd(CF_ChunkList_t *chunks, CF_ChunkOffset_t offset, CF_ChunkSize_t size)
{
    UT_GenStub_AddParam(CF_ChunkListAdd, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_ChunkListAdd, CF_ChunkOffset_t, offset);
    UT_GenStub_AddParam(CF_ChunkListAdd, CF_ChunkSize_t, size);

    UT_GenStub_Execute(CF_ChunkListAdd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ChunkListInit()
 * ----------------------------------------------------
 */
void CF_ChunkListInit(CF_ChunkList_t *chunks, CF_ChunkIdx_t max_chunks, CF_Chunk_t *chunks_mem)
{
    UT_GenStub_AddParam(CF_ChunkListInit, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_ChunkListInit, CF_ChunkIdx_t, max_chunks);
    UT_GenStub_AddParam(CF_ChunkListInit, CF_Chunk_t *, chunks_mem);

    UT_GenStub_Execute(CF_ChunkListInit, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ChunkListReset()
 * ----------------------------------------------------
 */
void CF_ChunkListReset(CF_ChunkList_t *chunks)
{
    UT_GenStub_AddParam(CF_ChunkListReset, CF_ChunkList_t *, chunks);

    UT_GenStub_Execute(CF_ChunkListReset, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ChunkList_ComputeGaps()
 * ----------------------------------------------------
 */
uint32 CF_ChunkList_ComputeGaps(const CF_ChunkList_t *chunks, CF_ChunkIdx_t max_gaps, CF_ChunkSize_t total,
                                CF_ChunkOffset_t start, CF_ChunkList_ComputeGapFn_t compute_gap_fn, void *opaque)
{
    UT_GenStub_SetupReturnBuffer(CF_ChunkList_ComputeGaps, uint32);

    UT_GenStub_AddParam(CF_ChunkList_ComputeGaps, const CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_ChunkList_ComputeGaps, CF_ChunkIdx_t, max_gaps);
    UT_GenStub_AddParam(CF_ChunkList_ComputeGaps, CF_ChunkSize_t, total);
    UT_GenStub_AddParam(CF_ChunkList_ComputeGaps, CF_ChunkOffset_t, start);
    UT_GenStub_AddParam(CF_ChunkList_ComputeGaps, CF_ChunkList_ComputeGapFn_t, compute_gap_fn);
    UT_GenStub_AddParam(CF_ChunkList_ComputeGaps, void *, opaque);

    UT_GenStub_Execute(CF_ChunkList_ComputeGaps, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_ChunkList_ComputeGaps, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ChunkList_GetFirstChunk()
 * ----------------------------------------------------
 */
const CF_Chunk_t *CF_ChunkList_GetFirstChunk(const CF_ChunkList_t *chunks)
{
    UT_GenStub_SetupReturnBuffer(CF_ChunkList_GetFirstChunk, const CF_Chunk_t *);

    UT_GenStub_AddParam(CF_ChunkList_GetFirstChunk, const CF_ChunkList_t *, chunks);

    UT_GenStub_Execute(CF_ChunkList_GetFirstChunk, Basic, UT_DefaultHandler_CF_ChunkList_GetFirstChunk);

    return UT_GenStub_GetReturnValue(CF_ChunkList_GetFirstChunk, const CF_Chunk_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_ChunkList_RemoveFromFirst()
 * ----------------------------------------------------
 */
void CF_ChunkList_RemoveFromFirst(CF_ChunkList_t *chunks, CF_ChunkSize_t size)
{
    UT_GenStub_AddParam(CF_ChunkList_RemoveFromFirst, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_ChunkList_RemoveFromFirst, CF_ChunkSize_t, size);

    UT_GenStub_Execute(CF_ChunkList_RemoveFromFirst, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_CombineNext()
 * ----------------------------------------------------
 */
int CF_Chunks_CombineNext(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    UT_GenStub_SetupReturnBuffer(CF_Chunks_CombineNext, int);

    UT_GenStub_AddParam(CF_Chunks_CombineNext, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_Chunks_CombineNext, CF_ChunkIdx_t, i);
    UT_GenStub_AddParam(CF_Chunks_CombineNext, const CF_Chunk_t *, chunk);

    UT_GenStub_Execute(CF_Chunks_CombineNext, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Chunks_CombineNext, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_CombinePrevious()
 * ----------------------------------------------------
 */
int CF_Chunks_CombinePrevious(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    UT_GenStub_SetupReturnBuffer(CF_Chunks_CombinePrevious, int);

    UT_GenStub_AddParam(CF_Chunks_CombinePrevious, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_Chunks_CombinePrevious, CF_ChunkIdx_t, i);
    UT_GenStub_AddParam(CF_Chunks_CombinePrevious, const CF_Chunk_t *, chunk);

    UT_GenStub_Execute(CF_Chunks_CombinePrevious, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Chunks_CombinePrevious, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_EraseChunk()
 * ----------------------------------------------------
 */
void CF_Chunks_EraseChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t erase_index)
{
    UT_GenStub_AddParam(CF_Chunks_EraseChunk, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_Chunks_EraseChunk, CF_ChunkIdx_t, erase_index);

    UT_GenStub_Execute(CF_Chunks_EraseChunk, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_EraseRange()
 * ----------------------------------------------------
 */
void CF_Chunks_EraseRange(CF_ChunkList_t *chunks, CF_ChunkIdx_t start, CF_ChunkIdx_t end)
{
    UT_GenStub_AddParam(CF_Chunks_EraseRange, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_Chunks_EraseRange, CF_ChunkIdx_t, start);
    UT_GenStub_AddParam(CF_Chunks_EraseRange, CF_ChunkIdx_t, end);

    UT_GenStub_Execute(CF_Chunks_EraseRange, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_FindInsertPosition()
 * ----------------------------------------------------
 */
CF_ChunkIdx_t CF_Chunks_FindInsertPosition(CF_ChunkList_t *chunks, const CF_Chunk_t *chunk)
{
    UT_GenStub_SetupReturnBuffer(CF_Chunks_FindInsertPosition, CF_ChunkIdx_t);

    UT_GenStub_AddParam(CF_Chunks_FindInsertPosition, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_Chunks_FindInsertPosition, const CF_Chunk_t *, chunk);

    UT_GenStub_Execute(CF_Chunks_FindInsertPosition, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Chunks_FindInsertPosition, CF_ChunkIdx_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_FindSmallestSize()
 * ----------------------------------------------------
 */
CF_ChunkIdx_t CF_Chunks_FindSmallestSize(const CF_ChunkList_t *chunks)
{
    UT_GenStub_SetupReturnBuffer(CF_Chunks_FindSmallestSize, CF_ChunkIdx_t);

    UT_GenStub_AddParam(CF_Chunks_FindSmallestSize, const CF_ChunkList_t *, chunks);

    UT_GenStub_Execute(CF_Chunks_FindSmallestSize, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_Chunks_FindSmallestSize, CF_ChunkIdx_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_Insert()
 * ----------------------------------------------------
 */
void CF_Chunks_Insert(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    UT_GenStub_AddParam(CF_Chunks_Insert, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_Chunks_Insert, CF_ChunkIdx_t, i);
    UT_GenStub_AddParam(CF_Chunks_Insert, const CF_Chunk_t *, chunk);

    UT_GenStub_Execute(CF_Chunks_Insert, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_Chunks_InsertChunk()
 * ----------------------------------------------------
 */
void CF_Chunks_InsertChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t index_before, const CF_Chunk_t *chunk)
{
    UT_GenStub_AddParam(CF_Chunks_InsertChunk, CF_ChunkList_t *, chunks);
    UT_GenStub_AddParam(CF_Chunks_InsertChunk, CF_ChunkIdx_t, index_before);
    UT_GenStub_AddParam(CF_Chunks_InsertChunk, const CF_Chunk_t *, chunk);

    UT_GenStub_Execute(CF_Chunks_InsertChunk, Basic, NULL);
}
