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
#include <process.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include "TestApi.h"
#include "CDHashArray.h"

void TestCase_CDHashArray_TestCase1(void);
void TestCase_CDHashArray_TestCase2(void);
void TestCase_CDHashArray_TestCase3(void);
void TestCase_CDHashArray_TestCase4(void);

void Test_CDHashArray()
{
	TestCase_Add(TestCase_CDHashArray_TestCase1);
	TestCase_Add(TestCase_CDHashArray_TestCase2);
    TestCase_Add(TestCase_CDHashArray_TestCase3);
	TestCase_Add(TestCase_CDHashArray_TestCase4);
}

REGISTER_TESTFUNC(Test_CDHashArray)



void TestCase_CDHashArray_TestCase1(void)
{
	CDHashArray<int, CSearchArray<int>>	ds(2, HashInt);

	int i;
	for ( i = 400; i > 0 ; i-- )
	{
		if ( i == 32 )
		{
			printf( "i = %ld\n", i);
		}
		ds.Insert(i);
	}
	int a = 3;
	int ret = -1;
	ds.Find(a, ret);

	assertTrue( ret == a);

	for ( i = 1; i <= 400; i++ )
	{
		ret = -1;
		ds.Find(i, ret);
		if ( ret != i )
		{
			printf("ret = %ld, i = %ld\n", ret, i);
		}
	}


	ds.Delete(a);


	ret = -1;
	ds.Find(a, ret);

	assertTrue(ret == -1);

	ds.Insert(a);

	for ( i = 1; i <= 400; i++ )
	{
		ret = -1;
		ds.Find(i, ret);
		if ( ret != i )
		{
			printf("ret = %ld, i = %ld\n", ret, i);
		}
	}

}


void CDHashArray_Find_Thread(LPVOID args)
{
	CDHashArray<int, CSearchArray<int>> *p = (CDHashArray<int, CSearchArray<int>> *)args;

	int Key;
	int i;
	int data = 1001;
	for(i = 0; i < 100000; i++)
	{
		Key = -1;
		
		p->Find(data, Key);

		assertTrue(Key == data || Key == -1);
	}
}



void TestCase_CDHashArray_TestCase2(void)
{
	CDHashArray<int, CSearchArray<int>>	ds(32768, HashInt);

	int i;
	clock_t  t1, t2;
	t1 = clock();
#pragma omp parallel for num_threads(4) schedule(static, 16)
	for ( i = 1; i <= 1000000; i++ )
	{
		ds.Insert(i);
	}
	t2 = clock();
	printf("CDHashArray::Insert, time = %ld\n", t2-t1);

	int data = 1001;
	_beginthread(CDHashArray_Find_Thread, 0, (void *)&ds);
	for ( i = 0; i < 100000; i++ )
	{
		ds.Delete(data);
		ds.Insert(data);
	}
	t1 = clock();
#pragma omp parallel for //schedule(static, 4096)
	for ( i = 1; i <= 1000000; i++ )
	{
		int Key;
		Key = -1;
		ds.Find(i, Key);
	}
	t2 = clock();
	printf("CDHashArray::Find 1000000, time = %ld\n", t2-t1);

	t1 = clock();
#pragma omp parallel for //schedule(static, 4096)
	for ( i = 1; i <= 1000000; i++ )
	{
		int Key;
		Key = -1;
		ds.Find(i, Key);
	}
	t2 = clock();
	printf("CDHashArray::Find 1000000, time = %ld\n", t2-t1);

	Sleep(100);
}

void TestCase_CDHashArray_TestCase3(void)
{
	CDHashArray<int, CSearchArray<int>>	ds(65536, HashInt);

	int i;
	clock_t  t1, t2;

	int nTestCount = 1000000;

	int *pData = new int[nTestCount];
	for ( i = 0; i < nTestCount; i++ )
	{
		pData[i] = (i+1) * 0x6bdef3c9;
	}

	t1 = clock();
#pragma omp parallel for num_threads(4) schedule(static, 16)
	for ( i = 0; i < nTestCount; i++ )
	{
		ds.Insert(pData[i]);
	}
	t2 = clock();


	printf("CDHashArray::Insert, time = %ld\n", t2-t1);

	t1 = clock();
#pragma omp parallel for num_threads(8) schedule(static, 2048)
	for ( i = 0; i < nTestCount; i++ )
	{
		int Key;
		Key = -1;
		ds.Find(pData[i], Key);
	}
	t2 = clock();
	printf("CDHashArray::Find %ld, time = %ld\n", nTestCount, t2-t1);

	Sleep(100);
}

void TestCase_CDHashArray_TestCase4(void)
{
	CDHashArray<int, CSearchArray<int>>	ds(65536, HashInt);

	int i;
	clock_t  t1, t2;

	int nTestCount = 1000000;

	int *pData = new int[nTestCount];
	for ( i = 0; i < nTestCount; i++ )
	{
		pData[i] = (i+1) * 0x6bdef3c9;
	}

	t1 = clock();
#pragma omp parallel for num_threads(8) schedule(static, 1024)
	for ( i = 0; i < nTestCount; i++ )
	{
		ds.Insert(pData[i]);
	}
	t2 = clock();


	printf("CDHashArray::Insert, time = %ld\n", t2-t1);

	t1 = clock();
#pragma omp parallel for num_threads(8) schedule(static, 1)
	for ( i = 0; i < 8; i++ )
	{
		int Key;
		Key = -1;
		int j;
		for ( j = 0; j < nTestCount; j++ )
		{
			ds.Find(pData[i], Key);
		}
	}
	t2 = clock();
	printf("CDHashArray::Find %ld, time = %ld\n", nTestCount*8, t2-t1);

	Sleep(100);
    //printf("sizeof(INDEXNODE) = %ld\n", sizeof(INDEXNODE<int>));
}

