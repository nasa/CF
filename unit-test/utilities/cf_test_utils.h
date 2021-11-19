#ifndef _cf_test_utils_h_
#define _cf_test_utils_h_

/* library includes */
#include <time.h>

/* cfe includes */
#include "cfe.h"
#include "common_types.h"

/* cf includes */
#include "cf_cfdp_helpers.h"
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

#define ERROR_RETRIEVING_ANY_VALUE -86

#ifndef RANDOM_VALUES_SEED
#define RANDOM_VALUES_SEED 0
#endif /* !RANDOM_VALUES_SEED */

#define FIRST_CALL  1
#define SECOND_CALL 2
#define NEXT_CALL   1

int32  result;
uint16 EventID;

extern const char   *ut_default_const_char;
extern const uint8   ut_default_uint8;
extern const void   *ut_default_ptr;
extern UT_HookFunc_t stub_reporter;

void Handler_CF_CFDP_ConstructPduHeader_ForceReturnOnly(void *UserObj, UT_EntryKey_t FuncKey,
                                                        const UT_StubContext_t *Context);

/*******************************************************************************
**
**  cfe stub_reporter_hook contexts
**
**  Some that are very frequently use include a global context.
**
**  CF_PACK required to make sure things end up in the proper locations for
**  storage of arguments passed to functions.
**
*******************************************************************************/

typedef struct
{
    transaction_t  *t;
    int             num_segment_requests;
    cfdp_send_ret_t forced_return;
} CF_PACK CF_CFDP_SendNak_context_t;

typedef struct
{
    const transaction_t *t;
    uint8                directive_code;
    cf_entity_id_t       src_eid;
    cf_entity_id_t       dst_eid;
    uint8                towards_sender;
    cf_transaction_seq_t tsn;
    int                  silent;
    pdu_header_t        *forced_return;
} CF_PACK CF_CFDP_ConstructPduHeader_context_t;

typedef struct
{
    CFE_MSG_Message_t *MsgPtr;
    CFE_TIME_SysTime_t Time;
} CF_PACK                    CFE_MSG_SetMsgTime_context_t;
CFE_MSG_SetMsgTime_context_t context_CFE_MSG_SetMsgTime;

typedef struct
{
    const CFE_MSG_Message_t *MsgPtr;
    CFE_SB_MsgId_t          *returned_MsgId;
} CF_PACK                  CFE_MSG_GetMsgId_context_t;
CFE_MSG_GetMsgId_context_t context_CFE_MSG_GetMsgId;

typedef struct
{
    uint16      EventID;
    uint16      EventType;
    const char *Spec;
} CF_PACK                   CFE_EVS_SendEvent_context_t;
CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

typedef struct
{
    const CFE_MSG_Message_t *MsgPtr;
    CFE_MSG_Size_t          *Size;
} CF_PACK                 CFE_MSG_GetSize_context_t;
CFE_MSG_GetSize_context_t context_CFE_MSG_GetSize;

typedef struct
{
    CFE_MSG_Message_t *MsgPtr;
    CFE_SB_MsgId_t     MsgId;
} CF_PACK CFE_MSG_SetMsgId_context_t;

typedef struct
{
    CFE_MSG_Message_t *MsgPtr;
    bool               IncrementSequenceCount;
} CF_PACK CFE_SB_TransmitMsg_context_t;

typedef struct
{
    const CFE_MSG_Message_t *MsgPtr;
    CFE_MSG_FcnCode_t       *FcnCode;
} CF_PACK CFE_MSG_GetFcnCode_context_t;

typedef struct
{
    CFE_MSG_Message_t *MsgPtr;
    CFE_MSG_Size_t     Size;
} CF_PACK CFE_MSG_SetSize_context_t;

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
    const char     src_filename[CF_FILENAME_MAX_LEN];
    const char     dst_filename[CF_FILENAME_MAX_LEN];
    cfdp_class_t   cfdp_class;
    uint8          keep;
    uint8          chan;
    uint8          priority;
    cf_entity_id_t dest_id;
} CF_PACK CF_CFDP_TxFile_context_t;

typedef struct
{
    const char     src_filename[CF_FILENAME_MAX_LEN];
    const char     dst_filename[CF_FILENAME_MAX_LEN];
    cfdp_class_t   cfdp_class;
    uint8          keep;
    uint8          chan;
    uint8          priority;
    cf_entity_id_t dest_id;
} CF_PACK CF_CFDP_PlaybackDir_context_t;

typedef struct
{
    channel_t           *c;
    cf_transaction_seq_t transaction_sequence_number;
    cf_entity_id_t       src_eid;
    transaction_t       *forced_return;
} CF_PACK CF_CFDP_FindTransactionBySequenceNumber_context_t;

typedef struct
{
    CF_TraverseAllTransactions_fn_t fn;
    void                           *context;
    int                             forced_return;
} CF_PACK CF_TraverseAllTransactions_All_Channels_context_t;

typedef struct
{
    transaction_t *t;
    int            keep_history;
} CF_PACK CF_CFDP_ResetTransaction_context_t;

typedef struct
{
    channel_t *c;
    history_t *h;
} CF_PACK CF_CFDP_ResetHistory_context_t;

typedef struct
{
    clist_node start;
    clist_fn_t fn;
    void      *context;
} CF_PACK CF_CList_Traverse_POINTER_context_t;

typedef struct
{
    int32            fd;
    channel_t       *c;
    cf_queue_index_t q;
} CF_PACK CF_WriteQueueDataToFile_context_t;

typedef struct
{
    int32       fd;
    channel_t  *c;
    direction_t dir;
} CF_PACK CF_WriteHistoryQueueDataToFile_context_t;

typedef struct
{
    channel_t                      *c;
    CF_TraverseAllTransactions_fn_t fn;
    void                           *context;
    /* TODO: add forced return? Stub is kinda using it but not from context */
} CF_PACK CF_TraverseAllTransactions_context_t;

typedef struct
{
    transaction_t   *t;
    cf_queue_index_t q;
} CF_PACK CF_InsertSortPrio_context_t;

typedef struct
{
    transaction_t  *t;
    cfdp_send_ret_t forced_return;
} CF_PACK CF_CFDP_SendEof_context_t;

typedef struct
{
    transaction_t *t;
    int            silent;
    pdu_header_t  *forced_return;
} CF_PACK CF_CFDP_MsgOutGet_context_t;

typedef struct
{
    osal_id_t fd;
    off_t     offset;
    int       mode;
    int       forced_return;
} CF_PACK CF_WrappedLseek_context_t;

typedef struct
{
    osal_id_t fd;
    void     *buf;
    size_t    read_size;
    int       forced_return;
} CF_PACK CF_WrappedRead_context_t;

typedef struct
{
    const chunks_t *chunks;
    const chunk_t  *forced_return;
} CF_PACK CF_Chunks_GetFirstChunk_context_t;

typedef struct
{
    osal_id_t  *fd;
    const char *fname;
    int32       flags;
    int32       access;
    int32       forced_return;
} CF_PACK CF_WrappedOpenCreate_context_t;

typedef struct
{
    transaction_t           *t;
    ack_transaction_status_t ts;
    file_directive_t         dir_code;
    condition_code_t         cc;
    cf_entity_id_t           peer_eid;
    cf_transaction_seq_t     tsn;
    cfdp_send_ret_t          forced_return;
} CF_PACK CF_CFDP_SendAck_context_t;

typedef struct
{
    chunks_t      *chunks;
    chunk_offset_t offset;
    chunk_size_t   size;
} CF_PACK CF_Chunks_Add_context_t;

typedef struct
{
    clist_node *head;
    clist_node  node;
} CF_PACK CF_CList_Remove_context_t;

typedef struct
{
    transaction_t  *t;
    uint32          offset;
    int             len;
    cfdp_send_ret_t forced_return;
} CF_PACK CF_CFDP_SendFd_context_t;

typedef struct
{
    cf_crc_t    *c;
    const uint8 *data;
    int          len;
} CF_PACK CF_CRC_Digest_context_t;

typedef struct
{
    chunks_t    *chunks;
    chunk_size_t size;
} CF_PACK CF_Chunks_RemoveFromFirst_context_t;

typedef struct
{
    cf_timer_t *t;
    uint32      rel_sec;
} CF_PACK CF_Timer_InitRelSec_context_t;

typedef struct
{
    clist_node *head;
    clist_node  forced_return;
} CF_PACK CF_CList_Pop_context_t;

typedef struct
{
    clist_node *head;
    clist_node  node;
} CF_PACK CF_CList_InsertBack_context_t;

typedef struct
{
    clist_node           start;
    clist_fn_t           fn;
    cf_transaction_seq_t context_transaction_sequence_number;
    cf_entity_id_t       context_src_eid;
    transaction_t       *context_forced_t; /* out param */
} CF_PACK CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t;

typedef struct
{
    clist_node start;
    clist_fn_t fn;
    void      *context;
} CF_PACK CF_CList_Traverse_CLOSE_FILES_context_t;

typedef struct
{
    clist_node *head;
    clist_node  node;
} CF_PACK CF_Clist_Remove_context_t;

typedef struct
{
    clist_node *head;
    clist_node  start;
    clist_node  after;
} CF_PACK CF_CList_InsertAfter_context_t;

typedef struct
{
    clist_node start;
    clist_fn_t fn;
    int32      context_fd;
    int32      context_result;
    int32      context_counter;
} CF_PACK CF_CList_Traverse_TRAV_ARG_T_context_t;

typedef struct
{
    clist_node                      start;
    clist_fn_t                      fn;
    CF_TraverseAllTransactions_fn_t context_fn;
    void                           *context_context;
    int                             context_counter;
} CF_PACK CF_CList_Traverse_TRAVERSE_ALL_ARGS_T_context_t;

typedef struct
{
    clist_node     end;
    clist_fn_t     fn;
    transaction_t *context_t;
} CF_PACK CF_CList_Traverse_R_context_t;

typedef struct
{
    transaction_t *t;
    void          *context;
} CF_PACK DummyFunctionFor_CF_TraverseAllTransactions__context_t;

typedef struct
{
    transaction_t *t;
    pdu_header_t  *pdu;
} CF_PACK Dummy_fd_fn_context_t;

typedef struct
{
    transaction_t *t;
    pdu_header_t  *pdu;
} CF_PACK Dummy_fns_context_t;

typedef struct
{
    transaction_t      *t;
    const pdu_header_t *pdu;
} CF_PACK Dummy_fns_CF_CFDP_S_DispatchRecv_context_t;

/* bottom */

typedef enum
{
    NOT_YET_SET,
    TRAV_ARG_T,
    TRAVERSE_ALL_ARGS_T,
    POINTER,
    FIND_T_BY_SEQ_NUM,
    CLOSE_FILES,
    MAX_TYPE_OF_CONTEXT_CF_CLIST_TRAVERSE
} type_of_context_CF_CList_Traverse_t;

type_of_context_CF_CList_Traverse_t
    type_of_context_CF_CList_Traverse; /* moved here to remove warning, a special setting for the stub */

void cf_tests_Setup(void);
void cf_tests_Teardown(void);

void unimplemented(const char *func, const char *file, int line);
void TestUtil_InitializeRandomSeed(void);
void TestUtil_InitMsg(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId, CFE_MSG_Size_t Size);

void Handler_int_ForcedReturnOnly(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);

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

CFE_SB_MsgId_t Any_MsgId(void);
CFE_SB_MsgId_t Any_MsgId_ExceptThese(CFE_SB_MsgId_t exceptions[], uint8 num_exceptions);

#endif /* _cf_test_utils_h_ */
