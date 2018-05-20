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
#include <stdio.h>
#include "CapiGlobal.h"
#include "CapiCommon.h"
#include "BinTree.h"
#include "AVLTree.h"
#include "FastLock.h"
#include "CScopedLock.h"
#include "DHashAVLTree.h"


/**	��������ϱ߽���Һ���
	�����������С��Ϊ4������˲���˳�����

	@param	INDEXTABLE *pIndexTable - ������ָ��	
	@param	void *pData - �����ҵ�����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@return	static INDEXNODE * - ���ز��ҵ��������ڵ�ָ��	
*/
static INDEXNODE *IndexTable_UpperBound(INDEXTABLE *pIndexTable, void *pData, 
                              COMPAREFUNC CompareFunc)
{
    int i;

	for ( i = 0; i < (int)(pIndexTable->uCount - 1); i++ )
	{
		if ( (*CompareFunc)(pIndexTable->pNode[i].pKey, pData) < 0 )
		{
			continue;
		}
		else
		{
			break;
		}
	}

    /* ���ظպô��ڵ���pData�Ľڵ�ָ�� */
    return &(pIndexTable->pNode[i]);
}


/**	AVL���Ļ�ȡ���ؼ��ʺ���

	@param	AVLTREE *pTree - AVL��ָ��	
	@return	void * - ����AVL���е����Ĺؼ��ʣ����AVL��Ϊ���򷵻�NULL	
*/
void *AVLTree_GetMaxKey(AVLTREE *pTree)
{
    AVLTREENODE *pNode;
    pNode = pTree->pRoot;
    if ( pNode == NULL )
    {
        return NULL;
    }
    if ( pNode->pRight == NULL )
    {
        return pNode->pData;
    }

    while ( pNode->pRight != NULL )
    {
        pNode = pNode->pRight;
    }

    return pNode->pData;
}

/**	��һ��AVL���ֲ����4��AVL����ɵ�������
	�����мٶ���AVL����2���4���ڵ㶼���ڣ�
	��Ҫ��AVL��������Ҫ��11�����ϵĽڵ�
	��˺�MAX_NODE_COUNT_FOR_SPLIT��ֵ������ڵ���11

	@param	AVLTREE *pTree - AVL��ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	COPYFUNC CopyFunc - ���ݿ����ص�����	
	@return	INDEXTABLE * - �����ɷֲ�ɵ�4��AVL����ɵ�������ָ��	
*/
INDEXTABLE *AVLTree_SplitFourTable(AVLTREE *pTree, COMPAREFUNC CompareFunc,
								   COPYFUNC CopyFunc)
{
    INDEXTABLE *pTable = (INDEXTABLE *)malloc(sizeof(INDEXTABLE) 
        + sizeof(INDEXNODE) * 4);

    pTable->pNode = (INDEXNODE *)((char *)pTable + sizeof(INDEXTABLE));

    pTable->uCount = 4;
   
    UINT i;
    AVLTREE *ppTree[4];

    for ( i = 0; i < pTable->uCount; i++ )
    {
        FastLockInit(&(pTable->pNode[i].lock));
        pTable->pNode[i].lType = FIND_TABLE;
        ppTree[i] = AVLTree_Create();
    }

    AVLTREENODE *pTreeNode = pTree->pRoot;
    ppTree[0]->pRoot = pTreeNode->pLeft->pLeft;
    ppTree[0]->pRoot->pParent = NULL;
    ppTree[1]->pRoot = pTreeNode->pLeft->pRight;
    ppTree[1]->pRoot->pParent = NULL;
    ppTree[2]->pRoot = pTreeNode->pRight->pLeft;
    ppTree[2]->pRoot->pParent = NULL;
    ppTree[3]->pRoot = pTreeNode->pRight->pRight;
    ppTree[3]->pRoot->pParent = NULL;
    
    if ( ppTree[0]->pRoot->uCount > ppTree[1]->pRoot->uCount )
    {
        AVLTree_Insert(ppTree[1], pTreeNode->pLeft->pData, CompareFunc);
    }
    else
    {
        AVLTree_Insert(ppTree[0], pTreeNode->pLeft->pData, CompareFunc);
    }
	AVLTree_Insert(ppTree[1], pTreeNode->pData, CompareFunc);

    if ( ppTree[2]->pRoot->uCount > ppTree[3]->pRoot->uCount )
    {
        AVLTree_Insert(ppTree[3], pTreeNode->pRight->pData, CompareFunc);
    }
    else
    {
        AVLTree_Insert(ppTree[2], pTreeNode->pRight->pData, CompareFunc);
    }


	 //�ͷ�AVL���еĸ��ڵ㼴���ҽڵ㹲�����ڵ��ڴ棨���������ݣ�
	 pTreeNode->pLeft->pLeft = NULL;
	 pTreeNode->pLeft->pRight = NULL;
	 pTreeNode->pRight->pLeft = NULL;
	 pTreeNode->pRight->pRight = NULL;
	 AVLTree_Destroy(pTree, NULL);

    for ( i = 0; i < pTable->uCount; i++ )
    {
        pTable->pNode[i].pSubTable = ppTree[i];
        pTable->pNode[i].pKey = (*CopyFunc)(AVLTree_GetMaxKey(ppTree[i]));
    }

    return pTable;
}

/**	�ֲ�ʽ��ϣAVL���Ĵ�������

	@param	UINT uBucketCount - �ֲ�ʽ��ϣAVL���ڹ�ϣ���Ͱ�ĸ���	
	@return	DHASHAVLTREE * - ���طֲ�ʽ��ϣAVL����ָ��	
*/
DHASHAVLTREE *DHashAVLTree_Create(UINT uBucketCount)
{
	DHASHAVLTREE	*pTree;

	pTree = (DHASHAVLTREE *)malloc(sizeof(DHASHAVLTREE) + uBucketCount * sizeof(INDEXNODE));

	if ( pTree != NULL )
	{
		pTree->pIndexNode = (INDEXNODE *)((char *)pTree + sizeof(DHASHAVLTREE));
		if ( pTree->pIndexNode != NULL )
		{
			pTree->uBucketCount = uBucketCount;
            pTree->uMaxNodeCount = MAX_NODE_COUNT_FOR_SPLIT;

			int i = 0;

			//�����64�ǹ��Ƶ�һ�����֣�ʵ��������������Ҫ���в���
#pragma omp parallel for num_threads(dtn(uBucketCount, 64))
			for ( i = 0; i < (int)uBucketCount; i++)
			{
				AVLTREE * pAVLTree;
				
				pAVLTree = AVLTree_Create();

				FastLockInit(&(pTree->pIndexNode[i].lock)); 
				pTree->pIndexNode[i].lType = FIND_TABLE;
				pTree->pIndexNode[i].pSubTable = (void *)pAVLTree;
			}
		}
		else
		{
			free(pTree);
			pTree = NULL;
		}
	}
	return pTree;
}


/**	��������ͷź���

	@param	INDEXTABLE *pIndexTable - ������ָ��	
	@param	DESTROYFUNC KeyDestroy - �����ͷŻص�����	
	@return	void - ��	
*/
static void IndexTable_Destroy(INDEXTABLE *pIndexTable, DESTROYFUNC KeyDestroy)
{
	UINT i = 0;
	INDEXNODE	*pNode;

	for ( i = 0; i < pIndexTable->uCount; i++ )
	{
		pNode = &(pIndexTable->pNode[i]);
		if ( pNode->lType == FIND_TABLE )
		{
			AVLTree_Destroy((AVLTREE *)pNode->pSubTable, KeyDestroy);
		}
		else
		{
			IndexTable_Destroy((INDEXTABLE *)pNode->pSubTable, KeyDestroy);
		}
		if ( KeyDestroy != NULL )
		{
			(*KeyDestroy)(pNode->pKey);
		}
		FastLockClose(&(pNode->lock));
	}
    free(pIndexTable);
}

/**	�ֲ�ʽ��ϣAVL�����ͷź���

	@param	DHASHAVLTREE *pTree - �ֲ�ʽ��ϣAVL��ָ��	
	@param	DESTROYFUNC KeyDestroy - �����ͷź���	
	@return	void - ��	
*/
void DHashAVLTree_Destroy(DHASHAVLTREE *pTree, DESTROYFUNC KeyDestroy)
{
	UINT i = 0;
	INDEXNODE	*pNode;

	for ( i = 0; i < pTree->uBucketCount; i++ )
	{
		pNode = &(pTree->pIndexNode[i]);
		if ( pNode->lType == FIND_TABLE )
		{
			AVLTree_Destroy((AVLTREE *)pNode->pSubTable, KeyDestroy);
		}
		else
		{
			IndexTable_Destroy((INDEXTABLE *)pNode->pSubTable, KeyDestroy);
		}
		FastLockClose(&(pNode->lock));
	}
	free(pTree);
}


/**	�ӱ���Һ���

	@param	INDEXNODE *pNode - �����ڵ�ָ��	
	@param	void *pKey - �ؼ���	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	COPYFUNC    CopyFunc - ���ݿ�������	
	@return	static void * - ���ز��ҵ������ݣ��ҵ���������ԭʼ���ݵ�һ������
							����û���Ҫ��ʹ��������CopyFunc��Ӧ���ͷź��������ͷ�
*/
static void * SubTableFind(INDEXNODE *pNode, void *pKey, 
                           COMPAREFUNC CompareFunc,
                           COPYFUNC    CopyFunc)
{
	void *pData;

	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == FIND_TABLE)
	{
		AVLTREE	*pTree = (AVLTREE *)pNode->pSubTable;
		pData = AVLTree_Find(pTree, pKey, CompareFunc);
		pData = (*CopyFunc)(pData); 
	}
	else
	{
		//�����ұ�ָ��ת��Ϊ������ָ��;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pNode->pSubTable;

		//���������н��ж��ֲ��ҵ���Ӧ���ӱ�;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//�ݹ����,��ֹ�ҵ��Ĳ����ӱ��ַ��������������Ϊ������
		//���������ᷢ���༶������������
		pData = SubTableFind(pNode, pKey, CompareFunc, CopyFunc); 
	}
	
	FastLockRelease(&(pNode->lock));

	return pData;
}

/**	�ֲ�ʽ��ϣAVL���Ĳ��Һ���

	@param	DHASHAVLTREE *pTree - �ֲ�ʽ��ϣAVL��ָ��	
	@param	void *pKey - Ҫ���ҵĹؼ���	
	@param	HASHFUNC HashFunc - ��ϣ��������1������ʹ�ã�	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@param	COPYFUNC CopyFunc - ���ݿ�������	
	@return	void * - �����ҵ������ݣ��ҵ���������ԭʼ���ݵ�һ������
					 ����û���Ҫ��ʹ��������CopyFunc��Ӧ���ͷź��������ͷ�	
*/
void *DHashAVLTree_Find(DHASHAVLTREE *pTree, void *pKey, HASHFUNC HashFunc,
                        COMPAREFUNC CompareFunc, COPYFUNC CopyFunc)
{
	INDEXNODE	*pNode;
	void		*pTable;

	// ��1�����Ҳ��ù�ϣ�����в���
	UINT uPos = (*HashFunc)(pKey, pTree->uBucketCount);

	pNode = &(pTree->pIndexNode[uPos]);

	int lType = pNode->lType;

	//������ж༶����
	while (lType == INDEX_TABLE) 
	{
		//���ӱ��ַת����һ��������ָ��;
		pTable = pNode->pSubTable;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pTable;

		//�����ӱ� �� ���������н��ж��ֲ��ҵ���Ӧ�������ڵ�;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//�ӱ����� �� �����ڵ���ӱ�����;
		lType = pNode->lType;
	}

	return SubTableFind(pNode, pKey, CompareFunc, CopyFunc);
}

/**	�ֲ�ʽ��ϣAVL�����ӱ���뺯��

	@param	INDEXNODE *pNode - �����ڵ�	
	@param	void *pKey - Ҫ���������	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@return	static void  - ��	
*/
static void  SubTableInsert(INDEXNODE *pNode, void *pKey,
                             COMPAREFUNC CompareFunc, COPYFUNC CopyFunc)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == FIND_TABLE)
	{
		AVLTREE	*pAVLTree = (AVLTREE *)pNode->pSubTable;
		
		//�ڲ��ұ��в�������<Key, Data>;
		AVLTree_Insert(pAVLTree, pKey, CompareFunc);
		if ( pAVLTree->uNodeCount > MAX_NODE_COUNT_FOR_SPLIT )
		{
			//���ӱ��ֳ�һ���������AVLTREE��������;
			INDEXTABLE *pIndexTable;
			pIndexTable = AVLTree_SplitFourTable(pAVLTree, CompareFunc, CopyFunc);
			
			//�����µ�������ĸ��ڵ�ָ��
			pIndexTable->pParent = pNode;

			pNode->pSubTable = pIndexTable;
			
			//���ұ���ӱ����� = ������(ԭ�Ӳ���);
			AtomicIncrement(&(pNode->lType));
		}
	}
	else
	{
		//�����ұ�ָ��ת��Ϊ������ָ��;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pNode->pSubTable;

		//�����������ҵ���Ӧ���ӱ�;
		INDEXNODE *pTempNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//�����ڻ�ȡ��ǰ�������߳̽���������ɲ��ұ��Ϊ��������
		//����������п����кܶ������̷߳������������ʱ�ǲ���Ҫʹ������
		//���������Ҫ�ݹ����,��ֹ�ҵ��Ĳ����ӱ��ַ��������������Ϊ������
		//�����ᷢ���༶������������������������������ĸ����Ƿǳ��͵�
		SubTableInsert(pTempNode, pKey, CompareFunc, CopyFunc); 
	}
	
	FastLockRelease(&(pNode->lock));
	return ;
}


/**	�ֲ�ʽ��ϣAVL���Ĳ��뺯��

	@param	DHASHAVLTREE *pTree - �ֲ�ʽ��ϣAVL��ָ��	
	@param	void *pKey - Ҫ���������	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@return	void - ��	
*/
void DHashAVLTree_Insert(DHASHAVLTREE *pTree, void *pKey,
                         COMPAREFUNC CompareFunc, HASHFUNC HashFunc,
						 COPYFUNC CopyFunc)
{
	INDEXNODE	*pNode;
	void		*pTable;

	UINT uPos = (*HashFunc)(pKey, pTree->uBucketCount);

	pNode = &(pTree->pIndexNode[uPos]);

	int lType = pNode->lType;

	while (lType == INDEX_TABLE) 
	{
		//���ӱ��ַת����һ��������ָ��;
		pTable = pNode->pSubTable;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pTable;

		//�����ӱ� �� ���������н��ж��ֲ��ҵ���Ӧ�������ڵ�;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//�ӱ����� �� �����ڵ���ӱ�����;
		lType = pNode->lType;
	}

	SubTableInsert(pNode, pKey, CompareFunc, CopyFunc);
	return;
}

/**	�ֲ�ʽ��ϣAVL�����ӱ�ɾ������

	@param	INDEXNODE *pNode - �����ڵ�	
	@param	void *pKey - Ҫɾ��������	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	static void  - ��	
*/
static void SubTableDelete(INDEXNODE *pNode, void *pKey, 
                             COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == FIND_TABLE)
	{
		AVLTREE	*pTree = (AVLTREE *)pNode->pSubTable;
		
		//�ڲ��ұ���ɾ������Key�Ľڵ�;
		AVLTree_Delete(pTree, pKey, CompareFunc, DestroyFunc);
	}
	else
	{
		//�����ұ�ָ��ת��Ϊ������ָ��;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pNode->pSubTable;

		//���������н��ж��ֲ��ҵ���Ӧ���ӱ�;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//�����ڻ�ȡ��ǰ�������߳̽���������ɲ��ұ��Ϊ��������
		//����������п����кܶ������̷߳������������ʱ�ǲ���Ҫʹ������
		//��˵ݹ����,��ֹ�ҵ��Ĳ����ӱ��ַ��������������Ϊ������
		//���������ᷢ���༶������������������������������ĸ����Ƿǳ��͵�
		SubTableDelete(pNode, pKey, CompareFunc, DestroyFunc); 
	}
	FastLockRelease(&(pNode->lock));

	return;
}


/**	�ֲ�ʽ��ϣAVL����ɾ������

	@param	DHASHAVLTREE *pTree - �ֲ�ʽ��ϣAVL��ָ��	
	@param	void *pKey - Ҫɾ��������	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
*/
void DHashAVLTree_Delete(DHASHAVLTREE *pTree, void *pKey, HASHFUNC HashFunc,
                         COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc)
{
	INDEXNODE	*pNode;
	void		*pTable;

	UINT uPos = (*HashFunc)(pKey, pTree->uBucketCount);

	pNode = &(pTree->pIndexNode[uPos]);

	int lType = pNode->lType;

	while (lType == INDEX_TABLE) 
	{
		//���ӱ��ַת����һ��������ָ��;
		pTable = pNode->pSubTable;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pTable;

		//�����ӱ� �� ���������н��ж��ֲ��ҵ���Ӧ�������ڵ�;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//�ӱ����� �� �����ڵ���ӱ�����;
		lType = pNode->lType;
	}

	SubTableDelete(pNode, pKey, CompareFunc, DestroyFunc);
	return;
}

