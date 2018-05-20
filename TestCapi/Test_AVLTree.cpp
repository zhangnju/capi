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

void DRV_AVLTree_Delete(UINT i);
void DRV_AVLTree_Insert(UINT i);
void DRV_AVLTree_Destroy(UINT i);
void DRV_AVLTree_Find(UINT i);

void Test_AVLTree()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_AVLTree_Insert(i);
        DRV_AVLTree_Delete(i);
		DRV_AVLTree_Find(i);
		DRV_AVLTree_Destroy(i);
    }    
}

REGISTER_TESTFUNC(Test_AVLTree)


int AVLTree_CheckCount(AVLTREENODE *pStartNode)
{
    AVLTREENODE *pLeftNode, *pRightNode;
    UINT        uCount;
    if ( pStartNode == NULL )
    {
        return CAPI_FAILED;
    }

    uCount = AVLTree_GetSubTreeCount(pStartNode) + 1;

    assertEquals(uCount, pStartNode->uCount);
    if ( uCount != pStartNode->uCount )
    {
        printf("pStartNode->pData = %s\n", pStartNode->pData);
    }
    pLeftNode = pStartNode->pLeft;
    pRightNode = pStartNode->pRight;

    AVLTree_CheckCount(pLeftNode);
    AVLTree_CheckCount(pRightNode);
    return CAPI_SUCCESS;
}


void DRV_AVLTree_Create(UINT i)
{
    AVLTREE *pTree;
    switch( i )
    {
    case 1:
        pTree = AVLTree_Create();
        if ( pTree != NULL && pTree->pRoot == NULL 
            && pTree->uNodeCount == 0 )
        {
            
        }
        else
        {
            printf( "AVLTree_Create() ��������ʧ��!\n" );
        }
        AVLTree_Destroy(pTree, NULL);
        break;
    default:
        break;
    }
}


void DRV_AVLTree_Insert(UINT i)
{
    AVLTREE *pTree = AVLTree_Create();
    switch( i )
    {
    case 1:  /* ���Բ���1���ڵ����� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "Hello") == 0
            && pTree->pRoot->pLeft == NULL 
            && pTree->pRoot->pRight == NULL
            && pTree->pRoot->pParent == NULL 
            && pTree->pRoot->nMagic == 0 
            && pTree->uNodeCount == 1 )
        {
            //            printf( "Insert one node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������1 ���� ʧ��\n" );
        }
        break;
    case 2: /* ���Բ���2���ڵ�����, ��2���ڵ���ڵ�1���ڵ� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        AVLTree_Insert(pTree, strdup("zhouweiming"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "Hello") == 0
            && pTree->pRoot->pLeft == NULL 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "zhouweiming" ) == 0
            && pTree->pRoot->nMagic == -1 
            && pTree->pRoot->pRight->nMagic == 0 
            && pTree->pRoot->pRight->pLeft == NULL
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 2 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������2 ���� ʧ��\n" );
        }
        break;
    case 3: /* ���Բ���2���ڵ�����, ��2���ڵ�С�ڵ�1���ڵ� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        AVLTree_Insert(pTree, strdup("abcd"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "Hello") == 0
            && pTree->pRoot->pRight == NULL 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "abcd" ) == 0
            && pTree->pRoot->nMagic == 1 
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL
            && pTree->uNodeCount == 2 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������3 ���� ʧ��\n" );
        }
        break;
    case 4:/* ���Բ���3���ڵ�����, LL�� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        AVLTree_Insert(pTree, strdup("abcd"), StrCompare);
        AVLTree_Insert(pTree, strdup("aaaa"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "abcd") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "Hello") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "aaaa" ) == 0
            && pTree->pRoot->nMagic == 0 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft == NULL
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL 
            && pTree->uNodeCount == 3 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������4 ���� ʧ��\n" );
        }
        break;
    case 5:/* ���Բ���3���ڵ�����, LR�� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        AVLTree_Insert(pTree, strdup("abcd"), StrCompare);
        AVLTree_Insert(pTree, strdup("bbbb"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "bbbb") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "Hello") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "abcd" ) == 0
            && pTree->pRoot->nMagic == 0 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft == NULL
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL 
            && pTree->uNodeCount == 3 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������5 ���� ʧ��\n" );
        }
        break;    
    case 6:/* ���Բ���3���ڵ�����, RL�� */
        AVLTree_Insert(pTree, strdup("aa"), StrCompare);
        AVLTree_Insert(pTree, strdup("abcd"), StrCompare);
        AVLTree_Insert(pTree, strdup("aaaa"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "aaaa") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "abcd") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "aa" ) == 0
            && pTree->pRoot->nMagic == 0 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft == NULL
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL 
            && pTree->uNodeCount == 3 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������6 ���� ʧ��\n" );
        }
        break;
    case 7:/* ���Բ���3���ڵ�����, RR�� */
        AVLTree_Insert(pTree, strdup("aa"), StrCompare);
        AVLTree_Insert(pTree, strdup("abcd"), StrCompare);
        AVLTree_Insert(pTree, strdup("bbbb"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "abcd") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "bbbb") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "aa" ) == 0
            && pTree->pRoot->nMagic == 0 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft == NULL
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL 
            && pTree->uNodeCount == 3 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������7 ���� ʧ��\n" );
        }
        break;    
    case 8:/* ���Բ���9���ڵ�����, LL�� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("10"), StrCompare);
        AVLTree_Insert(pTree, strdup("1"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "24") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15" ) == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pData), "10") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pLeft->pData), "1") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "13") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0
            && pTree->pRoot->nMagic == 1 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft->pLeft == NULL
            && pTree->pRoot->pRight->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == 1 
            && pTree->pRoot->pLeft->pLeft->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight->pLeft == NULL 
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->uNodeCount == 9 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������8 ���� ʧ��\n" );
        }
        break;
    case 9:/* ���Բ���9���ڵ�����, A�ڵ�(13�ڵ�)ƽ������Ϊ1��������������� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("10"), StrCompare);
        AVLTree_Insert(pTree, strdup("14"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "24") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15" ) == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pData), "13") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pLeft->pData), "10") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "14") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0
            && pTree->pRoot->nMagic == 1 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft->pLeft == NULL
            && pTree->pRoot->pRight->pRight->pRight == NULL
            && pTree->pRoot->pRight->pParent == pTree->pRoot
            && pTree->pRoot->pLeft->nMagic == 1 
            && pTree->pRoot->pLeft->pLeft->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight->pLeft == NULL 
            && pTree->pRoot->pLeft->pParent == pTree->pRoot
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->uNodeCount == 9 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������9 ���� ʧ��\n" );
        }
        break;
    case 10:/* ���Բ���9���ڵ�����, LR�� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("10"), StrCompare);
        AVLTree_Insert(pTree, strdup("11"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "24") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15" ) == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pData), "11") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pLeft->pData), "10") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "13") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0
            && pTree->pRoot->nMagic == 1 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft->pLeft == NULL
            && pTree->pRoot->pRight->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == 1 
            && pTree->pRoot->pLeft->pLeft->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight->pLeft == NULL 
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->uNodeCount == 9 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������10 ���� ʧ��\n" );
        }
        break;
    case 11:/* ���Բ���9���ڵ�����, RL�� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("19"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "24") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15" ) == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pData), "13") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pRight->pData), "19") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pLeft->pData), "17") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pData), "18") == 0
            && pTree->pRoot->nMagic == 1 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft->pLeft == NULL
            && pTree->pRoot->pRight->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == -1 
            && pTree->pRoot->pLeft->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->uNodeCount == 9 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������11 ���� ʧ��\n" );
        }
        break;
    case 12:/* ���Բ���9���ڵ�����, RR�� */
        AVLTree_Insert(pTree, strdup("21"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("19"), StrCompare);
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "21") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "24") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15" ) == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pData), "13") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pRight->pData), "20") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pLeft->pData), "17") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pData), "19") == 0
            && pTree->pRoot->nMagic == 1 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft->pLeft == NULL
            && pTree->pRoot->pRight->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == -1 
            && pTree->pRoot->pLeft->pRight->nMagic == 0
            && pTree->pRoot->pLeft->pRight->pLeft->nMagic == 0
            && pTree->pRoot->pLeft->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->uNodeCount == 9 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������12 ���� ʧ��\n" );
        }
        break;
    case 13:/* ���Բ���11���ڵ�����, A�ڵ�ƽ������Ϊ-1, ����������������� */
        AVLTree_Insert(pTree, strdup("21"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("19"), StrCompare);
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("12"), StrCompare);
        AVLTree_Insert(pTree, strdup("14"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "21") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "24") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15" ) == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pData), "13") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pRight->pData), "20") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pLeft->pData), "17") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pRight->pData), "19") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pLeft->pData), "12") == 0
            && strcmp( (char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "14") == 0
            && pTree->pRoot->nMagic == 1 
            && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft->pLeft == NULL
            && pTree->pRoot->pRight->pRight->pRight == NULL
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pRight->nMagic == 0
            && pTree->pRoot->pLeft->pRight->pLeft->nMagic == 0
            && pTree->pRoot->pLeft->pLeft->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->uNodeCount == 11 )
        {
            //            printf( "Insert Two node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Insert()��������12 ���� ʧ��\n" );
        }
        break;
    default:
        break;
    }
    if ( pTree->pRoot != NULL )
    {
        if (pTree->pRoot->pParent != NULL )
        {
            printf( "Root Node's Parent Node is not NULL\n" );
        }
    }
    BinTree_CheckParent(pTree->pRoot);

    AVLTree_CheckCount(pTree->pRoot);

    AVLTree_Destroy(pTree, free);
}

void DRV_AVLTree_Delete(UINT i)
{
    AVLTREE *pTree = AVLTree_Create();
    switch( i )
    {
    case 1: /* ������Ϊ��ʱ��ɾ����� */
        AVLTree_Delete(pTree, "hELLO", StrCompare, free );
        break;
    case 2: /* ���Բ���һ���ڵ㣬ɾ��һ���ڵ����� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        AVLTree_Delete(pTree, "Hello", StrCompare, free);
        if ( pTree->pRoot == NULL
            && pTree->uNodeCount == 0 )
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������2 ɾ�� ʧ��\n" );
        }
        break;
    case 3: /* ���Բ��������ڵ㣬ɾ�����ڵ����� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        AVLTree_Insert(pTree, strdup("zhouweiming"), StrCompare);

        AVLTree_Delete(pTree, "Hello", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "zhouweiming") == 0 
            && pTree->pRoot->nMagic == 0 
            && pTree->pRoot->pLeft == NULL 
            && pTree->pRoot->pRight == NULL 
            && pTree->pRoot->pParent == NULL
            &&pTree->uNodeCount == 1 )
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������3 ɾ�� ʧ��\n" );
        }
        break;
    case 4: /* ���Բ��������ڵ㣬ɾ���Ǹ��ڵ����� */
        AVLTree_Insert(pTree, strdup("Hello"), StrCompare);
        AVLTree_Insert(pTree, strdup("zhouweiming"), StrCompare);

        AVLTree_Delete(pTree, "zhouweiming", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "Hello") == 0 
            && pTree->pRoot->nMagic == 0 
            && pTree->pRoot->pLeft == NULL 
            && pTree->pRoot->pRight == NULL 
            && pTree->pRoot->pParent == NULL 
            && pTree->uNodeCount == 1 )
        {
//            printf( "Delete \"Hello\" Node Successful\n" );
        }
        else
        {
            printf( "AVLTree_Delete()��������4 ɾ�� ʧ��\n" );
        }
        break;
    case 5: /* ����L0����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Insert(pTree, strdup("27"), StrCompare);
        AVLTree_Insert(pTree, strdup("22"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);

        AVLTree_Delete(pTree, "18", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "27") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "28") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "22") == 0 
            && pTree->pRoot->nMagic == 1 && pTree->pRoot->pLeft->nMagic == -1
            && pTree->pRoot->pRight->nMagic == 0 
            && pTree->pRoot->pLeft->pRight->nMagic == 0 
            && pTree->uNodeCount == 4 )
        {

        }
        else
        {
            printf( "AVLTree_Delete()��������5 L0�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 6: /* ����L-1����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Insert(pTree, strdup("27"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        
        AVLTree_Delete(pTree, "18", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "27") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "28") == 0 
            && pTree->pRoot->nMagic == 0 && pTree->pRoot->pLeft->nMagic == 0
            && pTree->pRoot->pRight->nMagic == 0 
            && pTree->uNodeCount == 3 )
        {
            
        }
        else
        {
            printf( "AVLTree_Delete()��������6 L-1�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 7: /* ����L1����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Insert(pTree, strdup("27"), StrCompare);
        AVLTree_Insert(pTree, strdup("25"), StrCompare);
        
        AVLTree_Delete(pTree, "18", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "25") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "27") == 0 
            && pTree->pRoot->nMagic == 0 && pTree->pRoot->pLeft->nMagic == 0
            && pTree->pRoot->pRight->nMagic == 0 
            && pTree->uNodeCount == 3 )
        {
            
        }
        else
        {
            printf( "AVLTree_Delete()��������7 L1�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 8: /* ����R0����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Insert(pTree, strdup("12"), StrCompare);
        
        AVLTree_Delete(pTree, "28", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "17") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "12") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "18") == 0 
            && pTree->pRoot->nMagic == -1 && pTree->pRoot->pLeft->nMagic == 0
            && pTree->pRoot->pRight->nMagic == 1 
            && pTree->uNodeCount == 4 )
        {
            
        }
        else
        {
            printf( "AVLTree_Delete()��������8 R0�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 9: /* ����R-1����� */
        break;
    case 10: /* ����R1����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("12"), StrCompare);
        
        AVLTree_Delete(pTree, "28", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "17") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "12") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0 
            && pTree->pRoot->nMagic == 0 && pTree->pRoot->pLeft->nMagic == 0
            && pTree->pRoot->pRight->nMagic == 0 
            && pTree->uNodeCount == 3 )
        {
            
        }
        else
        {
            printf( "AVLTree_Delete()��������10 R1�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 11: /* ����R-1����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        
        AVLTree_Delete(pTree, "28", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "18") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "17") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0 
            && pTree->pRoot->nMagic == 0 && pTree->pRoot->pLeft->nMagic == 0
            && pTree->pRoot->pRight->nMagic == 0 
            && pTree->uNodeCount == 3 )
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������11 R-1�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;    
    case 12: /* ����R-1�����, C�ڵ�ƽ������Ϊ1  */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("16"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("29"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("19"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        
        AVLTree_Delete(pTree, "28", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "19") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "16") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "15") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pRight->pData), "29") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0 
            && pTree->pRoot->nMagic == 0 && pTree->pRoot->pRight->nMagic == -1
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->pRoot->pRight->pRight->nMagic == 0
            && pTree->pRoot->pLeft->pRight->nMagic == 0
            && pTree->uNodeCount == 6 )
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������12 R-1�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;    
    case 13: /* ����R-1�����, C�ڵ�ƽ������Ϊ��1 */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("16"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("29"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("19"), StrCompare);
        
        AVLTree_Delete(pTree, "28", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "17") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "16") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "15") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pRight->pData), "29") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "19") == 0 
            && pTree->pRoot->nMagic == 0 && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pLeft->nMagic == 1 
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->pRoot->pRight->pRight->nMagic == 0
            && pTree->pRoot->pRight->pLeft->nMagic == 0
            && pTree->uNodeCount == 6 )
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������13 R1�Ͳ�ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 14: /* �������1 */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("14"), StrCompare);
        AVLTree_Insert(pTree, strdup("25"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Insert(pTree, strdup("22"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("12"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("19"), StrCompare);
        AVLTree_Insert(pTree, strdup("21"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("29"), StrCompare);
        AVLTree_Insert(pTree, strdup("16"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        
        AVLTree_Delete(pTree, "12", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "17") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "25") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "14") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "18") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pLeft->pData), "13") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "16") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pRight->pData), "19") == 0 
            && pTree->pRoot->nMagic == -1 && pTree->pRoot->pRight->nMagic == 1
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pLeft->pLeft->nMagic == 0
            && pTree->pRoot->pLeft->pRight->nMagic == -1
            && pTree->pRoot->pLeft->pLeft->pRight->nMagic == 0
            && pTree->pRoot->pLeft->pLeft->pLeft->nMagic == 0
            && pTree->pRoot->pLeft->pRight->pRight->nMagic == 0
            && pTree->uNodeCount == 14 )
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������14 ������1��ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 15: /* �������2��ɾ���ڵ�����ҽڵ㶼���ڵ���� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("14"), StrCompare);
        AVLTree_Insert(pTree, strdup("25"), StrCompare);
        AVLTree_Insert(pTree, strdup("13"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Insert(pTree, strdup("22"), StrCompare);
        AVLTree_Insert(pTree, strdup("28"), StrCompare);
        AVLTree_Insert(pTree, strdup("12"), StrCompare);
        AVLTree_Insert(pTree, strdup("17"), StrCompare);
        AVLTree_Insert(pTree, strdup("19"), StrCompare);
        AVLTree_Insert(pTree, strdup("21"), StrCompare);
        AVLTree_Insert(pTree, strdup("23"), StrCompare);
        AVLTree_Insert(pTree, strdup("29"), StrCompare);
        AVLTree_Insert(pTree, strdup("16"), StrCompare);
        AVLTree_Insert(pTree, strdup("24"), StrCompare);
        
        AVLTree_Delete(pTree, "25", StrCompare, free);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "14") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "24") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "22") == 0 
            && pTree->pRoot->nMagic == 1 && pTree->pRoot->pRight->nMagic == 0
            && pTree->pRoot->pLeft->nMagic == -1 
            && pTree->pRoot->pRight->pRight->nMagic == -1
            && pTree->uNodeCount == 14 )
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������15 ������2��ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 16: /* 2���ڵ��ƽ����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Delete(pTree, "18", StrCompare, free);

        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && pTree->pRoot->pLeft == NULL 
            && pTree->pRoot->pRight == NULL 
            && pTree->pRoot->nMagic == 0 ) 
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������16 2���ڵ��ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    case 17: /* 4���ڵ��ƽ����� */
        AVLTree_Insert(pTree, strdup("20"), StrCompare);
        AVLTree_Insert(pTree, strdup("18"), StrCompare);
        AVLTree_Insert(pTree, strdup("22"), StrCompare);
        AVLTree_Insert(pTree, strdup("15"), StrCompare);
        AVLTree_Delete(pTree, "15", StrCompare, free);
        
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "18") == 0 
            && strcmp((char *)(pTree->pRoot->pRight->pData), "22") == 0 
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL 
            && pTree->pRoot->pRight->pLeft == NULL 
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->pRoot->nMagic == 0 
            && pTree->pRoot->pLeft->nMagic == 0 
            && pTree->pRoot->pRight->nMagic == 0 ) 
        {
        }
        else
        {
            printf( "AVLTree_Delete()��������17 4���ڵ��ƽ����� ɾ�� ʧ��\n" );
        }
        break;
    default:
        break;
    }
    BinTree_CheckParent(pTree->pRoot);
    AVLTree_CheckCount(pTree->pRoot);
    if ( pTree->pRoot != NULL )
    {
        if (pTree->pRoot->pParent != NULL )
        {
            printf( "Root Node's Parent Node is not NULL\n" );
        }
    }
    AVLTree_Destroy(pTree, free);
}

void DRV_AVLTree_Find(UINT i)
{
	char *pszTemp;
    AVLTREE *pTree = AVLTree_Create();
    AVLTree_Insert(pTree, strdup("20"), StrCompare);
    AVLTree_Insert(pTree, strdup("16"), StrCompare);
    AVLTree_Insert(pTree, strdup("28"), StrCompare);
    AVLTree_Insert(pTree, strdup("29"), StrCompare);
    AVLTree_Insert(pTree, strdup("15"), StrCompare);
    AVLTree_Insert(pTree, strdup("17"), StrCompare);
    AVLTree_Insert(pTree, strdup("19"), StrCompare);

    switch( i )
    {
    case 1:
		pszTemp = (char *)AVLTree_Find(pTree, "20", StrCompare);
		if ( strcmp(pszTemp, "20") != 0 )
		{
			printf("AVLTree_Find()��������1 ʧ��\n");
		}
        break;
    case 2:
		pszTemp = (char *)AVLTree_Find(pTree, "28", StrCompare);
		if ( strcmp(pszTemp, "28") != 0 )
		{
			printf("AVLTree_Find()��������2 ʧ��\n");
		}
        break;
    case 3:
		pszTemp = (char *)AVLTree_Find(pTree, "16", StrCompare);
		if ( strcmp(pszTemp, "16") != 0 )
		{
			printf("AVLTree_Find()��������3 ʧ��\n");
		}
        break;
    case 4:
		pszTemp = (char *)AVLTree_Find(pTree, "29", StrCompare);
		if ( strcmp(pszTemp, "29") != 0 )
		{
			printf("AVLTree_Find()��������4 ʧ��\n");
		}
        break;
    case 5:
		pszTemp = (char *)AVLTree_Find(pTree, "15", StrCompare);
		if ( strcmp(pszTemp, "15") != 0 )
		{
			printf("AVLTree_Find()��������5 ʧ��\n");
		}
        break;    
	case 6:
		pszTemp = (char *)AVLTree_Find(pTree, "17", StrCompare);
		if ( strcmp(pszTemp, "17") != 0 )
		{
			printf("AVLTree_Find()��������6 ʧ��\n");
		}
        break;    
	case 7:
		pszTemp = (char *)AVLTree_Find(pTree, "19", StrCompare);
		if ( strcmp(pszTemp, "19") != 0 )
		{
			printf("AVLTree_Find()��������7 ʧ��\n");
		}
        break;    
	case 8:
		AVLTree_Delete(pTree,"17", StrCompare, free);
		pszTemp = (char *)AVLTree_Find(pTree, "17", StrCompare);
		if ( pszTemp != NULL )
		{
			printf("AVLTree_Find()��������8 ʧ��\n");
		}
        break;    		
	case 9:
		AVLTree_Delete(pTree,"17", StrCompare, free);
		AVLTree_Delete(pTree,"20", StrCompare, free);
        AVLTree_CheckCount(pTree->pRoot);
		AVLTree_Delete(pTree,"16", StrCompare, free);
		AVLTree_Delete(pTree,"15", StrCompare, free);
        AVLTree_CheckCount(pTree->pRoot);
		AVLTree_Delete(pTree,"29", StrCompare, free);

		pszTemp = (char *)AVLTree_Find(pTree, "28", StrCompare);
		if ( strcmp(pszTemp, "28") != 0 )
		{
			printf("AVLTree_Find()��������9 ʧ��\n");
		}
        break;    		
	default:
        break;
    }
    AVLTree_CheckCount(pTree->pRoot);
	AVLTree_Destroy(pTree, free);
}

void DRV_AVLTree_Destroy(UINT i)
{
	AVLTREE *pTree = AVLTree_Create();
    switch( i )
    {
    case 1:
		AVLTree_Destroy(pTree, free);
        break;
    case 2:
		AVLTree_Destroy(pTree, NULL);
        break;
    case 3:
		AVLTree_Insert(pTree, strdup("20"), StrCompare);
		AVLTree_Destroy(pTree, free);
        break;
    case 4:
		AVLTree_Insert(pTree, strdup("16"), StrCompare);
		AVLTree_Destroy(pTree, free);
        break;
    case 5:
		AVLTree_Insert(pTree, strdup("20"), StrCompare);
		AVLTree_Insert(pTree, strdup("16"), StrCompare);
		AVLTree_Destroy(pTree, free);
        break;
    case 6:
		AVLTree_Insert(pTree, strdup("20"), StrCompare);
		AVLTree_Insert(pTree, strdup("16"), StrCompare);
		AVLTree_Insert(pTree, strdup("18"), StrCompare);
		AVLTree_Destroy(pTree, free);
        break;
    default:
        AVLTree_Destroy(pTree, NULL);
        break;
    }
}