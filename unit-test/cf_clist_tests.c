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
**  Tests
**
*******************************************************************************/

void Test_CF_CList_InitNode(void)
{
    CF_CListNode_t node;

    memset(&node, 0, sizeof(node));

    UtAssert_VOIDCALL(CF_CList_InitNode(&node));
    UtAssert_ADDRESS_EQ(&node, node.next);
    UtAssert_ADDRESS_EQ(&node, node.prev);
}

void Test_CF_CList_InsertFront(void)
{
    CF_CListNode_t  node[3];
    CF_CListNode_t *head = NULL;

    memset(node, 0, sizeof(node));

    /* Already tested, so OK to use to initialize */
    CF_CList_InitNode(&node[0]);

    /* Insert to empty list */
    UtAssert_VOIDCALL(CF_CList_InsertFront(&head, &node[0]));
    UtAssert_ADDRESS_EQ(head, node);

    /* Insert to single node list */
    UtAssert_VOIDCALL(CF_CList_InsertFront(&head, &node[1]));
    UtAssert_ADDRESS_EQ(head, &node[1]);
    UtAssert_ADDRESS_EQ(node[0].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[1]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[0]);

    /* Insert to list with 2 nodes */
    UtAssert_VOIDCALL(CF_CList_InsertFront(&head, &node[2]));
    UtAssert_ADDRESS_EQ(head, &node[2]);
    UtAssert_ADDRESS_EQ(node[0].next, &node[2]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[1]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[2]);
    UtAssert_ADDRESS_EQ(node[2].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[2].prev, &node[0]);
}

void Test_CF_CList_InsertBack(void)
{
    CF_CListNode_t  node[3];
    CF_CListNode_t *head = NULL;

    memset(node, 0, sizeof(node));

    /* Already tested, so OK to use to initialize */
    CF_CList_InitNode(&node[0]);

    /* Insert to empty list */
    UtAssert_VOIDCALL(CF_CList_InsertBack(&head, &node[0]));
    UtAssert_ADDRESS_EQ(head, node);

    /* Insert to single node list */
    UtAssert_VOIDCALL(CF_CList_InsertBack(&head, &node[1]));
    UtAssert_ADDRESS_EQ(head, &node[0]);
    UtAssert_ADDRESS_EQ(node[0].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[1]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[0]);

    /* Insert to list with 2 nodes */
    UtAssert_VOIDCALL(CF_CList_InsertBack(&head, &node[2]));
    UtAssert_ADDRESS_EQ(head, &node[0]);
    UtAssert_ADDRESS_EQ(node[0].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[2]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[2]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[0]);
    UtAssert_ADDRESS_EQ(node[2].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[2].prev, &node[1]);
}

void Test_CF_CList_Pop(void)
{
    CF_CListNode_t  node[3];
    CF_CListNode_t *head = NULL;

    memset(node, 0, sizeof(node));

    /* Already tested, so OK to use to initialize */
    CF_CList_InitNode(&node[0]);
    CF_CList_InsertBack(&head, &node[0]);
    CF_CList_InsertBack(&head, &node[1]);
    CF_CList_InsertBack(&head, &node[2]);

    /* Pop leaves 2 nodes */
    UtAssert_ADDRESS_EQ(CF_CList_Pop(&head), &node[0]);
    UtAssert_ADDRESS_EQ(head, &node[1]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[2]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[2]);
    UtAssert_ADDRESS_EQ(node[2].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[2].prev, &node[1]);

    /* Pop leaves 1 node */
    UtAssert_ADDRESS_EQ(CF_CList_Pop(&head), &node[1]);
    UtAssert_ADDRESS_EQ(head, &node[2]);
    UtAssert_ADDRESS_EQ(node[2].next, &node[2]);
    UtAssert_ADDRESS_EQ(node[2].prev, &node[2]);

    /* Pop leaves empty list */
    UtAssert_ADDRESS_EQ(CF_CList_Pop(&head), &node[2]);
    UtAssert_ADDRESS_EQ(head, NULL);

    /* Pop a NULL list */
    UtAssert_ADDRESS_EQ(CF_CList_Pop(&head), NULL);
}

void Test_CF_CList_Remove(void)
{
    CF_CListNode_t  node[3];
    CF_CListNode_t *head = NULL;

    memset(node, 0, sizeof(node));

    /* Already tested, so OK to use to initialize */
    CF_CList_InitNode(&node[0]);
    CF_CList_InsertBack(&head, &node[0]);
    CF_CList_InsertBack(&head, &node[1]);
    CF_CList_InsertBack(&head, &node[2]);

    /* Note Pop tests exercise removing from the front, and null */

    /* Remove from the middle */
    UtAssert_VOIDCALL(CF_CList_Remove(&head, &node[1]));
    UtAssert_ADDRESS_EQ(head, &node[0]);
    UtAssert_ADDRESS_EQ(node[0].next, &node[2]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[2]);
    UtAssert_ADDRESS_EQ(node[2].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[2].prev, &node[0]);

    /* Remove from the end */
    UtAssert_VOIDCALL(CF_CList_Remove(&head, &node[2]));
    UtAssert_ADDRESS_EQ(head, &node[0]);
    UtAssert_ADDRESS_EQ(node[0].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[0]);
}

void Test_CF_CList_InsertAfter(void)
{
    CF_CListNode_t  node[4];
    CF_CListNode_t *head = node;

    memset(node, 0, sizeof(node));

    /* Already tested, so OK to use to initialize */
    CF_CList_InitNode(&node[0]);

    /* Insert to a single node list */
    UtAssert_VOIDCALL(CF_CList_InsertAfter(&head, &node[0], &node[1]));
    UtAssert_ADDRESS_EQ(head, node);
    UtAssert_ADDRESS_EQ(node[0].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[1]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[0]);

    /* Insert at the end of a 2 node list to confirm head node gets updated correctly */
    UtAssert_VOIDCALL(CF_CList_InsertAfter(&head, &node[1], &node[2]));
    UtAssert_ADDRESS_EQ(head, node);
    UtAssert_ADDRESS_EQ(node[0].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[2]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[2]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[0]);
    UtAssert_ADDRESS_EQ(node[2].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[2].prev, &node[1]);

    /* Insert in the middle */
    UtAssert_VOIDCALL(CF_CList_InsertAfter(&head, &node[1], &node[3]));
    UtAssert_ADDRESS_EQ(head, node);
    UtAssert_ADDRESS_EQ(node[0].next, &node[1]);
    UtAssert_ADDRESS_EQ(node[0].prev, &node[2]);
    UtAssert_ADDRESS_EQ(node[1].next, &node[3]);
    UtAssert_ADDRESS_EQ(node[1].prev, &node[0]);
    UtAssert_ADDRESS_EQ(node[2].next, &node[0]);
    UtAssert_ADDRESS_EQ(node[2].prev, &node[3]);
    UtAssert_ADDRESS_EQ(node[3].next, &node[2]);
    UtAssert_ADDRESS_EQ(node[3].prev, &node[1]);
}

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

/* Add tests */
void UtTest_Setup(void)
{
    TEST_CF_ADD(Test_CF_CList_InitNode);
    TEST_CF_ADD(Test_CF_CList_InsertFront);
    TEST_CF_ADD(Test_CF_CList_InsertBack);
    TEST_CF_ADD(Test_CF_CList_Pop);
    TEST_CF_ADD(Test_CF_CList_Remove);
    TEST_CF_ADD(Test_CF_CList_InsertAfter);
    TEST_CF_ADD(Test_CF_CList_Traverse);
    TEST_CF_ADD(Test_CF_CList_Traverse_R);
}
