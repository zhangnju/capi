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

#include <stdio.h>
#include <stdlib.h>
#include "CapiGlobal.h"
#include "Queue.h"


/**	队列的创建函数

	@param	UINT uMaxCount - 队列里的最大数据个数	
	@return	QUEUE * - 成功时返回创建的队列指针，失败返回NULL	
*/
QUEUE *	Queue_Create(UINT uMaxCount)
{
    QUEUE *pQueue;
    
    if ( uMaxCount < 2 ) /* 环形队列大小必须大于等于2才能插入元素 */
    {
        return NULL;
    }
    
    pQueue = (QUEUE *)malloc(sizeof(struct QUEUE_st));
    if ( pQueue != NULL )
    {
        pQueue->ppData = (void **)malloc(uMaxCount * sizeof(void *));
        if ( pQueue->ppData != NULL )
        {
            /* memset( pQueue->ppData, 0, uMaxCount * sizeof(void *)); */
            pQueue->ppData[0] = NULL;
            pQueue->uMaxCount = uMaxCount;

            pQueue->uHead = 0;
            pQueue->uTail = 0;
        }
        else
        {
            free( pQueue );
            pQueue = NULL;
        }
    }
    return pQueue;
}


/**	队列的释放函数，将队列中剩余数据全部释放掉及将队列释放掉

	@param	QUEUE *pQueue - 队列指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
*/
void Queue_Destroy( QUEUE *pQueue, DESTROYFUNC DestroyFunc )
{
    UINT    i;
    
    if ( pQueue == NULL )
    {
        return;
    }

    if ( DestroyFunc != NULL )
    {
        if ( pQueue->uHead > pQueue->uTail )
        {
            for ( i = 0; i < pQueue->uTail; i++ )
            {
                (*DestroyFunc)( pQueue->ppData[i] );
            }
            for ( i = pQueue->uHead; i < pQueue->uMaxCount; i++ )
            {
                (*DestroyFunc)( pQueue->ppData[i] );
            }
        }
        else
        {
            for ( i = pQueue->uHead; i < pQueue->uTail; i++ )
            {
                (*DestroyFunc)( pQueue->ppData[i] );
            }
        }
    }

    free( pQueue->ppData );
    free( pQueue );
}


/**	插入数据到队列的尾部的函数
	如果队列已满会自动将队列空间扩大一倍再插入

	@param	QUEUE *pQueue - 队列指针	
	@param	void *pData - 要插入的数据指针	
	@return	INT - CAPI_FAILED表示队列已满，申请不到内存将队列再扩大，插入失败。
                  CAPI_SUCCESS表示插入成功
*/
INT Queue_InsertTail( QUEUE *pQueue, void *pData )
{
    UINT    uTailNext;

    /* 求出尾部位置的下一个位置 */
    if ( pQueue->uTail == pQueue->uMaxCount - 1 )
    {
        /* 当到了数组的最尾部时，下一个要从数组头部重新计算 */
        uTailNext = 0;
    }
    else
    {
        uTailNext = pQueue->uTail + 1;
    }

    if ( uTailNext != pQueue->uHead ) /* 队列未满的情况 */
    {
        
        pQueue->ppData[pQueue->uTail] = pData;
        pQueue->uTail = uTailNext;
    }
    else /* 队列为满的情况 */
    {
		/* 将队列空间扩大一倍 */       
		void **ppData = (void **)malloc(pQueue->uMaxCount * 2 * sizeof(void *)); 

		if ( ppData == NULL )
		{
			return CAPI_FAILED;
		}
		if ( pQueue->uHead > pQueue->uTail )
		{
			UINT i;
			/* 复制uHead和队列最尾部间的数据 */
			for ( i = pQueue->uHead; i < pQueue->uMaxCount; i++)
			{
				ppData[i] = pQueue->ppData[i];
			}
			/* 复制从0到uTail间的数据 */
			for ( i = 0; i < pQueue->uTail; i++)
			{
				ppData[i + pQueue->uMaxCount] = pQueue->ppData[i];
			}
			pQueue->uTail += pQueue->uMaxCount;
		}
		else
		{
			UINT i;
			/* 复制从uHead到uTail间的数据 */
			for ( i = pQueue->uHead; i < pQueue->uTail; i++)
			{
				ppData[i] = pQueue->ppData[i];
			}
		}
		/* 将输入插入新分配的队列空间中 */
        ppData[pQueue->uTail] = pData;
		pQueue->uTail += 1;
		
		pQueue->uMaxCount *= 2;
		free(pQueue->ppData);
		pQueue->ppData = ppData;
    }

	return CAPI_SUCCESS;
}


/**	队列的弹出头部数据函数

	@param	QUEUE *pQueue - 队列指针	
	@return	void * - NULL表示队列为空，否则返回弹出的头部数据指针	
*/
void * Queue_PopHead(QUEUE *pQueue)
{
    UINT    uHead;

    uHead = pQueue->uHead;

    if ( uHead != pQueue->uTail )
    {
        /* 头部和尾部没有重合表示队列不为空的情况 */
        if ( uHead == pQueue->uMaxCount - 1 )
        {
            pQueue->uHead = 0;
        }
        else
        {
            pQueue->uHead += 1;
        }

		return pQueue->ppData[uHead];
    }
	else
	{
		return NULL;
	}
}


/**	队列的判断是否为满函数

    @param	QUEUE *pQueue - 队列指针	
    @return	INT  - 返回0表示队列未满，返回1表示队列满	
*/
INT Queue_IsFull(QUEUE *pQueue)
{
    if ( 0 == pQueue->uHead )
    {
        if ( pQueue->uTail + 1 == pQueue->uMaxCount )
        {
            return 1;   /* 队列满 */
        }
    }
    else
    {
        if ( pQueue->uTail + 1 == pQueue->uHead )
        {
            return 1;  /* 队列满 */
        }
    }
    return 0; /* 队列未满 */
}


/**	队列的判断是否为空函数

    @param	QUEUE *pQueue - 队列指针	
    @return	INT  - 返回0表示队列非空，返回1表示队列为空	
*/
INT Queue_IsEmpty(QUEUE *pQueue)
{
    if ( pQueue->uHead == pQueue->uTail )
    {
        return 1;  /* 队列为空 */
    }
    else
    {
        return 0; /* 队列不为空 */
    }
}
