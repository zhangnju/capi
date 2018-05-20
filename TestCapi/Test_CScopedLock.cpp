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
using namespace std;

#include "capiglobal.h"
#include "Testapi.h"
#include "CScopedLock.h"
#include "CRWLock.h"
#include "FastLock.h"


void TestCase_CScopedLock(void);

void Test_CScopedLock()
{
    TestCase_CScopedLock();
}

REGISTER_TESTFUNC(Test_CScopedLock)


void TestCase_CScopedLock(void)
{
    CRWLock  fast_lock;

    {
        CScopedLock<CRWLock> slock(fast_lock, RWLOCK_WRITE);

        cout << "loop Started.\n" << endl;
        int i, a;
        a = 0;

        for ( i = 0; i < 100000000; i++ )
        {
            a = i;
        }
        cout << "loop finished.\n"<<endl;
    }

}
