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
    THREADFUNC      m_StartFunc;  //Ϊ�̳߳�ʹ�õ��߳���ں���ָ��
    LONG  volatile  m_lTaskId;    //Task Id,�����ж��Ƿ��Ѷ�Ӧ���߳�

public:
    CNestTaskScheduler();
    virtual ~CNestTaskScheduler(){};


    //������������Ϊ����������ֱ��ʹ��
    void SetStartFunc(THREADFUNC StartFunc);
    int GetTask(TASK &Task);
    CThreadPool & GetThreadPool();
    LONG AtomicIncrementTaskId();
    
    //������������Ϊ��������ʹ����ʹ��
    void SpawnLocalTask(TASK &Task);
    void SpawnTask(TASK &Task);
    void BeginRootThread(TASK &Task);
};

#endif //__CNESTTASKSCHEDULER_H__
