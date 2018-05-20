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


/**	读写锁的创建函数，负责创建一个读写锁对象	
	@return	RWLOCK * - RWLOCK指针，失败返回NULL	
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

/**	释放一个RWLOCK对象

	@param	RWLOCK *pRWLock - 读写锁对象指针	
	@return	void - 无	
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

/**	读写锁的读操作锁函数，有读操作时需要调用这个函数进行读操作保护

	@param	RWLOCK *pRWLock - 读写锁对象指针	
	@return	void - 无	
*/
void RWLock_LockRead(RWLOCK *pRWLock)
{
    FastLockAcquire(&(pRWLock->ReadLock));
    
    pRWLock->uReadCount += 1;

    if ( pRWLock->uReadCount == 1 )
    {
        /* 当是第1个读者的时候，需要将写锁锁上防止写操作 */
        FastLockAcquire(&(pRWLock->WriteLock));
    }

    FastLockRelease(&(pRWLock->ReadLock));
}

/**	读写锁的读操作解锁函数，需要和RWLock_LockRead()函数配对使用

	@param	RWLOCK *pRWLock - 读写锁对象指针	
	@return	void - 无	
*/
void RWLock_UnlockRead(RWLOCK *pRWLock)
{
    FastLockAcquire(&(pRWLock->ReadLock));
    
    pRWLock->uReadCount -= 1;
    
    if ( pRWLock->uReadCount == 0 )
    {
        /* 当没有读者的时候，需要将些锁释放以便于其他任务可以进行写操作 */
        FastLockRelease(&(pRWLock->WriteLock));
    }
    
    FastLockRelease(&(pRWLock->ReadLock));
}

/**	读写锁的写操作锁函数，写操作发生前需要先调用这个函数进行写操作保护 

	@param	RWLOCK *pRWLock - 读写锁对象指针	
	@return	void - 无	
*/
void RWLock_LockWrite(RWLOCK *pRWLock)
{
    FastLockAcquire(&(pRWLock->WriteLock));
}

/**	读写锁的写操作解锁函数，需要和RWLock_LockWrite()函数配对使用 

	@param	RWLOCK *pRWLock - 读写锁对象指针	
	@return	void - 无	
*/
void RWLock_UnlockWrite(RWLOCK *pRWLock)
{
    FastLockRelease(&(pRWLock->WriteLock));
}
