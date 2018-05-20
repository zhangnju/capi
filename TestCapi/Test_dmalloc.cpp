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
#include "dmalloc.h"

void TestCase_dmalloc_TestCase1(void);

void Test_dmalloc()
{
    TestCase_Add(TestCase_dmalloc_TestCase1);
}

//REGISTER_TESTFUNC(Test_dmalloc)


void TestCase_dmalloc_TestCase1(void)
{
    int i = 0;
    clock_t     t1, t2;

    void *p1;
    void *p2;

    t1 = clock();
#pragma omp parallel sections num_threads(2)
    {
#pragma omp section
        {
            void *q1 = dmalloc(8);
             for ( i = 0; i < 1000000; i++ )
            {
                p1 = dmalloc(8);

                dfree(p1);
            }
            dfree(q1);
       }

#pragma omp section
        {
            void *q2 = dmalloc(8);
            for ( i = 0; i < 1000000; i++ )
            {
                p2 = dmalloc(16);

                dfree(p2);
            }
            dfree(q2);
       }
    }
    t2 = clock();

    printf("parallel dmalloc() and dfree() called %ld times, time = %ld\n", i, t2-t1);

    t1 = clock();

    for ( i = 0; i < 1000000; i++ )
    {
        void *p = dmalloc(8);

        dfree(p);
    }
    t2 = clock();
    printf("one thread: dmalloc() and dfree() called %ld times, time = %ld\n", i, t2-t1);



    t1 = clock();
#pragma omp parallel for
    for ( i = 0; i < 1000000; i++ )
    {
        void *p = malloc(8);

        free(p);
    }
    t2 = clock();
    printf("one thread: malloc() and free() called %ld times, time = %ld\n", i, t2-t1);


}
