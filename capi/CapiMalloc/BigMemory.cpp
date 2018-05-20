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

/**	����һ��BIG_MEMORY_SIZE��С�Ĵ���ڴ棬һ������ڴ��������������Block
	����Ĵ���ڴ���ʼ��ַ�����ȫ�ֱ���g_pCurBlock��
	ȫ�ֱ���g_pCurrentָ�����ڴ��е����һ�������Block

	@return	INT - ����ʧ�ܷ���CAPI_FAILED, �ɹ�����CAPI_SUCCESS.	
*/
static INT AllocLargeMemory()
{
    g_pCurBlock = malloc(BIG_MEMORY_SIZE );
    if ( g_pCurBlock == NULL )
    {
        return CAPI_FAILED;
    }

    int m = BIG_MEMORY_BLOCK_SIZE;
    
    // ��ȡ����ĵ�ַ�����g_pCurBlock�պ��Ƕ���ģ���ô���ص������������򷵻�
    // g_pCurBlock֮��ĵ�1�������ַ
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


/**	����ڴ�����ʼ������
    ��Ҫ�Ƿ���һ���������ڴ�������һ���ʼ�Ĵ���ڴ�
    
	@param	UINT uMaxBigMemoryCount - ����ڴ������	
	@return	INT - ʧ�ܷ���CAPI_FAILED, �ɹ�����CAPI_SUCCESS.	
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

/**	�Ӵ���ڴ��з���һ��Block

	@param	UINT uMemSize - Block�й�����ڴ��С�����û���Ҫ������ڴ��С	
	@return	BLOCK * - �ɹ�����Blockָ�룬ʧ�ܷ���NULL.	
*/
BLOCK *BigMemory_AllocBlock(UINT uMemSize)
{
	BLOCK *pBlock;

	CScopedLock<CFastLock> slock(g_FastLock);
	
	if ( (UINT)g_pCurrent < (UINT)g_pCurBlock )
	{
		if ( g_uBigMemoryCount < g_uMaxBigMemoryCount )
		{ 
            // �ڴ��Ѿ��������ˣ���Ҫ�����������ڴ�
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


/**	������һ���ڴ�������Block��Blockָ��
	������AllocLargeMemory()�������Ѿ������е�Block�׵�ַ��BIG_MEMORY_BLOCK_SIZE�����ˣ�
	��˿���ͨ�����ڴ��ַ��λ��Ϊ0�ķ���ȡ��Block�׵�ַ����Blockָ�룩

	@param	void *p - ͨ���ֲ�ʽ�ڴ�������������ڴ��ַ	
	@return	BLOCK * - ָ��pָ���ڴ�����Block��Blockָ��	
*/
BLOCK * BigMemory_GetBlockPtr(void *p)
{
    BLOCK *pBlock = (BLOCK *)((UINT)p & (~( BIG_MEMORY_BLOCK_SIZE -1)));

    return pBlock;
}


/**	����������Ĵ���ڴ��ͷŵĺ���
	�������һ������²�Ҫʹ�ã������ܹ�ȷ�����з�����ڴ��������������ǰȫ�����ͷ���

	@return	void - ��	
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
