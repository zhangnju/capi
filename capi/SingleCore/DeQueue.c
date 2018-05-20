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
#include "DeQueue.h"

#define MIN_DEQUE_MAP_SIZE		16
#define MIN_DEQUE_BLOCK_SIZE	128

/**	DeQue中的环形队列创建函数

	@param	UINT uBlockSize - 环形队列大小	
	@return	DEQUEBLOCK * - 环形队列指针	
*/
DEQUEBLOCK *DeQueBlock_Create(UINT uBlockSize)
{
	DEQUEBLOCK *pBlock;
	pBlock = (DEQUEBLOCK *)malloc(sizeof(DEQUEBLOCK));
	if ( pBlock != NULL )
	{
		pBlock->ppData = malloc(uBlockSize * sizeof(void *));
		if ( pBlock->ppData == NULL )
		{
			free(pBlock);
			return NULL;
		}
		pBlock->uHead = 0;
		pBlock->uTail = 0;
	}
	return pBlock;
}

/**	DeQue中的环形队列释放函数

	@param	DEQUE *pQue - DeQue指针	
	@param	DEQUEBLOCK *pBlock - 环形队列指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数指针	
	@return	void - 无	
*/
void DeQueBlock_Destroy(DEQUE *pQue, DEQUEBLOCK *pBlock, DESTROYFUNC DestroyFunc)
{
	UINT i;
	if (pBlock != NULL)
	{
		if ( DestroyFunc != NULL )
		{
			if ( pBlock->uHead > pBlock->uTail )
			{
				for ( i = 0; i < pBlock->uTail; i++ )
				{
					(*DestroyFunc)( pBlock->ppData[i] );
				}
				for ( i = pBlock->uHead; i < pQue->uBlockSize; i++ )
				{
					(*DestroyFunc)( pBlock->ppData[i] );
				}
			}
			else
			{
				for ( i = pBlock->uHead; i < pBlock->uTail; i++ )
				{
					(*DestroyFunc)( pBlock->ppData[i] );
				}
			}
		}
		free(pBlock->ppData);
		free(pBlock);
	}	
}

/**	DeQue的创建函数

	@param	UINT uMapSize - MAP的大小	
	@param	UINT uBlockSize - 每个环形队列的大小	
	@return	DEQUE * - 返回创建的DeQue指针，失败返回NULL	
*/
DEQUE *	DeQue_Create(UINT uMapSize, UINT uBlockSize)
{
	DEQUE	*pQue;
	if ( uMapSize < MIN_DEQUE_MAP_SIZE )
	{
		uMapSize = MIN_DEQUE_MAP_SIZE;
	}

	if ( uBlockSize < MIN_DEQUE_BLOCK_SIZE )
	{
		uBlockSize = MIN_DEQUE_BLOCK_SIZE;
	}

	pQue = malloc(sizeof(DEQUE));
	if ( pQue != NULL )
	{

		pQue->ppMap = malloc(uMapSize *sizeof(DEQUEBLOCK *));
		
		if ( pQue->ppMap != NULL )
		{
			DEQUEBLOCK *pBlock = DeQueBlock_Create(uBlockSize);
			if ( pBlock != NULL )
			{
				pBlock->uMapPos = uMapSize / 2;
				pQue->ppMap[pBlock->uMapPos] = pBlock;

				pQue->pLast = pBlock;
				pQue->pFirst = pBlock;
				pQue->uBlockSize = uBlockSize;
				pQue->uMapSize = uMapSize;
			}
			else
			{
				free(pQue->ppMap);
				free(pQue);
				pQue = NULL;
			}		
		}
		else
		{
			free(pQue);
			pQue = NULL;
		}
	}
	return pQue;
}

/**	DeQue的释放函数

	@param	DEQUE *pQue - 要释放的DeQue指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
*/
void DeQue_Destroy( DEQUE *pQue, DESTROYFUNC DestroyFunc )
{
	if ( pQue != NULL )
	{
		UINT i;
		DEQUEBLOCK	*pBlock;
		UINT	uStart, uEnd;
		uStart = pQue->pFirst->uMapPos;
		uEnd = pQue->pLast->uMapPos;
		for ( i = uStart; i <= uEnd; i++)
		{
			pBlock = pQue->ppMap[i];
			DeQueBlock_Destroy(pQue, pBlock, DestroyFunc);
		}
		free(pQue->ppMap);
		free(pQue);
	}
}

/**	DeQue的插入尾部函数

	@param	DEQUE *pQue - DeQue指针	
	@param	void *pData - 要插入的数据指针	
	@return	INT (by default) - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败	
*/
INT	DeQue_InsertTail( DEQUE *pQue, void *pData )
{
	DEQUEBLOCK *pBlock;
    UINT    uTailNext;
	
	pBlock= pQue->pLast;

	/* 计算尾部节点的下一位置 */
	if ( pBlock->uTail == pQue->uBlockSize - 1 )
	{
		uTailNext = 0;
	}
	else
	{
		uTailNext = pBlock->uTail + 1;
	}

	if ( pBlock->uHead != uTailNext ) /* 队列未满的情况 */
	{
		pBlock->ppData[pBlock->uTail] = pData;
		pBlock->uTail = uTailNext;
	}
	else
	{
		DEQUEBLOCK *pNewBlock = DeQueBlock_Create(pQue->uBlockSize);
		if ( pNewBlock == NULL )
		{
			return CAPI_FAILED;
		}
		if ( pBlock->uMapPos >= pQue->uMapSize - 1 )
		{
			/* 重新分配MAP */
			DEQUEBLOCK **ppMap = malloc(pQue->uMapSize * 2 * sizeof(DEQUEBLOCK *));
			if ( ppMap == NULL )
			{
				return CAPI_FAILED;
			}

			memcpy(ppMap, pQue->ppMap, 
				pQue->uMapSize * sizeof(DEQUEBLOCK *) );
			pQue->uMapSize *= 2;

			free(pQue->ppMap);
			pQue->ppMap = ppMap;
		}
		pNewBlock->uMapPos = pBlock->uMapPos + 1;
		pNewBlock->ppData[0] = pData;
		pNewBlock->uTail += 1;

		pQue->ppMap[pNewBlock->uMapPos] = pNewBlock;
		pQue->pLast = pNewBlock;
	}
	return CAPI_SUCCESS;
}

/**	DeQue的弹出头部节点数据函数

	@param	DEQUE *pQue - DeQue指针	
	@return	void * - 弹出的头部数据指针	
*/
void *  DeQue_PopHead(DEQUE *pQue)
{
	DEQUEBLOCK *pBlock;
	UINT		uHead;
	void		*pData;

	pBlock = pQue->pFirst;

	uHead = pBlock->uHead;
	if ( uHead != pBlock->uTail )
	{
		/* 头部和尾部没有重合表示队列不为空的情况 */
		if ( uHead == pQue->uBlockSize - 1 )
		{
			pBlock->uHead = 0;
		}
		else
		{
			pBlock->uHead += 1;
		}
		pData = pBlock->ppData[uHead];
		if ( pBlock->uHead == pBlock->uTail )
		{
			/* 队列中只有一个元素的情况, 如果存在下一块则需要指向下一个块 */
			if ( pQue->pLast != pQue->pFirst )
			{
				UINT uMapPos = pBlock->uMapPos;
				DeQueBlock_Destroy(pQue, pBlock, NULL);
				pQue->pFirst = pQue->ppMap[uMapPos + 1];
			}
		}
		return pData;
	}
	else
	{
		return NULL;
	}
}

void DeQue_EnumBegin(DEQUE *pQue)
{
    pQue->uCurMap = pQue->pFirst->uMapPos;
    pQue->uCurPos = pQue->pFirst->uHead;
}

void *DeQue_EnumNext(DEQUE *pQue)
{
    UINT    uPos;
    DEQUEBLOCK *p = pQue->ppMap[pQue->uCurMap];

    if ( pQue->uCurPos == p->uTail )
    {
        if ( pQue->pLast->uMapPos > pQue->uCurMap )
        {
            pQue->uCurMap += 1;
            p = pQue->ppMap[pQue->uCurMap];
            pQue->uCurPos = p->uHead;
        }
        else
        {
            return NULL;
        }
    }
    if ( pQue->uCurPos == p->uTail )
    {
        return NULL;
    }

    uPos = pQue->uCurPos;
    if ( pQue->uCurPos > p->uTail )
    {
        if ( pQue->uCurPos == pQue->uBlockSize )
        {
            pQue->uCurPos = 0;
        }
        else
        {
            pQue->uCurPos += 1;
        }
    }
    else
    {
        pQue->uCurPos += 1;
    }
    return p->ppData[uPos];
}