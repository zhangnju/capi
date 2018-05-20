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
#include "Testapi.h"
#include "BinTree.h"
#include "RBTree.h"
#include "MTask.h"
#include "MTree.h"

typedef struct MTREE_st {
    RBTREE      *pRBTree;
    RBTREENODE  *pCursor;
    MTASK       *pMTask;
} MTREE;

typedef MTREE   MTree;

INT DRV_MTree_Create(INT i);
INT DRV_MTree_Destroy(INT i);
INT DRV_MTree_Insert(INT i);
INT DRV_MTree_Delete(INT i);



void *StrCopy(void *psz)
{
    if ( psz != NULL )
    {
//      char *p = strdup((char *)psz);
        char *p = (char *)malloc(strlen((char *)psz) + 1);
        strcpy(p, (char *)psz);
        return (void *)p;
    }
    else
    {
        return NULL;
    }
}

void MTree_TraverseTask(LPVOID args)
{
    HANDLE   hTree = (HANDLE)args;
    MTree_EnterTask(hTree);
    for ( ; ; )
    {
        if ( MTree_GetExitFlag(hTree) == MTASK_EXIT )
        {
            break;
        }
        void *p;
        MTree_EnumBegin(hTree);
        while ( (p = MTree_EnumNextCopy(hTree, StrCopy)) != NULL )
        {
          //  printf("%s\n", p);
            free(p);
        }
        //printf("Traverse one turn.\n");
    }
    MTree_LeaveTask(hTree);

    return ;
}

void Test_MTree()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
//        DRV_MTree_Create(i);
//        DRV_MTree_Destroy(i);
//        DRV_MTree_Insert(i);
//        DRV_MTree_Delete(i);
//        DRV_MTree_Find(i);
    }    
#if 1
    HANDLE   hTree = MTree_Create();

    void *psz;
    psz = StrCopy("20");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("21");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("22");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("23");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("24");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("25");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("26");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("27");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("28");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("29");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("30");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("31");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("32");
    MTree_Insert(hTree, psz, StrCompare);
    psz = StrCopy("33");
    MTree_Insert(hTree, psz, StrCompare);

    _beginthread(MTree_TraverseTask, 0, hTree);
    //hThread = CreateThread(NULL, 0, MTree_TraverseTask, (void *)hTree, 0, &dwId);
#if 1
    for ( i = 0; i < 1000; i++ )
    {
        int ret = MTree_Delete(hTree, "27", StrCompare, free);
        if ( ret != CAPI_SUCCESS )
        {
            printf( "MTree_Delete(27) Failed.\n" );
        }
        psz = StrCopy("27");
        MTree_Insert(hTree, psz, StrCompare);
    }
#endif
    Sleep(100);
    MTree_Destroy(hTree, free);
#endif 
}

REGISTER_TESTFUNC(Test_MTree)

INT DRV_MTree_Create(INT i)
{
    MTREE  *pTree = NULL;
    switch( i )
    {
    case 1:
        pTree = (MTREE *)MTree_Create();
        if ( pTree != NULL
            && pTree->pRBTree->pCursor == NULL
            && pTree->pRBTree->pRoot == NULL 
            && pTree->pRBTree->uNodeCount == 0 
            && pTree->pMTask->pExitEvent != NULL 
            && pTree->pMTask->uExitFlag != MTASK_EXIT
            && pTree->pMTask->uTaskCount == 0 )
        {
        }
        else
        {
            printf("MTree_Create() 测试用例1失败\n");
        }
        break;
    default:
        break;
    }
    MTree_Destroy(pTree, free);
    return 1;
}

INT DRV_MTree_Destroy(INT i)
{
    MTree  *pTree = NULL;
    pTree = (MTREE *)MTree_Create();
    switch( i )
    {
    case 1:
        MTree_Destroy((HANDLE)pTree, free);
        break;
    default:
        MTree_Destroy((HANDLE)pTree, free);
        break;
    }
    return 1;
}

INT DRV_MTree_Insert(INT i)
{
    MTree  *pTree = NULL;
    pTree = (MTREE *)MTree_Create();
    switch( i )
    {
    case 1: /* 插入1个节点的情况 */
        MTree_Insert(pTree, StrCopy("20"), StrCompare);
        if ( strcmp((char *)(pTree->pRBTree->pRoot->pData), "20") == 0 
            && pTree->pRBTree->pRoot->pLeft == NULL 
            && pTree->pRBTree->pRoot->pRight == NULL
            && pTree->pRBTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRBTree->uNodeCount == 1
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("MTree_Insert() 测试用例1, 插入1个节点失败\n");
        }
        break;
    case 2: /* 插入两个节点的情况 */
        break;
    default:
        break;
    }
    MTree_Destroy(pTree, free);
    return 1;
}

INT DRV_MTree_Delete(INT i)
{
    MTREE  *pTree = NULL;
    HANDLE hTree = MTree_Create();
    pTree = (MTREE *)hTree;
    switch( i )
    {
    case 1:
        MTree_Insert(hTree, StrCopy("20"), StrCompare);
        MTree_Delete(hTree, "20", StrCompare, free);
        if ( pTree->pRBTree->pRoot == NULL
            && pTree->pRBTree->uNodeCount == 0
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("MTree_Delete() 测试用例1, 插入1个节点失败\n");
        }
        break;
    default:
        break;
    }
    MTree_Destroy(pTree, free);
    return 1;
}