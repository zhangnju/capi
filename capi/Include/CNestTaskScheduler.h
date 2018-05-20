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
 *	CNestTaskScheduler.h
 *
 * 	DESCRIPTION
 *		Module for Task Scheduler class 
 * 
 *	HISTORY
 *		06-08-2008	create by zhouweiming.
 *
 */
#ifndef __CNESTTASKSCHEDULER_H__
#define __CNESTTASKSCHEDULER_H__

#include "CLocalQueue.h"
#include "CStealQueue.h"
#include "CDistributedQueue.h"

class CNestTaskScheduler {
private:
    CThreadPool     m_ThreadPool;//(TaskScheduler_StartFunc, NULL, 0);
    CDistributedQueue<TASK, CLocalQueue<TASK>, CStealQueue<TASK>> m_DQueue;
    THREADFUNC      m_StartFunc;  //为线程池使用的线程入口函数指针
    LONG  volatile  m_lTaskId;    //Task Id,用于判断是否唤醒对应的线程

public:
    CNestTaskScheduler();
    virtual ~CNestTaskScheduler(){};


    //下面两个函数为调度器本身直接使用
    void SetStartFunc(THREADFUNC StartFunc);
    int GetTask(TASK &Task);
    CThreadPool & GetThreadPool();
    LONG AtomicIncrementTaskId();
    
    //下面三个函数为调度器的使用者使用
    void SpawnLocalTask(TASK &Task);
    void SpawnTask(TASK &Task);
    void BeginRootThread(TASK &Task);
};

#endif //__CNESTTASKSCHEDULER_H__
