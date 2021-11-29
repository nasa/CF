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

typedef struct
{
    osal_id_t fd;
    int32     result;
    int32     counter;
} trav_arg_t;

typedef struct priority_arg_t
{
    transaction_t *t;        /* OUT: holds value of transaction with which to call CF_CList_InsertAfter on */
    uint8          priority; /* seeking this priority */
} priority_arg_t;

typedef struct
{
    CF_TraverseAllTransactions_fn_t fn;
    void                           *context;
    int                             counter;
} traverse_all_args_t;

#define LINEBUF_LEN ((CF_FILENAME_MAX_LEN * 2) + 128)

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
static int CF_TraverseHistory(clist_node n, trav_arg_t *context)
{
    static const char *dstr[] = {"RX", "TX"};
    int                i;
    int32              ret;
    int32              len;
    char               linebuf[LINEBUF_LEN]; /* buffer for line data */
    history_t         *h = container_of(n, history_t, cl_node);

    CF_Assert(h->dir < CF_DIR_NUM);
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
            return CLIST_EXIT;
        }
    }

    return CLIST_CONT;
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
static int CF_TraverseTransactions(clist_node n, trav_arg_t *context)
{
    transaction_t *t = container_of(n, transaction_t, cl_node);

    /* use CF_TraverseHistory to print filenames and direction */
    /* NOTE: ok to ignore return value of CF_TraverseHistory. We care
     * about the value in context->result. The reason for this confusion
     * is CF_TraverseHistory is also a list traversal function. In this
     * function we are just calling it directly. */
    /* ignore return value */ CF_TraverseHistory(&t->history->cl_node, context);
    if (context->result)
    {
        return CLIST_EXIT;
    }

    return CLIST_CONT;
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
int32 CF_WriteQueueDataToFile(int32 fd, channel_t *c, cf_queue_index_t q)
{
    trav_arg_t arg = {fd, 0, 0};
    CF_CList_Traverse(c->qs[q], (clist_fn_t)CF_TraverseTransactions, &arg);
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
int32 CF_WriteHistoryQueueDataToFile(int32 fd, channel_t *c, direction_t dir)
{
    trav_arg_t arg = {fd, 0, 0};
    CF_CList_Traverse(c->qs[CF_Q_HIST], (clist_fn_t)CF_TraverseHistory, &arg);
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
static int CF_PrioSearch(clist_node node, void *context)
{
    transaction_t  *t = container_of(node, transaction_t, cl_node);
    priority_arg_t *p = (priority_arg_t *)context;

    if (t->priority <= p->priority)
    {
        /* found it!
         *
         * the current transaction's prio is less than desired (higher)
         */
        p->t = t;
        return CLIST_EXIT;
    }

    return CLIST_CONT;
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
void CF_InsertSortPrio(transaction_t *t, cf_queue_index_t q)
{
    int        insert_back = 0;
    channel_t *c           = &CF_AppData.engine.channels[t->chan_num];
    CF_Assert(t->chan_num < CF_NUM_CHANNELS);
    CF_Assert(t->state != CFDP_IDLE);

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
    t->flags.all.q_index = q;
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
static int CF_TraverseAllTransactions_(clist_node n, traverse_all_args_t *args)
{
    transaction_t *t = container_of(n, transaction_t, cl_node);
    args->fn(t, args->context);
    ++args->counter;
    return CLIST_CONT;
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
int CF_TraverseAllTransactions(channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    traverse_all_args_t args = {fn, context, 0};
    cf_queue_index_t    index;
    for (index = CF_Q_PEND; index <= CF_Q_RX; ++index)
        CF_CList_Traverse(c->qs[index], (clist_fn_t)CF_TraverseAllTransactions_, &args);

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
