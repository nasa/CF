/************************************************************************
** File: cf_utils.c
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
**  The CF Application general utility functions source file
**
**  Various odds and ends are put here.
**
**
**
*************************************************************************/

#include "cf_app.h"
#include "cf_verify.h"
#include "cf_cfdp.h"
#include "cf_utils.h"
#include "cf_events.h"
#include "cf_perfids.h"

#include "cf_assert.h"

#define LINEBUF_LEN ((CF_FILENAME_MAX_LEN * 2) + 128)

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
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *c)
{
    CF_Assert(c);

    if (c->qs[CF_QueueIdx_FREE])
    {
        int q_index; /* initialized below in if */

        CF_CListNode_t   *n = c->qs[CF_QueueIdx_FREE];
        CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);

        CF_CList_Remove_Ex(c, CF_QueueIdx_FREE, &t->cl_node);

        /* now that a transaction is acquired, must also acquire a history slot to go along with it */
        if (c->qs[CF_QueueIdx_HIST_FREE])
        {
            q_index = CF_QueueIdx_HIST_FREE;
        }
        else
        {
            /* no free history, so take the oldest one from the channel's history queue */
            CF_Assert(c->qs[CF_QueueIdx_HIST]);
            q_index = CF_QueueIdx_HIST;
        }

        t->history      = container_of(c->qs[q_index], CF_History_t, cl_node);
        t->history->dir = CF_Direction_NUM; /* start with no direction */

        CF_CList_Remove_Ex(c, q_index, &t->history->cl_node);

        return t;
    }
    else
    {
        return NULL;
    }
}

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
void CF_ResetHistory(CF_Channel_t *c, CF_History_t *h)
{
    CF_CList_Remove_Ex(c, CF_QueueIdx_HIST, &h->cl_node);
    CF_CList_InsertBack_Ex(c, CF_QueueIdx_HIST_FREE, &h->cl_node);
}

/************************************************************************/
/** \brief Frees and resets a transaction and returns it for later use.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_FreeTransaction(CF_Transaction_t *t)
{
    uint8 c = t->chan_num;
    memset(t, 0, sizeof(*t));
    t->flags.com.q_index = CF_QueueIdx_FREE;
    t->fd                = OS_OBJECT_ID_UNDEFINED;
    t->chan_num          = c;
    t->state             = CF_TxnState_IDLE; /* NOTE: this is redundant as long as CF_TxnState_IDLE == 0 */
    CF_CList_InitNode(&t->cl_node);
    CF_CList_InsertBack_Ex(&CF_AppData.engine.channels[c], CF_QueueIdx_FREE, &t->cl_node);
}

/************************************************************************/
/** \brief List traversal function to check if the desired sequence number matches.
**
**  \par Assumptions, External Events, and Notes:
**       context must not be NULL. n must not be NULL.
**
**  \returns
**  \retcode 1 when it's found, which terminates list traversal \endcode
**  \retcode 0 when it isn't found, which causes list traversal to continue \endcode
**  \endreturns
**
*************************************************************************/
int CF_FindTransactionBySequenceNumber_(CF_CListNode_t *n, trans_seq_arg_t *context)
{
    CF_Transaction_t *t   = container_of(n, CF_Transaction_t, cl_node);
    int               ret = 0;

    if ((t->history->src_eid == context->src_eid) && (t->history->seq_num == context->transaction_sequence_number))
    {
        context->t = t;
        ret        = 1; /* exit early */
    }

    return ret;
}

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
                                                     CF_EntityId_t src_eid)
{
    /* need to find transaction by sequence number. It will either be the active transaction (front of Q_PEND),
     * or on Q_TX or Q_RX. Once a transaction moves to history, then it's done.
     *
     * Let's put CF_QueueIdx_RX up front, because most RX packets will be file data PDUs */
    trans_seq_arg_t   ctx    = {transaction_sequence_number, src_eid, NULL};
    CF_CListNode_t   *ptrs[] = {c->qs[CF_QueueIdx_RX], c->qs[CF_QueueIdx_PEND], c->qs[CF_QueueIdx_TXA],
                              c->qs[CF_QueueIdx_TXW]};
    int               i;
    CF_Transaction_t *ret = NULL;

    for (i = 0; i < (sizeof(ptrs) / sizeof(ptrs[0])); ++i)
    {
        CF_CList_Traverse(ptrs[i], (CF_CListFn_t)CF_FindTransactionBySequenceNumber_, &ctx);
        if (ctx.t)
        {
            ret = ctx.t;
            break;
        }
    }

    return ret;
}

/************************************************************************/
/** \brief Walks through a history queue and builds a human readable representation of it.
**
**  \par Description
**       This function is used as both a list traversal function and a direct
**       function call.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL. context must not be NULL.
**
**  \returns
**  \retcode 1 when it's found, which terminates list traversal \endcode
**  \retcode 0 when it isn't found, which causes list traversal to continue \endcode
**  \endreturns
**
*************************************************************************/
int CF_TraverseHistory(CF_CListNode_t *n, trav_arg_t *context)
{
    static const char *dstr[] = {"RX", "TX"};
    int                i;
    int32              ret;
    int32              len;
    char               linebuf[LINEBUF_LEN]; /* buffer for line data */
    CF_History_t      *h = container_of(n, CF_History_t, cl_node);

    CF_Assert(h->dir < CF_Direction_NUM);
    len = snprintf(linebuf, sizeof(linebuf) - 1, "SEQ (%d, %d)\tDIR: %s\tPEER %d\tCC: %d", h->src_eid, h->seq_num,
                   dstr[h->dir], h->peer_eid, h->cc);
    for (i = 0; i < 2; ++i)
    {
        static const char *fstr[]   = {"SRC: %s", "DST: %s"};
        const char        *fnames[] = {h->fnames.src_filename, h->fnames.dst_filename};
        len                         = snprintf(linebuf, sizeof(linebuf) - 1, fstr[i], fnames[i]);
        ret                         = CF_WrappedWrite(context->fd, linebuf, len);
        if (ret != len)
        {
            context->result = 1; /* failed */
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_WHIST_WRITE, CFE_EVS_EventType_ERROR,
                              "CF: writing queue file failed, expected 0x%08x got 0x%08x", len, ret);
            return CF_CLIST_EXIT;
        }
    }

    return CF_CLIST_CONT;
}

/************************************************************************/
/** \brief Walk over all transactions and print information from their history.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode 1 when it's found, which terminates list traversal \endcode
**  \retcode 0 when it isn't found, which causes list traversal to continue \endcode
**  \endreturns
**
*************************************************************************/
int CF_TraverseTransactions(CF_CListNode_t *n, trav_arg_t *context)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);

    /* use CF_TraverseHistory to print filenames and direction */
    /* NOTE: ok to ignore return value of CF_TraverseHistory. We care
     * about the value in context->result. The reason for this confusion
     * is CF_TraverseHistory is also a list traversal function. In this
     * function we are just calling it directly. */
    /* ignore return value */ CF_TraverseHistory(&t->history->cl_node, context);
    if (context->result)
    {
        return CF_CLIST_EXIT;
    }

    return CF_CLIST_CONT;
}

/************************************************************************/
/** \brief Write a transaction-based queue's transaction history to a file.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retstmt 0 on success; 1 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WriteQueueDataToFile(int32 fd, CF_Channel_t *c, CF_QueueIdx_t q)
{
    trav_arg_t arg = {fd, 0, 0};
    CF_CList_Traverse(c->qs[q], (CF_CListFn_t)CF_TraverseTransactions, &arg);
    return arg.result;
}

/************************************************************************/
/** \brief Write a history-based queue's transaction history to a file.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retstmt 0 on success; 1 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WriteHistoryQueueDataToFile(int32 fd, CF_Channel_t *c, CF_Direction_t dir)
{
    trav_arg_t arg = {fd, 0, 0};
    CF_CList_Traverse(c->qs[CF_QueueIdx_HIST], (CF_CListFn_t)CF_TraverseHistory, &arg);
    return arg.result;
}

/************************************************************************/
/** \brief Searches for the first transaction with a lower priority than given.
**
**  \par Description
**        that the config table being loaded has correct data.
**
**  \par Assumptions, External Events, and Notes:
**       node must not be NULL. context must not be NULL.
**
**  \returns
**  \retcode 1 when it's found, which terminates list traversal \endcode
**  \retcode 0 when it isn't found, which causes list traversal to continue \endcode
**  \endreturns
**
*************************************************************************/
int CF_PrioSearch(CF_CListNode_t *node, void *context)
{
    CF_Transaction_t *t = container_of(node, CF_Transaction_t, cl_node);
    priority_arg_t   *p = (priority_arg_t *)context;

    if (t->priority <= p->priority)
    {
        /* found it!
         *
         * the current transaction's prio is less than desired (higher)
         */
        p->t = t;
        return CF_CLIST_EXIT;
    }

    return CF_CLIST_CONT;
}

/************************************************************************/
/** \brief Insert a transaction into a priority sorted transaction queue.
**
**  \par Description
**       This function works by walking the queue in reverse to find a
**       transaction with a higher priority than the given transaction.
**       The given transaction is then inserted after that one, since it
**       would be the next lower priority.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_InsertSortPrio(CF_Transaction_t *t, CF_QueueIdx_t q)
{
    int           insert_back = 0;
    CF_Channel_t *c           = &CF_AppData.engine.channels[t->chan_num];
    CF_Assert(t->chan_num < CF_NUM_CHANNELS);
    CF_Assert(t->state != CF_TxnState_IDLE);

    /* look for proper position on PEND queue for this transaction.
     * This is a simple priority sort. */

    if (!c->qs[q])
    {
        /* list is empty, so just insert */
        insert_back = 1;
    }
    else
    {
        priority_arg_t p = {NULL, t->priority};
        CF_CList_Traverse_R(c->qs[q], CF_PrioSearch, &p);
        if (p.t)
        {
            CF_CList_InsertAfter_Ex(c, q, &p.t->cl_node, &t->cl_node);
        }
        else
        {
            insert_back = 1;
        }
    }

    if (insert_back)
    {
        CF_CList_InsertBack_Ex(c, q, &t->cl_node);
    }
    t->flags.com.q_index = q;
}

/************************************************************************/
/** \brief List traversal function performs operation on every active transaction.
**
**  \par Description
**       Called on every transaction via list traversal. Calls another function
**       on that transaction.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL. args must not be NULL.
**
**  \returns
**  \retstmt Always 0 for do not exit early. \endcode
**  \endreturns
**
*************************************************************************/
int CF_TraverseAllTransactions_(CF_CListNode_t *n, traverse_all_args_t *args)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);
    args->fn(t, args->context);
    ++args->counter;
    return CF_CLIST_CONT;
}

/************************************************************************/
/** \brief Traverses all transactions on all active queues and performs an operation on them.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL. fn must be a valid function. context must not be NULL.
**
**  \returns
**  \retstmt Number of transactions traversed, or anything else on error. \endcode
**  \endreturns
**
*************************************************************************/
int CF_TraverseAllTransactions(CF_Channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    traverse_all_args_t args = {fn, context, 0};
    CF_QueueIdx_t       index;
    for (index = CF_QueueIdx_PEND; index <= CF_QueueIdx_RX; ++index)
        CF_CList_Traverse(c->qs[index], (CF_CListFn_t)CF_TraverseAllTransactions_, &args);

    return args.counter;
}

/************************************************************************/
/** \brief Traverses all transactions on all channels and performs an operation on them.
**
**  \par Assumptions, External Events, and Notes:
**       fn must be a valid function. context must not be NULL.
**
**  \returns
**  \retstmt Number of transactions traversed, or anything else on error. \endcode
**  \endreturns
**
*************************************************************************/
int CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context)
{
    int i;
    int ret = 0;
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
        ret += CF_TraverseAllTransactions(CF_AppData.engine.channels + i, fn, context);
    return ret;
}

/************************************************************************/
/** \brief Wrap the filesystem open call with a perf counter.
**
**  \par Assumptions, External Events, and Notes:
**       fname must not be NULL.
**
**  \returns
**  \retstmt Valid file descriptor, or anything else on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FOPEN);
    ret = OS_OpenCreate(fd, fname, flags, access);
    CFE_ES_PerfLogExit(CF_PERF_ID_FOPEN);
    return ret;
}

/************************************************************************/
/** \brief Wrap the filesystem close call with a perf counter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void CF_WrappedClose(osal_id_t fd)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FCLOSE);
    ret = OS_close(fd);
    CFE_ES_PerfLogExit(CF_PERF_ID_FCLOSE);

    if (ret != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_CLOSE_ERR, CFE_EVS_EventType_ERROR,
                          "CF: failed to close file 0x%x, OS_close returned 0x%08x", fd, ret);
    }
}

/************************************************************************/
/** \brief Wrap the filesystem read call with a perf counter.
**
**  \par Assumptions, External Events, and Notes:
**       buf must not be NULL.
**
**  \returns
**  \retstmt >=0 number of bytes read on success \endcode
**  \retstmt <0 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FREAD);
    ret = OS_read(fd, buf, read_size);
    CFE_ES_PerfLogExit(CF_PERF_ID_FREAD);
    return ret;
}

/************************************************************************/
/** \brief Wrap the filesystem write call with a perf counter.
**
**  \par Assumptions, External Events, and Notes:
**       buf must not be NULL.
**
**  \returns
**  \retstmt >=0 number of bytes read on success \endcode
**  \retstmt <0 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FWRITE);
    ret = OS_write(fd, buf, write_size);
    CFE_ES_PerfLogExit(CF_PERF_ID_FWRITE);
    return ret;
}

/************************************************************************/
/** \brief Wrap the filesystem lseek call with a perf counter.
**
**  \par Assumptions, External Events, and Notes:
**       fname must not be NULL.
**
**  \returns
**  \retstmt >=0 the current file position in bytes. \endcode
**  \retstmt <0 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WrappedLseek(osal_id_t fd, off_t offset, int mode)
{
    int ret;
    CFE_ES_PerfLogEntry(CF_PERF_ID_FSEEK);
    ret = OS_lseek(fd, offset, mode);
    CFE_ES_PerfLogExit(CF_PERF_ID_FSEEK);
    return ret;
}
