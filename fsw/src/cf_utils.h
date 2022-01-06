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

#ifndef CF_UTILS_H
#define CF_UTILS_H

#include "cf_cfdp.h"
#include "cf_app.h"
#include "cf_assert.h"

typedef struct trans_seq_arg_t
{
    CF_TransactionSeq_t transaction_sequence_number;
    CF_EntityId_t       src_eid;
    CF_Transaction_t   *t; /* out param */
} trans_seq_arg_t;

typedef struct
{
    osal_id_t fd;
    int32     result;
    int32     counter;
} trav_arg_t;

typedef void (*CF_TraverseAllTransactions_fn_t)(CF_Transaction_t *t, void *context);

typedef struct
{
    CF_TraverseAllTransactions_fn_t fn;
    void                           *context;
    int                             counter;
} traverse_all_args_t;

typedef struct priority_arg_t
{
    CF_Transaction_t *t;        /* OUT: holds value of transaction with which to call CF_CList_InsertAfter on */
    uint8             priority; /* seeking this priority */
} priority_arg_t;

/* free a transaction from the queue it's on.
 * NOTE: this leaves the transaction in a bad state,
 * so it must be followed by placing the transaction on
 * another queue. Need this function because the path of
 * freeing a transaction (returning to default state)
 * means that it must be removed from the current queue
 * otherwise if the structure is zero'd out the queue
 * will become corrupted due to other nodes on the queue
 * pointing to an invalid node */
static inline void CF_DequeueTransaction(CF_Transaction_t *t)
{
    CF_Assert(t && (t->chan_num < CF_NUM_CHANNELS));
    CF_CList_Remove(&CF_AppData.engine.channels[t->chan_num].qs[t->flags.com.q_index], &t->cl_node);
    CF_Assert(CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index]); /* sanity check */
    --CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index];
}

static inline void CF_MoveTransaction(CF_Transaction_t *t, CF_QueueIdx_t q)
{
    CF_Assert(t && (t->chan_num < CF_NUM_CHANNELS));
    CF_CList_Remove(&CF_AppData.engine.channels[t->chan_num].qs[t->flags.com.q_index], &t->cl_node);
    CF_Assert(CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index]); /* sanity check */
    --CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index];
    CF_CList_InsertBack(&CF_AppData.engine.channels[t->chan_num].qs[q], &t->cl_node);
    t->flags.com.q_index = q;
    ++CF_AppData.hk.channel_hk[t->chan_num].q_size[t->flags.com.q_index];
}

static inline void CF_CList_Remove_Ex(CF_Channel_t *c, CF_QueueIdx_t index, CF_CListNode_t *node)
{
    CF_CList_Remove(&c->qs[index], node);
    CF_Assert(CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index]); /* sanity check */
    --CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index];
}

static inline void CF_CList_InsertAfter_Ex(CF_Channel_t *c, CF_QueueIdx_t index, CF_CListNode_t *start,
                                           CF_CListNode_t *after)
{
    CF_CList_InsertAfter(&c->qs[index], start, after);
    ++CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index];
}

static inline void CF_CList_InsertBack_Ex(CF_Channel_t *c, CF_QueueIdx_t index, CF_CListNode_t *node)
{
    CF_CList_InsertBack(&c->qs[index], node);
    ++CF_AppData.hk.channel_hk[c - CF_AppData.engine.channels].q_size[index];
}

/************************************************************************/
/** \brief Find an unused transaction on a channel.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retstmt Returns a free transaction, or NULL if none are available. \endcode
**  \endreturns
**
*************************************************************************/
/* finds an unused transaction and returns with it on no Q */
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *c);

/************************************************************************/
/** \brief Returns a history structure back to its unused state.
**
**  \par Description
**       There's nothing to do currently other than remove the history
**       from its current queue and put it back on CF_QueueIdx_HIST_FREE.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL. h must not be NULL.
**
*************************************************************************/
void CF_ResetHistory(CF_Channel_t *c, CF_History_t *h);

/************************************************************************/
/** \brief Frees and resets a transaction and returns it for later use.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_FreeTransaction(CF_Transaction_t *t);

/************************************************************************/
/** \brief Finds an active transaction by sequence number.
**
**  \par Description
**       This function traverses the active rx, pending, txa, and txw
**       transaction and looks for the requested transaction.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retstmt The given transaction is returned if found, otherwise NULL. \endcode
**  \endreturns
**
*************************************************************************/
CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *c, CF_TransactionSeq_t transaction_sequence_number,
                                                     CF_EntityId_t src_eid);

int CF_FindTransactionBySequenceNumber_(CF_CListNode_t *n, trans_seq_arg_t *context);

int32 CF_WriteQueueDataToFile(int32 fd, CF_Channel_t *c, CF_QueueIdx_t q);
int32 CF_WriteHistoryQueueDataToFile(int32 fd, CF_Channel_t *c, CF_Direction_t dir);

void CF_InsertSortPrio(CF_Transaction_t *t, CF_QueueIdx_t q);

/* these return the number of transactions traversed */
int CF_TraverseAllTransactions(CF_Channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context);
int CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context);

int CF_TraverseAllTransactions_(CF_CListNode_t *n, traverse_all_args_t *args);
int CF_TraverseHistory(CF_CListNode_t *n, trav_arg_t *context);
int CF_TraverseTransactions(CF_CListNode_t *n, trav_arg_t *context);

int32 CF_WriteQueueDataToFile(int32 fd, CF_Channel_t *c, CF_QueueIdx_t q);
int32 CF_WriteHistoryQueueDataToFile(int32 fd, CF_Channel_t *c, CF_Direction_t dir);
int   CF_PrioSearch(CF_CListNode_t *node, void *context);

int32 CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access);
void  CF_WrappedClose(osal_id_t fd);
int32 CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size);
int32 CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size);
int32 CF_WrappedLseek(osal_id_t fd, off_t offset, int mode);

#endif /* !CF_UTILS_H */
