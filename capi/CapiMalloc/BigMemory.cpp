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
#include "capiglobal.h"
#include "CScopedLock.h"
#include "BigMemory.h"


static UINT		g_uMaxBigMemoryCount;

static void		*g_pCurBlock = NULL;
static void		*g_pCurrent = NULL;
static void		**g_ppBlock = NULL;
static UINT		g_uBigMemoryCount = 0;

CFastLock		g_FastLock;

/**	分配一个BIG_MEMORY_SIZE大小的大块内存，一个大块内存里包含多个对齐的Block
	分配的大块内存起始地址存放在全局变量g_pCurBlock里
	全局变量g_pCurrent指向大块内存中的最后一个对齐的Block

	@return	INT - 分配失败返回CAPI_FAILED, 成功返回CAPI_SUCCESS.	
*/
static INT AllocLargeMemory()
{
    g_pCurBlock = malloc(BIG_MEMORY_SIZE );
    if ( g_pCurBlock == NULL )
    {
        return CAPI_FAILED;
    }

    int m = BIG_MEMORY_BLOCK_SIZE;
    
    // 获取对齐的地址，如果g_pCurBlock刚好是对齐的，那么返回的是它自身，否则返回
    // g_pCurBlock之后的第1个对齐地址
    void * p = (void *)(((UINT)g_pCurBlock + m - 1) & (-m));

    int offset = (UINT)p - (UINT)g_pCurBlock;

    if ( offset == 0 )
    {
        g_pCurrent = (void *)((char *)g_pCurBlock + BIG_MEMORY_SIZE - BIG_MEMORY_BLOCK_SIZE);
    }
    else
    {
        g_pCurrent = (void *)((char *)p + BIG_MEMORY_SIZE - 2 * BIG_MEMORY_BLOCK_SIZE);
    }
    return CAPI_SUCCESS;
}


/**	大块内存管理初始化函数
    主要是分配一个管理大块内存的数组和一块初始的大块内存
    
	@param	UINT uMaxBigMemoryCount - 大块内存的数量	
	@return	INT - 失败返回CAPI_FAILED, 成功返回CAPI_SUCCESS.	
*/
INT BigMemory_Init(UINT uMaxBigMemoryCount)
{
	if ( g_pCurBlock == NULL )
	{
        if ( AllocLargeMemory() == CAPI_FAILED )
        {
            return CAPI_FAILED;
        }
		
        g_ppBlock = (void **)malloc(uMaxBigMemoryCount * sizeof(void *));
		if (g_ppBlock == NULL)
		{
			free(g_pCurBlock);
			return CAPI_FAILED;
		}
		g_uMaxBigMemoryCount = uMaxBigMemoryCount;
		
		g_uBigMemoryCount = 0;
		g_ppBlock[g_uBigMemoryCount] = g_pCurBlock;
		g_uBigMemoryCount++;

	}
	return CAPI_SUCCESS;
}

/**	从大块内存中分配一个Block

	@param	UINT uMemSize - Block中管理的内存大小，即用户需要分配的内存大小	
	@return	BLOCK * - 成功返回Block指针，失败返回NULL.	
*/
BLOCK *BigMemory_AllocBlock(UINT uMemSize)
{
	BLOCK *pBlock;

	CScopedLock<CFastLock> slock(g_FastLock);
	
	if ( (UINT)g_pCurrent < (UINT)g_pCurBlock )
	{
		if ( g_uBigMemoryCount < g_uMaxBigMemoryCount )
		{ 
            // 内存已经分配完了，需要重新申请大块内存
            if ( AllocLargeMemory() == CAPI_FAILED )
            {
                return NULL;
            }
			g_ppBlock[g_uBigMemoryCount] = g_pCurBlock;
			g_uBigMemoryCount++;
		}
		else
		{
			/* reached the limitation of system memory */
			return NULL;
		}
	}
	pBlock = (BLOCK *)g_pCurrent;
	g_pCurrent = (void *)((char *)g_pCurrent - BIG_MEMORY_BLOCK_SIZE);

    pBlock->uMemSize = uMemSize;
    pBlock->uBlockSize = BIG_MEMORY_BLOCK_SIZE - sizeof(BLOCK);
    pBlock->uMemCount = pBlock->uBlockSize / uMemSize;
    pBlock->pDataPtr = (void *)((char *)pBlock + sizeof(BLOCK));

	return (BLOCK *)pBlock;
}


/**	求任意一个内存所属的Block的Block指针
	由于在AllocLargeMemory()函数里已经将所有的Block首地址按BIG_MEMORY_BLOCK_SIZE对齐了，
	因此可以通过将内存地址低位变为0的方法取得Block首地址（即Block指针）

	@param	void *p - 通过分布式内存管理分配的任意内存地址	
	@return	BLOCK * - 指针p指向内存所属Block的Block指针	
*/
BLOCK * BigMemory_GetBlockPtr(void *p)
{
    BLOCK *pBlock = (BLOCK *)((UINT)p & (~( BIG_MEMORY_BLOCK_SIZE -1)));

    return pBlock;
}


/**	将所有申请的大块内存释放的函数
	这个函数一般情况下不要使用，除非能够确定所有分配的内存在这个函数调用前全部被释放了

	@return	void - 无	
*/
void BigMemory_Close()
{
	UINT		i;

	CScopedLock<CFastLock> slock(g_FastLock);
	for ( i = 0; i < g_uBigMemoryCount; i++ )
	{
		free(g_ppBlock[i]);
	}
	free(g_ppBlock);
}
