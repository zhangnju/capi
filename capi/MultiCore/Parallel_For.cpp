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


/**	设置任务调度类指针

	@param	CNestTaskScheduler *p - 任务调度类指针	
	@return	void - 无	
*/
void CRange::SetTaskScheduler(CNestTaskScheduler *p)
{
    m_pTaskScheduler = p;
};


/**	获取任务调度类指针

	@return	CNestTaskScheduler * - 返回任务调度类指针	
*/
CNestTaskScheduler *CRange::GetTaskScheduler()
{
    return m_pTaskScheduler;
};


/**	CRange的任务处理入口函数

	@param	void *pArg - 实际为一个CRange指针	
	@return	unsigned int WINAPI - 返回CAPI_FAILED表示失败，返回CAPI_SUCCESS表示成功	
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


/**	并行for循环的处理函数
    将一个CRange进行并行处理

	@param	CRange *pRange - CRange指针	
	@return	void - 无	
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

