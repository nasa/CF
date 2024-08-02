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

#include <stdlib.h>
#include <math.h>

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"

/* CF includes */
#include "cf_test_utils.h"

#define HEADS true
#define TAILS false

uint16 UT_CF_CapturedEventIDs[4];

void *UT_CF_GetContextBufferImpl(UT_EntryKey_t FuncKey, size_t ReqSize)
{
    void * TempPtr;
    size_t ActualSize;
    size_t Position;
    void * Val;

    UT_GetDataBuffer(FuncKey, &TempPtr, &ActualSize, &Position);

    if (TempPtr != NULL && (ActualSize % ReqSize) != 0)
    {
        UtAssert_Failed("Setup Error: Actual context buffer size (%lu) does not match required size (%lu)",
                        (unsigned long)ActualSize, (unsigned long)ReqSize);
        TempPtr = NULL;
    }

    if (TempPtr != NULL)
    {
        TempPtr = ((uint8 *)TempPtr) + Position;

        /* This is just a roundabout way to increment the position, because
         * UT assert does not currently have an API call to do so */
        while (ReqSize >= sizeof(Val))
        {
            UT_Stub_CopyToLocal(FuncKey, &Val, sizeof(Val));
            ReqSize -= sizeof(Val);
        }

        if (ReqSize > 0)
        {
            UT_Stub_CopyToLocal(FuncKey, &Val, ReqSize);
        }
    }

    return TempPtr;
}

void UT_CF_ResetEventCapture(void)
{
    /* set up the buffer to store the generated event ID */
    memset(UT_CF_CapturedEventIDs, 0, sizeof(UT_CF_CapturedEventIDs));
    UT_ResetState(UT_KEY(CFE_EVS_SendEvent));
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), UT_CF_CapturedEventIDs, sizeof(UT_CF_CapturedEventIDs), false);
}

void UT_CF_CheckEventID_Impl(uint16 ExpectedID, const char *EventIDStr)
{
    /* check if the event exists anywhere in the buffer */
    bool   found;
    uint16 i;

    found = false;
    for (i = 0; !found && i < (sizeof(UT_CF_CapturedEventIDs) / sizeof(UT_CF_CapturedEventIDs[0])); ++i)
    {
        found = (UT_CF_CapturedEventIDs[i] == ExpectedID);
    }

    UtAssert_True(found, "Generated event: %s (%u)", EventIDStr, (unsigned int)ExpectedID);
}

/******************************************************************************
 * The Setup and Teardown
 ******************************************************************************/

void cf_tests_Setup(void)
{
    UT_ResetState(0);

    /* Reset CFE_EVS_SendEvent capture */
    UT_CF_ResetEventCapture();

    memset(&CF_AppData, 0, sizeof(CF_AppData));
}

void cf_tests_Teardown(void)
{
    /* do nothing by design */
}

/******************************************************************************
 * ut utility functions that we may want to add to ut_assert
 ******************************************************************************/

void TestUtil_InitializeRandomSeed(void)
{
    int seed = RANDOM_VALUES_SEED;

    if (seed > 1)
    {
        srand(RANDOM_VALUES_SEED);
    }
    else
    {
        seed = time(NULL);
        srand(seed);
    }

    UtPrintf("RANDOM_VALUES_SEED = %d\n", seed);
}

/******************************************************************************
 * The 'Any' randomized values section
 ******************************************************************************/

bool Any_bool(void)
{
    bool random_bool = false;

    if (AnyCoinFlip())
    {
        random_bool = true;
    }

    return random_bool;
}

unsigned int AnyCoinFlip(void)
{
    return rand() % 2;
}

char Any_char(void)
{
    return (char)Any_uint8();
}

unsigned int Any_unsigned_int(void)
{
    int random_val = rand();

    return random_val;
}

/*
 * AnyBufferOf_uint8_WithSize works, but name/design could use overhaul
 * should it return a pointer? or should it be named different because it alters
 * the buffer given?
 */
void AnyBufferOf_uint8_WithSize(uint8 *buffer, size_t size)
{
    int i;

    for (i = 0; i < size; ++i)
    {
        *(buffer + (sizeof(uint8) * i)) = Any_uint8();
    }
}

uint8 Any_0_or_1(void)
{
    return Any_uint8_LessThan(2);
}

uint8 Any_uint8(void)
{
    return (uint8)Any_unsigned_int();
}

uint8 Any_uint8_ExceptSetBits(uint8 bits)
{
    uint8 random_value = bits;
    uint8 max_tries    = 10;
    uint8 num_tries    = 0;

    while ((random_value & bits) == bits)
    {
        if (num_tries == max_tries)
        {
            UtPrintf("Any_uint8_ExceptSetBits unable to get valid number in %u checks\n", num_tries);
            UtAssert_Abort("Any_uint8_ExceptSetBits failed to get valid number");
        }
        else
        {
            ++num_tries;
        }

        random_value = Any_uint8();
    }

    return random_value;
}

uint8 Any_uint8_ExceptUnsetBits(uint8 bits)
{
    uint8 random_value = bits;
    uint8 max_tries    = 10;
    uint8 num_tries    = 0;

    while ((random_value | bits) == bits)
    {
        if (num_tries == max_tries)
        {
            UtPrintf("Any_uint8_ExceptUnsetBits unable to get valid number in %u checks\n", num_tries);
            UtAssert_Abort("Any_uint8_ExceptUnsetBits failed to get valid number");
        }
        else
        {
            ++num_tries;
        }

        random_value = Any_uint8();
    }

    return random_value;
}

uint8 Any_uint8_FromThese(uint8 values[], uint8 num_values)
{
    uint8 random_index = rand() % num_values;
    uint8 random_value = values[random_index];

    return random_value;
}

uint8 Any_uint8_BetweenExcludeMax(uint8 floor, uint8 ceiling)
{
    uint8 random_val;
    uint8 diff = ceiling - floor;

    random_val = (rand() % diff) + floor;

    return random_val;
}

uint8 Any_uint8_BetweenInclusive(uint8 floor, uint8 ceiling)
{
    uint8 random_val;
    uint8 diff = ceiling - floor + 1; /* +1 for inclusive */

    random_val = (rand() % diff) + floor;

    return random_val;
}

uint8 Any_uint8_LessThan(uint8 ceiling)
{
    uint8 random_val;

    random_val = rand() % ceiling;

    return random_val;
}

uint8 Any_uint8_GreaterThan(uint8 floor)
{
    uint8 random_val;

    random_val = 255 - (rand() % (256 - floor - 1)); /* -1 for non-inclusive */

    return random_val;
}

uint8 Any_uint8_GreaterThan_or_EqualTo(uint8 floor)
{
    uint8 random_val;

    random_val = 255 - (rand() % (256 - floor));

    return random_val;
}

uint8 Any_uint8_Except(uint8 exception)
{
    uint8 random_val = exception;

    while (random_val == exception)
    {
        random_val = Any_uint8();
    }

    return random_val;
}

uint16 Any_uint16(void)
{
    return (uint16)Any_unsigned_int();
}

uint16 Any_uint16_BetweenExcludeMax(uint16 floor, uint16 ceiling)
{
    uint16 difference = ceiling - floor;

    return (uint16)((rand() % difference) + floor);
}

uint16 Any_uint16_Except(uint16 exception)
{
    uint32 random_val = exception;

    while (random_val == exception)
    {
        random_val = Any_uint16();
    }

    return random_val;
}

uint16 Any_uint16_GreaterThan(uint16 floor)
{
    uint16 random_val;

    random_val = 65535 - (rand() % (65536 - floor - 1)); /* -1 for non-inclusive */

    return random_val;
}

uint16 Any_uint16_LessThan(uint16 ceiling)
{
    return rand() % ceiling;
}

uint32 Any_uint32(void)
{
    return Any_unsigned_int();
}

uint32 Any_uint32_BetweenInclusive(uint32 min, uint32 max)
{
    uint32 difference = max - min + 1; /* +1 for max inclusive */

    return (uint32)((rand() % difference) + min);
}

uint32 Any_uint32_BetweenExcludeMax(uint32 min, uint32 max)
{
    uint32 difference = max - min;

    /* check that rand % 0 does not happen */
    if (difference == 0)
    {
        UtAssert_Abort(__func__);
    }

    return (uint32)((rand() % difference) + min);
}

uint32 Any_uint32_Except(uint32 exception)
{
    uint32 random_val = exception;

    while (random_val == exception)
    {
        random_val = Any_unsigned_int();
    }

    return random_val;
}

int32 Any_int32_LessThan(int32 ceiling)
{
    int32 random_val;
    int32 new_ceiling;

    if (ceiling > 0)
    {
        random_val = (rand() % ceiling) - rand();
    }
    else
    {
        new_ceiling = abs(INT32_MIN - ceiling);
        random_val  = ceiling - (rand() % new_ceiling);
    }

    return random_val;
}

uint32 Any_uint32_GreaterThan(uint32 floor)
{
    uint32 random_val;
    uint32 diff = UINT32_MAX - floor; /* floor not inclusive */

    random_val = (rand() % diff) + floor + 1; /* +1 for floor not inclusive */

    return random_val;
}

uint32 Any_uint32_LessThan(uint32 ceiling)
{
    return rand() % ceiling;
}

uint32 Any_uint32_LessThan_or_EqualTo(uint32 max)
{
    return rand() % (max + 1);
}

int Any_int(void)
{
    int random_val = rand() % MAX_INT;

    bool coin_toss = rand() % 2;

    if (coin_toss == HEADS)
    {
        /* 0 to INT_MAX becomes -1 to INT_MIN */
        random_val *= -1; /* flip sign */
        random_val += -1; /* subtract 1, 0 becomes -1 */
    }

    return random_val;
}

int Any_int_Except(int exception)
{
    int random_val = exception;

    while (random_val == exception)
    {
        random_val = Any_int();
    }

    return random_val;
}

int Any_int_Negative(void)
{
    return ((rand() % MAX_INT) * -1) - 1; /* *-1 flips sign, -1 for 0 and MIN_INT */
}

int Any_int_Positive(void)
{
    return (rand() % (UINT16_MAX)) + 1; /* 0 to RAND_MAX - 1, then +1 for 1 to RAND_MAX */
}

int Any_int_PositiveExcept(int exception)
{
    int rand_val = exception;

    while (rand_val == exception)
    {
        rand_val = rand();
    }

    return rand_val;
}

int Any_int_ZeroOrPositiveLessThan(int ceiling)
{
    return rand() % ceiling;
}

int32 Any_int32(void)
{
    return (int32)Any_int();
}

int32 Any_int32_Except(int32 exception)
{
    return (int32)Any_int_Except(exception);
}

int32 Any_int32_Negative(void)
{
    return Any_int_Negative();
}

int32 Any_int32_ZeroOrPositive()
{
    int32 random_val = rand() % INT32_MAX;

    return random_val;
}

uint64 Any_uint64()
{
    uint64 rand_val;

    uint64 left  = (uint64)Any_uint32() << 32;
    uint64 right = Any_uint32();
    rand_val     = left | right;

    return rand_val;
}

uint64 Any_uint64_Except(uint64 exception)
{
    uint64 rand_val;

    rand_val = exception;

    while (rand_val == exception)
    {
        rand_val = Any_uint64();
    }

    return rand_val;
}

char random_length_string[500];

char *AnyFilenameOfLength(size_t length)
{
    return AnyRandomStringOfLettersOfLength(length);
}

char *AnyRandomStringOfTextOfLength(size_t length)
{
    size_t i;
    int    value;

    for (i = 0; i < length; ++i)
    {
        value                   = 32 + (rand() % 95); /* ASCII 32 to 126 */
        random_length_string[i] = (char)value;
    }
    random_length_string[i] = '\0';

    return random_length_string;
}

char *AnyRandomStringOfLettersOfLength(size_t length)
{
    size_t i;
    int    value;

    for (i = 0; i < length; ++i)
    {
        value = 65 + (rand() % 26); /* ASCII 65 to 91 */

        if (AnyCoinFlip() == HEADS)
        {
            value += 32;
        }

        random_length_string[i] = (char)value;
    }
    random_length_string[i] = '\0';

    return random_length_string;
}

void AnyRandomStringOfLettersOfLengthCopy(char *random_string, size_t length)
{
    size_t i;
    int    value;

    for (i = 0; i < (length - 1); ++i)
    {
        value = 65 + (rand() % 26); /* ASCII 65 to 91 */

        if (AnyCoinFlip() == HEADS)
        {
            value += 32;
        }

        random_string[i] = (char)value;
    }
    random_string[i] = '\0';
}

/* cfe specific */
void Any_CFE_TIME_SysTime_Set(CFE_TIME_SysTime_t *fake_time)
{
    fake_time->Seconds    = Any_uint32();
    fake_time->Subseconds = Any_uint32();
}

CFE_Status_t Any_CFE_Status_t_Negative()
{
    return Any_int32_Negative();
}

CFE_Status_t Any_CFE_Status_t_Except(CFE_Status_t exception)
{
    CFE_Status_t rand_val = exception;

    /* technically could continue forever, but it won't */
    while (rand_val == exception)
    {
        rand_val = Any_int32();
    }

    return rand_val;
}

/* cf specific */
uint8 Any_cf_chan_num(void)
{
    return Any_uint8_LessThan(CF_NUM_CHANNELS);
}
