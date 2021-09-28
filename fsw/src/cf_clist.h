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

#ifndef __CF_CLIST_H_
#define __CF_CLIST_H_

#include <stddef.h>

#define CLIST_CONT 0
#define CLIST_EXIT 1

/* circular linked list */

struct clist_node_t;
typedef struct clist_node_t {
    struct clist_node_t *next;
    struct clist_node_t *prev;
} clist_node_t;

typedef struct clist_node_t *clist_node;

/* good a place as any to put container_of for CF */
#define container_of(ptr, type, member) \
    ((type*)((char*)(ptr) - (char*)offsetof(type, member)))

void CF_CList_InitNode(clist_node node);

/* in the functions, head is the list head */
void CF_CList_InsertFront(clist_node *head, clist_node node);
void CF_CList_InsertBack(clist_node *head, clist_node node);
void CF_CList_Remove(clist_node *head, clist_node node);
clist_node CF_CList_Pop(clist_node *head);
void CF_CList_InsertAfter(clist_node *head, clist_node start, clist_node after);

/* NOTE: if clist_fn_t returns non-zero, the list traversal stops */
typedef int (*clist_fn_t)(clist_node node, void *context);

/* NOTE on traversal: it's ok to delete the current node, but do not delete
 * other nodes in the same list!! */
void CF_CList_Traverse(clist_node start, clist_fn_t fn, void *context);
/* NOTE: traverse_R will work backwards from the parameter's prev, and end on param */
void CF_CList_Traverse_R(clist_node end, clist_fn_t fn, void *context);

#endif /* !__CF_CLIST_H_ */

