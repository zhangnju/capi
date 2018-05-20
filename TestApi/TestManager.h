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
/* 
 *	TestManager.h
 *
 *	DESCRIPTION
 *		Interface to a generic test case manager object.
 *
 *	HISTORY
 *		08-20-2006	create by zhouweiming.
 *
 */
#ifndef __TESTMANAGER_H__
#define __TESTMANAGER_H__

#pragma warning(disable : 4996)

#define     MAX_TESTFUNC_COUNT      4096
#define		MAX_TESTCASE_COUNT		128

typedef void(*TESTFUNC)(void);


struct TESTFUNCNODE {
	TESTFUNC	testfunc;
};


class TestCase {
public:
	typedef void (TestCase::*TCFUNC)();

	virtual ~TestCase();
	struct TESTCASENODE {
		TCFUNC	TestCaseFunc;
		char	*pszFuncName;
	};

	virtual	void	Init(){};
	virtual void	Close(){};

	SORTTABLE		*m_pTestCaseTable;
	char			*m_pszName;

	void AddTestCaseFunc(TCFUNC func, char *pszFuncName)
	{	
		TESTCASENODE	*pNode = new TESTCASENODE;
		pNode->pszFuncName = strdup(pszFuncName);
		pNode->TestCaseFunc = func;
		SortTable_Add(m_pTestCaseTable, (void *)pNode);
	};		  
};

#define	BEGIN_TESTCASE(TestCaseClass)\
public:\
	typedef TestCaseClass TESTCASECLASS;\
	TestCaseClass(char *pszName) {\
		m_pTestCaseTable = SortTable_Create(MAX_TESTCASE_COUNT);\
		m_pszName = pszName;\
		assertTrue(m_pTestCaseTable != NULL);

#define	ADDTESTCASE(x)  AddTestCaseFunc((TestCase::TCFUNC)&TESTCASECLASS::x, #x);

#define	END_TESTCASE()  \
	}


class TestManager {
public:
	struct TESTCASECLASSNODE {
		TestCase	*pTestCase;
		char		*pTestCaseClassName;
	};

	static SORTTABLE	*m_pTestCaseTable;
	static bool			m_bTestCaseFirstRun;
    static SORTTABLE    *m_pTable;
    static bool          m_bFirstRun;
    TESTFUNC            m_TestFunc;
	char				*m_pFuncName;

    TestManager(TESTFUNC TestFunc, char *pszName);
	TestManager(TestCase *pTestCase);
	~TestManager();

	void RegisterTestFunc();

    void RunTestCase();
};

#define RETISTER_TESTCASECLASS(x) TestManager g_t##x(new x(#x));
#define REGISTER_TESTFUNC(x)  TestManager g_s##x((x), #x);


#endif //__TESTMANAGER_H__
