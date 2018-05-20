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
 *	CTaskTeam.h
 *
 *	DESCRIPTION
 *		Module for Task Team class 
 *
 *	HISTORY
 *		06-08-2008	create by zhouweiming.
 *
 */
#ifndef __CTASKTEAM_H__
#define __CTASKTEAM_H__

#include "SingleList.h"
#include "Graph.h"

#define     DEFAULT_TASK_COUNT_IN_TASKTEAM      32

class CTaskTeam {
private:
    int     m_nMaxCount;
    int     m_nTaskCount;
    int     m_nThreadCount;

    TASK        **m_ppTaskArray;  //待分配给各个线程的任务数组
    SINGLELIST  **m_ppSingleList; //分配给各个线程的任务列表
    int         *m_pnDeltaArray;  //各个线程的误差数组

    int         FindMaxDelta();   //查找最大负数误差值

public:
    CTaskTeam();
    CTaskTeam(int nInitTaskCount);
    virtual ~CTaskTeam();

    void   AddTask(TASK *pTaskNode);
    void   AddTask(THREADFUNC TaskFunc, void *pPara, DWORD dwTaskTime);

    TASK * GetTask(int nPos); //获取m_ppTaskArray数组中的第nPos个元素
    TASK **GetTaskArray();    //获取m_ppTaskArray数组指针


    int GetTaskCount();

    void Schedule();   //将任务组内的所有任务平均分配给各个线程

    void Exec();       //并行执行分配好了的给各个线程的任务
};


void Edge_Destroy(void *p);


#endif //__CTASKTEAM_H__
