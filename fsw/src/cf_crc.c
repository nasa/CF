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
 *  The CF Application CRC calculation source file
 *
 *  This is a streaming CRC calculator. Data can all be given at once for
 *  a result or it can trickle in.
 *
 *  This file is intended to be generic and usable by other apps.
 */

#include "cfe.h"
#include "cf_verify.h"
#include "cf_crc.h"
#include <string.h>

/*----------------------------------------------------------------
 *
 * Function: CF_CRC_Start
 *
 * Application-scope internal function
 * See description in cf_crc.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CRC_Start(CF_Crc_t *c)
{
    memset(c, 0, sizeof(*c));
}

/*----------------------------------------------------------------
 *
 * Function: CF_CRC_Digest
 *
 * Application-scope internal function
 * See description in cf_crc.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CRC_Digest(CF_Crc_t *c, const uint8 *data, int len)
{
    int i = 0;

    for (; i < len; ++i)
    {
        c->working <<= 8;
        c->working |= data[i];

        if (++c->index == 4)
        {
            c->result += c->working;
            c->index = 0;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CRC_Finalize
 *
 * Application-scope internal function
 * See description in cf_crc.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CRC_Finalize(CF_Crc_t *c)
{
    if (c->index)
    {
        c->result += (c->working << (8 * (4 - c->index)));

        /* set the index to 0 in case the user calls CF_CRC_Digest() again. It
         * will add the new data to the CRC result. This lets the user get
         * the current result in the stream if they want. */
        c->index   = 0;
        c->working = 0;
    }
}
