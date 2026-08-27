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
 *   Specification for the CFS CFDP (CF) command and telemetry
 *   message payload and constant definitions.
 */
#ifndef DEFAULT_CF_MSGDEFS_H
#define DEFAULT_CF_MSGDEFS_H

#include "common_types.h"
#include "cf_extern_typedefs.h"

/**
 * \defgroup cfscfmsgtypes Typedefs used in CFS CFDP Commands and Telemetry
 * \{
 */

/**
 * \brief Type IDs for use for Write Queue cmd
 */
typedef enum CF_Type
{
    CF_Type_all  = 0, /**< \brief Type all */
    CF_Type_up   = 1, /**< \brief Type up */
    CF_Type_down = 2  /**< \brief Type down */
} CF_Type_t;

/**
 * @brief External type to use for CFDP queue type
 *
 * This uses the labels defined in enum CF_Type
 * but maps to a fixed-width type for use in CMD/TLM/Tables
 */
typedef uint8 CF_Type_Enum_t;

/**
 * @brief Standardized type used for channel selection in commands
 *
 * 255 = all channels (if applicable; not all commands allow all channels)
 * 0-X = single channel number
 */
typedef uint8 CF_ChannelSelect_t;

/**
 * Standardized type used for polling directory selection in commands
 *
 * 255 = all polling directory indices (if applicable)
 * 0-X = single polling directory index number
 */
typedef uint8 CF_PollIdxSelect_t;

/**
 * \brief Enum labels for use for queue selection
 */
enum CF_QueueSelect
{
    CF_QueueSelect_Pending = 0, /**< \brief Pending Queue */
    CF_QueueSelect_Active  = 1, /**< \brief Active Queue */
    CF_QueueSelect_History = 2, /**< \brief History Queue */
    CF_QueueSelect_All     = 3, /**< \brief All queues */
};

/**
 * @brief External type to use for CFDP queue select
 *
 * This uses the labels defined in enum CF_QueueSelect
 * but maps to a fixed-width type for use in CMD/TLM/Tables
 */
typedef uint8 CF_QueueSelect_Enum_t;

/** \} */

/**
 * \defgroup cfscftlm CFS CFDP Telemetry
 * \{
 */

/**
 * \brief Housekeeping command counters
 */
typedef struct CF_HkCmdCounters
{
    uint16 cmd; /**< \brief Command success counter */
    uint16 err; /**< \brief Command error counter */
} CF_HkCmdCounters_t;

/**
 * \brief Housekeeping sent counters
 */
typedef struct CF_HkSent
{
    uint64 file_data_bytes;      /**< \brief Sent File data bytes */
    uint32 pdu;                  /**< \brief Sent PDUs counter */
    uint32 nak_segment_requests; /**< \brief Sent NAK segment requests counter */
    uint32 files_sent;           /**< \brief Files successfully sent counter */
    uint32 files_started;        /**< \brief Files initialized to be sent counter */
} CF_HkSent_t;

/**
 * \brief Housekeeping received counters
 */
typedef struct CF_HkRecv
{
    uint64 file_data_bytes;      /**< \brief Received File data bytes */
    uint32 pdu;                  /**< \brief Received PDUs with valid header counter */
    uint32 error;                /**< \brief Received PDUs with error counter, see related event for cause */
    uint16 spurious;             /**< \brief Received PDUs with invalid directive code for current context or
                                  *          file directive FIN without matching active transaction counter,
                                  *          see related event for cause
                                  */
    uint16 dropped;              /**< \brief Received PDUs dropped due to a transaction error */
    uint32 nak_segment_requests; /**< \brief Received NAK segment requests counter */
    uint32 files_recv;           /**< \brief Files successfully received counter */
    uint32 files_started;        /**< \brief Files initialized to be received counter */
} CF_HkRecv_t;

/**
 * \brief Housekeeping fault counters
 */
typedef struct CF_HkFault
{
    uint16 file_open;          /**< \brief File open fault counter */
    uint16 file_read;          /**< \brief File read fault counter */
    uint16 file_seek;          /**< \brief File seek fault counter */
    uint16 file_write;         /**< \brief File write fault counter */
    uint16 file_rename;        /**< \brief File rename fault counter */
    uint16 directory_read;     /**< \brief Directory read fault counter */
    uint16 crc_mismatch;       /**< \brief CRC mismatch fault counter */
    uint16 file_size_mismatch; /**< \brief File size mismatch fault counter */
    uint16 nak_limit;          /**< \brief NAK limit exceeded fault counter */
    uint16 ack_limit;          /**< \brief ACK limit exceeded fault counter */
    uint16 inactivity_timer;   /**< \brief Inactivity timer exceeded counter */
    uint16 spare;              /**< \brief Alignment spare to avoid implicit padding */
} CF_HkFault_t;

/**
 * \brief Housekeeping counters
 */
typedef struct CF_HkCounters
{
    CF_HkSent_t  sent;  /**< \brief Sent counters */
    CF_HkRecv_t  recv;  /**< \brief Received counters */
    CF_HkFault_t fault; /**< \brief Fault counters */
} CF_HkCounters_t;

/**
 * \brief Housekeeping channel data
 */
typedef struct CF_HkChannel_Data
{
    CF_HkCounters_t counters;                /**< \brief Counters */
    uint16          q_size[CF_QueueIdx_NUM]; /**< \brief Queue sizes */
    uint8           poll_counter;            /**< \brief Number of active polling directories */
    uint8           playback_counter;        /**< \brief Number of active playback directories */
    uint8           frozen;                  /**< \brief Frozen state: 0 == not frozen, else frozen */
    uint8           spare;                   /**< \brief Alignment spare (uint64 values in the counters) */
} CF_HkChannel_Data_t;

/**
 * \brief Housekeeping packet
 */
typedef struct CF_HkPacket_Payload
{
    CF_HkCmdCounters_t counters; /**< \brief Command counters */
    uint8              Padding[4];

    CF_HkChannel_Data_t channel_hk[CF_NUM_CHANNELS]; /**< \brief Per channel housekeeping data */
} CF_HkPacket_Payload_t;

/**
 * \brief End of transaction packet
 */
typedef struct CF_EotPacket_Payload
{
    CF_TransactionSeq_t seq_num;    /**< \brief transaction identifier, stays constant for entire transfer */
    uint32              channel;    /**< \brief Channel number */
    uint32              direction;  /**< \brief direction of this transaction */
    uint32              state;      /**< \brief Transaction state */
    uint32              txn_stat;   /**< \brief final status code of transaction (extended CFDP CC) */
    CF_EntityId_t       src_eid;    /**< \brief the source eid of the transaction */
    CF_EntityId_t       peer_eid;   /**< \brief peer_eid is always the "other guy", same src_eid for RX */
    uint32              fsize;      /**< \brief File size */
    uint32              crc_result; /**< \brief CRC result */
    CF_TxnFilenames_t   fnames;     /**< \brief file names associated with this transaction */
} CF_EotPacket_Payload_t;

/**\}*/

/**
 * \defgroup cfscfcmdstructs CFS CFDP Command Structures
 * \{
 */

/**
 * \brief Enum labels for use for Reset cmd
 */
enum CF_Reset
{
    CF_Reset_all     = 0, /**< \brief Reset all */
    CF_Reset_command = 1, /**< \brief Reset command */
    CF_Reset_fault   = 2, /**< \brief Reset fault */
    CF_Reset_up      = 3, /**< \brief Reset up */
    CF_Reset_down    = 4  /**< \brief Reset down */
};

/**
 * @brief External type to use for Reset command
 *
 * This uses the labels defined in enum CF_Reset
 * but maps to a fixed-width type for use in CMD/TLM/Tables
 */
typedef uint8 CF_Reset_Enum_t;

/**
 * \brief Reset command payload
 *
 * For command details see #CF_RESET_CC
 */
typedef struct CF_ResetCountersCmd_Payload
{
    CF_Reset_Enum_t ResetType;
} CF_ResetCountersCmd_Payload_t;

/**
 * \brief Single byte channel selector payload
 *
 * Used in all commands needing only a channel selection
 *       - 255 = all channels
 *       - else = single channel
 */
typedef struct CF_ChannelSelect_Payload
{
    CF_ChannelSelect_t ChannelSelect;
} CF_ChannelSelect_Payload_t;

/**
 * \brief Polling directory selector payload
 *
 * Used in all commands needing a polldir selection
 *
 * Single byte channel selector
 *       - 255 = all channels
 *       - else = single channel
 *
 * Single byte polling directory index
 *       - 255 = all polling directories
 *       - else = single polling directory index
 */
typedef struct CF_PollDirSelect_Payload
{
    CF_ChannelSelect_t ChannelSelect;
    CF_PollIdxSelect_t PollDirIndx;
} CF_PollDirSelect_Payload_t;

/**
 * \brief Queue selector payload
 *
 * Used in all commands needing a queue selection
 *
 *       Single byte channel selector
 *       - 255 = all channels
 *       - else = single channel
 *
 *       Single byte queue selection enum
 *       Values indicated by #CF_QueueSelect enum
 *       - 0 = Pending queue
 *       - 1 = History queue
 *       - 2 = Both pending and history queue
 */
typedef struct CF_QueueSelect_Payload
{
    CF_ChannelSelect_t    ChannelSelect;
    CF_QueueSelect_Enum_t QueueSelect;
} CF_QueueSelect_Payload_t;

/**
 * \brief Parameter IDs for use with Get/Set parameter messages
 *
 * Specifically these are used for the "key" field within CF_GetParamCmd_t and
 * CF_SetParamCmd_t message structures.
 */
enum CF_GetSet_ValueID
{
    CF_GetSet_ValueID_ticks_per_second,                      /**< \brief Ticks per second key */
    CF_GetSet_ValueID_rx_crc_calc_bytes_per_wakeup,          /**< \brief Receive CRC calculated bytes per wake-up key */
    CF_GetSet_ValueID_ack_timer_s,                           /**< \brief ACK timer in seconds key */
    CF_GetSet_ValueID_nak_timer_s,                           /**< \brief NAK timer in seconds key */
    CF_GetSet_ValueID_inactivity_timer_s,                    /**< \brief Inactivity timer in seconds key */
    CF_GetSet_ValueID_outgoing_file_chunk_size,              /**< \brief Outgoing file chunk size key */
    CF_GetSet_ValueID_ack_limit,                             /**< \brief ACK retry limit key */
    CF_GetSet_ValueID_nak_limit,                             /**< \brief NAK retry limit key */
    CF_GetSet_ValueID_local_eid,                             /**< \brief Local entity id key */
    CF_GetSet_ValueID_chan_max_outgoing_messages_per_wakeup, /**< \brief Max outgoing messages per wake-up key */
    CF_GetSet_ValueID_MAX                                    /**< \brief Key limit used for validity check */
};

/**
 * @brief External type to use for CF_GetSet_ValueID
 *
 * This uses the labels defined in enum CF_GetSet_ValueID
 * but maps to a fixed-width type for use in CMD/TLM/Tables
 */
typedef uint8 CF_GetSet_ValueID_Enum_t;

/**
 * \brief Get parameter command structure
 *
 * For command details see #CF_GET_PARAM_CC
 */
typedef struct CF_GetParam_Payload
{
    CF_GetSet_ValueID_Enum_t key;      /**< \brief Parameter key, see #CF_GetSet_ValueID_t */
    CF_ChannelSelect_t       chan_num; /**< \brief Channel number */
} CF_GetParam_Payload_t;

/**
 * \brief Set parameter command structure
 *
 * For command details see #CF_SET_PARAM_CC
 */
typedef struct CF_SetParam_Payload
{
    uint32                   value;    /**< \brief Parameter value to set */
    CF_GetSet_ValueID_Enum_t key;      /**< \brief Parameter key, see #CF_GetSet_ValueID_t */
    CF_ChannelSelect_t       chan_num; /**< \brief Channel number */
    uint8                    spare[2]; /**< \brief Alignment spare, uint32 multiple */
} CF_SetParam_Payload_t;

/**
 * \brief Transmit file command structure
 *
 * For command details see #CF_TX_FILE_CC
 */
typedef struct CF_TxFile_Payload
{
    CF_CFDP_Class_Enum_t cfdp_class;                        /**< \brief CFDP class: 0=class 1, 1=class 2 */
    uint8                keep;                              /**< \brief Keep file flag: 1=keep, else delete */
    CF_ChannelSelect_t   chan_num;                          /**< \brief Channel number */
    uint8                priority;                          /**< \brief Priority: 0=highest priority */
    CF_EntityId_t        dest_id;                           /**< \brief Destination entity id */
    char                 src_filename[CF_FILENAME_MAX_LEN]; /**< \brief Source file/directory name */
    char                 dst_filename[CF_FILENAME_MAX_LEN]; /**< \brief Destination file/directory name */
} CF_TxFile_Payload_t;

/**
 * \brief Write Queue command structure
 *
 * For command details see #CF_WRITE_QUEUE_CC
 */
typedef struct CF_WriteQueue_Payload
{
    CF_Type_Enum_t        type;  /**< \brief Transaction direction: all=0, up=1, down=2 */
    CF_ChannelSelect_t    chan;  /**< \brief Channel number */
    CF_QueueSelect_Enum_t queue; /**< \brief Queue type: 0=pending, 1=active, 2=history, 3=all */
    uint8                 spare; /**< \brief Alignment spare, puts filename on 32-bit boundary */

    char filename[CF_FILENAME_MAX_LEN]; /**< \brief Filename written to */
} CF_WriteQueue_Payload_t;

/**
 * \brief Transaction command structure
 *
 * For command details see #CF_SUSPEND_CC, #CF_RESUME_CC, #CF_CANCEL_CC, #CF_ABANDON_CC
 */
typedef struct CF_Transaction_Payload
{
    CF_TransactionSeq_t ts;       /**< \brief Transaction sequence number */
    CF_EntityId_t       eid;      /**< \brief Entity id */
    CF_ChannelSelect_t  chan_num; /**< \brief Channel number: 254=use ts, 255=all channels, else channel */
    uint8               spare[3]; /**< \brief Alignment spare for 32-bit multiple */
} CF_Transaction_Payload_t;

/**\}*/

/**
 * @brief Internal type (enum) for the CFDP class.
 *
 * This typedef should only be used internally but the same labels/values
 * apply externally as well.  Notably this is a native-width type, and for
 * external cmd/tlm use it needs to be a fixed-width type.
 *
 * @sa #CF_CFDP_Class_Enum_t
 */
typedef enum CF_CFDP_Class CF_CFDP_Class_t;

/**
 * @brief Internal type (enum) for the CFDP value identifier.
 *
 * This typedef should only be used internally but the same labels/values
 * apply externally as well.  Notably this is a native-width type, and for
 * external cmd/tlm use it needs to be a fixed-width type.
 *
 * @sa #CF_GetSet_ValueID_Enum_t
 */
typedef enum CF_GetSet_ValueID CF_GetSet_ValueID_t;

#endif
