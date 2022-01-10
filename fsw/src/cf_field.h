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
 *  The CF Application field macro header file
 */

#ifndef CF_FIELD_COMMON_H
#define CF_FIELD_COMMON_H

#define xstr(s) str(s)
#define str(s)  #s

#include <stdint.h>

typedef struct CF_FIELD_FIELD
{
    uint32 shift;
    uint32 mask;
} CF_FIELD_FIELD;

/* NBITS == number of bits */
#ifdef CF_DO_DECLARE_FIELDS
#define DECLARE_FIELD(NAME, NBITS, SHIFT) \
    static const CF_FIELD_FIELD NAME = {.shift = (SHIFT), .mask = ((1 << NBITS) - 1)};
#else
#define DECLARE_FIELD(NAME, NBITS, SHIFT)
#endif

/*
 * All CFDP sub-fields are fewer than 8 bits in size
 */
static inline uint8 CF_FieldGetVal(const uint8 *src, uint8 shift, uint8 mask)
{
    return (*src >> shift) & mask;
}

static inline void CF_FieldSetVal(uint8 *dest, uint8 shift, uint8 mask, uint8 val)
{
    *dest &= ~(mask << shift);
    *dest |= ((val & mask) << shift);
}

/* FGV, FSV, and FAV are just simple shortenings of the field macros.
 *
 * FGV == field get val
 * FSV == field set val
 */

#define FGV(SRC, NAME)       CF_FieldGetVal((SRC).octets, (NAME).shift, (NAME).mask)
#define FSV(DEST, NAME, VAL) CF_FieldSetVal((DEST).octets, (NAME).shift, (NAME).mask, VAL)

#endif /* !CF_FIELD_COMMON_H */
