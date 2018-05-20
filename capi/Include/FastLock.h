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
 *	FastLock.h
 *
 *	DESCRIPTION
 *		base definition for fast lock and spin lock operation.
 *
 *	HISTORY
 *		04-26-2008	create by zhouweiming.
 *
 */
#ifndef __FASTLOCK_H__
#define __FASTLOCK_H__

#if _WIN32 || _WIN64
#if _WIN32_WINNT < 0x0404
#undef _WIN32_WINNT
#define _WIN32_WINNT  0x0404
#endif
#include <windows.h>
#else
#include <pthread.h>
#endif /* _WIN32 || _WIN64 */



#if _WIN32 || _WIN64


#define  FASTLOCK					CRITICAL_SECTION
#define  FastLockInit(x)			InitializeCriticalSection(x)
#define  FastLockAcquire(x)			EnterCriticalSection(x)
#define  FastLockTryAcquire(x)		TryEnterCriticalSection(x)
#define  FastLockRelease(x)			LeaveCriticalSection(x)
#define  FastLockClose(x)			DeleteCriticalSection(x)

#define  SPINLOCK					CRITICAL_SECTION
#define  SpinLockInit(x, num)	    InitializeCriticalSectionAndSpinCount(x, num)
#define  SpinLockAcquire(x)			EnterCriticalSection(x)
#define  SpinLockTryAcquire(x)		TryEnterCriticalSection(x)
#define  SpinlockRelease(x)			LeaveCriticalSection(x)
#define  SpinLockClose(x)			DeleteCriticalSection(x)

#else

#define  FASTLOCK					pthread_mutex_t
#define  FastLockInit(x)			pthread_mutex_create(x, NULL)
#define  FastLockAcquire(x)			pthread_mutex_lock(x)
#define  FastLockTryAcquire(x)		pthread_mutex_trylock(x)
#define  FastlockRelease(x)			pthread_mutex_unlock(x)
#define  FastLockClose(x)			pthread_mutex_destroy(x)

#define SPINLOCK					pthread_spinlock_t
#define SpinLockInit(lock, num)		pthread_spin_init(lock, num)
#define SpinLockAcquire(lock)		pthread_spin_lock(lock)
#define SpinLockRelease(lock)		pthread_spin_unlock(lock)
#define SpinLockTryAcquire(lock)	pthread_spin_trylock(lock)
#define SpinLockClose(lock)			pthread_spin_destroy(lock)

#endif /* _WIN32 || _WIN64 */


#endif /* __FASTLOCK_H__ */