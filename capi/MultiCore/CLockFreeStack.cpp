/*
 * Copyright (c) 2006-2008
 * Author: Weiming Zhou
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  
 */

#include <windows.h>
#include "CapiGlobal.h"
#include "dmalloc.h"
#include "CLockFreeStack.h"


CLockFreeStack::~CLockFreeStack()
{
    SINGLENODE    *pNode;
    SINGLENODE    *pTempNode;

    pNode = m_Head.Ptr;
    while ( pNode != NULL )
    {
        pTempNode = pNode;
        pNode = pNode->pNext;

        if ( m_DestroyFunc != NULL )
        {
            (*m_DestroyFunc)(pTempNode->pData);
        }
        dfree(pTempNode);
    }
}


void *CLockFreeStack::Pop()
{
    void *pData = NULL;

    LINK Old;
    LINK New;
    SINGLENODE *pNode;

    do {
        Old = m_Head;
        pNode = Old.Ptr;

        if ( pNode != NULL )
        {
            New.Ptr = pNode->pNext;
        }
        else
        {
            return NULL;
        }
        New.nRefCount = Old.nRefCount + 1;
        if ( *((LONGLONG *)(&Old)) != *((LONGLONG*)(&m_Head)) )
        {
            continue;
        }
    } while ( AtomicCAS64( (LONGLONG volatile *)&m_Head, 
        *((LONGLONG *)(&New)), *((LONGLONG*)(&Old)) ));

    pData = pNode->pData;
    dfree( pNode );

    return pData;
}



void CLockFreeStack::Push(void *pData)
{
    SINGLENODE *pNode = (SINGLENODE *)dmalloc( sizeof(SINGLENODE) );

    pNode->pData = pData;

    LINK Old;
    LINK New;

    do {
        Old = m_Head;
        pNode->pNext = Old.Ptr;
        New.nRefCount = Old.nRefCount + 1;
        New.Ptr = pNode;
    } while ( AtomicCAS64( (LONGLONG volatile *)&m_Head, 
        *((LONGLONG *)(&New)), 
        *((LONGLONG *)(&Old)) ) );    
}



