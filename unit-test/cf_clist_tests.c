/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_clist.h"

/*******************************************************************************
**
**  cf_clist_tests local utility functions
**
*******************************************************************************/

typedef struct
{
    CF_CListNode_t *node;
    void           *context;
} Dummy_clist_fn_t_context_t;

typedef struct
{
    CF_CListNode_t *node;
    void           *context;
} Hook_clist_fn_t_context_t;

int Dummy_clist_fn_t(CF_CListNode_t *node, void *context)
{
    Dummy_clist_fn_t_context_t my_ctxt;

    my_ctxt.node    = node;
    my_ctxt.context = context;
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_clist_fn_t), &my_ctxt, sizeof(my_ctxt));

    /* UT_DEFAULT_IMPL returns uint32 */
    return (int)UT_DEFAULT_IMPL(Dummy_clist_fn_t);
}

int Hook_clist_fn_t(CF_CListNode_t *node, void *context)
{
    Hook_clist_fn_t_context_t my_ctxt;

    my_ctxt.node    = node;
    my_ctxt.context = context;
    UT_Stub_CopyFromLocal(UT_KEY(Hook_clist_fn_t), &my_ctxt, sizeof(my_ctxt));

    /* UT_DEFAULT_IMPL returns uint32 */
    return (int)UT_DEFAULT_IMPL(Hook_clist_fn_t);
}

/*******************************************************************************
**
**  cf_clist_tests Setup and Teardown
**
*******************************************************************************/

void cf_clist_tests_Setup(void)
{
    cf_tests_Setup();
} /* end cf_clist_tests_Setup */

void cf_clist_tests_Teardown(void)
{
    cf_tests_Teardown();
} /* end cf_clist_tests_Teardown */

/* end cf_clist_tests Setup and Teardown */

/*******************************************************************************
**
**  CF_CList_InitNode tests
**
*******************************************************************************/

void Test_CF_CList_InitNode_PointNodeToItselfAsNextAndPrev(void)
{
    /* Arrange */
    CF_CListNode_t test_node;

    test_node.next = NULL;
    test_node.prev = NULL;

    /* Act */
    CF_CList_InitNode(&test_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(test_node.prev, &test_node);
    UtAssert_ADDRESS_EQ(test_node.next, &test_node);

} /* Test_CF_CList_InitNode_PointNodeToItselfAsNextAndPrev */

/*******************************************************************************
**
**  CF_CList_InsertFront tests
**
*******************************************************************************/

void Test_CF_CList_InsertFront_AssertsBecauseHeadIs_NULL(void)
{
    // /* Arrange */
    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - head");
} /* end Test_CF_CList_InsertFront_AssertsBecauseHeadIs_NULL */

void Test_CF_CList_InsertFront_AssertsBecauseNodeIs_NULL(void)
{
    // /* Arrange */
    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert- node");
} /* end Test_CF_CList_InsertFront_AssertsBecauseNodeIs_NULL */

void Test_CF_CList_InsertFront_AssertsBecauseNodeNextDoesNotPointToItself(void)
{
    // /* Arrange */
    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - node->next==node");
} /* end Test_CF_CList_InsertFront_AssertsBecauseNodeNextDoesNotPointToItself */

void Test_CF_CList_InsertFront_AssertsBecauseNodePrevDoesNotPointToItself(void)
{
    // /* Arrange */
    // /* Act */
    // /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - node->prev==node");
} /* end Test_CF_CList_InsertFront_AssertsBecauseNodePrevDoesNotPointToItself */

void Test_CF_CList_InsertFront_InsertNodeIntoEmptyList(void)
{
    /* Arrange */
    CF_CListNode_t  *dummy_head = NULL;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertFront(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(*arg_head, arg_node);
} /* end CF_CList_InsertFront_InsertNodeIntoEmptyList */

void Test_CF_CList_InsertFront_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;

    (*arg_head)->next = dummy_head;
    (*arg_head)->prev = dummy_head;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertFront(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(arg_node->prev, &dummy_head_node);
    UtAssert_ADDRESS_EQ(arg_node->next, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.prev, arg_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.next, arg_node);
    UtAssert_ADDRESS_EQ(*arg_head, arg_node);

} /* end Test_CF_CList_InsertFront_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode */

void Test_CF_CList_InsertFront_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;
    CF_CListNode_t   dummy_last_node;

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = &dummy_head_node;

    (*arg_head)->next = &dummy_last_node;
    (*arg_head)->prev = &dummy_last_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertFront(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(arg_node->prev, &dummy_last_node);
    UtAssert_ADDRESS_EQ(arg_node->next, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.prev, arg_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.next, &dummy_last_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.prev, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.next, arg_node);
    UtAssert_ADDRESS_EQ(*arg_head, arg_node);

} /* end Test_CF_CList_InsertFront_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode */

void Test_CF_CList_InsertFront_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t  *dummy_second_node;
    CF_CListNode_t  *dummy_next_to_last_node;
    uint8            num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int              i                    = 0;

    dummy_second_node       = NULL;
    dummy_next_to_last_node = NULL;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            dummy_second_node       = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev        = dummy_next_to_last_node;
            dummy_clist_node->next        = &dummy_last_node;
            dummy_next_to_last_node       = dummy_clist_node;
        }
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertFront(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(arg_node->prev, &dummy_last_node);
    UtAssert_ADDRESS_EQ(arg_node->next, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.prev, arg_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.next, dummy_second_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.prev, dummy_next_to_last_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.next, arg_node);
    UtAssert_ADDRESS_EQ(*arg_head, arg_node);

    /* removes all malloc nodes - arg_node head (not malloc) -> next is old head (not malloc) -> next is second node
     * (malloc)  */
    CF_CListNode_t *free_up_node = arg_node->next->next;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }

} /* end Test_CF_CList_InsertFront_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode */

/*******************************************************************************
**
**  CF_CList_InsertBack tests
**
*******************************************************************************/

void Test_CF_CList_InsertBack_AssertsBecauseHeadIs_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - head");
} /* end Test_CF_CList_InsertBack_AssertsBecauseHeadIs_NULL */

void Test_CF_CList_InsertBack_AssertsBecauseNodeIs_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - node");
} /* end Test_CF_CList_InsertBack_AssertsBecauseNodeIs_NULL */

void Test_CF_CList_InsertBack_AssertsBecauseNodeNextDoesNotPointToItself(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - node->next==node");
} /* end Test_CF_CList_InsertBack_AssertsBecauseNodeNextDoesNotPointToItself */

void Test_CF_CList_InsertBack_AssertsBecauseNodePrevDoesNotPointToItself(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - node->prev==node");
} /* end Test_CF_CList_InsertBack_AssertsBecauseNodePrevDoesNotPointToItself */

void Test_CF_CList_InsertBack_InsertNodeIntoEmptyList(void)
{
    /* Arrange */
    CF_CListNode_t  *dummy_head = NULL;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertBack(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(*arg_head, arg_node);
} /* end Test_CF_CList_InsertBack_InsertNodeIntoEmptyList */

void Test_CF_CList_InsertBack_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;

    (*arg_head)->next = dummy_head;
    (*arg_head)->prev = dummy_head;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertBack(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(arg_node->prev, &dummy_head_node);
    UtAssert_ADDRESS_EQ(arg_node->next, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.prev, arg_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.next, arg_node);
    UtAssert_ADDRESS_EQ(*arg_head, dummy_head);

} /* end Test_CF_CList_InsertBack_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode */

void Test_CF_CList_InsertBack_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;
    CF_CListNode_t   dummy_last_node;

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = &dummy_head_node;

    (*arg_head)->next = &dummy_last_node;
    (*arg_head)->prev = &dummy_last_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertBack(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(arg_node->prev, &dummy_last_node);
    UtAssert_ADDRESS_EQ(arg_node->next, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.prev, arg_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.next, &dummy_last_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.prev, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.next, arg_node);
    UtAssert_ADDRESS_EQ(*arg_head, dummy_head);

} /* end Test_CF_CList_InsertBack_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode */

void Test_CF_CList_InsertBack_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t  *dummy_second_node;
    CF_CListNode_t  *dummy_next_to_last_node;
    uint8            num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int              i                    = 0;

    dummy_second_node       = NULL;
    dummy_next_to_last_node = NULL;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            dummy_second_node       = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev        = dummy_next_to_last_node;
            dummy_clist_node->next        = &dummy_last_node;
            dummy_next_to_last_node       = dummy_clist_node;
        }
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertBack(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(arg_node->prev, &dummy_last_node);
    UtAssert_ADDRESS_EQ(arg_node->next, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.prev, arg_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.next, dummy_second_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.prev, dummy_next_to_last_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.next, arg_node);
    UtAssert_ADDRESS_EQ(*arg_head, dummy_head);

    /* removes all malloc nodes */
    CF_CListNode_t *free_up_node = dummy_head->next;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }

} /* end Test_CF_CList_InsertBack_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode */

/*******************************************************************************
**
**  CF_CList_Pop tests - In File Calls:CF_CList_Remove
**
*******************************************************************************/

void Test_CF_CList_Pop_AssertsBecause_head_Is_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - head");
} /* end  Test_CF_CList_Pop_AssertsBecause_head_Is_NULL */

void Test_CF_CList_Pop_WhenListIsEmptySuccessReturn_NULL(void)
{
    /* Arrange */
    CF_CListNode_t  *dummy_head = NULL;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t  *local_result;

    /* Act */
    local_result = CF_CList_Pop(arg_head);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, NULL);
} /* end  Test_CF_CList_Pop_WhenListIsEmptySuccessReturn_NULL */

void Test_CF_CList_Pop_WhenItIsOnlyNodePopHeadNodeAndReturn_head_(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t  *local_result;

    /* Arrange unstubbable: CF_CList_Remove */
    dummy_head->prev = dummy_head;
    dummy_head->next = dummy_head;

    /* Act */
    local_result = CF_CList_Pop(arg_head);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &dummy_head_node);
} /* end  Test_CF_CList_Pop_WhenItIsOnlyNodePopHeadNodeAndReturn_head_ */

void Test_CF_CList_Pop_WhenListIsAnySizeGreaterThanOneSuccessPopsHeadNodeAndReturns_head(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t  *dummy_second_node       = &dummy_last_node;
    CF_CListNode_t  *dummy_next_to_last_node = dummy_head;
    uint8            num_extraneous_nodes    = Any_uint8_LessThan(19); // 0 to 18
    int              i                       = 0;
    CF_CListNode_t  *local_result;

    /* Arrange unstubbable: CF_CList_Remove */
    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            dummy_second_node       = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev        = dummy_next_to_last_node;
            dummy_clist_node->next        = &dummy_last_node;
            dummy_next_to_last_node       = dummy_clist_node;
        }
    }

    dummy_head_node.prev = &dummy_last_node;
    dummy_head_node.next = dummy_second_node;

    dummy_last_node.prev = dummy_next_to_last_node;
    dummy_last_node.next = dummy_head;

    /* Act */
    local_result = CF_CList_Pop(arg_head);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, &dummy_head_node);

    /* removes all malloc nodes */
    CF_CListNode_t *free_up_node = dummy_second_node;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end  Test_CF_CList_Pop_WhenListIsAnySizeGreaterThanOneSuccessPopsHeadNodeAndReturns_head */

/*******************************************************************************
**
**  CF_CList_Remove tests
**
*******************************************************************************/

void Test_CF_CList_Remove_AssertsBecauseHeadIs_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - head");
} /* end Test_CF_CList_Remove_AssertsBecauseHeadIs_NULL */

void Test_CF_CList_Remove_AssertsBecauseNodeIs_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - node");
} /* end Test_CF_CList_Remove_AssertsBecauseNodeIs_NULL */

void Test_CF_CList_Remove_AssertsBecauseHeadPointedAtValueIs_NULL(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - *head");
} /* end Test_CF_CList_Remove_AssertsBecauseHeadPointedAtValueIs_NULL */

void Test_CF_ClistRemove_AssertsBecauseHeadPointedAtValueIsNotNode(void)
{
    /* Arrange */
    CF_CListNode_t  dummy_node;
    CF_CListNode_t *arg_node = &dummy_node;

    arg_node->prev = arg_node;
    arg_node->next = arg_node;

    /* Act */
    // CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - node==*head");
} /* end Test_CF_ClistRemove_AssertsBecauseHeadPointedAtValueIsNotNode */

void Test_CF_ClistRemove_WhenOnlyNodeSetHeadTo_NULL(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_node;
    CF_CListNode_t  *arg_node = &dummy_node;
    CF_CListNode_t **arg_head = &arg_node;

    arg_node->prev = arg_node;
    arg_node->next = arg_node;

    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(*arg_head, NULL);
    /* Assert for CF_CList_InitNode - note this cannot be verified because node state does not change */
    UtAssert_ADDRESS_EQ(dummy_node.prev, &dummy_node);
    UtAssert_ADDRESS_EQ(dummy_node.next, &dummy_node);
} /* end Test_CF_ClistRemove_WhenOnlyNodeSetHeadTo_NULL */

void Test_CF_ClistRemove_WhenOnlyTwoNodesAndLastIsRemovedSetHeadToPointToItself(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_arg_node;
    CF_CListNode_t  *arg_node = &dummy_arg_node;

    dummy_head_node.prev = arg_node;
    dummy_head_node.next = arg_node;

    dummy_arg_node.prev = dummy_head;
    dummy_arg_node.next = dummy_head;

    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(*arg_head, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head->prev, dummy_head);
    UtAssert_ADDRESS_EQ(dummy_head->next, dummy_head);
    /* Assert for CF_CList_InitNode - note this cannot be verified because node state does not change */
    UtAssert_ADDRESS_EQ(arg_node->prev, arg_node);
    UtAssert_ADDRESS_EQ(arg_node->next, arg_node);
} /* end Test_CF_ClistRemove_WhenOnlyTwoNodesAndLastIsRemovedSetHeadToPointToItself */

void Test_CF_ClistRemove_RemovingHeadSetSecondNodeToHeadAndUpdateLastNode(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t  *arg_node   = *arg_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t  *dummy_second_node;
    CF_CListNode_t  *dummy_next_to_last_node;
    uint8            num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int              i                    = 0;

    dummy_second_node       = NULL;
    dummy_next_to_last_node = NULL;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            dummy_second_node       = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev        = dummy_next_to_last_node;
            dummy_clist_node->next        = &dummy_last_node;
            dummy_next_to_last_node       = dummy_clist_node;
        }
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;

    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(*arg_head, dummy_second_node);
    UtAssert_ADDRESS_EQ((*arg_head)->prev, &dummy_last_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.next, dummy_second_node);
    /* Assert for CF_CList_InitNode */
    UtAssert_ADDRESS_EQ(dummy_head_node.prev, &dummy_head_node);
    UtAssert_ADDRESS_EQ(dummy_head_node.next, &dummy_head_node);

    /* removes all malloc nodes */
    /* dummy_head is old second node (head removed) which was malloc'd */
    CF_CListNode_t *free_up_node = dummy_head;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_ClistRemove_RemovingHeadSetSecondNodeToHeadAndUpdateLastNode */

void Test_CF_ClistRemove_RemovingLastPointHeadAndNextToLastToEachOther(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t  *arg_node = &dummy_last_node;
    CF_CListNode_t  *dummy_second_node;
    CF_CListNode_t  *dummy_next_to_last_node;
    uint8            num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int              i                    = 0;

    dummy_second_node       = NULL;
    dummy_next_to_last_node = NULL;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            dummy_second_node       = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev        = dummy_next_to_last_node;
            dummy_clist_node->next        = &dummy_last_node;
            dummy_next_to_last_node       = dummy_clist_node;
        }
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;

    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(*arg_head, dummy_head);
    UtAssert_ADDRESS_EQ((*arg_head)->prev, dummy_next_to_last_node);
    UtAssert_ADDRESS_EQ(dummy_next_to_last_node->next, dummy_head);
    /* Assert for CF_CList_InitNode */
    UtAssert_ADDRESS_EQ(dummy_last_node.prev, &dummy_last_node);
    UtAssert_ADDRESS_EQ(dummy_last_node.next, &dummy_last_node);

    /* removes all malloc nodes */
    CF_CListNode_t *free_up_node = dummy_head->next;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_ClistRemove_RemovingLastPointHeadAndNextToLastToEachOther */

void Test_CF_ClistRemove_RemovingAnyNodeHasNodesPrevAndNextPointToEachOther(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t  *dummy_removed_node;
    CF_CListNode_t  *dummy_removed_node_prev;
    CF_CListNode_t  *dummy_removed_node_next;
    CF_CListNode_t  *arg_node;
    CF_CListNode_t  *dummy_second_node;
    CF_CListNode_t  *dummy_next_to_last_node;
    uint8            num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    uint8            num_of_removed_node  = Any_uint8_LessThan(num_extraneous_nodes);
    int              i                    = 0;

    dummy_removed_node      = NULL;
    arg_node                = NULL;
    dummy_second_node       = NULL;
    dummy_next_to_last_node = NULL;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == num_of_removed_node)
        {
            arg_node = dummy_removed_node = dummy_clist_node;
        }

        if (i == 0)
        {
            dummy_second_node       = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev        = dummy_next_to_last_node;
            dummy_clist_node->next        = &dummy_last_node;
            dummy_next_to_last_node       = dummy_clist_node;
        }
    }

    dummy_removed_node_prev = dummy_removed_node->prev;
    dummy_removed_node_next = dummy_removed_node->next;

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;

    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_ADDRESS_EQ(*arg_head, dummy_head);
    UtAssert_ADDRESS_EQ(dummy_removed_node_prev->next, dummy_removed_node_next);
    UtAssert_ADDRESS_EQ(dummy_removed_node_next->prev, dummy_removed_node_prev);
    /* Assert for CF_CList_InitNode */
    UtAssert_ADDRESS_EQ(dummy_removed_node->prev, dummy_removed_node);
    UtAssert_ADDRESS_EQ(dummy_removed_node->next, dummy_removed_node);

    /* removes all malloc nodes */
    CF_CListNode_t *free_up_node = dummy_head->next;
    /* free removed node because it was malloc'd */
    free(dummy_removed_node);
    /* subtract one because of node removal */
    for (i = 0; i < num_extraneous_nodes - 1; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }

} /* end Test_CF_ClistRemove_RemovingAnyNodeHasNodesPrevAndNextPointToEachOther */

void Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot(void)
{
    // /* Arrange */
    // CF_CListNode_t    dummy_head_node;
    // CF_CListNode_t *      dummy_head = &dummy_head_node;
    // CF_CListNode_t **     arg_head = &dummy_head;
    // CF_CListNode_t    dummy_last_node;
    // CF_CListNode_t *      arg_node = &dummy_last_node;

    // dummy_head->prev = dummy_head;
    // dummy_head->next = dummy_head;

    // /* setting bad node */
    // arg_node->prev = dummy_head;
    // arg_node->next = arg_node;

    // /* Act */
    // CF_CList_Remove(arg_head, arg_node);

    // /* Assert */
    // UtAssert_ADDRESS_EQ(*arg_head, &dummy_head_node);
    // UtAssert_ADDRESS_EQ(dummy_head->prev, dummy_head);
    // UtAssert_ADDRESS_EQ(dummy_head->next, dummy_head);
    // /* Assert for CF_CList_InitNode - note this cannot be verified because node state does not change */
    // UtAssert_ADDRESS_EQ(arg_node->prev, arg_node);
    // UtAssert_ADDRESS_EQ(arg_node->next, arg_node);
    UtAssert_MIR("JIRA: GSFCCFS-1719 Odd behavior - Is this desired?\n"
                 "A bad node is passed to CF_CList_Remove, but it carries on unaware\n"
                 "This was found because branch 3 of if((node->next==node)&&(node->prev==node))\n"
                 "can only be covered by this type of test, node->next == node, node-prev != node");
} /* end Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot */

/* end CF_CList_Remove tests */

/*******************************************************************************
**
**  CF_CList_InsertAfter tests
**
*******************************************************************************/

void Test_CF_CList_InsertAfter_AssertsBecause_head_Is_NULL(void)
{
    /* Arrange */
    // CF_CListNode_t **     arg_head = NULL;
    // CF_CListNode_t *      arg_start;
    // CF_CListNode_t *      arg_after;

    /* Act */
    // CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - head");
} /* end CF_CList_InsertAfter_AssertsBecause_head_Is_NULL */

void Test_CF_CList_InsertAfter_AssertsBecauseValueAt_head_Is_NULL(void)
{
    /* Arrange */
    // CF_CListNode_t *      dummy_head = NULL;
    // CF_CListNode_t **     arg_head = &dummy_head;
    // CF_CListNode_t *      arg_start;
    // CF_CListNode_t *      arg_after;

    /* Act */
    // CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - *head");
} /* end Test_CF_CList_InsertAfter_AssertsBecauseValueAt_head_Is_NULL */

void Test_CF_CList_InsertAfter_AssertsBecause_start_Is_NULL(void)
{
    /* Arrange */
    // CF_CListNode_t    dummy_head_node;
    // CF_CListNode_t *      dummy_head = &dummy_head_node;
    // CF_CListNode_t **     arg_head = &dummy_head;
    // CF_CListNode_t *      arg_start = NULL;
    // CF_CListNode_t *      arg_after;

    /* Act */
    // CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - start");
} /* end Test_CF_CList_InsertAfter_AssertsBecause_start_Is_NULL */

void Test_CF_CList_InsertAfter_AssertsBecause_start_IsEqTo_after(void)
{
    /* Arrange */
    // CF_CListNode_t    dummy_head_node;
    // CF_CListNode_t *      dummy_head = &dummy_head_node;
    // CF_CListNode_t **     arg_head = &dummy_head;
    // CF_CListNode_t    dummy_start_node;
    // CF_CListNode_t *      arg_start = &dummy_start_node;
    // CF_CListNode_t *      arg_after = arg_start;

    /* Act */
    // CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_MIR("JIRA: GSFCCFS-1733 CF_Assert - start!=after");
} /* end Test_CF_CList_InsertAfter_AssertsBecause_start_Is_NULL */

void Test_CF_CList_InsertAfter_WhenOnlyOneNodeSuccess_after_IsInsertedAfter_start(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t  *arg_start  = dummy_head;
    CF_CListNode_t   dummy_after_node;
    CF_CListNode_t  *arg_after = &dummy_after_node;

    dummy_head->prev = dummy_head;
    dummy_head->next = dummy_head;

    arg_after->prev = arg_after;
    arg_after->next = arg_after;

    /* Act */
    CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_ADDRESS_EQ(dummy_head->prev, arg_after);
    UtAssert_ADDRESS_EQ(dummy_head->next, arg_after);
    UtAssert_ADDRESS_EQ(arg_after->prev, dummy_head);
    UtAssert_ADDRESS_EQ(arg_after->next, dummy_head);
} /* end Test_CF_CList_InsertAfter_WhenOnlyOneNodeSuccess_after_IsInsertedAfter_start */

void Test_CF_CList_InsertAfter_WhenAnyNodeSuccess_after_IsInsertedAfter_start(void)
{
    /* Arrange */
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t  *dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t  *dummy_second_node;
    CF_CListNode_t  *dummy_next_to_last_node;
    CF_CListNode_t  *arg_start;
    CF_CListNode_t   dummy_after;
    CF_CListNode_t  *arg_after = &dummy_after;
    CF_CListNode_t  *dummy_after_next;
    uint8            num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    uint8            insertion_point      = Any_uint8_LessThan(num_extraneous_nodes);
    int              i                    = 0;

    dummy_second_node       = NULL;
    dummy_next_to_last_node = NULL;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            dummy_second_node       = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev        = dummy_next_to_last_node;
            dummy_clist_node->next        = &dummy_last_node;
            dummy_next_to_last_node       = dummy_clist_node;
        }
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;

    /* set insertion point */
    arg_start = dummy_head;
    for (i = 0; i < insertion_point; ++i)
    {
        arg_start = arg_start->next;
    }
    dummy_after_next = arg_start->next;

    arg_after->prev = arg_after;
    arg_after->next = arg_after;

    /* Act */
    CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_ADDRESS_EQ(arg_after->next, dummy_after_next);
    UtAssert_ADDRESS_EQ(arg_start->next, arg_after);
    UtAssert_ADDRESS_EQ(arg_after->prev, arg_start);
    UtAssert_ADDRESS_EQ(dummy_after_next->prev, arg_after);

    /* removes all malloc nodes */
    CF_CListNode_t *free_up_node = dummy_head->next;

    for (i = 0; i < num_extraneous_nodes; ++i)
    {
        /* skip inserted node that was not malloc'd */
        if (i == insertion_point)
        {
            free_up_node = free_up_node->next;
        }

        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_CList_InsertAfter_WhenAnyNodeSuccess_after_IsInsertedAfter_start */

/* end CF_CList_InsertAfter tests */

/*******************************************************************************
**
**  CF_CList_Traverse tests
**
*******************************************************************************/

void Test_CF_CList_Traverse_When_start_Is_NULL_DoNothing(void)
{
    /* Arrange */
    CF_CListNode_t *arg_start   = NULL;
    CF_CListFn_t    arg_fn      = Dummy_clist_fn_t;
    void           *arg_context = NULL;

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 0);
} /* end Test_CF_CList_Traverse_When_start_Is_NULL_DoNothing */

void Test_CF_CList_Traverse_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails(void)
{
    /* Arrange */
    CF_CListNode_t             dummy_start;
    CF_CListNode_t            *arg_start = &dummy_start;
    CF_CListFn_t               arg_fn    = Dummy_clist_fn_t;
    int                        dummy_context;
    void                      *arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t context_Dummy_clist_fn_t;

    arg_start->next = NULL;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 1); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t, sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == arg_start, "context_Dummy_clist_fn_t.node ==  arg_start");
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context, "context_Dummy_clist_fn_t.context ==  arg_context");
} /* end Test_CF_CList_Traverse_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails */

void Test_CF_CList_Traverse_WhenListIsOneNodeSuccess(void)
{
    /* Arrange */
    CF_CListNode_t             dummy_start;
    CF_CListNode_t            *arg_start = &dummy_start;
    CF_CListFn_t               arg_fn    = Dummy_clist_fn_t;
    int                        dummy_context;
    void                      *arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t context_Dummy_clist_fn_t;

    arg_start->next = arg_start;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t, sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == arg_start, "context_Dummy_clist_fn_t.node ==  arg_start");
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context, "context_Dummy_clist_fn_t.context ==  arg_context");
} /* end Test_CF_CList_Traverse_WhenListIsOneNodeSuccess */

/* NOTE: Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt may need redesigned, or
 * maybe the production code */
void Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt(void)
{
    /* Arrange */
    CF_CListNode_t            dummy_start;
    CF_CListNode_t           *arg_start = &dummy_start;
    CF_CListFn_t              arg_fn    = Hook_clist_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    Hook_clist_fn_t_context_t context_Hook_clist_fn_t;

    arg_start->next = arg_start;

    UT_SetDefaultReturnValue(UT_KEY(Hook_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Hook_clist_fn_t), &context_Hook_clist_fn_t, sizeof(context_Hook_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Hook_clist_fn_t, 1);
    UtAssert_True(context_Hook_clist_fn_t.node == arg_start, "context_Hook_clist_fn_t.node ==  arg_start");
    UtAssert_True(context_Hook_clist_fn_t.context == arg_context, "context_Hook_clist_fn_t.context ==  arg_context");
} /* end Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt */

void Test_CF_CList_Traverse_WhenListIsManyNodesErrorIn_fn_Call(void)
{
    /* Arrange */
    CF_CListNode_t            *arg_start;
    CF_CListNode_t            *adder_node;
    CF_CListFn_t               arg_fn = Dummy_clist_fn_t;
    int                        dummy_context;
    void                      *arg_context    = &dummy_context;
    uint8                      list_size      = Any_uint8_LessThan(9) + 2;     /* 2 - 10 */
    uint8                      error_location = Any_uint8_LessThan(list_size); /* 0 to list_size - 1 */
    int                        i              = 0;
    Dummy_clist_fn_t_context_t context_Dummy_clist_fn_t[list_size];

    arg_start  = NULL;
    adder_node = NULL;

    /* set up list */
    for (i = 0; i < list_size; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            arg_start = dummy_clist_node;
        }
        else if (i == 1)
        {
            adder_node       = dummy_clist_node;
            arg_start->next  = adder_node;
            adder_node->prev = arg_start;
        }
        else
        {
            dummy_clist_node->prev = adder_node;
            adder_node->next       = dummy_clist_node;
            adder_node             = dummy_clist_node;
        }

        if (i == list_size - 1)
        {
            adder_node->next = arg_start;
            arg_start->prev  = adder_node;
        }
    }

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDeferredRetcode(UT_KEY(Dummy_clist_fn_t), error_location + 1, 1);
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t, sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, error_location + 1);
    UtAssert_True(context_Dummy_clist_fn_t[0].node == arg_start, "context_Dummy_clist_fn_t[0].node ==  arg_start");
    UtAssert_True(context_Dummy_clist_fn_t[0].context == arg_context,
                  "context_Dummy_clist_fn_t[0].context ==  arg_context");

    CF_CListNode_t *expected_node = arg_start->next;
    for (i = 1; i <= error_location; ++i)
    {
        UtAssert_True(context_Dummy_clist_fn_t[i].node == expected_node,
                      "context_Dummy_clist_fn_t[i].node ==  expected_node");
        UtAssert_True(context_Dummy_clist_fn_t[i].context == arg_context,
                      "context_Dummy_clist_fn_t[i].context ==  arg_context");
        expected_node = expected_node->next;
    }

    /* removes all malloc nodes */
    CF_CListNode_t *free_up_node = arg_start;

    for (i = 0; i < list_size; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_CList_Traverse_WhenListIsManyNodesErrorIn_fn_Call */

void Test_CF_CList_Traverse_WhenListIsManyNodesSuccess(void)
{
    /* Arrange */
    CF_CListNode_t            *arg_start;
    CF_CListNode_t            *adder_node;
    CF_CListFn_t               arg_fn = Dummy_clist_fn_t;
    int                        dummy_context;
    void                      *arg_context = &dummy_context;
    uint8                      list_size   = Any_uint8_LessThan(9) + 2; /* 2 - 10 */
    int                        i           = 0;
    Dummy_clist_fn_t_context_t context_Dummy_clist_fn_t[list_size + 1];

    arg_start  = NULL;
    adder_node = NULL;

    /* set up list */
    for (i = 0; i < list_size; ++i)
    {
        CF_CListNode_t *dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            arg_start = dummy_clist_node;
        }
        else if (i == 1)
        {
            adder_node       = dummy_clist_node;
            arg_start->next  = adder_node;
            adder_node->prev = arg_start;
        }
        else
        {
            dummy_clist_node->prev = adder_node;
            adder_node->next       = dummy_clist_node;
            adder_node             = dummy_clist_node;
        }

        if (i == list_size - 1)
        {
            adder_node->next = arg_start;
            arg_start->prev  = adder_node;
        }
    }

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t, sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, list_size);
    UtAssert_True(context_Dummy_clist_fn_t[0].node == arg_start, "context_Dummy_clist_fn_t[0].node ==  arg_start");
    UtAssert_True(context_Dummy_clist_fn_t[0].context == arg_context,
                  "context_Dummy_clist_fn_t[0].context ==  arg_context");

    CF_CListNode_t *expected_node = arg_start->next;
    for (i = 1; i < list_size; ++i)
    {
        UtAssert_True(context_Dummy_clist_fn_t[i].node == expected_node,
                      "context_Dummy_clist_fn_t[i].node ==  expected_node");
        UtAssert_True(context_Dummy_clist_fn_t[i].context == arg_context,
                      "context_Dummy_clist_fn_t[i].context ==  arg_context");
        expected_node = expected_node->next;
    }

    /* removes all malloc nodes */
    CF_CListNode_t *free_up_node = arg_start;

    for (i = 0; i < list_size; ++i)
    {
        CF_CListNode_t *old_free_up_node = free_up_node;
        free_up_node                     = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_CList_Traverse_WhenListIsManyNodesSuccess */

/*******************************************************************************
**
**  CF_CList_Traverse_R tests
**
*******************************************************************************/

void Test_CF_CList_Traverse_R_When_end_Is_NULL_DoNothing(void)
{
    /* Arrange */
    CF_CListNode_t *arg_end     = NULL;
    CF_CListFn_t    arg_fn      = NULL;
    void           *arg_context = NULL;

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 0);
} /* end Test_CF_CList_Traverse_R_When_end_Is_NULL_DoNothing */

void Test_CF_CList_Traverse_R_When_end_prev_Is_NULLDoNothing(void)
{
    /* Arrange */
    CF_CListNode_t  dummy_end;
    CF_CListNode_t *arg_end = &dummy_end;
    CF_CListFn_t    arg_fn  = Dummy_clist_fn_t;
    int             dummy_context;
    void           *arg_context = &dummy_context;

    arg_end->prev = NULL;

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 0);
} /* end Test_CF_CList_Traverse_R_When_end_prev_Is_NULLDoNothing */

void Test_CF_CList_Traverse_R_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails(void)
{
    /* Arrange */
    CF_CListNode_t             dummy_end;
    CF_CListNode_t            *arg_end = &dummy_end;
    CF_CListFn_t               arg_fn  = Dummy_clist_fn_t;
    int                        dummy_context;
    void                      *arg_context = &dummy_context;
    CF_CListNode_t             dummy_end_prev_node;
    CF_CListNode_t            *dummy_end_prev = &dummy_end_prev_node;
    Dummy_clist_fn_t_context_t context_Dummy_clist_fn_t;

    arg_end->prev = dummy_end_prev;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 1); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t, sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == dummy_end_prev, "context_Dummy_clist_fn_t.node ==  dummy_end_prev");
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context, "context_Dummy_clist_fn_t.context ==  arg_context");
} /* end Test_CF_CList_Traverse_R_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails */

void Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0(void)
{
    /* Arrange */
    CF_CListNode_t             dummy_end;
    CF_CListNode_t            *arg_end = &dummy_end;
    CF_CListFn_t               arg_fn  = Dummy_clist_fn_t;
    int                        dummy_context;
    void                      *arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t context_Dummy_clist_fn_t;

    arg_end->prev = arg_end;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t, sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == arg_end, "context_Dummy_clist_fn_t.node ==  arg_end");
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context, "context_Dummy_clist_fn_t.context ==  arg_context");
} /* end Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0 */

void Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast(void)
{
    /* Arrange */
    CF_CListNode_t             dummy_node;
    CF_CListNode_t             dummy_end;
    CF_CListNode_t            *arg_end = &dummy_end;
    CF_CListFn_t               arg_fn  = Dummy_clist_fn_t;
    int                        dummy_context;
    void                      *arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t context_Dummy_clist_fn_t[2];

    arg_end->prev   = &dummy_node;
    dummy_node.prev = arg_end;

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), context_Dummy_clist_fn_t, sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 2);
    UtAssert_True(context_Dummy_clist_fn_t[0].node == &dummy_node, "context_Dummy_clist_fn_t[0].node ==  &dummy_node");
    UtAssert_True(context_Dummy_clist_fn_t[0].context == arg_context,
                  "context_Dummy_clist_fn_t[0].context ==  arg_context");
    UtAssert_True(context_Dummy_clist_fn_t[1].node == arg_end, "context_Dummy_clist_fn_t[1].node ==  arg_end");
    UtAssert_True(context_Dummy_clist_fn_t[1].context == arg_context,
                  "context_Dummy_clist_fn_t[1].context ==  arg_context");
} /* end Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast */

void Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt(void)
{
    /* Arrange */
    CF_CListNode_t            dummy_end;
    CF_CListNode_t           *arg_end = &dummy_end;
    CF_CListFn_t              arg_fn  = Hook_clist_fn_t;
    int                       dummy_context;
    void                     *arg_context = &dummy_context;
    Hook_clist_fn_t_context_t context_Hook_clist_fn_t;

    arg_end->prev = arg_end;

    UT_SetDefaultReturnValue(UT_KEY(Hook_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Hook_clist_fn_t), &context_Hook_clist_fn_t, sizeof(context_Hook_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    /* Assert */
    UtAssert_STUB_COUNT(Hook_clist_fn_t, 1);
    UtAssert_True(context_Hook_clist_fn_t.node == arg_end, "context_Hook_clist_fn_t.node ==  arg_end");
    UtAssert_True(context_Hook_clist_fn_t.context == arg_context, "context_Hook_clist_fn_t.context ==  arg_context");
} /* end Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt */

/*******************************************************************************
**
**  cf_clist_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CList_InitNode_tests(void)
{
    UtTest_Add(Test_CF_CList_InitNode_PointNodeToItselfAsNextAndPrev, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InitNode_PointNodeToItselfAsNextAndPrev");
} /* end add_CF_CList_InitNode_tests */

void add_CF_CList_InsertFront_tests(void)
{
    UtTest_Add(Test_CF_CList_InsertFront_AssertsBecauseHeadIs_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertFront_AssertsBecauseHeadIs_NULL");
    UtTest_Add(Test_CF_CList_InsertFront_AssertsBecauseNodeIs_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertFront_AssertsBecauseNodeIs_NULL");
    UtTest_Add(Test_CF_CList_InsertFront_AssertsBecauseNodeNextDoesNotPointToItself, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertFront_AssertsBecauseNodeNextDoesNotPointToItself");
    UtTest_Add(Test_CF_CList_InsertFront_AssertsBecauseNodePrevDoesNotPointToItself, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertFront_AssertsBecauseNodePrevDoesNotPointToItself");
    UtTest_Add(Test_CF_CList_InsertFront_InsertNodeIntoEmptyList, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertFront_InsertNodeIntoEmptyList");
    UtTest_Add(Test_CF_CList_InsertFront_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode, cf_clist_tests_Setup,
               cf_clist_tests_Teardown,
               "Test_CF_CList_InsertFront_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode");
    UtTest_Add(Test_CF_CList_InsertFront_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertFront_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode");
    UtTest_Add(Test_CF_CList_InsertFront_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertFront_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode");
} /* end add_CF_CList_InsertFront_tests */

void add_CF_CList_InsertBack_tests(void)
{
    UtTest_Add(Test_CF_CList_InsertBack_AssertsBecauseHeadIs_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertBack_AssertsBecauseHeadIs_NULL");
    UtTest_Add(Test_CF_CList_InsertBack_AssertsBecauseNodeIs_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertBack_AssertsBecauseNodeIs_NULL");
    UtTest_Add(Test_CF_CList_InsertBack_AssertsBecauseNodeNextDoesNotPointToItself, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertBack_AssertsBecauseNodeNextDoesNotPointToItself");
    UtTest_Add(Test_CF_CList_InsertBack_AssertsBecauseNodePrevDoesNotPointToItself, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertBack_AssertsBecauseNodePrevDoesNotPointToItself");
    UtTest_Add(Test_CF_CList_InsertBack_InsertNodeIntoEmptyList, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertBack_InsertNodeIntoEmptyList");
    UtTest_Add(Test_CF_CList_InsertBack_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertBack_WhenHeadIsOnlyNodeAndTheyPointToEachOtherInsertNode");
    UtTest_Add(Test_CF_CList_InsertBack_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertBack_WhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodesInsertNode");
    UtTest_Add(Test_CF_CList_InsertBack_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertBack_WhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodesInsertNode");
} /* end add_CF_CList_InsertBack_tests */

void add_CF_CList_Pop_tests(void)
{
    UtTest_Add(Test_CF_CList_Pop_AssertsBecause_head_Is_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Pop_AssertsBecause_head_Is_NULL");
    UtTest_Add(Test_CF_CList_Pop_WhenListIsEmptySuccessReturn_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Pop_WhenListIsEmptySuccessReturn_NULL");
    UtTest_Add(Test_CF_CList_Pop_WhenItIsOnlyNodePopHeadNodeAndReturn_head_, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_Pop_WhenItIsOnlyNodePopHeadNodeAndReturn_head_");
    UtTest_Add(Test_CF_CList_Pop_WhenListIsAnySizeGreaterThanOneSuccessPopsHeadNodeAndReturns_head,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Pop_WhenListIsAnySizeGreaterThanOneSuccessPopsHeadNodeAndReturns_head");
} /* end add_CF_CList_Pop_tests */

void add_CF_CList_Remove_tests(void)
{
    UtTest_Add(Test_CF_CList_Remove_AssertsBecauseHeadIs_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Remove_AssertsBecauseHeadIs_NULL");
    UtTest_Add(Test_CF_CList_Remove_AssertsBecauseNodeIs_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Remove_AssertsBecauseNodeIs_NULL");
    UtTest_Add(Test_CF_CList_Remove_AssertsBecauseHeadPointedAtValueIs_NULL, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_Remove_AssertsBecauseHeadPointedAtValueIs_NULL");
    UtTest_Add(Test_CF_ClistRemove_AssertsBecauseHeadPointedAtValueIsNotNode, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_ClistRemove_AssertsBecauseHeadPointedAtValueIsNotNode");
    UtTest_Add(Test_CF_ClistRemove_WhenOnlyNodeSetHeadTo_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_ClistRemove_WhenOnlyNodeSetHeadTo_NULL");
    UtTest_Add(Test_CF_ClistRemove_WhenOnlyTwoNodesAndLastIsRemovedSetHeadToPointToItself, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_ClistRemove_WhenOnlyTwoNodesAndLastIsRemovedSetHeadToPointToItself");
    UtTest_Add(Test_CF_ClistRemove_RemovingHeadSetSecondNodeToHeadAndUpdateLastNode, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_ClistRemove_RemovingHeadSetSecondNodeToHeadAndUpdateLastNode");
    UtTest_Add(Test_CF_ClistRemove_RemovingLastPointHeadAndNextToLastToEachOther, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_ClistRemove_RemovingLastPointHeadAndNextToLastToEachOther");
    UtTest_Add(Test_CF_ClistRemove_RemovingAnyNodeHasNodesPrevAndNextPointToEachOther, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_ClistRemove_RemovingAnyNodeHasNodesPrevAndNextPointToEachOther");
    UtTest_Add(Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot, cf_clist_tests_Setup,
               cf_clist_tests_Teardown,
               "Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot");
} /* end add_CF_CList_Remove_tests */

void add_CF_CList_InsertAfter_tests(void)
{
    UtTest_Add(Test_CF_CList_InsertAfter_AssertsBecause_head_Is_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertAfter_AssertsBecause_head_Is_NULL");
    UtTest_Add(Test_CF_CList_InsertAfter_AssertsBecauseValueAt_head_Is_NULL, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertAfter_AssertsBecause_head_Is_NULL");
    UtTest_Add(Test_CF_CList_InsertAfter_AssertsBecause_start_Is_NULL, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_InsertAfter_AssertsBecause_head_Is_NULL");
    UtTest_Add(Test_CF_CList_InsertAfter_AssertsBecause_start_IsEqTo_after, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertAfter_AssertsBecause_head_Is_NULL");
    UtTest_Add(Test_CF_CList_InsertAfter_WhenOnlyOneNodeSuccess_after_IsInsertedAfter_start, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertAfter_WhenOnlyOneNodeSuccess_after_IsInsertedAfter_start");
    UtTest_Add(Test_CF_CList_InsertAfter_WhenAnyNodeSuccess_after_IsInsertedAfter_start, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertAfter_WhenAnyNodeSuccess_after_IsInsertedAfter_start");
} /* end add_CF_CList_InsertAfter_tests */

void add_CF_CList_Traverse_tests(void)
{
    UtTest_Add(Test_CF_CList_Traverse_When_start_Is_NULL_DoNothing, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_When_start_Is_NULL_DoNothing");
    UtTest_Add(Test_CF_CList_Traverse_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_Traverse_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails");
    UtTest_Add(Test_CF_CList_Traverse_WhenListIsOneNodeSuccess, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_WhenListIsOneNodeSuccess");
    UtTest_Add(Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt");
    UtTest_Add(Test_CF_CList_Traverse_WhenListIsManyNodesErrorIn_fn_Call, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_WhenListIsManyNodesErrorIn_fn_Call");
    UtTest_Add(Test_CF_CList_Traverse_WhenListIsManyNodesSuccess, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_WhenListIsManyNodesSuccess");
} /* end add_CF_CList_Traverse_tests */

void add_CF_CList_Traverse_R_tests(void)
{
    UtTest_Add(Test_CF_CList_Traverse_R_When_end_Is_NULL_DoNothing, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_R_When_end_Is_NULL_DoNothing");
    UtTest_Add(Test_CF_CList_Traverse_R_When_end_prev_Is_NULLDoNothing, cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_R_When_end_prev_Is_NULLDoNothing");
    UtTest_Add(Test_CF_CList_Traverse_R_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_Traverse_R_WhenListIsMoreThanOneNodeErrorOutFirst_fn_CallFails");
    UtTest_Add(Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0");
    UtTest_Add(Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast");
    UtTest_Add(Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt,
               cf_clist_tests_Setup, cf_clist_tests_Teardown,
               "Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt");
} /* end add_CF_CList_Traverse_R_tests */

/*******************************************************************************
**
**  cf_clist_tests UtTest_Setup
**
*******************************************************************************/

void UtTest_Setup(void)
{
    TestUtil_InitializeRandomSeed();

    add_CF_CList_InitNode_tests();

    add_CF_CList_InsertFront_tests();

    add_CF_CList_InsertBack_tests();

    add_CF_CList_Pop_tests();

    add_CF_CList_Remove_tests();

    add_CF_CList_InsertAfter_tests();

    add_CF_CList_Traverse_tests();

    add_CF_CList_Traverse_R_tests();
} /* end UtTest_Setup for cf_clist_tests.c */

/* end cf_clist_tests.c */
