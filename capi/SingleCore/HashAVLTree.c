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

#include <windows.h>
#include "capiglobal.h"
#include "BinTree.h"
#include "AVLTree.h"
#include "HashAVLTree.h"

/**	��ϣAVL���Ĵ�������

	@param	UINT uBucketCount - ��ϣAVL����BUCKET����	
	@return	HASHAVLTREE * - ʧ�ܷ���NULL,�ɹ����ش����Ĺ�ϣAVL��ָ��	
*/
HASHAVLTREE *HashAVLTree_Create(UINT uBucketCount)
{
    HASHAVLTREE	*pTree;
    
    if ( uBucketCount == 0 )
    {
        return NULL;
    }
    
    pTree = (HASHAVLTREE *)malloc( sizeof(HASHAVLTREE) );
    if ( pTree == NULL )
    {
        return NULL;
    }
    
    pTree->uNodeCount = 0;
    pTree->uBucketCount = uBucketCount;
    
    pTree->ppBucket = (AVLTREENODE **)malloc( uBucketCount 
        * sizeof(AVLTREENODE *));
    
    if (pTree->ppBucket == NULL)
    {
        free( pTree );
        return NULL;
    }
    
    memset(pTree->ppBucket, 0, uBucketCount * sizeof(AVLTREENODE *));
    
    pTree->pCurEntry = NULL;
    pTree->uCurBucketNo = 0;
    
    return pTree;
}

/**	��ϣAVL�����ͷź���
        ����ϣAVL�����������ݺͽڵ㼰������ϣAVL�������ͷŵ�

	@param	HASHAVLTREE *pHashAVLTree - Ҫ�ͷŵĹ�ϣAVL��ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
*/
void HashAVLTree_Destroy(HASHAVLTREE *pHashAVLTree, DESTROYFUNC DestroyFunc)
{
    AVLTREENODE **ppBucket;
    AVLTREENODE  *pNode;
    UINT i;
    
    if ( pHashAVLTree == NULL )
    {
        return;
    }
    ppBucket = pHashAVLTree->ppBucket;
    for ( i = 0; i < pHashAVLTree->uBucketCount; i++ ) 
    {
        pNode = ppBucket[i];
        BinTreeBaseNode_Destroy(pNode, DestroyFunc);
    }
    free(ppBucket);
    
    /* ��ppbucket��ɿ�ָ���Ա����ϣ������ʹ��ʱ����ڴ�й© */
    pHashAVLTree->ppBucket = NULL;
    
    free( pHashAVLTree );    
}


/**	��ϣAVL���Ĳ��뺯��

	@param	HASHAVLTREE *pHashAVLTree - ��ϣAVL��ָ��	
	@param	void *pData - Ҫ���������ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@return	INT - ʧ�ܷ���CAPI_FAILED,�ɹ�����CAPI_SUCCESS.	
*/
INT HashAVLTree_Insert(HASHAVLTREE *pHashAVLTree, void *pData, 
                       HASHFUNC HashFunc, COMPAREFUNC CompareFunc)
{
    UINT	uIndex;
    INT     nRet;
    
    if ( pHashAVLTree == NULL || pData == NULL || HashFunc == NULL )
    {
        return CAPI_FAILED;
    }
    
    uIndex = (*HashFunc)( pData, pHashAVLTree->uBucketCount );
    
    nRet = AVLTreeNode_Insert(&((pHashAVLTree->ppBucket)[uIndex]), 
        pData, CompareFunc);
	if ( nRet == CAPI_SUCCESS )
	{
		pHashAVLTree->uNodeCount += 1;
	}
    
    return nRet;
}

/**	��ϣAVL���Ľڵ�ɾ������

	@param	HASHAVLTREE *pHashAVLTree - ��ϣAVL��ָ��	
	@param	void *pData - ����ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	INT - ʧ�ܷ���CAPI_FAILED,�ɹ�����CAPI_SUCCESS.	
*/
INT HashAVLTree_Delete(HASHAVLTREE *pHashAVLTree, void *pData, 
                      HASHFUNC HashFunc, 
                      COMPAREFUNC CompareFunc,
                      DESTROYFUNC DestroyFunc)
{
    UINT	    uIndex;
    
    if ( pHashAVLTree == NULL || pData == NULL || HashFunc == NULL 
        || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    
    uIndex = (*HashFunc)( pData, pHashAVLTree->uBucketCount );

    if ( AVLTreeNode_Delete(&((pHashAVLTree->ppBucket)[uIndex]), pData, 
        CompareFunc, DestroyFunc) != CAPI_NOT_FOUND )
    {
        pHashAVLTree->uNodeCount -= 1;
    }

    return CAPI_SUCCESS;
}

/**	��ϣAVL���Ĳ��Һ���

	@param	HASHAVLTREE *pHashAVLTree - ��ϣAVL��ָ��	
	@param	void *pData - Ҫ���ҵ�����ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@return	void * - ʧ�ܷ���NULL���ɹ������ҵ��Ľڵ��е�����ָ��	
*/
void * HashAVLTree_Find(HASHAVLTREE *pHashAVLTree, void *pData, 
                       HASHFUNC HashFunc, 
                       COMPAREFUNC CompareFunc )
{
    UINT            uIndex;
    AVLTREENODE *   pNode;
    
    if ( pHashAVLTree == NULL || HashFunc == NULL || CompareFunc == NULL )
    {
        return NULL;
    }
    
    uIndex = (*HashFunc)( pData, pHashAVLTree->uBucketCount );
    pNode = (pHashAVLTree->ppBucket)[uIndex];

    return BinTree_Find(pNode, pData, CompareFunc);
}


/**	��ϣAVL��������ڵ������ʼ����

	@param	HASHAVLTREE *pHashAVLTree - ��ϣAVL��ָ��	
	@return	void - ��	
*/
void HashAVLTree_EnumBegin(HASHAVLTREE *pHashAVLTree)
{
    AVLTREENODE *pCursor;

    pHashAVLTree->uCurBucketNo = 0;
    pCursor = pHashAVLTree->ppBucket[0];
    
    if ( pCursor != NULL )
    {
        while ( pCursor->pLeft != NULL )
        {
            pCursor = pCursor->pLeft;
        }
    }
    pHashAVLTree->pCurEntry = pCursor;
}

/**	��ϣAVL��������ڵ��������

	@param	HASHAVLTREE *pHashAVLTree - ��ϣAVL��ָ��	
	@return	void * - ���ر����Ľڵ�����ָ�룬����������򷵻�NULL.	
*/
void *HashAVLTree_EnumNext(HASHAVLTREE *pHashAVLTree)
{
    void *pData;
    AVLTREENODE *pCursor;
    
    pCursor = pHashAVLTree->pCurEntry;
	if ( pCursor == NULL )
	{
		while ( pCursor == NULL )
		{
			pHashAVLTree->uCurBucketNo += 1;
			if ( pHashAVLTree->uCurBucketNo >= pHashAVLTree->uBucketCount )
			{
				return NULL;
			}
			pCursor = pHashAVLTree->ppBucket[pHashAVLTree->uCurBucketNo];
		}
    
		if ( pCursor == NULL )
		{
			return NULL;
		}

		while ( pCursor->pLeft != NULL )
		{
			pCursor = pCursor->pLeft;
		}
	}

	pData = pCursor->pData;
    
    if ( pCursor->pRight != NULL )
    {
        pCursor = pCursor->pRight;
        while ( pCursor->pLeft != NULL )
        {
            pCursor = pCursor->pLeft;
        }
    }
    else
    {
        AVLTREENODE *pParent = pCursor->pParent;
        while ( pParent != NULL && pParent->pRight == pCursor )
        {
            pCursor = pParent;
            pParent = pParent->pParent;
        }
        pCursor = pParent;
    }
    pHashAVLTree->pCurEntry = pCursor;
    
    return pData;    
}


