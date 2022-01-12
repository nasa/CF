/************************************************************************
** File: cf_chunk.h
**
** NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
** Application version 3.0.0”
** Copyright © 2019 United States Government as represented by the
** Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
** Licensed under the Apache License, Version 2.0 (the "License"); you may
** not use this file except in compliance with the License. You may obtain
** a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**
** Purpose:
**  The CF Application chunks (spare gap tracking) header file
**
**
**
*************************************************************************/

#ifndef CF_CHUNK_H
#define CF_CHUNK_H

#include "cfe.h"

typedef uint32 CF_ChunkIdx_t;
typedef uint32 CF_ChunkOffset_t;
typedef uint32 CF_ChunkSize_t;

/* I talked this over with a friend, Stephen Newell (stephen@sjnewell.com) and he wrote something in c++.
 * I liked it, so converted it to C. Giving credit where it's due. */

typedef struct CF_Chunk
{
    CF_ChunkOffset_t offset;
    CF_ChunkSize_t   size;
} CF_Chunk_t;

typedef struct CF_ChunkList
{
    CF_ChunkIdx_t count;
    CF_ChunkIdx_t max_chunks;
    CF_Chunk_t   *chunks;
} CF_ChunkList_t;

static inline CF_ChunkOffset_t CF_Chunk_MAX(CF_ChunkOffset_t a, CF_ChunkOffset_t b)
{
    if (a > b)
    {
        return a;
    }
    else
    {
        return b;
    }
}

void CF_ChunkListInit(CF_ChunkList_t *chunks, CF_ChunkIdx_t max_chunks, CF_Chunk_t *chunks_mem);
void CF_ChunkListAdd(CF_ChunkList_t *chunks, CF_ChunkOffset_t offset, CF_ChunkSize_t size);
void CF_ChunkListReset(CF_ChunkList_t *chunks);
/* CF_ChunkList_RemoveFromFirst -
 *
 * Good computer science would have a generic remove function, but that's much more complex
 * than we need for the use case. We aren't trying to make chunks a general purpose
 * reusable module, so just take the simple case that we need.
 *
 * Same applies for CF_ChunkList_GetFirstChunk() */
void              CF_ChunkList_RemoveFromFirst(CF_ChunkList_t *chunks, CF_ChunkSize_t size);
const CF_Chunk_t *CF_ChunkList_GetFirstChunk(const CF_ChunkList_t *chunks);

typedef void (*CF_ChunkList_ComputeGapFn_t)(const CF_ChunkList_t *cs, const CF_Chunk_t *c, void *opaque);
/* returns number of gaps, in case anyone cares about number of gaps */
uint32 CF_ChunkList_ComputeGaps(const CF_ChunkList_t *chunks, CF_ChunkIdx_t max_gaps, CF_ChunkSize_t total,
                                CF_ChunkOffset_t start, CF_ChunkList_ComputeGapFn_t compute_gap_fn, void *opaque);

/* internal functions which need to be unit-tested */
void          CF_Chunks_EraseRange(CF_ChunkList_t *chunks, CF_ChunkIdx_t start, CF_ChunkIdx_t end);
void          CF_Chunks_EraseChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t erase_index);
void          CF_Chunks_InsertChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t index_before, const CF_Chunk_t *chunk);
CF_ChunkIdx_t CF_Chunks_FindInsertPosition(CF_ChunkList_t *chunks, const CF_Chunk_t *chunk);
int           CF_Chunks_CombinePrevious(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk);
int           CF_Chunks_CombineNext(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk);
CF_ChunkIdx_t CF_Chunks_FindSmallestSize(const CF_ChunkList_t *chunks);
void          CF_Chunks_Insert(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk);

#endif /* !CF_CHUNK_H */
