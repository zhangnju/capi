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
 *	CSegmentHashtable.h
 *
 *	DESCRIPTION
 *		class for hashtable with segment locks  
 *
 *	HISTORY
 *		08-26-2008	create by zhouweiming.
 *
 */


#ifndef __CSEGMENTHASHTABLE_H__
#define __CSEGMENTHASHTABLE_H__


#include "CapiGlobal.h"
#include "CScopedLock.h"

class CSegmentHashTable {
PRIVATE:
    UINT         m_uBucketCount;
    UINT         m_uSegmentSize;
    UINT         m_uSegmentCount;
    int          m_nSegmentBits;

    CFastLock    *m_pLocks;
    SINGLENODE   **m_ppBucket;

    HASHFUNC     m_HashFunc;
    COMPAREFUNC  m_CompareFunc;
    DESTROYFUNC  m_DestroyFunc;
    COPYFUNC     m_CopyFunc;

    UINT         uReserved[6];  //Keep Cache Align

    inline UINT  GetSegment(UINT uBucketNo);
public:

    CSegmentHashTable(UINT uBucketCount, HASHFUNC Hash, 
        COMPAREFUNC Comp, DESTROYFUNC Destroy, COPYFUNC Copy);
    ~CSegmentHashTable();

    void *Find(void *pData);
    void Insert(void *pData);
    void Delete(void *pData);
};

#endif // __CSEGMENTHASHTABLE_H__

