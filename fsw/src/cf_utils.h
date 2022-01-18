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
 *  The CF Application utils header file
 */

#ifndef CF_UTILS_H
#define CF_UTILS_H

#include "cf_cfdp.h"
#include "cf_app.h"
#include "cf_assert.h"

/**
 * @brief Argument structure for use with CList_Traverse()
 *
 * This identifies a specific transaction sequence number and entity ID
 * The transaction pointer is set by the implementation
 */
typedef struct CF_Traverse_TransSeqArg
{
    CF_TransactionSeq_t transaction_sequence_number;
    CF_EntityId_t       src_eid;
    CF_Transaction_t   *t; /**< output transaction pointer */
} CF_Traverse_TransSeqArg_t;

/**
 * @brief Argument structure for use with CF_Traverse_WriteHistoryQueueEntryToFile()
 *
 * This is used for writing status files.  It contains a designated
 * file descriptor for output and counters.
 *
 * When traversing history, the list contains all entries, and may need additional
 * filtering for direction (TX/RX) depending on what information the user has requested.
 */
typedef struct CF_Traverse_WriteHistoryFileArg
{
    osal_id_t      fd;
    CF_Direction_t filter_dir;

    bool   error;   /**< Will be set to true if any write failed */
    uint32 counter; /**< Total number of entries written */
} CF_Traverse_WriteHistoryFileArg_t;

/**
 * @brief Argument structure for use with CF_Traverse_WriteTxnQueueEntryToFile()
 *
 * This is used for writing status files.  It contains a designated
 * file descriptor for output and counters.
 *
 * When traversing transactions, the entire list is written to the file.
 * No additional filtering is necessary, because the queues themselves are
 * limited in what they contain (therefore "pre-filtered" to some degree).
 */
typedef struct CF_Traverse_WriteTxnFileArg
{
    osal_id_t fd;

    bool   error;   /**< Will be set to true if any write failed */
    uint32 counter; /**< Total number of entries written */
} CF_Traverse_WriteTxnFileArg_t;

/**
 * @brief Callback function type for use with CF_TraverseAllTransactions()
 *
 * @param t Pointer to current transaction being traversed
 * @param context Opaque object passed from initial call
 */
typedef void (*CF_TraverseAllTransactions_fn_t)(CF_Transaction_t *t, void *context);

/**
 * @brief Argument structure for use with CF_TraverseAllTransactions()
 *
 * This basically allows for running a CF_Traverse on several lists at once
 */
typedef struct CF_TraverseAll_Arg
{
    CF_TraverseAllTransactions_fn_t fn;      /**< internal callback to use for each CList_Traverse */
    void                           *context; /**< opaque object to pass to internal callback */
    int                             counter; /**< Running tally of all nodes traversed from all lists */
} CF_TraverseAll_Arg_t;

/**
 * @brief Argument structure for use with CF_CList_Traverse_R()
 *
 * This is for searching for transactions of a specific priority
 */
typedef struct CF_Traverse_PriorityArg
{
    CF_Transaction_t *t;        /**< OUT: holds value of transaction with which to call CF_CList_InsertAfter on */
    uint8             priority; /**< seeking this priority */
} CF_Traverse_PriorityArg_t;

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
/** @brief Find an unused transaction on a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL.
 *
 * @param c Pointer to the CF channel
 *
 * @returns Pointer to a free transaction
 * @retval  NULL if no free transactions available.
 */
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *c);

/************************************************************************/
/** @brief Returns a history structure back to its unused state.
 *
 * @par Description
 *       There's nothing to do currently other than remove the history
 *       from its current queue and put it back on CF_QueueIdx_HIST_FREE.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL. h must not be NULL.
 *
 * @param c Pointer to the CF channel
 * @param h Pointer to the history entry
 */
void CF_ResetHistory(CF_Channel_t *c, CF_History_t *h);

/************************************************************************/
/** @brief Frees and resets a transaction and returns it for later use.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t Pointer to the transaction object
 */
void CF_FreeTransaction(CF_Transaction_t *t);

/************************************************************************/
/** @brief Finds an active transaction by sequence number.
 *
 * @par Description
 *       This function traverses the active rx, pending, txa, and txw
 *       transaction and looks for the requested transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL.
 *
 * @param c Pointer to the CF channel
 * @param transaction_sequence_number  Sequence number to find
 * @param src_eid                      Entity ID associated with sequence number
 *
 * @returns Pointer to the given transaction if found
 * @retval  NULL if the transaction is not found
 */
CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *c, CF_TransactionSeq_t transaction_sequence_number,
                                                     CF_EntityId_t src_eid);

/************************************************************************/
/** @brief List traversal function to check if the desired sequence number matches.
 *
 * @par Assumptions, External Events, and Notes:
 *       context must not be NULL. n must not be NULL.
 *
 * @param n         Pointer to node currently being traversed
 * @param context   Pointer to state object passed through from initial call
 *
 * @retval 1 when it's found, which terminates list traversal
 * @retval 0 when it isn't found, which causes list traversal to continue
 *
 */
int CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *n, CF_Traverse_TransSeqArg_t *context);

/************************************************************************/
/** @brief Write a single history to a file.
 *
 * This creates a human-readable/string representation of the information in the
 * history object, and writes that string to the indicated file.
 *
 * This implements the common code between writing the history queue and transaction
 * queue to a file, as both ultimately store the same information in a CF_History_t
 * object, but have a different method to get to it.
 *
 * @par Assumptions, External Events, and Notes:
 *       fd should be a valid file descriptor, open for writing.
 *
 * @param fd Open File descriptor to write to
 * @param h  Pointer to CF history object to write
 *
 * @retval 0 on success
 * @retval -1 on error
 */
int CF_WriteHistoryEntryToFile(osal_id_t fd, const CF_History_t *h);

/************************************************************************/
/** @brief Write a transaction-based queue's transaction history to a file.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL.
 *
 * @param fd Open File descriptor to write to
 * @param c  Pointer to associated CF channel object
 * @param q  Queue Index to write
 *
 * @retval 0 on success
 * @retval 1 on error
 */
int32 CF_WriteTxnQueueDataToFile(osal_id_t fd, CF_Channel_t *c, CF_QueueIdx_t q);

/************************************************************************/
/** @brief Write a history-based queue's entries to a file.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL.
 *
 * @param fd  Open File descriptor to write to
 * @param c   Pointer to associated CF channel object
 * @param dir Direction to match/filter
 *
 * @retval 0 on success
 * @retval 1 on error
 */
int32 CF_WriteHistoryQueueDataToFile(osal_id_t fd, CF_Channel_t *c, CF_Direction_t dir);

/************************************************************************/
/** @brief Insert a transaction into a priority sorted transaction queue.
 *
 * @par Description
 *       This function works by walking the queue in reverse to find a
 *       transaction with a higher priority than the given transaction.
 *       The given transaction is then inserted after that one, since it
 *       would be the next lower priority.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t  Pointer to the transaction object
 * @param q  Index of queue to insert into
 */
void CF_InsertSortPrio(CF_Transaction_t *t, CF_QueueIdx_t q);

/************************************************************************/
/** @brief Traverses all transactions on all active queues and performs an operation on them.
 *
 * @par Assumptions, External Events, and Notes:
 *       c must not be NULL. fn must be a valid function. context must not be NULL.
 *
 * @param c       Channel to operate on
 * @param fn      Callback to invoke for all traversed transactions
 * @param context Opaque object to pass to all callbacks
 *
 * @returns Number of transactions traversed
 */
int CF_TraverseAllTransactions(CF_Channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context);

/************************************************************************/
/** @brief Traverses all transactions on all channels and performs an operation on them.
 *
 * @par Assumptions, External Events, and Notes:
 *       fn must be a valid function. context must not be NULL.
 *
 * @param fn      Callback to invoke for all traversed transactions
 * @param context Opaque object to pass to all callbacks
 *
 * @returns Number of transactions traversed
 */
int CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context);

/************************************************************************/
/** @brief List traversal function performs operation on every active transaction.
 *
 * @par Description
 *       Called on every transaction via list traversal. Calls another function
 *       on that transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       n must not be NULL. args must not be NULL.
 *
 * @param n    Node being currently traversed
 * @param args Intermediate context object from initial call
 *
 * @retval 0 for do not exit early (always continue)
 */
int CF_TraverseAllTransactions_Impl(CF_CListNode_t *n, CF_TraverseAll_Arg_t *args);

/************************************************************************/
/** @brief Writes a human readable representation of a history queue entry to a file
 *
 * This function is a wrapper around CF_WriteHistoryEntryToFile() that can be used with
 * CF_Traverse() to write history queue entries to the file.
 *
 * This also implements a direction filter, so only matching entries are actually written
 * to the file.
 *
 * @par Assumptions, External Events, and Notes:
 *       n must not be NULL. context must not be NULL.
 *
 * @param n       Node being currently traversed
 * @param context Pointer to CF_Traverse_WriteHistoryFileArg_t indicating the file information
 *
 * @retval CF_CLIST_CONT if everything is going well
 * @retval CF_CLIST_EXIT if a write error occurred, which means traversal should stop
 */
int CF_Traverse_WriteHistoryQueueEntryToFile(CF_CListNode_t *n, void *arg);

/************************************************************************/
/** @brief Writes a human readable representation of a transaction history entry to a file
 *
 * This function is a wrapper around CF_WriteHistoryEntryToFile() that can be used with
 * CF_Traverse() to write transaction queue entries to the file.
 *
 * @par Assumptions, External Events, and Notes:
 *       n must not be NULL. context must not be NULL.
 *
 * @param n       Node being currently traversed
 * @param context Pointer to CF_Traverse_WriteTxnFileArg_t indicating the file information
 *
 * @retval CF_CLIST_CONT if everything is going well
 * @retval CF_CLIST_EXIT if a write error occurred, which means traversal should stop
 */
int CF_Traverse_WriteTxnQueueEntryToFile(CF_CListNode_t *n, void *arg);

/************************************************************************/
/** @brief Searches for the first transaction with a lower priority than given.
 *
 * @par Assumptions, External Events, and Notes:
 *       node must not be NULL. context must not be NULL.
 *
 * @param n       Node being currently traversed
 * @param context Pointer to CF_Traverse_PriorityArg_t object indicating the priority to search for
 *
 * @retval CF_CLIST_EXIT when it's found, which terminates list traversal
 * @retval CF_CLIST_CONT when it isn't found, which causes list traversal to continue
 *
 */
int CF_PrioSearch(CF_CListNode_t *node, void *context);

/************************************************************************/
/** @brief Wrap the filesystem open call with a perf counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       fname must not be NULL.
 *
 * @sa OS_OpenCreate() for parameter descriptions
 *
 * @param fd      Passed directly to underlying OSAL call
 * @param fname   Passed directly to underlying OSAL call
 * @param flags   Passed directly to underlying OSAL call
 * @param access  Passed directly to underlying OSAL call
 *
 * @returns Status code from OSAL
 */
int32 CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access);

/************************************************************************/
/** @brief Wrap the filesystem close call with a perf counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @sa OS_close() for parameter descriptions
 *
 * @param fd      Passed directly to underlying OSAL call
 *
 */
void CF_WrappedClose(osal_id_t fd);

/************************************************************************/
/** @brief Wrap the filesystem read call with a perf counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       buf must not be NULL.
 *
 * @sa OS_read() for parameter descriptions
 *
 * @param fd        Passed directly to underlying OSAL call
 * @param buf       Passed directly to underlying OSAL call
 * @param read_size Passed directly to underlying OSAL call
 *
 * @returns Status code from OSAL (byte count or error code)
 */
int32 CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size);

/************************************************************************/
/** @brief Wrap the filesystem write call with a perf counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       buf must not be NULL.
 *
 * @sa OS_write() for parameter descriptions
 *
 * @param fd         Passed directly to underlying OSAL call
 * @param buf        Passed directly to underlying OSAL call
 * @param write_size Passed directly to underlying OSAL call
 *
 * @returns Status code from OSAL (byte count or error code)
 */
int32 CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size);

/************************************************************************/
/** @brief Wrap the filesystem lseek call with a perf counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       fname must not be NULL.
 *
 * @sa OS_lseek() for parameter descriptions
 *
 * @param fd         Passed directly to underlying OSAL call
 * @param offset     Passed directly to underlying OSAL call
 * @param mode       Passed directly to underlying OSAL call
 *
 * @returns Status code from OSAL (byte count or error code)
 */
int32 CF_WrappedLseek(osal_id_t fd, off_t offset, int mode);

#endif /* !CF_UTILS_H */
