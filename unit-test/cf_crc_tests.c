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

/* cf testing includes */
#include "cf_test_utils.h"

void Test_CF_CRC_Start(void)
{
    /* Arrange */
    CF_Crc_t crc;

    memset(&crc, 0xFF, sizeof(crc));

    /* Act */
    UtAssert_VOIDCALL(CF_CRC_Start(&crc));

    /* Assert */
    UtAssert_ZERO(crc.working);
    UtAssert_ZERO(crc.result);
    UtAssert_ZERO(crc.index);
}

void Test_CF_CRC_Digest(void)
{
    CF_Crc_t crc;
    uint8    data[] = {1, 2, 3, 4, 5};

    /* Already tested, so OK to use */
    CF_CRC_Start(&crc);

    /* Zero length should leave crc as zeros */
    UtAssert_VOIDCALL(CF_CRC_Digest(&crc, NULL, 0));
    UtAssert_ZERO(crc.working);
    UtAssert_ZERO(crc.result);
    UtAssert_ZERO(crc.index);

    /* Digest data and confirm */
    UtAssert_VOIDCALL(CF_CRC_Digest(&crc, data, sizeof(data)));
    UtAssert_UINT32_EQ(crc.working, (data[1] << 24) + (data[2] << 16) + (data[3] << 8) + data[4]);
    UtAssert_UINT32_EQ(crc.result, (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3]);
    UtAssert_UINT32_EQ(crc.index, 1);
}

void Test_CF_CRC_Finalize(void)
{
    CF_Crc_t crc;
    uint8    data[] = {1, 2, 3, 4, 5};

    /* Already tested, so OK to use */
    CF_CRC_Start(&crc);

    /* Test with clear crc */
    UtAssert_VOIDCALL(CF_CRC_Finalize(&crc));
    UtAssert_ZERO(crc.working);
    UtAssert_ZERO(crc.result);
    UtAssert_ZERO(crc.index);

    /* Already tested, so OK to use */
    CF_CRC_Digest(&crc, data, sizeof(data));

    /* Test with filled in crc */
    UtAssert_VOIDCALL(CF_CRC_Finalize(&crc));
    UtAssert_ZERO(crc.working);
    UtAssert_UINT32_EQ(crc.result, ((data[0] + data[4]) << 24) + (data[1] << 16) + (data[2] << 8) + data[3]);
    UtAssert_ZERO(crc.index);
}

void UtTest_Setup(void)
{
    TEST_CF_ADD(Test_CF_CRC_Start);
    TEST_CF_ADD(Test_CF_CRC_Digest);
    TEST_CF_ADD(Test_CF_CRC_Finalize);
}
