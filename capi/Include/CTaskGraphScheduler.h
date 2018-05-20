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
 *	CTaskGraphScheduler.h
 *
 *	DESCRIPTION
 *		Module for Task Graph Schedule class 
 *
 *	HISTORY
 *		06-08-2008	create by zhouweiming.
 *
 */
#ifndef __CTASKGRAPHSCHEDULER_H__
#define __CTASKGRAPHSCHEDULER_H__

#include "CTaskTeam.h"
#include "DoubleList.h"

#define DEFAULT_ONE_LAYER_TASK_COUNT    32


class  CTaskGraphScheduler {
private:
    DOUBLELIST  *m_pGraphNodeList;
    SINGLELIST  *m_pLayeredList;

    void DestroyLayeredList();
    void DestroyGraphNodeList();

    void DecInDegree(GRAPHNODE *pNode);

    void Layered();  //将任务进行分层

public:

    CTaskGraphScheduler();
    virtual ~CTaskGraphScheduler();

    void SetTaskDestroyFunc(DESTROYFUNC TaskDestroyFunc);

    //添加任务到任务图中
    GRAPHNODE * AddTask(THREADFUNC TaskFunc, void *pPara, DWORD dwTaskTime);
    void AddRelation(GRAPHNODE *pSrcNode, GRAPHNODE *pTagNode); //添加依赖关系
    void ExecAllTask();  //调度任务图中的所有任务，使其并行执行
};


#endif //__CTASKGRAPHSCHEDULER_H__
