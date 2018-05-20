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

#include "CScopedLock.h"


CFastLock::CFastLock() 
{
	FastLockInit(&m_Lock);
}

CFastLock::~CFastLock() 
{
	FastLockClose(&m_Lock);
}

void CFastLock::Acquire() 
{
	FastLockAcquire(&m_Lock);
}

int CFastLock::TryAcquire() 
{
	int nRet;
	nRet = FastLockTryAcquire(&m_Lock);
	return nRet;
}

void CFastLock::Release() 
{
	FastLockRelease(&m_Lock);
}


CSpinLock::CSpinLock(int SpinCount):m_SpinCount(SpinCount) 
{
	SpinLockInit(&m_SpinLock, SpinCount);
}

CSpinLock::~CSpinLock() 
{
	SpinLockClose(&m_SpinLock);
}

void CSpinLock::Acquire() 
{
	SpinLockAcquire(&m_SpinLock);
}

int CSpinLock::TryAcquire() 
{
	return SpinLockTryAcquire(&m_SpinLock);
}

void CSpinLock::Release() 
{
	SpinlockRelease(&m_SpinLock);
}