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
    UINT            m_uHead;     // ����ͷ��λ�� 
    UINT            m_uTail;     // ����β��λ�� 
    UINT            m_uMaxSize;	 // �����е������������ 
    T	*           m_pData;	 // ������ݵ����� 
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
        //ͷβ�غϱ�ʾ����Ϊ��
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
        // ���������β������Ҫ���´�ͷ���¿�ʼ
        uTailNext = 0;
    }
    else
    {
        uTailNext = m_uTail + 1;
    }
    
    if ( uTailNext == m_uHead )
    {
        UINT i;
        //������������������һ����������ԭ�����е�����
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
        // �����ݲ��뵽�����Ķ���β��
        pNewData[m_uTail] = data;
        m_uTail += 1;
        m_uMaxSize *= 2;
        delete [] m_pData;
        m_pData = pNewData;
    }
    else
    {
        // ����δ��������������ݲ���
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
