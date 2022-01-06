/************************************************************************
** File: cf_app_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application main application stubs file for unit tests
**
**  This file contains the stubs for the functions that initialize the
**  application and link all logic and functionality to the CFS.
**
** Revision 1.0 2020/08/04 agibso1
**  Initial revision
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_app.h"
#include "cf_events.h"
#include "cf_perfids.h"
#include "cf_cfdp.h"

#include <string.h>

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "uttools.h"

CF_AppData_t CF_AppData;
