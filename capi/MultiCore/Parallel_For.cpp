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
#include "CapiGlobal.h"
#include "Parallel_For.h"



CRange::CRange(CNestTaskScheduler *p)
{
    m_pTaskScheduler = p;
}


/**	�������������ָ��

	@param	CNestTaskScheduler *p - ���������ָ��	
	@return	void - ��	
*/
void CRange::SetTaskScheduler(CNestTaskScheduler *p)
{
    m_pTaskScheduler = p;
};


/**	��ȡ���������ָ��

	@return	CNestTaskScheduler * - �������������ָ��	
*/
CNestTaskScheduler *CRange::GetTaskScheduler()
{
    return m_pTaskScheduler;
};


/**	CRange����������ں���

	@param	void *pArg - ʵ��Ϊһ��CRangeָ��	
	@return	unsigned int WINAPI - ����CAPI_FAILED��ʾʧ�ܣ�����CAPI_SUCCESS��ʾ�ɹ�	
*/
unsigned int WINAPI RangeProcessTask(void *pArg)
{
    CRange *pRange = (CRange *)pArg;

    if ( pRange == NULL )
    {
        return CAPI_FAILED;
    }

    CRange *pNewRange = pRange->Split();
    if ( pNewRange == NULL )
    {
        delete pRange;
        return CAPI_SUCCESS;
    }

    CNestTaskScheduler *pTaskSched = pRange->GetTaskScheduler();

    pNewRange->SetTaskScheduler(pTaskSched);

    TASK t1, t2;
    t1.pArg = (void *)pRange;
    t1.func = RangeProcessTask;
    t2.pArg = (void *)pNewRange;
    t2.func = RangeProcessTask;

    pTaskSched->SpawnLocalTask(t1);
    pTaskSched->SpawnTask(t2);

    return CAPI_SUCCESS;
}


/**	����forѭ���Ĵ�����
    ��һ��CRange���в��д���

	@param	CRange *pRange - CRangeָ��	
	@return	void - ��	
*/
void Parallel_For(CRange *pRange )
{
    CNestTaskScheduler  *p = new CNestTaskScheduler;
    TASK    task;

    task.func = RangeProcessTask;
    task.pArg = pRange;

    pRange->SetTaskScheduler(p);

    p->BeginRootThread(task);

    delete p;
}

