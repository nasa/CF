/************************************************************************
** File: cf_tbldefs.h
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
**  The CF Application Table Definitions header file
**
**
**
*************************************************************************/

#ifndef CF_TBLDEFS__H
#define CF_TBLDEFS__H

#include "cf_platform_cfg.h"
#include "cf_cfdp_pdu.h"

typedef struct
{
    uint32_t interval_sec;                         /* number of seconds to wait before trying a new directory.
                                                      Must be >0 or slot is inactive. */
    uint8           priority;                      /* priority to use when placing transactions on the pending queue */
    CF_CFDP_Class_t cfdp_class;                    /* the CFDP class to send */
    CF_EntityId_t   dest_eid;                      /* destination entity id */
    char            src_dir[CF_FILENAME_MAX_PATH]; /* path to source dir */
    char            dst_dir[CF_FILENAME_MAX_PATH]; /* path to destination dir */
    uint8           enabled;
} polldir_t;

typedef struct
{
    uint32 max_outgoing_messages_per_wakeup; /* max number of messages to send per wakeup (0 - unlimited) */
    uint32 rx_max_messages_per_wakeup;       /* max number of rx messages to process per wakeup */

    uint16 apid_input;  /* apid for incoming messages */
    uint16 apid_output; /* apid for outgoing messages */

    uint16 pipe_depth_input; /* depth of pipe to receive incoming pdu */

    polldir_t polldir[CF_MAX_POLLING_DIR_PER_CHAN];

    char  sem_name[OS_MAX_API_NAME]; /* name of throttling semaphore in TO */
    uint8 dequeue_enabled;           /* if 1, then the channel will make pending transactions active */
} cf_channel_t;

typedef struct
{
    uint32 ticks_per_second;             /* expected ticks per second to cfdp app */
    uint32 rx_crc_calc_bytes_per_wakeup; /* max number of bytes per wakeup to calculate r2 crc for recvd file (must by
                                            1024-byte aligned */

    CF_EntityId_t local_eid; /* the local entity ID of the CF app */

    cf_channel_t chan[CF_NUM_CHANNELS];
    uint32       ack_timer_s;        /* in seconds */
    uint32       nak_timer_s;        /* in seconds */
    uint32       inactivity_timer_s; /* in seconds */

    uint8 ack_limit; /* number of times to retry ACK (for ex, send fin and wait for fin-ack) */
    uint8 nak_limit; /* number of times to retry NAK before giving up (resets on a single response */

    uint16 outgoing_file_chunk_size;      /* size of outgoing file data PDUs */
    char   tmp_dir[CF_FILENAME_MAX_PATH]; /* temp directory to put temp files */
} cf_config_table_t;

#endif /* !CF_TBLDEFS__H */
