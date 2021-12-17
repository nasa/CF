/************************************************************************
** File: cf_cfdp_pdu.h
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
**  The CF Application CFDP PDU definitions header file
**
**
**
*************************************************************************/

/**
 * @file
 *
 * Structures defining to CFDP PDUs
 *
 * Note that structures and enumerations defined in this file with a CF_CFDP
 * prefix are defined according to the CCSDS CFDP specifiction (727.0-B-5).
 * These values must match the specification for that structure/field, they are
 * not locally changeable.
 *
 * @note Many of the structures defined in this file are variably-sized when
 * encoded for network transmission.  As a result, C structures used to map
 * to these structures are of limited usefulness, generally only capable
 * of describing the first element(s) where offsets are fixed.  A marker member
 * is utilized to indicate where the fixed data ends and variable
 * length data begins.  At some point, the structures in this file
 * should change to encode/decode functions.
 */

#ifndef CF_CFDP_PDU_H
#define CF_CFDP_PDU_H

#include "cfe.h"
#include "cf_field.h"
#include <stddef.h>

#define CF_PACK __attribute__((packed))

/*
 * For now, the CFDP-encoded integers are just represented
 * as normal integer types.  This makes it compatible with
 * the source code as written, for now.
 *
 * Once the source code is updated, these types will be updated
 * accordingly, to reflect the fact that they are not normal
 * integer values.
 */
typedef uint8  CF_CFDP_uint8_t;
typedef uint16 CF_CFDP_uint16_t;
typedef uint32 CF_CFDP_uint32_t;

/**
 * @brief Structure representing base CFDP PDU header
 *
 * This header appears at the beginning of all CFDP PDUs, of all types.
 * Note that the header is variable length, it also contains source
 * and destination entity IDs, and the transaction sequence number.
 *
 * Defined per section 5.1 of CCSDS 727.0-B-5
 *
 * @note this contains variable length data for the EID+TSN, which is _not_ included
 * in this definition.  As a result, the sizeof(CF_CFDP_PduHeader_t) reflects only the
 * size of the fixed fields.  Use CF_HeaderSize() to get the actual size of this structure.
 */
typedef struct CF_CFDP_PduHeader
{
    CF_CFDP_uint8_t  flags;           /**< Flags indicating the PDU type, direction, mode, etc */
    CF_CFDP_uint16_t length;          /**< Length of the entire PDU, in octets */
    CF_CFDP_uint8_t  eid_tsn_lengths; /**< Lengths of the EID+TSN data (bitfields) */

    /* variable-length data goes here - it is at least 3 additional bytes */

} CF_PACK CF_CFDP_PduHeader_t;

/*
 * Fields within the "flags" byte of the PDU header
 */
DECLARE_FIELD(CF_CFDP_PduHeader_FLAGS_VERSION, 3, 5)
DECLARE_FIELD(CF_CFDP_PduHeader_FLAGS_TYPE, 1, 4)
DECLARE_FIELD(CF_CFDP_PduHeader_FLAGS_DIR, 1, 3)
DECLARE_FIELD(CF_CFDP_PduHeader_FLAGS_MODE, 1, 2)
DECLARE_FIELD(CF_CFDP_PduHeader_FLAGS_CRC, 1, 1)
DECLARE_FIELD(CF_CFDP_PduHeader_FLAGS_LARGEFILE, 1, 0)

/*
 * Fields within the "eid_tsn_lengths" byte of the PDU header
 */
DECLARE_FIELD(CF_CFDP_PduHeader_LENGTHS_ENTITY, 3, 4)
DECLARE_FIELD(CF_CFDP_PduHeader_LENGTHS_TRANSACTION_SEQUENCE, 3, 0)

extern int  CF_GetMemcpySize(const uint8 *num, int size);
extern void CF_MemcpyToBE(uint8 *dst, const uint8 *src, int src_size, int dst_size);
extern int  CF_GetVariableHeader(CF_CFDP_PduHeader_t *ph);
extern void CF_SetVariableHeader(CF_CFDP_PduHeader_t *ph, CF_EntityId_t src_eid, CF_EntityId_t dst_eid,
                                 CF_TransactionSeq_t tsn);
extern int  CF_HeaderSize(const CF_CFDP_PduHeader_t *ph);

#define CF_MAX_HEADER_SIZE (sizeof(CF_CFDP_PduHeader_t) + (2 * sizeof(CF_EntityId_t)) + sizeof(CF_TransactionSeq_t))
#define STATIC_CAST(ph, t) ((t *)(((uint8 *)ph) + CF_HeaderSize(ph)))

/**
 * @brief Structure representing CFDP File Directive Header
 *
 * Defined per section 5.2 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduFileDirectiveHeader
{
    CF_CFDP_uint8_t directive_code;
} CF_PACK CF_CFDP_PduFileDirectiveHeader_t;

/**
 * @brief Structure representing CFDP LV Object format
 *
 * These Length + Value pairs used in several CFDP PDU types,
 * typically for storage of strings such as file names.
 *
 * Defined per table 5-2 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_lv
{
    CF_CFDP_uint8_t length;                    /**< Length of data field */
    CF_CFDP_uint8_t data[CF_FILENAME_MAX_LEN]; /**< Variable-length Data */
} CF_PACK CF_CFDP_lv_t;

/**
 * @brief Structure representing CFDP TLV Object format
 *
 * These Type + Length + Value pairs used in several CFDP PDU types,
 * typically for file storage requests (section 5.4).
 *
 * Defined per table 5-3 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_tlv
{
    CF_CFDP_uint8_t type;    /**< Nature of data field */
    CF_CFDP_uint8_t length;  /**< Length of data field */
    CF_CFDP_uint8_t data[1]; /**< Variable-length Data */
} CF_PACK CF_CFDP_tlv_t;

/**
 * @brief Values for "type" field of TLV structure
 *
 * Defined per section 5.4 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_TLV_TYPE_FILESTORE_REQUEST      = 0,
    CF_CFDP_TLV_TYPE_FILESTORE_RESPONSE     = 1,
    CF_CFDP_TLV_TYPE_MESSAGE_TO_USER        = 2,
    CF_CFDP_TLV_TYPE_FAULT_HANDLER_OVERRIDE = 4,
    CF_CFDP_TLV_TYPE_FLOW_LABEL             = 5,
    CF_CFDP_TLV_TYPE_ENTITY_ID              = 6,
    CF_CFDP_TLV_TYPE_INVALID_MAX            = 7
} CF_CFDP_TlvType_t;

/**
 * @brief Values for "directive_code" within CF_CFDP_PduFileDirectiveHeader_t
 *
 * Defined per table 5-4 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_FileDirective_INVALID_MIN = 0, /* used to limit range */
    CF_CFDP_FileDirective_EOF         = 4,
    CF_CFDP_FileDirective_FIN         = 5,
    CF_CFDP_FileDirective_ACK         = 6,
    CF_CFDP_FileDirective_METADATA    = 7,
    CF_CFDP_FileDirective_NAK         = 8,
    CF_CFDP_FileDirective_PROMPT      = 9,
    CF_CFDP_FileDirective_KEEP_ALIVE  = 12,
    CF_CFDP_FileDirective_INVALID_MAX = 11, /* used to limit range */
} CF_CFDP_FileDirective_t;

/**
 * @brief Values for CFDP file transfer class
 *
 * The CFDP specification prescribes two classes/modes of file
 * transfer protocol operation - unacknowledged/simple or
 * acknowledged/reliable.
 *
 * Defined per section 7.1 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_CLASS_1 = 0, /**< CFDP class 1 - Unreliable transfer */
    CF_CFDP_CLASS_2 = 1, /**< CFDP class 2 - Reliable transfer */
} CF_CFDP_Class_t;

/**
 * @brief Values for "acknowledgement transfer status"
 *
 * This enum is pertient to the ACK PDU type, defines the
 * values for the directive field.
 *
 * Defined per section 5.2.4 / table 5-8 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_AckTxnStatus_UNDEFINED    = 0,
    CF_CFDP_AckTxnStatus_ACTIVE       = 1,
    CF_CFDP_AckTxnStatus_TERMINATED   = 2,
    CF_CFDP_AckTxnStatus_UNRECOGNIZED = 3,
    CF_CFDP_AckTxnStatus_INVALID      = 4,
} CF_CFDP_AckTxnStatus_t;

/**
 * @brief Values for "finished delivery code"
 *
 * This enum is pertient to the FIN PDU type, defines the
 * values for the delivery code field.
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_FinDeliveryCode_COMPLETE   = 0,
    CF_CFDP_FinDeliveryCode_INCOMPLETE = 1,
    CF_CFDP_FinDeliveryCode_INVALID    = 2,
} CF_CFDP_FinDeliveryCode_t;

/**
 * @brief Values for "finished file status"
 *
 * This enum is pertient to the FIN PDU type, defines the
 * values for the file status field.
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_FinFileStatus_DISCARDED           = 0,
    CF_CFDP_FinFileStatus_DISCARDED_FILESTORE = 1,
    CF_CFDP_FinFileStatus_RETAINED            = 2,
    CF_CFDP_FinFileStatus_UNREPORTED          = 3,
    CF_CFDP_FinFileStatus_INVALID             = 4,
} CF_CFDP_FinFileStatus_t;

/**
 * @brief Values for "condition code"
 *
 * This enum defines the values for the condition code field
 * for the PDU types which have this field (EOF, FIN, ACK)
 *
 * Defined per table 5-5 of CCSDS 727.0-B-5
 */
typedef enum
{
    CF_CFDP_ConditionCode_NO_ERROR                  = 0,
    CF_CFDP_ConditionCode_POS_ACK_LIMIT_REACHED     = 1,
    CF_CFDP_ConditionCode_KEEP_ALIVE_LIMIT_REACHED  = 2,
    CF_CFDP_ConditionCode_INVALID_TRANSMISSION_MODE = 3,
    CF_CFDP_ConditionCode_FILESTORE_REJECTION       = 4,
    CF_CFDP_ConditionCode_FILE_CHECKSUM_FAILURE     = 5,
    CF_CFDP_ConditionCode_FILE_SIZE_ERROR           = 6,
    CF_CFDP_ConditionCode_NAK_LIMIT_REACHED         = 7,
    CF_CFDP_ConditionCode_INACTIVITY_DETECTED       = 8,
    CF_CFDP_ConditionCode_INVALID_FILE_STRUCTURE    = 9,
    CF_CFDP_ConditionCode_CHECK_LIMIT_REACHED       = 10,
    CF_CFDP_ConditionCode_UNSUPPORTED_CHECKSUM_TYPE = 11,
    CF_CFDP_ConditionCode_SUSPEND_REQUEST_RECEIVED  = 14,
    CF_CFDP_ConditionCode_CANCEL_REQUEST_RECEIVED   = 15,
} CF_CFDP_ConditionCode_t;

/**
 * @brief Structure representing CFDP End of file PDU
 *
 * Defined per section 5.2.2 / table 5-6 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduEof
{
    CF_CFDP_PduFileDirectiveHeader_t fdh;
    CF_CFDP_uint8_t                  cc;
    CF_CFDP_uint32_t                 crc;
    CF_CFDP_uint32_t                 size;

    /* variable length member is of TLV type, placeholder declared as uint8 */
    /* may be omitted entirely in some cases */
    CF_CFDP_uint8_t fault_location[1];
} CF_PACK CF_CFDP_PduEof_t;

/*
 * Position of the condition code value within the CC field
 */
DECLARE_FIELD(CF_CFDP_PduEof_FLAGS_CC, 4, 4)

/**
 * @brief Structure representing CFDP Finished PDU
 *
 * Defined per section 5.2.3 / table 5-7 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduFin
{
    CF_CFDP_PduFileDirectiveHeader_t fdh;
    CF_CFDP_uint8_t                  flags;

    /* variable length member is of TLV type, placeholder declared as uint8 */
    CF_CFDP_uint8_t fault_location[1];
} CF_PACK CF_CFDP_PduFin_t;

/*
 * Position of the sub-field values within the flags field
 */
DECLARE_FIELD(CF_CFDP_PduFin_FLAGS_CC, 4, 4)
DECLARE_FIELD(CF_CFDP_PduFin_FLAGS_END_SYSTEM_STATUS, 1, 3)
DECLARE_FIELD(CF_CFDP_PduFin_FLAGS_DELIVERY_CODE, 1, 2)
DECLARE_FIELD(CF_CFDP_PduFin_FLAGS_FILE_STATUS, 2, 0)

/**
 * @brief Structure representing CFDP Acknowledge PDU
 *
 * Defined per section 5.2.4 / table 5-8 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduAck
{
    CF_CFDP_PduFileDirectiveHeader_t fdh;
    CF_CFDP_uint8_t                  directive_and_subtype_code;
    CF_CFDP_uint8_t                  cc_and_transaction_status;
} CF_PACK CF_CFDP_PduAck_t;

/*
 * Position of the sub-field values within the directive_and_subtype_code
 * and cc_and_transaction_status fields within the ACK PDU.
 */
DECLARE_FIELD(CF_CFDP_PduAck_DIR_CODE, 4, 4)
DECLARE_FIELD(CF_CFDP_PduAck_DIR_SUBTYPE_CODE, 4, 0)
DECLARE_FIELD(CF_CFDP_PduAck_CC, 4, 4)
DECLARE_FIELD(CF_CFDP_PduAck_TRANSACTION_STATUS, 2, 0)

/**
 * @brief Structure representing CFDP Segment Request
 *
 * Defined per section 5.2.6 / table 5-11 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_SegmentRequest
{
    CF_CFDP_uint32_t offset_start;
    CF_CFDP_uint32_t offset_end;
} CF_CFDP_SegmentRequest_t;

/**
 * @brief Structure representing CFDP Non-Acknowledge PDU
 *
 * Defined per section 5.2.6 / table 5-10 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduNak
{
    CF_CFDP_PduFileDirectiveHeader_t fdh;
    CF_CFDP_uint32_t                 scope_start;
    CF_CFDP_uint32_t                 scope_end;

    /* variable length member is of SegmentRequest type */
    CF_CFDP_SegmentRequest_t segment_requests[1];
} CF_PACK CF_CFDP_PduNak_t;

/**
 * @brief Structure representing CFDP Metadata PDU
 *
 * Defined per section 5.2.5 / table 5-9 of CCSDS 727.0-B-5
 */
typedef struct CF_CFDP_PduMd
{
    CF_CFDP_PduFileDirectiveHeader_t fdh;
    CF_CFDP_uint8_t                  segmentation_control;
    CF_CFDP_uint32_t                 size;

    /* variable length member is of LV type, placeholder declared as uint8 */
    CF_CFDP_uint8_t filename_lvs[1];
} CF_PACK CF_CFDP_PduMd_t;

/*
 * Position of the sub-field values within the directive_and_subtype_code
 * and cc_and_transaction_status fields within the ACK PDU.
 */
DECLARE_FIELD(CF_CFDP_PduMd_CLOSURE_REQUESTED, 1, 7)
DECLARE_FIELD(CF_CFDP_PduMd_CHECKSUM_TYPE, 4, 0)

typedef struct CF_CFDP_PduFileDataHeader
{
    CF_CFDP_uint32_t offset;
} CF_PACK CF_CFDP_PduFileDataHeader_t;

typedef struct CF_CFDP_PduFileDataContent
{
    uint8 data[CF_MAX_PDU_SIZE - sizeof(CF_CFDP_PduFileDataHeader_t) - CF_MAX_HEADER_SIZE];
} CF_PACK CF_CFDP_PduFileDataContent_t;

typedef struct CF_CFDP_PduFd
{
    CF_CFDP_PduFileDataHeader_t  fdh;
    CF_CFDP_PduFileDataContent_t fdd;
} CF_PACK CF_CFDP_PduFd_t;

/* NOTE: the use of CF_CFDP_PduHeader_t below is correct, but the CF_PduRecvMsg_t and CF_PduSendMsg_t
 * structures are both longer than these definitions. They are always backed by a buffer
 * of size CF_MAX_PDU_SIZE */
typedef struct CF_PduRecvMsg
{
    CFE_MSG_CommandHeader_t hdr;
    CF_CFDP_PduHeader_t     ph;
} CF_PACK CF_PduRecvMsg_t;

typedef struct CF_PduSendMsg
{
    CFE_MSG_TelemetryHeader_t hdr;
    CF_CFDP_PduHeader_t       ph;
} CF_PACK CF_PduSendMsg_t;

/* portable static CF_Assert that size of CF_NAK_MAX_SEGMENTS is small enough to fit in CF_MAX_PDU_SIZE */
typedef char
    p__LINE__[((CF_NAK_MAX_SEGMENTS * 8) + sizeof(CF_CFDP_PduNak_t) + CF_MAX_HEADER_SIZE) <= CF_MAX_PDU_SIZE ? 1 : -1];

#endif /* !CF_CFDP_PDU_H */
