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
#include "SpList.h"
}

void DRV_SpList_Create(UINT i);
void DRV_SpList_Destroy(UINT i);
void DRV_SpList_Alloc(UINT i);
void DRV_SpList_Free(UINT i);


void Test_SpList()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_SpList_Create(i);
//        DRV_SpList_Destroy(i);
        DRV_SpList_Alloc(i);
		DRV_SpList_Free(i);
    }    
}

REGISTER_TESTFUNC(Test_SpList)

void DRV_SpList_Create(UINT i)
{
    SPLIST *p = NULL;
    switch( i )
    {
    case 1:
        p = SpList_Create(2, 4, 0);
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
			|| p->pHead != p->pBlock
			|| p->uFreeCount != 2 
			|| p->pHead->uPos != 0
			|| p->pHead->pNext->uPos != 0)
        {           
            printf( "SpList_Create(2,4,0) ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
	case 2:
        p = SpList_Create(4, 16, 3);
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
			|| p->pHead != p->pBlock
			|| p->uFreeCount != 4 
			|| p->pHead->uPos != 3
			|| p->pHead->pNext->uPos != 3)
        {           
            printf( "SpList_Create(4,16,3) ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
	}
	SpList_Destroy(p);
}

void DRV_SpList_Alloc(UINT i)
{
    SPLIST *p = NULL;
	void *pData = NULL;
	int k = 0;
    switch( i )
    {
    case 1:
        p = SpList_Create(128, 32, 8);
		pData = SpList_Alloc(p);
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
			|| pData != (BYTE *)p->pBlock + sizeof(SPNODE)
			|| (UINT)p->pHead - (UINT)p->pBlock != sizeof(SPNODE) + 32
			|| p->uFreeCount != 127 )
        {           
            printf( "SpList_Alloc() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        p = SpList_Create(128, 32, 8);
		for ( k = 0; k < 127; k++ )
		{
			pData = SpList_Alloc(p);
			if ( p == NULL 
				|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
				|| pData != (BYTE *)p->pBlock + sizeof(SPNODE) + k * (32 + sizeof(SPNODE))
				|| (UINT)p->pHead - (UINT)p->pBlock != (k+1) * (sizeof(SPNODE) + 32)
				|| p->uFreeCount != 127 - k )
			{           
				printf( "SpList_Alloc() ≤‚ ‘”√¿˝2 ß∞‹1!, k = %d \n", k);
			}
		}
		pData = SpList_Alloc(p);
		if ( p == NULL 
			|| pData != (BYTE *)p->pBlock + sizeof(SPNODE) + k * (32 + sizeof(SPNODE))
			|| p->pHead != NULL
			|| p->uFreeCount != 0 )
		{           
			printf( "SpList_Alloc() ≤‚ ‘”√¿˝2 ß∞‹2!\n" );
		}
		pData = SpList_Alloc(p);
		if ( pData != NULL )
		{
			printf( "SpList_Alloc() ≤‚ ‘”√¿˝2 ß∞‹3 !\n" );
		}
        break;
    case 3:
        p = SpList_Create(128, 32, 8);
		for ( k = 0; k < 127; k++ )
		{
			pData = SpList_Alloc(p);
			if ( p == NULL 
				|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
				|| pData != (BYTE *)p->pBlock + sizeof(SPNODE) + k * (32 + sizeof(SPNODE))
				|| (UINT)p->pHead - (UINT)p->pBlock != (k+1) * (sizeof(SPNODE) + 32)
				|| p->uFreeCount != 127 - k )
			{           
				printf( "SpList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹1!, k = %d \n", k);
			}
		}
		pData = SpList_Alloc(p);
		if ( p == NULL 
			|| pData != (BYTE *)p->pBlock + sizeof(SPNODE) + k * (32 + sizeof(SPNODE))
			|| p->pHead != NULL
			|| p->uFreeCount != 0 )
		{           
			printf( "SpList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹2!\n" );
		}
		pData = SpList_Alloc(p);
		if ( pData != NULL )
		{
			printf( "SpList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹3 !\n" );
		}
        break;
	default:
		break;
	}
	SpList_Destroy(p);
}

void DRV_SpList_Free(UINT i)
{
    SPLIST *p = NULL;
	void *pData = NULL;
	int k = 0;
    switch( i )
    {
    case 1:
        p = SpList_Create(128, 32, 8);
		pData = SpList_Alloc(p);
		SpList_Free(p, pData);

        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
			|| p->pHead != p->pBlock
			|| p->uFreeCount != 128 
			|| p->pHead->uPos != 8
			|| p->pHead->pNext->uPos != 8)
        {           
            printf( "SpList_Free() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        p = SpList_Create(128, 32, 8);
		SpList_Alloc(p);
		SpList_Alloc(p);
		pData = SpList_Alloc(p);
		SpList_Free(p, pData);
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
			|| (BYTE *)p->pHead != (BYTE *)p->pBlock + 2 * (sizeof(SPNODE)+32)
			|| p->uFreeCount != 126 
			|| p->pHead->uPos != 8
			|| p->pHead->pNext->uPos != 8
			|| (BYTE *)p->pHead->pNext != (BYTE *)p->pHead + sizeof(SPNODE) + 32 )
        {           
            printf( "SpList_Free() ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        p = SpList_Create(128, 32, 8);
		for ( k = 0; k < 128; k++ )
		{
			pData = SpList_Alloc(p);
		}
		SpList_Free(p, (BYTE *)p->pBlock + sizeof(SPNODE) + 7 * (32 + sizeof(SPNODE)));

		if ( p == NULL 
			|| (BYTE *)p->pHead != (BYTE *)p->pBlock + 7 * (sizeof(SPNODE)+32)
			|| p->uFreeCount != 1 
			|| p->pHead->uPos != 8
			|| p->pHead->pNext != NULL )
		{           
			printf( "SpList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹2!\n" );
		}
		pData = SpList_Alloc(p);
		pData = SpList_Alloc(p);
		if ( pData != NULL )
		{
			printf( "SpList_Alloc() ≤‚ ‘”√¿˝3 ß∞‹3 !\n" );
		}
		SpList_Free(p, (BYTE *)p->pBlock + sizeof(SPNODE));
		for ( k = 1; k < 128; k++ )
		{
			SpList_Free(p, (BYTE *)p->pBlock + sizeof(SPNODE) + k * (32 + sizeof(SPNODE)));
			if ( (BYTE *)p->pHead != (BYTE *)p->pBlock + k * (32 + sizeof(SPNODE))
				|| (BYTE *)p->pHead->pNext != (BYTE *)p->pHead - 32 - sizeof(SPNODE)
				|| p->uFreeCount != k + 1)
			{
	            printf( "SpList_Free() ≤‚ ‘”√¿˝3 ß∞‹4!\n" );
			}
		}
        if ( p == NULL 
			|| (UINT)p->pBlock - (UINT)p != sizeof(SPLIST) 
			|| p->uFreeCount != 128 
			|| (BYTE *)p->pHead != (BYTE *)p->pBlock + 127*(32+sizeof(SPNODE))
			|| p->pHead->uPos != 8
			|| p->pHead->pNext->uPos != 8)
        {           
            printf( "SpList_Free() ≤‚ ‘”√¿˝3 ß∞‹5!\n" );
        }
        break;
	default:
		break;
	}
	SpList_Destroy(p);
}
