/************************************************************************
** File: cf_cfdp_s_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application CFDP send logic stubs file
**
**  Stub file for all CFDP engine functionality specific to TX transactions.
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
#include "cf_utils.h"
#include "cf_cfdp_helpers.h"

#include <stdio.h>
#include <string.h>
#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "uttools.h"
#include "utgenstub.h"

/************************************************************************/
/** \brief CFDP S1 transaction reset function.
**  
**  \par Description
**       All S transactions use this call to indicate the transaction
**       state can be returned to the system. While this function currently
**       only calls CF_CFDP_ResetTransaction(), it is here as a placeholder.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S_Reset(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Send an eof pdu.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode CF_SEND_SUCCESS on success. \endcode
**  \retcode CF_SEND_NO_MSG if message buffer cannot be obtained. \endcode
**  \retcode CF_SEND_ERROR if an error occurred while building the packet. \endcode
**  \endreturns
**
*************************************************************************/
// static cfdp_send_ret_t CF_CFDP_S_SendEof(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Sends an eof for S1.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S1_SubstateSendEof(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Triggers tick processing to send an EOF and wait for EOF-ACK for S2
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S2_SubstateSendEof(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Helper function to populate the pdu with file data and send it.
**  
**  \par Description
**       This function checks the file offset cache and if the desired
**       location is where the file offset is, it can skip a seek() call.
**       The file is read into the filedata pdu and then the pdu is sent.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
/* if bytes_to_read is 0, then read max possible */
// static int32 CF_CFDP_S_SendFileData(transaction_t *t, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Standard state function to send the next file data PDU for active transaction.
**  
**  \par Description
**       During the transfer of active transaction file data pdus, the file
**       offset is saved. This function sends the next chunk of data. If
**       the file offset equals the file size, then transition to the EOF
**       state.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
/* regular filedata send
 * based on t->foffs for current offset
 * checks for EOF and changes state if necessary */
// static void CF_CFDP_S_SubstateSendFileData(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Respond to a nak by sending filedata pdus as response.
**  
**  \par Description
**       Checks to see if a metadata pdu or filedata re-transmits must
**       occur.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 0 if no NAK processed. 1 if NAK processed. <0 if error. \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_CFDP_S_CheckAndRespondNak(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Send filedata handling for S2.
**  
**  \par Description
**       S2 will either respond to a NAK by sending retransmits, or in
**       absence of a NAK, it will send more of the original file data.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S2_SubstateSendFileData(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Send metadata PDU.
**  
**  \par Description
**       Construct and send a metadata PDU. This function determines the
**       size of the file to put in the metadata PDU.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S_SubstateSendMetadata(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Send FIN-ACK packet for S2.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S_SubstateSendFinAck(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief A fin was received before file complete, so abandon the transaction.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S2_EarlyFin(transaction_t *t, const pdu_header_t *pdu)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief S2 received FIN, so set flag to send FIN-ACK.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S2_Fin(transaction_t *t, const pdu_header_t *pdu)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief S2 NAK pdu received handling.
**  
**  \par Description
**       Stores the segment requests from the NAK packet in the chunks
**       structure. These can be used to generate re-transmit filedata
**       PDUs.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S2_Nak(transaction_t *t, const pdu_header_t *pdu)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief S2 NAK handling but with arming the NAK timer.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S2_Nak_Arm(transaction_t *t, const pdu_header_t *pdu)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief S2 received ack pdu in wait for EOF-ACK state.
**  
**  \par Description
**       This function will trigger a state transition to SEND_WAIT_FOR_FIN,
**       which waits for a FIN pdu.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. pdu must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S2_WaitForEofAck(transaction_t *t, const pdu_header_t *pdu)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Dispatch function for all received packets.
**  
**  \par Description
**       For either S1 or S2 this function handles common logic for
**       state processing based on current sub-state and the received
**       pdu type.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. fns must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_S_DispatchRecv(transaction_t *t, void (*const fns[SEND_NUM_STATES][PDU_INVALID_MAX])(transaction_t*, const pdu_header_t*))
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief S1 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S1_Recv(transaction_t *t)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}

/************************************************************************/
/** \brief S2 receive pdu processing.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Recv(transaction_t *t)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}

/************************************************************************/
/** \brief S1 dispatch function.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S1_Tx(transaction_t *t)
{
    UT_GenStub_AddParam(CF_CFDP_S1_Tx, transaction_t*, t);
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_S1_Tx), &t, sizeof(t));
    
    UT_GenStub_Execute(CF_CFDP_S1_Tx, Basic, NULL);
}

/************************************************************************/
/** \brief S2 dispatch function.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S2_Tx(transaction_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_S2_Tx), &t, sizeof(t));
    
    UT_DEFAULT_IMPL(CF_CFDP_S2_Tx);
}

/************************************************************************/
/** \brief Cancel an S transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_Cancel(transaction_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_S_Cancel), &t, sizeof(t));
    
    UT_DEFAULT_IMPL(CF_CFDP_S_Cancel);
}

/************************************************************************/
/** \brief Perform tick (time-based) processing for S transactions.
**
**  \par Description
**       This function is called on every transaction by the engine on
**       every CF wakeup. This is where flags are checked to send EOF or
**       FIN-ACK. If nothing else is sent, it checks to see if a NAK
**       retransmit must occur.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_Tick(transaction_t *t, int *cont /* unused */)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}

/************************************************************************/
/** \brief Perform NAK response for TX transactions
**
**  \par Description
**       This function is called at tick processing time to send pending
**       NAK responses. It indicates "cont" is 1 if there are more responses
**       left to send.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. cont must not be NULL.
**
*************************************************************************/
void CF_CFDP_S_Tick_Nak(transaction_t *t, int *cont)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}
