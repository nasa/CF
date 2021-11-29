/************************************************************************
** File: cf_fields.h
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
**  The CF Application field macro header file
**
**
**
*************************************************************************/
#ifndef CF_FIELD_COMMON__H
#define CF_FIELD_COMMON__H

#define xstr(s) str(s)
#define str(s)  #s

#include <stdint.h>

#define inc_subfield(TYPE, field, mask, shift) \
    do                                         \
    {                                          \
        TYPE old = *(field) & (mask);          \
        old >>= (shift);                       \
        ++old;                                 \
        old &= (mask);                         \
        old <<= (shift);                       \
        *(field) &= ~(mask);                   \
        *(field) |= old;                       \
    } while (0)

typedef struct CF_FIELD_FIELD
{
    uint32 shift;
    uint32 mask;
} CF_FIELD_FIELD;

/* NBITS == number of bits */

#define DECLARE_FIELD(NAME, NBITS, SHIFT) \
    static const CF_FIELD_FIELD NAME = {.shift = (SHIFT), .mask = ((1 << NBITS) - 1)};
#define FIELD_GET_VAL(SRC, NAME) (((SRC) >> (NAME).shift) & ((NAME).mask))
#define FIELD_SET_VAL(DEST, NAME, VAL)                         \
    do                                                         \
    {                                                          \
        (DEST) &= ~(((NAME).mask) << ((NAME).shift));          \
        (DEST) |= (((VAL) & ((NAME).mask)) << ((NAME).shift)); \
    } while (0)
#define FIELD_ADD_VAL(DEST, NAME, VAL)            \
    do                                            \
    {                                             \
        uint32 noise = FIELD_GET_VAL(DEST, NAME); \
        noise += VAL;                             \
        FIELD_SET_VAL(DEST, NAME, noise);         \
    } while (0)
#define FIELD_INC_VAL(DEST, NAME) FIELD_ADD_VAL(DEST, NAME, 1)

/* FGV, FSV, and FAV are just simple shortenings of the field macros.
 *
 * FGV == field get val
 * FSV == field set val
 * FAV == field add val
 */

#define FGV FIELD_GET_VAL
#define FSV FIELD_SET_VAL
#define FAV FIELD_ADD_VAL

#endif /* !CF_FIELD_COMMON__H */
