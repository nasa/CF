/************************************************************************
** File: cf_def_config.c
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
**  The CF Application default configuration table
**
**
**
*************************************************************************/

#include "cfe.h"
#include "cfe_tbl_filedef.h"
#include "cf_tbldefs.h"

CF_ConfigTable_t CF_config_table = {
    10,    /* ticks_per_second */
    16384, /* max number of bytes per wakeup to calculate r2 recv file crc */
    25,    /* temp local id */
    {{
         5, /* max number of outgoing messages per wakeup */
         5, /* max number of rx messages per wakeup */
         0x18c8,
         0x08c2,
         16,
         {{5, 25, CF_CFDP_CLASS_2, 23, "/cf/poll_dir", "./poll_dir", 0}, {0}, {0}, {0}, {0}},
         "", /* throttle sem for channel 1, empty string means no throttle */
         1,
     },
     {5, /* max number of outgoing messages per wakeup */
      5, /* max number of rx messages per wakeup */
      0x18c9,
      0x08c3,
      16,
      {{0}, {0}, {0}, {0}, {0}},
      "", /* throttle sem for channel 2, empty string means no throttle */
      1}},
    3,   /* ack timer */
    3,   /* nak timer */
    30,  /* inactivity timer */
    4,   /* ack limit */
    4,   /* nak limit */
    480, /* outgoing_file_chunk_size */
    "/cf/tmp",
};
CFE_TBL_FILEDEF(CF_config_table, CF_APP.config_table, CF config table, cf_def_config.tbl)
