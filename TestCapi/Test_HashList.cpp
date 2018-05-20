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
#include "CapiGlobal.h"
#include "TestApi.h"
#include "SingleList.h"
#include "HashTable.h"
#include "HashList.h"

INT DRV_HashList_Create(INT i);
INT DRV_HashList_AddHead(INT i);

void Test_HashList()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_HashList_Create(i);
        DRV_HashList_AddHead(i);
    }    
}

REGISTER_TESTFUNC(Test_HashList)

INT DRV_HashList_Create(INT i)
{
    HASHLIST    *pHashList = NULL;
    switch ( i )
    {
    case 1: //测试创建参数uBucketNum为0的情况
        pHashList = HashList_Create(0);
        if ( pHashList == NULL )
        {
            printf("测试用例1失败\r\n");
        }
        break;
    case 2: //测试创建参数uBucketNum为1的情况
        pHashList = HashList_Create(1);
        if ( pHashList == NULL )
        {
            printf("测试用例1失败\r\n");
        }
        break;
    case 3: //测试创建参数uBucketNum为32的情况
        pHashList = HashList_Create(32);
        if ( pHashList == NULL )
        {
            printf("测试用例1失败\r\n");
        }
        break;
    default:
        break;
    }
    if ( pHashList != NULL )
    {
        HashList_Destroy(pHashList, NULL);
    }
    return 0;
}

INT DRV_HashList_AddHead(INT i)
{
    HASHLIST    *pHashList = NULL;
    void *p;

    switch ( i )
    {
    case 1: //测试创建参数uBucketNum为0的情况
        pHashList = HashList_Create(0);
        if ( pHashList == NULL )
        {
            printf("测试用例1失败\r\n");
        }
        HashList_InsertHead(pHashList, strdup("Hello"), HashString);

        p = HashList_FindData(pHashList, "Hello", HashString, StrCompare);

        printf( "result = %s\r\n", (char *)p);

        HashList_Destroy(pHashList, free);
        pHashList = NULL;
        break;
    case 2: //测试创建参数uBucketNum为1的情况
        pHashList = HashList_Create(1);
        if ( pHashList == NULL )
        {
            printf("测试用例1失败\r\n");
        }
        break;
    case 3: //测试创建参数uBucketNum为32的情况
        pHashList = HashList_Create(32);
        if ( pHashList == NULL )
        {
            printf("测试用例1失败\r\n");
        }
        break;
    default:
        break;
    }
    if ( pHashList != NULL )
    {
        HashList_Destroy(pHashList, NULL);
    }
    return 0;
}