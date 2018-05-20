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
#include "CLocalQueue.h"
#include "CStealQueue.h"
#include "CQueuePool.h"
#include "CDistributedQueue.h"

void TestCase_CDistributedQueue_Constructor1(void);
void TestCase_CDistributedQueue_Constructor2(void);
void TestCase_CDistributedQueue_Pop(void);
void TestCase_CDistributedQueue_Push(void);

void Test_CDistributedQueue()
{
    TestCase_Add(TestCase_CDistributedQueue_Constructor1);
    TestCase_Add(TestCase_CDistributedQueue_Constructor2);
    TestCase_Add(TestCase_CDistributedQueue_Pop);
    TestCase_Add(TestCase_CDistributedQueue_Push);
}

REGISTER_TESTFUNC(Test_CDistributedQueue)

void TestCase_CDistributedQueue_Constructor1(void)
{
#if 0
    CStealQueue<int>     Q(4);

    int nSize;

    int nCore = omp_get_num_procs();
    nSize = DEFAULT_STEALQUEUE_RESERVED_SIZE+nCore;

    assertTrue(nSize == Q.m_uMaxSize);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);
    assertTrue(Q.m_lSize == 0 && Q.m_uFullSize == nCore);
    assertTrue(Q.IsEmpty());
    assertFalse(Q.IsFull());
#endif
}

void TestCase_CDistributedQueue_Constructor2(void)
{
#if 0
    CStealQueue<int>     Q(8);

    int nSize;

    int nCore = omp_get_num_procs();
    nSize = DEFAULT_STEALQUEUE_RESERVED_SIZE+nCore;

    assertTrue(8 == Q.m_uMaxSize);
    assertTrue(Q.m_uHead == 0 && Q.m_uTail == 0);
    assertTrue(Q.m_lSize == 0 && Q.m_uFullSize == 8-DEFAULT_STEALQUEUE_RESERVED_SIZE);
    assertTrue(Q.IsEmpty());
    assertFalse(Q.IsFull());
#endif
}


void TestCase_CDistributedQueue_Pop(void)
{
    CDistributedQueue<int, CLocalQueue<int>, CQueuePool<int, CStealQueue<int>>> dque;

    dque.Create(16, 0, 16, 0);

    int x = 10;
    dque.EnQueue(x);

    x = 15;
    dque.EnQueue(x);

    x = 20;
    dque.EnQueue(x);

    int a;
    dque.DeQueue(a);
    assertTrue(a==10);
    dque.DeQueue(a);
    assertTrue(a==15);
    dque.DeQueue(a);
    assertTrue(a==20);

    x = 121;
    dque.PushToSharedQueue(x);
    dque.DeQueue(a);
    assertTrue(a==121);

    int ret = dque.DeQueue(a);
    assertTrue(ret == CAPI_FAILED && a == 121);
    
    x = 90;
    dque.PushToLocalQueue(x);
    dque.DeQueue(a);
    assertTrue(a==90);



}


void TestCase_CDistributedQueue_Push(void)
{
    CDistributedQueue<int, CLocalQueue<int>, CStealQueue<int>> dque;

    dque.Create(16, 0, 16, 0);

    int x = 10;
    dque.EnQueue(x);

    x = 15;
    dque.EnQueue(x);

    x = 20;
    dque.EnQueue(x);

    int a;
    dque.DeQueue(a);
    assertTrue(a==10);
    dque.DeQueue(a);
    assertTrue(a==15);
    dque.DeQueue(a);
    assertTrue(a==20);

    x = 121;
    dque.PushToSharedQueue(x);
    dque.DeQueue(a);
    assertTrue(a==121);

    int ret = dque.DeQueue(a);
    assertTrue(ret == CAPI_FAILED && a == 121);

    x = 90;
    dque.PushToLocalQueue(x);
    dque.DeQueue(a);
    assertTrue(a==90);

}


