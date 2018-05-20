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
#include "HashTable.h"


void DRV_HashTable_Create(UINT i);
void DRV_HashTable_Insert(UINT i);
void DRV_HashTable_Delete(UINT i);
void DRV_HashTable_Find(UINT i);
void DRV_HashTable_EnumNext(UINT i);

typedef HASHTABLE HashTable;

void FindRepeatKey();

void Test_HashTable()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_HashTable_Create(i);
        DRV_HashTable_Insert(i);
        DRV_HashTable_Delete(i);
        DRV_HashTable_Find(i);
        DRV_HashTable_EnumNext(i);
        
    }    

    FindRepeatKey();
}

REGISTER_TESTFUNC(Test_HashTable)

void FindRepeatKey()
{
    UINT    uKey;
    INT     i = 0 ;

    char    szMsg[8];

    for ( i = 0; i < 1000; i++ )
    {
        itoa(i, szMsg, 10);
        
        uKey = HashString(szMsg, 128);

        if ( uKey == 97 )
        {
            printf( "Key 97 = %s\n", szMsg);
        }
        if ( uKey == 98 )
        {
            printf( "Key 98 = %s\n", szMsg);
        }

        if ( i == 77 )
        {
            printf( "Key 77 = %u\n", uKey);
        }
        if ( i == 88 )
        {
            printf( "Key 88 = %u\n", uKey);
        }
    }
}

void DRV_HashTable_Create(UINT i)
{
    HashTable *pTable = NULL;
    switch( i )
    {
    case 1:
        pTable = HashTable_Create(0);
        if ( pTable != NULL )
        {           
            printf( "HashTable_Create(0) ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        pTable = HashTable_Create(1);
        if ( pTable == NULL
            || pTable->ppBucket == NULL 
            || pTable->uBucketCount != 1 
            || pTable->uNodeCount != 0 
            || pTable->uCurBucketNo != 0 
            || pTable->pCurEntry != NULL )
        {           
            printf( "HashTable_Create(1) ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        break;
    case 3:
        pTable = HashTable_Create(2);
        if ( pTable == NULL
            || pTable->ppBucket == NULL 
            || pTable->uBucketCount != 2 
            || pTable->uNodeCount != 0 
            || pTable->uCurBucketNo != 0 
            || pTable->pCurEntry != NULL )
        {           
            printf( "HashTable_Create(2) ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }
        break;
    case 4:
        pTable = HashTable_Create(1024);
        if ( pTable == NULL
            || pTable->ppBucket == NULL 
            || pTable->uBucketCount != 1024 
            || pTable->uNodeCount != 0 
            || pTable->uCurBucketNo != 0 
            || pTable->pCurEntry != NULL )
        {           
            printf( "HashTable_Create(1024) ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        break;
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashTable_Destroy(pTable, NULL);
    }
}

void DRV_HashTable_Insert(UINT i)
{
    INT         nRet;
    UINT         uIndex;
    HashTable *pTable = NULL;
    pTable = HashTable_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->pData, "100") != 0 
            || pTable->ppBucket[uIndex]->pNext != NULL 
            || pTable->uNodeCount != 1
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashTable_Insert() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        nRet = HashTable_Insert(pTable, strdup("120"), HashString);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->pData, "100") != 0 
            || pTable->ppBucket[uIndex]->pNext != NULL 
            || pTable->uNodeCount != 2
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashTable_Insert() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        break;
    case 3:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->pData, "100") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->pData, "728") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->pNext->pData, "252") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext->pNext != NULL 
            || pTable->uNodeCount != 3
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashTable_Insert() ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }

        break;
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashTable_Destroy(pTable, free);
    }
}


void DRV_HashTable_Delete(UINT i)
{
    INT         nRet;
    UINT         uIndex;
    HashTable *pTable = NULL;
    pTable = HashTable_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        HashTable_Delete(pTable, "100", HashString, StrCompare, free);

        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || pTable->ppBucket[uIndex] != NULL
            || pTable->uNodeCount != 0
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashTable_Delete() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        HashTable_Delete(pTable, "728", HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->pData, "100") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->pData, "252") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext != NULL 
            || pTable->uNodeCount != 2
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashTable_Delete() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        
        break;
    case 3:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        HashTable_Delete(pTable, "100", HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->pData, "728") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->pData, "252") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext != NULL 
            || pTable->uNodeCount != 2
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashTable_Delete() ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }
        
        break;
    case 4:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        HashTable_Delete(pTable, "252", HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->pData, "100") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->pData, "728") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext != NULL 
            || pTable->uNodeCount != 2
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashTable_Delete() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        
        break;
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashTable_Destroy(pTable, free);
    }
}

void DRV_HashTable_Find(UINT i)
{
    void        *pData;
    INT         nRet;
    HashTable *pTable = NULL;
    pTable = HashTable_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);

        pData = HashTable_Find(pTable, "100", HashString, StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashTable_Find() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        pData = HashTable_Find(pTable, "100", HashString, StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashTable_Find() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 3:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        pData = HashTable_Find(pTable, "252", HashString, StrCompare);
        if ( strcmp((char *)pData, "252") != 0 )
        {           
            printf( "HashTable_Find() ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }
        break;
    case 4:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        pData = HashTable_Find(pTable, "728", HashString, StrCompare);
        if ( strcmp((char *)pData, "728") != 0 )
        {           
            printf( "HashTable_Find() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        break;    
    case 5:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        pData = HashTable_Find(pTable, "100", HashString, StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashTable_Find() ²âÊÔÓÃÀý5Ê§°Ü!\n" );
        }
        break;    
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashTable_Destroy(pTable, free);
    }
}

void DRV_HashTable_EnumNext(UINT i)
{
    void        *pData;
    INT         nRet;
    HashTable *pTable = NULL;
    pTable = HashTable_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        
        HashTable_EnumBegin(pTable);
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( pData != NULL )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }

        break;

    case 3:
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("728"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        HashTable_EnumBegin(pTable);
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "728") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "252") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( pData != NULL )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }

        break;
    case 4:
        nRet = HashTable_Insert(pTable, strdup("88"), HashString);
        nRet = HashTable_Insert(pTable, strdup("77"), HashString);
        nRet = HashTable_Insert(pTable, strdup("100"), HashString);
        nRet = HashTable_Insert(pTable, strdup("252"), HashString);
        nRet = HashTable_Insert(pTable, strdup("101"), HashString);
        HashTable_EnumBegin(pTable);
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "77") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "88") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "252") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( strcmp((char *)pData, "101") != 0 )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashTable_EnumNext(pTable);
        if ( pData != NULL )
        {           
            printf( "HashTable_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        break;    

    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashTable_Destroy(pTable, free);
    }
}

