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

/**	Ƕ��������ȵĹ��캯��

	@return	constructor - ��	
*/
CNestTaskScheduler::CNestTaskScheduler()
{
    m_StartFunc = NestTaskScheduler_StartFunc;

    int n = m_ThreadPool.GetThreadCount();
    m_DQueue.Create(NESTTASK_QUEUE_SIZE, n, NESTTASK_QUEUE_SIZE, 0, 
        ThreadPool_GetThreadId, &m_ThreadPool);
}

/**	Ƕ��������ȵ������̳߳ص���ں���

	@param	THREADFUNC StartFunc - �̳߳ص���ں���	
	@return	void - ��	
*/
void CNestTaskScheduler::SetStartFunc(THREADFUNC StartFunc)
{
    m_StartFunc = StartFunc;
}

/**	Ƕ��������ȵĻ�ȡ������

	@param	TASK &Task - ���մӷֲ�ʽ�����л�ȡ������	
	@return	int - �ɹ�����CAPI_SUCCESS, ʧ�ܷ���CAPI_FAILED.	
*/
int CNestTaskScheduler::GetTask(TASK &Task) 
{
    //�ȴӱ��ض��л�ȡ����
    //���ػ�ȡ����ʧ�ܺ�ӹ�����л�ȡ����
    return m_DQueue.DeQueue(Task);
};


/**	Ƕ��������ȵĻ�ȡ�̳߳غ���

    @return	CThreadPool & - �����̳߳ض���	
*/
CThreadPool & CNestTaskScheduler::GetThreadPool()
{
    return m_ThreadPool;
}

/**	Ƕ��������ȵ�ԭ����������Id����

    @return	int - ����ԭ�Ӽ�1�������Id.	
*/
LONG CNestTaskScheduler::AtomicIncrementTaskId()
{
    LONG Id = AtomicIncrement(&m_lTaskId);
    return Id;
}



/**	Ƕ��������ȵ����ɵ�ǰ�̵߳ı�������
    ���񱻷��뵱ǰ�̵߳ı��ض�����

	@param	TASK &Task - ��ִ�е�����	
	@return	void - ��	
*/
void CNestTaskScheduler::SpawnLocalTask(TASK &Task) 
{
    //��������뱾�ض�����
    m_DQueue.PushToLocalQueue(Task);
};


/**	Ƕ��������ȵ�����������
    ���ɵ����񱻷���ֲ�ʽ������

	@param	TASK &Task - ��ִ�е�����	
	@return	void - ��	
*/
void CNestTaskScheduler::SpawnTask(TASK &Task) 
{
    if ( m_lTaskId < m_ThreadPool.GetThreadCount() )
    {
        //���λ��Ѹ���������߳�
        LONG Id = AtomicIncrement(&m_lTaskId);
        if ( Id < m_ThreadPool.GetThreadCount() )
        {
            //����֮���Կ��Զ������̵߳ı��ض��н�����ͬ���Ĳ���������Ϊ
            // ������Щ���е��߳��ڽ��Ӳ���֮��ſ�ʼ����
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
        //���жϹ�������Ƿ��������δ������빲�������
        //�����������뱾�ض�����
        m_DQueue.EnQueue(Task);
    }
};


/**	Ƕ��������ȵ��������̺߳���

	@param	TASK &Task - Ҫִ�е��������	
	@return	void - ��	
*/
void CNestTaskScheduler::BeginRootThread(TASK &Task)
{
    m_lTaskId = 0;

    m_ThreadPool.CreateThreadPool(m_StartFunc, this, 0);

    m_DQueue.PushToLocalQueue(Task, 0);

    m_ThreadPool.ExecThread( 0 );  

    m_ThreadPool.WaitAllThread();
}

/**	Ƕ��������ȵ��̳߳���ں���

	@param	void *pArgs - CNestTaskScheduler���͵Ĳ���	
	@return	unsigned int WINAPI - ����0	
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
            
            // ����һ��������߳�,��ֹ��������С��CPU����ʱ��
            // ��Ȼ�������ڹ���״̬,�Ӷ�����WaitAllThread()��������״̬
            // ������ѹ�����һ�����еĹ��̣������ѵ�������������һ�������߳�
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



