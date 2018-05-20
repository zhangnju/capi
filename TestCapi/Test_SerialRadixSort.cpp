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
#include "SerialRadixSort.h"

extern UINT GetDoubleByteKey( UINT uData, UINT uKeyIndex );

void TestCase_SerialRadixSort(void);
void TestCase_SerialRadixSortPer(void);

void Test_SerialRadixSort()
{
    TestCase_Add(TestCase_SerialRadixSort);
    TestCase_Add(TestCase_SerialRadixSortPer);
}

//REGISTER_TESTFUNC(Test_SerialRadixSort)



void TestCase_SerialRadixSort(void)
{
    
    int a[16] = {86, 69, 57, 9, 66, 13, 122, 54, 47, 43, 49, 32, 0, 69, 119, 79};


    Serial_RadixSort<int>(a, sizeof(a)/sizeof(int), 65536, 2, GetWordKey);

    int i;
    printf("SerialRadixSort begin, result as following:\n");
    for ( i = 0; i < sizeof(a)/sizeof(int); i++ )
    {
        printf("a[%ld] = %ld\n", i, a[i]);
    }
}



void TestCase_SerialRadixSortPer(void)
{
    clock_t     t1, t2;
    int i;

    int count = 1024*1024*4;

    int * a = new int[count];

    UINT uRand = (UINT)time(NULL);

    srand(uRand);

    for ( i = 0; i < count; i++ )
    {
        a[i] = rand();
    }

    t1 = clock();
    Serial_RadixSort<int>(a, count, 1<<16, 2, GetDoubleByteKey);
    t2 = clock();

    printf("SerialRadixSort 0x%lx, %ld, Total Time: %ld ms\n", count, count, t2-t1);

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



