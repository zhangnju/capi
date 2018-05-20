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

/**	DeQue�еĻ��ζ��д�������

	@param	UINT uBlockSize - ���ζ��д�С	
	@return	DEQUEBLOCK * - ���ζ���ָ��	
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

/**	DeQue�еĻ��ζ����ͷź���

	@param	DEQUE *pQue - DeQueָ��	
	@param	DEQUEBLOCK *pBlock - ���ζ���ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����ָ��	
	@return	void - ��	
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

/**	DeQue�Ĵ�������

	@param	UINT uMapSize - MAP�Ĵ�С	
	@param	UINT uBlockSize - ÿ�����ζ��еĴ�С	
	@return	DEQUE * - ���ش�����DeQueָ�룬ʧ�ܷ���NULL	
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

/**	DeQue���ͷź���

	@param	DEQUE *pQue - Ҫ�ͷŵ�DeQueָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
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

/**	DeQue�Ĳ���β������

	@param	DEQUE *pQue - DeQueָ��	
	@param	void *pData - Ҫ���������ָ��	
	@return	INT (by default) - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ��	
*/
INT	DeQue_InsertTail( DEQUE *pQue, void *pData )
{
	DEQUEBLOCK *pBlock;
    UINT    uTailNext;
	
	pBlock= pQue->pLast;

	/* ����β���ڵ����һλ�� */
	if ( pBlock->uTail == pQue->uBlockSize - 1 )
	{
		uTailNext = 0;
	}
	else
	{
		uTailNext = pBlock->uTail + 1;
	}

	if ( pBlock->uHead != uTailNext ) /* ����δ������� */
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
			/* ���·���MAP */
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

/**	DeQue�ĵ���ͷ���ڵ����ݺ���

	@param	DEQUE *pQue - DeQueָ��	
	@return	void * - ������ͷ������ָ��	
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
		/* ͷ����β��û���غϱ�ʾ���в�Ϊ�յ���� */
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
			/* ������ֻ��һ��Ԫ�ص����, ���������һ������Ҫָ����һ���� */
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