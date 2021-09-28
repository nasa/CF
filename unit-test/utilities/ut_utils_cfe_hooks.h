#ifndef _ut_utils_cfe_hooks_h_
#define _ut_utils_cfe_hooks_h_

/* cfe includes */
#include "common_types.h"

/* UT includes */
#include "utstubs.h"


int32 stub_reporter_hook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);
#endif