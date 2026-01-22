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
 *   CFS CFDP (CF) Application Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.  This includes the CMD/TLM message
 * interface, tables definitions, and any other data products that
 * serve to exchange information with other entities.
 *
 * @note This file may be overridden/superceded by mission-provided defintions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */

#ifndef EDS_CF_INTERFACE_CFG_VALUES_H
#define EDS_CF_INTERFACE_CFG_VALUES_H

#include "cf_eds_designparameters.h"

#define CF_INTERFACE_CFGVAL(x) EdsParam_CF_##x

#endif