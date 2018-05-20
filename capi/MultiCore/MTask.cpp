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
	EVENT pExitEvent;	/* �˳��¼� */
	LONG volatile lExitFlag;	/* �˳���־ */
	LONG volatile lTaskCount;	/* ���������������� */
} MTASK;



/**	��������Դ�ͷŵĴ�������
	@return	MTASK *�����������ͷŽṹָ��	
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
/**	��������Դ�ͷŵ��ͷź���
	@param	MTASK *pMTask�����������ͷŽṹָ��	
	@return	void������	
*/
void MTask_Destroy(MTASK *pMTask)
{
	AtomicWrite(&(pMTask->lExitFlag), MTASK_EXIT);
	if ( pMTask->lTaskCount != 0 )
	{
		WaitEvent( pMTask->pExitEvent );
	}
	/* �رղ����������˳��¼� */
	EventClose( pMTask->pExitEvent );  
	free( pMTask );
}

/**	��������Դ�ͷŵĻ�ȡ�˳���־����
	@param	MTASK *pMTask�����������ͷŽṹָ��	
	@return	UINT��������MTASK_NO_EXIT��ʾ���˳�;����MTASK_EXIT��ʾ�˳�
*/
UINT MTask_GetExitFlag(MTASK *pMTask)
{
	return pMTask->lExitFlag;
}
/**	�����˳���־����
	@param	MTASK *pMTask�����������ͷŽṹָ��	
	@param	UINT uExitFlag��������MTASK_NO_EXIT��ʾ���˳�;����
					MTASK_EXIT��ʾ�˳�
	@return	void������	
*/
void MTask_SetExitFlag(MTASK *pMTask, UINT uExitFlag)
{
	AtomicWrite(&(pMTask->lExitFlag), uExitFlag);
}
/**	���������������������ʹ����������1
	@param	MTASK *pMTask�����������ͷŽṹָ��	
	@return	void������	
*/
void MTask_EnterTask(MTASK *pMTask)
{
	AtomicIncrement(&(pMTask->lTaskCount));
}
/**	�뿪������������������ʹ����������1������������Ϊ0ʱ
	�����˳��¼�֪ͨ���ͷŲ�����������ͷ���
	@param	MTASK *pMTask�����������ͷŽṹָ��	
	@return	void������	
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
			//�ͷű������ڱ�Ҫ����Դ
			break;
		}

		//�����̴߳���

		//����߳��ڵ�һ��ѭ�������¼��ǳ����Ļ������Զ�Ӽ����˳���־�ж�
		if ( MTask_GetExitFlag() == MTASK_EXIT )
		{
			break;
		}

		//�����̴߳���
	
	}
	MTask_LeaveTask();
}
#endif