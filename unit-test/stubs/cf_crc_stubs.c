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
 ************************************************************************/

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cf_crc header
 */

#include "cf_crc.h"
#include "utgenstub.h"

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CRC_Digest()
 * ----------------------------------------------------
 */
void CF_CRC_Digest(CF_Crc_t *c, const uint8 *data, int len)
{
    UT_GenStub_AddParam(CF_CRC_Digest, CF_Crc_t *, c);
    UT_GenStub_AddParam(CF_CRC_Digest, const uint8 *, data);
    UT_GenStub_AddParam(CF_CRC_Digest, int, len);

    UT_GenStub_Execute(CF_CRC_Digest, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CRC_Finalize()
 * ----------------------------------------------------
 */
void CF_CRC_Finalize(CF_Crc_t *c)
{
    UT_GenStub_AddParam(CF_CRC_Finalize, CF_Crc_t *, c);

    UT_GenStub_Execute(CF_CRC_Finalize, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CRC_Start()
 * ----------------------------------------------------
 */
void CF_CRC_Start(CF_Crc_t *c)
{
    UT_GenStub_AddParam(CF_CRC_Start, CF_Crc_t *, c);

    UT_GenStub_Execute(CF_CRC_Start, Basic, NULL);
}
