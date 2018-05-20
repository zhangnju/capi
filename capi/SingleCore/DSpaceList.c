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
#include "DSpaceList.h"

/**	动态等尺寸内存分配管理的创建函数

	@param	UINT uDataSize -所管理的空间链表数据大小	
	@param	UINT uDataCount -所管理的空间链表数据数量	
	@param	UINT uMinDSNodeCount -最小保存的空间链表个数	
	@param	UINT uDSNodeCount -保存最大的空间链表个数	
	@return	DSPACELIST * - 成功返回等尺寸内存分配管理指针，失败返回NULL
*/
DSPACELIST *DSpaceList_Create(UINT uDataSize, UINT uDataCount,
                              UINT uMinDSNodeCount, UINT uDSNodeCount)
{
    DSPACELIST  *pList;
    UINT        i;

    pList = (DSPACELIST *)malloc(sizeof(DSPACELIST));
    if ( pList == NULL )
    {
        return NULL;
    }
    pList->pDSNode = (DSPACENODE *)malloc(uDSNodeCount * sizeof(DSPACENODE));
    if ( pList->pDSNode == NULL )
    {
        free(pList);
        return NULL;
    }

    for ( i = 0; i < uDSNodeCount - 1; i++ )
    {
        pList->pDSNode[i].pNext = &(pList->pDSNode[i+1]);
        pList->pDSNode[i].uPos = i;
    }
    
    for ( i = 1; i < uDSNodeCount; i++ )
    {
        pList->pDSNode[i].pSpList = NULL;
        pList->pDSNode[i].uInListFlag = DSPACENODE_NOT_IN_LIST;
        pList->pDSNode[i].pPrev = NULL;
    }
    
    pList->pDSNode[uDSNodeCount-1].pNext = NULL;
    pList->pDSNode[uDSNodeCount-1].uPos = uDSNodeCount-1;

    pList->pEmpty = &(pList->pDSNode[1]);

    pList->pDSNode[0].pSpList = SpList_Create(uDataCount, uDataSize, 0);
    if ( pList->pDSNode[0].pSpList == NULL )
    {
        free(pList->pDSNode);
        free(pList);
        return NULL;
    }
    
    /* 将第0个节点插入到双向链表中 */
    pList->pHead = &(pList->pDSNode[0]);
    pList->pTail = pList->pHead;
    pList->pHead->pNext = NULL;
    pList->pHead->pPrev = NULL;
    pList->pDSNode[0].uInListFlag = DSPACENODE_IN_LIST;

    if ( uDSNodeCount < uMinDSNodeCount )
    {
        uDSNodeCount = uMinDSNodeCount;
    }

    pList->uCurrentDSNode = 0;
    pList->uDataCount = uDataCount;
    pList->uDataSize = uDataSize;
    pList->uDSNodeCount = uDSNodeCount;
    pList->uMinDSNodeCount = uMinDSNodeCount;
    pList->uUsedDSNodeCount = 1;


    return pList;
}


/**	动态等尺寸内存分配管理的释放函数

	@param	DSPACELIST *pList -动态等尺寸内存分配管理指针	
	@return	void -无	
*/
void DSpaceList_Destroy(DSPACELIST *pList)
{
    UINT    i;

    if ( pList != NULL )
    {
        for ( i = 0; i < pList->uDSNodeCount; i++ )
        {
            if ( pList->pDSNode[i].pSpList != NULL )
            {
                SpList_Destroy(pList->pDSNode[i].pSpList);
            }
        }
        free(pList->pDSNode);
        free(pList);
    }
}


/**	动态等尺寸内存分配管理的内存分配函数

	@param	DSPACELIST *pList -动态等尺寸内存分配管理指针	
	@return	void * -成功返回分配到的内存，失败返回NULL	
*/
void *DSpaceList_Alloc(DSPACELIST *pList)
{
    UINT        i;
    void       *pData;
    DSPACENODE *pDSNode;

    pData = NULL;
    pDSNode = pList->pHead;

    if ( pDSNode != NULL )
    {
        pData = SpList_Alloc(pDSNode->pSpList);
        if ( pDSNode->pSpList->uFreeCount == 0 )
        {
            /* 头部节点指向的可用空间表已没有自由空间，将头部节点弹出 */
            pList->pHead = pDSNode->pNext;
            if ( pList->pHead != NULL )
            {
                pList->pHead->pPrev = NULL;
            }
			/* 以下else语句开始的4行代码为新增，2006-07-14 */
			else
			{
				pList->pTail = NULL;
			}
            pDSNode->uInListFlag = DSPACENODE_NOT_IN_LIST;
        }
    }
    else
    {
        /* 查找并重新分配新的可用空间表来使用 */
        if ( pList->pEmpty == NULL )
        {
            UINT j;
            UINT uLen; 
            DSPACENODE *p;

            uLen = 2 * pList->uDSNodeCount;
            /* 可用空间表已用完，将DSPACENODE表的空间扩大一倍 */
            p = (DSPACENODE *)malloc( uLen * sizeof(DSPACENODE));
            if ( p == NULL )
            {
                return NULL;
            }

            /* 就将数据复制到新分配的内存中 */
            for ( i = 0; i < pList->uDSNodeCount; i++ )
            {
                if ( pList->pDSNode[i].pNext != NULL )
                {
                    p[i].pNext = &(p[(pList->pDSNode[i].pNext)->uPos]);
                }
                else
                {
                    p[i].pNext = NULL;
                }
                if ( pList->pDSNode[i].pPrev != NULL )
                {
                    p[i].pPrev = &(p[(pList->pDSNode[i].pPrev)->uPos]);
                }
                else
                {
                    p[i].pPrev = NULL;
                }
                p[i].pSpList = pList->pDSNode[i].pSpList;
                p[i].uInListFlag = pList->pDSNode[i].uInListFlag;
                p[i].uPos = i;
            }
            
            pList->uDSNodeCount = uLen;
            
            free(pList->pDSNode);
            pList->pDSNode = p;
            
            /* 重新初始化扩大后增加部分的数据 */
            for ( j = i; j < pList->uDSNodeCount-1; j++ )
            {
                pList->pDSNode[j].pSpList = NULL;
                pList->pDSNode[j].uInListFlag = DSPACENODE_NOT_IN_LIST;
                pList->pDSNode[j].pNext = &(pList->pDSNode[j+1]);
                pList->pDSNode[j].pPrev = NULL;
                pList->pDSNode[j].uPos = j;
            }
            pList->pDSNode[j].pSpList = NULL;
            pList->pDSNode[j].pNext = NULL;
            pList->pDSNode[j].uInListFlag = DSPACENODE_NOT_IN_LIST;
            pList->pDSNode[j].pPrev = NULL;
            pList->pDSNode[j].uPos = j;

            pList->pEmpty = &(pList->pDSNode[i]);
        }

        pDSNode = pList->pEmpty;
        pList->pEmpty = pList->pEmpty->pNext;

        pDSNode->pSpList = SpList_Create(pList->uDataCount, 
                                         pList->uDataSize, pDSNode->uPos);
        if ( pDSNode->pSpList != NULL )
        {
            pList->uUsedDSNodeCount++;

            /* 将pDSNode插入到双向链表尾部 */
            pDSNode->pNext = NULL;
            pDSNode->pPrev = pList->pTail;
            if ( pList->pTail != NULL )
            {
                pList->pTail->pNext = pDSNode;
            }
            else
            {
                pList->pHead = pDSNode;
            }
            pList->pTail = pDSNode;
            pDSNode->uInListFlag = DSPACENODE_IN_LIST;

            pData = SpList_Alloc(pDSNode->pSpList);
        }
    }
    return pData;
}


/**	动态等尺寸内存分配管理的内存释放函数

	@param	DSPACELIST *pList -动态等尺寸内存分配管理指针	
	@param	void *pData -要释放的内存数据	
	@return	void -无	
*/
void DSpaceList_Free(DSPACELIST *pList, void *pData)
{
    SPNODE      *pNode;
    DSPACENODE  *pDSNode;

    pNode = (SPNODE *)((char *)pData - sizeof(SPNODE));
    pDSNode = &(pList->pDSNode[pNode->uPos]);
    
    /* 判断是否符合将整个可用空间表释放的条件 */
    if ( pDSNode->pSpList->uFreeCount == pList->uDataCount - 1 /* 增加减1 2006-07-14 */
        && pList->uUsedDSNodeCount > pList->uMinDSNodeCount )
        //&& pNode->uPos != pList->uCurrentDSNode ) //待删除，冗余代码
    {
        /* 释放此节点指向的可用空间表 */
        SpList_Destroy(pDSNode->pSpList);
        pDSNode->pSpList = NULL;

		if ( pDSNode->uInListFlag == DSPACENODE_IN_LIST )
		{
			/* 从双向链表中删除此节点 */
			if ( pDSNode->pPrev != NULL )
			{
				pDSNode->pPrev->pNext = pDSNode->pNext;
			}
			if ( pDSNode->pNext != NULL )
			{
				pDSNode->pNext->pPrev = pDSNode->pPrev;
			}
			if ( pList->pHead == pDSNode ) /* 漏掉一个等号=，2006-07-14 */
			{
				pList->pHead = pDSNode->pNext;
			}
			if ( pList->pTail == pDSNode )
			{
				pList->pTail = pDSNode->pPrev;
			}
		}
        /* pDSNode加入到pEmpty为头节点的未使用链表中 */
        pDSNode->pNext = pList->pEmpty;
        pDSNode->pPrev = NULL;
        pDSNode->uInListFlag = DSPACENODE_NOT_IN_LIST;
        pList->pEmpty = pDSNode;


        /* 更新当前使用的DSPACENODE总数的计数 */
        pList->uUsedDSNodeCount--;
    }
    else
    {
        SpList_Free(pDSNode->pSpList, pData);                                  
        if ( pDSNode->uInListFlag == DSPACENODE_NOT_IN_LIST )
        {
            /* 将其插入双向链表尾部 */
            pDSNode->pPrev = pList->pTail;
            pDSNode->pNext = NULL;
            if ( pList->pTail != NULL )
            {
                pList->pTail->pNext = pDSNode;
            }
            else
            {
                pList->pHead = pDSNode;
            }
            pList->pTail = pDSNode;

            pDSNode->uInListFlag = DSPACENODE_IN_LIST;
        }
    }
    return;
}
