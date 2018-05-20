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
#include "CThreadPool.h"
#include "CLocalQueue.h"
#include "CStealQueue.h"
#include "CDistributedQueue.h"
#include "CNestTaskScheduler.h"

static unsigned int WINAPI NestTaskScheduler_StartFunc(void *pArgs);

#define     NESTTASK_QUEUE_SIZE     128

/**	嵌套任务调度的构造函数

	@return	constructor - 无	
*/
CNestTaskScheduler::CNestTaskScheduler()
{
    m_StartFunc = NestTaskScheduler_StartFunc;

    int n = m_ThreadPool.GetThreadCount();
    m_DQueue.Create(NESTTASK_QUEUE_SIZE, n, NESTTASK_QUEUE_SIZE, 0, 
        ThreadPool_GetThreadId, &m_ThreadPool);
}

/**	嵌套任务调度的设置线程池的入口函数

	@param	THREADFUNC StartFunc - 线程池的入口函数	
	@return	void - 无	
*/
void CNestTaskScheduler::SetStartFunc(THREADFUNC StartFunc)
{
    m_StartFunc = StartFunc;
}

/**	嵌套任务调度的获取任务函数

	@param	TASK &Task - 接收从分布式队列中获取的任务	
	@return	int - 成功返回CAPI_SUCCESS, 失败返回CAPI_FAILED.	
*/
int CNestTaskScheduler::GetTask(TASK &Task) 
{
    //先从本地队列获取任务
    //本地获取任务失败后从共享队列获取任务
    return m_DQueue.DeQueue(Task);
};


/**	嵌套任务调度的获取线程池函数

    @return	CThreadPool & - 返回线程池对象	
*/
CThreadPool & CNestTaskScheduler::GetThreadPool()
{
    return m_ThreadPool;
}

/**	嵌套任务调度的原子增加任务Id函数

    @return	int - 返回原子加1后的任务Id.	
*/
LONG CNestTaskScheduler::AtomicIncrementTaskId()
{
    LONG Id = AtomicIncrement(&m_lTaskId);
    return Id;
}



/**	嵌套任务调度的生成当前线程的本地任务
    任务被放入当前线程的本地队列中

	@param	TASK &Task - 待执行的任务	
	@return	void - 无	
*/
void CNestTaskScheduler::SpawnLocalTask(TASK &Task) 
{
    //将任务放入本地队列中
    m_DQueue.PushToLocalQueue(Task);
};


/**	嵌套任务调度的生成任务函数
    生成的任务被放入分布式队列中

	@param	TASK &Task - 待执行的任务	
	@return	void - 无	
*/
void CNestTaskScheduler::SpawnTask(TASK &Task) 
{
    if ( m_lTaskId < m_ThreadPool.GetThreadCount() )
    {
        //依次唤醒各个挂起的线程
        LONG Id = AtomicIncrement(&m_lTaskId);
        if ( Id < m_ThreadPool.GetThreadCount() )
        {
            //下面之所以可以对其他线程的本地队列进行无同步的操作，是因为
            // 访问这些队列的线程在进队操作之后才开始运行
            m_DQueue.PushToLocalQueue(Task, Id);
            m_ThreadPool.ExecThread(Id);
        }
        else
        {
            m_DQueue.EnQueue(Task);
        }
    }
    else
    {
        //先判断共享队列是否满，如果未满则放入共享队列中
        //如果满了则放入本地队列中
        m_DQueue.EnQueue(Task);
    }
};


/**	嵌套任务调度的启动根线程函数

	@param	TASK &Task - 要执行的最初任务	
	@return	void - 无	
*/
void CNestTaskScheduler::BeginRootThread(TASK &Task)
{
    m_lTaskId = 0;

    m_ThreadPool.CreateThreadPool(m_StartFunc, this, 0);

    m_DQueue.PushToLocalQueue(Task, 0);

    m_ThreadPool.ExecThread( 0 );  

    m_ThreadPool.WaitAllThread();
}

/**	嵌套任务调度的线程池入口函数

	@param	void *pArgs - CNestTaskScheduler类型的参数	
	@return	unsigned int WINAPI - 返回0	
*/
unsigned int WINAPI NestTaskScheduler_StartFunc(void *pArgs)
{
    CNestTaskScheduler  *pSched = (CNestTaskScheduler *)pArgs;

    TASK    Task;
    int     nRet;

    for ( ;; )
    {
        nRet = pSched->GetTask(Task);

        if ( nRet == CAPI_FAILED )
        {
            CThreadPool &ThreadPool = pSched->GetThreadPool();
            
            // 唤醒一个挂起的线程,防止任务数量小于CPU核数时，
            // 仍然有任务处于挂起状态,从而导致WaitAllThread()处于死等状态
            // 这个唤醒过程是一个串行的过程，被唤醒的任务会继续唤醒一个挂起线程
            LONG Id = pSched->AtomicIncrementTaskId();
            if ( Id < ThreadPool.GetThreadCount() )
            {
                ThreadPool.ExecThread(Id);
            }
            break;
        }

        (*(Task.func))(Task.pArg);
    }

    return 0;
}



