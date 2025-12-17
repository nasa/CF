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
 * Declarations and prototypes for cf_extern_typedefs module
 */

#ifndef CF_EXTERN_TYPEDEFS_H
#define CF_EXTERN_TYPEDEFS_H

/* constants such as CF_FILENAME_MAX_LEN are in mission_cfg.h */
#include "cf_mission_cfg.h"

#ifdef CFE_EDS_ENABLED_BUILD

#include "cf_eds_typedefs.h"

typedef CF_QueueIdx_Enum_t CF_QueueIdx_t;
#define CF_QueueIdx_NUM       (1 + EdsDataType_CF_QueueIdx_t_MAX)
#define CF_GetSet_ValueID_MAX (1 + EdsDataType_CF_GetSet_ValueID_t_MAX)

typedef CF_EntityId_Atom_t       CF_EntityId_t;
typedef CF_TransactionSeq_Atom_t CF_TransactionSeq_t;

typedef CF_CFDP_Enum_t           CF_CFDP_Class_t;
typedef CF_GetSet_ValueID_Enum_t CF_GetSet_ValueID_t;

typedef EdsDataType_BASE_TYPES_PathName_t CF_PathName_t;
typedef EdsDataType_BASE_TYPES_FileName_t CF_FileName_t;

#define CF_FILENAME_MAX_NAME ((int)sizeof(CF_FileName_t))
#define CF_FILENAME_MAX_LEN  ((int)sizeof(CF_PathName_t))

#else

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
    CF_CFDP_CLASS_1 = 0, /**< \brief CFDP class 1 - Unreliable transfer */
    CF_CFDP_CLASS_2 = 1, /**< \brief CFDP class 2 - Reliable transfer */
} CF_CFDP_Class_t;

/**
 * @brief CF queue identifiers
 */
typedef enum
{
    CF_QueueIdx_PEND      = 0, /**< \brief tx transactions that have not started */
    CF_QueueIdx_TX        = 1, /**< \brief tx transactions in progress */
    CF_QueueIdx_RX        = 2, /**< \brief rx transactions in progress */
    CF_QueueIdx_HIST      = 3, /**< \brief transaction history (completed) */
    CF_QueueIdx_HIST_FREE = 4, /**< \brief unused transaction history structs */
    CF_QueueIdx_FREE      = 5, /**< \brief unused transaction structs */
    CF_QueueIdx_NUM       = 6
} CF_QueueIdx_t;

/**
 * @brief Cache of source and destination filename
 *
 * This pairs a source and destination file name together
 * to be retained for future reference in the transaction/history
 */
typedef struct CF_TxnFilenames
{
    char src_filename[CF_FILENAME_MAX_LEN];
    char dst_filename[CF_FILENAME_MAX_LEN];
} CF_TxnFilenames_t;

/**
 * @brief Entity id size
 *
 * @par Description:
 *      The maximum size of the entity id as expected for all CFDP packets.
 *      CF supports the spec's variable size of EID, where the actual size is
 *      selected at runtime, and therefore the size in CFDP PDUs may be smaller
 *      than the size specified here.  This type only establishes the maximum
 *      size (and therefore maximum value) that an EID may be.
 *
 * @note This type is used in several CF commands, and so changing the size
 *       of this type will affect the following structs:
 *        CF_ConfigTable_t, configuration table - will change size of file
 *        CF_ConfigPacket_t, set config params command
 *        CF_TxFileCmd_t, transmit file command
 *        CF_PlaybackDirCmd_t, equivalent to above
 *        CF_Transaction_Payload_t, any command that selects a transaction based on EID
 *
 * @par Limits
 *         Must be one of uint8, uint16, uint32, uint64.
 */
typedef uint32 CF_EntityId_t;

/**
 * @brief transaction sequence number size
 *
 * @par Description:
 *      The max size of the transaction sequence number as expected for all CFDP packets.
 *      CF supports the spec's variable size of TSN, where the actual size is
 *      selected at runtime, and therefore the size in CFDP PDUs may be smaller
 *      than the size specified here.  This type only establishes the maximum
 *      size (and therefore maximum value) that a TSN may be.
 *
 * @note This type is used in several CF commands, and so changing the size
 *       of this type will affect the following structure:
 *        CF_Transaction_Payload_t, any command that selects a transaction based on TSN
 *
 * @par Limits
 *         Must be one of uint8, uint16, uint32, uint64.
 */
typedef uint32 CF_TransactionSeq_t;

#endif

#endif /* CF_EXTERN_TYPEDEFS_H */
