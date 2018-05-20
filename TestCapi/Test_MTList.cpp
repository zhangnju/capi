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
#include <process.h>
#include "CapiGlobal.h"
#include "TestApi.h"
#include "DoubleList.h"
#include "MTask.h"
#include "MTList.h"

extern void *StrCopy(void *psz);


void TestCase_MTList_Insert();

void Test_MTList()
{
    TestCase_Add(TestCase_MTList_Insert);
}

REGISTER_TESTFUNC(Test_MTList)

INT Visit(void *p)
{
    char *psz = (char *)p;
    if ( psz != NULL )
    {
        *psz = '8';
        return CAPI_SUCCESS;
    }
    return CAPI_FAILED;
}


void MTList_TraverseTask(LPVOID args)
{
    MTLIST *pList = (MTLIST *)args;
    MTList_EnterTask(pList);
    for ( ; ; )
    {
        if ( MTList_GetExitFlag(pList) == MTASK_EXIT )
        {
            break;
        }
        MTList_EnumBegin(pList);
        while ( ( MTList_EnumNext(pList, Visit)) == CAPI_FAILED )
        {
            break;
            //  printf("%s\n", p);
        }
    }
    MTList_LeaveTask(pList);

    return ;
}




void TestCase_MTList_Insert()
{
    MTLIST  *pList = MTList_Create();

    MTList_InsertHead(pList, StrCopy("20"));
    MTList_InsertHead(pList, StrCopy("21"));
    MTList_InsertHead(pList, StrCopy("22"));
    MTList_InsertHead(pList, StrCopy("23"));
    MTList_InsertHead(pList, StrCopy("24"));
    MTList_InsertHead(pList, StrCopy("25"));
    MTList_InsertHead(pList, StrCopy("26"));
    MTList_InsertHead(pList, StrCopy("27"));
    MTList_InsertHead(pList, StrCopy("28"));
    MTList_InsertHead(pList, StrCopy("29"));
    MTList_InsertHead(pList, StrCopy("30"));
    MTList_InsertHead(pList, StrCopy("31"));
    MTList_InsertHead(pList, StrCopy("32"));
    MTList_InsertHead(pList, StrCopy("33"));
    MTList_InsertHead(pList, StrCopy("34"));
    MTList_InsertHead(pList, StrCopy("35"));
    MTList_InsertHead(pList, StrCopy("36"));
    MTList_InsertHead(pList, StrCopy("37"));

    _beginthread(MTList_TraverseTask, 0, pList);

    int i;
    for ( i = 0; i < 100; i++ )
    {
        void *pData = MTList_PopHead(pList);
        if ( pData != NULL )
        {
//            printf( "pData = %s\n", pData );
            free(pData);
        }
        char *psz = (char *)StrCopy("27");
        MTList_InsertTail(pList, psz);
    }
    Sleep(100);
    MTList_Destroy(pList, free);
    return;
}
