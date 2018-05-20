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


/**	索引表的上边界查找函数
	由于索引表较小（为4），因此采用顺序查找

	@param	INDEXTABLE *pIndexTable - 索引表指针	
	@param	void *pData - 待查找的数据指针	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	static INDEXNODE * - 返回查找到的索引节点指针	
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

    /* 返回刚好大于等于pData的节点指针 */
    return &(pIndexTable->pNode[i]);
}


/**	AVL树的获取最大关键词函数

	@param	AVLTREE *pTree - AVL树指针	
	@return	void * - 返回AVL树中的最大的关键词，如果AVL树为空则返回NULL	
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

/**	将一颗AVL树分拆成由4颗AVL树组成的索引表
	函数中假定了AVL树第2层的4个节点都存在，
	这要求AVL树至少需要有11个以上的节点
	因此宏MAX_NODE_COUNT_FOR_SPLIT的值必须大于等于11

	@param	AVLTREE *pTree - AVL树指针	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	COPYFUNC CopyFunc - 数据拷贝回调函数	
	@return	INDEXTABLE * - 返回由分拆成的4颗AVL树组成的索引表指针	
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


	 //释放AVL树中的根节点即左右节点共三个节点内存（不包括数据）
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

/**	分布式哈希AVL树的创建函数

	@param	UINT uBucketCount - 分布式哈希AVL树内哈希表的桶的个数	
	@return	DHASHAVLTREE * - 返回分布式哈希AVL树的指针	
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

			//下面的64是估计的一个数字，实际设多少最合适需要进行测试
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


/**	索引表的释放函数

	@param	INDEXTABLE *pIndexTable - 索引表指针	
	@param	DESTROYFUNC KeyDestroy - 数据释放回调函数	
	@return	void - 无	
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

/**	分布式哈希AVL树的释放函数

	@param	DHASHAVLTREE *pTree - 分布式哈希AVL树指针	
	@param	DESTROYFUNC KeyDestroy - 数据释放函数	
	@return	void - 无	
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


/**	子表查找函数

	@param	INDEXNODE *pNode - 索引节点指针	
	@param	void *pKey - 关键词	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	COPYFUNC    CopyFunc - 数据拷贝函数	
	@return	static void * - 返回查找到的数据，找到的数据是原始数据的一个拷贝
							因此用户需要在使用完后采用CopyFunc对应的释放函数进行释放
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
		//将查找表指针转换为索引表指针;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pNode->pSubTable;

		//在索引表中进行二分查找到对应的子表;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//递归调用,防止找到的查找子表又发生大量插入而变为索引表
		//不过这样会发生多级加锁解锁操作
		pData = SubTableFind(pNode, pKey, CompareFunc, CopyFunc); 
	}
	
	FastLockRelease(&(pNode->lock));

	return pData;
}

/**	分布式哈希AVL树的查找函数

	@param	DHASHAVLTREE *pTree - 分布式哈希AVL树指针	
	@param	void *pKey - 要查找的关键词	
	@param	HASHFUNC HashFunc - 哈希函数（第1级查找使用）	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@param	COPYFUNC CopyFunc - 数据拷贝函数	
	@return	void * - 返回找到的数据，找到的数据是原始数据的一个拷贝
					 因此用户需要在使用完后采用CopyFunc对应的释放函数进行释放	
*/
void *DHashAVLTree_Find(DHASHAVLTREE *pTree, void *pKey, HASHFUNC HashFunc,
                        COMPAREFUNC CompareFunc, COPYFUNC CopyFunc)
{
	INDEXNODE	*pNode;
	void		*pTable;

	// 第1级查找采用哈希法进行查找
	UINT uPos = (*HashFunc)(pKey, pTree->uBucketCount);

	pNode = &(pTree->pIndexNode[uPos]);

	int lType = pNode->lType;

	//下面进行多级查找
	while (lType == INDEX_TABLE) 
	{
		//将子表地址转换成一个索引表指针;
		pTable = pNode->pSubTable;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pTable;

		//最终子表 ＝ 在索引表中进行二分查找到对应的索引节点;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//子表类型 ＝ 索引节点的子表类型;
		lType = pNode->lType;
	}

	return SubTableFind(pNode, pKey, CompareFunc, CopyFunc);
}

/**	分布式哈希AVL树的子表插入函数

	@param	INDEXNODE *pNode - 索引节点	
	@param	void *pKey - 要插入的数据	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@return	static void  - 无	
*/
static void  SubTableInsert(INDEXNODE *pNode, void *pKey,
                             COMPAREFUNC CompareFunc, COPYFUNC CopyFunc)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == FIND_TABLE)
	{
		AVLTREE	*pAVLTree = (AVLTREE *)pNode->pSubTable;
		
		//在查找表中插入数据<Key, Data>;
		AVLTree_Insert(pAVLTree, pKey, CompareFunc);
		if ( pAVLTree->uNodeCount > MAX_NODE_COUNT_FOR_SPLIT )
		{
			//将子表拆分成一个包含多个AVLTREE的索引表;
			INDEXTABLE *pIndexTable;
			pIndexTable = AVLTree_SplitFourTable(pAVLTree, CompareFunc, CopyFunc);
			
			//设置新的索引表的父节点指针
			pIndexTable->pParent = pNode;

			pNode->pSubTable = pIndexTable;
			
			//查找表的子表类型 = 索引表(原子操作);
			AtomicIncrement(&(pNode->lType));
		}
	}
	else
	{
		//将查找表指针转换为索引表指针;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pNode->pSubTable;

		//在索引表中找到对应的子表;
		INDEXNODE *pTempNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//由于在获取锁前，其他线程将表的类型由查找表改为了索引表
		//这种情况下有可能有很多其他线程访问这个索引表时是不需要使用锁的
		//因此这里需要递归调用,防止找到的查找子表又发生大量插入而变为索引表
		//这样会发生多级加锁解锁操作，不过发生这种情况的概率是非常低的
		SubTableInsert(pTempNode, pKey, CompareFunc, CopyFunc); 
	}
	
	FastLockRelease(&(pNode->lock));
	return ;
}


/**	分布式哈希AVL树的插入函数

	@param	DHASHAVLTREE *pTree - 分布式哈希AVL树指针	
	@param	void *pKey - 要插入的数据	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	HASHFUNC HashFunc - 哈希函数	
	@return	void - 无	
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
		//将子表地址转换成一个索引表指针;
		pTable = pNode->pSubTable;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pTable;

		//最终子表 ＝ 在索引表中进行二分查找到对应的索引节点;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//子表类型 ＝ 索引节点的子表类型;
		lType = pNode->lType;
	}

	SubTableInsert(pNode, pKey, CompareFunc, CopyFunc);
	return;
}

/**	分布式哈希AVL树的子表删除函数

	@param	INDEXNODE *pNode - 索引节点	
	@param	void *pKey - 要删除的数据	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	static void  - 无	
*/
static void SubTableDelete(INDEXNODE *pNode, void *pKey, 
                             COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == FIND_TABLE)
	{
		AVLTREE	*pTree = (AVLTREE *)pNode->pSubTable;
		
		//在查找表中删除包含Key的节点;
		AVLTree_Delete(pTree, pKey, CompareFunc, DestroyFunc);
	}
	else
	{
		//将查找表指针转换为索引表指针;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pNode->pSubTable;

		//在索引表中进行二分查找到对应的子表;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//由于在获取锁前，其他线程将表的类型由查找表改为了索引表
		//这种情况下有可能有很多其他线程访问这个索引表时是不需要使用锁的
		//因此递归调用,防止找到的查找子表又发生大量插入而变为索引表
		//不过这样会发生多级加锁解锁操作，不过发生这种情况的概率是非常低的
		SubTableDelete(pNode, pKey, CompareFunc, DestroyFunc); 
	}
	FastLockRelease(&(pNode->lock));

	return;
}


/**	分布式哈希AVL树的删除函数

	@param	DHASHAVLTREE *pTree - 分布式哈希AVL树指针	
	@param	void *pKey - 要删除的数据	
	@param	HASHFUNC HashFunc - 哈希函数	
	@param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
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
		//将子表地址转换成一个索引表指针;
		pTable = pNode->pSubTable;
		INDEXTABLE	*pIndexTable = (INDEXTABLE *)pTable;

		//最终子表 ＝ 在索引表中进行二分查找到对应的索引节点;
		pNode = IndexTable_UpperBound(pIndexTable, pKey, CompareFunc);

		//子表类型 ＝ 索引节点的子表类型;
		lType = pNode->lType;
	}

	SubTableDelete(pNode, pKey, CompareFunc, DestroyFunc);
	return;
}

