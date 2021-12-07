/************************************************************************
** File: cf_clist.h
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
**  The CF Application circular list header file
**
**
**
*************************************************************************/

#ifndef CF_CLIST_H
#define CF_CLIST_H

#include <stddef.h>

#define CF_CLIST_CONT 0
#define CF_CLIST_EXIT 1

/* circular linked list */

struct CF_CListNode
{
    struct CF_CListNode *next;
    struct CF_CListNode *prev;
};

typedef struct CF_CListNode CF_CListNode_t;

/* good a place as any to put container_of for CF */
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - (char *)offsetof(type, member)))

void CF_CList_InitNode(CF_CListNode_t *node);

/* in the functions, head is the list head */
void            CF_CList_InsertFront(CF_CListNode_t **head, CF_CListNode_t *node);
void            CF_CList_InsertBack(CF_CListNode_t **head, CF_CListNode_t *node);
void            CF_CList_Remove(CF_CListNode_t **head, CF_CListNode_t *node);
CF_CListNode_t *CF_CList_Pop(CF_CListNode_t **head);
void            CF_CList_InsertAfter(CF_CListNode_t **head, CF_CListNode_t *start, CF_CListNode_t *after);

/* NOTE: if CF_CListFn_t  returns non-zero, the list traversal stops */
typedef int (*CF_CListFn_t)(CF_CListNode_t *node, void *context);

/* NOTE on traversal: it's ok to delete the current node, but do not delete
 * other nodes in the same list!! */
void CF_CList_Traverse(CF_CListNode_t *start, CF_CListFn_t fn, void *context);
/* NOTE: traverse_R will work backwards from the parameter's prev, and end on param */
void CF_CList_Traverse_R(CF_CListNode_t *end, CF_CListFn_t fn, void *context);

#endif /* !CF_CLIST_H */
