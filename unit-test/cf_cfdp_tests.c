/* cf testing includes */
#include "cf_test_utils.h"

/* undefs/redefs for stubbing before source inclusion */
#undef cfdp_set_uint8
#define cfdp_set_uint8(dst, src) (dst) = (src)
#undef cfdp_set_uint16
#define cfdp_set_uint16(dst, src) (dst) = (src)
#undef cfdp_get_uint16
#define cfdp_get_uint16(dst, src) (dst) = (src)
#undef cfdp_ldst_uint16
#define cfdp_ldst_uint16(dst, src) (dst) = (src)
#undef cfdp_get_uint32
#define cfdp_get_uint32(dst, src) (dst) = (src)
#undef cfdp_ldst_uint32
#define cfdp_ldst_uint32(dst, src) (dst) = (src)

/* stub redefines for unit testing */
#undef FGV
#define FGV Stub_FGV
int32 Stub_FGV(uint8 source, CF_FIELD_FIELD name);
#undef cf_dequeue_transaction
#define cf_dequeue_transaction Stub_cf_dequeue_transaction
void cf_dequeue_transaction(CF_Transaction_t *t);
#undef CF_CList_InsertBack_Ex
#define CF_CList_InsertBack_Ex Stub_CF_CList_InsertBack_Ex
void Stub_CF_CList_InsertBack_Ex(CF_Channel_t *c, CF_QueueIdx_t index, CF_CListNode_t *node);

#include "cf_cfdp.c"
#include "cf_cfdp_pdu.h"

/* cf_cfdp_tests globals */

extern type_of_context_CF_CList_Traverse_t type_of_context_CF_CList_Traverse;

/*******************************************************************************
**
**  cf_cfdp_tests local utility functions
**
*******************************************************************************/

uint8 Any_cf_queue_index_t_Except(uint8 exception)
{
    uint8       num_checks = 0;
    const uint8 max_checks = 10;
    uint8       random_val = exception;

    while (random_val == exception)
    {
        random_val = rand() % CF_QueueIdx_NUM;

        if (num_checks >= max_checks)
        {
            UtPrintf("Any_cf_queue_index_t_Except unable to get valid number in %u checks\n", num_checks);
            UtAssert_Abort("Any_cf_queue_index_t_Except failed");
        }
        else
        {
            ++num_checks;
        }
    }

    return random_val;
}

/* uint8 selected for ease of use */
uint8 Any_cfdp_state_t(void)
{
    uint8 random_val = rand() % CF_TxnState_INVALID;

    return random_val;
}

uint8 Any_cfdp_state_t_ExceptThese(uint8 exceptions[], uint8 num_exceptions)
{

    uint8       i          = 0;
    uint8       num_checks = 0;
    const uint8 max_checks = 10;
    uint8       random_val = Any_cfdp_state_t();

    while (i < num_exceptions)
    {
        if (random_val == exceptions[i])
        {
            random_val = Any_cfdp_state_t();
            i          = 0;

            if (num_checks >= max_checks)
            {
                UtPrintf("Any_cfdp_state_t_ExceptThese unable to get valid number in %u checks\n", num_checks);
                UtAssert_Abort("Any_cfdp_state_t_ExceptThese failed");
            }
            else
            {
                ++num_checks;
            }
        }
        else
        {
            ++i;
        }
    }

    return random_val;
}

CF_CFDP_FileDirective_t Any_file_directive_t_Except(CF_CFDP_FileDirective_t exception)
{
    uint8                   num_tries = 0;
    uint8                   max_tries = 10;
    CF_CFDP_FileDirective_t rand_val  = rand() % CF_CFDP_FileDirective_INVALID_MAX;

    while (rand_val == exception)
    {
        if (++num_tries > max_tries)
        {
            UtAssert_Failed("Unable to achieve different random value within %u tries.", max_tries);
            UtAssert_Abort(__func__);
        }

        rand_val = rand() % CF_CFDP_FileDirective_INVALID_MAX;
    }

    return rand_val;
}

CF_Direction_t Any_direction_t(void)
{
    return (CF_Direction_t)Any_uint8_LessThan(CF_Direction_NUM);
}

CF_CFDP_Class_t Any_cfdp_class_t(void)
{
    if (AnyCoinFlip())
    {
        return CF_CFDP_CLASS_1;
    }
    else
    {
        return CF_CFDP_CLASS_2;
    }
}

void Handler_int_ForcedReturnArray(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int   forced_return;
    uint8 array_location = UT_GetStubCount(FuncKey) - 1;

    memcpy(&forced_return, ((uint8 *)UserObj + array_location * sizeof(forced_return)), sizeof(forced_return));

    UT_Stub_SetReturnValue(FuncKey, forced_return);
}

void Handler_size_t_ForcedReturnOnly(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t forced_return;

    memcpy(&forced_return, UserObj, sizeof(forced_return));

    UT_Stub_SetReturnValue(FuncKey, forced_return);
}

uint8 callNumberToSet_notNULL_for_CF_CFDP_S1_Tx_args_c_cur = 0;
void  Handler_CF_CFDP_S1_Tx_notNull_cur_AtSetCallNumber(void *UserObj, UT_EntryKey_t FuncKey,
                                                        const UT_StubContext_t *Context)
{
    CF_CFDP_CycleTx_args_t *args = (CF_CFDP_CycleTx_args_t *)UserObj;

    if (UT_GetStubCount(FuncKey) >= callNumberToSet_notNULL_for_CF_CFDP_S1_Tx_args_c_cur)
    {
        args->c->cur = CF_AppData.engine.transactions; /* args->c->cur = CF_AppData.engine.transactions is used to set
                                                          to an arbitrary transaction that exists */
    }
}

void Handler_CF_CFDP_S1_Tx_notCF_Q_TXA_q_index(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_Transaction_t *t = *((CF_Transaction_t **)((uint8 *)UserObj + sizeof(void *)));

    if (UT_GetStubCount(FuncKey) > 1)
    {
        t->flags.com.q_index = CF_QueueIdx_NUM;
    }
}

void Handler_CF_CList_Traverse_Set_ran_one_To_1(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_CFDP_CycleTx_args_t *stub_context = UT_Hook_GetArgValueByName(Context, "context", CF_CFDP_CycleTx_args_t *);

    stub_context->ran_one = 1;
}

void Handler_CF_CList_Traverse_SecondCallSet_ran_one_To_1(void *UserObj, UT_EntryKey_t FuncKey,
                                                          const UT_StubContext_t *Context)
{
    if (UT_GetStubCount(FuncKey) == 2)
    {
        Handler_CF_CList_Traverse_Set_ran_one_To_1(UserObj, FuncKey, Context);
    }
}

void *Dummy_tick_args_t_fn(CF_Transaction_t *t, int *cont)
{
    UT_GenStub_Execute(Dummy_tick_args_t_fn, Basic, NULL);

    return NULL;
}

void Handler_Dummy_tick_args_t_fn_Set_cur_notNULL(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CF_Transaction_t **cur = (CF_Transaction_t **)UserObj;

    *cur = CF_AppData.engine.transactions; /* *cur = CF_AppData.engine.transactions is used to set to an arbitrary
                                              transaction that exists */
}

void Handler_CF_CList_Traverse_Sets_args_early_exit_To_1(void *UserObj, UT_EntryKey_t FuncKey,
                                                         const UT_StubContext_t *Context)
{
    tick_args_t *args = UT_Hook_GetArgValueByName(Context, "context", tick_args_t *);

    args->early_exit = 1;
}

void Handler_CF_CList_Traverse_Sets_args_cont_To_1_UntilGivenCount(void *UserObj, UT_EntryKey_t FuncKey,
                                                                   const UT_StubContext_t *Context)
{
    tick_args_t *args       = UT_Hook_GetArgValueByName(Context, "context", tick_args_t *);
    uint32      *stop_count = (uint32 *)UserObj;

    if (UT_GetStubCount(FuncKey) >= *stop_count)
    {

        args->cont = 0;
    }
    else
    {
        args->cont = 1;
    }
}

void Handler_OS_DirectoryClose_ReportOnly(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    /* Handler_OS_DirectoryClose_ReportOnly does nothing, written to override OS_DirectoryClose follow-on behavior that
     * causes failures and is not required for unit testing */
}

void Handler_OS_DirectoryRead_Set_dirent_FileName_ToGivenString(void *UserObj, UT_EntryKey_t FuncKey,
                                                                const UT_StubContext_t *Context)
{

    os_dirent_t *dirent = UT_Hook_GetArgValueByName(Context, "dirent", os_dirent_t *);

    memcpy(dirent->FileName, UserObj, OS_MAX_FILE_NAME);
}

/* int32 selected only for ease of Stub use */
int32 Stub_FGV(uint8 source, CF_FIELD_FIELD name)
{
    UT_GenStub_SetupReturnBuffer(Stub_FGV, int32);

    UT_GenStub_AddParam(Stub_FGV, uint8, source);
    UT_GenStub_AddParam(Stub_FGV, CF_FIELD_FIELD, name);

    UT_GenStub_Execute(Stub_FGV, Basic, NULL);

    return UT_GenStub_GetReturnValue(Stub_FGV, int32);
}

void Handler_CF_Timer_InitRelSec_Change_t_state_To_CFDP_IDLE(void *UserObj, UT_EntryKey_t FuncKey,
                                                             const UT_StubContext_t *Context)
{
    ((CF_Transaction_t *)UserObj)->state = CF_TxnState_IDLE;
}

void handler_CFE_SB_AllocateMessageBuffer_ReturnForced_CFE_SB_Buffer(void *UserObj, UT_EntryKey_t FuncKey,
                                                                     const UT_StubContext_t *Context)
{
    UT_Stub_SetReturnValue(FuncKey, UserObj);
}

void Stub_cf_dequeue_transaction(CF_Transaction_t *t)
{
    UT_GenStub_AddParam(Stub_cf_dequeue_transaction, CF_Transaction_t *, t);

    UT_GenStub_Execute(Stub_cf_dequeue_transaction, Basic, NULL);
}

void handler_CF_CList_InsertBack_Ex_Record_indexes(void *UserObj, UT_EntryKey_t FuncKey,
                                                   const UT_StubContext_t *Context)
{
    CF_QueueIdx_t index = UT_Hook_GetArgValueByName(Context, "index", CF_QueueIdx_t);

    *(((CF_QueueIdx_t *)UserObj) + (UT_GetStubCount(FuncKey) - 1)) = index;
}

void Stub_CF_CList_InsertBack_Ex(CF_Channel_t *c, CF_QueueIdx_t index, CF_CListNode_t *node)
{
    UT_GenStub_AddParam(Stub_CF_CList_InsertBack_Ex, CF_Channel_t *, c);
    UT_GenStub_AddParam(Stub_CF_CList_InsertBack_Ex, CF_QueueIdx_t, index);
    UT_GenStub_AddParam(Stub_CF_CList_InsertBack_Ex, CF_CListNode_t *, node);

    UT_GenStub_Execute(Stub_CF_CList_InsertBack_Ex, Basic, NULL);
}

void handler_OS_remove_Report_path(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    *((char **)UserObj) = UT_Hook_GetArgValueByName(Context, "path", char *);
}

/* end cf_cfdp_tests local utility functions */

/*******************************************************************************
**
**  cf_cfdp_tests Setup and Teardown
**
*******************************************************************************/

void cf_cfdp_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_cfdp_tests_Setup */

void cf_cfdp_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_cfdp_tests_Teardown */

/* end cf_cfdp_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_CFDP_ArmAckTimer tests (simple) - full coverage
**
*******************************************************************************/

void Test_CF_CFDP_ArmAckTimer_Call_CF_Timer_InitRelSec_WithCorrectParamsAndArmsTimer(void)
{
    /* Arrange */
    CF_Transaction_t              dummy_t;
    CF_Transaction_t             *arg_t = &dummy_t;
    cf_config_table_t             dummy_config_table;
    CF_Timer_InitRelSec_context_t context_CF_Timer_InitRelSec;

    CF_AppData.config_table              = &dummy_config_table;
    CF_AppData.config_table->ack_timer_s = Any_uint32();

    arg_t->flags.com.ack_timer_armed = 0; /* not required but helps show assert */

    UT_SetDataBuffer(UT_KEY(CF_Timer_InitRelSec), &context_CF_Timer_InitRelSec, sizeof(context_CF_Timer_InitRelSec),
                     false);

    /* Act */
    CF_CFDP_ArmAckTimer(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_InitRelSec.t, &arg_t->ack_timer);
    UtAssert_True(context_CF_Timer_InitRelSec.rel_sec == CF_AppData.config_table->ack_timer_s,
                  "CF_Timer_InitRelSec received rel_sec %u and should be %u (CF_AppData.config_table->ack_timer_s)",
                  context_CF_Timer_InitRelSec.rel_sec, CF_AppData.config_table->ack_timer_s);
    UtAssert_True(arg_t->flags.com.ack_timer_armed == 1,
                  "CF_CFDP_ArmAckTimer set ack_timer_armed to %u and should be 1", arg_t->flags.com.ack_timer_armed);
} /* end Test_CF_CFDP_ArmAckTimer_Call_CF_Timer_InitRelSec_WithCorrectParamsAndArmsTimer */

/* end CF_CFDP_ArmAckTimer tests */

/*******************************************************************************
**
**  CF_CFDP_GetClass tests (simple) - full coverage - 1 coverage JIRA issue
**
*******************************************************************************/

void Test_CF_CFDP_GetClass_AssertsBecause_q_index_IsEqTo_CF_Q_FREE(void)
{
    /* Arrange */
    // CF_Transaction_t   dummy_ti;
    // CF_Transaction_t*  arg_ti = &dummy_ti;

    // arg_ti->flags.com.q_index = CF_QueueIdx_FREE;

    // /* Act */
    // //CF_CFDP_GetClass(arg_ti);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");

} /* end Test_CF_CFDP_GetClass_AssertsBecause_q_index_IsEqTo_CF_Q_FREE */

void Test_CF_CFDP_GetClass_WhenNeitherStateIsSet_Return_CLASS_1(void)
{
    /* Arrange */
    uint8             excepted_states[2] = {CF_TxnState_S2, CF_TxnState_R2};
    CF_Transaction_t  dummy_ti;
    CF_Transaction_t *arg_ti = &dummy_ti;
    CF_CFDP_Class_t   local_result;

    arg_ti->flags.com.q_index = Any_cf_queue_index_t_Except(CF_QueueIdx_FREE);
    arg_ti->state = Any_cfdp_state_t_ExceptThese(excepted_states, sizeof(excepted_states) / sizeof(excepted_states[0]));

    /* Act */
    local_result = CF_CFDP_GetClass(arg_ti);

    /* Assert */
    UtAssert_True(local_result == CF_CFDP_CLASS_1, "CF_CFDP_GetClass returned %u and should be %u (CF_CFDP_CLASS_1)",
                  local_result, CF_CFDP_CLASS_1);
} /* end Test_CF_CFDP_GetClass_WhenNeitherStateIsSet_Return_CLASS_1 */

void Test_CF_CFDP_GetClass_WhenStateIs_CFDP_S2_Return_CLASS_1(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_ti;
    CF_Transaction_t *arg_ti = &dummy_ti;
    CF_CFDP_Class_t   local_result;

    arg_ti->flags.com.q_index = Any_cf_queue_index_t_Except(CF_QueueIdx_FREE);
    arg_ti->state             = CF_TxnState_S2;

    /* Act */
    local_result = CF_CFDP_GetClass(arg_ti);

    /* Assert */
    UtAssert_True(local_result == CF_CFDP_CLASS_2, "CF_CFDP_GetClass returned %u and should be %u (CF_CFDP_CLASS_2)",
                  local_result, CF_CFDP_CLASS_2);
} /* end Test_CF_CFDP_GetClass_WhenStateIs_CFDP_S2_Return_CLASS_1 */

void Test_CF_CFDP_GetClass_WhenStateIs_CFDP_R2_Return_CLASS_1(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_ti;
    CF_Transaction_t *arg_ti = &dummy_ti;
    CF_CFDP_Class_t   local_result;

    arg_ti->flags.com.q_index = Any_cf_queue_index_t_Except(CF_QueueIdx_FREE);
    arg_ti->state             = CF_TxnState_R2;

    /* Act */
    local_result = CF_CFDP_GetClass(arg_ti);

    /* Assert */
    UtAssert_True(local_result == CF_CFDP_CLASS_2, "CF_CFDP_GetClass returned %u and should be %u (CF_CFDP_CLASS_2)",
                  local_result, CF_CFDP_CLASS_2);
} /* end Test_CF_CFDP_GetClass_WhenStateIs_CFDP_R2_Return_CLASS_1 */

/* end CF_CFDP_GetClass tests */

/*******************************************************************************
**
**  CF_CFDP_IsSender tests (simple) - full coverage - 1 coverage JIRA issue
**
*******************************************************************************/

void Test_CF_CFDP_IsSender_AssertsBecause_q_index_IsEqTo_CF_Q_FREE(void)
{
    // /* Arrange */
    // CF_Transaction_t   dummy_ti;
    // CF_Transaction_t*  arg_ti = &dummy_ti;

    // arg_ti->flags.com.q_index = CF_QueueIdx_FREE;

    // /* Act */
    // CF_CFDP_IsSender(arg_ti);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_IsSender_AssertsBecause_q_index_IsEqTo_CF_Q_FREE */

void Test_CF_CFDP_IsSender_WhenNeitherStateIsSetReturn_0_MeaningReciever(void)
{
    /* Arrange */
    uint8             excepted_states[2] = {CF_TxnState_S1, CF_TxnState_S2};
    CF_Transaction_t  dummy_ti;
    CF_Transaction_t *arg_ti = &dummy_ti;
    int               local_result;

    arg_ti->flags.com.q_index = Any_cf_queue_index_t_Except(CF_QueueIdx_FREE);
    arg_ti->state = Any_cfdp_state_t_ExceptThese(excepted_states, sizeof(excepted_states) / sizeof(excepted_states[0]));

    /* Act */
    local_result = CF_CFDP_IsSender(arg_ti);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_GetClass returned %u and should be 0 (receiver)", local_result);
} /* end Test_CF_CFDP_IsSender_WhenNeitherStateIsSetReturn_0_MeaningReciever */

void Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S1_Return_1_MeaningSender(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_ti;
    CF_Transaction_t *arg_ti = &dummy_ti;
    int               local_result;

    arg_ti->flags.com.q_index = Any_cf_queue_index_t_Except(CF_QueueIdx_FREE);
    arg_ti->state             = CF_TxnState_S1;

    /* Act */
    local_result = CF_CFDP_IsSender(arg_ti);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_GetClass returned %u and should be 1 (sender)", local_result);
} /* end Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S1_Return_1_MeaningSender */

void Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S2_Return_1_MeaningSender(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_ti;
    CF_Transaction_t *arg_ti = &dummy_ti;
    int               local_result;

    arg_ti->flags.com.q_index = Any_cf_queue_index_t_Except(CF_QueueIdx_FREE);
    arg_ti->state             = CF_TxnState_S2;

    /* Act */
    local_result = CF_CFDP_IsSender(arg_ti);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_GetClass returned %u and should be 1 (sender)", local_result);
} /* end Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S2_Return_1_MeaningSender */

/* end CF_CFDP_IsSender tests */

/*******************************************************************************
**
**  CF_CFDP_ArmInactTimer tests (simple) - full coverage
**
*******************************************************************************/

void Test_CF_CFDP_ArmInactTimer_Call_CF_Timer_InitRelSec_WithCorrectParams(void)
{
    /* Arrange */
    CF_Transaction_t              dummy_t;
    CF_Transaction_t             *arg_t = &dummy_t;
    cf_config_table_t             dummy_config_table;
    CF_Timer_InitRelSec_context_t context_CF_Timer_InitRelSec;

    CF_AppData.config_table                     = &dummy_config_table;
    CF_AppData.config_table->inactivity_timer_s = Any_uint32();

    UT_SetDataBuffer(UT_KEY(CF_Timer_InitRelSec), &context_CF_Timer_InitRelSec, sizeof(context_CF_Timer_InitRelSec),
                     false);

    /* Act */
    CF_CFDP_ArmInactTimer(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
    UtAssert_ADDRESS_EQ(context_CF_Timer_InitRelSec.t, &arg_t->inactivity_timer);
    UtAssert_True(context_CF_Timer_InitRelSec.rel_sec == CF_AppData.config_table->inactivity_timer_s,
                  "CF_Timer_InitRelSec received rel_sec %u and should be %u (CF_AppData.config_table->ack_timer_s)",
                  context_CF_Timer_InitRelSec.rel_sec, CF_AppData.config_table->inactivity_timer_s);
} /* end Test_CF_CFDP_ArmInactTimer_Call_CF_Timer_InitRelSec_WithCorrectParams */

/* end CF_CFDP_ArmInactTimer tests */

/*******************************************************************************
**
**  CF_CFDP_DispatchRecv tests (small) - full coverage - 1 coverage JIRA issue - [unstubbables: CF_CFDP_RecvIdle,
*CF_CFDP_R1_Recv, CF_CFDP_S1_Recv, CF_CFDP_R2_Recv, CF_CFDP_S2_Recv, CF_CFDP_RecvDrop, CF_CFDP_ArmInactTimer]
**  NOTE: unstubbables for CF_CFDP_DispatchRecv tests are wrong, most of these calls are stubbable
**
*******************************************************************************/

void Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_EqTo_CFDP_INVALID(void)
{
    // /* Arrange */
    // CF_Transaction_t           dummy_t;
    // CF_Transaction_t*          arg_t = &dummy_t;

    // arg_t->state = CF_TxnState_INVALID;

    // /* Act */
    // CF_CFDP_DispatchRecv(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state<CF_TxnState_INVALID");

} /* end Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_EqTo_CFDP_INVALID */

/* TODO: Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_GreaterThan_CFDP_INVALID not required but desired */
// void Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_GreaterThan_CFDP_INVALID(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = Any_uint8_GreaterThan(CF_TxnState_INVALID);

//     /* Act */
//     //CF_CFDP_DispatchRecv(arg_t);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state<CF_TxnState_INVALID");
// } /* end Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_GreaterThan_CFDP_INVALID */

// TODO: CF_CFDP_DispatchRecv tests really should check all the possible states, but only 1 is necessary for coverage
// void Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_RecvIdle_BecauseStateEq_CFDP_IDLE(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_IDLE;

//     /* Act */
//     //CF_CFDP_DispatchRecv(arg_t);

//     /* Assert */
//     UtAssert_Failed("Deep Setup - uses CF_CFDP_RecvIdle, CF_CFDP_ArmInactTimer");
// } /* end Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_RecvIdle_BecauseStateEq_CFDP_IDLE */

// void Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R1_Recv_BecauseStateEq_CFDP_R1(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_R1;

//     /* Act */
//     //CF_CFDP_DispatchRecv(arg_t);

//     /* Assert */
//     UtAssert_Failed("Deep Setup - uses CF_CFDP_R1_Recv, CF_CFDP_ArmInactTimer");
// } /* end Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R1_Recv_BecauseStateEq_CFDP_R1 */

// void Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S1_Recv_BecauseStateEq_CFDP_S1(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_S1;

//     /* Act */
//     //CF_CFDP_DispatchRecv(arg_t);

//     /* Assert */
//     UtAssert_Failed("Deep Setup - uses CF_CFDP_S1_Recv, CF_CFDP_ArmInactTimer");
// } /* end Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S1_Recv_BecauseStateEq_CFDP_S1 */

// void Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R2_Recv_BecauseStateEq_CFDP_R2(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_R2;

//     /* Act */
//     //CF_CFDP_DispatchRecv(arg_t);

//     /* Assert */
//     UtAssert_Failed("Deep Setup - uses CF_CFDP_R2_Recv, CF_CFDP_ArmInactTimer");
// } /* end Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R2_Recv_BecauseStateEq_CFDP_R2 */

// void Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S2_Recv_BecauseStateEq_CFDP_S2(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_S2;

//     /* Act */
//     // CF_CFDP_DispatchRecv(arg_t);

//     /* Assert */
//     UtAssert_Failed("Deep Setup - uses CF_CFDP_S2_Recv, CF_CFDP_ArmInactTimer");
// } /* end Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S2_Recv_BecauseStateEq_CFDP_S2 */

void Test_CF_CFDP_DispatchRecv_WhenStateEq_CFDP_DROP_Call_CF_CFDP_RecvDrop(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                = &dummy_t;
    uint16            initial_recv_dropped = Any_uint16();

    arg_t->state = CF_TxnState_DROP;

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped = initial_recv_dropped;

    /* Arrange for CF_CFDP_ArmInactTimer */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table = &dummy_config_table;

    /* Act */
    CF_CFDP_DispatchRecv(arg_t);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped == (uint16)(initial_recv_dropped + 1),
                  "CF_AppData recv.dropped is %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped, initial_recv_dropped);
    /* Assert for CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
} /* end Test_CF_CFDP_DispatchRecv_WhenStateEq_CFDP_DROP_Call_CF_CFDP_RecvDrop */

/* end CF_CFDP_DispatchRecv tests */

/*******************************************************************************
**
**  CF_CFDP_DispatchTx tests (small) - full coverage - 2 coverage JIRA issue
**
*******************************************************************************/

void Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_INVALID(void)
{
    // /* Arrange */
    // CF_Transaction_t           dummy_t;
    // CF_Transaction_t*          arg_t = &dummy_t;

    // arg_t->state = CF_TxnState_INVALID;

    // /* Act */
    // CF_CFDP_DispatchTx(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state<CF_TxnState_INVALID");

} /* end Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_INVALID */

/* TODO: Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_GreaterThanOrEqTo_CFDP_INVALID desired but not required */
// void Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_GreaterThanOrEqTo_CFDP_INVALID(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = Any_uint8_GreaterThan(CF_TxnState_INVALID);

//     /* Act */
//     //CF_CFDP_DispatchTx(arg_t);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_GreaterThanOrEqTo_CFDP_INVALID */

void Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_IDLE(void)
{
    // /* Arrange */
    // CF_Transaction_t           dummy_t;
    // CF_Transaction_t*          arg_t = &dummy_t;

    // arg_t->state = CF_TxnState_IDLE;

    // /* Act */
    // CF_CFDP_DispatchTx(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_IDLE */

/* TODO: Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R1 desired but not required */
// void Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R1(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_R1;

//     /* Act */
//     //CF_CFDP_DispatchTx(arg_t);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R1 */

/* TODO: Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R2 desired but not required */
// void Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R2(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_R2;

//     /* Act */
//     //CF_CFDP_DispatchTx(arg_t);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R2 */

/* TODO: Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_DROP desired but not required */
// void Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_DROP(void)
// {
//     /* Arrange */
//     CF_Transaction_t           dummy_t;
//     CF_Transaction_t*          arg_t = &dummy_t;

//     arg_t->state = CF_TxnState_DROP;

//     /* Act */
//     //CF_CFDP_DispatchTx(arg_t);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_DROP */

void Test_CF_CFDP_DispatchTx_When_t_state_EqTo_CFDP_S1_Call_CF_CFDP_S1_Tx(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_Transaction_t *context_CF_CFDP_S1_Tx;

    arg_t->state = CF_TxnState_S1;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_S1_Tx), &context_CF_CFDP_S1_Tx, sizeof(context_CF_CFDP_S1_Tx), false);

    /* Act */
    CF_CFDP_DispatchTx(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_S1_Tx, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_S1_Tx, arg_t);
} /* end Test_CF_CFDP_DispatchTx_When_t_state_EqTo_CFDP_S1_Call_CF_CFDP_S1_Tx */

void Test_CF_CFDP_DispatchTx_Call_When_t_state_EqTo_CFDP_S2_CF_CFDP_S1_Tx(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_Transaction_t *context_CF_CFDP_S2_Tx;

    arg_t->state = CF_TxnState_S2;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_S2_Tx), &context_CF_CFDP_S2_Tx, sizeof(context_CF_CFDP_S2_Tx), false);

    /* Act */
    CF_CFDP_DispatchTx(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_S2_Tx, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_S2_Tx, arg_t);
} /* end Test_CF_CFDP_DispatchTx_Call_When_t_state_EqTo_CFDP_S2_CF_CFDP_S1_Tx */

/* end CF_CFDP_DispatchTx tests */

/*******************************************************************************
**
**  CF_CFDP_FindUnusedChunks tests (small) - full coverage - 2 coverage JIRA issue
**
*******************************************************************************/

void Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsEq_CF_DIR_NUM(void)
{
    /* Arrange */
    // CF_Channel_t       dummy_c;
    // CF_Channel_t*      arg_c = &dummy_c;
    // CF_Direction_t     arg_dir = CF_Direction_NUM;

    /* Act */
    // CF_CFDP_FindUnusedChunks(arg_c, arg_dir);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsEq_CF_DIR_NUM */

/* TODO: Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsGreaterThan_CF_DIR_NUM desired but not required */
// void Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsGreaterThan_CF_DIR_NUM(void)
// {
//     /* Arrange */
//     // CF_Channel_t       dummy_c;
//     // CF_Channel_t*      arg_c = &dummy_c;
//     // CF_Direction_t     arg_dir = Any_uint8_GreaterThan(CF_Direction_NUM);

//     /* Act */
//     //CF_CFDP_FindUnusedChunks(arg_c, arg_dir);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsGreaterThan_CF_DIR_NUM */

void Test_CF_CFDP_FindUnusedChunks_AssertsBecause_c_cs_dir_IsNull(void)
{
    /* Arrange */
    // CF_Channel_t       dummy_c;
    // CF_Channel_t*      arg_c = &dummy_c;
    // CF_Direction_t     arg_dir = Any_direction_t();

    // arg_c->cs[arg_dir] = NULL;

    /* Act */
    // CF_CFDP_FindUnusedChunks(arg_c, arg_dir);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_FindUnusedChunks_AssertsBecause_c_cs_dir_IsNull */

void Test_CF_CFDP_FindUnusedChunks_Success(void)
{
    /* Arrange */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *arg_c   = &dummy_c;
    CF_Direction_t         arg_dir = Any_direction_t();
    CF_ChunkWrapper_t      dummy_expected_result;
    CF_ChunkWrapper_t     *expected_result = &dummy_expected_result;
    CF_ChunkWrapper_t     *local_result;
    CF_CListNode_t        *forced_return_CF_CList_Pop;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &expected_result->cl_node;
    arg_c->cs[arg_dir]                 = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);

    /* Act */
    local_result = CF_CFDP_FindUnusedChunks(arg_c, arg_dir);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, expected_result);
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_Pop.head, &arg_c->cs[arg_dir]);
} /* end Test_CF_CFDP_FindUnusedChunks_Success */

/* end CF_CFDP_FindUnusedChunks tests */

/*******************************************************************************
**
**  CF_CFDP_FindUnusedTransaction tests (large) - full coverage - 3 coverage JIRA issue - [unstubbables:
*CF_CList_Remove_Ex]
**
*******************************************************************************/

void Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_c_IsNull(void)
{
    // /* Arrange */
    // CF_Channel_t*          arg_c;

    // /* Act */
    // CF_CFDP_FindUnusedTransaction(arg_c);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeTransactionsAreAvailableReturn_NULL */

void Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_q_size_CF_Q_HIST_IsNotLessThan_CF_NUM_HISTORIES_PER_CHANNEL(void)
{
    // /* Arrange */
    // CF_Channel_t*          arg_c;

    // /* Act */
    // CF_CFDP_FindUnusedTransaction(arg_c);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_q_size_CF_Q_HIST_IsNotLessThan_CF_NUM_HISTORIES_PER_CHANNEL
   */

void Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_c_qs_CF_Q_HIST_IsNull(void)
{
    // /* Arrange */
    // CF_Channel_t*          arg_c;

    // /* Act */
    // CF_CFDP_FindUnusedTransaction(arg_c);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_c_qs_CF_Q_HIST_IsNull */

void Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeTransactionsAreAvailableReturn_NULL(void)
{
    /* Arrange */
    CF_Channel_t      dummy_c;
    CF_Channel_t     *arg_c = &dummy_c;
    CF_Transaction_t *local_result;

    arg_c->qs[CF_QueueIdx_FREE] = NULL; /* No free transactions */

    /* Act */
    local_result = CF_CFDP_FindUnusedTransaction(arg_c);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, NULL);

} /* end Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeTransactionsAreAvailableReturn_NULL */

void Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeHistoryReturnExpected_t(void)
{
    /* Arrange */
    CF_Channel_t             *arg_c;
    CF_CListNode_t           *dummy_n;
    CF_History_t              dummy_history;
    CF_Transaction_t          dummy_expected_t;
    uint8                     dummy_channel = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_Transaction_t         *expected_t    = &dummy_expected_t;
    CF_Transaction_t         *local_result;
    CF_CList_Remove_context_t context_CF_CList_Remove;

    dummy_n                     = &expected_t->cl_node;
    arg_c                       = &CF_AppData.engine.channels[dummy_channel];
    arg_c->qs[CF_QueueIdx_FREE] = dummy_n; /* No free transactions */
    arg_c->qs[CF_QueueIdx_HIST] = &dummy_history.cl_node;

    /* Arrange unstubbable: CF_CList_Remove_Ex */
    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_CF_CList_Remove, sizeof(context_CF_CList_Remove), false);
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_FREE] = Any_uint16_Except(0);
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] = Any_uint16_Except(0);

    /* Act */
    local_result = CF_CFDP_FindUnusedTransaction(arg_c);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, expected_t);
    UtAssert_ADDRESS_EQ(local_result->history, &dummy_history);
    UtAssert_True(
        local_result->history->dir == CF_Direction_NUM,
        "CF_CFDP_FindUnusedTransaction set t->history->dir to %u and should be %u (CF_Direction_NUM - no direction)",
        local_result->history->dir, CF_Direction_NUM);
} /* end Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeHistoryReturnExpected_t */

void Test_CF_CFDP_FindUnusedTransaction_WhenFreeHistoryReturnExpected_t(void)
{
    /* Arrange */
    CF_Channel_t             *arg_c;
    CF_CListNode_t           *dummy_n;
    CF_History_t              dummy_history;
    CF_Transaction_t          dummy_expected_t;
    uint8                     dummy_channel = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_Transaction_t         *expected_t    = &dummy_expected_t;
    CF_Transaction_t         *local_result;
    CF_CList_Remove_context_t context_CF_CList_Remove;

    dummy_n                          = &expected_t->cl_node;
    arg_c                            = &CF_AppData.engine.channels[dummy_channel];
    arg_c->qs[CF_QueueIdx_FREE]      = dummy_n; /* No free transactions */
    arg_c->qs[CF_QueueIdx_HIST_FREE] = &dummy_history.cl_node;
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] =
        Any_uint16_LessThan(CF_NUM_HISTORIES_PER_CHANNEL);

    /* Arrange unstubbable: CF_CList_Remove_Ex */
    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_CF_CList_Remove, sizeof(context_CF_CList_Remove), false);
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_FREE]      = Any_uint16_Except(0);
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST_FREE] = Any_uint16_Except(0);

    /* Act */
    local_result = CF_CFDP_FindUnusedTransaction(arg_c);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, expected_t);
    UtAssert_ADDRESS_EQ(local_result->history, &dummy_history);
    UtAssert_True(
        local_result->history->dir == CF_Direction_NUM,
        "CF_CFDP_FindUnusedTransaction set t->history->dir to %u and should be %u (CF_Direction_NUM - no direction)",
        local_result->history->dir, CF_Direction_NUM);
} /* end Test_CF_CFDP_FindUnusedTransaction_WhenFreeHistoryReturnExpected_t */

/* end CF_CFDP_FindUnusedTransaction tests */

/*******************************************************************************
**
**  CF_CFDP_ResetHistory tests (small) - full coverage - [unstubbables: CF_CList_Remove_Ex, CF_CList_InsertBack_Ex]
**
*******************************************************************************/

void Test_CF_CFDP_ResetHistory_Call_CF_CList_Remove_Ex_And_CF_CList_InsertBack_Ex_WithCorrectParams(void)
{
    /* Arrange */
    CF_Channel_t *arg_c;
    uint8         dummy_channel = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_History_t  dummy_h;
    CF_History_t *arg_h = &dummy_h;

    arg_c = &CF_AppData.engine.channels[dummy_channel];

    /* Arrange unstubbable: CF_CList_Remove_Ex */
    uint16                    inital_q_size_HIST = Any_uint16_Except(0);
    CF_CList_Remove_context_t context_CF_CList_Remove;

    UT_SetDataBuffer(UT_KEY(CF_CList_Remove), &context_CF_CList_Remove, sizeof(context_CF_CList_Remove), false);
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] = inital_q_size_HIST;

    /* Arrange unstubbable: CF_CList_InsertBack_Ex */
    uint16                        inital_q_size_HIST_FREE = Any_uint16_Except(0);
    CF_CList_InsertBack_context_t context_CF_CList_InsertBack;

    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_CF_CList_InsertBack, sizeof(context_CF_CList_InsertBack),
                     false);
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST_FREE] =
        inital_q_size_HIST_FREE;

    /* Act */
    CF_CFDP_ResetHistory(arg_c, arg_h);

    /* Assert */
    /* Assert for CF_CList_Remove */
    UtAssert_ADDRESS_EQ(context_CF_CList_Remove.head, &arg_c->qs[CF_QueueIdx_HIST]);
    UtAssert_ADDRESS_EQ(context_CF_CList_Remove.node, &arg_h->cl_node);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] == inital_q_size_HIST - 1,
        "q_size[CF_QueueIdx_HIST] is %u and should be 1 less than %u (value before call)",
        CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST], inital_q_size_HIST - 1);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 1);
} /* end Test_CF_CFDP_ResetHistory_Call_CF_CList_Remove_Ex_And_CF_CList_InsertBack_Ex_WithCorrectParams */

/* end CF_CFDP_ResetHistory tests */

/*******************************************************************************
**
**  CF_CFDP_FreeTransaction tests (small) - full coverge
**
*******************************************************************************/

void Test_CF_CFDP_FreeTransaction_Call_CF_CList_InitNode_And_CF_CList_InsertBack_Ex_WithCorrectParams(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t          = &dummy_t;
    uint8             dummy_chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_CListNode_t   *context_CF_CList_InitNode;

    arg_t->chan_num = dummy_chan_num;

    UT_SetDataBuffer(UT_KEY(CF_CList_InitNode), &context_CF_CList_InitNode, sizeof(context_CF_CList_InitNode), false);

    /* Arrange unstubbable: CF_CList_InsertBack_Ex */
    uint16                        inital_q_size_HIST_FREE = Any_uint16_Except(0);
    CF_CList_InsertBack_context_t context_CF_CList_InsertBack;

    UT_SetDataBuffer(UT_KEY(CF_CList_InsertBack), &context_CF_CList_InsertBack, sizeof(context_CF_CList_InsertBack),
                     false);
    CF_AppData.hk.channel_hk[&CF_AppData.engine.channels[dummy_chan_num] - CF_AppData.engine.channels]
        .q_size[CF_QueueIdx_FREE] = inital_q_size_HIST_FREE;

    /* Act */
    CF_CFDP_FreeTransaction(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_InitNode, 1);
    UtAssert_ADDRESS_EQ(context_CF_CList_InitNode, &arg_t->cl_node);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 1);
} /* end Test_CF_CFDP_FreeTransaction_Call_CF_CList_InitNode_And_CF_CList_InsertBack_Ex_WithCorrectParams */

/* end CF_CFDP_FreeTransaction tests */

/*******************************************************************************
**
**  CF_CFDP_FindTransactionBySequenceNumber_ tests (small) - full coverage
**
*******************************************************************************/

void Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_src_eid_NotEq_history_src_eid_DoNotFindTransaction(void)
{
    /* Arrange */
    CF_CListNode_t  *arg_n;
    trans_seq_arg_t  dummy_context;
    trans_seq_arg_t *arg_context = &dummy_context;
    CF_Transaction_t dummy_t;
    CF_History_t     dummy_history;
    int              local_result;

    memset(&dummy_context, 0, sizeof(dummy_context));
    memset(&dummy_history, 0, sizeof(dummy_history));

    arg_n = &dummy_t.cl_node;

    dummy_t.history          = &dummy_history;
    dummy_t.history->src_eid = Any_uint8();
    arg_context->src_eid     = Any_uint8_Except(dummy_t.history->src_eid);

    /* Act */
    local_result = CF_CFDP_FindTransactionBySequenceNumber_(arg_n, arg_context);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_FindTransactionBySequenceNumber_ returned %d and should be 0 (Not found)",
                  local_result);
} /* end Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_src_eid_NotEq_history_src_eid_DoNotFindTransaction
   */

void Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_transaction_sequence_number_NotEq_history_seq_num_DoNotFindTransaction(
    void)
{
    /* Arrange */
    CF_CListNode_t  *arg_n;
    trans_seq_arg_t  dummy_context;
    trans_seq_arg_t *arg_context = &dummy_context;
    CF_Transaction_t dummy_t;
    CF_History_t     dummy_history;
    int              local_result;

    arg_n = &dummy_t.cl_node;

    dummy_t.history          = &dummy_history;
    dummy_t.history->src_eid = Any_uint8();
    arg_context->src_eid     = dummy_t.history->src_eid;

    dummy_t.history->seq_num                 = Any_uint32();
    arg_context->transaction_sequence_number = Any_uint32_Except(dummy_t.history->seq_num);

    /* Act */
    local_result = CF_CFDP_FindTransactionBySequenceNumber_(arg_n, arg_context);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_FindTransactionBySequenceNumber_ returned %d and should be 0 (Not found)",
                  local_result);
} /* end
     Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_transaction_sequence_number_NotEq_history_seq_num_DoNotFindTransaction
   */

void Test_CF_CFDP_FindTransactionBySequenceNumber__When_history_And_context_Match_src_eid_And_seq_num_FindTransaction(
    void)
{
    /* Arrange */
    CF_CListNode_t  *arg_n;
    trans_seq_arg_t  dummy_context;
    trans_seq_arg_t *arg_context = &dummy_context;
    CF_Transaction_t dummy_t;
    CF_History_t     dummy_history;
    int              local_result;

    arg_n = &dummy_t.cl_node;

    dummy_t.history          = &dummy_history;
    dummy_t.history->src_eid = Any_uint8();
    arg_context->src_eid     = dummy_t.history->src_eid;

    dummy_t.history->seq_num                 = Any_uint32();
    arg_context->transaction_sequence_number = dummy_t.history->seq_num;

    /* Act */
    local_result = CF_CFDP_FindTransactionBySequenceNumber_(arg_n, arg_context);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_FindTransactionBySequenceNumber_ returned %d and should be 1 (Found)",
                  local_result);
} /* end
     Test_CF_CFDP_FindTransactionBySequenceNumber__When_history_And_context_Match_src_eid_And_seq_num_FindTransaction */

/* end CF_CFDP_FindTransactionBySequenceNumber_ tests */

/*******************************************************************************
**
**  CF_CFDP_FindTransactionBySequenceNumber tests (medium) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_FindTransactionBySequenceNumber_DoNotFindTransaction(void)
{
    /* Arrange */
    CF_Channel_t        dummy_c;
    CF_Channel_t       *arg_c                           = &dummy_c;
    CF_TransactionSeq_t arg_transaction_sequence_number = Any_uint32();
    CF_EntityId_t       arg_src_eid                     = Any_uint8();
    CF_CListNode_t     *expected_ptrs[NUM_CLISTS];
    CF_Transaction_t   *local_result;
    int                 i = 0;

    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    memset(&dummy_c, 0, sizeof(dummy_c));

    expected_ptrs[0] = arg_c->qs[CF_QueueIdx_RX];
    expected_ptrs[1] = arg_c->qs[CF_QueueIdx_PEND];
    expected_ptrs[2] = arg_c->qs[CF_QueueIdx_TXA];
    expected_ptrs[3] = arg_c->qs[CF_QueueIdx_TXW];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Act */
    local_result = CF_CFDP_FindTransactionBySequenceNumber(arg_c, arg_transaction_sequence_number, arg_src_eid);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, NULL);
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    for (i = 0; i < NUM_CLISTS; ++i)
    {
        UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[i].start, expected_ptrs[i]);
        UtAssert_True(context_CF_CList_Traverse[i].fn == (CF_CListFn_t)CF_CFDP_FindTransactionBySequenceNumber_,
                      "context_CF_CList_Traverse[i].fn ==  (CF_CListFn_t )CF_CFDP_FindTransactionBySequenceNumber_");
        UtAssert_True(
            context_CF_CList_Traverse[i].context_transaction_sequence_number == arg_transaction_sequence_number,
            "CF_CList_Traverse received context.transaction_sequence_number %u and should be %u "
            "(transaction_sequence_number)",
            context_CF_CList_Traverse[i].context_transaction_sequence_number, arg_transaction_sequence_number);
        UtAssert_True(context_CF_CList_Traverse[i].context_src_eid == arg_src_eid,
                      "CF_CList_Traverse received context.context_src_eid %u and should be %u (src_eid)",
                      context_CF_CList_Traverse[i].context_src_eid, arg_src_eid);
    }
} /* end Test_CF_CFDP_FindTransactionBySequenceNumber_DoNotFindTransaction */

void Test_CF_CFDP_FindTransactionBySequenceNumber_FindTransactionOnLastClist(void)
{
    /* Arrange */
    CF_Channel_t        dummy_c;
    CF_Channel_t       *arg_c                           = &dummy_c;
    CF_TransactionSeq_t arg_transaction_sequence_number = Any_uint32();
    CF_EntityId_t       arg_src_eid                     = Any_uint8();
    CF_CListNode_t     *expected_ptrs[NUM_CLISTS];
    CF_Transaction_t    dummy_result;
    CF_Transaction_t   *expected_result = &dummy_result;
    CF_Transaction_t   *local_result;
    int                 i = 0;

    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    memset(&dummy_c, 0, sizeof(dummy_c));

    expected_ptrs[0] = arg_c->qs[CF_QueueIdx_RX];
    expected_ptrs[1] = arg_c->qs[CF_QueueIdx_PEND];
    expected_ptrs[2] = arg_c->qs[CF_QueueIdx_TXA];
    expected_ptrs[3] = arg_c->qs[CF_QueueIdx_TXW];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS - 1; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    context_CF_CList_Traverse[NUM_CLISTS - 1].context_forced_t = expected_result;

    /* Act */
    local_result = CF_CFDP_FindTransactionBySequenceNumber(arg_c, arg_transaction_sequence_number, arg_src_eid);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, expected_result);
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    for (i = 0; i < NUM_CLISTS; ++i)
    {
        UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[i].start, expected_ptrs[i]);
        UtAssert_True(context_CF_CList_Traverse[i].fn == (CF_CListFn_t)CF_CFDP_FindTransactionBySequenceNumber_,
                      "context_CF_CList_Traverse[i].fn ==  (CF_CListFn_t )CF_CFDP_FindTransactionBySequenceNumber_");
        UtAssert_True(
            context_CF_CList_Traverse[i].context_transaction_sequence_number == arg_transaction_sequence_number,
            "CF_CList_Traverse received context.transaction_sequence_number %u and should be %u "
            "(transaction_sequence_number)",
            context_CF_CList_Traverse[i].context_transaction_sequence_number, arg_transaction_sequence_number);
        UtAssert_True(context_CF_CList_Traverse[i].context_src_eid == arg_src_eid,
                      "CF_CList_Traverse received context.context_src_eid %u and should be %u (src_eid)",
                      context_CF_CList_Traverse[i].context_src_eid, arg_src_eid);
    }
} /* end Test_CF_CFDP_FindTransactionBySequenceNumber_FindTransactionOnLastClist */

/* end CF_CFDP_FindTransactionBySequenceNumber tests */

/*******************************************************************************
**
**  CF_CFDP_MsgOutGet tests (large) - full coverage - 1 coverage JIRA Issue - []
**
*******************************************************************************/

void Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_notNULL_ReturnPointerTo_msg_ph(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg;
    uint32                initial_outgoing_counter = Any_uint32();
    CF_Transaction_t      dummy_t;
    CF_Transaction_t     *arg_t      = &dummy_t;
    int                   arg_silent = Any_int();
    CF_CFDP_PduHeader_t  *local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = &dummy_msg.cfe_sb_buffer;

    CF_AppData.engine.outgoing_counter = initial_outgoing_counter;

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &dummy_msg.pdu_s_msg.ph);
    UtAssert_True(CF_AppData.engine.outgoing_counter == initial_outgoing_counter,
                  "outgoing_counter is now %u and should not have changed from %u (value before call)",
                  CF_AppData.engine.outgoing_counter, initial_outgoing_counter);
} /* end Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_notNULL_ReturnPointerTo_msg_ph */

void Test_CF_CFDP_MsgOutGet_WhenChannel_max_outgoing_messages_per_wakeup_Is_0_AndChannel_frozen_Is_non0_Return_NULL(
    void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t                    = &dummy_t;
    uint32               initial_outgoing_counter = Any_uint32();
    int                  arg_silent               = Any_int();
    CF_CFDP_PduHeader_t *local_result;

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = NULL;

    CF_AppData.engine.outgoing_counter                                              = initial_outgoing_counter;
    CF_AppData.config_table                                                         = &dummy_config_table;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 1;

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1726 No coverage impact - Does not return NULL when it should, returns 0x10 "
                 "(&((CF_PduSendMsg_t*)NULL)->ph");
    UtPrintf("The returned value is %p", (void *)local_result);
    // UtAssert_NULL(local_result);
    UtAssert_True(CF_AppData.engine.outgoing_counter == initial_outgoing_counter,
                  "outgoing_counter is now %u and should not have changed from %u (value before call)",
                  CF_AppData.engine.outgoing_counter, initial_outgoing_counter);
} /* end Test_CF_CFDP_MsgOutGet_WhenChannel_max_outgoing_messages_per_wakeup_Is_0_AndChannel_frozen_Is_non0_Return_NULL
   */

void Test_CF_CFDP_MsgOutGet_When_outgoing_counter_DoesNotEq_max_outgoing_messages_per_wakeup_And_t_flags_all_suspended_Is_1_Return_NULL(
    void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    uint32               initial_outgoing_counter;
    int                  arg_silent = Any_int();
    CF_CFDP_PduHeader_t *local_result;

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = NULL;

    CF_AppData.config_table                                                         = &dummy_config_table;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = Any_uint32_Except(0);
    CF_AppData.engine.outgoing_counter =
        Any_uint32_Except(CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup);
    initial_outgoing_counter = CF_AppData.engine.outgoing_counter;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 1;

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1726 No coverage impact - Does not return NULL when it should, returns 0x10 "
                 "(&((CF_PduSendMsg_t*)NULL)->ph");
    UtPrintf("The returned value is %p", (void *)local_result);
    // UtAssert_NULL(local_result);;
    UtAssert_True(CF_AppData.engine.outgoing_counter == initial_outgoing_counter,
                  "outgoing_counter is now %u and should not have changed from %u (value before call)",
                  CF_AppData.engine.outgoing_counter, initial_outgoing_counter);
} /* end
     Test_CF_CFDP_MsgOutGet_When_outgoing_counter_DoesNotEq_max_outgoing_messages_per_wakeup_And_t_flags_all_suspended_Is_1_Return_NULL
   */

void Test_CF_CFDP_MsgOutGet_When_max_outgoing_messages_per_wakeup_IsNot_0_And_outgoing_counter_IsEqTo_max_outgoing_messages_per_wakeup_Return_NULL(
    void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    uint32               initial_outgoing_counter;
    int                  arg_silent = Any_int();
    CF_CFDP_PduHeader_t *local_result;

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = NULL;

    CF_AppData.config_table                                                         = &dummy_config_table;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = Any_uint32_Except(0);
    CF_AppData.engine.outgoing_counter =
        CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup;
    initial_outgoing_counter = CF_AppData.engine.outgoing_counter;

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_NULL(local_result);
    UtAssert_True(CF_AppData.engine.outgoing_counter == initial_outgoing_counter,
                  "outgoing_counter is now %u and should not have changed from %u (value before call)",
                  CF_AppData.engine.outgoing_counter, initial_outgoing_counter);
} /* end
     Test_CF_CFDP_MsgOutGet_When_max_outgoing_messages_per_wakeup_IsNot_0_And_outgoing_counter_IsEqTo_max_outgoing_messages_per_wakeup_Return_NULL
   */

void Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_nonNULL_ButSecondIfCheckingSameValueIs_NULL_WhichDoesNotAppearToBePossible(
    void)
{
    UtAssert_MIR("JIRA: GSFCCFS-1727 Cannot hit branch where CF_AppData.engine.out.msg is checked again for NULL when "
                 "nothing between first and second check could have changed it, it can NEVER be NULL");
} /* end
     Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_nonNULL_ButSecondIfCheckingSameValueIs_NULL_WhichDoesNotAppearToBePossible
   */

void Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_AppData_engine_out_msg_IsStill_NULL_SendEventBecause_silent_Is_0_Return_NULL(
    void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    uint32               initial_outgoing_counter;
    int                  arg_silent    = 0;
    const char          *expected_Spec = "CF: no output message buffer available";
    CF_CFDP_PduHeader_t *local_result;

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = NULL;

    CF_AppData.config_table                                                         = &dummy_config_table;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = Any_uint32_Except(0);
    CF_AppData.engine.outgoing_counter =
        Any_uint32_LessThan(CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup);
    initial_outgoing_counter = CF_AppData.engine.outgoing_counter;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    CF_AppData.config_table->chan[arg_t->chan_num].sem_name[0] = Any_char_Except(0);
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), Any_int32_Except(OS_SUCCESS));

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_NULL(local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_NO_MSG,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_NO_MSG)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_NO_MSG);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.engine.outgoing_counter == initial_outgoing_counter,
                  "outgoing_counter is now %u and should not have changed from %u (value before call)",
                  CF_AppData.engine.outgoing_counter, initial_outgoing_counter);
} /* end
     Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_AppData_engine_out_msg_IsStill_NULL_SendEventBecause_silent_Is_0_Return_NULL
   */

void Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_AppData_engine_out_msg_IsStill_NULL_SendNoEventBecause_silent_Is_non0_Return_NULL(
    void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    uint32               initial_outgoing_counter;
    int                  arg_silent = Any_int_Except(0);
    CF_CFDP_PduHeader_t *local_result;

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = NULL;

    CF_AppData.config_table                                                         = &dummy_config_table;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = Any_uint32_Except(0);
    CF_AppData.engine.outgoing_counter =
        Any_uint32_LessThan(CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup);
    initial_outgoing_counter = CF_AppData.engine.outgoing_counter;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    CF_AppData.config_table->chan[arg_t->chan_num].sem_name[0] = Any_char_Except(0);
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), Any_int32_Except(OS_SUCCESS));

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_NULL(local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(CF_AppData.engine.outgoing_counter == initial_outgoing_counter,
                  "outgoing_counter is now %u and should not have changed from %u (value before call)",
                  CF_AppData.engine.outgoing_counter, initial_outgoing_counter);
} /* end
     Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_AppData_engine_out_msg_IsStill_NULL_SendEventBecause_silent_Is_0_Return_NULL
   */

void Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_ThenReturn_outgoing_msg_ph_Address(
    void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg;
    CFE_SB_Buffer_t      *forced_return_CFE_SB_AllocateMessageBuffer = &dummy_msg.cfe_sb_buffer;
    cf_config_table_t     dummy_config_table;
    CF_Transaction_t      dummy_t;
    CF_Transaction_t     *arg_t = &dummy_t;
    uint32                initial_outgoing_counter;
    int                   arg_silent = 0;
    CF_CFDP_PduHeader_t  *local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = NULL;

    CF_AppData.config_table                                                         = &dummy_config_table;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = Any_uint32_Except(0);
    CF_AppData.engine.outgoing_counter =
        Any_uint32_LessThan(CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup);
    initial_outgoing_counter = CF_AppData.engine.outgoing_counter;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    CF_AppData.config_table->chan[arg_t->chan_num].sem_name[0] = Any_char_Except(0);
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), OS_SUCCESS);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_AllocateMessageBuffer),
                          handler_CFE_SB_AllocateMessageBuffer_ReturnForced_CFE_SB_Buffer,
                          forced_return_CFE_SB_AllocateMessageBuffer);

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &((CF_PduSendMsg_t *)forced_return_CFE_SB_AllocateMessageBuffer)->ph);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_UINT32_EQ(CF_AppData.engine.outgoing_counter, initial_outgoing_counter + 1);
} /* end
     Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_ThenReturn_outgoing_msg_ph_Address
   */

void Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_ThenReturn_outgoing_msg_ph_Address(
    void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg;
    CFE_SB_Buffer_t      *forced_return_CFE_SB_AllocateMessageBuffer = &dummy_msg.cfe_sb_buffer;
    cf_config_table_t     dummy_config_table;
    CF_Transaction_t      dummy_t;
    CF_Transaction_t     *arg_t = &dummy_t;
    uint32                initial_outgoing_counter;
    int                   arg_silent = 0;
    CF_CFDP_PduHeader_t  *local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_t->chan_num = Any_cf_chan_num();

    CF_AppData.engine.out.msg = NULL;

    CF_AppData.config_table                                                         = &dummy_config_table;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = Any_uint32_Except(0);
    CF_AppData.engine.outgoing_counter =
        Any_uint32_LessThan(CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup);
    initial_outgoing_counter = CF_AppData.engine.outgoing_counter;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    CF_AppData.config_table->chan[arg_t->chan_num].sem_name[0] = '\0';
    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), OS_SUCCESS);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_AllocateMessageBuffer),
                          handler_CFE_SB_AllocateMessageBuffer_ReturnForced_CFE_SB_Buffer,
                          forced_return_CFE_SB_AllocateMessageBuffer);

    /* Act */
    local_result = CF_CFDP_MsgOutGet(arg_t, arg_silent);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &dummy_msg.content.cfdp.common);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_MSG_Init, 1);
    UtAssert_UINT32_EQ(CF_AppData.engine.outgoing_counter, initial_outgoing_counter + 1);
} /* end
     Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_ThenReturn_outgoing_msg_ph_Address
   */

/* end CF_CFDP_MsgOutGet tests */

/*******************************************************************************
**
**  CF_CFDP_Send tests (small) - full coverage - 1 coverage JIRA issue - []
**
*******************************************************************************/

void Test_CF_CFDP_Send_AssertsBecause_chan_num_IsEqTo_CF_NUM_CHANNELS(void)
{
    // /* Arrange */
    // uint8         arg_chan_num = CF_NUM_CHANNELS;
    // uint32        arg_len;

    // /* Act */
    // CF_CFDP_Send(arg_chan_num, arg_len);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_Send_AssertsBecause_chan_num_IsEqTo_CF_NUM_CHANNELS */

/* TODO: Test_CF_CFDP_Send_AssertsBecause_chan_num_IsGreaterThanTo_CF_NUM_CHANNELS not required but desired */
// void Test_CF_CFDP_Send_AssertsBecause_chan_num_IsGreaterThanTo_CF_NUM_CHANNELS(void)
// {
//     /* Arrange */
//     // uint8         arg_chan_num = Any_uint8_GreaterThan(CF_NUM_CHANNELS);
//     // uint32        arg_len;

//     /* Act */
//     //CF_CFDP_Send(arg_chan_num, arg_len);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_Send_AssertsBecause_chan_num_IsGreaterThanTo_CF_NUM_CHANNELS */

void Test_CF_CFDP_Send_InitializeMsgTimestampsItAndSendsIt(void)
{
    /* Arrange */
    uint8                 arg_chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    uint32                arg_len      = Any_uint32();
    uint32                initial_pdu  = CF_AppData.hk.channel_hk[arg_chan_num].counters.sent.pdu;
    CF_UT_outmsg_buffer_t expected_msg;
    CFE_TIME_SysTime_t    fake_time;

    memset(&expected_msg, 0, sizeof(expected_msg));

    CF_AppData.engine.out.msg = &expected_msg.cfe_sb_buffer;

    Any_CFE_TIME_SysTime_Set(&fake_time);
    UT_SetDataBuffer(UT_KEY(CFE_TIME_GetTime), &fake_time, sizeof(fake_time), false);

    UT_SetHookFunction(UT_KEY(CFE_MSG_SetMsgTime), stub_reporter, &context_CFE_MSG_SetMsgTime);

    // CF_AppData.engine.hdl = dummy_hdl;

    /* Act */
    CF_CFDP_Send(arg_chan_num, arg_len);

    uint32 resultant_pdu = CF_AppData.hk.channel_hk[arg_chan_num].counters.sent.pdu;

    /* Assert */
    UtAssert_STUB_COUNT(CFE_MSG_SetMsgTime, 1);
    UtAssert_ADDRESS_EQ(context_CFE_MSG_SetMsgTime.MsgPtr, &expected_msg);
    UtAssert_True(context_CFE_MSG_SetMsgTime.Time.Seconds == fake_time.Seconds,
                  "CFE_MSG_SetMsgTime received Time.Seconds %u and should be %u (call to CFE_TIME_GetTime Seconds)",
                  context_CFE_MSG_SetMsgTime.Time.Seconds, fake_time.Seconds);
    UtAssert_True(
        context_CFE_MSG_SetMsgTime.Time.Subseconds == fake_time.Subseconds,
        "CFE_MSG_SetMsgTime received Time.Subseconds %u and should be %u (call to CFE_TIME_GetTime Subseconds)",
        context_CFE_MSG_SetMsgTime.Time.Subseconds, fake_time.Subseconds);
    UtAssert_STUB_COUNT(CFE_SB_TransmitBuffer, 1);
    UtAssert_True(
        resultant_pdu == initial_pdu + 1,
        "CF_AppData.hk.channel_hk[chan_num].counters.sent.pdu is %d and should be 1 more than %d (value before call)",
        resultant_pdu, initial_pdu);
    UtAssert_ADDRESS_EQ(CF_AppData.engine.out.msg, NULL);
} /* end Test_CF_CFDP_Send_InitializeMsgTimestampsItAndSendsIt */

/* end CF_CFDP_Send tests */

/*******************************************************************************
**
**  CF_CFDP_SetPduLength tests (simple) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_SetPduLength_SetGiven_ph_length_ToGiven_length(void)
{
    /* Arrange */
    CF_CFDP_PduHeader_t  dummy_ph;
    CF_CFDP_PduHeader_t *arg_ph     = &dummy_ph;
    uint16               arg_length = Any_uint16();

    arg_ph->length = Any_uint16_Except(arg_length);
    /* Act */
    CF_CFDP_SetPduLength(arg_ph, arg_length);

    /* Assert */
    UtAssert_True(arg_ph->length == arg_length, "ph->length is %u and should be %u (length)", arg_ph->length,
                  arg_length);
} /* end Test_CF_CFDP_SetPduLength_SetGiven_ph_length_ToGiven_length */

/* end CF_CFDP_SetPduLength tests */

/*******************************************************************************
**
**  CF_CFDP_ConstructPduHeader tests (medium) - full coverage - [unstubbables: CF_CFDP_MsgOutGet (large),
*CF_CFDP_GetClass (simple),]
**
*******************************************************************************/

void Test_CF_CFDP_ConstructPduHeader_CallTo_CF_CFDP_MsgOutGet_Returns_NULL_DoNothingReturn_NULL(void)
{
    /* Arrange */
    uint8                dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t    *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    uint8                arg_directive_code    = Any_uint8();
    CF_EntityId_t        arg_src_eid           = Any_uint8();
    CF_EntityId_t        arg_dst_eid           = Any_uint8();
    uint8                arg_towards_sender    = Any_uint8();
    CF_TransactionSeq_t  arg_tsn               = Any_uint32();
    int                  arg_silent            = Any_int();
    CF_CFDP_PduHeader_t *local_result;

    /* Arrange for CF_CFDP_MsgOutGet*/
    CF_AppData.engine.out.msg                                                       = NULL;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), Any_uint32_Except(OS_SUCCESS));

    /* Act */
    local_result = CF_CFDP_ConstructPduHeader(arg_t, arg_directive_code, arg_src_eid, arg_dst_eid, arg_towards_sender,
                                              arg_tsn, arg_silent);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, NULL);
} /* end Test_CF_CFDP_ConstructPduHeader_CallTo_CF_CFDP_MsgOutGet_Returns_NULL_DoNothingReturn_NULL */

void Test_CF_CFDP_ConstructPduHeader_Given_directive_code_0_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg;
    uint8                 dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t     *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    uint8                 arg_directive_code    = 0;
    CF_EntityId_t         arg_src_eid           = Any_uint8();
    CF_EntityId_t         arg_dst_eid           = Any_uint8();
    uint8                 arg_towards_sender    = Any_uint8();
    CF_TransactionSeq_t   arg_tsn               = Any_uint32();
    int                   arg_silent            = Any_int();
    CF_CFDP_PduHeader_t  *local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    /* Arrange for CF_CFDP_MsgOutGet*/
    CF_AppData.engine.out.msg = &dummy_msg.cfe_sb_buffer;

    /* Act */
    local_result = CF_CFDP_ConstructPduHeader(arg_t, arg_directive_code, arg_src_eid, arg_dst_eid, arg_towards_sender,
                                              arg_tsn, arg_silent);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &dummy_msg.pdu_s_msg.ph);
    UtAssert_STUB_COUNT(CF_SetVariableHeader, 1);

    /* Teardown -- TODO:figure out how to get rid of this, test independence is a problem here */
    CF_AppData.engine.out.msg = NULL;
} /* end Test_CF_CFDP_ConstructPduHeader_Given_directive_code_0_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg */

void Test_CF_CFDP_ConstructPduHeader_Given_directive_code_1_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg;
    uint8                 dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t     *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    uint8                 arg_directive_code    = 1;
    CF_EntityId_t         arg_src_eid           = Any_uint8();
    CF_EntityId_t         arg_dst_eid           = Any_uint8();
    uint8                 arg_towards_sender    = Any_uint8();
    CF_TransactionSeq_t   arg_tsn               = Any_uint32();
    int                   arg_silent            = Any_int();
    CF_CFDP_PduHeader_t  *local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    /* Arrange for CF_CFDP_MsgOutGet*/
    CF_AppData.engine.out.msg = &dummy_msg.cfe_sb_buffer;

    /* Act */
    local_result = CF_CFDP_ConstructPduHeader(arg_t, arg_directive_code, arg_src_eid, arg_dst_eid, arg_towards_sender,
                                              arg_tsn, arg_silent);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &dummy_msg.pdu_s_msg.ph);
    UtAssert_STUB_COUNT(CF_SetVariableHeader, 1);

    /* Teardown -- TODO:figure out how to get rid of this, test independence is a problem here */
    CF_AppData.engine.out.msg = NULL;
} /* end Test_CF_CFDP_ConstructPduHeader_Given_directive_code_1_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg */

/* end CF_CFDP_ConstructPduHeader tests */

/*******************************************************************************
**
**  CF_strnlen tests (small) - full coverage - [std_lib_unstubbed: memchr]
**
*******************************************************************************/

void Test_CF_strnlen_When_end_IsNot_NULL_ReturnLengthOfGiven_s(void)
{
    /* Arrange */
    const char arg_s[7]        = "NO NULL";
    size_t     arg_maxlen      = 7; /* 256 is arbitrary and used for small size */
    size_t     expected_length = 7;
    size_t     local_result;

    /* Act */
    local_result = CF_strnlen(arg_s, arg_maxlen);

    /* Assert */
    UtAssert_True(local_result == expected_length, "CF_strnlen returned %lu and should be %lu (length of s)",
                  local_result, expected_length);
} /* end Test_CF_strnlen_When_end_IsNot_NULL_ReturnLengthOfGiven_s */

void Test_CF_strnlen_When_end_Is_NULL_ReturnLengthOfGiven_s(void)
{
    /* Arrange */
    const char arg_s[15]  = "HAS NULL AT 14\0";
    size_t     arg_maxlen = Any_uint8_LessThan(strlen(arg_s));
    size_t     local_result;

    /* Act */
    local_result = CF_strnlen(arg_s, arg_maxlen);

    /* Assert */
    UtAssert_True(local_result == arg_maxlen, "CF_strnlen returned %lu and should be %lu (maxlen)", local_result,
                  arg_maxlen);
    UtAssert_True(local_result != strlen(arg_s), "CF_strnlen returned %lu and should not be %lu (string len)",
                  local_result, strlen(arg_s));
} /* end Test_CF_strnlen_When_end_Is_NULL_ReturnLengthOfGiven_s */

/* end CF_strnlen tests */

/*******************************************************************************
**
**  CF_CFDP_SendMd tests (large) - full coverage - 5 coverage JIRA issue - [unstubbbles: CF_CFDP_ConstructPduHeader
*(medium), CF_CFDP_CopyDataToLv (small), CF_CFDP_SetPduLength (simple), CF_CFDP_Send (small)]
**
*******************************************************************************/

void Test_CF_CFDP_SendMd_GetNull_pdu_header_Return_CF_SEND_NO_MSG(void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_SendRet_t      local_result;

    /* Arrange unstubbable: CF_CFDP_ConstructPduHeader and CF_CFDP_MsgOutGet */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table            = &dummy_config_table;
    arg_t->chan_num                    = Any_uint8_LessThan(CF_NUM_CHANNELS);
    arg_t->history                     = &dummy_history;
    CF_AppData.engine.outgoing_counter = Any_uint32_Except(0);
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup =
        CF_AppData.engine.outgoing_counter;

    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    local_result = CF_CFDP_SendMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_NO_MSG, "CF_CFDP_SendMd returned %u and should be %u (CF_SendRet_NO_MSG)",
                  local_result, CF_SendRet_NO_MSG);
} /* end Test_CF_CFDP_SendMd_GetNull_pdu_header_Return_CF_SEND_NO_MSG */

void Test_CF_CFDP_SendMd_AssertsBecause_state_NotEq_CFDP_S1_Or_CFDP_S2(void)
{
    /* Arrange */
    // pdu_msg_t         dummy_msg_out;
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t = &dummy_t;
    // CF_SendRet_t   local_result;
    // int               excepted_states[2] = {CF_TxnState_S1, CF_TxnState_S2};

    // arg_t->state = Any_cfdp_state_t_ExceptThese(excepted_states,
    //   sizeof(excepted_states)/sizeof(excepted_states[0]));

    // /* Arrange unstubbable: CF_CFDP_MsgOutGet */
    // cf_config_table_t   dummy_config_table;

    // CF_AppData.config_table = &dummy_config_table;
    // arg_t->chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    // CF_AppData.engine.outgoing_counter = Any_uint32_Except(0);
    // CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup =
    //   CF_AppData.engine.outgoing_counter + 1;
    // CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    // arg_t->flags.com.suspended = 0;
    // CF_AppData.engine.out.msg = (CFE_SB_Buffer_t*)&dummy_msg_out;

    // arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
    // ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    // local_result = CF_CFDP_SendMd(arg_t);

    /* Assert */
    /* This causes 3 branches to not be taken */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (t->state == CF_TxnState_S1) || (t->state == CF_TxnState_S2)");
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (t->state == CF_TxnState_S1) || (t->state == CF_TxnState_S2)");
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (t->state == CF_TxnState_S1) || (t->state == CF_TxnState_S2)");
} /* end Test_CF_CFDP_SendMd_AssertsBecause_state_NotEq_CFDP_S1_Or_CFDP_S2 */

void Test_CF_CFDP_SendMd_When_src_len_Eq_sizeof_src_filename_Return_CF_SEND_FAILURE(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg_out;
    CF_History_t          dummy_history;
    CF_Transaction_t      dummy_t;
    CF_Transaction_t     *arg_t = &dummy_t;
    CF_SendRet_t          local_result;
    uint8                 passing_states[2] = {CF_TxnState_S1, CF_TxnState_S2};
    size_t                src_filename_size = sizeof(arg_t->history->fnames.src_filename);
    size_t                dst_filename_size = sizeof(arg_t->history->fnames.dst_filename);

    memset(&dummy_msg_out, 0, sizeof(dummy_msg_out));

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.src_filename, src_filename_size);
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename, dst_filename_size);
    arg_t->state = Any_uint8_FromThese(passing_states, sizeof(passing_states) / sizeof(passing_states[0]));

    /* Arrange unstubbable: CF_CFDP_ConstructPduHeader and CF_CFDP_MsgOutGet and CF_CFDP_GetClass */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table            = &dummy_config_table;
    arg_t->chan_num                    = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_AppData.engine.outgoing_counter = 0;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 1;
    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen                                = 0;
    arg_t->flags.com.suspended                                                      = 0;
    CF_AppData.engine.out.msg                                                       = &dummy_msg_out.cfe_sb_buffer;

    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    local_result = CF_CFDP_SendMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_FAILURE,
                  "CF_CFDP_SendMd returned %u and should be %u (CF_SendRet_FAILURE)", local_result, CF_SendRet_FAILURE);
} /* end Test_CF_CFDP_SendMd_When_src_len_Eq_sizeof_src_filename_Return_CF_SEND_FAILURE */

void Test_CF_CFDP_SendMd_When_dst_len_Eq_sizeof_dst_filename_Return_CF_SEND_FAILURE(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg_out;
    CF_History_t          dummy_history;
    CF_Transaction_t      dummy_t;
    CF_Transaction_t     *arg_t = &dummy_t;
    CF_SendRet_t          local_result;
    uint8                 passing_states[2] = {CF_TxnState_S1, CF_TxnState_S2};
    size_t                src_filename_size = sizeof(arg_t->history->fnames.src_filename) - 1;
    size_t                dst_filename_size = sizeof(arg_t->history->fnames.dst_filename);

    memset(&dummy_msg_out, 0, sizeof(dummy_msg_out));

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.src_filename, src_filename_size);
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename, dst_filename_size);
    arg_t->state = Any_uint8_FromThese(passing_states, sizeof(passing_states) / sizeof(passing_states[0]));

    /* Arrange for CF_CFDP_ConstructPduHeader and CF_CFDP_MsgOutGet */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table            = &dummy_config_table;
    arg_t->chan_num                    = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_AppData.engine.outgoing_counter = 0;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 1;
    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen                                = 0;
    arg_t->flags.com.suspended                                                      = 0;
    CF_AppData.engine.out.msg                                                       = &dummy_msg_out.cfe_sb_buffer;

    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    local_result = CF_CFDP_SendMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_FAILURE,
                  "CF_CFDP_SendMd returned %u and should be %u (CF_SendRet_FAILURE)", local_result, CF_SendRet_FAILURE);
} /* end Test_CF_CFDP_SendMd_When_dst_len_Eq_sizeof_dst_filename_Return_CF_SEND_FAILURE */

void Test_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_CF_CFDP_CopyDataToLv_Returns_neg1_On_src_Call(void)
{
    // /* Arrange */
    // CF_CFDP_PduMd_t          dummy_msg_out;
    // CF_History_t         dummy_history;
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t = &dummy_t;
    // CF_SendRet_t   local_result;
    // uint8             passing_states[2] = {CF_TxnState_S1, CF_TxnState_S2};
    // size_t            src_filename_size = Any_uint8_LessThan(sizeof(arg_t->history->fnames.src_filename));
    // size_t            dst_filename_size = Any_uint8_LessThan(sizeof(arg_t->history->fnames.dst_filename));

    // arg_t->history = &dummy_history;
    // AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.src_filename, src_filename_size);
    // AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename, dst_filename_size);
    // arg_t->state = Any_uint8_FromThese(passing_states, sizeof(passing_states)/sizeof(passing_states[0]));

    // /* Arrange unstubbable: CF_CFDP_MsgOutGet */
    // cf_config_table_t   dummy_config_table;

    // CF_AppData.config_table = &dummy_config_table;
    // arg_t->chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    // CF_AppData.engine.outgoing_counter = 0;
    // CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 1;
    // CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    // arg_t->flags.com.suspended = 0;
    // CF_AppData.engine.out.msg = (CFE_SB_Buffer_t*)&dummy_msg_out;

    // arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
    // ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    // /* Arrange unstubbable: CF_CFDP_CopyDataToLv */
    // /* no way to arrange it to fail */

    // /* Act */
    // local_result = CF_CFDP_SendMd(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCFCS-1737 Unable to stub CF_CFDP_CopyDataToLv -- cannot hit if(ret<0) for src\nComment in "
                 "code states it 'should not happen' and it cannot be forced to fail.");
} /* end Test_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_CF_CFDP_CopyDataToLv_Returns_neg1_On_src_Call */

void Test_CF_CFDP_SendMd_WhenCallTo_CF_CFDP_CopyDataToLv_Returns_neg1_OnThe_dst_Call_Return_CF_SEND_ERROR(void)
{
    // /* Arrange */
    // CF_CFDP_PduMd_t          dummy_msg_out;
    // CF_History_t         dummy_history;
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t = &dummy_t;
    // CF_SendRet_t   local_result;
    // uint8             passing_states[2] = {CF_TxnState_S1, CF_TxnState_S2};
    // size_t            src_filename_size = Any_uint8_LessThan(sizeof(arg_t->history->fnames.src_filename));
    // size_t            dst_filename_size = Any_uint8_LessThan(sizeof(arg_t->history->fnames.dst_filename));

    // arg_t->history = &dummy_history;
    // AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.src_filename, src_filename_size);
    // AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename, dst_filename_size);
    // arg_t->state = Any_uint8_FromThese(passing_states, sizeof(passing_states)/sizeof(passing_states[0]));

    // /* Arrange unstubbable: CF_CFDP_MsgOutGet */
    // cf_config_table_t   dummy_config_table;

    // CF_AppData.config_table = &dummy_config_table;
    // arg_t->chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);
    // CF_AppData.engine.outgoing_counter = 0;
    // CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 1;
    // CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    // arg_t->flags.com.suspended = 0;
    // CF_AppData.engine.out.msg = (CFE_SB_Buffer_t*)&dummy_msg_out;

    // arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
    // ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    // /* Arrange for CF_CFDP_CopyDataToLv */
    // /* no way to arrange it to fail */

    // /* Act */
    // local_result = CF_CFDP_SendMd(arg_t);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCFCS-1737 Unable to stub CF_CFDP_CopyDataToLv -- cannot hit if(ret<0) for dst\nComment in "
                 "code states it 'should not happen' and it cannot be forced to fail.");
} /* end Test_CF_CFDP_SendMd_WhenCallTo_CF_CFDP_CopyDataToLv_Returns_neg1_OnThe_dst_Call_Return_CF_SEND_ERROR */

void Test_CF_CFDP_SendMd_Return_CF_SEND_SUCCESS(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg_out;
    CF_History_t          dummy_history;
    CF_Transaction_t      dummy_t;
    CF_Transaction_t     *arg_t = &dummy_t;
    CF_SendRet_t          local_result;
    uint8                 passing_states[2] = {CF_TxnState_S1, CF_TxnState_S2};
    size_t                src_filename_size = 1; // TODO: large filenames cause segfault;
    size_t                dst_filename_size = 1; // TODO: large filenames cause segfault;

    memset(&dummy_msg_out, 0, sizeof(dummy_msg_out));

    arg_t->history = &dummy_history;
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.src_filename, src_filename_size);
    AnyRandomStringOfLettersOfLengthCopy(arg_t->history->fnames.dst_filename, dst_filename_size);
    arg_t->state = Any_uint8_FromThese(passing_states, sizeof(passing_states) / sizeof(passing_states[0]));

    /* Arrange unstubbable: CF_CFDP_MsgOutGet */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table            = &dummy_config_table;
    arg_t->chan_num                    = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_AppData.engine.outgoing_counter = 0;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 1;
    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen                                = 0;
    arg_t->flags.com.suspended                                                      = 0;
    CF_AppData.engine.out.msg                                                       = &dummy_msg_out.cfe_sb_buffer;

    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Arrange for CF_CFDP_CopyDataToLv */
    /* no way to arrange it to fail - which for success test is fine */

    /* Act */
    local_result = CF_CFDP_SendMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendMd returned %u and should be %u (CF_SendRet_SUCCESS)", local_result, CF_SendRet_SUCCESS);
} /* end Test_CF_CFDP_SendMd_Return_CF_SEND_SUCCESS */

/* end CF_CFDP_SendMd tests */

/*******************************************************************************
**
**  CF_CFDP_SendFd tests (medium) - full coverage - 1 CF_Assert - [unstubbables: CF_CFDP_ConstructPduHeader (medium),
*CF_CFDP_Send (medium)]
**
*******************************************************************************/

void Test_CF_CFDP_SendFd_When_len_GreaterThan_sizeof_pdu_pd_data_t_Return_CF_SEND_ERROR(void)
{
    /* Arrange */
    CF_Transaction_t *arg_t      = NULL;
    uint32            arg_offset = Any_uint32();
    int               arg_len    = sizeof(CF_CFDP_PduFileDataContent_t) + 1;
    CF_SendRet_t      local_result;

    /* Act */
    local_result = CF_CFDP_SendFd(arg_t, arg_offset, arg_len);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_ERROR, "CF_CFDP_SendMd returned %u and should be %u (CF_SendRet_ERROR)",
                  local_result, CF_SendRet_ERROR);
} /* end Test_CF_CFDP_SendFd_When_len_GreaterThan_sizeof_pdu_pd_data_t_Return_CF_SEND_ERROR */

void Test_CF_CFDP_SendFd_Return_CF_SEND_SUCCESS(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg;
    CF_Transaction_t      dummy_t;
    CF_Transaction_t     *arg_t      = &dummy_t;
    uint32                arg_offset = Any_uint32();
    int                   arg_len    = sizeof(CF_CFDP_PduFileDataContent_t) - 1;
    CF_SendRet_t          local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.out.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->chan_num = Any_cf_chan_num();

    /* Arrange unstubbable: CF_CFDP_SetPduLength */

    /* Arrange unstubbable: CF_CFDP_Send */

    /* Act */
    local_result = CF_CFDP_SendFd(arg_t, arg_offset, arg_len);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendMd returned %u and should be %u (CF_SendRet_SUCCESS)", local_result, CF_SendRet_SUCCESS);
} /* end Test_CF_CFDP_SendFd_Return_CF_SEND_SUCCESS */

/* end CF_CFDP_SendFd tests */

/*******************************************************************************
**
**  CF_CFDP_FinishEofAck tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_FinishEofAck_SetExpectedValues(void)
{
    /* Arrange */
    CF_CFDP_tlv_t     dummy_tlv;
    CF_CFDP_tlv_t    *arg_tlv   = &dummy_tlv;
    CF_EntityId_t     dummy_eid = Any_uint8();
    cf_config_table_t dummy_config_table;
    int               forced_return_CF_GetMemcpySize = Any_int();
    int               local_result;

    CF_AppData.config_table            = &dummy_config_table;
    CF_AppData.config_table->local_eid = dummy_eid;

    UT_SetDataBuffer(UT_KEY(CF_GetMemcpySize), &forced_return_CF_GetMemcpySize, sizeof(forced_return_CF_GetMemcpySize),
                     false);

    /* Act */
    local_result = CF_CFDP_FinishEofAck(arg_tlv);

    /* Assert */
    UtAssert_True(local_result == offsetof(CF_CFDP_tlv_t, data) + forced_return_CF_GetMemcpySize,
                  "CF_CFDP_FinishEofAck returned %d and should be %lu (offsetof(CF_CFDP_tlv_t, data) + %d (csize)",
                  local_result, offsetof(CF_CFDP_tlv_t, data), forced_return_CF_GetMemcpySize);
    UtAssert_STUB_COUNT(CF_GetMemcpySize, 1);
    UtAssert_STUB_COUNT(CF_MemcpyToBE, 1);
} /* end Test_CF_CFDP_FinishEofAck_SetExpectedValues */

/* end CF_CFDP_FinishEofAck tests */

/*******************************************************************************
**
**  CF_CFDP_SendEof tests (medium) - full coverage - [unstubbables: CF_CFDP_ConstructPduHeader (medium - unstubbables:
*CF_CFDP_MsgOutGet (large), CF_CFDP_GetClass (simple) ), CF_CFDP_FinishEofAck (small), CF_CFDP_SetPduLength (simple),
*CF_CFDP_Send (small)]
**
*******************************************************************************/

void Test_CF_CFDP_SendEof_Get_NULL_pdu_Return_CF_SEND_NO_MSG(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    uint8             dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_SendRet_t      local_result;

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    /* Arrange for CF_CFDP_MsgOutGet*/
    CF_AppData.engine.out.msg                                                       = NULL;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), Any_uint32_Except(OS_SUCCESS));

    /* Act */
    local_result = CF_CFDP_SendEof(arg_t);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_NO_MSG, "CF_CFDP_SendEof returned %u and should be %u (CF_SendRet_NO_MSG)",
                  local_result, CF_SendRet_NO_MSG);
} /* end Test_CF_CFDP_SendEof_Get_NULL_pdu_Return_CF_SEND_NO_MSG */

void Test_CF_CFDP_SendEof_SuccessWithNoError(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_eof;
    cf_config_table_t     dummy_config_table;
    CF_History_t          dummy_history;
    uint8                 dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t     *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_SendRet_t          local_result;

    memset(&dummy_eof, 0, sizeof(dummy_eof));

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;
    arg_t->history->cc      = CF_CFDP_ConditionCode_NO_ERROR;

    /* Arrange for CF_CFDP_MsgOutGet*/
    CF_AppData.engine.out.msg                                                       = &dummy_eof.cfe_sb_buffer;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), Any_uint32_Except(OS_SUCCESS));

    /* Act */
    local_result = CF_CFDP_SendEof(arg_t);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendEof returned %u and should be %u (CF_SendRet_SUCCESS)", local_result,
                  CF_SendRet_SUCCESS);
} /* end Test_CF_CFDP_SendEof_SuccessWithNoError */

void Test_CF_CFDP_SendEof_SuccessWithError(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_eof;
    cf_config_table_t     dummy_config_table;
    CF_History_t          dummy_history;
    uint8                 dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t     *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_SendRet_t          local_result;

    memset(&dummy_eof, 0, sizeof(dummy_eof));

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;
    arg_t->history->cc      = Any_uint8_Except(CF_CFDP_ConditionCode_NO_ERROR);

    /* Arrange for CF_CFDP_MsgOutGet*/
    CF_AppData.engine.out.msg                                                       = &dummy_eof.cfe_sb_buffer;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemTimedWait), Any_uint32_Except(OS_SUCCESS));

    /* Act */
    local_result = CF_CFDP_SendEof(arg_t);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendEof returned %u and should be %u (CF_SendRet_SUCCESS)", local_result,
                  CF_SendRet_SUCCESS);
    /* Assert for CF_CFDP_FinishEofAck */
    UtAssert_STUB_COUNT(CF_MemcpyToBE, 1);
} /* end Test_CF_CFDP_SendEof_SuccessWithError */

/* end CF_CFDP_SendEof tests */

/*******************************************************************************
**
**  CF_CFDP_SendAck tests (medium) - full coverage - 1 coverage JIRA issue - [unstubbables: CF_CFDP_IsSender (simple),
*CF_CFDP_ConstructPduHeader (medium - unstubbables: CF_CFDP_MsgOutGet (large), CF_CFDP_GetClass (simple) ),
*CF_CFDP_SetPduLength (simple), CF_CFDP_Send (small)]
**
*******************************************************************************/

void Test_CF_CFDP_SendAck_When_CF_CFDP_IsSender_Returns_false_Get_NULL_ph_Return_CF_SEND_NO_MSG(void)
{
    /* Arrange */
    cf_config_table_t       dummy_config_table;
    CF_History_t            dummy_history;
    uint8                   dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t       *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_CFDP_AckTxnStatus_t  arg_ts                = Any_uint8();
    CF_CFDP_FileDirective_t arg_dir_code          = Any_uint8();
    CF_CFDP_ConditionCode_t arg_cc                = Any_uint8();
    CF_EntityId_t           arg_peer_eid          = Any_uint8();
    CF_TransactionSeq_t     arg_tsn               = Any_uint8();
    CF_SendRet_t            local_result;

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    /* Arrange for CF_CFDP_IsSender */
    arg_t->flags.com.q_index = Any_uint8_LessThan(CF_QueueIdx_FREE);
    arg_t->state             = CF_TxnState_DROP; /* ensures false from CF_CFDP_IsSender */

    /* Arrange for CF_CFDP_ConstructPduHeader */
    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.engine.out.msg                                                       = NULL;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    /* Act */
    local_result = CF_CFDP_SendAck(arg_t, arg_ts, arg_dir_code, arg_cc, arg_peer_eid, arg_tsn);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_NO_MSG, "CF_CFDP_SendAck returned %u and should be %u (CF_SendRet_NO_MSG)",
                  local_result, CF_SendRet_NO_MSG);
} /* end Test_CF_CFDP_SendAck_When_CF_CFDP_IsSender_Returns_false_Get_NULL_ph_Return_CF_SEND_NO_MSG */

void Test_CF_CFDP_SendAck_AssertsBecauseGiven_dir_code_Is_Not_PDU_EOF_Or_PDU_FIN(void)
{
    // /* Arrange */
    // CF_CFDP_PduAck_t                   dummy_ack;
    // cf_config_table_t           dummy_config_table;
    // CF_History_t                   dummy_history;
    // uint8                       dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    // CF_Transaction_t*              arg_t = &CF_AppData.engine.transactions[dummy_transaction_num];
    // CF_CFDP_AckTxnStatus_t    arg_ts = Any_uint8();
    // uint8                       exceptions[2] = {CF_CFDP_FileDirective_EOF, CF_CFDP_FileDirective_FIN};
    // CF_CFDP_FileDirective_t            arg_dir_code = Any_uint8_ExceptThese(exceptions, 2);
    // CF_CFDP_ConditionCode_t            arg_cc = Any_uint8();
    // CF_EntityId_t              arg_peer_eid = Any_uint8();
    // CF_TransactionSeq_t        arg_tsn = Any_uint8();
    // CF_SendRet_t             local_result;

    // CF_AppData.config_table = &dummy_config_table;
    // arg_t->history = &dummy_history;

    // /* Arrange for CF_CFDP_IsSender */
    // arg_t->flags.com.q_index = Any_uint8_LessThan(CF_QueueIdx_FREE);
    // arg_t->state = CF_TxnState_S1; /* ensures true from CF_CFDP_IsSender */

    // /* Arrange for CF_CFDP_ConstructPduHeader */
    // /* Arrange for CF_CFDP_MsgOutGet */
    // CF_AppData.engine.out.msg = (CFE_SB_Buffer_t*)&dummy_ack;
    // CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    // CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    // arg_t->flags.com.suspended = 0;

    // /* Act */
    // local_result = CF_CFDP_SendAck(arg_t, arg_ts, arg_dir_code, arg_cc, arg_peer_eid, arg_tsn);

    // /* Assert */
    // UtAssert_True(local_result == CF_SendRet_NO_MSG,
    //   "CF_CFDP_SendAck returned %u and should be %u (CF_SendRet_NO_MSG)",
    //   local_result, CF_SendRet_NO_MSG);
    UtAssert_MIR(
        "JIRA: GSFCCFS-1733 CF_Assert - (dir_code==CF_CFDP_FileDirective_EOF)||(dir_code==CF_CFDP_FileDirective_FIN)");
} /* end Test_CF_CFDP_SendAck_AssertsBecauseGiven_dir_code_Is_Not_PDU_EOF_Or_PDU_FIN */

void Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_EOF_And_CF_CFDP_IsSender_Returns_true_GetMsg_pdu_Return_CF_SEND_NO_MSG(
    void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t   dummy_ack;
    cf_config_table_t       dummy_config_table;
    CF_History_t            dummy_history;
    uint8                   dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t       *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_CFDP_AckTxnStatus_t  arg_ts                = Any_uint8();
    CF_CFDP_FileDirective_t arg_dir_code          = CF_CFDP_FileDirective_EOF;
    CF_CFDP_ConditionCode_t arg_cc                = Any_uint8();
    CF_EntityId_t           arg_peer_eid          = Any_uint8();
    CF_TransactionSeq_t     arg_tsn               = Any_uint8();
    CF_SendRet_t            local_result;

    memset(&dummy_ack, 0, sizeof(dummy_ack));

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    /* Arrange for CF_CFDP_IsSender */
    arg_t->flags.com.q_index = Any_uint8_LessThan(CF_QueueIdx_FREE);
    arg_t->state             = CF_TxnState_S1; /* ensures true from CF_CFDP_IsSender */

    /* Arrange for CF_CFDP_ConstructPduHeader */
    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.engine.out.msg                                                       = &dummy_ack.cfe_sb_buffer;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    /* Act */
    local_result = CF_CFDP_SendAck(arg_t, arg_ts, arg_dir_code, arg_cc, arg_peer_eid, arg_tsn);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendAck returned %u and should be %u (CF_SendRet_SUCCESS)", local_result,
                  CF_SendRet_SUCCESS);
} /* end
     Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_EOF_And_CF_CFDP_IsSender_Returns_true_GetMsg_pdu_Return_CF_SEND_NO_MSG
   */

void Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_FIN_And_CF_CFDP_IsSender_Returns_true_GetMsg_ph_Return_CF_SEND_SUCCESS(
    void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t   dummy_ack;
    cf_config_table_t       dummy_config_table;
    CF_History_t            dummy_history;
    uint8                   dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t       *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_CFDP_AckTxnStatus_t  arg_ts                = Any_uint8();
    CF_CFDP_FileDirective_t arg_dir_code          = CF_CFDP_FileDirective_EOF;
    CF_CFDP_ConditionCode_t arg_cc                = Any_uint8();
    CF_EntityId_t           arg_peer_eid          = Any_uint8();
    CF_TransactionSeq_t     arg_tsn               = Any_uint8();
    CF_SendRet_t            local_result;

    memset(&dummy_ack, 0, sizeof(dummy_ack));

    CF_AppData.config_table = &dummy_config_table;
    arg_t->history          = &dummy_history;

    /* Arrange for CF_CFDP_IsSender */
    arg_t->flags.com.q_index = Any_uint8_LessThan(CF_QueueIdx_FREE);
    arg_t->state             = CF_TxnState_S1; /* ensures true from CF_CFDP_IsSender */

    /* Arrange for CF_CFDP_ConstructPduHeader */
    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.engine.out.msg                                                       = &dummy_ack.cfe_sb_buffer;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    /* Act */
    local_result = CF_CFDP_SendAck(arg_t, arg_ts, arg_dir_code, arg_cc, arg_peer_eid, arg_tsn);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendAck returned %u and should be %u (CF_SendRet_SUCCESS)", local_result,
                  CF_SendRet_SUCCESS);
} /* end
     Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_FIN_And_CF_CFDP_IsSender_Returns_true_GetMsg_ph_Return_CF_SEND_SUCCESS
   */

/* end CF_CFDP_SendAck tests */

/*******************************************************************************
**
**  CF_CFDP_SendFin tests (medium) - full coverage - [unstubbable: CF_CFDP_ConstructPduHeader (medium - unstubbables:
*CF_CFDP_MsgOutGet (large), CF_CFDP_GetClass (simple) ), CF_CFDP_FinishEofAck (small), CF_CFDP_SetPduLength (simple),
*CF_CFDP_Send (small)]
**
*******************************************************************************/

void Test_CF_CFDP_SendFin_Get_NULL_ph_Return_CF_SEND_NO_MSG(void)
{
    /* Arrange */
    cf_config_table_t         dummy_config_table;
    CF_History_t              dummy_history;
    uint8                     dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t         *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_CFDP_FinDeliveryCode_t arg_dc                = Any_uint8();
    CF_CFDP_FinFileStatus_t   arg_fs                = Any_uint8();
    CF_CFDP_ConditionCode_t   arg_cc                = Any_uint8();
    CF_SendRet_t              local_result;

    arg_t->history          = &dummy_history;
    CF_AppData.config_table = &dummy_config_table;

    /* Arrange for CF_CFDP_ConstructPduHeader */
    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.engine.out.msg                                                       = NULL;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    /* Act */
    local_result = CF_CFDP_SendFin(arg_t, arg_dc, arg_fs, arg_cc);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_NO_MSG, "CF_CFDP_SendFin returned %u and should be %u (CF_SendRet_NO_MSG)",
                  local_result, CF_SendRet_NO_MSG);
} /* end Test_CF_CFDP_SendFin_Get_NULL_ph_Return_CF_SEND_NO_MSG */

void Test_CF_CFDP_SendFin_Given_cc_NotEqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t     dummy_ack;
    cf_config_table_t         dummy_config_table;
    CF_History_t              dummy_history;
    uint8                     dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t         *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_CFDP_FinDeliveryCode_t arg_dc                = Any_uint8();
    CF_CFDP_FinFileStatus_t   arg_fs                = Any_uint8();
    CF_CFDP_ConditionCode_t   arg_cc                = CF_CFDP_ConditionCode_POS_ACK_LIMIT_REACHED;
    CF_SendRet_t              local_result;

    memset(&dummy_ack, 0, sizeof(dummy_ack));

    arg_t->history          = &dummy_history;
    CF_AppData.config_table = &dummy_config_table;

    /* Arrange for CF_CFDP_ConstructPduHeader */
    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.engine.out.msg                                                       = &dummy_ack.cfe_sb_buffer;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    /* Act */
    local_result = CF_CFDP_SendFin(arg_t, arg_dc, arg_fs, arg_cc);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendFin returned %u and should be %u (CF_SendRet_SUCCESS)", local_result,
                  CF_SendRet_SUCCESS);
} /* end Test_CF_CFDP_SendFin_Given_cc_NotEqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS */

void Test_CF_CFDP_SendFin_Given_cc_EqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t     dummy_ack;
    cf_config_table_t         dummy_config_table;
    CF_History_t              dummy_history;
    uint8                     dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t         *arg_t                 = &CF_AppData.engine.transactions[dummy_transaction_num];
    CF_CFDP_FinDeliveryCode_t arg_dc                = Any_uint8();
    CF_CFDP_FinFileStatus_t   arg_fs                = Any_uint8();
    CF_CFDP_ConditionCode_t   arg_cc                = CF_CFDP_ConditionCode_NO_ERROR;
    CF_SendRet_t              local_result;

    memset(&dummy_ack, 0, sizeof(dummy_ack));
    memset(&dummy_config_table, 0, sizeof(dummy_config_table));
    memset(&dummy_history, 0, sizeof(dummy_history));

    arg_t->history          = &dummy_history;
    CF_AppData.config_table = &dummy_config_table;

    /* Arrange for CF_CFDP_ConstructPduHeader */
    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.engine.out.msg                                                       = &dummy_ack.cfe_sb_buffer;
    CF_AppData.config_table->chan[arg_t->chan_num].max_outgoing_messages_per_wakeup = 0;

    CF_AppData.hk.channel_hk[arg_t->chan_num].frozen = 0;
    arg_t->flags.com.suspended                       = 0;

    /* Act */
    local_result = CF_CFDP_SendFin(arg_t, arg_dc, arg_fs, arg_cc);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendFin returned %u and should be %u (CF_SendRet_SUCCESS)", local_result,
                  CF_SendRet_SUCCESS);
} /* end Test_CF_CFDP_SendFin_Given_cc_EqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS */

/* end CF_CFDP_SendFin tests */

/*******************************************************************************
**
**  CF_CFDP_SendNak tests (medium) - high coverage - 2 coverage JIRA issue - [unstubbables: CF_CFDP_GetClass (simple),
*CF_CFDP_FinishEofAck (small), CF_CFDP_SetPduLength (simple), CF_CFDP_Send (small)]
**
*******************************************************************************/

void Test_CF_CFDP_SendNak_GetNull_ph_Return_CF_SEND_NO_MSG(void)
{
    // /* Arrange */
    // uint8               dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    // CF_Transaction_t*      arg_t = &CF_AppData.engine.transactions[dummy_transaction_num];
    // int                 arg_num_segment_requests = 0;
    // CF_SendRet_t     local_result;

    // ((CF_PduSendMsg_t*)CF_AppData.engine.out.msg)->ph = NULL;

    // /* Act */
    // local_result = CF_CFDP_SendNak(arg_t, arg_num_segment_requests);

    // /* Assert */
    // UtAssert_True(local_result == CF_SendRet_NO_MSG,
    //   "CF_CFDP_SendNak returned %u and should be %u (CF_SendRet_NO_MSG)",
    //   local_result, CF_SendRet_NO_MSG);
    UtAssert_MIR("JIRA: GSFCCFS-1689 if statement to return CF_SendRet_NO_MSG");
} /* end Test_CF_CFDP_SendNak_GetNull_ph_Return_CF_SEND_NO_MSG */

void Test_CF_CFDP_SendNak_AssertsBecause_CF_CFDP_GetClass_With_t_Eq_CLASS_2(void)
{
    // /* Arrange */
    // CF_CFDP_PduNak_t           dummy_nak;
    // uint8               dummy_transaction_num = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    // CF_Transaction_t*      arg_t = &CF_AppData.engine.transactions[dummy_transaction_num];
    // int                 arg_num_segment_requests;
    // CF_SendRet_t     local_result;

    // CF_AppData.engine.out.msg = (CFE_SB_Buffer_t*)&dummy_nak;

    // /* Arrange for CF_CFDP_GetClass */
    // arg_t->flags.com.q_index = Any_uint8_LessThan(CF_QueueIdx_FREE);
    // arg_t->state = CF_TxnState_S1; /* ensures failre */

    // /* Act */
    // local_result = CF_CFDP_SendNak(arg_t, arg_num_segment_requests);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_CFDP_GetClass(t)==CF_CFDP_CLASS_2");
} /* end Test_CF_CFDP_SendNak_AssertsBecause_CF_CFDP_GetClass_With_t_Eq_CLASS_2 */

void Test_CF_CFDP_SendNak_Success_Return_CF_SEND_SUCCESS(void)
{
    /* Arrange */
    CF_UT_outmsg_buffer_t dummy_msg;
    CF_CFDP_PduNak_t     *dummy_nak;
    uint8                 dummy_chan_num              = Any_cf_chan_num();
    uint8                 dummy_transaction_num       = Any_uint8_LessThan(CF_NUM_TRANSACTIONS);
    CF_Transaction_t     *arg_t                       = &CF_AppData.engine.transactions[dummy_transaction_num];
    int                   arg_num_segment_requests    = 1;
    int                   forced_return_CF_HeaderSize = sizeof(CF_CFDP_PduHeader_t);
    CF_SendRet_t          local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    dummy_nak = &dummy_msg.content.cfdp.secondary.nak;

    CF_AppData.engine.out.msg = &dummy_msg.cfe_sb_buffer;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    dummy_nak->scope_start = 0;
    dummy_nak->scope_end   = 1;

    arg_t->chan_num = dummy_chan_num;

    /* Arrange for CF_CFDP_GetClass */
    arg_t->flags.com.q_index = CF_QueueIdx_PEND;
    arg_t->state             = CF_TxnState_S2; /* ensures pass */

    /* Act */
    local_result = CF_CFDP_SendNak(arg_t, arg_num_segment_requests);

    /* Assert */
    UtAssert_True(local_result == CF_SendRet_SUCCESS,
                  "CF_CFDP_SendNak returned %u and should be %u (CF_SendRet_SUCCESS)", local_result,
                  CF_SendRet_SUCCESS);
} /* end Test_CF_CFDP_SendNak_Success_Return_CF_SEND_SUCCESS */

/* end CF_CFDP_SendNak tests */

/*******************************************************************************
**
**  CF_CFDP_RecvPh tests (large) - full coverge - 2 coverage JIRA issue - []
**
*******************************************************************************/

void Test_CF_CFDP_RecvPh_AssertsBecause_chan_num_GreaterThan_CF_NUM_CHANNELS(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - chan_num<CF_NUM_CHANNELS");
} /* end Test_CF_CFDP_RecvPh_AssertsBecause_chan_num_GreaterThan_CF_NUM_CHANNELS */

void Test_CF_CFDP_RecvPh_AssertsBecause_msg_in_Is_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.engine.in.msg");
} /* end Test_CF_CFDP_RecvPh_AssertsBecause_msg_in_Is_NULL */

void Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsLessThan_hsize_SendEventCountErrorReturn_neg1(void)
{
    /* Arrange */
    uint8                arg_chan_num = Any_cf_chan_num();
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength = Any_int_ZeroOrPositiveLessThan(
        forced_return_CF_HeaderSize); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    uint32 initial_counters_recv_pdu    = Any_uint32();
    uint32 expected_counters_recv_error = Any_uint32();
    int    local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu   = initial_counters_recv_pdu;
    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error = expected_counters_recv_error - 1;

    /* Act */
    local_result = CF_CFDP_RecvPh(arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvPh returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu, initial_counters_recv_pdu);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error, expected_counters_recv_error);
} /* end Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsLessThan_hsize_SendEventCountErrorReturn_neg1 */

void Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsEqTo_hsize_CallTo_CF_GetVariableHeader_IsNot_0_CountErrorReturn_neg1(
    void)
{
    /* Arrange */
    uint8                arg_chan_num = Any_cf_chan_num();
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize; /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int    forced_return_CF_GetVariableHeader = Any_int_Except(0);
    uint32 initial_counters_recv_pdu          = Any_uint32();
    uint32 expected_counters_recv_error       = Any_uint32();
    int    local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu   = initial_counters_recv_pdu;
    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error = expected_counters_recv_error - 1;

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    /* Act */
    local_result = CF_CFDP_RecvPh(arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvPh returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu, initial_counters_recv_pdu);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error, expected_counters_recv_error);
} /* end
     Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsEqTo_hsize_CallTo_CF_GetVariableHeader_IsNot_0_CountErrorReturn_neg1
   */

void Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsGreaterThan_hsize_CallTo_CF_GetVariableHeader_IsNot_0_CountErrorReturn_neg1(
    void)
{
    /* Arrange */
    uint8                arg_chan_num = Any_cf_chan_num();
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength = Any_int_GreaterThan(
        forced_return_CF_HeaderSize); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int    forced_return_CF_GetVariableHeader = Any_int_Except(0);
    uint32 initial_counters_recv_pdu          = Any_uint32();
    uint32 expected_counters_recv_error       = Any_uint32();
    int    local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu   = initial_counters_recv_pdu;
    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error = expected_counters_recv_error - 1;

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    /* Act */
    local_result = CF_CFDP_RecvPh(arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvPh returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu, initial_counters_recv_pdu);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error, expected_counters_recv_error);
} /* end
     Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsGreaterThan_hsize_CallTo_CF_GetVariableHeader_IsNot_0_CountErrorReturn_neg1
   */

void Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_DoesNotEq_bytes_received_SendEventCountErrorReturn_neg1(void)
{
    /* Arrange */
    uint8                arg_chan_num = Any_cf_chan_num();
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength = Any_uint16_GreaterThan(
        forced_return_CF_HeaderSize); /* Any_uint16_GreaterThan used here to ensure that subtracting
                                         forced_return_CF_HeaderSize later cannot result in 0 */
    int    forced_return_CF_GetVariableHeader = 0;
    uint32 initial_counters_recv_pdu          = Any_uint32();
    uint32 expected_counters_recv_error       = Any_uint32();
    int    local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu   = initial_counters_recv_pdu;
    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error = expected_counters_recv_error - 1;

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg_in.pdu_r_msg.ph.length = Any_uint16_LessThan(
        (uint16)(forced_return_CFE_SB_GetUserDataLength -
                 forced_return_CF_HeaderSize)); /* ensures (temp+hsize)!=CF_AppData.engine.in.bytes_received */

    /* Act */
    local_result = CF_CFDP_RecvPh(arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvPh returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu, initial_counters_recv_pdu);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error, expected_counters_recv_error);
} /* end Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_DoesNotEq_bytes_received_SendEventCountErrorReturn_neg1 */

void Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_NotEq_bytes_received_Count_pdu_Return_0(void)
{
    /* Arrange */
    uint8                arg_chan_num = Any_cf_chan_num();
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize +
        Any_uint16(); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int    forced_return_CF_GetVariableHeader = 0;
    uint32 expected_counters_recv_pdu         = Any_uint32();
    uint32 initial_counters_recv_error        = Any_uint32();
    int    local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu   = expected_counters_recv_pdu - 1;
    CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error = initial_counters_recv_error;

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg_in.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Act */
    local_result = CF_CFDP_RecvPh(arg_chan_num);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_RecvPh returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.pdu, expected_counters_recv_pdu);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_chan_num].counters.recv.error, initial_counters_recv_error);
} /* end Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_NotEq_bytes_received_Count_pdu_Return_0 */

/* end CF_CFDP_RecvPh tests */

/*******************************************************************************
**
**  CF_CFDP_RecvMd tests (large) - full coverage - 1 coverage JIRA Issue - [unstubbables: CF_CFDP_CopyDataFromLv
*(small),]
**  NOTE: May be unstable or poor designed CUT, tests were very difficult to write
**
*******************************************************************************/

void Test_CF_CFDP_RecvMd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL(void)
{
    // /* Arrange */
    // CF_AppData.engine.in.msg = NULL;

    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_RecvMd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL */

void Test_CF_CFDP_RecvMd_Has_bytes_received_LessThan_sizof_ph_Plus_size_of_pdu_md_t_CountErrorReturn_neg1(void)
{
    /* Arrange */
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t                        = &dummy_t;
    const char          *expected_Spec                = "CF: metadata packet too short: %d bytes received";
    uint32               expected_counters_recv_error = Any_uint32();
    int                  force_return_CF_HeaderSize   = 0; /* force_return_CF_HeaderSize = 6 as a reasonable size */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;

    CF_AppData.engine.in.bytes_received = Any_uint32_LessThan(sizeof(CF_CFDP_PduMd_t));
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = expected_counters_recv_error - 1;

    /* Act */
    local_result = CF_CFDP_RecvMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvMd returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_MD_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_MD_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_MD_SHORT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, expected_counters_recv_error);

} /* end Test_CF_CFDP_RecvMd_Has_bytes_received_LessThan_sizof_ph_Plus_size_of_pdu_md_t_CountErrorReturn_neg1 */

/* NOTE: Test_CF_CFDP_RecvMd_HasFirst_lv_ret_LessThan_0_SendsEventCountErrorAndReturn_neg1 at received==size */

void Test_CF_CFDP_RecvMd_HasFirst_lv_ret_LessThan_0_SendsEventCountErrorAndReturn_neg1(void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;
    const char          *expected_Spec = "CF: metadata pdu rejected due to invalid length in source filename of 0x%02x";
    uint32               expected_counters_recv_error = Any_uint32();
    int                  force_return_CF_HeaderSize   = sizeof(CF_CFDP_PduHeader_t);
    CF_CFDP_PduMd_t     *dummy_md;
    int                  local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;
    dummy_md                 = &dummy_msg.content.cfdp.secondary.md;

    CF_AppData.engine.in.bytes_received = sizeof(CF_CFDP_PduMd_t) + force_return_CF_HeaderSize;
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    dummy_md->size = Any_uint32();

    arg_t->history = &dummy_history;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CFDP_CopyDataFromLv */
    ((CF_CFDP_lv_t *)dummy_md->filename_lvs)->length = 64; /* 64 is a fail for -1 return */

    arg_t->chan_num                                               = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = expected_counters_recv_error - 1;

    /* Act */
    local_result = CF_CFDP_RecvMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvMd returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_INVALID_SRC_LEN,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_INVALID_SRC_LEN)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_INVALID_SRC_LEN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, expected_counters_recv_error);
} /* end Test_CF_CFDP_RecvMd_HasFirst_lv_ret_LessThan_0_SendsEventCountErrorAndReturn_neg1 */

void Test_CF_CFDP_RecvMd_HasSecond_lv_ret_LessThan_0_BecauseLengthEqSizeSendEventAndReturn_neg1(void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;
    const char          *expected_Spec = "CF: metadata pdu rejected due to invalid length in dest filename of 0x%02x";
    uint32               expected_counters_recv_error = Any_uint32();
    int                  force_return_CF_HeaderSize   = sizeof(CF_CFDP_PduHeader_t);
    CF_CFDP_PduMd_t     *dummy_md;
    int                  local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;
    dummy_md                 = &dummy_msg.content.cfdp.secondary.md;

    CF_AppData.engine.in.bytes_received = sizeof(CF_CFDP_PduMd_t) + force_return_CF_HeaderSize;
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    dummy_md->size = Any_uint32();

    arg_t->history = &dummy_history;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CFDP_CopyDataFromLv */
    ((CF_CFDP_lv_t *)dummy_md->filename_lvs)->length       = 1;  /* 19 is arbitrary but small enough to work */
    ((CF_CFDP_lv_t *)(dummy_md->filename_lvs + 2))->length = 64; /* 64 is a fail for -1 return */

    arg_t->chan_num                                               = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = expected_counters_recv_error - 1;

    /* Act */
    local_result = CF_CFDP_RecvMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvMd returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_INVALID_DST_LEN,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_INVALID_DST_LEN)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_INVALID_DST_LEN);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, expected_counters_recv_error);
} /* end Test_CF_CFDP_RecvMd_HasSecond_lv_ret_LessThan_0_BecauseLengthEqSizeSendEventAndReturn_neg1 */

void Test_CF_CFDP_RecvMd_WhenNoErrorConditions_SendEventAndReturn_0(void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;
    const char          *expected_Spec               = "CF: md received for source: %s, dest: %s";
    uint32               initial_counters_recv_error = Any_uint32();
    int                  force_return_CF_HeaderSize  = sizeof(CF_CFDP_PduHeader_t);
    CF_CFDP_PduMd_t     *dummy_md;
    int                  local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;
    dummy_md                 = &dummy_msg.content.cfdp.secondary.md;

    CF_AppData.engine.in.bytes_received = sizeof(CF_CFDP_PduMd_t) + force_return_CF_HeaderSize;
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    dummy_md->size = Any_uint32();

    arg_t->history = &dummy_history;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Arrange unstubbable: CF_CFDP_CopyDataFromLv */
    ((CF_CFDP_lv_t *)dummy_md->filename_lvs)->length       = 1; /* 19 is arbitrary but small enough to work */
    ((CF_CFDP_lv_t *)(dummy_md->filename_lvs + 2))->length = 1; /* 1 gets us a success, for a non-negative return */

    arg_t->chan_num                                               = Any_cf_chan_num();
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_counters_recv_error;

    /* Act */
    local_result = CF_CFDP_RecvMd(arg_t);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_RecvMd returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_INF_PDU_MD_RECVD,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_INF_PDU_MD_RECVD)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_INF_PDU_MD_RECVD);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_INFORMATION,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_INFORMATION)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_INFORMATION);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_counters_recv_error);
} /* end Test_CF_CFDP_RecvMd_WhenNoErrorConditions_SendEventAndReturn_0 */

/* end CF_CFDP_RecvMd tests */

/*******************************************************************************
**
**  CF_CFDP_RecvFd tests (medium) - full coverage - 1 coverage JIRA issue - []
**
*******************************************************************************/

void Test_CF_CFDP_RecvFd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL(void)
{
    // /* Arrange */
    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.engine.in.msg");
} /* end Test_CF_CFDP_RecvFd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL */

void Test_CF_CFDP_RecvFd_When_bytes_received_LessThan_sizeof_pdu_file_data_header_t_CountsErrorAndReturns_neg1_FailAndSendsEvent(
    void)
{
    /* Arrange */
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg_in;
    const char          *expected_Spec                = "CF: filedata pdu too short: %d bytes received";
    uint32               expected_counters_recv_error = Any_uint32();
    int                  force_return_CF_HeaderSize =
        Any_uint8_Except(0); /* Any_uint8_Except(0) for positive result with reasonably expected size */
    int local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = Any_uint32_LessThan(
        force_return_CF_HeaderSize + sizeof(CF_CFDP_PduFileDataHeader_t)); // TODO Any_CFE_MSG_Size_t_LessThan();

    arg_t->chan_num                                               = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = expected_counters_recv_error - 1;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    /* Act */
    local_result = CF_CFDP_RecvFd(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_FD_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_FD_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_FD_SHORT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, expected_counters_recv_error);
    UtAssert_True(local_result == -1, "CF_CFDP_RecvFd received %d and should be -1", local_result);
} /* end
     Test_CF_CFDP_RecvFd_When_bytes_received_LessThan_sizeof_pdu_file_data_header_t_CountsErrorAndReturns_neg1_FailAndSendsEvent
   */

/* TODO: EXAMPLE: Good example of why not checking things is not verifying behavior */
void Test_CF_CFDP_RecvFd_When_bytes_received_EqTo_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_Success(void)
{
    /* Arrange */
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg_in;
    uint32               initial_counters_recv_error = Any_uint32();
    int                  force_return_CF_HeaderSize =
        Any_uint8_Except(0); /* Any_uint8_Except(0) for positive result with reasonably expected size */
    int local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = force_return_CF_HeaderSize + sizeof(CF_CFDP_PduFileDataHeader_t);

    arg_t->chan_num                                               = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_counters_recv_error;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    /* Act */
    local_result = CF_CFDP_RecvFd(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_counters_recv_error);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end Test_CF_CFDP_RecvFd_When_bytes_received_EqTo_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_Success */

void Test_CF_CFDP_RecvFd_When_bytes_received_GreaterThan_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_Success(
    void)
{
    /* Arrange */
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg_in;
    uint32               initial_counters_recv_error = Any_uint32();
    int                  force_return_CF_HeaderSize =
        Any_uint8_Except(0); /* Any_uint8_Except(0) for positive result with reasonably expected size */
    int local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received =
        Any_uint32_GreaterThan(force_return_CF_HeaderSize + sizeof(CF_CFDP_PduFileDataHeader_t));

    arg_t->chan_num                                               = Any_uint8_LessThan(CF_NUM_CHANNELS);
    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_counters_recv_error;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    /* Act */
    local_result = CF_CFDP_RecvFd(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_UINT32_EQ(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_counters_recv_error);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end
     Test_CF_CFDP_RecvFd_When_bytes_received_GreaterThan_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_Success
   */

/* end CF_CFDP_RecvFd tests */

/*******************************************************************************
**
**  CF_CFDP_RecvEof tests (medium) - full coverage - 1 coverage JIRA issue - []
**
*******************************************************************************/

void Test_CF_CFDP_RecvEof_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.engine.in.msg");
} /* end Test_CF_CFDP_RecvEof_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL */

void Test_CF_CFDP_RecvEof_When_bytes_received_LessThan_offsetof_pdu_eof_t_fault_location_Returns_neg1_Fail(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    const char          *expected_Spec = "CF: eof pdu too short: %d bytes received";
    int                  force_return_CF_HeaderSize =
        Any_uint8_Except(0); /* Any_uint8_Except(0) for positive result with reasonably expected size */
    int local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received =
        Any_uint32_LessThan(force_return_CF_HeaderSize + offsetof(CF_CFDP_PduEof_t, fault_location));

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvEof();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_EOF_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_EOF_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_EOF_SHORT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(local_result == -1, "CF_CFDP_RecvFd received %d and should be -1", local_result);
} /* end Test_CF_CFDP_RecvEof_When_bytes_received_LessThan_offsetof_pdu_eof_t_fault_location_Returns_neg1_Fail */

void Test_CF_CFDP_RecvEof_BytesReceivedEq_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_eof_size_Return_0(
    void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  force_return_CF_HeaderSize =
        Any_uint8_Except(0); /* Any_uint8_Except(0) for positive result with reasonably expected size */
    int local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = force_return_CF_HeaderSize + offsetof(CF_CFDP_PduEof_t, fault_location);

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvEof();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end
     Test_CF_CFDP_RecvEof_BytesReceivedEq_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_eof_size_Return_0
   */

void Test_CF_CFDP_RecvEof_BytesReceivedGreaterThan_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_eof_size_Return_0(
    void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  force_return_CF_HeaderSize =
        Any_uint8_Except(0); /* Any_uint8_Except(0) for positive result with reasonably expected size */
    int local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received =
        Any_uint32_GreaterThan(force_return_CF_HeaderSize + offsetof(CF_CFDP_PduEof_t, fault_location));

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvEof();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end
     Test_CF_CFDP_RecvEof_BytesReceivedGreaterThan_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_eof_size_Return_0
   */

/* end CF_CFDP_RecvEof tests */

/*******************************************************************************
**
**  CF_CFDP_RecvAck tests (small) - full coverage - 1 coverage JIRA issue - []
**
*******************************************************************************/

void Test_CF_CFDP_RecvAck_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL(void)
{
    // /* Arrange */
    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.engine.in.msg");
} /* end Test_CF_CFDP_RecvAck_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL */

void Test_CF_CFDP_RecvAck_FailsBecause_bytes_received_LessThan_sizeof_pdu_ack_t_Returns_neg1(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    const char          *expected_Spec = "CF: ack pdu too short: %d bytes received";
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = Any_uint32_LessThan(sizeof(CF_CFDP_PduAck_t)); // Any_CFE_MSG_Size_t();

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvAck();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_ACK_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_ACK_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_ACK_SHORT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(local_result == -1, "CF_CFDP_RecvFd received %d and should be -1", local_result);
} /* end Test_CF_CFDP_RecvAck_FailsBecause_bytes_received_LessThan_sizeof_pdu_ack_t_Returns_neg1 */

void Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_EqTo_sizeof_pdu_ack_t_Returns_0(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = sizeof(CF_CFDP_PduAck_t);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvAck();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_EqTo_sizeof_pdu_ack_t_Returns_0 */

void Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_GreaterThan_sizeof_pdu_ack_t_Returns_0(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = Any_uint32_GreaterThan(sizeof(CF_CFDP_PduAck_t));

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvAck();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_GreaterThan_sizeof_pdu_ack_t_Returns_0 */

/* end CF_CFDP_RecvAck tests */

/*******************************************************************************
**
**  CF_CFDP_RecvFin tests (small) - full coverage - 1 CF_Assert - []
**
*******************************************************************************/

void Test_CF_CFDP_RecvFin_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL(void)
{
    // /* Arrange */
    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.engine.in.msg");
} /* end Test_CF_CFDP_RecvFin_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL */

void Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_IsLessThan_offsetof_pdu_fin_t_fault_location_Returns_neg1(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    const char          *expected_Spec = "CF: fin pdu too short: %d bytes received";
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = Any_uint32_LessThan(offsetof(CF_CFDP_PduFin_t, fault_location));

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvFin();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_FIN_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_FIN_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_FIN_SHORT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(local_result == -1, "CF_CFDP_RecvFd received %d and should be -1", local_result);
} /* end Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_IsLessThan_offsetof_pdu_fin_t_fault_location_Returns_neg1 */

void Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_Is_1_LessThan_offsetof_pdu_fin_t_fault_location_Returns_neg1(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    const char          *expected_Spec = "CF: fin pdu too short: %d bytes received";
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = offsetof(CF_CFDP_PduFin_t, fault_location) - 1;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvFin();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_FIN_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_FIN_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_FIN_SHORT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(local_result == -1, "CF_CFDP_RecvFd received %d and should be -1", local_result);
} /* end Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_Is_1_LessThan_offsetof_pdu_fin_t_fault_location_Returns_neg1
   */

void Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsEqTo_offsetof_pdu_fin_t_fault_location_Returns_0(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = offsetof(CF_CFDP_PduFin_t, fault_location);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvFin();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsEqTo_offsetof_pdu_fin_t_fault_location_Returns_0 */

void Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsGreaterThan_offsetof_pdu_fin_t_fault_location_Returns_0(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg_in;
    int                  local_result;

    memset(&dummy_msg_in, 0, sizeof(dummy_msg_in));

    CF_AppData.engine.in.msg            = &dummy_msg_in.cfe_sb_buffer;
    CF_AppData.engine.in.bytes_received = Any_uint32_GreaterThan(offsetof(CF_CFDP_PduFin_t, fault_location));

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvFin();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(local_result == 0, "CF_CFDP_RecvFd received %d and should be 0", local_result);
} /* end Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsGreaterThan_offsetof_pdu_fin_t_fault_location_Returns_0 */

/* end CF_CFDP_RecvFin tests */

/*******************************************************************************
**
**  CF_CFDP_RecvNak tests (medium) - full coverage - 2 coverage JIRA issue - []
**
*******************************************************************************/

void Test_CF_CFDP_RecvNak_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - CF_AppData.engine.in.msg");
} /* end Test_CF_CFDP_RecvNak_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL */

void Test_CF_CFDP_RecvNak_AssertsBecause_num_segment_requests_Is_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - num_segment_requests");
} /* end Test_CF_CFDP_RecvNak_AssertsBecause_num_segment_requests_Is_NULL */

void Test_CF_CFDP_RecvNak_FailsBecause_bytes_received_IsLessThan_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_Return_neg1(
    void)
{
    /* Arrange */
    int                  dummy_num_segment_requests;
    int                 *arg_num_segment_requests = &dummy_num_segment_requests;
    CF_UT_inmsg_buffer_t dummy_pdu_msg;
    int                  force_return_CF_HeaderSize =
        Any_uint8_Except(0); /* Any_uint8_Except(0) for positive reasonable sized value for test */
    int local_result;

    memset(&dummy_pdu_msg, 0, sizeof(dummy_pdu_msg));

    CF_AppData.engine.in.msg = &dummy_pdu_msg.cfe_sb_buffer;

    CF_AppData.engine.in.bytes_received =
        Any_uint32_LessThan(force_return_CF_HeaderSize + offsetof(CF_CFDP_PduNak_t, segment_requests));
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    /* Act */
    local_result = CF_CFDP_RecvNak(arg_num_segment_requests);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_RecvAck returned %d and should be -1", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_NAK_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_NAK_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_NAK_SHORT);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
} /* end
     Test_CF_CFDP_RecvNak_FailsBecause_bytes_received_IsLessThan_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_Return_neg1
   */

void Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_Sets_num_segment_requests_To_1_BecauseThatIsTheOnlySegmentThenReturn_0(
    void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg;
    int                  dummy_num_segment_requests;
    int                 *arg_num_segment_requests   = &dummy_num_segment_requests;
    int                  force_return_CF_HeaderSize = Any_uint8_LessThan(
                         offsetof(CF_CFDP_PduNak_t, segment_requests[2]) -
                         offsetof(CF_CFDP_PduNak_t,
                                  segment_requests[1])); /* Any_uint8_LessThan(offsetof(CF_CFDP_PduNak_t, segment_requests[2]) -
                                                     offsetof(CF_CFDP_PduNak_t, segment_requests[1])) provides a value that
                                                     will not interfere with the number of segments */
    int local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    CF_AppData.engine.in.bytes_received = force_return_CF_HeaderSize + offsetof(CF_CFDP_PduNak_t, segment_requests[1]);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    /* Act */
    local_result = CF_CFDP_RecvNak(arg_num_segment_requests);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_RecvAck returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(*arg_num_segment_requests == 1, "Value put into arg_num_segment_requests is %d and should be 1",
                  *arg_num_segment_requests);
} /* end
     Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_Sets_num_segment_requests_To_1_BecauseThatIsTheOnlySegmentThenReturn_0
   */

void Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_OneLessThan_CF_NAK_MAX_SEGMENTS_Sets_num_segment_requests_To_OneLessThan_CF_NAK_MAX_SEGMENTS_ThenReturn_OneLessThan_CF_NAK_MAX_SEGMENTS(
    void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg;
    int                  dummy_num_segment_requests;
    int                 *arg_num_segment_requests   = &dummy_num_segment_requests;
    int                  force_return_CF_HeaderSize = Any_uint8_LessThan(
                         offsetof(CF_CFDP_PduNak_t, segment_requests[2]) -
                         offsetof(CF_CFDP_PduNak_t,
                                  segment_requests[1])); /* Any_uint8_LessThan(offsetof(CF_CFDP_PduNak_t, segment_requests[2]) -
                                                     offsetof(CF_CFDP_PduNak_t, segment_requests[1])) provides a value that
                                                     will not interfere with the number of segments */
    int local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    CF_AppData.engine.in.bytes_received =
        force_return_CF_HeaderSize + offsetof(CF_CFDP_PduNak_t, segment_requests[CF_NAK_MAX_SEGMENTS - 1]);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    /* Act */
    local_result = CF_CFDP_RecvNak(arg_num_segment_requests);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_RecvAck returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(*arg_num_segment_requests == CF_NAK_MAX_SEGMENTS - 1,
                  "Value put into arg_num_segment_requests is %d and should be %d (one less than CF_NAK_MAX_SEGMENTS)",
                  *arg_num_segment_requests, CF_NAK_MAX_SEGMENTS - 1);
} /* end
     Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_OneLessThan_CF_NAK_MAX_SEGMENTS_Sets_num_segment_requests_To_OneLessThan_CF_NAK_MAX_SEGMENTS_ThenReturn_OneLessThan_CF_NAK_MAX_SEGMENTS
   */

void Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_SEGMENTS_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS(
    void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg;
    int                  dummy_num_segment_requests;
    int                 *arg_num_segment_requests   = &dummy_num_segment_requests;
    int                  force_return_CF_HeaderSize = Any_uint8_LessThan(
                         offsetof(CF_CFDP_PduNak_t, segment_requests[2]) -
                         offsetof(CF_CFDP_PduNak_t,
                                  segment_requests[1])); /* Any_uint8_LessThan(offsetof(CF_CFDP_PduNak_t, segment_requests[2]) -
                                                     offsetof(CF_CFDP_PduNak_t, segment_requests[1])) provides a value that
                                                     will not interfere with the number of segments */
    int local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    CF_AppData.engine.in.bytes_received =
        force_return_CF_HeaderSize + offsetof(CF_CFDP_PduNak_t, segment_requests[CF_NAK_MAX_SEGMENTS]);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    /* Act */
    local_result = CF_CFDP_RecvNak(arg_num_segment_requests);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_RecvAck returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(*arg_num_segment_requests == CF_NAK_MAX_SEGMENTS,
                  "Value put into arg_num_segment_requests is %d and should be %d (CF_NAK_MAX_SEGMENTS)",
                  *arg_num_segment_requests, CF_NAK_MAX_SEGMENTS);
} /* end
     Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_SEGMENTS_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS
   */

void Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_SEGMENTS_PlusAnotherSegment_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS(
    void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg;
    int                  dummy_num_segment_requests;
    int                 *arg_num_segment_requests = &dummy_num_segment_requests;
    int                  segment_size =
        offsetof(CF_CFDP_PduNak_t, segment_requests[2]) - offsetof(CF_CFDP_PduNak_t, segment_requests[1]);
    int force_return_CF_HeaderSize = Any_uint8_LessThan(
        segment_size); /* Any_uint8_LessThan(offsetof(CF_CFDP_PduNak_t, segment_requests[2]) -
                          offsetof(CF_CFDP_PduNak_t,
                          segment_requests[1])) provides a value that will not interfere with the number of segments */
    int local_result;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    CF_AppData.engine.in.bytes_received =
        force_return_CF_HeaderSize + offsetof(CF_CFDP_PduNak_t, segment_requests[CF_NAK_MAX_SEGMENTS]) + segment_size;
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    /* Act */
    local_result = CF_CFDP_RecvNak(arg_num_segment_requests);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_RecvAck returned %d and should be 0", local_result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_True(*arg_num_segment_requests == CF_NAK_MAX_SEGMENTS,
                  "Value put into arg_num_segment_requests is %d and should be %d (CF_NAK_MAX_SEGMENTS)",
                  *arg_num_segment_requests, CF_NAK_MAX_SEGMENTS);
} /* end
     Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_SEGMENTS_PlusAnotherSegment_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS
   */

/* end CF_CFDP_RecvNak tests */

/*******************************************************************************
**
**  CF_CFDP_RecvDrop tests (simple) - full coverage
**
*******************************************************************************/

void Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                   = &dummy_t;
    uint16            initial_dropped_counter = Any_uint16();

    arg_t->chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped = initial_dropped_counter;

    /* Act */
    CF_CFDP_RecvDrop(arg_t);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped ==
                      (uint16)(initial_dropped_counter + 1),
                  "CF_CFDP_RecvDrop incremented dropped counter to %u and should be 1 more than %u (value before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped, initial_dropped_counter);
} /* end Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData */

void Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData_AndItRollsOver(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t                   = &dummy_t;
    uint16            initial_dropped_counter = UINT16_MAX;

    arg_t->chan_num = Any_uint8_LessThan(CF_NUM_CHANNELS);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped = initial_dropped_counter;

    /* Act */
    CF_CFDP_RecvDrop(arg_t);

    /* Assert */
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped == 0,
                  "CF_CFDP_RecvDrop incremented dropped counter to %u (roll over) and should be 1 more than %u (value "
                  "before call, max uint16)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.dropped, initial_dropped_counter);
} /* end Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData_AndItRollsOver */

/* end CF_CFDP_RecvDrop tests */

/*******************************************************************************
**
**  CF_CFDP_RecvIdle tests (very large) - full coverage - 1 coverge JIRA issue - [unstubbables: CF_CFDP_FindUnusedChunks
*(small), CF_CFDP_DispatchRecv (small) [unstubbables: CF_CFDP_ArmInactTimer (simple)], CF_CFDP_RecvMd (large),
*CF_CFDP_ResetTransaction (large) [unstubbables: CF_CFDP_IsSender (simple), CF_CList_InsertBack_Ex (simple),
*CF_CFDP_IsSender (simple), CF_CFDP_FreeTransaction (small), [unstubbables: CF_CList_InsertBack_Ex (simple)]]]
**
*******************************************************************************/

void Test_CF_CFDP_RecvIdle_AssertBecause_CF_AppData_engine_in_msg_Is_NULL(void)
{
    // /* Arrange */
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t = &dummy_t;

    // CF_AppData.engine.in.msg = NULL;

    // /* Act */
    // CF_CFDP_RecvIdle(arg_t);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_RecvIdle_AssertBecause_CF_AppData_engine_in_msg_Is_NULL */

void Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_And_PDU_HDR_FLAGS_MODE_AreBoth_true_Set_t_state_To_CFDP_DROP(void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->history = &dummy_history;

    arg_t->chan_num = Any_cf_chan_num();

    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);

    arg_t->state = Any_uint8_Except(CF_TxnState_DROP); /* arg_t->state = Any_uint8_Except(CF_TxnState_DROP) not required
                                                          by helpful for test verification */

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *ptr_c = &dummy_c;
    CF_ChunkWrapper_t      dummy_CF_CFDP_FindUnusedChunks_result;
    CF_ChunkWrapper_t     *forced_CF_CFDP_FindUnusedChunks_result = &dummy_CF_CFDP_FindUnusedChunks_result;
    CF_CListNode_t         dummy_c_list_node;
    CF_CListNode_t        *forced_return_CF_CList_Pop = &dummy_c_list_node;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    CF_AppData.engine.channels[arg_t->chan_num].cs[CF_Direction_RX] = forced_return_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &forced_CF_CFDP_FindUnusedChunks_result->cl_node;
    ptr_c->cs[CF_Direction_RX]         = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);

    /* Act */
    CF_CFDP_RecvIdle(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 2);
    UtAssert_True(arg_t->state == CF_TxnState_DROP, "t->state is %u and should be %u (CF_TxnState_DROP)", arg_t->state,
                  CF_TxnState_DROP);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
} /* end Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_And_PDU_HDR_FLAGS_MODE_AreBoth_true_Set_t_state_To_CFDP_DROP
   */

void Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_NotChangedNoReset(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->history = &dummy_history;

    arg_t->chan_num = Any_cf_chan_num();

    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);
    UT_SetDeferredRetcode(UT_KEY(FGV), 2, 0);

    arg_t->state = Any_uint8_Except(CF_TxnState_R2); /* arg_t->state = Any_uint8_Except(CF_TxnState_R2) not required by
                                                        helpful for test verification */

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *ptr_c = &dummy_c;
    CF_ChunkWrapper_t      dummy_CF_CFDP_FindUnusedChunks_result;
    CF_ChunkWrapper_t     *forced_CF_CFDP_FindUnusedChunks_result = &dummy_CF_CFDP_FindUnusedChunks_result;
    CF_CListNode_t         dummy_c_list_node;
    CF_CListNode_t        *forced_return_CF_CList_Pop = &dummy_c_list_node;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    CF_AppData.engine.channels[arg_t->chan_num].cs[CF_Direction_RX] = forced_return_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &forced_CF_CFDP_FindUnusedChunks_result->cl_node;
    ptr_c->cs[CF_Direction_RX]         = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);

    /* Arrange for CF_CFDP_DispatchRecv, CF_CFDP_ArmInactTimer */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table = &dummy_config_table;

    /* Act */
    CF_CFDP_RecvIdle(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 2);
    UtAssert_True(arg_t->state == CF_TxnState_R2, "t->state is %u and should be %u (CF_TxnState_R2)", arg_t->state,
                  CF_TxnState_R2);
    UtAssert_STUB_COUNT(CF_CFDP_R_Init, 1);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
    /* Assert for CF_CFDP_DispatchRecv (CF_Timer_InitRelSec from CF_CFDP_ArmInactTimer) */
    UtAssert_STUB_COUNT(CF_CFDP_R2_Recv, 1);
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
    /* Assert for CF_CFDP_ResetTransaction */
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 0);

} /* end
     Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_NotChangedNoReset
   */

void Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_ChangedTo_CFDP_IDLE_DoReset(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->history = &dummy_history;

    arg_t->chan_num = Any_cf_chan_num();

    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);
    UT_SetDeferredRetcode(UT_KEY(FGV), 2, 0);

    arg_t->state = Any_uint8_Except(CF_TxnState_R2); /* arg_t->state = Any_uint8_Except(CF_TxnState_R2) not required by
                                                        helpful for test verification */

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *ptr_c = &dummy_c;
    CF_ChunkWrapper_t      dummy_CF_CFDP_FindUnusedChunks_result;
    CF_ChunkWrapper_t     *forced_CF_CFDP_FindUnusedChunks_result = &dummy_CF_CFDP_FindUnusedChunks_result;
    CF_CListNode_t         dummy_c_list_node;
    CF_CListNode_t        *forced_return_CF_CList_Pop = &dummy_c_list_node;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    CF_AppData.engine.channels[arg_t->chan_num].cs[CF_Direction_RX] = forced_return_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &forced_CF_CFDP_FindUnusedChunks_result->cl_node;
    ptr_c->cs[CF_Direction_RX]         = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);

    /* Arrange for CF_CFDP_DispatchRecv, CF_CFDP_ArmInactTimer */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table = &dummy_config_table;

    /* Special arrange for CF_Timer_InitRelSec to change t->state value within call to CF_CFDP_DispatchRecv */
    UT_SetHandlerFunction(UT_KEY(CF_Timer_InitRelSec), Handler_CF_Timer_InitRelSec_Change_t_state_To_CFDP_IDLE, arg_t);
    arg_t->flags.com.q_index                                                   = 0;
    CF_AppData.hk.channel_hk[arg_t->chan_num].q_size[arg_t->flags.com.q_index] = 1;

    arg_t->history->dir = CF_Direction_TX;
    arg_t->p            = NULL; /* arg_t->p = NULL bypasses an unnecessary branch */

    /* Arrange for CF_CFDP_ResetTransaction */
    CF_Channel_t *fake_c = &CF_AppData.engine.channels[arg_t->chan_num];

    fake_c->num_cmd_tx = 1; /* fake_c->num_cmd_tx = 1; bypasses CF_Assert(c->num_cmd_tx) */

    /* Act */
    CF_CFDP_RecvIdle(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 2);
    UtAssert_True(arg_t->state == CF_TxnState_IDLE, "t->state is %u and should be %u (CF_TxnState_IDLE)", arg_t->state,
                  CF_TxnState_IDLE);
    UtAssert_STUB_COUNT(CF_CFDP_R_Init, 1);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
    /* Assert for CF_CFDP_DispatchRecv (CF_Timer_InitRelSec from CF_CFDP_ArmInactTimer) */
    UtAssert_STUB_COUNT(CF_CFDP_R2_Recv, 1);
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
    /* Assert for CF_CFDP_ResetTransaction */
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_ChangedTo_CFDP_IDLE_DoReset
   */

void Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_But_fdh_directive_code_IsNot_PDU_METADATA_SendEventAnd_Increment_recv_error(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;
    const char          *expected_Spec      = "CF: unhandled file directive code 0x%02x in idle state";
    uint8                exceptions[1]      = {CF_TxnState_IDLE};
    CF_TxnState_t        initial_t_state    = Any_cfdp_state_t_ExceptThese(exceptions, 1);
    uint32               initial_recv_error = Any_uint32();

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = Any_file_directive_t_Except(CF_CFDP_FileDirective_METADATA);

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->history = &dummy_history;

    arg_t->chan_num = Any_cf_chan_num();

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    arg_t->state = initial_t_state;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *ptr_c = &dummy_c;
    CF_ChunkWrapper_t      dummy_CF_CFDP_FindUnusedChunks_result;
    CF_ChunkWrapper_t     *forced_CF_CFDP_FindUnusedChunks_result = &dummy_CF_CFDP_FindUnusedChunks_result;
    CF_CListNode_t         dummy_c_list_node;
    CF_CListNode_t        *forced_return_CF_CList_Pop = &dummy_c_list_node;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    CF_AppData.engine.channels[arg_t->chan_num].cs[CF_Direction_RX] = forced_return_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &forced_CF_CFDP_FindUnusedChunks_result->cl_node;
    ptr_c->cs[CF_Direction_RX]         = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);

    /* Act */
    CF_CFDP_RecvIdle(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_FD_UNHANDLED,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_FD_UNHANDLED)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_FD_UNHANDLED);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(arg_t->state == initial_t_state, "t->state is %u and should be %u (unchanged - value before call)",
                  arg_t->state, initial_t_state);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 1,
                  "CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error is %u and should be 1 more than %u (value "
                  "before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
} /* end
     Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_But_fdh_directive_code_IsNot_PDU_METADATA_SendEventAnd_Increment_recv_error
   */

void Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_non0_SendEventAnd_Increment_recv_error(
    void)
{
    /* Arrange */
    CF_History_t                dummy_history;
    CF_Transaction_t            dummy_t;
    CF_Transaction_t           *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t        dummy_msg;
    const char                 *expected_Spec      = "CF: got invalid md pdu -- abandoning transaction";
    uint8                       exceptions[1]      = {CF_TxnState_IDLE};
    CF_TxnState_t               initial_t_state    = Any_cfdp_state_t_ExceptThese(exceptions, 1);
    uint32                      initial_recv_error = Any_uint32();
    CFE_EVS_SendEvent_context_t local_context_CFE_EVS_SendEvent[2];

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = CF_CFDP_FileDirective_METADATA;

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->history = &dummy_history;

    arg_t->chan_num = Any_cf_chan_num();

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    arg_t->state = initial_t_state;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, local_context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *ptr_c = &dummy_c;
    CF_ChunkWrapper_t      dummy_CF_CFDP_FindUnusedChunks_result;
    CF_ChunkWrapper_t     *forced_CF_CFDP_FindUnusedChunks_result = &dummy_CF_CFDP_FindUnusedChunks_result;
    CF_CListNode_t         dummy_c_list_node;
    CF_CListNode_t        *forced_return_CF_CList_Pop = &dummy_c_list_node;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    CF_AppData.engine.channels[arg_t->chan_num].cs[CF_Direction_RX] = forced_return_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &forced_CF_CFDP_FindUnusedChunks_result->cl_node;
    ptr_c->cs[CF_Direction_RX]         = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);
    UT_SetDefaultReturnValue(UT_KEY(CF_HeaderSize), sizeof(CF_CFDP_PduHeader_t));

    /* Arrange for CF_CFDP_RecvMd */
    CF_AppData.engine.in.bytes_received = 0; /* force err_out */

    /* Act */
    CF_CFDP_RecvIdle(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 2);
    /* NOTE: only the second event is covered here, the other is from unstubbable CF_CFDP_RecvMd */
    UtAssert_True(
        local_context_CFE_EVS_SendEvent[1].EventID == CF_EID_ERR_CFDP_IDLE_MD,
        "CFE_EVS_SendEvent 2nd call received EventID %u and should have received %u (CF_EID_ERR_CFDP_IDLE_MD)",
        local_context_CFE_EVS_SendEvent[1].EventID, CF_EID_ERR_CFDP_IDLE_MD);
    UtAssert_True(
        local_context_CFE_EVS_SendEvent[1].EventType == CFE_EVS_EventType_ERROR,
        "CFE_EVS_SendEvent 2nd call received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
        local_context_CFE_EVS_SendEvent[1].EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(local_context_CFE_EVS_SendEvent[1].Spec, expected_Spec,
                    "CFE_EVS_SendEvent 2nd call received expected Spec\n'%s' - Received\n'%s' - Expected",
                    local_context_CFE_EVS_SendEvent[1].Spec, expected_Spec);
    UtAssert_True(arg_t->state == initial_t_state, "t->state is %u and should be %u (unchanged - value before call)",
                  arg_t->state, initial_t_state);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error + 2,
                  "CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error is %u and should be 2 more than %u (value "
                  "before call)",
                  CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
    /* Assert for CF_CFDP_RecvMd */
    UtAssert_True(
        local_context_CFE_EVS_SendEvent[0].EventID == CF_EID_ERR_PDU_MD_SHORT,
        "CFE_EVS_SendEvent 1st call received EventID %u and should have received %u (CF_EID_ERR_PDU_MD_SHORT)",
        local_context_CFE_EVS_SendEvent[0].EventID, CF_EID_ERR_PDU_MD_SHORT);
} /* end
     Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_non0_SendEventAnd_Increment_recv_error
   */

void Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R1_NoReset(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;
    uint8                exceptions[1]      = {CF_TxnState_IDLE};
    CF_TxnState_t        initial_t_state    = Any_cfdp_state_t_ExceptThese(exceptions, 1);
    uint32               initial_recv_error = Any_uint32();

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = CF_CFDP_FileDirective_METADATA;

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->history = &dummy_history;

    arg_t->chan_num = Any_cf_chan_num();

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    arg_t->state = initial_t_state;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    UT_SetDeferredRetcode(UT_KEY(FGV), 2, 1);

    arg_t->flags.rx.md_recv = 0; /* arg_t->flags.rx.md_recv = 0 not required but facilitates testing */

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *ptr_c = &dummy_c;
    CF_ChunkWrapper_t      dummy_CF_CFDP_FindUnusedChunks_result;
    CF_ChunkWrapper_t     *forced_CF_CFDP_FindUnusedChunks_result = &dummy_CF_CFDP_FindUnusedChunks_result;
    CF_CListNode_t         dummy_c_list_node;
    CF_CListNode_t        *forced_return_CF_CList_Pop = &dummy_c_list_node;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    CF_AppData.engine.channels[arg_t->chan_num].cs[CF_Direction_RX] = forced_return_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &forced_CF_CFDP_FindUnusedChunks_result->cl_node;
    ptr_c->cs[CF_Direction_RX]         = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);

    /* Arrange for CF_CFDP_RecvMd */
    CF_AppData.engine.in.bytes_received         = sizeof(CF_CFDP_PduHeader_t) + sizeof(CF_CFDP_PduMd_t);
    int              force_return_CF_HeaderSize = sizeof(CF_CFDP_PduHeader_t);
    CF_CFDP_PduMd_t *dummy_msg_in               = &dummy_msg.content.cfdp.secondary.md;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    dummy_msg_in->size = Any_uint32();

    // /* Arrange for CF_CFDP_CopyDataFromLv */
    ((CF_CFDP_lv_t *)dummy_msg_in->filename_lvs)->length       = 1; /* 19 is arbitrary but small enough to work */
    ((CF_CFDP_lv_t *)(dummy_msg_in->filename_lvs + 2))->length = 1; /* 1 gets us a success, for a non-negative return */

    /* Act */
    CF_CFDP_RecvIdle(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 2);
    /* NOTE: only the second event is covered here, the other is from unstubbable CF_CFDP_RecvMd */
    UtAssert_True(arg_t->state == CF_TxnState_R1, "t->state is %u and should be %u (CF_TxnState_R1)", arg_t->state,
                  CF_TxnState_R1);
    UtAssert_True(arg_t->flags.rx.md_recv == 1, "t->flags.rx.md_recv is %u and should be 1", arg_t->flags.rx.md_recv);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error,
        "CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error is %u and should be %u (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
    /* Assert for CF_CFDP_RecvMd */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_INF_PDU_MD_RECVD,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_INF_PDU_MD_RECVD)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_INF_PDU_MD_RECVD);
} /* end
     Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R1_NoReset
   */

void Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R2_NoReset(
    void)
{
    /* Arrange */
    CF_History_t         dummy_history;
    CF_Transaction_t     dummy_t;
    CF_Transaction_t    *arg_t = &dummy_t;
    CF_UT_inmsg_buffer_t dummy_msg;
    uint8                exceptions[1]      = {CF_TxnState_IDLE};
    CF_TxnState_t        initial_t_state    = Any_cfdp_state_t_ExceptThese(exceptions, 1);
    uint32               initial_recv_error = Any_uint32();

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = CF_CFDP_FileDirective_METADATA;

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    arg_t->history = &dummy_history;

    arg_t->chan_num = Any_cf_chan_num();

    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);

    arg_t->state = initial_t_state;

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter, &context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error = initial_recv_error;

    arg_t->flags.rx.md_recv = 0; /* arg_t->flags.rx.md_recv = 0 not required but facilitates testing */

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_Channel_t           dummy_c;
    CF_Channel_t          *ptr_c = &dummy_c;
    CF_ChunkWrapper_t      dummy_CF_CFDP_FindUnusedChunks_result;
    CF_ChunkWrapper_t     *forced_CF_CFDP_FindUnusedChunks_result = &dummy_CF_CFDP_FindUnusedChunks_result;
    CF_CListNode_t         dummy_c_list_node;
    CF_CListNode_t        *forced_return_CF_CList_Pop = &dummy_c_list_node;
    CF_CList_Pop_context_t context_CF_CList_Pop;

    CF_AppData.engine.channels[arg_t->chan_num].cs[CF_Direction_RX] = forced_return_CF_CList_Pop;

    forced_return_CF_CList_Pop         = &forced_CF_CFDP_FindUnusedChunks_result->cl_node;
    ptr_c->cs[CF_Direction_RX]         = (CF_CListNode_t *)&forced_return_CF_CList_Pop;
    context_CF_CList_Pop.forced_return = forced_return_CF_CList_Pop;

    UT_SetDataBuffer(UT_KEY(CF_CList_Pop), &context_CF_CList_Pop, sizeof(context_CF_CList_Pop), false);

    /* Arrange for CF_CFDP_RecvMd */
    CF_AppData.engine.in.bytes_received         = sizeof(CF_CFDP_PduHeader_t) + sizeof(CF_CFDP_PduMd_t);
    int              force_return_CF_HeaderSize = sizeof(CF_CFDP_PduHeader_t);
    CF_CFDP_PduMd_t *dummy_msg_in               = &dummy_msg.content.cfdp.secondary.md;

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &force_return_CF_HeaderSize);

    dummy_msg_in->size = Any_uint32();

    // /* Arrange for CF_CFDP_CopyDataFromLv */
    ((CF_CFDP_lv_t *)dummy_msg_in->filename_lvs)->length       = 1; /* 19 is arbitrary but small enough to work */
    ((CF_CFDP_lv_t *)(dummy_msg_in->filename_lvs + 2))->length = 1; /* 1 gets us a success, for a non-negative return */

    /* Act */
    CF_CFDP_RecvIdle(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(FGV, 2);
    /* NOTE: only the second event is covered here, the other is from unstubbable CF_CFDP_RecvMd */
    UtAssert_True(arg_t->state == CF_TxnState_R2, "t->state is %u and should be %u (CF_TxnState_R2)", arg_t->state,
                  CF_TxnState_R2);
    UtAssert_True(arg_t->flags.rx.md_recv == 1, "t->flags.rx.md_recv is %u and should be 1", arg_t->flags.rx.md_recv);
    UtAssert_True(
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error == initial_recv_error,
        "CF_AppData.hk.channel_hk[t->chan_num].counters.recv.error is %u and should be %u (value before call)",
        CF_AppData.hk.channel_hk[arg_t->chan_num].counters.recv.error, initial_recv_error);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
    /* Assert for CF_CFDP_RecvMd */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_INF_PDU_MD_RECVD,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_INF_PDU_MD_RECVD)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_INF_PDU_MD_RECVD);
} /* end
     Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R2_NoReset
   */

/* end CF_CFDP_RecvIdle tests */

/*******************************************************************************
**
**  CF_CFDP_InitEngine tests (very large) - full coverage - 1 coverage JIRA issue - [unstubbables:
*CF_CFDP_FreeTransaction (small), [unstubbables: CF_CList_InsertBack_Ex (simple)], CF_CList_InsertBack_Ex (simple)]
**
*******************************************************************************/

void Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_CreatePipe_Returns_nonCFE_SUCCESS_Value_SendEventAndReturnThatValue(
    void)
{
    /* Arrange */
    int32       expectedResult = Any_int32_Except(CFE_SUCCESS);
    const char *expected_Spec  = "CF: failed to create pipe %s, returned 0x%08x";

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), expectedResult);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    CF_AppData.engine.enabled = 0;

    /* Act */
    result = CF_CFDP_InitEngine();

    /* Assert */
    UtAssert_INT32_EQ(result, expectedResult);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_SubscribeLocal, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_INIT_PIPE,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_INIT_PIPE)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_INIT_PIPE);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.engine.enabled == 0, "CF_AppData.engine.enabled is %u and should be 0",
                  CF_AppData.engine.enabled);
} /* end Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_CreatePipe_Returns_nonCFE_SUCCESS_Value_SendEventAndReturnThatValue
   */

void Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_SubscribeLocal_Returns_nonCFE_SUCCESS_Value_SendEventAndReturnThatValue(
    void)
{
    /* Arrange */
    int32       expectedResult = Any_int32_Except(CFE_SUCCESS);
    const char *expected_Spec  = "CF: failed to subscribe to MID 0x%04x, returned 0x%08x";

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_SubscribeLocal), expectedResult);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    CF_AppData.engine.enabled = 0;

    /* Act */
    result = CF_CFDP_InitEngine();

    /* Assert */
    UtAssert_INT32_EQ(result, expectedResult);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_SubscribeLocal, 1);
    UtAssert_STUB_COUNT(OS_CountSemGetIdByName, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_INIT_SUB,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_INIT_SUB)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_INIT_SUB);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.engine.enabled == 0, "CF_AppData.engine.enabled is %u and should be 0",
                  CF_AppData.engine.enabled);
} /* end
     Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_SubscribeLocal_Returns_nonCFE_SUCCESS_Value_SendEventAndReturnThatValue
   */

void Test_CF_CFDP_InitEngine_FirstCallTo_OS_CountSemGetIdByNamel_Returns_nonOS_SUCCESS_Value_SendEventAndReturnThatValue(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    int32             expectedResult = Any_int32_Except(OS_SUCCESS);
    const char       *expected_Spec  = "CF: failed to get sem id for name %s, error=0x%08x";

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_SubscribeLocal), CFE_SUCCESS);

    CF_AppData.config_table                      = &dummy_config_table;
    CF_AppData.config_table->chan[0].sem_name[0] = 1; /* 1 forces branch to run */

    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemGetIdByName), expectedResult);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    CF_AppData.engine.enabled = 0;

    /* Act */
    result = CF_CFDP_InitEngine();

    /* Assert */
    UtAssert_INT32_EQ(result, expectedResult);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, 1);
    UtAssert_STUB_COUNT(CFE_SB_SubscribeLocal, 1);
    UtAssert_STUB_COUNT(OS_CountSemGetIdByName, 1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_INIT_SEM,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_INIT_SEM)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_INIT_SEM);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    UtAssert_True(CF_AppData.engine.enabled == 0, "CF_AppData.engine.enabled is %u and should be 0",
                  CF_AppData.engine.enabled);
} /* end
     Test_CF_CFDP_InitEngine_FirstCallTo_OS_CountSemGetIdByNamel_Returns_nonOS_SUCCESS_Value_SendEventAndReturnThatValue
   */

void Test_CF_CFDP_InitEngine_AssertsBecause_chunk_mem_offset_Plus_CF_max_chunks_k_i_IsLessThan_CF_NUM_CHUNKS_ALL_CHANNELS(
    void)
{
    // /* Arrange */
    // cf_config_table_t   dummy_config_table;
    // const char*         expected_Spec = "CF: failed to get sem id for name %s, error=0x%08x";
    // uint8               i = 0;

    // UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), CFE_SUCCESS);

    // UT_SetDefaultReturnValue(UT_KEY(CFE_SB_SubscribeLocal), CFE_SUCCESS);

    // CF_AppData.config_table = &dummy_config_table;
    // CF_AppData.config_table->chan[i].sem_name[0] = 1; /* 1 forces branch to run */

    // for(i=1; i<CF_NUM_CHANNELS; ++i)
    // {
    //   CF_AppData.config_table->chan[i].sem_name[0] = 0; /* 0 means always bypass the branch */
    // }

    // UT_SetDefaultReturnValue(UT_KEY(OS_CountSemGetIdByName), OS_SUCCESS);

    // CF_AppData.engine.enabled = 0;

    // /* Act */
    // result = CF_CFDP_InitEngine();

    // /* Assert */
    // UtAssert_INT32_EQ(result, CFE_SUCCESS);
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    // UtAssert_STUB_COUNT(CFE_SB_CreatePipe, CF_NUM_CHANNELS);
    // UtAssert_STUB_COUNT(CFE_SB_SubscribeLocal, CF_NUM_CHANNELS);
    // UtAssert_STUB_COUNT(OS_CountSemGetIdByName, 1);
    // UtAssert_True(CF_AppData.engine.enabled == 1,
    //   "CF_AppData.engine.enabled is %u and should be 1",
    //   CF_AppData.engine.enabled);
    // /* NOTE: not sure how to make this one fail as none of the items in this Assert are controllable,
    // chunk_mem_offset is local variable only, the other two are constants */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - (chunk_mem_offset+CF_max_chunks[k][i])<=CF_NUM_CHUNKS_ALL_CHANNELS");
} /* end
     Test_CF_CFDP_InitEngine_AssertsBecause_chunk_mem_offset_Plus_CF_max_chunks_k_i_IsLessThan_CF_NUM_CHUNKS_ALL_CHANNELS
   */

void Test_CF_CFDP_InitEngine_SuccessSet_CF_AppData_engine_enabled_To_1_AndReturn_CFE_SUCCESS(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             i = 0;

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_CreatePipe), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_SubscribeLocal), CFE_SUCCESS);

    CF_AppData.config_table                      = &dummy_config_table;
    CF_AppData.config_table->chan[i].sem_name[0] = 1; /* 1 forces branch to run */

    for (i = 1; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.config_table->chan[i].sem_name[0] = 0; /* 0 means always bypass the branch */
    }

    UT_SetDefaultReturnValue(UT_KEY(OS_CountSemGetIdByName), OS_SUCCESS);

    CF_AppData.engine.enabled = 0;

    /* Act */
    result = CF_CFDP_InitEngine();

    /* Assert */
    UtAssert_INT32_EQ(result, CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    UtAssert_STUB_COUNT(CFE_SB_CreatePipe, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(CFE_SB_SubscribeLocal, CF_NUM_CHANNELS);
    UtAssert_STUB_COUNT(OS_CountSemGetIdByName, 1);
    UtAssert_True(CF_AppData.engine.enabled == 1, "CF_AppData.engine.enabled is %u and should be 1",
                  CF_AppData.engine.enabled);
} /* end Test_CF_CFDP_InitEngine_SuccessSet_CF_AppData_engine_enabled_To_1_AndReturn_CFE_SUCCESS */

/* end CF_CFDP_InitEngine tests */

/*******************************************************************************
**
**  CF_CFDP_ReceiveMessage tests (very large) - full coverage - 3 coverage JIRA issues - [unstubbables: CF_CFDP_RecvPh
*(medium) [], CF_CFDP_FindTransactionBySequenceNumber (small) [unstubbables: CF_CFDP_FindTransactionBySequenceNumber_
*(small)], CF_CFDP_DispatchRecv (simple) [unstubbables: CF_CFDP_ArmInactTimer (simple)], CF_CFDP_RecvFin (small) [],
*CF_CFDP_TxFile__ (simple) [], CF_CFDP_SendAck (medium) [unstubbables: CF_CFDP_ConstructPduHeader (TODO:Ahhhh!! stopped
*here on this investigation, this one is gonna be deeeeeeeeep)]]
**
*******************************************************************************/

void Test_CF_CFDP_ReceiveMessage_When_rx_max_messages_per_wakeup_For_chan_num_Is_0_Set_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 0; /* 0 forces for loop to not run */

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 0);
    UtAssert_NULL(CF_AppData.engine.in.msg);
} /* end
     Test_CF_CFDP_ReceiveMessage_When_rx_max_messages_per_wakeup_For_chan_num_Is_0_Set_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTO_CFE_SB_ReceiveBuffer_Returns_CFE_SB_NO_MESSAGE_Set_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SB_NO_MESSAGE);

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 0);
    UtAssert_NULL(CF_AppData.engine.in.msg);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTO_CFE_SB_ReceiveBuffer_Returns_CFE_SB_NO_MESSAGE_Set_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTO_CF_CFDP_RecvPh_Returns_non0_Set_CF_AppData_engine_in_msg_To_NULL(void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    /* Arrange for CF_CFDP_RecvPh */
    int forced_return_CF_HeaderSize = INT32_MIN; /* forces nothing to be greater than */

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 0);
} /* end Test_CF_CFDP_ReceiveMessage_CallTO_CF_CFDP_RecvPh_Returns_non0_Set_CF_AppData_engine_in_msg_To_NULL */

void Test_CF_CFDP_ReceiveMessage_CallTo_CF_CFDP_FindTransactionBySequenceNumber_Returns_nonNULL_Call_CF_CFDP_DispatchRecv_ThenSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer;

    /* Arrange for CF_CFDP_RecvPh */
    int forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize +
        Any_uint16(); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_Transaction_t                              dummy_t;
    CF_Transaction_t                             *forced_t_result = &dummy_t;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS - 1; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    forced_t_result->state                                     = CF_TxnState_R1;
    context_CF_CList_Traverse[NUM_CLISTS - 1].context_forced_t = forced_t_result;

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_CF_CFDP_FindTransactionBySequenceNumber_Returns_nonNULL_Call_CF_CFDP_DispatchRecv_ThenSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_state_IsGreaterThan_CFDP_IDLE(void)
{
    // /* Arrange */
    // uint8               dummy_chan_num = Any_cf_chan_num();
    // cf_config_table_t   dummy_config_table;
    // CF_Channel_t*          arg_c;
    // CF_PduRecvMsg_t         dummy_msg;

    // arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    // CF_AppData.config_table = &dummy_config_table;
    // CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    // UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), Any_CFE_Status_t_Except(CFE_SB_NO_MESSAGE));

    // CF_AppData.engine.in.msg = (CFE_SB_Buffer_t*)&dummy_msg;

    // /* Arrange for CF_CFDP_RecvPh */
    // int               forced_return_CF_HeaderSize = Any_int_Positive();/* Any_int_Positive() used because this is
    // what the CUT sees, in practice this will be relatively small I, ASG, would think */ size_t
    // forced_return_CFE_SB_GetUserDataLength = forced_return_CF_HeaderSize + Any_uint16();/* Any_CFE_MSG_Size_t not
    // used because only need a small number here */ int               forced_return_CF_GetVariableHeader = 0;

    // /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    // /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = (CFE_SB_Buffer_t*)&dummy_msg; */

    // UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    // UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
    // &forced_return_CFE_SB_GetUserDataLength);

    // UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
    // &forced_return_CF_GetVariableHeader);

    // dummy_msg.ph.length = forced_return_CFE_SB_GetUserDataLength - forced_return_CF_HeaderSize; /* ensures equality
    // so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    // /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    // uint8                   i = 0;
    // CF_Transaction_t           dummy_t;
    // CF_Transaction_t*          forced_t_result = &dummy_t;
    // CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t  context_CF_CList_Traverse[NUM_CLISTS];

    // type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    // UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse,
    //   sizeof(context_CF_CList_Traverse), false);

    // for(i = 0; i < NUM_CLISTS - 1; ++i)
    // {
    //   context_CF_CList_Traverse[i].context_forced_t = NULL;
    // }

    // forced_t_result->state = CF_TxnState_R1;
    // context_CF_CList_Traverse[NUM_CLISTS - 1].context_forced_t = forced_t_result;

    // /* Act */
    // CF_CFDP_ReceiveMessage(arg_c);

    // /* Assert */
    // UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    // UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    // UtAssert_NULL(CF_AppData.engine.in.msg);
    // /* Assert for CF_CFDP_RecvPh */
    // UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    // UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    // /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    // UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    // /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    // UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state>CF_TxnState_IDLE");
} /* end Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_state_IsGreaterThan_CFDP_IDLE */

void Test_CF_CFDP_ReceiveMessage_CallTo_src_And_dst_AreNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                       dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t           dummy_config_table;
    CF_Channel_t               *arg_c;
    CF_UT_inmsg_buffer_t        dummy_msg;
    const char                 *expected_Spec = "CF: dropping packet for invalid destination eid 0x%x";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8(); /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for src check fail */

    CF_AppData.engine.in.dst = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Arrange for CF_CFDP_RecvPh */
    int forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize +
        Any_uint16(); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 0);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_INVALID_DST_EID,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_INVALID_DST_EID)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_INVALID_DST_EID);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_src_And_dst_AreNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_FGV_Returns_non0_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                       dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t           dummy_config_table;
    CF_Channel_t               *arg_c;
    CF_UT_inmsg_buffer_t        dummy_msg;
    const char                 *expected_Spec = "CF: dropping packet for invalid destination eid 0x%x";
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8();                        /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = CF_AppData.config_table->local_eid; /* for src check fail */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 1);

    CF_AppData.engine.in.dst = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Arrange for CF_CFDP_RecvPh */
    int forced_return_CF_HeaderSize =
        Any_int_Positive(); /* Any_int_Positive() used because this is what the CUT sees, in practice this will be
                               relatively small I, ASG, would think */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize +
        Any_uint16(); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_INVALID_DST_EID,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_INVALID_DST_EID)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_INVALID_DST_EID);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_FGV_Returns_non0_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTo_Then_bytes_received_IsLessThanExpected_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;
    const char          *expected_Spec = "CF: dropping packet for invalid destination eid 0x%x";

    /* 8 arbitrary for a reasonable size but UINT16_MAX + 1 is to force a false (in CUT) */
    int forced_return_CF_HeaderSize[2] = {8, UINT16_MAX + 1};

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8();                        /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = CF_AppData.config_table->local_eid; /* for src check fail */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnArray, forced_return_CF_HeaderSize);

    CF_AppData.engine.in.dst = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    /* Arrange for CF_CFDP_RecvPh */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize[0] +
        Any_uint8_Except(0); /* Any_CFE_MSG_Size_t not used because a small number is necessary here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize[0]; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_STUB_COUNT(CF_HeaderSize, 2);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_INVALID_DST_EID,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_INVALID_DST_EID)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_INVALID_DST_EID);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_Then_bytes_received_IsLessThanExpected_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTo_Then_directive_code_IsNotEqTo_PDU_FIN_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;
    const char          *expected_Spec = "CF: dropping packet for invalid destination eid 0x%x";

    /* 8 arbitrary for a reasonable size (in CF_CFDP_RecvPh call), 8 to
       force a true (in CUT), offsetof() for nominal value */
    int forced_return_CF_HeaderSize[3] = {8, 8, sizeof(CF_CFDP_PduHeader_t)};

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    memset(&dummy_config_table, 0, sizeof(dummy_config_table));

    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8();                        /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = CF_AppData.config_table->local_eid; /* for src check fail */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnArray, forced_return_CF_HeaderSize);

    CF_AppData.engine.in.dst = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = Any_uint8_Except(CF_CFDP_FileDirective_FIN);

    /* Arrange for CF_CFDP_RecvPh */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize[0] +
        Any_uint16_Except(0); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize[0]; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_STUB_COUNT(CF_HeaderSize, 3);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_INVALID_DST_EID,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_INVALID_DST_EID)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_INVALID_DST_EID);
    UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
                  "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
                  context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
                    "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
                    context_CFE_EVS_SendEvent.Spec, expected_Spec);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_Then_directive_code_IsNotEqTo_PDU_FIN_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_neg1_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;

    /* 8 arbitrary for a reasonable size (in CF_CFDP_RecvPh call), 8 to force a true (in CUT),
       offsetof() for nominal value, UINT16_MAX + 1 is to force a false (in CUT) */
    int forced_return_CF_HeaderSize[4] = {8, 8, sizeof(CF_CFDP_PduHeader_t), UINT16_MAX + 1};

    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8();                        /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = CF_AppData.config_table->local_eid; /* for src check fail */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnArray, forced_return_CF_HeaderSize);

    CF_AppData.engine.in.dst = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = CF_CFDP_FileDirective_FIN;

    /* Arrange for CF_CFDP_RecvPh */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize[0] +
        Any_uint8_GreaterThan(0); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize[0]; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 1);
    UtAssert_STUB_COUNT(CF_HeaderSize, 4);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    /* Assert for CF_CFDP_RecvFin */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_PDU_FIN_SHORT,
                  "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_PDU_FIN_SHORT)",
                  context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_PDU_FIN_SHORT);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_neg1_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_Increment_counters_recv_spurious_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;

    /* 8 arbitrary for a reasonable size (in CF_CFDP_RecvPh call), 8 to force a true (in CUT),
       offsetof() for nominal value, 0 is to force a true (in CUT), 0 is arbitrary just to have a value */
    int forced_return_CF_HeaderSize[5] = {8, 8, sizeof(CF_CFDP_PduHeader_t), 0, 0};

    uint16                      initial_counters_recv_serial = Any_uint16();
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8();                        /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = CF_AppData.config_table->local_eid; /* for src check fail */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnArray, forced_return_CF_HeaderSize);

    CF_AppData.engine.in.dst = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = CF_CFDP_FileDirective_FIN;

    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_counters_recv_serial;

    /* Arrange for CF_CFDP_RecvPh */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize[0] +
        Any_uint8_GreaterThan(0); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize[0]; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.config_table->chan[dummy_chan_num].max_outgoing_messages_per_wakeup = 1;
    CF_AppData.engine.outgoing_counter                                             = 1; /* Force fail */
    UT_SetHandlerFunction(UT_KEY(CFE_SB_AllocateMessageBuffer),
                          handler_CFE_SB_AllocateMessageBuffer_ReturnForced_CFE_SB_Buffer, &dummy_msg);

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 2);
    UtAssert_STUB_COUNT(CF_HeaderSize, 5);
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious ==
                      (uint16)(initial_counters_recv_serial + 1),
                  "CF_AppData.hk.channel_hk[chan_num].counters.recv.spurious is %u and should be 1 more than %u (value "
                  "before call)",
                  CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious, initial_counters_recv_serial);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_Increment_counters_recv_spurious_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_CallTo_CF_CFDP_SendAck_Retuns_CFE_SEND_SUCCESS_Set_c_cur_To_NULL_Increment_counters_recv_spurious_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL2(
    void)
{
    /* Arrange */
    uint8                dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t    dummy_config_table;
    CF_Channel_t        *arg_c;
    CF_UT_inmsg_buffer_t dummy_msg;

    /* 0th 8 arbitrary for a reasonable size (in CF_CFDP_RecvPh call), 8 to force a true (in CUT), 0 to
       offsetof() for nominal value, 0 is to force a true (in CUT), remaining 0s is arbitrary just to have a value */
    int forced_return_CF_HeaderSize[8] = {8, 8, sizeof(CF_CFDP_PduHeader_t), 0, 0, 0, 0, 0};

    uint16                      initial_counters_recv_serial = Any_uint16();
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;

    memset(&dummy_msg, 0, sizeof(dummy_msg));
    memset(&dummy_msg, 0, sizeof(dummy_msg));
    arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8();                        /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = CF_AppData.config_table->local_eid; /* for src check fail */
    UT_SetDefaultReturnValue(UT_KEY(FGV), 0);
    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnArray, forced_return_CF_HeaderSize);

    CF_AppData.engine.in.dst = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    dummy_msg.content.cfdp.secondary.fdirh.directive_code = CF_CFDP_FileDirective_FIN;

    CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious = initial_counters_recv_serial;

    /* Arrange for CF_CFDP_RecvPh */
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize[0] + 2; /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize[0]; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_Transaction_t                              dummy_t;
    CF_UT_outmsg_buffer_t                         forced_set_CFE_MSG_Init;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    memset(&forced_set_CFE_MSG_Init, 0, sizeof(forced_set_CFE_MSG_Init));
    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Arrange for CF_CFDP_MsgOutGet */
    CF_AppData.config_table->chan[dummy_chan_num].max_outgoing_messages_per_wakeup = 0;
    CF_AppData.engine.outgoing_counter                                             = 1; /* Force fail */

    CF_AppData.engine.out.msg = &forced_set_CFE_MSG_Init.cfe_sb_buffer;

    arg_c->cur = &dummy_t;

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 2);
    UtAssert_STUB_COUNT(CF_HeaderSize, 8);
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious ==
                      (uint16)(initial_counters_recv_serial + 1),
                  "CF_AppData.hk.channel_hk[chan_num].counters.recv.spurious is %u and should be 1 more than %u (value "
                  "before call)",
                  CF_AppData.hk.channel_hk[dummy_chan_num].counters.recv.spurious, initial_counters_recv_serial);
    UtAssert_NULL(arg_c->cur);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    /* Assert for CF_CFDP_MsgOutGet */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_CallTo_CF_CFDP_SendAck_Retuns_CFE_SEND_SUCCESS_Set_c_cur_To_NULL_Increment_counters_recv_spurious_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL2
   */

void Test_CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    // /* Arrange */
    // uint8               dummy_chan_num = Any_cf_chan_num();
    // cf_config_table_t   dummy_config_table;
    // CF_Channel_t*          arg_c;
    // CF_PduRecvMsg_t         dummy_msg;
    // const char*         expected_Spec = "CF: dropping packet from %d transaction number 0x%08x due max RX
    // transactions reached"; CFE_EVS_SendEvent_context_t   context_CFE_EVS_SendEvent;

    // arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    // CF_AppData.config_table = &dummy_config_table;
    // CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    // UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), Any_CFE_Status_t_Except(CFE_SB_NO_MESSAGE));

    // CF_AppData.engine.in.msg = (CFE_SB_Buffer_t*)&dummy_msg;
    // CF_AppData.config_table->local_eid = Any_uint8(); /*TODO: change to any_CF_EntityId_t */
    // CF_AppData.engine.in.src = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for src check fail */

    // CF_AppData.engine.in.dst = CF_AppData.config_table->local_eid; /* for dst check fail */

    // UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    // CF_AppData.hk.channel_hk[dummy_chan_num].q_size[CF_QueueIdx_RX] = CF_MAX_SIMULTANEOUS_RX;

    // /* Arrange for CF_CFDP_RecvPh */
    // int               forced_return_CF_HeaderSize = Any_int_Positive();/* Any_int_Positive() used because this is
    // what the CUT sees, in practice this will be relatively small I, ASG, would think */ size_t
    // forced_return_CFE_SB_GetUserDataLength = forced_return_CF_HeaderSize + 2;/* Any_CFE_MSG_Size_t not used because
    // only need a small number here */ int               forced_return_CF_GetVariableHeader = 0;

    // /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    // /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = (CFE_SB_Buffer_t*)&dummy_msg; */

    // UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    // UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
    // &forced_return_CFE_SB_GetUserDataLength);

    // UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
    // &forced_return_CF_GetVariableHeader);

    // dummy_msg.ph.length = forced_return_CFE_SB_GetUserDataLength - forced_return_CF_HeaderSize; /* ensures equality
    // so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    // /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    // uint8                   i = 0;
    // CF_Transaction_t           dummy_t;
    // CF_Transaction_t*          forced_t_result = &dummy_t;
    // CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t  context_CF_CList_Traverse[NUM_CLISTS];

    // type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    // UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse,
    //   sizeof(context_CF_CList_Traverse), false);

    // for(i = 0; i < NUM_CLISTS; ++i)
    // {
    //   context_CF_CList_Traverse[i].context_forced_t = NULL;
    // }

    // /* Act */
    // CF_CFDP_ReceiveMessage(arg_c);

    // /* Assert */
    // UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    // UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    // UtAssert_NULL(CF_AppData.engine.in.msg);
    // UtAssert_STUB_COUNT(FGV, 0);
    // UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_RX_DROPPED,
    //   "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_RX_DROPPED)",
    //   context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_RX_DROPPED);
    // UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
    //   "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
    //   context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    // UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
    //   "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
    //   context_CFE_EVS_SendEvent.Spec, expected_Spec);
    // /* Assert for CF_CFDP_RecvPh */
    // UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    // UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    // /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    // UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    // /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    // UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_MIR("JIRA: GSFCFCS-1734, t is always going to be NULL thus t->chan_num will always segfault");
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_AppData_engine_in_msg_To_NULL
   */

void Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_Is_NULL(void)
{
    // /* Arrange */
    // uint8               dummy_chan_num = Any_cf_chan_num();
    // cf_config_table_t   dummy_config_table;
    // CF_Channel_t*          arg_c;
    // CF_PduRecvMsg_t         dummy_msg;
    // const char*         expected_Spec = "CF: dropping packet from %d transaction number 0x%08x due max RX
    // transactions reached"; CFE_EVS_SendEvent_context_t   context_CFE_EVS_SendEvent;

    // arg_c = &CF_AppData.engine.channels[dummy_chan_num];

    // CF_AppData.config_table = &dummy_config_table;
    // CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    // UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), Any_CFE_Status_t_Except(CFE_SB_NO_MESSAGE));

    // CF_AppData.engine.in.msg = (CFE_SB_Buffer_t*)&dummy_msg;
    // CF_AppData.config_table->local_eid = Any_uint8(); /*TODO: change to any_CF_EntityId_t */
    // CF_AppData.engine.in.src = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for src check fail */

    // CF_AppData.engine.in.dst = CF_AppData.config_table->local_eid; /* for dst check fail */

    // UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    // CF_AppData.hk.channel_hk[dummy_chan_num].q_size[CF_QueueIdx_RX] = CF_MAX_SIMULTANEOUS_RX;

    // /* Arrange for CF_CFDP_RecvPh */
    // int               forced_return_CF_HeaderSize = Any_int_Positive();/* Any_int_Positive() used because this is
    // what the CUT sees, in practice this will be relatively small I, ASG, would think */ size_t
    // forced_return_CFE_SB_GetUserDataLength = forced_return_CF_HeaderSize + 2;/* Any_CFE_MSG_Size_t not used because
    // only need a small number here */ int               forced_return_CF_GetVariableHeader = 0;

    // /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    // /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = (CFE_SB_Buffer_t*)&dummy_msg; */

    // UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    // UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
    // &forced_return_CFE_SB_GetUserDataLength);

    // UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
    // &forced_return_CF_GetVariableHeader);

    // dummy_msg.ph.length = forced_return_CFE_SB_GetUserDataLength - forced_return_CF_HeaderSize; /* ensures equality
    // so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    // /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    // uint8                   i = 0;
    // CF_Transaction_t           dummy_t;
    // CF_Transaction_t*          forced_t_result = &dummy_t;
    // CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t  context_CF_CList_Traverse[NUM_CLISTS];

    // type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    // UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse,
    //   sizeof(context_CF_CList_Traverse), false);

    // for(i = 0; i < NUM_CLISTS; ++i)
    // {
    //   context_CF_CList_Traverse[i].context_forced_t = NULL;
    // }

    // /* Act */
    // CF_CFDP_ReceiveMessage(arg_c);

    // /* Assert */
    // UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    // UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    // UtAssert_NULL(CF_AppData.engine.in.msg);
    // UtAssert_STUB_COUNT(FGV, 0);
    // UtAssert_True(context_CFE_EVS_SendEvent.EventID == CF_EID_ERR_CFDP_RX_DROPPED,
    //   "CFE_EVS_SendEvent received EventID %u and should have received %u (CF_EID_ERR_CFDP_RX_DROPPED)",
    //   context_CFE_EVS_SendEvent.EventID, CF_EID_ERR_CFDP_RX_DROPPED);
    // UtAssert_True(context_CFE_EVS_SendEvent.EventType == CFE_EVS_EventType_ERROR,
    //   "CFE_EVS_SendEvent received EventType %u and should have received %u (CFE_EVS_EventType_ERROR)",
    //   context_CFE_EVS_SendEvent.EventType, CFE_EVS_EventType_ERROR);
    // UtAssert_StrCmp(context_CFE_EVS_SendEvent.Spec, expected_Spec,
    //   "CFE_EVS_SendEvent received expected Spec\n'%s' - Received\n'%s' - Expected",
    //   context_CFE_EVS_SendEvent.Spec, expected_Spec);
    // /* Assert for CF_CFDP_RecvPh */
    // UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    // UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    // /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    // UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    // /* Assert for CF_CFDP_DispatchRecv via CF_CFDP_ArmInactTimer */
    // UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t");
} /* end Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_Is_NULL */

void Test_2CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_AppData_engine_in_msg_To_NULL(
    void)
{
    /* Arrange */
    uint8                       dummy_chan_num = Any_cf_chan_num();
    cf_config_table_t           dummy_config_table;
    CF_Channel_t               *arg_c;
    CF_UT_inmsg_buffer_t        dummy_msg;
    CFE_EVS_SendEvent_context_t context_CFE_EVS_SendEvent;
    CF_History_t                dummy_history;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    arg_c                       = &CF_AppData.engine.channels[dummy_chan_num];
    arg_c->qs[CF_QueueIdx_HIST] = &dummy_history.cl_node;

    CF_AppData.config_table                                                  = &dummy_config_table;
    CF_AppData.config_table->chan[dummy_chan_num].rx_max_messages_per_wakeup = 1; /* 1 forces for loop to run */

    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SUCCESS);

    CF_AppData.engine.in.msg           = &dummy_msg.cfe_sb_buffer;
    CF_AppData.config_table->local_eid = Any_uint8(); /*TODO: change to any_CF_EntityId_t */
    CF_AppData.engine.in.src           = Any_uint8_Except(CF_AppData.config_table->local_eid); /* for src check fail */

    CF_AppData.engine.in.dst = CF_AppData.config_table->local_eid; /* for dst check fail */

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), stub_reporter_hook, &context_CFE_EVS_SendEvent);

    CF_AppData.hk.channel_hk[dummy_chan_num].q_size[CF_QueueIdx_RX] = Any_uint16_Except(CF_MAX_SIMULTANEOUS_RX);

    /* Arrange for CF_CFDP_RecvPh */
    int    forced_return_CF_HeaderSize = 0;
    size_t forced_return_CFE_SB_GetUserDataLength =
        forced_return_CF_HeaderSize +
        Any_uint16(); /* Any_CFE_MSG_Size_t not used because only need a small number here */
    int forced_return_CF_GetVariableHeader = 0;

    /* CF_Assert(chan_num<CF_NUM_CHANNELS) covered by arg_c = &CF_AppData.engine.channels[dummy_chan_num] */
    /* CF_Assert(CF_AppData.engine.in.msg) covered by CF_AppData.engine.in.msg = &dummy_msg.cfe_sb_buffer; */

    UT_SetHandlerFunction(UT_KEY(CF_HeaderSize), Handler_int_ForcedReturnOnly, &forced_return_CF_HeaderSize);

    UT_SetHandlerFunction(UT_KEY(CFE_SB_GetUserDataLength), Handler_size_t_ForcedReturnOnly,
                          &forced_return_CFE_SB_GetUserDataLength);

    UT_SetHandlerFunction(UT_KEY(CF_GetVariableHeader), Handler_int_ForcedReturnOnly,
                          &forced_return_CF_GetVariableHeader);

    dummy_msg.pdu_r_msg.ph.length =
        forced_return_CFE_SB_GetUserDataLength -
        forced_return_CF_HeaderSize; /* ensures equality so (temp+hsize)==CF_AppData.engine.in.bytes_received */

    /* Arrange for CF_CFDP_FindTransactionBySequenceNumber */
    uint8                                         i = 0;
    CF_Transaction_t                              dummy_t;
    CF_CList_Traverse_FIND_T_BY_SEQ_NUM_context_t context_CF_CList_Traverse[NUM_CLISTS];

    type_of_context_CF_CList_Traverse = FIND_T_BY_SEQ_NUM;

    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), &context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    for (i = 0; i < NUM_CLISTS; ++i)
    {
        context_CF_CList_Traverse[i].context_forced_t = NULL;
    }

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_CListNode_t dummy_node;

    (&CF_AppData.engine.channels[dummy_t.chan_num])->cs[CF_Direction_RX] = &dummy_node;

    UT_SetDefaultReturnValue(UT_KEY(OS_ObjectIdDefined), true);

    /* Arrange for CF_CFDP_FindUnusedTransaction */
    CF_CListNode_t dummy_cf_q_hist;

    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_FREE] = 1;
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] = 1;

    arg_c->qs[CF_QueueIdx_FREE] = &dummy_t.cl_node;
    arg_c->qs[CF_QueueIdx_HIST] = &dummy_cf_q_hist;

    dummy_t.state = CF_TxnState_R1;

    /* Act */
    CF_CFDP_ReceiveMessage(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_NULL(CF_AppData.engine.in.msg);
    UtAssert_STUB_COUNT(FGV, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF_CFDP_RecvPh */
    UtAssert_STUB_COUNT(CF_HeaderSize, 1);
    UtAssert_STUB_COUNT(CF_GetVariableHeader, 1);
    /* Assert for CF_CFDP_FindTransactionBySequenceNumber */
    UtAssert_STUB_COUNT(CF_CList_Traverse, NUM_CLISTS);
    /* Assert for CF_CFDP_FindUnusedTransaction */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
} /* end
     Test_CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_AppData_engine_in_msg_To_NULL
   */

/* end CF_CFDP_ReceiveMessage tests */

/*******************************************************************************
**
**  CF_CFDP_CycleTx_ tests (medium) - full coverage - 1 coverage JIRA issue - [unstubbables: CF_CFDP_DispatchTx (small)]
**
*******************************************************************************/

void Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_suspended_Return_0(void)
{
    /* Arrange */
    CF_Transaction_t       dummy_t;
    CF_CListNode_t        *arg_clist_node = &dummy_t.cl_node;
    CF_CFDP_CycleTx_args_t dummy_args;
    void                  *arg_context = (void *)&dummy_args;
    int                    local_result;

    dummy_t.flags.com.suspended = true;

    /* Act */
    local_result = CF_CFDP_CycleTx_(arg_clist_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == 0, "CF_CFDP_CycleTx_ returned %d and should be 0", local_result);
} /* end Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_suspended_Return_0 */

void Test_CF_CFDP_CycleTx__AssertsBecauseGiven_node_TransactionContainer_t_flags_all_q_index_IsNotEqTo_CF_Q_TXA(void)
{
    // /* Arrange */
    // CF_Transaction_t           dummy_t;
    // CF_CListNode_t *              arg_clist_node = &dummy_t.cl_node;
    // CF_CFDP_CycleTx_args_t  dummy_args;
    // void*                   arg_context = (void*)&dummy_args;
    // int                     local_result;

    // dummy_t.flags.com.suspended = false;

    // dummy_t.flags.com.q_index = Any_uint8_Except(CF_QueueIdx_TXA);

    // /* Act */
    // local_result = CF_CFDP_CycleTx_(arg_clist_node, arg_context);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->flags.com.q_index==CF_QueueIdx_TXA");
} /* end Test_CF_CFDP_CycleTx__AssertsBecauseGiven_node_TransactionContainer_t_flags_all_q_index_IsNotEqTo_CF_Q_TXA */

void Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_notNULL_Sets_args_ran_one_To_1_ThenReturn_1(
    void)
{
    /* Arrange */
    CF_Channel_t           dummy_c;
    CF_Transaction_t       dummy_cur;
    CF_Transaction_t       dummy_t;
    CF_CListNode_t        *arg_clist_node = &dummy_t.cl_node;
    CF_CFDP_CycleTx_args_t dummy_args;
    void                  *arg_context = (void *)&dummy_args;
    int                    local_result;

    dummy_t.flags.com.suspended = false;

    dummy_t.flags.com.q_index = CF_QueueIdx_TXA;

    dummy_c.cur  = &dummy_cur;
    dummy_args.c = &dummy_c;

    dummy_t.chan_num = Any_cf_chan_num();

    /* Act */
    local_result = CF_CFDP_CycleTx_(arg_clist_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_CycleTx_ returned %d and should be 1", local_result);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 0);
    UtAssert_True(dummy_args.ran_one == 1, "CF_CFDP_CycleTx_ set args->ran_one to %d and should be 1",
                  dummy_args.ran_one);
} /* end
     Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_notNULL_Sets_args_ran_one_To_1_ThenReturn_1
   */

void Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_OnFirstCallTo_CF_CFDP_DispatchTx_ThenReturn_1(
    void)
{
    /* Arrange */
    CF_Channel_t           dummy_c;
    CF_Transaction_t       dummy_t;
    CF_CListNode_t        *arg_clist_node = &dummy_t.cl_node;
    CF_CFDP_CycleTx_args_t dummy_args;
    void                  *arg_context = (void *)&dummy_args;
    int                    local_result;

    dummy_t.flags.com.suspended = false;

    dummy_t.flags.com.q_index = CF_QueueIdx_TXA;

    dummy_c.cur  = NULL;
    dummy_args.c = &dummy_c;

    dummy_t.chan_num = Any_cf_chan_num();

    /* Arrange unstubbable: CF_CFDP_DispatchTx */
    dummy_t.state = CF_TxnState_S1; /* results in CF_CFDP_S1_Tx function */

    callNumberToSet_notNULL_for_CF_CFDP_S1_Tx_args_c_cur = 1;
    UT_SetHandlerFunction(UT_KEY(CF_CFDP_S1_Tx), Handler_CF_CFDP_S1_Tx_notNull_cur_AtSetCallNumber, &dummy_args);

    /* Act */
    local_result = CF_CFDP_CycleTx_(arg_clist_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_CycleTx_ returned %d and should be 1", local_result);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2);
    UtAssert_True(dummy_args.ran_one == 1, "CF_CFDP_CycleTx_ set args->ran_one to %d and should be 1",
                  dummy_args.ran_one);
    /* Assert for CF_CFDP_DispatchTx */
    UtAssert_STUB_COUNT(CF_CFDP_S1_Tx, 1);
} /* end
     Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_OnFirstCallTo_CF_CFDP_DispatchTx_ThenReturn_1
   */

void Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_AtSomePointThenReturn_1(
    void)
{
    /* Arrange */
    CF_Channel_t           dummy_c;
    CF_Transaction_t       dummy_t;
    CF_CListNode_t        *arg_clist_node = &dummy_t.cl_node;
    CF_CFDP_CycleTx_args_t dummy_args;
    void                  *arg_context = (void *)&dummy_args;
    int                    local_result;

    dummy_t.flags.com.suspended = false;

    dummy_t.flags.com.q_index = CF_QueueIdx_TXA;

    dummy_c.cur  = NULL;
    dummy_args.c = &dummy_c;

    dummy_t.chan_num = Any_cf_chan_num();

    /* Arrange unstubbable: CF_CFDP_DispatchTx */
    dummy_t.state = CF_TxnState_S1; /* results in CF_CFDP_S1_Tx function */

    callNumberToSet_notNULL_for_CF_CFDP_S1_Tx_args_c_cur =
        Any_uint8_LessThan(20) +
        1; /* Any_uint8_LessThan(20) + 1 is a positive number used for reasonable time frame test */
    UT_SetHandlerFunction(UT_KEY(CF_CFDP_S1_Tx), Handler_CF_CFDP_S1_Tx_notNull_cur_AtSetCallNumber, &dummy_args);

    /* Act */
    local_result = CF_CFDP_CycleTx_(arg_clist_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_CycleTx_ returned %d and should be 1", local_result);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 2 * callNumberToSet_notNULL_for_CF_CFDP_S1_Tx_args_c_cur);
    UtAssert_True(dummy_args.ran_one == 1, "CF_CFDP_CycleTx_ set args->ran_one to %d and should be 1",
                  dummy_args.ran_one);
    /* Assert for CF_CFDP_DispatchTx */
    UtAssert_STUB_COUNT(CF_CFDP_S1_Tx, callNumberToSet_notNULL_for_CF_CFDP_S1_Tx_args_c_cur);
} /* end
     Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_AtSomePointThenReturn_1
   */

void Test_CF_CFDP_CycleTx_Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_Then_t_flags_all_q_index_BecomesNotEqTo_CF_Q_TXA__OnFirstCallTo_CF_CFDP_DispatchTx_And_args_c_cur_Becomes_notNULL_StillRunsAndStopsWhen_t_flags_all_q_index_Becomes_CF_Q_TXA_AgainAnd_args_c_cur_Becomes_notNULL_ThenReturn_1(
    void)
{
    /* Arrange */
    CF_Channel_t           dummy_c;
    CF_Transaction_t       dummy_t;
    CF_CListNode_t        *arg_clist_node = &dummy_t.cl_node;
    CF_CFDP_CycleTx_args_t dummy_args;
    void                  *arg_context = (void *)&dummy_args;
    int                    local_result;

    dummy_t.flags.com.suspended = false;

    dummy_t.flags.com.q_index = CF_QueueIdx_TXA;

    dummy_c.cur  = NULL;
    dummy_args.c = &dummy_c;

    dummy_t.chan_num = Any_cf_chan_num();

    /* Arrange unstubbable: CF_CFDP_DispatchTx */
    void *cur_and_q_index[2] = {(void *)&dummy_args, (void *)&dummy_t};

    dummy_t.state = CF_TxnState_S1; /* results in CF_CFDP_S1_Tx function */

    UT_SetHandlerFunction(UT_KEY(CF_CFDP_S1_Tx), Handler_CF_CFDP_S1_Tx_notCF_Q_TXA_q_index, cur_and_q_index);

    /* Act */
    local_result = CF_CFDP_CycleTx_(arg_clist_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == 1, "CF_CFDP_CycleTx_ returned %d and should be 1", local_result);
    UtAssert_STUB_COUNT(CFE_ES_PerfLogAdd, 4);
    UtAssert_True(dummy_args.ran_one == 1, "CF_CFDP_CycleTx_ set args->ran_one to %d and should be 1",
                  dummy_args.ran_one);
    /* Assert for CF_CFDP_DispatchTx */
    UtAssert_STUB_COUNT(CF_CFDP_S1_Tx, 2);
} /* end
     Test_CF_CFDP_CycleTx_Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_Then_t_flags_all_q_index_BecomesNotEqTo_CF_Q_TXA__OnFirstCallTo_CF_CFDP_DispatchTx_And_args_c_cur_Becomes_notNULL_StillRunsAndStopsWhen_t_flags_all_q_index_Becomes_CF_Q_TXA_AgainAnd_args_c_cur_Becomes_notNULL_ThenReturn_1
   */

/* end CF_CFDP_CycleTx_ tests */

/*******************************************************************************
**
**  CF_CFDP_CycleTx tests (small) - full coverage - [unstubbables: cf_move_transaction (small) [], CF_CFDP_CycleTx_
*tests (medium) - full coverage - [unstubbables: CF_CFDP_DispatchTx (small)]].
**
*******************************************************************************/

void Test_CF_CFDP_CycleTx_When_dequeue_enabled_IsTrueIn_config_table_FromGiven_c_DoNothing(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_Channel_t     *arg_c;

    CF_AppData.config_table = &dummy_config_table;
    arg_c                   = &CF_AppData.engine.channels[Any_cf_chan_num()];
    CF_AppData.config_table->chan[(arg_c - CF_AppData.engine.channels)].dequeue_enabled = false;

    /* Act */
    CF_CFDP_CycleTx(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
} /* end Test_CF_CFDP_CycleTx_When_dequeue_enabled_IsTrueIn_config_table_FromGiven_c_DoNothing */

void Test_CF_CFDP_CycleTx_When_cur_FromGiven_c_Is_notNULL_Set_c_cur_To_NULL(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_cur;
    cf_config_table_t dummy_config_table;
    CF_Channel_t     *arg_c;

    CF_AppData.config_table = &dummy_config_table;
    arg_c                   = &CF_AppData.engine.channels[Any_cf_chan_num()];
    CF_AppData.config_table->chan[(arg_c - CF_AppData.engine.channels)].dequeue_enabled = true;

    arg_c->cur = &dummy_cur;

    /* Act */
    CF_CFDP_CycleTx(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 0);
    UtAssert_NULL(arg_c->cur);
} /* end Test_CF_CFDP_CycleTx_When_cur_FromGiven_c_Is_notNULL_Set_c_cur_To_NULL */

void Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_CF_CList_Traverse_CalledOnlyOnce_Set_c_cur_To_NULL(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_Channel_t     *arg_c;

    CF_AppData.config_table = &dummy_config_table;
    arg_c                   = &CF_AppData.engine.channels[Any_cf_chan_num()];
    CF_AppData.config_table->chan[(arg_c - CF_AppData.engine.channels)].dequeue_enabled = true;

    arg_c->cur = NULL;

    arg_c->qs[CF_QueueIdx_PEND]       = 0;
    type_of_context_CF_CList_Traverse = POINTER;

    /* Act */
    CF_CFDP_CycleTx(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_NULL(arg_c->cur);
} /* end Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_CF_CList_Traverse_CalledOnlyOnce_Set_c_cur_To_NULL */

void Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_Then_CF_CList_Traverse_CalledOnlyOnce_ReturnsAnd_ran_one_IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_Channel_t     *arg_c;

    CF_AppData.config_table = &dummy_config_table;
    arg_c                   = &CF_AppData.engine.channels[Any_cf_chan_num()];
    CF_AppData.config_table->chan[(arg_c - CF_AppData.engine.channels)].dequeue_enabled = true;

    arg_c->cur = NULL;

    arg_c->qs[CF_QueueIdx_PEND]       = 0;
    type_of_context_CF_CList_Traverse = POINTER;

    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), Handler_CF_CList_Traverse_Set_ran_one_To_1, arg_c);

    /* Act */
    CF_CFDP_CycleTx(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    UtAssert_NULL(arg_c->cur);
} /* end
     Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_Then_CF_CList_Traverse_CalledOnlyOnce_ReturnsAnd_ran_one_IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL
   */

void Test_CF_CFDP_CycleTx_EnterWhileLoopOnceAndCall_cf_move_transaction_SecondCallTo_CF_CList_Traverse_ReturnsAnd_ran_one_IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL(
    void)
{
    /* Arrange */
    CF_Transaction_t  dummy_cur;
    cf_config_table_t dummy_config_table;
    CF_Channel_t     *arg_c;
    void             *context_CF_CList_Traverse[2];

    CF_AppData.config_table = &dummy_config_table;
    arg_c                   = &CF_AppData.engine.channels[Any_cf_chan_num()];
    CF_AppData.config_table->chan[(arg_c - CF_AppData.engine.channels)].dequeue_enabled = true;

    arg_c->cur = NULL;

    arg_c->qs[CF_QueueIdx_PEND] = &dummy_cur.cl_node;

    context_CF_CList_Traverse[0] = (void *)&arg_c->qs[CF_QueueIdx_PEND];
    context_CF_CList_Traverse[1] = (void *)arg_c;
    // TRAV_ARG_T,
    // TRAVERSE_ALL_ARGS_T,
    // POINTER,
    // FIND_T_BY_SEQ_NUM,
    // CLOSE_FILES,
    /* Arrange for cf_move_transaction */
    type_of_context_CF_CList_Traverse = POINTER;

    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), Handler_CF_CList_Traverse_SecondCallSet_ran_one_To_1,
                          &context_CF_CList_Traverse);

    /* Arrange for cf_move_transaction */
    dummy_cur.chan_num = Any_cf_chan_num();

    CF_AppData.hk.channel_hk[dummy_cur.chan_num].q_size[dummy_cur.flags.com.q_index] = 1; /* 1 for non zero */

    /* Act */
    CF_CFDP_CycleTx(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 2);
    UtAssert_NULL(arg_c->cur);
    /* Assert for cf_move_transaction */
    UtAssert_STUB_COUNT(CF_CList_Remove, 1);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_CycleTx_EnterWhileLoopOnceAndCall_cf_move_transaction_SecondCallTo_CF_CList_Traverse_ReturnsAnd_ran_one_IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL
   */

/* end CF_CFDP_CycleTx tests */

/*******************************************************************************
**
**  CF_CFDP_DoTick tests (medium) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNotNull_And_NotEqTo_t_Return_CLIST_CONT(void)
{
    /* Arrange */
    CF_Transaction_t dummy_cur;
    CF_Channel_t     dummy_c;
    CF_Transaction_t dummy_t;
    CF_CListNode_t  *arg_node = &dummy_t.cl_node;
    tick_args_t      dummy_args;
    void            *arg_context = (void *)&dummy_args;
    int              local_result;

    dummy_args.c      = &dummy_c;
    dummy_args.c->cur = &dummy_cur;

    /* Act */
    local_result = CF_CFDP_DoTick(arg_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_CFDP_DoTick returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);
} /* end Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNotNull_And_NotEqTo_t_Return_CLIST_CONT */

void Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNull_But_t_flags_all_suspended_IsTrueReturn_CLIST_CONT(
    void)
{
    /* Arrange */
    CF_Channel_t     dummy_c;
    CF_Transaction_t dummy_t;
    CF_CListNode_t  *arg_node = &dummy_t.cl_node;
    tick_args_t      dummy_args;
    void            *arg_context = (void *)&dummy_args;
    int              local_result;

    dummy_args.c      = &dummy_c;
    dummy_args.c->cur = NULL;

    dummy_t.flags.com.suspended = true;

    /* Act */
    local_result = CF_CFDP_DoTick(arg_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_CFDP_DoTick returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);
    UtAssert_NULL(dummy_args.c->cur);
} /* end
     Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNull_But_t_flags_all_suspended_IsTrueReturn_CLIST_CONT */

void Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_But_t_flags_all_suspended_IsTrueReturn_CLIST_CONT(
    void)
{
    /* Arrange */
    CF_Channel_t     dummy_c;
    CF_Transaction_t dummy_t;
    CF_CListNode_t  *arg_node = &dummy_t.cl_node;
    tick_args_t      dummy_args;
    void            *arg_context = (void *)&dummy_args;
    int              local_result;

    dummy_args.c      = &dummy_c;
    dummy_args.c->cur = &dummy_t;

    dummy_t.flags.com.suspended = true;

    /* Act */
    local_result = CF_CFDP_DoTick(arg_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_CFDP_DoTick returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);
    UtAssert_NULL(dummy_args.c->cur);
} /* end Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_But_t_flags_all_suspended_IsTrueReturn_CLIST_CONT
   */

void Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_DoesNotChange_cur_From_NULL_Return_CLIST_CONT(
    void)
{
    /* Arrange */
    CF_Channel_t     dummy_c;
    CF_Transaction_t dummy_t;
    CF_CListNode_t  *arg_node = &dummy_t.cl_node;
    tick_args_t      dummy_args;
    void            *arg_context        = (void *)&dummy_args;
    int              initial_early_exit = Any_int_Except(1);
    int              local_result;

    dummy_args.c      = &dummy_c;
    dummy_args.c->cur = &dummy_t;

    dummy_t.flags.com.suspended = false;

    dummy_args.fn = (void (*)(CF_Transaction_t *, int *))Dummy_tick_args_t_fn;

    dummy_args.early_exit = initial_early_exit;

    /* Act */
    local_result = CF_CFDP_DoTick(arg_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_CFDP_DoTick returned %d and should be %d (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);
    UtAssert_STUB_COUNT(Dummy_tick_args_t_fn, 1);
    UtAssert_NULL(dummy_args.c->cur);
    UtAssert_True(dummy_args.early_exit == initial_early_exit,
                  "args->early_exit is %d and should be %d (value before call)", dummy_args.early_exit,
                  initial_early_exit);
} /* end
     Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_DoesNotChange_cur_From_NULL_Return_CLIST_CONT
   */

void Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_Changea_cur_To_notNULL_Set_args_early_exit_To_1_AndReturn_CLIST_EXIT(
    void)
{
    /* Arrange */
    CF_Channel_t     dummy_c;
    CF_Transaction_t dummy_t;
    CF_CListNode_t  *arg_node = &dummy_t.cl_node;
    tick_args_t      dummy_args;
    void            *arg_context = (void *)&dummy_args;
    int              local_result;

    dummy_args.c      = &dummy_c;
    dummy_args.c->cur = &dummy_t;

    dummy_t.flags.com.suspended = false;

    dummy_args.fn = (void (*)(CF_Transaction_t *, int *))Dummy_tick_args_t_fn;

    UT_SetHandlerFunction(UT_KEY(Dummy_tick_args_t_fn), Handler_Dummy_tick_args_t_fn_Set_cur_notNULL,
                          &dummy_args.c->cur);

    dummy_args.early_exit = Any_int_Except(1);

    /* Act */
    local_result = CF_CFDP_DoTick(arg_node, arg_context);

    /* Assert */
    UtAssert_True(local_result == CF_CLIST_EXIT, "CF_CFDP_DoTick returned %d and should be %d (CF_CLIST_EXIT)",
                  local_result, CF_CLIST_EXIT);
    UtAssert_STUB_COUNT(Dummy_tick_args_t_fn, 1);
    UtAssert_NOT_NULL(dummy_args.c->cur);
    UtAssert_True(dummy_args.early_exit == 1, "args->early_exit is %d and should be 1", dummy_args.early_exit);
} /* end
     Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_Changea_cur_To_notNULL_Set_args_early_exit_To_1_AndReturn_CLIST_EXIT
   */

/* end CF_CFDP_DoTick tests */

/*******************************************************************************
**
**  CF_CFDP_TickTransactions tests (medium) - full coverage - 1 coverage JIRA issue - [unstubbables: CF_CFDP_DoTick
*(medium)]
**
**  NOTE: The number of tests are determined by CF_TickType_t enum, if values are added there, there will be more values
*this function can use
**
*******************************************************************************/

void Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsEqTo_CF_TICK_NUM_TYPES(void)
{
    // /* Arrange */
    // CF_Channel_t         dummy_c;
    // CF_Channel_t*        arg_c = &dummy_c;

    // arg_c->tick_type = CF_TickType_NUM_TYPES;

    // /* Act */
    // CF_CFDP_TickTransactions(arg_c);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - c->tick_type<CF_TickType_NUM_TYPES");
} /* end Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsEqTo_CF_TICK_NUM_TYPES */

/* NOTE: Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsGreaterThan_CF_TICK_NUM_TYPES not required but
 * desired */
// void Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsGreaterThan_CF_TICK_NUM_TYPES(void)
// {
//     /* Arrange */
//     CF_Channel_t         dummy_c;
//     CF_Channel_t*        arg_c = &dummy_c;

//     arg_c->tick_type = Any_uint16_GreaterThan(CF_TickType_NUM_TYPES);

//     /* Act */
//     // CF_CFDP_TickTransactions(arg_c);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsGreaterThan_CF_TICK_NUM_TYPES */

void Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_RX_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_RX(
    void)
{
    /* Arrange */
    CF_Channel_t  dummy_c;
    CF_Channel_t *arg_c                                      = &dummy_c;
    uint8         expected_number_of_CF_CList_Traverse_calls = CF_TickType_NUM_TYPES - CF_TickType_RX;

    arg_c->tick_type = CF_TickType_RX;

    type_of_context_CF_CList_Traverse = POINTER;

    /* Act */
    CF_CFDP_TickTransactions(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, expected_number_of_CF_CList_Traverse_calls);
    UtAssert_True(arg_c->tick_type == CF_TickType_RX, "c->tick_type is %d and should be %d (CF_TickType_RX)",
                  arg_c->tick_type, CF_TickType_RX);
} /* end
     Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_RX_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_RX
   */

void Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NORM(
    void)
{
    /* Arrange */
    CF_Channel_t  dummy_c;
    CF_Channel_t *arg_c                                      = &dummy_c;
    uint8         expected_number_of_CF_CList_Traverse_calls = CF_TickType_NUM_TYPES - CF_TickType_TXW_NORM;

    arg_c->tick_type = CF_TickType_TXW_NORM;

    type_of_context_CF_CList_Traverse = POINTER;

    /* Act */
    CF_CFDP_TickTransactions(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, expected_number_of_CF_CList_Traverse_calls);
    UtAssert_True(arg_c->tick_type == CF_TickType_RX, "c->tick_type is %d and should be %d (CF_TickType_RX)",
                  arg_c->tick_type, CF_TickType_RX);
} /* end
     Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NORM
   */

void Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NAK(
    void)
{
    /* Arrange */
    CF_Channel_t  dummy_c;
    CF_Channel_t *arg_c                                      = &dummy_c;
    uint8         expected_number_of_CF_CList_Traverse_calls = CF_TickType_NUM_TYPES - CF_TickType_TXW_NAK;

    arg_c->tick_type = CF_TickType_TXW_NAK;

    type_of_context_CF_CList_Traverse = POINTER;

    /* Act */
    CF_CFDP_TickTransactions(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, expected_number_of_CF_CList_Traverse_calls);
    UtAssert_True(arg_c->tick_type == CF_TickType_RX, "c->tick_type is %d and should be %d (CF_TickType_RX)",
                  arg_c->tick_type, CF_TickType_RX);
} /* end
     Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NAK
   */

void Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_And_Determined_args_early_exit_Is_true_RunsLoopOnlyOnce(
    void)
{
    /* Arrange */
    CF_Channel_t  dummy_c;
    CF_Channel_t *arg_c                                      = &dummy_c;
    uint8         expected_number_of_CF_CList_Traverse_calls = CF_TickType_NUM_TYPES - CF_TickType_TXW_NAK;

    arg_c->tick_type = CF_TickType_TXW_NAK;

    type_of_context_CF_CList_Traverse = POINTER;

    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), Handler_CF_CList_Traverse_Sets_args_early_exit_To_1, NULL);

    /* Act */
    CF_CFDP_TickTransactions(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, expected_number_of_CF_CList_Traverse_calls);
    UtAssert_True(arg_c->tick_type == CF_TickType_RX, "c->tick_type is %d and should be %d (CF_TickType_RX)",
                  arg_c->tick_type, CF_TickType_RX);
} /* end
     Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_And_Determined_args_early_exit_Is_true_RunsLoopOnlyOnce
   */

void Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_And_Determined_args_early_exit_Is_true_GoesTo_early_exit(
    void)
{
    /* Arrange */
    CF_Channel_t  dummy_c;
    CF_Channel_t *arg_c                                      = &dummy_c;
    uint8         expected_number_of_CF_CList_Traverse_calls = 1;

    arg_c->tick_type = CF_TickType_TXW_NORM;

    type_of_context_CF_CList_Traverse = POINTER;

    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), Handler_CF_CList_Traverse_Sets_args_early_exit_To_1, NULL);

    /* Act */
    CF_CFDP_TickTransactions(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, expected_number_of_CF_CList_Traverse_calls);
    UtAssert_True(arg_c->tick_type == CF_TickType_TXW_NORM,
                  "c->tick_type is %d and should be %d (CF_TickType_TXW_NORM)", arg_c->tick_type, CF_TickType_TXW_NORM);
} /* end
     Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_And_Determined_args_early_exit_Is_true_GoesTo_early_exit
   */

void Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_RunsLoopAsLongAs_args_cont_Is_true(void)
{
    /* Arrange */
    CF_Channel_t  dummy_c;
    CF_Channel_t *arg_c      = &dummy_c;
    uint32        stop_count = Any_uint8_LessThan(9) + 2; /* Any_uint8_LessThan(9) + 2 is 2-10 */

    arg_c->tick_type = CF_TickType_TXW_NAK;

    type_of_context_CF_CList_Traverse = POINTER;

    UT_SetHandlerFunction(UT_KEY(CF_CList_Traverse), Handler_CF_CList_Traverse_Sets_args_cont_To_1_UntilGivenCount,
                          &stop_count);

    /* Act */
    CF_CFDP_TickTransactions(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CList_Traverse, stop_count);
    UtAssert_True(arg_c->tick_type == CF_TickType_RX, "c->tick_type is %d and should be %d (CF_TickType_RX)",
                  arg_c->tick_type, CF_TickType_RX);
} /* end Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_RunsLoopAsLongAs_args_cont_Is_true */

/* end CF_CFDP_TickTransactions tests */

/*******************************************************************************
**
**  CF_CFDP_TxFile__ tests (simple) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction_cfdp_class_SetTo_CLASS_1(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t          = &dummy_t;
    CF_CFDP_Class_t   arg_cfdp_class = CF_CFDP_CLASS_1;
    uint8             arg_keep       = Any_uint8();
    uint8             arg_chan       = Any_uint8();
    uint8             arg_priority   = Any_uint8();

    /* Act */
    CF_CFDP_TxFile__(arg_t, arg_cfdp_class, arg_keep, arg_chan, arg_priority);

    /* Assert */
    UtAssert_True(arg_t->chan_num == arg_chan, "CF_CFDP_TxFile__ set t->chan_num to %u and should be %u (chan)",
                  arg_t->chan_num, arg_chan);
    UtAssert_True(arg_t->priority == arg_priority, "CF_CFDP_TxFile__ set t->priority to %u and should be %u (priority)",
                  arg_t->priority, arg_priority);
    UtAssert_True(arg_t->keep == arg_keep, "CF_CFDP_TxFile__ set t->keep to %u and should be %u (keep)", arg_t->keep,
                  arg_keep);
    UtAssert_True(arg_t->state == CF_TxnState_S1,
                  "CF_CFDP_TxFile__ set t->state to %u and should be %u (CF_TxnState_S1) because cfdp_class is %u "
                  "(CF_CFDP_CLASS_1)",
                  arg_t->state, CF_TxnState_S1, arg_cfdp_class);
} /* end Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction_cfdp_class_SetTo_CLASS_1 */

void Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction_cfdp_class_SetTo_CLASS_2(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t          = &dummy_t;
    CF_CFDP_Class_t   arg_cfdp_class = CF_CFDP_CLASS_2;
    uint8             arg_keep       = Any_uint8();
    uint8             arg_chan       = Any_uint8();
    uint8             arg_priority   = Any_uint8();

    /* Act */
    CF_CFDP_TxFile__(arg_t, arg_cfdp_class, arg_keep, arg_chan, arg_priority);

    /* Assert */
    UtAssert_True(arg_t->chan_num == arg_chan, "CF_CFDP_TxFile__ set t->chan_num to %u and should be %u (chan)",
                  arg_t->chan_num, arg_chan);
    UtAssert_True(arg_t->priority == arg_priority, "CF_CFDP_TxFile__ set t->priority to %u and should be %u (priority)",
                  arg_t->priority, arg_priority);
    UtAssert_True(arg_t->keep == arg_keep, "CF_CFDP_TxFile__ set t->keep to %u and should be %u (keep)", arg_t->keep,
                  arg_keep);
    UtAssert_True(arg_t->state == CF_TxnState_S2,
                  "CF_CFDP_TxFile__ set t->state to %u and should be %u (CF_TxnState_S2) because cfdp_class is %u "
                  "(CF_CFDP_CLASS_2)",
                  arg_t->state, CF_TxnState_S2, arg_cfdp_class);
} /* end Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction */

/* end CF_CFDP_TxFile__ tests */

/*******************************************************************************
**
**  CF_CFDP_TxFile_ tests (medium) - full coverage - [unstubbables: CF_CFDP_TxFile__ (simple) - [],
*CF_CFDP_ArmInactTimer (simple) - [],  CF_CFDP_FindUnusedChunks (small) - []]
**
*******************************************************************************/

void Test_CF_CFDP_TxFile_DoesNotError(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t          = &dummy_t;
    CF_CFDP_Class_t   arg_cfdp_class = Any_cfdp_class_t();
    uint8             arg_keep       = Any_uint8();
    uint8             arg_chan       = Any_cf_chan_num();
    uint8             arg_priority   = Any_uint8();
    CF_EntityId_t     arg_dest_id    = Any_uint8();

    CF_AppData.config_table = &dummy_config_table;

    arg_t->history = &dummy_history;

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_CListNode_t dummy_cs;

    CF_AppData.engine.channels[arg_chan].cs[CF_Direction_TX] = &dummy_cs;

    /* Act */
    CF_CFDP_TxFile_(arg_t, arg_cfdp_class, arg_keep, arg_chan, arg_priority, arg_dest_id);

    /* Assert */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_STUB_COUNT(CF_InsertSortPrio, 1);
    /* Assert for CF_CFDP_ArmInactTimer */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 1);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
} /* end Test_CF_CFDP_TxFile_DoesNotError */

/* end CF_CFDP_TxFile_ tests */

/*******************************************************************************
**
**  CF_CFDP_TxFile tests (medium) - full coverage - 3 coverage JIRA issue - [unstubbables: CF_CFDP_FindUnusedTransaction
*(large) - [unstubbables: CF_CList_Remove_Ex (small)], CF_CFDP_TxFile_ (medium) - [unstubbables: CF_CFDP_TxFile__
*(simple) - [], CF_CFDP_ArmInactTimer (simple) - [],  CF_CFDP_FindUnusedChunks (small) - []]
**
*******************************************************************************/

void Test_CF_CFDP_TxFile_AssertsBecause_chan_IsGreaterThan_CF_NUM_CHANNELS(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
} /* end Test_CF_CFDP_TxFile_AssertsBecause_chan_IsGreaterThan_CF_NUM_CHANNELS */

/* TODO: Test_CF_CFDP_TxFile_AssertsBecause_chan_IsEqTo_CF_NUM_CHANNELS not required but desired */
// void Test_CF_CFDP_TxFile_AssertsBecause_chan_IsEqTo_CF_NUM_CHANNELS(void)
// {
//     /* Arrange */
//     /* Act */
//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert");
// } /* end Test_CF_CFDP_TxFile_AssertsBecause_chan_IsEqTo_CF_NUM_CHANNELS */

void Test_CF_CFDP_TxFile_FailsBecause_c_num_cmd_tx_IsEqTo_CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN(void)
{
    /* Arrange */
    CF_Channel_t   *dummy_c;
    char            arg_src_filename[CF_FILENAME_MAX_LEN] = "placeholder\0";
    char            arg_dst_filename[CF_FILENAME_MAX_LEN] = "dstholder\0";
    CF_CFDP_Class_t arg_cfdp_class                        = Any_cfdp_class_t();
    uint8           arg_keep                              = Any_uint8();
    uint8           arg_chan                              = Any_uint8_LessThan(CF_NUM_CHANNELS);
    uint8           arg_priority                          = Any_uint8();
    CF_EntityId_t   arg_dest_id                           = Any_uint8();

    dummy_c             = &CF_AppData.engine.channels[arg_chan];
    dummy_c->num_cmd_tx = CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN;

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    result = CF_CFDP_TxFile(arg_src_filename, arg_dst_filename, arg_cfdp_class, arg_keep, arg_chan, arg_priority,
                            arg_dest_id);

    /* Assert */
    UtAssert_True(result == -1, "CF_CFDP_TxFile returned %d and should be -1", result);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_MAX_CMD_TX,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_MAX_CMD_TX)", EventID,
                  CF_EID_ERR_CFDP_MAX_CMD_TX);
} /* end Test_CF_CFDP_TxFile_FailsBecause_c_num_cmd_tx_IsEqTo_CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN */

void Test_CF_CFDP_TxFile_AssertsBecause_t_IsReturned_NULL_From_CF_CFDP_FindUnusedTransaction(void)
{
    // /* Arrange */
    // CF_History_t       dummy_history;
    // CF_Transaction_t   dummy_t;
    // CF_Channel_t*      dummy_c;
    // char            arg_src_filename[CF_FILENAME_MAX_LEN] = "placeholder\0";
    // char            arg_dst_filename[CF_FILENAME_MAX_LEN] = "dstholder\0";
    // CF_CFDP_Class_t    arg_cfdp_class = Any_cfdp_class_t();
    // uint8           arg_keep = Any_uint8();
    // uint8           arg_chan = Any_uint8_LessThan(CF_NUM_CHANNELS);
    // uint8           arg_priority = Any_uint8();
    // CF_EntityId_t  arg_dest_id = Any_uint8();

    // dummy_c = &CF_AppData.engine.channels[arg_chan];
    // dummy_c->num_cmd_tx = Any_uint32_Except(CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN);

    // UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID,
    //   sizeof(EventID), false);

    // dummy_t.history = &dummy_history;

    // dummy_t.state = Any_uint8_Except(CF_TxnState_IDLE); /* asserts */

    // /* Arrange for CF_CFDP_FindUnusedTransaction */
    // dummy_c->qs[CF_QueueIdx_FREE] = NULL; /* makes t NULL */

    // /* Act */
    // result = CF_CFDP_TxFile(arg_src_filename,
    //                         arg_dst_filename,
    //                         arg_cfdp_class,
    //                         arg_keep,
    //                         arg_chan,
    //                         arg_priority,
    //                         arg_dest_id);

    // /* Assert */
    // UtAssert_UINT32_EQ(result, CFE_SUCCESS);
    // /* Assert for CF_CFDP_TxFile_ */
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    // UtAssert_True(EventID == CF_EID_INF_CFDP_S_START_SEND,
    //   "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_INF_CFDP_S_START_SEND)",
    //   EventID, CF_EID_INF_CFDP_S_START_SEND);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t");
} /* end Test_CF_CFDP_TxFile_AssertsBecause_t_IsReturned_NULL_From_CF_CFDP_FindUnusedTransaction */

void Test_CF_CFDP_TxFile_AssertsBecause_t_state_IsNotEqTo_CFDP_IDLE(void)
{
    // /* Arrange */
    // CF_History_t       dummy_history;
    // CF_Transaction_t   dummy_t;
    // CF_Channel_t*      dummy_c;
    // char            arg_src_filename[CF_FILENAME_MAX_LEN] = "placeholder\0";
    // char            arg_dst_filename[CF_FILENAME_MAX_LEN] = "dstholder\0";
    // CF_CFDP_Class_t    arg_cfdp_class = Any_cfdp_class_t();
    // uint8           arg_keep = Any_uint8();
    // uint8           arg_chan = Any_uint8_LessThan(CF_NUM_CHANNELS);
    // uint8           arg_priority = Any_uint8();
    // CF_EntityId_t  arg_dest_id = Any_uint8();

    // dummy_c = &CF_AppData.engine.channels[arg_chan];
    // dummy_c->num_cmd_tx = Any_uint32_Except(CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN);

    // UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID,
    //   sizeof(EventID), false);

    // dummy_t.history = &dummy_history;

    // dummy_t.state = Any_uint8_Except(CF_TxnState_IDLE); /* asserts */

    // /* Arrange for CF_CFDP_FindUnusedTransaction */
    // dummy_c->qs[CF_QueueIdx_FREE] = &dummy_t.cl_node;
    // dummy_c->qs[CF_QueueIdx_HIST] = &dummy_t.history->cl_node;
    // dummy_c->qs[CF_QueueIdx_HIST_FREE] = NULL; /* always chooses CF_QueueIdx_HIST */

    // /* Arrange for CF_CList_Remove_Ex */
    // CF_AppData.hk.channel_hk[dummy_c-CF_AppData.engine.channels].q_size[CF_QueueIdx_FREE] = 1; /* avoid assert first
    // call */ CF_AppData.hk.channel_hk[dummy_c-CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] = 1; /* avoid
    // assert second call
    // */

    // /* Act */
    // result = CF_CFDP_TxFile(arg_src_filename,
    //                         arg_dst_filename,
    //                         arg_cfdp_class,
    //                         arg_keep,
    //                         arg_chan,
    //                         arg_priority,
    //                         arg_dest_id);

    // /* Assert */
    // UtAssert_UINT32_EQ(result, CFE_SUCCESS);
    // /* Assert for CF_CFDP_TxFile_ */
    // UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    // UtAssert_True(EventID == CF_EID_INF_CFDP_S_START_SEND,
    //   "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_INF_CFDP_S_START_SEND)",
    //   EventID, CF_EID_INF_CFDP_S_START_SEND);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - t->state==CF_TxnState_IDLE");
} /* end Test_CF_CFDP_TxFile_AssertsBecause_t_state_IsNotEqTo_CFDP_IDLE */

void Test_CF_CFDP_TxFile_SuccessIncrements_c_num_cmd_tx_AndSets_t_flags_tx_cmd_tx_To_1(void)
{
    /* Arrange */
    CF_History_t     dummy_history;
    CF_Transaction_t dummy_t;
    CF_Channel_t    *dummy_c;
    char             arg_src_filename[CF_FILENAME_MAX_LEN] = "placeholder\0";
    char             arg_dst_filename[CF_FILENAME_MAX_LEN] = "dstholder\0";
    CF_CFDP_Class_t  arg_cfdp_class                        = Any_cfdp_class_t();
    uint8            arg_keep                              = Any_uint8();
    uint8            arg_chan                              = Any_uint8_LessThan(CF_NUM_CHANNELS);
    uint8            arg_priority                          = Any_uint8();
    CF_EntityId_t    arg_dest_id                           = Any_uint8();

    dummy_c             = &CF_AppData.engine.channels[arg_chan];
    dummy_c->num_cmd_tx = Any_uint32_Except(CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN);

    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    dummy_t.history = &dummy_history;

    dummy_t.state = CF_TxnState_IDLE; /* bypass assert */

    /* Arrange for CF_CFDP_FindUnusedTransaction */
    dummy_c->qs[CF_QueueIdx_FREE]      = &dummy_t.cl_node;
    dummy_c->qs[CF_QueueIdx_HIST]      = &dummy_t.history->cl_node;
    dummy_c->qs[CF_QueueIdx_HIST_FREE] = NULL; /* always chooses CF_QueueIdx_HIST */

    /* Arrange for CF_CList_Remove_Ex */
    CF_AppData.hk.channel_hk[dummy_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_FREE] =
        1; /* avoid assert first call */
    CF_AppData.hk.channel_hk[dummy_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] =
        1; /* avoid assert second call */

    /* Arrange for CF_CFDP_TxFile_ */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_CListNode_t dummy_cs;

    dummy_c->cs[CF_Direction_TX] = &dummy_cs;

    /* Act */
    result = CF_CFDP_TxFile(arg_src_filename, arg_dst_filename, arg_cfdp_class, arg_keep, arg_chan, arg_priority,
                            arg_dest_id);

    /* Assert */
    UtAssert_UINT32_EQ(result, CFE_SUCCESS);
    /* Assert for CF_CFDP_TxFile_ */
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_INF_CFDP_S_START_SEND,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_INF_CFDP_S_START_SEND)", EventID,
                  CF_EID_INF_CFDP_S_START_SEND);
} /* end Test_CF_CFDP_TxFile_SuccessIncrements_c_num_cmd_tx_AndSets_t_flags_tx_cmd_tx_To_1 */

/* end CF_CFDP_TxFile tests */

/*******************************************************************************
**
**  CF_CFDP_PlaybackDir_ tests (medium) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_PlaybackDir_FailsBecauseDirectoryWasNotOpenedThenIncrements_fault_directory_read(void)
{
    /* Arrange */
    CF_Playback_t   dummy_p;
    CF_Playback_t  *arg_p                                 = &dummy_p;
    char            arg_src_filename[CF_FILENAME_MAX_LEN] = "placeholder\0";
    char            arg_dst_filename[CF_FILENAME_MAX_LEN] = "dstholder\0";
    CF_CFDP_Class_t arg_cfdp_class                        = Any_cfdp_class_t();
    uint8           arg_keep                              = Any_uint8();
    uint8           arg_chan                              = Any_uint8_LessThan(CF_NUM_CHANNELS);
    uint8           arg_priority                          = Any_uint8();
    CF_EntityId_t   arg_dest_id                           = Any_uint8();
    uint16          initial_fault_directory_read          = Any_uint16();
    int32           expected_result                       = Any_int32_Except(OS_SUCCESS);

    CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read = initial_fault_directory_read;

    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryOpen), expected_result);
    UT_SetDataBuffer(UT_KEY(CFE_EVS_SendEvent), &EventID, sizeof(EventID), false);

    /* Act */
    result = CF_CFDP_PlaybackDir_(arg_p, arg_src_filename, arg_dst_filename, arg_cfdp_class, arg_keep, arg_chan,
                                  arg_priority, arg_dest_id);

    /* Assert */
    UtAssert_True(result == expected_result, "CF_CFDP_TxFile returned %d and should be %d (fail)", result,
                  expected_result);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read ==
                      (uint16)(initial_fault_directory_read + 1),
                  "CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read is %d and should be 1 more than %d "
                  "(value before call)",
                  CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read, initial_fault_directory_read);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
    UtAssert_True(EventID == CF_EID_ERR_CFDP_OPENDIR,
                  "CFE_EVS_SendEvent receive event id %u and should receive %u (CF_EID_ERR_CFDP_OPENDIR)", EventID,
                  CF_EID_ERR_CFDP_OPENDIR);
} /* end Test_CF_CFDP_PlaybackDir_FailsBecauseDirectoryWasNotOpenedThenIncrements_fault_directory_read */

void Test_CF_CFDP_PlaybackDir_SuccessSetsCorrect_p_ValuesAndReturns_OS_SUCCESS(void)
{
    /* Arrange */
    CF_Playback_t   dummy_p;
    CF_Playback_t  *arg_p = &dummy_p;
    char            arg_src_filename[CF_FILENAME_MAX_LEN];
    char            arg_dst_filename[CF_FILENAME_MAX_LEN];
    CF_CFDP_Class_t arg_cfdp_class               = Any_cfdp_class_t();
    uint8           arg_keep                     = Any_uint8_LessThan(2);
    uint8           arg_chan                     = Any_uint8_LessThan(CF_NUM_CHANNELS);
    uint8           arg_priority                 = Any_uint8_LessThan(2);
    CF_EntityId_t   arg_dest_id                  = Any_uint8();
    uint16          initial_fault_directory_read = Any_uint16();
    int32           expected_result              = OS_SUCCESS;

    AnyRandomStringOfLettersOfLengthCopy(arg_src_filename, Any_uint8_LessThan(CF_FILENAME_MAX_LEN));
    AnyRandomStringOfLettersOfLengthCopy(arg_dst_filename, Any_uint8_LessThan(CF_FILENAME_MAX_LEN));
    CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read = initial_fault_directory_read;

    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryOpen), expected_result);

    /* Act */
    result = CF_CFDP_PlaybackDir_(arg_p, arg_src_filename, arg_dst_filename, arg_cfdp_class, arg_keep, arg_chan,
                                  arg_priority, arg_dest_id);

    /* Assert */
    UtAssert_True(result == expected_result, "CF_CFDP_PlaybackDir_ returned %d and should be %d (OS_SUCCESS)", result,
                  expected_result);
    UtAssert_True(arg_p->diropen == 1, "p->diropen set to %u and should be 1", arg_p->diropen);
    UtAssert_True(arg_p->busy == 1, "p->busy set to %u and should be 1", arg_p->busy);
    UtAssert_True(arg_p->keep == arg_keep, "p->keep set to %u and should be %u (keep)", arg_p->keep, arg_keep);
    UtAssert_True(arg_p->priority == arg_priority, "p->priority set to %u and should be %u (priority)", arg_p->priority,
                  arg_priority);
    UtAssert_True(arg_p->dest_id == arg_dest_id, "p->dest_id set to %u and should be %u (dest_id)", arg_p->dest_id,
                  arg_dest_id);
    UtAssert_True(arg_p->cfdp_class == arg_cfdp_class, "p->cfdp_class set to %u and should be %u (cfdp_class)",
                  arg_p->cfdp_class, arg_cfdp_class);
    UtAssert_StrCmp(arg_p->fnames.src_filename, arg_src_filename,
                    "arg_p->fnames.src_filename set to '%s' and should be '%s' (src_filename)",
                    arg_p->fnames.src_filename, arg_src_filename);
    UtAssert_StrCmp(arg_p->fnames.dst_filename, arg_dst_filename,
                    "arg_p->fnames.dst_filename set to '%s' and should be '%s' (src_filename)",
                    arg_p->fnames.dst_filename, arg_dst_filename);
    UtAssert_True(CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read == initial_fault_directory_read,
                  "CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read is %d and should not have changed "
                  "from %d (value before call)",
                  CF_AppData.hk.channel_hk[arg_chan].counters.fault.directory_read, initial_fault_directory_read);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
} /* end Test_CF_CFDP_PlaybackDir_FailsBecauseDirectoryWasNotOpenedThenIncrements_fault_directory_read */

/* end CF_CFDP_PlaybackDir_ tests */

/*******************************************************************************
**
**  CF_CFDP_PlaybackDir tests (small) - full coverage - [unstubbables: CF_CFDP_PlaybackDir_ (medium) - []]
**
*******************************************************************************/

void Test_CF_CFDP_PlaybackDir_All_p_busy_Are_1_SendEventThenReturn_neg1(void)
{
    /* Arrange */
    const char      arg_src_filename[CF_FILENAME_MAX_LEN];
    const char      arg_dst_filename[CF_FILENAME_MAX_LEN];
    CF_CFDP_Class_t arg_cfdp_class = Any_cfdp_class_t();
    uint8           arg_keep       = Any_uint8();
    uint8           arg_chan       = Any_cf_chan_num();
    uint8           arg_priority   = Any_uint8();
    uint16          arg_dest_id    = Any_uint16();
    int             i              = 0;

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        CF_AppData.engine.channels[arg_chan].playback[i].busy = 1;
    }

    /* Act */
    result = CF_CFDP_PlaybackDir(arg_src_filename, arg_dst_filename, arg_cfdp_class, arg_keep, arg_chan, arg_priority,
                                 arg_dest_id);

    /* Assert */
    UtAssert_INT32_EQ(result, -1);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 1);
} /* end Test_CF_CFDP_PlaybackDir_All_p_busy_Are_1_SendEventThenReturn_neg1 */

void Test_CF_CFDP_PlaybackDir_First_p_busy_Is_0_Call_CF_CFDP_PlaybackDir__AndReturnCallValue(void)
{
    /* Arrange */
    const char      arg_src_filename[CF_FILENAME_MAX_LEN];
    const char      arg_dst_filename[CF_FILENAME_MAX_LEN];
    CF_CFDP_Class_t arg_cfdp_class = Any_cfdp_class_t();
    uint8           arg_keep       = Any_uint8();
    uint8           arg_chan       = Any_cf_chan_num();
    uint8           arg_priority   = Any_uint8();
    uint16          arg_dest_id    = Any_uint16();
    int             i              = 0;

    CF_AppData.engine.channels[arg_chan].playback[i].busy = 0;

    /* Arrange for CF_CFDP_PlaybackDir_ */
    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryOpen), OS_SUCCESS);

    /* Act */
    result = CF_CFDP_PlaybackDir(arg_src_filename, arg_dst_filename, arg_cfdp_class, arg_keep, arg_chan, arg_priority,
                                 arg_dest_id);

    /* Assert */
    UtAssert_INT32_EQ(result, 0);
    UtAssert_STUB_COUNT(CFE_EVS_SendEvent, 0);
    /* Assert for CF_CFDP_PlaybackDir_ */
    UtAssert_STUB_COUNT(OS_DirectoryOpen, 1);
} /* end Test_CF_CFDP_PlaybackDir_First_p_busy_Is_0_Call_CF_CFDP_PlaybackDir__AndReturnCallValue */

/* end CF_CFDP_PlaybackDir tests */

/*******************************************************************************
**
**  CF_CFDP_ProcessPlaybackDirectory tests (very large) - full coverage - 1 coverage JIRA issue - [unstubbables:
*CF_CFDP_FindUnusedTransaction (large) - [unstubbables: CF_CList_Remove_Ex  (medium) - [unstubbables: CF_CFDP_TxFile__
*(simple) - [], CF_CFDP_ArmInactTimer (simple) - [],  CF_CFDP_FindUnusedChunks (small) - [], ]
**
*******************************************************************************/

void Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_But_p_num_ts_Is_non0_DoesNothingAndGiven_p_busy_IsUnchanged(
    void)
{
    /* Arrange */
    CF_Channel_t   dummy_c;
    CF_Channel_t  *arg_c = &dummy_c;
    CF_Playback_t  dummy_p;
    CF_Playback_t *arg_p = &dummy_p;

    arg_p->diropen = 0;
    arg_p->num_ts  = Any_uint16_Except(0);

    arg_p->busy = 1;

    /* Act */
    CF_CFDP_ProcessPlaybackDirectory(arg_c, arg_p);

    /* Assert */
    UtAssert_True(arg_p->busy == 1, "p->busy is %d and should be 1", arg_p->busy);
} /* end
     Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_But_p_num_ts_Is_non0_DoesNothingAndGiven_p_busy_IsUnchanged
   */

void Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_And_p_num_ts_Is_0_Given_p_busy_IsSetTo_0(void)
{
    /* Arrange */
    CF_Channel_t   dummy_c;
    CF_Channel_t  *arg_c = &dummy_c;
    CF_Playback_t  dummy_p;
    CF_Playback_t *arg_p = &dummy_p;

    arg_p->diropen = 0;
    arg_p->num_ts  = 0;

    arg_p->busy = 1;

    /* Act */
    CF_CFDP_ProcessPlaybackDirectory(arg_c, arg_p);

    /* Assert */
    UtAssert_True(arg_p->busy == 0, "p->busy is %d and should be 0", arg_p->busy);
} /* end Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_And_p_num_ts_Is_0_Given_p_busy_IsSetTo_0 */

void Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_1_And_p_num_ts_IsLessThan_CF_NUM_TRANSACTIONS_PER_PLAYBACK_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0(
    void)
{
    /* Arrange */
    CF_Channel_t   dummy_c;
    CF_Channel_t  *arg_c = &dummy_c;
    CF_Playback_t  dummy_p;
    CF_Playback_t *arg_p = &dummy_p;

    arg_p->diropen = 1;
    arg_p->num_ts  = Any_uint8_BetweenExcludeMax(1, CF_NUM_TRANSACTIONS_PER_PLAYBACK);

    arg_p->busy = 1;

    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryRead), Any_int32_Except(CFE_SUCCESS));

    UT_SetHandlerFunction(UT_KEY(OS_DirectoryClose), Handler_OS_DirectoryClose_ReportOnly, NULL);

    /* Act */
    CF_CFDP_ProcessPlaybackDirectory(arg_c, arg_p);

    /* Assert */
    UtAssert_STUB_COUNT(OS_DirectoryRead, 1);
    UtAssert_STUB_COUNT(OS_DirectoryClose, 1);
    UtAssert_True(arg_p->diropen == 0, "p->diropen is %d and should be 0", arg_p->diropen);
    UtAssert_True(arg_p->busy == 1, "p->busy is %d and should be 1", arg_p->busy);
} /* end
     Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_1_And_p_num_ts_IsLessThan_CF_NUM_TRANSACTIONS_PER_PLAYBACK_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0
   */

void Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0(
    void)
{
    /* Arrange */
    CF_Channel_t   dummy_c;
    CF_Channel_t  *arg_c = &dummy_c;
    CF_Playback_t  dummy_p;
    CF_Playback_t *arg_p                                      = &dummy_p;
    char           context_OS_DirectoryRead[OS_MAX_FILE_NAME] = ".\0";

    arg_p->diropen = 1;
    arg_p->num_ts  = Any_uint8_BetweenExcludeMax(1, CF_NUM_TRANSACTIONS_PER_PLAYBACK);

    arg_p->busy = 1;

    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryRead), CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 2, Any_int32_Except(CFE_SUCCESS));

    UT_SetDataBuffer(UT_KEY(OS_DirectoryRead), context_OS_DirectoryRead, sizeof(context_OS_DirectoryRead), false);

    UT_SetHandlerFunction(UT_KEY(OS_DirectoryClose), Handler_OS_DirectoryClose_ReportOnly, NULL);

    /* Act */
    CF_CFDP_ProcessPlaybackDirectory(arg_c, arg_p);

    /* Assert */
    UtAssert_STUB_COUNT(OS_DirectoryRead, 2);
    UtAssert_STUB_COUNT(OS_DirectoryClose, 1);
    UtAssert_True(arg_p->diropen == 0, "p->diropen is %d and should be 0", arg_p->diropen);
    UtAssert_True(arg_p->busy == 1, "p->busy is %d and should be 1", arg_p->busy);
} /* end
     Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0
   */

void Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDoubleDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0(
    void)
{
    /* Arrange */
    CF_Channel_t   dummy_c;
    CF_Channel_t  *arg_c = &dummy_c;
    CF_Playback_t  dummy_p;
    CF_Playback_t *arg_p                                      = &dummy_p;
    char           context_OS_DirectoryRead[OS_MAX_FILE_NAME] = "..\0";

    arg_p->diropen = 1;
    arg_p->num_ts  = 0;

    arg_p->busy = 1;

    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryRead), CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 2, Any_int32_Except(CFE_SUCCESS));

    UT_SetDataBuffer(UT_KEY(OS_DirectoryRead), context_OS_DirectoryRead, sizeof(context_OS_DirectoryRead), false);

    UT_SetHandlerFunction(UT_KEY(OS_DirectoryClose), Handler_OS_DirectoryClose_ReportOnly, NULL);

    /* Act */
    CF_CFDP_ProcessPlaybackDirectory(arg_c, arg_p);

    /* Assert */
    UtAssert_STUB_COUNT(OS_DirectoryRead, 2);
    UtAssert_STUB_COUNT(OS_DirectoryClose, 1);
    UtAssert_True(arg_p->diropen == 0, "p->diropen is %d and should be 0", arg_p->diropen);
    UtAssert_True(arg_p->busy == 0, "p->busy is %d and should be 0", arg_p->busy);
} /* end
     Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDoubleDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0
   */

void Test_CF_CFDP_ProcessPlaybackDirectory_AssertsBecause_CallTo_CF_CFDP_FindUnusedTransaction_Returns_NULL(void)
{
    // /* Arrange */
    // CF_Channel_t*        arg_c = &CF_AppData.engine.channels[Any_cf_chan_num()];
    // CF_Playback_t        dummy_p;
    // CF_Playback_t*       arg_p = &dummy_p;
    // const char        context_OS_DirectoryRead[OS_MAX_FILE_NAME];
    // CF_History_t         dummy_history;
    // CF_Transaction_t     dummy_pt;

    // AnyRandomStringOfLettersOfLengthCopy(context_OS_DirectoryRead, OS_MAX_FILE_NAME - 1);

    // arg_p->diropen = 1;
    // arg_p->num_ts = CF_NUM_TRANSACTIONS_PER_PLAYBACK - 1; /* run one loop */

    // arg_p->busy = 1;

    // UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryRead), CFE_SUCCESS);
    // UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 2, Any_int32_Except(CFE_SUCCESS));

    // UT_SetDataBuffer(UT_KEY(OS_DirectoryRead), context_OS_DirectoryRead,
    //   sizeof(context_OS_DirectoryRead), false);

    // UT_SetHandlerFunction(UT_KEY(OS_DirectoryClose), Handler_OS_DirectoryClose_ReportOnly, NULL);

    // dummy_pt.history = &dummy_history;

    // AnyRandomStringOfLettersOfLengthCopy(dummy_pt.history->fnames.src_filename, CF_FILENAME_MAX_LEN - 1);
    // AnyRandomStringOfLettersOfLengthCopy(dummy_pt.history->fnames.dst_filename, CF_FILENAME_MAX_LEN - 1);

    // /* Arrange for CF_CFDP_FindUnusedTransaction */
    // arg_c->qs[CF_QueueIdx_FREE] = NULL;

    // /* Act */
    // CF_CFDP_ProcessPlaybackDirectory(arg_c, arg_p);

    // /* Assert */
    // UtAssert_STUB_COUNT(OS_DirectoryRead, 1);
    // UtAssert_STUB_COUNT(OS_DirectoryClose, 0);
    // UtAssert_True(arg_p->diropen == 1,
    //   "p->diropen is %d and should be 1",
    //   arg_p->diropen);
    // UtAssert_True(arg_p->num_ts == CF_NUM_TRANSACTIONS_PER_PLAYBACK,
    //   "p->num_ts is %d and should be %u (CF_NUM_TRANSACTIONS_PER_PLAYBACK)",
    //   arg_p->num_ts, CF_NUM_TRANSACTIONS_PER_PLAYBACK);
    // UtAssert_True(arg_p->busy == 1,
    //   "p->busy is %d and should be 1",
    //   arg_p->busy);
    // /* Assert for CF_CFDP_FindUnusedTransaction */
    // UtAssert_STUB_COUNT(CF_CList_Remove, 2);
    // /* Assert for CF_CFDP_FindUnusedChunks */
    // UtAssert_STUB_COUNT(CF_CList_Pop, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - pt ");
} /* end Test_CF_CFDP_ProcessPlaybackDirectory_AssertsBecause_CallTo_CF_CFDP_FindUnusedTransaction_Returns_NULL */

void Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDirectoryDotsFailsThenCall_CF_CFDP_FindUnusedTransaction_And_CF_CFDP_TxFile__Increment_p_num_ts_ExitLoop(
    void)
{
    /* Arrange */
    CF_Channel_t    *arg_c = &CF_AppData.engine.channels[Any_cf_chan_num()];
    CF_Playback_t    dummy_p;
    CF_Playback_t   *arg_p = &dummy_p;
    char             context_OS_DirectoryRead[OS_MAX_FILE_NAME];
    CF_History_t     dummy_history;
    CF_Transaction_t dummy_pt;

    AnyRandomStringOfLettersOfLengthCopy(context_OS_DirectoryRead, OS_MAX_FILE_NAME - 1);

    arg_p->diropen = 1;
    arg_p->num_ts  = CF_NUM_TRANSACTIONS_PER_PLAYBACK - 1; /* run one loop */

    arg_p->busy = 1;

    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryRead), CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_DirectoryRead), 2, Any_int32_Except(CFE_SUCCESS));

    UT_SetDataBuffer(UT_KEY(OS_DirectoryRead), context_OS_DirectoryRead, sizeof(context_OS_DirectoryRead), false);

    UT_SetHandlerFunction(UT_KEY(OS_DirectoryClose), Handler_OS_DirectoryClose_ReportOnly, NULL);

    dummy_pt.history = &dummy_history;

    AnyRandomStringOfLettersOfLengthCopy(dummy_pt.history->fnames.src_filename, CF_FILENAME_MAX_LEN - 1);
    AnyRandomStringOfLettersOfLengthCopy(dummy_pt.history->fnames.dst_filename, CF_FILENAME_MAX_LEN - 1);

    /* Arrange for CF_CFDP_FindUnusedTransaction */
    arg_c->qs[CF_QueueIdx_FREE]      = &dummy_pt.cl_node;
    arg_c->qs[CF_QueueIdx_HIST]      = &dummy_pt.history->cl_node;
    arg_c->qs[CF_QueueIdx_HIST_FREE] = NULL; /* always chooses CF_QueueIdx_HIST */

    /* Arrange for CF_CList_Remove_Ex */
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_FREE] =
        1; /* avoid assert first call */
    CF_AppData.hk.channel_hk[arg_c - CF_AppData.engine.channels].q_size[CF_QueueIdx_HIST] =
        1; /* avoid assert second call */

    /* Arrange for CF_CFDP_TxFile_ */
    cf_config_table_t dummy_config_table;

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange for CF_CFDP_FindUnusedChunks */
    CF_CListNode_t dummy_cs;

    arg_c->cs[CF_Direction_TX] = &dummy_cs;

    /* Act */
    CF_CFDP_ProcessPlaybackDirectory(arg_c, arg_p);

    /* Assert */
    UtAssert_STUB_COUNT(OS_DirectoryRead, 1);
    UtAssert_STUB_COUNT(OS_DirectoryClose, 0);
    UtAssert_True(arg_p->diropen == 1, "p->diropen is %d and should be 1", arg_p->diropen);
    UtAssert_True(arg_p->num_ts == CF_NUM_TRANSACTIONS_PER_PLAYBACK,
                  "p->num_ts is %d and should be %u (CF_NUM_TRANSACTIONS_PER_PLAYBACK)", arg_p->num_ts,
                  CF_NUM_TRANSACTIONS_PER_PLAYBACK);
    UtAssert_True(arg_p->busy == 1, "p->busy is %d and should be 1", arg_p->busy);
    /* Assert for CF_CFDP_FindUnusedTransaction */
    UtAssert_STUB_COUNT(CF_CList_Remove, 2);
    /* Assert for CF_CFDP_FindUnusedChunks */
    UtAssert_STUB_COUNT(CF_CList_Pop, 1);
} /* end
     Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDirectoryDotsFailsThenCall_CF_CFDP_FindUnusedTransaction_And_CF_CFDP_TxFile__Increment_p_num_ts_ExitLoop
   */

/* end CF_CFDP_ProcessPlaybackDirectory tests */

/*******************************************************************************
**
**  CF_CFDP_UpdatePollPbCounted tests (small) - full coverage - 1 coverage JIRA issue - []
**
*******************************************************************************/

void Test_CF_CFDP_DoesNothing_pb_counted_IsEqTo_up(void)
{
    /* Arrange */
    CF_Playback_t  dummy_pb;
    CF_Playback_t *arg_pb = &dummy_pb;
    int            arg_up;
    uint8          dummy_counter   = Any_uint8();
    uint8         *arg_counter     = &dummy_counter;
    uint8          initial_counter = dummy_counter;

    arg_pb->counted = Any_uint8_LessThan(2);
    arg_up          = arg_pb->counted;

    /* Act */
    CF_CFDP_UpdatePollPbCounted(arg_pb, arg_up, arg_counter);

    /* Assert */
    UtAssert_True(*arg_counter == initial_counter,
                  "*counter is %u and should not have changed from %u (value before call)", *arg_counter,
                  initial_counter);
    UtAssert_True(arg_pb->counted == arg_up, "pb->counted is %u and should be %u (no change)", arg_pb->counted, arg_up);

} /* end Test_CF_CFDP_DoesNothing_pb_counted_IsEqTo_up */

void Test_CF_CFDP_UpdatePollPbCounted_IncrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_1(void)
{
    /* Arrange */
    CF_Playback_t  dummy_pb;
    CF_Playback_t *arg_pb              = &dummy_pb;
    int            arg_up              = 1; // TODO: assume 1 is up, 0 is down
    uint8          dummy_counter       = Any_uint8();
    uint8         *arg_counter         = &dummy_counter;
    uint8          initial_counter     = dummy_counter;
    uint8          expected_pb_counted = !!arg_up;

    arg_pb->counted = !arg_up;

    /* Act */
    CF_CFDP_UpdatePollPbCounted(arg_pb, arg_up, arg_counter);

    /* Assert */
    UtAssert_True(*arg_counter == (uint8)(initial_counter + 1),
                  "*counter is %u and should be 1 more than %u (value before call)", *arg_counter, initial_counter);
    UtAssert_True(arg_pb->counted == expected_pb_counted, "pb->counted is %u and should be %u (!!up)", arg_pb->counted,
                  expected_pb_counted);
} /* end Test_CF_CFDP_UpdatePollPbCounted_IncrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_1 */

void Test_CF_CFDP_UpdatePollPbCounted_AssertsBecause_pb_counted_IsNotEqTo_up_And_up_Is_0_AndValueAt_counter_Is_0(void)
{
    /* Arrange */
    // CF_Playback_t  dummy_pb;
    // CF_Playback_t* arg_pb = &dummy_pb;
    // int         arg_up = 0; //TODO: assume 1 is up, 0 is down
    // uint8       dummy_counter = 0;
    // uint8*      arg_counter = &dummy_counter;
    // uint8       initial_counter = dummy_counter;
    // uint8       expected_pb_counted = !!arg_up;

    // arg_pb->counted = !arg_up;

    /* Act */
    // CF_CFDP_UpdatePollPbCounted(arg_pb, arg_up, arg_counter);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - *counter");
} /* end Test_CF_CFDP_UpdatePollPbCounted_DecrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_0 */

void Test_CF_CFDP_UpdatePollPbCounted_DecrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_0(void)
{
    /* Arrange */
    CF_Playback_t  dummy_pb;
    CF_Playback_t *arg_pb              = &dummy_pb;
    int            arg_up              = 0; // TODO: assume 1 is up, 0 is down
    uint8          dummy_counter       = Any_uint8_Except(0);
    uint8         *arg_counter         = &dummy_counter;
    uint8          initial_counter     = dummy_counter;
    uint8          expected_pb_counted = !!arg_up;

    arg_pb->counted = !arg_up;

    /* Act */
    CF_CFDP_UpdatePollPbCounted(arg_pb, arg_up, arg_counter);

    /* Assert */
    UtAssert_True(*arg_counter == initial_counter - 1,
                  "*counter is %u and should be 1 less than %u (value before call)", *arg_counter, initial_counter);
    UtAssert_True(arg_pb->counted == expected_pb_counted, "pb->counted is %u and should be %u (!!up)", arg_pb->counted,
                  expected_pb_counted);
} /* end Test_CF_CFDP_UpdatePollPbCounted_DecrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_0 */

/* end CF_CFDP_UpdatePollPbCounted tests */

/*******************************************************************************
**
**  CF_CFDP_ProcessPlaybackDirectories tests (small) - full coverage
**
*******************************************************************************/

void Test_CF_CFDP_ProcessPlaybackDirectories_Calls_CF_CFDP_ProcessPlaybackDirectory_And_CF_CFDP_UpdatePollPbCounted_TheNumberOfTimesEqualTo_CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN(
    void)
{
    /* Arrange */
    uint8         dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int           i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].playback_counter = 0;

    /* Arrange for CF_CFDP_ProcessPlaybackDirectory */
    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        arg_c->playback[i].diropen = 1;                                    /* ensures enter while loop */
        arg_c->playback[i].num_ts  = CF_NUM_TRANSACTIONS_PER_PLAYBACK - 1; /* ensures enter while loop */

        /* Arrange for CF_CFDP_UpdatePollPbCounted */
        /* counted = 0 and busy = 1 ensures always up */
        arg_c->playback[i].counted = 0;
        arg_c->playback[i].busy    = 1;
    }

    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryRead),
                             Any_int32_Except(CFE_SUCCESS)); /* always fail for shortest route in unstubbable code */
    UT_SetHandlerFunction(UT_KEY(OS_DirectoryClose), Handler_OS_DirectoryClose_ReportOnly, NULL);

    /* Act */
    CF_CFDP_ProcessPlaybackDirectories(arg_c);

    /* Assert */
    /* Assert for CF_CFDP_ProcessPlaybackDirectory */
    UtAssert_STUB_COUNT(OS_DirectoryRead, CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN);
    UtAssert_STUB_COUNT(OS_DirectoryClose, CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN);
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(
        CF_AppData.hk.channel_hk[dummy_chan_index].playback_counter == CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN,
        "playback_counter is %u and should be %u (CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN) more than 0 (value "
        "before call)",
        CF_AppData.hk.channel_hk[dummy_chan_index].playback_counter, CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN);
} /* end
     Test_CF_CFDP_ProcessPlaybackDirectories_Calls_CF_CFDP_ProcessPlaybackDirectory_And_CF_CFDP_UpdatePollPbCounted_TheNumberOfTimesEqualTo_CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN
   */

/* end CF_CFDP_ProcessPlaybackDirectories tests */

/*******************************************************************************
**
**  CF_CFDP_ProcessPollingDirectories tests - (large) - full coverage - [unstubbables: CF_CFDP_PlaybackDir_,
*CF_CFDP_ProcessPlaybackDirectory, CF_CFDP_UpdatePollPbCounted]
**
*******************************************************************************/

void Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_enabled_Is_0(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = CF_MAX_POLLING_DIR_PER_CHAN;

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange for CF_CFDP_UpdatePollPbCounted */
    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled = 0;
        arg_c->poll[i].pb.counted                                          = 1; /* 1 ensures down every time */
    }

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == 0,
                  "poll_counter is %u and should be 0 (was CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter);
} /* end Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_enabled_Is_0 */

void Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_interval_sec_Is_0(void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = CF_MAX_POLLING_DIR_PER_CHAN;

    CF_AppData.config_table = &dummy_config_table;

    /* Arrange for CF_CFDP_UpdatePollPbCounted */
    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled      = 1;
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].interval_sec = 0;
        arg_c->poll[i].pb.counted                                               = 1; /* 1 ensures down every time */
    }

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == 0,
                  "poll_counter is %u and should be 0 (was CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter);
} /* end Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_interval_sec_Is_0 */

void Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_true_PlaybackIsActive_Calls_CF_CFDP_ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = 0;

    CF_AppData.config_table = &dummy_config_table;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        arg_c->poll[i].pb.busy = 1; /* busy of 1 ensures inner if fails */

        /* Arrange for CF_CFDP_ProcessPlaybackDirectory */
        arg_c->poll[i].pb.diropen = 0;
        arg_c->poll[i].pb.num_ts  = 0;

        /* Arrange for CF_CFDP_UpdatePollPbCounted */
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled      = 1;
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].interval_sec = 1;
        arg_c->poll[i].pb.counted                                               = 0; /* 0 ensures up every time */
    }

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    /* Assert for CF_CFDP_ProcessPlaybackDirectory */
    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        UtAssert_True(arg_c->poll[i].pb.busy == 0, "c->poll[%d].pb.busy is %u and should be 0 (was 1)", i,
                      arg_c->poll[i].pb.busy);
    }
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == CF_MAX_POLLING_DIR_PER_CHAN,
                  "poll_counter is %u and should be %u (CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter, CF_MAX_POLLING_DIR_PER_CHAN);
} /* end
     Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_true_PlaybackIsActive_Calls_CF_CFDP_ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted
   */

void Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_false_But_p_pb_num_ts_Is_true_PlaybackIsActive_Calls_CF_CFDP_ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = 0;

    CF_AppData.config_table = &dummy_config_table;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        arg_c->poll[i].pb.busy   = 0;
        arg_c->poll[i].pb.num_ts = 1; /* num_ts of 1 ensures inner if fails */

        /* Arrange for CF_CFDP_ProcessPlaybackDirectory */
        arg_c->poll[i].pb.diropen = 1;

        /* Arrange for CF_CFDP_UpdatePollPbCounted */
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled      = 1;
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].interval_sec = 1;
        arg_c->poll[i].pb.counted                                               = 0; /* 0 ensures up every time */
    }

    /* Arrange for CF_CFDP_ProcessPlaybackDirectory */
    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryRead),
                             Any_int32_Except(CFE_SUCCESS)); /* always fail for shortest route in unstubbable code */
    UT_SetHandlerFunction(UT_KEY(OS_DirectoryClose), Handler_OS_DirectoryClose_ReportOnly, NULL);

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    /* Assert for CF_CFDP_ProcessPlaybackDirectory */
    UtAssert_STUB_COUNT(OS_DirectoryRead, CF_MAX_POLLING_DIR_PER_CHAN);
    UtAssert_STUB_COUNT(OS_DirectoryClose, CF_MAX_POLLING_DIR_PER_CHAN);
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == CF_MAX_POLLING_DIR_PER_CHAN,
                  "poll_counter is %u and should be %u (CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter, CF_MAX_POLLING_DIR_PER_CHAN);
} /* end
     Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_false_But_p_pb_num_ts_Is_true_PlaybackIsActive_Calls_CF_CFDP_ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted
   */

void Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_1_And_CF_Timer_Expired_Returns_0_Calls_CF_Timer_Tick(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = 0;

    CF_AppData.config_table = &dummy_config_table;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        arg_c->poll[i].pb.busy   = 0;
        arg_c->poll[i].pb.num_ts = 0;
        arg_c->poll[i].timer_set = 1;

        /* Arrange for CF_CFDP_UpdatePollPbCounted */
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled      = 1;
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].interval_sec = 1;
        arg_c->poll[i].pb.counted                                               = 0; /* 0 ensures up every time */
    }

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired),
                             0); /* TODO: update CF_Timer_Expired stub to be the handler style stub */

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, CF_MAX_POLLING_DIR_PER_CHAN);
    UtAssert_STUB_COUNT(CF_Timer_Tick, CF_MAX_POLLING_DIR_PER_CHAN);
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == CF_MAX_POLLING_DIR_PER_CHAN,
                  "poll_counter is %u and should be %u (CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter, CF_MAX_POLLING_DIR_PER_CHAN);
} /* end Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_1_And_CF_Timer_Expired_Returns_0_Calls_CF_Timer_Tick
   */

void Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_PlaybackDir_Returns_1_EachTimeSetsAll_p_timer_set_To_0(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = 0;

    CF_AppData.config_table = &dummy_config_table;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        arg_c->poll[i].pb.busy   = 0;
        arg_c->poll[i].pb.num_ts = 0;
        arg_c->poll[i].timer_set = 1;

        /* Arrange for CF_CFDP_UpdatePollPbCounted */
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled      = 1;
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].interval_sec = 1;
        arg_c->poll[i].pb.counted                                               = 0; /* 0 ensures up every time */
    }

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired),
                             1); /* TODO: update CF_Timer_Expired stub to be the handler style stub */

    /* Arrange for CF_CFDP_PlaybackDir_ */
    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryOpen), OS_SUCCESS);

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, 0);
    UtAssert_STUB_COUNT(CF_Timer_Expired, CF_MAX_POLLING_DIR_PER_CHAN);
    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        UtAssert_True(arg_c->poll[i].timer_set == 0, "c->poll[%d].timer_set is %u and should be 0 (was 1)", i,
                      arg_c->poll[i].timer_set);
    }
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == CF_MAX_POLLING_DIR_PER_CHAN,
                  "poll_counter is %u and should be %u (CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter, CF_MAX_POLLING_DIR_PER_CHAN);
} /* end
     Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_PlaybackDir_Returns_1_EachTimeSetsAll_p_timer_set_To_0
   */

void Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_PlaybackDir_Returns_0_EachTimeCalls_CF_Timer_InitRelSec(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = 0;

    CF_AppData.config_table = &dummy_config_table;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        arg_c->poll[i].pb.busy   = 0;
        arg_c->poll[i].pb.num_ts = 0;
        arg_c->poll[i].timer_set = 1;

        /* Arrange for CF_CFDP_UpdatePollPbCounted */
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled      = 1;
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].interval_sec = 1;
        arg_c->poll[i].pb.counted                                               = 0; /* 0 ensures up every time */
    }

    UT_SetDefaultReturnValue(UT_KEY(CF_Timer_Expired),
                             1); /* TODO: update CF_Timer_Expired stub to be the handler style stub */

    /* Arrange for CF_CFDP_PlaybackDir_ */
    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryOpen), Any_int32_Except(OS_SUCCESS));

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_Expired, CF_MAX_POLLING_DIR_PER_CHAN);
    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        UtAssert_True(arg_c->poll[i].timer_set == 1, "c->poll[%d].timer_set is %u and should be 1 (was 0)", i,
                      arg_c->poll[i].timer_set);
    }
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, CF_MAX_POLLING_DIR_PER_CHAN);
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == CF_MAX_POLLING_DIR_PER_CHAN,
                  "poll_counter is %u and should be %u (CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter, CF_MAX_POLLING_DIR_PER_CHAN);
} /* end
     Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_PlaybackDir_Returns_0_EachTimeCalls_CF_Timer_InitRelSec
   */

void Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_Calls_CF_Timer_InitRelSec_And_Sets_p_timer_set_To_1(
    void)
{
    /* Arrange */
    cf_config_table_t dummy_config_table;
    uint8             dummy_chan_index = Any_cf_chan_num();
    CF_Channel_t     *arg_c            = &CF_AppData.engine.channels[dummy_chan_index];
    int               i                = 0;

    CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter = 0;

    CF_AppData.config_table = &dummy_config_table;

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        arg_c->poll[i].pb.busy   = 0;
        arg_c->poll[i].pb.num_ts = 0;
        arg_c->poll[i].timer_set = 0;

        /* Arrange for CF_CFDP_UpdatePollPbCounted */
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].enabled      = 1;
        CF_AppData.config_table->chan[dummy_chan_index].polldir[i].interval_sec = 1;
        arg_c->poll[i].pb.counted                                               = 0; /* 0 ensures up every time */
    }

    /* Act */
    CF_CFDP_ProcessPollingDirectories(arg_c);

    /* Assert */
    UtAssert_STUB_COUNT(CF_Timer_InitRelSec, CF_MAX_POLLING_DIR_PER_CHAN);
    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        UtAssert_True(arg_c->poll[i].timer_set == 1, "c->poll[%d].timer_set is %u and should be 1 (was 0)", i,
                      arg_c->poll[i].timer_set);
    }
    UtAssert_STUB_COUNT(CF_Timer_Expired, 0);
    /* Assert for CF_CFDP_UpdatePollPbCounted */
    UtAssert_True(CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter == CF_MAX_POLLING_DIR_PER_CHAN,
                  "poll_counter is %u and should be %u (CF_MAX_POLLING_DIR_PER_CHAN)",
                  CF_AppData.hk.channel_hk[dummy_chan_index].poll_counter, CF_MAX_POLLING_DIR_PER_CHAN);
} /* end
     Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_Calls_CF_Timer_InitRelSec_And_Sets_p_timer_set_To_1 */

/* end CF_CFDP_ProcessPollingDirectories tests */

/*******************************************************************************
**
**  CF_CFDP_CycleEngine tests (medium) - full coverage - [unstubbables: CF_CFDP_ReceiveMessage,
*CF_CFDP_TickTransactions, CF_CFDP_CycleTx, CF_CFDP_ProcessPlaybackDirectories, CF_CFDP_ProcessPollingDirectories]
**
*******************************************************************************/

void Test_CF_CFDP_CycleEngine_DoesNothingWhen_engine_Is_0(void)
{
    /* Arrange */
    CF_UT_inmsg_buffer_t dummy_msg;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.in.msg  = &dummy_msg.cfe_sb_buffer;
    CF_AppData.engine.enabled = 0;

    /* Act */
    CF_CFDP_CycleEngine();

    /* Assert */
    UtAssert_ADDRESS_EQ(CF_AppData.engine.in.msg, &dummy_msg);
} /* end Test_CF_CFDP_CycleEngine_DoesNothingWhen_engine_Is_0 */

void Test_CF_CFDP_CycleEngine_ConsumesAllMessagesWithAllChannelsFrozen(void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_UT_inmsg_buffer_t dummy_msg;
    int                  i = 0;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.in.msg  = &dummy_msg.cfe_sb_buffer;
    CF_AppData.engine.enabled = 1;
    CF_AppData.config_table   = &dummy_config_table;

    for (i = 0; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].frozen = 1;

        /* Arrange for CF_CFDP_ReceiveMessage */
        CF_AppData.config_table->chan[i].rx_max_messages_per_wakeup = 1;
    }

    /* Arrange for CF_CFDP_ReceiveMessage */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SB_NO_MESSAGE);

    /* Act */
    CF_CFDP_CycleEngine();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, CF_NUM_CHANNELS);
    /* Assert for CF_CFDP_ReceiveMessage */
    UtAssert_ADDRESS_EQ(CF_AppData.engine.in.msg, NULL);
} /* end Test_CF_CFDP_CycleEngine_ConsumesAllMessagesWithAllChannelsFrozen */

void Test_CF_CFDP_CycleEngine_WhenOneChannelNotFrozenCalls_CF_CFDP_TickTransactions_And_CF_CFDP_CycleTx_And_CF_CFDP_ProcessPlaybackDirectories_And_CF_CFDP_ProcessPollingDirectories(
    void)
{
    /* Arrange */
    cf_config_table_t    dummy_config_table;
    CF_UT_inmsg_buffer_t dummy_msg;
    int                  i = 0;

    memset(&dummy_msg, 0, sizeof(dummy_msg));

    CF_AppData.engine.in.msg  = &dummy_msg.cfe_sb_buffer;
    CF_AppData.engine.enabled = 1;
    CF_AppData.config_table   = &dummy_config_table;

    CF_AppData.hk.channel_hk[0].frozen                          = 0;
    CF_AppData.config_table->chan[i].rx_max_messages_per_wakeup = 0;

    for (i = 1; i < CF_NUM_CHANNELS; ++i)
    {
        CF_AppData.hk.channel_hk[i].frozen = 1;

        /* Arrange for CF_CFDP_ReceiveMessage */
        CF_AppData.config_table->chan[i].rx_max_messages_per_wakeup = 1;
    }

    /* Arrange for CF_CFDP_ReceiveMessage */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_ReceiveBuffer), CFE_SB_NO_MESSAGE);

    /* Arrange for CF_CFDP_TickTransactions */
    CF_Channel_t *dummy_c = &CF_AppData.engine.channels[0];

    dummy_c->tick_type                = CF_TickType_NUM_TYPES - 1;
    type_of_context_CF_CList_Traverse = POINTER;

    /* Arrange for CF_CFDP_CycleTx */
    CF_Transaction_t dummy_t;

    CF_AppData.config_table->chan[0].dequeue_enabled = 1;
    dummy_c->cur                                     = &dummy_t;

    /* Arrange for CF_CFDP_ProcessPlaybackDirectories, CF_CFDP_ProcessPlaybackDirectory, CF_CFDP_UpdatePollPbCounted */
    CF_Playback_t *dummy_p[CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN] = {0};

    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        dummy_p[i] = &dummy_c->playback[i];

        dummy_p[i]->diropen = 0; /* dummy_p[i]->diropen = 0 forces no while loop */
        dummy_p[i]->num_ts  = 0; /* dummy_p[i]->num_ts = 1 forces if block */
        dummy_p[i]->busy    = 1; /* dummy_p[i]->busy = 1 not required by helps verify going to 0 */
        CF_AppData.hk.channel_hk[i].playback_counter =
            10; /* 10 is arbitrary, but should keep assert from happening in CF_CFDP_UpdatePollPbCounted */
    }

    /* Arrange for CF_CFDP_ProcessPollingDirectories */

    for (i = 0; i < CF_MAX_POLLING_DIR_PER_CHAN; ++i)
    {
        CF_AppData.config_table->chan[0].polldir[i].enabled = 0; /* forces no run of if block */
        dummy_c->poll[i].pb.counted                         = 0;
    }

    /* Act */
    CF_CFDP_CycleEngine();

    /* Assert */
    UtAssert_STUB_COUNT(CFE_SB_ReceiveBuffer, 1);
    /* Assert for CF_CFDP_ReceiveMessage */
    UtAssert_ADDRESS_EQ(CF_AppData.engine.in.msg, NULL);
    /* Assert for CF_CFDP_TickTransactions */
    UtAssert_STUB_COUNT(CF_CList_Traverse, 1);
    /* Assert for CF_CFDP_CycleTx */
    UtAssert_NULL(CF_AppData.engine.channels[0].cur);
    /* Asssert for CF_CFDP_ProcessPlaybackDirectories */
    for (i = 0; i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++i)
    {
        UtAssert_True(dummy_p[i]->busy == 0, "c playback p[%u] is %u and should be 0", i, dummy_p[i]->busy);
    }
    /* Asssert for CF_CFDP_ProcessPollingDirectories - no discernable result to check*/

} /* end
     Test_CF_CFDP_CycleEngine_WhenOneChannelNotFrozenCalls_CF_CFDP_TickTransactions_And_CF_CFDP_CycleTx_And_CF_CFDP_ProcessPlaybackDirectories_And_CF_CFDP_ProcessPollingDirectories
   */

/* end CF_CFDP_CycleEngine tests */

/*******************************************************************************
**
**  CF_CFDP_ResetTransaction tests (very large) - full coverage - 4 coverage JIRA issues (1 causes a line and a branch
*problem) [unstubbables: CF_CFDP_IsSender, CF_CList_InsertBack_Ex, CF_CFDP_IsSender (yes twice),
*CF_CFDP_FreeTransaction]
**
*******************************************************************************/

void Test_CF_CFDP_ResetTransaction_AssertsBecause_t_chan_num_IsEqTo_CF_NUM_CHANNELS(void)
{
    // /* Arrange */
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t = &dummy_t;
    // int               arg_keep_history = Any_int();

    // arg_t->chan_num = CF_NUM_CHANNELS;

    // /* Act */
    // CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert: t->chan_num<CF_NUM_CHANNELS");
} /* end Test_CF_CFDP_ResetTransaction_AssertsBecause_t_chan_num_IsEqTo_CF_NUM_CHANNELS */

/* NOTE: Test_CF_CFDP_ResetTransaction_AssertsBecause_t_chan_num_IsGreaterThan_CF_NUM_CHANNELS not required but desired
 */
// void Test_CF_CFDP_ResetTransaction_AssertsBecause_t_chan_num_IsGreaterThan_CF_NUM_CHANNELS(void)
// {
//     /* Arrange */
//     CF_Transaction_t     dummy_t;
//     CF_Transaction_t*    arg_t = &dummy_t;
//     int               arg_keep_history;

//     arg_t->chan_num = Any_uint8_GreaterThan(CF_NUM_CHANNELS);

//     /* Act */
//     CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

//     /* Assert */
//     UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert: t->chan_num<CF_NUM_CHANNELS");
// } /* end Test_CF_CFDP_ResetTransaction_AssertsBecause_t_chan_num_IsGreaterThan_CF_NUM_CHANNELS */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t            = &dummy_t;
    int               arg_keep_history = 0;
    CF_QueueIdx_t     recorded_index[2];

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->keep = Any_uint8_Except(0);

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_RX;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = Any_uint32_Except(0); /* Any_uint32_Except(0) causes OS_ObjectIdDefined (non 0 != 0) to return true */

    /* Arrange for CF_CFDP_GetClass */
    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_STUB_COUNT(OS_remove, 0);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST_FREE,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST_FREE)",
                  recorded_index[0], CF_QueueIdx_HIST_FREE);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_0_And_CF_CFDP_IsSender_Returns_0_Call_OS_remove_With_dst_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t            = &dummy_t;
    int               arg_keep_history = 0;
    CF_QueueIdx_t     recorded_index[2];
    char             *expected_path;
    char             *recorded_path;

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->keep = 0;

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_RX;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    UT_SetHandlerFunction(UT_KEY(OS_remove), handler_OS_remove_Report_path, &recorded_path);
    expected_path = arg_t->history->fnames.dst_filename; /* either arg_t or history becomes NULL during CUT run, so save
                                                            the location that will was sent as argument to OS_remove */

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = Any_uint32_Except(0); /* Any_uint32_Except(0) causes OS_ObjectIdDefined (non 0 != 0) to return true */

    /* Arrange for CF_CFDP_IsSender */
    arg_t->flags.com.q_index = CF_QueueIdx_RX; /* bypass CF_Assert */

    arg_t->state = CF_TxnState_R1; /* arg_t->state = CF_TxnState_R1 forces 0 return */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_STUB_COUNT(OS_remove, 1);
    UtAssert_ADDRESS_EQ(recorded_path, expected_path);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST_FREE,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST_FREE)",
                  recorded_index[0], CF_QueueIdx_HIST_FREE);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_0_And_CF_CFDP_IsSender_Returns_0_Call_OS_remove_With_dst_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_And_CF_CFDP_IsSender_Returns_non0_Call_OS_remove_With_src_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t            = &dummy_t;
    int               arg_keep_history = 0;
    CF_QueueIdx_t     recorded_index[2];
    char             *expected_path;
    char             *recorded_path;

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->keep = 0;

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_RX;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    UT_SetHandlerFunction(UT_KEY(OS_remove), handler_OS_remove_Report_path, &recorded_path);
    expected_path = arg_t->history->fnames.src_filename; /* either arg_t or history becomes NULL during CUT run, so save
                                                            the location that will was sent as argument to OS_remove */

    CF_AppData.engine.channels[arg_t->chan_num].cur = arg_t;

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = Any_uint32_Except(0); /* Any_uint32_Except(0) causes OS_ObjectIdDefined (non 0 != 0) to return true */

    /* Arrange for CF_CFDP_IsSender */
    arg_t->flags.com.q_index = CF_QueueIdx_RX; /* bypass CF_Assert */

    arg_t->state = CF_TxnState_S1; /* arg_t->state = CF_TxnState_S1 forces non 0 return */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_STUB_COUNT(OS_remove, 1);
    UtAssert_ADDRESS_EQ(recorded_path, expected_path);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST_FREE,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST_FREE)",
                  recorded_index[0], CF_QueueIdx_HIST_FREE);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_NULL(CF_AppData.engine.channels[arg_t->chan_num].cur);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_And_CF_CFDP_IsSender_Returns_non0_Call_OS_remove_With_src_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t            = &dummy_t;
    int               arg_keep_history = 0;
    CF_QueueIdx_t     recorded_index[2];

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_RX;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    /* Arrange for CF_CFDP_GetClass */
    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST_FREE,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST_FREE)",
                  recorded_index[0], CF_QueueIdx_HIST_FREE);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t            = &dummy_t;
    int               arg_keep_history = Any_int_Except(0);
    CF_QueueIdx_t     recorded_index[2];

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_RX;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    /* Arrange for CF_CFDP_GetClass */
    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST)",
                  recorded_index[0], CF_QueueIdx_HIST);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_AndBoth_t_flags_tx_cmd_tx_And_t_p_Are_0_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t            = &dummy_t;
    int               arg_keep_history = Any_int_Except(0);
    CF_QueueIdx_t     recorded_index[2];

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_TX;

    arg_t->flags.tx.cmd_tx = 0;

    arg_t->p = NULL;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    /* Arrange for CF_CFDP_GetClass */
    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST)",
                  recorded_index[0], CF_QueueIdx_HIST);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_AndBoth_t_flags_tx_cmd_tx_And_t_p_Are_0_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_AssertsBecause_c_num_cmd_tx_Is_0(void)
{
    // /* Arrange */
    // CF_History_t         dummy_history;
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t = &dummy_t;
    // int               arg_keep_history = Any_int_Except(0);
    // CF_QueueIdx_t  recorded_index[2];

    // arg_t->chan_num = Any_cf_chan_num();

    // arg_t->history = &dummy_history;
    // arg_t->history->dir = CF_Direction_TX;

    // arg_t->flags.tx.cmd_tx = Any_uint32_Except(0); /* TODO make Any_unsigned_int_Except() */

    // (&CF_AppData.engine.channels[arg_t->chan_num])->num_cmd_tx = 0;

    // arg_t->p = NULL;

    // UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
    // &recorded_index);

    // /* Arrange for OS_ObjectIdDefined */
    // arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    // /* Arrange for CF_CFDP_GetClass */
    // arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
    // ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    // /* Act */
    // CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    // /* Assert */
    // UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    // UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    // UtAssert_UINT32_EQ((&CF_AppData.engine.channels[arg_t->chan_num])->num_cmd_tx, initial_num_cmd_tx - 1);
    // UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    // UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST,
    //   "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST)",
    //   recorded_index[0], CF_QueueIdx_HIST);
    // UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - c->num_cmd_tx");
} /* end Test_CF_CFDP_ResetTransaction_AssertsBecause_c_num_cmd_tx_Is_0 */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_flags_tx_cmd_tx_Is_1_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t              = &dummy_t;
    uint32            initial_num_cmd_tx = Any_uint32_Except(0);
    int               arg_keep_history   = Any_int_Except(0);
    CF_QueueIdx_t     recorded_index[2];

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_TX;

    arg_t->flags.tx.cmd_tx = 1;

    (&CF_AppData.engine.channels[arg_t->chan_num])->num_cmd_tx = initial_num_cmd_tx;

    arg_t->p = NULL;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    /* Arrange for CF_CFDP_GetClass */
    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    UtAssert_UINT32_EQ((&CF_AppData.engine.channels[arg_t->chan_num])->num_cmd_tx, initial_num_cmd_tx - 1);
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST)",
                  recorded_index[0], CF_QueueIdx_HIST);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_flags_tx_cmd_tx_Is_1_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_AssertsBecause_t_p_num_ts_Is_0(void)
{
    // /* Arrange */
    // CF_History_t         dummy_history;
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t = &dummy_t;
    // uint32            initial_num_cmd_tx = Any_uint32_Except(0);
    // int               arg_keep_history = Any_int_Except(0);
    // CF_QueueIdx_t  recorded_index[2];

    // arg_t->chan_num = Any_cf_chan_num();

    // arg_t->history = &dummy_history;
    // arg_t->history->dir = CF_Direction_TX;

    // arg_t->flags.tx.cmd_tx = 1;

    // (&CF_AppData.engine.channels[arg_t->chan_num])->num_cmd_tx = initial_num_cmd_tx;

    // arg_t->p = NULL;

    // UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
    // &recorded_index);

    // /* Arrange for OS_ObjectIdDefined */
    // arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    // /* Arrange for CF_CFDP_GetClass */
    // arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
    // ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    // /* Act */
    // CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    // /* Assert */
    // UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    // UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    // UtAssert_UINT32_EQ((&CF_AppData.engine.channels[arg_t->chan_num])->num_cmd_tx, initial_num_cmd_tx - 1);
    // UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    // UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST,
    //   "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST)",
    //   recorded_index[0], CF_QueueIdx_HIST);
    // UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert: t->p->num_ts");
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_flags_tx_cmd_tx_Is_1_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_p_Is_nonNULL_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction(
    void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_Playback_t     dummy_p;
    uint16            initial_num_ts   = Any_uint16_Except(0);
    int               arg_keep_history = Any_int_Except(0);
    CF_QueueIdx_t     recorded_index[2];

    arg_t->chan_num = Any_cf_chan_num();

    arg_t->history      = &dummy_history;
    arg_t->history->dir = CF_Direction_TX;

    arg_t->flags.tx.cmd_tx = 0;

    dummy_p.num_ts = initial_num_ts;
    arg_t->p       = &dummy_p;

    UT_SetHandlerFunction(UT_KEY(CF_CList_InsertBack_Ex), handler_CF_CList_InsertBack_Ex_Record_indexes,
                          &recorded_index);

    /* Arrange for OS_ObjectIdDefined */
    arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    /* Arrange for CF_CFDP_GetClass */
    arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
                                                    ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    /* Act */
    CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    /* Assert */
    UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    UtAssert_UINT32_EQ(dummy_p.num_ts,
                       initial_num_ts -
                           1); /* dummy_p is used in UtAssert_UINT32_EQ(dummy_p.num_ts, initial_num_ts - 1) because t
                                  loses it's pointer to p in unstubbable CF_CFDP_FreeTransaction */
    UtAssert_STUB_COUNT(CF_CList_InsertBack_Ex, 2); /* Includes one call in CF_CFDP_FreeTransaction */
    UtAssert_True(recorded_index[0] == CF_QueueIdx_HIST,
                  "index sent to first call of CF_CList_InsertBack_Ex was %u and should be %u (CF_QueueIdx_HIST)",
                  recorded_index[0], CF_QueueIdx_HIST);
    UtAssert_STUB_COUNT(CF_CList_InsertBack, 1);
} /* end
     Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_p_Is_nonNULL_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction
   */

void Test_CF_CFDP_ResetTransaction_AssertsBecause_t_history_dir_IsNeither_CF_DIR_TX_or_CF_DIR_RX(void)
{
    // /* Arrange */
    // CF_History_t         dummy_history;
    // CF_Transaction_t     dummy_t;
    // CF_Transaction_t*    arg_t  = &dummy_t;
    // CF_Playback_t        dummy_p;
    // uint16            initial_num_ts = Any_uint16_Except(0);
    // int               arg_keep_history = Any_int_Except(0);
    // uint8             exceptions[2] = {CF_Direction_RX, CF_Direction_TX};
    // CF_QueueIdx_t  recorded_index[2];

    // arg_t->chan_num = Any_cf_chan_num();

    // arg_t->history = &dummy_history;
    // arg_t->history->dir = Any_uint8_ExceptThese(exceptions, 2);

    // /* Arrange for OS_ObjectIdDefined */
    // arg_t->fd = 0; /* arg_t->fd = 0 causes OS_ObjectIdDefined (0 != 0) to return false */

    // /* Arrange for CF_CFDP_GetClass */
    // arg_t->flags.com.q_index = CF_QueueIdx_PEND; /* arg_t->flags.com.q_index = CF_QueueIdx_PEND ensures
    // ti->flags.com.q_index!=CF_QueueIdx_FREE is never false */

    // /* Act */
    // CF_CFDP_ResetTransaction(arg_t, arg_keep_history);

    // /* Assert */
    // UtAssert_STUB_COUNT(cf_dequeue_transaction, 1);
    // UtAssert_STUB_COUNT(CF_WrappedClose, 0);
    UtAssert_MIR(
        "JIRA: GSFCCFS-1733 CF_Assert - 0 (no not a typo, literally 0 is this one, comes from a default switch)");
} /* end Test_CF_CFDP_ResetTransaction_AssertsBecause_t_history_dir_IsNeither_CF_DIR_TX_or_CF_DIR_RX */

/* end CF_CFDP_ResetTransaction tests */

/*******************************************************************************
**
**  CF_CFDP_CopyDataToLv tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsGreaterThan_sizeof_dest_lv_data_Returns_neg1(void)
{
    /* Arrange */
    CF_CFDP_lv_t  dummy_dest_lv;
    CF_CFDP_lv_t *arg_dest_lv = &dummy_dest_lv;
    uint8         arg_data[sizeof(arg_dest_lv->data)];
    uint32        arg_len = Any_uint32_GreaterThan(sizeof(arg_dest_lv->data));
    int           local_result;

    memset(arg_data, 0xEE, sizeof(arg_data));

    /* Act */
    local_result = CF_CFDP_CopyDataToLv(arg_dest_lv, arg_data, arg_len);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_CopyDataToLv returned %d and should be -1 (fail)", local_result);

} /* end Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsGreaterThan_sizeof_dest_lv_data_Returns_neg1 */

void Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsEqTo_sizeof_dest_lv_data_Returns_neg1(void)
{
    /* Arrange */
    CF_CFDP_lv_t  dummy_dest_lv;
    CF_CFDP_lv_t *arg_dest_lv = &dummy_dest_lv;
    uint8         arg_data[sizeof(arg_dest_lv->data)];
    uint32        arg_len = sizeof(arg_dest_lv->data);
    int           local_result;

    memset(arg_data, 0xEE, sizeof(arg_data));

    /* Act */
    local_result = CF_CFDP_CopyDataToLv(arg_dest_lv, arg_data, arg_len);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_CopyDataToLv returned %d and should be -1 (fail)", local_result);

} /* end Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsGreaterThan_sizeof_dest_lv_data_Returns_neg1 */

void Test_CF_CFDP_CopyDataToLv_Success_len_LessThan_sizeof_dest_lv_data_Copies_data_To_dest_lv_data(void)
{
    /* Arrange */
    CF_CFDP_lv_t  dummy_dest_lv;
    CF_CFDP_lv_t *arg_dest_lv = &dummy_dest_lv;
    uint8         arg_data[sizeof(arg_dest_lv->data)];
    uint32        arg_len = Any_uint32_LessThan(sizeof(arg_dest_lv->data));
    int           local_result;

    AnyBufferOf_uint8_WithSize(arg_data, arg_len);

    /* Act */
    local_result = CF_CFDP_CopyDataToLv(arg_dest_lv, arg_data, arg_len);

    /* Assert */
    UtAssert_True(local_result == arg_len + 1, "CF_CFDP_CopyDataToLv returned %d and should be 1 more than %u (len)",
                  local_result, arg_len);
    UtAssert_MemCmp(arg_dest_lv->data, arg_data, arg_len, "CF_CFDP_CopyDataToLv copied data to dest_lv->data");

} /* end Test_CF_CFDP_CopyDataToLv_Success_len_LessThan_sizeof_dest_lv_data_Copies_data_To_dest_lv_data */

/* end CF_CFDP_CopyDataToLv tests */

/*******************************************************************************
**
**  CF_CFDP_CopyDataFromLv tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsGreaterThan_sizeof_src_lv_data_Returns_neg1(void)
{
    /* Arrange */
    CF_CFDP_lv_t  dummy_src_lv;
    CF_CFDP_lv_t *arg_src_lv = &dummy_src_lv;
    uint8         arg_buf[CF_FILENAME_MAX_LEN];
    // uint32      arg_src_len = Any_uint32_GreaterThan(sizeof(arg_dest_lv->data));
    int local_result;

    arg_src_lv->length = Any_uint8_GreaterThan(sizeof(arg_src_lv->data));

    /* Act */
    local_result = CF_CFDP_CopyDataFromLv(arg_buf, arg_src_lv);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_CopyDataFromLv returned %d and should be -1 (fail)", local_result);

} /* end Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsGreaterThan_sizeof_dest_lv_data_Returns_neg1 */

void Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsEqTo_sizeof_src_lv_data_Returns_neg1(void)
{
    /* Arrange */
    CF_CFDP_lv_t  dummy_src_lv;
    CF_CFDP_lv_t *arg_src_lv = &dummy_src_lv;
    uint8         arg_buf[CF_FILENAME_MAX_LEN];
    // uint32      arg_src_len = Any_uint32_GreaterThan(sizeof(arg_dest_lv->data));
    int local_result;

    arg_src_lv->length = sizeof(arg_src_lv->data);

    /* Act */
    local_result = CF_CFDP_CopyDataFromLv(arg_buf, arg_src_lv);

    /* Assert */
    UtAssert_True(local_result == -1, "CF_CFDP_CopyDataFromLv returned %d and should be -1 (fail)", local_result);

} /* end Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsEqTo_sizeof_src_lv_data_Returns_neg1 */

void Test_CF_CFDP_CF_CFDP_CopyDataFromLv_Success_src_lv_len_LessThan_sizeof_src_lv_data_Copies_src_lv_data_To_buf(void)
{
    /* Arrange */
    CF_CFDP_lv_t  dummy_src_lv;
    CF_CFDP_lv_t *arg_src_lv = &dummy_src_lv;
    uint8         arg_buf[CF_FILENAME_MAX_LEN];
    // uint32      arg_src_len = Any_uint32_GreaterThan(sizeof(arg_dest_lv->data));
    int local_result;

    arg_src_lv->length = Any_uint8_LessThan(sizeof(arg_src_lv->data));

    AnyBufferOf_uint8_WithSize(arg_src_lv->data, arg_src_lv->length);

    /* Act */
    local_result = CF_CFDP_CopyDataFromLv(arg_buf, arg_src_lv);

    /* Assert */
    UtAssert_True(local_result == arg_src_lv->length,
                  "CF_CFDP_CopyDataFromLv returned %d and should be %u (src_lv->length)", local_result,
                  arg_src_lv->length);
    UtAssert_MemCmp(arg_buf, arg_src_lv->data, arg_src_lv->length,
                    "CF_CFDP_CopyDataFromLv copied data to buf from src_lv_data");

} /* end Test_CF_CFDP_CF_CFDP_CopyDataFromLv_Success_src_lv_len_LessThan_sizeof_src_lv_data_Copies_src_lv_data_To_buf */

/* end CF_CFDP_CopyDataFromLv tests */

/*******************************************************************************
**
**  CF_CFDP_CancelTransaction tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_CancelTransaction_DoesNothingBecause_flags_all_cancelled_IsAlreadyTrue(void)
{
    /* Arrange */
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_Transaction_t *context_CF_CFDP_R_Cancel;
    CF_Transaction_t *context_CF_CFDP_S_Cancel;

    arg_t->flags.com.canceled = 1; /* 1 = true */

    UT_SetDataBuffer(UT_KEY(CF_CFDP_R_Cancel), &context_CF_CFDP_R_Cancel, sizeof(context_CF_CFDP_R_Cancel), false);
    UT_SetDataBuffer(UT_KEY(CF_CFDP_S_Cancel), &context_CF_CFDP_S_Cancel, sizeof(context_CF_CFDP_S_Cancel), false);

    /* Act */
    CF_CFDP_CancelTransaction(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_R_Cancel, 0);
    UtAssert_STUB_COUNT(CF_CFDP_S_Cancel, 0);
    UtAssert_True(arg_t->flags.com.canceled == 1, "t->flags.com.canceled is %d and should not have changed from 1",
                  arg_t->flags.com.canceled);

} /* end Test_CF_CFDP_CancelTransaction_DoesNothingBecause_flags_all_cancelled_IsAlreadyTrue */

void Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsSender(void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_Transaction_t *context_CF_CFDP_S_Cancel;

    arg_t->flags.com.canceled = 0; /* 0 = false */
    arg_t->history            = &dummy_history;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_S_Cancel), &context_CF_CFDP_S_Cancel, sizeof(context_CF_CFDP_S_Cancel), false);

    /* Arrange unstubbable: CF_CFDP_IsSender */
    uint8 send_states[2] = {CF_TxnState_S1, CF_TxnState_S2};

    arg_t->flags.com.q_index = Any_uint8_LessThan(CF_QueueIdx_FREE); /* removes CF_QueueIdx_NUM by design */
    arg_t->state             = Any_uint8_FromThese(send_states, sizeof(send_states));

    /* Act */
    CF_CFDP_CancelTransaction(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_R_Cancel, 0);
    UtAssert_STUB_COUNT(CF_CFDP_S_Cancel, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_S_Cancel, arg_t);
    UtAssert_True(arg_t->flags.com.canceled == 1, "t->flags.com.canceled is %d and should not have changed from 1",
                  arg_t->flags.com.canceled);

} /* end Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsSender */

void Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsNotSender(void)
{
    /* Arrange */
    CF_History_t      dummy_history;
    CF_Transaction_t  dummy_t;
    CF_Transaction_t *arg_t = &dummy_t;
    CF_Transaction_t *context_CF_CFDP_R_Cancel;

    arg_t->flags.com.canceled = 0; /* 0 = false */
    arg_t->history            = &dummy_history;

    UT_SetDataBuffer(UT_KEY(CF_CFDP_R_Cancel), &context_CF_CFDP_R_Cancel, sizeof(context_CF_CFDP_R_Cancel), false);

    /* Arrange unstubbable: CF_CFDP_IsSender */
    uint8 non_send_states[2] = {CF_TxnState_S1, CF_TxnState_S2};

    arg_t->flags.com.q_index = Any_uint8_LessThan(CF_QueueIdx_FREE); /* removes CF_QueueIdx_NUM by design */
    arg_t->state             = Any_uint8_ExceptThese(non_send_states, sizeof(non_send_states));

    /* Act */
    CF_CFDP_CancelTransaction(arg_t);

    /* Assert */
    UtAssert_STUB_COUNT(CF_CFDP_R_Cancel, 1);
    UtAssert_ADDRESS_EQ(context_CF_CFDP_R_Cancel, arg_t);
    UtAssert_STUB_COUNT(CF_CFDP_S_Cancel, 0);
    UtAssert_True(arg_t->flags.com.canceled == 1, "t->flags.com.canceled is %d and should not have changed from 1",
                  arg_t->flags.com.canceled);

} /* end Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsNotSender */

/* end CF_CFDP_CancelTransaction tests */

/*******************************************************************************
**
**  CF_CFDP_CloseFiles tests (small) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_CloseFiles_DoesNothingBecause_t_fd_Is_0_Returns_CLIST_CONT(void)
{
    /* Arrange */
    CF_Transaction_t dummy_t;
    CF_CListNode_t  *arg_n       = &dummy_t.cl_node;
    void            *arg_context = NULL;
    int              local_result;

    dummy_t.fd = 0;

    /* Act */
    local_result = CF_CFDP_CloseFiles(arg_n, arg_context);

    /* Assert */
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_CFDP_CloseFiles returned %u and should be %u (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);
    UtAssert_STUB_COUNT(CF_WrappedClose, 0);

} /* end Test_CF_CFDP_CloseFiles_DoesNothingBecause_t_fd_Is_0_Returns_CLIST_CONT */

void Test_CF_CFDP_CloseFiles_Calls_CF_WrappedClose_Because_t_fd_Is_not0_Returns_CLIST_CONT(void)
{
    /* Arrange */
    CF_Transaction_t dummy_t;
    CF_CListNode_t  *arg_n       = &dummy_t.cl_node;
    void            *arg_context = NULL;
    int              local_result;
    int32            context_CF_CFDP_CloseFiles;

    dummy_t.fd = Any_int32_Except(0);

    UT_SetDataBuffer(UT_KEY(CF_WrappedClose), &context_CF_CFDP_CloseFiles, sizeof(context_CF_CFDP_CloseFiles), false);

    /* Act */
    local_result = CF_CFDP_CloseFiles(arg_n, arg_context);

    /* Assert */
    UtAssert_True(local_result == CF_CLIST_CONT, "CF_CFDP_CloseFiles returned %u and should be %u (CF_CLIST_CONT)",
                  local_result, CF_CLIST_CONT);
    UtAssert_STUB_COUNT(CF_WrappedClose, 1);
    UtAssert_True(context_CF_CFDP_CloseFiles == dummy_t.fd, "CF_WrappedClose received fd %d should be %d (t->fd)",
                  context_CF_CFDP_CloseFiles, dummy_t.fd);

} /* end Test_CF_CFDP_CloseFiles_DoesNothingBecause_t_fd_Is_0_Returns_CLIST_CONT */

/* end CF_CFDP_CloseFiles tests */

/*******************************************************************************
**
**  CF_CFDP_DisableEngine tests (large) - full coverage - []
**
*******************************************************************************/

void Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndNoOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe(void)
{
    /* Arrange */
    CF_Channel_t                           *dummy_channel[CF_NUM_CHANNELS];
    uint8                                   ci                     = 0;
    uint8                                   qs_index[]             = {CF_QueueIdx_RX, CF_QueueIdx_TXA, CF_QueueIdx_TXW};
    uint8                                   num_clist_node_ptrs    = sizeof(qs_index) / sizeof(*qs_index);
    uint8                                   qi                     = 0;
    uint8                                   num_playback_dirs_busy = 0;
    uint8                                   playback_i             = 0;
    uint8                                   num_poll_dirs_busy     = 0;
    uint8                                   poll_i                 = 0;
    CF_CList_Traverse_CLOSE_FILES_context_t context_CF_CList_Traverse[CF_NUM_CHANNELS * num_clist_node_ptrs];

    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        dummy_channel[ci]       = CF_AppData.engine.channels + ci;
        dummy_channel[ci]->pipe = CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(1 + ci));
        for (qi = 0; qi < num_clist_node_ptrs; ++qi)
        {
            (CF_AppData.engine.channels + ci)->qs[qs_index[qi]] = malloc(sizeof(CF_CListNode_t *));
        }
    }

    /* set random busy playback dirents */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (playback_i = 0; playback_i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++playback_i)
        {
            dummy_channel[ci]->playback[playback_i].busy = 0;
        }
    }

    /* set random busy polling dirents */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (poll_i = 0; poll_i < CF_MAX_POLLING_DIR_PER_CHAN; ++poll_i)
        {
            dummy_channel[ci]->poll[poll_i].pb.busy = 0;
        }
    }

    type_of_context_CF_CList_Traverse = CLOSE_FILES;
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    /* TODO: must set a fail (function does not care) to avoid stub implementation yelling about deleting uncreated pipe
     * - this should not be required! */
    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryClose), -1);

    UT_SetDefaultReturnValue(
        UT_KEY(CFE_SB_DeletePipe),
        CFE_SUCCESS); /* TODO: CFE_SB_DeletePipe stub only saves argument value on success, this function "doesn't care"
                         about the result and should be able to save it no matter the result. */

    /* Act */
    CF_CFDP_DisableEngine();

    /* Assert */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        int node_i = 0;

        for (node_i = 0; node_i < num_clist_node_ptrs; ++node_i)
        {
            UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].start,
                                dummy_channel[ci]->qs[qs_index[node_i]]);
            UtAssert_True(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].fn == CF_CFDP_CloseFiles,
                          "context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].fn ==  CF_CFDP_CloseFiles");
            UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].context, NULL);
        }

        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[ci].q_size, 0, sizeof(CF_AppData.hk.channel_hk[ci].q_size),
                             "All of CF_AppData.hk.channel_hk[%u].q_size[] values set to 0", ci);
    }

    /* TODO: OS_DirectoryClose count check does not fully test what is happening here
     * the stub does not save the dir_id for the test to check, but that should be checked at this level */
    UtAssert_STUB_COUNT(OS_DirectoryClose, num_playback_dirs_busy + num_poll_dirs_busy);
    UtAssert_STUB_COUNT(CFE_SB_DeletePipe, 2);

    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (qi = 0; qi < num_clist_node_ptrs; ++qi)
        {
            free((CF_AppData.engine.channels + ci)->qs[qs_index[qi]]);
        }
    }

} /* end Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndNoOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe */

void Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndAnyOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe(void)
{
    /* Arrange */
    CF_Channel_t                           *dummy_channel[CF_NUM_CHANNELS];
    uint8                                   ci                     = 0;
    uint8                                   qs_index[]             = {CF_QueueIdx_RX, CF_QueueIdx_TXA, CF_QueueIdx_TXW};
    uint8                                   num_clist_node_ptrs    = sizeof(qs_index) / sizeof(*qs_index);
    uint8                                   qi                     = 0;
    uint8                                   num_playback_dirs_busy = 0;
    uint8                                   playback_i             = 0;
    uint8                                   num_poll_dirs_busy     = 0;
    uint8                                   poll_i                 = 0;
    CF_CList_Traverse_CLOSE_FILES_context_t context_CF_CList_Traverse[CF_NUM_CHANNELS * num_clist_node_ptrs];

    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        dummy_channel[ci]       = CF_AppData.engine.channels + ci;
        dummy_channel[ci]->pipe = CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(1 + ci));
        for (qi = 0; qi < num_clist_node_ptrs; ++qi)
        {
            (CF_AppData.engine.channels + ci)->qs[qs_index[qi]] = malloc(sizeof(CF_CListNode_t *));
        }
    }

    /* set random busy playback dirents */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (playback_i = 0; playback_i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++playback_i)
        {
            if (Any_bool())
            {
                dummy_channel[ci]->playback[playback_i].busy = 1;
                ++num_playback_dirs_busy;
            }
            else
            {
                dummy_channel[ci]->playback[playback_i].busy = 0;
            }
        }
    }

    /* set random busy polling dirents */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (poll_i = 0; poll_i < CF_MAX_POLLING_DIR_PER_CHAN; ++poll_i)
        {
            if (Any_bool())
            {
                dummy_channel[ci]->poll[poll_i].pb.busy = 1;
                ++num_poll_dirs_busy;
            }
            else
            {
                dummy_channel[ci]->poll[poll_i].pb.busy = 0;
            }
        }
    }

    type_of_context_CF_CList_Traverse = CLOSE_FILES;
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    /* TODO: must set a fail (function does not care) to avoid stub implementation yelling about deleting uncreated pipe
     * - this should not be required! */
    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryClose), -1);

    UT_SetDefaultReturnValue(
        UT_KEY(CFE_SB_DeletePipe),
        CFE_SUCCESS); /* TODO: CFE_SB_DeletePipe stub only saves argument value on success, this function "doesn't care"
                         about the result and should be able to save it no matter the result. */

    /* Act */
    CF_CFDP_DisableEngine();

    /* Assert */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        int node_i = 0;

        for (node_i = 0; node_i < num_clist_node_ptrs; ++node_i)
        {
            UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].start,
                                dummy_channel[ci]->qs[qs_index[node_i]]);
            UtAssert_True(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].fn == CF_CFDP_CloseFiles,
                          "context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].fn ==  CF_CFDP_CloseFiles");
            UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].context, NULL);
        }

        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[ci].q_size, 0, sizeof(CF_AppData.hk.channel_hk[ci].q_size),
                             "All of CF_AppData.hk.channel_hk[%u].q_size[] values set to 0", ci);
    }

    /* TODO: OS_DirectoryClose count check does not fully test what is happening here
     * the stub does not save the dir_id for the test to check, but that should be checked at this level */
    UtAssert_STUB_COUNT(OS_DirectoryClose, num_playback_dirs_busy + num_poll_dirs_busy);
    UtAssert_STUB_COUNT(CFE_SB_DeletePipe, 2);

    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (qi = 0; qi < num_clist_node_ptrs; ++qi)
        {
            free((CF_AppData.engine.channels + ci)->qs[qs_index[qi]]);
        }
    }

} /* end Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndAnyOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe
   */

void Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndAllOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe(void)
{
    /* Arrange */
    CF_Channel_t                           *dummy_channel[CF_NUM_CHANNELS];
    uint8                                   ci                     = 0;
    uint8                                   qs_index[]             = {CF_QueueIdx_RX, CF_QueueIdx_TXA, CF_QueueIdx_TXW};
    uint8                                   num_clist_node_ptrs    = sizeof(qs_index) / sizeof(*qs_index);
    uint8                                   qi                     = 0;
    uint8                                   num_playback_dirs_busy = 0;
    uint8                                   playback_i             = 0;
    uint8                                   num_poll_dirs_busy     = 0;
    uint8                                   poll_i                 = 0;
    CF_CList_Traverse_CLOSE_FILES_context_t context_CF_CList_Traverse[CF_NUM_CHANNELS * num_clist_node_ptrs];

    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        dummy_channel[ci]       = CF_AppData.engine.channels + ci;
        dummy_channel[ci]->pipe = CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(1 + ci));
        for (qi = 0; qi < num_clist_node_ptrs; ++qi)
        {
            (CF_AppData.engine.channels + ci)->qs[qs_index[qi]] = malloc(sizeof(CF_CListNode_t *));
        }
    }

    /* set random busy playback dirents */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (playback_i = 0; playback_i < CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN; ++playback_i)
        {
            dummy_channel[ci]->playback[playback_i].busy = 1;
            ++num_playback_dirs_busy;
        }
    }

    /* set random busy polling dirents */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (poll_i = 0; poll_i < CF_MAX_POLLING_DIR_PER_CHAN; ++poll_i)
        {
            dummy_channel[ci]->poll[poll_i].pb.busy = 1;
            ++num_poll_dirs_busy;
        }
    }

    type_of_context_CF_CList_Traverse = CLOSE_FILES;
    UT_SetDataBuffer(UT_KEY(CF_CList_Traverse), context_CF_CList_Traverse, sizeof(context_CF_CList_Traverse), false);

    /* TODO: must set a fail (function does not care) to avoid stub implementation yelling about deleting uncreated pipe
     * - this should not be required! */
    UT_SetDefaultReturnValue(UT_KEY(OS_DirectoryClose), -1);

    UT_SetDefaultReturnValue(
        UT_KEY(CFE_SB_DeletePipe),
        CFE_SUCCESS); /* TODO: CFE_SB_DeletePipe stub only saves argument value on success, this function "doesn't care"
                         about the result and should be able to save it no matter the result. */

    /* Act */
    CF_CFDP_DisableEngine();

    /* Assert */
    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        int node_i = 0;

        for (node_i = 0; node_i < num_clist_node_ptrs; ++node_i)
        {
            UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].start,
                                dummy_channel[ci]->qs[qs_index[node_i]]);
            UtAssert_True(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].fn == CF_CFDP_CloseFiles,
                          "context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].fn ==  CF_CFDP_CloseFiles");
            UtAssert_ADDRESS_EQ(context_CF_CList_Traverse[(ci * num_clist_node_ptrs) + node_i].context, NULL);
        }

        UtAssert_MemCmpValue(&CF_AppData.hk.channel_hk[ci].q_size, 0, sizeof(CF_AppData.hk.channel_hk[ci].q_size),
                             "All of CF_AppData.hk.channel_hk[%u].q_size[] values set to 0", ci);
    }

    /* TODO: OS_DirectoryClose count check does not fully test what is happening here
     * the stub does not save the dir_id for the test to check, but that should be checked at this level */
    UtAssert_STUB_COUNT(OS_DirectoryClose, num_playback_dirs_busy + num_poll_dirs_busy);
    UtAssert_STUB_COUNT(CFE_SB_DeletePipe, 2);

    for (ci = 0; ci < CF_NUM_CHANNELS; ++ci)
    {
        for (qi = 0; qi < num_clist_node_ptrs; ++qi)
        {
            free((CF_AppData.engine.channels + ci)->qs[qs_index[qi]]);
        }
    }

} /* end Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndAllOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe
   */

/* end CF_CFDP_DisableEngine tests */

/*******************************************************************************
**
**  cf_cfdp_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CFDP_ArmAckTimer_tests(void)
{
    UtTest_Add(Test_CF_CFDP_ArmAckTimer_Call_CF_Timer_InitRelSec_WithCorrectParamsAndArmsTimer, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ArmAckTimer_Call_CF_Timer_InitRelSec_WithCorrectParamsAndArmsTimer");
}

void add_CF_CFDP_GetClass_tests(void)
{
    UtTest_Add(Test_CF_CFDP_GetClass_AssertsBecause_q_index_IsEqTo_CF_Q_FREE, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_GetClass_AssertsBecause_q_index_IsEqTo_CF_Q_FREE");
    UtTest_Add(Test_CF_CFDP_GetClass_WhenNeitherStateIsSet_Return_CLASS_1, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_GetClass_WhenNeitherStateIsSet_Return_CLASS_1");
    UtTest_Add(Test_CF_CFDP_GetClass_WhenStateIs_CFDP_S2_Return_CLASS_1, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_GetClass_WhenStateIs_CFDP_S2_Return_CLASS_1");
    UtTest_Add(Test_CF_CFDP_GetClass_WhenStateIs_CFDP_R2_Return_CLASS_1, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_GetClass_WhenStateIs_CFDP_R2_Return_CLASS_1");
}

void add_CF_CFDP_IsSender_tests(void)
{
    UtTest_Add(Test_CF_CFDP_IsSender_AssertsBecause_q_index_IsEqTo_CF_Q_FREE, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_IsSender_AssertsBecause_q_index_IsEqTo_CF_Q_FREE");
    UtTest_Add(Test_CF_CFDP_IsSender_WhenNeitherStateIsSetReturn_0_MeaningReciever, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_IsSender_WhenNeitherStateIsSetReturn_0_MeaningReciever");
    UtTest_Add(Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S1_Return_1_MeaningSender, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S1_Return_1_MeaningSender");
    UtTest_Add(Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S2_Return_1_MeaningSender, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_IsSender_WhenStateIs_CFDP_S2_Return_1_MeaningSender");
}

void add_CF_CFDP_ArmInactTimer_tests(void)
{
    UtTest_Add(Test_CF_CFDP_ArmInactTimer_Call_CF_Timer_InitRelSec_WithCorrectParams, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_ArmInactTimer_Call_CF_Timer_InitRelSec_WithCorrectParams");
}

void add_CF_CFDP_DispatchRecv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_EqTo_CFDP_INVALID, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_EqTo_CFDP_INVALID");
    // TODO: Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_GreaterThan_CFDP_INVALID an edge case CF_Assert
    // UtTest_Add(Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_GreaterThan_CFDP_INVALID,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchRecv_AssertsBecause_t_state_GreaterThan_CFDP_INVALID");
    // TODO: tests commented out because they are not necessary for coverage and are not yet implemented correctly
    // UtTest_Add(Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_RecvIdle_BecauseStateEq_CFDP_IDLE,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_RecvIdle_BecauseStateEq_CFDP_IDLE");
    // UtTest_Add(Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R1_Recv_BecauseStateEq_CFDP_R1,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R1_Recv_BecauseStateEq_CFDP_R1");
    // UtTest_Add(Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S1_Recv_BecauseStateEq_CFDP_S1,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S1_Recv_BecauseStateEq_CFDP_S1");
    // UtTest_Add(Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R2_Recv_BecauseStateEq_CFDP_R2,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_R2_Recv_BecauseStateEq_CFDP_R2");
    // UtTest_Add(Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S2_Recv_BecauseStateEq_CFDP_S2,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchRecv_Calls_CF_CFDP_S2_Recv_BecauseStateEq_CFDP_S2");
    UtTest_Add(Test_CF_CFDP_DispatchRecv_WhenStateEq_CFDP_DROP_Call_CF_CFDP_RecvDrop, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_DispatchRecv_WhenStateEq_CFDP_DROP_Call_CF_CFDP_RecvDrop");
}

void add_CF_CFDP_DispatchTx_tests(void)
{
    UtTest_Add(Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_INVALID, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_INVALID");
    // UtTest_Add(Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_GreaterThanOrEqTo_CFDP_INVALID,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_GreaterThanOrEqTo_CFDP_INVALID");
    UtTest_Add(Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_IDLE, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_IDLE");
    // UtTest_Add(Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R1,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R1");
    // UtTest_Add(Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R2,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_R2");
    // UtTest_Add(Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_DROP,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_DispatchTx_AssertsBecause_t_state_EqTo_CFDP_DROP");
    UtTest_Add(Test_CF_CFDP_DispatchTx_When_t_state_EqTo_CFDP_S1_Call_CF_CFDP_S1_Tx, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_DispatchTx_When_t_state_EqTo_CFDP_S1_Call_CF_CFDP_S1_Tx");
    UtTest_Add(Test_CF_CFDP_DispatchTx_Call_When_t_state_EqTo_CFDP_S2_CF_CFDP_S1_Tx, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_DispatchTx_Call_When_t_state_EqTo_CFDP_S2_CF_CFDP_S1_Tx");
}

void add_CF_CFDP_FindUnusedChunks_tests(void)
{
    UtTest_Add(Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsEq_CF_DIR_NUM, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsEq_CF_DIR_NUM");
    // UtTest_Add(Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsGreaterThan_CF_DIR_NUM,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_FindUnusedChunks_AssertsBecause_dir_IsGreaterThan_CF_DIR_NUM");
    UtTest_Add(Test_CF_CFDP_FindUnusedChunks_AssertsBecause_c_cs_dir_IsNull, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_FindUnusedChunks_AssertsBecause_c_cs_dir_IsNull");
    UtTest_Add(Test_CF_CFDP_FindUnusedChunks_Success, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_FindUnusedChunks_Success");
}

void add_CF_CFDP_FindUnusedTransaction_tests(void)
{
    UtTest_Add(Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_c_IsNull, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_c_IsNull");
    UtTest_Add(
        Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_q_size_CF_Q_HIST_IsNotLessThan_CF_NUM_HISTORIES_PER_CHANNEL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_q_size_CF_Q_HIST_IsNotLessThan_CF_NUM_HISTORIES_PER_"
        "CHANNEL");
    UtTest_Add(Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_c_qs_CF_Q_HIST_IsNull, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_FindUnusedTransaction_AssertsBecause_c_qs_CF_Q_HIST_IsNull");
    UtTest_Add(Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeTransactionsAreAvailableReturn_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeTransactionsAreAvailableReturn_NULL");
    UtTest_Add(Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeHistoryReturnExpected_t, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_FindUnusedTransaction_WhenNoFreeHistoryReturnExpected_t");
    UtTest_Add(Test_CF_CFDP_FindUnusedTransaction_WhenFreeHistoryReturnExpected_t, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_FindUnusedTransaction_WhenFreeHistoryReturnExpected_t");
}

void add_CF_CFDP_ResetHistory_tests(void)
{
    UtTest_Add(Test_CF_CFDP_ResetHistory_Call_CF_CList_Remove_Ex_And_CF_CList_InsertBack_Ex_WithCorrectParams,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ResetHistory_Call_CF_CList_Remove_Ex_And_CF_CList_InsertBack_Ex_WithCorrectParams");
}

void add_CF_CFDP_FreeTransaction_tests(void)
{
    UtTest_Add(Test_CF_CFDP_FreeTransaction_Call_CF_CList_InitNode_And_CF_CList_InsertBack_Ex_WithCorrectParams,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_FreeTransaction_Call_CF_CList_InitNode_And_CF_CList_InsertBack_Ex_WithCorrectParams");
}

void add_CF_CFDP_FindTransactionBySequenceNumber__tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_src_eid_NotEq_history_src_eid_DoNotFindTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_src_eid_NotEq_history_src_eid_"
        "DoNotFindTransaction");
    UtTest_Add(
        Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_transaction_sequence_number_NotEq_history_seq_num_DoNotFindTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_FindTransactionBySequenceNumber__When_context_transaction_sequence_number_NotEq_history_seq_num_"
        "DoNotFindTransaction");
    UtTest_Add(
        Test_CF_CFDP_FindTransactionBySequenceNumber__When_history_And_context_Match_src_eid_And_seq_num_FindTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_FindTransactionBySequenceNumber__When_history_And_context_Match_src_eid_And_seq_num_"
        "FindTransaction");
}

void add_CF_CFDP_FindTransactionBySequenceNumber_tests(void)
{
    UtTest_Add(Test_CF_CFDP_FindTransactionBySequenceNumber_DoNotFindTransaction, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_FindTransactionBySequenceNumber_DoNotFindTransaction");
    UtTest_Add(Test_CF_CFDP_FindTransactionBySequenceNumber_FindTransactionOnLastClist, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_FindTransactionBySequenceNumber_FindTransactionOnLastClist");
}

void add_CF_CFDP_MsgOutGet_tests(void)
{
    UtTest_Add(Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_notNULL_ReturnPointerTo_msg_ph,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_notNULL_ReturnPointerTo_msg_ph");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_WhenChannel_max_outgoing_messages_per_wakeup_Is_0_AndChannel_frozen_Is_non0_Return_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_WhenChannel_max_outgoing_messages_per_wakeup_Is_0_AndChannel_frozen_Is_non0_Return_"
        "NULL");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_When_outgoing_counter_DoesNotEq_max_outgoing_messages_per_wakeup_And_t_flags_all_suspended_Is_1_Return_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_When_outgoing_counter_DoesNotEq_max_outgoing_messages_per_wakeup_And_t_flags_all_"
        "suspended_Is_1_Return_NULL");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_When_max_outgoing_messages_per_wakeup_IsNot_0_And_outgoing_counter_IsEqTo_max_outgoing_messages_per_wakeup_Return_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_When_max_outgoing_messages_per_wakeup_IsNot_0_And_outgoing_counter_IsEqTo_max_outgoing_"
        "messages_per_wakeup_Return_NULL");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_nonNULL_ButSecondIfCheckingSameValueIs_NULL_WhichDoesNotAppearToBePossible,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_When_CF_AppData_engine_out_msg_Is_nonNULL_ButSecondIfCheckingSameValueIs_NULL_"
        "WhichDoesNotAppearToBePossible");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_AppData_engine_out_msg_IsStill_NULL_SendEventBecause_silent_Is_0_Return_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_"
        "AppData_engine_out_msg_IsStill_NULL_SendEventBecause_silent_Is_0_Return_NULL");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_AppData_engine_out_msg_IsStill_NULL_SendNoEventBecause_silent_Is_non0_Return_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_But_CallTo_OS_CountSemTimedWait_Returns_nonOS_SUCCESS_And_CF_"
        "AppData_engine_out_msg_IsStill_NULL_SendNoEventBecause_silent_Is_non0_Return_NULL");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_ThenReturn_outgoing_msg_ph_Address,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_non0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_"
        "AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_"
        "ThenReturn_outgoing_msg_ph_Address");
    UtTest_Add(
        Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_ThenReturn_outgoing_msg_ph_Address,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_MsgOutGet_When_sem_name_0_Is_0_Then_CallTo_OS_CountSemTimedWait_Returns_OS_SUCCESS_And_CF_"
        "AppData_engine_out_msg_Is_notNULL_Call_CFE_MSG_Init_And_Increment_CF_AppData_engine_outgoing_counter_"
        "ThenReturn_outgoing_msg_ph_Address");
}

void add_CF_CFDP_Send_tests(void)
{
    UtTest_Add(Test_CF_CFDP_Send_AssertsBecause_chan_num_IsEqTo_CF_NUM_CHANNELS, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_Send_AssertsBecause_chan_num_IsEqTo_CF_NUM_CHANNELS");
    // UtTest_Add(Test_CF_CFDP_Send_AssertsBecause_chan_num_IsGreaterThanTo_CF_NUM_CHANNELS,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_Send_AssertsBecause_chan_num_IsGreaterThanTo_CF_NUM_CHANNELS");
    UtTest_Add(Test_CF_CFDP_Send_InitializeMsgTimestampsItAndSendsIt, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_Send_InitializeMsgTimestampsItAndSendsIt");
}

void add_CF_CFDP_SetPduLength_tests(void)
{
    UtTest_Add(Test_CF_CFDP_SetPduLength_SetGiven_ph_length_ToGiven_length, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SetPduLength_SetGiven_ph_length_ToGiven_length");
}

void add_CF_CFDP_ConstructPduHeader_tests(void)
{
    UtTest_Add(Test_CF_CFDP_ConstructPduHeader_CallTo_CF_CFDP_MsgOutGet_Returns_NULL_DoNothingReturn_NULL,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ConstructPduHeader_CallTo_CF_CFDP_MsgOutGet_Returns_NULL_DoNothingReturn_NULL");
    UtTest_Add(Test_CF_CFDP_ConstructPduHeader_Given_directive_code_0_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ConstructPduHeader_Given_directive_code_0_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg");
    UtTest_Add(Test_CF_CFDP_ConstructPduHeader_Given_directive_code_1_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ConstructPduHeader_Given_directive_code_1_CallTo_CF_CFDP_MsgOutGet_ReturnsMsgReturnMsg");
}

void add_CF_strnlen_tests(void)
{
    UtTest_Add(Test_CF_strnlen_When_end_IsNot_NULL_ReturnLengthOfGiven_s, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_strnlen_When_end_IsNot_NULL_ReturnLengthOfGiven_s");
    UtTest_Add(Test_CF_strnlen_When_end_Is_NULL_ReturnLengthOfGiven_s, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_strnlen_When_end_Is_NULL_ReturnLengthOfGiven_s");
}

void add_CF_CFDP_SendMd_tests(void)
{
    UtTest_Add(Test_CF_CFDP_SendMd_GetNull_pdu_header_Return_CF_SEND_NO_MSG, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_SendMd_GetNull_pdu_header_Return_CF_SEND_NO_MSG");
    UtTest_Add(Test_CF_CFDP_SendMd_AssertsBecause_state_NotEq_CFDP_S1_Or_CFDP_S2, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_SendMd_AssertsBecause_state_NotEq_CFDP_S1_Or_CFDP_S2");
    UtTest_Add(Test_CF_CFDP_SendMd_When_src_len_Eq_sizeof_src_filename_Return_CF_SEND_FAILURE, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendMd_When_src_len_Eq_sizeof_src_filename_Return_CF_SEND_FAILURE");
    UtTest_Add(Test_CF_CFDP_SendMd_When_dst_len_Eq_sizeof_dst_filename_Return_CF_SEND_FAILURE, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendMd_When_dst_len_Eq_sizeof_dst_filename_Return_CF_SEND_FAILURE");
    UtTest_Add(Test_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_CF_CFDP_CopyDataToLv_Returns_neg1_On_src_Call,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendMd_Returns_CF_SEND_ERROR_CF_CFDP_CopyDataToLv_Returns_neg1_On_src_Call");
    UtTest_Add(Test_CF_CFDP_SendMd_WhenCallTo_CF_CFDP_CopyDataToLv_Returns_neg1_OnThe_dst_Call_Return_CF_SEND_ERROR,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendMd_WhenCallTo_CF_CFDP_CopyDataToLv_Returns_neg1_OnThe_dst_Call_Return_CF_SEND_ERROR");
    UtTest_Add(Test_CF_CFDP_SendMd_Return_CF_SEND_SUCCESS, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendMd_Return_CF_SEND_SUCCESS");
}

void add_CF_CFDP_SendFd_tests(void)
{
    UtTest_Add(Test_CF_CFDP_SendFd_When_len_GreaterThan_sizeof_pdu_pd_data_t_Return_CF_SEND_ERROR, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendFd_When_len_GreaterThan_sizeof_pdu_pd_data_t_Return_CF_SEND_ERROR");
    UtTest_Add(Test_CF_CFDP_SendFd_Return_CF_SEND_SUCCESS, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendFd_Return_CF_SEND_SUCCESS");
}

void add_CF_CFDP_FinishEofAck_tests(void)
{
    UtTest_Add(Test_CF_CFDP_FinishEofAck_SetExpectedValues, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_FinishEofAck_SetExpectedValues");
}

void add_CF_CFDP_SendEof_tests(void)
{
    UtTest_Add(Test_CF_CFDP_SendEof_Get_NULL_pdu_Return_CF_SEND_NO_MSG, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendEof_Get_NULL_pdu_Return_CF_SEND_NO_MSG");
    UtTest_Add(Test_CF_CFDP_SendEof_SuccessWithNoError, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendEof_SuccessWithNoError");
    UtTest_Add(Test_CF_CFDP_SendEof_SuccessWithError, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendEof_SuccessWithError");
}

void add_CF_CFDP_SendAck_tests(void)
{
    UtTest_Add(Test_CF_CFDP_SendAck_When_CF_CFDP_IsSender_Returns_false_Get_NULL_ph_Return_CF_SEND_NO_MSG,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendAck_When_CF_CFDP_IsSender_Returns_false_Get_NULL_ph_Return_CF_SEND_NO_MSG");
    UtTest_Add(Test_CF_CFDP_SendAck_AssertsBecauseGiven_dir_code_Is_Not_PDU_EOF_Or_PDU_FIN, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_SendAck_AssertsBecauseGiven_dir_code_Is_Not_PDU_EOF_Or_PDU_FIN");
    UtTest_Add(
        Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_EOF_And_CF_CFDP_IsSender_Returns_true_GetMsg_pdu_Return_CF_SEND_NO_MSG,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_EOF_And_CF_CFDP_IsSender_Returns_true_GetMsg_pdu_Return_CF_"
        "CF_TxSubState_NO_MSG");
    UtTest_Add(
        Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_FIN_And_CF_CFDP_IsSender_Returns_true_GetMsg_ph_Return_CF_SEND_SUCCESS,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_SendAck_WhenGiven_dir_code_Is_PDU_FIN_And_CF_CFDP_IsSender_Returns_true_GetMsg_ph_Return_CF_SEND_"
        "SUCCESS");
}

void add_CF_CFDP_SendFin_tests(void)
{
    UtTest_Add(Test_CF_CFDP_SendFin_Get_NULL_ph_Return_CF_SEND_NO_MSG, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendFin_Get_NULL_ph_Return_CF_SEND_NO_MSG");
    UtTest_Add(Test_CF_CFDP_SendFin_Given_cc_NotEqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendFin_Given_cc_NotEqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS");
    UtTest_Add(Test_CF_CFDP_SendFin_Given_cc_EqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendFin_Given_cc_EqTo_CC_NO_ERROR_GetNull_ph_Return_CF_SEND_SUCCESS");
}

void add_CF_CFDP_SendNak_tests(void)
{
    UtTest_Add(Test_CF_CFDP_SendNak_GetNull_ph_Return_CF_SEND_NO_MSG, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendNak_GetNull_ph_Return_CF_SEND_NO_MSG");
    UtTest_Add(Test_CF_CFDP_SendNak_AssertsBecause_CF_CFDP_GetClass_With_t_Eq_CLASS_2, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_SendNak_AssertsBecause_CF_CFDP_GetClass_With_t_Eq_CLASS_2");
    UtTest_Add(Test_CF_CFDP_SendNak_Success_Return_CF_SEND_SUCCESS, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_SendNak_Success_Return_CF_SEND_SUCCESS");
}

void add_CF_CF_CFDP_RecvPh_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvPh_AssertsBecause_chan_num_GreaterThan_CF_NUM_CHANNELS, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvPh_AssertsBecause_chan_num_GreaterThan_CF_NUM_CHANNELS");
    UtTest_Add(Test_CF_CFDP_RecvPh_AssertsBecause_msg_in_Is_NULL, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvPh_AssertsBecause_msg_in_Is_NULL");
    UtTest_Add(Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsLessThan_hsize_SendEventCountErrorReturn_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsLessThan_hsize_SendEventCountErrorReturn_neg1");
    UtTest_Add(
        Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsEqTo_hsize_CallTo_CF_GetVariableHeader_IsNot_0_CountErrorReturn_neg1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsEqTo_hsize_CallTo_CF_GetVariableHeader_IsNot_0_CountErrorReturn_"
        "neg1");
    UtTest_Add(
        Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsGreaterThan_hsize_CallTo_CF_GetVariableHeader_IsNot_0_CountErrorReturn_neg1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvPh_NumberOf_bytes_recieved_IsGreaterThan_hsize_CallTo_CF_GetVariableHeader_IsNot_0_"
        "CountErrorReturn_neg1");
    UtTest_Add(Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_DoesNotEq_bytes_received_SendEventCountErrorReturn_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_DoesNotEq_bytes_received_SendEventCountErrorReturn_neg1");
    UtTest_Add(Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_NotEq_bytes_received_Count_pdu_Return_0, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvPh_ValueOf_temp_Plus_hsize_NotEq_bytes_received_Count_pdu_Return_0");
}

void add_CF_CFDP_RecvMd_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvMd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvMd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL");
    UtTest_Add(Test_CF_CFDP_RecvMd_Has_bytes_received_LessThan_sizof_ph_Plus_size_of_pdu_md_t_CountErrorReturn_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvMd_Has_bytes_received_LessThan_sizof_ph_Plus_size_of_pdu_md_t_CountErrorReturn_neg1");
    UtTest_Add(Test_CF_CFDP_RecvMd_HasFirst_lv_ret_LessThan_0_SendsEventCountErrorAndReturn_neg1, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvMd_HasFirst_lv_ret_LessThan_0_SendsEventCountErrorAndReturn_neg1");
    UtTest_Add(Test_CF_CFDP_RecvMd_HasSecond_lv_ret_LessThan_0_BecauseLengthEqSizeSendEventAndReturn_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvMd_HasSecond_lv_ret_LessThan_0_BecauseLengthEqSizeSendEventAndReturn_neg1");
    UtTest_Add(Test_CF_CFDP_RecvMd_WhenNoErrorConditions_SendEventAndReturn_0, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvMd_WhenNoErrorConditions_SendEventAndReturn_0");
}

void add_CF_CFDP_RecvFd_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvFd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvFd_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL");
    UtTest_Add(
        Test_CF_CFDP_RecvFd_When_bytes_received_LessThan_sizeof_pdu_file_data_header_t_CountsErrorAndReturns_neg1_FailAndSendsEvent,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvFd_When_bytes_received_LessThan_sizeof_pdu_file_data_header_t_CountsErrorAndReturns_neg1_"
        "FailAndSendsEvent");
    UtTest_Add(
        Test_CF_CFDP_RecvFd_When_bytes_received_EqTo_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_Success,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvFd_When_bytes_received_EqTo_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_Success");
    UtTest_Add(
        Test_CF_CFDP_RecvFd_When_bytes_received_GreaterThan_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_Success,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvFd_When_bytes_received_GreaterThan_sizeof_pdu_file_data_header_t_ByteSwaps_fd_fdh_offset_"
        "Success");
}

void add_CF_CFDP_RecvEof_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvEof_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvEof_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL");
    UtTest_Add(Test_CF_CFDP_RecvEof_When_bytes_received_LessThan_offsetof_pdu_eof_t_fault_location_Returns_neg1_Fail,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvEof_When_bytes_received_LessThan_offsetof_pdu_eof_t_fault_location_Returns_neg1_Fail");
    UtTest_Add(
        Test_CF_CFDP_RecvEof_BytesReceivedEq_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_eof_size_Return_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvEof_BytesReceivedEq_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_eof_size_"
        "Return_0");
    UtTest_Add(
        Test_CF_CFDP_RecvEof_BytesReceivedGreaterThan_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_eof_size_Return_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvEof_BytesReceivedGreaterThan_offsetof_fault_location_Calls_cfdp_get_uint32_With_eof_crc_And_"
        "eof_size_Return_0");
}

void add_CF_CFDP_RecvAck_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvAck_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvAck_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL");
    UtTest_Add(Test_CF_CFDP_RecvAck_FailsBecause_bytes_received_LessThan_sizeof_pdu_ack_t_Returns_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvAck_FailsBecause_bytes_received_LessThan_sizeof_pdu_ack_t_Returns_neg1");
    UtTest_Add(Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_EqTo_sizeof_pdu_ack_t_Returns_0, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_EqTo_sizeof_pdu_ack_t_Returns_0");
    UtTest_Add(Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_GreaterThan_sizeof_pdu_ack_t_Returns_0,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvAck_SuccessBecause_bytes_received_GreaterThan_sizeof_pdu_ack_t_Returns_0");
}

void add_CF_CFDP_RecvFin_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvFin_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvFin_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL");
    UtTest_Add(
        Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_IsLessThan_offsetof_pdu_fin_t_fault_location_Returns_neg1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_IsLessThan_offsetof_pdu_fin_t_fault_location_Returns_neg1");
    UtTest_Add(
        Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_Is_1_LessThan_offsetof_pdu_fin_t_fault_location_Returns_neg1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvFin_FailsBecause_bytes_received_Is_1_LessThan_offsetof_pdu_fin_t_fault_location_Returns_"
        "neg1");
    UtTest_Add(Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsEqTo_offsetof_pdu_fin_t_fault_location_Returns_0,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsEqTo_offsetof_pdu_fin_t_fault_location_Returns_0");
    UtTest_Add(
        Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsGreaterThan_offsetof_pdu_fin_t_fault_location_Returns_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvFin_SuccessBecause_bytes_received_IsGreaterThan_offsetof_pdu_fin_t_fault_location_Returns_0");
}

void add_CF_CFDP_RecvNak_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvNak_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvNak_AssertsBecause_CF_AppData_engine_msg_in_Is_NULL");
    UtTest_Add(Test_CF_CFDP_RecvNak_AssertsBecause_num_segment_requests_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvNak_AssertsBecause_num_segment_requests_Is_NULL");
    UtTest_Add(
        Test_CF_CFDP_RecvNak_FailsBecause_bytes_received_IsLessThan_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_Return_neg1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvNak_FailsBecause_bytes_received_IsLessThan_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_"
        "requests_Return_neg1");
    UtTest_Add(
        Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_Sets_num_segment_requests_To_1_BecauseThatIsTheOnlySegmentThenReturn_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_Sets_num_"
        "segment_requests_To_1_BecauseThatIsTheOnlySegmentThenReturn_0");
    UtTest_Add(
        Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_OneLessThan_CF_NAK_MAX_SEGMENTS_Sets_num_segment_requests_To_OneLessThan_CF_NAK_MAX_SEGMENTS_ThenReturn_OneLessThan_CF_NAK_MAX_SEGMENTS,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_OneLessThan_"
        "CF_NAK_MAX_SEGMENTS_Sets_num_segment_requests_To_OneLessThan_CF_NAK_MAX_SEGMENTS_ThenReturn_OneLessThan_CF_"
        "NAK_MAX_SEGMENTS");
    UtTest_Add(
        Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_SEGMENTS_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_"
        "SEGMENTS_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS");
    UtTest_Add(
        Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_SEGMENTS_PlusAnotherSegment_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvNak_bytes_received_IsEqTo_CF_HeaderSize_Plus_offsetof_pdu_nak_t_segment_requests_CF_NAK_MAX_"
        "SEGMENTS_PlusAnotherSegment_Sets_num_segment_requests_To_CF_NAK_MAX_SEGMENTS_ThenReturn_CF_NAK_MAX_SEGMENTS");
}

void add_CF_CFDP_RecvDrop_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData");
    UtTest_Add(Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData_AndItRollsOver, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvDrop_IncrementsCounterIn_CF_AppData_AndItRollsOver");
}

void add_CF_CFDP_RecvIdle_tests(void)
{
    UtTest_Add(Test_CF_CFDP_RecvIdle_AssertBecause_CF_AppData_engine_in_msg_Is_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_RecvIdle_AssertBecause_CF_AppData_engine_in_msg_Is_NULL");
    UtTest_Add(
        Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_And_PDU_HDR_FLAGS_MODE_AreBoth_true_Set_t_state_To_CFDP_DROP,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_And_PDU_HDR_FLAGS_MODE_AreBoth_true_Set_t_state_To_CFDP_"
        "DROP");
    UtTest_Add(
        Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_NotChangedNoReset,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_"
        "R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_NotChangedNoReset");
    UtTest_Add(
        Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_ChangedTo_CFDP_IDLE_DoReset,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Is_true_And_PDU_HDR_FLAGS_MODE_Is_false_Set_t_state_To_CFDP_"
        "R2_And_Call_CF_CFDP_R_Init_And_CF_CFDP_DispatchRecv_t_state_ChangedTo_CFDP_IDLE_DoReset");
    UtTest_Add(
        Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_But_fdh_directive_code_IsNot_PDU_METADATA_SendEventAnd_Increment_recv_error,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_But_fdh_directive_code_IsNot_PDU_METADATA_"
        "SendEventAnd_Increment_recv_error");
    UtTest_Add(
        Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_non0_SendEventAnd_Increment_recv_error,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_"
        "ButCallTo_CF_CFDP_RecvMd_Returns_non0_SendEventAnd_Increment_recv_error");
    UtTest_Add(
        Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R1_NoReset,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_"
        "ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R1_NoReset");
    UtTest_Add(
        Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R2_NoReset,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_RecvIdle_CheckOf_PDU_HDR_FLAGS_TYPE_Returns_false_And_fdh_directive_code_Is_PDU_METADATA_"
        "ButCallTo_CF_CFDP_RecvMd_Returns_0_t_state_SetTo_CFDP_R2_NoReset");
}

void add_CF_CFDP_InitEngine_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_CreatePipe_Returns_nonCFE_SUCCESS_Value_SendEventAndReturnThatValue,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_CreatePipe_Returns_nonCFE_SUCCESS_Value_"
        "SendEventAndReturnThatValue");
    UtTest_Add(
        Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_SubscribeLocal_Returns_nonCFE_SUCCESS_Value_SendEventAndReturnThatValue,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_InitEngine_FirstCallTo_CFE_SB_SubscribeLocal_Returns_nonCFE_SUCCESS_Value_"
        "SendEventAndReturnThatValue");
    UtTest_Add(
        Test_CF_CFDP_InitEngine_FirstCallTo_OS_CountSemGetIdByNamel_Returns_nonOS_SUCCESS_Value_SendEventAndReturnThatValue,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_InitEngine_FirstCallTo_OS_CountSemGetIdByNamel_Returns_nonOS_SUCCESS_Value_"
        "SendEventAndReturnThatValue");
    UtTest_Add(
        Test_CF_CFDP_InitEngine_AssertsBecause_chunk_mem_offset_Plus_CF_max_chunks_k_i_IsLessThan_CF_NUM_CHUNKS_ALL_CHANNELS,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_InitEngine_AssertsBecause_chunk_mem_offset_Plus_CF_max_chunks_k_i_IsLessThan_CF_NUM_CHUNKS_ALL_"
        "CHANNELS");
    UtTest_Add(Test_CF_CFDP_InitEngine_SuccessSet_CF_AppData_engine_enabled_To_1_AndReturn_CFE_SUCCESS,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_InitEngine_SuccessSet_CF_AppData_engine_enabled_To_1_AndReturn_CFE_SUCCESS");
}

void add_CF_CFDP_ReceiveMessage_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_When_rx_max_messages_per_wakeup_For_chan_num_Is_0_Set_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_When_rx_max_messages_per_wakeup_For_chan_num_Is_0_Set_CF_AppData_engine_in_msg_To_"
        "NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTO_CFE_SB_ReceiveBuffer_Returns_CFE_SB_NO_MESSAGE_Set_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTO_CFE_SB_ReceiveBuffer_Returns_CFE_SB_NO_MESSAGE_Set_CF_AppData_engine_in_"
        "msg_To_NULL");
    UtTest_Add(Test_CF_CFDP_ReceiveMessage_CallTO_CF_CFDP_RecvPh_Returns_non0_Set_CF_AppData_engine_in_msg_To_NULL,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ReceiveMessage_CallTO_CF_CFDP_RecvPh_Returns_non0_Set_CF_AppData_engine_in_msg_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_CF_CFDP_FindTransactionBySequenceNumber_Returns_nonNULL_Call_CF_CFDP_DispatchRecv_ThenSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_CF_CFDP_FindTransactionBySequenceNumber_Returns_nonNULL_Call_CF_CFDP_"
        "DispatchRecv_ThenSet_CF_AppData_engine_in_msg_To_NULL");
    UtTest_Add(Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_state_IsGreaterThan_CFDP_IDLE, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_state_IsGreaterThan_CFDP_IDLE");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_src_And_dst_AreNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_src_And_dst_AreNot_config_table_local_eid_SendEvenThenSet_CF_AppData_"
        "engine_in_msg_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_FGV_Returns_non0_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_FGV_Returns_non0_And_dst_IsNot_config_table_local_eid_"
        "SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_Then_bytes_received_IsLessThanExpected_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_Then_bytes_received_IsLessThanExpected_And_dst_IsNot_config_table_local_"
        "eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_Then_directive_code_IsNotEqTo_PDU_FIN_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_Then_directive_code_IsNotEqTo_PDU_FIN_And_dst_IsNot_config_table_local_eid_"
        "SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_neg1_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_neg1_And_dst_IsNot_config_table_local_"
        "eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_Increment_counters_recv_spurious_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_Increment_counters_recv_spurious_And_"
        "dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_CallTo_CF_CFDP_SendAck_Retuns_CFE_SEND_SUCCESS_Set_c_cur_To_NULL_Increment_counters_recv_spurious_And_dst_IsNot_config_table_local_eid_SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL2,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_Then_CallTo_CF_CFDP_RecvFin_Returns_0_CallTo_CF_CFDP_SendAck_Retuns_CFE_"
        "CF_TxSubState_SUCCESS_Set_c_cur_To_NULL_Increment_counters_recv_spurious_And_dst_IsNot_config_table_local_eid_"
        "SendEvenThenSet_CF_AppData_engine_in_msg_To_NULL2");
    UtTest_Add(
        Test_CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_AppData_engine_in_msg_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_"
        "And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_"
        "AppData_engine_in_msg_To_NULL");
    UtTest_Add(Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_Is_NULL, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ReceiveMessage_AssertsBecause_t_Is_NULL");
    // UtTest_Add(Test_2CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_AppData_engine_in_msg_To_NULL,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_2CF_CFDP_ReceiveMessage_CallTo_src_IsNotEqTo_config_table_local_eid_But_dst_IsEqTo_config_table_local_eid_And_q_size_CF_Q_RX_IsNotEqTo_CF_MAX_SIMULTANEOUS_RX_SendEventThenIncrement_counters_recv_dropped_AndSet_CF_AppData_engine_in_msg_To_NULL");
}

void add_CF_CFDP_CycleTx__tests(void)
{
    UtTest_Add(Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_suspended_Return_0,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_suspended_Return_0");
    UtTest_Add(
        Test_CF_CFDP_CycleTx__AssertsBecauseGiven_node_TransactionContainer_t_flags_all_q_index_IsNotEqTo_CF_Q_TXA,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleTx__AssertsBecauseGiven_node_TransactionContainer_t_flags_all_q_index_IsNotEqTo_CF_Q_TXA");
    UtTest_Add(
        Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_notNULL_Sets_args_ran_one_To_1_ThenReturn_1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_"
        "notNULL_Sets_args_ran_one_To_1_ThenReturn_1");
    UtTest_Add(
        Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_OnFirstCallTo_CF_CFDP_DispatchTx_ThenReturn_1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_"
        "NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_OnFirstCallTo_CF_CFDP_DispatchTx_ThenReturn_1");
    UtTest_Add(
        Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_AtSomePointThenReturn_1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleTx__Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_"
        "NULL_Sets_args_ran_one_To_1_When_args_c_cur_Becomes_notNULL_AtSomePointThenReturn_1");
    UtTest_Add(
        Test_CF_CFDP_CycleTx_Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_NULL_Sets_args_ran_one_To_1_Then_t_flags_all_q_index_BecomesNotEqTo_CF_Q_TXA__OnFirstCallTo_CF_CFDP_DispatchTx_And_args_c_cur_Becomes_notNULL_StillRunsAndStopsWhen_t_flags_all_q_index_Becomes_CF_Q_TXA_AgainAnd_args_c_cur_Becomes_notNULL_ThenReturn_1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleTx_Given_node_TransactionContainer_t_flags_all_q_index_IsEqTo_CF_Q_TXA_And_args_c_cur_Is_"
        "NULL_Sets_args_ran_one_To_1_Then_t_flags_all_q_index_BecomesNotEqTo_CF_Q_TXA__OnFirstCallTo_CF_CFDP_"
        "DispatchTx_And_args_c_cur_Becomes_notNULL_StillRunsAndStopsWhen_t_flags_all_q_index_Becomes_CF_Q_TXA_AgainAnd_"
        "args_c_cur_Becomes_notNULL_ThenReturn_1");
} /* end add_CF_CFDP_CycleTx_tests */

void add_CF_CFDP_CycleTx_tests(void)
{
    UtTest_Add(Test_CF_CFDP_CycleTx_When_dequeue_enabled_IsTrueIn_config_table_FromGiven_c_DoNothing,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CycleTx_When_dequeue_enabled_IsTrueIn_config_table_FromGiven_c_DoNothing");
    UtTest_Add(Test_CF_CFDP_CycleTx_When_cur_FromGiven_c_Is_notNULL_Set_c_cur_To_NULL, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_CycleTx_When_cur_FromGiven_c_Is_notNULL_SetTo_NULL");
    UtTest_Add(Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_CF_CList_Traverse_CalledOnlyOnce_Set_c_cur_To_NULL,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_CF_CList_Traverse_CalledOnlyOnce_Set_c_cur_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_Then_CF_CList_Traverse_CalledOnlyOnce_ReturnsAnd_ran_one_IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleTx_When_c_qs_CF_Q_PEND_Is_NULL_Then_CF_CList_Traverse_CalledOnlyOnce_ReturnsAnd_ran_one_"
        "IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL");
    UtTest_Add(
        Test_CF_CFDP_CycleTx_EnterWhileLoopOnceAndCall_cf_move_transaction_SecondCallTo_CF_CList_Traverse_ReturnsAnd_ran_one_IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleTx_EnterWhileLoopOnceAndCall_cf_move_transaction_SecondCallTo_CF_CList_Traverse_ReturnsAnd_"
        "ran_one_IsNow_non0_And_qs_NULL_Set_c_cur_To_NULL");
}

void add_CF_CFDP_DoTick_tests(void)
{
    UtTest_Add(Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNotNull_And_NotEqTo_t_Return_CLIST_CONT,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNotNull_And_NotEqTo_t_Return_CLIST_CONT");
    UtTest_Add(
        Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNull_But_t_flags_all_suspended_IsTrueReturn_CLIST_CONT,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_IsNull_But_t_flags_all_suspended_IsTrueReturn_CLIST_"
        "CONT");
    UtTest_Add(
        Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_But_t_flags_all_suspended_IsTrueReturn_CLIST_CONT,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_But_t_flags_all_suspended_IsTrueReturn_CLIST_"
        "CONT");
    UtTest_Add(
        Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_DoesNotChange_cur_From_NULL_Return_CLIST_CONT,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_"
        "DoesNotChange_cur_From_NULL_Return_CLIST_CONT");
    UtTest_Add(
        Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_Changea_cur_To_notNULL_Set_args_early_exit_To_1_AndReturn_CLIST_EXIT,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_DoTick_Given_context_Determined_args_c_cur_Is_t_And_t_flags_all_suspended_IsFalse_CallTo_fn_"
        "Changea_cur_To_notNULL_Set_args_early_exit_To_1_AndReturn_CLIST_EXIT");
}

void add_CF_CFDP_TickTransactions_tests(void)
{
    UtTest_Add(Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsEqTo_CF_TICK_NUM_TYPES, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsEqTo_CF_TICK_NUM_TYPES");
    // UtTest_Add(Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsGreaterThan_CF_TICK_NUM_TYPES,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_TickTransactions_AssertsBecause_c_tick_type_IsGreaterThan_CF_TICK_NUM_TYPES");
    UtTest_Add(
        Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_RX_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_RX,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_RX_AndDetermined_args_Item_cont_StaysSetAt_0_"
        "RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_RX");
    UtTest_Add(
        Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NORM,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_AndDetermined_args_Item_cont_StaysSetAt_0_"
        "RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NORM");
    UtTest_Add(
        Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_AndDetermined_args_Item_cont_StaysSetAt_0_RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NAK,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_AndDetermined_args_Item_cont_StaysSetAt_0_"
        "RunsLoopNumberOfTimesTheDifferenceBetween_CF_TICK_NUM_TYPES_And_CF_TICK_TXW_NAK");
    UtTest_Add(
        Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_And_Determined_args_early_exit_Is_true_RunsLoopOnlyOnce,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_And_Determined_args_early_exit_Is_true_"
        "RunsLoopOnlyOnce");
    UtTest_Add(
        Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_And_Determined_args_early_exit_Is_true_GoesTo_early_exit,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NORM_And_Determined_args_early_exit_Is_true_"
        "GoesTo_early_exit");
    UtTest_Add(Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_RunsLoopAsLongAs_args_cont_Is_true,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_TickTransaction_Given_c_tick_type_Is_CF_TICK_TXW_NAK_RunsLoopAsLongAs_args_cont_Is_true");
}

void add_CF_CFDP_TxFile___tests(void)
{
    UtTest_Add(Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction_cfdp_class_SetTo_CLASS_1, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction_cfdp_class_SetTo_CLASS_1");
    UtTest_Add(Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction_cfdp_class_SetTo_CLASS_2, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_TxFile___SetsGivenValuesOnTransaction_cfdp_class_SetTo_CLASS_2");
}

void add_CF_CFDP_TxFile__tests(void)
{
    UtTest_Add(Test_CF_CFDP_TxFile_DoesNotError, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_TxFile_DoesNotError");
}

void add_CF_CFDP_TxFile_tests(void)
{
    UtTest_Add(Test_CF_CFDP_TxFile_AssertsBecause_chan_IsGreaterThan_CF_NUM_CHANNELS, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_TxFile_AssertsBecause_chan_IsGreaterThan_CF_NUM_CHANNELS");
    // UtTest_Add(Test_CF_CFDP_TxFile_AssertsBecause_chan_IsEqTo_CF_NUM_CHANNELS,
    //   cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
    //   "Test_CF_CFDP_TxFile_AssertsBecause_chan_IsEqTo_CF_NUM_CHANNELS");
    UtTest_Add(Test_CF_CFDP_TxFile_FailsBecause_c_num_cmd_tx_IsEqTo_CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_TxFile_FailsBecause_c_num_cmd_tx_IsEqTo_CF_MAX_COMMANDED_PLAYBACK_FILES_PER_CHAN");
    UtTest_Add(Test_CF_CFDP_TxFile_AssertsBecause_t_IsReturned_NULL_From_CF_CFDP_FindUnusedTransaction,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_TxFile_AssertsBecause_t_IsReturned_NULL_From_CF_CFDP_FindUnusedTransaction");
    UtTest_Add(Test_CF_CFDP_TxFile_AssertsBecause_t_state_IsNotEqTo_CFDP_IDLE, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_TxFile_AssertsBecause_t_state_IsNotEqTo_CFDP_IDLE");
    UtTest_Add(Test_CF_CFDP_TxFile_SuccessIncrements_c_num_cmd_tx_AndSets_t_flags_tx_cmd_tx_To_1, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_TxFile_SuccessIncrements_c_num_cmd_tx_AndSets_t_flags_tx_cmd_tx_To_1");
}

void add_CF_CFDP_PlaybackDir__tests(void)
{
    UtTest_Add(Test_CF_CFDP_PlaybackDir_FailsBecauseDirectoryWasNotOpenedThenIncrements_fault_directory_read,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_PlaybackDir_FailsBecauseDirectoryWasNotOpenedThenIncrements_fault_directory_read");
    UtTest_Add(Test_CF_CFDP_PlaybackDir_SuccessSetsCorrect_p_ValuesAndReturns_OS_SUCCESS, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_PlaybackDir_SuccessSetsCorrect_p_ValuesAndReturns_OS_SUCCESS");
}

void add_CF_CFDP_PlaybackDir_tests(void)
{
    UtTest_Add(Test_CF_CFDP_PlaybackDir_All_p_busy_Are_1_SendEventThenReturn_neg1, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_PlaybackDir_All_p_busy_Are_1_SendEventThenReturn_neg1");
    UtTest_Add(Test_CF_CFDP_PlaybackDir_First_p_busy_Is_0_Call_CF_CFDP_PlaybackDir__AndReturnCallValue,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_PlaybackDir_First_p_busy_Is_0_Call_CF_CFDP_PlaybackDir__AndReturnCallValue");
}

void add_CF_CFDP_ProcessPlaybackDirectory_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_But_p_num_ts_Is_non0_DoesNothingAndGiven_p_busy_IsUnchanged,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_But_p_num_ts_Is_non0_DoesNothingAndGiven_p_busy_"
        "IsUnchanged");
    UtTest_Add(Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_And_p_num_ts_Is_0_Given_p_busy_IsSetTo_0,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_0_And_p_num_ts_Is_0_Given_p_busy_IsSetTo_0");
    UtTest_Add(
        Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_1_And_p_num_ts_IsLessThan_CF_NUM_TRANSACTIONS_PER_PLAYBACK_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPlaybackDirectory_Given_p_diropen_Is_1_And_p_num_ts_IsLessThan_CF_NUM_TRANSACTIONS_PER_"
        "PLAYBACK_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0");
    UtTest_Add(
        Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_"
        "FileName_IsDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0");
    UtTest_Add(
        Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDoubleDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_"
        "FileName_IsDoubleDotSucceeds_SecondCallReturns_notCFE_SUCCESS_Calls_OS_DirectoryClose_AndSets_p_diropen_To_0");
    UtTest_Add(
        Test_CF_CFDP_ProcessPlaybackDirectory_AssertsBecause_CallTo_CF_CFDP_FindUnusedTransaction_Returns_NULL,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPlaybackDirectory_AssertsBecause_CallTo_CF_CFDP_FindUnusedTransaction_Returns_NULL");
    UtTest_Add(
        Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_FileName_IsDirectoryDotsFailsThenCall_CF_CFDP_FindUnusedTransaction_And_CF_CFDP_TxFile__Increment_p_num_ts_ExitLoop,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPlaybackDirectory_FirstCallTo_OS_DirectoryRead_Returns_CFE_SUCCESS_AndCheckThat_dirent_"
        "FileName_IsDirectoryDotsFailsThenCall_CF_CFDP_FindUnusedTransaction_And_CF_CFDP_TxFile__Increment_p_num_ts_"
        "ExitLoopSet_p_busy_To_0");
}

void add_CF_CFDP_UpdatePollPbCounted_tests(void)
{
    UtTest_Add(Test_CF_CFDP_DoesNothing_pb_counted_IsEqTo_up, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_DoesNothing_pb_counted_IsEqTo_up");
    UtTest_Add(
        Test_CF_CFDP_UpdatePollPbCounted_IncrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_UpdatePollPbCounted_IncrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_1");
    UtTest_Add(
        Test_CF_CFDP_UpdatePollPbCounted_AssertsBecause_pb_counted_IsNotEqTo_up_And_up_Is_0_AndValueAt_counter_Is_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_UpdatePollPbCounted_AssertsBecause_pb_counted_IsNotEqTo_up_And_up_Is_0_AndValueAt_counter_Is_0");
    UtTest_Add(
        Test_CF_CFDP_UpdatePollPbCounted_DecrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_UpdatePollPbCounted_DecrementsValueAt_counter_Because_pb_counted_IsNotEqTo_up_And_up_Is_0");
}

void add_CF_CFDP_ProcessPlaybackDirectories_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_ProcessPlaybackDirectories_Calls_CF_CFDP_ProcessPlaybackDirectory_And_CF_CFDP_UpdatePollPbCounted_TheNumberOfTimesEqualTo_CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPlaybackDirectories_Calls_CF_CFDP_ProcessPlaybackDirectory_And_CF_CFDP_"
        "UpdatePollPbCounted_TheNumberOfTimesEqualTo_CF_MAX_COMMANDED_PLAYBACK_DIRECTORIES_PER_CHAN");
}

void add_CF_CFDP_ProcessPollingDirectories_tests(void)
{
    UtTest_Add(Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_enabled_Is_0,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_enabled_Is_0");
    UtTest_Add(
        Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_interval_sec_Is_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPollingDirectories_CallsOnly_CF_CFDP_UpdatePollPbCounted_When_pd_interval_sec_Is_0");
    UtTest_Add(
        Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_true_PlaybackIsActive_Calls_CF_CFDP_ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_true_PlaybackIsActive_Calls_CF_CFDP_"
        "ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted");
    UtTest_Add(
        Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_false_But_p_pb_num_ts_Is_true_PlaybackIsActive_Calls_CF_CFDP_ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPollingDirectories_When_p_pb_busy_Is_false_But_p_pb_num_ts_Is_true_PlaybackIsActive_Calls_"
        "CF_CFDP_ProcessPlaybackDirectory_Sets_count_check_To_1_Calls_CF_CFDP_UpdatePollPbCounted");
    UtTest_Add(
        Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_1_And_CF_Timer_Expired_Returns_0_Calls_CF_Timer_Tick,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_1_And_CF_Timer_Expired_Returns_0_Calls_CF_Timer_"
        "Tick");
    UtTest_Add(
        Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_PlaybackDir_Returns_1_EachTimeSetsAll_p_timer_set_To_0,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_"
        "PlaybackDir_Returns_1_EachTimeSetsAll_p_timer_set_To_0");
    UtTest_Add(
        Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_PlaybackDir_Returns_0_EachTimeCalls_CF_Timer_InitRelSec,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_CF_Timer_Expired_Returns_1_CallsTo_CF_CFDP_"
        "PlaybackDir_Returns_0_EachTimeCalls_CF_Timer_InitRelSec");
    UtTest_Add(
        Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_Calls_CF_Timer_InitRelSec_And_Sets_p_timer_set_To_1,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ProcessPollingDirectories_When_p_timer_set_Is_0_Calls_CF_Timer_InitRelSec_And_Sets_p_timer_set_"
        "To_1");
}

void add_CF_CFDP_CycleEngine_tests(void)
{
    UtTest_Add(Test_CF_CFDP_CycleEngine_DoesNothingWhen_engine_Is_0, cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CycleEngine_DoesNothingWhen_engine_Is_0");
    UtTest_Add(
        Test_CF_CFDP_CycleEngine_WhenOneChannelNotFrozenCalls_CF_CFDP_TickTransactions_And_CF_CFDP_CycleTx_And_CF_CFDP_ProcessPlaybackDirectories_And_CF_CFDP_ProcessPollingDirectories,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CycleEngine_WhenOneChannelNotFrozenCalls_CF_CFDP_TickTransactions_And_CF_CFDP_CycleTx_And_CF_"
        "CF_TxnState_ProcessPlaybackDirectories_And_CF_CFDP_ProcessPollingDirectories");
}

void add_CF_CFDP_ResetTransaction_tests(void)
{
    UtTest_Add(Test_CF_CFDP_ResetTransaction_AssertsBecause_t_chan_num_IsEqTo_CF_NUM_CHANNELS, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ResetTransaction_AssertsBecause_t_chan_num_IsEqTo_CF_NUM_CHANNELS");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_"
        "And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_"
        "CF_CList_InsertBack_And_CF_CFDP_FreeTransaction");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_0_And_CF_CFDP_IsSender_Returns_0_Call_OS_remove_With_dst_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_0_And_"
        "CF_CFDP_IsSender_Returns_0_Call_OS_remove_With_dst_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_"
        "Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_"
        "FreeTransaction");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_And_CF_CFDP_IsSender_Returns_non0_Call_OS_remove_With_src_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_true_Call_CF_WrappedClose_But_t_keep_Is_non0_"
        "And_CF_CFDP_IsSender_Returns_non0_Call_OS_remove_With_src_filename_And_t_history_dir_Is_CF_DIR_RX_But_keep_"
        "history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_"
        "FreeTransaction");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_But_keep_history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_But_keep_"
        "history_Is_0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_FREE_ThenCall_CF_CList_InsertBack_And_CF_CFDP_"
        "FreeTransaction");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_RX_And_keep_"
        "history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_"
        "FreeTransaction");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_AndBoth_t_flags_tx_cmd_tx_And_t_p_Are_0_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_AndBoth_"
        "t_flags_tx_cmd_tx_And_t_p_Are_0_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_"
        "CF_CList_InsertBack_And_CF_CFDP_FreeTransaction");
    UtTest_Add(Test_CF_CFDP_ResetTransaction_AssertsBecause_c_num_cmd_tx_Is_0, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_ResetTransaction_AssertsBecause_c_num_cmd_tx_Is_0");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_flags_tx_cmd_tx_Is_1_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_"
        "flags_tx_cmd_tx_Is_1_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_"
        "InsertBack_And_CF_CFDP_FreeTransaction");
    UtTest_Add(Test_CF_CFDP_ResetTransaction_AssertsBecause_t_p_num_ts_Is_0, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_ResetTransaction_AssertsBecause_t_p_num_ts_Is_0");
    UtTest_Add(
        Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_p_Is_nonNULL_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_And_CF_CFDP_FreeTransaction,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_ResetTransaction_CallTo_OS_ObjectIdDefined_Returns_false_And_t_history_dir_Is_CF_DIR_TX_And_t_p_"
        "Is_nonNULL_And_keep_history_Is_non0_Call_CF_CList_InsertBack_Ex_With_CF_Q_HIST_ThenCall_CF_CList_InsertBack_"
        "And_CF_CFDP_FreeTransaction");
    UtTest_Add(Test_CF_CFDP_ResetTransaction_AssertsBecause_t_history_dir_IsNeither_CF_DIR_TX_or_CF_DIR_RX,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_ResetTransaction_AssertsBecause_t_history_dir_IsNeither_CF_DIR_TX_or_CF_DIR_RX");
}

void add_CF_CFDP_CopyDataToLv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsGreaterThan_sizeof_dest_lv_data_Returns_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsGreaterThan_sizeof_dest_lv_data_Returns_neg1");
    UtTest_Add(Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsEqTo_sizeof_dest_lv_data_Returns_neg1, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CopyDataToLv_FailsBecause_len_IsEqTo_sizeof_dest_lv_data_Returns_neg1");
    UtTest_Add(Test_CF_CFDP_CopyDataToLv_Success_len_LessThan_sizeof_dest_lv_data_Copies_data_To_dest_lv_data,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CopyDataToLv_Success_len_LessThan_sizeof_dest_lv_data_Copies_data_To_dest_lv_data");
}

void add_CF_CFDP_CopyDataFromLv_tests(void)
{
    UtTest_Add(Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsGreaterThan_sizeof_src_lv_data_Returns_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsGreaterThan_sizeof_src_lv_data_Returns_neg1");
    UtTest_Add(Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsEqTo_sizeof_src_lv_data_Returns_neg1,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CopyDataFromLv_FailsBecause_src_lv_len_IsEqTo_sizeof_src_lv_data_Returns_neg1");
    UtTest_Add(
        Test_CF_CFDP_CF_CFDP_CopyDataFromLv_Success_src_lv_len_LessThan_sizeof_src_lv_data_Copies_src_lv_data_To_buf,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CF_CFDP_CopyDataFromLv_Success_src_lv_len_LessThan_sizeof_src_lv_data_Copies_src_lv_data_To_buf");
}

void add_CF_CFDP_CancelTransaction_tests(void)
{
    UtTest_Add(Test_CF_CFDP_CancelTransaction_DoesNothingBecause_flags_all_cancelled_IsAlreadyTrue, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CancelTransaction_DoesNothingBecause_flags_all_cancelled_IsAlreadyTrue");
    UtTest_Add(
        Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsSender,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsSender");
    UtTest_Add(
        Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsNotSender,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_CancelTransaction_Because_flags_all_canceled_IsFalse_CancelesTransaction_t_When_t_IsNotSender");
}

void add_CF_CFDP_CloseFiles_tests(void)
{
    UtTest_Add(Test_CF_CFDP_CloseFiles_DoesNothingBecause_t_fd_Is_0_Returns_CLIST_CONT, cf_cfdp_tests_Setup,
               cf_cfdp_tests_Teardown, "Test_CF_CFDP_CloseFiles_DoesNothingBecause_t_fd_Is_0_Returns_CLIST_CONT");
    UtTest_Add(Test_CF_CFDP_CloseFiles_Calls_CF_WrappedClose_Because_t_fd_Is_not0_Returns_CLIST_CONT,
               cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
               "Test_CF_CFDP_CloseFiles_Calls_CF_WrappedClose_Because_t_fd_Is_not0_Returns_CLIST_CONT");
}

void add_CF_CFDP_DisableEngine_tests(void)
{
    UtTest_Add(
        Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndNoOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndNoOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe");
    UtTest_Add(
        Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndAnyOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_DisableEngine_"
        "ClosesAllActiveFilesAndAnyOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe");
    UtTest_Add(
        Test_CF_CFDP_DisableEngine_ClosesAllActiveFilesAndAllOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe,
        cf_cfdp_tests_Setup, cf_cfdp_tests_Teardown,
        "Test_CF_CFDP_DisableEngine_"
        "ClosesAllActiveFilesAndAllOpenPlaybackDirectoriesResetsAllQueueCountersDeletesPipe");
}

/* end cf_cfdp_tests UtTest_Add groups */

/*******************************************************************************
**
**  cf_cfdp_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_CFDP_ArmAckTimer_tests();

    add_CF_CFDP_GetClass_tests();

    add_CF_CFDP_IsSender_tests();

    add_CF_CFDP_ArmInactTimer_tests();

    add_CF_CFDP_DispatchRecv_tests();

    add_CF_CFDP_DispatchTx_tests();

    add_CF_CFDP_FindUnusedChunks_tests();

    add_CF_CFDP_FindUnusedTransaction_tests();

    add_CF_CFDP_ResetHistory_tests();

    add_CF_CFDP_FreeTransaction_tests();

    add_CF_CFDP_FindTransactionBySequenceNumber__tests();

    add_CF_CFDP_FindTransactionBySequenceNumber_tests();

    add_CF_CFDP_MsgOutGet_tests();

    add_CF_CFDP_Send_tests();

    add_CF_CFDP_SetPduLength_tests();

    add_CF_CFDP_ConstructPduHeader_tests();

    add_CF_strnlen_tests();

    add_CF_CFDP_SendMd_tests();

    add_CF_CFDP_SendFd_tests();

    add_CF_CFDP_FinishEofAck_tests();

    add_CF_CFDP_SendEof_tests();

    add_CF_CFDP_SendAck_tests();

    add_CF_CFDP_SendFin_tests();

    add_CF_CFDP_SendNak_tests();

    add_CF_CF_CFDP_RecvPh_tests();

    add_CF_CFDP_RecvMd_tests();

    add_CF_CFDP_RecvFd_tests();

    add_CF_CFDP_RecvEof_tests();

    add_CF_CFDP_RecvAck_tests();

    add_CF_CFDP_RecvFin_tests();

    add_CF_CFDP_RecvNak_tests();

    add_CF_CFDP_RecvDrop_tests();

    add_CF_CFDP_RecvIdle_tests();

    add_CF_CFDP_InitEngine_tests();

    add_CF_CFDP_ReceiveMessage_tests();

    add_CF_CFDP_CycleTx__tests();

    add_CF_CFDP_CycleTx_tests();

    add_CF_CFDP_DoTick_tests();

    add_CF_CFDP_TickTransactions_tests();

    add_CF_CFDP_TxFile___tests();

    add_CF_CFDP_TxFile__tests();

    add_CF_CFDP_TxFile_tests();

    add_CF_CFDP_PlaybackDir__tests();

    add_CF_CFDP_PlaybackDir_tests();

    add_CF_CFDP_ProcessPlaybackDirectory_tests();

    add_CF_CFDP_UpdatePollPbCounted_tests();

    add_CF_CFDP_ProcessPlaybackDirectories_tests();

    add_CF_CFDP_ProcessPollingDirectories_tests();

    add_CF_CFDP_CycleEngine_tests();

    add_CF_CFDP_ResetTransaction_tests();

    add_CF_CFDP_CopyDataToLv_tests();

    add_CF_CFDP_CopyDataFromLv_tests();

    add_CF_CFDP_CancelTransaction_tests();

    add_CF_CFDP_CloseFiles_tests();

    add_CF_CFDP_DisableEngine_tests();

} /* end UtTest_Setup for cf_cfdp_tests.c */

/* end cf_cfdp_tests.c */
