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
 *   Specification for the CFS CFDP (CF) table structures
 *
 * Provides default definitions for HK table structures
 *
 * @note This file may be overridden/superceded by mission-provided defintions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CF_TBLSTRUCT_H
#define CF_TBLSTRUCT_H

#include "common_types.h"
#include "cf_mission_cfg.h"
#include "cf_tbldefs.h"

/**
 * \brief Top-level CFDP configuration structure
 */
typedef struct CF_ConfigTable
{
    uint32 ticks_per_second;             /**< \brief expected ticks per second to CFDP app */
    uint32 rx_crc_calc_bytes_per_wakeup; /**< \brief max number of bytes per wakeup to calculate
                                          * r2 CRC for recvd file (must be 1024-byte aligned)
                                          */

    CF_EntityId_t local_eid; /**< \brief the local entity ID of the CF app */

    CF_ChannelConfig_t chan[CF_NUM_CHANNELS]; /**< \brief Channel configuration */

    uint16 outgoing_file_chunk_size;    /**< \brief maximum size of outgoing file data chunk in a PDU.
                                         *   Limited by CF_MAX_PDU_SIZE minus the PDU header(s) */
    char tmp_dir[CF_FILENAME_MAX_PATH]; /**< \brief directory to put temp files */
    char fail_dir[CF_FILENAME_MAX_PATH]; /**< \brief fail directory */
} CF_ConfigTable_t;

#endif
