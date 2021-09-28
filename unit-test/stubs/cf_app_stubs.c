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

CF_AppData_t CF_AppData;

/************************************************************************/
/** \brief Send CF housekeeping packet
**  
**  \par Description
**       The command to send the CF housekeeping packet comes in on
**       the software bus. This function sends the message.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static void CF_HkCmd(void)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Checks to see if a table update is pending, and perform it.
**  
**  \par Description
**       Updates the table if the engine is disabled.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static void CF_CheckTables(void)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Validation function for config table.
**  
**  \par Description
**       Checks that the config table being loaded has correct data.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int32 CF_ValidateConfigTable(void *tbl_ptr)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Load the table on application start
**  
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int32 CF_TableInit(void)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief CF app init function
**  
**  \par Description
**       Initializes all aspects of the CF application. Messages,
**       pipes, events, table, and the cfdp engine.
**
**  \par Assumptions, External Events, and Notes:
**       This must only be called once.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
static int32 CF_Init(void)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief CF wakeup function
**  
**  \par Description
**       Performs a single engine cycle for each wakeup
**
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
static void CF_WakeUp(void)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief CF message processing function
**  
**  \par Description
**       Initializes all aspects of the CF application. Messages,
**       pipes, events, table, and the cfdp engine.
**
**  \par Assumptions, External Events, and Notes:
**       msg must not be NULL.
**
*************************************************************************/
static void CF_ProcessMsg(CFE_SB_Buffer_t msg)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief CF app entry point
**  
**  \par Description
**       Main entry point of CF application.
**       Calls the init function and manages the app run loop.
**
**  \par Assumptions, External Events, and Notes:
**       This must only be called once.
**
*************************************************************************/
void CF_AppMain(void)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

