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
    UINT            m_uHead;     // ����ͷ��λ�� 
    UINT            m_uTail;     // ����β��λ�� 
    UINT            m_uMaxSize;	 // �����е������������ 
    T	*           m_pData;	 // ������ݵ����� 
    LONG volatile   m_lFullFlag; // ����Ϊ���ı�־
    LONG volatile   m_lEmptyFlag; // ����Ϊ�յı�־
    CFastLock       m_Lock;      // ��
public:
    typedef         CStealQueue     SubQueue;
public:
    CStealQueue() { m_pData = NULL; m_lFullFlag = 0; m_lEmptyFlag = 1;};
    CStealQueue(UINT uMaxSize);
    CStealQueue(UINT uQueueCount, UINT uMaxSize); //Ϊ�����Զ���ƵĹ��캯��
                                                  //��ҪΪ����ֲ�ʽ����ʹ��          
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

    UINT GetCountNoLock(); //��ȡ�����е����ݸ���
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
        // ���������β������Ҫ���´�ͷ���¿�ʼ
        uTailNext = 0;
    }
    else
    {
        uTailNext = m_uTail + 1;
    }

    if ( uTailNext == m_uHead )
    {
        return CAPI_FAILED; //����Ϊ����״̬������ʧ��
    }
    // ����δ��������������ݲ���
    m_pData[m_uTail] = Data;

    if ( m_uTail == m_uHead )
    {
        //����ԭ��Ϊ�գ���Ҫ�����пձ�־��Ϊ0
        AtomicDecrement(&m_lEmptyFlag);
    }

    m_uTail = uTailNext;

    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //�������ݺ󣬶��б�������������Ҫ��m_lFullFlag��Ϊ1
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
        //ͷβ�غϱ�ʾ����Ϊ��
        return CAPI_FAILED;
    }

    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //����������������Ӳ�������ɷ�����״̬�������Ҫ��m_lFullFlag��Ϊ0
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
        //���Ӻ󣬶��б�գ���Ҫ��m_lEmptyFlag��Ϊ1
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
        // ���������β������Ҫ���´�ͷ���¿�ʼ
        uTailNext = 0;
    }
    else
    {
        uTailNext = m_uTail + 1;
    }
    
    if ( uTailNext == m_uHead )
    {
        return CAPI_FAILED; //������������ʧ��
    }

    if ( m_uTail == m_uHead )
    {
        //����ԭ��Ϊ�գ���Ҫ�����пձ�־��Ϊ0
        AtomicDecrement(&m_lEmptyFlag);
    }

    m_pData[m_uTail] = Data;

    m_uTail = uTailNext;

    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //�������ݺ󣬶��б�������������Ҫ��m_lFullFlag��Ϊ1
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
        //ͷβ�غϱ�ʾ����Ϊ��
        return CAPI_FAILED;
    }
    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead))
    {
        //����������������Ӳ�������̷�����״̬�������Ҫ��m_lFullFlag��Ϊ0
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
        //���Ӻ󣬶��б�գ���Ҫ��m_lEmptyFlag��Ϊ1
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
