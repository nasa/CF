/************************************************************************
** File: cf_timer_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application timer stubs file
**
**  A timer in CF is really just a structure that holds a counter that
**  indicates the timer expired when it reaches 0. The goal is that
**  any timer is driven by the scheduler ticks. There is no reason
**  we need any finer grained resolution than this for CF.
**
** Revision 1.0 2020/08/04 asgibso1
**  Initial revision
*************************************************************************/

#include "cfe.h"
#include "cf_verify.h"
#include "cf_timer.h"
#include "cf_app.h"
#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"

/* NOTE: sub-second resolution is not required */

/************************************************************************/
/** \brief Converts seconds into scheduler ticks.
**
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retstmt Number of ticks for the given seconds. \endcode
**  \endreturns
**
*************************************************************************/
// static uint32 CF_Timer_Sec2Ticks(cf_timer_sec_t sec)
// {
//     UtPrintf("NOT YET IMPLEMENTED stub in \n%s:line #%d\n", 
//       __FILE__, __LINE__);
//     exit(-86);
// }

/************************************************************************/
/** \brief Initialize a timer with a relative number of seconds.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_Timer_InitRelSec(cf_timer_t *t, uint32 rel_sec)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_Timer_InitRelSec), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_Timer_InitRelSec), &rel_sec, sizeof(rel_sec));

    UT_DEFAULT_IMPL(CF_Timer_InitRelSec);
}

/************************************************************************/
/** \brief Check if a timer has expired.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
**  \returns
**  \retstmt 1 if expired; otherwise 0. \endcode
**  \endreturns
**
*************************************************************************/
int CF_Timer_Expired(const cf_timer_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_Timer_Expired), &t, sizeof(t));
    
    return (int)UT_DEFAULT_IMPL(CF_Timer_Expired);
}

/************************************************************************/
/** \brief Notify a timer object a tick has occurred.
**  
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_Timer_Tick(cf_timer_t *t)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_Timer_Tick), &t, sizeof(t));
    
    UT_DEFAULT_IMPL(CF_Timer_Tick);
}
