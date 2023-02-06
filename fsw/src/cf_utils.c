/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Transaction_t *CF_FindUnusedTransaction(CF_Channel_t *c)
{
    CF_CListNode_t *  n;
    CF_Transaction_t *t;
    int               q_index; /* initialized below in if */

    CF_Assert(c);

    if (c->qs[CF_QueueIdx_FREE])
    {
        n = c->qs[CF_QueueIdx_FREE];
        t = container_of(n, CF_Transaction_t, cl_node);

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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_FindTransactionBySequenceNumber_Impl(CF_CListNode_t *n, CF_Traverse_TransSeqArg_t *context)
{
    CF_Transaction_t *t   = container_of(n, CF_Transaction_t, cl_node);
    CFE_Status_t      ret = 0;

    if ((t->history->src_eid == context->src_eid) && (t->history->seq_num == context->transaction_sequence_number))
    {
        context->t = t;
        ret        = 1; /* exit early */
    }

    return ret;
}

/*----------------------------------------------------------------
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
    CF_CListNode_t *          ptrs[] = {c->qs[CF_QueueIdx_RX], c->qs[CF_QueueIdx_PEND], c->qs[CF_QueueIdx_TXA],
                              c->qs[CF_QueueIdx_TXW]};
    int                       i;
    CF_Transaction_t *        ret = NULL;

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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WriteHistoryEntryToFile(osal_id_t fd, const CF_History_t *h)
{
    static const char *CF_DSTR[] = {"RX", "TX"}; /* conversion of CF_Direction_t to string */

    int          i;
    CFE_Status_t ret;
    int32        len;
    char         linebuf[(CF_FILENAME_MAX_LEN * 2) + 128]; /* buffer for line data */

    for (i = 0; i < 3; ++i)
    {
        switch (i)
        {
            case 0:
                CF_Assert(h->dir < CF_Direction_NUM);
                snprintf(linebuf, sizeof(linebuf), "SEQ (%lu, %lu)\tDIR: %s\tPEER %lu\tSTAT: %d\t",
                         (unsigned long)h->src_eid, (unsigned long)h->seq_num, CF_DSTR[h->dir],
                         (unsigned long)h->peer_eid, (int)h->txn_stat);
                break;
            case 1:
                snprintf(linebuf, sizeof(linebuf), "SRC: %s\t", h->fnames.src_filename);
                break;
            case 2:
            default:
                snprintf(linebuf, sizeof(linebuf), "DST: %s\n", h->fnames.dst_filename);
                break;
        }

        len = strlen(linebuf);
        ret = CF_WrappedWrite(fd, linebuf, len);
        if (ret != len)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_CMD_WHIST_WRITE, CFE_EVS_EventType_ERROR,
                              "CF: writing queue file failed, expected %ld got %ld", (long)len, (long)ret);
            return CF_ERROR;
        }
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_Traverse_WriteHistoryQueueEntryToFile(CF_CListNode_t *n, void *arg)
{
    CF_Traverse_WriteHistoryFileArg_t *context = arg;
    CF_History_t *                     h       = container_of(n, CF_History_t, cl_node);

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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_Traverse_WriteTxnQueueEntryToFile(CF_CListNode_t *n, void *arg)
{
    CF_Traverse_WriteTxnFileArg_t *context = arg;
    CF_Transaction_t *             t       = container_of(n, CF_Transaction_t, cl_node);

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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WriteTxnQueueDataToFile(osal_id_t fd, CF_Channel_t *c, CF_QueueIdx_t q)
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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WriteHistoryQueueDataToFile(osal_id_t fd, CF_Channel_t *c, CF_Direction_t dir)
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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PrioSearch(CF_CListNode_t *node, void *context)
{
    CF_Transaction_t *         t = container_of(node, CF_Transaction_t, cl_node);
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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_TraverseAllTransactions_Impl(CF_CListNode_t *n, CF_TraverseAll_Arg_t *args)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);
    args->fn(t, args->context);
    ++args->counter;
    return CF_CLIST_CONT;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_TraverseAllTransactions(CF_Channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    CF_TraverseAll_Arg_t args = {fn, context, 0};
    CF_QueueIdx_t        queueidx;
    for (queueidx = CF_QueueIdx_PEND; queueidx <= CF_QueueIdx_RX; ++queueidx)
        CF_CList_Traverse(c->qs[queueidx], (CF_CListFn_t)CF_TraverseAllTransactions_Impl, &args);

    return args.counter;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context)
{
    int          i;
    CFE_Status_t ret = 0;
    for (i = 0; i < CF_NUM_CHANNELS; ++i)
        ret += CF_TraverseAllTransactions(CF_AppData.engine.channels + i, fn, context);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access)
{
    CFE_Status_t ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FOPEN);
    ret = OS_OpenCreate(fd, fname, flags, access);
    CFE_ES_PerfLogExit(CF_PERF_ID_FOPEN);
    return ret;
}

/*----------------------------------------------------------------
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
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)
{
    CFE_Status_t ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FREAD);
    ret = OS_read(fd, buf, read_size);
    CFE_ES_PerfLogExit(CF_PERF_ID_FREAD);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size)
{
    CFE_Status_t ret;

    CFE_ES_PerfLogEntry(CF_PERF_ID_FWRITE);
    ret = OS_write(fd, buf, write_size);
    CFE_ES_PerfLogExit(CF_PERF_ID_FWRITE);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WrappedLseek(osal_id_t fd, off_t offset, int mode)
{
    CFE_Status_t ret;
    CFE_ES_PerfLogEntry(CF_PERF_ID_FSEEK);
    ret = OS_lseek(fd, offset, mode);
    CFE_ES_PerfLogExit(CF_PERF_ID_FSEEK);
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_TxnStatus_IsError
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CF_TxnStatus_IsError(CF_TxnStatus_t txn_stat)
{
    /* The value of CF_TxnStatus_UNDEFINED (-1) indicates a transaction is in progress and no error
     * has occurred yet.  This will be will be set to CF_TxnStatus_NO_ERROR (0) after successful
     * completion of the transaction (FIN/EOF).  Anything else indicates a problem has occurred. */
    return (txn_stat > CF_TxnStatus_NO_ERROR);
}

/*----------------------------------------------------------------
 *
 * Function: CF_TxnStatus_To_ConditionCode
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CFDP_ConditionCode_t CF_TxnStatus_To_ConditionCode(CF_TxnStatus_t txn_stat)
{
    CF_CFDP_ConditionCode_t result;

    if (!CF_TxnStatus_IsError(txn_stat))
    {
        /* If no status has been set (CF_TxnStatus_UNDEFINED), treat that as NO_ERROR for
         * the purpose of CFDP CC.  This can occur e.g. when sending ACK PDUs and no errors
         * have happened yet, but the transaction is not yet complete and thus not final. */
        result = CF_CFDP_ConditionCode_NO_ERROR;
    }
    else
    {
        switch (txn_stat)
        {
            /* The definition of CF_TxnStatus_t is such that the 4-bit codes (0-15) share the same
             * numeric values as the CFDP condition codes, and can be put directly into the 4-bit
             * CC field of a FIN/ACK/EOF PDU.  Extended codes use the upper bits (>15) to differentiate */
            case CF_TxnStatus_NO_ERROR:
            case CF_TxnStatus_POS_ACK_LIMIT_REACHED:
            case CF_TxnStatus_KEEP_ALIVE_LIMIT_REACHED:
            case CF_TxnStatus_INVALID_TRANSMISSION_MODE:
            case CF_TxnStatus_FILESTORE_REJECTION:
            case CF_TxnStatus_FILE_CHECKSUM_FAILURE:
            case CF_TxnStatus_FILE_SIZE_ERROR:
            case CF_TxnStatus_NAK_LIMIT_REACHED:
            case CF_TxnStatus_INACTIVITY_DETECTED:
            case CF_TxnStatus_INVALID_FILE_STRUCTURE:
            case CF_TxnStatus_CHECK_LIMIT_REACHED:
            case CF_TxnStatus_UNSUPPORTED_CHECKSUM_TYPE:
            case CF_TxnStatus_SUSPEND_REQUEST_RECEIVED:
            case CF_TxnStatus_CANCEL_REQUEST_RECEIVED:
                result = (CF_CFDP_ConditionCode_t)txn_stat;
                break;

                /* Extended status codes below here ---
                 * There are no CFDP CCs to directly represent these status codes. Normally this should
                 * not happen as the engine should not be sending a CFDP CC (FIN/ACK/EOF PDU) for a
                 * transaction that is not in a valid CFDP-defined state.  This should be translated
                 * to the closest CFDP CC per the intent/meaning of the transaction status code. */

            case CF_TxnStatus_ACK_LIMIT_NO_FIN:
            case CF_TxnStatus_ACK_LIMIT_NO_EOF:
                /* this is similar to the inactivity timeout (no fin-ack) */
                result = CF_CFDP_ConditionCode_INACTIVITY_DETECTED;
                break;

            default:
                /* Catch-all: any invalid protocol state will cancel the transaction, and thus this
                 * is the closest CFDP CC in practice for all other unhandled errors. */
                result = CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED;
                break;
        }
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Function: CF_TxnStatus_From_ConditionCode
 *
 * Application-scope internal function
 * See description in cf_utils.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_TxnStatus_t CF_TxnStatus_From_ConditionCode(CF_CFDP_ConditionCode_t cc)
{
    /* All CFDP CC values directly correspond to a Transaction Status of the same numeric value */
    return (CF_TxnStatus_t)cc;
}
