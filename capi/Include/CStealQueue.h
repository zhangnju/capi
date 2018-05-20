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
 *	CStealQueue.h
 *
 *	DESCRIPTION
 *		Module for Steal Queue class 
 *
 *	HISTORY
 *		06-02-2008	create by zhouweiming.
 *
 */
#ifndef __CSTEALQUEUE_H__
#define __CSTEALQUEUE_H__

#include  <windows.h>
#include "CapiGlobal.h"
#include "FastLock.h"
#include "CScopedLock.h"

#define     DEFAULT_STEALQUEUE_SIZE    4

template <class T> class CStealQueue {
PRIVATE:
    UINT            m_uHead;     // 队列头部位置 
    UINT            m_uTail;     // 队列尾部位置 
    UINT            m_uMaxSize;	 // 队列中的最大数据数量 
    T	*           m_pData;	 // 存放数据的数组 
    LONG volatile   m_lFullFlag; // 队列为满的标志
    LONG volatile   m_lEmptyFlag; // 队列为空的标志
    CFastLock       m_Lock;      // 锁
public:
    typedef         CStealQueue     SubQueue;
public:
    CStealQueue() { m_pData = NULL; m_lFullFlag = 0; m_lEmptyFlag = 1;};
    CStealQueue(UINT uMaxSize);
    CStealQueue(UINT uQueueCount, UINT uMaxSize); //为兼容性而设计的构造函数
                                                  //主要为方便分布式队列使用          
    virtual ~CStealQueue()
    {
        if ( m_pData != NULL )
        {
            delete [] m_pData;
        }
    };
    
    void Create(UINT uMaxSize);
    int EnQueue(T &Data);
    int DeQueue(T &Data);

    int Push(T Data) { return EnQueue(Data);};
    int Pop(T &Data) { return DeQueue(Data);};

    int IsFull();
    int IsEmpty();

    void Lock();
    void Unlock();
    int EnQueueNoLock(T &Data);
    int DeQueueNoLock(T &Data);

    UINT GetCountNoLock(); //获取队列中的数据个数
    SubQueue *GetSubQueue() { return this; };
};


template <class T> 
CStealQueue<T>::CStealQueue(UINT uMaxSize)
{
    Create(uMaxSize);
}


template <class T> 
CStealQueue<T>::CStealQueue(UINT uQueueCount, UINT uMaxSize)
{
    Create(uMaxSize);
}


template <class T> 
void CStealQueue<T>::Create(UINT uMaxSize)
{
    if ( uMaxSize < DEFAULT_STEALQUEUE_SIZE )
    {
        m_uMaxSize = DEFAULT_STEALQUEUE_SIZE;
    }
    else
    {
        m_uMaxSize = uMaxSize;
    }
    m_uHead = 0;
    m_uTail = 0;
    m_lFullFlag = 0;
    m_lEmptyFlag = 1;
    m_pData = new T[uMaxSize+1];
}


template <class T> 
int CStealQueue<T>::EnQueue(T &Data)
{
    UINT    uTailNext;

    if ( m_lFullFlag )
    {
        return CAPI_FAILED;
    }

    CScopedLock<CFastLock> fLock(m_Lock);

    if ( m_uTail == m_uMaxSize )
    {
        // 到了数组的尾部，需要重新从头重新开始
        uTailNext = 0;
    }
    else
    {
        uTailNext = m_uTail + 1;
    }

    if ( uTailNext == m_uHead )
    {
        return CAPI_FAILED; //队列为满的状态，返回失败
    }
    // 队列未满的情况，将数据插入
    m_pData[m_uTail] = Data;

    if ( m_uTail == m_uHead )
    {
        //队列原来为空，需要将队列空标志改为0
        AtomicDecrement(&m_lEmptyFlag);
    }

    m_uTail = uTailNext;

    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //插入数据后，队列变成满的情况，需要将m_lFullFlag改为1
        AtomicIncrement(&m_lFullFlag);
    }
    
    return CAPI_SUCCESS;   
}


template <class T> 
int CStealQueue<T>::DeQueue(T &Data)
{
    UINT    uHead;

    if ( m_lEmptyFlag )
    {
        return CAPI_FAILED;
    }

    CScopedLock<CFastLock> fLock(m_Lock);

    uHead = m_uHead;
    if ( uHead == m_uTail )
    {
        //头尾重合表示队列为空
        return CAPI_FAILED;
    }

    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //队列满的情况，出队操作后会变成非满的状态，因此需要将m_lFullFlag改为0
        AtomicDecrement(&m_lFullFlag);
    }

    if ( uHead == m_uMaxSize )
    {
        m_uHead = 0;
    }
    else
    {
        m_uHead += 1;
    }
    Data = m_pData[uHead];

    if ( m_uHead == m_uTail )
    {
        //出队后，队列变空，需要将m_lEmptyFlag改为1
        AtomicIncrement(&m_lEmptyFlag);
    }

    return CAPI_SUCCESS;
}


template <class T> 
int CStealQueue<T>::IsFull()
{
    return m_lFullFlag;
}


template <class T> 
int CStealQueue<T>::IsEmpty()
{
    return m_lEmptyFlag;
}

template <class T> 
void CStealQueue<T>::Lock()
{
    m_Lock.Acquire();
}

template <class T> 
void CStealQueue<T>::Unlock()
{
    m_Lock.Release();
}

template <class T> 
int CStealQueue<T>::EnQueueNoLock(T &Data)
{
    UINT    uTailNext;

    if ( m_uTail == m_uMaxSize )
    {
        // 到了数组的尾部，需要重新从头重新开始
        uTailNext = 0;
    }
    else
    {
        uTailNext = m_uTail + 1;
    }
    
    if ( uTailNext == m_uHead )
    {
        return CAPI_FAILED; //队列满，返回失败
    }

    if ( m_uTail == m_uHead )
    {
        //队列原来为空，需要将队列空标志改为0
        AtomicDecrement(&m_lEmptyFlag);
    }

    m_pData[m_uTail] = Data;

    m_uTail = uTailNext;

    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //插入数据后，队列变成满的情况，需要将m_lFullFlag改为1
        AtomicIncrement(&m_lFullFlag);
    }

    return CAPI_SUCCESS;   
}


template <class T> 
int CStealQueue<T>::DeQueueNoLock(T &Data)
{
    UINT    uHead;

    uHead = m_uHead;
    if ( uHead == m_uTail )
    {
        //头尾重合表示队列为空
        return CAPI_FAILED;
    }
    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //队列满的情况，出队操作后会编程非满的状态，因此需要将m_lFullFlag改为0
        AtomicDecrement(&m_lFullFlag);
    }
    if ( uHead == m_uMaxSize )
    {
        m_uHead = 0;
    }
    else
    {
        m_uHead += 1;
    }
    Data = m_pData[uHead];
    
    if ( m_uHead == m_uTail )
    {
        //出队后，队列变空，需要将m_lEmptyFlag改为1
        AtomicIncrement(&m_lEmptyFlag);
    }

    return CAPI_SUCCESS;
}


template <class T> 
UINT CStealQueue<T>::GetCountNoLock( )
{
    UINT    uCount; 
    if ( m_uHead <= m_uTail )
    {
        uCount = m_uTail - m_uHead;
    }
    else
    {
        uCount = m_uTail + m_uMaxSize + 1 - m_uHead;
    }
    return uCount;
}


#endif //__CSTEALQUEUE_H__
