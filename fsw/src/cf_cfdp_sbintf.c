/************************************************************************
 * File: cf_cfdp_sbintf.c
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
 *
 ************************************************************************/

/**
 * @file
 *
 * This is the interface to the CFE Software Bus for CF transmit/recv.
 * Specifically this implements 3 functions used by the CFDP engine:
 *  - CF_CFDP_MsgOutGet() - gets a buffer prior to transmitting
 *  - CF_CFDP_Send() - sends the buffer from CF_CFDP_MsgOutGet
 *  - CF_CFDP_ReceiveMessage() - gets a received message
 *
 * These functions were originally part of the CFDP engine itself
 * but were split into a separate file, both to improve testability
 * as well as to (potentially) allow interfaces to message/packet services
 * other than the CFE software bus.  However, Note that in this version,
 * there is still a fair amount of CFDP engine mixed into these functions
 * that would have to be isolated.
 *
 * Also note that the creation and deletion of SB pipes is not yet
 * moved into this file.
 */

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include "cf_cfdp_r.h"
#include "cf_cfdp_s.h"

#include <string.h>
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_MsgOutGet
 *
 * Application-scope internal function
 * See description in cf_cfdp_sbintf.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Logical_PduBuffer_t *CF_CFDP_MsgOutGet(const CF_Transaction_t *t, bool silent)
{
    /* if channel is frozen, do not take message */
    CF_Channel_t           *c = CF_AppData.engine.channels + t->chan_num;
    CF_Logical_PduBuffer_t *ret;

    /* this function should not be called more than once before the message
     * is sent, so if there's already an outgoing message allocated
     * then drop and get a new one (not likely) */
    ret = NULL;
    if (CF_AppData.engine.out.msg)
    {
        CFE_SB_ReleaseMessageBuffer(CF_AppData.engine.out.msg);
        CF_AppData.engine.out.msg = NULL;
    }

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
        if (!OS_ObjectIdDefined(c->sem_id) || OS_CountSemTimedWait(c->sem_id, 0) == OS_SUCCESS)
        {
            CF_AppData.engine.out.msg = CFE_SB_AllocateMessageBuffer(offsetof(CF_PduSendMsg_t, ph) + CF_MAX_PDU_SIZE);
        }

        if (!CF_AppData.engine.out.msg)
        {
            c->cur = t; /* remember where we were for next time */
            if (!silent)
            {
                CFE_EVS_SendEvent(CF_EID_ERR_CFDP_NO_MSG, CFE_EVS_EventType_ERROR,
                                  "CF: no output message buffer available");
            }
            goto error_out;
        }

        CFE_MSG_Init(&CF_AppData.engine.out.msg->Msg,
                     CFE_SB_ValueToMsgId(CF_AppData.config_table->chan[t->chan_num].mid_output), 0);
        ++CF_AppData.engine.outgoing_counter; /* even if max_outgoing_messages_per_wakeup is 0 (unlimited), it's ok
                                                    to inc this */

        /* prepare for encoding - the "tx_pdudata" is what serves as the temporary holding area for content */
        ret = &CF_AppData.engine.out.tx_pdudata;
    }

    /* if returning a buffer, then reset the encoder state to point to the beginning of the encapsulation msg */
    if (ret != NULL)
    {
        CF_CFDP_EncodeStart(&CF_AppData.engine.out.encode, CF_AppData.engine.out.msg, ret,
                            offsetof(CF_PduSendMsg_t, ph), offsetof(CF_PduSendMsg_t, ph) + CF_MAX_PDU_SIZE);
    }

error_out:
    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_Send
 *
 * Application-scope internal function
 * See description in cf_cfdp_sbintf.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_Send(uint8 chan_num, const CF_Logical_PduBuffer_t *ph)
{
    CFE_MSG_Size_t sb_msgsize;

    CF_Assert(chan_num < CF_NUM_CHANNELS);

    /* now handle the SB encapsulation - this should reflect the
     * length of the entire message, including encapsulation */
    sb_msgsize = offsetof(CF_PduSendMsg_t, ph);
    sb_msgsize += ph->pdu_header.header_encoded_length;
    sb_msgsize += ph->pdu_header.data_encoded_length;

    CFE_MSG_SetSize(&CF_AppData.engine.out.msg->Msg, sb_msgsize);
    CFE_MSG_SetMsgTime(&CF_AppData.engine.out.msg->Msg, CFE_TIME_GetTime());
    CFE_SB_TransmitBuffer(CF_AppData.engine.out.msg, true);

    ++CF_AppData.hk.channel_hk[chan_num].counters.sent.pdu;

    CF_AppData.engine.out.msg = NULL;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_ReceiveMessage
 *
 * Application-scope internal function
 * See description in cf_cfdp_sbintf.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ReceiveMessage(CF_Channel_t *c)
{
    CF_Transaction_t       *t; /* initialized below */
    uint32                  count = 0;
    int32                   status;
    const int               chan_num = (c - CF_AppData.engine.channels);
    CFE_SB_Buffer_t        *bufptr;
    CFE_MSG_Size_t          msg_size;
    CF_Logical_PduBuffer_t *ph;

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
        ph                       = &CF_AppData.engine.in.rx_pdudata;
        CFE_ES_PerfLogEntry(CF_PERF_ID_PDURCVD(chan_num));
        CFE_MSG_GetSize(&bufptr->Msg, &msg_size);
        CF_CFDP_DecodeStart(&CF_AppData.engine.in.decode, bufptr, ph, offsetof(CF_PduRecvMsg_t, ph), msg_size);
        if (!CF_CFDP_RecvPh(chan_num, ph))
        {
            /* got a valid pdu -- look it up by sequence number */
            t = CF_FindTransactionBySequenceNumber(c, ph->pdu_header.sequence_num, ph->pdu_header.source_eid);
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
                if (ph->pdu_header.source_eid == CF_AppData.config_table->local_eid &&
                    ph->fdirective.directive_code == CF_CFDP_FileDirective_FIN)
                {
                    if (!CF_CFDP_RecvFin(t, ph))
                    {
                        CF_Transaction_t t;

                        memset(&t, 0, sizeof(t));
                        CF_CFDP_InitTxnTxFile(&t, CF_CFDP_CLASS_2, 1, chan_num,
                                              0); /* populate transaction with needed fields for CF_CFDP_SendAck() */
                        if (CF_CFDP_SendAck(&t, CF_CFDP_AckTxnStatus_UNRECOGNIZED, CF_CFDP_FileDirective_FIN,
                                            ph->int_header.fin.cc, ph->pdu_header.destination_eid,
                                            ph->pdu_header.sequence_num) != CF_SendRet_NO_MSG)
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
                if (ph->pdu_header.destination_eid == CF_AppData.config_table->local_eid)
                {
                    /* we didn't find a match, so assign it to a transaction */
                    if (CF_AppData.hk.channel_hk[chan_num].q_size[CF_QueueIdx_RX] == CF_MAX_SIMULTANEOUS_RX)
                    {
                        CFE_EVS_SendEvent(
                            CF_EID_ERR_CFDP_RX_DROPPED, CFE_EVS_EventType_ERROR,
                            "CF: dropping packet from %lu transaction number 0x%08lx due max RX transactions reached",
                            (unsigned long)ph->pdu_header.source_eid, (unsigned long)ph->pdu_header.sequence_num);

                        /* NOTE: as there is no transaction (t) associated with this, there is no known channel,
                            and therefore no known counter to account it to (because dropped is per-chan) */
                    }
                    else
                    {
                        t = CF_FindUnusedTransaction(c);
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
                                      "CF: dropping packet for invalid destination eid 0x%lx",
                                      (unsigned long)ph->pdu_header.destination_eid);
                }
            }
        }

        CFE_ES_PerfLogExit(CF_PERF_ID_PDURCVD(chan_num));
    }
    CF_AppData.engine.in.msg = NULL;
}
