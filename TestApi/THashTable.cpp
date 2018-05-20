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
#include <stdlib.h>
#include <string.h>
#include "CapiGlobal.h"
#include "TestApi.h"
#include "THashTable.h"
#include "FastLock.h"

FASTLOCK    g_lock;

THashTable::THashTable()
{
    m_uBucketCount = DEFAULT_HASHTABLE_SIZE;
    m_HashFunc = NULL;
    m_CompareFunc = NULL;
    m_DestroyFunc = NULL;
    m_uNodeCount = 0;
    m_uCurBucketNo = 0;
    m_pCurEntry = NULL;
    m_ppBucket = NULL;
    FastLockInit(&g_lock);
}

THashTable::THashTable( UINT uBucketCount, 
                       HASHFUNC HashFunc, 
                       COMPAREFUNC CompareFunc, 
                       DESTROYFUNC DestroyFunc)
{
    m_uBucketCount = uBucketCount;
    m_HashFunc = HashFunc;
    m_CompareFunc = CompareFunc;
    m_DestroyFunc = DestroyFunc;
    m_uNodeCount = 0;
    m_uCurBucketNo = 0;
    m_pCurEntry = NULL;
    m_ppBucket = NULL;
    FastLockInit(&g_lock);
}


THashTable::~THashTable()
{
    SINGLENODE  *pNode;
    SINGLENODE  *pFreeNode;
    UINT i;

    if ( m_ppBucket != NULL )
    {
        for ( i = 0; i < m_uBucketCount; i++ ) 
        {
            pNode = m_ppBucket[i];
            while ( pNode != NULL )
            {
                if ( m_DestroyFunc != NULL )
                {
                    (*m_DestroyFunc)(pNode->pData);
                }
                pFreeNode = pNode;
                pNode = pNode->pNext;
                unhooked_free(pFreeNode);
            }
        }
        char *psz = (char *)m_ppBucket;
        unhooked_free(psz);

        /* 将m_ppbucket设成空指针以避免哈希表被重新使用时造成内存泄漏 */
        m_ppBucket = NULL;
    }
    FastLockClose(&g_lock);
}


INT THashTable::Create()
{
    if ( m_uBucketCount == 0 )
    {
        return CAPI_FAILED;
    }

    char *psz = (char *)unhooked_malloc(sizeof(SINGLENODE *) * m_uBucketCount);
    m_ppBucket = (SINGLENODE **)psz;

    memset(m_ppBucket, 0, m_uBucketCount * sizeof(SINGLENODE *));

    return CAPI_SUCCESS;
}

INT THashTable::Create(UINT uBucketCount, HASHFUNC HashFunc,
                     COMPAREFUNC CompareFunc,
                     DESTROYFUNC DestroyFunc )
{
    if ( uBucketCount == 0 )
    {
        return CAPI_FAILED;
    }

    char *psz = (char *)unhooked_malloc(sizeof(SINGLENODE *) * uBucketCount);
    m_ppBucket = (SINGLENODE **)psz;

    memset(m_ppBucket, 0, uBucketCount * sizeof(SINGLENODE *));

    m_uBucketCount = uBucketCount;

    m_HashFunc = HashFunc;
    m_CompareFunc = CompareFunc;
    m_DestroyFunc = DestroyFunc;

    return CAPI_SUCCESS;
}

void THashTable::Lock()
{
    FastLockAcquire(&g_lock);
}

void THashTable::Unlock()
{
    FastLockRelease(&g_lock);
}


INT THashTable::Insert(void *pData)
{
    UINT		uIndex;
    SINGLENODE	*pNode;
    SINGLENODE	*pNewNode;

    if ( pData == NULL || m_HashFunc == NULL )
    {
        return CAPI_FAILED;
    }

    FastLockAcquire(&g_lock);

    uIndex = (*m_HashFunc)( pData, m_uBucketCount );
    pNode = m_ppBucket[uIndex];

    pNewNode = (SINGLENODE *)unhooked_malloc(sizeof(SINGLENODE));

    /* 将新节点插入到链表的头部 */
    pNewNode->pData = pData;
    pNewNode->pNext = pNode;

    m_ppBucket[uIndex] = pNewNode;
    m_uNodeCount += 1;

    FastLockRelease(&g_lock);

    return CAPI_SUCCESS;
}

void *  THashTable::Find(void *pData)
{
    void *pRet;

    pRet = Find(pData, m_HashFunc);

    return pRet;
}

void * THashTable::Find(void *pData, HASHFUNC HashFunc)
{
    void  *	pRet;

    FastLockAcquire(&g_lock);
    pRet = Find_S(pData, HashFunc);
    FastLockRelease(&g_lock);

    return pRet;
}

void * THashTable::Find_S(void *pData, HASHFUNC HashFunc)
{
	UINT			uIndex;
	SINGLENODE *	pNode;

	if ( HashFunc == NULL || m_CompareFunc == NULL )
	{
		return NULL;
	}

	uIndex = (*HashFunc)( pData, m_uBucketCount );
	pNode = m_ppBucket[uIndex];
	
	/* 在 HASHTABLE 中进行查找 */
	while ( pNode != NULL )
	{
		if ( (*m_CompareFunc)( pNode->pData, pData) == 0 )
		{
            /* 已经找到了关键词，返回 */
			return pNode->pData;
		}
		pNode = pNode->pNext;
	}
    return NULL;
}

INT THashTable::Delete(void *pData)
{
    return Delete(pData, m_HashFunc);
}


INT THashTable::Delete(void *pData, HASHFUNC HashFunc)
{
    UINT			uRet;

    FastLockAcquire(&g_lock);
    uRet = Delete_S(pData, HashFunc);
    FastLockRelease(&g_lock);
    return uRet;
}



INT THashTable::Delete_S(void *pData, HASHFUNC HashFunc)
{
	UINT			uIndex;
	SINGLENODE *	pNode;
	SINGLENODE *    pPrevNode;

	if ( pData == NULL || HashFunc == NULL 
        || m_CompareFunc == NULL )
	{
		return CAPI_FAILED;
	}
	uIndex = (*HashFunc)( pData, m_uBucketCount );
	pNode = m_ppBucket[uIndex];
    pPrevNode = pNode;

    /* 从哈希表中查找 */
    while ( pNode != NULL )
    {
        if ( (*m_CompareFunc)( pNode->pData, pData ) == 0 )
        {
            if ( pPrevNode == pNode )
            {
                m_ppBucket[uIndex] = pNode->pNext;
            }
            else
            {
                pPrevNode->pNext = pNode->pNext;
            }

            if ( pNode == m_pCurEntry )
            {
                EnumNext_S();
            }
            /* 删除对应节点 */
            if ( m_DestroyFunc != NULL )
            {
                (*m_DestroyFunc)(pNode->pData);
            }
            unhooked_free(pNode);

            m_uNodeCount -= 1;

            return CAPI_SUCCESS;
        }

        pPrevNode = pNode;
        pNode = pNode->pNext;
    }
    return CAPI_FAILED;
}

UINT THashTable::GetNodeCount()
{
    UINT uCount;
    FastLockAcquire(&g_lock);

    uCount = m_uNodeCount;

    FastLockRelease(&g_lock);
    return uCount;
}

UINT THashTable::GetBucketCount()
{
    return m_uBucketCount;
}

UINT THashTable::GetCurBucketNo()
{
    UINT uCurBucketNo;

    FastLockAcquire(&g_lock);
    uCurBucketNo = m_uCurBucketNo;
    FastLockRelease(&g_lock);
    
    return uCurBucketNo;
}

void THashTable::SetHashFunc(HASHFUNC HashFunc)
{
    FastLockAcquire(&g_lock);
    m_HashFunc = HashFunc;
    FastLockRelease(&g_lock);

}

void THashTable::SetCompareFunc(COMPAREFUNC CompareFunc)
{
    FastLockAcquire(&g_lock);
    m_CompareFunc = CompareFunc;
    FastLockRelease(&g_lock);
}

void THashTable::SetDestroyFunc(DESTROYFUNC DestroyFunc)
{
    FastLockAcquire(&g_lock);
    m_DestroyFunc = DestroyFunc;
    FastLockRelease(&g_lock);
}

void THashTable::EnumBegin()
{
    m_uCurBucketNo = 0;
    m_pCurEntry = m_ppBucket[0];
}


void * THashTable::EnumNext_S()
{
    void *pData;

    while ( m_pCurEntry == NULL )
    {
        m_uCurBucketNo += 1;
        if ( m_uCurBucketNo >= m_uBucketCount )
        {
            return NULL;
        }
        m_pCurEntry = m_ppBucket[m_uCurBucketNo];
    }

    pData = m_pCurEntry->pData;

    m_pCurEntry = m_pCurEntry->pNext;

    return pData;
}


void * THashTable::EnumNext()   //需要改成拷贝返回的数据，否则操作枚举返回的数据会出现异常
{
    void *pData;
    FastLockAcquire(&g_lock);

	while ( m_pCurEntry == NULL )
	{
		m_uCurBucketNo += 1;
		if ( m_uCurBucketNo >= m_uBucketCount )
		{
            FastLockRelease(&g_lock);
			return NULL;
		}
		m_pCurEntry = m_ppBucket[m_uCurBucketNo];
	}

	pData = m_pCurEntry->pData;

    m_pCurEntry = m_pCurEntry->pNext;
    FastLockRelease(&g_lock);

    return pData;
}


