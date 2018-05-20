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
#include <iostream>
#include <CapiGlobal.h>
#include <SortTable.h>
#include "TestApi.h"

using namespace std;

static SORTTABLE *g_pSortTable;


INT TestCase_Init(void)
{
	g_pSortTable = SortTable_Create(DEFAULT_TESTCASE_COUNT);
	if ( g_pSortTable == NULL )
	{
		return CAPI_FAILED;
	}
	else
	{
		return CAPI_SUCCESS;
	}
}

void TestCase_Add(TESTCASEFUNC func)
{
	if ( SortTable_Add(g_pSortTable, (void*)func) != CAPI_SUCCESS )
	{
		cout << "TestCase_Add() Failed,"
			<< "please change the size of macro DEFAULT_TESTCASE_COUNT\n"
			<< endl;
	}
}

void TestCase_Close(void)
{
	SortTable_Destroy(g_pSortTable, NULL);
}

void TestCase_Run(void)
{
	TESTCASEFUNC  func;
	UINT i;
	for ( i = 0; i < g_pSortTable->uCursorCount; i++ )
	{
		func = (TESTCASEFUNC)g_pSortTable->ppData[i];
		(*func)();
	}
}
