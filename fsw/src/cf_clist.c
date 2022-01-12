/************************************************************************
 *
 * NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
 * Application version 3.0.0”
 * Copyright © 2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ************************************************************************/

/**
 * @file
 *
 *  The CF Application circular list definition source file
 *
 *  This is a circular doubly-linked list implementation. It is used for
 *  all data structures in CF.
 *
 *  This file is intended to be a generic class that can be used in other apps.
 */

#include "cf_verify.h"
#include "cf_clist.h"
#include "cf_assert.h"

/*----------------------------------------------------------------
 *
 * Function: CF_CList_InitNode
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CList_InitNode(CF_CListNode_t *node)
{
    node->next = node;
    node->prev = node;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CList_InsertFront
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CList_InsertFront(CF_CListNode_t **head, CF_CListNode_t *node)
{
    CF_Assert(head);
    CF_Assert(node);
    CF_Assert(node->next == node);
    CF_Assert(node->prev == node);

    if (*head)
    {
        CF_CListNode_t *last = (*head)->prev;

        node->next = *head;
        node->prev = last;

        last->next = (*head)->prev = node;
    }

    *head = node;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CList_InsertBack
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CList_InsertBack(CF_CListNode_t **head, CF_CListNode_t *node)
{
    CF_Assert(head);
    CF_Assert(node);
    CF_Assert(node->next == node);
    CF_Assert(node->prev == node);

    if (!*head)
    {
        *head = node;
    }
    else
    {
        CF_CListNode_t *last = (*head)->prev;

        node->next    = *head;
        (*head)->prev = node;
        node->prev    = last;
        last->next    = node;
    }
}

/*----------------------------------------------------------------
 *
 * Function: CF_CList_Pop
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
CF_CListNode_t *CF_CList_Pop(CF_CListNode_t **head)
{
    CF_CListNode_t *ret;

    CF_Assert(head);

    ret = *head;
    if (ret)
    {
        CF_CList_Remove(head, ret);
    }

    return ret;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CList_Remove
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CList_Remove(CF_CListNode_t **head, CF_CListNode_t *node)
{
    CF_Assert(head);
    CF_Assert(node);
    CF_Assert(*head);

    if ((node->next == node) && (node->prev == node))
    {
        /* only node in the list, so this one is easy */
        CF_Assert(node == *head); /* sanity check */
        *head = NULL;
    }
    else if (*head == node)
    {
        /* removing the first node in the list, so make the second node in the list the first */
        (*head)->prev->next = node->next;
        *head               = node->next;

        (*head)->prev = node->prev;
    }
    else
    {
        node->next->prev = node->prev;
        node->prev->next = node->next;
    }

    CF_CList_InitNode(node);
}

/*----------------------------------------------------------------
 *
 * Function: CF_CList_InsertAfter
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CList_InsertAfter(CF_CListNode_t **head, CF_CListNode_t *start, CF_CListNode_t *after)
{
    /* calling insert_after with nothing to insert after (no head) makes no sense */
    CF_Assert(head);
    CF_Assert(*head);
    CF_Assert(start);
    CF_Assert(start != after);

    /* knowing that head is not empty, and knowing that start is non-zero, this is an easy operation */
    after->next       = start->next;
    start->next       = after;
    after->prev       = start;
    after->next->prev = after;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CList_Traverse
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CList_Traverse(CF_CListNode_t *start, CF_CListFn_t fn, void *context)
{
    CF_CListNode_t *n = start;
    CF_CListNode_t *nn;
    int             last = 0;

    if (n)
    {
        do
        {
            /* set nn in case callback removes this node from the list */
            nn = n->next;
            if (nn == start)
            {
                last = 1;
            }
            if (fn(n, context))
            {
                goto err_out;
            }
            /* list traversal is robust against an item deleting itself during traversal,
             * but there is a special case if that item is the starting node. Since this is
             * a circular list, start is remembered so we know when to stop. Must set start
             * to the next node in this case. */
            if ((start == n) && (n->next != nn))
            {
                start = nn;
            }
            n = nn;
        } while (!last);
    }

err_out:;
}

/*----------------------------------------------------------------
 *
 * Function: CF_CList_Traverse_R
 *
 * Application-scope internal function
 * See description in cf_clist.h for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CF_CList_Traverse_R(CF_CListNode_t *end, CF_CListFn_t fn, void *context)
{
    if (end)
    {
        CF_CListNode_t *n = end->prev;
        CF_CListNode_t *nn;
        int             last = 0;

        if (n)
        {
            end = n;

            do
            {
                /* set nn in case callback removes this node from the list */
                nn = n->prev;
                if (nn == end)
                {
                    last = 1;
                }

                if (fn(n, context))
                {
                    goto err_out;
                }

                /* list traversal is robust against an item deleting itself during traversal,
                 * but there is a special case if that item is the starting node. Since this is
                 * a circular list, "end" is remembered so we know when to stop. Must set "end"
                 * to the next node in this case. */
                if ((end == n) && (n->prev != nn))
                {
                    end = nn;
                }
                n = nn;
            } while (!last);
        }
    }

err_out:;
}
