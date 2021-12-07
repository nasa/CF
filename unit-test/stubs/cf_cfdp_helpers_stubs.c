/************************************************************************
** File: cf_cfdp_helpers_stubs.h
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  Stubs to facilitate unit testing based upon the cf_cfdp_pdu.h file to
**  create these stubs because the definitions for the methods in
**  cf_cfdp_helpers.c are in there not in cf_cfdp_helpers.h.
**
** Revision 1.0 2020/05/01 sseeger
**  Initial revision
*************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cf_cfdp_pdu
 *header.  Manual updates Alan S. Gibson NASA/GSFC 587
 */
#include "cfe.h"
#include "cf_cfdp_helpers.h"
#include "cf_platform_cfg.h"
#include "cf_cfdp_pdu.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_GetMemcpySize()
 * ----------------------------------------------------
 */
int CF_GetMemcpySize(const uint8_t *num, int size)
{
    int forced_return; /* TODO: CF_GetMemcpySize should be using a full context with num and size, but is only using a
                          forced_return */

    UT_GenStub_SetupReturnBuffer(CF_GetMemcpySize, int);

    UT_GenStub_AddParam(CF_GetMemcpySize, const uint8_t *, num);
    UT_GenStub_AddParam(CF_GetMemcpySize, int, size);

    UT_GenStub_Execute(CF_GetMemcpySize, Basic, NULL);

    UT_Stub_CopyToLocal(UT_KEY(CF_GetMemcpySize), &forced_return, sizeof(forced_return));
    UT_Stub_SetReturnValue(UT_KEY(CF_GetMemcpySize), forced_return);

    return UT_GenStub_GetReturnValue(CF_GetMemcpySize, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_GetVariableHeader()
 * ----------------------------------------------------
 */
int CF_GetVariableHeader(void)
{
    UT_GenStub_SetupReturnBuffer(CF_GetVariableHeader, int);

    UT_GenStub_Execute(CF_GetVariableHeader, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_GetVariableHeader, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_HeaderSize()
 * ----------------------------------------------------
 */
int CF_HeaderSize(const pdu_header_t *ph)
{
    UT_GenStub_SetupReturnBuffer(CF_HeaderSize, int);

    UT_GenStub_AddParam(CF_HeaderSize, const pdu_header_t *, ph);

    UT_GenStub_Execute(CF_HeaderSize, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_HeaderSize, int);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_MemcpyToBE()
 * ----------------------------------------------------
 */
void CF_MemcpyToBE(uint8 *dst, const uint8 *src, int src_size, int dst_size)
{
    UT_GenStub_AddParam(CF_MemcpyToBE, uint8 *, dst);
    UT_GenStub_AddParam(CF_MemcpyToBE, const uint8 *, src);
    UT_GenStub_AddParam(CF_MemcpyToBE, int, src_size);
    UT_GenStub_AddParam(CF_MemcpyToBE, int, dst_size);

    UT_GenStub_Execute(CF_MemcpyToBE, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_SetVariableHeader()
 * ----------------------------------------------------
 */
void CF_SetVariableHeader(cf_entity_id_t src_eid, cf_entity_id_t dst_eid, cf_transaction_seq_t tsn)
{
    UT_GenStub_AddParam(CF_SetVariableHeader, cf_entity_id_t, src_eid);
    UT_GenStub_AddParam(CF_SetVariableHeader, cf_entity_id_t, dst_eid);
    UT_GenStub_AddParam(CF_SetVariableHeader, cf_transaction_seq_t, tsn);

    UT_GenStub_Execute(CF_SetVariableHeader, Basic, NULL);
}
