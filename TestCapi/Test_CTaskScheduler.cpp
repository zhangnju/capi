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
#include  "CTaskScheduler.h"
#include "CThreadPool.h"

void TestCase_CTaskScheduler_TestCase1(void);

void Test_CTaskScheduler()
{
    TestCase_Add(TestCase_CTaskScheduler_TestCase1);
}

REGISTER_TESTFUNC(Test_CTaskScheduler)



void TestCase_CTaskScheduler_TestCase1(void)
{
    CTaskScheduler<int>  s(4);

    int a = 3;
    s.PushTask(a);
    a = 8;
    s.PushTask(a);
    a = 10;
    s.PushTask(a);

    int Data[16];
    s.PopTask(Data[0]);
    s.PopTask(Data[1]);
    s.PopTask(Data[2]);

}

