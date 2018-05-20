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
#include "SpList.h"

/**	空间链表的创建函数

	@param	UINT uSpaceCount - 空间数量	
	@param	UINT uDataSize -数据大小	
	@param	UINT uPos -在管理数组中的位置	
	@return	SPLIST * -成功返回空间链表指针，失败返回NULL	
*/
SPLIST *SpList_Create(UINT uSpaceCount, UINT uDataSize, UINT uPos)
{
    SPLIST   *pSpList;
    SPNODE   *pNode;
    UINT     i;
    
    pSpList = (SPLIST *)malloc( sizeof(SPLIST)
        + uSpaceCount * (uDataSize + sizeof(SPNODE)) );
    if ( pSpList != NULL )
    {
        pSpList->pBlock = (void *)((char *)pSpList +sizeof(SPLIST) );
        
        /* 创建自由空间链表 */
        pSpList->pHead = (SPNODE *)pSpList->pBlock;
        pNode = pSpList->pHead;
        
        for (i = 0; i < uSpaceCount - 1; i++) /* 将uSpaceCount改为uSpaceCount-1 */
        {
            pNode->uPos = uPos;
            pNode->pNext = (SPNODE *)((char *)pNode + sizeof(SPNODE)
                + uDataSize);
            pNode = pNode->pNext;
        }
		pNode->uPos = uPos; /* 新增加的一行代码 */
        pNode->pNext = NULL;

        pSpList->uFreeCount = uSpaceCount;
    }
    
    return pSpList;
}


/**	空间链表的释放函数

	@param	SPLIST *pSpList - 空间链表指针	
	@return	void -无	
*/
void SpList_Destroy(SPLIST *pSpList)
{
    if ( pSpList != NULL )
    {
        free( pSpList );
    }
}

/**	空间链表的分配内存函数

	@param	SPLIST *pSpList - 空间链表指针	
	@return	void * -成功返回分配到的内存，如果没有自由内存进行分配则返回NULL	
*/
void *SpList_Alloc(SPLIST *pSpList)
{
    SPNODE   *pNode;
    
    pNode = pSpList->pHead;
    if ( pNode != NULL )
    {
        pSpList->pHead = pNode->pNext;
        pSpList->uFreeCount--;
        return (void *)((char *)pNode + sizeof(SPNODE));
    }
    return NULL;
}

/**	空间链表的释放内存函数

	@param	SPLIST *pSpList -空间链表指针	
	@param	void *pData -要释放的内存数据指针	
	@return	void -无	
*/
void SpList_Free(SPLIST *pSpList, void *pData)
{
    SPNODE  *pNode;
    
    pNode= (SPNODE *)((char *)pData - sizeof(SPNODE));
    pNode->pNext = pSpList->pHead;
    pSpList->pHead = pNode;
    pSpList->uFreeCount++;
}

