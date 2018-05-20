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
#include "CLocalQueue.h"
#include "CSharedQueue.h"
#include "BigMemory.h"
#include "FreeList.h"
#include "MemoryPool.h"

#define  DEFAULT_MEMORYPOOL_QUEUE_SIZE     128

typedef struct MEMORYPOOL_st {
    CSharedQueue<FREENODE *, CLocalQueue<FREENODE *>>  freelist_queue;
} MEMORYPOOL;

MEMORYPOOL   g_MemoryPool[MAX_MEMORY_CLASS];

UINT g_auMemSize[MAX_MEMORY_CLASS] = { 8, 16, 32, 40, 48, 56, 64, 72, 96, 128,
192, 256, 384, 512, 1024, 2048, 4096, 8192, 16384};


/**	��ȡ�ڴ��С��ȫ������g_auMemSize���±�λ������

	@param	UINT uMemSize - �ڴ�ߴ�	
	@return	int - λ������	
*/
int MemoryPool_GetMemSizeIndex(UINT uMemSize)
{
    if ( uMemSize <= 72 )
    {
        int index = (uMemSize >> 3)-1;
        if( g_auMemSize[index] >= uMemSize )
        {
            return index;
        }
        else
        {
            return index + 1;
        }
    }
    else
    {
        int i;
        for ( i = 8; i < MAX_MEMORY_CLASS; i++ )
        {
            if ( g_auMemSize[i] >= uMemSize )
            {
                return i;
            }
        }
        return -1;
    }
}


/**	�ڴ�س�ʼ������

	@return	void - ��	
*/
void MemoryPool_Init()
{
    int i; 
    for ( i = 0; i < MAX_MEMORY_CLASS; i++ )
    {
        g_MemoryPool[i].freelist_queue.Create(DEFAULT_MEMORYPOOL_QUEUE_SIZE);
    }
}


/**	��һ��FreeListѹ�뵽�ڴ����
    �ڴ���е�FreeList��������FreeList�ṹ�屾��
    ֻ��һ����ͷ���ڵ㿪ʼ������

	@param	FREENODE *pNode - ѹ���FreeListͷ���ڵ�ָ��	
	@param	UINT uMemSize - ѹ���FreeList������ڴ��С	
	@return	void - ��	
*/
void MemoryPool_Push(FREENODE *pNode, UINT uMemSize)
{
    int index = MemoryPool_GetMemSizeIndex(uMemSize);

    g_MemoryPool[index].freelist_queue.EnQueue(pNode);
}


/**	���ڴ���е���һ��FreeList

	@param	UINT uMemSize - Ҫ������FreeList�����ڴ�Ĵ�С	
	@return	FREENODE * - Ҫ������FreeList��ͷ���ڵ�ָ��	
*/
FREENODE *MemoryPool_Pop(UINT uMemSize)
{
    FREENODE *pNode;

    int index = MemoryPool_GetMemSizeIndex(uMemSize);

    int ret = g_MemoryPool[index].freelist_queue.DeQueue(pNode);

    if ( ret == CAPI_SUCCESS )
    {
        return pNode;
    }
    else
    {
        return NULL;
    }
}

