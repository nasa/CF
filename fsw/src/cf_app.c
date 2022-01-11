/************************************************************************
 * File: cf_app.c
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
 * Purpose:
 *  The CF Application main application source file
 *
 *  This file contains the functions that initialize the application and link
 *  all logic and functionality to the CFS.
 *
 ************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_version.h"
#include "cf_cmd.h"

#include <string.h>

CF_AppData_t CF_AppData;

/*----------------------------------------------------------------
 *
 * Function: CF_HkCmd
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_HkCmd(void)
{
    CFE_MSG_SetMsgTime(&CF_AppData.hk.tlm_header.Msg, CFE_TIME_GetTime());
    /* return value ignored */ CFE_SB_TransmitMsg(&CF_AppData.hk.tlm_header.Msg, true);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CheckTables
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CheckTables(void)
{
    CFE_Status_t status;

    /* check the table for an update only if engine is disabled */
    if (!CF_AppData.engine.enabled)
    {
        /*
         * NOTE: As of CFE 7.0 (Caelum), some the CFE TBL APIs return success codes
         * other than CFE_SUCCESS, so it is not sufficient to check for only this
         * result here.  For example they may return something like CFE_TBL_INFO_UPDATED.
         * But from the standpoint of this routine, they are all success, because the
         * function still did its expected job.
         *
         * For now, the safest way to check is to check for negative values,
         * as the alt-success codes are in the positive range by design, and
         * error codes are all in the negative range of CFE_Status_t.
         *
         * This should continue to work even if CFE TBL APIs change to
         * remove the problematic alt-success codes at some point.
         */
        status = CFE_TBL_ReleaseAddress(CF_AppData.config_handle);
        if (status < CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_TBL_CHECK_REL, CFE_EVS_EventType_ERROR,
                              "CF: error in CFE_TBL_ReleaseAddress (check), returned 0x%08lx", (unsigned long)status);
            CF_AppData.run_status = CFE_ES_RunStatus_APP_ERROR;
        }

        status = CFE_TBL_Manage(CF_AppData.config_handle);
        if (status < CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_TBL_CHECK_MAN, CFE_EVS_EventType_ERROR,
                              "CF: error in CFE_TBL_Manage (check), returned 0x%08lx", (unsigned long)status);
            CF_AppData.run_status = CFE_ES_RunStatus_APP_ERROR;
        }

        status = CFE_TBL_GetAddress((void *)&CF_AppData.config_table, CF_AppData.config_handle);
        if (status < CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_TBL_CHECK_GA, CFE_EVS_EventType_ERROR,
                              "CF: failed to get table address (check), returned 0x%08lx", (unsigned long)status);
            CF_AppData.run_status = CFE_ES_RunStatus_APP_ERROR;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_ValidateConfigTable
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_ValidateConfigTable(void *tbl_ptr)
{
    CF_ConfigTable_t  *tbl = (CF_ConfigTable_t *)tbl_ptr;
    int32              ret; /* initialized below */
    static const int32 no_ticks_per_second = -1;
    static const int32 crc_alignment       = -2;
    static const int32 outgoing_chunk_size = -3;

    if (!tbl->ticks_per_second)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_INIT_TPS, CFE_EVS_EventType_ERROR, "CF: config table has zero ticks per second");
        ret = no_ticks_per_second;
    }
    else if (!tbl->rx_crc_calc_bytes_per_wakeup || (tbl->rx_crc_calc_bytes_per_wakeup & 0x3ff))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_INIT_CRC_ALIGN, CFE_EVS_EventType_ERROR,
                          "CF: config table has rx crc size not aligned with 1024");
        ret = crc_alignment; /* must be 1024-byte aligned */
    }
    else if (tbl->outgoing_file_chunk_size > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_INIT_OUTGOING_SIZE, CFE_EVS_EventType_ERROR,
                          "CF: config table has outgoing file chunk size too large");
        ret = outgoing_chunk_size; /* must be smaller than file data character array */
    }
    else
    {
        ret = CFE_SUCCESS;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_TableInit
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_TableInit(void)
{
    int32 status = CFE_SUCCESS;

    status = CFE_TBL_Register(&CF_AppData.config_handle, CF_CONFIG_TABLE_NAME, sizeof(CF_ConfigTable_t),
                              CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP, CF_ValidateConfigTable);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_INIT_TBL_REG, CFE_EVS_EventType_ERROR,
                          "CF: error registering table, returned 0x%08lx", (unsigned long)status);
        goto err_out;
    }

    status = CFE_TBL_Load(CF_AppData.config_handle, CFE_TBL_SRC_FILE, CF_CONFIG_TABLE_FILENAME);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_INIT_TBL_LOAD, CFE_EVS_EventType_ERROR,
                          "CF: error loading table, returned 0x%08lx", (unsigned long)status);
        goto err_out;
    }

    status = CFE_TBL_Manage(CF_AppData.config_handle);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_EID_ERR_INIT_TBL_MANAGE, CFE_EVS_EventType_ERROR,
                          "CF: error in CFE_TBL_Manage, returned 0x%08lx", (unsigned long)status);
        goto err_out;
    }

    status = CFE_TBL_GetAddress((void **)&CF_AppData.config_table, CF_AppData.config_handle);
    /* status will be CFE_TBL_INFO_UPDATED because it was just loaded, but we can use CFE_SUCCESS too */
    if ((status != CFE_TBL_INFO_UPDATED) && (status != CFE_SUCCESS))
    {
        CFE_EVS_SendEvent(CF_EID_ERR_INIT_TBL_GETADDR, CFE_EVS_EventType_ERROR,
                          "CF: error getting table address, returned 0x%08lx", (unsigned long)status);
        goto err_out;
    }
    else
    {
        status = CFE_SUCCESS;
    }

err_out:
    return status;
}

/*----------------------------------------------------------------
 *
 * Function: CF_Init
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CF_Init(void)
{
    static CFE_EVS_BinFilter_t cf_event_filters[] = {
        {CF_EID_ERR_ASSERT, 0x0000},
        {CF_EID_INF_INIT, 0x0000},
        {CF_EID_ERR_INIT_TBL_CHECK_REL, 0x0000},
        {CF_EID_ERR_INIT_TBL_CHECK_MAN, 0x0000},
        {CF_EID_ERR_INIT_TBL_CHECK_GA, 0x0000},
        {CF_EID_ERR_INIT_TBL_REG, 0x0000},
        {CF_EID_ERR_INIT_TBL_LOAD, 0x0000},
        {CF_EID_ERR_INIT_TBL_MANAGE, 0x0000},
        {CF_EID_ERR_INIT_TBL_GETADDR, 0x0000},
        {CF_EID_ERR_INIT_CMD_LENGTH, 0x0000},
        {CF_EID_ERR_INIT_MSG_RECV, 0x0000},
        {CF_EID_ERR_INIT_SEM, 0x0000},
        {CF_EID_ERR_INIT_PIPE, 0x0000},
        {CF_EID_ERR_INIT_SUB, 0x0000},
        {CF_EID_ERR_INIT_TPS, 0x0000},
        {CF_EID_ERR_INIT_CRC_ALIGN, 0x0000},
        {CF_EID_ERR_INIT_OUTGOING_SIZE, 0x0000},
        {CF_EID_INF_PDU_MD_RECVD, 0x0000},
        {CF_EID_ERR_PDU_SHORT_HEADER, 0x0000},
        {CF_EID_ERR_PDU_LONG_VS_RECVD, 0x0000},
        {CF_EID_ERR_PDU_MD_SHORT, 0x0000},
        {CF_EID_ERR_PDU_INVALID_SRC_LEN, 0x0000},
        {CF_EID_ERR_PDU_INVALID_DST_LEN, 0x0000},
        {CF_EID_ERR_PDU_FD_SHORT, 0x0000},
        {CF_EID_ERR_PDU_EOF_SHORT, 0x0000},
        {CF_EID_ERR_PDU_ACK_SHORT, 0x0000},
        {CF_EID_ERR_PDU_FIN_SHORT, 0x0000},
        {CF_EID_ERR_PDU_NAK_SHORT, 0x0000},
        {CF_EID_ERR_PDU_BAD_RX_MSG_SIZE, 0x0000},
        {CF_EID_ERR_PDU_GET_EID_SIZE, 0x0000},
        {CF_EID_ERR_PDU_GET_TSN_SIZE, 0x0000},
        {CF_EID_ERR_CFDP_RX_DROPPED, 0x0000},
        {CF_EID_ERR_CFDP_INVALID_DST_EID, 0x0000},
        {CF_EID_ERR_CFDP_IDLE_MD, 0x0000},
        {CF_EID_ERR_CFDP_FD_UNHANDLED, 0x0000},
        {CF_EID_ERR_CFDP_MAX_CMD_TX, 0x0000},
        {CF_EID_ERR_CFDP_OPENDIR, 0x0000},
        {CF_EID_ERR_CFDP_DIR_SLOT, 0x0000},
        {CF_EID_ERR_CFDP_NO_MSG, 0x0000},
        {CF_EID_INF_CFDP_R_REQUEST_MD, 0x0000},
        {CF_EID_INF_CFDP_R_TEMP_FILE, 0x0000},
        {CF_EID_ERR_CFDP_R_NAK_LIMIT, 0x0000},
        {CF_EID_ERR_CFDP_R_ACK_LIMIT, 0x0000},
        {CF_EID_ERR_CFDP_R_CRC, 0x0000},
        {CF_EID_ERR_CFDP_R_SEEK_FD, 0x0000},
        {CF_EID_ERR_CFDP_R_SEEK_CRC, 0x0000},
        {CF_EID_ERR_CFDP_R_WRITE, 0x0000},
        {CF_EID_ERR_CFDP_R_SIZE_MISMATCH, 0x0000},
        {CF_EID_ERR_CFDP_R_PDU_EOF, 0x0000},
        {CF_EID_ERR_CFDP_R_CREAT, 0x0000},
        {CF_EID_ERR_CFDP_R_PDU_FINACK, 0x0000},
        {CF_EID_ERR_CFDP_R_EOF_MD_SIZE, 0x0000},
        {CF_EID_ERR_CFDP_R_RENAME, 0x0000},
        {CF_EID_ERR_CFDP_R_OPEN, 0x0000},
        {CF_EID_ERR_CFDP_R_PDU_MD, 0x0000},
        {CF_EID_ERR_CFDP_R_READ, 0x0000},
        {CF_EID_ERR_CFDP_R_DC_INV, 0x0000},
        {CF_EID_ERR_CFDP_R_INACT_TIMER, 0x0000},
        {CF_EID_INF_CFDP_S_START_SEND, 0x0000},
        {CF_EID_ERR_CFDP_S_SEEK_FD, 0x0000},
        {CF_EID_ERR_CFDP_S_READ, 0x0000},
        {CF_EID_ERR_CFDP_S_SEND_FD, 0x0000},
        {CF_EID_ERR_CFDP_S_ALREADY_OPEN, 0x0000},
        {CF_EID_ERR_CFDP_S_OPEN, 0x0000},
        {CF_EID_ERR_CFDP_S_SEEK_END, 0x0000},
        {CF_EID_ERR_CFDP_S_SEEK_BEG, 0x0000},
        {CF_EID_ERR_CFDP_S_SEND_MD, 0x0000},
        {CF_EID_ERR_CFDP_S_PDU_FIN, 0x0000},
        {CF_EID_ERR_CFDP_S_INVALID_SR, 0x0000},
        {CF_EID_ERR_CFDP_S_PDU_NAK, 0x0000},
        {CF_EID_ERR_CFDP_S_PDU_EOF, 0x0000},
        {CF_EID_ERR_CFDP_S_EARLY_FIN, 0x0000},
        {CF_EID_ERR_CFDP_S_DC_INV, 0x0000},
        {CF_EID_ERR_CFDP_S_NON_FD_PDU, 0x0000},
        {CF_EID_ERR_CFDP_S_ACK_LIMIT, 0x0000},
        {CF_EID_ERR_CFDP_S_INACT_TIMER, 0x0000},
        {CF_EID_INF_CMD_NOOP, 0x0000},
        {CF_EID_INF_CMD_RESET, 0x0000},
        {CF_EID_INF_CMD_GETSET1, 0x0000},
        {CF_EID_INF_CMD_GETSET2, 0x0000},
        {CF_EID_ERR_CMD_RESET_INVALID, 0x0000},
        {CF_EID_ERR_CMD_CHAN_PARAM, 0x0000},
        {CF_EID_ERR_CMD_TRANS_NOT_FOUND, 0x0000},
        {CF_EID_ERR_CMD_TSN_CHAN_INVALID, 0x0000},
        {CF_EID_ERR_CMD_SUSPRES_SAME, 0x0000},
        {CF_EID_ERR_CMD_SUSPRES_CHAN, 0x0000},
        {CF_EID_ERR_CMD_POLLDIR_INVALID, 0x0000},
        {CF_EID_ERR_CMD_PURGE_ARG, 0x0000},
        {CF_EID_ERR_CMD_WQ_CHAN, 0x0000},
        {CF_EID_ERR_CMD_WQ_ARGS, 0x0000},
        {CF_EID_ERR_CMD_WQ_OPEN, 0x0000},
        {CF_EID_ERR_CMD_WQ_WRITEQ_RX, 0x0000},
        {CF_EID_ERR_CMD_WQ_WRITEHIST_RX, 0x0000},
        {CF_EID_ERR_CMD_WQ_WRITEQ_TX, 0x0000},
        {CF_EID_ERR_CMD_WQ_WRITEQ_PEND, 0x0000},
        {CF_EID_ERR_CMD_WQ_WRITEHIST_TX, 0x0000},
        {CF_EID_ERR_CMD_GETSET_VALIDATE, 0x0000},
        {CF_EID_ERR_CMD_GETSET_PARAM, 0x0000},
        {CF_EID_ERR_CMD_ENABLE_ENGINE, 0x0000},
        {CF_EID_ERR_CMD_ENG_ALREADY_ENA, 0x0000},
        {CF_EID_ERR_CMD_ENG_ALREADY_DIS, 0x0000},
        {CF_EID_ERR_CMD_GCMD_LEN, 0x0000},
        {CF_EID_ERR_CMD_GCMD_CC, 0x0000},
        {CF_EID_ERR_CMD_WHIST_WRITE, 0x0000},
    };

    int32                            status       = CFE_SUCCESS;
    static const CFE_SB_MsgId_Atom_t MID_VALUES[] = {CF_CMD_MID, CF_SEND_HK_MID, CF_WAKE_UP_MID};
    uint32                           i;

    CF_AppData.run_status = CFE_ES_RunStatus_APP_RUN;

    CFE_MSG_Init(&CF_AppData.hk.tlm_header.Msg, CFE_SB_ValueToMsgId(CF_HK_TLM_MID), sizeof(CF_AppData.hk));
    CFE_MSG_Init(&CF_AppData.cfg.tlm_header.Msg, CFE_SB_ValueToMsgId(CF_CONFIG_TLM_MID), sizeof(CF_AppData.cfg));

    if ((status = CFE_EVS_Register(cf_event_filters, sizeof(cf_event_filters) / sizeof(*cf_event_filters),
                                   CFE_EVS_EventFilter_BINARY)) != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CF app: error registering with EVS, returned 0x%08lx", (unsigned long)status);
        goto err_out;
    }

    if ((status = CFE_SB_CreatePipe(&CF_AppData.cmd_pipe, CF_PIPE_DEPTH, CF_PIPE_NAME)) != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CF app: error creating pipe %s, returend 0x%08lx", CF_PIPE_NAME, (unsigned long)status);
        goto err_out;
    }

    for (i = 0; i < (sizeof(MID_VALUES) / sizeof(MID_VALUES[0])); ++i)
    {
        if ((status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MID_VALUES[i]), CF_AppData.cmd_pipe)) != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("CF app: failed to subscribe to MID 0x%04lx, returned 0x%08lx",
                                 (unsigned long)MID_VALUES[i], (unsigned long)status);
            goto err_out;
        }
    }

    if ((status = CF_TableInit()) != CFE_SUCCESS)
    {
        /* function sends event internally */
        goto err_out;
    }

    if ((status = CF_CFDP_InitEngine()) != CFE_SUCCESS)
    {
        /* function sends event internally */
        goto err_out;
    }

    status = CFE_EVS_SendEvent(CF_EID_INF_INIT, CFE_EVS_EventType_INFORMATION, "CF Initialized. Version %d.%d.%d",
                               CF_MAJOR_VERSION, CF_MINOR_VERSION, CF_REVISION);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CF: error sending init event, returned 0x%08lx", (unsigned long)status);
        goto err_out;
    }

err_out:
    return status;
}

/*----------------------------------------------------------------
 *
 * Function: CF_WakeUp
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_WakeUp(void)
{
    CFE_ES_PerfLogEntry(CF_PERF_ID_CYCLE_ENG);
    CF_CFDP_CycleEngine();
    CFE_ES_PerfLogExit(CF_PERF_ID_CYCLE_ENG);
}

/*----------------------------------------------------------------
 *
 * Function: CF_ProcessMsg
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_ProcessMsg(CFE_SB_Buffer_t *msg)
{
    CFE_SB_MsgId_t msg_id;

    CFE_MSG_GetMsgId(&msg->Msg, &msg_id);

    switch (CFE_SB_MsgIdToValue(msg_id))
    {
        case CF_CMD_MID:
            CF_ProcessGroundCommand(msg);
            break;

        case CF_WAKE_UP_MID:
            CF_WakeUp();
            break;

        case CF_SEND_HK_MID:
            CF_HkCmd();
            CF_CheckTables();
            break;

        default:
            ++CF_AppData.hk.counters.err;
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_CMD_LENGTH, CFE_EVS_EventType_ERROR,
                              "CF: invalid command packet id=0x%lx", (unsigned long)CFE_SB_MsgIdToValue(msg_id));
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_AppMain
 *
 * Entry point function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_AppMain(void)
{
    int32            status;
    CFE_SB_Buffer_t *msg;

    CFE_ES_PerfLogEntry(CF_PERF_ID_APPMAIN);

    status = CF_Init();
    if (status != CFE_SUCCESS)
    {
        CF_AppData.run_status = CFE_ES_RunStatus_APP_ERROR;
    }

    msg = NULL;

    while (CFE_ES_RunLoop(&CF_AppData.run_status))
    {
        CFE_ES_PerfLogExit(CF_PERF_ID_APPMAIN);

        status = CFE_SB_ReceiveBuffer(&msg, CF_AppData.cmd_pipe, CF_RCVMSG_TIMEOUT);
        CFE_ES_PerfLogEntry(CF_PERF_ID_APPMAIN);

        /*
         * note that CFE_SB_ReceiveBuffer() guarantees that a CFE_SUCCESS status is accompanied by
         * a valid (non-NULL) output message pointer.  However the unit test can force this condition.
         */
        if (status == CFE_SUCCESS && msg != NULL)
        {
            CF_ProcessMsg(msg);
        }
        else if (status != CFE_SB_TIME_OUT && status != CFE_SB_NO_MESSAGE)
        {
            CFE_EVS_SendEvent(CF_EID_ERR_INIT_MSG_RECV, CFE_EVS_EventType_ERROR,
                              "CF: exiting due to CFE_SB_ReceiveBuffer error 0x%08lx", (unsigned long)status);
            CF_AppData.run_status = CFE_ES_RunStatus_APP_ERROR;
        }
        else
        {
            /* nothing */
        }
    }

    CFE_ES_PerfLogExit(CF_PERF_ID_APPMAIN);
    CFE_ES_ExitApp(CF_AppData.run_status);
}
