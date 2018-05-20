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
 *	CSharedQueue.h
 *
 *	DESCRIPTION
 *		Module for Shared Queue class
 *       It use lock to protect the EnQueue and DeQueue Operation.
 *
 *	HISTORY
 *		06-10-2008	create by zhouweiming.
 *
 */
#ifndef __CSHAREDQUEUE_H__
#define __CSHAREDQUEUE_H__


#include "CScopedLock.h"

template <class T, class TQueue>
class CSharedQueue 
{
private:
    TQueue      m_queue;
    int         m_nQueueSize;
    CFastLock   m_lock;

public:
    CSharedQueue();
    CSharedQueue(int nQueueSize);
    virtual ~CSharedQueue(){};

    void Create(int nQueueSize);

    int EnQueue(T &Data);
    int DeQueue(T &Data);

};


template <class T, class TQueue>
CSharedQueue<T, TQueue>::CSharedQueue()
{
    m_nQueueSize = 0;
}

template <class T, class TQueue>
CSharedQueue<T, TQueue>::CSharedQueue(int nQueueSize)
{
    m_nQueueSize = nQueueSize;

    m_queue.Create(nQueueSize);
}


template <class T, class TQueue>
void CSharedQueue<T, TQueue>::Create(int nQueueSize)
{
    m_nQueueSize = nQueueSize;
    m_queue.Create(nQueueSize);
}


template <class T, class TQueue>
int CSharedQueue<T, TQueue>::EnQueue(T &Data)
{
    int nRet; 

    m_lock.Acquire();
    nRet = m_queue.EnQueue(Data);
    m_lock.Release();

    return nRet;
}


template <class T, class TQueue>
int CSharedQueue<T, TQueue>::DeQueue(T &Data)
{
    int nRet; 

    m_lock.Acquire();
    nRet = m_queue.DeQueue(Data);
    m_lock.Release();

    return nRet;
}

#endif //__CSHAREDQUEUE_H__
