
/* cf testing includes */
#include "cf_clist.c"
#include "cf_test_utils.h"


/*******************************************************************************
**
**  cf_clist_tests local utility functions
**
*******************************************************************************/
/* TODO: these copy from local usages may need changed to context */
int Dummy_clist_fn_t(clist_node node, void*context)
{
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_clist_fn_t), &node, sizeof(node));
    UT_Stub_CopyFromLocal(UT_KEY(Dummy_clist_fn_t), &context, sizeof(context));

    /* UT_DEFAULT_IMPL returns uint32 */
    return (int) UT_DEFAULT_IMPL(Dummy_clist_fn_t);
}

int Hook_clist_fn_t(clist_node node, void*context)
{

    /* Modification of node to meet n->next!=nn as true for coverage */
    node->next = NULL;
    node->prev = NULL;

    UT_Stub_CopyFromLocal(UT_KEY(Hook_clist_fn_t), &node, sizeof(node));
    UT_Stub_CopyFromLocal(UT_KEY(Hook_clist_fn_t), &context, sizeof(context));

    /* UT_DEFAULT_IMPL returns uint32 */
    return (int) UT_DEFAULT_IMPL(Hook_clist_fn_t);
}

typedef struct {
    clist_node node;
    void* context;
} CF_PACK Dummy_clist_fn_t_context_t;

typedef struct {
    clist_node node;
    void* context;
} CF_PACK Hook_clist_fn_t_context_t;

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


void Test_CF_CList_InitNode_PointsNodeToItselfAsNextAndPrev(void)
{
    /* Arrange */
    clist_node_t test_node;
    
    test_node.next = NULL;
    test_node.prev = NULL;
    
    /* Act */
    CF_CList_InitNode(&test_node);
    
    /* Assert */      
    UtAssert_True(test_node.prev == &test_node,
      "CF_CList_InitNode pointed test_node->prev to %p which is itself %p",
      test_node.prev, &test_node);
    UtAssert_True(test_node.next == &test_node,
      "CF_CList_InitNode pointed test_node->next to %p which is itself %p",
      test_node.next, &test_node);
    
} /* Test_CF_CList_InitNode_PointsNodeToItselfAsNextAndPrev */

/*******************************************************************************
**
**  CF_CList_InsertFront tests
**
*******************************************************************************/

void Test_CF_CList_InsertFront_FailsAssertHeadIsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end CF_CList_InsertFront_FailsAssertHeadIsNull */

void Test_CF_CList_InsertFront_FailsAssertNodeIsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertFront_FailsAssertNodeIsNull */

void Test_CF_CList_InsertFront_FailsAssertNodeNextDoesNotPointToItself(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertFront_FailsAssertNodeNextDoesNotPointToItself */

void Test_CF_CList_InsertFront_FailsAssertNodePrevDoesNotPointToItself(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertFront_FailsAssertNodePrevDoesNotPointToItself */

void Test_CF_CList_InsertFront_InsertsNodeIntoEmptyList(void)
{
    /* Arrange */
    clist_node      dummy_head = NULL;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertFront(arg_head, arg_node);

    /* Assert */
    UtAssert_True(*arg_head == arg_node,
      "head is now %p and should have been changed to %p (node)",
      *arg_head, arg_node);
} /* end CF_CList_InsertFront_InsertsNodeIntoEmptyList */

void Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;
    
    (*arg_head)->next = dummy_head;
    (*arg_head)->prev = dummy_head;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertFront(arg_head, arg_node);

    /* Assert */
    UtAssert_True(arg_node->prev == &dummy_head_node,
      "node->prev is now %p and should have been changed to %p (last)",
      arg_node->prev, &dummy_head_node);
    UtAssert_True(arg_node->next == &dummy_head_node,
      "node->next is now %p and should have been changed to %p (*head before call)",
      arg_node->next, &dummy_head_node);
    UtAssert_True(dummy_head_node.prev == arg_node,
      "original head.prev is now %p and should have been changed to %p (node)",
      dummy_head_node.prev, arg_node);
    UtAssert_True(dummy_head_node.next == arg_node,
      "original head.next is now %p should have been changed to %p (node)",
      dummy_head_node.next, arg_node);
    UtAssert_True(*arg_head == arg_node,
      "head is now %p and should have been changed to %p (node)",
      *arg_head, arg_node);

} /* end Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther */

void Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodes(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;
    clist_node_t    dummy_last_node;

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = &dummy_head_node;
    
    (*arg_head)->next = &dummy_last_node;
    (*arg_head)->prev = &dummy_last_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertFront(arg_head, arg_node);

    /* Assert */
    UtAssert_True(arg_node->prev == &dummy_last_node,
      "node->prev is now %p and should have been changed to %p (last)",
      arg_node->prev, &dummy_last_node);
    UtAssert_True(arg_node->next == &dummy_head_node,
      "node->next is now %p and should have been changed to %p (*head before call)",
      arg_node->next, &dummy_head_node);
    UtAssert_True(dummy_head_node.prev == arg_node,
      "original head.prev is now %p and should have been changed to %p (node)",
      dummy_head_node.prev, arg_node);
    UtAssert_True(dummy_head_node.next == &dummy_last_node,
      "original head.next is %p and should not have changed from %p (last)",
      dummy_head_node.next, &dummy_last_node);
    UtAssert_True(dummy_last_node.prev == &dummy_head_node,
      "last.prev is %p and should not have changed from %p (*head before call)",
      dummy_head_node.next, &dummy_last_node);
    UtAssert_True(dummy_last_node.next == arg_node,
      "last.next is %p and should have been change to %p (node)",
      dummy_head_node.next, &dummy_last_node);
    UtAssert_True(*arg_head == arg_node,
      "head is now %p and should have been changed to %p (node)",
      *arg_head, arg_node);

} /* end Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther */

void Test_CF_CList_InsertFront_InsertsNodeWhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodes(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;
    clist_node_t    dummy_last_node;
    clist_node      dummy_second_node;
    clist_node      dummy_next_to_last_node;
    uint8           num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int             i = 0;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));
        
        if (i == 0)
        {
            dummy_second_node = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev = dummy_next_to_last_node;
            dummy_clist_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_clist_node;
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
    UtAssert_True(arg_node->prev == &dummy_last_node,
      "node->prev is now %p and should have been changed to %p (last)",
      arg_node->prev, &dummy_last_node);
    UtAssert_True(arg_node->next == &dummy_head_node,
      "node->next is now %p and should have been changed to %p (*head before call)",
      arg_node->next, &dummy_head_node);
    UtAssert_True(dummy_head_node.prev == arg_node,
      "original head.prev is now %p and should have been changed to %p (node)",
      dummy_head_node.prev, arg_node);
    UtAssert_True(dummy_head_node.next == dummy_second_node,
      "original head.next is %p and should not have changed from %p (second)",
      dummy_head_node.next, dummy_second_node);
    UtAssert_True(dummy_last_node.prev == dummy_next_to_last_node,
      "last.prev is %p and should not have changed from %p (next to last)",
      dummy_last_node.prev, dummy_next_to_last_node);
    UtAssert_True(dummy_last_node.next == arg_node,
      "last.next is %p and should have been change to %p (node)",
      dummy_last_node.next, arg_node);
    UtAssert_True(*arg_head == arg_node,
      "head is now %p and should have been changed to %p (node)",
      *arg_head, arg_node);

    /* removes all malloc nodes - arg_node head (not malloc) -> next is old head (not malloc) -> next is second node (malloc)  */
    clist_node  free_up_node = arg_node->next->next; 
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }

} /* end Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther */

/*******************************************************************************
**
**  CF_CList_InsertBack tests
**
*******************************************************************************/

void Test_CF_CList_InsertBack_FailsAssertHeadIsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertBack_FailsAssertHeadIsNull */

void Test_CF_CList_InsertBack_FailsAssertNodeIsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertBack_FailsAssertNodeIsNull */

void Test_CF_CList_InsertBack_FailsAssertNodeNextDoesNotPointToItself(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertBack_FailsAssertNodeNextDoesNotPointToItself */

void Test_CF_CList_InsertBack_FailsAssertNodePrevDoesNotPointToItself(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertBack_FailsAssertNodePrevDoesNotPointToItself */

void Test_CF_CList_InsertBack_InsertsNodeIntoEmptyList(void)
{
    /* Arrange */
    clist_node      dummy_head = NULL;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertBack(arg_head, arg_node);

    /* Assert */
    UtAssert_True(*arg_head == arg_node,
      "head is now %p and should have been changed to %p (node)",
      *arg_head, arg_node);
} /* end CF_CList_InsertFront_InsertsNodeIntoEmptyList */

void Test_CF_CList_InsertBack_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;
    
    (*arg_head)->next = dummy_head;
    (*arg_head)->prev = dummy_head;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertBack(arg_head, arg_node);

    /* Assert */
    UtAssert_True(arg_node->prev == &dummy_head_node,
      "node->prev is now %p and should have been changed to %p (last before call)",
      arg_node->prev, &dummy_head_node);
    UtAssert_True(arg_node->next == &dummy_head_node,
      "node->next is now %p and should have been changed to %p (*head)",
      arg_node->next, &dummy_head_node);
    UtAssert_True(dummy_head_node.prev == arg_node,
      "original head.prev is now %p and should have been changed to %p (node)",
      dummy_head_node.prev, arg_node);
    UtAssert_True(dummy_head_node.next == arg_node,
      "original head.next is now %p should have been changed to %p (node)",
      dummy_head_node.next, arg_node);
    UtAssert_True(*arg_head == dummy_head,
      "head is %p and should not have changed from %p (*head)",
      *arg_head, dummy_head);

} /* end Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther */

void Test_CF_CList_InsertBack_InsertsNodeWhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodes(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;
    clist_node_t    dummy_last_node;

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = &dummy_head_node;
    
    (*arg_head)->next = &dummy_last_node;
    (*arg_head)->prev = &dummy_last_node;

    arg_node->next = &dummy_node;
    arg_node->prev = &dummy_node;

    /* Act */
    CF_CList_InsertBack(arg_head, arg_node);

    /* Assert */
    UtAssert_True(arg_node->prev == &dummy_last_node,
      "node->prev is now %p and should have been changed to %p (last before call)",
      arg_node->prev, &dummy_last_node);
    UtAssert_True(arg_node->next == &dummy_head_node,
      "node->next is now %p and should have been changed to %p (*head)",
      arg_node->next, &dummy_head_node);
    UtAssert_True(dummy_head_node.prev == arg_node,
      "original head.prev is now %p and should have been changed to %p (node)",
      dummy_head_node.prev, arg_node);
    UtAssert_True(dummy_head_node.next == &dummy_last_node,
      "original head.next is %p and should not have changed from %p (last before call)",
      dummy_head_node.next, &dummy_last_node);
    UtAssert_True(dummy_last_node.prev == &dummy_head_node,
      "last.prev is %p and should not have changed from %p (*head)",
      dummy_head_node.next, &dummy_last_node);
    UtAssert_True(dummy_last_node.next == arg_node,
      "last.next is %p and should have been change to %p (node)",
      dummy_head_node.next, &dummy_last_node);
    UtAssert_True(*arg_head == dummy_head,
      "head is %p and should not have changed from %p (*head)",
      *arg_head, dummy_head);

} /* end Test_CF_CList_InsertBack_InsertsNodeWhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodes */

void Test_CF_CList_InsertBack_InsertsNodeWhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodes(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;
    clist_node_t    dummy_last_node;
    clist_node      dummy_second_node;
    clist_node      dummy_next_to_last_node;
    uint8           num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int             i = 0;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));
        
        if (i == 0)
        {
            dummy_second_node = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev = dummy_next_to_last_node;
            dummy_clist_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_clist_node;
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
    UtAssert_True(arg_node->prev == &dummy_last_node,
      "node->prev is now %p and should have been changed to %p (last before call)",
      arg_node->prev, &dummy_last_node);
    UtAssert_True(arg_node->next == &dummy_head_node,
      "node->next is now %p and should have been changed to %p (*head)",
      arg_node->next, &dummy_head_node);
    UtAssert_True(dummy_head_node.prev == arg_node,
      "original head.prev is now %p and should have been changed to %p (node)",
      dummy_head_node.prev, arg_node);
    UtAssert_True(dummy_head_node.next == dummy_second_node,
      "original head.next is %p and should not have changed from %p (second)",
      dummy_head_node.next, dummy_second_node);
    UtAssert_True(dummy_last_node.prev == dummy_next_to_last_node,
      "last.prev is %p and should not have changed from %p (next to last)",
      dummy_last_node.prev, dummy_next_to_last_node);
    UtAssert_True(dummy_last_node.next == arg_node,
      "last.next is %p and should have been change to %p (node)",
      dummy_last_node.next, arg_node);
    UtAssert_True(*arg_head == dummy_head,
      "head is %p and should not have changed from %p (*head)",
      *arg_head, dummy_head);

    /* removes all malloc nodes */
    clist_node  free_up_node = dummy_head->next;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }

} /* end Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther */

/*******************************************************************************
**
**  CF_CList_Pop tests - In File Calls:CF_CList_Remove
**
*******************************************************************************/

void Test_CF_CList_Pop_FailsAssert_head_IsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end  Test_CF_CList_Pop_FailsAssert_head_IsNull */

void Test_CF_CList_Pop_SuccessReturnsNullBecauseListIsEmpty(void)
{
    /* Arrange */
    clist_node      dummy_head = NULL;
    clist_node*     arg_head = &dummy_head;
    clist_node      local_result;
    
    /* Act */
    local_result = CF_CList_Pop(arg_head);

    /* Assert */
    UtAssert_True(local_result == NULL,
      "CF_CList_Pop returned %p and should be %p (NULL, list was empty)",
      local_result, NULL);
} /* end  Test_CF_CList_Pop_SuccessReturnsNullBecauseListIsEmpty */

void Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenItIsOnlyNode(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node      local_result;
    
    /* Arrange unstubbable: CF_CList_Remove */
    dummy_head->prev = dummy_head;
    dummy_head->next = dummy_head;

    /* Act */
    local_result = CF_CList_Pop(arg_head);
    
    /* Assert */
    UtAssert_True(local_result == &dummy_head_node,
      "CF_CList_Pop returned %p and should be %p (head)",
      local_result, &dummy_head_node);
} /* end  Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenItIsOnlyNode */

void Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenListIsAnySizeGreaterThanOne(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_last_node;
    clist_node      dummy_second_node = &dummy_last_node;
    clist_node      dummy_next_to_last_node = dummy_head;
    uint8           num_extraneous_nodes = Any_uint8_LessThan(19); // 0 to 18
    int             i = 0;
    clist_node      local_result;

    /* Arrange unstubbable: CF_CList_Remove */   
    for(i = 0; i < num_extraneous_nodes; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));
        
        if (i == 0)
        {
            dummy_second_node = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev = dummy_next_to_last_node;
            dummy_clist_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_clist_node;
        }
        
    }

    dummy_head_node.prev = &dummy_last_node;
    dummy_head_node.next = dummy_second_node;

    dummy_last_node.prev = dummy_next_to_last_node;
    dummy_last_node.next = dummy_head;

    /* Act */
    local_result = CF_CList_Pop(arg_head);
    
    /* Assert */
    UtAssert_True(local_result == &dummy_head_node,
      "CF_CList_Pop returned %p and should be %p (head)",
      local_result, &dummy_head_node);

    /* removes all malloc nodes */
    clist_node  free_up_node = dummy_second_node; 
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end  Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenItIsOnlyNode */

/*******************************************************************************
**
**  CF_CList_Remove tests
**
*******************************************************************************/

void Test_CF_CList_Remove_FailsAssertHeadPointedAtValueIsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_Remove_FailsAssertHeadPointedAtValueIsNull */

void Test_CF_CList_Remove_FailsAssertNodeIsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_Remove_FailsAssertNodeIsNull */

void Test_CF_CList_Remove_FailsAssertHeadIsNull(void)
{
    /* Arrange */
    /* Act */
    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_Remove_FailsAssertHeadIsNull */


void Test_CF_ClistRemove_FailsAssertHeadPointedAtValueIsNotNode(void)
{
    /* Arrange */
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;

    arg_node->prev = arg_node;
    arg_node->next = arg_node;
    
    /* Act */
    //CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_ClistRemove_FailsAssertHeadPointedAtValueIsNotNode */


void Test_CF_ClistRemove_SetsHeadToNullWhenOnlyNode(void)
{
    /* Arrange */
    clist_node_t    dummy_node;
    clist_node      arg_node = &dummy_node;
    clist_node*     arg_head = &arg_node;

    arg_node->prev = arg_node;
    arg_node->next = arg_node;
    
    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_True(*arg_head == NULL,
      "*head is %p and should be %p (NULL)",
      *arg_head, NULL);
    /* Assert for CF_CList_InitNode - note this cannot be verified because node state does not change */
    UtAssert_True(dummy_node.prev == &dummy_node,
      "CF_CList_InitNode set node->prev to %p and should be %p (node before call)",
      dummy_node.prev, &dummy_node);
    UtAssert_True(dummy_node.next == &dummy_node,
      "CF_CList_InitNode set node->next to %p and should be %p (node before call)",
      dummy_node.next, &dummy_node);
} /* end Test_CF_ClistRemove_SetsHeadToNullWhenOnlyNode */

void Test_CF_ClistRemove_SetsHeadToPointToItselWhenOnlyTwoNodesAndLastIsRemoved(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_arg_node;
    clist_node      arg_node = &dummy_arg_node;

    dummy_head_node.prev = arg_node;
    dummy_head_node.next = arg_node;

    dummy_arg_node.prev = dummy_head;
    dummy_arg_node.next = dummy_head;

    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_True(*arg_head == &dummy_head_node,
      "*head is %p and should be %p (head)",
      *arg_head, &dummy_head_node);
    UtAssert_True(dummy_head->prev == dummy_head,
      "CF_CList_InitNode set head->prev to %p and should be %p (head)",
      dummy_head->prev, dummy_head);
    UtAssert_True(dummy_head->next == dummy_head,
      "CF_CList_InitNode set head->next to %p and should be %p (head)",
      dummy_head->next, dummy_head);
    /* Assert for CF_CList_InitNode - note this cannot be verified because node state does not change */
    UtAssert_True(arg_node->prev == arg_node,
      "CF_CList_InitNode set node->prev to %p and should be %p (node)",
      arg_node->prev, arg_node);
    UtAssert_True(arg_node->next == arg_node,
      "CF_CList_InitNode set node->next to %p and should be %p (node)",
      arg_node->next, arg_node);
} /* end Test_CF_ClistRemove_SetsHeadToPointToItselWhenOnlyTwoNodesAndLastIsRemoved */

void Test_CF_ClistRemove_RemovingHeadSetsSecondNodeToHeadAndUpdatesLastNode(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node      arg_node = *arg_head;
    clist_node_t    dummy_last_node;
    clist_node      dummy_second_node;
    clist_node      dummy_next_to_last_node;
    uint8           num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int             i = 0;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));
        
        if (i == 0)
        {
            dummy_second_node = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev = dummy_next_to_last_node;
            dummy_clist_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_clist_node;
        }
        
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;
    
    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;
    
    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_True(*arg_head == dummy_second_node,
      "*head is %p and should be %p (second node)",
      *arg_head, dummy_second_node);
    UtAssert_True((*arg_head)->prev == &dummy_last_node,
      "*head->prev is %p and should have been changed to %p (last node)",
      (*arg_head)->prev, &dummy_last_node);
    UtAssert_True(dummy_last_node.next == dummy_second_node,
      "last.next is %p and should have been changed to %p (second node)",
      *arg_head, dummy_second_node);
    /* Assert for CF_CList_InitNode */
    UtAssert_True(dummy_head_node.prev == &dummy_head_node,
      "CF_CList_InitNode set node->prev to %p and should be %p (node before call)",
      dummy_head_node.prev, &dummy_head_node);
    UtAssert_True(dummy_head_node.next == &dummy_head_node,
      "CF_CList_InitNode set node->next to %p and should be %p (node before call)",
      dummy_head_node.next, &dummy_head_node);

    /* removes all malloc nodes */
    /* dummy_head is old second node (head removed) which was malloc'd */
    clist_node  free_up_node = dummy_head;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_ClistRemove_RemovingHeadSetsSecondNodeToHeadAndUpdatesLastNode */

void Test_CF_ClistRemove_RemovingLastPointsHeadAndNextToLastToEachOther(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_last_node;
    clist_node      arg_node = &dummy_last_node;
    clist_node      dummy_second_node;
    clist_node      dummy_next_to_last_node;
    uint8           num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    int             i = 0;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));
        
        if (i == 0)
        {
            dummy_second_node = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev = dummy_next_to_last_node;
            dummy_clist_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_clist_node;
        }
        
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;
    
    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_True(*arg_head == dummy_head,
      "*head is %p and should not have changed from %p (*head before call)",
      *arg_head, dummy_head);
    UtAssert_True((*arg_head)->prev == dummy_next_to_last_node,
      "*head->prev is %p and should have been changed to %p (next to last node)",
      (*arg_head)->prev, dummy_next_to_last_node);
    UtAssert_True(dummy_next_to_last_node->next == dummy_head,
      "new last.next is %p and should have been changed to %p (*head)",
      dummy_next_to_last_node->next, dummy_head);
    /* Assert for CF_CList_InitNode */
    UtAssert_True(dummy_last_node.prev == &dummy_last_node,
      "CF_CList_InitNode set node->prev to %p and should be %p (node before call)",
      dummy_last_node.prev, &dummy_last_node);
    UtAssert_True(dummy_last_node.next == &dummy_last_node,
      "CF_CList_InitNode set node->next to %p and should be %p (node before call)",
      dummy_last_node.next, &dummy_last_node);

    /* removes all malloc nodes */
    clist_node  free_up_node = dummy_head->next;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_ClistRemove_RemovingLastPointsHeadAndNextToLastToEachOther */

void Test_CF_ClistRemove_RemovingAnyNodeHasNodesPrevAndNextPointToEachOther(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_last_node;
    clist_node      dummy_removed_node;
    clist_node      dummy_removed_node_prev;
    clist_node      dummy_removed_node_next;
    clist_node      arg_node;
    clist_node      dummy_second_node;
    clist_node      dummy_next_to_last_node;
    uint8           num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    uint8           num_of_removed_node = Any_uint8_LessThan(num_extraneous_nodes);
    int             i = 0;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == num_of_removed_node)
        {
            arg_node = dummy_removed_node = dummy_clist_node;
        }
        
        if (i == 0)
        {
            dummy_second_node = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev = dummy_next_to_last_node;
            dummy_clist_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_clist_node;
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
    UtAssert_True(*arg_head == dummy_head,
      "*head is %p and should not have changed from %p (*head before call)",
      *arg_head, dummy_head);
    UtAssert_True(dummy_removed_node_prev->next == dummy_removed_node_next,
      "removed node's previous node next is %p and should have been changed to %p (node->next before call)",
      dummy_removed_node_prev->next, dummy_removed_node_next);
    UtAssert_True(dummy_removed_node_next->prev == dummy_removed_node_prev,
      "removed node's next node prev is %p and should have been changed to %p (node->prev before call)",
      dummy_removed_node_next->prev, dummy_removed_node_prev);
    /* Assert for CF_CList_InitNode */
    UtAssert_True(dummy_removed_node->prev == dummy_removed_node,
      "CF_CList_InitNode set node->prev to %p and should be %p (node before call)",
      dummy_removed_node->prev, dummy_removed_node);
    UtAssert_True(dummy_removed_node->next == dummy_removed_node,
      "CF_CList_InitNode set node->next to %p and should be %p (node before call)",
      dummy_removed_node->next, dummy_removed_node);

    /* removes all malloc nodes */
    clist_node  free_up_node = dummy_head->next;
    /* free removed node because it was malloc'd */
    free(dummy_removed_node);
    /* subtract one because of node removal */
    for(i = 0; i < num_extraneous_nodes - 1; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }

} /* end Test_CF_ClistRemove_RemovingLastPointsHeadAndNextToLastToEachOther */

/* TODO: Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot is not curently running because there is doubt that a bad node can make it to this function - revisit when this is determined */
void Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_last_node;
    clist_node      arg_node = &dummy_last_node;

    dummy_head->prev = dummy_head;
    dummy_head->next = dummy_head;

    /* setting bad node */
    arg_node->prev = dummy_head;
    arg_node->next = arg_node;

    /* Act */
    CF_CList_Remove(arg_head, arg_node);

    /* Assert */
    UtAssert_Failed("Odd behavior - Is this desired?\n"
                    "A bad node is passed to CF_CList_Remove, but it carries on unaware\n"
                    "This was found because branch 3 of if((node->next==node)&&(node->prev==node))\n"
                    "can only be covered by this type of test, node->next == node, node-prev != node");
    UtAssert_True(*arg_head == &dummy_head_node,
      "*head is %p and should be %p (head)",
      *arg_head, &dummy_head_node);
    UtAssert_True(dummy_head->prev == dummy_head,
      "CF_CList_InitNode set head->prev to %p and should be %p (head)",
      dummy_head->prev, dummy_head);
    UtAssert_True(dummy_head->next == dummy_head,
      "CF_CList_InitNode set head->next to %p and should be %p (head)",
      dummy_head->next, dummy_head);
    /* Assert for CF_CList_InitNode - note this cannot be verified because node state does not change */
    UtAssert_True(arg_node->prev == arg_node,
      "CF_CList_InitNode set node->prev to %p and should be %p (node)",
      arg_node->prev, arg_node);
    UtAssert_True(arg_node->next == arg_node,
      "CF_CList_InitNode set node->next to %p and should be %p (node)",
      arg_node->next, arg_node);
} /* end Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot */


/*******************************************************************************
**
**  CF_CList_InsertAfter tests
**
*******************************************************************************/

void Test_CF_CList_InsertAfter_FailsAssert_head_IsNull(void)
{
    /* Arrange */
    // clist_node*     arg_head = NULL;
    // clist_node      arg_start;
    // clist_node      arg_after;

    /* Act */
    //CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end CF_CList_InsertAfter_FailsAssert_head_IsNull */

void Test_CF_CList_InsertAfter_FailsAssertValueAt_head_IsNull(void)
{
    /* Arrange */
    // clist_node      dummy_head = NULL;
    // clist_node*     arg_head = &dummy_head;
    // clist_node      arg_start;
    // clist_node      arg_after;

    /* Act */
    //CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertAfter_FailsAssertValueAt_head_IsNull */

void Test_CF_CList_InsertAfter_FailsAssert_start_IsNull(void)
{
    /* Arrange */
    // clist_node_t    dummy_head_node;
    // clist_node      dummy_head = &dummy_head_node;
    // clist_node*     arg_head = &dummy_head;
    // clist_node      arg_start = NULL;
    // clist_node      arg_after;

    /* Act */
    //CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertAfter_FailsAssert_start_IsNull */

void Test_CF_CList_InsertAfter_FailsAssert_start_IsEqTo_after(void)
{
    /* Arrange */
    // clist_node_t    dummy_head_node;
    // clist_node      dummy_head = &dummy_head_node;
    // clist_node*     arg_head = &dummy_head;
    // clist_node_t    dummy_start_node;
    // clist_node      arg_start = &dummy_start_node;
    // clist_node      arg_after = arg_start;

    /* Act */
    //CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_Failed("CF_Assert issue");
} /* end Test_CF_CList_InsertAfter_FailsAssert_start_IsNull */

void Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenOnlyOneNode(void)
{
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node      arg_start = dummy_head;
    clist_node_t    dummy_after_node;
    clist_node      arg_after = &dummy_after_node;
    
    dummy_head->prev = dummy_head;
    dummy_head->next = dummy_head;

    arg_after->prev = arg_after;
    arg_after->next = arg_after;
    
    /* Act */
    CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_True(dummy_head->prev == arg_after,
      "CF_CList_InsertAfter set head->prev to %p and should be %p (after)",
      dummy_head->prev, arg_after);
    UtAssert_True(dummy_head->next == arg_after,
      "CF_CList_InsertAfter set head->next to %p and should be %p (after)",
      dummy_head->next, arg_after);
    UtAssert_True(arg_after->prev == dummy_head,
      "CF_CList_InsertAfter set after->prev to %p and should be %p (head)",
      arg_after->prev, dummy_head);
    UtAssert_True(arg_after->next == dummy_head,
      "CF_CList_InsertAfter set after->next to %p and should be %p (head)",
      arg_after->next, dummy_head);
} /* end Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenOnlyOneNode */

void Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenAnyNode(void)
{
    /* Arrange */
    /* Arrange */
    clist_node_t    dummy_head_node;
    clist_node      dummy_head = &dummy_head_node;
    clist_node*     arg_head = &dummy_head;
    clist_node_t    dummy_last_node;
    clist_node      dummy_second_node;
    clist_node      dummy_next_to_last_node;
    clist_node      arg_start;
    clist_node_t    dummy_after;
    clist_node      arg_after = &dummy_after;
    clist_node      dummy_after_next;
    uint8           num_extraneous_nodes = Any_uint8_LessThan(18) + 1; // 1 to 18
    uint8           insertion_point = Any_uint8_LessThan(num_extraneous_nodes);
    int             i = 0;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));
        
        if (i == 0)
        {
            dummy_second_node = dummy_clist_node;
            dummy_second_node->prev = dummy_head;
            dummy_second_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_second_node;
        }
        else
        {
            dummy_next_to_last_node->next = dummy_clist_node;
            dummy_clist_node->prev = dummy_next_to_last_node;
            dummy_clist_node->next = &dummy_last_node;
            dummy_next_to_last_node = dummy_clist_node;
        }
        
    }

    dummy_last_node.next = &dummy_head_node;
    dummy_last_node.prev = dummy_next_to_last_node;

    (*arg_head)->next = dummy_second_node;
    (*arg_head)->prev = &dummy_last_node;

    /* set insertion point */
    arg_start = dummy_head;
    for(i = 0; i < insertion_point; ++i)
    {
        arg_start = arg_start->next;
    }
    dummy_after_next = arg_start->next;
    
    arg_after->prev = arg_after;
    arg_after->next = arg_after;

    /* Act */
    CF_CList_InsertAfter(arg_head, arg_start, arg_after);

    /* Assert */
    UtAssert_True(arg_after->next == dummy_after_next,
      "CF_CList_InsertAfter set after->next to %p and should be %p (old start->next)",
      arg_after->next, dummy_after_next);
    UtAssert_True(arg_start->next == arg_after,
      "CF_CList_InsertAfter set start->next to %p and should be %p (after)",
      arg_start->next, arg_after);
    UtAssert_True(arg_after->prev == arg_start,
      "CF_CList_InsertAfter set after->prev to %p and should be %p (start)",
      arg_after->prev, arg_start);
    UtAssert_True(dummy_after_next->prev == arg_after,
      "CF_CList_InsertAfter set after->next->prev to %p and should be %p (after)",
      dummy_after_next->prev, arg_after);

    /* removes all malloc nodes */
    clist_node  free_up_node = dummy_head->next;
    
    for(i = 0; i < num_extraneous_nodes; ++i)
    {   
        /* skip inserted node that was not malloc'd */
        if(i == insertion_point)
        {
            free_up_node = free_up_node->next;
        }

        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenAnyNode */

/* end CF_CList_InsertAfter tests */

/*******************************************************************************
**
**  CF_CList_Traverse tests
**
*******************************************************************************/

void Test_CF_CList_Traverse_DoesNothing_start_IsNull(void)
{
    /* Arrange */
    clist_node      arg_start = NULL;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    void*           arg_context = NULL;

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 0);
} /* end CF_CList_Traverse_DoesNothing_start_IsNull */

void Test_CF_CList_Traverse_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode(void)
{
    /* Arrange */
    clist_node_t    dummy_start;
    clist_node      arg_start = &dummy_start;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t  context_Dummy_clist_fn_t;

    arg_start->next = NULL;
    
    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 1); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t,
      sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == arg_start,
      "fn received node %p and should be %p (start)",
      context_Dummy_clist_fn_t.node, arg_start);
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Dummy_clist_fn_t.context, arg_context);
} /* end Test_CF_CList_Traverse_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode */

void Test_CF_CList_Traverse_SuccessWhenListIsOneNode(void)
{
    /* Arrange */
    clist_node_t    dummy_start;
    clist_node      arg_start = &dummy_start;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t  context_Dummy_clist_fn_t;

    arg_start->next = arg_start;
    
    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t,
      sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == arg_start,
      "fn received node %p and should be %p (start)",
      context_Dummy_clist_fn_t.node, arg_start);
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Dummy_clist_fn_t.context, arg_context);
} /* end Test_CF_CList_Traverse_SuccessWhenListIsOneNode */

/* TODO: Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt may need redesigned, or maybe the production code */
void Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt(void)
{
    /* Arrange */
    clist_node_t    dummy_start;
    clist_node      arg_start = &dummy_start;
    clist_fn_t      arg_fn = Hook_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    Hook_clist_fn_t_context_t  context_Hook_clist_fn_t;

    arg_start->next = arg_start;

    UT_SetDefaultReturnValue(UT_KEY(Hook_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Hook_clist_fn_t), &context_Hook_clist_fn_t,
      sizeof(context_Hook_clist_fn_t), false);
      
    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Hook_clist_fn_t, 1);
    UtAssert_True(context_Hook_clist_fn_t.node == arg_start,
      "fn received node %p and should be %p (start)",
      context_Hook_clist_fn_t.node, arg_start);
    UtAssert_True(context_Hook_clist_fn_t.context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Hook_clist_fn_t.context, arg_context);
} /* end Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt */


void Test_CF_CList_Traverse_ErrorIn_fn_CallWhenListIsManyNodes(void)
{
    /* Arrange */
    clist_node      arg_start;
    clist_node      adder_node;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    uint8           list_size = Any_uint8_LessThan(9) + 2; /* 2 - 10 */
    uint8           error_location = Any_uint8_LessThan(list_size); /* 0 to list_size - 1 */
    int             i = 0;
    Dummy_clist_fn_t_context_t  context_Dummy_clist_fn_t[list_size];
    
    /* set up list */
    for(i = 0; i < list_size; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            arg_start = dummy_clist_node;
        }
        else if(i == 1)
        {
            adder_node = dummy_clist_node;
            arg_start->next = adder_node;
            adder_node->prev = arg_start;
        }
        else
        {
            dummy_clist_node->prev = adder_node;
            adder_node->next = dummy_clist_node;
            adder_node = dummy_clist_node;
        }

        if(i == list_size - 1)
        {
            adder_node->next = arg_start;
            arg_start->prev = adder_node;
        }
        
    }

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDeferredRetcode(UT_KEY(Dummy_clist_fn_t), error_location + 1, 1);
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t,
      sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, error_location + 1);
    UtAssert_True(context_Dummy_clist_fn_t[0].node == arg_start,
      "fn received node %p and should be %p (start)",
      context_Dummy_clist_fn_t[0].node, arg_start);
    UtAssert_True(context_Dummy_clist_fn_t[0].context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Dummy_clist_fn_t[0].context, arg_context);

    clist_node expected_node = arg_start->next;
    for(i = 1; i <= error_location; ++i)
    {
        UtAssert_True(context_Dummy_clist_fn_t[i].node == expected_node,
        "fn received node %p and should be %p (node %u)",
        context_Dummy_clist_fn_t[i].node, expected_node, i + 1);
        UtAssert_True(context_Dummy_clist_fn_t[i].context == arg_context,
        "fn received context %p and should be %p (node %u)",
        context_Dummy_clist_fn_t[i].context, arg_context, i + 1);
        expected_node = expected_node->next;
    }

    /* removes all malloc nodes */
    clist_node  free_up_node = arg_start;
    
    for(i = 0; i < list_size; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_CList_Traverse_SuccessWhenListIsManyNodes */

void Test_CF_CList_Traverse_SuccessWhenListIsManyNodes(void)
{
    /* Arrange */
    clist_node      arg_start;
    clist_node      adder_node;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    uint8           list_size = Any_uint8_LessThan(9) + 2; /* 2 - 10 */
    int             i = 0;
    Dummy_clist_fn_t_context_t  context_Dummy_clist_fn_t[list_size + 1];

    /* set up list */
    for(i = 0; i < list_size; ++i)
    {
        clist_node dummy_clist_node = malloc(sizeof(*dummy_clist_node));

        if (i == 0)
        {
            arg_start = dummy_clist_node;
        }
        else if(i == 1)
        {
            adder_node = dummy_clist_node;
            arg_start->next = adder_node;
            adder_node->prev = arg_start;
        }
        else
        {
            dummy_clist_node->prev = adder_node;
            adder_node->next = dummy_clist_node;
            adder_node = dummy_clist_node;
        }

        if(i == list_size - 1)
        {
            adder_node->next = arg_start;
            arg_start->prev = adder_node;
        }
        
    }

    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t,
      sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse(arg_start, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, list_size);
    UtAssert_True(context_Dummy_clist_fn_t[0].node == arg_start,
      "fn received node %p and should be %p (start)",
      context_Dummy_clist_fn_t[0].node, arg_start);
    UtAssert_True(context_Dummy_clist_fn_t[0].context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Dummy_clist_fn_t[0].context, arg_context);

    clist_node expected_node = arg_start->next;
    for(i = 1; i < list_size; ++i)
    {
        UtAssert_True(context_Dummy_clist_fn_t[i].node == expected_node,
        "fn received node %p and should be %p (node %u)",
        context_Dummy_clist_fn_t[i].node, expected_node, i);
        UtAssert_True(context_Dummy_clist_fn_t[i].context == arg_context,
        "fn received context %p and should be %p (node %u)",
        context_Dummy_clist_fn_t[i].context, arg_context, i);
        expected_node = expected_node->next;
    }

    /* removes all malloc nodes */
    clist_node  free_up_node = arg_start;
    
    for(i = 0; i < list_size; ++i)
    {      
        clist_node old_free_up_node = free_up_node;
        free_up_node = old_free_up_node->next;
        free(old_free_up_node);
    }
} /* end Test_CF_CList_Traverse_SuccessWhenListIsManyNodes */



/*******************************************************************************
**
**  CF_CList_Traverse_R tests
**
*******************************************************************************/

void Test_CF_CList_Traverse_R_DoesNothing_end_IsNull(void)
{
    /* Arrange */
    clist_node      arg_end = NULL;
    clist_fn_t      arg_fn = NULL;
    void*           arg_context = NULL;

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 0);
} /* end Test_CF_CList_Traverse_R_DoesNothing_end_IsNull */

void Test_CF_CList_Traverse_R_DoesNothing_end_prev_IsNull(void)
{
    /* Arrange */
    clist_node_t    dummy_end;
    clist_node      arg_end = &dummy_end;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;

    arg_end->prev = NULL;

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 0);
} /* end Test_CF_CList_Traverse_R_DoesNothing_end_prev_IsNull */

void Test_CF_CList_Traverse_R_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode(void)
{
    /* Arrange */
    clist_node_t    dummy_end;
    clist_node      arg_end = &dummy_end;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    clist_node_t    dummy_end_prev_node;
    clist_node      dummy_end_prev = &dummy_end_prev_node;
    Dummy_clist_fn_t_context_t  context_Dummy_clist_fn_t;

    arg_end->prev = dummy_end_prev;
  
    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 1); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t,
      sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == dummy_end_prev,
      "fn received node %p and should be %p (end->prev)",
      context_Dummy_clist_fn_t.node, dummy_end_prev);
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Dummy_clist_fn_t.context, arg_context);
} /* end Test_CF_CList_Traverse_R_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode */

void Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0(void)
{
    /* Arrange */
    clist_node_t    dummy_end;
    clist_node      arg_end = &dummy_end;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t  context_Dummy_clist_fn_t;

    arg_end->prev = arg_end;
    
    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), &context_Dummy_clist_fn_t,
      sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 1);
    UtAssert_True(context_Dummy_clist_fn_t.node == arg_end,
      "fn received node %p and should be %p (end->prev)",
      context_Dummy_clist_fn_t.node, arg_end);
    UtAssert_True(context_Dummy_clist_fn_t.context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Dummy_clist_fn_t.context, arg_context);
} /* end Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0 */

void Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast(void)
{
    /* Arrange */
    clist_node_t    dummy_node;
    clist_node_t    dummy_end;
    clist_node      arg_end = &dummy_end;
    clist_fn_t      arg_fn = Dummy_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    Dummy_clist_fn_t_context_t  context_Dummy_clist_fn_t[2];

    arg_end->prev = &dummy_node;
    dummy_node.prev = arg_end;
    
    UT_SetDefaultReturnValue(UT_KEY(Dummy_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Dummy_clist_fn_t), context_Dummy_clist_fn_t,
      sizeof(context_Dummy_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Dummy_clist_fn_t, 2);
    UtAssert_True(context_Dummy_clist_fn_t[0].node == &dummy_node,
      "fn call 1 received node %p and should be %p (passed in end->prev)",
      context_Dummy_clist_fn_t[0].node, &dummy_node);
    UtAssert_True(context_Dummy_clist_fn_t[0].context == arg_context,
      "fn call 1 received context %p and should be %p (context)",
      context_Dummy_clist_fn_t[0].context, arg_context);
    UtAssert_True(context_Dummy_clist_fn_t[1].node == arg_end,
      "fn call 2 received node %p and should be %p (original end->prev)",
      context_Dummy_clist_fn_t[1].node, arg_end);
    UtAssert_True(context_Dummy_clist_fn_t[1].context == arg_context,
      "fn call 2 received context %p and should be %p (context)",
      context_Dummy_clist_fn_t[1].context, arg_context);
} /* end Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast */

void Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt(void)
{
    /* Arrange */
    clist_node_t    dummy_end;
    clist_node      arg_end = &dummy_end;
    clist_fn_t      arg_fn = Hook_clist_fn_t;
    int             dummy_context;
    void*           arg_context = &dummy_context;
    Hook_clist_fn_t_context_t  context_Hook_clist_fn_t;

    arg_end->prev = arg_end;
    
    UT_SetDefaultReturnValue(UT_KEY(Hook_clist_fn_t), 0); /* 0 is pass, 1 is fail */
    UT_SetDataBuffer(UT_KEY(Hook_clist_fn_t), &context_Hook_clist_fn_t,
      sizeof(context_Hook_clist_fn_t), false);

    /* Act */
    CF_CList_Traverse_R(arg_end, arg_fn, arg_context);

    
    /* Assert */
    UtAssert_STUB_COUNT(Hook_clist_fn_t, 1);
    UtAssert_True(context_Hook_clist_fn_t.node == arg_end,
      "fn received node %p and should be %p (end->prev)",
      context_Hook_clist_fn_t.node, arg_end);
    UtAssert_True(context_Hook_clist_fn_t.context == arg_context,
      "fn received context %p and should be %p (context)",
      context_Hook_clist_fn_t.context, arg_context);
} /* end Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt */



/*******************************************************************************
**
**  cf_clist_tests UtTest_Add groups
**
*******************************************************************************/

void add_CF_CList_InitNode_tests(void)
{    
    UtTest_Add(Test_CF_CList_InitNode_PointsNodeToItselfAsNextAndPrev, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InitNode_PointsNodeToItselfAsNextAndPrev");
}

void add_CF_CList_InsertFront_tests(void)
{    
    UtTest_Add(Test_CF_CList_InsertFront_FailsAssertHeadIsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_FailsAssertHeadIsNull");    
    UtTest_Add(Test_CF_CList_InsertFront_FailsAssertNodeIsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_FailsAssertNodeIsNull");    
    UtTest_Add(Test_CF_CList_InsertFront_FailsAssertNodeNextDoesNotPointToItself, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_FailsAssertNodeNextDoesNotPointToItself");    
    UtTest_Add(Test_CF_CList_InsertFront_FailsAssertNodePrevDoesNotPointToItself, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_FailsAssertNodePrevDoesNotPointToItself");    
    UtTest_Add(Test_CF_CList_InsertFront_InsertsNodeIntoEmptyList, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_InsertsNodeIntoEmptyList");    
    UtTest_Add(Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther");    
    UtTest_Add(Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodes, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_InsertsNodeWhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodes");    
    UtTest_Add(Test_CF_CList_InsertFront_InsertsNodeWhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodes, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertFront_InsertsNodeWhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodes");
}

void add_CF_CList_InsertBack_tests(void)
{    
    UtTest_Add(Test_CF_CList_InsertBack_FailsAssertHeadIsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_FailsAssertHeadIsNull");    
    UtTest_Add(Test_CF_CList_InsertBack_FailsAssertNodeIsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_FailsAssertNodeIsNull");    
    UtTest_Add(Test_CF_CList_InsertBack_FailsAssertNodeNextDoesNotPointToItself, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_FailsAssertNodeNextDoesNotPointToItself");    
    UtTest_Add(Test_CF_CList_InsertBack_FailsAssertNodePrevDoesNotPointToItself, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_FailsAssertNodePrevDoesNotPointToItself");    
    UtTest_Add(Test_CF_CList_InsertBack_InsertsNodeIntoEmptyList, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_InsertsNodeIntoEmptyList");    
    UtTest_Add(Test_CF_CList_InsertBack_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_InsertsNodeWhenHeadIsOnlyNodeAndTheyPointToEachOther");    
    UtTest_Add(Test_CF_CList_InsertBack_InsertsNodeWhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodes, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_InsertsNodeWhenHeadIsOneOfTwoNodesAndTheyPointToCorrectNodes");    
    UtTest_Add(Test_CF_CList_InsertBack_InsertsNodeWhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodes, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_InsertBack_InsertsNodeWhenNodeListIsGreaterThanTwoNodesAndTheyPointToCorrectNodes");
}

void add_CF_CList_Pop_tests(void)
{    
    UtTest_Add(Test_CF_CList_Pop_FailsAssert_head_IsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_Pop_FailsAssert_head_IsNull"); 
    UtTest_Add(Test_CF_CList_Pop_SuccessReturnsNullBecauseListIsEmpty, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_Pop_SuccessReturnsNullBecauseListIsEmpty"); 
    UtTest_Add(Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenItIsOnlyNode, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenItIsOnlyNode");  
    UtTest_Add(Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenListIsAnySizeGreaterThanOne, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_Pop_SuccessPopsHeadNodeAndReturns_head_WhenListIsAnySizeGreaterThanOne");   
}

void add_CF_CList_Remove_tests(void)
{
    UtTest_Add(Test_CF_CList_Remove_FailsAssertHeadPointedAtValueIsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_Remove_FailsAssertHeadPointedAtValueIsNull");    
    UtTest_Add(Test_CF_CList_Remove_FailsAssertNodeIsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_Remove_FailsAssertNodeIsNull");    
    UtTest_Add(Test_CF_CList_Remove_FailsAssertHeadIsNull, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_CList_Remove_FailsAssertHeadIsNull");    
    UtTest_Add(Test_CF_ClistRemove_FailsAssertHeadPointedAtValueIsNotNode, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_ClistRemove_FailsAssertHeadPointedAtValueIsNotNode");
    UtTest_Add(Test_CF_ClistRemove_SetsHeadToNullWhenOnlyNode, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_ClistRemove_SetsHeadToNullWhenOnlyNode");
    UtTest_Add(Test_CF_ClistRemove_SetsHeadToPointToItselWhenOnlyTwoNodesAndLastIsRemoved, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_ClistRemove_SetsHeadToPointToItselWhenOnlyTwoNodesAndLastIsRemoved");
    UtTest_Add(Test_CF_ClistRemove_RemovingHeadSetsSecondNodeToHeadAndUpdatesLastNode, 
         cf_clist_tests_Setup, cf_clist_tests_Teardown, 
         "Test_CF_ClistRemove_RemovingHeadSetsSecondNodeToHeadAndUpdatesLastNode");
    UtTest_Add(Test_CF_ClistRemove_RemovingLastPointsHeadAndNextToLastToEachOther, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_ClistRemove_RemovingLastPointsHeadAndNextToLastToEachOther");
    UtTest_Add(Test_CF_ClistRemove_RemovingAnyNodeHasNodesPrevAndNextPointToEachOther, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_ClistRemove_RemovingAnyNodeHasNodesPrevAndNextPointToEachOther"); 
      /* TODO: Re-examine Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot */
    // UtTest_Add(Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot, 
    //   cf_clist_tests_Setup, cf_clist_tests_Teardown, 
    //   "Test_CF_CList_Remove_ReceivesBad_node_Because_next_PointsTo_node_But_prev_DoesNot");   
}

void add_CF_CList_InsertAfter_tests(void)
{
    UtTest_Add(Test_CF_CList_InsertAfter_FailsAssert_head_IsNull, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_InsertAfter_FailsAssert_head_IsNull"); 
    UtTest_Add(Test_CF_CList_InsertAfter_FailsAssertValueAt_head_IsNull, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_InsertAfter_FailsAssert_head_IsNull"); 
    UtTest_Add(Test_CF_CList_InsertAfter_FailsAssert_start_IsNull, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_InsertAfter_FailsAssert_head_IsNull"); 
    UtTest_Add(Test_CF_CList_InsertAfter_FailsAssert_start_IsEqTo_after, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_InsertAfter_FailsAssert_head_IsNull");  
    UtTest_Add(Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenOnlyOneNode, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenOnlyOneNode");
    UtTest_Add(Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenAnyNode, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_InsertAfter_Success_after_IsInsertedAfter_start_WhenAnyNode");
}

void add_CF_CList_Traverse_tests(void)
{
    UtTest_Add(Test_CF_CList_Traverse_DoesNothing_start_IsNull, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_DoesNothing_start_IsNull");
    UtTest_Add(Test_CF_CList_Traverse_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode");   
    UtTest_Add(Test_CF_CList_Traverse_SuccessWhenListIsOneNode, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_SuccessWhenListIsOneNode");   
    UtTest_Add(Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt");  
    UtTest_Add(Test_CF_CList_Traverse_ErrorIn_fn_CallWhenListIsManyNodes, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_ErrorIn_fn_CallWhenListIsManyNodes"); 
    UtTest_Add(Test_CF_CList_Traverse_SuccessWhenListIsManyNodes, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_SuccessWhenListIsManyNodes"); 
}

void add_CF_CList_Traverse_R_tests(void)
{
    UtTest_Add(Test_CF_CList_Traverse_R_DoesNothing_end_IsNull, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_R_DoesNothing_end_IsNull");
    UtTest_Add(Test_CF_CList_Traverse_R_DoesNothing_end_prev_IsNull, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_R_DoesNothing_end_prev_IsNull");
    UtTest_Add(Test_CF_CList_Traverse_R_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_R_ErrorsOutFirst_fn_CallFailsWhenListIsMoreThanOneNode");  
    UtTest_Add(Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_R_PassedIn_end_IsTheOnlyNode_fn_Returned_non0");
    UtTest_Add(Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_R_PassedIn_end_IsNotTheOnlyNode_fn_Returned_non0_Original_end_UsedLast");
    UtTest_Add(Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt, 
      cf_clist_tests_Setup, cf_clist_tests_Teardown, 
      "Test_CF_CList_Traverse_R_CanActuallyGet_n_next_NotEqTo_nn_ButOnlyWithAn_fn_ThatForcesIt");  
}

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