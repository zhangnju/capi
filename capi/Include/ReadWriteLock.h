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

/* 
 *	ReadWriteLock.h
 *
 *	DESCRIPTION
 *		读写锁模块.
 *
 *	HISTORY
 *		10-03-98	create by zhouweiming.
 *
 */

#ifndef __RWLOCK_H__
#define __RWLOCK_H__

#include "FastLock.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct RWLOCK_st {
	FASTLOCK    ReadLock;     /* 读操作锁，主要用来保护计数器变量的读写 */
	FASTLOCK    WriteLock;    /* 写操作锁 */
	UINT		uReadCount;   /* 正在进行的读操作的数量 */
} RWLOCK;


RWLOCK *RWLock_Create(void);
void RWLock_Destroy(RWLOCK *pRWLock);

void RWLock_LockRead(RWLOCK *pRWLock);
void RWLock_UnlockRead(RWLOCK *pRWLock);

void RWLock_LockWrite(RWLOCK *pRWLock);
void RWLock_UnlockWrite(RWLOCK *pRWLock);

#ifdef __cplusplus
}
#endif

#endif /* __RWLOCK_H__ */

