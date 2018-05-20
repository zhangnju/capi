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

#pragma warning(disable : 4996)


/**	红黑树的创建函数

	@param	void - 无	
	@return	RBTREE * - 成功返回创建的红黑树指针，
                       失败返回NULL。
*/
RBTREE *RBTree_Create(void)
{
    RBTREE *pTree;

    pTree = (RBTREE *)malloc(sizeof(RBTREE));
    if ( pTree != NULL )
    {
        pTree->pRoot = NULL;
        pTree->uNodeCount = 0;
        pTree->pCursor = NULL;
    }

    return pTree;
}

/**	红黑树节点创建函数

	@param	void *pData - 节点数据指针	
	@return	static void * - 返回创建的节点指针	
*/
static void *RBTreeNode_Create(void *pData)
{
    RBTREENODE *pNewNode;

    pNewNode = (RBTREENODE *)malloc(sizeof(RBTREENODE));
    if ( pNewNode != NULL )
    {
        pNewNode->pLeft= NULL;
        pNewNode->pRight = NULL;
        pNewNode->nMagic = RBTREE_COLOR_RED;
        pNewNode->pData = pData;
    }
    return (void *)pNewNode;
}

/**	红黑树的释放函数，释放以指定节点为根节点的子树

	@param	RBTREENODE *pNode - 要释放的根节点	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	static void - 无	
*/
static void RBTreeNode_Destroy(RBTREENODE *pNode, DESTROYFUNC DestroyFunc)
{
    if ( pNode != NULL )
    {
        if ( pNode->pLeft != NULL )
        {
            RBTreeNode_Destroy(pNode->pLeft, DestroyFunc);
        }
        if ( pNode->pRight != NULL )
        {
            RBTreeNode_Destroy(pNode->pRight, DestroyFunc);
        }
        if ( DestroyFunc != NULL && pNode->pData != NULL )
        {
            (*DestroyFunc)(pNode->pData);
        }
        free(pNode);
    }
}

/**	红黑树的释放函数

	@param	RBTREE *pTree - 红黑树指针	
	@param	DESTROYFUNC DestroyFunc -xxx	
	@return	void -xxxx	
*/
void RBTree_Destroy(RBTREE *pTree, DESTROYFUNC DestroyFunc)
{
    if ( pTree != NULL )
    {
        RBTreeNode_Destroy(pTree->pRoot, DestroyFunc);
        free( pTree );
    }
}

/**	红黑树的左旋操作，旋转前A节点和它的右节点B均存在
    旋转后A节点的右指针指向B节点的左节点，B节点的左指针指向A节点，

	@param	RBTREE *pTree -红黑树指针	
	@param	RBTREENODE *pANode -要旋转的节点	
	@return	static void -无	
*/
static void RBTree_RotateLeft(RBTREE *pTree, RBTREENODE *pANode)
{
    RBTREENODE  *pBNode; /* B节点指针 */

    pBNode = pANode->pRight;

    /* 将B节点的左节点变成A节点的右节点 */
    pANode->pRight = pBNode->pLeft;
    if ( pBNode->pLeft != NULL )
    {
        pBNode->pLeft->pParent = pANode;
    }

    /* 修改A节点的父指针和B节点的关系 */
    pBNode->pParent = pANode->pParent;
    if ( pANode == pTree->pRoot )
    {
        pTree->pRoot = pBNode;
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


/**	红黑树的右旋操作，旋转前A节点和它的左节点B均存在
    旋转后A节点的左指针指向B节点的右节点，B节点的右指针指向A节点，

	@param	RBTREE *pTree -红黑树指针	
	@param	RBTREENODE *pANode -要旋转的节点	
	@return	static void -无	
*/
static void RBTree_RotateRight(RBTREE *pTree, RBTREENODE *pANode)
{
    RBTREENODE  *pBNode; /* B节点指针 */

    pBNode = pANode->pLeft;

    /* 将B节点的右节点变成A节点的左节点 */
    pANode->pLeft = pBNode->pRight;
    if ( pBNode->pRight != NULL )
    {
        pBNode->pRight->pParent = pANode;
    }

    /* 修改A节点的父指针和B节点的关系 */
    pBNode->pParent = pANode->pParent;
    if ( pANode == pTree->pRoot )
    {
        pTree->pRoot = pBNode;
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

/**	红黑树的为插入操作调整颜色函数，

	@param	RBTREE *pTree - 红黑树指针	
	@param	RBTREENODE *pANode - 插入的节点指针	
	@return	static void - 无	
*/
static void RBTree_AdjustColorForInsert(RBTREE *pTree, RBTREENODE *pANode)
{
    /* 根据红黑树的算法，需要先将插入的A节点颜色预设成红色 */
    pANode->nMagic = RBTREE_COLOR_RED;

    while ( pANode != pTree->pRoot 
        && pANode->pParent->nMagic == RBTREE_COLOR_RED )
    {
        if ( pANode->pParent == pANode->pParent->pParent->pLeft )
        {
            /* 父节点为祖父节点左节点的情况 */

            RBTREENODE  *pNode = pANode->pParent->pParent->pRight;
            if ( pNode != NULL && pNode->nMagic == RBTREE_COLOR_RED )
            {
                /* LLr和LRr类型，直接修改颜色然后再继续循环调整祖父节点颜色 */
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pNode->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                /* 由于祖父节点颜色被改变成了红色，因此祖父节点和它的父节点
                 * 有可能都是红色，这里需要继续循环来矫正祖父节点的颜色平衡情况 
                 */
                pANode = pANode->pParent->pParent;
            }
            else
            {
                /* LLb和LRb类型，直接通过旋转操作即可矫正颜色 */
                if ( pANode == pANode->pParent->pRight )
                {
                    pANode = pANode->pParent;
                    RBTree_RotateLeft(pTree, pANode);
                }
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                RBTree_RotateRight(pTree, pANode->pParent->pParent);

                /* 这里由于A节点的父节点颜色是黑色的，实际上会退出循环
                 * 因此如果要优化代码大小的话, break 语句可以省略掉 
                 */
                break;
            }
        }
        else
        {
            /* 父节点为祖父节点右节点的情况 */

            RBTREENODE  *pNode = pANode->pParent->pParent->pLeft;
            if ( pNode != NULL && pNode->nMagic == RBTREE_COLOR_RED )
            {
                /* RLr和RRr类型，直接修改颜色然后再继续循环调整祖父节点颜色 */
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pNode->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                /* 由于祖父节点颜色被改变成了红色，因此祖父节点和它的父节点
                 * 有可能都是红色，这里需要继续循环来矫正祖父节点的颜色平衡情况 
                 */
                pANode = pANode->pParent->pParent;
            }
            else
            {
                /* RLb和RRb类型，直接通过旋转操作即可矫正颜色 */
                if ( pANode == pANode->pParent->pLeft )
                {
                    pANode = pANode->pParent;
                    RBTree_RotateRight(pTree, pANode);
                }
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                RBTree_RotateLeft(pTree, pANode->pParent->pParent);

                /* 这里由于A节点的父节点颜色是黑色的，实际上会退出循环
                 * 因此如果要优化代码大小的话, break 语句可以省略掉 
                 */
                break;
            }
        }
    }

    /* 在上面的颜色矫正过程中，根节点颜色可能会被改成红色 
     * 因此需要将根节点颜色重新赋为黑色即可
     */
    pTree->pRoot->nMagic = RBTREE_COLOR_BLACK;
}

/**	红黑树的插入函数

	@param	RBTREE *pTree - 红黑树指针	
	@param	void *pData - 要插入的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功。	
*/
INT RBTree_Insert(RBTREE *pTree, void *pData, COMPAREFUNC CompareFunc)
{
    INT     nRet;
    RBTREENODE * pNode;
    
    nRet = CAPI_FAILED;

    pNode = RBTreeNode_Create(pData);
    if ( pNode != NULL )
    {
        nRet = RBTree_Inter_Insert(pTree, pNode, CompareFunc);
    }
    return nRet;
}

/**	红黑树的插入函数，主要为需要继承的模块而使用，
    一般的红黑树应用模块请不要调用这个函数

	@param	RBTREE *pTree - 红黑树指针
	@param	void *pData - 数据指针	
	@param	COMPAREFUNC CompareFunc - 比较函数	
	@param	void *pTreeNode - 插入的节点	
	@return	INT -CAPI_SUCCESS表示成功	
*/
INT RBTree_Inter_Insert(RBTREE *pTree,  RBTREENODE *pNewNode, 
                        COMPAREFUNC CompareFunc)
{
    RBTREENODE  *pNode;
    RBTREENODE  *pParentNode;
    INT         nRet = 0;

    pParentNode = NULL;
    pNode = pTree->pRoot;

    while ( pNode != NULL )
    {
        pParentNode = pNode;
        nRet = (*CompareFunc)(pNode->pData, pNewNode->pData);
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
            return CAPI_FAILED;
        }
    }

    if ( pParentNode == NULL )
    {
        /* 树为空的情况 */
        pTree->pRoot = pNewNode;
        pNewNode->pParent = NULL;
    }
    else 
    {
        if ( nRet < 0 )
        {
            pParentNode->pRight = pNewNode;
        }
        else 
        {
            pParentNode->pLeft = pNewNode;
        }
        pNewNode->pParent = pParentNode;
    }

    pTree->uNodeCount += 1;
        
    RBTree_AdjustColorForInsert(pTree, pNewNode);

    return CAPI_SUCCESS;
}

/**	红黑树的查找函数

	@param	RBTREE *pTree - 红黑树指针	
	@param	void *pData - 要查找的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	void * - 成功返回查找到的数据指针，失败返回NULL	
*/
void *RBTree_Find(RBTREE *pTree, void *pData, COMPAREFUNC CompareFunc)
{
    RBTREENODE *pNode;

    if ( pTree == NULL || pData == NULL || CompareFunc == NULL )
    {
        return NULL;
    }
    
    pNode = pTree->pRoot;
    
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


/**	红黑树删除操作的Rr-rr和Rr-br型的旋转
    这个函数的操作相当于将两次右旋和一次左旋操作合并到一个函数里
    合并后的计算量相当于两次旋转的计算量。

    如果A为基准节点，则B为A的左节点，C为B的右节点
    先对C作左旋，再对B左旋，再对A进行右旋而成。

	@param	RBTREE *pTree - 红黑树指针	
	@param	RBTREENODE *pStartNode - 要旋转的基准节点	
	@return	void - 无	
*/
void RBTree_RotateLeftLeftRight(RBTREE *pTree, RBTREENODE *pStartNode)
{
    RBTREENODE  *pParentNode;
    RBTREENODE  *pBNode;
    RBTREENODE  *pCNode;
    RBTREENODE  *pDNode;

    pParentNode = pStartNode;
    pBNode = pParentNode->pLeft;
    pCNode = pBNode->pRight;
    pDNode = pCNode->pRight;

    if ( pParentNode->pParent == NULL )
    {
        pTree->pRoot = pDNode;
    }
    else if ( pParentNode->pParent->pLeft == pParentNode )
    {
        pParentNode->pParent->pLeft = pDNode;
    }
    else
    {
        pParentNode->pParent->pRight = pDNode;
    }
    pDNode->pParent = pParentNode->pParent;

    pCNode->pRight = pDNode->pLeft;
    if ( pDNode->pLeft != NULL )
    {
        pDNode->pLeft->pParent = pCNode;
    }

    pParentNode->pLeft = pDNode->pRight;
    if ( pDNode->pRight != NULL )
    {
        pDNode->pRight->pParent = pParentNode;
    }

    pDNode->pLeft = pBNode;
    pBNode->pParent = pDNode;

    pDNode->pRight = pParentNode;
    pParentNode->pParent = pDNode;
}

/**	红黑树删除操作的Lr-rr和Lr-rb型的旋转
    两次左旋和一次右旋操作合并到一个函数里

    如果A为基准节点，则B为A的右节点，C为B的左节点
    先对C作右旋，再对B右旋，再对A进行左旋而成。

    @param	RBTREE *pTree - 红黑树指针	
	@param	RBTREENODE *pStartNode - 要旋转的基准节点	
	@return	void - 无	
*/
static void RBTree_RotateRightRightLeft(RBTREE *pTree, RBTREENODE *pStartNode)
{
    RBTREENODE  *pParentNode;
    RBTREENODE  *pBNode;
    RBTREENODE  *pCNode;
    RBTREENODE  *pDNode;

    pParentNode = pStartNode;
    pBNode = pParentNode->pRight;
    pCNode = pBNode->pLeft;
    pDNode = pCNode->pLeft;

    /* 修改使D节点成为A节点祖父节点的子节点，即节点替代祖父节点位置 */
    if ( pParentNode->pParent == NULL )
    {
        pTree->pRoot = pDNode;
    }
    else if ( pParentNode->pParent->pLeft == pParentNode )
    {
        pParentNode->pParent->pLeft = pDNode;
    }
    else
    {
        pParentNode->pParent->pRight = pDNode;
    }
    pDNode->pParent = pParentNode->pParent;

    /* D节点的右节点变成C节点的左节点 */
    pCNode->pLeft = pDNode->pRight;
    if ( pDNode->pRight != NULL )
    {
        pDNode->pRight->pParent = pCNode;
    }

    /* D节点的左节点变成父节点的右节点 */
    pParentNode->pRight = pDNode->pLeft;
    if ( pDNode->pLeft != NULL )
    {
        pDNode->pLeft->pParent = pParentNode;
    }

    /* B节点变成D节点的右节点 */
    pDNode->pRight = pBNode;
    pBNode->pParent = pDNode;

    /* 父节点变成D节点的左节点 */
    pDNode->pLeft = pParentNode;
    pParentNode->pParent = pDNode;
}



/**	红黑树的删除操作中对不平衡节点的调整操作

	@param	RBTREE *pTree - 红黑树指针	
	@param	RBTREENODE *pParentNode - 要调整平衡节点的父节点	
	@param	RBTREENODE *pReplaceNode - 要平衡的节点	
	@return	static void - 无	
*/
static void RBTree_AdjustColorForDelete(RBTREE *pTree, RBTREENODE *pParentNode, 
                                 RBTREENODE *pReplaceNode)
{
    RBTREENODE  *pANode;       /* 需要调整平衡的节点 */
    RBTREENODE  *pAParentNode; /* A节点的父节点 */
    RBTREENODE  *pBNode;       /* pAParentNode的另一子节点 */
    RBTREENODE  *pCNode;       /* 临时变量用来记录B节点的一个子节点 */

    pANode = pReplaceNode;

    pAParentNode = pParentNode;

    while ( pANode != pTree->pRoot 
            && ( pANode == NULL || pANode->nMagic == RBTREE_COLOR_BLACK) )
    {
        if ( pANode == pAParentNode->pLeft )
        {
            pBNode = pAParentNode->pRight;
            if ( pBNode->nMagic == RBTREE_COLOR_RED )
            {
                /* Lr型 */
                pCNode = pBNode->pLeft;
                if ( (pCNode->pLeft == NULL 
                    || pCNode->pLeft->nMagic == RBTREE_COLOR_BLACK)
                    && (pCNode->pRight == NULL 
                    || pCNode->pRight->nMagic == RBTREE_COLOR_BLACK) )
                {
                    /* Lr-bb型，即C节点左右节点都是黑色的情况 */
                    pCNode->nMagic = RBTREE_COLOR_RED;
                    pBNode->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeft(pTree, pAParentNode);
                }  
                else if ( pCNode->pRight != NULL 
                    && pCNode->pRight->nMagic == RBTREE_COLOR_RED 
                    && ( pCNode->pLeft == NULL 
                    || pCNode->pLeft->nMagic == RBTREE_COLOR_BLACK) )
                {
                    /* Lr-br型，即C节点左节点为黑色，右节点为红色的情况 */
                    pCNode->pRight->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateRight(pTree, pBNode);
                    RBTree_RotateLeft(pTree, pAParentNode);
                }
                else
                {
                    /* Lr-rb型和Lr-rr型的情况 */
                    pCNode->pLeft->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateRightRightLeft(pTree, pAParentNode);
                }
            }
            else
            {
                /* Lb型, Lb型分为Lb-bb型, Lb-br型, Lb-rb型, Lb-rr型四种情况 */
                if ( ( pBNode->pLeft == NULL 
                    || pBNode->pLeft->nMagic == RBTREE_COLOR_BLACK )
                    && ( pBNode->pRight == NULL  
                    || pBNode->pRight->nMagic == RBTREE_COLOR_BLACK ) )
                {
                    /* Lb-bb型, B节点的左右子节点都是黑色的情况 */
                    if ( pAParentNode->nMagic == RBTREE_COLOR_BLACK )
                    {
                        /* 如果A节点的父节点颜色为黑色的话，需要将B节点改成红色，
                         * 然后将A节点的父节点变成A节点重新调整平衡。
                        */
                        pBNode->nMagic = RBTREE_COLOR_RED;
                        pANode = pAParentNode;
                        pAParentNode = pANode->pParent;
                        continue;
                    }
                    else
                    {
                        /* A节点的父节点为红色的情况，只需要将A节点的颜色
                         * 改成黑色，将B节点颜色改成红色即可达到平衡
                         */
                        pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                        pBNode->nMagic = RBTREE_COLOR_RED;
                    }
                }
                else if ( pBNode->pRight != NULL 
                    && pBNode->pRight->nMagic == RBTREE_COLOR_RED
                    && (pBNode->pLeft == NULL 
                    || pBNode->pLeft->nMagic == RBTREE_COLOR_BLACK ) )
                {
                    /* Lb-br型，即B节点的左子节点为黑色，右子节点为红色的情况*/
                    pBNode->nMagic = pAParentNode->nMagic;
                    pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                    pBNode->pRight->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeft(pTree, pAParentNode);
                }
                else
                {
                    /* Lb-rb型和Lb-rr型的情况, B节点的左子节点为红色的情况，需
                     * 要先对B节点进行右旋再对A节点的父节点进行左旋即可达到平衡
                    */
                    pBNode->pLeft->nMagic = pAParentNode->nMagic;
                    pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateRight(pTree, pBNode);
                    RBTree_RotateLeft(pTree, pAParentNode);
                }
            }
        }
        else
        {
            pBNode = pAParentNode->pLeft;
            if ( pBNode->nMagic == RBTREE_COLOR_RED )
            {
                /* 以下处理Rr型的不平衡情况Rr型不平衡处理分为4种情况
                 * Rr-bb型，Rr-br型，Rr-rb型，Rr-rr型
                 */
                pCNode = pBNode->pRight;
                if ( (pCNode->pLeft == NULL 
                    || pCNode->pLeft->nMagic == RBTREE_COLOR_BLACK)
                    && (pCNode->pRight == NULL 
                    || pCNode->pRight->nMagic == RBTREE_COLOR_BLACK))
                {
                    /* Rr-bb型，w节点的左右节点均为黑色，和Lr-bb对称 */
                    pBNode->nMagic = RBTREE_COLOR_BLACK;
                    pCNode->nMagic = RBTREE_COLOR_RED;
                    RBTree_RotateRight(pTree, pAParentNode);
                }
                else if ( pCNode->pRight == NULL 
                    || pCNode->pRight->nMagic == RBTREE_COLOR_BLACK )
                {
                    /* Rr-rb型，C节点的左节点为红色，右节点为黑色, 和Lr-br对称*/
                    pCNode->pLeft->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeft(pTree, pAParentNode->pLeft);
                    RBTree_RotateRight(pTree, pAParentNode);
                }
                else 
                {
                    /* 有两种情况：
                     * 1) Rr-br型: C节点的右节点为红色，左节点为黑色，和Lr-rb对称 
                     * 2) Rr-rr类型: C节点的左右节点均为红色，和Lr-rr对称 
                     * 这两种情况的旋转操作是一样的，因此合并到一起 
                     */
                    pCNode->pRight->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeftLeftRight(pTree, pAParentNode);
                }
            }
            else
            {
                /* Rb型, Rb型分为Rb-bb型, Rb-br型, Rb-rb型, Rb-rr型四种情况 */
                if ( ( pBNode->pLeft == NULL 
                    || pBNode->pLeft->nMagic == RBTREE_COLOR_BLACK )
                    && ( pBNode->pRight == NULL  
                    || pBNode->pRight->nMagic == RBTREE_COLOR_BLACK ) )
                {
                    /* Rb-bb型, B节点的左右子节点都是黑色的情况，和Lb-bb对称 */
                    if ( pAParentNode->nMagic == RBTREE_COLOR_BLACK )
                    {
                        /* 如果A节点的父节点颜色为黑色的话，需要将B节点改成红色，
                         * 然后将A节点的父节点变成A节点重新调整平衡。
                        */
                        pBNode->nMagic = RBTREE_COLOR_RED;
                        pANode = pAParentNode;
                        pAParentNode = pANode->pParent;
                        continue;
                    }
                    else
                    {
                        /* A节点的父节点为红色的情况，只需要将A节点的颜色
                         * 改成黑色，将B节点颜色改成红色即可达到平衡
                         */
                        pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                        pBNode->nMagic = RBTREE_COLOR_RED;
                    }
                }
                else if ( pBNode->pLeft != NULL 
                    && pBNode->pLeft->nMagic == RBTREE_COLOR_RED
                    && (pBNode->pRight == NULL 
                    || pBNode->pRight->nMagic == RBTREE_COLOR_BLACK ) )
                {
                    /* Rb-rb型，即B节点的左子节点为红色，右子节点为黑色的情况，
                     *  和Lb-br对称 
                     */
                    pBNode->nMagic = pAParentNode->nMagic;
                    pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                    pBNode->pLeft->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateRight(pTree, pAParentNode);
                }
                else
                {
                    /* Rb-br型和Rb-rr型的情况, B节点的左子节点为红色的情况，需
                     * 要先对B节点进行右旋再对A节点的父节点进行左旋即可达到平衡
                    */
                    pBNode->pRight->nMagic = pAParentNode->nMagic;
                    pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeft(pTree, pBNode);
                    RBTree_RotateRight(pTree, pAParentNode);
                }
            }
        }
        break;
    } /* while(...) */
    if ( pANode != NULL )
    {
        pANode->nMagic = RBTREE_COLOR_BLACK;
    }
}


/**	红黑树的删除函数

	@param	RBTREE *pTree - 红黑树指针	
	@param	void *pData - 数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功。	
*/
INT RBTree_Delete(RBTREE *pTree, void *pData, 
                       COMPAREFUNC CompareFunc,
                       DESTROYFUNC DestroyFunc)
{
    RBTREENODE  *pNode;
    RBTREENODE  *pParentNode;

    RBTREENODE  *pANode;
    RBTREENODE  *pDelNode;
    RBTREENODE  *pAParentNode;
    INT         nRet = 0;

    if ( pTree == NULL || pData == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    pParentNode = NULL;
    pNode = pTree->pRoot;

    while ( pNode != NULL )
    {
        pParentNode = pNode;
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
        /* 未找到指定节点，认为删除失败 */
        return CAPI_FAILED;
    }
    if ( pTree->pCursor == pNode )
    {
        RBTree_EnumNext(pTree);
    }

    pDelNode = pNode;
    if ( pNode->pLeft != NULL && pNode->pRight != NULL )
    {
        INT         nMagic;
        /* 处理查找到的pNode有两个子节点的情况 */
        pDelNode = pDelNode->pLeft;   
        while (pDelNode->pRight != 0)
        {
            pDelNode = pDelNode->pRight;
        }
        pANode = pDelNode->pLeft;   /* pANode可以是空节点 */

        pNode->pRight->pParent = pDelNode; 
        pDelNode->pRight = pNode->pRight;
        if (pDelNode != pNode->pLeft) 
        {
            pAParentNode = pDelNode->pParent;
            if (pANode != NULL)
            {
                pANode->pParent = pDelNode->pParent;
            }
            pDelNode->pParent->pRight = pANode;
            pDelNode->pLeft = pNode->pLeft;
            pNode->pLeft->pParent = pDelNode;
        }
        else
        {
            pAParentNode = pDelNode;  
        }
        if (pTree->pRoot == pNode)
        {
            pTree->pRoot = pDelNode;
        }
        else if (pNode->pParent->pLeft == pNode)
        {
            pNode->pParent->pLeft = pDelNode;
        }
        else
        {
            pNode->pParent->pRight = pDelNode;
        }
        pDelNode->pParent = pNode->pParent;

        nMagic = pDelNode->nMagic;
        pDelNode->nMagic = pNode->nMagic;
        pNode->nMagic = nMagic;

        pDelNode = pNode;
    }
    else
    {
        /* 处理最多只有一个子节点的情况 */
        if ( pNode->pLeft != NULL )
        {
            pANode = pNode->pLeft;
        }
        else
        {
            pANode = pNode->pRight;
        }
        pAParentNode = pDelNode->pParent;
        if ( pANode != NULL ) 
        {
            pANode->pParent = pDelNode->pParent;   
        }
        if ( pTree->pRoot == pNode )
        {
            pTree->pRoot = pANode;
        }
        else
        {
            if ( pNode->pParent->pLeft == pNode )
            {
                pNode->pParent->pLeft = pANode;
            }
            else
            {
                pNode->pParent->pRight = pANode;
            }
        }
    }
    if ( pDelNode->nMagic != RBTREE_COLOR_RED )
    {
        RBTree_AdjustColorForDelete(pTree, pAParentNode, pANode);
    }


    if ( DestroyFunc != NULL )
    {
        (*DestroyFunc)(pDelNode->pData);
    }
    free(pDelNode);
    /* 将节点数量减1 */
    pTree->uNodeCount -= 1;

    return CAPI_SUCCESS;
}

/**	红黑树的枚举开始函数

	@param	RBTREE *pTree - 红黑树指针	
	@return	void - 无	
*/
void RBTree_EnumBegin(RBTREE *pTree)
{
    RBTREENODE *pCursor;

    pCursor = pTree->pRoot;
    if ( pCursor != NULL )
    {
        while ( pCursor->pLeft != NULL )
        {
            pCursor = pCursor->pLeft;
        }
    }
    pTree->pCursor = pCursor;
}

/**	红黑树的枚举下一个元素函数
    按照中序遍历顺序依次获取下一个元素

	@param	RBTREE *pTree - 红黑树指针	
	@return	void * - 返回获取的下一个元素，如果下一个元素不存在则返回空	
*/
void * RBTree_EnumNext(RBTREE *pTree)
{
    void *pData;
    RBTREENODE *pCursor;

    if ( pTree->pCursor == NULL )
    {
        return NULL;
    }

    pCursor = pTree->pCursor;
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
        RBTREENODE *pParent = pCursor->pParent;
        while ( pParent != NULL && pParent->pRight == pCursor )
        {
            pCursor = pParent;
            pParent = pParent->pParent;
        }
        pCursor = pParent;
    }
    pTree->pCursor = pCursor;

    return pData;
}


/**	红黑树的获取指定节点为根节点子树的最小元素函数

	@param	RBTREE *pTree - 红黑树指针	
	@param	RBTREENODE *pNode - 子树的根节点指针	
	@return	void * - 返回获取的最小节点数据指针	
*/
void *RBTree_GetMinium(RBTREE *pTree, RBTREENODE *pNode)
{
    RBTREENODE  *pTempNode;

    pTempNode = pNode;
    while ( pTempNode->pLeft != NULL )
    {
        pTempNode = pTempNode->pLeft;
    }
    return pTempNode->pData;
}


/**	红黑树的获取指定节点为根节点子树的最大元素函数

    @param RBTREE *pTree - 红黑树指针
    @param RBTREENODE *pNode - 子树的根节点指针
    @return void * -  返回获取的最大节点数据指针
*/
void *RBTree_GetMaxium(RBTREE *pTree, RBTREENODE *pNode)
{
    RBTREENODE  *pTempNode;

    pTempNode = pNode;
    while ( pTempNode->pRight != NULL )
    {
        pTempNode = pTempNode->pRight;
    }
    return pTempNode->pData;
}

INT ChangeNode(RBTREE *pTree, char *pSrcData, char *pTagData, INT nMagic)
{
    RBTREENODE *pNode;
    
    
    pNode = pTree->pRoot;
    
    while ( pNode != NULL )
    {
        INT nRet = strcmp((char *)pNode->pData, pSrcData);
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
            if ( nMagic != -1 )
            {
                pNode->nMagic = nMagic;
            }
            free((char *)(pNode->pData));

            if ( pTagData != NULL )
            {
                pNode->pData = (char *)strdup(pTagData);
            }
            else
            {
                if ( pNode->pParent != NULL )
                {
                    if ( pNode == pNode->pParent->pLeft )
                    {
                        pNode->pParent->pLeft = NULL;
                    }
                    else
                    {
                        pNode->pParent->pRight = NULL;
                    }
                }
                free(pNode);
                pTree->uNodeCount -= 1;
            }
            
            return 1;

        }
    }
    return 0;
}

INT CompareRBTree(RBTREENODE *pSrcNode, RBTREENODE *pTagNode)
{
    if ( pSrcNode == NULL )
    {
        if ( pTagNode == NULL )
        {
            return 1;
        }

        return 0;
    }

    if ( strcmp((char *)(pSrcNode->pData), (char *)(pTagNode->pData)) == 0 
        && pSrcNode->nMagic == pTagNode->nMagic )
    {
        if ( CompareRBTree(pSrcNode->pLeft, pTagNode->pLeft) == 1 )
        {
            if ( CompareRBTree(pSrcNode->pRight, pTagNode->pRight) == 1 )
            {
                return 1;
            }
        }
        return 0;
    }
    else
    {
        return 0;
    }
}

