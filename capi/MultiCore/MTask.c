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
#include "MTask.h"

/**	多任务资源释放的创建函数

	@return	MTASK * - 多任务释放结构指针	
*/
MTASK *MTask_Create()
{
    MTASK  *pMTask;

    pMTask = (MTASK *)malloc( sizeof(MTASK) );
    
    if ( pMTask != NULL )
    {
        FastLockInit(&(pMTask->FastLock));

		pMTask->pExitEvent = EventCreate();
        if ( pMTask->pExitEvent == NULL )
        {
            FastLockClose( &(pMTask->FastLock) );
            free( pMTask );
            return NULL;
        }
        pMTask->uTaskCount = 0;
        pMTask->uExitFlag = MTASK_NO_EXIT;
    }
    return pMTask;
}


/**	多任务资源释放的释放函数

	@param	MTASK *pMTask - 多任务释放结构指针	
	@return	void - 无	
*/
void MTask_Destroy(MTASK *pMTask)
{
    FastLockAcquire( &(pMTask->FastLock) );
    
    pMTask->uExitFlag = MTASK_EXIT;
    
    if ( pMTask->uTaskCount != 0 )
    {
        FastLockRelease( &(pMTask->FastLock) );
        WaitEvent( pMTask->pExitEvent );
    }
    else
    {
		FastLockRelease( &(pMTask->FastLock) );
    }

    /* 关闭操作的锁和退出事件 */
	FastLockClose( &(pMTask->FastLock) );
    EventClose( pMTask->pExitEvent );   
    
    free( pMTask );
}

/**	多任务资源释放的锁操作函数

	@param	MTASK *pMTask - 多任务释放结构指针	
	@return	void - 无	
*/
void MTask_Lock(MTASK *pMTask)
{
	FastLockAcquire( &(pMTask->FastLock) );
}

/**	多任务资源释放的解锁操作函数

	@param	MTASK *pMTask - 多任务释放结构指针	
	@return	void - 无	
*/
void MTask_Unlock(MTASK *pMTask)
{
	FastLockRelease( &(pMTask->FastLock) );
}

/**	多任务资源释放的获取退出标志函数

	@param	MTASK *pMTask - 多任务释放结构指针	
	@return	UINT - 退出标志，MTASK_NO_EXIT表示不退出，MTASK_EXIT表示退出	
*/
UINT MTask_GetExitFlag(MTASK *pMTask)
{
    UINT    uExitFlag;

	FastLockAcquire( &(pMTask->FastLock) );
    uExitFlag = pMTask->uExitFlag;
	FastLockRelease( &(pMTask->FastLock) );

    return uExitFlag;
}


/**	设置退出标志函数

	@param	MTASK *pMTask - 多任务释放结构指针	
	@param	UINT uExitFlag - 退出标志，MTASK_NO_EXIT表示不退出，
                             MTASK_EXIT表示退出	
	@return	void - 无	
*/
void MTask_SetExitFlag(MTASK *pMTask, UINT uExitFlag)
{
	FastLockAcquire( &(pMTask->FastLock) );
    pMTask->uExitFlag = uExitFlag;
	FastLockRelease( &(pMTask->FastLock) );
}


/**	进入操作任务函数，作用是使计数变量加1

	@param	MTASK *pMTask - 多任务释放结构指针	
	@return	void - 无	
*/
void MTask_EnterTask(MTASK *pMTask)
{
	FastLockAcquire( &(pMTask->FastLock) );
    pMTask->uTaskCount += 1;
	FastLockRelease( &(pMTask->FastLock) );
}


/**	离开操作任务函数，作用是使计数变量减1，当计数变量为0时
    发送退出事件通知释放操作任务可以释放了

	@param	MTASK *pMTask - 多任务释放结构指针	
	@return	void - 无	
*/
void MTask_LeaveTask(MTASK *pMTask)
{
	FastLockAcquire( &(pMTask->FastLock) );

    pMTask->uTaskCount -= 1;
    
    if ( pMTask->uTaskCount == 0 )
    {
        SendEvent( pMTask->pExitEvent );
    }

	FastLockRelease( &(pMTask->FastLock) );
}
