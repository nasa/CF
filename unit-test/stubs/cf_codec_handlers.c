/************************************************************************
*************************************************************************/

#include "cf_test_utils.h"
#include "cf_app.h"

#include <string.h>

/* UT includes */
#include "uttest.h"
#include "utstubs.h"

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CFDP_CodecCheckSize
 *
 * Translates return value into the correct size for returning
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_CodecCheckSize(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool  retval;
    int32 status_code;

    if (UT_Stub_GetInt32StatusCode(Context, &status_code))
    {
        retval = status_code;
    }
    else
    {
        retval = false;
    }

    UT_Stub_SetReturnValue(FuncKey, retval);
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CFDP_DoEncodeChunk
 *
 * Default always returns NULL, an alt handler can be registered for other pointer returns
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_DoEncodeChunk(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void *retval;

    /* This may not need to do anything else, it shouldn't be called outside of this module */
    retval = NULL;

    UT_Stub_SetReturnValue(FuncKey, retval);
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CFDP_DoDecodeChunk
 *
 * Default always returns NULL, an alt handler can be registered for other pointer returns
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_DoDecodeChunk(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    const void *retval;

    /* This may not need to do anything else, it shouldn't be called outside of this module */
    retval = NULL;

    UT_Stub_SetReturnValue(FuncKey, retval);
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_CFDP_GetValueEncodedSize
 *
 * Translates return value into the correct size for returning
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_CFDP_GetValueEncodedSize(void *UserObj, UT_EntryKey_t FuncKey,
                                                   const UT_StubContext_t *Context)
{
    uint8 retval;
    int32 status_code;

    if (UT_Stub_GetInt32StatusCode(Context, &status_code))
    {
        retval = status_code;
    }
    else
    {
        /* this defaults to 1 since nothing can get encoded in a size of 0.
         * test case can still set a different value, of course. */
        retval = 1;
    }

    UT_Stub_SetReturnValue(FuncKey, retval);
}

/*----------------------------------------------------------------
 *
 * Function: UT_DefaultHandler_CF_DecodeIntegerInSize
 *
 * Translates return value into the correct size for returning
 *
 *-----------------------------------------------------------------*/
void UT_DefaultHandler_CF_DecodeIntegerInSize(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint64 retval;
    int32  status_code;

    if (UT_Stub_GetInt32StatusCode(Context, &status_code))
    {
        retval = status_code;
    }
    else
    {
        retval = 0;
    }

    UT_Stub_SetReturnValue(FuncKey, retval);
}
