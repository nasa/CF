/************************************************************************
** File: cf_chunk.c
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
**  The CF Application chunks (sparse gap tracking) logic file
**
**  This class handles the complexity of sparse gap tracking so that
**  the CFDP engine doesn't need to worry about it. Information is given
**  to the class and when needed calculations are made internally to
**  help the engine build NAK packets. Received NAK segmnent requests
**  are stored in this class as well and used for re-transmit processing.
**
**  This is intended to be mostly a generic purpose class used by CF.
**
**
**
*************************************************************************/

/* Most of this was originally written by Stephen Newell stephen@sjnewell.com
 * who wrote it responding to my asking him about the problem. He wrote it
 * in C++ and I (Steven Seeger) ported it to C and fixed a couple bugs and
 * added some stuff.
 *
 * This is a pretty generic implemenation of a solution to the problem of
 * sparse gap tracking over a linear range. */
#include <string.h>
#include "cf_verify.h"
#include "cf_assert.h"
#include "cf_chunk.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

/************************************************************************/
/** \brief Erase a range of chunks.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
static void CF_Chunks_EraseRange(chunks_t *chunks, index_t start, index_t end)
{
    CF_Assert(end >= start);
    if (start < chunks->count)
    {
        memmove(&chunks->chunks[start], &chunks->chunks[end], sizeof(*chunks->chunks) * (end - start));
        chunks->count -= (end - start);
    }
}

/************************************************************************/
/** \brief Erase a single chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
static void CF_Chunks_EraseChunk(chunks_t *chunks, index_t erase_index)
{
    CF_Assert(chunks->count > 0);
    CF_Assert(erase_index < chunks->count);

    /* to erase, move memory over the old one */
    memmove(&chunks->chunks[erase_index], &chunks->chunks[erase_index + 1],
            sizeof(*chunks->chunks) * (chunks->count - 1 - erase_index));
    --chunks->count;
}

/************************************************************************/
/** \brief Insert a chunk before index_before.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
*************************************************************************/
static void CF_Chunks_InsertChunk(chunks_t *chunks, index_t index_before, const chunk_t *chunk)
{
    CF_Assert(chunks->count < chunks->CF_max_chunks);
    CF_Assert(index_before <= chunks->count);

    if (chunks->count && (index_before != chunks->count))
    {
        memmove(&chunks->chunks[index_before + 1], &chunks->chunks[index_before],
                sizeof(*chunk) * (chunks->count - index_before));
    }
    memcpy(&chunks->chunks[index_before], chunk, sizeof(*chunk));

    ++chunks->count;
}

/************************************************************************/
/** \brief Finds where a chunk should be inserted in the chunks.
**
**  \par Description
**       This is a C version of std::lower_bound from C++ algorithms.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
**  \returns
**  \retstmt Returns an index to the first chunk that is greater than or equal to the requested's offset. \endcode
**  \endreturns
**
*************************************************************************/
static index_t CF_Chunks_FindInsertPosition(chunks_t *chunks, const chunk_t *chunk)
{
    index_t first = 0;
    index_t i;
    index_t count = chunks->count;
    index_t step;

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

/************************************************************************/
/** \brief Possibly combines the given chunk with the previous chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
**  \returns
**  \retstmt Returns 1 if combined with another chunk; otherwise 0. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_Chunks_CombinePrevious(chunks_t *chunks, index_t i, const chunk_t *chunk)
{
    int ret = 0;
    CF_Assert(i <= chunks->CF_max_chunks);

    chunk_offset_t chunk_end = chunk->offset + chunk->size;

    if ((i && chunks->count))
    {
        chunk_t       *prev     = &chunks->chunks[i - 1];
        chunk_offset_t prev_end = prev->offset + prev->size;
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

/************************************************************************/
/** \brief Possibly combines the given chunk with the next chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
**  \returns
**  \retstmt Returns 1 if combined with another chunk; otherwise 0. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_Chunks_CombineNext(chunks_t *chunks, index_t i, const chunk_t *chunk)
{
    /* check if not at the end */
    int ret = 0;
    if (i != chunks->count)
    {
        chunk_offset_t chunk_end = chunk->offset + chunk->size;
        CF_Assert(chunk_end >= chunk->offset);

        /* check if anything can be combined */
        if (chunks->chunks[i].offset <= chunk_end)
        {
            /* figure out how many chunks can be combined */
            index_t        combined_i = i;
            chunk_offset_t new_end; /* initialized below */

            for (; combined_i < chunks->count; ++combined_i)
            {
                chunk_offset_t existing_end = chunks->chunks[combined_i].offset + chunks->chunks[combined_i].size;
                if (chunk_end < existing_end)
                {
                    break;
                }
            }

            if (combined_i != chunks->count)
            {
                /* not at the end */
                new_end = MAX(chunks->chunks[combined_i].offset + chunks->chunks[combined_i].size, chunk_end);
            }
            else
            {
                /* new chunk replaces everything until the end */
                new_end =
                    MAX(chunks->chunks[chunks->count - 1].offset + chunks->chunks[chunks->count - 1].size, chunk_end);
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

/************************************************************************/
/** \brief Finds the smallest size out of all chunks.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
**  \returns
**  \retstmt The chunk index with the smallest size. \endcode
**  \endreturns
**
*************************************************************************/
static index_t CF_Chunks_FindSmallestSize(const chunks_t *chunks)
{
    index_t i;
    index_t smallest = 0;

    for (i = 1; i < chunks->count; ++i)
    {
        if (chunks->chunks[i].size < chunks->chunks[smallest].size)
        {
            smallest = i;
        }
    }

    return smallest;
}

/************************************************************************/
/** \brief Insert a chunk.
**
**  \par Description
**       Finds the correct insertion point for a chunk. May combine with
**       an existing chunk if contiguous.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunk must not be NULL.
**
*************************************************************************/
static void CF_Chunks_Insert(chunks_t *chunks, index_t i, const chunk_t *chunk)
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
            if (chunks->count < chunks->CF_max_chunks)
            {
                CF_Chunks_InsertChunk(chunks, i, chunk);
            }
            else
            {
                index_t  smallest_i = CF_Chunks_FindSmallestSize(chunks);
                chunk_t *smallest_c = &chunks->chunks[smallest_i];
                if (smallest_c->size < chunk->size)
                {
                    CF_Chunks_EraseChunk(chunks, smallest_i);
                    CF_Chunks_InsertChunk(chunks, CF_Chunks_FindInsertPosition(chunks, chunk), chunk);
                }
            }
        }
    }
}

/************************************************************************/
/** \brief Public function to add a chunk.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
void CF_Chunks_Add(chunks_t *chunks, chunk_offset_t offset, chunk_size_t size)
{
    const chunk_t chunk = {offset, size};
    const index_t i     = CF_Chunks_FindInsertPosition(chunks, &chunk);

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

/************************************************************************/
/** \brief Public function to remove some amount of size from the first chunk.
**
**  \par Description
**       This may remove the chunk entirely. This function is to satisfy the
**       use-case where data is retrieved from the structure in-order and
**       once consumed should be removed.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
void CF_Chunks_RemoveFromFirst(chunks_t *chunks, chunk_size_t size)
{
    chunk_t *c = &chunks->chunks[0]; /* front is always 0 */

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

/************************************************************************/
/** \brief Public function to remove some amount of size from the first chunk.
**
**  \par Description
**       This may remove the chunk entirely. This function is to satisfy the
**       use-case where data is retrieved from the structure in-order and
**       once consumed should be removed.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
const chunk_t *CF_Chunks_GetFirstChunk(const chunks_t *chunks)
{
    return chunks->count ? &chunks->chunks[0] : NULL;
}

/************************************************************************/
/** \brief Initialize a chunks structure.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. chunks_mem must not be NULL.
**
*************************************************************************/
void CF_Chunks_Init(chunks_t *chunks, index_t CF_max_chunks, chunk_t *chunks_mem)
{
    CF_Assert(CF_max_chunks > 0);
    chunks->CF_max_chunks = CF_max_chunks;
    chunks->chunks        = chunks_mem;
    CF_ChunksReset(chunks);
}

/************************************************************************/
/** \brief Resets a chunks structure.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL.
**
*************************************************************************/
void CF_ChunksReset(chunks_t *chunks)
{
    chunks->count = 0;
    memset(chunks->chunks, 0, sizeof(*chunks->chunks) * chunks->CF_max_chunks);
}

/************************************************************************/
/** \brief Compute gaps between chunks, and call a callback for each.
**
**  \par Description
**       This function walks over all chunks and computes the gaps between.
**       It can exit early if the calculated gap start is larger than the
**       desired total.
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. compute_gap_fn is a valid function address.
**
**  \returns
**  \retstmt The number of computed gaps. \endcode
**  \endreturns
**
*************************************************************************/
uint32 CF_Chunks_ComputeGaps(const chunks_t *chunks, index_t max_gaps, chunk_size_t total, chunk_offset_t start,
                             compute_gap_fn_t compute_gap_fn, void *opaque)
{
    uint32  ret     = 0;
    index_t i       = 0;
    int     started = 0;
    CF_Assert(total); /* does it make sense to have a 0 byte file? */
    CF_Assert(start < total);

    /* simple case: there is no chunk data, which means there is a single gap of the entire size */
    if (!chunks->count)
    {
        chunk_t c = {0, total};
        if (compute_gap_fn)
        {
            compute_gap_fn(chunks, &c, opaque);
        }
        ret = 1;
        goto err_out;
    }

    while ((ret < max_gaps) && (i < chunks->count))
    {
        chunk_offset_t next_off  = (i == (chunks->count - 1)) ? total : chunks->chunks[i + 1].offset;
        chunk_offset_t gap_start = (chunks->chunks[i].offset + chunks->chunks[i].size);
        chunk_size_t   gap_size  = (next_off - gap_start);
        chunk_t        c         = {gap_start, gap_size};

        if (gap_start >= total)
        {
            break;
        }

        /* check if start has been passed */
        if (!started && ((c.offset + c.size) >= start))
        {
            chunk_size_t start_diff = (start - c.offset);
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
