/************************************************************************
** File: cf_utils.h
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
**  The CF Application utils header file
**
**
**
*************************************************************************/

#ifndef __CF_UTILS_H_
#define __CF_UTILS_H_

#include "cf_cfdp.h"
#include "cf_assert.h"

/* free a transaction from the queue it's on.
 * NOTE: this leaves the transaction in a bad state,
 * so it must be followed by placing the transaction on
 * another queue. Need this function because the path of
 * freeing a transaction (returning to default state)
 * means that it must be removed from the current queue
 * otherwise if the structure is zero'd out the queue
 * will become corrupted due to other nodes on the queue
 * pointing to an invalid node */
static inline void cf_dequeue_transaction(transaction_t *t)
{
    CF_Assert(t && (t->chan_num < CF_NUM_CHANNELS));
    CF_CList_Remove(&CF_AppData.engine.channels[t->chan_num].qs[t->flags.all.q_index], &t->cl_node);
    CF_Assert(CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.all.q_index]); /* sanity check */
    --CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.all.q_index];
}

static inline void cf_move_transaction(transaction_t *t, cf_queue_index_t q)
{
    CF_Assert(t && (t->chan_num < CF_NUM_CHANNELS));
    CF_CList_Remove(&CF_AppData.engine.channels[t->chan_num].qs[t->flags.all.q_index], &t->cl_node);
    CF_Assert(CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.all.q_index]); /* sanity check */
    --CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.all.q_index];
    CF_CList_InsertBack(&CF_AppData.engine.channels[t->chan_num].qs[q], &t->cl_node);
    t->flags.all.q_index = q;
    ++CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.all.q_index];
}

static inline void CF_CList_Remove_Ex(channel_t *c, cf_queue_index_t index, clist_node node)
{
    CF_CList_Remove(&c->qs[index], node);
    CF_Assert(CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index]); /* sanity check */
    --CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index];
}

static inline void CF_CList_InsertAfter_Ex(channel_t *c, cf_queue_index_t index, clist_node start, clist_node after)
{
    CF_CList_InsertAfter(&c->qs[index], start, after);
    ++CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index];
}

static inline void CF_CList_InsertBack_Ex(channel_t *c, cf_queue_index_t index, clist_node node)
{
    CF_CList_InsertBack(&c->qs[index], node);
    ++CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index];
}

int32 CF_WriteQueueDataToFile(int32 fd, channel_t *c, cf_queue_index_t q);
int32 CF_WriteHistoryQueueDataToFile(int32 fd, channel_t *c, direction_t dir);

void CF_InsertSortPrio(transaction_t *t, cf_queue_index_t q);

typedef void (*CF_TraverseAllTransactions_fn_t)(transaction_t *, void *);
/* these return the number of transactions traversed */
extern int CF_TraverseAllTransactions(channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *);
extern int CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *);

extern int32 CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access);
extern void  CF_WrappedClose(osal_id_t fd);
extern int32 CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size);
extern int32 CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size);
extern int32 CF_WrappedLseek(osal_id_t fd, off_t offset, int mode);

#endif /* !__CF_UTILS_H_ */
