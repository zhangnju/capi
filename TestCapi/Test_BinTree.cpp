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
#include "BinTree.h"
#include "AVLTree.h"

void DRV_BinTree_InOrderTraverse(UINT i);
void DRV_BinTree_PreOrderTraverse(UINT i);
void DRV_BinTree_PostOrderTraverse(UINT i);

void Test_BinTree()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_BinTree_InOrderTraverse(i);
        DRV_BinTree_PreOrderTraverse(i);
        DRV_BinTree_PostOrderTraverse(i);
    }    
}

REGISTER_TESTFUNC(Test_BinTree);

static INT  g_nIndex = 0;

const char *g_szMsg[10] = {"30", "20", "15", "25", "50" };
char *g_szMsg1[10] = {"15", "20", "25", "30", "50" };
char *g_szMsg2[10] = {"15", "25", "20", "50", "30" };
char *g_szMsg3[15] = {"10", "15", "22", "26", "27", "25","20", "40","65", "60","50", "30" };
char *g_szMsgPre[15] = {"30", "20", "15", "10", "25", "22","27", "26","50", "40","60", "65" };
char *g_szMsgIn[15] = {"10", "15", "20", "22", "25", "26","27", "30","40", "50","60", "65" };

INT PreDataPrint(void *pData)
{
    if ( strcmp(g_szMsg[g_nIndex], (char *)pData) != 0 )
    {
        printf( "pData = %s is not equal to %s.\n", (char *)pData,
            g_szMsg[g_nIndex]);        
    }
    g_nIndex++;
    return 0;
}

INT PreDataPrint1(void *pData)
{
    if ( strcmp(g_szMsgPre[g_nIndex], (char *)pData) != 0 )
    {
        printf( "pData = %s is not equal to %s.\n", (char *)pData,
            g_szMsg[g_nIndex]);        
    }
    g_nIndex++;
    return 0;
}

INT InDataPrint(void *pData)
{
    if ( strcmp(g_szMsg1[g_nIndex], (char *)pData) != 0 )
    {
        printf( "pData = %s is not equal to %s.\n", (char *)pData,
            g_szMsg1[g_nIndex]);        
    }
    g_nIndex++;
    return 0;
}

INT InDataPrint1(void *pData)
{
    if ( strcmp(g_szMsgIn[g_nIndex], (char *)pData) != 0 )
    {
        printf( "pData = %s is not equal to %s.\n", (char *)pData,
            g_szMsg1[g_nIndex]);        
    }
    g_nIndex++;
    return 0;
}

INT PostDataPrint(void *pData)
{
    if ( strcmp(g_szMsg2[g_nIndex], (char *)pData) != 0 )
    {
        printf( "pData = %s is not equal to %s.\n", (char *)pData,
            g_szMsg2[g_nIndex]);        
    }
    g_nIndex++;
    return 0;
}

INT PostDataPrint1(void *pData)
{
    if ( strcmp(g_szMsg3[g_nIndex], (char *)pData) != 0 )
    {
        printf( "pData = %s is not equal to %s.\n", (char *)pData,
            g_szMsg3[g_nIndex]);        
    }
    g_nIndex++;
    return 0;
}

void DRV_BinTree_InOrderTraverse(UINT i)
{
    BINTREE *pTree;
    pTree = BinTree_Create();
    switch( i )
    {
    case 1:
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);

        g_nIndex = 0;
        BinTree_InOrderTraverse(pTree, InDataPrint);
        break;
    case 2:
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_InOrderTraverse(pTree, InDataPrint);
        break;
    case 3:
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("22"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("27"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("26"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("60"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("65"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("10"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_InOrderTraverse(pTree, InDataPrint1);
        break;
    case 4:
        break;
    default:
        break;
    }
    if ( pTree != NULL )
    {
        BinTree_Destroy(pTree, free);
    }
}

void DRV_BinTree_PreOrderTraverse(UINT i)
{
    BINTREE *pTree;
    pTree = BinTree_Create();
    switch( i )
    {
    case 1:
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_PreOrderTraverse(pTree, PreDataPrint);
        break;
    case 2:
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_PreOrderTraverse(pTree, PreDataPrint);
        break;
    case 3:
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("22"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("27"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("26"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("60"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("65"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("10"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_PreOrderTraverse(pTree, PreDataPrint1);
        break;
    case 4:
        break;
    default:
        break;
    }
    if ( pTree != NULL )
    {
        BinTree_Destroy(pTree, free);
    }
}

void DRV_BinTree_PostOrderTraverse(UINT i)
{
    BINTREE *pTree;
    pTree = BinTree_Create();
    switch( i )
    {
    case 1:
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_PostOrderTraverse(pTree, PostDataPrint);
        break;
    case 2:
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_PostOrderTraverse(pTree, PostDataPrint);
        break;
    case 3:
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("22"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("27"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("26"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("60"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("65"), StrCompare, 0);
        BinTree_Insert(&(pTree->pRoot), strdup("10"), StrCompare, 0);
        
        g_nIndex = 0;
        BinTree_PostOrderTraverse(pTree, PostDataPrint1);
        break;
    case 4:
        break;
    default:
        break;
    }
    if ( pTree != NULL )
    {
        BinTree_Destroy(pTree, free);
    }
}