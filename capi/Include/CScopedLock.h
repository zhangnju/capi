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
 *	CScopedLock.h
 *
 *	DESCRIPTION
 *		Module for automatic unlock class CScopedLock 
 *
 *	HISTORY
 *		04-26-2008	create by zhouweiming.
 *
 */


#ifndef __CSCOPEDLOCK_H__
#define __CSCOPEDLOCK_H__

//#include "CapiGlobal.h"
#include "FastLock.h"


class CFastLock {
private:
	FASTLOCK	m_Lock;
public:
	CFastLock();
	~CFastLock(); 

	void Acquire(); 
	int TryAcquire();
	void Release();

    void Release(UINT uType) {}; //�յĺ�������Ҫ�Ǳ��ֺ�CScopedLock��ļ�����
};


class CSpinLock {
private:
	SPINLOCK	m_SpinLock;
	int			m_SpinCount;
public:
	CSpinLock(int SpinCount);
	~CSpinLock() ;

	void Acquire();
	int TryAcquire();
	void Release();

    void Release(UINT uType) {}; //�յĺ�������Ҫ�Ǳ��ֺ�CScopedLock��ļ�����
};


template <class T> class CScopedLock {
private:
	T *m_pLock;
    UINT  m_uType;
public:
    //���캯��ΪCFastLock, CSpinLock��ʹ��
	CScopedLock(T &l) : m_uType(0) {
		m_pLock = &l;
		m_pLock->Acquire();
	};

    // ����Ĺ��캯����ҪΪ��д��CRWLock��ʹ��
    CScopedLock(T &l, UINT uType) : m_uType(uType)  {
        m_pLock = &l;
        m_pLock->Acquire(uType);
    };

	~CScopedLock() {
		if ( m_pLock != NULL )
		{
            if ( !m_uType )
            {
			    m_pLock->Release();
           }
            else
            {
                m_pLock->Release(m_uType);
            }
		}
	};
};

#endif //__CSCOPEDLOCK_H__
