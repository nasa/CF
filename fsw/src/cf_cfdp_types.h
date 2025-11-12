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
 * Macros and data types used across the CF application
 *
 * @note Functions should not be declared in this file.  This should
 * be limited to shared macros and data types only.  For unit testing,
 * functions should be declared only in a header file with the same name
 * as the C file that defines that function.
 */

#ifndef CF_CFDP_TYPES_H
#define CF_CFDP_TYPES_H

#include "common_types.h"
#include "cf_cfdp_pdu.h"
#include "cf_extern_typedefs.h"
#include "cf_platform_cfg.h"
#include "cf_msg.h"
#include "cf_clist.h"
#include "cf_chunk.h"
#include "cf_timer.h"
#include "cf_crc.h"
#include "cf_codec.h"

/**
 * @brief Maximum possible number of transactions that may exist on a single CF channel
 */
#define CF_NUM_TRANSACTIONS_PER_CHANNEL                                                \
    (CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN + CF_MAX_SIMULTANEOUS_RX +               \
     ((CF_MAX_POLLING_DIR_PER_CHAN + CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN) * \
      CF_NUM_TRANSACTIONS_PER_PLAYBACK))

/**
 * @brief Maximum possible number of transactions that may exist in the CF application
 */
#define CF_NUM_TRANSACTIONS (CF_NUM_CHANNELS * CF_NUM_TRANSACTIONS_PER_CHANNEL)

/**
 * @brief Maximum possible number of history entries that may exist in the CF application
 */
#define CF_NUM_HISTORIES (CF_NUM_CHANNELS * CF_NUM_HISTORIES_PER_CHANNEL)

/**
 * @brief Maximum possible number of chunk entries that may exist in the CF application
 */
#define CF_NUM_CHUNKS_ALL_CHANNELS (CF_TOTAL_CHUNKS * CF_NUM_TRANSACTIONS_PER_CHANNEL)

/**
 * @brief High-level state of a transaction
 */
typedef enum
{
    CF_TxnState_UNDEF   = 0, /**< \brief State assigned to an unused object on the free list */
    CF_TxnState_INIT    = 1, /**< \brief State assigned to a newly allocated transaction object */
    CF_TxnState_R1      = 2, /**< \brief Receive file as class 1 */
    CF_TxnState_S1      = 3, /**< \brief Send file as class 1 */
    CF_TxnState_R2      = 4, /**< \brief Receive file as class 2 */
    CF_TxnState_S2      = 5, /**< \brief Send file as class 2 */
    CF_TxnState_DROP    = 6, /**< \brief State where all PDUs are dropped */
    CF_TxnState_HOLD    = 7, /**< \brief State assigned to a transaction after freeing it */
    CF_TxnState_INVALID = 8  /**< \brief Marker value for the highest possible state number */
} CF_TxnState_t;

/**
 * @brief Sub-state of a send file transaction
 */
typedef enum
{
    CF_TxSubState_DATA_NORMAL = 0, /**< sending the initial MD directive and file data */
    CF_TxSubState_DATA_EOF    = 1, /**< Sent an EOF, waiting on EOF-ACK and FIN (or NAK) */
    CF_TxSubState_FILESTORE   = 2, /**< Performing file store ops */
    CF_TxSubState_COMPLETE    = 3, /**< Transaction is done */
    CF_TxSubState_NUM_STATES  = 4
} CF_TxSubState_t;

/**
 * @brief Sub-state of a receive file transaction
 */
typedef enum
{
    CF_RxSubState_DATA_NORMAL = 0, /**< waiting for more PDUs, no EOF received yet (normal recv) */
    CF_RxSubState_DATA_EOF    = 1, /**< Got an EOF, filling in remaining gaps (NAKs may be sent) */
    CF_RxSubState_VALIDATE    = 2, /**< Checking the CRC on the complete file */
    CF_RxSubState_FILESTORE   = 3, /**< Performing file store ops */
    CF_RxSubState_FINACK      = 4, /**< pending final fin/fin-ack exchange */
    CF_RxSubState_COMPLETE    = 5, /**< Transaction is done */
    CF_RxSubState_NUM_STATES  = 6
} CF_RxSubState_t;

/**
 * @brief Direction identifier
 *
 * Differentiates between send and receive history entries
 */
typedef enum
{
    CF_Direction_RX  = 0,
    CF_Direction_TX  = 1,
    CF_Direction_NUM = 2,
} CF_Direction_t;

/**
 * @brief Values for Transaction Status code
 *
 * This enum defines the possible values representing the
 * result of a transaction.  This is a superset of the condition codes
 * defined in CCSDS book 727 (condition codes) but with additional
 * values for local conditions that the blue book does not have,
 * such as protocol/state machine or decoding errors.
 *
 * The values here are designed to not overlap with the condition
 * codes defined in the blue book, but can be translated to one
 * of those codes for the purposes of FIN/ACK/EOF PDUs.
 */
typedef enum
{
    /**
     * The undefined status is a placeholder for new transactions before a value is set.
     */
    CF_TxnStatus_UNDEFINED = -1,

    /* Status codes 0-15 share the same values/meanings as the CFDP condition code (CC) */
    CF_TxnStatus_NO_ERROR                  = CF_CFDP_ConditionCode_NO_ERROR,
    CF_TxnStatus_POS_ACK_LIMIT_REACHED     = CF_CFDP_ConditionCode_POS_ACK_LIMIT_REACHED,
    CF_TxnStatus_KEEP_ALIVE_LIMIT_REACHED  = CF_CFDP_ConditionCode_KEEP_ALIVE_LIMIT_REACHED,
    CF_TxnStatus_INVALID_TRANSMISSION_MODE = CF_CFDP_ConditionCode_INVALID_TRANSMISSION_MODE,
    CF_TxnStatus_FILESTORE_REJECTION       = CF_CFDP_ConditionCode_FILESTORE_REJECTION,
    CF_TxnStatus_FILE_CHECKSUM_FAILURE     = CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE,
    CF_TxnStatus_FILE_SIZE_ERROR           = CF_CFDP_ConditionCode_FILE_SIZE_ERROR,
    CF_TxnStatus_NAK_LIMIT_REACHED         = CF_CFDP_ConditionCode_NAK_LIMIT_REACHED,
    CF_TxnStatus_INACTIVITY_DETECTED       = CF_CFDP_ConditionCode_INACTIVITY_DETECTED,
    CF_TxnStatus_INVALID_FILE_STRUCTURE    = CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE,
    CF_TxnStatus_CHECK_LIMIT_REACHED       = CF_CFDP_ConditionCode_CHECK_LIMIT_REACHED,
    CF_TxnStatus_UNSUPPORTED_CHECKSUM_TYPE = CF_CFDP_ConditionCode_UNSUPPORTED_CHECKSUM_TYPE,
    CF_TxnStatus_SUSPEND_REQUEST_RECEIVED  = CF_CFDP_ConditionCode_SUSPEND_REQUEST_RECEIVED,
    CF_TxnStatus_CANCEL_REQUEST_RECEIVED   = CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED,

    /* Additional status codes for items not representable in a CFDP CC, these can be set in
     * transactions that did not make it to the point of sending FIN/EOF. */
    CF_TxnStatus_PROTOCOL_ERROR     = 16,
    CF_TxnStatus_ACK_LIMIT_NO_FIN   = 17,
    CF_TxnStatus_ACK_LIMIT_NO_EOF   = 18,
    CF_TxnStatus_NAK_RESPONSE_ERROR = 19,
    CF_TxnStatus_SEND_EOF_FAILURE   = 20,
    CF_TxnStatus_EARLY_FIN          = 21,
    CF_TxnStatus_READ_FAILURE       = 22, /* reading local file encountered an error */
    CF_TxnStatus_NO_RESOURCE        = 23, /* lack of internal resources */

    /* keep last */
    CF_TxnStatus_MAX = 24
} CF_TxnStatus_t;

/**
 * @brief CF History entry
 *
 * Records CF app operations for future reference
 */
typedef struct CF_History
{
    CF_TxnFilenames_t   fnames;   /**< \brief file names associated with this history entry */
    CF_CListNode_t      cl_node;  /**< \brief for connection to a CList */
    CF_Direction_t      dir;      /**< \brief direction of this history entry */
    CF_TxnStatus_t      txn_stat; /**< \brief final status of operation */
    CF_EntityId_t       src_eid;  /**< \brief the source eid of the transaction */
    CF_EntityId_t       peer_eid; /**< \brief peer_eid is always the "other guy", same src_eid for RX */
    CF_TransactionSeq_t seq_num;  /**< \brief transaction identifier, stays constant for entire transfer */
} CF_History_t;

/**
 * @brief Wrapper around a CF_ChunkList_t object
 *
 * This allows a CF_ChunkList_t to be stored within a CList data storage structure
 */
typedef struct CF_ChunkWrapper
{
    CF_ChunkList_t chunks;
    CF_CListNode_t cl_node;
} CF_ChunkWrapper_t;

/**
 * @brief CF Playback entry
 *
 * Keeps the state of CF playback requests
 */
typedef struct CF_Playback
{
    osal_id_t         dir_id;
    CF_CFDP_Class_t   cfdp_class;
    CF_TxnFilenames_t fnames;
    uint16            num_ts; /**< \brief number of transactions */
    uint8             priority;
    CF_EntityId_t     dest_id;
    char              pending_file[OS_MAX_FILE_NAME];

    bool busy;
    bool diropen;
    bool keep;
    bool counted;
} CF_Playback_t;

/**
 * @brief CF Poll entry
 *
 * Keeps the state of CF directory polling
 */
typedef struct CF_Poll
{
    CF_Playback_t pb;
    CF_Timer_t    interval_timer;
    bool          timer_set;
} CF_Poll_t;

/**
 * @brief Data that applies to all types of transactions
 */
typedef struct CF_Flags_Common
{
    uint8 q_index; /**< \brief Q index this is in */

    bool close_req; /**< Indicates if a FIN should be used in class 1 (optional) */
    bool ack_timer_armed;
    bool suspended;
    bool canceled;
    bool is_complete;      /**< Latches that all expected PDUs (MD + all FD + EOF) are processed */
    bool crc_complete;     /**< Latches that the CRC computation is completed */
    bool inactivity_fired; /**< \brief set whenever the inactivity timeout expires */
    bool keep_history;     /**< \brief whether history should be preserved during recycle */
} CF_Flags_Common_t;

/**
 * @brief Flags that apply to receive transactions
 */
typedef struct CF_Flags_Rx
{
    CF_Flags_Common_t com;

    bool tempfile_created; /**< Latches that the tempfile was created at txn start */

    bool  md_recv;       /**< Latches that the MD PDU is received */
    uint8 eof_count;     /**< Count of EOF PDUs received  */
    uint8 eof_ack_count; /**< Count of EOF-ACKs sent to peer */
    bool  finack_recv;   /**< Latches that the fin-ack PDU is received */

    bool send_nak; /**< Indicates need to send NAK to peer */
    bool send_fin; /**< Indicates need to send FIN to peer */
} CF_Flags_Rx_t;

/**
 * @brief Flags that apply to send transactions
 */
typedef struct CF_Flags_Tx
{
    CF_Flags_Common_t com;

    bool cmd_tx; /**< \brief indicates transaction is commanded (ground) tx */

    bool  fd_nak_pending; /**< Peer sent a NAK on file data */
    bool  eof_ack_recv;   /**< Latches that the EOF-ACK was received */
    uint8 fin_count;      /**< Count of FIN PDUs received  */
    uint8 fin_ack_count;  /**< Count of FIN-ACKs sent to peer */

    bool send_md;  /**< Indicates need to send MD to peer */
    bool send_eof; /**< Indicates need to send EOF to peer */

} CF_Flags_Tx_t;

/**
 * @brief Summary of all possible transaction flags (tx and rx)
 */
typedef union CF_StateFlags
{
    CF_Flags_Common_t com; /**< \brief applies to all transactions */
    CF_Flags_Rx_t     rx;  /**< \brief applies to only receive file transactions */
    CF_Flags_Tx_t     tx;  /**< \brief applies to only send file transactions */
} CF_StateFlags_t;

/**
 * @brief Summary of all possible transaction state information (tx and rx)
 */
typedef struct CF_StateData
{
    uint8 sub_state;
    uint8 acknak_count; /**< Number of times the ack_timer expired and reset */

    uint8 peer_cc; /**< \brief the peer cc from the received FIN or EOF PDU */
    uint8 fin_dc;  /**< \brief the dc in FIN PDU */
    uint8 fin_fs;  /**< \brief the fs in FIN PDU */

    CF_FileSize_t cached_pos;
    uint32        eof_crc;  /**< \brief remember the crc in the received EOF PDU  */
    CF_FileSize_t eof_size; /**< \brief remember the size in the received EOF PDU  */

} CF_StateData_t;

/**
 * @brief Transaction state object
 *
 * This keeps the state of CF file transactions
 */
typedef struct CF_Transaction
{
    CF_TxnState_t state; /**< \brief each engine is commanded to do something, which is the overall state */

    CF_History_t *     history;          /**< \brief weird, holds active filenames and possibly other info */
    CF_ChunkWrapper_t *chunks;           /**< \brief for gap tracking, only used on class 2 */
    CF_Timer_t         inactivity_timer; /**< \brief set to the overall inactivity timer of a remote */
    CF_Timer_t         ack_timer;        /**< \brief called ack_timer, but is also nak_timer */

    CF_FileSize_t fsize; /**< \brief lseek() should be 64-bit on 64-bit system, but osal limits to 32-bit */
    CF_FileSize_t foffs; /**< \brief offset into file for next read */
    osal_id_t     fd;

    CF_Crc_t crc;

    bool  reliable_mode; /**< Set true if class 2, false in class 1 */
    uint8 keep;
    uint8 chan_num; /**< \brief if ever more than one engine, this may need to change to pointer */
    uint8 priority;

    CF_CListNode_t cl_node;

    CF_Playback_t *pb; /**< \brief NULL if transaction does not belong to a playback */

    CF_StateData_t state_data;

    /**
     * @brief State flags
     *
     * \note The flags here look a little strange, because there are different flags for TX and RX.
     * Both types share the same type of flag, though. Since RX flags plus the global flags is
     * over one byte, storing them this way allows 2 bytes to cover all possible flags.
     * Please ignore the duplicate declarations of the "all" flags.
     */
    CF_StateFlags_t flags;
} CF_Transaction_t;

/**
 * @brief Identifies the type of timer tick being processed
 */
typedef enum
{
    CF_TickState_INIT,
    CF_TickState_RX_STATE,
    CF_TickState_TX_STATE,
    CF_TickState_TX_NAK,
    CF_TickState_TX_FILEDATA,
    CF_TickState_TX_PEND,
    CF_TickState_COMPLETE,
    CF_TickState_NUM_TYPES
} CF_TickState_t;

/**
 * @brief Channel state object
 *
 * This keeps the state of CF channels
 *
 * Each CF channel has a separate transaction list, PDU throttle, playback,
 * and poll state, as well as separate addresses on the underlying message
 * transport (e.g. SB).
 */
typedef struct CF_Channel
{
    CF_CListNode_t *qs[CF_QueueIdx_NUM];
    CF_CListNode_t *cs[CF_Direction_NUM];

    CFE_SB_PipeId_t pipe;

    uint32 num_cmd_tx;

    CF_Playback_t playback[CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN];

    /* For polling directories, the configuration data is in a table. */
    CF_Poll_t poll[CF_MAX_POLLING_DIR_PER_CHAN];

    osal_id_t sem_id; /**< \brief semaphore id for output pipe */

    uint32 outgoing_counter;

    /* If tick processing gets blocked due to TX limits (i.e. tx_blocked gets set during
     * tick processing) then this captures where the tick processing left off.
     * This is because it is important to tick every txn and not let traffic from the
     * first item(s) in the queue to consume all the bandwidth and never let the later
     * items get ticked. */
    const CF_Transaction_t *tick_resume;

    bool tx_blocked; /**< Set true if PDU transmission was blocked due to limits */

} CF_Channel_t;

/**
 * @brief CF engine output state
 *
 * Keeps the state of the current output PDU in the CF engine
 */
typedef struct CF_Output
{
    CFE_SB_Buffer_t *      msg;        /**< \brief Binary message to be sent to underlying transport */
    CF_EncoderState_t      encode;     /**< \brief Encoding state (while building message) */
    CF_Logical_PduBuffer_t tx_pdudata; /**< \brief Tx PDU logical values */
} CF_Output_t;

/**
 * @brief CF engine input state
 *
 * Keeps the state of the current input PDU in the CF engine
 */
typedef struct CF_Input
{
    CFE_SB_Buffer_t *      msg;        /**< \brief Binary message received from underlying transport */
    CF_DecoderState_t      decode;     /**< \brief Decoding state (while interpreting message) */
    CF_Logical_PduBuffer_t rx_pdudata; /**< \brief Rx PDU logical values */
} CF_Input_t;

/**
 * @brief An engine represents a pairing to a local EID
 *
 * Each engine can have at most CF_MAX_SIMULTANEOUS_TRANSACTIONS
 */
typedef struct CF_Engine
{
    CF_TransactionSeq_t seq_num; /* \brief keep track of the next sequence number to use for sends */

    CF_Output_t out;
    CF_Input_t  in;

    /* NOTE: could have separate array of transactions as part of channel? */
    CF_Transaction_t transactions[CF_NUM_TRANSACTIONS];
    CF_History_t     histories[CF_NUM_HISTORIES];
    CF_Channel_t     channels[CF_NUM_CHANNELS];

    CF_ChunkWrapper_t chunks[CF_NUM_TRANSACTIONS * CF_Direction_NUM];
    CF_Chunk_t        chunk_mem[CF_NUM_CHUNKS_ALL_CHANNELS];

    bool enabled;
} CF_Engine_t;

#endif
