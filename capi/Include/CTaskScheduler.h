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

/**	CTaskScheduler类的构造函数

	@param	int nThreadCount - 线程数量	
	@return	 - 无	
*/
template <class T>
CTaskScheduler<T>::CTaskScheduler(int nThreadCount)
{
    m_nThreadCount = nThreadCount;
    m_DQueue.Create(128, 128, nThreadCount,nThreadCount);
}


/**	任务调度的创建线程池函数

	@param	THREADFUNC StartFunc - 线程池入口函数	
	@param	void *pArg - 线程池入口函数的参数	
	@return	void - 无	
*/
template <class T>
void CTaskScheduler<T>::CreateThreadPool(THREADFUNC StartFunc, void *pArg)
{
    m_ThreadPool.Create(StartFunc, pArg, m_nThreadCount);

    m_ThreadPool.ExecAndWait();
}

/**	任务调度的弹出任务函数

	@param	T &Data - 弹出的任务数据	
	@return	int - 返回CAPI_SUCCESS表示成功，返回CAPI_FAILED表示失败	
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

/**	任务调度的压入任务函数
    任务均被压入到共享队列池中

	@param	T &Data - 要压入的任务数据	
	@return	int - 返回CAPI_SUCCESS表示成功，返回CAPI_FAILED表示失败
*/
template <class T>
int CTaskScheduler<T>::PushTask(T &Data)
{
    CQueuePool<T, CStealQueue<T>> *queue_pool = m_DQueue.GetSharedQueue();

    return queue_pool->Push(Data); 
}



/**	任务调度的弹出任务函数
    采用循环等待的方式一直运行到弹出任务为止
    如果队列一直为空，那么将一直循环等待下去

    @param	T &Data - 弹出的任务数据	
    @return	void - 无	
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

/**	任务调度的压入任务函数
    任务均被压入共享队列池中
    这个函数采用了循环等待的方式将数据压入到队列池中，
    如果队列一直满，那么将一直循环下去，
    因此必须保证有其他线程会从队列池中取走数据

    @param	T &Data - 要压入的任务数据	
    @return	void - 无	
*/
template <class T>
void CTaskScheduler<T>::PushTaskAndWait(T &Data)
{
    CQueuePool<T, CStealQueue<T>> *queue_pool = m_DQueue.GetSharedQueue();

    //下面代码采用循环等待方式将数据压入队列池中
    //如果队列池一直满，将一直循环下去
    //如果只有这一个线程在运行，那么会死循环
    while ( queue_pool->Push(Data) == CAPI_FAILED )
    {
        CAPI_Yield();
    }
    return;
}

#endif //__CTASKSCHEDULER_H__

