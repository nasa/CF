/************************************************************************
** File: cf_crc.h
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
**  The CF Application CRC calculation header file
**
**
**
*************************************************************************/

#ifndef CF_CRC_H
#define CF_CRC_H

#include "cfe.h"

typedef struct CF_Crc
{
    uint32 working;
    uint32 result;
    uint8  index;
} CF_Crc_t;

extern void CF_CRC_Start(CF_Crc_t *c);
extern void CF_CRC_Digest(CF_Crc_t *c, const uint8 *data, int len);
extern void CF_CRC_Finalize(CF_Crc_t *c);

#endif /* !CF_CRC_H */
