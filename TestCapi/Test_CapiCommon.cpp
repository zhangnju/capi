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
#include "Testapi.h"
#include "CapiCommon.h"


void TestCase_GetCacheAlignedAddr(void);

void Test_CapiCommon()
{
    TestCase_Add(TestCase_GetCacheAlignedAddr);

}

REGISTER_TESTFUNC(Test_CapiCommon)


void TestCase_GetCacheAlignedAddr(void)
{
    void * pAddr = (void *)0x8012ff45;
    void *pRet = GetCacheAlignedAddr(pAddr);
    printf( "Cache Aligned Addr for %lx = %lx\n", (UINT)pAddr, (UINT)pRet);
    assertTrue(pRet == (void *)0x8012ff80);

    pAddr = (void *)0x8012ff40;
    pRet = GetCacheAlignedAddr(pAddr);
    printf( "Cache Aligned Addr for %lx = %lx\n", (UINT)pAddr, (UINT)pRet);
    assertTrue(pRet == (void *)0x8012ff40);


    pAddr = (void *)0x8012ff39;
    pRet = GetCacheAlignedAddr(pAddr);
    printf( "Cache Aligned Addr for %lx = %lx\n", (UINT)pAddr, (UINT)pRet);
    assertTrue(pRet == (void *)0x8012ff40);

    pAddr = (void *)0x8012ff79;
    pRet = GetCacheAlignedAddr(pAddr);
    printf( "Cache Aligned Addr for %lx = %lx\n", (UINT)pAddr, (UINT)pRet);
    assertTrue(pRet == (void *)0x8012ff80);

}

