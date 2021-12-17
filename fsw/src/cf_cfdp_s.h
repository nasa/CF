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

#ifndef CF_CFDP_S_H
#define CF_CFDP_S_H

#include "cf_cfdp_types.h"

/* Engine functional dispatch. These are all implemented in cf_cfdp_r.c or cf_cfdp_s.c */
void CF_CFDP_S1_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_S2_Recv(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void CF_CFDP_S1_Tx(CF_Transaction_t *t);
void CF_CFDP_S2_Tx(CF_Transaction_t *t);
void CF_CFDP_S_Tick(CF_Transaction_t *t, int *cont);
void CF_CFDP_S_Tick_Nak(CF_Transaction_t *t, int *cont);
void CF_CFDP_S_Cancel(CF_Transaction_t *t);

/* Handler routines for send-file transactions */
/* These are not called from outside this module, but are declared here so they can be unit tested */
CF_SendRet_t CF_CFDP_S_SendEof(CF_Transaction_t *t);
void         CF_CFDP_S1_SubstateSendEof(CF_Transaction_t *t);
void         CF_CFDP_S2_SubstateSendEof(CF_Transaction_t *t);
int32        CF_CFDP_S_SendFileData(CF_Transaction_t *t, uint32 foffs, uint32 bytes_to_read, uint8 calc_crc);
void         CF_CFDP_S_SubstateSendFileData(CF_Transaction_t *t);
int          CF_CFDP_S_CheckAndRespondNak(CF_Transaction_t *t);
void         CF_CFDP_S2_SubstateSendFileData(CF_Transaction_t *t);
void         CF_CFDP_S_SubstateSendMetadata(CF_Transaction_t *t);
void         CF_CFDP_S_SubstateSendFinAck(CF_Transaction_t *t);
void         CF_CFDP_S2_EarlyFin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void         CF_CFDP_S2_Fin(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void         CF_CFDP_S2_Nak(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void         CF_CFDP_S2_Nak_Arm(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);
void         CF_CFDP_S2_WaitForEofAck(CF_Transaction_t *t, CF_Logical_PduBuffer_t *ph);

#endif /* !CF_CFDP_S_H */

#ifdef __cplusplus
static inline void CF_CFDP_S_Reset(CF_Transaction_t *t)
#endif
