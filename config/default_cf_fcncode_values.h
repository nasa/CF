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
 *   Specification for the CFS CFDP (CF) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef DEFAULT_CF_FCNCODES_H
#define DEFAULT_CF_FCNCODES_H

/************************************************************************
 * Macro Definitions
 ************************************************************************/

#define CF_CCVAL(x) CF_FunctionCode_##x

/*
 * For the interim release, the pre-existing function codes will point
 * to the compatibility shim for CF commands.  This shim will convert the
 * arguments into the new form and invoke the new command.  The new commands
 * have the command code shifted up by 30.
 */
enum CF_FunctionCode
{
    CF_FunctionCode_NOOP                       = 0,
    CF_FunctionCode_RESET_COUNTERS_COMPAT      = 1,
    CF_FunctionCode_TX_FILE_COMPAT             = 2,
    CF_FunctionCode_PLAYBACK_DIR_COMPAT        = 3,
    CF_FunctionCode_FREEZE_COMPAT              = 4,
    CF_FunctionCode_THAW_COMPAT                = 5,
    CF_FunctionCode_SUSPEND_COMPAT             = 6,
    CF_FunctionCode_RESUME_COMPAT              = 7,
    CF_FunctionCode_CANCEL_COMPAT              = 8,
    CF_FunctionCode_ABANDON_COMPAT             = 9,
    CF_FunctionCode_SET_PARAM_COMPAT           = 10,
    CF_FunctionCode_GET_PARAM_COMPAT           = 11,
    CF_FunctionCode_WRITE_QUEUE_COMPAT         = 15,
    CF_FunctionCode_ENABLE_DEQUEUE_COMPAT      = 16,
    CF_FunctionCode_DISABLE_DEQUEUE_COMPAT     = 17,
    CF_FunctionCode_ENABLE_DIR_POLLING_COMPAT  = 18,
    CF_FunctionCode_DISABLE_DIR_POLLING_COMPAT = 19,
    CF_FunctionCode_PURGE_QUEUE_COMPAT         = 21,

    /* NOTE: these commands did not change (they have no args) */
    CF_FunctionCode_ENABLE_ENGINE  = 22,
    CF_FunctionCode_DISABLE_ENGINE = 23,

    CF_FunctionCode_RESET_COUNTERS      = 31,
    CF_FunctionCode_TX_FILE             = 32,
    CF_FunctionCode_PLAYBACK_DIR        = 33,
    CF_FunctionCode_FREEZE              = 34,
    CF_FunctionCode_THAW                = 35,
    CF_FunctionCode_SUSPEND             = 36,
    CF_FunctionCode_RESUME              = 37,
    CF_FunctionCode_CANCEL              = 38,
    CF_FunctionCode_ABANDON             = 39,
    CF_FunctionCode_SET_PARAM           = 40,
    CF_FunctionCode_GET_PARAM           = 41,
    CF_FunctionCode_WRITE_QUEUE         = 45,
    CF_FunctionCode_ENABLE_DEQUEUE      = 46,
    CF_FunctionCode_DISABLE_DEQUEUE     = 47,
    CF_FunctionCode_ENABLE_DIR_POLLING  = 48,
    CF_FunctionCode_DISABLE_DIR_POLLING = 49,
    CF_FunctionCode_PURGE_QUEUE         = 51,

};

#endif
