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

#include <windows.h>
#include <stdio.h>
#include "CapiGlobal.h"
#include "ReadWriteLock.h"


/**	��д���Ĵ������������𴴽�һ����д������	
	@return	RWLOCK * - RWLOCKָ�룬ʧ�ܷ���NULL	
*/
RWLOCK *RWLock_Create(void)
{
    RWLOCK  *pRWLock;
    pRWLock = (RWLOCK *)malloc(sizeof(RWLOCK));
    if ( pRWLock != NULL )
    {
        FastLockInit(&(pRWLock->ReadLock));
		FastLockInit(&(pRWLock->WriteLock));
        pRWLock->uReadCount = 0;
    }
    return pRWLock;
}

/**	�ͷ�һ��RWLOCK����

	@param	RWLOCK *pRWLock - ��д������ָ��	
	@return	void - ��	
*/
void RWLock_Destroy(RWLOCK *pRWLock)
{
    if ( pRWLock != NULL )
    {
        FastLockClose(&(pRWLock->ReadLock));
        FastLockClose(&(pRWLock->WriteLock));
        free(pRWLock);
    }
}

/**	��д���Ķ��������������ж�����ʱ��Ҫ��������������ж���������

	@param	RWLOCK *pRWLock - ��д������ָ��	
	@return	void - ��	
*/
void RWLock_LockRead(RWLOCK *pRWLock)
{
    FastLockAcquire(&(pRWLock->ReadLock));
    
    pRWLock->uReadCount += 1;

    if ( pRWLock->uReadCount == 1 )
    {
        /* ���ǵ�1�����ߵ�ʱ����Ҫ��д�����Ϸ�ֹд���� */
        FastLockAcquire(&(pRWLock->WriteLock));
    }

    FastLockRelease(&(pRWLock->ReadLock));
}

/**	��д���Ķ�����������������Ҫ��RWLock_LockRead()�������ʹ��

	@param	RWLOCK *pRWLock - ��д������ָ��	
	@return	void - ��	
*/
void RWLock_UnlockRead(RWLOCK *pRWLock)
{
    FastLockAcquire(&(pRWLock->ReadLock));
    
    pRWLock->uReadCount -= 1;
    
    if ( pRWLock->uReadCount == 0 )
    {
        /* ��û�ж��ߵ�ʱ����Ҫ��Щ���ͷ��Ա�������������Խ���д���� */
        FastLockRelease(&(pRWLock->WriteLock));
    }
    
    FastLockRelease(&(pRWLock->ReadLock));
}

/**	��д����д������������д��������ǰ��Ҫ�ȵ��������������д�������� 

	@param	RWLOCK *pRWLock - ��д������ָ��	
	@return	void - ��	
*/
void RWLock_LockWrite(RWLOCK *pRWLock)
{
    FastLockAcquire(&(pRWLock->WriteLock));
}

/**	��д����д����������������Ҫ��RWLock_LockWrite()�������ʹ�� 

	@param	RWLOCK *pRWLock - ��д������ָ��	
	@return	void - ��	
*/
void RWLock_UnlockWrite(RWLOCK *pRWLock)
{
    FastLockRelease(&(pRWLock->WriteLock));
}
