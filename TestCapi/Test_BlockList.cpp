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
extern "C" {
#include "BlockList.h"
}

void DRV_BlockList_Create(UINT i);
void DRV_BlockList_Alloc(UINT i);
void DRV_BlockList_Free(UINT i);
void DRV_BlockList_InsertHead(UINT i);
void DRV_BlockList_DeleteHead(UINT i);


void Test_BlockList()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_BlockList_Create(i);
        DRV_BlockList_Alloc(i);
		DRV_BlockList_Free(i);
		DRV_BlockList_InsertHead(i);
		DRV_BlockList_DeleteHead(i);
    }    
}

REGISTER_TESTFUNC(Test_BlockList)

void DRV_BlockList_Create(UINT i)
{
    BLOCKLIST *p = NULL;
    switch( i )
    {
    case 1:
        p = BlockList_Create(4, 2);
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
			|| p->pEmpty != p->pBlock
			|| p->pHead != NULL
			|| p->uFreeCount != 2 
			|| p->uDataSize != 4
			|| p->uMaxDataCount != 2
			|| (BYTE *)p->pEmpty->pNext != (BYTE *)p->pEmpty + 4 + sizeof(SINGLENODE))
        {           
            printf( "BlockList_Create(4, 2) ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
	case 2:
        p = BlockList_Create(16, 4);
        if ( p == NULL 
			|| p->pHead != NULL
			|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
			|| p->pEmpty != p->pBlock
			|| p->uFreeCount != 4 
			|| p->uDataSize != 16
			|| p->uMaxDataCount != 4 )
        {           
            printf( "BlockList_Create(16, 4) ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
	}
	BlockList_Destroy(p);
}

void DRV_BlockList_Alloc(UINT i)
{
    BLOCKLIST *p = NULL;
	void *pData = NULL;
	int k = 0;
    p = BlockList_Create(32, 128);

    switch( i )
    {
    case 1:
		pData = BlockList_Alloc(p);
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
			|| pData != (BYTE *)p->pBlock
			|| (UINT)p->pEmpty - (UINT)p->pBlock != sizeof(SINGLENODE) + 32
			|| p->uFreeCount != 127 )
        {           
            printf( "BlockList_Alloc() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
		for ( k = 0; k < 127; k++ )
		{
			pData = BlockList_Alloc(p);
			if ( p == NULL 
				|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
				|| pData != (BYTE *)p->pBlock + k * (32 + sizeof(SINGLENODE))
				|| (UINT)p->pEmpty - (UINT)p->pBlock != (k+1) * (sizeof(SINGLENODE) + 32) )
			{           
				printf( "BlockList_Alloc() ≤‚ ‘”√¿˝2 ß∞‹1!, k = %d \n", k);
			}
		}
		pData = BlockList_Alloc(p);
		if ( p == NULL 
			|| pData != (BYTE *)p->pBlock + k * (32 + sizeof(SINGLENODE))
			|| p->pEmpty != NULL
			|| p->uFreeCount != 0 )
		{           
			printf( "BlockList_Alloc() ≤‚ ‘”√¿˝2 ß∞‹2!\n" );
		}
		pData = BlockList_Alloc(p);
		if ( pData != NULL )
		{
			printf( "BlockList_Alloc() ≤‚ ‘”√¿˝2 ß∞‹3 !\n" );
		}
        break;
    case 3:
		for ( k = 0; k < 127; k++ )
		{
			pData = BlockList_Alloc(p);
			if ( p == NULL 
				|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
				|| pData != (BYTE *)p->pBlock + k * (32 + sizeof(SINGLENODE))
				|| (UINT)p->pEmpty - (UINT)p->pBlock != (k+1) * (sizeof(SINGLENODE) + 32)
				|| p->uFreeCount != 127 - k )
			{           
				printf( "BlockList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹1!, k = %d \n", k);
			}
		}
		pData = BlockList_Alloc(p);
		if ( p == NULL 
			|| pData != (BYTE *)p->pBlock + k * (32 + sizeof(SINGLENODE))
			|| p->pEmpty != NULL
			|| p->uFreeCount != 0 )
		{           
			printf( "BlockList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹2!\n" );
		}
		pData = BlockList_Alloc(p);
		if ( pData != NULL )
		{
			printf( "BlockList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹3 !\n" );
		}
        break;
	default:
		break;
	}
	BlockList_Destroy(p);
}

void DRV_BlockList_Free(UINT i)
{
    BLOCKLIST *p = NULL;
	void *pData = NULL;
	int k = 0;
    p = BlockList_Create(32, 128);
    switch( i )
    {
    case 1:
		pData = BlockList_Alloc(p);
		BlockList_Free(p, pData);

        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
			|| p->pEmpty != p->pBlock
			|| p->uFreeCount != 128 )
        {           
            printf( "BlockList_Free() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
		BlockList_Alloc(p);
		BlockList_Alloc(p);
		pData = BlockList_Alloc(p);
		BlockList_Free(p, pData);
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
			|| (BYTE *)p->pEmpty != (BYTE *)p->pBlock + 2 * (sizeof(SINGLENODE)+32)
			|| p->uFreeCount != 126 
			|| (BYTE *)p->pEmpty->pNext != (BYTE *)p->pEmpty + sizeof(SINGLENODE) + 32 )
        {           
            printf( "BlockList_Free() ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
		for ( k = 0; k < 128; k++ )
		{
			pData = BlockList_Alloc(p);
		}
		BlockList_Free(p, (BYTE *)p->pBlock + 7 * (32 + sizeof(SINGLENODE)));

		if ( p == NULL 
			|| (BYTE *)p->pEmpty != (BYTE *)p->pBlock + 7 * (sizeof(SINGLENODE)+32)
			|| p->uFreeCount != 1 
			|| p->pEmpty->pNext != NULL )
		{           
			printf( "BlockList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹2!\n" );
		}
		pData = BlockList_Alloc(p);
		pData = BlockList_Alloc(p);
		if ( pData != NULL )
		{
			printf( "BlockList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹3 !\n" );
		}
		BlockList_Free(p, (BYTE *)p->pBlock);
		for ( k = 1; k < 128; k++ )
		{
			BlockList_Free(p, (BYTE *)p->pBlock + k * (32 + sizeof(SINGLENODE)));
			if ( (BYTE *)p->pEmpty != (BYTE *)p->pBlock + k * (32 + sizeof(SINGLENODE))
				|| (BYTE *)p->pEmpty->pNext != (BYTE *)p->pEmpty - 32 - sizeof(SINGLENODE)
				|| p->uFreeCount != k + 1)
			{
	            printf( "BlockList_Free() ≤‚ ‘”√¿˝3 ß∞‹4!\n" );
			}
		}
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(BLOCKLIST) 
			|| p->uFreeCount != 128 
			|| (BYTE *)p->pEmpty != (BYTE *)p->pBlock + 127*(32+sizeof(SINGLENODE)) )
        {           
            printf( "BlockList_Free() ≤‚ ‘”√¿˝3 ß∞‹5!\n" );
        }
        break;
	default:
		break;
	}
	BlockList_Destroy(p);
}

void DRV_BlockList_InsertHead(UINT i)
{
	int k = 0;
    BLOCKLIST *pList;
	SINGLENODE	*pNode = NULL;
    pList = BlockList_Create(8, 128);
    switch( i )
    {
    case 1: /* ¡¥±ÌŒ™ø’ ±µƒ≤Â»Î */
        BlockList_InsertHead(pList, "1234", 5);
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pHead->pNext != NULL
            || pList->uFreeCount != 127 
			|| pList->pHead != pList->pBlock
            || (BYTE *)pList->pEmpty != (BYTE *)pList->pBlock + 8 + sizeof(SINGLENODE) )
        {
            printf( "BlockList_InsertHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        break;
    case 2: /* “—”–1∏ˆΩ⁄µ„ ±µƒ≤Â»Î */
        BlockList_InsertHead(pList, "1234", 5);
        BlockList_InsertHead(pList, "5678", 5);
        if ( strcmp((char *)(pList->pHead->pData), "5678") != 0 
            || strcmp((char *)(pList->pHead->pNext->pData), "1234") != 0 
            || pList->uFreeCount != 126 
            || (BYTE *)pList->pHead != (BYTE *)pList->pBlock + 8 + sizeof(SINGLENODE) 
			|| (BYTE *)pList->pEmpty != (BYTE *)pList->pHead + 8 + sizeof(SINGLENODE) )
        {
            printf( "BlockList_InsertHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3: /* 128∏ˆΩ⁄µ„µƒ≤Â»Î */
		for ( k = 0; k < 128; k++ )
		{
			BlockList_InsertHead(pList, "1234", 5);
		}
		pNode = pList->pHead;
		for ( k = 0; k < 128 && pNode != NULL; k++ )
		{
			if ( strcmp((char *)pNode->pData, "1234") != 0 
				|| (BYTE *)pNode != (BYTE *)pList->pBlock + (127-k) * ( 8 + sizeof(SINGLENODE)) )
			{
				printf( "BlockList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹1!\n");
			}
			pNode = pNode->pNext;
		}
        if ( pList->uFreeCount != 0 
            || pList->pEmpty != NULL 
			|| pList->uDataSize != 8
			|| pList->uMaxDataCount != 128 )
        {
            printf( "BlockList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹2!\n" );
        }
        if ( BlockList_InsertHead(pList, "5678", 5) != -1 )
		{
            printf( "BlockList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹3!\n" );
		}
        break;
    case 4:
        break;
    default:
        break;
    }
	BlockList_Destroy(pList);
}

void DRV_BlockList_DeleteHead(UINT i)
{
	int k = 0;
    BLOCKLIST *pList;
	SINGLENODE	*pNode = NULL;
    pList = BlockList_Create(8, 128);
    switch( i )
    {
    case 1: /* ¡¥±ÌŒ™ø’ ±µƒ…æ≥˝ */
		BlockList_DeleteHead(pList);
        if ( pList->pHead != NULL
            || pList->uFreeCount != 128 
			|| pList->pEmpty != pList->pBlock )
        {
            printf( "BlockList_DeleteHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        break;
    case 2: /* “—”–1∏ˆΩ⁄µ„ ±µƒ…æ≥˝ */
        BlockList_InsertHead(pList, "1234", 5);
        BlockList_DeleteHead(pList);
        if ( pList->pHead != NULL
            || pList->uFreeCount != 128 
			|| pList->pEmpty != pList->pBlock )
        {
            printf( "BlockList_DeleteHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3: /* “—”–128∏ˆΩ⁄µ„ ±µƒ…æ≥˝ */
		for ( k = 0; k < 128; k++ )
		{
			BlockList_InsertHead(pList, "1234", 5);
		}
		for ( k = 0; k < 127; k++ )
		{
			BlockList_DeleteHead(pList);
			if ( (UINT)pList->pEmpty - (UINT)pList->pBlock != (127-k) * (sizeof(SINGLENODE)+8)
				|| (UINT)pList->pEmpty - (UINT)pList->pHead != sizeof(SINGLENODE) + 8 
				|| pList->uFreeCount != k + 1 )
			{
				printf( "BlockList_DeleteHead() ≤‚ ‘”√¿˝3  ß∞‹1!\n");
			}
		}
		BlockList_DeleteHead(pList);
		if ( pList->pEmpty != pList->pBlock 
			|| pList->pHead != NULL 
			|| pList->uFreeCount != 128 )
		{
			printf( "BlockList_DeleteHead() ≤‚ ‘”√¿˝3  ß∞‹2!\n");
		}
		BlockList_InsertHead(pList, "1234", 5);
		
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pHead->pNext != NULL
            || pList->uFreeCount != 127 
			|| pList->pHead != pList->pBlock
            || (BYTE *)pList->pEmpty != (BYTE *)pList->pBlock + 8 + sizeof(SINGLENODE) )
		{
            printf( "BlockList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹3!\n" );
		}
        break;
    case 4:
        break;
    default:
        break;
    }
	BlockList_Destroy(pList);
}
