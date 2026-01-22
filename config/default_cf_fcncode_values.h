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

 enum CF_FunctionCode_
 {
    CF_FunctionCode_NOOP                  = 0,
    CF_FunctionCode_RESET_COUNTERS        = 1,
    CF_FunctionCode_TX_FILE               = 2,
    CF_FunctionCode_PLAYBACK_DIR          = 3,
    CF_FunctionCode_FREEZE                = 4,
    CF_FunctionCode_THAW                  = 5,
    CF_FunctionCode_SUSPEND               = 6,
    CF_FunctionCode_RESUME                = 7,
    CF_FunctionCode_CANCEL                = 8,
    CF_FunctionCode_ABANDON               = 9,
    CF_FunctionCode_SET_PARAM             = 10,
    CF_FunctionCode_GET_PARAM             = 11,
    CF_FunctionCode_WRITE_QUEUE           = 15,
    CF_FunctionCode_ENABLE_DEQUEUE        = 16,
    CF_FunctionCode_DISABLE_DEQUEUE       = 17,
    CF_FunctionCode_ENABLE_DIR_POLLING    = 18,
    CF_FunctionCode_DISABLE_DIR_POLLING   = 19,
    CF_FunctionCode_PURGE_QUEUE           = 21,
    CF_FunctionCode_ENABLE_ENGINE         = 22,
    CF_FunctionCode_DISABLE_ENGINE        = 23,
 };

 #endif