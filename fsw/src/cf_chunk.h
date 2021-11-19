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

#ifndef CF_CHUNK__H
#define CF_CHUNK__H

#include "cfe.h"

typedef uint32 index_t;
typedef uint32 chunk_offset_t;
typedef uint32 chunk_size_t;

/* I talked this over with a friend, Stephen Newell (stephen@sjnewell.com) and he wrote something in c++.
 * I liked it, so converted it to C. Giving credit where it's due. */

typedef struct
{
    chunk_offset_t offset;
    chunk_size_t   size;
} chunk_t;

typedef struct
{
    index_t  count;
    index_t  CF_max_chunks;
    chunk_t *chunks;
} chunks_t;

void CF_Chunks_Init(chunks_t *chunks, index_t CF_max_chunks, chunk_t *chunks_mem);
void CF_Chunks_Add(chunks_t *chunks, chunk_offset_t offset, chunk_size_t size);
void CF_ChunksReset(chunks_t *chunks);
/* CF_Chunks_RemoveFromFirst -
 *
 * Good computer science would have a generic remove function, but that's much more complex
 * than we need for the use case. We aren't trying to make chunks a general purpose
 * reusable module, so just take the simple case that we need.
 *
 * Same applies for CF_Chunks_GetFirstChunk() */
void           CF_Chunks_RemoveFromFirst(chunks_t *chunks, chunk_size_t size);
const chunk_t *CF_Chunks_GetFirstChunk(const chunks_t *chunks);

typedef void (*compute_gap_fn_t)(const chunks_t *cs, const chunk_t *c, void *opaque);
/* returns number of gaps, in case anyone cares about number of gaps */
uint32 CF_Chunks_ComputeGaps(const chunks_t *chunks, index_t max_gaps, chunk_size_t total, chunk_offset_t start,
                             compute_gap_fn_t compute_gap_fn, void *opaque);

#endif /* !CF_CHUNK__H */
