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
#define _WIN32_WINNT 0x0401
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include "TestApi.h"
#include  "CLockFreeQueue.h"
#include "CThreadPool.h"

void TestCase_CLockFreeQueue_TestCase1(void);

void Test_CLockFreeQueue()
{
    TestCase_Add(TestCase_CLockFreeQueue_TestCase1);
}

REGISTER_TESTFUNC(Test_CLockFreeQueue)



void TestCase_CLockFreeQueue_TestCase1(void)
{
    CLockFreeQueue  q;

    q.EnQueue((void *)19);

    int a = (int)q.DeQueue();

    assertTrue( a == 19 );

}

