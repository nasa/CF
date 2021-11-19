/************************************************************************
** File: cf_cfdp_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  Stubs file for the CF Application main cfdp engine and pdu parsing file
**
**  This file contains two sets of functions. The first is what is needed
**  to deal with CFDP PDUs. Specifically validating them for correctness
**  and ensuring the byte-order is correct for the target. The second
**  is incoming and outgoing CFDP PDUs pass through here. All receive
**  CFDP PDU logic is performed here and the data is passed to the
**  R (rx) and S (tx) logic.
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

#include <string.h>
#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

#include "cf_test_utils.h"

#define NUM_CLISTS 4
const int CF_max_chunks[CF_DIR_NUM][CF_NUM_CHANNELS] = {CF_CHANNEL_NUM_RX_CHUNKS_PER_TRANSACTION, CF_CHANNEL_NUM_TX_CHUNKS_PER_TRANSACTION};

// static void CF_CFDP_RecvIdle(transaction_t*);
// static void CF_CFDP_RecvDrop(transaction_t*);

// static void CF_CFDP_TxFile__(transaction_t *t, cfdp_class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority);


typedef struct trans_seq_arg_t {
    cf_transaction_seq_t transaction_sequence_number;
    cf_entity_id_t src_eid;
    transaction_t *t; /* out param */
} trans_seq_arg_t;

typedef struct CF_CFDP_CycleTx_args_t {
    channel_t *c;
    int ran_one;
} CF_CFDP_CycleTx_args_t;

typedef struct {
    channel_t *c; /* IN param */
    void(*fn)(transaction_t*); /* IN param */
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
void CF_CFDP_ArmAckTimer(transaction_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ArmAckTimer), &t, sizeof(t));
    
    UT_DEFAULT_IMPL(CF_CFDP_ArmAckTimer);
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
// static cfdp_class_t CF_CFDP_GetClass(transaction_t *ti)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static int CF_CFDP_IsSender(transaction_t *ti)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static void CF_CFDP_ArmInactTimer(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Dispatch received packet to its transaction.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be null. It must be an initialized transaction.
**
*************************************************************************/
// static void CF_CFDP_DispatchRecv(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Dispatches control to the active tx transaction.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. t must be a valid tx transaction.
**
*************************************************************************/
// static void CF_CFDP_DispatchTx(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static chunks_wrapper_t *CF_CFDP_FindUnusedChunks(channel_t *c, direction_t dir)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static transaction_t *CF_CFDP_FindUnusedTransaction(channel_t *c)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Returns a history structure back to its unused state.
**  
**  \par Description
**       There's nothing to do currently other than remove the history
**       from its current queue and put it back on CF_Q_HIST_FREE.
**
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL. h must not be NULL.
**
*************************************************************************/
void CF_CFDP_ResetHistory(channel_t *c, history_t *h)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ResetHistory), &c, sizeof(c));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ResetHistory), &h, sizeof(h));
    
    UT_DEFAULT_IMPL(CF_CFDP_ResetHistory);
}

/************************************************************************/
/** \brief Frees and resets a transaction and returns it for later use.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_FreeTransaction(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static int CF_CFDP_FindTransactionBySequenceNumber_(clist_node n, trans_seq_arg_t *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
transaction_t *CF_CFDP_FindTransactionBySequenceNumber(channel_t *c, cf_transaction_seq_t transaction_sequence_number, cf_entity_id_t src_eid)
{
    transaction_t*  forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &c,
      sizeof(channel_t *));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &transaction_sequence_number,
      sizeof(cf_transaction_seq_t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &src_eid,
      sizeof(cf_entity_id_t));
    
    UT_DEFAULT_IMPL(CF_CFDP_FindTransactionBySequenceNumber);
    
    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_FindTransactionBySequenceNumber), &forced_return,
      sizeof(transaction_t*));
      
    return forced_return;
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
**  \retstmt Pointer to a pdu_header_t within a software bus buffer on success. Otherwise NULL. \endcode
**  \endreturns
**
*************************************************************************/
pdu_header_t *CF_CFDP_MsgOutGet(const transaction_t *t, int silent)
{
    pdu_header_t* forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_MsgOutGet), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_MsgOutGet), &silent, 
      sizeof(silent));
    
    UT_DEFAULT_IMPL(CF_CFDP_MsgOutGet);
      
    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_MsgOutGet), &forced_return, 
      sizeof(forced_return));
      
    return forced_return;
}

/************************************************************************/
/** \brief Sends the current output buffer via the software bus.
**  
**  \par Assumptions, External Events, and Notes:
**       The PDU in the output buffer is ready to transmit.
**
*************************************************************************/
// static void CF_CFDP_Send(uint8 chan_num, uint32 len)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Build the PDU header in the output buffer to prepare to send a packet.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/

void Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_Stub_CopyToReturnValue(UT_KEY(CF_CFDP_ConstructPduHeader), UserObj, sizeof(pdu_header_t *));
}

void Handler_CF_CFDP_ConstructPduHeader_Default(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    const transaction_t *t = UT_Hook_GetArgValueByName(Context, "t", const transaction_t *);
    uint8                directive_code = UT_Hook_GetArgValueByName(Context, "directive_code", uint8);
    cf_entity_id_t       src_eid = UT_Hook_GetArgValueByName(Context, "src_eid", cf_entity_id_t);
    cf_entity_id_t       dst_eid = UT_Hook_GetArgValueByName(Context, "dst_eid", cf_entity_id_t);
    uint8                towards_sender = UT_Hook_GetArgValueByName(Context, "towards_sender", uint8);
    cf_transaction_seq_t tsn = UT_Hook_GetArgValueByName(Context, "tsn", cf_transaction_seq_t);
    int                  silent = UT_Hook_GetArgValueByName(Context, "silent", int);
    pdu_header_t*        forced_return;

    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &directive_code, sizeof(directive_code));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &src_eid, sizeof(src_eid));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &dst_eid, sizeof(dst_eid));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &towards_sender, sizeof(towards_sender));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &tsn, sizeof(tsn));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &silent, sizeof(silent));

    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_ConstructPduHeader), &forced_return, sizeof(forced_return));

    UT_Stub_SetReturnValue(UT_KEY(CF_CFDP_ConstructPduHeader), forced_return);
}

pdu_header_t *CF_CFDP_ConstructPduHeader(const transaction_t *t, uint8 directive_code, cf_entity_id_t src_eid, cf_entity_id_t dst_eid, uint8 towards_sender, cf_transaction_seq_t tsn, int silent)
{  
    UT_GenStub_SetupReturnBuffer(CF_CFDP_ConstructPduHeader, pdu_header_t *);

    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, const transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, uint8, directive_code);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, cf_entity_id_t, src_eid);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, cf_entity_id_t, dst_eid);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, uint8, towards_sender);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, cf_transaction_seq_t, tsn);
    UT_GenStub_AddParam(CF_CFDP_ConstructPduHeader, int, silent);

    UT_GenStub_Execute(CF_CFDP_ConstructPduHeader, Basic, Handler_CF_CFDP_ConstructPduHeader_Default);

    return UT_GenStub_GetReturnValue(CF_CFDP_ConstructPduHeader, pdu_header_t *);
}

/************************************************************************/
/** \brief Build a metadata PDU for transmit.
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
cfdp_send_ret_t CF_CFDP_SendMd(transaction_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendMd), &t, sizeof(transaction_t*));

    /* TODO: this type of return is a short cut, update to new handler for correct usage */
    return (cfdp_send_ret_t)UT_DEFAULT_IMPL(CF_CFDP_SendMd); 
}

/************************************************************************/
/** \brief Construct a fd pdu header
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt A valid pdu header if successful, otherwise NULL. \endcode
**  \endreturns
**
*************************************************************************/
pdu_header_t *CF_CFDP_EarlySendFd(transaction_t *t)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}

/************************************************************************/
/** \brief Build a filedata PDU for transmit.
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
cfdp_send_ret_t CF_CFDP_SendFd(transaction_t *t, uint32 offset, int len)
{
    cfdp_send_ret_t  forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendFd), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendFd), &offset, sizeof(offset));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendFd), &len, sizeof(len));
    
    UT_DEFAULT_IMPL(CF_CFDP_SendFd);
    
    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_SendFd), &forced_return,
      sizeof(forced_return));
    
    return forced_return;
}

/************************************************************************/
/** \brief Common functionality between SendEof and SendFin
**  
**  \par Assumptions, External Events, and Notes:
**       pdu must not be NULL.
**
**  \returns
**  \retstmt Returns the tlv length \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_CFDP_FinishEofAck(pdu_header_t *pdu)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Build a eof PDU for transmit.
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
cfdp_send_ret_t CF_CFDP_SendEof(transaction_t *t)
{
    cfdp_send_ret_t forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendEof), &t, sizeof(transaction_t*));
    
    UT_DEFAULT_IMPL(CF_CFDP_SendEof);
    
    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_SendEof), &forced_return, 
      sizeof(forced_return));
    
    return forced_return;
}

/************************************************************************/
/** \brief Build a ack PDU for transmit.
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
cfdp_send_ret_t CF_CFDP_SendAck(transaction_t *t, ack_transaction_status_t ts, file_directive_t dir_code, condition_code_t cc, cf_entity_id_t peer_eid, cf_transaction_seq_t tsn)
{
    cfdp_send_ret_t forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendAck), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendAck), &ts, sizeof(ts));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendAck), &dir_code, sizeof(dir_code));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendAck), &cc, sizeof(cc));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendAck), &peer_eid, sizeof(peer_eid));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendAck), &tsn, sizeof(tsn));
    
    UT_DEFAULT_IMPL(CF_CFDP_SendAck);
    
    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_SendAck), &forced_return, 
      sizeof(forced_return));
      
    return forced_return;
}

/************************************************************************/
/** \brief Build a fin PDU for transmit.
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
cfdp_send_ret_t CF_CFDP_SendFin(transaction_t *t, fin_delivery_code_t dc, fin_file_status_t fs, condition_code_t cc)
{
    UT_GenStub_SetupReturnBuffer(CF_CFDP_SendFin, int32);

    UT_GenStub_AddParam(CF_CFDP_SendFin, transaction_t *, t);
    UT_GenStub_AddParam(CF_CFDP_SendFin, fin_delivery_code_t, dc);
    UT_GenStub_AddParam(CF_CFDP_SendFin, fin_file_status_t, fs);
    UT_GenStub_AddParam(CF_CFDP_SendFin, condition_code_t, cc);

    UT_GenStub_Execute(CF_CFDP_SendFin, Basic, NULL);

    /* TODO: CF_CFDP_SendFin stub using short cut return type and needs replaced with correct return handling */
    return (cfdp_send_ret_t) UT_GenStub_GetReturnValue(CF_CFDP_SendFin, int32);
}

/************************************************************************/
/** \brief Build a nak PDU for transmit.
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
cfdp_send_ret_t CF_CFDP_SendNak(transaction_t *t, int num_segment_requests)
{
    
    cfdp_send_ret_t forced_return;

    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendNak), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_SendNak), &num_segment_requests, sizeof(num_segment_requests));

    UT_DEFAULT_IMPL(CF_CFDP_SendNak);

    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_SendNak), &forced_return, sizeof(forced_return));

    return forced_return;
}

/************************************************************************/
/** \brief Construct a nak pdu header
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt A valid pdu header if successful, otherwise NULL. \endcode
**  \endreturns
**
*************************************************************************/
pdu_header_t *CF_CFDP_EarlySendNak(transaction_t *t)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
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
// static int CF_CFDP_RecvPh(int chan_num)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
int CF_CFDP_RecvMd(transaction_t *t)
{   
    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvMd, int32);

    UT_GenStub_AddParam(CF_CFDP_RecvMd, transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_RecvMd, Basic, NULL);
    /* TODO: cast as int is a quick work around, should update to using int */
    return (int) UT_GenStub_GetReturnValue(CF_CFDP_RecvMd, int32);
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

int CF_CFDP_RecvFd(transaction_t *t)
{   
    int forced_return = __INT_MAX__;

    UT_GenStub_SetupReturnBuffer(CF_CFDP_RecvFd, int);

    UT_Stub_SetReturnValue(UT_KEY(CF_CFDP_RecvFd), forced_return);

    UT_GenStub_AddParam(CF_CFDP_RecvFd, transaction_t *, t);

    UT_GenStub_Execute(CF_CFDP_RecvFd, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CFDP_RecvFd, int);
}

/************************************************************************/
/** \brief Unpack an eof PDU from a received message.
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
int CF_CFDP_RecvEof(void)
{
    return (int) UT_DEFAULT_IMPL(CF_CFDP_RecvEof);
}

/************************************************************************/
/** \brief Unpack an ack PDU from a received message.
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
int CF_CFDP_RecvAck(void)
{
    return (int) UT_DEFAULT_IMPL(CF_CFDP_RecvAck);
}

/************************************************************************/
/** \brief Unpack an ack PDU from a received message.
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
int CF_CFDP_RecvFin(void)
{
    return UT_DEFAULT_IMPL(CF_CFDP_RecvFin);
}

/************************************************************************/
/** \brief Unpack a nak PDU from a received message.
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
int CF_CFDP_RecvNak(int *num_segment_requests)
{
    UT_Stub_CopyToLocal(UT_KEY(CF_CFDP_RecvNak), num_segment_requests, sizeof(int));

    return (int) UT_DEFAULT_IMPL(CF_CFDP_RecvNak);
}

/************************************************************************/
/** \brief Receive state function to ignore a packet.
**  
**  \par Description
**       This function signature must match all receive state functions.
**       The parameter t is ignored here.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. There must be a received message.
**
*************************************************************************/
// static void CF_CFDP_RecvDrop(transaction_t *t)
// {
//     ++CF_AppData.hk.channel_hk[t->chan_num].counters.recv.dropped;
// }

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
// static void CF_CFDP_RecvIdle(transaction_t *t)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
    return UT_DEFAULT_IMPL(CF_CFDP_InitEngine);
}

/************************************************************************/
/** \brief Process received message on channel PDU input pipe.
**  
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_ReceiveMessage(channel_t *c)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static int CF_CFDP_CycleTx_(clist_node node, void *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static void CF_CFDP_CycleTx(channel_t *c)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static int CF_CFDP_DoTick(clist_node node, void *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Call R and then S tick functions for all active transactions.
**  
**  \par Description
**       Traverses all transactions in the RX and TXW queues, and calls
**       their tick functions.
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
// static void CF_CFDP_TickTransactions(channel_t *c)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Helper function to set tx file state in a transaction.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_TxFile__(transaction_t *t, cfdp_class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Helper function to set tx fiel state in a transaction.
**  
**  \par Description
**       Sets up the transaction structure, including finding unused
**       chunks and inserting the transaction into the PEND queue.
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
// static void CF_CFDP_TxFile_(transaction_t *t, cfdp_class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, cf_entity_id_t dest_id)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
int32 CF_CFDP_TxFile(const char src_filename[CF_FILENAME_MAX_LEN], const char dst_filename[CF_FILENAME_MAX_LEN], cfdp_class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, cf_entity_id_t dest_id)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_TxFile), src_filename, 
      CF_FILENAME_MAX_LEN);
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_TxFile), dst_filename, 
      CF_FILENAME_MAX_LEN);
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_TxFile), &cfdp_class, 
      sizeof(cfdp_class));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_TxFile), &keep, 
      sizeof(keep));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_TxFile), &chan, 
      sizeof(chan));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_TxFile), &priority, 
      sizeof(priority));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_TxFile), &dest_id, 
      sizeof(dest_id));
    
    
    return UT_DEFAULT_IMPL(CF_CFDP_TxFile);
}

/************************************************************************/
/** \brief Helper function to set up directory playback.
**  
**  \par Description
**       Sets up playback_t data. Used by both playback and polling functions.
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
// static int32 CF_CFDP_PlaybackDir_(playback_t *p, const char src_filename[CF_FILENAME_MAX_LEN], const char dst_filename[CF_FILENAME_MAX_LEN], cfdp_class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, cf_entity_id_t dest_id)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Begin transmit of a directory.
**  
**  \par Description
**       This function sets up playback_t structure with state so it can
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
int32 CF_CFDP_PlaybackDir(const char src_filename[CF_FILENAME_MAX_LEN], const char dst_filename[CF_FILENAME_MAX_LEN], cfdp_class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, uint16 dest_id)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_PlaybackDir), src_filename, 
      CF_FILENAME_MAX_LEN);
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_PlaybackDir), dst_filename, 
      CF_FILENAME_MAX_LEN);
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_PlaybackDir), &cfdp_class, 
      sizeof(cfdp_class));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_PlaybackDir), &keep, 
      sizeof(keep));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_PlaybackDir), &chan, 
      sizeof(chan));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_PlaybackDir), &priority, 
      sizeof(priority));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_PlaybackDir), &dest_id, 
      sizeof(dest_id));
    
    
    return UT_DEFAULT_IMPL(CF_CFDP_PlaybackDir);
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
// static void CF_CFDP_ProcessPlaybackDirectory(channel_t *c, playback_t *p)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Update the playback or polling counter for channel HK
**  
**  \par Assumptions, External Events, and Notes:
**       c must be a valid channel index. pb must not be NULL. counter
**       must not be NULL.
**
*************************************************************************/
// static void CF_CFDP_UpdatePollPbCounted(int chan_index, playback_t *pb, int up, uint8 *counter)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static void CF_CFDP_ProcessPlaybackDirectories(channel_t *c)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

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
// static void CF_CFDP_ProcessPollingDirectories(channel_t *c)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Cycle the engine. Called once per wakeup.
**  
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void CF_CFDP_CycleEngine(void)
{
    UT_DEFAULT_IMPL(CF_CFDP_CycleEngine);
}

/************************************************************************/
/** \brief Reset a transaction and all its internals to an unused state.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_ResetTransaction(transaction_t *t, int keep_history)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ResetTransaction), &t, 
      sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_ResetTransaction), &keep_history, 
      sizeof(keep_history));
    
    UT_DEFAULT_IMPL(CF_CFDP_ResetTransaction);
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
int CF_CFDP_CopyDataToLv(lv_t *dest_lv, const uint8 *data, uint32 len)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
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
int CF_CFDP_CopyDataFromLv(uint8 buf[CF_FILENAME_MAX_LEN], const lv_t *src_lv)
{
    UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", __FILE__, __LINE__);
    exit(-86);
}

/************************************************************************/
/** \brief Cancels a transaction.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_CFDP_CancelTransaction(transaction_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_CFDP_CancelTransaction), &t, sizeof(t));
    
    UT_DEFAULT_IMPL(CF_CFDP_CancelTransaction);
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
// static int CF_CFDP_CloseFiles(clist_node n, void *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Disables the cfdp engine and resets all state in it.
**  
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void CF_CFDP_DisableEngine(void)
{
    UT_DEFAULT_IMPL(CF_CFDP_DisableEngine);
}

