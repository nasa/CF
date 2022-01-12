/************************************************************************
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
 ************************************************************************/

/**
 * @file
 *
 * CF command processing function declarations
 */

#ifndef CF_CMD_H
#define CF_CMD_H

#include "cfe.h"
#include "cf_app.h"
#include "cf_utils.h"

/**
 * @brief A callback function for use with CF_DoChanAction()
 *
 * @param chan_num The CF channel number, for statistics purposes
 * @param context  Opaque object passed through from initial call
 */
typedef int (*CF_ChanActionFn_t)(uint8 chan_num, void *context);

/**
 * @brief An object to use with channel-scope actions requiring only a boolean argument
 */
typedef struct CF_ChanAction_BoolArg
{
    bool barg;
} CF_ChanAction_BoolArg_t;

/**
 * @brief A callback to use with transaction actions
 *
 * For now this is the same as CF_TraverseAllTransactions_fn_t
 */
typedef CF_TraverseAllTransactions_fn_t CF_TsnChanAction_fn_t;

/**
 * @brief An object to use with channel-scope actions for suspend/resume
 *
 * This combines a boolean action arg with an output that indicates if it
 * was a change or not.
 */
typedef struct CF_ChanAction_SuspResArg
{
    int  same; /**< out param -- indicates at least one action was set to its current value */
    bool action;
} CF_ChanAction_SuspResArg_t;

/**
 * @brief An object to use with channel-scope actions that require the message value
 *
 * This combines a boolean action arg with the command message value
 */
typedef struct CF_ChanAction_BoolMsgArg
{
    const CF_UnionArgsCmd_t *msg;
    bool                     barg;
} CF_ChanAction_BoolMsgArg_t;

/************************************************************************/
/** @brief Increment the command accepted counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 */
static inline void CF_CmdAcc(void)
{
    ++CF_AppData.hk.counters.cmd;
}

/************************************************************************/
/** @brief Increment the command rejected counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 */
static inline void CF_CmdRej(void)
{
    ++CF_AppData.hk.counters.err;
}

/************************************************************************/
/** @brief Conditionally increment the command accept or reject counters.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 */
static inline void CF_CmdCond(int cond)
{
    static void (*const fns[])(void) = {CF_CmdAcc, CF_CmdRej};
    fns[!!cond]();
}

/************************************************************************/
/** @brief The no-operation command.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       This function simply prints an event message.
 *       Increments the command accept counter.
 *       The msg parameter is ignored in this one.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param msg   Pointer to command message
 */
void CF_CmdNoop(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief The reset counters command.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       Resets the given counter, or all.
 *       Increments the command accept or reject counter. If the command
 *       counters are reset, then there is no increment.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdReset(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Ground command to start a file transfer.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       Increments the command accept or reject counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 *
 */
void CF_CmdTxFile(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Ground command to start directory playback.
 *
 * @par Description
 *       This function has a signature the same of all cmd_ functions.
 *       Increments the command accept or reject counter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdPlaybackDir(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Common logic for all channel-based commands.
 *
 * @par Description
 *       All the commands that act on channels or have the special
 *       "all channels" parameter come through this function. This puts
 *       all common logic in one place. It does not handle the command
 *       accept or reject counters.
 *
 * @par Assumptions, External Events, and Notes:
 *       cmd must not be NULL. errstr must not be NULL. fn must be a valid function. context may be NULL.
 *
 * @param cmd       Pointer to command being processed
 * @param errstr    String to be included in the EVS event if command should fail
 * @param fn        Callback action function to invoke for each affected channel
 * @param context   Opaque pointer to pass through to callback (not used in this function)
 *
 * @returns The return value from the given action function.
 *
 */
int CF_DoChanAction(CF_UnionArgsCmd_t *cmd, const char *errstr, CF_ChanActionFn_t fn, void *context);

/************************************************************************/
/** @brief Channel action to set the frozen bit for a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       context must not be NULL.
 *
 * @param chan_num  channel number
 * @param context   Pointer to object passed through from initial call
 *
 * @returns Always succeeds, so returns 0.
 */
int CF_DoFreezeThaw(uint8 chan_num, const CF_ChanAction_BoolArg_t *context);

/************************************************************************/
/** @brief Freeze a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdFreeze(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Thaw a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdThaw(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Search for a transaction across all channels.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param ts  Transaction sequence number to find
 * @param eid Entity ID of the transaction
 *
 * @returns Pointer to the transaction, if found
 * @retval  NULL if transaction not found
 *
 */
CF_Transaction_t *CF_FindTransactionBySequenceNumberAllChannels(CF_TransactionSeq_t ts, CF_EntityId_t eid);

/************************************************************************/
/** @brief Common logic for all transaction sequence number and channel commands.
 *
 * @par Description
 *       All the commands that on a transaction on a particular channel come
 *       through this function. This puts all common logic in one place. It
 *       does handle the command accept or reject counters.
 *
 * @par Assumptions, External Events, and Notes:
 *       cmd must not be NULL. fn must be a valid function. context may be NULL.
 *
 * @param cmd       Pointer to the command message
 * @param cmdstr    String to include in any generated EVS events
 * @param fn        Callback function to invoke for each matched transaction
 * @param context   Opaque object to pass through to the callback
 *
 * @returns returns the number of transactions acted upon
 *
 */
int CF_TsnChanAction(CF_TransactionCmd_t *cmd, const char *cmdstr, CF_TsnChanAction_fn_t fn, void *context);

/************************************************************************/
/** @brief Set the suspended bit in a transaction.
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL. context must not be NULL.
 *
 * @param t         Pointer to the transaction object
 * @param context   Pointer to CF_ChanAction_SuspResArg_t structure from initial call
 */
void CF_DoSuspRes_Txn(CF_Transaction_t *t, CF_ChanAction_SuspResArg_t *context);

/************************************************************************/
/** @brief Handle transaction suspend and resume commands.
 *
 * @par Description
 *       This is called for both suspend and resume ground commands.
 *       It uses the CF_TsnChanAction() function to perform the command.
 *
 * @par Assumptions, External Events, and Notes:
 *       cmd must not be NULL.
 *
 * @param cmd       Pointer to the command message
 * @param action    Action to take (suspend or resume)
 */
void CF_DoSuspRes(CF_TransactionCmd_t *cmd, uint8 action);

/************************************************************************/
/** @brief Handle transaction suspend command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdSuspend(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Handle transaction resume command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdResume(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief tsn chan action to cancel a transaction.
 *
 * This helper function is used with CF_TsnChanAction() to cancel matched transactions
 *
 * @par Assumptions, External Events, and Notes:
 *       t must not be NULL.
 *
 * @param t        Pointer to transaction object
 * @param ignored  Not used by this function
 */
void CF_CmdCancel_Txn(CF_Transaction_t *t, void *ignored);

/************************************************************************/
/** @brief Handle a cancel ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdCancel(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief tsn chan action to abandon a transaction.
 *
 * This helper function is used with CF_TsnChanAction() to abandon matched transactions
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param t        Pointer to transaction object
 * @param ignored  Not used by this function
 */
void CF_CmdAbandon_Txn(CF_Transaction_t *t, void *ignored);

/************************************************************************/
/** @brief Handle an abandon ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdAbandon(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Sets the dequeue enable/disable flag for a channel.
 *
 * @par Assumptions, External Events, and Notes:
 *       context must not be NULL.
 *
 * @param chan_num  channel number
 * @param context   Pointer to object passed through from initial call
 *
 * @returns Always succeeds, so returns 0.
 *
 */
int CF_DoEnableDisableDequeue(uint8 chan_num, const CF_ChanAction_BoolArg_t *context);

/************************************************************************/
/** @brief Handle an enable dequeue ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdEnableDequeue(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Handle a disable dequeue ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdDisableDequeue(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Sets the enable/disable flag for the specified polling directory.
 *
 * @par Assumptions, External Events, and Notes:
 *       context must not be NULL.
 *
 * @param chan_num  channel number
 * @param context   Pointer to object passed through from initial call
 *
 * @returns success/fail status code
 * @retval  0 if successful
 * @retval  -1 if failed
 */
int CF_DoEnableDisablePolldir(uint8 chan_num, const CF_ChanAction_BoolMsgArg_t *context);

/************************************************************************/
/** @brief Enable a polling dir ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdEnablePolldir(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Disable a polling dir ground command.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdDisablePolldir(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Purge the history queue for the given channel.
 *
 * This helper function is used in conjunction with CF_CList_Traverse()
 *
 * @par Assumptions, External Events, and Notes:
 *       n must not be NULL. c must not be NULL.
 *
 * @param n  Current list node being traversed
 * @param c  Channel pointer passed through as opaque object
 *
 * @returns Always #CF_CLIST_CONT to process all entries
 */
int CF_PurgeHistory(CF_CListNode_t *n, CF_Channel_t *c);

/************************************************************************/
/** @brief Purge the pending transaction queue.
 *
 * This helper function is used in conjunction with CF_CList_Traverse()
 *
 * @par Assumptions, External Events, and Notes:
 *       n must not be NULL.
 *
 * @param n  Current list node being traversed
 * @param ignored  Not used by this implementation
 *
 * @returns Always #CF_CLIST_CONT to process all entries
 */
int CF_PurgeTransaction(CF_CListNode_t *n, void *ignored);

/************************************************************************/
/** @brief Channel action command to perform purge queue operations.
 *
 * @par Description
 *       Determines from the command parameters which queues to traverse
 *       and purge state.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param chan_num  CF channel number
 * @param cmd       Pointer to purge queue command
 *
 * @returns integer status code indicating success or failure
 * @retval  0 if successful
 * @retval  -1 if error occurred
 */
int CF_DoPurgeQueue(uint8 chan_num, CF_UnionArgsCmd_t *cmd);

/************************************************************************/
/** @brief Ground command to purge either the history or pending queues.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdPurgeQueue(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Ground command to write a file with queue information.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdWriteQueue(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Ground command to send configuration parameters.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdSendCfgParams(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Checks if the value is less than or equal to the max pdu size.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param val       Size of chunk to test
 * @param chan_num  Ignored by this implementation
 *
 * @returns status code indicating if check passed
 * @retval 0 if successful (val is less than or equal to max PDU)
 * @retval 1 if failed (val is greater than max PDU)
 *
 */
int CF_CmdValidateChunkSize(uint32 val, uint8 chan_num);

/************************************************************************/
/** @brief Checks if the value is within allowable range as outgoing packets per wakeup
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param val       Number to test
 * @param chan_num  CF channel number
 *
 * @returns status code indicating if check passed
 * @retval 0 if successful (val is allowable as max packets per wakeup)
 * @retval 1 if failed (val is not allowed)
 *
 */
int CF_CmdValidateMaxOutgoing(uint32 val, uint8 chan_num);

/************************************************************************/
/** @brief Perform a configuration get/set operation.
 *
 * For a set, this sets the value within the CF configuration
 * For a get, this generates an EVS event with the requested information
 *
 * @par Description
 *       Combine get and set in one function with common logic.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 * @param is_set    Whether to get (0) or set (1)
 * @param param_id  Parameter ID
 * @param value     Value to get/set
 * @param chan_num  Channel number to operate on
 *
 */
void CF_CmdGetSetParam(uint8 is_set, CF_GetSet_ValueID_t param_id, uint32 value, uint8 chan_num);

/************************************************************************/
/** @brief Ground command to set a configuration parameter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdSetParam(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Ground command to set a configuration parameter.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdGetParam(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Ground command enable engine.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdEnableEngine(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Ground command disable engine.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_CmdDisableEngine(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief Process any ground command contained in the given message.
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param msg   Pointer to command message
 */
void CF_ProcessGroundCommand(CFE_SB_Buffer_t *msg);

#endif
