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
#include "CStealQueue.h"
#include "CQueuePool.h"

void TestCase_CQueuePool_Constructor1(void);
void TestCase_CQueuePool_DeQueue(void);
void TestCase_CQueuePool_EnQueue(void);

void Test_CQueuePool()
{
    TestCase_Add(TestCase_CQueuePool_Constructor1);
    TestCase_Add(TestCase_CQueuePool_DeQueue);
    TestCase_Add(TestCase_CQueuePool_EnQueue);
}

REGISTER_TESTFUNC(Test_CQueuePool)

void TestCase_CQueuePool_Constructor1(void)
{
    int ret = -1;
    CQueuePool<int, CStealQueue<int>>     Q;

    Q.Create(4, 5);
    assertTrue(Q.m_nQueueCount == 4 && Q.m_nQueueSize == 5 );

    int a = 121;
    Q.Push(a);

    int b = -1;
    Q.Pop(b);

    assertTrue(b==121);

    int c = -5;
    ret = Q.Pop(c);
    assertTrue( ret == CAPI_FAILED && c == -5 );

    ret = Q.EnQueue(a, 2);
    assertTrue(ret == CAPI_SUCCESS);

    ret = Q.DeQueue(c, 3);
    assertTrue( ret == CAPI_FAILED );

    ret = Q.DeQueue(c, 2);
    assertTrue( ret == CAPI_SUCCESS && c == 121 );
    

}



void TestCase_CQueuePool_DeQueue(void)
{
 //   CQueuePool<int>     Q(8, 16);



}


void TestCase_CQueuePool_EnQueue(void)
{
 //   CQueuePool<int>     Q(8);



}












