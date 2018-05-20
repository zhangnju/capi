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
#define _WIN32_WINNT 0x0403
#include <windows.h>

#define MTASK_NO_EXIT		0
#define MTASK_EXIT			1

#define LOCK	HANDLE
#define EVENT	HANDLE

#define EventCreate()   CreateEvent(NULL, FALSE, FALSE, NULL)
#define WaitEvent(x)    WaitForSingleObject(x, INFINITE)
#define SendEvent(x)	SetEvent(x)
#define EventClose(x)	CloseHandle(x)


//#define AtomicDecrement(x)  InterlockedDecrement(x)
//#define AtomicIncrement(x)  InterlockedIncrement(x)
#define AtomicCAS(x, y, z)  InterlockedCompareExchange(x, y, z)

//#define AtomicWrite(x, y)   InterlockedExchange(x, y)


LONG AtomicWrite(LONG volatile *Target, LONG Value)
{
	LONG	Old;

	do {
		Old = *Target;

	}while (!AtomicCAS(Target, Value, Old));

	return Old;
}

LONG AtomicIncrement(LONG volatile *Target)
{
	LONG	Old;

	do {
		Old = *Target;

	}while (!AtomicCAS(Target, Old + 1, Old));

	return Old;
}

BOOL TAS(LONG volatile *value)
{
	LONG	ret;
	ret = AtomicCAS(value, 1, 0);
	if ( ret == 0 )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

#if 0	
	atomic {
		if (*value == 0) {
			*value = 1;
			return TRUE;
		}
		else {
		  return FALSE;
		}
	}
#endif
}

LONG AtomicDecrement(LONG volatile *Target)
{
	LONG	Old;

	do {
		Old = *Target;

	}while (!AtomicCAS(Target, Old - 1, Old));

	return Old;
}


HANDLE LockCreate()  	
{
	CRITICAL_SECTION	*pcs = new CRITICAL_SECTION;

	InitializeCriticalSectionAndSpinCount(pcs, 20);

	return pcs;
}

void Lock(HANDLE hLock)
{
	EnterCriticalSection((CRITICAL_SECTION *)hLock);
}

void Unlock(HANDLE hLock)
{
	LeaveCriticalSection((CRITICAL_SECTION *)hLock);
}


void LockClose(HANDLE hLock)
{
	DeleteCriticalSection((CRITICAL_SECTION *)hLock);
	delete (CRITICAL_SECTION *)hLock;
}


typedef struct MTASK_st {
	EVENT pExitEvent;	/* 退出事件 */
	LONG volatile lExitFlag;	/* 退出标志 */
	LONG volatile lTaskCount;	/* 操作的子任务数量 */
} MTASK;



/**	多任务资源释放的创建函数
	@return	MTASK *——多任务释放结构指针	
*/
MTASK *MTask_Create()
{
	MTASK  *pMTask;
	pMTask = (MTASK *)malloc( sizeof(MTASK) );
	if ( pMTask != NULL )
	{
		pMTask->pExitEvent = EventCreate();
		if ( pMTask->pExitEvent == NULL )
		{
			free( pMTask );
			return NULL;
		}
		pMTask->lTaskCount = 0;
		pMTask->lExitFlag = MTASK_NO_EXIT;
	}
	return pMTask;
}
/**	多任务资源释放的释放函数
	@param	MTASK *pMTask——多任务释放结构指针	
	@return	void——无	
*/
void MTask_Destroy(MTASK *pMTask)
{
	AtomicWrite(&(pMTask->lExitFlag), MTASK_EXIT);
	if ( pMTask->lTaskCount != 0 )
	{
		WaitEvent( pMTask->pExitEvent );
	}
	/* 关闭操作的锁和退出事件 */
	EventClose( pMTask->pExitEvent );  
	free( pMTask );
}

/**	多任务资源释放的获取退出标志函数
	@param	MTASK *pMTask——多任务释放结构指针	
	@return	UINT——返回MTASK_NO_EXIT表示不退出;返回MTASK_EXIT表示退出
*/
UINT MTask_GetExitFlag(MTASK *pMTask)
{
	return pMTask->lExitFlag;
}
/**	设置退出标志函数
	@param	MTASK *pMTask——多任务释放结构指针	
	@param	UINT uExitFlag——返回MTASK_NO_EXIT表示不退出;返回
					MTASK_EXIT表示退出
	@return	void——无	
*/
void MTask_SetExitFlag(MTASK *pMTask, UINT uExitFlag)
{
	AtomicWrite(&(pMTask->lExitFlag), uExitFlag);
}
/**	进入操作任务函数，作用是使计数变量加1
	@param	MTASK *pMTask——多任务释放结构指针	
	@return	void——无	
*/
void MTask_EnterTask(MTASK *pMTask)
{
	AtomicIncrement(&(pMTask->lTaskCount));
}
/**	离开操作任务函数，作用是使计数变量减1，当计数变量为0时
	发送退出事件通知，释放操作任务可以释放了
	@param	MTASK *pMTask——多任务释放结构指针	
	@return	void——无	
*/
void MTask_LeaveTask(MTASK *pMTask)
{
	AtomicDecrement(&(pMTask->lTaskCount));
	if ( pMTask->lTaskCount == 0 )
	{
		SendEvent( pMTask->pExitEvent );
	}
}


#if 0
void  ThreadFunc(LPVOID args)
{
	MTask_EnterTask();
	for(;;)
	{
		if ( MTask_GetExitFlag() == MTASK_EXIT )
		{
			//释放本函数内必要的资源
			break;
		}

		//进行线程处理

		//如果线程内的一次循环操作事件非常长的话，可以多加几次退出标志判断
		if ( MTask_GetExitFlag() == MTASK_EXIT )
		{
			break;
		}

		//进行线程处理
	
	}
	MTask_LeaveTask();
}
#endif