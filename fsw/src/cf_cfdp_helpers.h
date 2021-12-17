/************************************************************************
** File: cf_cfdp_helpers.h
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
**  The CF Application cfdp helper functions header file
**
**
**
*************************************************************************/

#ifndef CF_CFDP_HELPERS__H
#define CF_CFDP_HELPERS__H

/* this file is intended to be included by cf_cfdp.h */

/* the "pdu" header defines the CFDP protocol types, e.g. CF_CFDP_uint[8/16/32/64]_t */
#include "cf_cfdp_pdu.h"

#if ENDIAN == _EL
#define CF_BSWAP16(x) ((uint16)((((x) >> 8) & 0xff) | (((x)&0xff) << 8)))
#define CF_BSWAP32(x)                                                                           \
    ((uint32)((((x)&0xff000000u) >> 24) | (((x)&0x00ff0000u) >> 8) | (((x)&0x0000ff00u) << 8) | \
              (((x)&0x000000ffu) << 24)))
#define CF_BSWAP64(x)                                                                                                  \
    ((uint64)((((x)&0xff00000000000000) >> 56) | (((x)&0x00ff000000000000) >> 48) | (((x)&0x0000ff0000000000) >> 40) | \
              (((x)&0x000000ff00000000) >> 32) | (((x)&0xff000000u) >> 24) | (((x)&0x00ff0000u) >> 8) |                \
              (((x)&0x0000ff00u) << 8) | (((x)&0x000000ffu) << 24)))
#define CF_HTOBE16(x) CF_BSWAP16(x)
#define CF_HTOBE32(x) CF_BSWAP32(x)
#define CF_HTOBE64(x) CF_BSWAP64(x)
#define CF_BE16TOH(x) CF_BSWAP16(x)
#define CF_BE32TOH(x) CF_BSWAP32(x)
#define CF_BE64TOH(x) CF_BSWAP64(x)
#elif ENDIAN == _EB
#define CF_HTOBE16(x) (x)
#define CF_HTOBE32(x) (x)
#define CF_HTOBE64(x) (x)
#define CF_BE16TOH(x) (x)
#define CF_BE32TOH(x) (x)
#define CF_BE64TOH(x) (x)
#endif

/* NOTE: ldst stands for load/store */
#if defined(CF_HW_ALIGNMENT)
#define DECL_LDST(type, dst, src, efn) \
    do                                 \
    {                                  \
        (dst) = efn((src));            \
    } while (0)
#define DECL_LDST_NOSWAP(type, dst, src) \
    do                                   \
    {                                    \
        (dst) = (src);                   \
    } while (0)
#else
#define DECL_LDST(type, dst, src, efn)             \
    do                                             \
    {                                              \
        type t_____s = efn((src));                 \
        memcpy(&(dst), &t_____s, sizeof(t_____s)); \
    } while (0)
#define DECL_LDST_NOSWAP(type, dst, src)           \
    do                                             \
    {                                              \
        type t_____s = (src);                      \
        memcpy(&(dst), &t_____s, sizeof(t_____s)); \
    } while (0)
#endif

/* NOTE: get/set will handle endianess */
/*
 * ALSO NOTE: These store/set inline functions/macros are used with
 * literal integers as well as variables.  So they operate on value, where
 * the load/get functions operate by reference
 */
static inline void cfdp_store_uint8(CF_CFDP_uint8_t *pdst, uint8 val)
{
    pdst->octets[0] = val;
}
#define cfdp_set_uint8(dst, src) cfdp_store_uint8(&(dst), src)

static inline void cfdp_store_uint16(CF_CFDP_uint16_t *pdst, uint16 val)
{
    pdst->octets[1] = val & 0xFF;
    val >>= 8;
    pdst->octets[0] = val & 0xFF;
}
#define cfdp_set_uint16(dst, src) cfdp_store_uint16(&(dst), src)

static inline void cfdp_store_uint32(CF_CFDP_uint32_t *pdst, uint32 val)
{
    pdst->octets[3] = val & 0xFF;
    val >>= 8;
    pdst->octets[2] = val & 0xFF;
    val >>= 8;
    pdst->octets[1] = val & 0xFF;
    val >>= 8;
    pdst->octets[0] = val & 0xFF;
}
#define cfdp_set_uint32(dst, src) cfdp_store_uint32(&(dst), src)

static inline void cfdp_store_uint64(CF_CFDP_uint64_t *pdst, uint64 val)
{
    pdst->octets[7] = val & 0xFF;
    val >>= 8;
    pdst->octets[6] = val & 0xFF;
    val >>= 8;
    pdst->octets[5] = val & 0xFF;
    val >>= 8;
    pdst->octets[4] = val & 0xFF;
    val >>= 8;
    pdst->octets[3] = val & 0xFF;
    val >>= 8;
    pdst->octets[2] = val & 0xFF;
    val >>= 8;
    pdst->octets[1] = val & 0xFF;
    val >>= 8;
    pdst->octets[0] = val & 0xFF;
}
#define cfdp_set_uint64(dst, src) cfdp_store_uint64(&(dst), src)

static inline void cfdp_load_uint8(uint8 *pdst, const CF_CFDP_uint8_t *psrc)
{
    *pdst = psrc->octets[0];
}
#define cfdp_get_uint8(dst, src) cfdp_load_uint8(&(dst), &(src))

static inline void cfdp_load_uint16(uint16 *pdst, const CF_CFDP_uint16_t *psrc)
{
    uint16 val = 0;

    val |= psrc->octets[0];
    val <<= 8;
    val |= psrc->octets[1];

    *pdst = val;
}
#define cfdp_get_uint16(dst, src) cfdp_load_uint16(&(dst), &(src))

static inline void cfdp_load_uint32(uint32 *pdst, const CF_CFDP_uint32_t *psrc)
{
    uint32 val = 0;

    val |= psrc->octets[0];
    val <<= 8;
    val |= psrc->octets[1];
    val <<= 8;
    val |= psrc->octets[2];
    val <<= 8;
    val |= psrc->octets[3];

    *pdst = val;
}
#define cfdp_get_uint32(dst, src) cfdp_load_uint32(&(dst), &(src))

static inline void cfdp_load_uint64(uint64 *pdst, const CF_CFDP_uint64_t *psrc)
{
    uint64 val = 0;

    val |= psrc->octets[0];
    val <<= 8;
    val |= psrc->octets[1];
    val <<= 8;
    val |= psrc->octets[2];
    val <<= 8;
    val |= psrc->octets[3];
    val <<= 8;
    val |= psrc->octets[4];
    val <<= 8;
    val |= psrc->octets[5];
    val <<= 8;
    val |= psrc->octets[6];
    val <<= 8;
    val |= psrc->octets[7];

    *pdst = val;
}
#define cfdp_get_uint64(dst, src) cfdp_load_uint64(&(dst), &(src))

#define cfdp_ldst_uint8(dst, src) \
    do                            \
    {                             \
        (dst) = (src);            \
    } while (0)
#define cfdp_ldst_uint16(dst, src) DECL_LDST_NOSWAP(uint16, dst, src)
#define cfdp_ldst_uint32(dst, src) DECL_LDST_NOSWAP(uint32, dst, src)
#define cfdp_ldst_uint64(dst, src) DECL_LDST_NOSWAP(uint64, dst, src)

extern uint8 CF_GetNumberMinSize(uint64 Value);
extern void  CF_CopyNumberMinSize(void *DestBuffer, uint64 Value, uint8 Size);

#endif /* !CF_CFDP_HELPERS__H */
