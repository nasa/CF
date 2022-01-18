/************************************************************************
 *
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 * Copyright © 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************/

/**
 * @file
 *
 *  The CF Application message definitions header file
 */

#ifndef CF_MSG_H
#define CF_MSG_H

#include "cfe.h"
#include "cf_platform_cfg.h"
#include "cf_cfdp.h"
#include "cf_tbldefs.h"

typedef struct CF_HkCmdCounters
{
    uint16 cmd;
    uint16 err;
} CF_HkCmdCounters_t;

typedef struct CF_HkSent
{
    uint64 file_data_bytes;
    uint32 pdu;
    uint32 nak_segment_requests;
} CF_HkSent_t;

typedef struct CF_HkRecv
{
    uint64 file_data_bytes;
    uint32 pdu;
    uint32 error; /* generic pdu error */
    uint16 spurious;
    uint16 dropped;
    uint32 nak_segment_requests;
} CF_HkRecv_t;

typedef struct CF_HkFault
{
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
} CF_HkFault_t;

typedef struct CF_HkCounters
{
    CF_HkSent_t  sent;
    CF_HkRecv_t  recv;
    CF_HkFault_t fault;
} CF_HkCounters_t;

typedef struct CF_HkChannel_Data
{
    CF_HkCounters_t counters;
    uint16          q_size[CF_QueueIdx_NUM];
    uint8           poll_counter;
    uint8           playback_counter;
    uint8           frozen;   /* NOTE: this could be more than one flag if we ever need it */
    uint8           spare[7]; /* Structure has uint64 values in the counters, so size should be multiple of 8 */
} CF_HkChannel_Data_t;

typedef struct CF_HkPacket
{
    CFE_MSG_TelemetryHeader_t tlm_header;

    /* app HK */
    CF_HkCmdCounters_t counters;

    uint8 spare[4]; /* CF_HkCmdCounters_t is 4 bytes, and CF_HkChannel_Data_t uses uint64 values  */

    /* per-channel HK */
    CF_HkChannel_Data_t channel_hk[CF_NUM_CHANNELS];
} CF_HkPacket_t;

/* used with CF_SendRet_CFG_PARAMS_CC */
typedef struct CF_ConfigPacket
{
    CFE_MSG_TelemetryHeader_t tlm_header;

    uint32 ticks_per_second;             /* expected ticks per second to cfdp app */
    uint32 rx_crc_calc_bytes_per_wakeup; /* max number of bytes per wakeup to calculate r2 crc for recvd file (must by
                                            1024-byte aligned */

    uint32 ack_timer_s;        /* in seconds */
    uint32 nak_timer_s;        /* in seconds */
    uint32 inactivity_timer_s; /* in seconds */

    uint16 outgoing_file_chunk_size;
    uint8  ack_limit; /* number of times to retry ACK (for ex, send fin and wait for fin-ack) */
    uint8  nak_limit; /* number of times to retry NAK before giving up (resets on a single response */

    CF_EntityId_t local_eid;

} CF_ConfigPacket_t;

/****************************************
 * CF app command packet command codes
 ***************************************/

/* NOTE: these are what was in the original app (may have slightly different names)
 * Not sure that we need to implement all these for cf 3.0 */

/* ANOTHER NOTE: not all of these are used, but they are here legacy from the old app */
typedef enum
{
    CF_NOOP_CC               = 0,
    CF_RESET_CC              = 1,
    CF_TX_FILE_CC            = 2,
    CF_PLAYBACK_DIR_CC       = 3,
    CF_FREEZE_CC             = 4,
    CF_THAW_CC               = 5,
    CF_SUSPEND_CC            = 6,
    CF_RESUME_CC             = 7,
    CF_CANCEL_CC             = 8,
    CF_ABANDON_CC            = 9,
    CF_SET_MIB_PARAM_CC      = 10,
    CF_GET_MIB_PARAM_CC      = 11,
    CF_SendRet_CFG_PARAMS_CC = 14,
    CF_WRITE_QUEUE_CC        = 15,

    /* NOTE: ENABLE/DISABLE commands should be a single command code, but legacy has them separate (ugh) */
    CF_ENABLE_DEQUEUE_CC      = 16,
    CF_DISABLE_DEQUEUE_CC     = 17,
    CF_ENABLE_DIR_POLLING_CC  = 18,
    CF_DISABLE_DIR_POLLING_CC = 19,
    CF_DELETE_QUEUE_NODE_CC   = 20,
    CF_PURGE_QUEUE_CC         = 21,
    CF_ENABLE_ENGINE_CC       = 22,
    CF_DISABLE_ENGINE_CC      = 23,
    CF_NUM_COMMANDS           = 24,
} CF_CMDS;

/****************************
 *  CF Command Formats     **
 ****************************/
typedef struct CF_NoArgsCmd
{
    CFE_MSG_CommandHeader_t cmd_header;
} CF_NoArgsCmd_t;

/* unionargs -
 *
 * A lot of commands have a single byte arg, and the old application had 3 spare bytes after that.
 *
 * So this structure gives the flexibility of a single byte arg, 2 halfwords, or a dword, all in one declaration. */
typedef union CF_UnionArgs_Payload
{
    uint32 dword;
    uint16 hword[2];
    uint8  byte[4];
} CF_UnionArgs_Payload_t;

typedef struct
{
    CFE_MSG_CommandHeader_t cmd_header;
    CF_UnionArgs_Payload_t  data;
} CF_UnionArgsCmd_t;

/**
 * @brief Parameter IDs for use with Get/Set param messages
 *
 * Specifically these are used for the "key" field within CF_GetParamCmd_t and
 * CF_SetParamCmd_t message structures.
 */
typedef enum
{
    CF_GetSet_ValueID_ticks_per_second,
    CF_GetSet_ValueID_rx_crc_calc_bytes_per_wakeup,
    CF_GetSet_ValueID_ack_timer_s,
    CF_GetSet_ValueID_nak_timer_s,
    CF_GetSet_ValueID_inactivity_timer_s,
    CF_GetSet_ValueID_outgoing_file_chunk_size,
    CF_GetSet_ValueID_ack_limit,
    CF_GetSet_ValueID_nak_limit,
    CF_GetSet_ValueID_local_eid,
    CF_GetSet_ValueID_chan_max_outgoing_messages_per_wakeup,
    CF_GetSet_ValueID_MAX
} CF_GetSet_ValueID_t;

typedef struct CF_GetParamCmd
{
    CFE_MSG_CommandHeader_t cmd_header;
    uint8                   key;
    uint8                   chan_num;
} CF_GetParamCmd_t;

typedef struct CF_SetParamCmd
{
    CFE_MSG_CommandHeader_t cmd_header;
    uint32                  value;
    uint8                   key;
    uint8                   chan_num;
    uint8                   spare[2]; /* Required to make the size a multiple of uint32 */
} CF_SetParamCmd_t;

typedef struct CF_TxFileCmd
{
    CFE_MSG_CommandHeader_t cmd_header;
    uint8                   cfdp_class; /* 0=class 1, 1=class 2 */
    uint8                   keep;       /* if 1, then keep the file -- otherwise delete */
    uint8                   chan_num;
    uint8                   priority;
    CF_EntityId_t           dest_id;
    char                    src_filename[CF_FILENAME_MAX_LEN];
    char                    dst_filename[CF_FILENAME_MAX_LEN];
} CF_TxFileCmd_t;

typedef struct CF_WriteQueueCmd
{
    CFE_MSG_CommandHeader_t cmd_header;
    uint8                   type; /* all=0, up=1, down=2 */
    uint8                   chan;
    uint8                   queue; /* 0=pending, 1=active, 2=history, 3=all */
    uint8                   spare; /* why? */
    char                    filename[CF_FILENAME_MAX_LEN];
} CF_WriteQueueCmd_t;

typedef CF_TxFileCmd_t CF_PlaybackDirCmd_t;

typedef struct CF_TransactionCmd
{
    CFE_MSG_CommandHeader_t cmd_header;
    CF_TransactionSeq_t     ts;
    CF_EntityId_t           eid;
    uint8                   chan;     /* if 254, use ts. if 255, all channels */
    uint8                   spare[3]; /* To make structure a multiple of uint32 */
} CF_TransactionCmd_t;

#endif /* !CF_MSG_H */
