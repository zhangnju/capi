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

/**	��������Դ�ͷŵĴ�������

	@return	MTASK * - �������ͷŽṹָ��	
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


/**	��������Դ�ͷŵ��ͷź���

	@param	MTASK *pMTask - �������ͷŽṹָ��	
	@return	void - ��	
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

    /* �رղ����������˳��¼� */
	FastLockClose( &(pMTask->FastLock) );
    EventClose( pMTask->pExitEvent );   
    
    free( pMTask );
}

/**	��������Դ�ͷŵ�����������

	@param	MTASK *pMTask - �������ͷŽṹָ��	
	@return	void - ��	
*/
void MTask_Lock(MTASK *pMTask)
{
	FastLockAcquire( &(pMTask->FastLock) );
}

/**	��������Դ�ͷŵĽ�����������

	@param	MTASK *pMTask - �������ͷŽṹָ��	
	@return	void - ��	
*/
void MTask_Unlock(MTASK *pMTask)
{
	FastLockRelease( &(pMTask->FastLock) );
}

/**	��������Դ�ͷŵĻ�ȡ�˳���־����

	@param	MTASK *pMTask - �������ͷŽṹָ��	
	@return	UINT - �˳���־��MTASK_NO_EXIT��ʾ���˳���MTASK_EXIT��ʾ�˳�	
*/
UINT MTask_GetExitFlag(MTASK *pMTask)
{
    UINT    uExitFlag;

	FastLockAcquire( &(pMTask->FastLock) );
    uExitFlag = pMTask->uExitFlag;
	FastLockRelease( &(pMTask->FastLock) );

    return uExitFlag;
}


/**	�����˳���־����

	@param	MTASK *pMTask - �������ͷŽṹָ��	
	@param	UINT uExitFlag - �˳���־��MTASK_NO_EXIT��ʾ���˳���
                             MTASK_EXIT��ʾ�˳�	
	@return	void - ��	
*/
void MTask_SetExitFlag(MTASK *pMTask, UINT uExitFlag)
{
	FastLockAcquire( &(pMTask->FastLock) );
    pMTask->uExitFlag = uExitFlag;
	FastLockRelease( &(pMTask->FastLock) );
}


/**	���������������������ʹ����������1

	@param	MTASK *pMTask - �������ͷŽṹָ��	
	@return	void - ��	
*/
void MTask_EnterTask(MTASK *pMTask)
{
	FastLockAcquire( &(pMTask->FastLock) );
    pMTask->uTaskCount += 1;
	FastLockRelease( &(pMTask->FastLock) );
}


/**	�뿪������������������ʹ����������1������������Ϊ0ʱ
    �����˳��¼�֪ͨ�ͷŲ�����������ͷ���

	@param	MTASK *pMTask - �������ͷŽṹָ��	
	@return	void - ��	
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
