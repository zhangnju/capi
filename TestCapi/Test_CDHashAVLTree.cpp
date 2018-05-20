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
#include "CDHashAVLTree.h"

void TestCase_CDHashAVLTree_TestCase1(void);
void TestCase_CDHashAVLTree_TestCase2(void);

void Test_CDHashAVLTree()
{
	TestCase_Add(TestCase_CDHashAVLTree_TestCase1);
	TestCase_Add(TestCase_CDHashAVLTree_TestCase2);
}

REGISTER_TESTFUNC(Test_CDHashAVLTree)


extern void *IntCopy(void *p); //¼ûTest_DHashAVLTree.cpp

void TestCase_CDHashAVLTree_TestCase1(void)
{
	CDHashAVLTree	dtree(2, HashInt, IntCompare, NULL, IntCopy);

	int i;
	for ( i = 400; i > 0 ; i-- )
	{
		if ( i == 32 )
		{
			printf( "i = %ld\n", i);
		}
		dtree.Insert((void *)i);
	}
	int a = 3;
	int ret = -1;
	ret = (int)dtree.Find((void *)a);

	assertTrue( ret == a);

	for ( i = 1; i <= 400; i++ )
	{
		ret = (int)dtree.Find((void *)i);
		if ( ret != i )
		{
			printf("ret = %ld, i = %ld\n", ret, i);
		}
	}


	dtree.Delete((void *)a);

	ret = (int)dtree.Find((void *)a);

	assertTrue(ret == 0);

	dtree.Insert((void *)a);

	for ( i = 1; i <= 400; i++ )
	{
		ret = (int)dtree.Find((void *)i);
		if ( ret != i )
		{
			printf("ret = %ld, i = %ld\n", ret, i);
		}
	}
	printf("TestCase1 Finished.\n");

}


void CDHashAVLTree_Find_Thread(LPVOID args)
{
	CDHashAVLTree *p = (CDHashAVLTree *)args;

	int Key;
	int i;
	int data = 1001;
	for(i = 0; i < 100000; i++)
	{
		Key = (int)p->Find((void *)data);
		assertTrue(Key == data || Key == 0);
	}
}



void TestCase_CDHashAVLTree_TestCase2(void)
{
	CDHashAVLTree	dtree(32768, HashInt, IntCompare, NULL, IntCopy);

	int i;
	clock_t  t1, t2;
	t1 = clock();
//#pragma omp parallel for num_threads(4) schedule(static, 16)
	for ( i = 1; i <= 1000000; i++ )
	{
		dtree.Insert((void *)i);
	}
	t2 = clock();
	printf("CDHashAVLTree::Insert, time = %ld\n", t2-t1);

	int data = 1001;
	_beginthread(CDHashAVLTree_Find_Thread, 0, (void *)&dtree);
	for ( i = 0; i < 100000; i++ )
	{
		dtree.Delete((void *)data);
		dtree.Insert((void *)data);
	}
	t1 = clock();
#pragma omp parallel for //schedule(static, 4096)
	for ( i = 1; i <= 1000000; i++ )
	{
		int Key;
		Key = (int)dtree.Find((void *)i);
	}
	t2 = clock();
	printf("CDHashAVLTree::Find 1000000, time = %ld\n", t2-t1);

	t1 = clock();
#pragma omp parallel for //schedule(static, 4096)
	for ( i = 1; i <= 1000000; i++ )
	{
		int Key;
		Key = (int)dtree.Find((void *)i);
	}
	t2 = clock();
	printf("CDHashAVLTree::Find 1000000, time = %ld\n", t2-t1);

	Sleep(100);
}
