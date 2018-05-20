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
#include <time.h>
#include <omp.h>
#include "TestApi.h"
#include  "CTaskGraphScheduler.h"
#include "CThreadPool.h"

void TestCase_CTaskGraphScheduler_ScheduleTaskTeam(void);
void TestCase_CTaskGraphScheduler_ScheduleTaskTeam2(void);

void Test_CTaskGraphScheduler()
{
    TestCase_Add(TestCase_CTaskGraphScheduler_ScheduleTaskTeam);
//    TestCase_Add(TestCase_CTaskGraphScheduler_ScheduleTaskTeam2);
}

REGISTER_TESTFUNC(Test_CTaskGraphScheduler)

#define TASK_NUM        10

DWORD  g_dwTime[TASK_NUM] = {100, 307, 32, 56, 77, 28, 12, 3, 2, 1};

unsigned int WINAPI f0(void *args)
{
#ifdef _DEBUG
    printf("f0 called, time = %ld.\n", g_dwTime[0]);
#endif
    Sleep(g_dwTime[0]);
    return 0;
}

unsigned int WINAPI f1(void *args)
{
#ifdef _DEBUG
    printf("f1 called, time = %ld.\n", g_dwTime[1]);
#endif
    Sleep(g_dwTime[1]);
    return 0;
}

unsigned int WINAPI f2(void *args)
{
#ifdef _DEBUG
    printf("f2 called, time = %ld.\n", g_dwTime[2]);
#endif
    Sleep(g_dwTime[2]);
    return 0;
}

unsigned int WINAPI f3(void *args)
{
#ifdef _DEBUG
    printf("f3 called, time = %ld.\n", g_dwTime[3]);
#endif
    Sleep(g_dwTime[3]);
    return 0;
}

unsigned int WINAPI f4(void *args)
{
#ifdef _DEBUG
    printf("f4 called, time = %ld.\n", g_dwTime[4]);
#endif
    Sleep(g_dwTime[4]);
    return 0;
}


unsigned int WINAPI f5(void *args)
{
#ifdef _DEBUG
    printf("f5 called, time = %ld.\n", g_dwTime[5]);
#endif
    Sleep(g_dwTime[5]);
    return 0;
}

unsigned int WINAPI f6(void *args)
{
#ifdef _DEBUG
    printf("f6 called, time = %ld.\n", g_dwTime[6]);
#endif
    Sleep(g_dwTime[6]);
    return 0;
}

unsigned int WINAPI f7(void *args)
{
#ifdef _DEBUG
    printf("f7 called, time = %ld.\n", g_dwTime[7]);
#endif
    Sleep(g_dwTime[7]);
    return 0;
}

unsigned int WINAPI f8(void *args)
{
#ifdef _DEBUG
    printf("f8 called, time = %ld.\n", g_dwTime[8]);
#endif
    Sleep(g_dwTime[8]);
    return 0;
}

unsigned int WINAPI f9(void *args)
{
#ifdef _DEBUG
    printf("f9 called, time = %ld.\n", g_dwTime[9]);
#endif
    Sleep(g_dwTime[9]);
    return 0;
}

THREADFUNC Taskfunc[TASK_NUM] = {f0, f1, f2, f3, f4, f5, f6, f7, f8, f9};

void TestCase_CTaskGraphScheduler_ScheduleTaskTeam(void)
{
    CTaskGraphScheduler  s;

    int i;
    GRAPHNODE **ppGraphNode = new GRAPHNODE *[TASK_NUM];

    for ( i = 0; i < TASK_NUM; i++ )
    {
        ppGraphNode[i] = s.AddTask(Taskfunc[i], NULL, g_dwTime[i]);
    }

    clock_t     t1, t2;

    t1 = clock();

    s.ExecAllTask();

    t2 = clock();

    printf("Total Time = %ld\n", t2-t1);


    delete [] ppGraphNode;
}

DWORD  g_dwTime2[TASK_NUM] = {100, 200, 55, 77, 28, 12, 33, 16, 6, 3};

void TestCase_CTaskGraphScheduler_ScheduleTaskTeam2(void)
{
    CTaskGraphScheduler  s;

    int i;
    GRAPHNODE **ppGraphNode = new GRAPHNODE *[TASK_NUM];

    for ( i = 0; i < TASK_NUM; i++ )
    {
        g_dwTime[i] = g_dwTime2[i];
        ppGraphNode[i] = s.AddTask(Taskfunc[i], NULL, g_dwTime[i]);
    }

    s.AddRelation(ppGraphNode[0], ppGraphNode[6]);
    s.AddRelation(ppGraphNode[0], ppGraphNode[3]);

    s.AddRelation(ppGraphNode[1], ppGraphNode[3]);
    s.AddRelation(ppGraphNode[1], ppGraphNode[4]);

    s.AddRelation(ppGraphNode[2], ppGraphNode[4]);
    s.AddRelation(ppGraphNode[2], ppGraphNode[5]);

    s.AddRelation(ppGraphNode[6], ppGraphNode[7]);

    s.AddRelation(ppGraphNode[3], ppGraphNode[7]);
    s.AddRelation(ppGraphNode[3], ppGraphNode[8]);

    s.AddRelation(ppGraphNode[4], ppGraphNode[8]);
    s.AddRelation(ppGraphNode[4], ppGraphNode[9]);

    s.AddRelation(ppGraphNode[5], ppGraphNode[9]);

    clock_t     t1, t2;

    t1 = clock();

    s.ExecAllTask();

    t2 = clock();

    printf("Total Time = %ld\n", t2-t1);


    delete [] ppGraphNode;
}