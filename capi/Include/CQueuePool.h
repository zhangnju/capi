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
 *	CQueuePool.h
 *
 *	DESCRIPTION
 *		Module for QueuePool class 
 *
 * 	HISTORY
 *		06-08-2008	create by zhouweiming.
 *       09-08-2008  add Push() and Pop() functions.
 *
 */
#ifndef __CQUEUEPOOL_H__
#define __CQUEUEPOOL_H__

#include "time.h"
#include "CFastRandom.h"

template <class T, class SharedQueue>
class CQueuePool 
{
PRIVATE:
    SharedQueue  *m_pQueue;
    int         m_nQueueCount;
    int         m_nQueueSize;
    CFastRandom m_Random;
public:
    typedef     SharedQueue     SubQueue;
public:
    CQueuePool();
    CQueuePool(int nQueueCount, int nQueueSize);
    virtual ~CQueuePool();

    void Create(int nQueueCount, int nQueueSize);

    int EnQueue(T &Data);
    int EnQueue(T &Data, int nIndex);

    int DeQueue(T &Data);
    int DeQueue(T &Data, int nIndex);

    int Push(T &Data);
    int Pop( T &Data);

    int GetQueueCount() { return m_nQueueCount;}

    SubQueue *GetSubQueue(int nIndex) { return &(m_pQueue[nIndex]);};
};


/**	缺省构造函数

    @return	 - 无	
*/
template <class T, class SharedQueue>
CQueuePool<T, SharedQueue>::CQueuePool()
{
    m_pQueue = NULL;
    m_nQueueSize = 0;
    m_nQueueCount = 0;
    m_Random.Init(clock());
}

/**	构造函数

    @param	int nQueueCount - 子队列的数量（有多少个子队列）	
    @param	int nQueueSize - 子队列的大小（可以存放元素的个数）	
    @return	- 无	
*/
template <class T, class SharedQueue>
CQueuePool<T, SharedQueue>::CQueuePool(int nQueueCount, int nQueueSize)
{
    m_nQueueCount = nQueueCount;
    m_nQueueSize = nQueueSize;

    if ( nQueueCount == 0 )
    {
        m_nQueueCount = omp_get_num_procs();
    }

    m_pQueue =  new SharedQueue[nQueueCount];

    int i;
    for ( i = 0; i < nQueueCount; i++ )
    {
        m_pQueue[i].Create(nQueueSize);
    }   
    m_Random.Init(clock());
}

/**	析构函数

    @return	- 无	
*/
template <class T, class SharedQueue>
CQueuePool<T, SharedQueue>::~CQueuePool()
{
    if ( m_pQueue != NULL )
    {
        delete [] m_pQueue;
    }
}


/**	创建队列成员函数
    负责将所有的子队列创建好

    @param	int nQueueCount - 子队列的数量	
    @param	int nQueueSize - 子队列大小	
    @return	void  - 无	
*/
template <class T, class SharedQueue>
void CQueuePool<T, SharedQueue>::Create(int nQueueCount, int nQueueSize)
{
    m_nQueueCount = nQueueCount;
    m_nQueueSize = nQueueSize;

    if ( m_pQueue != NULL )
    {
        delete [] m_pQueue;
    }
    m_pQueue =  new SharedQueue[nQueueCount];
    
    int i;
    for ( i = 0; i < nQueueCount; i++ )
    {
        m_pQueue[i].Create(nQueueSize);
    }
    m_Random.Init(clock());
}


/**	队列池的随机进队函数
    将数据随机进入到某个子队列中

	@param	T &Data - 要进队的数据	
	@return	int - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::EnQueue(T &Data)
{
    int nIndex = m_Random.Get();
    nIndex = nIndex % m_nQueueCount;

    return m_pQueue[nIndex].EnQueue(Data);
}

/**	进队成员函数
    将数据进入到指定序号的子队列中

	@param	T &Data - 要进队的数据	
	@param	int nIndex - 子队列序号	
	@return	int - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::EnQueue(T &Data, int nIndex)
{
    if ( nIndex >= m_nQueueCount )
    {
        return CAPI_FAILED;
    }

    return m_pQueue[nIndex].EnQueue(Data);
}


/**	随机出队函数
    随机从某个子队列中取数据

	@param	T &Data - 接收出队的数据	
	@return	int - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::DeQueue(T &Data)
{
    int nIndex = m_Random.Get();
    nIndex = nIndex % m_nQueueCount;

    return m_pQueue[nIndex].DeQueue(Data);
}

/**	出队成员函数
    从指定序号的子队列中出队

	@param	T &Data - 接收出队的数据	
	@param	int nIndex - 子队列序号	
	@return	int - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::DeQueue(T &Data, int nIndex)
{
    if ( nIndex >= m_nQueueCount )
    {
        return CAPI_FAILED;
    }

    return m_pQueue[nIndex].DeQueue(Data);  
}


/**	队列池的进队成员函数
    将数据放入到队列池中，采用将所有子队列轮循的方法来放数据，
    数据将被放到随机位置开始的第1个未满的子队列中

	@param	T &Data - 要进队的数据	
	@return	int - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::Push(T &Data)
{
    int i;
    int nRet = CAPI_FAILED;
    int nIndex = m_Random.Get();
    nIndex = nIndex % m_nQueueCount;

    if ( EnQueue(Data, nIndex) == CAPI_FAILED )
    {
        for ( i = nIndex + 1; i < m_nQueueCount; i++ )
        {
            if ( EnQueue(Data, i) == CAPI_SUCCESS )
            {
                nRet = CAPI_SUCCESS;
                break;
            }
        }
        if ( nRet == CAPI_FAILED )
        {
            for ( i = 0; i < nIndex; i++ )
            {
                if ( EnQueue(Data, i) == CAPI_SUCCESS )
                {
                    nRet = CAPI_SUCCESS;
                    break;
                }
            }
        }
    }
    else
    {
        nRet = CAPI_SUCCESS;
    }
    return nRet;
}


/**	队列池的出队函数
    从队列池中取数据，采用将所有子队列轮循的方法来取数据，
    数据将从随机位置开始的第1个非空的子队列中获取

	@param	T &Data - 接收出队的数据	
	@return	int - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::Pop(T &Data)
{
    int i;
    int nRet = CAPI_FAILED;
    int nIndex = m_Random.Get();
    nIndex = nIndex % m_nQueueCount;

    if ( DeQueue(Data, nIndex) == CAPI_FAILED )
    {
        for ( i = nIndex + 1; i < m_nQueueCount; i++ )
        {
            if ( DeQueue(Data, i) == CAPI_SUCCESS )
            {
                nRet = CAPI_SUCCESS;
                break;
            }
        }
        if ( nRet == CAPI_FAILED )
        {
            for ( i = 0; i < nIndex; i++ )
            {
                if ( DeQueue(Data, i) == CAPI_SUCCESS )
                {
                    nRet = CAPI_SUCCESS;
                    break;
                }
            }
        }
    }
    else
    {
        nRet = CAPI_SUCCESS;
    }
    return nRet;
}

#endif //__CQUEUEPOOL_H__
