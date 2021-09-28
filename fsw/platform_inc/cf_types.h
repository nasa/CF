/************************************************************************
** File: cf_types.h
**
** NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF) 
** Application version 3.0.0”
** Copyright © 2019 United States Government as represented by the 
** Administrator of the National Aeronautics and Space Administration. 
** All Rights Reserved.
** Licensed under the Apache License, Version 2.0 (the "License"); you may 
** not use this file except in compliance with the License. You may obtain 
** a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software 
** distributed under the License is distributed on an "AS IS" BASIS, 
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
** See the License for the specific language governing permissions and 
** limitations under the License.
** 
**
** Purpose:
**  The CF Application Types header file
**
** 
** 
*************************************************************************/

#ifndef CF_TYPES__H
#define CF_TYPES__H

#include "cfe.h"

typedef enum fault_handler_t {
    FH_ISSUE_NOTICE_OF_CANCELLATION,
    FH_ISSUE_NOTICE_OF_SUSPENSION,
    FH_NO_ACTION,
    FH_ABANDON,
} fault_handler_t;

typedef enum local_entity_flags_t {
    LE_SENT_INDICATION_REQUIRED = (1<<0),
    LE_RECV_INDICATION_REQUIRED = (1<<1),
    LE_FILE_SEGMENT_RECV_INDICATION_REQUIRED = (1<<2),
    LE_TRANSACTION_FINISHED_INDICATION_REQUIRED = (1<<3),
    LE_SUSPENDED_INDICATION_REQUIRED = (1<<4),
    LE_RESUMED_INDICATION_REQUIRED = (1<<5),
} local_entity_flags_t;

#endif /* !CF_TYPES__H */

