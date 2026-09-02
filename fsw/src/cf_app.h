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
#include "cf_tbl.h"
#include "cf_msgids.h"
#include "cf_tbldefs.h"
#include "cf_mission_cfg.h"
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

/**
 * @brief Marker used to flag filenames suspected of being truncated
 */
#define CF_FILENAME_TRUNCATED '$'

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
    uint32 RunStatus;

    CF_HkCmdCounters_t counters;
    CFE_SB_PipeId_t    CmdPipe;
    CFE_TBL_Handle_t   config_handle;

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
/**
 * @brief Gets the CFDP engine struct
 *
 * @par Description
 *      Obtains a pointer to the CFDP engine.
 *
 * @par Assumptions, External Events, and Notes:
 *      This is currently a singleon in the CF app, but the architecture
 *      does permit there to be more than one.
 *
 *      This never returns NULL, in the current implementation there is
 *      one (and only one) engine.
 *
 * @returns engine pointer
 */
static inline CF_Engine_t *CF_GetEngine(void)
{
    return &CF_AppData.engine;
}

/************************************************************************/
/**
 * @brief Checks if a class identifier is valid
 *
 * @par Description
 *      checks if the given class number matches one of the defined enum values
 *
 * @par Assumptions, External Events, and Notes:
 *      None
 *
 * @retval true if value is valid/acceptable
 * @retval false if value is invalid
 */
static inline bool CF_IsValidClass(CF_CFDP_Class_Enum_t cv)
{
    return (cv == CF_CFDP_Class_1 || cv == CF_CFDP_Class_2);
}

/************************************************************************/
/**
 * @brief Checks if a channel identifier is valid
 *
 * @par Description
 *      checks if the given channel number matches one of the defined values
 *
 * @par Assumptions, External Events, and Notes:
 *      This only checks if it matches a single channel.  It does not test
 *      for the special values that some commands might accept.
 *
 * @retval true if value is valid/acceptable
 * @retval false if value is invalid
 */
static inline bool CF_IsValidChannel(CF_ChannelSelect_t cs)
{
    return (cs > 0 && cs <= CF_NUM_CHANNELS);
}

/************************************************************************/
/**
 * @brief Gets the channel number as an integer
 *
 * @par Description
 *      For consistent representation of channel numbers in logs messages
 *      and other strings.
 *
 * @par Assumptions, External Events, and Notes:
 *      Format log messages with printf "%d" conversion
 *
 * @returns channel number represented as "int" type
 */
static inline int CF_ChannelSelect_AsInt(CF_ChannelSelect_t cs)
{
    return (cs);
}

/************************************************************************/
/**
 * @brief Gets the channel number as an array index (0-based)
 *
 * @par Description
 *      For use when indexing the channel number in a another array such
 *      as configuration tables or performance logs, requiriing a 0-based index.
 *
 * @par Assumptions, External Events, and Notes:
 *      Format log messages with printf "%d" conversion
 *
 * @returns channel number represented as "int" type
 */
static inline int CF_ChannelSelect_AsIndex(CF_ChannelSelect_t cs)
{
    return CF_ChannelSelect_AsInt(cs) - 1;
}

/************************************************************************/
/**
 * @brief Gets the channel number from integer
 *
 * @par Description
 *      For intentional conversion from an integer value to a channel number
 *
 * @par Assumptions, External Events, and Notes:
 *      this exists for special/corner cases only, should really not be
 *      needed in general.  Prefer to use the APIs that invoke a subroutine
 *      using the standard selector logic.
 *
 * @returns channel number value
 */
static inline CF_ChannelSelect_t CF_ChannelSelect_FromInt(int c)
{
    return (CF_ChannelSelect_t) { c };
}

/************************************************************************/
/**
 * @brief Checks if a channel identifier represents "all channels"
 *
 * @par Description
 *      checks if the given channel number matches the special all channels value
 *
 * @par Assumptions, External Events, and Notes:
 *      This only checks if it matches all channels.
 *
 * @retval true if value is valid/acceptable
 * @retval false if value is invalid
 */
static inline bool CF_IsAllChannels(CF_ChannelSelect_t cs)
{
    return (cs == CF_ALL_CHANNELS);
}

/************************************************************************/
/**
 * @brief Gets the channel pointer from a channel number
 *
 * @par Description
 *      Obtains the channel pointer associated with the channel number
 *
 * @par Assumptions, External Events, and Notes:
 *      This implements range-checking and may return NULL
 *
 * @returns channel pointer
 * @retval NULL if channel number is invalid
 */
static inline CF_Channel_t *CF_GetChannelPtr(CF_ChannelSelect_t cs)
{
    if (CF_IsValidChannel(cs))
    {
        /* the channel value from user interface should be 1-based */
        return &CF_GetEngine()->channels[CF_ChannelSelect_AsIndex(cs)];
    }
    else
    {
        return NULL;
    }
}

/************************************************************************/
/**
 * @brief Gets the channel number from a channel pointer
 *
 * @par Description
 *      Obtains the channel number associated with the channel pointer
 *
 * @par Assumptions, External Events, and Notes:
 *      This is primarily for log messages.
 *
 * @returns channel number
 */
static inline CF_ChannelSelect_t CF_GetChannelFromPtr(const CF_Channel_t *ChanPtr)
{
    CF_Engine_t *engine_ptr = CF_GetEngine();
    /* the channel value to user interface should be 1-based */
    return (CF_ChannelSelect_t) { 1 + (ChanPtr - engine_ptr->channels) };
}

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
CFE_Status_t CF_AppInit(void);

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
