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
#include "CapiGlobal.h"
#include "BinTree.h"
#include "RBTree.h"
#include "HashRBTree.h"


/**	��ϣ������Ľڵ㴴������

	@param	void *pData - ����ָ��	
	@return	static HASHRBTREENODE * - �ɹ����ش����Ľڵ�ָ�룬ʧ�ܷ���NULL	
*/
static HASHRBTREENODE *HashRBTreeNode_Create( void *pData )
{
    HASHRBTREENODE *pNewNode;

    pNewNode = (HASHRBTREENODE *)malloc(sizeof(HASHRBTREENODE));
    if ( pNewNode != NULL )
    {
        RBTREENODE     *pTreeNode;

        pTreeNode = &(pNewNode->TreeNode);

        pTreeNode->pLeft = NULL;
        pTreeNode->pRight = NULL;
        pTreeNode->nMagic = RBTREE_COLOR_RED;
        pTreeNode->pData = pData;
        pTreeNode->pParent = NULL;

        pNewNode->pNext = NULL;
    }
    return pNewNode;
}

/**	��ϣ������Ĵ�������

	@param	UINT uBucketNum - ��ϣ���bucket����	
	@return	HASHRBTREE * - �ɹ����ع�ϣ�����ָ�룬ʧ�ܷ���NULL��	
*/
HASHRBTREE *HashRBTree_Create(UINT uBucketCount)
{
    HASHRBTREE  *pHashRBTree;

    if ( uBucketCount == 0 )
    {
        return NULL;
    }

    pHashRBTree = (HASHRBTREE *)malloc( sizeof(HASHRBTREE) );
    if ( pHashRBTree != NULL )
    {
        pHashRBTree->ppBucket = (HASHRBTREENODE **)malloc( uBucketCount 
            * sizeof(HASHRBTREENODE *) );
        if ( pHashRBTree->ppBucket != NULL )
        {
            pHashRBTree->pTree = RBTree_Create();
            if ( pHashRBTree->pTree == NULL )
            {
                free( pHashRBTree->ppBucket );
                free( pHashRBTree );
                pHashRBTree = NULL;
            }
            else
            {
                memset(pHashRBTree->ppBucket, 0, uBucketCount * sizeof(HASHRBTREENODE *));
                pHashRBTree->uBucketCount = uBucketCount;
            }
        }
        else
        {
            free( pHashRBTree );
            pHashRBTree = NULL;
        }
    }
    return pHashRBTree;
}

/**	��ϣ��������ͷź���

	@param	HASHRBTREE *pHashRBTree - ��ϣ�����ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷź���	
	@return	void - ��	
*/
void HashRBTree_Destroy(HASHRBTREE *pHashRBTree, DESTROYFUNC DestroyFunc)
{
    /* ��ϣ������У�����ֻҪ����������ͷŷ��������нڵ��ͷż��� */
    if ( pHashRBTree != NULL && pHashRBTree->pTree != NULL )
    {
        RBTree_Destroy( pHashRBTree->pTree, DestroyFunc );

        /* �ͷŹ�ϣ��ʱ�����ڽڵ��Ѿ����ͷ���, ��˲���Ҫ�ͷŽڵ� */
        free(pHashRBTree->ppBucket);
        free(pHashRBTree);
    }
}


/**	��ϣ������Ĳ��뺯��

	@param	HASHRBTREE *pHashRBTree - ��ϣ�����ָ��	
	@param	void *pData - ����ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@param	DESTROYFUNC DestroyFunc - �����ͷź���	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT HashRBTree_Insert(HASHRBTREE *pHashRBTree, void *pData, HASHFUNC HashFunc,
                      COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc)
{
    UINT        uIndex;

    INT nRet = CAPI_FAILED;
    if ( pHashRBTree != NULL )
    {
        HASHRBTREENODE  *pNewNode;

        pNewNode = (HASHRBTREENODE *)HashRBTreeNode_Create( pData );
        if ( pNewNode == NULL )
        {
            return CAPI_FAILED;
        }

        /* �Ƚ��ڵ���뵽������� */
        nRet = RBTree_Inter_Insert(pHashRBTree->pTree, 
            (RBTREENODE *)pNewNode, CompareFunc);
        if ( nRet == CAPI_SUCCESS )
        {
            HASHRBTREENODE  *pNode;

            /* �ں�����в���ɹ����ٽ�����뵽��ϣ���� */
            uIndex = (*HashFunc)( pData, pHashRBTree->uBucketCount );
            pNode = (HASHRBTREENODE *)(pHashRBTree->ppBucket[uIndex]);

	        /* �ڹ�ϣ���в��Ҷ�Ӧ�ڵ� */
            while ( pNode != NULL )
            {
                if ( (*CompareFunc)( (pNode->TreeNode).pData, pData) == 0 )
                {
			        /* ��ϣ���д�����ͬ���ݵĽڵ㣬���ڵ���������µ����ݸ��� */
                    (*DestroyFunc)( (pNode->TreeNode).pData );
                    (pNode->TreeNode).pData = pData;

                    return nRet;
		        }
                pNode = pNode->pNext;
	        }

            /* �������ɵĽڵ���뵽BUCKET��ͷ�� */
            pNode = (HASHRBTREENODE *)pHashRBTree->ppBucket[uIndex];
            pNewNode->pNext = pNode;
            pHashRBTree->ppBucket[uIndex] = (HASHRBTREENODE *)pNewNode;
        }
    }

    return nRet;
}

/**	��ϣ�������ɾ������

	@param	HASHRBTREE *pHashRBTree - ��ϣ�����ָ��	
	@param	void *pData - ����ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@param	DESTROYFUNC DestroyFunc - �����ͷź���	
	@return	INT - �ɹ�����CAPI_SUCCESS, ʧ�ܷ���CAPI_FAILED	
*/
INT HashRBTree_Delete(HASHRBTREE *pHashRBTree, void *pData, 
                      HASHFUNC HashFunc, 
                      COMPAREFUNC CompareFunc,
                      DESTROYFUNC DestroyFunc)
{
    UINT    uIndex;
    HASHRBTREENODE  *pNode;
    HASHRBTREENODE  *pPrevNode;

    uIndex = (*HashFunc)( pData, pHashRBTree->uBucketCount );
    pNode = (HASHRBTREENODE *)(pHashRBTree->ppBucket[uIndex]);
    pPrevNode = pNode;

    /* �ڹ�ϣ����ɾ����Ӧ�Ľڵ㣬ע��������Ϊ��Ҫ�ں������ɾ����Ӧ���ݵĽڵ㣬
     * ��˽ڵ��ڴ������ɾ�������ʱ�����ͷ�.
     */
    while ( pNode != NULL )
    {
        if ( (*CompareFunc)( (pNode->TreeNode).pData, pData ) == 0 )
        {
            if ( pPrevNode == pNode )
            {
                pHashRBTree->ppBucket[uIndex] = pNode->pNext;
            }
            else
            {
                pPrevNode->pNext = pNode->pNext;
            }

            break;
        }

        pPrevNode = pNode;
        pNode = pNode->pNext;
    }    

    /* �ں�����н���Ӧ�ڵ�ɾ�� */
    return RBTree_Delete(pHashRBTree->pTree, pData, CompareFunc, DestroyFunc);
}

/**	��ϣ������İ���������ҷ������в��ҵĺ���

	@param	HASHRBTREE *pHashRBTree - ��ϣ�����ָ��	
	@param	void *pData - Ҫ���ҵ�����	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	void * - �ɹ����ز��ҵ�������ָ�룬ʧ�ܷ���NULL	
*/
void * HashRBTree_HashFind(HASHRBTREE *pHashRBTree, void *pData, 
                       HASHFUNC HashFunc, COMPAREFUNC CompareFunc )
{
	UINT			    uIndex;
	HASHRBTREENODE *	pNode;

    /* ����У�� */
	if ( pHashRBTree == NULL || pData == NULL 
        || HashFunc == NULL || CompareFunc == NULL )
	{
		return NULL;
	}

	uIndex = (*HashFunc)( pData, pHashRBTree->uBucketCount );
	pNode = (HASHRBTREENODE *)(pHashRBTree->ppBucket[uIndex]);
	
	/* �ڹ�ϣ���ͻ���н��в��� */
	while ( pNode != NULL )
	{
		if ( (*CompareFunc)( (pNode->TreeNode).pData, pData) == 0 )
		{
			/* �ҵ��˶�Ӧ�����ݣ������ҵ�������ָ�� */
			return (pNode->TreeNode).pData;
		}
		pNode = pNode->pNext;
	}

    return NULL;
}


/**	��ϣ������İ���������ҷ������в��ҵĺ���

	@param	HASHRBTREE *pHashRBTree - ��ϣ�����ָ��	
	@param	void *pData - Ҫ���ҵ�����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	void * - �ɹ����ز��ҵ�������ָ�룬ʧ�ܷ���NULL	
*/
void * HashRBTree_TreeFind(HASHRBTREE *pHashRBTree, void *pData, 
                           COMPAREFUNC CompareFunc )
{
    return RBTree_Find(pHashRBTree->pTree, pData, CompareFunc);
}


/**	��ϣ�����������ڵ������ʼ������

	@param	HASHRBTREE *pHashRBTree - ��ϣ�����ָ��	
	@return	void - ��	
*/
void HashRBTree_EnumBegin(HASHRBTREE *pHashRBTree)
{
    RBTree_EnumBegin(pHashRBTree->pTree);
}


/**	��ϣ�����������ڵ�������������պ�����ı����������б���

	@param	HASHRBTREE *pHashRBTree - ��ϣ�����ָ��	
	@return	void * - ����ָ��	
*/
void *HashRBTree_EnumNext(HASHRBTREE *pHashRBTree)
{
    return RBTree_EnumNext(pHashRBTree->pTree);
}

