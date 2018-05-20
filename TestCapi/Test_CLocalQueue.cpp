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
#include "TestApi.h"
#include "CLocalQueue.h"

void TestCase_CLocalQueue_DeQueue(void);
void TestCase_CLocalQueue_EnQueue(void);
void TestCase_CLocalQueue_EnQueue1(void);

void Test_CLocalQueue()
{
    TestCase_Add(TestCase_CLocalQueue_DeQueue);
    TestCase_Add(TestCase_CLocalQueue_EnQueue);
    TestCase_Add(TestCase_CLocalQueue_EnQueue1);
}

REGISTER_TESTFUNC(Test_CLocalQueue)


void TestCase_CLocalQueue_DeQueue(void)
{
    CLocalQueue<int>     Q(4);
    int a;

    assertTrue(Q.m_uMaxSize == 4);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);

    int ret;
    int Data = 123;
    ret = Q.DeQueue(Data);
    assertTrue( ret == CAPI_FAILED && Data == 123 );

    a = 1;
    Q.EnQueue(a);

    assertTrue(Q.m_uTail == 1 && Q.m_uMaxSize == 4);

    a = 2;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 2 && Q.m_uMaxSize == 4);

    a = 3;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 3 && Q.m_uMaxSize == 4);

    a = 4;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 4 && Q.m_uMaxSize == 4);

    a = 5;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 5 && Q.m_uMaxSize == 8);
}


void TestCase_CLocalQueue_EnQueue(void)
{
    CLocalQueue<int>     Q(4);
    int a;

    assertTrue(Q.m_uMaxSize == 4);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);

    assertTrue(Q.IsEmpty());

    a = 1;
    Q.EnQueue(a);

    assertTrue(Q.m_uTail == 1 && Q.m_uMaxSize == 4);

    a = 2;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 2 && Q.m_uMaxSize == 4);

    a = 3;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 3 && Q.m_uMaxSize == 4);

    a = 4;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 4 && Q.m_uMaxSize == 4 && Q.m_uHead == 0);
    
    int ret;
    int Data = 123;
    ret = Q.DeQueue(Data);
    assertTrue( ret == CAPI_SUCCESS && Data == 1 && Q.m_uHead == 1);

    a = 5;
    Q.EnQueue(a);
    assertTrue(Q.m_uTail == 0 && Q.m_uMaxSize == 4);

    Q.DeQueue(Data);
    Q.DeQueue(Data);
    Q.DeQueue(Data);
    Q.DeQueue(Data);

    assertTrue(Q.IsEmpty() && Data == 5);
}

struct Block8 {
    int a[8]; 
};

void TestCase_CLocalQueue_EnQueue1(void)
{
    CLocalQueue<Block8>     Q(4);
    {
        Block8   x;
        for ( int i = 0; i < 8; i++ )
        {
            x.a[i] = i;
        }

        Q.EnQueue(x);
        Q.EnQueue(x);

    }

    Block8 y;
    Q.DeQueue(y);
    assertEquals(y.a[5], 5);
}
