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
#include <stdlib.h>
#include <time.h>
#include "Testapi.h"

#include "CapiGlobal.h"
#include "CapiCommon.h"
#include "ApiHook.h"
#include <iostream>
using namespace std;


extern void Test_Stack();
extern void Test_Queue();
extern void Test_DeQue();
extern void Test_SortTable();

extern void Test_SingleList();
extern void Test_DoubleList();
extern void Test_BlockList();

extern void Test_HashTable();
extern void Test_HashList();

extern void Test_BinTree();
extern void Test_RBTree();
extern void Test_AVLTree();

extern void Test_HashRBTree();
extern void Test_HashAVLTree();

extern void Test_MTree();

extern void Test_WithSTL();

extern void Test_SpList();
extern void Test_DSpaceList();


#if 1
void * operator new(size_t size)
{
    return malloc(size);
}

void * operator new[](size_t size)
{
    return malloc(size);
}

void operator delete(void *p)
{
    free(p);
}

void operator delete[](void *p)
{
    free(p);
}

#endif




void main(int argc, char *argv[])
{
//    TMemHookInit();
//    TCheckMemory    mem;

    if ( TestCase_Init() != CAPI_SUCCESS )
	{
		return;
	}

    TestManager *pManager = new TestManager(NULL);

    pManager->RunTestCase();
    delete pManager;

	TestCase_Run();

	TestCase_Close();


//    mem.TCheckMemAll();

//    atexit(TMemHookClose);

    printf("Test Finished.\n");
    return;
}



