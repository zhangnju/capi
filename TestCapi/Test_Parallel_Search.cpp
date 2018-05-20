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
// Test_Parallel_Search.cpp
#include <windows.h>
#include <stdio.h>
#include "Testapi.h"
#include "Parallel_Search.h"

void TestCase_Parallel_SearchData(void);
void TestCase_Parallel_SearchData2(void);
void TestCase_Parallel_SearchMaxData(void);

void Test_Parallel_Search()
{
    TestCase_Add(TestCase_Parallel_SearchData);

    TestCase_Add(TestCase_Parallel_SearchData2);

    TestCase_Add(TestCase_Parallel_SearchMaxData);
}

REGISTER_TESTFUNC(Test_Parallel_Search)



void TestCase_Parallel_SearchData(void)
{
    int i;
    int nData[16] = {0, 8, 12, 27, 43, 65, 71, 125, 5, 17, 22, 33, 41,  55, 107, 657 };
    void **ppIntData = (void **)malloc(16 * sizeof(void *));

    for ( i = 0; i < 16; i++ )
    {
        ppIntData[i] = (void *)nData[i];
    }

    int nPos = Parallel_SearchData(ppIntData, sizeof(nData)/sizeof(int), (void *)0, IntCompare);
    printf( "nPos = %ld\n", nPos );

    nPos = Parallel_SearchData(ppIntData, sizeof(nData)/sizeof(int), (void *)65, IntCompare);
    printf( "nPos = %ld\n", nPos );

 //   nPos = Parallel_Search_Data2(ppIntData, sizeof(nData)/sizeof(int), (void *)17, IntCompare);
 //   printf( "nPos = %ld\n", nPos );

    free(ppIntData);
}

void TestCase_Parallel_SearchMaxData(void)
{
    int i;
    int nData[16] = {0, 8, 12, 27, 43, 65, 71, 125, 5, 17, 22, 33, 41,  55, 107, 657 };
    void **ppIntData = (void **)malloc(16 * sizeof(void *));

    for ( i = 0; i < 16; i++ )
    {
        ppIntData[i] = (void *)nData[i];
    }

    int nPos = Parallel_SearchMaxData(ppIntData, sizeof(nData)/sizeof(int), IntCompare);
    printf( "nPos = %ld\n", nPos );
    free(ppIntData);
}

#define SEARCH_SIZE      1024*32

void TestCase_Parallel_SearchData2(void)
{
    int i;

    void **ppIntData = (void **)malloc(SEARCH_SIZE * sizeof(void *));
    if ( ppIntData == NULL )
    {
        return;
    }

    for ( i = 0; i < SEARCH_SIZE; i++ )
    {
        ppIntData[i] = (void *)(i+50);
    }

    ppIntData[20000] = (void *)44;


    int nPos = Parallel_SearchData2(ppIntData, SEARCH_SIZE, (void *)44, IntCompare);
    printf( "nPos = %ld\n", nPos );

    free(ppIntData);
}