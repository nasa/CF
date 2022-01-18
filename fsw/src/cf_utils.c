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
 *  The CF Application general utility functions source file
 *
 *  Various odds and ends are put here.
 */

#include "cf_app.h"
#include "cf_verify.h"
#include "cf_cfdp.h"
#include "cf_utils.h"
#include "cf_events.h"
#include "cf_perfids.h"

#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Function: CF_FindUnusedTransaction
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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

/*----------------------------------------------------------------
 *
 * Function: CF_ResetHistory
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ResetHistory(CF_Channel_t *c, CF_History_t *h)
{
    CF_CList_Remove_Ex(c, CF_QueueIdx_HIST, &h->cl_node);
    CF_CList_InsertBack_Ex(c, CF_QueueIdx_HIST_FREE, &h->cl_node);
}

/*----------------------------------------------------------------
 *
 * Function: CF_FreeTransaction
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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

/*----------------------------------------------------------------
 *
 * Function: CF_FindTransactionBySequenceNumber_Impl
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *n, CF_Traverse_TransSeqArg_t *context)
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

/*----------------------------------------------------------------
 *
 * Function: CF_FindTransactionBySequenceNumber
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Transaction_t *CF_FindTransactionBySequenceNumber(CF_Channel_t *c, CF_TransactionSeq_t transaction_sequence_number,
                                                     CF_EntityId_t src_eid)
{
    /* need to find transaction by sequence number. It will either be the active transaction (front of Q_PEND),
     * or on Q_TX or Q_RX. Once a transaction moves to history, then it's done.
     *
     * Let's put CF_QueueIdx_RX up front, because most RX packets will be file data PDUs */
    CF_Traverse_TransSeqArg_t ctx    = {transaction_sequence_number, src_eid, NULL};
    CF_CListNode_t           *ptrs[] = {c->qs[CF_QueueIdx_RX], c->qs[CF_QueueIdx_PEND], c->qs[CF_QueueIdx_TXA],
                              c->qs[CF_QueueIdx_TXW]};
    int                       i;
    CF_Transaction_t         *ret = NULL;

    for (i = 0; i < (sizeof(ptrs) / sizeof(ptrs[0])); ++i)
    {
        CF_CList_Traverse(ptrs[i], (CF_CListFn_t)CF_FindTransactionBySequenceNumber_Impl, &ctx);
        if (ctx.t)
        {
            ret = ctx.t;
            break;
        }
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WriteHistoryEntryToFile
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_WriteHistoryEntryToFile(osal_id_t fd, const CF_History_t *h)
{
    static const char *CF_DSTR[] = {"RX", "TX"}; /* conversion of CF_Direction_t to string */

    int   i;
    int32 ret;
    int32 len;
    char  linebuf[(CF_FILENAME_MAX_LEN * 2) + 128]; /* buffer for line data */

    for (i = 0; i < 3; ++i)
    {
        switch (i)
        {
            case 0:
                CF_Assert(h->dir < CF_Direction_NUM);
                snprintf(linebuf, sizeof(linebuf), "SEQ (%lu, %lu)\tDIR: %s\tPEER %lu\tCC: %u\t",
                         (unsigned long)h->src_eid, (unsigned long)h->seq_num, CF_DSTR[h->dir],
                         (unsigned long)h->peer_eid, (unsigned int)h->cc);
                break;
            case 1:
                snprintf(linebuf, sizeof(linebuf), "SRC: %s\t", h->fnames.src_filename);
                break;
            case 2:
                snprintf(linebuf, sizeof(linebuf), "DST: %s\n", h->fnames.dst_filename);
                break;
        }

        len = strlen(linebuf);
        ret = CF_WrappedWrite(fd, linebuf, len);
        if (ret != len)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_WHIST_WRITE, CFE_EVS_EventType_ERROR,
                              "CF: writing queue file failed, expected %ld got %ld", (long)len, (long)ret);
            return -1;
        }
    }

    return 0;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Traverse_WriteHistoryQueueEntryToFile
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_Traverse_WriteHistoryQueueEntryToFile(CF_CListNode_t *n, void *arg)
{
    CF_Traverse_WriteHistoryFileArg_t *context = arg;
    CF_History_t                      *h       = container_of(n, CF_History_t, cl_node);

    /* if filter_dir is CF_Direction_NUM, this means both directions (all match) */
    if (context->filter_dir == CF_Direction_NUM || h->dir == context->filter_dir)
    {
        if (CF_WriteHistoryEntryToFile(context->fd, h) < 0)
        {
            /* failed */
            context->error = true;
            return CF_CLIST_EXIT;
        }

        ++context->counter;
    }

    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Traverse_WriteTxnQueueEntryToFile
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_Traverse_WriteTxnQueueEntryToFile(CF_CListNode_t *n, void *arg)
{
    CF_Traverse_WriteTxnFileArg_t *context = arg;
    CF_Transaction_t              *t       = container_of(n, CF_Transaction_t, cl_node);

    if (CF_WriteHistoryEntryToFile(context->fd, t->history) < 0)
    {
        /* failed */
        context->error = true;
        return CF_CLIST_EXIT;
    }

    ++context->counter;
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WriteTxnQueueDataToFile
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_WriteTxnQueueDataToFile(osal_id_t fd, CF_Channel_t *c, CF_QueueIdx_t q)
{
    CF_Traverse_WriteTxnFileArg_t arg;

    arg.fd      = fd;
    arg.error   = false;
    arg.counter = 0;

    CF_CList_Traverse(c->qs[q], CF_Traverse_WriteTxnQueueEntryToFile, &arg);
    return arg.error;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WriteHistoryQueueDataToFile
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_WriteHistoryQueueDataToFile(osal_id_t fd, CF_Channel_t *c, CF_Direction_t dir)
{
    CF_Traverse_WriteHistoryFileArg_t arg;

    arg.fd         = fd;
    arg.filter_dir = dir;
    arg.error      = false;
    arg.counter    = 0;

    CF_CList_Traverse(c->qs[CF_QueueIdx_HIST], CF_Traverse_WriteHistoryQueueEntryToFile, &arg);
    return arg.error;
}

/*----------------------------------------------------------------
 *
 * Function: CF_PrioSearch
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_PrioSearch(CF_CListNode_t *node, void *context)
{
    CF_Transaction_t          *t = container_of(node, CF_Transaction_t, cl_node);
    CF_Traverse_PriorityArg_t *p = (CF_Traverse_PriorityArg_t *)context;

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

/*----------------------------------------------------------------
 *
 * Function: CF_InsertSortPrio
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
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
        CF_Traverse_PriorityArg_t p = {NULL, t->priority};
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

/*----------------------------------------------------------------
 *
 * Function: CF_TraverseAllTransactions_Impl
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_TraverseAllTransactions_Impl(CF_CListNode_t *n, CF_TraverseAll_Arg_t *args)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);
    args->fn(t, args->context);
    ++args->counter;
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Function: CF_TraverseAllTransactions
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_TraverseAllTransactions(CF_Channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    CF_TraverseAll_Arg_t args = {fn, context, 0};
    CF_QueueIdx_t        index;
    for (index = CF_QueueIdx_PEND; index <= CF_QueueIdx_RX; ++index)
        CF_CList_Traverse(c->qs[index], (CF_CListFn_t)CF_TraverseAllTransactions_Impl, &args);

    return args.counter;
}

/*----------------------------------------------------------------
 *
 * Function: CF_TraverseAllTransactions_All_Channels
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context)
{
    int i;
    int ret = 0;
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
        ret += CF_TraverseAllTransactions(CF_AppData.engine.channels + i, fn, context);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WrappedOpenCreate
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FOPEN);
    ret = OS_OpenCreate(fd, fname, flags, access);
    CFE_ES_PerfLogExit(CF_PERF_ID_FOPEN);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WrappedClose
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_WrappedClose(osal_id_t fd)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FCLOSE);
    ret = OS_close(fd);
    CFE_ES_PerfLogExit(CF_PERF_ID_FCLOSE);

    if (ret != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_CLOSE_ERR, CFE_EVS_EventType_ERROR,
                          "CF: failed to close file 0x%lx, OS_close returned %ld", OS_ObjectIdToInteger(fd), (long)ret);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_WrappedRead
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FREAD);
    ret = OS_read(fd, buf, read_size);
    CFE_ES_PerfLogExit(CF_PERF_ID_FREAD);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WrappedWrite
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size)
{
    int32 ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FWRITE);
    ret = OS_write(fd, buf, write_size);
    CFE_ES_PerfLogExit(CF_PERF_ID_FWRITE);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WrappedLseek
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_WrappedLseek(osal_id_t fd, off_t offset, int mode)
{
    int ret;
    CFE_ES_PerfLogEntry(CF_PERF_ID_FSEEK);
    ret = OS_lseek(fd, offset, mode);
    CFE_ES_PerfLogExit(CF_PERF_ID_FSEEK);
    return ret;
}
