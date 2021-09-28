/************************************************************************
** File: cf_cfdp_r_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application CFDP receive logic stubs file
**
**  Stub file for all CFDP engine functionality specific to RX transactions.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_cfdp_helpers.h"
#include "cf_utils.h"

#include <stdio.h>
#include <string.h>
#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

typedef struct {
    transaction_t *t;
    pdu_header_t *pdu;
    uint32 gap_counter;
} gap_compute_args_t;

/************************************************************************/
/** \brief Helper function to store condition code set send_fin flag.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_SetCc(transaction_t *t, condition_code_t cc)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief CFDP R1 transaction reset function.
**  
**  \par Description
**       All R transactions use this call to indicate the transaction
**       state can be returned to the system. While this function currently
**       only calls CF_CFDP_ResetTransaction(), it is here as a placeholder.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R1_Reset(transaction_t *t)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief CFDP R2 transaction reset function.
**  
**  \par Description
**       Handles reset logic for R2, then calls R1 reset logic.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_Reset(transaction_t *t)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Checks that the transaction file's CRC matches expected.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on CRC match, otherwise error. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_CheckCrc(transaction_t *t, uint32 expected_crc)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Checks R2 transaction state for transaction completion status.
**  
**  \par Description
**       This function is called anywhere there's a desire to know if the
**       transaction has completed. It may trigger other actions by setting
**       flags to be handled during tick processing. In order for a
**       transaction to be complete, it must have had its meta-data PDU
**       received, the EOF must have been received, and there must be
**       no gaps in the file. EOF is not checked in this function, because
**       it's only called from functions after EOF is received.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_Complete(transaction_t *t, int ok_to_send_nak)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Process a filedata PDU on a transaction.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. bytes_received must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_ProcessFd(transaction_t *t, uint32 *bytes_received)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Processing receive EOF common functionality for R1/R2.
**  
**  \par Description
**       This function is used for both R1 and R2 eof receive. It calls
**       the unmarshaling function and then checks known transaction
**       data against the PDU.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_SubstateRecvEof(transaction_t *t, const pdu_header_t *pdu)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Process receive EOF for R1.
**  
**  \par Description
**       Only need to confirm crc for R1.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_R1_SubstateRecvEof(transaction_t *t, const pdu_header_t *pdu)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Process receive EOF for R2.
**  
**  \par Description
**       For R2, need to trigger the send of EOF-ACK and then call the
**       check complete function which will either send NAK or FIN.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_R2_SubstateRecvEof(transaction_t *t, const pdu_header_t *pdu)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Process received file data for R1.
**  
**  \par Description
**       For R1, only need to digest the CRC.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R1_SubstateRecvFileData(transaction_t *t, const pdu_header_t *pdu)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Process received file data for R2.
**  
**  \par Description
**       For R2, the CRC is checked after the whole file is received
**       since there may be gaps. Instead, insert file received range
**       data into chunks. Once NAK has been received, this function
**       always checks for completion. This function also re-arms
**       the ack timer.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_SubstateRecvFileData(transaction_t *t, const pdu_header_t *pdu)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Loads a single NAK segment request.
**  
**  \par Description
**       This is a function callback from cf_chunks_compuete_gaps().
**
**  \par Assumptions, External Events, and Notes:
**       chunks must not be NULL. c must not be NULL. opaque must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static void CF_CFDP_R2_GapCompute(const chunks_t *chunks, const chunk_t *c, void *opaque)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Send a NAK pdu for R2.
**  
**  \par Description
**       NAK pdu is sent when there are gaps in the received data. The
**       chunks class tracks this and generates the nak pdu by calculating
**       gaps internally and calling CF_CFDP_R2_GapCompute(). There is a special
**       case where if a metadata pdu has not been received, then a nak
**       packet will be sent to request another.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R_SubstateSendNak(transaction_t *t)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Initialize a transaction structure for R.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R_Init(transaction_t *t)
{    
    UT_GenStub_SetupReturnBuffer(CF_CFDP_R_Init, int);

    UT_GenStub_AddParam(CF_CFDP_R_Init, transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_R_Init, int);
}

/************************************************************************/
/** \brief Calculate up to the configured amount of bytes of CRC.
**  
**  \par Description
**       The configuration table has a number of bytes to calculate per
**       transaction per wakeup. At each wakeup, the file is read and
**       this number of bytes are calculated. This function will set
**       the checksum error condition code if the final crc does not match.
**
**  \par PTFO
**       Increase throughput by consuming all crc bytes per wakeup in
**       transaction-order. This would require a change to the meaning
**       of the value in the configuration table.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on completion, and -1 on non-completion. Error status is stored in condition code. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_CFDP_R2_CalcCrcChunk(transaction_t *t)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Send a FIN pdu.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 on success. Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int CFDP_R2_SubstateSendFin(transaction_t *t)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Process receive FIN-ACK pdu.
**  
**  \par Description
**       This is the end of an R2 transaction. Simply reset the transaction
**       state.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_Recv_fin_ack(transaction_t *t, const pdu_header_t *pdu)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Process receive metadata pdu for R2.
**  
**  \par Description
**       It's possible that metadata PDU was missed in cf_cfdp.c, or that
**       it was re-sent. This function checks if it was already processed,
**       and if not, handles it. If there was a temp file opened due to
**       missed metadata pdu, it will move the file to the correct
**       destination according to the metadata pdu.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R2_RecvMd(transaction_t *t, const pdu_header_t *pdu)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Dispatch function for all received packets.
**  
**  \par Description
**       For either R1 or R2 this function handles common logic for
**       state processing based on current sub-state and the received
**       pdu type.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. fns must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R_DispatchRecv(transaction_t *t, void (*const fns[SEND_NUM_STATES][PDU_INVALID_MAX])(transaction_t*, const pdu_header_t*), void (*const fd_fn)(transaction_t*, const pdu_header_t*))
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief R1 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R1_Recv(transaction_t *t)
{    
    UT_GenStub_AddParam(CF_CFDP_R_Init, transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R_Init, Basic, NULL);
}

/************************************************************************/
/** \brief R2 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R2_Recv(transaction_t *t)
{    
    UT_GenStub_AddParam(CF_CFDP_R2_Recv, transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_R2_Recv, Basic, NULL);
}

/************************************************************************/
/** \brief Cancel an R transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R_Cancel(transaction_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_R_Cancel), &t, sizeof(t));
    
    UT_DEFAULT_IMPL(CF_CFDP_R_Cancel);
}

/************************************************************************/
/** \brief Sends an inactivity timer expired event to EVS.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
static void CF_CFDP_R_SendInactivityEvent(transaction_t *t)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Perform tick (time-based) processing for R transactions.
**
**  \par Description
**       This function is called on every transaction by the engine on
**       every CF wakeup. This is where flags are checked to send ACK,
**       NAK, and FIN. It checks for inactivity timer and processes the
**       ack timer. The ack timer is what triggers re-sends of PDUs
**       that require acknowledgment.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_R_Tick(transaction_t *t, int *cont)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

