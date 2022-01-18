#ifndef _cf_test_utils_h_
#define _cf_test_utils_h_

/* library includes */
#include <time.h>

/* cfe includes */
#include "cfe.h"
#include "common_types.h"

/* cf includes */
#include "cf_platform_cfg.h"
#include "cf_cfdp_pdu.h"
#include "cf_app.h"
#include "cf_utils.h"

/* UT includes */
#include "uttest.h"
#include "utassert.h"
#include "utstubs.h"
#include "utgenstub.h"

/* ut_utils_lib includes */
#include "ut_utils_cfe_hooks.h"

/* cf_tests constants */
#define UT_INT_32_DEFAULT  0xABBACABB /* 0xABBACABB == 2881145531 decimal */
#define UT_UINT_16_DEFAULT 0xEFFD     /* 0xEFFD == 61437 decimal */
#define UT_INT_16_DEFAULT  0xBADD     /* 0xBADD == 47837 decimal */
#define UT_UINT_8_DEFAULT  0xED       /* 0xED == 237 decimal */

#define MAX_INT 2147484647 /* Set at 32bit for now,  pow(2, 31) - 1 */

#define ALL_CHANNELS 255
#define COMPOUND_KEY 254

#define ERROR_RETRIEVING_ANY_VALUE -86

#ifndef RANDOM_VALUES_SEED
#define RANDOM_VALUES_SEED 0
#endif /* !RANDOM_VALUES_SEED */

#define FIRST_CALL  1
#define SECOND_CALL 2
#define NEXT_CALL   1

#define UT_CFDP_CHANNEL 0

typedef enum
{
    UT_CF_Setup_NONE,
    UT_CF_Setup_TX,
    UT_CF_Setup_RX

} UT_CF_Setup_t;

/*******************************************************************************
**
**  cfs_cf stub_reporter_hook contexts
**
**  CF_PACK required to make sure things end up in the proper locations for
**  storage of arguments passed to functions.
**
*******************************************************************************/

typedef struct
{
    char            src_filename[CF_FILENAME_MAX_LEN];
    char            dst_filename[CF_FILENAME_MAX_LEN];
    CF_CFDP_Class_t cfdp_class;
    uint8           keep;
    uint8           chan;
    uint8           priority;
    CF_EntityId_t   dest_id;
} CF_CFDP_TxFile_context_t;

typedef struct
{
    char            src_filename[CF_FILENAME_MAX_LEN];
    char            dst_filename[CF_FILENAME_MAX_LEN];
    CF_CFDP_Class_t cfdp_class;
    uint8           keep;
    uint8           chan;
    uint8           priority;
    CF_EntityId_t   dest_id;
} CF_CFDP_PlaybackDir_context_t;

typedef struct
{
    CF_Channel_t       *c;
    CF_TransactionSeq_t transaction_sequence_number;
    CF_EntityId_t       src_eid;
    CF_Transaction_t   *forced_return;
} CF_FindTransactionBySequenceNumber_context_t;

typedef struct
{
    CF_TraverseAllTransactions_fn_t fn;
    void                           *context;
    int                             forced_return;
} CF_TraverseAllTransactions_All_Channels_context_t;

typedef struct
{
    CF_Transaction_t *t;
    int               keep_history;
} CF_CFDP_ResetTransaction_context_t;

typedef struct
{
    CF_Channel_t *c;
    CF_History_t *h;
} CF_CFDP_ResetHistory_context_t;

typedef struct
{
    CF_CListNode_t *start;
    CF_CListFn_t    fn;
    void           *context;
} CF_CList_Traverse_POINTER_context_t;

typedef struct
{
    int32         fd;
    CF_Channel_t *c;
    CF_QueueIdx_t q;
} CF_WriteTxnQueueDataToFile_context_t;

typedef struct
{
    int32          fd;
    CF_Channel_t  *c;
    CF_Direction_t dir;
} CF_WriteHistoryQueueDataToFile_context_t;

typedef struct
{
    CF_Channel_t                   *c;
    CF_TraverseAllTransactions_fn_t fn;
    void                           *context;
    /* TODO: add forced return? Stub is kinda using it but not from context */
} CF_TraverseAllTransactions_context_t;

typedef struct
{
    osal_id_t  *fd;
    const char *fname;
    int32       flags;
    int32       access;
    int32       forced_return;
} CF_WrappedOpenCreate_context_t;

typedef struct
{
    CF_CListNode_t **head;
    CF_CListNode_t  *node;
} CF_CList_Remove_context_t;

typedef struct
{
    CF_CListNode_t **head;
    CF_CListNode_t  *forced_return;
} CF_CList_Pop_context_t;

typedef struct
{
    CF_CListNode_t **head;
    CF_CListNode_t  *node;
} CF_CList_InsertBack_context_t;

typedef struct
{
    CF_CListNode_t **head;
    CF_CListNode_t  *start;
    CF_CListNode_t  *after;
} CF_CList_InsertAfter_context_t;

typedef struct
{
    CF_CListNode_t *start;
    CF_CListFn_t    fn;
    osal_id_t       context_fd;
    int32           context_result;
    int32           context_counter;
} CF_CList_Traverse_TRAV_ARG_T_context_t;

typedef struct
{
    CF_CListNode_t                 *start;
    CF_CListFn_t                    fn;
    CF_TraverseAllTransactions_fn_t context_fn;
    void                           *context_context;
    int                             context_counter;
} CF_CList_Traverse_TRAVERSE_ALL_ARGS_T_context_t;

typedef struct
{
    CF_CListNode_t   *end;
    CF_CListFn_t      fn;
    CF_Transaction_t *context_t;
} CF_CList_Traverse_R_context_t;

void *UT_CF_GetContextBufferImpl(UT_EntryKey_t FuncKey, size_t ReqSize);
#define UT_CF_GetContextBuffer(key, type) ((type *)UT_CF_GetContextBufferImpl(key, sizeof(type)))

/*
 * Helper functions for capturing and checking generated event IDs.
 *
 * Provides a simplified way to capture and check for calls to CFE_EVS_SendEvent.
 *
 */
void UT_CF_ResetEventCapture(UT_EntryKey_t FuncKey);
void UT_CF_CheckEventID_Impl(uint16 ExpectedID, const char *EventIDStr);
#define UT_CF_AssertEventID(eid) UT_CF_CheckEventID_Impl(eid, #eid)

/* bottom */

void cf_tests_Setup(void);
void cf_tests_Teardown(void);

void TestUtil_InitializeRandomSeed(void);
void TestUtil_InitMsg(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId, CFE_MSG_Size_t Size);

unsigned int AnyCoinFlip(void);

bool         Any_bool(void);
void         AnyBufferOf_uint8_WithSize(uint8 *buffer, size_t size);
char         Any_char(void);
char         Any_char_Except(char exception);
uint8        Any_0_or_1(void);
uint8        Any_uint8(void);
uint8        Any_uint8_BetweenExcludeMax(uint8 floor, uint8 ceiling);
uint8        Any_uint8_BetweenInclusive(uint8 floor, uint8 ceiling);
uint8        Any_uint8_ExceptSetBits(uint8 mask);
uint8        Any_uint8_ExceptUnsetBits(uint8 mask);
uint8        Any_uint8_FromThese(uint8 values[], uint8 num_values);
uint8        Any_uint8_LessThan(uint8 ceiling);
uint8        Any_uint8_LessThanCeilingExcept(uint8 ceiling, uint8 exception);
uint8        Any_uint8_GreaterThan(uint8 floor);
uint8        Any_uint8_GreaterThan_or_EqualTo(uint8 floor);
uint8        Any_uint8_Except(uint8 exception);
uint8        Any_uint8_ExceptThese(uint8 exceptions[], uint8 num_exceptions);
uint16       Any_uint16(void);
uint16       Any_uint16_BetweenExcludeMax(uint16 floor, uint16 ceiling);
uint16       Any_uint16_Except(uint16 exception);
uint16       Any_uint16_ExceptThese(uint16 exceptions[], uint8 num_exceptions);
uint16       Any_uint16_GreaterThan(uint16 floor);
uint16       Any_uint16_LessThan(uint16 ceiling);
uint32       Any_uint32(void);
uint32       Any_uint32_BetweenInclusive(uint32 min, uint32 max);
uint32       Any_uint32_BetweenExcludeMax(uint32 min, uint32 max);
uint32       Any_uint32_Except(uint32 exception);
uint32       Any_uint32_ExceptThese(uint32 exceptions[], uint8 num_exceptions);
uint32       Any_uint32_GreaterThan(uint32 floor);
uint32       Any_uint32_LessThan(uint32 ceiling);
uint32       Any_uint32_LessThan_or_EqualTo(uint32 max);
int32        Any_int32(void);
int32        Any_int32_Except(int32 exception);
int32        Any_int32_ExceptThese(int32 exceptions[], uint8 num_exceptions);
int32        Any_int32_LessThan(int32 ceiling);
int32        Any_int32_Negative(void);
int32        Any_int32_ZeroOrPositive(void);
uint64       Any_uint64(void);
uint64       Any_uint64_Except(uint64 exception);
unsigned int Any_unsigned_int(void);
int          Any_int(void);
int          Any_int_Except(int exception);
int          Any_int_GreaterThan(int floor);
int          Any_int_Negative(void);
int          Any_int_Positive(void);
int          Any_int_PositiveExcept(int exception);
int          Any_int_ZeroOrPositiveLessThan(int ceiling);
char        *AnyFilenameOfLength(size_t length);
char        *AnyRandomStringOfTextOfLength(size_t stringLength);
char        *AnyRandomStringOfLettersOfLength(size_t length);
void         AnyRandomStringOfLettersOfLengthCopy(char *random_string, size_t length);

uint8 Any_cf_chan_num(void);

void           Any_CFE_TIME_SysTime_Set(CFE_TIME_SysTime_t *fake_time);
CFE_Status_t   Any_CFE_Status_t_Negative(void);
CFE_Status_t   Any_CFE_Status_t_Except(CFE_Status_t exception);
CFE_MSG_Size_t Any_CFE_MSG_Size_t(void);
CFE_MSG_Size_t Any_CFE_MSG_Size_t_LessThan(size_t ceiling);

#endif /* _cf_test_utils_h_ */
