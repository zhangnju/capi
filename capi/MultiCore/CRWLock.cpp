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

#include "CRWLock.h"

CRWLock::CRWLock() 
{
	m_pRWLock = RWLock_Create();
}

CRWLock::~CRWLock() 
{
	if ( m_pRWLock != NULL )
	{
		RWLock_Destroy(m_pRWLock);
	}
}


void CRWLock::Acquire(UINT uType)
{
    switch ( uType )
    {
    case RWLOCK_READ:
        RWLock_LockRead(m_pRWLock);
        break;
    case RWLOCK_WRITE:
        RWLock_LockWrite(m_pRWLock);
        break;
    }
}

void CRWLock::Release(UINT uType)
{
    switch ( uType )
    {
    case RWLOCK_READ:
        RWLock_UnlockRead(m_pRWLock);
        break;
    case RWLOCK_WRITE:
        RWLock_UnlockWrite(m_pRWLock);
        break;
    }
}



void CRWLock::LockRead() 
{
	RWLock_LockRead(m_pRWLock);
}

void CRWLock::LockWrite() 
{
	RWLock_LockWrite(m_pRWLock);
}

void CRWLock::UnlockRead()
{
	RWLock_UnlockRead(m_pRWLock);
}

void CRWLock::UnlockWrite() 
{
	RWLock_UnlockWrite(m_pRWLock);
}
