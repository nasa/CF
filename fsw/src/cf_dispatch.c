/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *  The CF Application main application source file
 *
 *  This file contains the functions that initialize the application and link
 *  all logic and functionality to the CFS.
 */

#include "cf_dispatch.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_cmd.h"

#include "cfe.h"
#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ProcessGroundCommand(const CFE_SB_Buffer_t *BufPtr)
{
    typedef void (*const handler_fn_t)(const void *);

    static handler_fn_t fns[] = {
        [CF_NOOP_CC]                = (handler_fn_t)CF_NoopCmd,
        [CF_RESET_CC]               = (handler_fn_t)CF_ResetCountersCmd,
        [CF_TX_FILE_CC]             = (handler_fn_t)CF_TxFileCmd,
        [CF_PLAYBACK_DIR_CC]        = (handler_fn_t)CF_PlaybackDirCmd,
        [CF_FREEZE_CC]              = (handler_fn_t)CF_FreezeCmd,
        [CF_THAW_CC]                = (handler_fn_t)CF_ThawCmd,
        [CF_SUSPEND_CC]             = (handler_fn_t)CF_SuspendCmd,
        [CF_RESUME_CC]              = (handler_fn_t)CF_ResumeCmd,
        [CF_CANCEL_CC]              = (handler_fn_t)CF_CancelCmd,
        [CF_ABANDON_CC]             = (handler_fn_t)CF_AbandonCmd,
        [CF_SET_PARAM_CC]           = (handler_fn_t)CF_SetParamCmd,
        [CF_GET_PARAM_CC]           = (handler_fn_t)CF_GetParamCmd,
        [CF_WRITE_QUEUE_CC]         = (handler_fn_t)CF_WriteQueueCmd,
        [CF_ENABLE_DEQUEUE_CC]      = (handler_fn_t)CF_EnableDequeueCmd,
        [CF_DISABLE_DEQUEUE_CC]     = (handler_fn_t)CF_DisableDequeueCmd,
        [CF_ENABLE_DIR_POLLING_CC]  = (handler_fn_t)CF_EnableDirPollingCmd,
        [CF_DISABLE_DIR_POLLING_CC] = (handler_fn_t)CF_DisableDirPollingCmd,
        [CF_PURGE_QUEUE_CC]         = (handler_fn_t)CF_PurgeQueueCmd,
        [CF_ENABLE_ENGINE_CC]       = (handler_fn_t)CF_EnableEngineCmd,
        [CF_DISABLE_ENGINE_CC]      = (handler_fn_t)CF_DisableEngineCmd,
    };

    static const uint16 expected_lengths[] = {
        [CF_NOOP_CC]                = sizeof(CF_NoopCmd_t),
        [CF_RESET_CC]               = sizeof(CF_ResetCountersCmd_t),
        [CF_TX_FILE_CC]             = sizeof(CF_TxFileCmd_t),
        [CF_PLAYBACK_DIR_CC]        = sizeof(CF_PlaybackDirCmd_t),
        [CF_FREEZE_CC]              = sizeof(CF_FreezeCmd_t),
        [CF_THAW_CC]                = sizeof(CF_ThawCmd_t),
        [CF_SUSPEND_CC]             = sizeof(CF_SuspendCmd_t),
        [CF_RESUME_CC]              = sizeof(CF_ResumeCmd_t),
        [CF_CANCEL_CC]              = sizeof(CF_CancelCmd_t),
        [CF_ABANDON_CC]             = sizeof(CF_AbandonCmd_t),
        [CF_SET_PARAM_CC]           = sizeof(CF_SetParamCmd_t),
        [CF_GET_PARAM_CC]           = sizeof(CF_GetParamCmd_t),
        [CF_WRITE_QUEUE_CC]         = sizeof(CF_WriteQueueCmd_t),
        [CF_ENABLE_DEQUEUE_CC]      = sizeof(CF_EnableDequeueCmd_t),
        [CF_DISABLE_DEQUEUE_CC]     = sizeof(CF_DisableDequeueCmd_t),
        [CF_ENABLE_DIR_POLLING_CC]  = sizeof(CF_EnableDirPollingCmd_t),
        [CF_DISABLE_DIR_POLLING_CC] = sizeof(CF_DisableDirPollingCmd_t),
        [CF_PURGE_QUEUE_CC]         = sizeof(CF_UnionArgs_Payload_t),
        [CF_ENABLE_ENGINE_CC]       = sizeof(CF_EnableEngineCmd_t),
        [CF_DISABLE_ENGINE_CC]      = sizeof(CF_DisableEngineCmd_t),
    };

    CFE_MSG_FcnCode_t cmd = 0;
    size_t            len = 0;

    CFE_MSG_GetFcnCode(&BufPtr->Msg, &cmd);

    if (cmd < (sizeof(expected_lengths) / sizeof(expected_lengths[0])))
    {
        CFE_MSG_GetSize(&BufPtr->Msg, &len);

        /* first, verify command length */
        if (len == expected_lengths[cmd])
        {
            /* if valid, process command */
            if (fns[cmd])
            {
                fns[cmd](BufPtr);
            }
        }
        else
        {
            CFE_EVS_SendEvent(CF_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: invalid ground command length for command 0x%02x, expected %d got %zd", cmd,
                              expected_lengths[cmd], len);
            ++CF_AppData.hk.Payload.counters.err;
        }
    }
    else
    {
        CFE_EVS_SendEvent(CF_CC_ERR_EID, CFE_EVS_EventType_ERROR, "CF: invalid ground command packet cmd_code=0x%02x",
                          cmd);
        ++CF_AppData.hk.Payload.counters.err;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_AppPipe(const CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetMsgId(&BufPtr->Msg, &MessageID);

    switch (CFE_SB_MsgIdToValue(MessageID))
    {
        case CF_CMD_MID:
            CF_ProcessGroundCommand(BufPtr);
            break;

        case CF_WAKE_UP_MID:
            CF_WakeupCmd((const CF_WakeupCmd_t *)BufPtr);
            break;

        case CF_SEND_HK_MID:
            CF_SendHkCmd((const CF_SendHkCmd_t *)BufPtr);
            break;

        default:
            ++CF_AppData.hk.Payload.counters.err;
            CFE_EVS_SendEvent(CF_MID_ERR_EID, CFE_EVS_EventType_ERROR, "CF: invalid command packet id=0x%lx",
                              (unsigned long)CFE_SB_MsgIdToValue(MessageID));
            break;
    }
}
