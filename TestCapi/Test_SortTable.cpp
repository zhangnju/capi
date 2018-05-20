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
#include "SortTable.h"

void DRV_SortTable_Create(UINT i);
void DRV_SortTable_Add(UINT i);
void DRV_SortTable_FindData(UINT i);
void DRV_SortTable_BlurFind(UINT i);


void Test_SortTable()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_SortTable_Create(i);
        DRV_SortTable_Add(i);
        DRV_SortTable_FindData(i);
        DRV_SortTable_BlurFind(i);
    }    
}

REGISTER_TESTFUNC(Test_SortTable)

void DRV_SortTable_Create(UINT i)
{
    SORTTABLE *pTable = NULL;
    switch( i )
    {
    case 1:
        pTable = SortTable_Create(0);
        if ( pTable != NULL )
        {           
            printf( "SortTable_Create(0) ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        pTable = SortTable_Create(1);
        if ( pTable == NULL
            || pTable->ppData == NULL 
            || pTable->uCursorCount != 0 
            || pTable->uMaxCount != 1 )
        {           
            printf( "SortTable_Create(1) ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        pTable = SortTable_Create(2);
        if ( pTable == NULL
            || pTable->ppData == NULL 
            || pTable->uCursorCount != 0 
            || pTable->uMaxCount != 2 )
        {           
            printf( "SortTable_Create(2) ≤‚ ‘”√¿˝3 ß∞‹!\n" );
        }
        break;
    case 4:
        pTable = SortTable_Create(1024);
        if ( pTable == NULL
            || pTable->ppData == NULL 
            || pTable->uCursorCount != 0 
            || pTable->uMaxCount != 1024 )
        {           
            printf( "SortTable_Create(1024) ≤‚ ‘”√¿˝4 ß∞‹!\n" );
        }
        break;
    default:
        break;
    }
    if ( pTable != NULL && pTable->uMaxCount != 0  )
    {
        SortTable_Destroy(pTable, free);
    }
}


void DRV_SortTable_Add(UINT i)
{
    INT     nRet;
    SORTTABLE *pTable = NULL;
    pTable = SortTable_Create(2);
    switch( i )
    {
    case 1:
        nRet = SortTable_Add(pTable, strdup("sorttable_1"));
        if ( nRet != CAPI_SUCCESS 
            || pTable->uCursorCount != 1
            || pTable->uMaxCount != 2 
            || strcmp((char *)(pTable->ppData[0]), "sorttable_1") != 0
             )
        {
            printf( "SortTable_Add() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }
        break;
    case 2:
        SortTable_Add(pTable, strdup("sorttable_1"));
        nRet = SortTable_Add(pTable, strdup("sorttable_2"));
        if ( nRet != CAPI_SUCCESS
            || pTable->uCursorCount != 2
            || pTable->uMaxCount != 2 
            || strcmp((char *)pTable->ppData[0], "sorttable_1") != 0
             )
        {
            printf( "SortTable_Add() ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        break;
    case 4:
        break;
    default:
        break;
    }
    if ( pTable != NULL && pTable->uMaxCount != 0  )
    {
        SortTable_Destroy(pTable, free);
    }
}

void DRV_SortTable_FindData(UINT i)
{
    SORTTABLE *pTable = NULL;
    pTable = SortTable_Create(128);
    char *psz;
    switch( i )
    {
    case 1:
        SortTable_Add(pTable, strdup("sorttable_8"));
        SortTable_Add(pTable, strdup("sorttable_3"));
        SortTable_Add(pTable, strdup("sorttable_1"));
        SortTable_Add(pTable, strdup("sorttable_4"));
        SortTable_Add(pTable, strdup("sorttable_6"));
        SortTable_Add(pTable, strdup("sorttable_7"));
        SortTable_Add(pTable, strdup("sorttable_9"));
        SortTable_Add(pTable, strdup("sorttable_5"));
        SortTable_Add(pTable, strdup("sorttable_2"));
        SortTable_Add(pTable, strdup("sorttable_0"));
        SortTable_Add(pTable, strdup("sorttable_99"));

        SortTable_Sort(pTable, StrCompare);

        psz = (char *)SortTable_FindData(pTable, "sorttable_8", StrCompare);
        if ( strcmp(psz, "sorttable_8") != 0
            || strcmp((char *)pTable->ppData[0], "sorttable_0") != 0
            || strcmp((char *)pTable->ppData[1], "sorttable_1") != 0
            || strcmp((char *)pTable->ppData[2], "sorttable_2") != 0
            || strcmp((char *)pTable->ppData[3], "sorttable_3") != 0
            || strcmp((char *)pTable->ppData[4], "sorttable_4") != 0
            || strcmp((char *)pTable->ppData[5], "sorttable_5") != 0
            || strcmp((char *)pTable->ppData[6], "sorttable_6") != 0
            || strcmp((char *)pTable->ppData[7], "sorttable_7") != 0
            || strcmp((char *)pTable->ppData[8], "sorttable_8") != 0
            || strcmp((char *)pTable->ppData[9], "sorttable_9") != 0
            || strcmp((char *)pTable->ppData[10], "sorttable_99") != 0
            )
        {
            printf( "SortTable_FindData() ≤‚ ‘”√¿˝1 ß∞‹!\n" );
        }

        break;
    case 2:
        SortTable_Add(pTable, strdup("sorttable_8"));

        SortTable_Sort(pTable, StrCompare);
        
        psz = (char *)SortTable_FindData(pTable, "sorttable_8", StrCompare);
        if ( strcmp(psz, "sorttable_8") != 0
            || strcmp((char *)pTable->ppData[0], "sorttable_8") != 0
            )
        {
            printf( "SortTable_FindData() ≤‚ ‘”√¿˝2 ß∞‹!\n" );
        }
        break;
    case 3:
        SortTable_Add(pTable, strdup("sorttable_8"));
        SortTable_Add(pTable, strdup("sorttable_5"));
        
        SortTable_Sort(pTable, StrCompare);
        
        psz = (char *)SortTable_FindData(pTable, "sorttable_8", StrCompare);
        if ( strcmp(psz, "sorttable_8") != 0
            || strcmp((char *)pTable->ppData[0], "sorttable_5") != 0
            || pTable->uCursorCount != 2 
            )
        {
            printf( "SortTable_FindData() ≤‚ ‘”√¿˝3 ß∞‹!\n" );
        }
        break; 
    case 4:
        break;
    default:
        break;
    }
    SortTable_Destroy(pTable, free);
}


void DRV_SortTable_BlurFind(UINT i)
{
    SORTTABLE *pTable = NULL;
    pTable = SortTable_Create(128);
    char *psz;
    switch( i )
    {
    case 1:
        psz = (char *)SortTable_BlurFind(pTable, "123", StrCompare);
        assertTrue(psz == NULL);
        break;
    case 2:
        SortTable_Add(pTable, strdup("sorttable_1"));
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_1", StrCompare);
        assertTrue(strcmp(psz, "sorttable_1") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_0", StrCompare);
        assertTrue(strcmp(psz, "sorttable_1") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_2", StrCompare);
        assertTrue(psz == NULL);
        break;
    case 3:
        SortTable_Add(pTable, strdup("sorttable_1"));
        SortTable_Add(pTable, strdup("sorttable_5"));
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_0", StrCompare);
        assertTrue(strcmp(psz, "sorttable_1") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_1", StrCompare);
        assertTrue(strcmp(psz, "sorttable_1") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_2", StrCompare);
        assertTrue(strcmp(psz, "sorttable_5") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_5", StrCompare);
        assertTrue(strcmp(psz, "sorttable_5") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_6", StrCompare);
        assertTrue(psz == NULL);
        break;
    case 4:
        SortTable_Add(pTable, strdup("sorttable_1"));
        SortTable_Add(pTable, strdup("sorttable_3"));
        SortTable_Add(pTable, strdup("sorttable_5"));
        SortTable_Add(pTable, strdup("sorttable_7"));
        SortTable_Add(pTable, strdup("sorttable_9"));
        SortTable_Add(pTable, strdup("sorttable_99"));


        psz = (char *)SortTable_BlurFind(pTable, "sorttable_0", StrCompare);
        assertTrue(strcmp(psz, "sorttable_1") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_1", StrCompare);
        assertTrue(strcmp(psz, "sorttable_1") == 0);

        psz = (char *)SortTable_BlurFind(pTable, "sorttable_2", StrCompare);
        assertTrue(strcmp(psz, "sorttable_3") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_3", StrCompare);
        assertTrue(strcmp(psz, "sorttable_3") == 0);

        psz = (char *)SortTable_BlurFind(pTable, "sorttable_4", StrCompare);
        assertTrue(strcmp(psz, "sorttable_5") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_5", StrCompare);
        assertTrue(strcmp(psz, "sorttable_5") == 0);

        psz = (char *)SortTable_BlurFind(pTable, "sorttable_6", StrCompare);
        assertTrue(strcmp(psz, "sorttable_7") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_7", StrCompare);
        assertTrue(strcmp(psz, "sorttable_7") == 0);

        psz = (char *)SortTable_BlurFind(pTable, "sorttable_8", StrCompare);
        assertTrue(strcmp(psz, "sorttable_9") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_9", StrCompare);
        assertTrue(strcmp(psz, "sorttable_9") == 0);

        psz = (char *)SortTable_BlurFind(pTable, "sorttable_91", StrCompare);
        assertTrue(strcmp(psz, "sorttable_99") == 0);
        psz = (char *)SortTable_BlurFind(pTable, "sorttable_99", StrCompare);
        assertTrue(strcmp(psz, "sorttable_99") == 0);

        psz = (char *)SortTable_BlurFind(pTable, "sorttable_991", StrCompare);
        assertTrue(psz == NULL);
        break;
    default:
        break;
    }

}