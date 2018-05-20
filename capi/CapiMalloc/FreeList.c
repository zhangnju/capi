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
#include "BigMemory.h"
#include "FreeList.h"
#include "MemoryPool.h"

/**	FreeList的创建函数

	@param	UINT uMemSize -分配的内存块大小	
	@return	FREELIST * -成功返回空间链表指针，失败返回NULL	
*/
FREELIST * FreeList_Create( UINT uMemSize)
{
	FREELIST   *pFreeList;
	FREENODE   *pNode;
	UINT		uMemCount;
	UINT		i;

    BLOCK  *pBlock = BigMemory_AllocBlock(uMemSize);
    if ( pBlock == NULL )
    {
        return NULL;
    }

	pFreeList = (FREELIST *)pBlock->pDataPtr;

	if ( pFreeList != NULL )
	{
        uMemCount = (pBlock->uBlockSize - sizeof(FREELIST)) / uMemSize;
#ifdef USE_BUMP  /* 如果使用pBump指针后，就不需要将链表初始化了 */
        pFreeList->pBump = (FREENODE *)((char *)pFreeList + sizeof(FREELIST));
        pFreeList->pTailEdge = (void *)((char *)pFreeList->pBump + uMemCount * uMemSize);

        pFreeList->pHead = NULL;
        pFreeList->uFreeCount = 0;
#else 
        /* 创建并初始化freelist链表 */
        pFreeList->pHead = (FREENODE *)((char *)pFreeList +sizeof(FREELIST));
        pNode = pFreeList->pHead;

        for (i = 0; i < uMemCount - 1; i++) 
        {
            pNode->pNext = (FREENODE *)((char *)pNode + uMemSize);
            pNode = pNode->pNext;
        }
        pNode->pNext = NULL;
        pFreeList->uFreeCount = uMemCount; 
#endif
		pFreeList->uMemCount = uMemCount;
		pFreeList->uMemSize = uMemSize;
	}

	return pFreeList;
}


/**	FreeList的初始化函数
    对一个没有空闲内存的已经存在freelist结构体进行初始化

	@param	UINT uMemSize -分配的内存块大小	
	@return	FREELIST * -成功返回空间链表指针，失败返回NULL	
*/
FREELIST * FreeList_Init( FREELIST *pFreeList, UINT uMemSize)
{
	FREENODE   *pNode;
	UINT		uMemCount;
	UINT		i;
	void		*pMem;

    BLOCK *pBlock = BigMemory_AllocBlock(uMemSize);
    if ( pBlock == NULL )
    {
        return NULL;
    }
	pMem = pBlock->pDataPtr;
	uMemCount = pBlock->uBlockSize / uMemSize;
	if ( pMem != NULL )
	{
#ifdef USE_BUMP  /* 如果使用pBump指针后，就不需要将链表初始化了 */
        pFreeList->pBump = (FREENODE *)pMem;
        pFreeList->pTailEdge = (void *)((char *)pMem + uMemCount * uMemSize);

        pFreeList->pHead = NULL;
        pFreeList->uFreeCount = 0;
#else 
        /* 创建freelist链表 */
		pFreeList->pHead = (FREENODE *)pMem;
		pNode = pFreeList->pHead;

		for (i = 0; i < uMemCount - 1; i++) 
		{
			pNode->pNext = (FREENODE *)((char *)pNode + uMemSize);
			pNode = pNode->pNext;
		}
		pNode->pNext = NULL;
		pFreeList->uFreeCount = uMemCount; 
#endif
		pFreeList->uMemCount = uMemCount;
		pFreeList->uMemSize = uMemSize;
	}

	return pFreeList;
}


/**	FreeList的初始化函数

@param	UINT uMemSize -分配的内存块大小	
@return	FREELIST * -成功返回空间链表指针，失败返回NULL	
*/
void FreeList_EmptyInit( FREELIST *pFreeList, UINT uMemSize)
{
	pFreeList->pHead = NULL;
#ifdef USE_BUMP
    pFreeList->pBump = NULL;
#endif
	pFreeList->uFreeCount = 0; 
	pFreeList->uMemCount = 0;
	pFreeList->uMemSize = uMemSize;

	return;
}


/**	FreeList的释放函数

	@param	FREELIST *pFreeList - FreeList指针	
	@return	void -无	
*/
void FreeList_Destroy(FREELIST *pFreeList)
{
	if ( pFreeList != NULL )
	{
		free( pFreeList );
	}
}

/**	FreeList的分配内存函数

	@param	FREELIST *pFreeList - FreeList指针	
	@return	void * -成功返回分配到的内存，如果没有自由内存进行分配则返回NULL	
*/
void * FreeList_Alloc(FREELIST *pFreeList)
{
	FREENODE   *pNode;
#ifdef USE_BUMP
	pNode = pFreeList->pBump;
	if ( pNode != NULL )
	{
		pFreeList->pBump = (FREENODE *)((char *)pFreeList->pBump + pFreeList->uMemSize);

		if ( (UINT)pFreeList->pBump >= (UINT)pFreeList->pTailEdge )
		{
			pFreeList->pBump = NULL;
		}
		return (void *)pNode;
	}

	/* if pBump has no memory to use, then try to allocate from pHead */
#endif
	pNode = pFreeList->pHead;
	if ( pNode != NULL )
	{
		pFreeList->pHead = pNode->pNext;
		pFreeList->uFreeCount--;
		return (void *)pNode;
	}

	return NULL;
}

/**	FreeList的释放内存函数

	@param	FREELIST *pFreeList - FreeList指针	
	@param	void *pData -要释放的内存数据指针	
	@return	void -无	
*/
void FreeList_Free(FREELIST *pFreeList, void *pData)
{
	FREENODE  *pNode;

	pNode= (FREENODE *)pData;
	pNode->pNext = pFreeList->pHead;
	pFreeList->pHead = pNode;
	pFreeList->uFreeCount++;
}


/* 可以考虑将FreeLists模块改成和DSpaceList模块一样，
 * 使用freelist来管理多个freelist 
 */
/**	FreeLists的初始化函数

    @param	FREELISTS *pFreeLists - FreeLists指针	
    @param	UINT uMemSize - 管理的内存大小单位	
	@return	void  - 无	
*/
void FreeLists_Init(FREELISTS *pFreeLists, UINT uMemSize)
{
	UINT i;
	for( i = 0; i < MAX_FREELIST_IN_FREELISTS; i++)
	{
		FreeList_EmptyInit(&(pFreeLists->aFreeList[i]), uMemSize);
	}

	pFreeLists->uFreeListCount = MAX_FREELIST_IN_FREELISTS;
	pFreeLists->uMemSize = uMemSize;
	pFreeLists->uMemCount = pFreeLists->aFreeList[0].uMemCount;
	pFreeLists->uCurAlloc = 0;
	pFreeLists->uCurFree = 0;
}

/**	从FreeLists中分配一块内存

	@param	FREELISTS *pFreeLists - FreeLists指针	
	@return	void * - 返回分配的内存指针，如果系统内存不够则返回NULL	
*/
void *FreeLists_Alloc(FREELISTS *pFreeLists)
{
	FREELIST	*pFreeList;
	UINT		uCurAlloc;
	UINT		i;
	FREENODE *  pNode;
    UINT        uMemSize;

	uCurAlloc = pFreeLists->uCurAlloc;
	pFreeList = &(pFreeLists->aFreeList[uCurAlloc]);
	if ( pFreeList->uFreeCount > 0 )
	{
		return FreeList_Alloc(pFreeList);
	}

	for ( i = uCurAlloc+1; i < pFreeLists->uFreeListCount; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount > 0 )
		{
			pFreeLists->uCurAlloc = i;
			return FreeList_Alloc(pFreeList);
		}
	}

	for ( i = 0; i < uCurAlloc; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount > 0 )
		{
			pFreeLists->uCurAlloc = i;
			return FreeList_Alloc(pFreeList);
		}
	}

	/* if no memory have been allocated, then get a memory block 
	 * from the memory pool 
	 */
    uMemSize = pFreeList->uMemSize;
    pFreeList = &(pFreeLists->aFreeList[0]);
	pNode = MemoryPool_Pop(uMemSize);
    if ( pNode == NULL )
    {
        // 队列中已经没有可供分配的FREENODE 链表 
        // 从BigMemory中分配一个Block块来返回给申请者
        BLOCK       *pBlock;
        void		*pMem;
        UINT		uMemCount;

        pBlock = BigMemory_AllocBlock(uMemSize);
        if ( pBlock == NULL )
        {
            return NULL;
        }
        pMem = pBlock->pDataPtr;
        uMemCount = pBlock->uMemCount;
#ifdef USE_BUMP
        pFreeList->pBump = pMem;
        pFreeList->pTailEdge = (void *)((char *)pMem + uMemCount * uMemSize);
#else
        pNode = (FREENODE *)pMem;

        for (i = 0; i < uMemCount - 1; i++) 
        {
            pNode->pNext = (FREENODE *)((char *)pNode + uMemSize);
            pNode = pNode->pNext;
        }
        pNode->pNext = NULL;
#endif
        pFreeList->pHead = (FREENODE *)pMem;
        pFreeList->uMemCount = uMemCount;
    }
    else
    {
#ifdef USE_BUMP
        pFreeList->pBump = NULL;
#endif
        pFreeList->pHead = pNode;
        pFreeList->uMemCount = (BIG_MEMORY_BLOCK_SIZE - sizeof(BLOCK)) / uMemSize;
    }
    pFreeList->uFreeCount = pFreeList->uMemCount;	
	pFreeLists->uCurAlloc = 0;
	pFreeLists->uCurFree = 0;

	return FreeList_Alloc(pFreeList);
}

/**	释放一块内存到FreeLists中
    必须保证释放的内存大小和FreeLists管理的内存大小相同

	@param	FREELISTS *pFreeLists - FreeLists指针	
	@param	void *pData - 释放的内存指针	
	@return	void - 无	
*/
void FreeLists_Free(FREELISTS *pFreeLists, void *pData)
{
	FREELIST	*pFreeList;
	UINT		uCurFree;
	UINT		i;

	uCurFree = pFreeLists->uCurFree;
	pFreeList = &(pFreeLists->aFreeList[uCurFree]);
	if ( pFreeList->uFreeCount < pFreeList->uMemCount )
	{
		FreeList_Free(pFreeList, pData);
		goto LabelMemoryPool;
	}

	for ( i = uCurFree+1; i < pFreeLists->uFreeListCount; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount < pFreeList->uMemCount )
		{
			pFreeLists->uCurFree = i;
			FreeList_Free(pFreeList, pData);
			goto LabelMemoryPool;
		}
	}

	for ( i = 0; i < uCurFree; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount < pFreeList->uMemCount )
		{
			pFreeLists->uCurFree = i;
			FreeList_Free(pFreeList, pData);
			goto LabelMemoryPool;
		}
	}

LabelMemoryPool:

	if ( pFreeList->uFreeCount == pFreeList->uMemCount )
	{
		UINT	uTotalCount = 0;
		
        /* Calculate the total count of freelist that have full memory */
		for ( i = 0 ; i < pFreeLists->uFreeListCount; i++ )
		{
			if ( pFreeLists->aFreeList[i].uFreeCount == pFreeLists->uMemCount )
			{
				uTotalCount++;
			}
		}
		if ( uTotalCount > MIN_FULL_FREELIST_COUNT )
		{
			MemoryPool_Push(pFreeList->pHead, pFreeList->uMemSize);
#ifdef USE_BUMP
            pFreeList->pBump = NULL;
#endif
			pFreeList->pHead = NULL;
			pFreeList->uFreeCount = 0;
		}
	}
	return;
}

