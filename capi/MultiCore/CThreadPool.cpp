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
#include <windows.h>
#include <process.h>
#include <omp.h>
#include "CapiGlobal.h"
#include "CThreadPool.h"


/**	�̳߳ص��߳���ں���
    �û�������߳���ں�������������б�����
    ��������Ĺ�����Ҫ�Ǹ��߳̽��б��

	@param	void *pArgs - һ��THREADPOOL_STARTFUNC_PARMETERS���͵Ĳ���	
	@return	unsigned __stdcall - ���û�����ķ���ֵ	
*/
unsigned __stdcall ThreadPool_StartFunc(void *pArgs)
{
    THREADPOOL_STARTFUNC_PARMETERS *p = (THREADPOOL_STARTFUNC_PARMETERS *)pArgs;
    TlsSetValue(p->dwTlsIndex, (void *)(p->nId));

    return (*(p->StartFunc))(p->pArgs);
}


/**	�̳߳صĻ�ȡ�߳�Id�ص�������������ģ��ʹ��
    �����������ʱ������ʹ��CThreadPool����Ϊ����

	@param	void *pArgs - CThreadPool���͵Ĳ���	
	@return	int - ���ش�0��ʼ�ı��	
*/
int ThreadPool_GetThreadId(void *pArgs)
{
    CThreadPool *pThreadPool = (CThreadPool *)pArgs;
    return pThreadPool->GetThreadId();
}


/**	CThreadPool��Ĺ��캯��

	@return	constructor - ��	
*/
CThreadPool::CThreadPool()
{
    m_nThreadCount = omp_get_num_procs();
    m_dwTlsIndex = TlsAlloc();
    m_pThreadArray = new HANDLE[m_nThreadCount];
    m_pPara = new THREADPOOL_STARTFUNC_PARMETERS[m_nThreadCount];
    m_nSingleThreadCount = 0;
}


/**	CThreadPool�����������

	@return	destructor - ��	
*/
CThreadPool::~CThreadPool()
{
    TlsFree(m_dwTlsIndex);
    delete [] m_pThreadArray;
    delete [] m_pPara;
}


/**	CThreadPool��Ĺ��캯��

	@param	THREADFUNC StartFunc - �û����߳���������	
	@param	void *pArgs - �û����߳����������Ĳ���	
	@param	int nThreadCount - ��Ҫ�������߳�����	
	@return	constructor - ��	
*/
CThreadPool::CThreadPool(THREADFUNC StartFunc, void *pArgs, int nThreadCount)
{
    m_StartFunc = StartFunc;
    m_pArgs = pArgs;
    m_dwTlsIndex = TlsAlloc();
    if ( nThreadCount == 0 )
    {
        m_nThreadCount = omp_get_num_procs();
    }
    else
    {
        m_nThreadCount = nThreadCount;
    }

    m_pThreadArray = new HANDLE[m_nThreadCount];
    m_pPara = new THREADPOOL_STARTFUNC_PARMETERS[m_nThreadCount];
    m_nSingleThreadCount = 0;

}

/**	��ȡ�̳߳�����߳�Id

	@return	int - ���ش�0��ʼ��ŵ��߳�Id	
*/
int CThreadPool::GetThreadId()
{
    int nId = (int)TlsGetValue(m_dwTlsIndex);

    return (nId-1);
}


/**	��ȡ�̳߳��ڵ��߳�����

	@return	int - �����߳�����	
*/
int CThreadPool::GetThreadCount()
{
    return m_nThreadCount;
}


/**	�����̳߳ز�������

	@param	THREADFUNC StartFunc - �û����߳�������ں���	
	@param	void *pArgs - �û����߳�������ں����Ĳ���	
	@param	int nThreadCount - ��Ҫ�������߳�����	
	@return	void - ��	
*/
void CThreadPool::CreateThreadPool(THREADFUNC StartFunc, void *pArgs, int nThreadCount)
{
    int i;

    m_StartFunc = StartFunc;
    m_pArgs = pArgs;
    if ( nThreadCount == 0 )
    {
        m_nThreadCount = omp_get_num_procs();
    }
    else
    {
        m_nThreadCount = nThreadCount;
    }

    for ( i = 0; i < m_nThreadCount; i++ )
    {
        m_pPara[i].dwTlsIndex = m_dwTlsIndex;
        m_pPara[i].nId = i + 1;
        m_pPara[i].pArgs = m_pArgs;
        m_pPara[i].StartFunc = m_StartFunc;

        m_pThreadArray[i] = (HANDLE)_beginthreadex( NULL, 0, 
            &ThreadPool_StartFunc, &(m_pPara[i]), CREATE_SUSPENDED , NULL );
    }
}

/**	���������̺߳���
    �����ڵ�����CreateThreadPool������ſɵ����������

	@param	THREADFUNC StartFunc - �û����߳�������ں���	
	@param	void *pArgs - �û����߳�������ں����Ĳ���	
	@return	void - ��	
*/
void CThreadPool::CreateThread(THREADFUNC StartFunc, void *pArgs)
{
    LONG nIndex = AtomicIncrement((LONG volatile *)&m_nSingleThreadCount);
    nIndex -= 1;

    m_SinglePara[nIndex].dwTlsIndex = m_dwTlsIndex;
    m_SinglePara[nIndex].nId = m_nThreadCount + nIndex + 1;
    m_SinglePara[nIndex].pArgs = m_pArgs;
    m_SinglePara[nIndex].StartFunc = m_StartFunc;

    m_pThreadArray[nIndex] = (HANDLE)_beginthreadex( NULL, 0, 
        &ThreadPool_StartFunc, &(m_SinglePara[nIndex]), 
        CREATE_SUSPENDED, NULL );
}



/**	ִ���̳߳��������̲߳��ȴ������߳�ִ����

	@return	void - ��	
*/
void CThreadPool::ExecAndWait()
{
    int i;
    for ( i = 0; i < m_nThreadCount; i++ )
    {
        ResumeThread(m_pThreadArray[i]);
    }
    for ( i = 0; i < m_nSingleThreadCount; i++ )
    {
        ResumeThread(m_SingleThread[i]);
    }
    WaitAllThread();    
}


/**	ִ���̳߳��ڵĵ����̵߳ĺ���

	@param	int nIndex - Ҫִ�е��̵߳ı��	
	@return	void - ��	
*/
void CThreadPool::ExecThread(int nIndex)
{
    if ( nIndex < m_nThreadCount )
    {
        ResumeThread(m_pThreadArray[nIndex]);
    }
    return;
}

/**	�ȴ��̳߳��������߳�ִ����ĺ���

	@return	void - ��	
*/
void CThreadPool::WaitAllThread()
{
    WaitForMultipleObjects(m_nThreadCount,m_pThreadArray, TRUE, INFINITE);
    WaitForMultipleObjects(m_nSingleThreadCount, m_SingleThread, TRUE, INFINITE);
}
