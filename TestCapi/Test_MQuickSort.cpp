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
#include <TIME.H>
#include "CapiGlobal.h"
#include "TestApi.h"
#include "TestCapi.h"

//extern "C" INT MQuickSort(void **ppData, UINT uStart, UINT uEnd, COMPAREFUNC CompareFunc);
extern "C" UINT Split(void **ppData, UINT uStart, UINT uEnd,
                     COMPAREFUNC CompareFunc);
extern "C" void QuickSort(void **ppData, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc);

extern "C" void QuickSort2(void **ppData, UINT uStart, UINT uEnd, 
                          COMPAREFUNC CompareFunc);
extern "C" void QuickSort3(void **ppData, UINT uStart, UINT uEnd, 
                          COMPAREFUNC CompareFunc);


//void TestCase_MQuickSort_1(void);
//void TestCase_MQuickSort_2(void);

void TestCase_Split(void);
void TestCase_QuickSort(void);
void TestCase_QuickSort_2(void);

void TestCase_QuickSort2(void);
void TestCase_QuickSort3(void);


void Test_MQuickSort()
{
//	TestCase_Add(TestCase_MQuickSort_1);

//	TestCase_Add(TestCase_MQuickSort_2);

	TestCase_Add(TestCase_Split);
	TestCase_Add(TestCase_QuickSort);
	TestCase_Add(TestCase_QuickSort_2);

    TestCase_Add(TestCase_QuickSort2);
    TestCase_Add(TestCase_QuickSort3);

}

REGISTER_TESTFUNC(Test_MQuickSort)

INT UIntCompare(void *p1, void *p2)
{
    if ( (UINT)p1 > (UINT)p2 )
    {
        return 1;
    }
    else if ( (UINT)p1 < (UINT)p2 )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

void TestCase_Split(void)
{
    UINT i;
    UINT uCount = 10; //10万个

    void **pp = (void **)malloc(uCount * sizeof(void *));
    for ( i = 0; i < uCount; i++ )
    {
        pp[i] = (void *)(uCount-i);
    }
    UINT uPos = Split(pp, 0, 9, IntCompare);

    assertEquals(uPos, 9);

    free(pp);
}

void TestCase_QuickSort(void)
{
    UINT i;
    UINT uCount = 1000; //10万个

    void **pp = (void **)malloc(uCount * sizeof(void *));
    for ( i = 0; i < uCount; i++ )
    {
        pp[i] = (void *)(uCount-i);
    }
    QuickSort(pp, 0, uCount-1, IntCompare);

    for ( i = 0; i < uCount; i++ )
    {
        assertEquals((UINT)(pp[i]), i+1);
    }

    free(pp);
}


void TestCase_QuickSort2(void)
{
    UINT i;
    UINT uCount = 1000; //10万个

    void **pp = (void **)malloc(uCount * sizeof(void *));
    for ( i = 0; i < uCount; i++ )
    {
        pp[i] = (void *)(uCount-i);
    }
    QuickSort2(pp, 0, uCount-1, IntCompare);

    for ( i = 0; i < uCount; i++ )
    {
        assertEquals((UINT)(pp[i]), i+1);
    }

    free(pp);
}


void TestCase_QuickSort3(void)
{
    UINT i;
    UINT uCount = 1000; //10万个

    void **pp = (void **)malloc(uCount * sizeof(void *));
    for ( i = 0; i < uCount; i++ )
    {
        pp[i] = (void *)(uCount-i);
    }
    QuickSort3(pp, 0, uCount-1, IntCompare);

    for ( i = 0; i < uCount; i++ )
    {
        assertEquals((UINT)(pp[i]), i+1);
    }

    free(pp);
}


void TestCase_QuickSort_2(void)
{
    UINT i;
    UINT uCount = 1000000; //1000000个

    UINT uRand = (UINT)time(NULL);

    srand(uRand);
    void **pp = (void **)malloc(uCount * sizeof(void *));
    for ( i = 0; i < uCount; i++ )
    {
        pp[i] = (void *)(rand() % uCount);
    }

	clock_t t1 = clock();
    QuickSort(pp, 0, uCount-1, UIntCompare);
	clock_t t2 = clock();

	printf("QuickSort 1000000 Time %ld\n", t2-t1);

    for ( i = 0; i < uCount-1; i++ )
    {
        if ( (UINT)(pp[i]) >  (UINT)(pp[i+1]) )
        {
            printf("i=%u, pp[i] = %u, pp[i+1] = %u\n", i, (UINT)pp[i], (UINT)pp[i+1]);
        }
    }
    free(pp);
}

#if 0

void TestCase_MQuickSort_1(void)
{
    UINT i;
    UINT uCount = 10000; //1000个

    void **pp = (void **)malloc(uCount * sizeof(void *));
    for ( i = 0; i < uCount; i++ )
    {
        pp[i] = (void *)(uCount-i);
    }
	clock_t t1 = clock();

    INT nRet = MQuickSort(pp, 0, uCount-1, IntCompare);
	clock_t t2 = clock();

	printf("MQuickSort1 10000 Time %ld\n", t2-t1);

    assertEquals(nRet, CAPI_SUCCESS);

    for ( i = 0; i < uCount; i++ )
    {
        assertEquals((UINT)(pp[i]), i+1);
    }
    free(pp);
}

void TestCase_MQuickSort_2(void)
{
    UINT i;
    UINT uCount = 1000000; //1000个

    srand(time(NULL));
    void **pp = (void **)malloc(uCount * sizeof(void *));
    for ( i = 0; i < uCount; i++ )
    {
        pp[i] = (void *)(rand() % uCount);
    }

	clock_t t1 = clock();
    INT nRet = MQuickSort(pp, 0, uCount-1, UIntCompare);
	clock_t t2 = clock();

	printf("MQuickSort 1000000 Time %ld\n", t2-t1);

    assertEquals(nRet, CAPI_SUCCESS);

    for ( i = 0; i < uCount-1; i++ )
    {
        if ( (UINT)(pp[i]) >  (UINT)(pp[i+1]) )
        {
            printf("i=%u, pp[i] = %u, pp[i+1] = %u\n", i, (UINT)pp[i], (UINT)pp[i+1]);
            assertLess((UINT)pp[i], (UINT)pp[i+1]);
        }
    }
    free(pp);
}

#endif
