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
#include "DoubleList.h"

void DRV_DoubleList_Create(UINT i);
void DRV_DoubleList_InsertHead(UINT i);
void DRV_DoubleList_InsertTail(UINT i);
void DRV_DoubleList_PopHead(UINT i);
void DRV_DoubleList_PopTail(UINT i);
void DRV_DoubleList_Find(UINT i);
void DRV_DoubleList_InsertSort(UINT i);
void DRV_DoubleList_MergeSort(UINT i);
void DRV_DoubleList_RadixSort(UINT i);

void Test_DoubleList()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_DoubleList_Create( i);
        DRV_DoubleList_InsertHead( i);
        DRV_DoubleList_InsertTail( i);
        DRV_DoubleList_PopHead( i);
        DRV_DoubleList_PopTail( i);
        DRV_DoubleList_Find( i);
        DRV_DoubleList_InsertSort( i);
        DRV_DoubleList_MergeSort( i);
        DRV_DoubleList_RadixSort( i);
    }    
}

REGISTER_TESTFUNC(Test_DoubleList)

void DRV_DoubleList_Create(UINT i)
{
    DOUBLELIST *pList;
    switch( i )
    {
    case 1:
        pList = DoubleList_Create();
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_Create() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        DoubleList_Destroy(pList, free);
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

void DRV_DoubleList_InsertHead(UINT i)
{
    DOUBLELIST *pList;
    pList = DoubleList_Create();
    switch( i )
    {
    case 1: /* ¡¥±ÌŒ™ø’ ±µƒ≤Â»Î */
        DoubleList_InsertHead(pList, strdup("1234"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_InsertHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        break;
    case 2: /* “—”–1∏ˆΩ⁄µ„µƒ≤Â»Î */
        DoubleList_InsertHead(pList, strdup("1234"));
        DoubleList_InsertHead(pList, strdup("5678"));
        if ( strcmp((char *)(pList->pHead->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "1234") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 2 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_InsertHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3: /* “—”–¡Ω∏ˆΩ⁄µ„µƒ≤Â»Î */
        DoubleList_InsertHead(pList, strdup("1234"));
        DoubleList_InsertHead(pList, strdup("5678"));
        DoubleList_InsertHead(pList, strdup("910"));
        if ( strcmp((char *)(pList->pHead->pData), "910") != 0 
            || strcmp((char *)(pList->pHead->pNext->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "1234") != 0 
            || pList->pTail != pList->pHead->pNext->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 3 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        break;
    case 4:
        break;
    default:
        break;
    }
    DoubleList_Destroy(pList, free);
}

void DRV_DoubleList_InsertTail(UINT i)
{
    DOUBLELIST *pList;
    pList = DoubleList_Create();
    switch( i )
    {
    case 1: /* ¡¥±ÌŒ™ø’ ±µƒ≤Â»Î */
        DoubleList_InsertTail(pList, strdup("1234"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_InsertHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        break;
    case 2: /* “—”–1∏ˆΩ⁄µ„µƒ≤Â»Î */
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pTail->pData), "5678") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 2 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_InsertHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3: /* “—”–¡Ω∏ˆΩ⁄µ„µƒ≤Â»Î */
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        DoubleList_InsertTail(pList, strdup("910"));
        if ( strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pHead->pNext->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "910") != 0 
            || pList->pTail != pList->pHead->pNext->pNext 
            || pList->pTail->pNext != NULL
            || pList->uCount != 3 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_InsertHead() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        break;
    case 4:
        break;
    default:
        break;
    }
    DoubleList_Destroy(pList, free);
}

void DRV_DoubleList_PopHead(UINT i)
{
    DOUBLELIST *pList;
    void       *pData;
    pList = DoubleList_Create();
    switch( i )
    {
    case 1:
        DoubleList_InsertTail(pList, strdup("1234"));

        pData = DoubleList_PopHead(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopHead() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 2:
        pData = DoubleList_PopHead(pList);
        if ( pData != NULL
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopHead() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3:
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        
        pData = DoubleList_PopHead(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || strcmp((char *)(pList->pHead->pData), "5678") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopHead() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        free(pData);
        break; 
    case 4:
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        DoubleList_InsertTail(pList, strdup("910"));
        
        pData = DoubleList_PopHead(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || strcmp((char *)(pList->pHead->pData), "5678") != 0 
            || strcmp((char *)(pList->pTail->pData), "910") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 2
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopHead() ≤‚ ‘”√¿˝4  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 5:
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        DoubleList_InsertTail(pList, strdup("910"));
        
        pData = DoubleList_PopHead(pList);
        free(pData);
        pData = DoubleList_PopHead(pList);
        if ( strcmp((char *)pData, "5678") != 0 
            || strcmp((char *)(pList->pHead->pData), "910") != 0 
            || strcmp((char *)(pList->pTail->pData), "910") != 0 
            || pList->pTail != pList->pHead 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 1
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopHead() ≤‚ ‘”√¿˝5  ß∞‹!\n" );
        }
        free(pData);
        break;
    default:
        break;
    }
    DoubleList_Destroy(pList, free);
}

void DRV_DoubleList_PopTail(UINT i)
{
    DOUBLELIST *pList;
    void       *pData;
    pList = DoubleList_Create();
    switch( i )
    {
    case 1:
        DoubleList_InsertTail(pList, strdup("1234"));
        
        pData = DoubleList_PopTail(pList);
        if ( strcmp((char *)pData, "1234") != 0 
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopTail() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 2:
        pData = DoubleList_PopTail(pList);
        if ( pData != NULL
            || pList->pTail != NULL 
            || pList->pHead != NULL
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopTail() ≤‚ ‘”√¿˝2  ß∞‹!\n" );
        }
        break;
    case 3:
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        
        pData = DoubleList_PopTail(pList);
        if ( strcmp((char *)pData, "5678") != 0 
            || strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pHead->pNext != NULL
            || pList->uCount != 1 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopTail() ≤‚ ‘”√¿˝3  ß∞‹!\n" );
        }
        free(pData);
        break; 
    case 4:
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        DoubleList_InsertTail(pList, strdup("910"));
        
        pData = DoubleList_PopTail(pList);
        if ( strcmp((char *)pData, "910") != 0 
            || strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pTail->pData), "5678") != 0 
            || pList->pTail != pList->pHead->pNext 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 2
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopTail() ≤‚ ‘”√¿˝4  ß∞‹!\n" );
        }
        free(pData);
        break;
    case 5:
        DoubleList_InsertTail(pList, strdup("1234"));
        DoubleList_InsertTail(pList, strdup("5678"));
        DoubleList_InsertTail(pList, strdup("910"));
        
        pData = DoubleList_PopTail(pList);
        free(pData);
        pData = DoubleList_PopTail(pList);
        if ( strcmp((char *)pData, "5678") != 0 
            || strcmp((char *)(pList->pHead->pData), "1234") != 0 
            || strcmp((char *)(pList->pTail->pData), "1234") != 0 
            || pList->pTail != pList->pHead 
            || pList->pTail->pNext != NULL 
            || pList->uCount != 1
            || pList->pCur != NULL )
        {
            printf( "DoubleList_PopTail() ≤‚ ‘”√¿˝5  ß∞‹!\n" );
        }
        free(pData);
        break;
    default:
        break;
    }
    DoubleList_Destroy(pList, free);
}

void DRV_DoubleList_Find(UINT i)
{
    DOUBLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = DoubleList_Create();
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->uCount != 0 
            || pList->pCur != NULL )
        {
            printf( "DoubleList_Create() ≤‚ ‘”√¿˝1  ß∞‹!\n" );
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
    DoubleList_Destroy(pList, free);
}


DOUBLELIST *BuildSortCase1()
{
    DOUBLELIST *pList = DoubleList_Create();

    if ( pList != NULL )
    {
        DoubleList_InsertTail( pList, strdup("19"));
        DoubleList_InsertTail( pList, strdup("20"));
        DoubleList_InsertTail( pList, strdup("22"));
        DoubleList_InsertTail( pList, strdup("23"));
        DoubleList_InsertTail( pList, strdup("21"));
        DoubleList_InsertTail( pList, strdup("18"));
        DoubleList_InsertTail( pList, strdup("11"));
        DoubleList_InsertTail( pList, strdup("10"));
        DoubleList_InsertTail( pList, strdup("17"));
        DoubleList_InsertTail( pList, strdup("15"));
        DoubleList_InsertTail( pList, strdup("12"));
        DoubleList_InsertTail( pList, strdup("13"));
        DoubleList_InsertTail( pList, strdup("16"));
        DoubleList_InsertTail( pList, strdup("14"));
        pList->pCur = pList->pHead->pNext->pNext->pNext->pNext->pNext->pNext->pNext->pNext->pNext;
    }
    return pList;
}

DOUBLELIST *BuildSortCase2()
{
    DOUBLELIST *pList = DoubleList_Create();
    
    if ( pList != NULL )
    {
        DoubleList_InsertTail( pList, strdup("19"));
        DoubleList_InsertTail( pList, strdup("18"));
        DoubleList_InsertTail( pList, strdup("17"));
        DoubleList_InsertTail( pList, strdup("16"));
        DoubleList_InsertTail( pList, strdup("15"));
        DoubleList_InsertTail( pList, strdup("14"));
        DoubleList_InsertTail( pList, strdup("13"));
        DoubleList_InsertTail( pList, strdup("12"));
        DoubleList_InsertTail( pList, strdup("11"));
        DoubleList_InsertTail( pList, strdup("10"));
        pList->pCur = pList->pHead->pNext->pNext->pNext->pNext;
    }
    return pList;
}


DOUBLELIST *BuildSortCase3()
{
    DOUBLELIST *pList = DoubleList_Create();
    
    if ( pList != NULL )
    {
        DoubleList_InsertTail( pList, strdup("10"));
        DoubleList_InsertTail( pList, strdup("11"));
        DoubleList_InsertTail( pList, strdup("12"));
        DoubleList_InsertTail( pList, strdup("13"));
        DoubleList_InsertTail( pList, strdup("14"));
        DoubleList_InsertTail( pList, strdup("15"));
        DoubleList_InsertTail( pList, strdup("16"));
        DoubleList_InsertTail( pList, strdup("17"));
        DoubleList_InsertTail( pList, strdup("18"));
        DoubleList_InsertTail( pList, strdup("19"));
        pList->pCur = pList->pHead->pNext->pNext->pNext->pNext->pNext;
    }
    return pList;
}

INT CheckSortCase(DOUBLELIST *pList, INT nMax)
{
    DOUBLENODE  *pNode;
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

    /* ºÏ≤È«∞œÚ÷∏’Î */
    pNode = pList->pHead;

    if ( pNode->pPrev != NULL )
    {
        return 0;
    }

    while ( pNode->pNext != NULL )
    {
        if ( pNode->pNext->pPrev != pNode )
        {
            return 0;
        }
        pNode = pNode->pNext;
    }

    return 1;

}


void DRV_DoubleList_InsertSort(UINT i)
{
    DOUBLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = BuildSortCase1();
        DoubleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 24) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝1 ß∞‹\n" );
        }
        break;
    case 2:
        pList = BuildSortCase2();
        DoubleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝2 ß∞‹\n" );
        }
        break;
    case 3:
        pList = BuildSortCase3();
        DoubleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝3 ß∞‹\n" );
        }
        break;
    case 4:
        pList = DoubleList_Create();
        DoubleList_InsertHead(pList, strdup("10"));
        DoubleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 11) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝4 ß∞‹\n" );
        }
        break;
    case 5:
        pList = DoubleList_Create();
        DoubleList_InsertHead(pList, strdup("10"));
        DoubleList_InsertHead(pList, strdup("11"));
        DoubleList_InsertSort(pList, StrCompare);
        if ( CheckSortCase(pList, 12) == 0 )
        {
            printf( "InsertSort ≤‚ ‘”√¿˝5 ß∞‹\n" );
        }
        break;
    case 6:
        pList = DoubleList_Create();
        DoubleList_InsertSort(pList, StrCompare);
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
        DoubleList_Destroy(pList, free);
    }
}

void DRV_DoubleList_MergeSort(UINT i)
{
    DOUBLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = BuildSortCase1();
        DoubleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 24) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝1 ß∞‹\n" );
        }
        break;
    case 2:
        pList = BuildSortCase2();
        DoubleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝2 ß∞‹\n" );
        }
        break;
    case 3:
        pList = BuildSortCase3();
        DoubleList_MergeSort(pList, StrCompare, 2);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝3 ß∞‹\n" );
        }
        break;
    case 4:
        pList = DoubleList_Create();
        DoubleList_InsertHead(pList, strdup("10"));
        DoubleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 11) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝4 ß∞‹\n" );
        }
        break;
    case 5:
        pList = DoubleList_Create();
        DoubleList_InsertHead(pList, strdup("10"));
        DoubleList_InsertHead(pList, strdup("11"));
        DoubleList_MergeSort(pList, StrCompare, 0);
        if ( CheckSortCase(pList, 12) == 0 )
        {
            printf( "MergeSort ≤‚ ‘”√¿˝5 ß∞‹\n" );
        }
        break;
    case 6:
        pList = DoubleList_Create();
        DoubleList_MergeSort(pList, StrCompare, 0);
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
        DoubleList_Destroy(pList, free);
    }
}

void DRV_DoubleList_RadixSort(UINT i)
{
    DOUBLELIST *pList = NULL;
    switch( i )
    {
    case 1:
        pList = BuildSortCase1();
        DoubleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 24) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝1 ß∞‹\n" );
        }
        break;
    case 2:
        pList = BuildSortCase2();
        DoubleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝2 ß∞‹\n" );
        }
        break;
    case 3:
        pList = BuildSortCase3();
        DoubleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 20) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝3 ß∞‹\n" );
        }
        break;
    case 4:
        pList = DoubleList_Create();
        DoubleList_InsertHead(pList, strdup("10"));
        DoubleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 11) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝4 ß∞‹\n" );
        }
        break;
    case 5:
        pList = DoubleList_Create();
        DoubleList_InsertHead(pList, strdup("10"));
        DoubleList_InsertHead(pList, strdup("11"));
        DoubleList_RadixSort(pList, 256, 5, GetStrKey);
        if ( CheckSortCase(pList, 12) == 0 )
        {
            printf( "RadixSort ≤‚ ‘”√¿˝5 ß∞‹\n" );
        }
        break;
    case 6:
        pList = DoubleList_Create();
        DoubleList_RadixSort(pList, 256, 5, GetStrKey);
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
        DoubleList_Destroy(pList, free);
    }
}