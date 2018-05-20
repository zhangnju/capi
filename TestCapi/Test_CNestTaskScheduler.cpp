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
#include "CThreadPool.h"
#include  "CNestTaskScheduler.h"

void TestCase_CNestTaskScheduler_TestCase1(void);

void Test_CNestTaskScheduler()
{
    TestCase_Add(TestCase_CNestTaskScheduler_TestCase1);
}

REGISTER_TESTFUNC(Test_CNestTaskScheduler)


struct RANGE {
    int begin;
    int end;
};

CNestTaskScheduler  *pTaskSched = NULL;

/**	任务处理入口函数

	@param	void *args - 参数，实际为RANGE类型	
	@return	unsigned int WINAPI - 总是返回CAPI_SUCCESS	
*/
unsigned int WINAPI RootTask(void *args)
{
    RANGE  *p = (RANGE *)args;
    if ( p != NULL )
    {
        printf("Range: %ld - %ld\n", p->begin, p->end);
        if ( p->end - p->begin < 128 )
        {
            //当区间大小小于128时，不再进行分拆
            delete p;
            return 0;
        }
        int mid = (p->begin + p->end + 1) / 2;
        RANGE *range1, *range2;

        range1 = new RANGE;
        range2 = new RANGE;

        range1->begin = p->begin;
        range1->end = mid - 1;
        range2->begin = mid;
        range2->end = p->end;

        TASK t1, t2;
        t1.pArg = range1;
        t2.pArg = range2;
        t1.func = RootTask;
        t2.func = RootTask;

        pTaskSched->SpawnLocalTask(t1);
        pTaskSched->SpawnTask(t2);

        delete p;
    }
    return CAPI_SUCCESS;
}



void TestCase_CNestTaskScheduler_TestCase1(void)
{

    TASK    task;
    RANGE   *pRange = new RANGE;

    pRange->begin = 0;
    pRange->end = 800;

    task.func = RootTask;
    task.pArg = pRange;

    pTaskSched = new CNestTaskScheduler;
    
    pTaskSched->BeginRootThread(task);

    delete pTaskSched;

}

