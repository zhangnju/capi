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
*	CDistributedQueue.h
*
*	DESCRIPTION
*		Module for Distributed Queue class 
*
*	HISTORY
*		06-08-2008	create by zhouweiming.
*
*/
#ifndef __CDISTRIBUTEDQUEUE_H__
#define __CDISTRIBUTEDQUEUE_H__

#if _DEBUG 
#include <stdio.h>
#endif

#include <omp.h>
#include "CapiGlobal.h"
#include "FastLock.h"
#include "CScopedLock.h"
#include "CapiGlobal.h"

//获取线程Id回调函数定义
typedef int (*GetThreadIdFunc)(void *pArg); 

template <class T, class LocalQueue, class SharedQueue> 
class CDistributedQueue {
private:
    LocalQueue **    m_ppLocalQueue;    // 本地队列数组
    SharedQueue *   m_pSharedQueue;     // 共享队列池或共享队列
  
    int             m_nLocalQueueSize;
    int             m_nSharedQueueSize;
    int             m_nLocalQueueCount;
    int             m_nSharedQueueCount; 
    DWORD           m_dwTlsIndex;        //线程本地存储索引
    LONG volatile   m_lThreadIdIndex;    //线程编号最大值
    GetThreadIdFunc m_GetThreadIdFunc;   //获取线程编号回调函数指针，如果由外面
										 //的线程池提供编号时，需要传入回调函数
    void *          m_pThreadIdFuncArg;  //获取线程编号回调函数的参数

    CFastLock       m_LocalQueueResizeLock; //专为下面的ResizeLocalQueue函数使用
    void ResizeLocalQueue();             //将m_ppLocalQueue数组的大小扩大一倍
public:

    CDistributedQueue(){
        m_GetThreadIdFunc = NULL;
        m_pThreadIdFuncArg = NULL;
        m_lThreadIdIndex = 0;
    };
    void Create( int nLocalQueueSize, int nLocalQueueCount, 
        int nSharedQueueSize, int nSharedQueueCount); 
    void Create( int nLocalQueueSize, int nLocalQueueCount, 
        int nSharedQueueSize, int nSharedQueueCount,
        GetThreadIdFunc GetThreadId, void * pThreadIdFuncArg);

    virtual ~CDistributedQueue();

    LONG ThreadIdGet();

    void EnQueue(T &Data);
    int  DeQueue(T &Data);

    void PushToLocalQueue(T &Data);
    void PushToLocalQueue(T &Data, int nIndex);
    int PopFromLocalQueue(T &Data);

    int PushToSharedQueue(T &Data);
    int PopFromSharedQueue(T &Data);

    SharedQueue *GetSharedQueue() { return m_pSharedQueue; };
    int PrivatizeSharedQueue(int nSharedQueueIndex);
};



/**	分布式队列的创建函数

	@param	int nLocalQueueSize - 本地子队列的大小	
	@param	int nLocalQueueCount - 本地队列的个数(数组的大小)
                                   为0表示和CPU核数相等的个数
	@param	int nSharedQueueSize - 共享子队列的大小	
	@param	int nSharedQueueCount - 共享子队列的个数	
                                    为0表示和CPU核数相等的个数
	@return	void - 无 
*/
template <class T, class LocalQueue, class SharedQueue> 
void CDistributedQueue<T, LocalQueue, SharedQueue>::Create(
                   int nLocalQueueSize, int nLocalQueueCount, 
                   int nSharedQueueSize, int nSharedQueueCount) 
{
    m_nLocalQueueSize = nLocalQueueSize;
    m_nSharedQueueSize = nSharedQueueSize;
    if ( nLocalQueueCount != 0 )
    {
        m_nLocalQueueCount = nLocalQueueCount;
    }
    else
    {
        m_nLocalQueueCount = omp_get_num_procs();
    }

    if ( nSharedQueueCount != 0 )
    {
        m_nSharedQueueCount = nSharedQueueCount;
    }
    else
    {
        m_nSharedQueueCount = omp_get_num_procs();
    }

    m_ppLocalQueue =  new LocalQueue *[m_nLocalQueueCount];
    int i;
    for ( i = 0; i < m_nLocalQueueCount; i++ )
    {
        m_ppLocalQueue[i] = NULL;
    }
    m_pSharedQueue = new SharedQueue(m_nSharedQueueCount, m_nSharedQueueSize);
    m_dwTlsIndex = TlsAlloc();
    m_lThreadIdIndex = 0;
}


/**	分布式队列的创建函数

	@param	int nLocalQueueSize - 本地子队列的大小	
	@param	int nLocalQueueCount - 本地队列的个数(数组的大小)	
	@param	int nSharedQueueSize - 共享子队列的大小	
	@param	int nSharedQueueCount - 共享子队列的个数	
	@param	GetThreadIdFunc GetThreadId - 获取线程Id回调函数	
	@param	void * pThreadIdFuncArg - GetThreadId回调函数的参数	
	@return	void - 无	
*/
template <class T, class LocalQueue, class SharedQueue> 
void CDistributedQueue<T, LocalQueue, SharedQueue>::Create(
    int nLocalQueueSize, int nLocalQueueCount, 
    int nSharedQueueSize, int nSharedQueueCount,
    GetThreadIdFunc GetThreadId, void * pThreadIdFuncArg)
{
    m_GetThreadIdFunc = GetThreadId;
    m_pThreadIdFuncArg = pThreadIdFuncArg;
    Create(nLocalQueueSize, nLocalQueueCount, nSharedQueueSize, nSharedQueueCount);
}



/**	分布式队列的析构函数

	@return	 - 无	
*/
template <class T, class LocalQueue, class SharedQueue> 
CDistributedQueue<T, LocalQueue, SharedQueue>::~CDistributedQueue()
{
    int i;
    for ( i = 0; i < m_nLocalQueueCount; i++ )
    {
        if ( m_ppLocalQueue[i] != NULL )
        {
            delete m_ppLocalQueue[i];
        }
    }
    delete [] m_ppLocalQueue;
    delete m_pSharedQueue;
    TlsFree(m_dwTlsIndex);
}

/**	分布式队列的将本地队列数组扩大一倍的内部成员函数

	@return	void - 无	
*/
template <class T, class LocalQueue, class SharedQueue> 
void CDistributedQueue<T, LocalQueue, SharedQueue>::ResizeLocalQueue()
{
    //将本地队列数组扩大一倍, 防止线程数量多于队列数量,以保证程序安全
    int i;

    LocalQueue **ppQueue = new LocalQueue *[m_nLocalQueueCount * 2];
    for ( i = 0; i < m_nLocalQueueCount; i++ )
    {
        ppQueue[i] = m_ppLocalQueue[i];
    }
    for ( i = m_nLocalQueueCount; i < m_nLocalQueueCount * 2; i++ )
    {
        ppQueue[i] = NULL;
    }
    delete [] m_ppLocalQueue;
    m_ppLocalQueue = ppQueue;

    //使用原子操作避免m_nLocalQueueCount的数据竞争问题
    AtomicWrite((LONG volatile *)&m_nLocalQueueCount, m_nLocalQueueCount * 2);

}

/**	分布式队列的获取线程Id函数
	如果m_GetThreadIdFunc回调函数不为空，则使用它获取Id
	否则根据分布式队列内部的编号机制获取线程Id

	@return	LONG - 返回线程的编号	
*/
template <class T, class LocalQueue, class SharedQueue> 
LONG CDistributedQueue<T, LocalQueue, SharedQueue>::ThreadIdGet()
{
    LONG Id;
    LocalQueue *pQueue = NULL;

    if ( m_GetThreadIdFunc != NULL )
    {
        Id = (*m_GetThreadIdFunc)(m_pThreadIdFuncArg);
        if ( Id >= m_nLocalQueueCount )
        {
            CScopedLock<CFastLock> slock(m_LocalQueueResizeLock);
            if ( Id >= m_nLocalQueueCount )
            {
                ResizeLocalQueue();
            }
        }
        if ( m_ppLocalQueue[Id] == NULL )
        {
            m_ppLocalQueue[Id] = new LocalQueue(m_nLocalQueueSize);
        }
        return Id;
    }
    else
    {
        Id = (LONG )TlsGetValue(m_dwTlsIndex);
        if ( Id == 0 )
        {
            Id = AtomicIncrement(&m_lThreadIdIndex);
            TlsSetValue(m_dwTlsIndex, (void *)Id);
            pQueue = new LocalQueue(m_nLocalQueueSize);
        }
        --Id;
    }

    if ( Id >= m_nLocalQueueCount)
    {
        CScopedLock<CFastLock> slock(m_LocalQueueResizeLock);
        if ( Id >= m_nLocalQueueCount )
        {
            ResizeLocalQueue();
        }
    }
    if ( pQueue != NULL )
    {
        m_ppLocalQueue[Id] = pQueue;
    }
    return Id;
}


/**	分布式队列的进队操作函数
	这里假定了本地队列可以无限进队
	进队策略按以下优先级进行：
	1、本地队列空时进入本地队列，2、共享队列未满时进入共享队列
	3、共享队列满时进入本地队列

	@param	T &Data - 要进队的数据	
	@return	void - 无	
*/
template <class T, class LocalQueue, class SharedQueue> 
void CDistributedQueue<T, LocalQueue, SharedQueue>::EnQueue(T &Data)
{
    int nId = ThreadIdGet();

    if ( m_ppLocalQueue[nId]->IsEmpty() )
    {
        m_ppLocalQueue[nId]->EnQueue(Data);
    }
    else if ( m_pSharedQueue->Push(Data) != CAPI_SUCCESS )
    {
        int nId = ThreadIdGet();
        m_ppLocalQueue[nId]->EnQueue(Data);
    }
    else
    {
        //这个分支不需要做任何事
    }
    return;
}

/**	分布式队列的本地队列进队函数
	将数据进入到当前线程的本地队列中

	@param	T &Data - 要进队的数据	
	@return	void - 无	
*/
template <class T, class LocalQueue, class SharedQueue> 
void CDistributedQueue<T, LocalQueue, SharedQueue>::PushToLocalQueue(T &Data)
{
    int nId = ThreadIdGet();
    m_ppLocalQueue[nId]->EnQueue(Data);
    return;
}

/**	分布式队列的指定序号本地队列进队函数
    这是一个为特殊需求而设计的函数
    使用这个函数要特别小心，必须保证不会发生数据竞争问题

	@param	T &Data - 要进队的数据	
	@param	int nIndex - 本地队列的序号	
	@return	void - 无	
*/
template <class T, class LocalQueue, class SharedQueue> 
void CDistributedQueue<T, LocalQueue, SharedQueue>::PushToLocalQueue(T &Data, int nIndex)
{
    if ( nIndex >= m_nLocalQueueCount * 2)
    {
        return;
    }

    if ( nIndex >= m_nLocalQueueCount )
    {
        CScopedLock<CFastLock> slock(m_LocalQueueResizeLock);
        if ( nIndex >= m_nLocalQueueCount )
        {
            ResizeLocalQueue();
        }
    }

    if ( m_ppLocalQueue[nIndex] == NULL )
    {
        m_ppLocalQueue[nIndex] = new LocalQueue(m_nLocalQueueSize);
    }

    m_ppLocalQueue[nIndex]->EnQueue(Data);
    return;
}

/**	分布式队列的本地队列出队函数

	@param	T &Data - 接收出队的数据	
	@return	int - 出队成功返回CAPI_SUCCESS, 失败(队列为空）返回CAPI_FAILED.	
*/
template <class T, class LocalQueue, class SharedQueue> 
int CDistributedQueue<T, LocalQueue, SharedQueue>::PopFromLocalQueue(T &Data)
{
    int nId = ThreadIdGet();
    return m_ppLocalQueue[nId]->DeQueue(Data);
}

/**	分布式队列的共享队列进队函数

	@param	T &Data - 要进队的数据	
	@return	int - 成功返回CAPI_SUCCESS, 失败返回CAPI_FAILED.	
*/
template <class T, class LocalQueue, class SharedQueue> 
int CDistributedQueue<T, LocalQueue, SharedQueue>::PushToSharedQueue(T &Data)
{
   return m_pSharedQueue->Push(Data);
}

/**	分布式队列的共享队列出队函数

	@param	T &Data - 接收出队的数据	
	@return	int - 成功返回CAPI_SUCCESS, 失败返回CAPI_FAILED.	
*/
template <class T, class LocalQueue, class SharedQueue> 
int CDistributedQueue<T, LocalQueue, SharedQueue>::PopFromSharedQueue(T &Data)
{
    return m_pSharedQueue->Pop(Data);
}

/**	分布式队列的出队函数
    出队操作策略为，先从本地队列中出队，如果失败则从共享队列中出队

	@param	T &Data - 接收出队的数据	
	@return	int - 成功返回CAPI_SUCCESS, 失败返回CAPI_FAILED.	
*/
template <class T, class LocalQueue, class SharedQueue> 
int CDistributedQueue<T, LocalQueue, SharedQueue>::DeQueue(T &Data)
{
    int nRet;

    int nId = ThreadIdGet();
    
    nRet = m_ppLocalQueue[nId]->DeQueue(Data);
    if ( nRet == CAPI_FAILED )
    {
        nRet = m_pSharedQueue->Pop(Data);
#if _DEBUG 
        if (nRet == CAPI_SUCCESS )
        {
//            printf("Thread Id:%ld, Get Data From SharedQueue\n", GetCurrentThreadId());
        }
#endif
    }
#if _DEBUG 
    else
    {
//        printf("Thread Id:%ld, Get Data From LocalQueue\n", GetCurrentThreadId());
    }
#endif
    return nRet;   
}

/**	分布式队列的共享队列批量私有化函数

	@param	int nSharedQueueIndex - 共享子队列序号	
	@return	int - 成功返回CAPI_SUCCESS, 失败返回CAPI_FAILED.	
*/
template <class T, class LocalQueue, class SharedQueue> 
int CDistributedQueue<T, LocalQueue, SharedQueue>::PrivatizeSharedQueue(int nSharedQueueIndex)
{
    int nRet = CAPI_FAILED;
    T   Data;

    int nId = ThreadIdGet();

    SharedQueue::SubQueue *pQueue;

    pQueue = m_pSharedQueue->GetSubQueue(nSharedQueueIndex);
    if ( pQueue->IsEmpty() )
    {
        return nRet;
    }

    pQueue->Lock();

    int i;

    int nCount = pQueue->GetCountNoLock();
    if ( nCount >= 2 )
    {
        nCount /= 2;
    }

    for ( i = 0; i < nCount; i++ )
    {
        nRet = pQueue->DeQueueNoLock( Data ); 
        m_ppLocalQueue[nId]->EnQueue(Data);
    }

    pQueue->Unlock();

    return nRet;
}


#endif //__CDISTRIBUTEDQUEUE_H__

