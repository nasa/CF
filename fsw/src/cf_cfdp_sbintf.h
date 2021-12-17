/************************************************************************
** File: cf_cfdp_sbintf.h
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
**
*************************************************************************/

#ifndef CF_CFDP_SBINTF_H
#define CF_CFDP_SBINTF_H

#include "cf_cfdp_types.h"

CF_Logical_PduBuffer_t *CF_CFDP_MsgOutGet(const CF_Transaction_t *t, bool silent);
void                    CF_CFDP_Send(uint8 chan_num, const CF_Logical_PduBuffer_t *ph);
void                    CF_CFDP_ReceiveMessage(CF_Channel_t *c);

#endif /* !CF_CFDP_SBINTF_H */
