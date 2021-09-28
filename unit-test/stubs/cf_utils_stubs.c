/************************************************************************
** File: cf_utils_stubs.c
**
** %LICENSE_START
** %LICENSE_STOP
**
** Purpose:
**  The CF Application general utility functions stubs file
**
**  Various odds and ends are put here.
**
** Revision 1.0 2020/05/01 sseeger
**  Initial revision
*************************************************************************/

#include "cf_app.h"
#include "cf_verify.h"
#include "cf_cfdp.h"
#include "cf_utils.h"
#include "cf_events.h"
#include "cf_perfids.h"

#include "cf_assert.h"

/* UT includes */
#include "uttest.h"
#include "utstubs.h"
#include "utgenstub.h"

#include "cf_test_utils.h"


typedef struct {
    int32 fd;
    int32 result;
    int32 counter;
} trav_arg_t;

typedef struct priority_arg_t {
    transaction_t *t; /* OUT: holds value of transaction with which to call CF_CList_InsertAfter on */
    uint8 priority; /* seeking this priority */
} priority_arg_t;

typedef struct {
    CF_TraverseAllTransactions_fn_t fn;
    void *context;
    int counter;
} traverse_all_args_t;

#define LINEBUF_LEN ((CF_FILENAME_MAX_LEN*2)+128)

/************************************************************************/
/** \brief Walks through a history queue and builds a human readable representation of it.
**  
**  \par Description
**       This function is used as both a list traversal function and a direct
**       function call.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL. context must not be NULL.
**
**  \returns
**  \retcode 1 when it's found, which terminates list traversal \endcode
**  \retcode 0 when it isn't found, which causes list traversal to continue \endcode
**  \endreturns
**
*************************************************************************/
static int CF_TraverseHistory(clist_node n, trav_arg_t *context)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Walk over all transactions and print information from their history.
**  
**  \par Assumptions, External Events, and Notes:
**       None
**
**  \returns
**  \retcode 1 when it's found, which terminates list traversal \endcode
**  \retcode 0 when it isn't found, which causes list traversal to continue \endcode
**  \endreturns
**
*************************************************************************/
static int CF_TraverseTransactions(clist_node n, trav_arg_t *context)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Write a transaction-based queue's transaction history to a file.
**  
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retstmt 0 on success; 1 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WriteQueueDataToFile(int32 fd, channel_t *c, cf_queue_index_t q)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_WriteQueueDataToFile), &fd, sizeof(fd));
    UT_Stub_CopyFromLocal(UT_KEY(CF_WriteQueueDataToFile), &c, sizeof(c));
    UT_Stub_CopyFromLocal(UT_KEY(CF_WriteQueueDataToFile), &q, sizeof(q));

    return UT_DEFAULT_IMPL(CF_WriteQueueDataToFile);
}

/************************************************************************/
/** \brief Write a history-based queue's transaction history to a file.
**  
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL.
**
**  \returns
**  \retstmt 0 on success; 1 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WriteHistoryQueueDataToFile(int32 fd, channel_t *c, direction_t dir)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_WriteHistoryQueueDataToFile), &fd, sizeof(fd));
    UT_Stub_CopyFromLocal(UT_KEY(CF_WriteHistoryQueueDataToFile), &c, sizeof(c));
    UT_Stub_CopyFromLocal(UT_KEY(CF_WriteHistoryQueueDataToFile), &dir, sizeof(dir));

    return UT_DEFAULT_IMPL(CF_WriteHistoryQueueDataToFile);
}

/************************************************************************/
/** \brief Searches for the first transaction with a lower priority than given.
**  
**  \par Description
**        that the config table being loaded has correct data.
**
**  \par Assumptions, External Events, and Notes:
**       node must not be NULL. context must not be NULL.
**
**  \returns
**  \retcode 1 when it's found, which terminates list traversal \endcode
**  \retcode 0 when it isn't found, which causes list traversal to continue \endcode
**  \endreturns
**
*************************************************************************/
static int CF_PrioSearch(clist_node node, void *context)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Insert a transaction into a priority sorted transaction queue.
**  
**  \par Description
**       This function works by walking the queue in reverse to find a
**       transaction with a higher priority than the given transaction.
**       The given transaction is then inserted after that one, since it
**       would be the next lower priority.
**
**  \par Assumptions, External Events, and Notes:
**       t must not be NULL.
**
*************************************************************************/
void CF_InsertSortPrio(transaction_t *t, cf_queue_index_t q)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_InsertSortPrio), &t, sizeof(t));
    UT_Stub_CopyFromLocal(UT_KEY(CF_InsertSortPrio), &q, sizeof(q));

    UT_DEFAULT_IMPL(CF_InsertSortPrio);
}

/************************************************************************/
/** \brief List traversal function performs operation on every active transaction.
**  
**  \par Description
**       Called on every transaction via list traversal. Calls another function
**       on that transaction.
**
**  \par Assumptions, External Events, and Notes:
**       n must not be NULL. args must not be NULL.
**
**  \returns
**  \retstmt Always 0 for do not exit early. \endcode
**  \endreturns
**
*************************************************************************/
static int CF_TraverseAllTransactions_(clist_node n, traverse_all_args_t *args)
{
    unimplemented(__FUNCTION__, __FILE__, __LINE__);
}

/************************************************************************/
/** \brief Traverses all transactions on all active queues and performs an operation on them.
**  
**  \par Assumptions, External Events, and Notes:
**       c must not be NULL. fn must be a valid function. context must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
int CF_TraverseAllTransactions(channel_t *c, CF_TraverseAllTransactions_fn_t fn, void *context)
{
    int forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_TraverseAllTransactions), &c,
      sizeof(c));
    UT_Stub_CopyFromLocal(UT_KEY(CF_TraverseAllTransactions), &fn,
      sizeof(fn));
    UT_Stub_CopyFromLocal(UT_KEY(CF_TraverseAllTransactions), &context,
      sizeof(context));

    forced_return = UT_DEFAULT_IMPL(CF_TraverseAllTransactions);

    return forced_return;
}

/************************************************************************/
/** \brief Traverses all transactions on all channels and performs an operation on them.
**
**  \par Assumptions, External Events, and Notes:
**       fn must be a valid function. context must not be NULL.
**
**  \returns
**  \retcode #CFE_SUCCESS \retdesc \copydoc CFE_SUCCESSS \endcode
**  \retstmt Returns anything else on error.             \endcode
**  \endreturns
**
*************************************************************************/
int CF_TraverseAllTransactions_All_Channels(CF_TraverseAllTransactions_fn_t fn, void *context)
{
    int forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_TraverseAllTransactions_All_Channels), &fn,
      sizeof(fn));
    UT_Stub_CopyFromLocal(UT_KEY(CF_TraverseAllTransactions_All_Channels), &context,
      sizeof(context));
    
    UT_DEFAULT_IMPL(CF_TraverseAllTransactions_All_Channels);
    
    UT_Stub_CopyToLocal(UT_KEY(CF_TraverseAllTransactions_All_Channels), &forced_return,
      sizeof(forced_return));

      /* TODO: setting the context here by using a specified force_return value is NOT the way to do things, this MUST be turned into a hook function */
    if (forced_return == 0xDCDCDCDC)
    {
      forced_return = -1;
      *((int*)context) = 1;
    }
    if (forced_return == 0xDC0000DC)
    {
      forced_return = -1;
      *((int*)context) = 0;
    }
      
    return forced_return;
}

/************************************************************************/
/** \brief Wrap the filesystem open call with a perf counter.
**  
**  \par Assumptions, External Events, and Notes:
**       fname must not be NULL.
**
**  \returns
**  \retstmt Valid file descriptor, or anything else on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WrappedOpenCreate(osal_id_t *fd, const char *fname, int32 flags, int32 access)
{    
    int forced_return;
    
    UT_Stub_CopyFromLocal(UT_KEY(CF_WrappedOpenCreate), &fd, sizeof(fd));
    UT_Stub_CopyFromLocal(UT_KEY(CF_WrappedOpenCreate), &fname, sizeof(fname));
    UT_Stub_CopyFromLocal(UT_KEY(CF_WrappedOpenCreate), &flags, sizeof(flags));
    UT_Stub_CopyFromLocal(UT_KEY(CF_WrappedOpenCreate), &access, sizeof(access));

    UT_DEFAULT_IMPL(CF_WrappedOpenCreate);
    
    UT_Stub_CopyToLocal(UT_KEY(CF_WrappedOpenCreate), &forced_return,
      sizeof(forced_return));

    return forced_return;
}

/************************************************************************/
/** \brief Wrap the filesystem close call with a perf counter.
**  
**  \par Assumptions, External Events, and Notes:
**       None
**
*************************************************************************/
void CF_WrappedClose(osal_id_t fd)
{
    UT_Stub_CopyFromLocal(UT_KEY(CF_WrappedClose), &fd, sizeof(fd));
    
    UT_DEFAULT_IMPL(CF_WrappedClose);
}

/************************************************************************/
/** \brief Wrap the filesystem read call with a perf counter.
**  
**  \par Assumptions, External Events, and Notes:
**       buf must not be NULL.
**
**  \returns
**  \retstmt >=0 number of bytes read on success \endcode
**  \retstmt <0 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WrappedRead(osal_id_t fd, void *buf, size_t read_size)
{  
    UT_GenStub_SetupReturnBuffer(CF_WrappedRead, int32);

    UT_GenStub_AddParam(CF_WrappedRead, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedRead, const char *, buf);
    UT_GenStub_AddParam(CF_WrappedRead, uint16, read_size);

    UT_GenStub_Execute(CF_WrappedRead, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedRead, int32);
}

/************************************************************************/
/** \brief Wrap the filesystem write call with a perf counter.
**  
**  \par Assumptions, External Events, and Notes:
**       buf must not be NULL.
**
**  \returns
**  \retstmt >=0 number of bytes read on success \endcode
**  \retstmt <0 on error. \endcode
**  \endreturns
**
*************************************************************************/
int32 CF_WrappedWrite(osal_id_t fd, const void *buf, size_t write_size)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedWrite, int32);

    UT_GenStub_AddParam(CF_WrappedWrite, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedWrite, const char *, buf);
    UT_GenStub_AddParam(CF_WrappedWrite, uint16, write_size);

    UT_GenStub_Execute(CF_WrappedWrite, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedWrite, int32);
}

/************************************************************************/
/** \brief Wrap the filesystem lseek call with a perf counter.
**  
**  \par Assumptions, External Events, and Notes:
**       fname must not be NULL.
**
**  \returns
**  \retstmt >=0 the current file position in bytes. \endcode
**  \retstmt <0 on error. \endcode
**  \endreturns
**
*************************************************************************/

int32 CF_WrappedLseek(osal_id_t fd, off_t offset, int mode)
{
    UT_GenStub_SetupReturnBuffer(CF_WrappedLseek, int32);

    UT_GenStub_AddParam(CF_WrappedLseek, osal_id_t, fd);
    UT_GenStub_AddParam(CF_WrappedLseek, off_t, offset);
    UT_GenStub_AddParam(CF_WrappedLseek, int, mode);

    UT_GenStub_Execute(CF_WrappedLseek, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_WrappedLseek, int32);
}

