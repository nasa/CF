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
 * Declarations and prototypes for cf_extern_typedefs module
 */

#ifndef EDS_CF_EXTERN_TYPEDEFS_H
#define EDS_CF_EXTERN_TYPEDEFS_H

#include "common_types.h"
#include "cfe_resourceid_typedef.h"

/* Source the definitions from EDS */
#include "cf_eds_typedefs.h"

/* Define type mappings for CF-specific types */
typedef CF_QueueIdx_Enum_t CF_QueueIdx_t;

#define CF_QueueIdx_NUM       (1 + EdsDataType_EdsEnum_CF_QueueIdx_t_MAX) 
#define CF_GetSet_ValueID_MAX (1 + EdsDataType_EdsEnum_CF_GetSet_ValueID_t_MAX)

typedef CF_EntityId_Atom_t       CF_EntityId_t;
typedef CF_TransactionSeq_Atom_t CF_TransactionSeq_t;

typedef CF_CFDP_Enum_t           CF_CFDP_Class_t;
typedef CF_GetSet_ValueID_Enum_t CF_GetSet_ValueID_t;

typedef EdsDataType_BASE_TYPES_PathName_t CF_PathName_t;
typedef EdsDataType_BASE_TYPES_FileName_t CF_FileName_t;

#endif