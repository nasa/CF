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
 *  The CF Application chunks (sparse gap tracking) logic file
 *
 *  This class handles the complexity of sparse gap tracking so that
 *  the CFDP engine doesn't need to worry about it. Information is given
 *  to the class and when needed calculations are made internally to
 *  help the engine build NAK packets. Received NAK segmnent requests
 *  are stored in this class as well and used for re-transmit processing.
 *
 *  This is intended to be mostly a generic purpose class used by CF.
 */

#include <string.h>
#include "cf_verify.h"
#include "cf_assert.h"
#include "cf_chunk.h"

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_EraseRange
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_Chunks_EraseRange(CF_ChunkList_t *chunks, CF_ChunkIdx_t start, CF_ChunkIdx_t end)
{
    CF_Assert(end >= start);
    if (start < chunks->count)
    {
        memmove(&chunks->chunks[start], &chunks->chunks[end], sizeof(*chunks->chunks) * (end - start));
        chunks->count -= (end - start);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_EraseChunk
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_Chunks_EraseChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t erase_index)
{
    CF_Assert(chunks->count > 0);
    CF_Assert(erase_index < chunks->count);

    /* to erase, move memory over the old one */
    memmove(&chunks->chunks[erase_index], &chunks->chunks[erase_index + 1],
            sizeof(*chunks->chunks) * (chunks->count - 1 - erase_index));
    --chunks->count;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_InsertChunk
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_Chunks_InsertChunk(CF_ChunkList_t *chunks, CF_ChunkIdx_t index_before, const CF_Chunk_t *chunk)
{
    CF_Assert(chunks->count < chunks->max_chunks);
    CF_Assert(index_before <= chunks->count);

    if (chunks->count && (index_before != chunks->count))
    {
        memmove(&chunks->chunks[index_before + 1], &chunks->chunks[index_before],
                sizeof(*chunk) * (chunks->count - index_before));
    }
    memcpy(&chunks->chunks[index_before], chunk, sizeof(*chunk));

    ++chunks->count;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_FindInsertPosition
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChunkIdx_t CF_Chunks_FindInsertPosition(CF_ChunkList_t *chunks, const CF_Chunk_t *chunk)
{
    CF_ChunkIdx_t first = 0;
    CF_ChunkIdx_t i;
    CF_ChunkIdx_t count = chunks->count;
    CF_ChunkIdx_t step;

    while (count > 0)
    {
        i    = first;
        step = count / 2;
        i += step;
        if (chunks->chunks[i].offset < chunk->offset)
        {
            first = i + 1;
            count -= step + 1;
        }
        else
        {
            count = step;
        }
    }

    return first;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_CombinePrevious
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_Chunks_CombinePrevious(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    int ret = 0;
    CF_Assert(i <= chunks->max_chunks);

    CF_ChunkOffset_t chunk_end = chunk->offset + chunk->size;

    if ((i && chunks->count))
    {
        CF_Chunk_t      *prev     = &chunks->chunks[i - 1];
        CF_ChunkOffset_t prev_end = prev->offset + prev->size;
        if (chunk->offset <= prev_end)
        {
            if (prev_end < chunk_end)
            {
                /* extend the previous chunk, then see if we can combine */
                prev->size = chunk_end - prev->offset;
                ret        = 1;
            }
        }
    }
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_CombineNext
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_Chunks_CombineNext(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    /* check if not at the end */
    int ret = 0;
    if (i != chunks->count)
    {
        CF_ChunkOffset_t chunk_end = chunk->offset + chunk->size;
        CF_Assert(chunk_end >= chunk->offset);

        /* check if anything can be combined */
        if (chunks->chunks[i].offset <= chunk_end)
        {
            /* figure out how many chunks can be combined */
            CF_ChunkIdx_t    combined_i = i;
            CF_ChunkOffset_t new_end; /* initialized below */

            for (; combined_i < chunks->count; ++combined_i)
            {
                CF_ChunkOffset_t existing_end = chunks->chunks[combined_i].offset + chunks->chunks[combined_i].size;
                if (chunk_end < existing_end)
                {
                    break;
                }
            }

            if (combined_i != chunks->count)
            {
                /* not at the end */
                new_end = CF_Chunk_MAX(chunks->chunks[combined_i].offset + chunks->chunks[combined_i].size, chunk_end);
            }
            else
            {
                /* new chunk replaces everything until the end */
                new_end = CF_Chunk_MAX(
                    chunks->chunks[chunks->count - 1].offset + chunks->chunks[chunks->count - 1].size, chunk_end);
            }

            chunks->chunks[i].size   = new_end - chunk->offset;
            chunks->chunks[i].offset = chunk->offset;

            if (i != combined_i)
            {
                /* erase the next chunk */
                CF_Chunks_EraseRange(chunks, i + 1, combined_i);
            }
            ret = 1;
        }
    }
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_FindSmallestSize
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_ChunkIdx_t CF_Chunks_FindSmallestSize(const CF_ChunkList_t *chunks)
{
    CF_ChunkIdx_t i;
    CF_ChunkIdx_t smallest = 0;

    for (i = 1; i < chunks->count; ++i)
    {
        if (chunks->chunks[i].size < chunks->chunks[smallest].size)
        {
            smallest = i;
        }
    }

    return smallest;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Chunks_Insert
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_Chunks_Insert(CF_ChunkList_t *chunks, CF_ChunkIdx_t i, const CF_Chunk_t *chunk)
{
    int n = CF_Chunks_CombineNext(chunks, i, chunk);
    if (n)
    {
        int combined = CF_Chunks_CombinePrevious(chunks, i, &chunks->chunks[i]);
        if (combined)
        {
            CF_Chunks_EraseChunk(chunks, i);
        }
    }
    else
    {
        int combined = CF_Chunks_CombinePrevious(chunks, i, chunk);
        if (!combined)
        {
            if (chunks->count < chunks->max_chunks)
            {
                CF_Chunks_InsertChunk(chunks, i, chunk);
            }
            else
            {
                CF_ChunkIdx_t smallest_i = CF_Chunks_FindSmallestSize(chunks);
                CF_Chunk_t   *smallest_c = &chunks->chunks[smallest_i];
                if (smallest_c->size < chunk->size)
                {
                    CF_Chunks_EraseChunk(chunks, smallest_i);
                    CF_Chunks_InsertChunk(chunks, CF_Chunks_FindInsertPosition(chunks, chunk), chunk);
                }
            }
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_ChunkListAdd
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ChunkListAdd(CF_ChunkList_t *chunks, CF_ChunkOffset_t offset, CF_ChunkSize_t size)
{
    const CF_Chunk_t    chunk = {offset, size};
    const CF_ChunkIdx_t i     = CF_Chunks_FindInsertPosition(chunks, &chunk);

    /* PTFO: files won't be so big we need to gracefully handle overflow,
     * and in that case the user should change everything in chunks
     * to use 64-bit numbers */
    CF_Assert((offset + size) >= offset);

    if (i != chunks->count)
    {
        /* not at the end */
        CF_Assert(chunk.offset <= chunks->chunks[i].offset);
        CF_Chunks_Insert(chunks, i, &chunk);
    }
    else
    {
        CF_Chunks_Insert(chunks, i, &chunk);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_ChunkList_RemoveFromFirst
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ChunkList_RemoveFromFirst(CF_ChunkList_t *chunks, CF_ChunkSize_t size)
{
    CF_Chunk_t *c = &chunks->chunks[0]; /* front is always 0 */

    if (size > c->size)
    {
        size = c->size;
    }
    c->size -= size;

    if (!c->size)
    {
        CF_Chunks_EraseChunk(chunks, 0);
    }
    else
    {
        c->offset += size;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_ChunkList_GetFirstChunk
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
const CF_Chunk_t *CF_ChunkList_GetFirstChunk(const CF_ChunkList_t *chunks)
{
    return chunks->count ? &chunks->chunks[0] : NULL;
}

/*----------------------------------------------------------------
 *
 * Function: CF_ChunkListInit
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ChunkListInit(CF_ChunkList_t *chunks, CF_ChunkIdx_t max_chunks, CF_Chunk_t *chunks_mem)
{
    CF_Assert(max_chunks > 0);
    chunks->max_chunks = max_chunks;
    chunks->chunks     = chunks_mem;
    CF_ChunkListReset(chunks);
}

/*----------------------------------------------------------------
 *
 * Function: CF_ChunkListReset
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ChunkListReset(CF_ChunkList_t *chunks)
{
    chunks->count = 0;
    memset(chunks->chunks, 0, sizeof(*chunks->chunks) * chunks->max_chunks);
}

/*----------------------------------------------------------------
 *
 * Function: CF_ChunkList_ComputeGaps
 *
 * Application-scope internal function
 * See description in cf_chunk.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CF_ChunkList_ComputeGaps(const CF_ChunkList_t *chunks, CF_ChunkIdx_t max_gaps, CF_ChunkSize_t total,
                                CF_ChunkOffset_t start, CF_ChunkList_ComputeGapFn_t compute_gap_fn, void *opaque)
{
    uint32        ret     = 0;
    CF_ChunkIdx_t i       = 0;
    int           started = 0;
    CF_Assert(total); /* does it make sense to have a 0 byte file? */
    CF_Assert(start < total);

    /* simple case: there is no chunk data, which means there is a single gap of the entire size */
    if (!chunks->count)
    {
        CF_Chunk_t c = {0, total};
        if (compute_gap_fn)
        {
            compute_gap_fn(chunks, &c, opaque);
        }
        ret = 1;
        goto err_out;
    }

    while ((ret < max_gaps) && (i < chunks->count))
    {
        CF_ChunkOffset_t next_off  = (i == (chunks->count - 1)) ? total : chunks->chunks[i + 1].offset;
        CF_ChunkOffset_t gap_start = (chunks->chunks[i].offset + chunks->chunks[i].size);
        CF_ChunkSize_t   gap_size  = (next_off - gap_start);
        CF_Chunk_t       c         = {gap_start, gap_size};

        if (gap_start >= total)
        {
            break;
        }

        /* check if start has been passed */
        if (!started && ((c.offset + c.size) >= start))
        {
            CF_ChunkSize_t start_diff = (start - c.offset);
            if (start_diff < c.offset)
            {
                c.offset += start_diff;
                c.size -= start_diff;
            }
            started = 1;
        }

        if (started)
        {
            if (compute_gap_fn)
            {
                compute_gap_fn(chunks, &c, opaque);
            }
            ++ret;
        }

        ++i;
    }

err_out:
    return ret;
}
