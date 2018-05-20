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
 *	CRWLock.h
 *
 *	DESCRIPTION
 *		Module for reader writer lock class 
 *
 *	HISTORY
 *		04-26-2008	create by zhouweiming.
 *
 */
#ifndef __CRWLOCK_H__
#define __CRWLOCK_H__

#include "ReadWriteLock.h"

#define RWLOCK_READ      1
#define RWLOCK_WRITE     2

class CRWLock {
private:
	RWLOCK    *m_pRWLock;
public:
	CRWLock();
	~CRWLock();

    void Acquire(UINT uType);
    void Release(UINT uType);

    void Release() {}; //空的函数，保持和CScopedLock类的兼容性

	void LockRead();
	void LockWrite();
	void UnlockRead();
	void UnlockWrite();
};

#endif // __CRWLOCK_H__