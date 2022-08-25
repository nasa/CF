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
    CF_Crc_t c;

    memset(&c, 0xFF, sizeof(c));

    /* Act */
    UtAssert_VOIDCALL(CF_CRC_Start(&c));

    /* Assert */
    UtAssert_ZERO(c.working);
    UtAssert_ZERO(c.result);
    UtAssert_ZERO(c.index);
}

void Test_CF_CRC_Digest(void)
{
    CF_Crc_t c;
    uint8    data[] = {1, 2, 3, 4, 5};

    /* Already tested, so OK to use */
    CF_CRC_Start(&c);

    /* Zero length should leave c as zeros */
    UtAssert_VOIDCALL(CF_CRC_Digest(&c, NULL, 0));
    UtAssert_ZERO(c.working);
    UtAssert_ZERO(c.result);
    UtAssert_ZERO(c.index);

    /* Digest data and confirm */
    UtAssert_VOIDCALL(CF_CRC_Digest(&c, data, sizeof(data)));
    UtAssert_UINT32_EQ(c.working, (data[1] << 24) + (data[2] << 16) + (data[3] << 8) + data[4]);
    UtAssert_UINT32_EQ(c.result, (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3]);
    UtAssert_UINT32_EQ(c.index, 1);
}

void Test_CF_CRC_Finalize(void)
{
    CF_Crc_t c;
    uint8    data[] = {1, 2, 3, 4, 5};

    /* Already tested, so OK to use */
    CF_CRC_Start(&c);

    /* Test with clear c */
    UtAssert_VOIDCALL(CF_CRC_Finalize(&c));
    UtAssert_ZERO(c.working);
    UtAssert_ZERO(c.result);
    UtAssert_ZERO(c.index);

    /* Already tested, so OK to use */
    CF_CRC_Digest(&c, data, sizeof(data));

    /* Test with filled in c */
    UtAssert_VOIDCALL(CF_CRC_Finalize(&c));
    UtAssert_ZERO(c.working);
    UtAssert_UINT32_EQ(c.result, ((data[0] + data[4]) << 24) + (data[1] << 16) + (data[2] << 8) + data[3]);
    UtAssert_ZERO(c.index);
}

void UtTest_Setup(void)
{
    TEST_CF_ADD(Test_CF_CRC_Start);
    TEST_CF_ADD(Test_CF_CRC_Digest);
    TEST_CF_ADD(Test_CF_CRC_Finalize);
}
