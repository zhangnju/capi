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

    void Layered();  //��������зֲ�

public:

    CTaskGraphScheduler();
    virtual ~CTaskGraphScheduler();

    void SetTaskDestroyFunc(DESTROYFUNC TaskDestroyFunc);

    //�����������ͼ��
    GRAPHNODE * AddTask(THREADFUNC TaskFunc, void *pPara, DWORD dwTaskTime);
    void AddRelation(GRAPHNODE *pSrcNode, GRAPHNODE *pTagNode); //���������ϵ
    void ExecAllTask();  //��������ͼ�е���������ʹ�䲢��ִ��
};


#endif //__CTASKGRAPHSCHEDULER_H__
