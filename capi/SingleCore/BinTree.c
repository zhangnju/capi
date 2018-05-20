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

/**	二叉搜索树的创建函数

	@param	void - 无	
	@return	BINTREE * -成功返回创建的二叉树指针，失败返回NULL。	
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

/**	二叉搜索树子树释放函数

	@param	BINTREEBASENODE *pRoot -子树根节点指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
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

/**	二叉搜索树释放函数

	@param	BINTREE *pBinTree - 二叉搜索树指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
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

/**	二叉搜索树增加节点函数，将指定数据插入到二叉树中

	@param	BINTREE *pBinTree - 二叉搜索树指针	
	@param	void *pData - 要增加的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
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



/**	二叉搜索树的查找函数

	@param	BINTREEBASENODE *pRoot - 二叉搜索树的根节点指针	
	@param	void *pData - 要查找的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	void * - 成功返回查找到的数据，失败返回NULL。	
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


/**	二叉树查找函数

	@param	BINTREEBASENODE *pRoot - 根节点指针	
	@param	void *pData - 数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	BINTREEBASENODE * - 根节点指针	
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

/**	二叉搜索树的添加节点到指定节点的左子树的函数

	@param	BINTREE *pBinTree - 二叉搜索树指针	
	@param	void *pTagNodeData - 指定节点指针	
	@param	void *pData - 要添加的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
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
    /* 查找要插入的节点 */
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
         *  找到了要插入的目标节点，生成新节点插入作为其左子树
         * 如果原来有左子树则将新节点左子树指针指向它
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

/**	二叉搜索树的添加节点到指定节点的右子树的函数

	@param	BINTREE *pBinTree - 二叉搜索树指针	
	@param	void *pTagNodeData - 指定节点指针	
	@param	void *pData - 数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
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
    /* 查找要插入的节点 */
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
         *  找到了要插入的目标节点，生成新节点插入作为其左子树
         * 如果原来有左子树则将新节点左子树指针指向它
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

/**	二叉搜索树的删除函数

	@param	BINTREE *pBinTree - 二叉树指针	
	@param	void *pData - 要删除的数据指针	
	@param	COMPAREFUNC  CompareFunc - 数据比较回调函数	
	@param	DESTROYFUNC  DestroyFunc - 数据释放回调函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功	
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
        /* 未找到指定节点 */
        return CAPI_FAILED;
    }

    /* 处理查找到的pNode有两个子节点的情况 */
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

    /* 处理最多只有一个子节点的情况 */
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


/**	二叉排序树的左旋操作函数

	@param	BINTREEBASENODE *pANode - 要旋转的基节点	
	@param	BINTREEBASENODE **ppRoot - 二叉排序树的根节点	
	@return	void - 无	
*/
void BinTree_RotateLeft(BINTREEBASENODE *pANode, BINTREEBASENODE **ppRoot )
{
    BINTREEBASENODE  *pBNode; /* B节点指针 */
    
    pBNode = pANode->pRight;
    
    /* 将B节点的左节点变成A节点的右节点 */
    pANode->pRight = pBNode->pLeft;
    if ( pBNode->pLeft != NULL )
    {
        pBNode->pLeft->pParent = pANode;
    }
    
    /* 修改A节点的父指针和B节点的关系 */
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
    
    /* 将A节点变成B节点的左节点 */
    pBNode->pLeft = pANode;
    pANode->pParent = pBNode;
}


/**	二叉排序树的右旋操作函数

	@param	BINTREEBASENODE *pANode - 要旋转的基节点	
	@param	BINTREEBASENODE **ppRoot - 二叉排序树的根节点	
	@return	void - 无	
*/
void BinTree_RotateRight(BINTREEBASENODE *pANode, BINTREEBASENODE **ppRoot)
{
    BINTREEBASENODE  *pBNode; /* B节点指针 */
    
    pBNode = pANode->pLeft;
    
    /* 将B节点的右节点变成A节点的左节点 */
    pANode->pLeft = pBNode->pRight;
    if ( pBNode->pRight != NULL )
    {
        pBNode->pRight->pParent = pANode;
    }
    
    /* 修改A节点的父指针和B节点的关系 */
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
    
    /* 将A节点变成B节点的左节点 */
    pBNode->pRight = pANode;
    pANode->pParent = pBNode;
}

/**	二叉搜索树的校验父节点链接函数

	@param	BINTREEBASENODE *pRootNode - 根节点指针	
	@return	void - 无	
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

/**	二叉排序树的插入操作函数

	@param	BINTREEBASENODE **pRoot - 指向根节点指针的指针	
	@param	void *pData - 要插入的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@param	INT nMagic - 魔法值，由调用者决定	
	@return	INT - CAPI_FAILED表示插入失败，CAPI_SUCCESS表示成功	
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


/**	二叉树的前序遍历函数

	@param	BINTREE *pTree - 要操作的二叉树	
	@param	VISITFUNC VisitFunc - 节点访问回调函数	
	@return	void - 无	
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
            /* 回溯到父节点 */
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


/**	二叉树的中序遍历函数

	@param	BINTREE *pTree - 要操作的二叉树	
	@param	VISITFUNC VisitFunc - 节点访问回调函数	
	@return	void - 无	
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
            /* 回溯到父节点 */
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


/**	二叉树的后序遍历函数

	@param	BINTREE *pTree - 要操作的二叉树	
	@param	VISITFUNC VisitFunc - 节点访问回调函数	
	@return	void - 无	
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

            /* 回溯到父节点 */
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

