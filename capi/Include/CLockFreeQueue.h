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

/* 
 *	CLockFreeQueue.h
 *
 *	DESCRIPTION
 *		Module for Lock-free queue class 
 *
 *	HISTORY
 *		04-08-2008	create by zhouweiming.
 *
 */
#ifndef __CLOCKFREEQUEUE_H__
#define __CLOCKFREEQUEUE_H__


#include "dmalloc.h"

class CLockFreeQueue {
private:
    SINGLENODE *   m_pHead;
    SINGLENODE *   m_pTail;
    SINGLENODE *   m_pDummyNode;
    DESTROYFUNC  m_DestroyFunc;
public:
    CLockFreeQueue() {
        m_pDummyNode = new SINGLENODE;
        m_pDummyNode->pData = NULL;
        m_pDummyNode->pNext = NULL;
        m_pHead = m_pDummyNode; 
        m_pTail = m_pDummyNode;

        m_DestroyFunc = NULL;
    };
    virtual ~CLockFreeQueue();

    void SetDataDestroyFunc(DESTROYFUNC DestroyFunc) {
        m_DestroyFunc = DestroyFunc;
    };

    void *DeQueue();
    void EnQueue(void *pData);

};


#endif //__CLOCKFREEQUEUE_H__
