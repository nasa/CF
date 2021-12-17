/************************************************************************
** File: cf_cfdp.c
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
**  The CF Application main cfdp engine and pdu parsing file
**
**  This file contains two sets of functions. The first is what is needed
**  to deal with CFDP PDUs. Specifically validating them for correctness
**  and ensuring the byte-order is correct for the target. The second
**  is incoming and outgoing CFDP PDUs pass through here. All receive
**  CFDP PDU logic is performed here and the data is passed to the
**  R (rx) and S (tx) logic.
**
**
**
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_cfdp_helpers.h"
#include "cf_utils.h"

#include <string.h>
#include "cf_assert.h"

#define NUM_CLISTS 4
const int CF_max_chunks[CF_Direction_NUM][CF_NUM_CHANNELS] = {CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION,
                                                              CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION};

static void CF_CFDP_RecvIdle(CF_Transaction_t *, CF_CFDP_PduHeader_t *);
static void CF_CFDP_RecvDrop(CF_Transaction_t *, CF_CFDP_PduHeader_t *);

static void CF_CFDP_TxFile__(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority);

typedef struct trans_seq_arg_t
{
    CF_TransactionSeq_t transaction_sequence_number;
    CF_EntityId_t       src_eid;
    CF_Transaction_t   *t; /* out param */
} trans_seq_arg_t;

typedef struct CF_CFDP_CycleTx_args_t
{
    CF_Channel_t *c;
    int           ran_one;
} CF_CFDP_CycleTx_args_t;

typedef struct
{
    CF_Channel_t *c;                       /* IN param */
    void (*fn)(CF_Transaction_t *, int *); /* IN param */
    int early_exit;                        /* OUT param */
    int cont;                              /* if 1, then re-traverse the list */
} tick_args_t;

/************************************************************************/
/** \brief Arm the ack timer
**
**  \par Description
**       Helper function to arm the ack timer and set the flag.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_ArmAckTimer(CF_Transaction_t *t)
{
    CF_Timer_InitRelSec(&t->ack_timer, CF_AppData.config_table->ack_timer_s);
    t->flags.com.ack_timer_armed = 1;
}

/************************************************************************/
/** \brief Determine the cfdp class (1 or 2) of the transaction
**
**  \par Assumptions, External Events, and Notes:
**       ti must not be null. ti must be an initialized transaction.
**
**  \returns
**  \retstmt 0 for class 1, and 1 for class 2 \endcode
**  \endreturns
**
*************************************************************************/
static inline CF_CFDP_Class_t CF_CFDP_GetClass(const CF_Transaction_t *ti)
{
    CF_Assert(ti->flags.com.q_index != CF_QueueIdx_FREE);
    return !!((ti->state == CF_TxnState_S2) || (ti->state == CF_TxnState_R2));
}

/************************************************************************/
/** \brief Determine if a cfdp transaction is a sender or not
**
**  \par Assumptions, External Events, and Notes:
**       ti must not be null. ti must be an initialized transaction.
**
**  \returns
**  \retstmt 0 for receiver, and 1 for sender \endcode
**  \endreturns
**
*************************************************************************/
static inline int CF_CFDP_IsSender(CF_Transaction_t *ti)
{
    CF_Assert(ti->flags.com.q_index != CF_QueueIdx_FREE);
    /* the state could actually be CF_TxnState_IDLE, which is still not a sender. This would
     * be an unused transaction in the RX (CF_CFDP_ReceiveMessage) path. */
    return !!((ti->state == CF_TxnState_S1) || (ti->state == CF_TxnState_S2));
}

/************************************************************************/
/** \brief arm inactivity timer
**
**  \par Description
**       Arms the inactivity timer for the given transaction from timeout
**       specified in the config table.
**
**  \par Assumptions, External Events, and Notes:
**       The given transaction is active and initialized.
**
*************************************************************************/
static inline void CF_CFDP_ArmInactTimer(CF_Transaction_t *t)
{
    CF_Timer_InitRelSec(&t->inactivity_timer, CF_AppData.config_table->inactivity_timer_s);
}

/************************************************************************/
/** \brief Transmit pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_TxStateDispatch(CF_Transaction_t *t, const CF_CFDP_TxnSendDispatchTable_t *dispatch)
{
    CF_CFDP_StateSendFunc_t selected_handler;

    CF_Assert(t->state < CF_TxnState_INVALID);
    selected_handler = dispatch->tx[t->state];
    if (selected_handler != NULL)
    {
        selected_handler(t);
    }
}

/************************************************************************/
/** \brief Receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_RxStateDispatch(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph,
                                    const CF_CFDP_TxnRecvDispatchTable_t *dispatch)
{
    CF_CFDP_StateRecvFunc_t selected_handler;

    CF_Assert(t->state < CF_TxnState_INVALID);
    selected_handler = dispatch->rx[t->state];
    if (selected_handler != NULL)
    {
        selected_handler(t, ph);
    }
}

/************************************************************************/
/** \brief Dispatch received packet to its transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be null. It must be an initialized transaction.
**
*************************************************************************/
static void CF_CFDP_DispatchRecv(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    static const CF_CFDP_TxnRecvDispatchTable_t state_fns = {.rx = {[CF_TxnState_IDLE] = CF_CFDP_RecvIdle,
                                                                    [CF_TxnState_R1]   = CF_CFDP_R1_Recv,
                                                                    [CF_TxnState_S1]   = CF_CFDP_S1_Recv,
                                                                    [CF_TxnState_R2]   = CF_CFDP_R2_Recv,
                                                                    [CF_TxnState_S2]   = CF_CFDP_S2_Recv,
                                                                    [CF_TxnState_DROP] = CF_CFDP_RecvDrop}};

    CF_CFDP_RxStateDispatch(t, ph, &state_fns);
    CF_CFDP_ArmInactTimer(t); /* whenever a packet was received by the other size, always arm its inactivity timer */
}

/************************************************************************/
/** \brief Dispatches control to the active tx transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. t must be a valid tx transaction.
**
*************************************************************************/
static void CF_CFDP_DispatchTx(CF_Transaction_t *t)
{
    static const CF_CFDP_TxnSendDispatchTable_t state_fns = {
        .tx = {[CF_TxnState_S1] = CF_CFDP_S1_Tx, [CF_TxnState_S2] = CF_CFDP_S2_Tx}};

    CF_CFDP_TxStateDispatch(t, &state_fns);
}

/************************************************************************/
/** \brief Get an unused chunks structure off the chunks queue.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retstmt Address to a free chunks structure. Will not be null. \endcode
**  \endreturns
**
*************************************************************************/
static CF_ChunkWrapper_t *CF_CFDP_FindUnusedChunks(CF_Channel_t *c, CF_Direction_t dir)
{
    CF_ChunkWrapper_t *ret;

    CF_Assert(dir < CF_Direction_NUM);
    CF_Assert(c->cs[dir]);

    ret = container_of(CF_CList_Pop(&c->cs[dir]), CF_ChunkWrapper_t, cl_node);
    return ret;
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
static CF_Transaction_t *CF_CFDP_FindUnusedTransaction(CF_Channel_t *c)
{
    CF_Assert(c);

    if (c->qs[CF_QueueIdx_FREE])
    {
        int       q_index; /* initialized below in if */
        const int chan_index = (c - CF_AppData.engine.channels);

        CF_CListNode_t   *n = c->qs[CF_QueueIdx_FREE];
        CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);

        CF_CList_Remove_Ex(c, CF_QueueIdx_FREE, &t->cl_node);

        /* now that a transaction is acquired, must also acquire a history slot to go along with it */
        if (c->qs[CF_QueueIdx_HIST_FREE])
        {
            CF_Assert(CF_AppData.hk.channel_hk[chan_index].q_size[CF_QueueIdx_HIST] <
                      CF_NUM_HISTORIES_PER_CHANNEL); /* sanity check */
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
void CF_CFDP_ResetHistory(CF_Channel_t *c, CF_History_t *h)
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
static void CF_CFDP_FreeTransaction(CF_Transaction_t *t)
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
static int CF_CFDP_FindTransactionBySequenceNumber_(CF_CListNode_t *n, trans_seq_arg_t *context)
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
CF_Transaction_t *CF_CFDP_FindTransactionBySequenceNumber(CF_Channel_t       *c,
                                                          CF_TransactionSeq_t transaction_sequence_number,
                                                          CF_EntityId_t       src_eid)
{
    /* need to find transaction by sequence number. It will either be the active transaction (front of Q_PEND),
     * or on Q_TX or Q_RX. Once a transaction moves to history, then it's done.
     *
     * Let's put CF_QueueIdx_RX up front, because most RX packets will be file data PDUs */
    trans_seq_arg_t   ctx              = {transaction_sequence_number, src_eid, NULL};
    CF_CListNode_t   *ptrs[NUM_CLISTS] = {c->qs[CF_QueueIdx_RX], c->qs[CF_QueueIdx_PEND], c->qs[CF_QueueIdx_TXA],
                                        c->qs[CF_QueueIdx_TXW]};
    int               i;
    CF_Transaction_t *ret = NULL;

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        CF_CList_Traverse(ptrs[i], (CF_CListFn_t)CF_CFDP_FindTransactionBySequenceNumber_, &ctx);
        if (ctx.t)
        {
            ret = ctx.t;
            break;
        }
    }

    return ret;
}

/************************************************************************/
/** \brief Obtain a message buffer to construct a PDU inside.
**
**  \par Description
**       This performs the handshaking via semaphore with the consumer
**       of the PDU. If the semaphore can be obtained, a software bus
**       buffer is obtained and it is returned. If the semaphore is
**       unavailable, then the current transaction is remembered for next
**       engine cycle. If silent is true, then the event message is not
**       printed in the case of no buffer available.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt Pointer to a CF_CFDP_PduHeader_t within a software bus buffer on success. Otherwise NULL. \endcode
**  \endreturns
**
*************************************************************************/
CF_CFDP_PduHeader_t *CF_CFDP_MsgOutGet(const CF_Transaction_t *t, int silent)
{
    /* if channel is frozen, do not take message */
    CF_Channel_t        *c   = CF_AppData.engine.channels + t->chan_num;
    CF_CFDP_PduHeader_t *ret = NULL;

    /* this function can be called more than once before the message
     * is sent, so if there's already an outgoing message allocated
     * just use it */
    if (!CF_AppData.engine.out.msg)
    {
        if (CF_AppData.config_table->chan[t->chan_num].max_outgoing_messages_per_wakeup &&
            (CF_AppData.engine.outgoing_counter ==
             CF_AppData.config_table->chan[t->chan_num].max_outgoing_messages_per_wakeup))
        {
            /* no more messages this wakeup allowed */
            c->cur = t; /* remember where we were for next time */
            goto error_out;
        }

        if (!CF_AppData.hk.channel_hk[t->chan_num].frozen && !t->flags.com.suspended)
        {
            /* first, check if there's room in the pipe for the message we want to build */
            if (!CF_AppData.engine.out.msg && ((CF_AppData.config_table->chan[t->chan_num].sem_name[0] &&
                                                (OS_CountSemTimedWait(c->sem_id, 0) == OS_SUCCESS)) ||
                                               (!CF_AppData.config_table->chan[t->chan_num].sem_name[0])))
            {
                CF_AppData.engine.out.msg =
                    CFE_SB_AllocateMessageBuffer(offsetof(CF_PduSendMsg_t, ph) + CF_MAX_PDU_SIZE);
            }

            if (!CF_AppData.engine.out.msg)
            {
                c->cur = t; /* remember where we were for next time */
                if (!silent)
                    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_NO_MSG, CFE_EVS_EventType_ERROR,
                                      "CF: no output message buffer available");
                goto error_out;
            }

            CFE_MSG_Init(&CF_AppData.engine.out.msg->Msg, CF_AppData.config_table->chan[t->chan_num].apid_output, 0);
            ++CF_AppData.engine.outgoing_counter; /* even if max_outgoing_messages_per_wakeup is 0 (unlimited), it's ok
                                                     to inc this */
        }
    }

    ret = &((CF_PduSendMsg_t *)CF_AppData.engine.out.msg)->ph;

error_out:
    return ret;
}

/************************************************************************/
/** \brief Sends the current output buffer via the software bus.
**
**  \par Assumptions, External Events, and Notes:
**       The PDU in the output buffer is ready to transmit.
**
*************************************************************************/
static void CF_CFDP_Send(uint8 chan_num, const CF_CFDP_PduHeader_t *ph, uint32 len)
{
    CF_Assert(chan_num < CF_NUM_CHANNELS);
    CFE_SB_SetUserDataLength(&CF_AppData.engine.out.msg->Msg, len + CF_HeaderSize(ph));
    CFE_MSG_SetMsgTime(&CF_AppData.engine.out.msg->Msg, CFE_TIME_GetTime());
    /* PTFO: CFS apps typically do not check CFE_SB_Send or CFE_SB_ZeroCopySend return values.
     * This means the packet will be dropped, but the state machine will think it was sent.
     * Might want to explore this. */
    /* ignore return value */ CFE_SB_TransmitBuffer(CF_AppData.engine.out.msg, true);

    ++CF_AppData.hk.channel_hk[chan_num].counters.sent.pdu;

    CF_AppData.engine.out.msg = NULL;
}

/************************************************************************/
/** \brief Sets the PDU header length.
**
**  \par Assumptions, External Events, and Notes:
**       ph must not be NULL.
**
*************************************************************************/
static void CF_CFDP_SetPduLength(CF_CFDP_PduHeader_t *ph, uint16 length)
{
    cfdp_set_uint16(ph->length, length); /* does not include generic header length */
}

/************************************************************************/
/** \brief Build the PDU header in the output buffer to prepare to send a packet.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**       CF_AppData.engine.out.msg must not be NULL.
**
*************************************************************************/
CF_CFDP_PduHeader_t *CF_CFDP_ConstructPduHeader(const CF_Transaction_t *t, uint8 directive_code, CF_EntityId_t src_eid,
                                                CF_EntityId_t dst_eid, uint8 towards_sender, CF_TransactionSeq_t tsn,
                                                int silent)
{
    /* directive_code == 0 if file data */
    CF_CFDP_PduHeader_t *ph = CF_CFDP_MsgOutGet(t, silent);
    if (ph)
    {
        ph->flags = 0;
        FSV(ph->flags, CF_CFDP_PduHeader_FLAGS_DIR, !!towards_sender);
        FSV(ph->flags, CF_CFDP_PduHeader_FLAGS_TYPE,
            !directive_code); /* directive code 0 is reserved, so use it to indicate file data */
        FSV(ph->flags, CF_CFDP_PduHeader_FLAGS_MODE, !CF_CFDP_GetClass(t));

        CF_SetVariableHeader(ph, src_eid, dst_eid, tsn);

        /* If directive code is zero, the pdu is a file data pdu which has no directive code field.
         * So only set if non-zero, otherwise it will write a 0 to a byte in a file data pdu where we
         * don't necessarily want a 0. */
        if (directive_code)
        {
            cfdp_set_uint8(STATIC_CAST(ph, CF_CFDP_PduFileDirectiveHeader_t)->directive_code, directive_code);
        }
    }

    return ph;
}

/************************************************************************/
/** \brief strnlen implementation because some older toolchain don't support
**
**  TODO: remove this in favor of OS_strnlen someday
**
**  \par Assumptions, External Events, and Notes:
**       s must not be NULL.
**
**  \returns
**  \retstmt The length of the string not including null terminator, or maxlen if no null.
**  \endreturns
**
*************************************************************************/
static inline size_t CF_strnlen(const char *s, size_t maxlen)
{
    const char *end = memchr(s, 0, maxlen);
    if (end != NULL)
    {
        /* actual length of string is difference */
        maxlen = end - s;
    }
    return maxlen;
}

/************************************************************************/
/** \brief Build a metadata PDU for transmit.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SendRet_SUCCESS on success. \endcode
**  \retcode CF_SendRet_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SendRet_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
CF_SendRet_t CF_CFDP_SendMd(CF_Transaction_t *t)
{
    CF_CFDP_PduHeader_t *ph =
        CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_METADATA, CF_AppData.config_table->local_eid,
                                   t->history->peer_eid, 0, t->history->seq_num, 0);
    CF_CFDP_PduMd_t *md;

    CF_SendRet_t sret = CF_SendRet_SUCCESS;

    if (!ph)
    {
        sret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    md = STATIC_CAST(ph, CF_CFDP_PduMd_t);

    const int src_len = CF_strnlen(t->history->fnames.src_filename, sizeof(t->history->fnames.src_filename));
    const int dst_len = CF_strnlen(t->history->fnames.dst_filename, sizeof(t->history->fnames.dst_filename));
    int       lv_ret, ret;

    CF_Assert((t->state == CF_TxnState_S1) || (t->state == CF_TxnState_S2));
    if ((src_len == sizeof(t->history->fnames.src_filename)) || (dst_len == sizeof(t->history->fnames.dst_filename)))
    {
        sret = CF_SendRet_FAILURE;
        goto err_out;
    }

    cfdp_set_uint8(md->segmentation_control, 0);
    cfdp_set_uint32(md->size, t->fsize);

    /* at this point, need to copy filenames into md packet */
    ret =
        CF_CFDP_CopyDataToLv((CF_CFDP_lv_t *)md->filename_lvs, (const uint8 *)t->history->fnames.src_filename, src_len);
    if (ret < 0)
    {
        sret = CF_SendRet_ERROR; /* should not happen, since filename lengths are checked above */
        goto err_out;
    }

    lv_ret = ret;
    ret    = CF_CFDP_CopyDataToLv((CF_CFDP_lv_t *)(md->filename_lvs + lv_ret),
                                  (const uint8 *)t->history->fnames.dst_filename, dst_len);
    if (ret < 0)
    {
        sret = CF_SendRet_ERROR; /* should not happen, since filename lengths are checked above */
        goto err_out;
    }

    lv_ret += ret;

    CF_CFDP_SetPduLength(ph, (offsetof(CF_CFDP_PduMd_t, filename_lvs) + lv_ret));

    CF_CFDP_Send(t->chan_num, ph, offsetof(CF_CFDP_PduMd_t, filename_lvs) + lv_ret);

err_out:
    return sret;
}

/************************************************************************/
/** \brief Build a filedata PDU for transmit.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SendRet_SUCCESS on success. \endcode
**  \retcode CF_SendRet_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SendRet_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
CF_SendRet_t CF_CFDP_SendFd(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph, uint32 offset, int len)
{
    CF_CFDP_PduFd_t *fd = STATIC_CAST(ph, CF_CFDP_PduFd_t);
    /* NOTE: SendFd does not need a call to CF_CFDP_MsgOutGet, as the caller already has it */
    CF_SendRet_t ret = CF_SendRet_SUCCESS;

    if (len > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        ret = CF_SendRet_ERROR;
        goto err_out;
    }

    cfdp_set_uint32(fd->fdh.offset, offset);

    /* update pdu length */
    CF_CFDP_SetPduLength(ph, offsetof(CF_CFDP_PduFd_t, fdd) + len); /* does not include generic header length */
    CF_CFDP_Send(t->chan_num, ph, offsetof(CF_CFDP_PduFd_t, fdd) + len);

err_out:
    return ret;
}

/************************************************************************/
/** \brief Common functionality between SendEof and SendFin
**
**  Writes the TLV for the faulting EID to the given address.
**
**  \par Assumptions, External Events, and Notes:
**       tlv must not be NULL.
**
**  \returns
**  \retstmt Returns the tlv length \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_FinishEofAck(CF_CFDP_tlv_t *tlv)
{
    const int csize =
        CF_GetMemcpySize((uint8 *)&CF_AppData.config_table->local_eid, sizeof(CF_AppData.config_table->local_eid));

    CF_MemcpyToBE(tlv->data, (uint8 *)&CF_AppData.config_table->local_eid, sizeof(CF_AppData.config_table->local_eid),
                  csize);
    cfdp_set_uint8(tlv->length, csize);
    cfdp_set_uint8(tlv->type, CF_CFDP_TLV_TYPE_ENTITY_ID);

    return offsetof(CF_CFDP_tlv_t, data) + csize;
}

/************************************************************************/
/** \brief Build a eof PDU for transmit.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SendRet_SUCCESS on success. \endcode
**  \retcode CF_SendRet_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SendRet_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
CF_SendRet_t CF_CFDP_SendEof(CF_Transaction_t *t)
{
    CF_CFDP_PduHeader_t *ph =
        CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_EOF, CF_AppData.config_table->local_eid,
                                   t->history->peer_eid, 0, t->history->seq_num, 0);
    CF_CFDP_PduEof_t *eof;
    int               tlv_length = 0;
    CF_SendRet_t      ret        = CF_SendRet_SUCCESS;

    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    eof = STATIC_CAST(ph, CF_CFDP_PduEof_t);

    FSV(eof->cc, CF_CFDP_PduEof_FLAGS_CC, t->history->cc);
    cfdp_set_uint32(eof->crc, t->crc.result);
    cfdp_set_uint32(eof->size, t->fsize);
    if (t->history->cc != CF_CFDP_ConditionCode_NO_ERROR)
    {
        tlv_length += CF_CFDP_FinishEofAck((CF_CFDP_tlv_t *)eof->fault_location);
    }

    CF_CFDP_SetPduLength(ph, offsetof(CF_CFDP_PduEof_t, fault_location) + tlv_length);
    CF_CFDP_Send(t->chan_num, ph, offsetof(CF_CFDP_PduEof_t, fault_location) + tlv_length);

err_out:
    return ret;
}

/************************************************************************/
/** \brief Build a ack PDU for transmit.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SendRet_SUCCESS on success. \endcode
**  \retcode CF_SendRet_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SendRet_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
CF_SendRet_t CF_CFDP_SendAck(CF_Transaction_t *t, CF_CFDP_AckTxnStatus_t ts, CF_CFDP_FileDirective_t dir_code,
                             CF_CFDP_ConditionCode_t cc, CF_EntityId_t peer_eid, CF_TransactionSeq_t tsn)
{
    CF_CFDP_PduHeader_t *ph;
    CF_CFDP_PduAck_t    *ack;
    CF_SendRet_t         ret = CF_SendRet_SUCCESS;

    if (CF_CFDP_IsSender(t))
    {
        ph = CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_ACK, CF_AppData.config_table->local_eid, peer_eid,
                                        dir_code == CF_CFDP_FileDirective_EOF ? 1 : 0, tsn, 0);
    }
    else
    {
        ph = CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_ACK, peer_eid, CF_AppData.config_table->local_eid,
                                        dir_code == CF_CFDP_FileDirective_EOF ? 1 : 0, tsn, 0);
    }

    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    ack = STATIC_CAST(ph, CF_CFDP_PduAck_t);

    CF_Assert((dir_code == CF_CFDP_FileDirective_EOF) || (dir_code == CF_CFDP_FileDirective_FIN));
    FSV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_CODE, dir_code);
    FSV(ack->directive_and_subtype_code, CF_CFDP_PduAck_DIR_SUBTYPE_CODE,
        1); /* looks like always 1 if not extended features */
    FSV(ack->cc_and_transaction_status, CF_CFDP_PduAck_CC, cc);
    FSV(ack->cc_and_transaction_status, CF_CFDP_PduAck_TRANSACTION_STATUS, ts);

    CF_CFDP_SetPduLength(ph, sizeof(CF_CFDP_PduAck_t));
    CF_CFDP_Send(t->chan_num, ph, sizeof(CF_CFDP_PduAck_t));
err_out:
    return ret;
}

/************************************************************************/
/** \brief Build a fin PDU for transmit.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SendRet_SUCCESS on success. \endcode
**  \retcode CF_SendRet_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SendRet_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
CF_SendRet_t CF_CFDP_SendFin(CF_Transaction_t *t, CF_CFDP_FinDeliveryCode_t dc, CF_CFDP_FinFileStatus_t fs,
                             CF_CFDP_ConditionCode_t cc)
{
    CF_CFDP_PduHeader_t *ph         = CF_CFDP_ConstructPduHeader(t, CF_CFDP_FileDirective_FIN, t->history->peer_eid,
                                                                 CF_AppData.config_table->local_eid, 1, t->history->seq_num, 0);
    CF_SendRet_t         ret        = CF_SendRet_SUCCESS;
    int                  tlv_length = 0;

    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    CF_CFDP_PduFin_t *fin = STATIC_CAST(ph, CF_CFDP_PduFin_t);

    cfdp_set_uint8(fin->flags, 0);
    FSV(fin->flags, CF_CFDP_PduFin_FLAGS_CC, cc);
    FSV(fin->flags, CF_CFDP_PduFin_FLAGS_DELIVERY_CODE, dc);
    FSV(fin->flags, CF_CFDP_PduFin_FLAGS_FILE_STATUS, fs);
    FSV(fin->flags, CF_CFDP_PduFin_FLAGS_END_SYSTEM_STATUS, 1); /* seems to always be 1 without extended features */
    if (cc != CF_CFDP_ConditionCode_NO_ERROR)
    {
        tlv_length += CF_CFDP_FinishEofAck((CF_CFDP_tlv_t *)fin->fault_location);
    }
    CF_CFDP_SetPduLength(ph, offsetof(CF_CFDP_PduFin_t, fault_location) + tlv_length);
    CF_CFDP_Send(t->chan_num, ph, offsetof(CF_CFDP_PduFin_t, fault_location) + tlv_length);

err_out:
    return ret;
}

/************************************************************************/
/** \brief Build a nak PDU for transmit.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SendRet_SUCCESS on success. \endcode
**  \retcode CF_SendRet_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SendRet_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
CF_SendRet_t CF_CFDP_SendNak(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph, int num_segment_requests)
{
    CF_CFDP_PduNak_t *nak = STATIC_CAST(ph, CF_CFDP_PduNak_t);
    int               index;
    CF_SendRet_t      ret = CF_SendRet_SUCCESS;

    if (!ph)
    {
        ret = CF_SendRet_NO_MSG;
        goto err_out;
    }

    CF_Assert(CF_CFDP_GetClass(t) == CF_CFDP_CLASS_2);

    cfdp_set_uint32(nak->scope_start, nak->scope_start);
    cfdp_set_uint32(nak->scope_end, nak->scope_end);

    for (index = 0; index < num_segment_requests; ++index)
    {
        /* nak->segment_requests is a pointer, so no need to use accessor function for alignment */
        CF_CFDP_SegmentRequest_t *s = nak->segment_requests + index;
        cfdp_set_uint32(s->offset_start, s->offset_start);
        cfdp_set_uint32(s->offset_end, s->offset_end);
    }

    index = offsetof(CF_CFDP_PduNak_t, segment_requests) +
            (index * sizeof(CF_CFDP_SegmentRequest_t)); /* calculate pdu size */
    CF_CFDP_SetPduLength(ph, index);                    /* does not include generic header length */
    CF_CFDP_Send(t->chan_num, ph, index);

err_out:
    return ret;
}

/************************************************************************/
/** \brief Unpack a PDU header from a received message.
**
**  \par Description
**       PDUs are received and processed in-place, but there may be endian
**       concerns as well as the need to check data for validity.
**
**  \par Assumptions, External Events, and Notes:
**       A new message has been received.
**
**  \returns
**  \retcode 0 on success \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_RecvPh(uint8 chan_num, CFE_SB_Buffer_t *msgbuf, CF_CFDP_PduHeader_t **pph)
{
    CF_Assert(chan_num < CF_NUM_CHANNELS);
    CF_Assert(msgbuf);

    CF_CFDP_PduHeader_t *ph = &((CF_PduRecvMsg_t *)msgbuf)->ph;
    uint16               temp;
    const int            hsize = CF_HeaderSize(ph);

    CF_AppData.engine.in.bytes_received = CFE_SB_GetUserDataLength(&msgbuf->Msg);

    if (CF_AppData.engine.in.bytes_received < hsize)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_SHORT_HEADER, CFE_EVS_EventType_ERROR, "CF: pdu too short (%d received)",
                          (uint32)CF_AppData.engine.in.bytes_received);
        goto err_out;
    }

    if (CF_GetVariableHeader(ph))
        goto err_out;

    cfdp_get_uint16(ph->length, ph->length);
    cfdp_ldst_uint16(temp, ph->length);
    if ((temp + hsize) != CF_AppData.engine.in.bytes_received)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_LONG_VS_RECVD, CFE_EVS_EventType_ERROR,
                          "CF: pdu header length of %d different than received %d", temp,
                          (uint32)CF_AppData.engine.in.bytes_received);
        goto err_out;
    }

    /* pdu is ok, so continue processing */
    ++CF_AppData.hk.channel_hk[chan_num].counters.recv.pdu;
    *pph = ph;

    /* going to leave two return statements in, since they are at the end of the function.
     * one is normal exit path, and one is error exit path */
    return 0;

err_out:
    ++CF_AppData.hk.channel_hk[chan_num].counters.recv.error;
    *pph = NULL;
    return -1;
}

/************************************************************************/
/** \brief Unpack a metadata PDU from a received message.
**
**  \par Assumptions, External Events, and Notes:
**       A new message has been received. t must not be NULL.
**
**  \returns
**  \retcode 0 on success \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_RecvMd(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    CF_CFDP_PduMd_t *md   = STATIC_CAST(ph, CF_CFDP_PduMd_t);
    int              offs = 0, lv_ret;

    if (CF_AppData.engine.in.bytes_received < (CF_HeaderSize(ph) + sizeof(CF_CFDP_PduMd_t)))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_MD_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: metadata packet too short: %d bytes received",
                          (uint32)CF_AppData.engine.in.bytes_received);
        goto err_out;
    }

    cfdp_get_uint32(md->size, md->size);
    cfdp_ldst_uint32(t->fsize, md->size);

    /* store the filenames */
    /* PTFO: could maybe make this a loop of 2 to store the filenames */
    lv_ret = CF_CFDP_CopyDataFromLv((uint8 *)t->history->fnames.src_filename, (CF_CFDP_lv_t *)md->filename_lvs);
    if (lv_ret < 0)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_INVALID_SRC_LEN, CFE_EVS_EventType_ERROR,
                          "CF: metadata pdu rejected due to invalid length in source filename of 0x%02x",
                          ((CF_CFDP_lv_t *)md->filename_lvs + offs)->length);
        goto err_out;
    }
    /* need to null-terminate file name */
    t->history->fnames.src_filename[lv_ret] = 0;
    offs += lv_ret + 1; /* +1 for the length byte in the lv -- CF_CFDP_CopyDataFromLv returns bytes copied to buffer */
    lv_ret =
        CF_CFDP_CopyDataFromLv((uint8 *)t->history->fnames.dst_filename, (CF_CFDP_lv_t *)(md->filename_lvs + offs));
    if (lv_ret < 0)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_INVALID_DST_LEN, CFE_EVS_EventType_ERROR,
                          "CF: metadata pdu rejected due to invalid length in dest filename of 0x%02x",
                          ((CF_CFDP_lv_t *)md->filename_lvs + offs)->length);
        goto err_out;
    }
    t->history->fnames.dst_filename[lv_ret] = 0;

    CFE_EVS_SendEvent(CF_EID_INF_PDU_MD_RECVD, CFE_EVS_EventType_INFORMATION,
                      "CF: md received for source: %s, dest: %s", t->history->fnames.src_filename,
                      t->history->fnames.dst_filename);

    /* normal return path */
    return 0;

err_out:
    ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
    /* error return path */
    return -1;
}

/************************************************************************/
/** \brief Unpack a file data PDU from a received message.
**
**  \par Assumptions, External Events, and Notes:
**       A new message has been received. t must not be NULL.
**
**  \returns
**  \retcode 0 on success \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_RecvFd(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    int              ret = 0;
    CF_CFDP_PduFd_t *fd  = STATIC_CAST(ph, CF_CFDP_PduFd_t);

    if (CF_AppData.engine.in.bytes_received < (CF_HeaderSize(ph) + sizeof(CF_CFDP_PduFileDataHeader_t)))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_FD_SHORT, CFE_EVS_EventType_ERROR,
                          "CF: filedata pdu too short: %d bytes received", (uint32)CF_AppData.engine.in.bytes_received);
        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
        ret = -1;
    }
    else
    {
        cfdp_get_uint32(fd->fdh.offset, fd->fdh.offset);
    }

    return ret;
}

/************************************************************************/
/** \brief Unpack an eof PDU from a received message.
**
**  \par Assumptions, External Events, and Notes:
**       A new message has been received.
**
**  \returns
**  \retcode 0 on success \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_RecvEof(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    int               ret = 0;
    CF_CFDP_PduEof_t *eof = STATIC_CAST(ph, CF_CFDP_PduEof_t);

    if (CF_AppData.engine.in.bytes_received < (CF_HeaderSize(ph) + offsetof(CF_CFDP_PduEof_t, fault_location)))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_EOF_SHORT, CFE_EVS_EventType_ERROR, "CF: eof pdu too short: %d bytes received",
                          (uint32)CF_AppData.engine.in.bytes_received);
        ret = -1;
    }
    else
    {
        /* NOTE: right now we don't care about the fault location */
        cfdp_get_uint32(eof->crc, eof->crc);
        cfdp_get_uint32(eof->size, eof->size);
    }

    return ret;
}

/************************************************************************/
/** \brief Unpack an ack PDU from a received message.
**
**  \par Assumptions, External Events, and Notes:
**       A new message has been received.
**
**  \returns
**  \retcode 0 on success \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_RecvAck(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    int ret = 0;

    if (CF_AppData.engine.in.bytes_received < (CF_HeaderSize(ph) + sizeof(CF_CFDP_PduAck_t)))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_ACK_SHORT, CFE_EVS_EventType_ERROR, "CF: ack pdu too short: %d bytes received",
                          (uint32)CF_AppData.engine.in.bytes_received);
        ret = -1;
    }

    /* nothing to do for this one, as all fields are bytes */
    return ret;
}

/************************************************************************/
/** \brief Unpack an ack PDU from a received message.
**
**  \par Assumptions, External Events, and Notes:
**       A new message has been received.
**
**  \returns
**  \retcode 0 on success \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_RecvFin(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    int ret = 0;

    if (CF_AppData.engine.in.bytes_received < (CF_HeaderSize(ph) + offsetof(CF_CFDP_PduFin_t, fault_location)))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_FIN_SHORT, CFE_EVS_EventType_ERROR, "CF: fin pdu too short: %d bytes received",
                          (uint32)CF_AppData.engine.in.bytes_received);
        ret = -1;
    }

    /* NOTE: right now we don't care about the fault location */
    /* nothing to do for this one. all fields are bytes */
    return ret;
}

/************************************************************************/
/** \brief Unpack a nak PDU from a received message.
**
**  \par Assumptions, External Events, and Notes:
**       A new message has been received. num_segment_requests must not be NULL.
**
**  \returns
**  \retcode 0 on success \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_RecvNak(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph, int *num_segment_requests)
{
    /* CF_CFDP_RecvPh() must have been called before this, so use ldst to access pdu header */
    CF_Assert(num_segment_requests);

    int               ret = 0;
    CF_CFDP_PduNak_t *nak = STATIC_CAST(ph, CF_CFDP_PduNak_t);

    if (CF_AppData.engine.in.bytes_received < (CF_HeaderSize(ph) + offsetof(CF_CFDP_PduNak_t, segment_requests)))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_NAK_SHORT, CFE_EVS_EventType_ERROR, "CF: nak pdu too short: %d bytes received",
                          (uint32)CF_AppData.engine.in.bytes_received);
        ret = -1;
        goto err_out;
    }

    cfdp_get_uint32(nak->scope_start, nak->scope_start);
    cfdp_get_uint32(nak->scope_end, nak->scope_end);

    /* determine number of segment requests based on size */
    *num_segment_requests = 0;

    while ((*num_segment_requests < CF_NAK_MAX_SEGMENTS) &&
           (offsetof(CF_CFDP_PduNak_t, segment_requests[*num_segment_requests + 1]) <=
            CF_AppData.engine.in.bytes_received))
    {
        /* segment_requests is a pointer, so its value does not need to consider alignment to access */
        struct CF_CFDP_SegmentRequest *s = nak->segment_requests + *num_segment_requests;
        cfdp_get_uint32(s->offset_start, s->offset_start);
        cfdp_get_uint32(s->offset_end, s->offset_end);
        ++*num_segment_requests;
    }
err_out:
    return ret;
}

/************************************************************************/
/** \brief Receive state function to ignore a packet.
**
**  \par Description
**       This function signature must match all receive state functions.
**       The parameter t is ignored here.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_RecvDrop(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped;
}

/************************************************************************/
/** \brief Receive state function to process new rx transaction.
**
**  \par Description
**       An idle transaction has never had message processing performed on it.
**       Typically, the first packet received for a transaction would be
**       the metadata pdu. There's a special case for R2 where the metadata
**       pdu could be missed, and filedata comes in instead. In that case,
**       an R2 transaction must still be started.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. There must be a received message.
**
*************************************************************************/
static void CF_CFDP_RecvIdle(CF_Transaction_t *t, CF_CFDP_PduHeader_t *ph)
{
    /* only RX transactions dare tread here */
    int ok_to_reset = 1;

    t->history->seq_num = CF_AppData.engine.in.tsn;

    /* peer_eid is always the remote partner. src_eid is always the transaction source.
     * in this case, they are the same */
    t->history->peer_eid = CF_AppData.engine.in.src;
    t->history->src_eid  = CF_AppData.engine.in.src;

    t->chunks = CF_CFDP_FindUnusedChunks(&CF_AppData.engine.channels[t->chan_num], CF_Direction_RX);

    /* this is an idle transaction, so see if there's a received packet that can
     * be bound to the transaction */
    if (FGV(ph->flags, CF_CFDP_PduHeader_FLAGS_TYPE))
    {
        /* file data PDU */
        /* being idle and receiving a file data PDU means that no active transaction knew
         * about the transaction in progress, so most likely PDUs were missed. */

        /* if class 2, switch into R2 state and let it handle */
        /* don't forget to bind the transaction */
        if (FGV(ph->flags, CF_CFDP_PduHeader_FLAGS_MODE))
        {
            /* R1, can't do anything without metadata first */
            t->state = CF_TxnState_DROP; /* drop all incoming */
            /* use inactivity timer to ultimately free the state */
        }
        else
        {
            /* R2 can handle missing metadata, so go ahead and create a temp file */
            t->state = CF_TxnState_R2;
            CF_CFDP_R_Init(t);
            CF_CFDP_DispatchRecv(t, ph); /* re-dispatch to enter r2 */
        }
    }
    else
    {
        int                               status;
        CF_CFDP_PduFileDirectiveHeader_t *fdh = STATIC_CAST(ph, CF_CFDP_PduFileDirectiveHeader_t);

        /* file directive PDU, but we are in an idle state. It only makes sense right now to accept metadata PDU. */
        switch (fdh->directive_code)
        {
            case CF_CFDP_FileDirective_METADATA:
                status = CF_CFDP_RecvMd(t, ph);
                if (!status)
                {
                    /* NOTE: whether or not class 1 or 2, get a free chunks. it's cheap, and simplifies cleanup path */
                    t->state = FGV(ph->flags, CF_CFDP_PduHeader_FLAGS_MODE) ? CF_TxnState_R1 : CF_TxnState_R2;
                    t->flags.rx.md_recv = 1;
                    CF_CFDP_R_Init(t); /* initialize R */
                    ok_to_reset = 0;   /* if error in CF_CFDP_R_Init(), the transaction will be reset. if no error, then
                                          reset won't occur */
                }
                else
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_IDLE_MD, CFE_EVS_EventType_ERROR,
                                      "CF: got invalid md pdu -- abandoning transaction");
                    ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
                    /* leave state as idle, which will reset below */
                }
                break;
            default:
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_FD_UNHANDLED, CFE_EVS_EventType_ERROR,
                                  "CF: unhandled file directive code 0x%02x in idle state", fdh->directive_code);
                ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error;
                break;
        }
    }

    if (ok_to_reset && (t->state == CF_TxnState_IDLE))
    {
        /* state was not changed, so free the transaction */
        CF_CFDP_ResetTransaction(t, 0);
    }
}

/************************************************************************/
/** \brief Initialization function for the cfdp engine
**
**  \par Description
**       Performs per-channel initialization.
**
**  \par Assumptions, External Events, and Notes:
**       Only called once.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_CFDP_InitEngine(void)
{
    /* initialize all transaction nodes */
    int                i, j;
    int                chunk_mem_offset = 0;
    CF_Transaction_t  *t                = CF_AppData.engine.transactions;
    CF_ChunkWrapper_t *c                = CF_AppData.engine.chunks;
    int32              ret              = CFE_SUCCESS;

    memset(&CF_AppData.engine, 0, sizeof(CF_AppData.engine));

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        char nbuf[64];
        snprintf(nbuf, sizeof(nbuf) - 1, "%s%d", CF_CHANNEL_PIPE_PREFIX, i);
        if ((ret = CFE_SB_CreatePipe(&CF_AppData.engine.channels[i].pipe,
                                     CF_AppData.config_table->chan[i].pipe_depth_input, nbuf)) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_PIPE, CFE_EVS_EventType_ERROR,
                              "CF: failed to create pipe %s, returned 0x%08x", nbuf, ret);
            goto err_out;
        }

        if ((ret =
                 CFE_SB_SubscribeLocal(CF_AppData.config_table->chan[i].apid_input, CF_AppData.engine.channels[i].pipe,
                                       CF_AppData.config_table->chan[i].pipe_depth_input)) != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_SUB, CFE_EVS_EventType_ERROR,
                              "CF: failed to subscribe to MID 0x%04x, returned 0x%08x",
                              CF_AppData.config_table->chan[i].apid_input, ret);
            goto err_out;
        }

        if (CF_AppData.config_table->chan[i].sem_name[0])
        {
            ret = OS_CountSemGetIdByName(&CF_AppData.engine.channels[i].sem_id,
                                         CF_AppData.config_table->chan[i].sem_name);
            if (ret != OS_SUCCESS)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_INIT_SEM, CFE_EVS_EventType_ERROR,
                                  "CF: failed to get sem id for name %s, error=0x%08x",
                                  CF_AppData.config_table->chan[i].sem_name, ret);
                goto err_out;
            }
        }

        for (j = 0; j < CF_NUM_TRANSACTIONS_PER_CHANNEL; ++j, ++t)
        {
            int k;

            t->chan_num = i;
            CF_CFDP_FreeTransaction(t);

            for (k = 0; k < CF_Direction_NUM; ++k, ++c)
            {
                CF_Assert((chunk_mem_offset + CF_max_chunks[k][i]) <= CF_NUM_CHUNKS_ALL_CHANNELS);
                CF_ChunkListInit(&c->chunks, CF_max_chunks[k][i], &CF_AppData.engine.chunk_mem[chunk_mem_offset]);
                chunk_mem_offset += CF_max_chunks[k][i];
                CF_CList_InitNode(&c->cl_node);
                CF_CList_InsertBack(&CF_AppData.engine.channels[i].cs[k], &c->cl_node);
            }
        }

        for (j = 0; j < CF_NUM_HISTORIES_PER_CHANNEL; ++j)
        {
            CF_History_t *h = &CF_AppData.engine.histories[(i * CF_NUM_HISTORIES_PER_CHANNEL) + j];
            CF_CList_InitNode(&h->cl_node);
            CF_CList_InsertBack_Ex(&CF_AppData.engine.channels[i], CF_QueueIdx_HIST_FREE, &h->cl_node);
        }
    }

    CF_AppData.engine.enabled = 1;

err_out:
    return ret;
}

/************************************************************************/
/** \brief Process received message on channel PDU input pipe.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
*************************************************************************/
static void CF_CFDP_ReceiveMessage(CF_Channel_t *c)
{
    CF_Transaction_t    *t; /* initialized below */
    uint32               count = 0;
    int32                status;
    const int            chan_num = (c - CF_AppData.engine.channels);
    CFE_SB_Buffer_t     *bufptr;
    CF_CFDP_PduHeader_t *ph;

    for (; count < CF_AppData.config_table->chan[chan_num].rx_max_messages_per_wakeup; ++count)
    {
        status = CFE_SB_ReceiveBuffer(&bufptr, c->pipe, CFE_SB_POLL);
        if (status != CFE_SUCCESS)
        {
            break; /* no more messages */
        }

        /* NOTE: this code originally stored current msg buffer in a global, but
           going forward this should _NOT_ be used.  It is still set just in case
           some code depends on it (mostly just UT at this point).  FSW should pass
           the pointer to the current PDU to any function that needs it (ph here). */
        CF_AppData.engine.in.msg = bufptr;
        CFE_ES_PerfLogEntry(CF_PERF_ID_PDURCVD(chan_num));
        if (!CF_CFDP_RecvPh(chan_num, bufptr, &ph))
        {
            /* got a valid pdu -- look it up by sequence number */
            t = CF_CFDP_FindTransactionBySequenceNumber(c, CF_AppData.engine.in.tsn, CF_AppData.engine.in.src);
            if (t)
            {
                /* found one! send it to the transaction state processor */
                CF_Assert(t->state > CF_TxnState_IDLE);
                CF_CFDP_DispatchRecv(t, ph);
            }
            else
            {
                /* didn't find a match, but there's a special case:
                 *
                 * If an R2 sent FIN-ACK, the transaction is freed and the history data
                 * is placed in the history queue. It's possible that the peer missed the
                 * FIN-ACK and is sending another FIN. Since we don't know about this
                 * transaction, we don't want to leave R2 hanging. That wouldn't be elegant.
                 * So, send a FIN-ACK by cobbling together a temporary transaction on the
                 * stack and calling CF_CFDP_SendAck() */
                if (CF_AppData.engine.in.src == CF_AppData.config_table->local_eid &&
                    !FGV(ph->flags, CF_CFDP_PduHeader_FLAGS_TYPE) &&
                    (CF_AppData.engine.in.bytes_received >=
                     (sizeof(CF_CFDP_PduFileDirectiveHeader_t) + CF_HeaderSize(ph))) &&
                    (STATIC_CAST(ph, CF_CFDP_PduFileDirectiveHeader_t)->directive_code == CF_CFDP_FileDirective_FIN))
                {
                    if (!CF_CFDP_RecvFin(t, ph))
                    {
                        CF_Transaction_t t;
                        const uint8      chan_num = (c - CF_AppData.engine.channels);
                        memset(&t, 0, sizeof(t));
                        CF_CFDP_TxFile__(&t, CF_CFDP_CLASS_2, 1, chan_num,
                                         0); /* populate transaction with needed fields for CF_CFDP_SendAck() */
                        if (CF_CFDP_SendAck(&t, CF_CFDP_AckTxnStatus_UNRECOGNIZED, CF_CFDP_FileDirective_FIN,
                                            FGV(STATIC_CAST(ph, CF_CFDP_PduFin_t)->flags, CF_CFDP_PduFin_FLAGS_CC),
                                            CF_AppData.engine.in.dst, CF_AppData.engine.in.tsn) != CF_SendRet_NO_MSG)
                        {
                            /* couldn't get output buffer -- don't care about a send error (oh well, can't send) but we
                             * do care that there was no message because c->cur will be set to this transaction */
                            c->cur = NULL; /* do not remember temp transaction for next time */
                        }

                        /* NOTE: recv and recv_spurious will both be incremented */
                        ++CF_AppData.hk.channel_hk[chan_num].counters.recv.spurious;
                    }

                    CFE_ES_PerfLogExit(CF_PERF_ID_PDURCVD(chan_num));
                    continue;
                }

                /* if no match found, then it must be the case that we would be the destination entity id, so verify it
                 */
                if (CF_AppData.engine.in.dst == CF_AppData.config_table->local_eid)
                {
                    /* we didn't find a match, so assign it to a transaction */
                    if (CF_AppData.hk.channel_hk[chan_num].q_size[CF_QueueIdx_RX] == CF_MAX_SIMULTANEOUS_RX)
                    {
                        CFE_EVS_SendEvent(
                            CF_EID_ERR_CFDP_RX_DROPPED, CFE_EVS_EventType_ERROR,
                            "CF: dropping packet from %d transaction number 0x%08x due max RX transactions reached",
                            CF_AppData.engine.in.src, CF_AppData.engine.in.tsn);
                        ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped;
                    }
                    else
                    {
                        t = CF_CFDP_FindUnusedTransaction(c);
                        CF_Assert(t);
                        t->history->dir = CF_Direction_RX;

                        /* set default fin status */
                        t->state_data.r.r2.dc = CF_CFDP_FinDeliveryCode_INCOMPLETE;
                        t->state_data.r.r2.fs = CF_CFDP_FinFileStatus_DISCARDED;

                        CF_CList_InsertBack_Ex(c, (t->flags.com.q_index = CF_QueueIdx_RX), &t->cl_node);
                        CF_CFDP_DispatchRecv(t, ph); /* will enter idle state */
                    }
                }
                else
                {
                    CFE_EVS_SendEvent(CF_EID_ERR_CFDP_INVALID_DST_EID, CFE_EVS_EventType_ERROR,
                                      "CF: dropping packet for invalid destination eid 0x%x", CF_AppData.engine.in.dst);
                }
            }
        }

        CFE_ES_PerfLogExit(CF_PERF_ID_PDURCVD(chan_num));
    }
    CF_AppData.engine.in.msg = NULL;
}

/************************************************************************/
/** \brief List traversal function that cycles the first active tx.
**
**  \par Description
**       There can only be one active tx transaction per engine cycle.
**       This function finds the first active, and then sends file
**       data pdus until there are no outgoing message buffers.
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
static int CF_CFDP_CycleTx_(CF_CListNode_t *node, void *context)
{
    CF_CFDP_CycleTx_args_t *args = (CF_CFDP_CycleTx_args_t *)context;
    CF_Transaction_t       *t    = container_of(node, CF_Transaction_t, cl_node);
    int                     ret  = 1; /* default option is exit traversal */

    if (t->flags.com.suspended)
    {
        ret = 0; /* suspended, so move on to next */
        goto err_out;
    }

    CF_Assert(t->flags.com.q_index == CF_QueueIdx_TXA); /* huh? */

    /* if no more messages, then c->cur will be set.
     * If the transaction sent the last filedata pdu and eof, it will move itself
     * off the active queue. Run until either of these occur. */
    while (!args->c->cur && t->flags.com.q_index == CF_QueueIdx_TXA)
    {
        CFE_ES_PerfLogEntry(CF_PERF_ID_PDUSENT(t->chan_num));
        CF_CFDP_DispatchTx(t);
        CFE_ES_PerfLogExit(CF_PERF_ID_PDUSENT(t->chan_num));
    }

    args->ran_one = 1;

err_out:
    return ret; /* exit traversal */
}

/************************************************************************/
/** \brief Cycle the current active tx or make a new one active.
**
**  \par Description
**       First traverses all tx transactions on the active queue. If at
**       least one is found, then it stops. Otherwise it moves a
**       transaction on the pending queue to the active queue and
**       tries again to find an active one.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_CycleTx(CF_Channel_t *c)
{
    if (CF_AppData.config_table->chan[(c - CF_AppData.engine.channels)].dequeue_enabled)
    {
        CF_CFDP_CycleTx_args_t args = {c, 0};
        /* loop through as long as there are pending transactions, and a message buffer to send their pdus on */

        /* NOTE: tick processesing is higher priority than sending new filedata pdus, so only send however many
         * PDUs that can be sent once we get to here */
        if (!c->cur)
        { /* don't enter if cur is set, since we need to pick up where we left off on tick processing next wakeup */
            goto entry_jump; /* code reviewers won't like this */
            while (!args.ran_one && c->qs[CF_QueueIdx_PEND])
            {
                /* didn't find anything on TXA to run, so pop one off Q_PEND and try again.
                 * Keep going until CF_QueueIdx_PEND is empty or something is run */
                CF_Transaction_t *t = container_of(c->qs[CF_QueueIdx_PEND], CF_Transaction_t, cl_node);
                CF_MoveTransaction(t, CF_QueueIdx_TXA);
                /* args is ok, still { c, 0 } */
            entry_jump:
                CF_CList_Traverse(c->qs[CF_QueueIdx_TXA], CF_CFDP_CycleTx_, &args);
            }
        }

        c->cur =
            NULL; /* in case the loop exited due to no message buffers, clear it and start from the top next time */
    }
}

/************************************************************************/
/** \brief List traversal function that calls a r or s tick function.
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
static int CF_CFDP_DoTick(CF_CListNode_t *node, void *context)
{
    int               ret  = CF_CLIST_CONT; /* CF_CLIST_CONT means don't tick one, keep looking for cur */
    tick_args_t      *args = (tick_args_t *)context;
    CF_Transaction_t *t    = container_of(node, CF_Transaction_t, cl_node);
    if (!args->c->cur || (args->c->cur == t))
    {
        /* found where we left off, so clear that and move on */
        args->c->cur = NULL;
        if (!t->flags.com.suspended)
        {
            args->fn(t, &args->cont);
        }

        /* if args->c->cur was set to not-NULL above, then exit early */
        /* NOTE: if channel is frozen, then tick processing won't have been entered.
         *     so there is no need to check it here */
        if (args->c->cur)
        {
            ret              = CF_CLIST_EXIT;
            args->early_exit = 1;
        }
    }

    return ret; /* don't tick one, keep looking for cur */
}

/************************************************************************/
/** \brief Call R and then S tick functions for all active transactions.
**
**  \par Description
**       Traverses all transactions in the RX and TXW queues, and calls
**       their tick functions. Note that the TXW queue is used twice:
**       once for regular tick processing, and one for NAK response.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_TickTransactions(CF_Channel_t *c)
{
    void (*fns[CF_TickType_NUM_TYPES])(CF_Transaction_t *, int *) = {CF_CFDP_R_Tick, CF_CFDP_S_Tick,
                                                                     CF_CFDP_S_Tick_Nak};
    int qs[CF_TickType_NUM_TYPES]                                 = {CF_QueueIdx_RX, CF_QueueIdx_TXW, CF_QueueIdx_TXW};

    CF_Assert(c->tick_type < CF_TickType_NUM_TYPES);

    for (; c->tick_type < CF_TickType_NUM_TYPES; ++c->tick_type)
    {
        tick_args_t args = {c, fns[c->tick_type], 0, 0};

        do
        {
            args.cont = 0;
            CF_CList_Traverse(c->qs[qs[c->tick_type]], CF_CFDP_DoTick, &args);
            if (args.early_exit)
            {
                /* early exit means we ran out of availalbe outgoing messages this wakeup.
                 * If current tick type is nak response, then reset tick type. It would be
                 * bad to let NAK response starve out RX or TXW ticks on the next cycle.
                 *
                 * If RX ticks use up all availalee messages, then we pick up where whe left
                 * off on the next cycle. (This causes some RX tick counts to be missed,
                 * but that's ok. Precise timing isn't required.)
                 *
                 * This scheme allows the following priority for use of outgoing messages:
                 *
                 * RX state messages
                 * TXW state messages
                 * NAK response (could be many)
                 *
                 * New file data on TXA
                 */
                if (c->tick_type == CF_TickType_TXW_NAK)
                    break; /* triggers tick type reset below */
                else
                    goto early_exit;
            }
        } while (args.cont);
    }

    c->tick_type = CF_TickType_RX; /* reset tick type */

early_exit:;
}

/************************************************************************/
/** \brief Helper function to set tx file state in a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_TxFile__(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority)
{
    t->chan_num = chan;
    t->priority = priority;
    t->keep     = keep;
    t->state    = cfdp_class ? CF_TxnState_S2 : CF_TxnState_S1;
}

/************************************************************************/
/** \brief Helper function to set tx field state in a transaction.
**
**  \par Description
**       Sets up the transaction structure, including finding unused
**       chunks and inserting the transaction into the PEND queue.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_TxFile_(CF_Transaction_t *t, CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority,
                            CF_EntityId_t dest_id)
{
    CFE_EVS_SendEvent(CF_EID_INF_CFDP_S_START_SEND, CFE_EVS_EventType_INFORMATION,
                      "CF: start class %d tx of file %d:%.*s -> %d:%.*s", cfdp_class + 1,
                      CF_AppData.config_table->local_eid, CF_FILENAME_MAX_LEN, t->history->fnames.src_filename, dest_id,
                      CF_FILENAME_MAX_LEN, t->history->fnames.dst_filename);

    CF_CFDP_TxFile__(t, cfdp_class, keep, chan, priority);

    t->history->dir      = CF_Direction_TX;
    t->history->seq_num  = ++CF_AppData.engine.seq_num;
    t->history->src_eid  = CF_AppData.config_table->local_eid;
    t->history->peer_eid = dest_id;
    CF_CFDP_ArmInactTimer(t);

    /* NOTE: whether or not class 1 or 2, get a free chunks. it's cheap, and simplifies cleanup path */
    t->chunks = CF_CFDP_FindUnusedChunks(&CF_AppData.engine.channels[chan], CF_Direction_TX);
    CF_InsertSortPrio(t, CF_QueueIdx_PEND);
}

/************************************************************************/
/** \brief Begin transmit of a file.
**
**  \par Description
**       This function sets up a transaction for and starts transmit of
**       the given filename.
**
**  \par Assumptions, External Events, and Notes:
**       src_filename must not be NULL. dst_filename must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_CFDP_TxFile(const char src_filename[CF_FILENAME_MAX_LEN], const char dst_filename[CF_FILENAME_MAX_LEN],
                     CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, CF_EntityId_t dest_id)
{
    CF_Transaction_t *t;
    CF_Channel_t     *c = &CF_AppData.engine.channels[chan];
    CF_Assert(chan < CF_NUM_CHANNELS);

    int32 ret = CFE_SUCCESS;

    if (c->num_cmd_tx == CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_MAX_CMD_TX, CFE_EVS_EventType_ERROR,
                          "CF: max number of commanded files reached");
        ret = -1;
        goto err_out;
    }

    t = CF_CFDP_FindUnusedTransaction(&CF_AppData.engine.channels[chan]);
    CF_Assert(t); /* should always have a free transaction at this point */

    CF_Assert(t->state == CF_TxnState_IDLE);

    /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */
    strcpy(t->history->fnames.src_filename, src_filename);
    strcpy(t->history->fnames.dst_filename, dst_filename);
    CF_CFDP_TxFile_(t, cfdp_class, keep, chan, priority, dest_id);

    ++c->num_cmd_tx;
    t->flags.tx.cmd_tx = 1;

err_out:
    return ret;
}

/************************************************************************/
/** \brief Helper function to set up directory playback.
**
**  \par Description
**       Sets up CF_Playback_t data. Used by both playback and polling functions.
**
**  \par Assumptions, External Events, and Notes:
**       p must not be NULL. src_filename must not be NULL. dst_filename must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int32 CF_CFDP_PlaybackDir_(CF_Playback_t *p, const char *src_filename, const char *dst_filename,
                                  CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority,
                                  CF_EntityId_t dest_id)
{
    int32 ret = CFE_SUCCESS;

    /* make sure the directory can be open */
    ret = OS_DirectoryOpen(&p->dir_id, src_filename);
    if (ret != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_OPENDIR, CFE_EVS_EventType_ERROR,
                          "CF: failed to open playback directory %s, error=0x%08x", src_filename, ret);
        ++CF_AppData.hk.channel_hk[chan].counters.fault.directory_read;
        goto err_out;
    }

    p->diropen    = 1;
    p->busy       = 1;
    p->keep       = keep;
    p->priority   = priority;
    p->dest_id    = dest_id;
    p->cfdp_class = cfdp_class;

    /* NOTE: the caller of this function ensures the provided src and dst filenames are NULL terminated */
    strcpy(p->fnames.src_filename, src_filename);
    strcpy(p->fnames.dst_filename, dst_filename);

    /* the executor will start the transfer next cycle */

err_out:
    return ret;
}

/************************************************************************/
/** \brief Begin transmit of a directory.
**
**  \par Description
**       This function sets up CF_Playback_t structure with state so it can
**       become part of the directory polling done at each engine cycle.
**
**  \par Assumptions, External Events, and Notes:
**       src_filename must not be NULL. dst_filename must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_CFDP_PlaybackDir(const char src_filename[CF_FILENAME_MAX_LEN], const char dst_filename[CF_FILENAME_MAX_LEN],
                          CF_CFDP_Class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, uint16 dest_id)
{
    int            i;
    CF_Playback_t *p;

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        p = &CF_AppData.engine.channels[chan].playback[i];
        if (!p->busy)
        {
            break;
        }
    }

    if (i == CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_DIR_SLOT, CFE_EVS_EventType_ERROR, "CF: no playback dir slot available");
        return -1;
    }

    return CF_CFDP_PlaybackDir_(p, src_filename, dst_filename, cfdp_class, keep, chan, priority, dest_id);
}

/************************************************************************/
/** \brief Step each active playback directory.
**
**  \par Description
**       Check if a playback directory needs iterated, and if so does, and
**       if a valid file is found initiates playback on it.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL. p must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_ProcessPlaybackDirectory(CF_Channel_t *c, CF_Playback_t *p)
{
    os_dirent_t dirent;
    /* either there's no transaction (first one) or the last one was finished, so check for a new one */

    while (p->diropen && (p->num_ts < CF_NUM_TRANSACTIONS_PER_PLAYBACK))
    {
        int32 status;

        CFE_ES_PerfLogEntry(CF_PERF_ID_DIRREAD);
        status = OS_DirectoryRead(p->dir_id, &dirent);
        CFE_ES_PerfLogExit(CF_PERF_ID_DIRREAD);

        if (status == CFE_SUCCESS)
        {
            if (!strcmp(dirent.FileName, ".") || !strcmp(dirent.FileName, ".."))
            {
                continue;
            }

            {
                CF_Transaction_t *pt = CF_CFDP_FindUnusedTransaction(c);
                CF_Assert(pt); /* should be impossible not to have one because there are limits on the number of uses of
                                  them */

                /* the -1 below is to make room for the slash */
                snprintf(pt->history->fnames.src_filename, sizeof(pt->history->fnames.src_filename), "%.*s/%.*s",
                         CF_FILENAME_MAX_PATH - 1, p->fnames.src_filename, CF_FILENAME_MAX_NAME - 1, dirent.FileName);
                snprintf(pt->history->fnames.dst_filename, sizeof(pt->history->fnames.dst_filename), "%.*s/%.*s",
                         CF_FILENAME_MAX_PATH - 1, p->fnames.dst_filename, CF_FILENAME_MAX_NAME - 1, dirent.FileName);

                /* in case snprintf didn't have room for NULL terminator */
                pt->history->fnames.src_filename[CF_FILENAME_MAX_LEN - 1] = 0;
                pt->history->fnames.dst_filename[CF_FILENAME_MAX_LEN - 1] = 0;

                CF_CFDP_TxFile_(pt, p->cfdp_class, p->keep, (c - CF_AppData.engine.channels), p->priority, p->dest_id);

                pt->p = p;
                ++p->num_ts;
            }
        }
        else
        {
            /* PFTO: can we figure out the difference between "end of dir" and an error? */
            OS_DirectoryClose(p->dir_id);
            p->diropen = 0;
        }
    }

    if (!p->diropen && !p->num_ts)
    {
        /* the directory has been exhausted, and there are no more active transactions
         * for this playback -- so mark it as not busy */
        p->busy = 0;
    }
}

/************************************************************************/
/** \brief Update the playback or polling counter for channel HK
**
**  \par Assumptions, External Events, and Notes:
**       pb must not be NULL. counter must not be NULL.
**
*************************************************************************/
static void CF_CFDP_UpdatePollPbCounted(CF_Playback_t *pb, int up, uint8 *counter)
{
    if (pb->counted != up)
    {
        /* only handle on state change */
        pb->counted = !!up; /* !! ensure 0 or 1, should be optimized out */

        if (up)
        {
            ++*counter;
        }
        else
        {
            CF_Assert(*counter); /* sanity check it isn't zero */
            --*counter;
        }
    }
}

/************************************************************************/
/** \brief Call CF_CFDP_ProcessPlaybackDirectory on all commanded playbacks.
**
**  \par Description
**       This function signature must match all receive state functions.
**       The parameter t is ignored here.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
*************************************************************************/
static void CF_CFDP_ProcessPlaybackDirectories(CF_Channel_t *c)
{
    int       i;
    const int chan_index = (c - CF_AppData.engine.channels);

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        CF_CFDP_ProcessPlaybackDirectory(c, &c->playback[i]);
        CF_CFDP_UpdatePollPbCounted(&c->playback[i], c->playback[i].busy,
                                    &CF_AppData.hk.channel_hk[chan_index].playback_counter);
    }
}

/************************************************************************/
/** \brief Kick the dir playback if timer elapsed.
**
**  \par Description
**       This function waits for the polling directory interval timer,
**       and if it has expired, starts a playback in the polling directory.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
*************************************************************************/
static void CF_CFDP_ProcessPollingDirectories(CF_Channel_t *c)
{
    int i;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        CF_Poll_t          *p           = &c->poll[i];
        int                 chan_index  = (c - CF_AppData.engine.channels);
        CF_ChannelConfig_t *cc          = &CF_AppData.config_table->chan[chan_index];
        CF_PollDir_t       *pd          = &cc->polldir[i];
        int                 count_check = 0;

        if (pd->enabled && pd->interval_sec)
        {
            /* only handle polling for polldirs configured with a non-zero interval */
            if (!p->pb.busy && !p->pb.num_ts)
            {
                if (!p->timer_set)
                {
                    /* timer was not set, so set it now */
                    CF_Timer_InitRelSec(&p->interval_timer, pd->interval_sec);
                    p->timer_set = 1;
                }
                else if (CF_Timer_Expired(&p->interval_timer))
                {
                    /* the timer has expired */
                    int ret = CF_CFDP_PlaybackDir_(&p->pb, pd->src_dir, pd->dst_dir, pd->cfdp_class, 0, chan_index,
                                                   pd->priority, pd->dest_eid);
                    if (!ret)
                    {
                        p->timer_set = 0;
                    }
                    else
                    {
                        /* error occured in playback directory, so reset the timer */
                        /* an event is sent in CF_CFDP_PlaybackDir_ so there is no reason to
                         * to have another here */
                        CF_Timer_InitRelSec(&p->interval_timer, pd->interval_sec);
                    }
                }
                else
                    CF_Timer_Tick(&p->interval_timer);
            }
            else
            {
                /* playback is active, so step it */
                CF_CFDP_ProcessPlaybackDirectory(c, &p->pb);
            }

            count_check = 1;
        }

        CF_CFDP_UpdatePollPbCounted(&p->pb, count_check, &CF_AppData.hk.channel_hk[chan_index].poll_counter);
    }
}

/************************************************************************/
/** \brief Cycle the engine. Called once per wakeup.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void CF_CFDP_CycleEngine(void)
{
    int i;

    if (CF_AppData.engine.enabled)
    {
        for (i = 0; i < CF_NUM_CHANNELS; ++i)
        {
            CF_Channel_t *c                    = &CF_AppData.engine.channels[i];
            CF_AppData.engine.outgoing_counter = 0;

            /* consume all received messages, even if channel is frozen */
            CF_CFDP_ReceiveMessage(c);

            if (!CF_AppData.hk.channel_hk[i].frozen)
            {
                /* handle ticks before tx cycle. Do this because there may be a limited number of TX messages available
                 * this cycle, and it's important to respond to class 2 ACK/NAK more than it is to send new filedata
                 * PDUs. */

                /* cycle all transactions (tick) */
                CF_CFDP_TickTransactions(c);

                /* cycle the current tx transaction */
                CF_CFDP_CycleTx(c);

                CF_CFDP_ProcessPlaybackDirectories(c);
                CF_CFDP_ProcessPollingDirectories(c);
            }
        }
    }
}

/************************************************************************/
/** \brief Reset a transaction and all its internals to an unused state.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_ResetTransaction(CF_Transaction_t *t, int keep_history)
{
    CF_Channel_t *c = &CF_AppData.engine.channels[t->chan_num];
    CF_Assert(t->chan_num < CF_NUM_CHANNELS);

    CF_DequeueTransaction(t);

    if (OS_ObjectIdDefined(t->fd))
    {
        CF_WrappedClose(t->fd);
        if (!t->keep)
        {
            if (CF_CFDP_IsSender(t))
            {
                OS_remove(t->history->fnames.src_filename);
            }
            else
            {
                OS_remove(t->history->fnames.dst_filename);
            }
        }
    }

    switch (t->history->dir)
    {
        case CF_Direction_TX:
            if (t->flags.tx.cmd_tx)
            {
                CF_Assert(c->num_cmd_tx); /* sanity check */
                --c->num_cmd_tx;
            }

            if (t->p)
            {
                /* a playback's transaction is now done. decrement the playback counter */
                CF_Assert(t->p->num_ts);
                --t->p->num_ts;
            }

            /* fall through */

        case CF_Direction_RX:
            /* move transaction history to history queue */
            if (keep_history)
            {
                CF_CList_InsertBack_Ex(c, CF_QueueIdx_HIST, &t->history->cl_node);
            }
            else
            {
                CF_CList_InsertBack_Ex(c, CF_QueueIdx_HIST_FREE, &t->history->cl_node);
            }

            break;
        default:
            CF_Assert(0);
            break;
    }

    CF_CList_InsertBack(&c->cs[!!CF_CFDP_IsSender(t)], &t->chunks->cl_node);

    if (c->cur == t)
    {
        c->cur = NULL; /* this transaction couldn't get a message previously, so clear it here to avoid problems */
    }
    CF_CFDP_FreeTransaction(t);
}

/************************************************************************/
/** \brief Copy data to a lv (length, value) pair.
**
**  \par Assumptions, External Events, and Notes:
**       dest_lv must not be NULL. data must not be NULL.
**
**  \returns
**  \retcode The number of bytes copied to the lv. \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_CopyDataToLv(CF_CFDP_lv_t *dest_lv, const uint8 *data, uint32 len)
{
    /* Per CFDP spec, an LV is limited to 255 bytes, but CF app does
     * not encode anything bigger than a filename */
    if (len < CF_FILENAME_MAX_LEN)
    {
        dest_lv->length = len;
        memcpy(dest_lv->data, data, len);
        return len + 1; /* +1 for the len byte */
    }

    return -1;
}

/************************************************************************/
/** \brief Copy data from a lv (length, value) pair.
**
**  \par Assumptions, External Events, and Notes:
**       src_lv must not be NULL. data must not be NULL.
**
**  \returns
**  \retcode The number of bytes copied from the lv. \endcode
**  \retcode -1 on error \endcode
**  \endreturns
**
*************************************************************************/
int CF_CFDP_CopyDataFromLv(uint8 buf[CF_FILENAME_MAX_LEN], const CF_CFDP_lv_t *src_lv)
{
    if (src_lv->length < CF_FILENAME_MAX_LEN)
    {
        memcpy(buf, src_lv->data, src_lv->length);
        return src_lv->length;
    }

    return -1; /* invalid len in lv? */
}

/************************************************************************/
/** \brief Cancels a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_CancelTransaction(CF_Transaction_t *t)
{
    void (*fns[2])(CF_Transaction_t * t) = {CF_CFDP_R_Cancel, CF_CFDP_S_Cancel};
    if (!t->flags.com.canceled)
    {
        t->flags.com.canceled = 1;
        t->history->cc        = CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED;
        fns[!!CF_CFDP_IsSender(t)](t);
    }
}

/************************************************************************/
/** \brief List traversal function to close all files in all active transactions.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL. context must not be NULL.
**
**  \returns
**  \retcode Always 0 indicate list traversal should not exit early. \endcode
**  \endreturns
*************************************************************************/
static int CF_CFDP_CloseFiles(CF_CListNode_t *n, void *context)
{
    CF_Transaction_t *t = container_of(n, CF_Transaction_t, cl_node);
    if (OS_ObjectIdDefined(t->fd))
    {
        CF_WrappedClose(t->fd);
    }
    return CF_CLIST_CONT;
}

/************************************************************************/
/** \brief Disables the cfdp engine and resets all state in it.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void CF_CFDP_DisableEngine(void)
{
    int i, j;
    CF_AppData.engine.enabled = 0;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_Channel_t *c = CF_AppData.engine.channels + i;
        /* first, close all active files */
        CF_CListNode_t *ptrs[] = {c->qs[CF_QueueIdx_RX], c->qs[CF_QueueIdx_TXA], c->qs[CF_QueueIdx_TXW]};
        for (j = 0; j < sizeof(ptrs) / sizeof(*ptrs); ++j)
            CF_CList_Traverse(ptrs[j], CF_CFDP_CloseFiles, NULL);

        /* any playback directories need to have their dirents closed */
        for (j = 0; j < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++j)
        {
            if (c->playback[j].busy)
            {
                OS_DirectoryClose(c->playback[j].dir_id);
            }
        }

        for (j = 0; j < CF_MAX_POLLING_DIR_PER_CHAN; ++j)
        {
            if (c->poll[j].pb.busy)
            {
                OS_DirectoryClose(c->poll[j].pb.dir_id);
            }
        }

        /* finally all queue counters must be reset */
        memset(&CF_AppData.hk.channel_hk[i].q_size, 0, sizeof(CF_AppData.hk.channel_hk[i].q_size));

        CFE_SB_DeletePipe(c->pipe);
    }
}
