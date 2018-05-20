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


/**	ȱʡ���캯��

    @return	 - ��	
*/
template <class T, class SharedQueue>
CQueuePool<T, SharedQueue>::CQueuePool()
{
    m_pQueue = NULL;
    m_nQueueSize = 0;
    m_nQueueCount = 0;
    m_Random.Init(clock());
}

/**	���캯��

    @param	int nQueueCount - �Ӷ��е��������ж��ٸ��Ӷ��У�	
    @param	int nQueueSize - �Ӷ��еĴ�С�����Դ��Ԫ�صĸ�����	
    @return	- ��	
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

/**	��������

    @return	- ��	
*/
template <class T, class SharedQueue>
CQueuePool<T, SharedQueue>::~CQueuePool()
{
    if ( m_pQueue != NULL )
    {
        delete [] m_pQueue;
    }
}


/**	�������г�Ա����
    �������е��Ӷ��д�����

    @param	int nQueueCount - �Ӷ��е�����	
    @param	int nQueueSize - �Ӷ��д�С	
    @return	void  - ��	
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


/**	���гص�������Ӻ���
    ������������뵽ĳ���Ӷ�����

	@param	T &Data - Ҫ���ӵ�����	
	@return	int - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::EnQueue(T &Data)
{
    int nIndex = m_Random.Get();
    nIndex = nIndex % m_nQueueCount;

    return m_pQueue[nIndex].EnQueue(Data);
}

/**	���ӳ�Ա����
    �����ݽ��뵽ָ����ŵ��Ӷ�����

	@param	T &Data - Ҫ���ӵ�����	
	@param	int nIndex - �Ӷ������	
	@return	int - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
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


/**	������Ӻ���
    �����ĳ���Ӷ�����ȡ����

	@param	T &Data - ���ճ��ӵ�����	
	@return	int - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
*/
template <class T, class SharedQueue>
int CQueuePool<T, SharedQueue>::DeQueue(T &Data)
{
    int nIndex = m_Random.Get();
    nIndex = nIndex % m_nQueueCount;

    return m_pQueue[nIndex].DeQueue(Data);
}

/**	���ӳ�Ա����
    ��ָ����ŵ��Ӷ����г���

	@param	T &Data - ���ճ��ӵ�����	
	@param	int nIndex - �Ӷ������	
	@return	int - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
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


/**	���гصĽ��ӳ�Ա����
    �����ݷ��뵽���г��У����ý������Ӷ�����ѭ�ķ����������ݣ�
    ���ݽ����ŵ����λ�ÿ�ʼ�ĵ�1��δ�����Ӷ�����

	@param	T &Data - Ҫ���ӵ�����	
	@return	int - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
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


/**	���гصĳ��Ӻ���
    �Ӷ��г���ȡ���ݣ����ý������Ӷ�����ѭ�ķ�����ȡ���ݣ�
    ���ݽ������λ�ÿ�ʼ�ĵ�1���ǿյ��Ӷ����л�ȡ

	@param	T &Data - ���ճ��ӵ�����	
	@return	int - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
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
