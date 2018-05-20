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
 *	CThreadPool.h
 *
 *	DESCRIPTION
 *		Module for Thread Pool class 
 *
 *	HISTORY
 *		06-05-2008	create by zhouweiming.
 *
 */
#ifndef __CTHREADPOOL_H__
#define __CTHREADPOOL_H__


struct THREADPOOL_STARTFUNC_PARMETERS {
    DWORD       dwTlsIndex; // ���ش洢����
    int         nId;        // �̱߳��
    THREADFUNC  StartFunc;  // �߳���ں���
    void        *pArgs;     // �߳���ں���
};

#define   MAX_SINGLE_THREAD_COUNT   16

class CThreadPool 
{
private:
    DWORD        m_dwTlsIndex;
    THREADFUNC   m_StartFunc;
    void  *      m_pArgs;  //m_StartFunc�Ĳ���

    int          m_nThreadCount;  //�߳����ڵ��߳�����
    HANDLE   *   m_pThreadArray;  //����߳���
    THREADPOOL_STARTFUNC_PARMETERS * m_pPara; //����߳���

    int          m_nSingleThreadCount; //���������ĵ����̵߳�����
    HANDLE       m_SingleThread[MAX_SINGLE_THREAD_COUNT]; //�����������������
                                                         //�����̵߳�����
    //������������ĵ����̵߳���ں�������
    THREADPOOL_STARTFUNC_PARMETERS m_SinglePara[MAX_SINGLE_THREAD_COUNT]; 
public:
    CThreadPool();
    CThreadPool(THREADFUNC StartFunc, void *pArgs, int nThreadCount);
    virtual ~CThreadPool();

    void CreateThreadPool(THREADFUNC StartFunc, void *pArgs, int nThreadCount=0);
    void CreateThread(THREADFUNC StartFunc, void *pArgs);

    int GetThreadId();
    int GetThreadCount();

    void ExecAndWait();

    void ExecThread(int nIndex);

    void WaitAllThread();
}; 

int ThreadPool_GetThreadId(void *pArgs);

#endif //__CTHREADPOOL_H__
