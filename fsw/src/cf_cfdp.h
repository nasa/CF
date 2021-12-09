/************************************************************************
** File: cf_cfdp.h
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
**  The CF Application cfdp engine and packet parsing header file
**
**
**
*************************************************************************/

#ifndef CF_CFDP__H
#define CF_CFDP__H

#include "cfe.h"
#include "cf_cfdp_pdu.h"
#include "cf_crc.h"
#include "cf_timer.h"
#include "cf_clist.h"
#include "cf_chunk.h"

#define CF_NUM_TRANSACTIONS_PER_CHANNEL                                                \
    (CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN + CF_MAX_SIMULTANEOUS_RX +               \
     ((CF_MAX_POLLING_DIR_PER_CHAN + CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN) * \
      CF_NUM_TRANSACTIONS_PER_PLAYBACK))
#define CF_NUM_TRANSACTIONS (CF_NUM_CHANNELS * CF_NUM_TRANSACTIONS_PER_CHANNEL)

#define CF_NUM_HISTORIES (CF_NUM_CHANNELS * CF_NUM_HISTORIES_PER_CHANNEL)

#define CF_NUM_CHUNKS_ALL_CHANNELS (CF_TOTAL_CHUNKS * CF_NUM_TRANSACTIONS_PER_CHANNEL)

typedef enum
{
    CFDP_IDLE    = 0,
    CFDP_R1      = 1,
    CFDP_S1      = 2,
    CFDP_R2      = 3,
    CFDP_S2      = 4,
    CFDP_DROP    = 5, /* class 1 received file data without metadata, no file info, so drop */
    CFDP_INVALID = 6
} cfdp_state_t;

typedef enum
{
    SEND_METADATA         = 0,
    SEND_FILEDATA         = 1,
    SEND_EOF              = 2,
    SEND_WAIT_FOR_EOF_ACK = 3,
    SEND_WAIT_FOR_FIN     = 4,
    SEND_SEND_FIN_ACK     = 5,
    SEND_NUM_STATES       = 6
} tx_sub_state;

typedef enum
{
    RECV_FILEDATA         = 0,
    RECV_EOF              = 1,
    RECV_WAIT_FOR_FIN_ACK = 2,
    RECV_NUM_STATES       = 3,
} rx_sub_state;

typedef enum
{
    R_EOF_SUCCESS        = 0,
    R_EOF_FSIZE_MISMATCH = 1,
    R_EOF_BAD_EOF        = 2,
    R_EOF_INVALID        = 3,
} r_eof_ret_t;

typedef struct
{
    char src_filename[CF_FILENAME_MAX_LEN];
    char dst_filename[CF_FILENAME_MAX_LEN];
} transaction_fnames_t;

typedef enum direction_t
{
    CF_DIR_RX  = 0,
    CF_DIR_TX  = 1,
    CF_DIR_NUM = 2,
} direction_t;

typedef struct
{
    transaction_fnames_t fnames;
    clist_node_t         cl_node;
    direction_t          dir;
    condition_code_t     cc;
    cf_entity_id_t       src_eid;  /* src_eid is always the source eid */
    cf_entity_id_t       peer_eid; /* peer_eid is always the "other guy", which is the same src_eid for RX */
    cf_transaction_seq_t seq_num;  /* stays constant for entire transfer */
} history_t;

typedef struct
{
    chunks_t     chunks;
    clist_node_t cl_node;
} chunks_wrapper_t;

struct transaction_t;

typedef struct
{
    uint32               dir_id;
    cfdp_class_t         cfdp_class;
    transaction_fnames_t fnames;
    uint16               num_ts; /* number of transactions -- 16 bit should be enough */
    uint8                priority;
    cf_entity_id_t       dest_id;

    bool busy;
    bool diropen;
    bool keep;
    bool counted;
} playback_t;

typedef struct
{
    playback_t pb;
    cf_timer_t interval_timer;
    bool       timer_set;
} poll_t;

typedef union
{
    unsigned ack;
    uint8    nak;
} rxtx_counters_t;

typedef struct
{
    uint8           fin_cc; /* remember the cc in the received fin pdu to echo in eof-fin */
    rxtx_counters_t counter;
} tx_s2_data_t;

typedef struct
{
    tx_sub_state sub_state;
    uint32       cached_pos;

    tx_s2_data_t s2;
} tx_state_data_t;

typedef struct
{
    uint32              eof_crc;
    uint32              eof_size;
    uint32              rx_crc_calc_bytes;
    fin_delivery_code_t dc;
    fin_file_status_t   fs;
    uint8               eof_cc; /* remember the cc in the received eof pdu to echo in eof-ack */
    rxtx_counters_t     counter;
} rx_r2_data_t;

typedef struct
{
    rx_sub_state sub_state;
    uint32       cached_pos;

    rx_r2_data_t r2;
} rx_state_data_t;

typedef struct
{
    uint8 q_index; /* which Q is this in? */
    bool  ack_timer_armed;
    bool  suspended;
    bool  canceled;
    bool  crc_calc;
} flags_all_t;

typedef struct
{
    flags_all_t com;

    bool md_recv; /* md received for r state */
    bool eof_recv;
    bool send_nak;
    bool send_fin;
    bool send_ack;
    bool inactivity_fired; /* used for r2 */
    bool complete;         /* r2 */
    bool fd_nak_sent;      /* latches that at least one nak has been sent for file data */
} flags_rx_t;

typedef struct
{
    flags_all_t com;

    bool md_need_send;
    bool cmd_tx; /* indicates transaction is commanded (ground) tx */
} flags_tx_t;

typedef union
{
    flags_all_t com;
    flags_rx_t  rx;
    flags_tx_t  tx;
} state_flags_t;

typedef union
{
    tx_state_data_t s;
    rx_state_data_t r;
} state_data_t;

typedef struct transaction_t
{
    cfdp_state_t state; /* each engine is commanded to do something, which is the overall state */

    history_t        *history;          /* weird, but this also holds active filenames and possibly other info */
    chunks_wrapper_t *chunks;           /* for gap tracking, only used on class 2 */
    cf_timer_t        inactivity_timer; /* set to the overall inactivity timer of a remote */
    cf_timer_t        ack_timer;        /* called ack_timer, but is also nak_timer */

    uint32    fsize; /* lseek() should be 64-bit on 64-bit system, but osal limits to 32-bit */
    uint32    foffs; /* offset into file for next read */
    osal_id_t fd;

    cf_crc_t crc;

    uint8 keep;
    uint8 chan_num; /* if ever more than one engine, this may need to change to pointer */
    uint8 priority;

    clist_node_t cl_node;

    playback_t *p; /* NULL if transaction does not belong to a playback */

    state_data_t state_data;

    /* NOTE: the flags here look a little strange, because there are different flags for TX and RX.
     * Both types share the same type of flag, though. Since RX flags plus the global flags is
     * over one byte, storing them this way allows 2 bytes to cover all possible flags.
     * Please ignore the duplicate declarations of the "all" flags. :) */
    state_flags_t flags;

} transaction_t;

typedef enum cf_queue_index_t
{
    CF_Q_PEND      = 0, /* first one on this list is active */
    CF_Q_TXA       = 1,
    CF_Q_TXW       = 2,
    CF_Q_RX        = 3,
    CF_Q_HIST      = 4,
    CF_Q_HIST_FREE = 5,
    CF_Q_FREE      = 6,
    CF_Q_NUM       = 7,
} cf_queue_index_t;

typedef enum
{
    CF_SEND_SUCCESS = 0,
    CF_SEND_NO_MSG  = 1,
    CF_SEND_ERROR   = 2, /* the send itself failed */
    CF_SEND_FAILURE = 3, /* generic failure message not relating to message send */
} cfdp_send_ret_t;

typedef enum
{
    CF_TICK_RX,
    CF_TICK_TXW_NORM,
    CF_TICK_TXW_NAK,
    CF_TICK_NUM_TYPES
} tick_type_t;

typedef struct
{
    clist_node qs[CF_Q_NUM];
    clist_node cs[CF_DIR_NUM];

    CFE_SB_PipeId_t pipe;

    uint32 num_cmd_tx;

    playback_t playback[CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN];

    /* For polling directories, the configuration data is in a table. */
    poll_t poll[CF_MAX_POLLING_DIR_PER_CHAN];

    uint32 sem_id; /* semaphore id for output pipe */

    const transaction_t *cur; /* current transaction during channel cycle */

    uint8 tick_type;
} channel_t;

typedef struct
{
    CFE_SB_Buffer_t *msg;
} out_t;

typedef struct
{
    CFE_SB_Buffer_t     *msg;
    CFE_MSG_Size_t       bytes_received;
    cf_entity_id_t       src;
    cf_entity_id_t       dst;
    cf_transaction_seq_t tsn;
} in_t;

/* An engine represents a pairing to a local EID
 *
 * Each engine can have at most CF_MAX_SIMULTANEOUS_TRANSACTIONS */
typedef struct
{
    cf_transaction_seq_t seq_num; /* keep track of the next sequence number to use for sends */

    out_t out;
    in_t  in;

    /* NOTE: could have separate array of transactions as part of channel? */
    transaction_t transactions[CF_NUM_TRANSACTIONS];
    history_t     histories[CF_NUM_HISTORIES];
    channel_t     channels[CF_NUM_CHANNELS];

    chunks_wrapper_t chunks[CF_NUM_TRANSACTIONS * CF_DIR_NUM];
    chunk_t          chunk_mem[CF_NUM_CHUNKS_ALL_CHANNELS];

    uint32 outgoing_counter;
    uint8  enabled;
} cfdp_engine_t;

/* NOTE: functions grouped together on contiguous lines are in groups that are described by
 * a simple comment at the top. Other comments below that apply to the whole group. */
/* reset functions */
extern void CF_CFDP_ResetTransaction(transaction_t *t, int keep_history);
extern void CF_CFDP_ResetHistory(channel_t *c, history_t *t);

/* engine execution functions */
extern int32 CF_CFDP_InitEngine(void);
extern void  CF_CFDP_CycleEngine(void);
extern void  CF_CFDP_DisableEngine(void);

/* ground commands into the engine */
/* returns NULL on err */
extern int32 CF_CFDP_TxFile(const char src_filename[CF_FILENAME_MAX_LEN], const char dst_filename[CF_FILENAME_MAX_LEN],
                            cfdp_class_t cfdp_class, uint8 keep, uint8 chan, uint8 priority, cf_entity_id_t dest_id);
extern int32 CF_CFDP_PlaybackDir(const char src_filename[CF_FILENAME_MAX_LEN],
                                 const char dst_filename[CF_FILENAME_MAX_LEN], cfdp_class_t cfdp_class, uint8 keep,
                                 uint8 chan, uint8 priority, uint16 dest_id);

/* PDU send functions */
/* CF_CFDP_ConstructPduHeader sets length of 0. Must set it after building packet */
extern pdu_header_t   *CF_CFDP_ConstructPduHeader(const transaction_t *t, uint8 directive_code, cf_entity_id_t src_eid,
                                                  cf_entity_id_t dst_eid, uint8 towards_sender, cf_transaction_seq_t tsn,
                                                  int silent);
extern cfdp_send_ret_t CF_CFDP_SendMd(transaction_t *t);
extern cfdp_send_ret_t CF_CFDP_SendFd(transaction_t *t, uint32 offset, int len);

extern cfdp_send_ret_t CF_CFDP_SendEof(transaction_t *t);
/* NOTE: CF_CFDP_SendAck() takes a cf_transaction_seq_t instead of getting it from transaction history because
 * of the special case where a FIN-ACK must be sent for an unknown transaction. It's better for
 * long term maintenance to not build an incomplete history_t for it.
 */
extern cfdp_send_ret_t CF_CFDP_SendAck(transaction_t *t, ack_transaction_status_t ts, file_directive_t dir_code,
                                       condition_code_t cc, cf_entity_id_t peer_eid, cf_transaction_seq_t tsn);
extern cfdp_send_ret_t CF_CFDP_SendFin(transaction_t *t, fin_delivery_code_t dc, fin_file_status_t fs,
                                       condition_code_t cc);
extern cfdp_send_ret_t CF_CFDP_SendNak(transaction_t *t, int num_segment_requests);

/* PDU receive functions */
/* returns 0 on success */
extern int CF_CFDP_RecvMd(transaction_t *t);
extern int CF_CFDP_RecvFd(transaction_t *t);
extern int CF_CFDP_RecvEof(void);
extern int CF_CFDP_RecvAck(void);
extern int CF_CFDP_RecvFin(void);
extern int CF_CFDP_RecvNak(int *num_segment_requests);

/* Engine functional dispatch. These are all implemented in cf_cfdp_r.c or cf_cfdp_s.c */
extern void CF_CFDP_S1_Recv(transaction_t *t);
extern void CF_CFDP_R1_Recv(transaction_t *t);
extern void CF_CFDP_S2_Recv(transaction_t *t);
extern void CF_CFDP_R2_Recv(transaction_t *t);
extern void CF_CFDP_S1_Tx(transaction_t *t);
extern void CF_CFDP_S2_Tx(transaction_t *t);
extern void CF_CFDP_R_Tick(transaction_t *t, int *cont);
extern void CF_CFDP_S_Tick(transaction_t *t, int *cont);
extern void CF_CFDP_S_Tick_Nak(transaction_t *t, int *cont);
extern void CF_CFDP_S_Cancel(transaction_t *t);
extern void CF_CFDP_R_Cancel(transaction_t *t);
extern void CF_CFDP_R_Init(transaction_t *t);

extern void CF_CFDP_CancelTransaction(transaction_t *t);

extern pdu_header_t *CF_CFDP_MsgOutGet(const transaction_t *t, int silent);

/* functions to handle LVs (length-value, cfdp spec) */
/* returns number of bytes copied, or -1 on error */
extern int CF_CFDP_CopyDataToLv(lv_t *dest_lv, const uint8 *data, uint32 len);
extern int CF_CFDP_CopyDataFromLv(uint8 buf[CF_FILENAME_MAX_LEN], const lv_t *dest_lv);

extern const int CF_max_chunks[CF_DIR_NUM][CF_NUM_CHANNELS];

extern void CF_CFDP_ArmAckTimer(transaction_t *);

extern transaction_t *CF_CFDP_FindTransactionBySequenceNumber(channel_t *c, uint32 transaction_sequence_number,
                                                              cf_entity_id_t src_eid);

#endif /* !CF_CFDP__H */
