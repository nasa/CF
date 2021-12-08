/************************************************************************
** File: cf_cfdp_helper.c
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
**  The CF Application CFDP helper functions
**
** Revision 1.0 2021/04/21 sseeger
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

int CF_GetMemcpySize(const uint8 *num, int size)
{
    int i;

#if ENDIAN == _EL
    for (i = size - 1; i > 0; --i)
        if (num[i])
            break;
    ++i;
#elif ENDIAN == _EB
    for (i = 0; i < (size - 1); ++i)
        if (num[i])
            break;
    i = size - i;
#else
#error define ENDIAN
#endif

    return i;
}

void CF_MemcpyToBE(uint8 *dst, const uint8 *src, int src_size, int dst_size)
{
    CF_Assert((src_size > 0) && (dst_size > 0));
    CF_Assert(src_size >= dst_size);

#if ENDIAN == _EL
    dst += (dst_size - 1);
    while (dst_size--)
        *dst-- = *src++;
#elif ENDIAN == _EB
    src += (src_size - dst_size);
    while (dst_size--)
        *dst++ = *src++;
#else
#error define ENDIAN
#endif
}

/* copies bytes in big-endian order from a byte source */
static void CF_MemcpyFromBE(uint8 *dst, const uint8 *src, int src_size, int dst_size)
{
    CF_Assert((src_size > 0) && (dst_size > 0));
    CF_Assert(dst_size >= src_size);

    memset(dst, 0, dst_size);
#if ENDIAN == _EL
    src += (src_size - 1);
    while (src_size--)
        *dst++ = *src--;
#elif ENDIAN == _EB
    dst += (dst_size - src_size);
    while (src_size--)
        *dst++ = *src++;
#else
#error define ENDIAN
#endif
}

static int CF_GetTSNSize(const pdu_header_t *ph)
{
    uint8 field;
    int   ret;

    cfdp_get_uint8(field, ph->eid_tsn_lengths);
    ret = FGV(field, PDU_LENGTHS_TRANSACTION_SEQUENCE) + 1;

    if (ret > sizeof(cf_transaction_seq_t))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_GET_TSN_SIZE, CFE_EVS_EventType_ERROR,
                          "received TSN size %d too large for compiled max of %d", ret,
                          (uint32)sizeof(cf_transaction_seq_t));
        return -1;
    }

    return ret;
}

static int CF_GetEIDSize(const pdu_header_t *ph)
{
    uint8 field;
    int   ret;

    cfdp_get_uint8(field, ph->eid_tsn_lengths);
    ret = FGV(field, PDU_LENGTHS_ENTITY) + 1;

    if (ret > sizeof(cf_entity_id_t))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_PDU_GET_EID_SIZE, CFE_EVS_EventType_ERROR,
                          "received EID size %d too large for compiled max of %d", ret, (uint32)sizeof(cf_entity_id_t));
        return -1;
    }

    return ret;
}

/* get the variable length header items out of the PDU header and store as incoming data */
/* in.msg must be valid PDU message */
int CF_GetVariableHeader(void)
{
    pdu_header_t *ph    = &((pdu_r_msg_t *)CF_AppData.engine.in.msg)->ph;
    const int     eid_l = CF_GetEIDSize(ph);
    const int     tsn_l = CF_GetTSNSize(ph);
    int           offs  = sizeof(*ph);
    int           ret   = -1;

    if ((eid_l > 0) && (tsn_l > 0))
    {
        CF_MemcpyFromBE((uint8 *)&CF_AppData.engine.in.src, ((uint8 *)ph) + offs, eid_l, sizeof(cf_entity_id_t));
        offs += eid_l;
        CF_MemcpyFromBE((uint8 *)&CF_AppData.engine.in.tsn, ((uint8 *)ph) + offs, tsn_l, sizeof(cf_transaction_seq_t));
        offs += tsn_l;
        CF_MemcpyFromBE((uint8 *)&CF_AppData.engine.in.dst, ((uint8 *)ph) + offs, eid_l, sizeof(cf_entity_id_t));
        ret = 0;
    }

    return ret;
}

void CF_SetVariableHeader(cf_entity_id_t src_eid, cf_entity_id_t dst_eid, cf_transaction_seq_t tsn)
{
    pdu_header_t *ph      = &((pdu_s_msg_t *)CF_AppData.engine.out.msg)->ph;
    int           offs    = sizeof(*ph);
    const int     eid_s_l = CF_GetMemcpySize((uint8 *)&src_eid, sizeof(src_eid));
    const int     eid_d_l = CF_GetMemcpySize((uint8 *)&dst_eid, sizeof(dst_eid));
    const int     tsn_l   = CF_GetMemcpySize((uint8 *)&tsn, sizeof(tsn));
    const int     csize   = ((eid_s_l > eid_d_l) ? eid_s_l : eid_d_l);

    CF_MemcpyToBE(((uint8 *)ph) + offs, (uint8 *)&src_eid, sizeof(src_eid), csize);
    offs += csize;
    CF_MemcpyToBE(((uint8 *)ph) + offs, (uint8 *)&tsn, sizeof(tsn), tsn_l);
    offs += tsn_l;
    CF_MemcpyToBE(((uint8 *)ph) + offs, (uint8 *)&dst_eid, sizeof(dst_eid), csize);

    FSV(ph->eid_tsn_lengths, PDU_LENGTHS_ENTITY, csize - 1);
    FSV(ph->eid_tsn_lengths, PDU_LENGTHS_TRANSACTION_SEQUENCE, tsn_l - 1);
}

int CF_HeaderSize(const pdu_header_t *ph)
{
    uint8 temp;

    /* NOTE: assume header size is correct here (packet already validated via CF_GetVariableHeader, or
     * set by CF for outgoing PDU */
    cfdp_ldst_uint8(temp, ph->eid_tsn_lengths);
    const int eid_l = 1 + FGV(temp, PDU_LENGTHS_ENTITY);
    const int tsn_l = 1 + FGV(temp, PDU_LENGTHS_TRANSACTION_SEQUENCE);

    CF_Assert((eid_l > 0) && (tsn_l > 0));
    return sizeof(pdu_header_t) + (2 * eid_l) + tsn_l;
}
