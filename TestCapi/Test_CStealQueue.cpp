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

void TestCase_CStealQueue_Constructor1(void);
void TestCase_CStealQueue_Constructor2(void);
void TestCase_CStealQueue_DeQueue(void);
void TestCase_CStealQueue_EnQueue(void);

void Test_CStealQueue()
{
    TestCase_Add(TestCase_CStealQueue_Constructor1);
    TestCase_Add(TestCase_CStealQueue_Constructor2);
    TestCase_Add(TestCase_CStealQueue_DeQueue);
    TestCase_Add(TestCase_CStealQueue_EnQueue);
}

REGISTER_TESTFUNC(Test_CStealQueue)

void TestCase_CStealQueue_Constructor1(void)
{
    CStealQueue<int>     Q(4);

    int nSize;

    nSize = DEFAULT_STEALQUEUE_SIZE;

    assertTrue(nSize == Q.m_uMaxSize);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);
    assertTrue(Q.m_lEmptyFlag == 1 && Q.m_lFullFlag == 0);
    assertTrue(Q.IsEmpty());
    assertFalse(Q.IsFull());
}

void TestCase_CStealQueue_Constructor2(void)
{
    CStealQueue<int>     Q(8);

    int nSize;

    nSize = DEFAULT_STEALQUEUE_SIZE;

    assertTrue(8 == Q.m_uMaxSize);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);
    assertTrue(Q.m_lEmptyFlag == 1 && Q.m_lFullFlag == 0);
    assertTrue(Q.IsEmpty());
    assertFalse(Q.IsFull());
}


void TestCase_CStealQueue_DeQueue(void)
{
    CStealQueue<int>     Q(8);

    assertTrue(Q.m_uMaxSize == 8);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);

    int ret;
    int Data = 123;
    ret = Q.DeQueue(Data);
    assertTrue( ret == CAPI_FAILED && Data == 123 );

    int a = 1;
    Q.EnQueue(a);

    assertTrue(Q.m_uTail == 1 && Q.m_uMaxSize == 8);

    a = 2;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 2 && Q.m_uMaxSize == 8);

    a = 3;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 3 && Q.m_uMaxSize == 8);

    a = 4;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 4 && Q.m_uMaxSize == 8);

    a = 5;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 5 && Q.m_uMaxSize == 8);
    a = 6;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 6 && Q.m_uMaxSize == 8);
    a = 7;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 7 && Q.m_uMaxSize == 8);
    a = 8;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 8 && Q.m_uMaxSize == 8);
    a = 9;
    ret = Q.EnQueue(a);
    assertTrue(Q.m_uTail == 8 && Q.m_uMaxSize == 8);
    assertTrue(ret == CAPI_FAILED);
}


void TestCase_CStealQueue_EnQueue(void)
{
    CStealQueue<int>     Q(8);

    assertTrue(Q.m_uMaxSize == 8);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);

    assertTrue(Q.IsEmpty());

    int a = 1;
    Q.EnQueue(a);

    assertTrue(Q.m_uTail == 1 && Q.m_uMaxSize == 8);

    a = 2;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 2 && Q.m_uMaxSize == 8);
    a = 3;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 3 && Q.m_uMaxSize == 8);
    assertFalse(Q.IsFull());

    a = 4;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 4 && Q.m_uMaxSize == 8 && Q.m_uHead == 0);

    a = 5;
    Q.EnQueue(a);
    a = 6;
    Q.EnQueue(a);
    a = 7;
    Q.EnQueue(a);
    a = 8;
    Q.EnQueue(a);

    assertTrue(Q.IsFull() && Q.GetCountNoLock() == 8);

    int ret;
    int Data = 123;
    ret = Q.DeQueue(Data);
    assertTrue( ret == CAPI_SUCCESS && Data == 1 && Q.m_uHead == 1);

    a = 5;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 0 && Q.m_uMaxSize == 8 && Q.m_uHead == 1);
    assertTrue(Q.IsFull());

    Q.DeQueue(Data);//2
    Q.DeQueue(Data);//3
    Q.DeQueue(Data);

    assertTrue(Q.GetCountNoLock() == 5);
    Q.DeQueue(Data);
    Q.DeQueue(Data);//6
    Q.DeQueue(Data);
    Q.DeQueue(Data);//8
    Q.DeQueue(Data);//5

    assertTrue(Q.IsEmpty() && Data == 5 && Q.m_uHead == 0);
    assertTrue(Q.GetCountNoLock() == 0);

    a = 10;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 1 && Q.m_uHead == 0);
    assertTrue(Q.GetCountNoLock() == 1);

}


