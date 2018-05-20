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
#include "CMsgQueue.h"
#include "MTask.h"
#include "DoubleList.h"
#include "MTList.h"
#include "MsgQueue.h"
#include "CSharedQueue.h"
#include "CLocalQueue.h"

void TestCase_CMsgQueue_TestCase1(void);
void TestCase_CMsgQueue_TestCase2(void);

void Test_CMsgQueue()
{
//    TestCase_Add(TestCase_CMsgQueue_TestCase1);
    TestCase_Add(TestCase_CMsgQueue_TestCase2);
}

REGISTER_TESTFUNC(Test_CMsgQueue)


void TestCase_CMsgQueue_TestCase1(void)
{
    CMsgQueue<int, CSharedQueue<int, CLocalQueue<int>>>  Q(4);

    int a = 4;
    Q.Send(a);
    a = 8;
    Q.Send(a);
    a = 9;
    Q.Send(a);
    a = 10;
    Q.Send(a);
    printf("CMsgQueue: 4,8.9.10 have been send.\n");

//    CStealQueue<int> *pQueue;
//    pQueue = &(Q.m_Queue);
//    pQueue
//    a = 11;
//    Q.Send(a);
//    printf("CMsgQueue: 11 have been send.\n");

}


void TestCase_CMsgQueue_TestCase2(void)
{
    MSGQUEUE    *pQ;

    pQ = MsgQueue_Create(4);

    printf("MsgQueue: Start recving.\n");

    MsgQueue_Destroy(pQ, NULL);
//    void *p = MsgQueue_Recv(pQ);
//    printf("MsgQueue: data have been send.\n");
}
