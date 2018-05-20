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
 *	THashTable.h
 *
 *	DESCRIPTION
 *		Interface to a generic hash object. This hashtable is used
 *      only for TMemory.cpp 
 *
 *	HISTORY
 *		08-21-2006	create by zhouweiming.
 *
 */
#ifndef __THASHTABLE_H__
#define __THASHTABLE_H__



#define	    DEFAULT_HASHTABLE_SIZE	    32768
#define		MAX_SEARCH_DEEP		30

class THashTable {
protected:
    SINGLENODE  **m_ppBucket;
    UINT        m_uBucketCount;
    UINT        m_uNodeCount;
    UINT        m_uCurBucketNo;
    SINGLENODE  *m_pCurEntry;

    HASHFUNC    m_HashFunc;
    COMPAREFUNC m_CompareFunc;
    DESTROYFUNC m_DestroyFunc;

public:
    THashTable();
    THashTable(UINT uBucketCount, 
        HASHFUNC HashFunc = NULL, 
        COMPAREFUNC CompareFunc = NULL, 
        DESTROYFUNC DestroyFunc = NULL);
    virtual ~THashTable();

    INT Create();
    INT Create(UINT uBucketCount, HASHFUNC HashFunc,
                         COMPAREFUNC CompareFunc,
                         DESTROYFUNC DataDestroyFunc );

    void Lock();
    void Unlock();

    INT     Insert(void *pData);
    void *  Find(void *pData);
    void *  Find(void *pData, HASHFUNC HashFunc);
    void *  Find_S(void *pData, HASHFUNC HashFunc);

    INT     Delete(void *pData);
    INT     Delete(void *pData, HASHFUNC HashFunc);
    INT     Delete_S(void *pData, HASHFUNC HashFunc);

    UINT    GetNodeCount();
    UINT    GetBucketCount();
    UINT    GetCurBucketNo();

    void    SetHashFunc(HASHFUNC HashFunc);
    void    SetCompareFunc(COMPAREFUNC CompareFunc);
    void    SetDestroyFunc(DESTROYFUNC DestroyFunc);
    
    void    EnumBegin();
    void *  EnumNext_S();
    void *  EnumNext();
};




#endif /* __THASHTABLE_H__ */
