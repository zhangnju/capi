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


/**	������Ĵ�������

	@param	void - ��	
	@return	RBTREE * - �ɹ����ش����ĺ����ָ�룬
                       ʧ�ܷ���NULL��
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

/**	������ڵ㴴������

	@param	void *pData - �ڵ�����ָ��	
	@return	static void * - ���ش����Ľڵ�ָ��	
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

/**	��������ͷź������ͷ���ָ���ڵ�Ϊ���ڵ������

	@param	RBTREENODE *pNode - Ҫ�ͷŵĸ��ڵ�	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	static void - ��	
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

/**	��������ͷź���

	@param	RBTREE *pTree - �����ָ��	
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

/**	�������������������תǰA�ڵ�������ҽڵ�B������
    ��ת��A�ڵ����ָ��ָ��B�ڵ����ڵ㣬B�ڵ����ָ��ָ��A�ڵ㣬

	@param	RBTREE *pTree -�����ָ��	
	@param	RBTREENODE *pANode -Ҫ��ת�Ľڵ�	
	@return	static void -��	
*/
static void RBTree_RotateLeft(RBTREE *pTree, RBTREENODE *pANode)
{
    RBTREENODE  *pBNode; /* B�ڵ�ָ�� */

    pBNode = pANode->pRight;

    /* ��B�ڵ����ڵ���A�ڵ���ҽڵ� */
    pANode->pRight = pBNode->pLeft;
    if ( pBNode->pLeft != NULL )
    {
        pBNode->pLeft->pParent = pANode;
    }

    /* �޸�A�ڵ�ĸ�ָ���B�ڵ�Ĺ�ϵ */
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

    /* ��A�ڵ���B�ڵ����ڵ� */
    pBNode->pLeft = pANode;
    pANode->pParent = pBNode;
}


/**	�������������������תǰA�ڵ��������ڵ�B������
    ��ת��A�ڵ����ָ��ָ��B�ڵ���ҽڵ㣬B�ڵ����ָ��ָ��A�ڵ㣬

	@param	RBTREE *pTree -�����ָ��	
	@param	RBTREENODE *pANode -Ҫ��ת�Ľڵ�	
	@return	static void -��	
*/
static void RBTree_RotateRight(RBTREE *pTree, RBTREENODE *pANode)
{
    RBTREENODE  *pBNode; /* B�ڵ�ָ�� */

    pBNode = pANode->pLeft;

    /* ��B�ڵ���ҽڵ���A�ڵ����ڵ� */
    pANode->pLeft = pBNode->pRight;
    if ( pBNode->pRight != NULL )
    {
        pBNode->pRight->pParent = pANode;
    }

    /* �޸�A�ڵ�ĸ�ָ���B�ڵ�Ĺ�ϵ */
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

    /* ��A�ڵ���B�ڵ����ڵ� */
    pBNode->pRight = pANode;
    pANode->pParent = pBNode;
}

/**	�������Ϊ�������������ɫ������

	@param	RBTREE *pTree - �����ָ��	
	@param	RBTREENODE *pANode - ����Ľڵ�ָ��	
	@return	static void - ��	
*/
static void RBTree_AdjustColorForInsert(RBTREE *pTree, RBTREENODE *pANode)
{
    /* ���ݺ�������㷨����Ҫ�Ƚ������A�ڵ���ɫԤ��ɺ�ɫ */
    pANode->nMagic = RBTREE_COLOR_RED;

    while ( pANode != pTree->pRoot 
        && pANode->pParent->nMagic == RBTREE_COLOR_RED )
    {
        if ( pANode->pParent == pANode->pParent->pParent->pLeft )
        {
            /* ���ڵ�Ϊ�游�ڵ���ڵ����� */

            RBTREENODE  *pNode = pANode->pParent->pParent->pRight;
            if ( pNode != NULL && pNode->nMagic == RBTREE_COLOR_RED )
            {
                /* LLr��LRr���ͣ�ֱ���޸���ɫȻ���ټ���ѭ�������游�ڵ���ɫ */
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pNode->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                /* �����游�ڵ���ɫ���ı���˺�ɫ������游�ڵ�����ĸ��ڵ�
                 * �п��ܶ��Ǻ�ɫ��������Ҫ����ѭ���������游�ڵ����ɫƽ����� 
                 */
                pANode = pANode->pParent->pParent;
            }
            else
            {
                /* LLb��LRb���ͣ�ֱ��ͨ����ת�������ɽ�����ɫ */
                if ( pANode == pANode->pParent->pRight )
                {
                    pANode = pANode->pParent;
                    RBTree_RotateLeft(pTree, pANode);
                }
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                RBTree_RotateRight(pTree, pANode->pParent->pParent);

                /* ��������A�ڵ�ĸ��ڵ���ɫ�Ǻ�ɫ�ģ�ʵ���ϻ��˳�ѭ��
                 * ������Ҫ�Ż������С�Ļ�, break ������ʡ�Ե� 
                 */
                break;
            }
        }
        else
        {
            /* ���ڵ�Ϊ�游�ڵ��ҽڵ����� */

            RBTREENODE  *pNode = pANode->pParent->pParent->pLeft;
            if ( pNode != NULL && pNode->nMagic == RBTREE_COLOR_RED )
            {
                /* RLr��RRr���ͣ�ֱ���޸���ɫȻ���ټ���ѭ�������游�ڵ���ɫ */
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pNode->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                /* �����游�ڵ���ɫ���ı���˺�ɫ������游�ڵ�����ĸ��ڵ�
                 * �п��ܶ��Ǻ�ɫ��������Ҫ����ѭ���������游�ڵ����ɫƽ����� 
                 */
                pANode = pANode->pParent->pParent;
            }
            else
            {
                /* RLb��RRb���ͣ�ֱ��ͨ����ת�������ɽ�����ɫ */
                if ( pANode == pANode->pParent->pLeft )
                {
                    pANode = pANode->pParent;
                    RBTree_RotateRight(pTree, pANode);
                }
                pANode->pParent->nMagic = RBTREE_COLOR_BLACK;
                pANode->pParent->pParent->nMagic = RBTREE_COLOR_RED;

                RBTree_RotateLeft(pTree, pANode->pParent->pParent);

                /* ��������A�ڵ�ĸ��ڵ���ɫ�Ǻ�ɫ�ģ�ʵ���ϻ��˳�ѭ��
                 * ������Ҫ�Ż������С�Ļ�, break ������ʡ�Ե� 
                 */
                break;
            }
        }
    }

    /* ���������ɫ���������У����ڵ���ɫ���ܻᱻ�ĳɺ�ɫ 
     * �����Ҫ�����ڵ���ɫ���¸�Ϊ��ɫ����
     */
    pTree->pRoot->nMagic = RBTREE_COLOR_BLACK;
}

/**	������Ĳ��뺯��

	@param	RBTREE *pTree - �����ָ��	
	@param	void *pData - Ҫ���������ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
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

/**	������Ĳ��뺯������ҪΪ��Ҫ�̳е�ģ���ʹ�ã�
    һ��ĺ����Ӧ��ģ���벻Ҫ�����������

	@param	RBTREE *pTree - �����ָ��
	@param	void *pData - ����ָ��	
	@param	COMPAREFUNC CompareFunc - �ȽϺ���	
	@param	void *pTreeNode - ����Ľڵ�	
	@return	INT -CAPI_SUCCESS��ʾ�ɹ�	
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
        /* ��Ϊ�յ���� */
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

/**	������Ĳ��Һ���

	@param	RBTREE *pTree - �����ָ��	
	@param	void *pData - Ҫ���ҵ�����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@return	void * - �ɹ����ز��ҵ�������ָ�룬ʧ�ܷ���NULL	
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


/**	�����ɾ��������Rr-rr��Rr-br�͵���ת
    ��������Ĳ����൱�ڽ�����������һ�����������ϲ���һ��������
    �ϲ���ļ������൱��������ת�ļ�������

    ���AΪ��׼�ڵ㣬��BΪA����ڵ㣬CΪB���ҽڵ�
    �ȶ�C���������ٶ�B�������ٶ�A�����������ɡ�

	@param	RBTREE *pTree - �����ָ��	
	@param	RBTREENODE *pStartNode - Ҫ��ת�Ļ�׼�ڵ�	
	@return	void - ��	
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

/**	�����ɾ��������Lr-rr��Lr-rb�͵���ת
    ����������һ�����������ϲ���һ��������

    ���AΪ��׼�ڵ㣬��BΪA���ҽڵ㣬CΪB����ڵ�
    �ȶ�C���������ٶ�B�������ٶ�A�����������ɡ�

    @param	RBTREE *pTree - �����ָ��	
	@param	RBTREENODE *pStartNode - Ҫ��ת�Ļ�׼�ڵ�	
	@return	void - ��	
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

    /* �޸�ʹD�ڵ��ΪA�ڵ��游�ڵ���ӽڵ㣬���ڵ�����游�ڵ�λ�� */
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

    /* D�ڵ���ҽڵ���C�ڵ����ڵ� */
    pCNode->pLeft = pDNode->pRight;
    if ( pDNode->pRight != NULL )
    {
        pDNode->pRight->pParent = pCNode;
    }

    /* D�ڵ����ڵ��ɸ��ڵ���ҽڵ� */
    pParentNode->pRight = pDNode->pLeft;
    if ( pDNode->pLeft != NULL )
    {
        pDNode->pLeft->pParent = pParentNode;
    }

    /* B�ڵ���D�ڵ���ҽڵ� */
    pDNode->pRight = pBNode;
    pBNode->pParent = pDNode;

    /* ���ڵ���D�ڵ����ڵ� */
    pDNode->pLeft = pParentNode;
    pParentNode->pParent = pDNode;
}



/**	�������ɾ�������жԲ�ƽ��ڵ�ĵ�������

	@param	RBTREE *pTree - �����ָ��	
	@param	RBTREENODE *pParentNode - Ҫ����ƽ��ڵ�ĸ��ڵ�	
	@param	RBTREENODE *pReplaceNode - Ҫƽ��Ľڵ�	
	@return	static void - ��	
*/
static void RBTree_AdjustColorForDelete(RBTREE *pTree, RBTREENODE *pParentNode, 
                                 RBTREENODE *pReplaceNode)
{
    RBTREENODE  *pANode;       /* ��Ҫ����ƽ��Ľڵ� */
    RBTREENODE  *pAParentNode; /* A�ڵ�ĸ��ڵ� */
    RBTREENODE  *pBNode;       /* pAParentNode����һ�ӽڵ� */
    RBTREENODE  *pCNode;       /* ��ʱ����������¼B�ڵ��һ���ӽڵ� */

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
                /* Lr�� */
                pCNode = pBNode->pLeft;
                if ( (pCNode->pLeft == NULL 
                    || pCNode->pLeft->nMagic == RBTREE_COLOR_BLACK)
                    && (pCNode->pRight == NULL 
                    || pCNode->pRight->nMagic == RBTREE_COLOR_BLACK) )
                {
                    /* Lr-bb�ͣ���C�ڵ����ҽڵ㶼�Ǻ�ɫ����� */
                    pCNode->nMagic = RBTREE_COLOR_RED;
                    pBNode->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeft(pTree, pAParentNode);
                }  
                else if ( pCNode->pRight != NULL 
                    && pCNode->pRight->nMagic == RBTREE_COLOR_RED 
                    && ( pCNode->pLeft == NULL 
                    || pCNode->pLeft->nMagic == RBTREE_COLOR_BLACK) )
                {
                    /* Lr-br�ͣ���C�ڵ���ڵ�Ϊ��ɫ���ҽڵ�Ϊ��ɫ����� */
                    pCNode->pRight->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateRight(pTree, pBNode);
                    RBTree_RotateLeft(pTree, pAParentNode);
                }
                else
                {
                    /* Lr-rb�ͺ�Lr-rr�͵���� */
                    pCNode->pLeft->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateRightRightLeft(pTree, pAParentNode);
                }
            }
            else
            {
                /* Lb��, Lb�ͷ�ΪLb-bb��, Lb-br��, Lb-rb��, Lb-rr��������� */
                if ( ( pBNode->pLeft == NULL 
                    || pBNode->pLeft->nMagic == RBTREE_COLOR_BLACK )
                    && ( pBNode->pRight == NULL  
                    || pBNode->pRight->nMagic == RBTREE_COLOR_BLACK ) )
                {
                    /* Lb-bb��, B�ڵ�������ӽڵ㶼�Ǻ�ɫ����� */
                    if ( pAParentNode->nMagic == RBTREE_COLOR_BLACK )
                    {
                        /* ���A�ڵ�ĸ��ڵ���ɫΪ��ɫ�Ļ�����Ҫ��B�ڵ�ĳɺ�ɫ��
                         * Ȼ��A�ڵ�ĸ��ڵ���A�ڵ����µ���ƽ�⡣
                        */
                        pBNode->nMagic = RBTREE_COLOR_RED;
                        pANode = pAParentNode;
                        pAParentNode = pANode->pParent;
                        continue;
                    }
                    else
                    {
                        /* A�ڵ�ĸ��ڵ�Ϊ��ɫ�������ֻ��Ҫ��A�ڵ����ɫ
                         * �ĳɺ�ɫ����B�ڵ���ɫ�ĳɺ�ɫ���ɴﵽƽ��
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
                    /* Lb-br�ͣ���B�ڵ�����ӽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�����*/
                    pBNode->nMagic = pAParentNode->nMagic;
                    pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                    pBNode->pRight->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeft(pTree, pAParentNode);
                }
                else
                {
                    /* Lb-rb�ͺ�Lb-rr�͵����, B�ڵ�����ӽڵ�Ϊ��ɫ���������
                     * Ҫ�ȶ�B�ڵ���������ٶ�A�ڵ�ĸ��ڵ�����������ɴﵽƽ��
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
                /* ���´���Rr�͵Ĳ�ƽ�����Rr�Ͳ�ƽ�⴦���Ϊ4�����
                 * Rr-bb�ͣ�Rr-br�ͣ�Rr-rb�ͣ�Rr-rr��
                 */
                pCNode = pBNode->pRight;
                if ( (pCNode->pLeft == NULL 
                    || pCNode->pLeft->nMagic == RBTREE_COLOR_BLACK)
                    && (pCNode->pRight == NULL 
                    || pCNode->pRight->nMagic == RBTREE_COLOR_BLACK))
                {
                    /* Rr-bb�ͣ�w�ڵ�����ҽڵ��Ϊ��ɫ����Lr-bb�Գ� */
                    pBNode->nMagic = RBTREE_COLOR_BLACK;
                    pCNode->nMagic = RBTREE_COLOR_RED;
                    RBTree_RotateRight(pTree, pAParentNode);
                }
                else if ( pCNode->pRight == NULL 
                    || pCNode->pRight->nMagic == RBTREE_COLOR_BLACK )
                {
                    /* Rr-rb�ͣ�C�ڵ����ڵ�Ϊ��ɫ���ҽڵ�Ϊ��ɫ, ��Lr-br�Գ�*/
                    pCNode->pLeft->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeft(pTree, pAParentNode->pLeft);
                    RBTree_RotateRight(pTree, pAParentNode);
                }
                else 
                {
                    /* �����������
                     * 1) Rr-br��: C�ڵ���ҽڵ�Ϊ��ɫ����ڵ�Ϊ��ɫ����Lr-rb�Գ� 
                     * 2) Rr-rr����: C�ڵ�����ҽڵ��Ϊ��ɫ����Lr-rr�Գ� 
                     * �������������ת������һ���ģ���˺ϲ���һ�� 
                     */
                    pCNode->pRight->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateLeftLeftRight(pTree, pAParentNode);
                }
            }
            else
            {
                /* Rb��, Rb�ͷ�ΪRb-bb��, Rb-br��, Rb-rb��, Rb-rr��������� */
                if ( ( pBNode->pLeft == NULL 
                    || pBNode->pLeft->nMagic == RBTREE_COLOR_BLACK )
                    && ( pBNode->pRight == NULL  
                    || pBNode->pRight->nMagic == RBTREE_COLOR_BLACK ) )
                {
                    /* Rb-bb��, B�ڵ�������ӽڵ㶼�Ǻ�ɫ���������Lb-bb�Գ� */
                    if ( pAParentNode->nMagic == RBTREE_COLOR_BLACK )
                    {
                        /* ���A�ڵ�ĸ��ڵ���ɫΪ��ɫ�Ļ�����Ҫ��B�ڵ�ĳɺ�ɫ��
                         * Ȼ��A�ڵ�ĸ��ڵ���A�ڵ����µ���ƽ�⡣
                        */
                        pBNode->nMagic = RBTREE_COLOR_RED;
                        pANode = pAParentNode;
                        pAParentNode = pANode->pParent;
                        continue;
                    }
                    else
                    {
                        /* A�ڵ�ĸ��ڵ�Ϊ��ɫ�������ֻ��Ҫ��A�ڵ����ɫ
                         * �ĳɺ�ɫ����B�ڵ���ɫ�ĳɺ�ɫ���ɴﵽƽ��
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
                    /* Rb-rb�ͣ���B�ڵ�����ӽڵ�Ϊ��ɫ�����ӽڵ�Ϊ��ɫ�������
                     *  ��Lb-br�Գ� 
                     */
                    pBNode->nMagic = pAParentNode->nMagic;
                    pAParentNode->nMagic = RBTREE_COLOR_BLACK;
                    pBNode->pLeft->nMagic = RBTREE_COLOR_BLACK;
                    RBTree_RotateRight(pTree, pAParentNode);
                }
                else
                {
                    /* Rb-br�ͺ�Rb-rr�͵����, B�ڵ�����ӽڵ�Ϊ��ɫ���������
                     * Ҫ�ȶ�B�ڵ���������ٶ�A�ڵ�ĸ��ڵ�����������ɴﵽƽ��
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


/**	�������ɾ������

	@param	RBTREE *pTree - �����ָ��	
	@param	void *pData - ����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@param	DESTROYFUNC DestroyFunc - �����ͷź���	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
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
        /* δ�ҵ�ָ���ڵ㣬��Ϊɾ��ʧ�� */
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
        /* ������ҵ���pNode�������ӽڵ����� */
        pDelNode = pDelNode->pLeft;   
        while (pDelNode->pRight != 0)
        {
            pDelNode = pDelNode->pRight;
        }
        pANode = pDelNode->pLeft;   /* pANode�����ǿսڵ� */

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
        /* �������ֻ��һ���ӽڵ����� */
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
    /* ���ڵ�������1 */
    pTree->uNodeCount -= 1;

    return CAPI_SUCCESS;
}

/**	�������ö�ٿ�ʼ����

	@param	RBTREE *pTree - �����ָ��	
	@return	void - ��	
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

/**	�������ö����һ��Ԫ�غ���
    �����������˳�����λ�ȡ��һ��Ԫ��

	@param	RBTREE *pTree - �����ָ��	
	@return	void * - ���ػ�ȡ����һ��Ԫ�أ������һ��Ԫ�ز������򷵻ؿ�	
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


/**	������Ļ�ȡָ���ڵ�Ϊ���ڵ���������СԪ�غ���

	@param	RBTREE *pTree - �����ָ��	
	@param	RBTREENODE *pNode - �����ĸ��ڵ�ָ��	
	@return	void * - ���ػ�ȡ����С�ڵ�����ָ��	
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


/**	������Ļ�ȡָ���ڵ�Ϊ���ڵ����������Ԫ�غ���

    @param RBTREE *pTree - �����ָ��
    @param RBTREENODE *pNode - �����ĸ��ڵ�ָ��
    @return void * -  ���ػ�ȡ�����ڵ�����ָ��
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

