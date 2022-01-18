/************************************************************************
 * File: cf_cfdp_dispatch.c
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
 *
 ************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_utils.h"

#include "cf_cfdp_dispatch.h"

#include <stdio.h>
#include <string.h>
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_R_DispatchRecv
 *
 * Application-scope internal function
 * See description in cf_cfdp_dispatch.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_R_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph,
                            const CF_CFDP_R_SubstateDispatchTable_t *dispatch, CF_CFDP_StateRecvFunc_t fd_fn)
{
    CF_Assert(t->state_data.r.sub_state < CF_RxSubState_NUM_STATES);
    CF_CFDP_StateRecvFunc_t              selected_handler;
    CF_Logical_PduFileDirectiveHeader_t *fdh;

    selected_handler = NULL;

    /* the CF_CFDP_R_SubstateDispatchTable_t is only used with file directive pdu */
    if (ph->pdu_header.pdu_type == 0)
    {
        fdh = &ph->fdirective;
        if (fdh->directive_code < CF_CFDP_FileDirective_INVALID_MAX)
        {
            if (dispatch->state[t->state_data.r.sub_state] != NULL)
            {
                selected_handler = dispatch->state[t->state_data.r.sub_state]->fdirective[fdh->directive_code];
            }
        }
        else
        {
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious;
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_R_DC_INV, CFE_EVS_EventType_ERROR,
                              "CF R%d(%lu:%lu): received pdu with invalid directive code %d for sub-state %d",
                              (t->state == CF_TxnState_R2), (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num, fdh->directive_code, t->state_data.r.sub_state);
        }
    }
    else
    {
        if (t->history->cc == CF_CFDP_ConditionCode_NO_ERROR)
        {
            selected_handler = fd_fn;
        }
        else
        {
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped;
        }
    }

    /*
     * NOTE: if no handler is selected, this will drop packets on the floor here,
     * without incrementing any counter.  This was existing behavior.
     */
    if (selected_handler != NULL)
    {
        selected_handler(t, ph);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_S_DispatchRecv
 *
 * Application-scope internal function
 * See description in cf_cfdp_dispatch.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_DispatchRecv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph,
                            const CF_CFDP_S_SubstateRecvDispatchTable_t *dispatch)
{
    CF_Assert(t->state_data.s.sub_state < CF_TxSubState_NUM_STATES);
    const CF_CFDP_FileDirectiveDispatchTable_t *substate_tbl;
    CF_CFDP_StateRecvFunc_t                     selected_handler;
    CF_Logical_PduFileDirectiveHeader_t        *fdh;

    /* send state, so we only care about file directive PDU */
    selected_handler = NULL;
    if (ph->pdu_header.pdu_type == 0)
    {
        fdh = &ph->fdirective;
        if (fdh->directive_code < CF_CFDP_FileDirective_INVALID_MAX)
        {
            /* This should be silent (no event) if no handler is defined in the table */
            substate_tbl = dispatch->substate[t->state_data.s.sub_state];
            if (substate_tbl != NULL)
            {
                selected_handler = substate_tbl->fdirective[fdh->directive_code];
            }
        }
        else
        {
            ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.spurious;
            CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_DC_INV, CFE_EVS_EventType_ERROR,
                              "CF S%d(%lu:%lu): received pdu with invalid directive code %d for sub-state %d",
                              (t->state == CF_TxnState_S2), (unsigned long)t->history->src_eid,
                              (unsigned long)t->history->seq_num, fdh->directive_code, t->state_data.s.sub_state);
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_EID_ERR_CFDP_S_NON_FD_PDU, CFE_EVS_EventType_ERROR,
                          "CF S%d(%lu:%lu): received non-file directive pdu", (t->state == CF_TxnState_S2),
                          (unsigned long)t->history->src_eid, (unsigned long)t->history->seq_num);
    }

    /* check that there's a valid function pointer. if there isn't,
     * then silently ignore. We may want to discuss if it's worth
     * shutting down the whole transation if a PDU is received
     * that doesn't make sense to be received (For example,
     * class 1 CFDP receiving a NAK PDU) but for now, we silently
     * ignore the received packet and keep chugging along. */
    if (selected_handler)
    {
        selected_handler(t, ph);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_S_DispatchTransmit
 *
 * Application-scope internal function
 * See description in cf_cfdp_dispatch.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_S_DispatchTransmit(CF_Transaction_t *t, const CF_CFDP_S_SubstateSendDispatchTable_t *dispatch)
{
    CF_CFDP_StateSendFunc_t selected_handler;

    selected_handler = dispatch->substate[t->state_data.s.sub_state];
    if (selected_handler != NULL)
    {
        selected_handler(t);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_TxStateDispatch
 *
 * Application-scope internal function
 * See description in cf_cfdp_dispatch.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_TxStateDispatch(CF_Transaction_t *t, const CF_CFDP_TxnSendDispatchTable_t *dispatch)
{
    CF_CFDP_StateSendFunc_t selected_handler;

    CF_Assert(t->state < CF_TxnState_INVALID);
    selected_handler = dispatch->tx[t->state];
    if (selected_handler != NULL)
    {
        selected_handler(t);
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CFDP_RxStateDispatch
 *
 * Application-scope internal function
 * See description in cf_cfdp_dispatch.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CFDP_RxStateDispatch(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph,
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
