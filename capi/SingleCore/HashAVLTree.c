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

/**	哈希AVL树的创建函数

	@param	UINT uBucketCount - 哈希AVL树中BUCKET数量	
	@return	HASHAVLTREE * - 失败返回NULL,成功返回创建的哈希AVL树指针	
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

/**	哈希AVL树的释放函数
        将哈希AVL树中所有数据和节点及整个哈希AVL树整体释放掉

	@param	HASHAVLTREE *pHashAVLTree - 要释放的哈希AVL树指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
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
    
    /* 将ppbucket设成空指针以避免哈希表被重新使用时造成内存泄漏 */
    pHashAVLTree->ppBucket = NULL;
    
    free( pHashAVLTree );    
}


/**	哈希AVL树的插入函数

	@param	HASHAVLTREE *pHashAVLTree - 哈希AVL树指针	
	@param	void *pData - 要插入的数据指针	
	@param	HASHFUNC HashFunc - 哈希函数	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	INT - 失败返回CAPI_FAILED,成功返回CAPI_SUCCESS.	
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

/**	哈希AVL树的节点删除函数

	@param	HASHAVLTREE *pHashAVLTree - 哈希AVL树指针	
	@param	void *pData - 数据指针	
	@param	HASHFUNC HashFunc - 哈希函数	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	INT - 失败返回CAPI_FAILED,成功返回CAPI_SUCCESS.	
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

/**	哈希AVL树的查找函数

	@param	HASHAVLTREE *pHashAVLTree - 哈希AVL树指针	
	@param	void *pData - 要查找的数据指针	
	@param	HASHFUNC HashFunc - 哈希函数	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	void * - 失败返回NULL，成功返回找到的节点中的数据指针	
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


/**	哈希AVL树的逐个节点遍历开始函数

	@param	HASHAVLTREE *pHashAVLTree - 哈希AVL树指针	
	@return	void - 无	
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

/**	哈希AVL树的逐个节点遍历函数

	@param	HASHAVLTREE *pHashAVLTree - 哈希AVL树指针	
	@return	void * - 返回遍历的节点数据指针，如果遍历完则返回NULL.	
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


