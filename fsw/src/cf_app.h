/************************************************************************
** File: cf_app.h
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
**  The CF Application main application header file
**
*************************************************************************/

#ifndef CF_APP__H
#define CF_APP__H

#include "cfe.h"

#include "cf_msg.h"
#include "cf_msgids.h"
#include "cf_tbldefs.h"
#include "cf_platform_cfg.h"
#include "cf_cfdp.h"
#include "cf_clist.h"

#define CF_PIPE_NAME           "CF_CMD_PIPE"
#define CF_CHANNEL_PIPE_PREFIX "CF_CHAN_"

typedef struct
{
    CF_HkPacket_t     hk;
    CF_ConfigPacket_t cfg;

    uint32 run_status;

    CFE_SB_PipeId_t cmd_pipe;

    CFE_TBL_Handle_t   config_handle;
    cf_config_table_t *config_table;

    CF_Engine_t engine;
} CF_AppData_t;

extern CF_AppData_t CF_AppData;

extern void CF_ProcessGroundCommand(CFE_SB_Buffer_t *msg);
#endif /* !CF_APP__H */
