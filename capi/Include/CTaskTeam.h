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

    TASK        **m_ppTaskArray;  //������������̵߳���������
    SINGLELIST  **m_ppSingleList; //����������̵߳������б�
    int         *m_pnDeltaArray;  //�����̵߳��������

    int         FindMaxDelta();   //������������ֵ

public:
    CTaskTeam();
    CTaskTeam(int nInitTaskCount);
    virtual ~CTaskTeam();

    void   AddTask(TASK *pTaskNode);
    void   AddTask(THREADFUNC TaskFunc, void *pPara, DWORD dwTaskTime);

    TASK * GetTask(int nPos); //��ȡm_ppTaskArray�����еĵ�nPos��Ԫ��
    TASK **GetTaskArray();    //��ȡm_ppTaskArray����ָ��


    int GetTaskCount();

    void Schedule();   //���������ڵ���������ƽ������������߳�

    void Exec();       //����ִ�з�����˵ĸ������̵߳�����
};


void Edge_Destroy(void *p);


#endif //__CTASKTEAM_H__
