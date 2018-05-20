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
 *	CMsgQueue.h
 *
 *	DESCRIPTION
 *		Module for Message Queue class 
 *
 *	HISTORY
 *		08-05-2008	create by zhouweiming.
 *
 */
#ifndef __CMSGQUEUE_H__
#define __CMSGQUEUE_H__


template <class T, class SharedQueue>
class CMsgQueue {
PRIVATE:
    SharedQueue  m_Queue;            
    int          m_nMaxSize;
    SEMAPHORE    m_SemaFull;
    SEMAPHORE    m_SemaEmpty;

    int          m_nFlag;
    int          Create(int nMaxSize);

public:
    CMsgQueue(int nMaxSize){ 
        Create(nMaxSize);
    };
    virtual ~CMsgQueue();

    void Send(T &Data);
    void Recv(T &Data);
};

template <class T, class SharedQueue>
int CMsgQueue<T, SharedQueue>::Create(int nMaxSize)
{
    m_nFlag = 0;
    m_nMaxSize = nMaxSize;
    m_SemaFull = SemaCreate(nMaxSize, nMaxSize);
    if ( m_SemaFull == NULL )
    {
        return CAPI_FAILED;
    }
    m_SemaEmpty = SemaCreate(0, nMaxSize);
    if ( m_SemaEmpty == NULL )
    {
        SemaClose(m_SemaFull);
        return CAPI_FAILED;
    }
    m_Queue.Create(nMaxSize);
    m_nFlag = 1;
    return CAPI_SUCCESS;
}


template <class T, class SharedQueue>
CMsgQueue<T, SharedQueue>::~CMsgQueue()
{
    if ( m_nFlag )
    {
        SemaClose(m_SemaFull);
        SemaClose(m_SemaEmpty);
    }
}


template <class T, class SharedQueue>
void CMsgQueue<T, SharedQueue>::Send(T &Data)
{
    SemaWait(m_SemaFull); //计数减1,为0则阻塞住,表示队列满
    m_Queue.EnQueue(Data);
    SemaRelease(m_SemaEmpty, 1); //计数加1
}


template <class T, class SharedQueue>
void CMsgQueue<T, SharedQueue>::Recv(T &Data)
{
    SemaWait(m_SemaEmpty); //计数减1,为0则阻塞住,表示队列空
    m_Queue.DeQueue(Data);
    SemaRelease(m_SemaFull, 1); //计数加1
}


#endif // __CMSGQUEUE_H__

