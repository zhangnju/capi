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
#include "TestApi.h"
#include "SpList.h"
#include "DSpaceList.h"

void DRV_DSpaceList_Create(UINT i);
void DRV_DSpaceList_Alloc(UINT i);
void DRV_DSpaceList_Free(UINT i);
void DRV_DSpaceList_Destroy(UINT i);

void Test_DSpaceList()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_DSpaceList_Create(i);
        DRV_DSpaceList_Alloc(i);
		DRV_DSpaceList_Free(i);
		DRV_DSpaceList_Destroy(i);
    }    
}

REGISTER_TESTFUNC(Test_DSpaceList)

void DRV_DSpaceList_Create(UINT i)
{
    DSPACELIST *p = NULL;
    switch( i )
    {
    case 1:
        p = DSpaceList_Create(32, 128, 5, 64);
        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pEmpty != &(p->pDSNode[1])
			|| p->pHead == NULL
			|| p->pTail == NULL
			|| p->uCurrentDSNode != 0
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 1 
			|| p->pDSNode[0].pSpList->uFreeCount != 128 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->pHead != p->pDSNode[0].pSpList->pBlock )
        {
            printf( "DSpaceList_Create() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    default:
        break;
    }

	DSpaceList_Destroy(p);
}

void DRV_DSpaceList_Alloc(UINT i)
{
    DSPACELIST *p = NULL;
	void *pData;
	int k = 0;
	
    switch( i )
    {
    case 1:
        p = DSpaceList_Create(32, 128, 5, 64);
		pData = DSpaceList_Alloc(p);

        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pEmpty != &(p->pDSNode[1])
			|| p->pHead == NULL
			|| p->pTail == NULL
			|| p->pHead->pNext != NULL
			|| p->pHead->pPrev != NULL
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 1 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 127
			|| (INT)pData - (INT)p->pDSNode[0].pSpList->pBlock != sizeof(SPNODE)
			|| (INT)p->pDSNode[0].pSpList->pHead - (INT)p->pDSNode[0].pSpList->pBlock != 32+sizeof(SPNODE))
        {
            printf( "DSpaceList_Alloc() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        p = DSpaceList_Create(32, 128, 5, 64);
		pData = DSpaceList_Alloc(p);
		pData = DSpaceList_Alloc(p);

        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pEmpty != &(p->pDSNode[1])
			|| p->pHead != &(p->pDSNode[0])
			|| p->pTail != p->pHead
			|| p->pHead->pPrev != NULL
			|| p->pTail->pNext != NULL
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 1 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 126
			|| (INT)pData - (INT)p->pDSNode[0].pSpList->pBlock != sizeof(SPNODE) + 32 + sizeof(SPNODE)
			|| (INT)p->pDSNode[0].pSpList->pHead - (INT)p->pDSNode[0].pSpList->pBlock != 2 *(32+sizeof(SPNODE)))
        {
            printf( "DSpaceList_Alloc() ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        p = DSpaceList_Create(32, 128, 5, 64);
		for ( k = 0; k < 128; k++ )
		{
			pData = DSpaceList_Alloc(p);
		}
        if ( p == NULL 
			|| pData == NULL
			|| p->pDSNode == NULL 
			|| p->pHead != NULL
			|| p->pTail != NULL
			|| p->pEmpty != &(p->pDSNode[1])
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 1 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 0
			|| (INT)pData - (INT)p->pDSNode[0].pSpList->pBlock != sizeof(SPNODE) + 127 * (sizeof(SPNODE)+32)
			|| (INT)p->pDSNode[0].pSpList->pHead != NULL )
        {
            printf( "DSpaceList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹!\n" );
        }
        break;    
	case 4:
        p = DSpaceList_Create(32, 128, 5, 64);
		for ( k = 0; k < 129; k++ )
		{
			pData = DSpaceList_Alloc(p);
		}
        if ( p == NULL 
			|| pData == NULL
			|| p->pDSNode == NULL 
			|| p->pHead != &(p->pDSNode[1])
			|| p->pTail != p->pHead
			|| p->pHead->pPrev != NULL
			|| p->pTail->pNext != NULL
			|| p->pEmpty != &(p->pDSNode[2])
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 2
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 0
			|| p->pDSNode[1].pSpList->uFreeCount != 127
			|| (INT)pData - (INT)p->pDSNode[1].pSpList->pBlock != sizeof(SPNODE) 
			|| p->pDSNode[0].pSpList->pHead != NULL 
			|| (INT)p->pDSNode[1].pSpList->pHead - (INT)p->pDSNode[1].pSpList->pBlock != sizeof(SPNODE) + 32 )
        {
            printf( "DSpaceList_Alloc() ≤‚ ‘”√¿˝4 ß∞‹!\n" );
        }
        break;
	case 5:
        p = DSpaceList_Create(32, 128, 5, 64);
		for ( k = 0; k < 128 * 64; k++ )
		{
			pData = DSpaceList_Alloc(p);
		}
        if ( p == NULL 
			|| pData == NULL
			|| p->pDSNode == NULL 
			|| p->pHead != NULL
			|| p->pTail != NULL
			|| p->pEmpty != NULL
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 64
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 0
			|| p->pDSNode[63].pSpList->uFreeCount != 0
			|| (INT)pData - (INT)p->pDSNode[63].pSpList->pBlock != sizeof(SPNODE) + 127 * (32 + sizeof(SPNODE))
			|| p->pDSNode[63].pSpList->pHead != NULL )
        {
            printf( "DSpaceList_Alloc() ≤‚ ‘”√¿˝5 ß∞‹!\n" );
        }
        break;
    default:
        break;
    }
	DSpaceList_Destroy(p);
}

void DRV_DSpaceList_Free(UINT i)
{
    DSPACELIST *p = NULL;
	void *pData;
	int k = 0;
	void *ppData[128*64];
	
	switch( i )
    {
    case 1:
        p = DSpaceList_Create(32, 128, 5, 64);
		pData = DSpaceList_Alloc(p);
		DSpaceList_Free(p, pData);

        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pEmpty != &(p->pDSNode[1])
			|| p->pHead != &(p->pDSNode[0])
			|| p->pTail != p->pHead
			|| p->pHead->pNext != NULL
			|| p->pHead->pPrev != NULL
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 1 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 128
			|| (INT)p->pDSNode[0].pSpList->pHead - (INT)p->pDSNode[0].pSpList->pBlock != 0 )
        {
            printf( "DSpaceList_Free() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        p = DSpaceList_Create(32, 128, 5, 64);
		pData = DSpaceList_Alloc(p);
		pData = DSpaceList_Alloc(p);
		DSpaceList_Free(p, pData);

        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pEmpty != &(p->pDSNode[1])
			|| p->pHead != &(p->pDSNode[0])
			|| p->pTail != p->pHead
			|| p->pHead->pNext != NULL
			|| p->pHead->pPrev != NULL
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 1 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 127
			|| (INT)p->pDSNode[0].pSpList->pHead - (INT)p->pDSNode[0].pSpList->pBlock != 32 + sizeof(SPNODE) )       
		{
            printf( "DSpaceList_Free() ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        p = DSpaceList_Create(32, 128, 5, 64);
		for ( k = 0; k < 128; k++ )
		{
			pData = DSpaceList_Alloc(p);
		}
		DSpaceList_Free(p, pData);

        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pEmpty != &(p->pDSNode[1])
			|| p->pHead != &(p->pDSNode[0])
			|| p->pTail != p->pHead
			|| p->pHead->pNext != NULL
			|| p->pHead->pPrev != NULL
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 1 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 1
			|| (INT)p->pDSNode[0].pSpList->pHead - (INT)p->pDSNode[0].pSpList->pBlock 
				!= 127 * (32 + sizeof(SPNODE)) )       
        {
            printf( "DSpaceList_Free() ≤‚ ‘”√¿˝3 ß∞‹!\n" );
        }
        break;    
	case 4:
        p = DSpaceList_Create(32, 128, 5, 64);
		for ( k = 0; k < 129; k++ )
		{
			pData = DSpaceList_Alloc(p);
		}
		DSpaceList_Free(p, pData);

        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pEmpty != &(p->pDSNode[2])
			|| p->pHead != &(p->pDSNode[1])
			|| p->pTail != p->pHead
			|| p->pDSNode[0].uInListFlag != DSPACENODE_NOT_IN_LIST
			|| p->pDSNode[1].uInListFlag != DSPACENODE_IN_LIST
			|| p->pHead->pNext != NULL
			|| p->pHead->pPrev != NULL
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 2 
			|| p->pDSNode[0].pSpList->pBlock == NULL 
			|| p->pDSNode[0].pSpList->uFreeCount != 0
			|| p->pDSNode[1].pSpList->uFreeCount != 128 )       
        {
            printf( "DSpaceList_Free() ≤‚ ‘”√¿˝4 ß∞‹!\n" );
        }
        break;
	case 5:
        p = DSpaceList_Create(32, 128, 5, 64);
		for ( k = 0; k < 128 * 64; k++ )
		{
			ppData[k] = DSpaceList_Alloc(p);
		}
		for ( k = 0; k < 128*64; k++ )
		{
			DSpaceList_Free( p, ppData[k]);
		}
        if ( p == NULL 
			|| p->pDSNode == NULL 
			|| p->pHead != &(p->pDSNode[59])
			|| p->pTail != &(p->pDSNode[63])
			|| p->pEmpty != &(p->pDSNode[58])
			|| p->uDataCount != 128
			|| p->uDataSize != 32
			|| p->uDSNodeCount != 64
			|| p->uMinDSNodeCount != 5
			|| p->uUsedDSNodeCount != 5
			|| p->pDSNode[59].pSpList->pBlock == NULL 
			|| p->pDSNode[59].pSpList->uFreeCount != 128
			|| p->pDSNode[63].pSpList->uFreeCount != 128 )
        {
            printf( "DSpaceList_Free() ≤‚ ‘”√¿˝5 ß∞‹!\n" );
        }
        break;
    default:
        break;
	}
	DSpaceList_Destroy(p);
}

void DRV_DSpaceList_Destroy(UINT i)
{
}
