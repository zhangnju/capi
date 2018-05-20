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


/**	获取内存大小在全局数组g_auMemSize的下标位置索引

	@param	UINT uMemSize - 内存尺寸	
	@return	int - 位置索引	
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


/**	内存池初始化函数

	@return	void - 无	
*/
void MemoryPool_Init()
{
    int i; 
    for ( i = 0; i < MAX_MEMORY_CLASS; i++ )
    {
        g_MemoryPool[i].freelist_queue.Create(DEFAULT_MEMORYPOOL_QUEUE_SIZE);
    }
}


/**	将一个FreeList压入到内存池中
    内存池中的FreeList并不包含FreeList结构体本身，
    只有一个由头部节点开始的链表

	@param	FREENODE *pNode - 压入的FreeList头部节点指针	
	@param	UINT uMemSize - 压入的FreeList管理的内存大小	
	@return	void - 无	
*/
void MemoryPool_Push(FREENODE *pNode, UINT uMemSize)
{
    int index = MemoryPool_GetMemSizeIndex(uMemSize);

    g_MemoryPool[index].freelist_queue.EnQueue(pNode);
}


/**	从内存池中弹出一个FreeList

	@param	UINT uMemSize - 要弹出的FreeList管理内存的大小	
	@return	FREENODE * - 要弹出的FreeList的头部节点指针	
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

