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
// Test_Emalloc.cpp
#include <windows.h>
#include <stdio.h>
#include "CapiGlobal.h"
#include "TestApi.h"
#include "SpList.h"
#include "DSpaceList.h"
#include "Emalloc.h"

void TestCase_Emalloc_Create_1(void);
void TestCase_Emalloc_Add_1(void);
void TestCase_Emalloc_Add_2(void);
void TestCase_Emalloc_Add_3(void);

void Test_Emalloc()
{
	TestCase_Add(TestCase_Emalloc_Create_1);

	TestCase_Add(TestCase_Emalloc_Add_1);
	TestCase_Add(TestCase_Emalloc_Add_2);
	TestCase_Add(TestCase_Emalloc_Add_3);

}

REGISTER_TESTFUNC(Test_Emalloc)

void TestCase_Emalloc_Create_1(void)
{
	INT nRet = Emalloc_Create();
	assertEquals(nRet, CAPI_SUCCESS);
    Emalloc_Destroy();
}

void TestCase_Emalloc_Add_1(void)
{
//	assertEquals(1, 2);
}

void TestCase_Emalloc_Add_2(void)
{
//	assertEquals(1, 52);
}

void TestCase_Emalloc_Add_3(void)
{
//	assertEquals(1, 7);
}
