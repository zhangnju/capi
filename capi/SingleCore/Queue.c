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


/**	���еĴ�������

	@param	UINT uMaxCount - �������������ݸ���	
	@return	QUEUE * - �ɹ�ʱ���ش����Ķ���ָ�룬ʧ�ܷ���NULL	
*/
QUEUE *	Queue_Create(UINT uMaxCount)
{
    QUEUE *pQueue;
    
    if ( uMaxCount < 2 ) /* ���ζ��д�С������ڵ���2���ܲ���Ԫ�� */
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


/**	���е��ͷź�������������ʣ������ȫ���ͷŵ����������ͷŵ�

	@param	QUEUE *pQueue - ����ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
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


/**	�������ݵ����е�β���ĺ���
	��������������Զ������пռ�����һ���ٲ���

	@param	QUEUE *pQueue - ����ָ��	
	@param	void *pData - Ҫ���������ָ��	
	@return	INT - CAPI_FAILED��ʾ�������������벻���ڴ潫���������󣬲���ʧ�ܡ�
                  CAPI_SUCCESS��ʾ����ɹ�
*/
INT Queue_InsertTail( QUEUE *pQueue, void *pData )
{
    UINT    uTailNext;

    /* ���β��λ�õ���һ��λ�� */
    if ( pQueue->uTail == pQueue->uMaxCount - 1 )
    {
        /* �������������β��ʱ����һ��Ҫ������ͷ�����¼��� */
        uTailNext = 0;
    }
    else
    {
        uTailNext = pQueue->uTail + 1;
    }

    if ( uTailNext != pQueue->uHead ) /* ����δ������� */
    {
        
        pQueue->ppData[pQueue->uTail] = pData;
        pQueue->uTail = uTailNext;
    }
    else /* ����Ϊ������� */
    {
		/* �����пռ�����һ�� */       
		void **ppData = (void **)malloc(pQueue->uMaxCount * 2 * sizeof(void *)); 

		if ( ppData == NULL )
		{
			return CAPI_FAILED;
		}
		if ( pQueue->uHead > pQueue->uTail )
		{
			UINT i;
			/* ����uHead�Ͷ�����β��������� */
			for ( i = pQueue->uHead; i < pQueue->uMaxCount; i++)
			{
				ppData[i] = pQueue->ppData[i];
			}
			/* ���ƴ�0��uTail������� */
			for ( i = 0; i < pQueue->uTail; i++)
			{
				ppData[i + pQueue->uMaxCount] = pQueue->ppData[i];
			}
			pQueue->uTail += pQueue->uMaxCount;
		}
		else
		{
			UINT i;
			/* ���ƴ�uHead��uTail������� */
			for ( i = pQueue->uHead; i < pQueue->uTail; i++)
			{
				ppData[i] = pQueue->ppData[i];
			}
		}
		/* ����������·���Ķ��пռ��� */
        ppData[pQueue->uTail] = pData;
		pQueue->uTail += 1;
		
		pQueue->uMaxCount *= 2;
		free(pQueue->ppData);
		pQueue->ppData = ppData;
    }

	return CAPI_SUCCESS;
}


/**	���еĵ���ͷ�����ݺ���

	@param	QUEUE *pQueue - ����ָ��	
	@return	void * - NULL��ʾ����Ϊ�գ����򷵻ص�����ͷ������ָ��	
*/
void * Queue_PopHead(QUEUE *pQueue)
{
    UINT    uHead;

    uHead = pQueue->uHead;

    if ( uHead != pQueue->uTail )
    {
        /* ͷ����β��û���غϱ�ʾ���в�Ϊ�յ���� */
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


/**	���е��ж��Ƿ�Ϊ������

    @param	QUEUE *pQueue - ����ָ��	
    @return	INT  - ����0��ʾ����δ��������1��ʾ������	
*/
INT Queue_IsFull(QUEUE *pQueue)
{
    if ( 0 == pQueue->uHead )
    {
        if ( pQueue->uTail + 1 == pQueue->uMaxCount )
        {
            return 1;   /* ������ */
        }
    }
    else
    {
        if ( pQueue->uTail + 1 == pQueue->uHead )
        {
            return 1;  /* ������ */
        }
    }
    return 0; /* ����δ�� */
}


/**	���е��ж��Ƿ�Ϊ�պ���

    @param	QUEUE *pQueue - ����ָ��	
    @return	INT  - ����0��ʾ���зǿգ�����1��ʾ����Ϊ��	
*/
INT Queue_IsEmpty(QUEUE *pQueue)
{
    if ( pQueue->uHead == pQueue->uTail )
    {
        return 1;  /* ����Ϊ�� */
    }
    else
    {
        return 0; /* ���в�Ϊ�� */
    }
}
