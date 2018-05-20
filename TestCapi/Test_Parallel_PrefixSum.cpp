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
// Test_Parallel_PrefixSum.cpp
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Testapi.h"
#include "CapiGlobal.h"
#include "Parallel_PrefixSum.h"
#include "CapiCommon.h"

void TestCase_Parallel_PrefixSum(void);

void Test_Parallel_PrefixSum()
{
    TestCase_Add(TestCase_Parallel_PrefixSum);
}

REGISTER_TESTFUNC(Test_Parallel_PrefixSum)


void TestCase_Parallel_PrefixSum(void)
{
    int i;
//    clock_t t1,t2;

    int a[17] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
    int b[17];

    Sequential_PrefixSum(a, b, sizeof(a)/sizeof(int));

    for ( i = 0; i < sizeof(a)/sizeof(int); i++ )
    {
        printf("b[%d] = %ld\n", i, b[i]);
    }

    int c[17];

    Parallel_PrefixSum(a, c, sizeof(a)/sizeof(int));
    for ( i = 0; i < sizeof(a)/sizeof(int); i++ )
    {
        assertTrue(c[i] == b[i]);
    }
}


