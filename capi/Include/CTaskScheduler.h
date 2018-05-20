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
 *	CTaskScheduler.h
 *
 *	DESCRIPTION
 *		Module for Non-Nest Task Schedule class 
 *
 *	HISTORY
 *		03-05-2008	create by zhouweiming.
 *
 */
#ifndef __CTASKSCHEDULER_H__
#define __CTASKSCHEDULER_H__
#define _WIN32_WINNT 0x0401
#include <Windows.h>
#include "CapiGlobal.h"
#include "CLocalQueue.h"
#include "CStealQueue.h"
#include "CQueuePool.h"
#include "CThreadPool.h"
#include "CDistributedQueue.h"

template <class T>
class CTaskScheduler {
private:
    int             m_nThreadCount;
    CThreadPool     m_ThreadPool;
    CDistributedQueue<T, CLocalQueue<T>, CQueuePool<T,CStealQueue<T>>> m_DQueue;
public:
    CTaskScheduler(int nThreadCount);
    virtual ~CTaskScheduler(){};

    void CreateThreadPool(THREADFUNC StartFunc, void *pArg);
    int PopTask(T &Data);
    int PushTask(T &Data);

    void PopTaskAndWait(T &Data);
    void PushTaskAndWait(T &Data);
};

/**	CTaskScheduler��Ĺ��캯��

	@param	int nThreadCount - �߳�����	
	@return	 - ��	
*/
template <class T>
CTaskScheduler<T>::CTaskScheduler(int nThreadCount)
{
    m_nThreadCount = nThreadCount;
    m_DQueue.Create(128, 128, nThreadCount,nThreadCount);
}


/**	������ȵĴ����̳߳غ���

	@param	THREADFUNC StartFunc - �̳߳���ں���	
	@param	void *pArg - �̳߳���ں����Ĳ���	
	@return	void - ��	
*/
template <class T>
void CTaskScheduler<T>::CreateThreadPool(THREADFUNC StartFunc, void *pArg)
{
    m_ThreadPool.Create(StartFunc, pArg, m_nThreadCount);

    m_ThreadPool.ExecAndWait();
}

/**	������ȵĵ���������

	@param	T &Data - ��������������	
	@return	int - ����CAPI_SUCCESS��ʾ�ɹ�������CAPI_FAILED��ʾʧ��	
*/
template <class T>
int CTaskScheduler<T>::PopTask(T &Data)
{
    int nRet;
    nRet = m_DQueue.PopFromLocalQueue(Data);
    if ( nRet == CAPI_FAILED )
    {
        int start = m_DQueue.ThreadIdGet();
        int flag = 0;
        int i;

        CQueuePool<T, CStealQueue<T>> *pQueue = m_DQueue.GetSharedQueue();

        for ( i = start; i < pQueue->GetQueueCount(); i++)
        {
            if ( m_DQueue.PrivatizeSharedQueue(i) == CAPI_SUCCESS)
            {
                flag = 1;
                break;
            }
        }
        if ( flag == 0 )
        {
            for ( i = 0; i < start; i++)
            {
                if ( m_DQueue.PrivatizeSharedQueue(i) == CAPI_SUCCESS)
                {
                    flag = 1;
                    break;
                }
            }
        }
        if ( flag == 1 )
        {
            nRet = m_DQueue.PopFromLocalQueue(Data);
        }
    }
    return nRet;
}

/**	������ȵ�ѹ��������
    �������ѹ�뵽������г���

	@param	T &Data - Ҫѹ�����������	
	@return	int - ����CAPI_SUCCESS��ʾ�ɹ�������CAPI_FAILED��ʾʧ��
*/
template <class T>
int CTaskScheduler<T>::PushTask(T &Data)
{
    CQueuePool<T, CStealQueue<T>> *queue_pool = m_DQueue.GetSharedQueue();

    return queue_pool->Push(Data); 
}



/**	������ȵĵ���������
    ����ѭ���ȴ��ķ�ʽһֱ���е���������Ϊֹ
    �������һֱΪ�գ���ô��һֱѭ���ȴ���ȥ

    @param	T &Data - ��������������	
    @return	void - ��	
*/
template <class T>
void CTaskScheduler<T>::PopTaskAndWait(T &Data)
{
    while ( PopTask(Data) == CAPI_FAILED )
    {
        CAPI_Yield();
    }
    return;
}

/**	������ȵ�ѹ��������
    �������ѹ�빲����г���
    �������������ѭ���ȴ��ķ�ʽ������ѹ�뵽���г��У�
    �������һֱ������ô��һֱѭ����ȥ��
    ��˱��뱣֤�������̻߳�Ӷ��г���ȡ������

    @param	T &Data - Ҫѹ�����������	
    @return	void - ��	
*/
template <class T>
void CTaskScheduler<T>::PushTaskAndWait(T &Data)
{
    CQueuePool<T, CStealQueue<T>> *queue_pool = m_DQueue.GetSharedQueue();

    //����������ѭ���ȴ���ʽ������ѹ����г���
    //������г�һֱ������һֱѭ����ȥ
    //���ֻ����һ���߳������У���ô����ѭ��
    while ( queue_pool->Push(Data) == CAPI_FAILED )
    {
        CAPI_Yield();
    }
    return;
}

#endif //__CTASKSCHEDULER_H__

