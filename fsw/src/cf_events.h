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
 *
 ************************************************************************/

/**
 * @file
 *
 *  The CF Application event id definition header file
 */

#ifndef CF_EVENTS_H
#define CF_EVENTS_H

#define CF_EID_ERR_ASSERT 1

/* CF_INIT event IDs */

#define CF_EID_INF_INIT               20
#define CF_EID_ERR_INIT_TBL_CHECK_REL 21
#define CF_EID_ERR_INIT_TBL_CHECK_MAN 22
#define CF_EID_ERR_INIT_TBL_CHECK_GA  23
#define CF_EID_ERR_INIT_TBL_REG       24
#define CF_EID_ERR_INIT_TBL_LOAD      25
#define CF_EID_ERR_INIT_TBL_MANAGE    26
#define CF_EID_ERR_INIT_TBL_GETADDR   27
#define CF_EID_ERR_INIT_CMD_LENGTH    28
#define CF_EID_ERR_INIT_MSG_RECV      29
#define CF_EID_ERR_INIT_SEM           30
#define CF_EID_ERR_INIT_PIPE          31
#define CF_EID_ERR_INIT_SUB           32
#define CF_EID_ERR_INIT_TPS           33
#define CF_EID_ERR_INIT_CRC_ALIGN     34
#define CF_EID_ERR_INIT_OUTGOING_SIZE 35

/* CF_PDU event ids */
#define CF_EID_INF_PDU_MD_RECVD        40
#define CF_EID_ERR_PDU_SHORT_HEADER    41
#define CF_EID_ERR_PDU_LONG_VS_RECVD   42
#define CF_EID_ERR_PDU_MD_SHORT        43
#define CF_EID_ERR_PDU_INVALID_SRC_LEN 44
#define CF_EID_ERR_PDU_INVALID_DST_LEN 45
#define CF_EID_ERR_PDU_FD_SHORT        46
#define CF_EID_ERR_PDU_EOF_SHORT       47
#define CF_EID_ERR_PDU_ACK_SHORT       48
#define CF_EID_ERR_PDU_FIN_SHORT       49
#define CF_EID_ERR_PDU_NAK_SHORT       50
#define CF_EID_ERR_PDU_BAD_RX_MSG_SIZE 51
#define CF_EID_ERR_PDU_GET_EID_SIZE    52
#define CF_EID_ERR_PDU_GET_TSN_SIZE    53
#define CF_EID_ERR_PDU_FD_UNSUPPORTED  54
#define CF_EID_ERR_PDU_LARGE_FILE      55

/* CF_CFDP event ids (engine) */
#define CF_EID_ERR_CFDP_RX_DROPPED      60
#define CF_EID_ERR_CFDP_INVALID_DST_EID 61
#define CF_EID_ERR_CFDP_IDLE_MD         62
#define CF_EID_ERR_CFDP_FD_UNHANDLED    63
#define CF_EID_ERR_CFDP_MAX_CMD_TX      64
#define CF_EID_ERR_CFDP_OPENDIR         65
#define CF_EID_ERR_CFDP_DIR_SLOT        66
#define CF_EID_ERR_CFDP_NO_MSG          67
#define CF_EID_ERR_CFDP_CLOSE_ERR       68

/* CF_CFDP_R event ids (engine) */
#define CF_EID_INF_CFDP_R_REQUEST_MD    70
#define CF_EID_INF_CFDP_R_TEMP_FILE     71
#define CF_EID_ERR_CFDP_R_NAK_LIMIT     72
#define CF_EID_ERR_CFDP_R_ACK_LIMIT     73
#define CF_EID_ERR_CFDP_R_CRC           74
#define CF_EID_ERR_CFDP_R_SEEK_FD       75
#define CF_EID_ERR_CFDP_R_SEEK_CRC      76
#define CF_EID_ERR_CFDP_R_WRITE         77
#define CF_EID_ERR_CFDP_R_SIZE_MISMATCH 78
#define CF_EID_ERR_CFDP_R_PDU_EOF       79
#define CF_EID_ERR_CFDP_R_CREAT         80
#define CF_EID_ERR_CFDP_R_PDU_FINACK    81
#define CF_EID_ERR_CFDP_R_EOF_MD_SIZE   82
#define CF_EID_ERR_CFDP_R_RENAME        83
#define CF_EID_ERR_CFDP_R_OPEN          84
#define CF_EID_ERR_CFDP_R_PDU_MD        85
#define CF_EID_ERR_CFDP_R_READ          86
#define CF_EID_ERR_CFDP_R_DC_INV        87
#define CF_EID_ERR_CFDP_R_INACT_TIMER   88

/* CF_CFDP_S event ids (engine) */
#define CF_EID_INF_CFDP_S_START_SEND   90
#define CF_EID_ERR_CFDP_S_SEEK_FD      91
#define CF_EID_ERR_CFDP_S_READ         92
#define CF_EID_ERR_CFDP_S_SEND_FD      93
#define CF_EID_ERR_CFDP_S_ALREADY_OPEN 94
#define CF_EID_ERR_CFDP_S_OPEN         95
#define CF_EID_ERR_CFDP_S_SEEK_END     96
#define CF_EID_ERR_CFDP_S_SEEK_BEG     97
#define CF_EID_ERR_CFDP_S_SEND_MD      98
#define CF_EID_ERR_CFDP_S_PDU_FIN      99
#define CF_EID_ERR_CFDP_S_INVALID_SR   100
#define CF_EID_ERR_CFDP_S_PDU_NAK      101
#define CF_EID_ERR_CFDP_S_PDU_EOF      102
#define CF_EID_ERR_CFDP_S_EARLY_FIN    103
#define CF_EID_ERR_CFDP_S_DC_INV       104
#define CF_EID_ERR_CFDP_S_NON_FD_PDU   105
#define CF_EID_ERR_CFDP_S_ACK_LIMIT    106
#define CF_EID_ERR_CFDP_S_INACT_TIMER  107

/* CF_CMD event ids */
#define CF_EID_INF_CMD_NOOP             110
#define CF_EID_INF_CMD_RESET            111
#define CF_EID_INF_CMD_GETSET1          112
#define CF_EID_INF_CMD_GETSET2          113
#define CF_EID_ERR_CMD_RESET_INVALID    114
#define CF_EID_ERR_CMD_CHAN_PARAM       115
#define CF_EID_ERR_CMD_TRANS_NOT_FOUND  116
#define CF_EID_ERR_CMD_TSN_CHAN_INVALID 117
#define CF_EID_ERR_CMD_SUSPRES_SAME     118
#define CF_EID_ERR_CMD_SUSPRES_CHAN     119
#define CF_EID_ERR_CMD_POLLDIR_INVALID  120
#define CF_EID_ERR_CMD_PURGE_ARG        121
#define CF_EID_ERR_CMD_WQ_CHAN          122
#define CF_EID_ERR_CMD_WQ_ARGS          123
#define CF_EID_ERR_CMD_WQ_OPEN          124
#define CF_EID_ERR_CMD_WQ_WRITEQ_RX     125
#define CF_EID_ERR_CMD_WQ_WRITEHIST_RX  126
#define CF_EID_ERR_CMD_WQ_WRITEQ_TX     127
#define CF_EID_ERR_CMD_WQ_WRITEQ_PEND   128
#define CF_EID_ERR_CMD_WQ_WRITEHIST_TX  129
#define CF_EID_ERR_CMD_GETSET_VALIDATE  130
#define CF_EID_ERR_CMD_GETSET_PARAM     131
#define CF_EID_ERR_CMD_GETSET_CHAN      132
#define CF_EID_ERR_CMD_ENABLE_ENGINE    133
#define CF_EID_ERR_CMD_ENG_ALREADY_ENA  134
#define CF_EID_ERR_CMD_ENG_ALREADY_DIS  135
#define CF_EID_ERR_CMD_GCMD_LEN         136
#define CF_EID_ERR_CMD_GCMD_CC          137
#define CF_EID_ERR_CMD_WHIST_WRITE      138
#define CF_EID_ERR_CMD_BAD_PARAM        139

#endif /* !CF_EVENTS_H */
