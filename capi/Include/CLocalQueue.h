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
*	CLocalQueue.h
*
*	DESCRIPTION
*		Module for Locale Queue class 
*
*	HISTORY
*		06-02-2008	create by zhouweiming.
*
*/
#ifndef __CLOCALQUEUE_H__
#define __CLOCALQUEUE_H__


#include "CapiGlobal.h"
#include "FastLock.h"
#include "CScopedLock.h"

template <class T> class CLocalQueue {
PRIVATE:
    UINT            m_uHead;     // 队列头部位置 
    UINT            m_uTail;     // 队列尾部位置 
    UINT            m_uMaxSize;	 // 队列中的最大数据数量 
    T	*           m_pData;	 // 存放数据的数组 
public:
    CLocalQueue(){ m_pData = NULL;};
    CLocalQueue(UINT uMaxSize);
    virtual ~CLocalQueue()
    {
        if ( m_pData != NULL )
        {
            delete [] m_pData;
        }
    };
    
    void Create(UINT uMaxSize);

    int  EnQueue(T &Data);
    int  DeQueue(T &Data);

    bool IsEmpty();
    bool IsFull();
};


template <class T> 
CLocalQueue<T>::CLocalQueue(UINT uMaxSize)
{
    m_uMaxSize = uMaxSize;
    m_uHead = 0;
    m_uTail = 0;
    m_pData = new T[uMaxSize+1];
}

template <class T> 
void CLocalQueue<T>::Create(UINT uMaxSize)
{
    if ( m_pData != NULL )
    {
        return;
    }
    m_uMaxSize = uMaxSize;
    m_uHead = 0;
    m_uTail = 0;
    m_pData = new T[uMaxSize+1];
}


template <class T> 
int CLocalQueue<T>::DeQueue(T &data)
{
    UINT    uHead;
    
    uHead = m_uHead;
    if ( uHead == m_uTail )
    {
        //头尾重合表示队列为空
        return CAPI_FAILED;
    }
    if ( uHead == m_uMaxSize )
    {
        m_uHead = 0;
    }
    else
    {
        m_uHead += 1;
    }
    data = m_pData[uHead];

    return CAPI_SUCCESS;
}

template <class T> 
int CLocalQueue<T>::EnQueue(T &data)
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
        UINT i;
        //队列满，将队列扩大一倍，并拷贝原队列中的数据
        T *pNewData = new T[2*m_uMaxSize + 1];
        if ( m_uHead < m_uTail )
        {
            for ( i = m_uHead; i < m_uTail; i++ )
            {
                pNewData[i] = m_pData[i];
            }
        }
        else
        {
            for ( i = m_uHead; i <= m_uMaxSize; i++ )
            {
                pNewData[i] = m_pData[i];
            }
            for ( i = 0; i < m_uTail; i++ )
            {
                pNewData[i + m_uMaxSize+1] = m_pData[i];
            }
            m_uTail += m_uMaxSize+1;
        }
        // 将数据插入到扩大后的队列尾部
        pNewData[m_uTail] = data;
        m_uTail += 1;
        m_uMaxSize *= 2;
        delete [] m_pData;
        m_pData = pNewData;
    }
    else
    {
        // 队列未满的情况，将数据插入
        m_pData[m_uTail] = data;
        m_uTail = uTailNext;

    }
    return CAPI_SUCCESS;   
}



template <class T> 
bool CLocalQueue<T>::IsEmpty()
{
    return (m_uHead == m_uTail ? true : false);
}


template <class T> 
bool CLocalQueue<T>::IsFull()
{
    if ( (m_uTail + 1 == m_uHead) || (m_uTail - m_uMaxSize == m_uHead) )
    {
        return true;
    }
    else
    {
        return false;
    }
}

#endif //__CLOCALQUEUE_H__
