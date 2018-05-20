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
#include "Queue.h"

void DRV_Queue_Create(UINT i);
void DRV_Queue_InsertTail(UINT i);
void DRV_Queue_PopHead(UINT i);


void Test_Queue()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_Queue_Create(i);
        DRV_Queue_InsertTail(i);
        DRV_Queue_PopHead(i);
    }    
}

REGISTER_TESTFUNC(Test_Queue)

void DRV_Queue_Create(UINT i)
{
    QUEUE *pQueue = NULL;
    switch( i )
    {
    case 1:
        pQueue = Queue_Create(0);
        if ( pQueue != NULL )
        {           
            printf( "Queue_Create(0) ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        pQueue = Queue_Create(1);
        if ( pQueue != NULL )
        {           
            printf( "Queue_Create(1) ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        pQueue = Queue_Create(2);
        if ( pQueue == NULL
            || pQueue->ppData == NULL 
            || pQueue->uHead != 0
            || pQueue->uTail != 0
            || pQueue->uMaxCount != 2 )
        {           
            printf( "Queue_Create(2) ≤‚ ‘”√¿˝3 ß∞‹!\n" );
        }
        break;
    case 4:
        pQueue = Queue_Create(1024);
        if ( pQueue == NULL
            || pQueue->ppData == NULL 
            || pQueue->uHead != 0
            || pQueue->uTail != 0
            || pQueue->uMaxCount != 1024 )
        {           
            printf( "Queue_Create(1024) ≤‚ ‘”√¿˝4 ß∞‹!\n" );
        }
        break;
    default:
        break;
    }
    if ( pQueue != NULL && pQueue->uMaxCount != 0  )
    {
        Queue_Destroy(pQueue, NULL);
    }
}


void DRV_Queue_InsertTail(UINT i)
{
    QUEUE *pQueue = NULL;
    switch( i )
    {
    case 1:
        pQueue = Queue_Create(2);

        Queue_InsertTail(pQueue, (void *)1);
        if ( pQueue == NULL
            || pQueue->ppData[0] != (void *)1
            || pQueue->uHead != 0
            || pQueue->uTail != 1 )
        {           
            printf( "Queue_InsertTail() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        pQueue = Queue_Create(3);
		Queue_InsertTail(pQueue, (void *)1);
		Queue_InsertTail(pQueue, (void *)2);
		Queue_InsertTail(pQueue, (void *)3);

        if ( pQueue == NULL
            || pQueue->ppData == NULL 
            || pQueue->uMaxCount != 6
			|| pQueue->ppData[0] != (void *)1 
			|| pQueue->ppData[1] != (void *)2 
			|| pQueue->ppData[2] != (void *)3 
			)
        {           
            printf( "Queue_Create(1) ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        pQueue = Queue_Create(3);
		Queue_InsertTail(pQueue, (void *)1);
		Queue_InsertTail(pQueue, (void *)2);
		Queue_InsertTail(pQueue, (void *)3);
		Queue_PopHead(pQueue);
		Queue_PopHead(pQueue);
		Queue_InsertTail(pQueue, (void *)4);
		Queue_InsertTail(pQueue, (void *)5);
		Queue_InsertTail(pQueue, (void *)6);
		Queue_InsertTail(pQueue, (void *)7);
		Queue_InsertTail(pQueue, (void *)8);
		Queue_InsertTail(pQueue, (void *)9);

		
        if ( pQueue == NULL
            || pQueue->ppData == NULL 
            || pQueue->uMaxCount != 12
			|| pQueue->ppData[3] != (void *)4 
			|| pQueue->ppData[4] != (void *)5 
			|| pQueue->ppData[5] != (void *)6 
			|| pQueue->ppData[2] != (void *)3 
			|| pQueue->ppData[6] != (void *)7 
			|| pQueue->ppData[7] != (void *)8 
			|| pQueue->ppData[8] != (void *)9 
			)
        {           
            printf( "Queue_Create(3) ≤‚ ‘”√¿˝3 ß∞‹!\n" );
        }
        break;    
	default:
        break;
    }
	if ( pQueue != NULL )
	{
		Queue_Destroy(pQueue, NULL);
	}
}

void DRV_Queue_PopHead(UINT i)
{
    QUEUE *pQueue = NULL;
    switch( i )
    {
    case 1:
        pQueue = Queue_Create(0);
        if ( pQueue != NULL )
        {           
            printf( "Queue_Create(0) ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        pQueue = Queue_Create(2);
        if ( pQueue == NULL
            || pQueue->ppData == NULL 
            || pQueue->uMaxCount != 2 )
        {           
            printf( "Queue_Create(1) ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    default:
        break;
    }
	if ( pQueue != NULL )
	{
		Queue_Destroy(pQueue, NULL);
	}
}
