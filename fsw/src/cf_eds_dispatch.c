#include "cf_app.h"
#include "cf_events.h"
#include "cf_dispatch.h"
#include "cf_cmd.h"

#include "cf_eds_dictionary.h"
#include "cf_eds_dispatcher.h"

#include "cfe_msg.h"

static const EdsDispatchTable_CF_Application_CFE_SB_Telecommand_t CF_TC_DISPATCH_TABLE = {
    .CMD =
        {

            .AbandonCmd_indication           = CF_AbandonCmd,
            .CancelCmd_indication            = CF_CancelCmd,
            .DisableDequeueCmd_indication    = CF_DisableDequeueCmd,
            .DisableDirPollingCmd_indication = CF_DisableDirPollingCmd,
            .DisableEngineCmd_indication     = CF_DisableEngineCmd,
            .EnableDequeueCmd_indication     = CF_EnableDequeueCmd,
            .EnableDirPollingCmd_indication  = CF_EnableDirPollingCmd,
            .EnableEngineCmd_indication      = CF_EnableEngineCmd,
            .FreezeCmd_indication            = CF_FreezeCmd,
            .GetParamCmd_indication          = CF_GetParamCmd,
            .NoopCmd_indication              = CF_NoopCmd,
            .PlaybackDirCmd_indication       = CF_PlaybackDirCmd,
            .PurgeQueueCmd_indication        = CF_PurgeQueueCmd,
            .ResetCmd_indication             = CF_ResetCmd,
            .ResumeCmd_indication            = CF_ResumeCmd,
            .SetParamCmd_indication          = CF_SetParamCmd,
            .SuspendCmd_indication           = CF_SuspendCmd,
            .ThawCmd_indication              = CF_ThawCmd,
            .TxFileCmd_indication            = CF_TxFileCmd,
            .WriteQueueCmd_indication        = CF_WriteQueueCmd,
        },
    .SEND_HK = {.indication = CF_SendHkCmd},
    .WAKE_UP = {.indication = CF_WakeupCmd}};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CF_TaskPipe() -- Process command pipe message           */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CF_AppPipe(const CFE_SB_Buffer_t *msg)
{
    CFE_Status_t      status;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    MsgSize;
    CFE_MSG_FcnCode_t MsgFc;

    status = EdsDispatch_CF_Application_Telecommand(msg, &CF_TC_DISPATCH_TABLE);

    if (status != CFE_SUCCESS)
    {
        CFE_MSG_GetMsgId(&msg->Msg, &MsgId);
        ++CF_AppData.hk.Payload.counters.err;

        if (status == CFE_STATUS_UNKNOWN_MSG_ID)
        {
            CFE_EVS_SendEvent(CF_MID_ERR_EID, CFE_EVS_EventType_ERROR, "L%d TO: Invalid Msg ID Rcvd 0x%x status=0x%08x",
                              __LINE__, (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)status);
        }
        else if (status == CFE_STATUS_WRONG_MSG_LENGTH)
        {
            CFE_MSG_GetSize(&msg->Msg, &MsgSize);
            CFE_MSG_GetFcnCode(&msg->Msg, &MsgFc);
            CFE_EVS_SendEvent(CF_CMD_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid length for command: ID = 0x%X, CC = %d, length = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), (int)MsgFc, (unsigned int)MsgSize);
        }
        else
        {
            CFE_MSG_GetFcnCode(&msg->Msg, &MsgFc);
            CFE_EVS_SendEvent(CF_CC_ERR_EID, CFE_EVS_EventType_ERROR,
                              "L%d TO: Invalid Function Code Rcvd In Ground Command 0x%x", __LINE__,
                              (unsigned int)MsgFc);
        }
    }
}
