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
#include "SingleList.h"

void DRV_SingleList_Create(UINT i);
void DRV_SingleList_InsertHead(UINT i);
void DRV_SingleList_InsertTail(UINT i);
void DRV_SingleList_PopHead(UINT i);
void DRV_SingleList_PopTail(UINT i);
void DRV_SingleList_Find(UINT i);
void DRV_SingleList_InsertSort(UINT i);
void DRV_SingleList_MergeSort(UINT i);
void DRV_SingleList_RadixSort(UINT i);

void Test_SingleList()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_SingleList_Create( i);
        DRV_SingleList_InsertHead( i);
        DRV_SingleList_InsertTail( i);
        DRV_SingleList_PopHead( i);
        DRV_SingleList_PopTail( i);
        DRV_SingleList_Find( i);
        DRV_SingleList_InsertSort( i);
        DRV_SingleList_MergeSort( i);
        DRV_SingleList_RadixSort( i);
    }    
}

REGISTER_TESTFUNC(Test_SingleList)

void DRV_SingleList_Create(UINT i)
{
    SINGLELIST *pList;
    switch( i )
    {
    case 1:
        pList = SingleList_Create();
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "SingleList_Create() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        SingleList_Destroy(pList, free);
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }
}

void DRV_SingleList_InsertHead(UINT i)
{
    SINGLELIST *pList;
    pList = SingleList_Create();
    switch( i )
    {
    case 1: /* ¡¥±ÌŒ™ø’ ±µƒ≤Â»Î */
        SingleList_InsertHead(pList, strdup("1234"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "SingleList_InsertHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        break;
    case 2: /* “—”–1∏ˆΩ⁄µ„µƒ≤Â»Î */
        SingleList_InsertHead(pList, strdup("1234"));
        SingleList_InsertHead(pList, strdup("5678"));
        if ( strcmp((char *)(pList->pHead->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "1234") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 2 
            || pList->pCur != NULL )
        {
            printf( "SingleList_InsertHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3: /* “—”–¡Ω∏ˆΩ⁄µ„µƒ≤Â»Î */
        SingleList_InsertHead(pList, strdup("1234"));
        SingleList_InsertHead(pList, strdup("5678"));
        SingleList_InsertHead(pList, strdup("910"));
        if ( strcmp((char *)(pList->pHead->pData), "910") != 0 
            || strcmp((char *)(pList->pHead->pNext->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "1234") != 0 
            || pList->pTail != pList->pHead->pNext->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 3 
            || pList->pCur != NULL )
        {
            printf( "SingleList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        break;
    case 4:
        break;
    default:
        break;
    }
    SingleList_Destroy(pList, free);
}

void DRV_SingleList_InsertTail(UINT i)
{
    SINGLELIST *pList;
    pList = SingleList_Create();
    switch( i )
    {
    case 1: /* ¡¥±ÌŒ™ø’ ±µƒ≤Â»Î */
        SingleList_InsertTail(pList, strdup("1234"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "SingleList_InsertHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        break;
    case 2: /* “—”–1∏ˆΩ⁄µ„µƒ≤Â»Î */
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pTail->pData), "5678") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 2 
            || pList->pCur != NULL )
        {
            printf( "SingleList_InsertHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3: /* “—”–¡Ω∏ˆΩ⁄µ„µƒ≤Â»Î */
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        SingleList_InsertTail(pList, strdup("910"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pHead->pNext->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "910") != 0 
            || pList->pTail != pList->pHead->pNext->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 3 
            || pList->pCur != NULL )
        {
            printf( "SingleList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        break;
    case 4:
        break;
    default:
        break;
    }
    SingleList_Destroy(pList, free);
}

void DRV_SingleList_PopHead(UINT i)
{
    SINGLELIST *pList;
    void       *pData;
    pList = SingleList_Create();
    switch( i )
    {
    case 1:
        SingleList_InsertTail(pList, strdup("1234"));

        pData = SingleList_PopHead(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 2:
        pData = SingleList_PopHead(pList);
        if ( pData != NULL
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3:
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        
        pData = SingleList_PopHead(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || strcmp((char *)(pList->pHead->pData), "5678") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopHead() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        free(pData);
        break; 
    case 4:
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        SingleList_InsertTail(pList, strdup("910"));
        
        pData = SingleList_PopHead(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || strcmp((char *)(pList->pHead->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "910") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 2
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopHead() ≤‚ ‘”√¿˝4  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 5:
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        SingleList_InsertTail(pList, strdup("910"));
        
        pData = SingleList_PopHead(pList);
        free(pData);
        pData = SingleList_PopHead(pList);
        if ( strcmp((char *)pData, "5678") != 0 
            || strcmp((char *)(pList->pHead->pData), "910") != 0 
            || strcmp((char *)(pList->pTail->pData), "910") != 0 
            || pList->pTail != pList->pHead 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 1
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopHead() ≤‚ ‘”√¿˝5  ß∞‹!\n" );
        }
        free(pData);
        break;
    default:
        break;
    }
    SingleList_Destroy(pList, free);
}

void DRV_SingleList_PopTail(UINT i)
{
    SINGLELIST *pList;
    void       *pData;
    pList = SingleList_Create();
    switch( i )
    {
    case 1:
        SingleList_InsertTail(pList, strdup("1234"));
        
        pData = SingleList_PopTail(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopTail() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 2:
        pData = SingleList_PopTail(pList);
        if ( pData != NULL
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopTail() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3:
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        
        pData = SingleList_PopTail(pList);
        if ( strcmp((char *)pData, "5678") != 0 
            || strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopTail() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        free(pData);
        break; 
    case 4:
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        SingleList_InsertTail(pList, strdup("910"));
        
        pData = SingleList_PopTail(pList);
        if ( strcmp((char *)pData, "910") != 0 
            || strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pTail->pData), "5678") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 2
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopTail() ≤‚ ‘”√¿˝4  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 5:
        SingleList_InsertTail(pList, strdup("1234"));
        SingleList_InsertTail(pList, strdup("5678"));
        SingleList_InsertTail(pList, strdup("910"));
        
        pData = SingleList_PopTail(pList);
        free(pData);
        pData = SingleList_PopTail(pList);
        if ( strcmp((char *)pData, "5678") != 0 
            || strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pTail->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 1
            || pList->pCur != NULL )
        {
            printf( "SingleList_PopTail() ≤‚ ‘”√¿˝5  ß∞‹!\n" );
        }
        free(pData);
        break;
    default:
        break;
    }
    SingleList_Destroy(pList, free);
}

void DRV_SingleList_Find(UINT i)
{
    SINGLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = SingleList_Create();
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "SingleList_Create() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        break;
    case 2:
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }
    SingleList_Destroy(pList, free);
}


SINGLELIST *BuildSortCase1()
{
    SINGLELIST *pList = SingleList_Create();

    if ( pList != NULL )
    {
        SingleList_InsertTail( pList, strdup("19"));
        SingleList_InsertTail( pList, strdup("20"));
        SingleList_InsertTail( pList, strdup("22"));
        SingleList_InsertTail( pList, strdup("23"));
        SingleList_InsertTail( pList, strdup("21"));
        SingleList_InsertTail( pList, strdup("18"));
        SingleList_InsertTail( pList, strdup("11"));
        SingleList_InsertTail( pList, strdup("10"));
        SingleList_InsertTail( pList, strdup("17"));
        SingleList_InsertTail( pList, strdup("15"));
        SingleList_InsertTail( pList, strdup("12"));
        SingleList_InsertTail( pList, strdup("13"));
        SingleList_InsertTail( pList, strdup("16"));
        SingleList_InsertTail( pList, strdup("14"));
        pList->pCur = pList->pHead->pNext->pNext->pNext->pNext->pNext->pNext->pNext->pNext->pNext;
    }
    return pList;
}

SINGLELIST *BuildSortCase2()
{
    SINGLELIST *pList = SingleList_Create();
    
    if ( pList != NULL )
    {
        SingleList_InsertTail( pList, strdup("19"));
        SingleList_InsertTail( pList, strdup("18"));
        SingleList_InsertTail( pList, strdup("17"));
        SingleList_InsertTail( pList, strdup("16"));
        SingleList_InsertTail( pList, strdup("15"));
        SingleList_InsertTail( pList, strdup("14"));
        SingleList_InsertTail( pList, strdup("13"));
        SingleList_InsertTail( pList, strdup("12"));
        SingleList_InsertTail( pList, strdup("11"));
        SingleList_InsertTail( pList, strdup("10"));
        pList->pCur = pList->pHead->pNext->pNext->pNext->pNext;
    }
    return pList;
}


SINGLELIST *BuildSortCase3()
{
    SINGLELIST *pList = SingleList_Create();
    
    if ( pList != NULL )
    {
        SingleList_InsertTail( pList, strdup("10"));
        SingleList_InsertTail( pList, strdup("11"));
        SingleList_InsertTail( pList, strdup("12"));
        SingleList_InsertTail( pList, strdup("13"));
        SingleList_InsertTail( pList, strdup("14"));
        SingleList_InsertTail( pList, strdup("15"));
        SingleList_InsertTail( pList, strdup("16"));
        SingleList_InsertTail( pList, strdup("17"));
        SingleList_InsertTail( pList, strdup("18"));
        SingleList_InsertTail( pList, strdup("19"));
        pList->pCur = pList->pHead->pNext->pNext->pNext->pNext->pNext;
    }
    return pList;
}

INT CheckSortCase(SINGLELIST *pList, INT nMax)
{
    SINGLENODE  *pNode;
    char        psz[16];
    char        *pszSrc;

    INT         n = 10;

    itoa(n, psz, 10);

    pNode = pList->pHead;

    pszSrc = (char *)(pNode->pData);

    while ( strcmp(pszSrc, psz) == 0 )
    {
        n += 1;
        itoa(n, psz, 10);
        pNode = pNode->pNext;
        if ( pNode != NULL )
        {
            pszSrc = (char *)(pNode->pData);
        }
        else
        {
            break;
        }
    }

    if ( n < nMax )
    {
        return 0;
    }

    itoa(nMax -1 , psz, 10);
    if ( strcmp((char *)(pList->pTail->pData), psz) != 0 )
    {
        return 0;
    }

    if ( pList->pCur != NULL && strcmp((char *)(pList->pCur->pData), "15" ) != 0 )
    {
        return 0;
    }

    if ( pList->uCount != (UINT)(nMax - 10) )
    {
        return 0;
    }

    return 1;

}


void DRV_SingleList_InsertSort(UINT i)
{
    SINGLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = BuildSortCase1();
        SingleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 24) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝1 ß∞‹\n" );
        }
        break;
    case 2:
        pList = BuildSortCase2();
        SingleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝2 ß∞‹\n" );
        }
        break;
    case 3:
        pList = BuildSortCase3();
        SingleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝3 ß∞‹\n" );
        }
        break;
    case 4:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        SingleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 11) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝4 ß∞‹\n" );
        }
        break;
    case 5:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        SingleList_InsertHead(pList, strdup("11"));
        SingleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 12) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝5 ß∞‹\n" );
        }
        break;
    case 6:
        pList = SingleList_Create();
        SingleList_InsertSort(pList, StrCompare);
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->pCur != NULL 
            || pList->uCount != 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝6 ß∞‹\n" );
        }
        break;
    default:
        break;
    }
    if ( pList != NULL )
    {
        SingleList_Destroy(pList, free);
    }
}

void DRV_SingleList_MergeSort(UINT i)
{
    SINGLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = BuildSortCase1();
        SingleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 24) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝1 ß∞‹\n" );
        }
        break;
    case 2:
        pList = BuildSortCase2();
        SingleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝2 ß∞‹\n" );
        }
        break;
    case 3:
        pList = BuildSortCase3();
        SingleList_MergeSort(pList, StrCompare, 2);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝3 ß∞‹\n" );
        }
        break;
    case 4:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        SingleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 11) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝4 ß∞‹\n" );
        }
        break;
    case 5:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        SingleList_InsertHead(pList, strdup("11"));
        SingleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 12) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝5 ß∞‹\n" );
        }
        break;
    case 6:
        pList = SingleList_Create();
        SingleList_MergeSort(pList, StrCompare, 0);
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->pCur != NULL 
            || pList->uCount != 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝6 ß∞‹\n" );
        }
        break;
    default:
        break;
    }
    if ( pList != NULL )
    {
        SingleList_Destroy(pList, free);
    }
}

void DRV_SingleList_RadixSort(UINT i)
{
    SINGLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = BuildSortCase1();
        SingleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 24) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝1 ß∞‹\n" );
        }
        break;
    case 2:
        pList = BuildSortCase2();
        SingleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝2 ß∞‹\n" );
        }
        break;
    case 3:
        pList = BuildSortCase3();
        SingleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝3 ß∞‹\n" );
        }
        break;
    case 4:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        SingleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 11) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝4 ß∞‹\n" );
        }
        break;
    case 5:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        SingleList_InsertHead(pList, strdup("11"));
        SingleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 12) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝5 ß∞‹\n" );
        }
        break;
    case 6:
        pList = SingleList_Create();
        SingleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->pCur != NULL 
            || pList->uCount != 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝6 ß∞‹\n" );
        }
        break;
    default:
        break;
    }
    if ( pList != NULL )
    {
        SingleList_Destroy(pList, free);
    }
}