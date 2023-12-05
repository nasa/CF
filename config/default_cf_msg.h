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
 *   Specification for the CFS CFDP (CF) command and telemetry
 *   message data types.
 *
 * This is a compatibility header for the "cf_msg.h" file that has
 * traditionally provided the message definitions for cFS apps.
 *
 * @note This file may be overridden/superceded by mission-provided defintions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CF_MSG_H
#define CF_MSG_H

#include "cf_interface_cfg.h"
#include "cf_fcncodes.h"
#include "cf_msgdefs.h"
#include "cf_msgstruct.h"

#define CF_COMPOUND_KEY (254)
#define CF_ALL_CHANNELS (255)
#define CF_ALL_POLLDIRS (CF_ALL_CHANNELS)

#endif
