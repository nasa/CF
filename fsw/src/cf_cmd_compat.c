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
 *
 *  The CF Application command handling source file
 *
 *  All ground commands are processed in this file. All supporting functions
 *  necessary to process the commands are also here.
 */

#include "cf_app.h"
#include "cf_eventids.h"
#include "cf_cmd.h"
#include "cf_cmd_compat.h"

#define CF_CMD_COMPAT_COMPOUND_KEY (254)
#define CF_CMD_COMPAT_ALL_CHANNELS (255)
#define CF_CMD_COMPAT_ALL_POLLDIRS (CF_ALL_CHANNELS)

/*----------------------------------------------------------------
 *
 * Local helper function
 * Convert the old-style uint8 chan number to the CF_ChannelSelect_t value that correlates
 *
 *-----------------------------------------------------------------*/
static CF_ChannelSelect_t CF_Translate_CompatChannel(uint8 input)
{
    if (input == CF_CMD_COMPAT_COMPOUND_KEY || input == CF_CMD_COMPAT_ALL_CHANNELS)
    {
        return CF_ALL_CHANNELS;
    }

    if (input <= CF_NUM_CHANNELS)
    {
        return CF_ChannelSelect_FromInt(input + 1);
    }

    /* invalid value, keep it as is for event reporting */
    return CF_ChannelSelect_FromInt(input);
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Convert the old-style uint8 poll dir idx to the CF_PollIdxSelect_t value that correlates
 *
 *-----------------------------------------------------------------*/
static CF_PollIdxSelect_t CF_Translate_CompatPollDir(uint8 input)
{
    if (input == CF_CMD_COMPAT_ALL_POLLDIRS)
    {
        return 0;
    }

    if (input <= CF_MAX_POLLING_DIR_PER_CHAN)
    {
        return (input + 1);
    }

    /* invalid value, keep it as is for event reporting */
    return (input);
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Convert the old-style uint8 key to the CF_GetSet_ValueID_Enum_t value that correlates
 *
 *-----------------------------------------------------------------*/
static CF_GetSet_ValueID_Enum_t CF_Translate_CompatValueID(uint8 input)
{
    CF_GetSet_ValueID_Enum_t key;

    /* 0 is now reserved.  All valid values shifted up by 1. */
    key = input;
    if (key < CF_GetSet_ValueID_MAX)
    {
        ++key;
    }
    return key;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Convert the old-style Transaction specifier payload
 * Most args are compatible, but the "compound key" is now a separate flag.
 *
 *-----------------------------------------------------------------*/
static void CF_Translate_Transaction_CompatPayload(CF_Transaction_Payload_t             *output,
                                                   const CF_Transaction_CompatPayload_t *input)
{
    output->ts  = input->ts;
    output->eid = input->eid;
    if (input->chan == CF_CMD_COMPAT_COMPOUND_KEY)
    {
        output->use_ts_eid = 1;
        output->chan_num   = CF_ALL_CHANNELS;
    }
    else
    {
        output->chan_num = CF_Translate_CompatChannel(input->chan);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ResetCountersCompatCmd(const CF_ResetCountersCompatCmd_t *msg)
{
    CF_ResetCountersCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ResetType = msg->Payload.byte[0]; /* reset type selector is compatible */

    return CF_ResetCountersCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_TxFileCompatCmd(const CF_TxFileCompatCmd_t *msg)
{
    CF_TxFileCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));

    newcmd.Payload.cfdp_class = msg->Payload.cfdp_class;
    newcmd.Payload.keep       = msg->Payload.keep;
    newcmd.Payload.chan_num   = CF_Translate_CompatChannel(msg->Payload.chan_num);
    newcmd.Payload.priority   = msg->Payload.priority;
    newcmd.Payload.dest_id    = msg->Payload.dest_id;
    strncpy(newcmd.Payload.src_filename, msg->Payload.src_filename, sizeof(newcmd.Payload.src_filename));
    strncpy(newcmd.Payload.dst_filename, msg->Payload.dst_filename, sizeof(newcmd.Payload.dst_filename));

    return CF_TxFileCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PlaybackDirCompatCmd(const CF_PlaybackDirCompatCmd_t *msg)
{
    CF_PlaybackDirCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));

    newcmd.Payload.cfdp_class = msg->Payload.cfdp_class;
    newcmd.Payload.keep       = msg->Payload.keep;
    newcmd.Payload.chan_num   = CF_Translate_CompatChannel(msg->Payload.chan_num);
    newcmd.Payload.priority   = msg->Payload.priority;
    newcmd.Payload.dest_id    = msg->Payload.dest_id;
    strncpy(newcmd.Payload.src_filename, msg->Payload.src_filename, sizeof(newcmd.Payload.src_filename));
    strncpy(newcmd.Payload.dst_filename, msg->Payload.dst_filename, sizeof(newcmd.Payload.dst_filename));

    return CF_PlaybackDirCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_FreezeCompatCmd(const CF_FreezeCompatCmd_t *msg)
{
    CF_FreezeCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ChannelSelect = CF_Translate_CompatChannel(msg->Payload.byte[0]);

    return CF_FreezeCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ThawCompatCmd(const CF_ThawCompatCmd_t *msg)
{
    CF_ThawCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ChannelSelect = CF_Translate_CompatChannel(msg->Payload.byte[0]);

    return CF_ThawCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SuspendCompatCmd(const CF_SuspendCompatCmd_t *msg)
{
    CF_SuspendCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    CF_Translate_Transaction_CompatPayload(&newcmd.Payload, &msg->Payload);

    return CF_SuspendCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ResumeCompatCmd(const CF_ResumeCompatCmd_t *msg)
{
    CF_ResumeCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    CF_Translate_Transaction_CompatPayload(&newcmd.Payload, &msg->Payload);

    return CF_ResumeCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_CancelCompatCmd(const CF_CancelCompatCmd_t *msg)
{
    CF_CancelCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    CF_Translate_Transaction_CompatPayload(&newcmd.Payload, &msg->Payload);

    return CF_CancelCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_AbandonCompatCmd(const CF_AbandonCompatCmd_t *msg)
{
    CF_AbandonCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    CF_Translate_Transaction_CompatPayload(&newcmd.Payload, &msg->Payload);

    return CF_AbandonCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableDequeueCompatCmd(const CF_EnableDequeueCompatCmd_t *msg)
{
    CF_EnableDequeueCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ChannelSelect = CF_Translate_CompatChannel(msg->Payload.byte[0]);

    return CF_EnableDequeueCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableDequeueCompatCmd(const CF_DisableDequeueCompatCmd_t *msg)
{
    CF_DisableDequeueCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ChannelSelect = CF_Translate_CompatChannel(msg->Payload.byte[0]);

    return CF_DisableDequeueCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_EnableDirPollingCompatCmd(const CF_EnableDirPollingCompatCmd_t *msg)
{
    CF_EnableDirPollingCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ChannelSelect = CF_Translate_CompatChannel(msg->Payload.byte[0]);
    newcmd.Payload.PollDirIndx   = CF_Translate_CompatPollDir(msg->Payload.byte[1]);

    return CF_EnableDirPollingCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_DisableDirPollingCompatCmd(const CF_DisableDirPollingCompatCmd_t *msg)
{
    CF_DisableDirPollingCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ChannelSelect = CF_Translate_CompatChannel(msg->Payload.byte[0]);
    newcmd.Payload.PollDirIndx   = CF_Translate_CompatPollDir(msg->Payload.byte[1]);

    return CF_DisableDirPollingCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_PurgeQueueCompatCmd(const CF_PurgeQueueCompatCmd_t *msg)
{
    CF_PurgeQueueCmd_t newcmd;
    uint8              queueidx;

    memset(&newcmd, 0, sizeof(newcmd));
    newcmd.Payload.ChannelSelect = CF_Translate_CompatChannel(msg->Payload.byte[0]);

    /* byte 1 specified the queue, only pending(0) and history(1) as options. 2=all */
    queueidx = msg->Payload.byte[1];
    if (queueidx < CF_QueueSelect_MAX)
    {
        /* history(1) and all(2) are shifted an extra value to match */
        if (queueidx != 0)
        {
            ++queueidx;
        }
        queueidx = (queueidx + 1) % CF_QueueSelect_MAX;
    }
    newcmd.Payload.QueueSelect = queueidx;

    return CF_PurgeQueueCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_WriteQueueCompatCmd(const CF_WriteQueueCompatCmd_t *msg)
{
    CF_WriteQueueCmd_t newcmd;
    uint8              queueidx;

    memset(&newcmd, 0, sizeof(newcmd));

    newcmd.Payload.dir_type = msg->Payload.type; /* direction is compatible */
    newcmd.Payload.chan_num = CF_Translate_CompatChannel(msg->Payload.chan);
    queueidx                = msg->Payload.queue; /* Queue type: 0=pending, 1=active, 2=history, 3=all */
    if (queueidx <= CF_QueueSelect_MAX)
    {
        /* we need to "roll" the value - 0 is now all, instead of 3. */
        queueidx = (queueidx + 1) % CF_QueueSelect_MAX;
    }
    newcmd.Payload.queue = queueidx;

    return CF_WriteQueueCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_SetParamCompatCmd(const CF_SetParamCompatCmd_t *msg)
{
    CF_SetParamCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));

    newcmd.Payload.value    = msg->Payload.value;
    newcmd.Payload.key      = CF_Translate_CompatValueID(msg->Payload.key);
    newcmd.Payload.chan_num = CF_Translate_CompatChannel(msg->Payload.chan_num);

    return CF_SetParamCmd(&newcmd);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_cmd.h for argument/return detail
 * This is the entry point function invoked from the SB command handler
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_GetParamCompatCmd(const CF_GetParamCompatCmd_t *msg)
{
    CF_GetParamCmd_t newcmd;

    memset(&newcmd, 0, sizeof(newcmd));

    newcmd.Payload.key      = CF_Translate_CompatValueID(msg->Payload.key);
    newcmd.Payload.chan_num = CF_Translate_CompatChannel(msg->Payload.chan_num);

    return CF_GetParamCmd(&newcmd);
}
