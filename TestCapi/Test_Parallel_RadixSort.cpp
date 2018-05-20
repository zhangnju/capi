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
// Test_Parallel_RadixSort.cpp
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Testapi.h"
#include "CapiGlobal.h"
#include "CapiCommon.h"
#include "singlelist.h"
#include "ArrayList.h"
#include "Parallel_RadixSort.h"
#include "Parallel_RadixSort_Array.h"

void DRV_Parallel_RadixSort(int nCase);

void TestCase_Parallel_RadixSort(void);
void TestCase_Partitioned_RadixSort(void);
void TestCase_Parallel_RadixSort_Array(void);
void TestCase_Parallel_RadixSort_LBR(void);
void TestCase_Parallel_RadixSort_PPR(void);

void TestCase_Parallel_RadixSort_Performance(void);

void Test_Parallel_RadixSort()
{
//    TestCase_Add(TestCase_Parallel_RadixSort);
//    TestCase_Add(TestCase_Parallel_RadixSort_Array);
    TestCase_Add(TestCase_Parallel_RadixSort_LBR);
//    TestCase_Add(TestCase_Parallel_RadixSort_PPR);
//    TestCase_Add(TestCase_Parallel_RadixSort_Performance);
//    TestCase_Add(TestCase_Partitioned_RadixSort);
}

//REGISTER_TESTFUNC(Test_Parallel_RadixSort)



void TestCase_Parallel_RadixSort(void)
{
    INT i;
    for ( i = 0; i < 20; i++ )
    {
        DRV_Parallel_RadixSort(i);
    }
}
UINT GetKey8( UINT uData, UINT uKeyIndex )
{
    UINT uRet;

    uRet = (uData >> (uKeyIndex << 3 )) & 0xff;

    return uRet;   
}


/**	基数排序的关键词取位回调函数
    一次取32位整数中的16位

    @param	UINT uData - 要取位的数据	
    @param	UINT uKeyIndex - 第几个取位，为0或1，为0表示取低16位，1表示取高16位	
    @return	UINT - 返回要取位整数的低16位或高16位数据	
*/
UINT GetDoubleByteKey( UINT uData, UINT uKeyIndex )
{
    UINT uRet;

    uRet = (uData >> (uKeyIndex << 4 )) & 0xffff;

    return uRet;   
}


#if 1
UINT GetKey16( void *pData, UINT uKeyIndex )
{
    unsigned short wData;

    wData = ((int)pData >> (uKeyIndex << 4)) & 0xffff;

    return wData;   
}
#endif

SINGLELIST *RadixSort_BuildSortCase1()
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


SINGLELIST *RadixSort_BuildSortCase2()
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


SINGLELIST *RadixSort_BuildSortCase3()
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


INT RadixSort_CheckSortCase(SINGLELIST *pList, INT nMax)
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


void DRV_Parallel_RadixSort(int nCase)
{
    SINGLELIST *pList = NULL;
    switch( nCase )
    {
    case 1:
        pList = RadixSort_BuildSortCase1();
        Parallel_RadixSort(pList, 256, 5, GetStrKey);
        if ( RadixSort_CheckSortCase(pList, 24) == 0 )
        {
            printf( "RadixSort 测试用例1失败\n" );
        }
        break;
    case 2:
        pList = RadixSort_BuildSortCase2();
        Parallel_RadixSort(pList, 256, 5, GetStrKey);
        if ( RadixSort_CheckSortCase(pList, 20) == 0 )
        {
            printf( "RadixSort 测试用例2失败\n" );
        }
        break;
    case 3:
        pList = RadixSort_BuildSortCase3();
        Parallel_RadixSort(pList, 256, 5, GetStrKey);
        if ( RadixSort_CheckSortCase(pList, 20) == 0 )
        {
            printf( "RadixSort 测试用例3失败\n" );
        }
        break;
    case 4:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        Parallel_RadixSort(pList, 256, 5, GetStrKey);
        if ( RadixSort_CheckSortCase(pList, 11) == 0 )
        {
            printf( "RadixSort 测试用例4失败\n" );
        }
        break;
    case 5:
        pList = SingleList_Create();
        SingleList_InsertHead(pList, strdup("10"));
        SingleList_InsertHead(pList, strdup("11"));
        Parallel_RadixSort(pList, 256, 5, GetStrKey);
        if ( RadixSort_CheckSortCase(pList, 12) == 0 )
        {
            printf( "RadixSort 测试用例5失败\n" );
        }
        break;
    case 6:
        pList = SingleList_Create();
        Parallel_RadixSort(pList, 256, 5, GetStrKey);
        if ( pList->pHead != NULL 
            || pList->pTail != NULL 
            || pList->pCur != NULL 
            || pList->uCount != 0 )
        {
            printf( "RadixSort 测试用例6失败\n" );
        }
        break;
    default:
        break;
    }

    if ( pList != NULL )
    {	
		SINGLENODE	*pNode;

		pNode = pList->pHead;

		while ( pNode != NULL )
		{
			printf("Data = %s\n", (char *)pNode->pData);
			pNode = pNode->pNext;
		}
        SingleList_Destroy(pList, free);
    }
}


void TestCase_Parallel_RadixSort_Array(void)
{
    void **ppData = (void **)malloc(16 *sizeof(void *));


    int i;
    int nTestCount = 1000000;
    UINT uRand = (UINT)time(NULL);

    srand(uRand);
    for ( i = 0; i < nTestCount; i++ )
    {
        ppData[i] = (void *)((i * 0xb495a877 % nTestCount + 1) + 100000);
    }

    clock_t  t1, t2;

    t1 = clock();
    Parallel_RadixSort_Array(ppData, nTestCount, 65536, 2, GetKey16);

    t2 = clock();

    for ( i = 0; i < nTestCount-1; i++ )
    {
//        assertTrue( (int)ppData[i] == (100000+i+1) );
    }
    printf("ppData[5] = %ld\n", ppData[5]);
    printf("Parallel_RadixSort_Array1: time = %ld\n", t2-t1);

}


void TestCase_Parallel_RadixSort_LBR(void)
{
    int i;
    int nTestCount = 1024*1024*4;
    int * pData = new int[nTestCount];

    UINT uRand = (UINT)time(NULL);

    srand(uRand);

    for ( i = 0; i < nTestCount; i++ )
    {
        pData[i] = rand(); //(i * 0xb495a877 % nTestCount + 1) + 1000000;
    }
    clock_t  t1, t2;

    t1 = clock();

    Parallel_RadixSort_LBR<int>(pData, nTestCount, 65536, 2, GetDoubleByteKey);
    t2 = clock();

    for ( i = 0; i < nTestCount-1; i++ )
    {
        assertTrue(pData[i] <= pData[i+1]);
//        assertTrue( pData[i] == (1000000+i+1) );
        if ( pData[i] > pData[i+1] )
        {
            printf("pData[%d] = %ld, pData[%d] = %ld\n", i, pData[i], i+1, pData[i+1]);
        }
    }
    printf("pData[5] = %ld\n", pData[5]);
    printf("Parallel_RadixSort_LBR: %ld,  time = %ld\n", nTestCount, t2-t1);

	delete [] pData;
}



void TestCase_Parallel_RadixSort_PPR(void)
{
    int i;
    int nTestCount = 1024*1024*4;
    int * pData = new int[nTestCount];

    UINT uRand = (UINT)time(NULL);

    srand(uRand);

    for ( i = 0; i < nTestCount; i++ )
    {
        pData[i] = rand() * 0xb495a877 % nTestCount; //(i * 0xb495a877 % nTestCount + 1) + 1000000;
    }
    clock_t  t1, t2;

    t1 = clock();

    Parallel_RadixSort_PPR<int>(pData, nTestCount, 256, 4, GetKey8);
    t2 = clock();

    for ( i = 0; i < nTestCount-1; i++ )
    {
        assertTrue(pData[i] <= pData[i+1]);
//        assertTrue( pData[i] == (1000000+i+1) );
        if ( pData[i] > pData[i+1] )
        {
            printf("pData[%d] = %ld, pData[%d] = %ld\n", i, pData[i], i+1, pData[i+1]);
        }
    }
    printf("pData[5] = %ld\n", pData[5]);
    printf("Parallel_RadixSort_PPR: %ld,  time = %ld\n", nTestCount, t2-t1);
	delete [] pData;
}




void TestCase_Parallel_RadixSort_Performance(void)
{
	int i;
	clock_t	t1,t2;

#define   DATA_COUNT		1000000

	char *psz = (char *)malloc(DATA_COUNT * 16);

    UINT uRand = (UINT)time(NULL);

    srand(uRand);
	for ( i = 0; i < DATA_COUNT; i++ )
	{
		int a =	rand();
		itoa(a, psz + i * 16, 10);
	}

	SINGLELIST *pList = SingleList_Create();

	for ( i = 0; i < DATA_COUNT; i++ )
	{
		SingleList_InsertTail(pList, psz+i*16);
	}

	t1 = clock();
	Parallel_RadixSort(pList, 256, 10, GetStrKey);
	t2 = clock();

	printf( "parallel on million data, time = %ld\n", t2-t1);

	void **ppData = (void **)malloc(DATA_COUNT *sizeof(void *));

    uRand = (UINT)time(NULL);

    srand(uRand);
	for ( i = 0; i < DATA_COUNT; i++ )
	{
		ppData[i] = (void *)rand();
	}

	t1 = clock();
	Parallel_RadixSort_Array(ppData, DATA_COUNT, 65536, 2, GetKey16);
	t2 = clock();

    for ( i = 0; i < DATA_COUNT-1; i++ )
    {
        //assertTrue( (int)ppData[i] <= (DATA_COUNT + i + 1) );
        if ( ppData[i] > ppData[i+1] )
        {
            printf("ppData[%d] = %ld, ppData[%d] = %ld\n", i, ppData[i], i+1, ppData[i+1]);
        }
    }
    printf("ppData[100] = %ld\n", ppData[100]);
	printf( "parallel_RadixSort_Array on million data, time = %ld\n", t2-t1);
	free(psz);
	free(ppData);
}

void TestCase_Partitioned_RadixSort(void)
{
	int a[10] = {0x15, 0x22, 0x44, 0x37, 0x28, 0x58, 0x61, 0x77, 0x85, 0x96};

	void **pp = (void **)malloc(sizeof(a));

	int i;
	for ( i = 0; i < sizeof(a)/sizeof(int); i++ )
	{
		pp[i] = (void *)a[i];
	}

//	UINT	 *puBoxDataCount = (UINT *)malloc(sizeof(UINT) * 16);
//	void **ppOutData = (void **)malloc(sizeof(void *) * sizeof(a) / sizeof(int));
//	DATA_ARRAY *pArray = (DATA_ARRAY *)malloc(sizeof(DATA_ARRAY) * uRadix);
	//					  UINT *puBoxDataCount, void **ppOutData,
		//				  DATA_ARRAY *pArray);

    //Serial_Partitioned(pp, sizeof(a)/sizeof(int), 16, 0, GetIntKey, 
	free(pp);
}


