/************************************************************************
** File: cf_cmd.h
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
*************************************************************************/

#ifndef CF_CMD_H
#define CF_CMD_H

#include "cfe.h"
#include "cf_app.h"
#include "cf_utils.h"

typedef int (*CF_ChanActionFn_t)(uint8 chan_num, void *context);

typedef struct CF_ChanAction_BoolArg
{
    bool barg;
} CF_ChanAction_BoolArg_t;

typedef CF_TraverseAllTransactions_fn_t CF_TsnChanAction_fn_t;

typedef struct CF_ChanAction_SuspResArg
{
    int  same; /* out param -- indicates at least one action was set to its current value */
    bool action;
} CF_ChanAction_SuspResArg_t;

typedef struct CF_ChanAction_BoolMsgArg
{
    const CF_UnionArgsCmd_t *msg;
    bool                     barg;
} CF_ChanAction_BoolMsgArg_t;

/************************************************************************/
/** \brief Increment the command accepted counter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static inline void CF_CmdAcc(void)
{
    ++CF_AppData.hk.counters.cmd;
}

/************************************************************************/
/** \brief Increment the command rejected counter.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static inline void CF_CmdRej(void)
{
    ++CF_AppData.hk.counters.err;
}

/************************************************************************/
/** \brief Conditionally increment the command accept or reject counters.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static inline void CF_CmdCond(int cond)
{
    static void (*const fns[])(void) = {CF_CmdAcc, CF_CmdRej};
    fns[!!cond]();
}

void              CF_CmdNoop(CFE_SB_Buffer_t *msg);
void              CF_CmdReset(CFE_SB_Buffer_t *msg);
void              CF_CmdTxFile(CFE_SB_Buffer_t *msg);
void              CF_CmdPlaybackDir(CFE_SB_Buffer_t *msg);
int               CF_DoChanAction(CF_UnionArgsCmd_t *cmd, const char *errstr, CF_ChanActionFn_t fn, void *context);
int               CF_DoFreezeThaw(uint8 chan_num, const CF_ChanAction_BoolArg_t *context);
void              CF_CmdFreeze(CFE_SB_Buffer_t *msg);
void              CF_CmdThaw(CFE_SB_Buffer_t *msg);
CF_Transaction_t *CF_FindTransactionBySequenceNumberAllChannels(CF_TransactionSeq_t ts, CF_EntityId_t eid);
int  CF_TsnChanAction(CF_TransactionCmd_t *cmd, const char *cmdstr, CF_TsnChanAction_fn_t fn, void *context);
void CF_DoSuspRes_Txn(CF_Transaction_t *t, CF_ChanAction_SuspResArg_t *context);
void CF_DoSuspRes(CF_TransactionCmd_t *cmd, uint8 action);
void CF_CmdSuspend(CFE_SB_Buffer_t *msg);
void CF_CmdResume(CFE_SB_Buffer_t *msg);
void CF_CmdCancel_Txn(CF_Transaction_t *t, void *ignored);
void CF_CmdCancel(CFE_SB_Buffer_t *msg);
void CF_CmdAbandon_Txn(CF_Transaction_t *t, void *ignored);
void CF_CmdAbandon(CFE_SB_Buffer_t *msg);
int  CF_DoEnableDisableDequeue(uint8 chan_num, const CF_ChanAction_BoolArg_t *context);
void CF_CmdEnableDequeue(CFE_SB_Buffer_t *msg);
void CF_CmdDisableDequeue(CFE_SB_Buffer_t *msg);
int  CF_DoEnableDisablePolldir(uint8 chan_num, const CF_ChanAction_BoolMsgArg_t *context);
void CF_CmdEnablePolldir(CFE_SB_Buffer_t *msg);
void CF_CmdDisablePolldir(CFE_SB_Buffer_t *msg);
int  CF_PurgeHistory(CF_CListNode_t *n, CF_Channel_t *c);
int  CF_PurgeTransaction(CF_CListNode_t *n, void *ignored);
int  CF_DoPurgeQueue(uint8 chan_num, CF_UnionArgsCmd_t *cmd);
void CF_CmdPurgeQueue(CFE_SB_Buffer_t *msg);
void CF_CmdWriteQueue(CFE_SB_Buffer_t *msg);
void CF_CmdSendCfgParams(CFE_SB_Buffer_t *msg);
int  CF_CmdValidateChunkSize(uint32 val, uint8 chan_num /* ignored */);
int  CF_CmdValidateMaxOutgoing(uint32 val, uint8 chan_num);
void CF_CmdGetSetParam(uint8 is_set, uint8 param_id, uint32 value, uint8 chan_num);
void CF_CmdSetParam(CFE_SB_Buffer_t *msg);
void CF_CmdGetParam(CFE_SB_Buffer_t *msg);
void CF_CmdEnableEngine(CFE_SB_Buffer_t *msg);
void CF_CmdDisableEngine(CFE_SB_Buffer_t *msg);
void CF_ProcessGroundCommand(CFE_SB_Buffer_t *msg);

#endif
