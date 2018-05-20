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
#include <omp.h>
#include "TestApi.h"
#include "CSegmentHashTable.h"

void TestCase_CSegmentHashTable_Insert(void);

void Test_CSegmentHashTable()
{
    TestCase_Add(TestCase_CSegmentHashTable_Insert);
}


REGISTER_TESTFUNC(Test_CSegmentHashTable)

void TestCase_CSegmentHashTable_Insert(void)
{
    CSegmentHashTable     t(1024, HashInt, IntCompare, NULL, NULL);

    t.Insert((void *)1000);

    t.Insert((void *)2024);

    void *pData = t.Find((void *)1000);

    assertTrue( pData == (void *)1000);

    pData = t.Find((void *)2024);

    assertTrue( pData == (void *)2024);


    printf("sizeof(CSegmentHashTable) = %ld\n", sizeof(t));
}


