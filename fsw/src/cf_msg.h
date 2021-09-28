/************************************************************************
** File: cf_msg.h
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
**  The CF Application message definitions header file
**
** 
** 
*************************************************************************/

#ifndef _CF_MSG_H_
#define _CF_MSG_H_

#include "cfe.h"
#include "cf_platform_cfg.h"
#include "cf_cfdp.h"
#include "cf_tbldefs.h"

typedef struct {
    uint16 cmd;
    uint16 err;
} CF_PACK hk_cmd_counters_t;

typedef struct {
    uint64 file_data_bytes;
    uint32 pdu;
    uint32 nak_segment_requests;
} CF_PACK hk_sent_t;

typedef struct {
    uint64 file_data_bytes;
    uint32 pdu;
    uint32 error; /* generic pdu error */
    uint16 spurious;
    uint16 dropped;
    uint32 nak_segment_requests;
} CF_PACK hk_recv_t;

typedef struct {
    uint16 file_open;
    uint16 file_read;
    uint16 file_seek;
    uint16 file_write;
    uint16 file_rename;
    uint16 directory_read;
    uint16 crc_mismatch;
    uint16 file_size_mismatch;
    uint16 nak_limit;
    uint16 ack_limit;
    uint16 inactivity_timer;
    uint16 spare;
} CF_PACK hk_fault_t;

typedef struct {
    hk_sent_t sent;
    hk_recv_t recv;
    hk_fault_t fault;
} hk_cfdp_counters_t;

typedef struct {
    hk_cfdp_counters_t counters;
    uint16 q_size[CF_Q_NUM];
    uint8 poll_counter;
    uint8 playback_counter;
    uint8 frozen; /* NOTE: this could be more than one flag if we ever need it */
    uint16 spare3;
    uint8 spare4;
} CF_PACK hk_channel_data_t;

typedef struct {
    CFE_MSG_TelemetryHeader_t tlm_header;

    /* app HK */
    hk_cmd_counters_t counters;

    /* per-channel HK */
    hk_channel_data_t channel_hk[CF_NUM_CHANNELS];
} cf_hk_packet_t;

/* used with CF_SEND_CFG_PARAMS_CC */
typedef struct {
    CFE_MSG_TelemetryHeader_t tlm_header;

    uint32 ticks_per_second; /* expected ticks per second to cfdp app */
    uint32 rx_crc_calc_bytes_per_wakeup; /* max number of bytes per wakeup to calculate r2 crc for recvd file (must by 1024-byte aligned */

    uint32 ack_timer_s; /* in seconds */
    uint32 nak_timer_s; /* in seconds */
    uint32 inactivity_timer_s; /* in seconds */

    uint16 outgoing_file_chunk_size;
    uint8 ack_limit; /* number of times to retry ACK (for ex, send fin and wait for fin-ack) */
    uint8 nak_limit; /* number of times to retry NAK before giving up (resets on a single response */

    cf_entity_id_t local_eid;
/* must #define the number of data items in this struct for command processing */
#define CF_NUM_CFG_PACKET_ITEMS 10
} CF_PACK cf_cfg_packet_t;

/****************************************
** CF app command packet command codes
****************************************/

/* NOTE: these are what was in the original app (may have slightly different names)
 * Not sure that we need to implement all these for cf 3.0 */

/* ANOTHER NOTE: not all of these are used, but they are here legacy from the old app */
typedef enum {
    CF_NOOP_CC                          = 0,
    CF_RESET_CC                         = 1,
    CF_TX_FILE_CC                       = 2,
    CF_PLAYBACK_DIR_CC                  = 3,
    CF_FREEZE_CC                        = 4,
    CF_THAW_CC                          = 5,
    CF_SUSPEND_CC                       = 6,
    CF_RESUME_CC                        = 7,
    CF_CANCEL_CC                        = 8,
    CF_ABANDON_CC                       = 9,
    CF_SET_MIB_PARAM_CC                 = 10,
    CF_GET_MIB_PARAM_CC                 = 11,
    CF_SEND_CFG_PARAMS_CC               = 14,
    CF_WRITE_QUEUE_CC                   = 15,

    /* NOTE: ENABLE/DISABLE commands should be a single command code, but legacy has them separate (ugh) */
    CF_ENABLE_DEQUEUE_CC                = 16,
    CF_DISABLE_DEQUEUE_CC               = 17,
    CF_ENABLE_DIR_POLLING_CC            = 18,
    CF_DISABLE_DIR_POLLING_CC           = 19,
    CF_DELETE_QUEUE_NODE_CC             = 20,
    CF_ENABLE_ENGINE_CC                 = 22,
    CF_DISABLE_ENGINE_CC                = 23,
    CF_NUM_COMMANDS                     = 24,
} CF_CMDS;

/****************************
**  CF Command Formats     **
*****************************/
typedef struct
{
    CFE_MSG_CommandHeader_t cmd_header;
} CF_PACK cf_cmd_noargs_t;

/* unionargs -
 *
 * A lot of commands have a single byte arg, and the old application had 3 spare bytes after that.
 *
 * So this structure gives the flexibility of a single byte arg, 2 halfwords, or a dword, all in one declaration. */
typedef union {
    uint32 dword;
    uint16 hword[2];
    uint8 byte[4];
} CF_PACK cf_cmd_unionargs_data_t;

typedef struct {
    CFE_MSG_CommandHeader_t cmd_header; 
    cf_cmd_unionargs_data_t data;
} CF_PACK cf_cmd_unionargs_t;

typedef struct {
    CFE_MSG_CommandHeader_t cmd_header;
    uint8 key;
    uint8 chan_num;
} CF_PACK cf_cmd_get_param_args_t;

typedef struct {
    CFE_MSG_CommandHeader_t cmd_header;
    uint32 value;
    uint8 key;
    uint8 chan_num;
} CF_PACK cf_cmd_set_param_args_t;

typedef struct {
    CFE_MSG_CommandHeader_t cmd_header;
    uint8 cfdp_class; /* 0=class 1, 1=class 2 */
    uint8 keep; /* if 1, then keep the file -- otherwise delete */
    uint8 chan_num;
    uint8 priority;
    cf_entity_id_t dest_id;
    char src_filename[CF_FILENAME_MAX_LEN];
    char dst_filename[CF_FILENAME_MAX_LEN];
} CF_PACK cf_cmd_tx_file_t;

typedef struct {
    CFE_MSG_CommandHeader_t cmd_header;
    uint8 type; /* all=0, up=1, down=2 */
    uint8 chan;
    uint8 queue; /* 0=pending, 1=active, 2=history, 3=all */
    uint8 spare; /* why? */
    char filename[CF_FILENAME_MAX_LEN];
} CF_PACK cf_cmd_write_q_t;

typedef cf_cmd_tx_file_t cf_cmd_playback_dir_t;

typedef struct {
    CFE_MSG_CommandHeader_t cmd_header;
    cf_transaction_seq_t ts;
    cf_entity_id_t eid;
    uint8 chan; /* if 254, use ts. if 255, all channels */
} CF_PACK cf_cmd_transaction_t;

#endif /* !_CF_MSG_H_ */


