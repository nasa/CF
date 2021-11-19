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

#ifndef CF_CFDPU_PDU__H
#define CF_CFDPU_PDU__H

#include "cfe.h"
#include "cf_field.h"
#include <stddef.h>

#define CF_PACK __attribute__((packed))

/* NOTE: these structures are all needed to work with CFDP PDUs.
 *
 * These are all C representations. Marshal/unmarshal functions will be required to move them into and out of buffers */

/* NOTE: TLV (type-length-value) is not supported as the CF app does not support file directive PDUs.
 * CF 2.0 also did not support these. LV (length-value) is supported */
typedef struct
{
    uint8  flags;
    uint16 length;
    uint8  eid_tsn_lengths;

    /* NOTE: the rest of the header is variable length */
#if 0
    /* NOTE: these are variable length, from 1-8. For now, only support 32-bit */
    cf_entity_id_t source_entity_id;
    cf_transaction_seq_t transaction_sequence_number;
    cf_entity_id_t destination_entity_id;
#endif
} CF_PACK pdu_header_t;

extern int   CF_GetMemcpySize(const uint8 *num, int size);
extern void  CF_MemcpyToBE(uint8 *dst, const uint8 *src, int src_size, int dst_size);
extern int   CF_GetVariableHeader(void);
extern void  CF_SetVariableHeader(cf_entity_id_t src_eid, cf_entity_id_t dst_eid, cf_transaction_seq_t tsn);
extern void *CF_EndOfHeaderPtr(const pdu_header_t *ph);
extern int   CF_HeaderSize(const pdu_header_t *ph);

#define CF_MAX_HEADER_SIZE (sizeof(pdu_header_t) + (2 * sizeof(cf_entity_id_t)) + sizeof(cf_transaction_seq_t))
#define STATIC_CAST(ph, t) ((t *)(((uint8 *)ph) + CF_HeaderSize(ph)))

typedef struct
{
    // pdu_header_t ph;
    uint8 directive_code;
} CF_PACK pdu_file_directive_header_t;

DECLARE_FIELD(PDU_HDR_FLAGS_VERSION, 3, 5)
DECLARE_FIELD(PDU_HDR_FLAGS_TYPE, 1, 4)
DECLARE_FIELD(PDU_HDR_FLAGS_DIR, 1, 3)
DECLARE_FIELD(PDU_HDR_FLAGS_MODE, 1, 2)
DECLARE_FIELD(PDU_HDR_FLAGS_CRC, 1, 1)
DECLARE_FIELD(PDU_HDR_FLAGS_RESERVED, 1, 0)
DECLARE_FIELD(PDU_LENGTHS_ENTITY, 3, 4)
DECLARE_FIELD(PDU_LENGTHS_TRANSACTION_SEQUENCE, 3, 0)

/* NOTE: the longest LV seem to be filenames, so clamp their size to CF_FILENAME_MAX_LEN */
#define LV_MAX_LEN CF_FILENAME_MAX_LEN
typedef struct
{
    uint8 length;
    uint8 data[CF_FILENAME_MAX_LEN];
} CF_PACK lv_t;

typedef struct
{
    uint8 type;
    uint8 length;
    uint8 data[CF_FILENAME_MAX_LEN];
} CF_PACK tlv_t;

typedef enum
{
    CLASS_1 = 0,
    CLASS_2 = 1,
} cfdp_class_t;

typedef enum
{
    ACK_TS_INACTIVE     = 0,
    ACK_TS_ACTIVE       = 1,
    ACK_TS_TERMINATED   = 2,
    ACK_TS_UNRECOGNIZED = 3,
    ACK_TS_INVALID      = 4,
} ack_transaction_status_t;

typedef enum
{
    FIN_COMPLETE              = 0,
    FIN_INCOMPLETE            = 1,
    FIN_DELIVERY_CODE_INVALID = 2,
} fin_delivery_code_t;

typedef enum
{
    FIN_DISCARDED           = 0,
    FIN_DISCARDED_FILESTORE = 1,
    FIN_RETAINED            = 2,
    FIN_UNREPORTED          = 3,
    FIN_FILE_STATUS_INVALID = 4,
} fin_file_status_t;

typedef enum
{
    PDU_INVALID_MIN = 0, /* used to limit range */
    PDU_EOF         = 4,
    PDU_FIN         = 5,
    PDU_ACK         = 6,
    PDU_METADATA    = 7,
    PDU_NAK         = 8,
    PDU_PROMPT      = 9,
    PDU_KEEP_ALIVE  = 10,
    PDU_INVALID_MAX = 11, /* used to limit range */
} file_directive_t;

typedef enum
{
    CC_NO_ERROR                  = 0,
    CC_POS_ACK_LIMIT_REACHED     = 1,
    CC_KEEP_ALIVE_LIMIT_REACHED  = 2,
    CC_INVALID_TRANSMISSION_MODE = 3,
    CC_FILESTORE_REJECTION       = 4,
    CC_FILE_CHECKSUM_FAILURE     = 5,
    CC_FILE_SIZE_ERROR           = 6,
    CC_NAK_LIMIT_REACHED         = 7,
    CC_INACTIVITY_DETECTED       = 8,
    CC_INVALID_FILE_STRUCTURE    = 9,
    CC_CHECK_LIMIT_REACHED       = 10,
    CC_SUSPEND_REQUEST_RECEIVED  = 14,
    CC_CANCEL_REQUEST_RECEIVED   = 15,
} condition_code_t;

typedef struct
{
    pdu_file_directive_header_t fdh;
    uint8                       cc;
    uint32                      crc;
    uint32                      size;
    tlv_t                       fault_location[1]; /* may be omitted when sent or received */
} CF_PACK pdu_eof_t;

DECLARE_FIELD(PDU_FLAGS_CC, 4, 4)

typedef struct
{
    pdu_file_directive_header_t fdh;
    uint8                       flags;
    tlv_t                       fault_location[1];
} CF_PACK pdu_fin_t;

DECLARE_FIELD(PDU_FIN_FLAGS_END_SYSTEM_STATUS, 1, 3)
DECLARE_FIELD(PDU_FIN_FLAGS_DELIVERY_CODE, 1, 2)
DECLARE_FIELD(PDU_FIN_FLAGS_FILE_STATUS, 2, 0)

typedef struct
{
    pdu_file_directive_header_t fdh;
    uint8                       directive_and_subtype_code;
    uint8                       cc_and_transaction_status;
} CF_PACK pdu_ack_t;

DECLARE_FIELD(PDU_ACK_DIR_CODE, 4, 4)
DECLARE_FIELD(PDU_ACK_DIR_SUBTYPE_CODE, 4, 0)
DECLARE_FIELD(PDU_ACK_CC, 4, 4)
DECLARE_FIELD(PDU_ACK_TRANSACTION_STATUS, 2, 0)

typedef struct segment_request_t
{
    uint32 offset_start;
    uint32 offset_end;
} segment_request_t;

typedef struct
{
    pdu_file_directive_header_t fdh;
    uint32                      scope_start;
    uint32                      scope_end;

    /* NOTE: if offset_start and offset_end == 0, then the nak is the metadata pdu */
    segment_request_t segment_requests[1];
} CF_PACK pdu_nak_t;

typedef struct
{
    pdu_file_directive_header_t fdh;
    uint8                       segmentation_control;
    uint32                      size;
    /* use a byte array for the two LVs because their size is variable */
    uint8 filename_lvs[1]; /* is it ok to use 0-length array? it works because pdu_md_t is contained in larger union */
    /* SDS no md tlv supported? */
} CF_PACK pdu_md_t;

typedef struct
{
    // pdu_header_t ph;
    uint32 offset;
} CF_PACK pdu_file_data_header_t;

typedef struct
{
    uint8 data[CF_MAX_PDU_SIZE - sizeof(pdu_file_data_header_t) - CF_MAX_HEADER_SIZE];
} CF_PACK pdu_fd_data_t;

typedef struct
{
    pdu_file_data_header_t fdh;
    pdu_fd_data_t          fdd;
} CF_PACK pdu_fd_t;

#if 0
typedef union {
    uint8 data[CF_MAX_PDU_SIZE];
    pdu_header_t ph;
    pdu_file_directive_header_t fdh;

    pdu_fd_t fd;
    pdu_md_t md;
    pdu_eof_t eof;
    pdu_ack_t ack;
    pdu_fin_t fin;
    pdu_nak_t nak;
} CF_PACK pdu_t;
#endif

/* NOTE: the use of pdu_header_t below is correct, but the pdu_r_msg_t and pdu_s_msg_t
 * structures are both longer than these definitions. They are always backed by a buffer
 * of size CF_MAX_PDU_SIZE */
typedef struct
{
    CFE_MSG_CommandHeader_t hdr;
    pdu_header_t            ph;
    // pdu_t pdu;
} CF_PACK pdu_r_msg_t;

typedef struct
{
    CFE_MSG_TelemetryHeader_t hdr;
    pdu_header_t              ph;
    // pdu_t pdu;
} CF_PACK pdu_s_msg_t;

DECLARE_FIELD(PDU_MD_SEGMENTATION_CONTROL, 1, 7)

/* portable static CF_Assert that size of CF_NAK_MAX_SEGMENTS is small enough to fit in CF_MAX_PDU_SIZE */
typedef char
    p__LINE__[((CF_NAK_MAX_SEGMENTS * 8) + sizeof(pdu_nak_t) + CF_MAX_HEADER_SIZE) <= CF_MAX_PDU_SIZE ? 1 : -1];

#define ENTITY_ID_TLV_TYPE 6

#endif /* !CF_CFDPU_PDU__H */
