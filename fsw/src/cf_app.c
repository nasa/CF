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

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_version.h"
#include "cf_dispatch.h"
#include "cf_tbl.h"

#include <string.h>

CF_AppData_t CF_AppData;

/*----------------------------------------------------------------
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
            CFE_EVS_SendEvent(CF_INIT_TBL_CHECK_REL_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: error in CFE_TBL_ReleaseAddress (check), returned 0x%08lx", (unsigned long)status);
            CF_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

        status = CFE_TBL_Manage(CF_AppData.config_handle);
        if (status < CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_INIT_TBL_CHECK_MAN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: error in CFE_TBL_Manage (check), returned 0x%08lx", (unsigned long)status);
            CF_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }

        status = CFE_TBL_GetAddress((void *)&CF_AppData.config_table, CF_AppData.config_handle);
        if (status < CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_INIT_TBL_CHECK_GA_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: failed to get table address (check), returned 0x%08lx", (unsigned long)status);
            CF_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_ValidateConfigTable(void *tbl_ptr)
{
    CF_ConfigTable_t *tbl = (CF_ConfigTable_t *)tbl_ptr;
    CFE_Status_t      ret = CFE_STATUS_VALIDATION_FAILURE;

    if (!tbl->ticks_per_second)
    {
        CFE_EVS_SendEvent(CF_INIT_TPS_ERR_EID, CFE_EVS_EventType_ERROR, "CF: config table has zero ticks per second");
    }
    else if (!tbl->rx_crc_calc_bytes_per_wakeup || (tbl->rx_crc_calc_bytes_per_wakeup & 0x3ff))
    {
        CFE_EVS_SendEvent(CF_INIT_CRC_ALIGN_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: config table has rx CRC size not aligned with 1024");
    }
    else if (tbl->outgoing_file_chunk_size > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        CFE_EVS_SendEvent(CF_INIT_OUTGOING_SIZE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: config table has outgoing file chunk size too large");
    }
    else
    {
        ret = CFE_SUCCESS;
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_TableInit(void)
{
    CFE_Status_t status;

    status = CFE_TBL_Register(&CF_AppData.config_handle, CF_CONFIG_TABLE_NAME, sizeof(CF_ConfigTable_t),
                              CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP, CF_ValidateConfigTable);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_INIT_TBL_REG_ERR_EID, CFE_EVS_EventType_ERROR,
                          "CF: error registering table, returned 0x%08lx", (unsigned long)status);
    }
    else
    {
        status = CFE_TBL_Load(CF_AppData.config_handle, CFE_TBL_SRC_FILE, CF_CONFIG_TABLE_FILENAME);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_INIT_TBL_LOAD_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: error loading table, returned 0x%08lx", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_TBL_Manage(CF_AppData.config_handle);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_INIT_TBL_MANAGE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: error in CFE_TBL_Manage, returned 0x%08lx", (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CFE_TBL_GetAddress((void **)&CF_AppData.config_table, CF_AppData.config_handle);
        /* status will be CFE_TBL_INFO_UPDATED because it was just loaded, but we can use CFE_SUCCESS too */
        if ((status != CFE_TBL_INFO_UPDATED) && (status != CFE_SUCCESS))
        {
            CFE_EVS_SendEvent(CF_INIT_TBL_GETADDR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: error getting table address, returned 0x%08lx", (unsigned long)status);
        }
        else
        {
            status = CFE_SUCCESS;
        }
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CF_AppInit(void)
{
    CFE_Status_t              status;
    const CFE_SB_MsgId_Atom_t MID_VALUES[] = {CF_CMD_MID, CF_SEND_HK_MID, CF_WAKE_UP_MID};
    uint32                    i;

    /* Zero-out global data structure */
    memset(&CF_AppData, 0, sizeof(CF_AppData));

    CF_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

    CFE_MSG_Init(CFE_MSG_PTR(CF_AppData.hk.TelemetryHeader), CFE_SB_ValueToMsgId(CF_HK_TLM_MID), sizeof(CF_AppData.hk));

    status = CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("CF app: error registering with EVS, returned 0x%08lx", (unsigned long)status);
    }
    else
    {
        status = CFE_SB_CreatePipe(&CF_AppData.CmdPipe, CF_PIPE_DEPTH, CF_PIPE_NAME);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_CR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF app: error creating pipe %s, returned 0x%08lx", CF_PIPE_NAME, (unsigned long)status);
        }
    }

    if (status == CFE_SUCCESS)
    {
        for (i = 0; i < (sizeof(MID_VALUES) / sizeof(MID_VALUES[0])); ++i)
        {
            status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(MID_VALUES[i]), CF_AppData.CmdPipe);
            if (status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("CF app: failed to subscribe to MID 0x%04lx, returned 0x%08lx",
                                     (unsigned long)MID_VALUES[i], (unsigned long)status);
                break;
            }
        }
    }

    if (status == CFE_SUCCESS)
    {
        status = CF_TableInit(); /* function sends event internally */
    }

    if (status == CFE_SUCCESS)
    {
        status = CF_CFDP_InitEngine(); /* function sends event internally */
    }

    if (status == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "CF Initialized. Version %d.%d.%d.%d",
                          CF_MAJOR_VERSION, CF_MINOR_VERSION, CF_REVISION, CF_MISSION_REV);
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Entry point function
 * See description in cf_app.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_AppMain(void)
{
    int32            status;
    CFE_SB_Buffer_t *BufPtr = NULL;

    CFE_ES_PerfLogEntry(CF_PERF_ID_APPMAIN);

    status = CF_AppInit();
    if (status != CFE_SUCCESS)
    {
        CF_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    while (CFE_ES_RunLoop(&CF_AppData.RunStatus))
    {
        CFE_ES_PerfLogExit(CF_PERF_ID_APPMAIN);

        status = CFE_SB_ReceiveBuffer(&BufPtr, CF_AppData.CmdPipe, CF_RCVMSG_TIMEOUT);
        CFE_ES_PerfLogEntry(CF_PERF_ID_APPMAIN);

        if (status == CFE_SUCCESS)
        {
            CF_AppPipe(BufPtr);
        }
        else if (status != CFE_SB_TIME_OUT && status != CFE_SB_NO_MESSAGE)
        {
            CFE_EVS_SendEvent(CF_INIT_MSG_RECV_ERR_EID, CFE_EVS_EventType_ERROR,
                              "CF: exiting due to CFE_SB_ReceiveBuffer error 0x%08lx", (unsigned long)status);
            CF_AppData.RunStatus = CFE_ES_RunStatus_APP_ERROR;
        }
        else
        {
            /* nothing */
        }
    }

    CFE_ES_PerfLogExit(CF_PERF_ID_APPMAIN);
    CFE_ES_ExitApp(CF_AppData.RunStatus);
}
