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
 * The CF Application main application header file
 */

#ifndef CF_APP_H
#define CF_APP_H

#include "cfe.h"

#include "cf_msg.h"
#include "cf_msgids.h"
#include "cf_tbldefs.h"
#include "cf_platform_cfg.h"
#include "cf_cfdp.h"
#include "cf_clist.h"

/**************************************************************************
 **
 ** Macro definitions
 **
 **************************************************************************/

/**
 * \name CF Error Codes
 * \{
 */
#define CF_ERROR                        -1 /**< \brief Generic CF error return code */
#define CF_PDU_METADATA_ERROR           -2 /**< \brief Invalid metadata PDU */
#define CF_SHORT_PDU_ERROR              -3 /**< \brief PDU too short */
#define CF_REC_PDU_FSIZE_MISMATCH_ERROR -4 /**< \brief Receive PDU: EOF file size mismatch */
#define CF_REC_PDU_BAD_EOF_ERROR        -5 /**< \brief Receive PDU: Invalid EOF packet */
#define CF_SEND_PDU_NO_BUF_AVAIL_ERROR  -6 /**< \brief Send PDU: No send buffer available, throttling limit reached */
#define CF_SEND_PDU_ERROR               -7 /**< \brief Send PDU: Send failed */
/**\}*/

/**
 * @brief The name of the application command pipe for CF
 */
#define CF_PIPE_NAME ("CF_CMD_PIPE")

/**
 * @brief A common prefix for all data pipes for CF
 */
#define CF_CHANNEL_PIPE_PREFIX ("CF_CHAN_")

/*************************************************************************
 **
 ** Type definitions
 **
 **************************************************************************/

/**
 * @brief The CF application global state structure
 *
 * This contains all variables related to CF application state
 */
typedef struct
{
    CF_HkPacket_t HkPacket;

    uint32 run_status;

    CFE_SB_PipeId_t cmd_pipe;

    CFE_TBL_Handle_t  config_handle;
    CF_ConfigTable_t *config_table;

    CF_Engine_t engine;
} CF_AppData_t;

/**************************************************************************
 **
 ** Exported data
 **
 **************************************************************************/

/**
 * @brief Singleton instance of the application global data
 */
extern CF_AppData_t CF_AppData;

/**************************************************************************
 **
 **  Function Prototypes
 **
 **************************************************************************/

/************************************************************************/
/** @brief Send CF housekeeping packet
 *
 * @par Description
 *      The command to send the CF housekeeping packet
 *
 * @par Assumptions, External Events, and Notes:
 *      None
 */
void CF_HkCmd(void);

/************************************************************************/
/** @brief Checks to see if a table update is pending, and perform it.
 *
 * @par Description
 *       Updates the table if the engine is disabled.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 */
void CF_CheckTables(void);

/************************************************************************/
/** @brief Validation function for config table.
 *
 * @par Description
 *       Checks that the config table being loaded has correct data.
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 * @retval CFE_STATUS_VALIDATION_FAILURE if the config table fails one of the validation checks
 *
 */
CFE_Status_t CF_ValidateConfigTable(void *tbl_ptr);

/************************************************************************/
/** @brief Load the table on application start
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 * @retval Returns anything else on error.
 *
 */
CFE_Status_t CF_TableInit(void);

/************************************************************************/
/** @brief CF app init function
 *
 * @par Description
 *       Initializes all aspects of the CF application. Messages,
 *       pipes, events, table, and the CFDP engine.
 *
 * @par Assumptions, External Events, and Notes:
 *       This must only be called once.
 *
 *
 * @retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 * @retval Returns anything else on error.
 *
 */
CFE_Status_t CF_Init(void);

/************************************************************************/
/** @brief CF wakeup function
 *
 * @par Description
 *       Performs a single engine cycle for each wakeup
 *
 * @par Assumptions, External Events, and Notes:
 *       None
 *
 */
void CF_WakeUp(void);

/************************************************************************/
/** @brief CF message processing function
 *
 * @par Description
 *       Process message packets received via the Software Bus command pipe
 *
 * @par Assumptions, External Events, and Notes:
 *       msg must not be NULL.
 *
 * @param[in] msg Software Bus message pointer
 *
 */
void CF_ProcessMsg(CFE_SB_Buffer_t *msg);

/************************************************************************/
/** @brief CF app entry point
 *
 * @par Description
 *       Main entry point of CF application.
 *       Calls the init function and manages the app run loop.
 *
 * @par Assumptions, External Events, and Notes:
 *       This must only be called once.
 *
 */
void CF_AppMain(void);

#endif /* !CF_APP_H */
