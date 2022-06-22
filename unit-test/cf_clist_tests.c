/************************************************************************
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 *
 * Copyright (c) 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/* cf testing includes */
#include "cf_test_utils.h"
#include "cf_clist.h"

/*******************************************************************************
**
**  cf_clist_tests local utility functions
**
*******************************************************************************/

int UT_CListFn(CF_CListNode_t *node, void *context)
{
    int  status = CF_CLIST_CONT;
    int *param  = context;

    /* Passing in a negative value will exit when zero is hit */
    (*param)++;
    if (*param == 0)
    {
        status = CF_CLIST_EXIT;
    }

    return status;
}

int UT_CListFn_Rm(CF_CListNode_t *node, void *context)
{
    (*((int *)context))--;
    node->next = node;
    node->prev = node;
    return CF_CLIST_CONT;
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

void Test_CF_CList_InsertFront_InsertNodeIntoEmptyList(void)
{
    /* Arrange */
    CF_CListNode_t * dummy_head = NULL;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;

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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;

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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;
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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t * dummy_second_node;
    CF_CListNode_t * dummy_next_to_last_node;
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

void Test_CF_CList_InsertBack_InsertNodeIntoEmptyList(void)
{
    /* Arrange */
    CF_CListNode_t * dummy_head = NULL;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;

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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;

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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;
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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t * dummy_second_node;
    CF_CListNode_t * dummy_next_to_last_node;
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

void Test_CF_CList_Pop_WhenListIsEmptySuccessReturn_NULL(void)
{
    /* Arrange */
    CF_CListNode_t * dummy_head = NULL;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t * local_result;

    /* Act */
    local_result = CF_CList_Pop(arg_head);

    /* Assert */
    UtAssert_ADDRESS_EQ(local_result, NULL);
} /* end  Test_CF_CList_Pop_WhenListIsEmptySuccessReturn_NULL */

void Test_CF_CList_Pop_WhenItIsOnlyNodePopHeadNodeAndReturn_head_(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t * local_result;

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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t * dummy_second_node       = &dummy_last_node;
    CF_CListNode_t * dummy_next_to_last_node = dummy_head;
    uint8            num_extraneous_nodes    = Any_uint8_LessThan(19); // 0 to 18
    int              i                       = 0;
    CF_CListNode_t * local_result;

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

void Test_CF_ClistRemove_WhenOnlyNodeSetHeadTo_NULL(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_node;
    CF_CListNode_t * arg_node = &dummy_node;
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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_arg_node;
    CF_CListNode_t * arg_node = &dummy_arg_node;

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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t * arg_node   = *arg_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t * dummy_second_node;
    CF_CListNode_t * dummy_next_to_last_node;
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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t * arg_node = &dummy_last_node;
    CF_CListNode_t * dummy_second_node;
    CF_CListNode_t * dummy_next_to_last_node;
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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t * dummy_removed_node;
    CF_CListNode_t * dummy_removed_node_prev;
    CF_CListNode_t * dummy_removed_node_next;
    CF_CListNode_t * arg_node;
    CF_CListNode_t * dummy_second_node;
    CF_CListNode_t * dummy_next_to_last_node;
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

/* end CF_CList_Remove tests */

/*******************************************************************************
**
**  CF_CList_InsertAfter tests
**
*******************************************************************************/

void Test_CF_CList_InsertAfter_WhenOnlyOneNodeSuccess_after_IsInsertedAfter_start(void)
{
    /* Arrange */
    CF_CListNode_t   dummy_head_node;
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t * arg_start  = dummy_head;
    CF_CListNode_t   dummy_after_node;
    CF_CListNode_t * arg_after = &dummy_after_node;

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
    CF_CListNode_t * dummy_head = &dummy_head_node;
    CF_CListNode_t **arg_head   = &dummy_head;
    CF_CListNode_t   dummy_last_node;
    CF_CListNode_t * dummy_second_node;
    CF_CListNode_t * dummy_next_to_last_node;
    CF_CListNode_t * arg_start;
    CF_CListNode_t   dummy_after;
    CF_CListNode_t * arg_after = &dummy_after;
    CF_CListNode_t * dummy_after_next;
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

void Test_CF_CList_Traverse(void)
{
    CF_CListNode_t node[2];
    int            context;

    memset(node, 0, sizeof(node));

    /* Null won't call function */
    context = 0;
    UtAssert_VOIDCALL(CF_CList_Traverse(NULL, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 0);

    /* Single node success */
    context      = 0;
    node[0].next = &node[0];
    UtAssert_VOIDCALL(CF_CList_Traverse(node, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 1);

    /* Two nodes nominal */
    context      = 0;
    node[0].next = &node[1];
    node[1].next = &node[0];
    UtAssert_VOIDCALL(CF_CList_Traverse(node, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 2);

    /* Two nodes, force exit on first call */
    context      = -1;
    node[0].next = &node[1];
    UtAssert_VOIDCALL(CF_CList_Traverse(node, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 0);

    /* Two nodes, self delete */
    context      = 0;
    node[0].next = &node[1];
    node[1].next = &node[1]; /* This would be the state after a real remove */
    UtAssert_VOIDCALL(CF_CList_Traverse(node, UT_CListFn_Rm, &context));
    UtAssert_INT32_EQ(context, -2);
}

/*******************************************************************************
**
**  CF_CList_Traverse_R tests
**
*******************************************************************************/

void Test_CF_CList_Traverse_R(void)
{
    CF_CListNode_t node[3];
    int            context;

    memset(node, 0, sizeof(node));

    /* Null won't call function */
    context = 0;
    UtAssert_VOIDCALL(CF_CList_Traverse_R(NULL, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 0);

    /* Null previous also skips logic */
    context = 0;
    UtAssert_VOIDCALL(CF_CList_Traverse_R(node, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 0);

    /* Single node success */
    context      = 0;
    node[0].prev = &node[0];
    UtAssert_VOIDCALL(CF_CList_Traverse_R(node, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 1);

    /* Two nodes nominal */
    context      = 0;
    node[0].prev = &node[1];
    node[1].prev = &node[0];
    UtAssert_VOIDCALL(CF_CList_Traverse_R(node, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 2);

    /* Two nodes, force exit on first call */
    context      = -1;
    node[0].prev = &node[1];
    UtAssert_VOIDCALL(CF_CList_Traverse_R(node, UT_CListFn, &context));
    UtAssert_INT32_EQ(context, 0);

    /* Two nodes, self delete */
    context      = 0;
    node[0].prev = &node[2];
    node[1].prev = &node[1]; /* Self point to cause end */
    node[2].prev = &node[1];
    UtAssert_VOIDCALL(CF_CList_Traverse_R(node, UT_CListFn_Rm, &context));
    UtAssert_INT32_EQ(context, -2);
}

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
} /* end add_CF_CList_Remove_tests */

void add_CF_CList_InsertAfter_tests(void)
{
    UtTest_Add(Test_CF_CList_InsertAfter_WhenOnlyOneNodeSuccess_after_IsInsertedAfter_start, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertAfter_WhenOnlyOneNodeSuccess_after_IsInsertedAfter_start");
    UtTest_Add(Test_CF_CList_InsertAfter_WhenAnyNodeSuccess_after_IsInsertedAfter_start, cf_clist_tests_Setup,
               cf_clist_tests_Teardown, "Test_CF_CList_InsertAfter_WhenAnyNodeSuccess_after_IsInsertedAfter_start");
} /* end add_CF_CList_InsertAfter_tests */

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

    UtTest_Add(Test_CF_CList_Traverse, cf_clist_tests_Setup, cf_clist_tests_Teardown, "Test_CF_CList_Traverse");
    UtTest_Add(Test_CF_CList_Traverse_R, cf_clist_tests_Setup, cf_clist_tests_Teardown, "Test_CF_CList_Traverse_R");
} /* end UtTest_Setup for cf_clist_tests.c */

/* end cf_clist_tests.c */
