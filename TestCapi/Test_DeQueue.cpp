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
#include <windows.h>
#include <stdio.h>
#include "CapiGlobal.h"
#include "TestApi.h"
#include "DeQueue.h"

void DRV_DeQue_Create(UINT i);
void DRV_DeQue_InsertTail(UINT i);
void DRV_DeQue_PopHead(UINT i);


void Test_DeQue()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_DeQue_Create(i);
        DRV_DeQue_InsertTail(i);
        DRV_DeQue_PopHead(i);
    }    
}

REGISTER_TESTFUNC(Test_DeQue);

#define MIN_DEQUE_MAP_SIZE		16
#define MIN_DEQUE_BLOCK_SIZE	128

void DRV_DeQue_Create(UINT i)
{
    DEQUE *pQueue = NULL;
    switch( i )
    {
    case 1:
        pQueue = DeQue_Create(1, 0);
        if ( pQueue == NULL 
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != MIN_DEQUE_BLOCK_SIZE 
            || pQueue->uMapSize != MIN_DEQUE_MAP_SIZE 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst )
        {           
            printf( "DeQue_Create(1, 0) ��������1ʧ��!\n" );
        }
        break;
    case 2:
        pQueue = DeQue_Create(0, 16);
        if ( pQueue == NULL
            || pQueue->uBlockSize != MIN_DEQUE_BLOCK_SIZE 
            || pQueue->uMapSize != MIN_DEQUE_MAP_SIZE 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst
			|| pQueue->ppMap == NULL )
        {           
            printf( "DeQue_Create(0,16) ��������2ʧ��!\n" );
        }
        break;
    case 3:
        pQueue = DeQue_Create(1, 256);
        if ( pQueue == NULL
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 256 
            || pQueue->uMapSize != MIN_DEQUE_MAP_SIZE 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst )
        {           
            printf( "DeQue_Create(1,256) ��������3ʧ��!\n" );
        }
        break;
    case 4:
        pQueue = DeQue_Create(128, 1024);
        if ( pQueue == NULL
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 1024 
            || pQueue->uMapSize != 128 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst )
        {           
            printf( "DeQue_Create(1024) ��������4ʧ��!\n" );
        }
        break;
    default:
        break;
    }
    if ( pQueue != NULL )
    {
        DeQue_Destroy(pQueue, NULL);
    }
}

void DRV_DeQue_InsertTail(UINT i)
{
	int k;
    DEQUE *pQueue = NULL;
    pQueue = DeQue_Create(16,128);
    switch( i )
    {
    case 1: //���Բ���һ��Ԫ�ص����
        DeQue_InsertTail(pQueue, (void *)1);
        if ( pQueue == NULL
            || pQueue->ppMap[pQueue->uMapSize/2]->ppData[0] != (void *)1
			|| pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
			|| pQueue->pLast != pQueue->pFirst
            || pQueue->uBlockSize != 128
            || pQueue->uMapSize != 16 )
        {           
            printf( "DeQue_InsertTail() ��������1ʧ��!\n" );
        }
        break;
    case 2: //���Բ���127��Ԫ�ص����
		for ( k = 0; k < 127; k++ )
		{
			DeQue_InsertTail(pQueue, (void *)k);
		}
		for ( k = 0; k < 127; k++ )
		{
			DEQUEBLOCK	*pBlock = pQueue->ppMap[pQueue->uMapSize / 2];
			if ( pBlock->ppData[k] != (void *)k )
			{
				printf( "DeQue_InsertTail(0~126) ��������2ʧ��, k=%d\n", k);
			}
		}
        if ( pQueue == NULL
			|| pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
			|| pQueue->pLast != pQueue->pFirst
            || pQueue->uBlockSize != 128
            || pQueue->uMapSize != 16 )
        {           
            printf( "DeQue_InsertTail(0~126) ��������2ʧ��!\n" );
        }
        break;
    case 3: //���Բ���128��Ԫ�ص����
		for ( k = 0; k < 128; k++ )
		{
			DeQue_InsertTail(pQueue, (void *)k);
		}
		for ( k = 0; k < 127; k++ )
		{
			DEQUEBLOCK	*pBlock = pQueue->ppMap[pQueue->uMapSize / 2];
			if ( pBlock->ppData[k] != (void *)k )
			{
				printf( "DeQue_InsertTail(0~127) ��������3ʧ��, k=%d\n", k);
			}
		}
        if ( pQueue == NULL
			|| pQueue->ppMap[pQueue->uMapSize/2+1]->ppData[0] != (void*)127
			|| pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
			|| pQueue->pLast != pQueue->ppMap[pQueue->uMapSize/2+1]
            || pQueue->uBlockSize != 128
            || pQueue->uMapSize != 16 )
        {           
            printf( "DeQue_InsertTail(0~127) ��������3ʧ��!\n" );
        }
        break;    
    case 4: //���Բ���256��Ԫ�ص����
		for ( k = 0; k < 256; k++ )
		{
			DeQue_InsertTail(pQueue, (void *)k);
		}
		for ( k = 0; k < 127; k++ )
		{
			DEQUEBLOCK	*pBlock = pQueue->ppMap[pQueue->uMapSize / 2];
			if ( pBlock->ppData[k] != (void *)k )
			{
				printf( "DeQue_InsertTail(0~127) ��������4ʧ��, k=%d\n", k);
			}
		}
		for ( k = 127; k < 254; k++ )
		{
			DEQUEBLOCK	*pBlock = pQueue->ppMap[pQueue->uMapSize / 2 + 1];
			if ( pBlock->ppData[k-127] != (void *)k )
			{
				printf( "DeQue_InsertTail(0~256) ��������4ʧ��, k=%d\n", k);
			}
		}
        if ( pQueue == NULL
			|| pQueue->ppMap[pQueue->uMapSize/2+2]->ppData[0] != (void*)254
			|| pQueue->ppMap[pQueue->uMapSize/2+2]->ppData[1] != (void*)255
			|| pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
			|| pQueue->pLast != pQueue->ppMap[pQueue->uMapSize/2+2]
            || pQueue->uBlockSize != 128
            || pQueue->uMapSize != 16 )
        {           
            printf( "DeQue_InsertTail(0~256) ��������4ʧ��!\n" );
        }
		break;
	default:
        break;
    }
	DeQue_Destroy(pQueue, NULL);
}


void DRV_DeQue_PopHead(UINT i)
{
	int k = 0;
    DEQUE *pQueue = DeQue_Create(16,128);
	void *pData = NULL;
    switch( i )
    {
    case 1: //���Զ���Ϊ��ʱ�ĵ������
		pData = DeQue_PopHead(pQueue);
		if ( pData != NULL )
		{
			printf("DeQue_PopTail() ��������1ʧ��!\n" );
		}
        if ( pQueue == NULL 
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 128 
            || pQueue->uMapSize != 16 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst )
        {           
            printf( "DeQue_PopHead() ��������1ʧ��!\n" );
        }
        break;
    case 2: //���Զ�������һ��Ԫ�صĵ������
		DeQue_InsertTail(pQueue, (void *)100);
		pData = DeQue_PopHead(pQueue);
		if ( pData != (void *)100 )
		{
			printf("DeQue_PopHead() ��������2ʧ��!\n" );
		}
        if ( pQueue == NULL 
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 128 
            || pQueue->uMapSize != 16 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst )
        {           
            printf( "DeQue_PopHead() ��������1ʧ��!\n" );
        }		
        break;
    case 3: //���Զ�����������Ԫ�ص��������
		DeQue_InsertTail(pQueue, (void *)100);
		DeQue_InsertTail(pQueue, (void *)101);
		pData = DeQue_PopHead(pQueue);
		if ( pData != (void *)100 )
		{
			printf("DeQue_PopHead() ��������3ʧ��1!\n" );
		}
        if ( pQueue == NULL 
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 128 
            || pQueue->uMapSize != 16 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst 
			|| pQueue->pFirst->ppData[1] != (void *)101 
			|| pQueue->pFirst->uHead != 1
			|| pQueue->pFirst->uTail != 2
			|| pQueue->pFirst->uMapPos != pQueue->uMapSize / 2 )
        {           
            printf( "DeQue_PopHead() ��������3ʧ��2!\n" );
        }		
		pData = DeQue_PopHead(pQueue);
		if ( pData != (void *)101 )
		{
			printf("DeQue_PopHead() ��������3ʧ��3!\n" );
		}
        if ( pQueue == NULL 
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 128 
            || pQueue->uMapSize != 16 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2]
            || pQueue->pLast != pQueue->pFirst 
			|| pQueue->pFirst->uHead != 2
			|| pQueue->pFirst->uTail != 2
			|| pQueue->pFirst->uMapPos != pQueue->uMapSize / 2 )
        {           
            printf( "DeQue_PopHead() ��������3ʧ��4!\n" );
        }		
        break;
    case 4: //���Զ������кܶ��Ԫ�ز��������Ԫ�ص����
		for ( k = 0; k < 256; k++ )
		{
			DeQue_InsertTail(pQueue, (void *)k);
		}
		for ( k = 0; k < 127; k++ )
		{
			pData = DeQue_PopHead(pQueue);
			if ( pData != (void *)k )
			{
				printf("DeQue_PopHead() ��������4ʧ��1, k = %d, pData = %d!\n", k, (int)pData );
			}
		}
        if ( pQueue == NULL 
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 128 
            || pQueue->uMapSize != 16 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2 +1]
            || pQueue->pLast != pQueue->ppMap[pQueue->uMapSize/2 +2]
			|| pQueue->pFirst->ppData[0] != (void *)127 
			|| pQueue->pFirst->uHead != 0
			|| pQueue->pFirst->uTail != 127
			|| pQueue->pFirst->uMapPos != pQueue->uMapSize / 2 + 1 )
        {           
            printf( "DeQue_PopHead() ��������4ʧ��2!\n" );
        }	
		
		for ( k = 127; k < 254; k++ )
		{
			pData = DeQue_PopHead(pQueue);
			if ( pData != (void *)k )
			{
				printf("DeQue_PopHead() ��������4ʧ��3, k = %d, pData = %d!\n", k, (int)pData );
			}
		}
        if ( pQueue == NULL 
			|| pQueue->ppMap == NULL 
            || pQueue->uBlockSize != 128 
            || pQueue->uMapSize != 16 
            || pQueue->pFirst != pQueue->ppMap[pQueue->uMapSize/2 +2]
            || pQueue->pLast != pQueue->ppMap[pQueue->uMapSize/2 +2]
			|| pQueue->pFirst->ppData[0] != (void *)254 
			|| pQueue->pFirst->uHead != 0
			|| pQueue->pFirst->uTail != 2
			|| pQueue->pFirst->uMapPos != pQueue->uMapSize / 2 + 2 )
        {           
            printf( "DeQue_PopHead() ��������4ʧ��4!\n" );
        }	
        break;
    default:
        break;
    }
    DeQue_Destroy(pQueue, NULL);
}
