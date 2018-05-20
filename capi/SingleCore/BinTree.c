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

#include <stdio.h>
#include <stdlib.h>
#include "CapiGlobal.h"
#include "BinTree.h"

/**	�����������Ĵ�������

	@param	void - ��	
	@return	BINTREE * -�ɹ����ش����Ķ�����ָ�룬ʧ�ܷ���NULL��	
*/
BINTREE *BinTree_Create(void)
{
    BINTREE *pBinTree;

    pBinTree = (BINTREE *)malloc(sizeof(BINTREE));
    if ( pBinTree != NULL )
    {
        pBinTree->pRoot = NULL;
        pBinTree->uNodeCount = 0;
    }
    return pBinTree;
}

/**	���������������ͷź���

	@param	BINTREEBASENODE *pRoot -�������ڵ�ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
*/
void BinTreeBaseNode_Destroy(BINTREEBASENODE *pRoot, 
                             DESTROYFUNC DestroyFunc)
{
    if ( pRoot != NULL )
    {
        if ( pRoot->pLeft != NULL )
        {
            BinTreeBaseNode_Destroy(pRoot->pLeft, DestroyFunc);
        }
        if ( pRoot->pRight != NULL )
        {
            BinTreeBaseNode_Destroy(pRoot->pRight, DestroyFunc);
        }
        if ( DestroyFunc != NULL && pRoot->pData != NULL )
        {
            (*DestroyFunc)(pRoot->pData);
        }
        free(pRoot);
    }
}

/**	�����������ͷź���

	@param	BINTREE *pBinTree - ����������ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
*/
void BinTree_Destroy(BINTREE *pBinTree, DESTROYFUNC DestroyFunc)
{
    if ( pBinTree == NULL )
    {
        return;
    }
    BinTreeBaseNode_Destroy(pBinTree->pRoot, DestroyFunc);
    free( pBinTree );
}

/**	�������������ӽڵ㺯������ָ�����ݲ��뵽��������

	@param	BINTREE *pBinTree - ����������ָ��	
	@param	void *pData - Ҫ���ӵ�����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
*/
INT BinTree_Add(BINTREE *pBinTree, void *pData, COMPAREFUNC CompareFunc)
{
    BINTREEBASENODE *pNode;
    BINTREEBASENODE *pNewNode;
    INT         nRet = 0;

    if ( pBinTree == NULL || pData == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pBinTree->pRoot;

    while ( pNode != NULL )
    {
        nRet = (*CompareFunc)(pNode->pData, pData);
        if ( nRet < 0 )
        {
            if ( pNode->pRight != NULL )
            {
                pNode = pNode->pRight;
                continue;
            }
        }
        else 
        {
            if ( pNode->pLeft != NULL )
            {
                pNode = pNode->pLeft;
                continue;
            }
        }
        break;
    }

    pNewNode = (BINTREEBASENODE *)malloc(sizeof(BINTREEBASENODE));
    if ( pNewNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNewNode->pLeft = NULL;
    pNewNode->pRight = NULL;
    pNewNode->pData = pData;

    if ( pNode == NULL )
    {
        pBinTree->pRoot = pNewNode;
    }
    else 
    {
        if ( nRet < 0 )
        {
            pNode->pRight = pNewNode;
        }
        else
        {
            pNode->pLeft = pNewNode;
        }
    }
    pBinTree->uNodeCount += 1;
        
    return CAPI_SUCCESS;
}



/**	�����������Ĳ��Һ���

	@param	BINTREEBASENODE *pRoot - �����������ĸ��ڵ�ָ��	
	@param	void *pData - Ҫ���ҵ�����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@return	void * - �ɹ����ز��ҵ������ݣ�ʧ�ܷ���NULL��	
*/
void *BinTree_Find(BINTREEBASENODE *pRoot, void *pData, COMPAREFUNC CompareFunc)
{
    BINTREEBASENODE *pNode;
    
    pNode = pRoot;
    
    while ( pNode != NULL )
    {
        INT nRet = (*CompareFunc)(pNode->pData, pData);
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            return pNode->pData;
        }
    }
    return NULL;
}


/**	���������Һ���

	@param	BINTREEBASENODE *pRoot - ���ڵ�ָ��	
	@param	void *pData - ����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	BINTREEBASENODE * - ���ڵ�ָ��	
*/
BINTREEBASENODE *BinTree_FindNode(BINTREEBASENODE *pRoot, 
                              void *pData, COMPAREFUNC CompareFunc)
{
    BINTREEBASENODE *pNode;
    
    pNode = pRoot;
    
    while ( pNode != NULL )
    {
        INT nRet = (*CompareFunc)(pNode->pData, pData);
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            return pNode;
        }
    }
    return NULL;
}

/**	��������������ӽڵ㵽ָ���ڵ���������ĺ���

	@param	BINTREE *pBinTree - ����������ָ��	
	@param	void *pTagNodeData - ָ���ڵ�ָ��	
	@param	void *pData - Ҫ��ӵ�����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
*/
INT BinTree_AddLeft(BINTREE *pBinTree, void *pTagNodeData, 
                    void *pData, COMPAREFUNC CompareFunc)
{
    BINTREEBASENODE *pNode;
    INT         nRet;

    if ( pBinTree == NULL || pTagNodeData == NULL 
        || pData == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    /* ����Ҫ����Ľڵ� */
    pNode = pBinTree->pRoot;
    while ( pNode != NULL )
    {
        nRet = (*CompareFunc)(pNode->pData, pTagNodeData);
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            break;
        }
    }
    if ( pNode != NULL )
    {
        /* 
         *  �ҵ���Ҫ�����Ŀ��ڵ㣬�����½ڵ������Ϊ��������
         * ���ԭ�������������½ڵ�������ָ��ָ����
         */
        BINTREEBASENODE *pNewNode;
        pNewNode = (BINTREEBASENODE *)malloc(sizeof(BINTREEBASENODE));
        if ( pNewNode != NULL )
        {
            pNewNode->pData = pData;
            pNewNode->pLeft = pNode->pLeft;
            pNewNode->pRight = NULL;
            pNode->pLeft = pNewNode;
            return CAPI_SUCCESS;
        }
    }
    return CAPI_FAILED;
}

/**	��������������ӽڵ㵽ָ���ڵ���������ĺ���

	@param	BINTREE *pBinTree - ����������ָ��	
	@param	void *pTagNodeData - ָ���ڵ�ָ��	
	@param	void *pData - ����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
*/
INT BinTree_AddRight(BINTREE *pBinTree, void *pTagNodeData, 
                    void *pData, COMPAREFUNC CompareFunc)
{
    BINTREEBASENODE *pNode;
    INT         nRet;

    if ( pBinTree == NULL || pTagNodeData == NULL 
        || pData == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    /* ����Ҫ����Ľڵ� */
    pNode = pBinTree->pRoot;
    while ( pNode != NULL )
    {
        nRet = (*CompareFunc)(pNode->pData, pTagNodeData);
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            break;
        }
    }
    if ( pNode != NULL )
    {
        /* 
         *  �ҵ���Ҫ�����Ŀ��ڵ㣬�����½ڵ������Ϊ��������
         * ���ԭ�������������½ڵ�������ָ��ָ����
         */
        BINTREEBASENODE *pNewNode;
        pNewNode = (BINTREEBASENODE *)malloc(sizeof(BINTREEBASENODE));
        if ( pNewNode != NULL )
        {
            pNewNode->pData = pData;
            pNewNode->pLeft = NULL;
            pNewNode->pRight = pNode->pRight;
            pNode->pRight = pNewNode;
            return CAPI_SUCCESS;
        }
    }
    return CAPI_FAILED;
}

/**	������������ɾ������

	@param	BINTREE *pBinTree - ������ָ��	
	@param	void *pData - Ҫɾ��������ָ��	
	@param	COMPAREFUNC  CompareFunc - ���ݱȽϻص�����	
	@param	DESTROYFUNC  DestroyFunc - �����ͷŻص�����	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT BinTree_Delete(BINTREE *pBinTree, void *pData, 
                   COMPAREFUNC  CompareFunc,
                   DESTROYFUNC  DestroyFunc)
{
    BINTREEBASENODE *pNode;
    BINTREEBASENODE *pMaxNode;
    INT         nRet;

    if ( pBinTree == NULL ||pBinTree->pRoot == NULL
        || pData == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    pNode = pBinTree->pRoot;
    while ( pNode != NULL )
    {
        nRet = (*CompareFunc)(pNode->pData, pData); 
        if ( nRet < 0 )
        {
            pNode = pNode->pRight;
        }
        else if ( nRet > 0 )
        {
            pNode = pNode->pLeft;
        }
        else
        {
            break;
        }
    }

    if ( pNode == NULL )
    {
        /* δ�ҵ�ָ���ڵ� */
        return CAPI_FAILED;
    }

    /* ������ҵ���pNode�������ӽڵ����� */
    if ( pNode->pLeft != NULL && pNode->pRight != NULL )
    {
        pMaxNode = pNode->pLeft;
        while ( pMaxNode->pRight != NULL )
        {
            pMaxNode = pMaxNode->pRight;
        }
        if ( DestroyFunc != NULL && pNode->pData != NULL )
        {
            (*DestroyFunc)(pNode->pData);
        }
        pNode->pData = pMaxNode->pData;
        if ( pMaxNode == pNode->pLeft )
        {
            pNode->pLeft = pMaxNode->pLeft;
            if ( pMaxNode->pLeft != NULL )
            {
                pMaxNode->pLeft->pParent = pNode;
            }
        }
        else
        {
            pMaxNode->pParent->pRight = pMaxNode->pLeft;          
            if ( pMaxNode->pLeft != NULL )
            {
                pMaxNode->pLeft->pParent = pMaxNode->pParent;
            }
        }
        free(pMaxNode);

        return CAPI_SUCCESS;
    }

    /* �������ֻ��һ���ӽڵ����� */
    if ( pNode->pLeft != NULL )
    {
        pMaxNode = pNode->pLeft;
    }
    else
    {
        pMaxNode = pNode->pRight;
    }

    if ( pNode == pBinTree->pRoot )
    {
        pBinTree->pRoot = pMaxNode;
        if ( pMaxNode != NULL )
        {
            pMaxNode->pParent = NULL;
        }
    }
    else
    {
        if ( pNode->pParent->pLeft == pNode )
        {
            pNode->pParent->pLeft = pMaxNode;
        }
        else
        {
            pNode->pParent->pRight = pMaxNode;
        }
        if ( pMaxNode != NULL )
        {
            pMaxNode->pParent = pNode->pParent;
        }
    }
    if ( DestroyFunc != NULL && pNode->pData != NULL )
    {
        (*DestroyFunc)(pNode->pData);
    }
    free( pNode );

    return CAPI_SUCCESS;
}


/**	������������������������

	@param	BINTREEBASENODE *pANode - Ҫ��ת�Ļ��ڵ�	
	@param	BINTREEBASENODE **ppRoot - �����������ĸ��ڵ�	
	@return	void - ��	
*/
void BinTree_RotateLeft(BINTREEBASENODE *pANode, BINTREEBASENODE **ppRoot )
{
    BINTREEBASENODE  *pBNode; /* B�ڵ�ָ�� */
    
    pBNode = pANode->pRight;
    
    /* ��B�ڵ����ڵ���A�ڵ���ҽڵ� */
    pANode->pRight = pBNode->pLeft;
    if ( pBNode->pLeft != NULL )
    {
        pBNode->pLeft->pParent = pANode;
    }
    
    /* �޸�A�ڵ�ĸ�ָ���B�ڵ�Ĺ�ϵ */
    pBNode->pParent = pANode->pParent;
    if ( pANode == *ppRoot )
    {
        *ppRoot = pBNode;
    }
    else if ( pANode == pANode->pParent->pLeft )
    {
        pANode->pParent->pLeft = pBNode;
    }
    else
    {
        pANode->pParent->pRight = pBNode;
    }
    
    /* ��A�ڵ���B�ڵ����ڵ� */
    pBNode->pLeft = pANode;
    pANode->pParent = pBNode;
}


/**	������������������������

	@param	BINTREEBASENODE *pANode - Ҫ��ת�Ļ��ڵ�	
	@param	BINTREEBASENODE **ppRoot - �����������ĸ��ڵ�	
	@return	void - ��	
*/
void BinTree_RotateRight(BINTREEBASENODE *pANode, BINTREEBASENODE **ppRoot)
{
    BINTREEBASENODE  *pBNode; /* B�ڵ�ָ�� */
    
    pBNode = pANode->pLeft;
    
    /* ��B�ڵ���ҽڵ���A�ڵ����ڵ� */
    pANode->pLeft = pBNode->pRight;
    if ( pBNode->pRight != NULL )
    {
        pBNode->pRight->pParent = pANode;
    }
    
    /* �޸�A�ڵ�ĸ�ָ���B�ڵ�Ĺ�ϵ */
    pBNode->pParent = pANode->pParent;
    if ( pANode == *ppRoot )
    {
        *ppRoot = pBNode;
    }
    else if ( pANode == pANode->pParent->pRight )
    {
        pANode->pParent->pRight = pBNode;
    }
    else
    {
        pANode->pParent->pLeft = pBNode;
    }
    
    /* ��A�ڵ���B�ڵ����ڵ� */
    pBNode->pRight = pANode;
    pANode->pParent = pBNode;
}

/**	������������У�鸸�ڵ����Ӻ���

	@param	BINTREEBASENODE *pRootNode - ���ڵ�ָ��	
	@return	void - ��	
*/
void BinTree_CheckParent(BINTREEBASENODE *pRootNode)
{
    BINTREEBASENODE *pNode = pRootNode;

    if ( pNode == NULL )
    {
        return;
    }

    if ( pNode->pLeft != NULL )
    {
        if ( pNode->pLeft->pParent != pNode )
        {
            printf( "%s Node's Parent is no correct!!!\n", (char *)(pNode->pLeft->pData) );
        }
        BinTree_CheckParent(pNode->pLeft);
    }

    if ( pNode->pRight != NULL )
    {
        if ( pNode->pRight->pParent != pNode )
        {
            printf( "%s Node's Parent is no correct!!!\n", (char *)(pNode->pRight->pData) );
        }
        BinTree_CheckParent(pNode->pRight);
    }
}

/**	�����������Ĳ����������

	@param	BINTREEBASENODE **pRoot - ָ����ڵ�ָ���ָ��	
	@param	void *pData - Ҫ���������ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@param	INT nMagic - ħ��ֵ���ɵ����߾���	
	@return	INT - CAPI_FAILED��ʾ����ʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT BinTree_Insert(BINTREEBASENODE **pRoot, void *pData, COMPAREFUNC CompareFunc, INT nMagic)
{
    BINTREEBASENODE *pNode;
    BINTREEBASENODE *pNewNode;
    INT         nRet = 0;
    
    if ( pData == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    
    pNode = *pRoot;
    
    while ( pNode != NULL )
    {
        nRet = (*CompareFunc)(pNode->pData, pData);
        if ( nRet < 0 )
        {
            if ( pNode->pRight != NULL )
            {
                pNode = pNode->pRight;
                continue;
            }
        }
        else 
        {
            if ( pNode->pLeft != NULL )
            {
                pNode = pNode->pLeft;
                continue;
            }
        }
        break;
    }
    
    pNewNode = (BINTREEBASENODE *)malloc(sizeof(BINTREEBASENODE));
    if ( pNewNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNewNode->pLeft = NULL;
    pNewNode->pRight = NULL;
    pNewNode->pData = pData;
    pNewNode->nMagic = nMagic;
    
    if ( pNode == NULL )
    {
        *pRoot = pNewNode;
        pNewNode->pParent = NULL;
    }
    else 
    {
        if ( nRet < 0 )
        {
            pNode->pRight = pNewNode;
        }
        else
        {
            pNode->pLeft = pNewNode;
        }
        pNewNode->pParent = pNode;
    }
    
    return CAPI_SUCCESS;
}


/**	��������ǰ���������

	@param	BINTREE *pTree - Ҫ�����Ķ�����	
	@param	VISITFUNC VisitFunc - �ڵ���ʻص�����	
	@return	void - ��	
*/
void BinTree_PreOrderTraverse(BINTREE *pTree, VISITFUNC VisitFunc)
{
    BINTREEBASENODE *pCursor;
    
    pCursor = pTree->pRoot;
    if ( pCursor == NULL )
    {
        return;
    }
    (void)(*VisitFunc)(pCursor->pData);
    
    while ( pCursor->pLeft != NULL )
    {
        pCursor = pCursor->pLeft;
        (void)(*VisitFunc)(pCursor->pData);
    }
    
    while ( pCursor != NULL )
    {      
        if ( pCursor->pRight != NULL )
        {
            pCursor = pCursor->pRight;
            (void)(*VisitFunc)(pCursor->pData);
            while ( pCursor->pLeft != NULL )
            {
                pCursor = pCursor->pLeft;
                (void)(*VisitFunc)(pCursor->pData);
            }
        }
        else
        {
            /* ���ݵ����ڵ� */
            BINTREEBASENODE *pParent = pCursor->pParent;
            while ( pParent != NULL && pParent->pRight == pCursor )
            {
                pCursor = pParent;
                pParent = pParent->pParent;
            }
            pCursor = pParent;
        }
    }
}


/**	�������������������

	@param	BINTREE *pTree - Ҫ�����Ķ�����	
	@param	VISITFUNC VisitFunc - �ڵ���ʻص�����	
	@return	void - ��	
*/
void BinTree_InOrderTraverse(BINTREE *pTree, VISITFUNC VisitFunc)
{
    BINTREEBASENODE *pCursor;
    void        *pData;
    
    pCursor = pTree->pRoot;
    if ( pCursor == NULL )
    {
        return;
    }
    while ( pCursor->pLeft != NULL )
    {
        pCursor = pCursor->pLeft;
    }

    while ( pCursor != NULL )
    {
        pData = pCursor->pData;
        (void)(*VisitFunc)(pData);
    
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
            /* ���ݵ����ڵ� */
            BINTREEBASENODE *pParent = pCursor->pParent;
            while ( pParent != NULL && pParent->pRight == pCursor )
            {
                pCursor = pParent;
                pParent = pParent->pParent;
            }
            pCursor = pParent;
        }
    }
}


/**	�������ĺ����������

	@param	BINTREE *pTree - Ҫ�����Ķ�����	
	@param	VISITFUNC VisitFunc - �ڵ���ʻص�����	
	@return	void - ��	
*/
void BinTree_PostOrderTraverse(BINTREE *pTree, VISITFUNC VisitFunc)
{
    BINTREEBASENODE *pCursor;
    
    pCursor = pTree->pRoot;
    if ( pCursor == NULL )
    {
        return;
    }
    
    while ( pCursor->pLeft != NULL )
    {
        pCursor = pCursor->pLeft;
    }
    
    while ( pCursor != NULL )
    {      
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

            /* ���ݵ����ڵ� */
            BINTREEBASENODE *pParent = pCursor->pParent;               
            
            (void)(*VisitFunc)(pCursor->pData);

            while ( pParent != NULL && pParent->pRight == pCursor )
            {
                pCursor = pParent;
                pParent = pParent->pParent;

                (void)(*VisitFunc)(pCursor->pData);
            }
            pCursor = pParent;
        }
    }
}

