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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CapiGlobal.h"
#include "SortTable.h"
#include "TestApi_Assert.h"
#include "TestManager.h"

bool TestManager::m_bFirstRun = true;
SORTTABLE *TestManager::m_pTable = NULL;

bool TestManager::m_bTestCaseFirstRun = true;
SORTTABLE *TestManager::m_pTestCaseTable = NULL;


TestCase::~TestCase()
{
	for ( UINT i= 0; i < m_pTestCaseTable->uCursorCount; i++ )
	{
		TESTCASENODE *pNode = (TESTCASENODE *)m_pTestCaseTable->ppData[i];
		if ( pNode != NULL )
		{
			if ( pNode->pszFuncName != NULL )
			{
				free(pNode->pszFuncName);
			}
			delete pNode;
		}
	}
	free(m_pTestCaseTable->ppData);
	free(m_pTestCaseTable);
};


void TestCase_Destroy(void *p)
{
	TestCase *pTestCase = (TestCase *)p;
	delete pTestCase;
}

TestManager::TestManager(TESTFUNC TestFunc, char *pszName):m_TestFunc(TestFunc)
{
    if (m_bFirstRun)
    {
        m_pTable = SortTable_Create(MAX_TESTFUNC_COUNT);
        assertTrue(m_pTable != NULL);

        m_bFirstRun = false;
    }
	m_pFuncName = strdup(pszName);

    if ( TestFunc != NULL)
    {
        INT nRet = SortTable_Add(m_pTable, (void *)TestFunc);
        assertTrue(nRet != CAPI_FAILED);
    }
}


TestManager::TestManager(TestCase *pTestCase)
{
	m_pFuncName = NULL;
    if (m_bTestCaseFirstRun)
    {
        m_pTestCaseTable = SortTable_Create(MAX_TESTFUNC_COUNT);
        assertTrue(m_pTable != NULL);

        m_bTestCaseFirstRun = false;
    }

    if ( pTestCase != NULL)
    {
        INT nRet = SortTable_Add(m_pTestCaseTable, (void *)pTestCase);
        assertTrue(nRet != CAPI_FAILED);
    }
}

TestManager::~TestManager()
{
	if ( m_pTable != NULL )
    {
        SortTable_Destroy(m_pTable, NULL);
        m_pTable = NULL;
    }
	if ( m_pFuncName != NULL )
	{
		free(m_pFuncName);
		m_pFuncName = NULL;
	}
	if ( m_pTestCaseTable != NULL )
	{
		SortTable_Destroy(m_pTestCaseTable, TestCase_Destroy);
		m_pTestCaseTable = NULL;
	}
}

void TestManager::RunTestCase()
{
    if ( m_pTable != NULL )
    {
	    TESTFUNC  TestFunc;
	    UINT i;
	    for ( i = 0; i < m_pTable->uCursorCount; i++ )
	    {
		    TestFunc = (TESTFUNC)m_pTable->ppData[i];
		    (*TestFunc)();
	    }    
    }
	if ( m_pTestCaseTable != NULL )
	{
	    for ( UINT i = 0; i < m_pTestCaseTable->uCursorCount; i++ )
	    {
		    TestCase *p = (TestCase *)m_pTestCaseTable->ppData[i];
			for ( UINT j = 0; j < p->m_pTestCaseTable->uCursorCount; j++ )
			{
				TestCase::TESTCASENODE *pNode = 
					(TestCase::TESTCASENODE *)p->m_pTestCaseTable->ppData[j];
				TestCase::TCFUNC f = pNode->TestCaseFunc;
				(p->*f)();
			}
	    }    
	}
}