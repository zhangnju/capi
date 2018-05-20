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
 // Test_ABS.cpp
#include <windows.h>
#include <stdio.h>
#include "Testapi.h"

UINT ABS( UINT a, UINT b)
{
    if ( a > b )
    {
        return (a - b);
    }
    else
    {
        return (b - a);
    }
}

void TestCase_ABS_a_LargerThan_b(void);
void TestCase_ABS_b_LargerThan_a(void);

void Test_ABS()
{
	TestCase_Add(TestCase_ABS_a_LargerThan_b);

	TestCase_Add(TestCase_ABS_b_LargerThan_a);
}

REGISTER_TESTFUNC(Test_ABS)


void TestCase_ABS_a_LargerThan_b(void)
{
    assertEquals(ABS(10, 5), 5);
}

void TestCase_ABS_b_LargerThan_a(void)
{
    assertEquals(ABS(0, 1), 1);
}


class TestABS:public TestCase {
	BEGIN_TESTCASE(TestABS)	
		ADDTESTCASE(a_LagrgerThan_b)
		ADDTESTCASE(b_LagrgerThan_a)
	END_TESTCASE()
public:
	void a_LagrgerThan_b();

	void b_LagrgerThan_a()
	{
		assertEquals(ABS(5, 8),3);
	}
};

//RETISTER_TESTCASECLASS(TestABS)

void TestABS::a_LagrgerThan_b()
{
	assertEquals(ABS(8, 5),3);
}
