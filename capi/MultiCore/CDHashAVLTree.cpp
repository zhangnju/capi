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


/**	CDHashAVLTree�Ĺ��캯��

	@param	int nBucketCount - Ͱ������	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC Compare - ���ݱȽϺ���	
	@param	DESTROYFUNC Destroy - �����ͷź���	
	@param	COPYFUNC CopyFunc - ���ݿ�������	
	@return	constructor - ��	
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

/**	CDHashAVLTree����������

	@return	destructor - ��	
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

/**	CDHashAVLTree�����ݲ����������

	@param	void *pData - Ҫ���������	
	@return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ��	
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


/**	CDHashAVLTree������ɾ����������

	@param	void *pData - Ҫɾ��������	
	@return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ��	
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

/**	CDHashAVLTree�����ݲ��Ҳ�������

	@param	void *pData - Ҫ���ҵĹؼ�������	
	@return	void * - �ɹ�ʱ�����ҵ�������ָ�룬ʧ��ʱ����NULL
					 ���ص�������ԭʼ���ݵ�һ�������������Ҫ�û�������ͷ���	
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

