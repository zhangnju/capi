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
 *	TestCaseMgr.h
 *
 *	DESCRIPTION
 *		Interface to a generic test case manager object.
 *
 *	HISTORY
 *		08-3-2006	create by zhouweiming.
 *
 */
#ifndef __TESTCASEMGR_H__
#define __TESTCASEMGR_H__


#define DEFAULT_TESTCASE_COUNT	8192

typedef void (*TESTCASEFUNC)(void);

INT  TestCase_Init(void);
void TestCase_Close(void);

void TestCase_Add(TESTCASEFUNC func);

void TestCase_Add(TESTCASEFUNC func);

void TestCase_Run(void);


#endif //__TESTCASEMGR_H__