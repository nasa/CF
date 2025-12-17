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
#include "cf_cfdp_sbintf.h"

#include <string.h>
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_sbintf.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_Logical_PduBuffer_t *CF_CFDP_MsgOutGet(const CF_Transaction_t *txn, bool silent)
{
    /* if channel is frozen, do not take message */
    CF_Channel_t *          chan    = CF_AppData.engine.channels + txn->chan_num;
    bool                    success = true;
    CF_Logical_PduBuffer_t *ret;
    int32                   os_status;

    /* this function should not be called more than once before the message
     * is sent, so if there's already an outgoing message allocated
     * then drop and get a new one (not likely) */
    ret = NULL;
    if (CF_AppData.engine.out.msg)
    {
        CFE_SB_ReleaseMessageBuffer(CF_AppData.engine.out.msg);
        CF_AppData.engine.out.msg = NULL;
    }

    if (CF_AppData.config_table->chan[txn->chan_num].max_outgoing_messages_per_wakeup &&
        (chan->outgoing_counter >= CF_AppData.config_table->chan[txn->chan_num].max_outgoing_messages_per_wakeup))
    {
        /* no more messages this wakeup allowed */
        success = false;
    }

    if (success && !CF_AppData.hk.Payload.channel_hk[txn->chan_num].frozen && !txn->flags.com.suspended)
    {
        /* first, check if there's room in the pipe for the message we want to build */
        if (OS_ObjectIdDefined(chan->sem_id))
        {
            os_status = OS_CountSemTimedWait(chan->sem_id, 0);
        }
        else
        {
            os_status = OS_SUCCESS;
        }

        /* Allocate message buffer on success */
        if (os_status == OS_SUCCESS)
        {
            CF_AppData.engine.out.msg = CFE_SB_AllocateMessageBuffer(offsetof(CF_PduTlmMsg_t, ph) + CF_MAX_PDU_SIZE +
                                                                     CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES);
        }

        if (!CF_AppData.engine.out.msg)
        {
            if (!silent && (os_status == OS_SUCCESS))
            {
                CFE_EVS_SendEvent(CF_CFDP_NO_MSG_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CF: no output message buffer available");
            }
            success = false;
        }

        if (success)
        {
            CFE_MSG_Init(&CF_AppData.engine.out.msg->Msg,
                         CFE_SB_ValueToMsgId(CF_AppData.config_table->chan[txn->chan_num].mid_output),
                         offsetof(CF_PduTlmMsg_t, ph));
            ++chan->outgoing_counter; /* even if max_outgoing_messages_per_wakeup is 0 (unlimited), it's ok
                                                    to inc this */

            /* prepare for encoding - the "tx_pdudata" is what serves as the temporary holding area for content */
            ret = &CF_AppData.engine.out.tx_pdudata;
        }
    }

    /* If returning NULL, this only happens when we hit some traffic limit for this channel,
     * either the limit of tx per wakeup, the sync sem is unavailable, or the SB pool is empty. */
    if (ret == NULL)
    {
        /* stop trying to send anything until next wake up */
        chan->tx_blocked = true;
    }
    else
    {
        /* if returning a buffer, then reset the encoder state to point to the beginning of the encapsulation msg */
        CF_CFDP_EncodeStart(&CF_AppData.engine.out.encode, CF_AppData.engine.out.msg, ret, offsetof(CF_PduTlmMsg_t, ph),
                            offsetof(CF_PduTlmMsg_t, ph) + CF_MAX_PDU_SIZE);
    }

    return ret;
}

/*----------------------------------------------------------------
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
    sb_msgsize = offsetof(CF_PduTlmMsg_t, ph);
    sb_msgsize += ph->pdu_header.header_encoded_length;
    sb_msgsize += ph->pdu_header.data_encoded_length;
    sb_msgsize += CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES;

    CFE_MSG_SetSize(&CF_AppData.engine.out.msg->Msg, sb_msgsize);
    CFE_MSG_SetMsgTime(&CF_AppData.engine.out.msg->Msg, CFE_TIME_GetTime());
    CFE_SB_TransmitBuffer(CF_AppData.engine.out.msg, true);

    ++CF_AppData.hk.Payload.channel_hk[chan_num].counters.sent.pdu;

    CF_AppData.engine.out.msg = NULL;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cfdp_sbintf.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_ReceiveMessage(CF_Channel_t *chan)
{
    uint32           count = 0;
    int32            status;
    const int        chan_num = (chan - CF_AppData.engine.channels);
    CFE_SB_Buffer_t *bufptr;
    CFE_MSG_Size_t   msg_size;
    CFE_MSG_Type_t   msg_type = CFE_MSG_Type_Invalid;

    CF_Logical_PduBuffer_t *ph;

    for (; count < CF_AppData.config_table->chan[chan_num].rx_max_messages_per_wakeup; ++count)
    {
        status = CFE_SB_ReceiveBuffer(&bufptr, chan->pipe, CFE_SB_POLL);
        if (status != CFE_SUCCESS)
        {
            break; /* no more messages */
        }

        ph = &CF_AppData.engine.in.rx_pdudata;
        CFE_ES_PerfLogEntry(CF_PERF_ID_PDURCVD(chan_num));
        CFE_MSG_GetSize(&bufptr->Msg, &msg_size);
        CFE_MSG_GetType(&bufptr->Msg, &msg_type);
        if (msg_size > CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES)
        {
            /* Ignore/subtract any fixed trailing bytes */
            msg_size -= CF_PDU_ENCAPSULATION_EXTRA_TRAILING_BYTES;
        }
        else
        {
            /* bad message size - not supposed to happen */
            msg_size = 0;
        }
        if (msg_type == CFE_MSG_Type_Tlm)
        {
            CF_CFDP_DecodeStart(&CF_AppData.engine.in.decode, bufptr, ph, offsetof(CF_PduTlmMsg_t, ph), msg_size);
        }
        else
        {
            CF_CFDP_DecodeStart(&CF_AppData.engine.in.decode, bufptr, ph, offsetof(CF_PduCmdMsg_t, ph), msg_size);
        }

        /* Identify and dispatch this PDU */
        CF_CFDP_ReceivePdu(chan, ph);

        CFE_ES_PerfLogExit(CF_PERF_ID_PDURCVD(chan_num));
    }
}
