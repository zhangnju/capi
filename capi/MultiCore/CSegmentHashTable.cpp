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
#include <omp.h>
#include "CSegmentHashTable.h"


CSegmentHashTable::CSegmentHashTable(UINT uBucketCount, HASHFUNC Hash, 
                      COMPAREFUNC Comp, DESTROYFUNC Destroy, COPYFUNC Copy)
{
    m_uBucketCount = uBucketCount;

    m_HashFunc = Hash;
    m_DestroyFunc = Destroy;
    m_CopyFunc = Copy;
    m_CompareFunc = Comp;

    int nCore = omp_get_num_procs();

    int nSegment = uBucketCount / nCore;

    int nBits = 0;
    while ( nSegment > 0 )
    {
        nSegment = nSegment >> 1;
        nBits += 1;
    }

    nBits -= 1;

    if ( nBits > 2 )
    {
        nBits -= 2;
    }

    m_nSegmentBits = nBits;

    m_uSegmentSize = 1 << nBits;

    m_uSegmentCount = uBucketCount / m_uSegmentSize;

    if ( (m_uSegmentCount * m_uSegmentSize) < uBucketCount )
    {
        m_uSegmentCount += 2;
    }
    else
    {
        m_uSegmentCount += 1;
    }

    m_pLocks = new CFastLock[m_uSegmentCount];

    m_ppBucket = new SINGLENODE *[uBucketCount];

    UINT i;
    for ( i = 0; i < uBucketCount; i++ )
    {
        m_ppBucket[i] = NULL;
    }
}



CSegmentHashTable::~CSegmentHashTable()
{
    UINT i;
    SINGLENODE *pNode;
    SINGLENODE *pFreeNode;

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
            delete pFreeNode;
        }
    }

    delete [] m_pLocks;
    delete [] m_ppBucket;
}


inline UINT CSegmentHashTable::GetSegment(UINT uBucketNo)
{
    return (uBucketNo >> m_nSegmentBits); 
}


void *CSegmentHashTable::Find(void *pData)
{
    UINT uBucketNo = (*m_HashFunc)(pData, m_uBucketCount);

    UINT uSegment = GetSegment(uBucketNo);

    CScopedLock<CFastLock> slock(m_pLocks[uSegment]);

    SINGLENODE *pNode = m_ppBucket[uBucketNo];

    // 在 HASHTABLE 中进行查找
    while ( pNode != NULL )
    {
        if ( (*m_CompareFunc)( pNode->pData, pData) == 0 )
        {
            // 已经找到了关键词，返回拷贝后的数据 
            void *pCopyData;
            if ( m_CopyFunc != NULL )
            {
                pCopyData = (*m_CopyFunc)(pNode->pData);
            }
            else
            {
                pCopyData = pNode->pData;
            }
            return pCopyData;
        }
        pNode = pNode->pNext;
    }

    return NULL;

}


void CSegmentHashTable::Insert(void *pData)
{
    UINT uBucketNo = (*m_HashFunc)(pData, m_uBucketCount);

    UINT uSegment = GetSegment(uBucketNo);

    CScopedLock<CFastLock> slock(m_pLocks[uSegment]);

    SINGLENODE *pNode = m_ppBucket[uBucketNo];

    SINGLENODE *pNewNode = new SINGLENODE;

    // 将新节点插入到链表的头部
    pNewNode->pData = pData;
    pNewNode->pNext = pNode;

    m_ppBucket[uBucketNo] = pNewNode;

    return;
}


void CSegmentHashTable::Delete(void *pData)
{
    UINT uBucketNo = (*m_HashFunc)(pData, m_uBucketCount);

    UINT uSegment = GetSegment(uBucketNo);

    CScopedLock<CFastLock> slock(m_pLocks[uSegment]);

    SINGLENODE *pNode = m_ppBucket[uBucketNo];

    SINGLENODE *pPrevNode = pNode;

    /* 从哈希表中查找 */
    while ( pNode != NULL )
    {
        if ( (*m_CompareFunc)( pNode->pData, pData ) == 0 )
        {
            if ( pPrevNode == pNode )
            {
                m_ppBucket[uBucketNo] = pNode->pNext;
            }
            else
            {
                pPrevNode->pNext = pNode->pNext;
            }

            /* 删除对应节点 */
            if ( m_DestroyFunc != NULL )
            {
                (*m_DestroyFunc)(pNode->pData);
            }
            delete pNode;
            return;
        }
        pPrevNode = pNode;
        pNode = pNode->pNext;
    }

    return;
}



