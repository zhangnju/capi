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
    DWORD       dwTlsIndex; // 本地存储索引
    int         nId;        // 线程编号
    THREADFUNC  StartFunc;  // 线程入口函数
    void        *pArgs;     // 线程入口函数
};

#define   MAX_SINGLE_THREAD_COUNT   16

class CThreadPool 
{
private:
    DWORD        m_dwTlsIndex;
    THREADFUNC   m_StartFunc;
    void  *      m_pArgs;  //m_StartFunc的参数

    int          m_nThreadCount;  //线程组内的线程数量
    HANDLE   *   m_pThreadArray;  //针对线程组
    THREADPOOL_STARTFUNC_PARMETERS * m_pPara; //针对线程组

    int          m_nSingleThreadCount; //独立创建的单个线程的数量
    HANDLE       m_SingleThread[MAX_SINGLE_THREAD_COUNT]; //用来保存独立创建的
                                                         //单个线程的数组
    //保存独立创建的单个线程的入口函数参数
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
