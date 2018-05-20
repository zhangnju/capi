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
#include "DHashAVLTree.h"

void TestCase_DHashAVLTree_TestCase1(void);
void TestCase_DHashAVLTree_TestCase2(void);

void Test_DHashAVLTree()
{
	TestCase_Add(TestCase_DHashAVLTree_TestCase1);
	TestCase_Add(TestCase_DHashAVLTree_TestCase2);
}

//REGISTER_TESTFUNC(Test_DHashAVLTree)

void * IntCopy(void *p)
{
	return p;
}

void TestCase_DHashAVLTree_TestCase1(void)
{
	DHASHAVLTREE	*pTree = DHashAVLTree_Create(2);

	int i;
	for ( i = 0; i < 400; i++ )
	{
		DHashAVLTree_Insert(pTree, (void *)i, IntCompare, HashInt, IntCopy);
	}

	void *pKey = DHashAVLTree_Find(pTree, (void *)3, HashInt, IntCompare, IntCopy);

	assertTrue( pKey == (void *)3);

	DHashAVLTree_Delete(pTree, (void *)3, HashInt, IntCompare, NULL);


	pKey = DHashAVLTree_Find(pTree, (void *)3, HashInt, IntCompare, IntCopy);

	assertTrue(pKey == NULL);

	DHashAVLTree_Destroy(pTree, NULL);
}


void Find_Thread(LPVOID args)
{
	DHASHAVLTREE *pTree = (DHASHAVLTREE *)args;

	void *pKey;
	int i;
	for(i = 0; i < 100000; i++)
	{
		pKey = DHashAVLTree_Find(pTree, (void *)1001, HashInt, IntCompare, IntCopy);

		assertTrue(pKey == (void *)1001 || pKey == (void *)0);
	}
}


void TestCase_DHashAVLTree_TestCase2(void)
{
	DHASHAVLTREE	*pTree = DHashAVLTree_Create(32768);

	int i;
	clock_t  t1, t2;
	t1 = clock();
//#pragma omp parallel for num_threads(4) schedule(static, 1024)
	for ( i = 1; i <= 1000000; i++ )
	{
		DHashAVLTree_Insert(pTree, (void *)i, IntCompare, HashInt, IntCopy);
	}
	t2 = clock();
	printf("DHashAVLTree_Insert, time = %ld\n", t2-t1);

	_beginthread(Find_Thread, 0, (void *)pTree);
	for ( i = 0; i < 100000; i++ )
	{
		DHashAVLTree_Delete(pTree, (void *)1001, HashInt, IntCompare, NULL);
		DHashAVLTree_Insert(pTree, (void *)1001, IntCompare, HashInt, IntCopy);
	}
	void *pKey;
	t1 = clock();
#pragma omp parallel for schedule(static, 4096)
	for ( i = 1; i <= 1000000; i++ )
	{
		pKey = DHashAVLTree_Find(pTree, (void *)i, HashInt, IntCompare, IntCopy);
	}
	t2 = clock();
	printf("DHashAVLTree_Find 1000000, time = %ld\n", t2-t1);

	Sleep(100);

	DHashAVLTree_Destroy(pTree, NULL);
}

