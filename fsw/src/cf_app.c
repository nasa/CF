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
#include "cf_eventids.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"
#include "cf_version.h"
#include "cf_dispatch.h"
#include "cf_utils.h"
#include "cf_tbl.h"

#include <string.h>

CF_AppData_t CF_AppData;

/*----------------------------------------------------------------
 *
 * Local helper function
 * Imports configuration from table for a single channel
 * Compatible with CF_ForEachChannel()
 *
 *-----------------------------------------------------------------*/
static int32 CF_DoChannelConfig(CF_Engine_t *engine_ptr, CF_Channel_t *chan, void *arg)
{
    const CF_ConfigTable_t   *config      = arg;
    const int                 chan_num    = CF_ChannelSelect_AsInt(CF_GetChannelFromPtr(chan));
    const CF_ChannelConfig_t *chan_config = &config->chan[chan_num];
    const CF_PollDir_t       *tbl_pd_config;
    CF_LocalPdConfig_t       *pd_config;
    int                       pd;

    chan->config.max_outgoing_messages_per_wakeup = chan_config->max_outgoing_messages_per_wakeup;
    chan->config.rx_max_messages_per_wakeup       = chan_config->rx_max_messages_per_wakeup;
    chan->config.ack_timer_s                      = chan_config->ack_timer_s;
    chan->config.nak_timer_s                      = chan_config->nak_timer_s;
    chan->config.inactivity_timer_s               = chan_config->inactivity_timer_s;
    chan->config.ack_limit                        = chan_config->ack_limit;
    chan->config.nak_limit                        = chan_config->nak_limit;
    chan->config.mid_input                        = chan_config->mid_input;
    chan->config.mid_output                       = chan_config->mid_output;
    chan->config.pipe_depth_input                 = chan_config->pipe_depth_input;
    chan->config.dequeue_enabled                  = chan_config->dequeue_enabled;

    CFE_SB_MessageStringGet(chan->config.sem_name,
                            chan_config->sem_name,
                            "",
                            sizeof(chan->config.sem_name),
                            sizeof(chan_config->sem_name));
    CFE_SB_MessageStringGet(chan->config.move_dir,
                            chan_config->move_dir,
                            "",
                            sizeof(chan->config.move_dir),
                            sizeof(chan_config->move_dir));

    for (pd = 0; pd < CF_MAX_POLLING_DIR_PER_CHAN; ++pd)
    {
        tbl_pd_config = &chan_config->polldir[pd];
        pd_config     = &chan->config.polldir[pd];

        pd_config->enabled      = tbl_pd_config->enabled;
        pd_config->priority     = tbl_pd_config->priority;
        pd_config->interval_sec = tbl_pd_config->interval_sec;
        pd_config->cfdp_class   = tbl_pd_config->cfdp_class;
        pd_config->dest_eid     = tbl_pd_config->dest_eid;

        CFE_SB_MessageStringGet(pd_config->src_dir,
                                tbl_pd_config->src_dir,
                                "",
                                sizeof(pd_config->src_dir),
                                sizeof(tbl_pd_config->src_dir));
        CFE_SB_MessageStringGet(pd_config->dst_dir,
                                tbl_pd_config->dst_dir,
                                "",
                                sizeof(pd_config->dst_dir),
                                sizeof(tbl_pd_config->dst_dir));
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Checks for table updates
 *
 *-----------------------------------------------------------------*/
static CFE_Status_t CF_ManageTables(CF_Engine_t *engine_ptr, bool always_import)
{
    CFE_Status_t            status;
    void                   *table_addr;
    const CF_ConfigTable_t *config;

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
    status = CFE_TBL_Manage(CF_AppData.config_handle);
    if (status < CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_INIT_TBL_MANAGE_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: error in CFE_TBL_Manage, returned 0x%08lx",
                          (unsigned long)status);
    }
    else if (always_import || status == CFE_TBL_INFO_UPDATED)
    {
        status = CFE_TBL_GetAddress(&table_addr, CF_AppData.config_handle);
        if (status < CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_INIT_TBL_GETADDR_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF: failed to get table address, returned 0x%08lx",
                              (unsigned long)status);
        }
        else
        {
            config = table_addr;

            engine_ptr->config.ticks_per_second             = config->ticks_per_second;
            engine_ptr->config.rx_crc_calc_bytes_per_wakeup = config->rx_crc_calc_bytes_per_wakeup;
            engine_ptr->config.local_eid                    = config->local_eid;
            engine_ptr->config.outgoing_file_chunk_size     = config->outgoing_file_chunk_size;

            CFE_SB_MessageStringGet(engine_ptr->config.tmp_dir,
                                    config->tmp_dir,
                                    "",
                                    sizeof(engine_ptr->config.tmp_dir),
                                    sizeof(config->tmp_dir));
            CFE_SB_MessageStringGet(engine_ptr->config.fail_dir,
                                    config->fail_dir,
                                    "",
                                    sizeof(engine_ptr->config.fail_dir),
                                    sizeof(config->fail_dir));

            CF_ForEachChannel(engine_ptr, CF_DoChannelConfig, table_addr);

            /* Release the address, the config is now in the CF global data */
            status = CFE_TBL_ReleaseAddress(CF_AppData.config_handle);
            if (status < CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CF_INIT_TBL_CHECK_REL_ERR_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "CF: error in CFE_TBL_ReleaseAddress, returned 0x%08lx",
                                  (unsigned long)status);
            }
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
void CF_CheckTables(void)
{
    CF_Engine_t *engine_ptr = CF_GetEngine();

    /* check the table for an update only if engine is disabled */
    if (!engine_ptr->enabled)
    {
        /* NOTE: We do not need to abort the app if this fails, it can
         * keep running with the existing config in RAM.  A failure here only
         * means that the config is not applied */
        CF_ManageTables(engine_ptr, false);
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
        CFE_EVS_SendEvent(CF_INIT_CRC_ALIGN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: config table has rx CRC size not aligned with 1024");
    }
    else if (tbl->outgoing_file_chunk_size > sizeof(CF_CFDP_PduFileDataContent_t))
    {
        CFE_EVS_SendEvent(CF_INIT_OUTGOING_SIZE_ERR_EID,
                          CFE_EVS_EventType_ERROR,
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

    status = CFE_TBL_Register(&CF_AppData.config_handle,
                              CF_CONFIG_TABLE_NAME,
                              sizeof(CF_ConfigTable_t),
                              CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP,
                              CF_ValidateConfigTable);
    if (status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_INIT_TBL_REG_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "CF: error registering table, returned 0x%08lx",
                          (unsigned long)status);
    }
    else
    {
        status = CFE_TBL_Load(CF_AppData.config_handle, CFE_TBL_SRC_FILE, CF_CONFIG_TABLE_FILENAME);
        if (status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CF_INIT_TBL_LOAD_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF: error loading table, returned 0x%08lx",
                              (unsigned long)status);
        }
        else
        {
            status = CF_ManageTables(CF_GetEngine(), true);
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
    const CFE_SB_MsgId_Atom_t MID_VALUES[] = { CF_CMD_MID, CF_SEND_HK_MID, CF_WAKE_UP_MID };
    uint32                    i;

    /* Zero-out global data structure */
    memset(&CF_AppData, 0, sizeof(CF_AppData));

    CF_AppData.RunStatus = CFE_ES_RunStatus_APP_RUN;

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
            CFE_EVS_SendEvent(CF_CR_PIPE_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF app: error creating pipe %s, returned 0x%08lx",
                              CF_PIPE_NAME,
                              (unsigned long)status);
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
                                     (unsigned long)MID_VALUES[i],
                                     (unsigned long)status);
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
        status = CF_CFDP_InitEngine(CF_GetEngine()); /* function sends event internally */
    }

    if (status == CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CF_INIT_INF_EID,
                          CFE_EVS_EventType_INFORMATION,
                          "CF Initialized. Version %d.%d.%d.%d",
                          CF_MAJOR_VERSION,
                          CF_MINOR_VERSION,
                          CF_REVISION,
                          CF_MISSION_REV);
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
            CFE_EVS_SendEvent(CF_INIT_MSG_RECV_ERR_EID,
                              CFE_EVS_EventType_ERROR,
                              "CF: exiting due to CFE_SB_ReceiveBuffer error 0x%08lx",
                              (unsigned long)status);
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
