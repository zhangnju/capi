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

#include "CDHashAVLTree.h"


/**	CDHashAVLTree的构造函数

	@param	int nBucketCount - 桶的数量	
	@param	HASHFUNC HashFunc - 哈希函数	
	@param	COMPAREFUNC Compare - 数据比较函数	
	@param	DESTROYFUNC Destroy - 数据释放函数	
	@param	COPYFUNC CopyFunc - 数据拷贝函数	
	@return	constructor - 无	
*/
CDHashAVLTree::CDHashAVLTree(int nBucketCount, HASHFUNC HashFunc, 
		 COMPAREFUNC Compare, DESTROYFUNC Destroy, COPYFUNC CopyFunc)

{
	m_HashFunc = HashFunc;
	m_Compare = Compare;
	m_nBucketCount = nBucketCount;
	m_Destroy = Destroy;
	m_Copy = CopyFunc;

	m_ppBucket = new AVLTREENODE *[nBucketCount];
	m_pLock = new FASTLOCK[nBucketCount];
	
	int i;
	for( i = 0; i < nBucketCount; i++ )
	{
		m_ppBucket[i] = NULL;
		FastLockInit(&(m_pLock[i]));
	}
}

/**	CDHashAVLTree的析构函数

	@return	destructor - 无	
*/
CDHashAVLTree::~CDHashAVLTree()
{
	int i;
	for( i = 0; i < m_nBucketCount; i++ )
	{
		BinTreeBaseNode_Destroy(m_ppBucket[i], m_Destroy);
		FastLockClose(&(m_pLock[i]));
	}
	delete [] m_ppBucket;
	delete [] m_pLock;
}

/**	CDHashAVLTree的数据插入操作函数

	@param	void *pData - 要插入的数据	
	@return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败	
*/
INT CDHashAVLTree::Insert(void *pData)
{
	UINT	uIndex;
	INT     nRet;

	uIndex = (*m_HashFunc)( pData, m_nBucketCount );

	FastLockAcquire(&(m_pLock[uIndex]));

	nRet = AVLTreeNode_Insert(&(m_ppBucket[uIndex]), 
		pData, m_Compare);

	FastLockRelease(&(m_pLock[uIndex]));

	return nRet;
}


/**	CDHashAVLTree的数据删除操作函数

	@param	void *pData - 要删除的数据	
	@return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败	
*/
INT CDHashAVLTree::Delete(void *pData)
{
	UINT	uIndex;
	INT     nRet;

	uIndex = (*m_HashFunc)( pData, m_nBucketCount );

	FastLockAcquire(&(m_pLock[uIndex]));

	nRet = AVLTreeNode_Delete(&(m_ppBucket[uIndex]), 
		pData, m_Compare, m_Destroy);

	FastLockRelease(&(m_pLock[uIndex]));

	return nRet;
}

/**	CDHashAVLTree的数据查找操作函数

	@param	void *pData - 要查找的关键词数据	
	@return	void * - 成功时返回找到的数据指针，失败时返回NULL
					 返回的数据是原始数据的一个拷贝，因此需要用户用完后释放它	
*/
void * CDHashAVLTree::Find(void *pData)
{
	UINT	uIndex;
	void    *pRetData;

	uIndex = (*m_HashFunc)( pData, m_nBucketCount );

	FastLockAcquire(&(m_pLock[uIndex]));

	pRetData = BinTree_Find(m_ppBucket[uIndex], 
		pData, m_Compare);

	pRetData = (*m_Copy)(pRetData);

	FastLockRelease(&(m_pLock[uIndex]));

	return pRetData;
}

