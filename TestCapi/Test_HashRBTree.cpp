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
#include "capiglobal.h"
#include "TestApi.h"
#include "BinTree.h"
#include "RBTree.h"
#include "HashRBTree.h"


void DRV_HashRBTree_Create(UINT i);
void DRV_HashRBTree_Insert(UINT i);
void DRV_HashRBTree_Delete(UINT i);
void DRV_HashRBTree_HashFind(UINT i);
void DRV_HashRBTree_TreeFind(UINT i);
void DRV_HashRBTree_EnumNext(UINT i);

typedef HASHRBTREE HashRBTree;


void Test_HashRBTree()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_HashRBTree_Create(i);
        DRV_HashRBTree_Insert(i);
        DRV_HashRBTree_Delete(i);
        DRV_HashRBTree_HashFind(i);
        DRV_HashRBTree_TreeFind(i);
        DRV_HashRBTree_EnumNext(i);
        
    }    
}

REGISTER_TESTFUNC(Test_HashRBTree)

void DRV_HashRBTree_Create(UINT i)
{
    HashRBTree *pTable = NULL;
    switch( i )
    {
    case 1:
        pTable = HashRBTree_Create(0);
        if ( pTable != NULL )
        {           
            printf( "HashRBTree_Create(0) ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        pTable = HashRBTree_Create(1);
        if ( pTable == NULL
            || pTable->pTree->pRoot != NULL
            || pTable->pTree->uNodeCount != 0
            || pTable->pTree->pCursor != NULL
            || pTable->ppBucket == NULL 
            || pTable->uBucketCount != 1  )
        {           
            printf( "HashRBTree_Create(1) ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        break;
    case 3:
        pTable = HashRBTree_Create(2);
        if ( pTable == NULL
            || pTable->pTree->pRoot != NULL
            || pTable->pTree->uNodeCount != 0
            || pTable->pTree->pCursor != NULL
            || pTable->ppBucket == NULL 
            || pTable->uBucketCount != 2  )
        {           
            printf( "HashRBTree_Create(2) ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }
        break;
    case 4:
        pTable = HashRBTree_Create(1024);
        if ( pTable == NULL
            || pTable->pTree->pRoot != NULL
            || pTable->pTree->uNodeCount != 0
            || pTable->pTree->pCursor != NULL
            || pTable->ppBucket == NULL 
            || pTable->uBucketCount != 1024  )
        {           
            printf( "HashRBTree_Create(1024) ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        break;
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashRBTree_Destroy(pTable, free);
    }
}

void DRV_HashRBTree_Insert(UINT i)
{
    INT         nRet;
    UINT         uIndex;
    HashRBTree *pTable = NULL;
    pTable = HashRBTree_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->TreeNode.pData, "100") != 0 
            || pTable->ppBucket[uIndex]->pNext != NULL 
            || pTable->pTree->uNodeCount != 1
            || strcmp( (char *)pTable->pTree->pRoot->pData, "100" ) != 0 
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashRBTree_Insert() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
//        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->TreeNode.pData, "100") != 0 
            || pTable->ppBucket[uIndex]->pNext != NULL 
            || pTable->pTree->uNodeCount != 1
            || strcmp( (char *)pTable->pTree->pRoot->pData, "100" ) != 0 
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashRBTree_Insert() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        break;
    case 3:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->TreeNode.pData, "100") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->TreeNode.pData, "728") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->pNext->TreeNode.pData, "252") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext->pNext != NULL 
            || pTable->pTree->uNodeCount != 3
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashRBTree_Insert() ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }

        break;
    default:
        break;
    }
    BinTree_CheckParent(pTable->pTree->pRoot);

    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashRBTree_Destroy(pTable, free);
    }
}


void DRV_HashRBTree_Delete(UINT i)
{
    INT         nRet;
    UINT         uIndex;
    HashRBTree *pTable = NULL;
    pTable = HashRBTree_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        HashRBTree_Delete(pTable, "100", HashString, StrCompare, free);

        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || pTable->ppBucket[uIndex] != NULL
            || pTable->pTree->uNodeCount != 0
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashRBTree_Delete() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        HashRBTree_Delete(pTable, "728", HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->TreeNode.pData, "100") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->TreeNode.pData, "252") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext != NULL 
            || pTable->pTree->uNodeCount != 2
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashRBTree_Delete() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        
        break;
    case 3:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        HashRBTree_Delete(pTable, "100", HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->TreeNode.pData, "728") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->TreeNode.pData, "252") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext != NULL 
            || pTable->pTree->uNodeCount != 2
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashRBTree_Delete() ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }
        
        break;
    case 4:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        HashRBTree_Delete(pTable, "252", HashString, StrCompare, free);
        uIndex = HashString("100", 128);
        if ( nRet != CAPI_SUCCESS
            || strcmp((char *)pTable->ppBucket[uIndex]->TreeNode.pData, "100") != 0 
            || strcmp((char *)pTable->ppBucket[uIndex]->pNext->TreeNode.pData, "728") != 0 
            || pTable->ppBucket[uIndex]->pNext->pNext != NULL 
            || pTable->pTree->uNodeCount != 2
            || pTable->uBucketCount != 128 )
        {           
            printf( "HashRBTree_Delete() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        
        break;
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashRBTree_Destroy(pTable, free);
    }
}

void DRV_HashRBTree_HashFind(UINT i)
{
    void        *pData;
    INT         nRet;
    HashRBTree *pTable = NULL;
    pTable = HashRBTree_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);

        pData = HashRBTree_HashFind(pTable, "100", HashString, StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_HashFind() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
//        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_HashFind(pTable, "100", HashString, StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_HashFind() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 3:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_HashFind(pTable, "252", HashString, StrCompare);
        if ( strcmp((char *)pData, "252") != 0 )
        {           
            printf( "HashRBTree_HashFind() ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }
        break;
    case 4:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_HashFind(pTable, "728", HashString, StrCompare);
        if ( strcmp((char *)pData, "728") != 0 )
        {           
            printf( "HashRBTree_HashFind() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        break;    
    case 5:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_HashFind(pTable, "100", HashString, StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_HashFind() ²âÊÔÓÃÀý5Ê§°Ü!\n" );
        }
        break;    
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashRBTree_Destroy(pTable, free);
    }
}

void DRV_HashRBTree_EnumNext(UINT i)
{
    void        *pData;
    INT         nRet;
    HashRBTree *pTable = NULL;
    pTable = HashRBTree_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        
        HashRBTree_EnumBegin(pTable);
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( pData != NULL )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }

        break;

    case 3:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        HashRBTree_EnumBegin(pTable);
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "252") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "728") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( pData != NULL )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý2Ê§°Ü!\n" );
        }

        break;
    case 4:
        nRet = HashRBTree_Insert(pTable, strdup("88"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("77"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("101"), HashString, StrCompare, free);
        HashRBTree_EnumBegin(pTable);
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "101") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "252") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "77") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( strcmp((char *)pData, "88") != 0 )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        pData = HashRBTree_EnumNext(pTable);
        if ( pData != NULL )
        {           
            printf( "HashRBTree_EnumNext() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        break;    

    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashRBTree_Destroy(pTable, free);
    }
}


void DRV_HashRBTree_TreeFind(UINT i)
{
    void        *pData;
    INT         nRet;
    HashRBTree *pTable = NULL;
    pTable = HashRBTree_Create(128);
    switch( i )
    {
    case 1:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        
        pData = HashRBTree_TreeFind(pTable, "100", StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_TreeFind() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 2:
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
//        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_TreeFind(pTable, "100", StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_TreeFind() ²âÊÔÓÃÀý1Ê§°Ü!\n" );
        }
        break;
    case 3:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_TreeFind(pTable, "252", StrCompare);
        if ( strcmp((char *)pData, "252") != 0 )
        {           
            printf( "HashRBTree_TreeFind() ²âÊÔÓÃÀý3Ê§°Ü!\n" );
        }
        break;
    case 4:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_TreeFind(pTable, "728", StrCompare);
        if ( strcmp((char *)pData, "728") != 0 )
        {           
            printf( "HashRBTree_TreeFind() ²âÊÔÓÃÀý4Ê§°Ü!\n" );
        }
        break;    
    case 5:
        nRet = HashRBTree_Insert(pTable, strdup("252"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("728"), HashString, StrCompare, free);
        nRet = HashRBTree_Insert(pTable, strdup("100"), HashString, StrCompare, free);
        pData = HashRBTree_TreeFind(pTable, "100", StrCompare);
        if ( strcmp((char *)pData, "100") != 0 )
        {           
            printf( "HashRBTree_TreeFind() ²âÊÔÓÃÀý5Ê§°Ü!\n" );
        }
        break;    
    default:
        break;
    }
    if ( pTable != NULL && pTable->uBucketCount != 0  )
    {
        HashRBTree_Destroy(pTable, free);
    }
}

