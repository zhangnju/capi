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
#include <stdlib.h>
#include <time.h>
#include "TestApi.h"
#include "CRankList.h"
#include "Parallel_BubbleSort.h"

void TestCase_CRankList_RankSort(void);
void TestCase_CRankList_RankSortPer(void);


void Test_CRankList()
{
    TestCase_Add(TestCase_CRankList_RankSort);
    TestCase_Add(TestCase_CRankList_RankSortPer);
}

REGISTER_TESTFUNC(Test_CRankList)


void TestCase_CRankList_RankSort(void)
{
    CRankList<int>   ranklist(16);

    int a[16] = {86, 69, 57, 9, 66, 13, 122, 54, 47, 43, 49, 32, 0, 69, 119, 79};

  
    ranklist.RankSort(a, sizeof(a)/sizeof(int), 4, 8);


    int i;
    printf("RankSort begin, result as following:\n");
    for ( i = 0; i < sizeof(a)/sizeof(int); i++ )
    {
        printf("a[%ld] = %ld\n", i, a[i]);
    }
}



void TestCase_CRankList_RankSortPer(void)
{
    clock_t     t1, t2;
    int i;

    int count = 1024*1024*4;

    CRankList<int>   ranklist(count);

    int * a = new int[count];

    UINT uRand = (UINT)time(NULL);

    srand(uRand);

    for ( i = 0; i < count; i++ )
    {
        a[i] = (((unsigned int )rand()) & ( (1<<24) -1 ));
    }
    

    t1 = clock();
    ranklist.RankSort(a, count, 22, 4);
    t2 = clock();

    printf("RankSort 0x%lx, %ld, Total Time: %ld ms\n", count, count, t2-t1);

    //check result order
    for ( i = 0; i < count-1; i++ )
    {
        if ( a[i] > a[i+1] )
        {
            printf( "Result Error: a[%d] = %ld, a[%d] = %ld\n", i, a[i], i+1, a[i+1]);
        }
    }

    delete[] a;
}



