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
 * Auto-Generated stub implementations for functions defined in cf_clist header
 */

#include "cf_clist.h"
#include "utgenstub.h"

void UT_DefaultHandler_CF_CList_InitNode(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CList_InsertAfter(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CList_InsertBack(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CList_Remove(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CList_Traverse(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CF_CList_Traverse_R(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_InitNode()
 * ----------------------------------------------------
 */
void CF_CList_InitNode(CF_CListNode_t *node)
{
    UT_GenStub_AddParam(CF_CList_InitNode, CF_CListNode_t *, node);

    UT_GenStub_Execute(CF_CList_InitNode, Basic, UT_DefaultHandler_CF_CList_InitNode);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_InsertAfter()
 * ----------------------------------------------------
 */
void CF_CList_InsertAfter(CF_CListNode_t **head, CF_CListNode_t *start, CF_CListNode_t *after)
{
    UT_GenStub_AddParam(CF_CList_InsertAfter, CF_CListNode_t **, head);
    UT_GenStub_AddParam(CF_CList_InsertAfter, CF_CListNode_t *, start);
    UT_GenStub_AddParam(CF_CList_InsertAfter, CF_CListNode_t *, after);

    UT_GenStub_Execute(CF_CList_InsertAfter, Basic, UT_DefaultHandler_CF_CList_InsertAfter);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_InsertBack()
 * ----------------------------------------------------
 */
void CF_CList_InsertBack(CF_CListNode_t **head, CF_CListNode_t *node)
{
    UT_GenStub_AddParam(CF_CList_InsertBack, CF_CListNode_t **, head);
    UT_GenStub_AddParam(CF_CList_InsertBack, CF_CListNode_t *, node);

    UT_GenStub_Execute(CF_CList_InsertBack, Basic, UT_DefaultHandler_CF_CList_InsertBack);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_InsertFront()
 * ----------------------------------------------------
 */
void CF_CList_InsertFront(CF_CListNode_t **head, CF_CListNode_t *node)
{
    UT_GenStub_AddParam(CF_CList_InsertFront, CF_CListNode_t **, head);
    UT_GenStub_AddParam(CF_CList_InsertFront, CF_CListNode_t *, node);

    UT_GenStub_Execute(CF_CList_InsertFront, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_Pop()
 * ----------------------------------------------------
 */
CF_CListNode_t *CF_CList_Pop(CF_CListNode_t **head)
{
    UT_GenStub_SetupReturnBuffer(CF_CList_Pop, CF_CListNode_t *);

    UT_GenStub_AddParam(CF_CList_Pop, CF_CListNode_t **, head);

    UT_GenStub_Execute(CF_CList_Pop, Basic, NULL);

    return UT_GenStub_GetReturnValue(CF_CList_Pop, CF_CListNode_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_Remove()
 * ----------------------------------------------------
 */
void CF_CList_Remove(CF_CListNode_t **head, CF_CListNode_t *node)
{
    UT_GenStub_AddParam(CF_CList_Remove, CF_CListNode_t **, head);
    UT_GenStub_AddParam(CF_CList_Remove, CF_CListNode_t *, node);

    UT_GenStub_Execute(CF_CList_Remove, Basic, UT_DefaultHandler_CF_CList_Remove);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_Traverse()
 * ----------------------------------------------------
 */
void CF_CList_Traverse(CF_CListNode_t *start, CF_CListFn_t fn, void *context)
{
    UT_GenStub_AddParam(CF_CList_Traverse, CF_CListNode_t *, start);
    UT_GenStub_AddParam(CF_CList_Traverse, CF_CListFn_t, fn);
    UT_GenStub_AddParam(CF_CList_Traverse, void *, context);

    UT_GenStub_Execute(CF_CList_Traverse, Basic, UT_DefaultHandler_CF_CList_Traverse);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CF_CList_Traverse_R()
 * ----------------------------------------------------
 */
void CF_CList_Traverse_R(CF_CListNode_t *end, CF_CListFn_t fn, void *context)
{
    UT_GenStub_AddParam(CF_CList_Traverse_R, CF_CListNode_t *, end);
    UT_GenStub_AddParam(CF_CList_Traverse_R, CF_CListFn_t, fn);
    UT_GenStub_AddParam(CF_CList_Traverse_R, void *, context);

    UT_GenStub_Execute(CF_CList_Traverse_R, Basic, UT_DefaultHandler_CF_CList_Traverse_R);
}
