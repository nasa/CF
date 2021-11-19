/************************************************************************
** File: cf_clist.c
**
** NASA Docket No. GSC-18,447-1, and identified as “CFS CFDP (CF)
** Application version 3.0.0”
** Copyright © 2019 United States Government as represented by the
** Administrator of the National Aeronautics and Space Administration.
** All Rights Reserved.
** Licensed under the Apache License, Version 2.0 (the "License"); you may
** not use this file except in compliance with the License. You may obtain
** a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
**
**
** Purpose:
**  The CF Application circular list definition source file
**
**  This is a circular doubly-linked list implementation. It is used for
**  all data structures in CF.
**
**  This file is intended to be a generic class that can be used in other apps.
**
**
**
*************************************************************************/

#include "cf_verify.h"
#include "cf_clist.h"
#include "cf_assert.h"

/************************************************************************/
/** \brief Initialize a clist node.
**
**  \par Assumptions, External Events, and Notes:
**       node must not be NULL.
**
*************************************************************************/
void CF_CList_InitNode(clist_node node)
{
    node->next = node;
    node->prev = node;
}

/************************************************************************/
/** \brief Insert the given node into the front of a list.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_InsertFront(clist_node *head, clist_node node)
{
    CF_Assert(head);
    CF_Assert(node);
    CF_Assert(node->next == node);
    CF_Assert(node->prev == node);

    if (*head)
    {
        clist_node_t *last = (*head)->prev;

        node->next = *head;
        node->prev = last;

        last->next = (*head)->prev = node;
    }

    *head = node;
}

/************************************************************************/
/** \brief Insert the given node into the back of a list.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_InsertBack(clist_node *head, clist_node node)
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
        clist_node_t *last = (*head)->prev;

        node->next    = *head;
        (*head)->prev = node;
        node->prev    = last;
        last->next    = node;
    }
}

/************************************************************************/
/** \brief Remove the first node from a list and return it.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL.
**
**  \returns
**  \retstmt The first node (now removed) in the list; NULL if list was empty. \endcode
**  \endreturns
**
*************************************************************************/
clist_node CF_CList_Pop(clist_node *head)
{
    clist_node ret;

    CF_Assert(head);

    ret = *head;
    if (ret)
    {
        CF_CList_Remove(head, ret);
    }

    return ret;
}

/************************************************************************/
/** \brief Remove the given node from the list.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_Remove(clist_node *head, clist_node node)
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

/************************************************************************/
/** \brief Insert the given node into the last after the given start node.
**
**  \par Assumptions, External Events, and Notes:
**       head must not be NULL. node must not be NULL.
**
*************************************************************************/
void CF_CList_InsertAfter(clist_node *head, clist_node start, clist_node after)
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

/************************************************************************/
/** \brief Traverse the entire list, calling the given function on all nodes.
**
**  \par Assumptions, External Events, and Notes:
**       start may be NULL. fn must be a valid function. context may be NULL.
**
*************************************************************************/
void CF_CList_Traverse(clist_node start, clist_fn_t fn, void *context)
{
    clist_node n = start;
    clist_node nn;
    int        last = 0;

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

/************************************************************************/
/** \brief Reverse list traversal, starting from end, calling given function on all nodes.
**
**  \par Assumptions, External Events, and Notes:
**       end may be NULL. fn must be a valid function. context may be NULL.
**
*************************************************************************/
void CF_CList_Traverse_R(clist_node end, clist_fn_t fn, void *context)
{
    if (end)
    {
        clist_node n = end->prev;
        clist_node nn;
        int        last = 0;

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
