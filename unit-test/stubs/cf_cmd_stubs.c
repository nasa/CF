/************************************************************************
** File: cf_cmd_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application command handling stubs file
**
**  All ground commands are processed in this file. All supporting functions
**  necessary to process the commands are also here.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cf_app.h"
#include "cf_verify.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_utils.h"

#include "cf_cfdp.h"

#include <string.h>

/* UT includes */
#include "uttest.h"
#include "utstubs.h"

#define ALL_CHANNELS 255
#define COMPOUND_KEY 254

typedef int (*chan_action_fn_t)(uint8 chan_num, void *context);

typedef struct
{
    uint8 barg;
} bool_arg_t;

typedef CF_TraverseAllTransactions_fn_t CF_TsnChanAction_fn_t;

typedef struct
{
    int   same; /* out param -- indicates at least one action was set to its current value */
    uint8 action;
} susp_res_arg_t;

typedef struct
{
    const cf_cmd_unionargs_t *msg;
    uint8                     barg;
} bool_msg_arg_t;

/************************************************************************/
/** \brief Increment the command accepted counter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
// static void CF_CmdAcc(void)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Increment the command rejected counter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
// static void CF_CmdRej(void)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Conditionally increment the command accept or reject counters.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
// static void CF_CmdCond(int cond)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief The no-operation command.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       This function simply prints an event message.
**       Increments the command accept counter.
**       The msg parameter is ignored in this one.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
// static void CF_CmdNoop(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief The reset counters command.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       Resets the given counter, or all.
**       Increments the command accept or reject counter. If the command
**       counters are reset, then there is no increment.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdReset(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command to start a file transfer.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       Increments the command accept or reject counter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdTxFile(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command to start directory playback.
**
**  \par Description
**       This function has a signature the same of all cmd_ functions.
**       Increments the command accept or reject counter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdPlaybackDir(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Common logic for all channel-based commands.
**
**  \par Description
**       All the commands that act on channels or have the special
**       "all channels" parameter come through this function. This puts
**       all common logic in one place. It does not handle the command
**       accept or reject counters.
**
**  \par Assumptions, External Events, and Notes:
**       cmd must not be NULL. errstr must not be NULL. fn must be a valid function. context may be NULL.
**
**  \returns
**  \retstmt The return value from the given function. \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_DoChanAction(cf_cmd_unionargs_t *cmd, const char *errstr, chan_action_fn_t fn, void *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Channel action to set the frozen bit for a channel.
**
**  \par Assumptions, External Events, and Notes:
**       context must not be NULL.
**
**  \returns
**  \retstmt Always succeeds, so returns 0. \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_DoFreezeThaw(uint8 chan_num, const bool_arg_t *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Freeze a channel.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdFreeze(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Thaw a channel.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdThaw(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Search for a transaction across all channels.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt The transaction, if found. Otherwise NULL. \endcode
**  \endreturns
**
*************************************************************************/
// static CF_Transaction_t *CF_CFDP_FindTransactionBySequenceNumberAllChannels(CF_TransactionSeq_t ts, CF_EntityId_t
// eid)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/* CF_TsnChanAction() returns the number of transactions acted upon */
/************************************************************************/
/** \brief Common logic for all transaction sequence number and channel commands.
**
**  \par Description
**       All the commands that on a transaction on a particular channel come
**       through this function. This puts all common logic in one place. It
**       does handle the command accept or reject counters.
**
**  \par Assumptions, External Events, and Notes:
**       cmd must not be NULL. fn must be a valid function. context may be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_TsnChanAction(cf_cmd_transaction_t *cmd, const char *cmdstr, CF_TsnChanAction_fn_t fn, void *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Set the suspended bit in a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL. context must not be NULL.
**
*************************************************************************/
// static void CF_DoSuspRes_(CF_Transaction_t *t, susp_res_arg_t *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Handle transaction suspend and resume commands.
**
**  \par Description
**       This is called for both suspend and resume ground commands.
**       It uses the CF_TsnChanAction() function to perform the command.
**
**  \par Assumptions, External Events, and Notes:
**       cmd must not be NULL.
**
*************************************************************************/
// static void CF_DoSuspRes(cf_cmd_transaction_t *cmd, uint8 action)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Handle transaction suspend command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdSuspend(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Handle transaction resume command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdResume(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief tsn chan action to cancel a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
// static void CF_CmdCancel_(CF_Transaction_t *t, void *ignored)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Handle a cancel ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdCancel(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief tsn chan action to abandon a transaction.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdAbandon_(CF_Transaction_t *t, void *ignored)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Handle an abandon ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdAbandon(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Sets the dequeue enable/disable flag for a channel.
**
**  \par Assumptions, External Events, and Notes:
**       context must not be NULL.
**
*************************************************************************/
// static void CF_DoEnableDisableDequeue(uint8 chan_num, const bool_arg_t *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Handle an enable dequeue ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdEnableDequeue(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Handle a disable dequeue ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdDisableDequeue(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Sets the enable/disable flag for the specified polling directory.
**
**  \par Assumptions, External Events, and Notes:
**       context must not be NULL.
**
*************************************************************************/
// static int CF_DoEnableDisablePolldir(uint8 chan_num, const bool_msg_arg_t *context)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Enable a polling dir ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdEnablePolldir(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Disable a polling dir ground command.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdDisablePolldir(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Purge the history queue for the given channel.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL. c must not be NULL.
**
*************************************************************************/
// static int CF_PurgeHistory(CF_CListNode_t * n, CF_Channel_t *c)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Purge the pending transaction queue.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL.
**
*************************************************************************/
// static int CF_PurgeTransaction(CF_CListNode_t * n, void *ignored)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Channel action command to perform purge queue operations.
**
**  \par Description
**       Determines from the command parameters which queues to traverse
**       and purge state.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt 0 on success; anything else on error. \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_DoPurgeQueue(uint8 chan_num, cf_cmd_unionargs_t *cmd)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command to purge either the history or pending queues.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdPurgeQueue(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command to write a file with queue information.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdWriteQueue(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command to send configuration parameters.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdSendCfgParams(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Checks if the value is less than or equal to the max pdu size.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt 0 if success, 1 if failed. \endcode
**  \endreturns
**
*************************************************************************/
// static int CF_CmdValidateChunkSize(uint32 val, uint8 chan_num /* ignored */)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Perform a configuration get/set operation.
**
**  \par Description
**       Combine get and set in one function with common logic.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
/* combine getset into a single function with a branch to avoid wasted memory footprint with duplicate
 * logic for finding the parameter */
// static void CF_CmdGetSetParam(uint8 is_set, uint8 param_id, uint32 value, uint8 chan_num)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command to set a configuration parameter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdSetParam(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command to set a configuration parameter.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdGetParam(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command enable engine.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdEnableEngine(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Ground command disable engine.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
// static void CF_CmdDisableEngine(CFE_SB_Buffer_t* msg)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n",
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Process any ground command contained in the given message.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
void CF_ProcessGroundCommand(CFE_SB_Buffer_t *msg)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_ProcessGroundCommand), &msg, sizeof(msg));

    UT_DEFAULT_IMPL(CF_ProcessGroundCommand);
}
