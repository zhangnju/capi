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
// Test_Parallel_MergeSort.cpp
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Testapi.h"
#include "CapiGlobal.h"
#include "Parallel_MergeSort.h"
#include "CapiCommon.h"

void TestCase_Parallel_MergeSort(void);
void TestCase_Serial_MergeSort(void);

void Test_Parallel_MergeSort()
{
    TestCase_Add(TestCase_Parallel_MergeSort);

    TestCase_Add(TestCase_Serial_MergeSort);
}

//REGISTER_TESTFUNC(Test_Parallel_MergeSort)


void TestCase_Parallel_MergeSort(void)
{
	int i;
	clock_t t1,t2;

	int nData[16] = {0, 8, 12, 27, 43, 65, 71, 125, 5, 17, 22, 33, 41,  55, 107, 657 };
	void **ppIntData = (void **)malloc(16 * sizeof(void *));

	for ( i = 0; i < 16; i++ )
	{
		ppIntData[i] = (void *)nData[i];
	}

	void **pp = (void **)malloc(sizeof(void *)*16);
	t1 = clock();

	for ( i = 0; i < 100000; i++ )
	{
		Serial_Merge(ppIntData, 0, 7, 8, 15, IntCompare, pp);
	}
	t2 = clock();

	printf("Serial_Merge() Time = %ld\n", t2-t1);

	Serial_Merge(ppIntData, 0, 7, 8, 15, IntCompare, pp);

	for ( i = 0; i < sizeof(nData)/sizeof(int); i++ )
	{
		printf( "pp[%d] = %ld\n", i, (int)pp[i]);
	}

	free(pp);
	free(ppIntData);



	int *pn;
	
	t1 = clock();
	for( i = 0; i < 100000; i++ )
	{
		pn = MergeInt(nData, 0, 7, 8, 15 );
		free(pn);
	}
	t2 = clock();


	printf("MergeInt() Time = %ld\n", t2-t1);

	pn = MergeInt(nData, 0, 7, 8, 15 );

	for ( i = 0; i < sizeof(nData)/sizeof(int); i++ )
	{
		printf( "pn[%d] = %ld\n", i, pn[i]);
	}

	free(pn);

	t1 = clock();
	for( i = 0; i < 100000; i++ )
	{
		pn = (int *)malloc(128);
		free(pn);
	}
	t2 = clock();


	printf("malloc() Time = %ld\n", t2-t1);


    UINT uRand = (UINT)time(NULL);

    srand(uRand);

#define DATA_SIZE		1000000

	pp = (void **)malloc(sizeof(void *)*DATA_SIZE);

    void **ppOut = (void **)malloc(sizeof(void *)*DATA_SIZE);

	for ( i = 0; i < DATA_SIZE; i++ )
	{
		pp[i] = (void *)rand();
        ppOut[i] = pp[i];
	}

    t1 = clock();

    Parallel_MergeSort(pp, DATA_SIZE, IntCompare);

    t2 = clock();

    printf( "Parallel merge sort %ld time = %ld\n", DATA_SIZE, t2-t1);

    for ( i = 0; i < DATA_SIZE - 1; i++ )
    {
        assertTrue( (int)pp[i] <= (int)pp[i+1] );
        if ( pp[i] > pp[i+1] )
        {
            printf("Parallel_MergeSort failed in i = %d, ppOut1[%d] = %ld, ppOut1[%d] = %ld\n", 
                i, i, pp[i], i+1, pp[i+1]);
        }
    }


    t1 = clock();
    Serial_MergeSort(ppOut, 0, DATA_SIZE -1, IntCompare);
    t2 = clock();


    for ( i = 0; i < DATA_SIZE - 1; i++ )
    {
        assertTrue( (int)ppOut[i] <= (int)ppOut[i+1] );
        if ( ppOut[i] > ppOut[i+1] )
        {
            printf("Serial_MergeSort failed in i = %d, ppOut1[%d] = %ld, ppOut1[%d] = %ld\n", 
                i, i, ppOut[i], i+1, ppOut[i+1]);
        }
    }


    printf( "Serial merge sort %ld time = %ld\n", DATA_SIZE, t2-t1);

	free(pp);
    free(ppOut);
}


void TestCase_Serial_MergeSort(void)
{

}


