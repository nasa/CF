/************************************************************************
** File: cf_crc_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application CRC calculation stubs file
**
**  This is a streaming CRC calculator. Data can all be given at once for
**  a result or it can trickle in.
**
**  This file is intended to be generic and usable by other apps.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_crc.h"
#include <string.h>

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

/************************************************************************/
/** \brief Start a CRC streamable digest.
**  
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
*************************************************************************/
void CF_CRC_Start(cf_crc_t *c)
{
    UT_GenStub_AddParam(CF_CRC_Start, cf_crc_t *, c);

    UT_GenStub_Execute(CF_CRC_Start, Basic, NULL);
}

/************************************************************************/
/** \brief Digest a chunk for crc calculation.
**
**  \par Description
**       Does the CRC calculation, and stores an index into the given
**       4-byte word in case the input was not evenly divisible for 4.
**  
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
*************************************************************************/
void CF_CRC_Digest(cf_crc_t *c, const uint8 *data, int len)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CRC_Digest), &c, sizeof(c));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CRC_Digest), &data, sizeof(data));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CRC_Digest), &len, sizeof(len));
    
    UT_DEFAULT_IMPL(CF_CRC_Digest);
}

/************************************************************************/
/** \brief Finalize a crc calculation.
**  
**  \par Description
**       Checks the index and if it isn't 0, does the final calculations
**       on the bytes in the shift register. After this call is made, the
**       result field of the structure holds the result.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
*************************************************************************/
void CF_CRC_Finalize(cf_crc_t *c)
{
    UT_GenStub_AddParam(CF_CRC_Finalize, cf_crc_t *, c);

    /* TODO: UT_Stub_CopyFromLocal being used, but should be removed in defference to handler style functions */
    UT_Stub_CopyFromLocal(UT_KEY(CF_CRC_Finalize), &c, sizeof(c));

    UT_GenStub_Execute(CF_CRC_Finalize, Basic, NULL);

}

