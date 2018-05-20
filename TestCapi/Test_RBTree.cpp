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
#include "RBTree.h"

void DRV_RBTree_Create(INT i);
void DRV_RBTree_Destroy(INT i);
void DRV_RBTree_Insert(INT i);
void DRV_RBTree_Delete(INT i);
void DRV_RBTree_Find(UINT i);

void Test_RBTree()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_RBTree_Create(i);
        DRV_RBTree_Destroy(i);
        DRV_RBTree_Insert(i);
        DRV_RBTree_Delete(i);
        DRV_RBTree_Find(i);
    }    
}

REGISTER_TESTFUNC(Test_RBTree)

void DRV_RBTree_Create(INT i)
{
    RBTREE  *pTree = NULL;
    switch( i )
    {
    case 1:
        pTree = RBTree_Create();
        if ( pTree == NULL
            || pTree->pCursor != NULL
            || pTree->pRoot != NULL 
            || pTree->uNodeCount != 0 )
        {
            printf("RBTree_Create() ��������1ʧ��\n");
        }
        break;
    default:
        break;
    }
    RBTree_Destroy(pTree, free);
	return;
}

void DRV_RBTree_Destroy(INT i)
{
    RBTREE  *pTree = RBTree_Create();
    switch( i )
    {
    case 1:
		RBTree_Destroy(pTree, free);
        break;
    case 2:
		RBTree_Destroy(pTree, NULL);
        break;
    case 3:
        RBTree_Insert(pTree, strdup("20"), StrCompare);
		RBTree_Destroy(pTree, free);
        break;
    case 4:
        RBTree_Insert(pTree, strdup("20"), StrCompare);
		RBTree_Destroy(pTree, free);
        break;
    case 5:
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("18"), StrCompare);
        RBTree_Insert(pTree, strdup("19"), StrCompare);
		RBTree_Destroy(pTree, free);
        break;
    default:
        RBTree_Destroy(pTree, NULL);
        break;
    }
	return;
}

void DRV_RBTree_Insert(INT i)
{
    RBTREE  *pTree = NULL;
    pTree = RBTree_Create();
    switch( i )
    {
    case 1: /* ����1���ڵ����� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && pTree->pRoot->pLeft == NULL 
            && pTree->pRoot->pRight == NULL
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->uNodeCount == 1
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������1, ����1���ڵ�ʧ��\n");
        }
        break;
    case 2: /* ���������ڵ����� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && pTree->pRoot->pRight == NULL
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->uNodeCount == 2
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������2, ����2���ڵ�ʧ��\n");
        }
        break;
    case 3: /* ����3���ڵ����� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "22") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 3
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������3, ����3���ڵ�ʧ��\n");
        }
        break;
    case 4: /* LLr�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        RBTree_Insert(pTree, strdup("10"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "22") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "10") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 4
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������4, LLr��ʧ��\n");
        }
        break;
    case 5: /* LLb�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("10"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "15") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "10") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 3
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������5, LLb��ʧ��\n");
        }
        break;
    case 6: /* LRb�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("17"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "17") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 3
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������6, LRb��ʧ��\n");
        }
        break;
    case 7: /* LRr�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        RBTree_Insert(pTree, strdup("17"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "22") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 4
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������7, LRr��ʧ��\n");
        }
        break;
    case 8: /* LRr�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        RBTree_Insert(pTree, strdup("17"), StrCompare);
        RBTree_Insert(pTree, strdup("13"), StrCompare);
        RBTree_Insert(pTree, strdup("14"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "22") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "13") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "14") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->uNodeCount == 6
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������8, LRr��ʧ��\n");
        }
        break;
    case 9: /* LRr�ͺ�LLb�͸�����*/
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        RBTree_Insert(pTree, strdup("17"), StrCompare);
        RBTree_Insert(pTree, strdup("13"), StrCompare);
        RBTree_Insert(pTree, strdup("14"), StrCompare);
        RBTree_Insert(pTree, strdup("10"), StrCompare);
        RBTree_Insert(pTree, strdup("12"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "15") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "13") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "14") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "10") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "12") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "17") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pRight->pData), "22") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pLeft->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->uNodeCount == 8
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������9, LRr�ͺ�LLb�͸�����ʧ��\n");
        }
        break;
    case 10: /* LLr�ͺ�LRb�͸�����*/
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        RBTree_Insert(pTree, strdup("13"), StrCompare);
        RBTree_Insert(pTree, strdup("18"), StrCompare);
        RBTree_Insert(pTree, strdup("11"), StrCompare);
        RBTree_Insert(pTree, strdup("17"), StrCompare);
        RBTree_Insert(pTree, strdup("19"), StrCompare);
        RBTree_Insert(pTree, strdup("16"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "18") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "13") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pLeft->pData), "11") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pLeft->pData), "16") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "19") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pRight->pData), "22") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pLeft->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pRight->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->uNodeCount == 9
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������10, LLr�ͺ�LRb�͸�����ʧ��\n");
        }
        break;
    case 11: /* LRb�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        RBTree_Insert(pTree, strdup("13"), StrCompare);
        RBTree_Insert(pTree, strdup("18"), StrCompare);
        RBTree_Insert(pTree, strdup("11"), StrCompare);
        RBTree_Insert(pTree, strdup("17"), StrCompare);
        RBTree_Insert(pTree, strdup("19"), StrCompare);
        RBTree_Insert(pTree, strdup("16"), StrCompare);
        RBTree_Insert(pTree, strdup("12"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "18") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "17") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "12") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pLeft->pData), "11") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pLeft->pData), "16") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "19") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pRight->pData), "22") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pRight->pData), "13") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pLeft->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pRight->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pLeft->pLeft->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->uNodeCount == 10
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������11, LRb��ʧ��\n");
        }
        break;
    case 12: /* RLr�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("25"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "25") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "22") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 4
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������12, RLr��ʧ��\n");
        }
        break;
    case 13: /* RLb�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("25"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "22") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "25") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pLeft == NULL
            && pTree->pRoot->pRight->pRight == NULL
            && pTree->uNodeCount == 3
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������13, RLb��ʧ��\n");
        }
        break;
    case 14: /* RRb�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("25"), StrCompare);
        RBTree_Insert(pTree, strdup("27"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "25") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "27") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL
            && pTree->uNodeCount == 3
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������14, RRb��ʧ��\n");
        }
        break;
    case 15: /* RRr�� */
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("25"), StrCompare);
        RBTree_Insert(pTree, strdup("27"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "20") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "25") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pRight->pData), "27") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pLeft->pLeft == NULL
            && pTree->pRoot->pLeft->pRight == NULL
            && pTree->uNodeCount == 4
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������15, RRr��ʧ��\n");
        }
        break;
    case 16: /* RRr�ͺ�RLb�͸�����*/
        RBTree_Insert(pTree, strdup("20"), StrCompare);
        RBTree_Insert(pTree, strdup("15"), StrCompare);
        RBTree_Insert(pTree, strdup("25"), StrCompare);
        RBTree_Insert(pTree, strdup("27"), StrCompare);
        RBTree_Insert(pTree, strdup("22"), StrCompare);
        RBTree_Insert(pTree, strdup("21"), StrCompare);
        RBTree_Insert(pTree, strdup("24"), StrCompare);
        RBTree_Insert(pTree, strdup("23"), StrCompare);
        if ( strcmp((char *)(pTree->pRoot->pData), "22") == 0 
            && strcmp((char *)(pTree->pRoot->pLeft->pData), "20") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pData), "25") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pRight->pData), "21") == 0
            && strcmp((char *)(pTree->pRoot->pLeft->pLeft->pData), "15") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pLeft->pData), "23") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pLeft->pData), "24") == 0
            && strcmp((char *)(pTree->pRoot->pRight->pRight->pData), "27") == 0
            && pTree->pRoot->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pLeft->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pLeft->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->pLeft->pLeft->nMagic == RBTREE_COLOR_RED
            && pTree->pRoot->pRight->pLeft->nMagic == RBTREE_COLOR_BLACK
            && pTree->pRoot->pRight->pRight->nMagic == RBTREE_COLOR_BLACK
            && pTree->uNodeCount == 8
            && pTree->pCursor == NULL )
        {
        }
        else
        {
            printf("RBTree_Insert() ��������16, RRr�ͺ�RLb�͸�����ʧ��\n");
        }
        break;
    default:
        break;
    }
    BinTree_CheckParent(pTree->pRoot);
    if ( pTree->pRoot != NULL )
    {
        if (pTree->pRoot->pParent != NULL )
        {
            printf( "Root Node's Parent Node is not NULL\n" );
        }
    }
    RBTree_Destroy(pTree, free);
	return;
}


RBTREE * BuildDeleteTestCase()
{
    RBTREE  *pTree;
    pTree = RBTree_Create();
    if ( pTree != NULL )
    {
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("70"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("60"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("90"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("35"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("45"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("32"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("38"), StrCompare, RBTREE_COLOR_RED);
        pTree->uNodeCount = 11;
        pTree->pCursor = NULL;
    }
    
    return pTree;
}


RBTREE * BuildDeleteTestCase4()
{
    RBTREE  *pTree;
    pTree = RBTree_Create();
    if ( pTree != NULL )
    {
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("35"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("45"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("32"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("38"), StrCompare, RBTREE_COLOR_RED);
        pTree->uNodeCount = 7;
        pTree->pCursor = NULL;
    }
    
    return pTree;
}

RBTREE * BuildDeleteTestCase5()
{
    RBTREE  *pTree;
    pTree = RBTree_Create();
    if ( pTree != NULL )
    {
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("22"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("28"), StrCompare, RBTREE_COLOR_RED);
        pTree->uNodeCount = 7;
        pTree->pCursor = NULL;
    }
    
    return pTree;
}

RBTREE * BuildDeleteTestCase2()
{
    RBTREE  *pTree;
    pTree = RBTree_Create();
    if ( pTree != NULL )
    {
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("70"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("60"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("90"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("15"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("22"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("28"), StrCompare, RBTREE_COLOR_RED);
        pTree->uNodeCount = 11;
        pTree->pCursor = NULL;
    }
    
    return pTree;
}

RBTREE * BuildDeleteTestCase3()
{
    RBTREE  *pTree;
    pTree = RBTree_Create();
    if ( pTree != NULL )
    {
        BinTree_Insert(&(pTree->pRoot), strdup("50"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("30"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("70"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("20"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("40"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("60"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("90"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("10"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("25"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("35"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("45"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("55"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("65"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("80"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("95"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("32"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("38"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("42"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("48"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("62"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("85"), StrCompare, RBTREE_COLOR_RED);
        BinTree_Insert(&(pTree->pRoot), strdup("31"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("33"), StrCompare, RBTREE_COLOR_BLACK);
        BinTree_Insert(&(pTree->pRoot), strdup("36"), StrCompare, RBTREE_COLOR_RED);
        pTree->uNodeCount = 24;
        pTree->pCursor = NULL;
    }
    
    return pTree;
}



void DRV_RBTree_Delete(INT i)
{
    INT     nRet;
    BINTREEBASENODE *pNode;
    RBTREE  *pTree1;
    RBTREE  *pTree2;
    if ( i < 6 )
    {
        pTree1 = BuildDeleteTestCase();
        pTree2 = BuildDeleteTestCase();
    }
    else if ( i < 20 )
    {
        pTree1 = BuildDeleteTestCase2();
        pTree2 = BuildDeleteTestCase2();
    }
    else if ( i < 26 )
    {
        pTree1 = BuildDeleteTestCase3();
        pTree2 = BuildDeleteTestCase3();
    }
    else if ( i < 38 )
    {
        pTree1 = RBTree_Create();
        pTree2 = RBTree_Create();
    }
    else if ( i < 46 )
    {
        pTree1 = BuildDeleteTestCase4();
        pTree2 = BuildDeleteTestCase4();
    }
    else
    {
        pTree1 = BuildDeleteTestCase5();
        pTree2 = BuildDeleteTestCase5();
    }
    switch( i )
    {
    case 1:

        RBTree_Delete(pTree1, "32", StrCompare, free);

        ChangeNode(pTree2, "32", NULL, -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 10
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������1��ɾ����ɫ�ڵ�ʧ��\n");
        }
        break;
    case 2: /* Lr-br�� */
        
        RBTree_Delete(pTree1, "32", StrCompare, free);
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "38", NULL, -1);
        ChangeNode(pTree2, "35", "38", -1);
        ChangeNode(pTree2, "30", "35", -1);
        ChangeNode(pTree2, "20", "30", -1);


        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������2��Lr-br��ʧ��\n");
        }
        break;
    case 3: /* Lr-rb�� */
        
        RBTree_Delete(pTree1, "38", StrCompare, free);
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "38", NULL, -1);
        ChangeNode(pTree2, "30", "32", -1);
        ChangeNode(pTree2, "20", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������3��Lr-rb��ʧ��\n");
        }
        break;
    case 4: /* Lr-rr�� */
        
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "30", "32", -1);
        ChangeNode(pTree2, "20", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 10
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������4��Lr-rr��ʧ��\n");
        }
        break;
    case 5: /* Lr-bb�� */
        
        RBTree_Delete(pTree1, "32", StrCompare, free);
        RBTree_Delete(pTree1, "38", StrCompare, free);
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "38", NULL, -1);
        ChangeNode(pTree2, "35", NULL, -1);
        ChangeNode(pTree2, "45", NULL, -1);

        ChangeNode(pTree2, "40", "45", RBTREE_COLOR_BLACK);
        ChangeNode(pTree2, "30", "40", -1);
        ChangeNode(pTree2, "20", "30", -1);

        BinTree_Insert(&(pTree2->pRoot), strdup("35"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 8
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������5��Lr-bb��ʧ��\n");
        }
        break;
    case 6: /* Rr-rr�� */
        
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "30", "28", -1);
        ChangeNode(pTree2, "40", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 10
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������6��Rr-rr��ʧ��\n");
        }
        break;
    case 7: /* Rr-rb�� */

        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "25", "22", -1);
        ChangeNode(pTree2, "30", "25", -1);
        ChangeNode(pTree2, "40", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������6��Rr-rb��ʧ��\n");
        }
        break;
    case 8: /* Rr-br�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "30", "28", -1);
        ChangeNode(pTree2, "40", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������8��Rr-br��ʧ��\n");
        }
        break;
    case 9: /* Rr-bb�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "25", NULL, -1);
        ChangeNode(pTree2, "15", NULL, -1);
        ChangeNode(pTree2, "20", "15", RBTREE_COLOR_BLACK);
        ChangeNode(pTree2, "30", "20", -1);
        ChangeNode(pTree2, "40", "30", -1);
        BinTree_Insert(&(pTree2->pRoot), strdup("25"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 8
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������9��Rr-bb��ʧ��\n");
        }
        break;
    case 10: /* Lb-rr�� */
        
        RBTree_Delete(pTree1, "15", StrCompare, free);
        
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "20", "22", -1);
        ChangeNode(pTree2, "15", "20", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 10
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������10��Lb-rr��ʧ��\n");
        }
        break;
    case 11: /* Lb-rb�� */
        
        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "15", StrCompare, free);
        
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "20", "22", -1);
        ChangeNode(pTree2, "15", "20", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������11��Lb-rb��ʧ��\n");
        }
        break;
    case 12: /* Lb-br�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "15", StrCompare, free);
        
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "25", "28", -1);
        ChangeNode(pTree2, "20", "25", -1);
        ChangeNode(pTree2, "15", "20", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������12��Lb-br��ʧ��\n");
        }
        break;
    case 13: /* Lb-bb�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "15", StrCompare, free);
        
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "25", "25", RBTREE_COLOR_RED);
        ChangeNode(pTree2, "20", "20", RBTREE_COLOR_BLACK);
        ChangeNode(pTree2, "15", NULL, -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 8
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������13��Lb-bb��ʧ��\n");
        }
        break;
    case 14: /* Lb-bb�ͺ�Rb-bb������ */
        
        RBTree_Delete(pTree1, "60", StrCompare, free);

        BinTree_RotateRight(pTree2->pRoot, &(pTree2->pRoot));
        ChangeNode(pTree2, "60", NULL, -1);
        ChangeNode(pTree2, "20", "20",RBTREE_COLOR_BLACK);
        ChangeNode(pTree2, "90", "90", RBTREE_COLOR_RED);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 10
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������14��Lb-bb�ͺ�Rb-bb������ʧ��\n");
        }
        break;
    case 15: /* Rb-bb�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "25", StrCompare, free);
        
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "25", NULL, -1);
        ChangeNode(pTree2, "15", "15",RBTREE_COLOR_RED);
        ChangeNode(pTree2, "20", "20", RBTREE_COLOR_BLACK);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 8
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������15��Rb-bb��ʧ��\n");
        }
        break;
    case 16: /* Rb-rr�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        BinTree_Insert(&(pTree1->pRoot), strdup("55"), StrCompare, RBTREE_COLOR_RED);
        pTree1->uNodeCount += 1;
        BinTree_Insert(&(pTree1->pRoot), strdup("65"), StrCompare, RBTREE_COLOR_RED);
        pTree1->uNodeCount += 1;
        RBTree_Delete(pTree1, "90", StrCompare, free);
        
        BinTree_Insert(&(pTree2->pRoot), strdup("55"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        BinTree_Insert(&(pTree2->pRoot), strdup("65"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);

        ChangeNode(pTree2, "65", NULL, -1);
        ChangeNode(pTree2, "90", "70", -1);
        ChangeNode(pTree2, "70", "65", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 10
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������16��Rb-rr��ʧ��\n");
        }
        break;
    case 17: /* Rb-br�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        BinTree_Insert(&(pTree1->pRoot), strdup("65"), StrCompare, RBTREE_COLOR_RED);
        pTree1->uNodeCount += 1;
        RBTree_Delete(pTree1, "90", StrCompare, free);
        
        BinTree_Insert(&(pTree2->pRoot), strdup("65"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);
        
        ChangeNode(pTree2, "65", NULL, -1);
        ChangeNode(pTree2, "70", "65", -1);
        ChangeNode(pTree2, "90", "70", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������17��Rb-br��ʧ��\n");
        }
        break;
    case 18: /* Rb-rb�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        BinTree_Insert(&(pTree1->pRoot), strdup("55"), StrCompare, RBTREE_COLOR_RED);
        pTree1->uNodeCount += 1;
        RBTree_Delete(pTree1, "90", StrCompare, free);
        
        BinTree_Insert(&(pTree2->pRoot), strdup("55"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);
        
        ChangeNode(pTree2, "55", NULL, -1);
        ChangeNode(pTree2, "60", "55", -1);
        ChangeNode(pTree2, "70", "60", -1);
        ChangeNode(pTree2, "90", "70", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 9
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������18��Rb-rb��ʧ��\n");
        }
        break;
    case 19: /* Rb-bb��Rb-rb������ */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "90", StrCompare, free);
        
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "28", NULL, -1);
        
        ChangeNode(pTree2, "15", NULL, -1);
        ChangeNode(pTree2, "25", NULL, -1);

        ChangeNode(pTree2, "20", "15", RBTREE_COLOR_BLACK);
        ChangeNode(pTree2, "40", "25", -1);
        ChangeNode(pTree2, "30", "20", -1);
        ChangeNode(pTree2, "50", "30", -1);
        ChangeNode(pTree2, "60", "40", -1); /* �����С�ĵ��󣬴��б������70ǰ�棬�����ȸ�70�Ļ�����Ҳ���60 */
        ChangeNode(pTree2, "70", "50", -1);
        ChangeNode(pTree2, "90", "70", -1);
        BinTree_Insert(&(pTree2->pRoot), strdup("60"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 8
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������19��Rb-bb��Rb-rb������ʧ��\n");
        }
        break;
    case 20: /* Rb-bb�ͺ�Lr��rb������ */
        
        RBTree_Delete(pTree1, "30", StrCompare, free);
        
        ChangeNode(pTree2, "25", NULL, -1);
        ChangeNode(pTree2, "10", "10", RBTREE_COLOR_RED);
        
        ChangeNode(pTree2, "30", "25", -1);
        ChangeNode(pTree2, "32", "32", RBTREE_COLOR_BLACK);

        pNode = BinTree_FindNode(pTree2->pRoot, "35", StrCompare);
        BinTree_RotateRight(pNode, &(pTree2->pRoot));

        pNode = BinTree_FindNode(pTree2->pRoot, "40", StrCompare);
        BinTree_RotateRight(pNode, &(pTree2->pRoot));

        pNode = BinTree_FindNode(pTree2->pRoot, "25", StrCompare);
        BinTree_RotateLeft(pNode, &(pTree2->pRoot));

        
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 23
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������20��Rb-bb��Lr-rb������ʧ��\n");
        }
        break;        
    case 21: /* A�ڵ�Ϊ��ɫ����� */
        
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "36", NULL, -1);
        
        ChangeNode(pTree2, "38", "36", RBTREE_COLOR_RED);
        ChangeNode(pTree2, "40", "38", -1);

        
        ChangeNode(pTree2, "36", "36", RBTREE_COLOR_BLACK);
        
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 23
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������21��A�ڵ�Ϊ��ɫ��ʧ��\n");
        }
        break;        
    case 22: /* A�ڵ�Ϊ��ɫ����� */
        
        RBTree_Delete(pTree1, "70", StrCompare, free);
        
        ChangeNode(pTree2, "62", NULL, -1);
        
        ChangeNode(pTree2, "65", "62", RBTREE_COLOR_RED);
        ChangeNode(pTree2, "70", "65", -1);
        
        
        ChangeNode(pTree2, "62", "62", RBTREE_COLOR_BLACK);
        
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 23
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������22��A�ڵ�Ϊ��ɫ��ʧ��\n");
        }
        break;        
    case 23: /* Rb-bb�� */
        
        RBTree_Delete(pTree1, "70", StrCompare, free);
        
        ChangeNode(pTree2, "62", NULL, -1);
        
        ChangeNode(pTree2, "65", "62", RBTREE_COLOR_RED);
        ChangeNode(pTree2, "70", "65", -1);
        
        
        ChangeNode(pTree2, "62", "62", RBTREE_COLOR_BLACK);
        
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 23
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������23��A�ڵ�Ϊ��ɫ��ʧ��\n");
        }
        break;        
    case 24: /* ɾ���ڵ���ҽڵ㲻������ */
        
        RBTree_Delete(pTree1, "65", StrCompare, free);
        
        ChangeNode(pTree2, "62", NULL, -1);
        
        ChangeNode(pTree2, "65", "62", RBTREE_COLOR_RED);
       
        ChangeNode(pTree2, "62", "62", RBTREE_COLOR_BLACK);
        
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 23
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������24��ɾ���ڵ���ҽڵ㲻������ʧ��\n");
        }
        break;        
    case 25: /* ɾ���ڵ����ڵ㲻������ */
        
        RBTree_Delete(pTree1, "80", StrCompare, free);
        
        ChangeNode(pTree2, "85", NULL, -1);
        
        ChangeNode(pTree2, "80", "85", RBTREE_COLOR_RED);
        
        ChangeNode(pTree2, "85", "85", RBTREE_COLOR_BLACK);
        
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 23
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������25��ɾ���ڵ����ڵ㲻������ʧ��\n");
        }
        break;      
    case 26:
        nRet = RBTree_Delete(pTree1, "20", StrCompare, free);
        if ( nRet == CAPI_SUCCESS )
        {
            printf("��������26������ɾ����ʧ��\n");

        }
        break;
    case 27:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        nRet = RBTree_Delete(pTree1, "20", StrCompare, free);
        if ( nRet == CAPI_FAILED || pTree1->pRoot != NULL 
            || pTree1->uNodeCount != 0 )
        {
            printf("��������27��1���ڵ�ɾ����ʧ��\n");
            
        }
        break;
    case 28:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        nRet = RBTree_Delete(pTree1, "10", StrCompare, free);
        if ( nRet == CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "20" )  != 0
            || pTree1->pRoot->pLeft != NULL
            || pTree1->pRoot->pRight != NULL 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK
            || pTree1->uNodeCount != 1 )
        {
            printf("��������28��1���ڵ�ɾ����ʧ��\n");
            
        }
        break;
    case 29:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("15"), StrCompare);
        nRet = RBTree_Delete(pTree1, "15", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "20" )  != 0
            || pTree1->pRoot->pLeft != NULL
            || pTree1->pRoot->pRight != NULL 
            || pTree1->uNodeCount != 1 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK )
        {
            printf("��������29��2���ڵ�ɾ����ʧ��\n");
            
        }
        break;                
        
    case 30:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("15"), StrCompare);
        nRet = RBTree_Delete(pTree1, "20", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "15" )  != 0
            || pTree1->pRoot->pLeft != NULL
            || pTree1->pRoot->pRight != NULL 
            || pTree1->uNodeCount != 1 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK )
        {
            printf("��������30��2���ڵ�ɾ����ʧ��\n");
            
        }
        break;                
    case 31:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("35"), StrCompare);
        nRet = RBTree_Delete(pTree1, "35", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "20" )  != 0
            || pTree1->pRoot->pLeft != NULL
            || pTree1->pRoot->pRight != NULL 
            || pTree1->uNodeCount != 1 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK )
        {
            printf("��������31��2���ڵ�ɾ����ʧ��\n");
            
        }
        break;                
    case 32:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("35"), StrCompare);
        nRet = RBTree_Delete(pTree1, "20", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "35" )  != 0
            || pTree1->pRoot->pLeft != NULL
            || pTree1->pRoot->pRight != NULL 
            || pTree1->uNodeCount != 1 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK )
        {
            printf("��������32��2���ڵ�ɾ����ʧ��\n");
            
        }
        break;    
        
    case 33:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("35"), StrCompare);
        RBTree_Insert(pTree1, strdup("15"), StrCompare);
        nRet = RBTree_Delete(pTree1, "20", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "15" )  != 0
            || strcmp((char *)(pTree1->pRoot->pRight->pData), "35" )  != 0
            || pTree1->pRoot->pLeft != NULL
            || pTree1->pRoot->pRight->pLeft != NULL 
            || pTree1->uNodeCount != 2 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK
            || pTree1->pRoot->pRight->nMagic != RBTREE_COLOR_RED )
        {
            printf("��������33��3���ڵ�ɾ����ʧ��\n");
            
        }
        break;    
        
    case 34:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("35"), StrCompare);
        RBTree_Insert(pTree1, strdup("15"), StrCompare);
        nRet = RBTree_Delete(pTree1, "15", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "20" )  != 0
            || strcmp((char *)(pTree1->pRoot->pRight->pData), "35" )  != 0
            || pTree1->pRoot->pLeft != NULL
            || pTree1->pRoot->pRight->pLeft != NULL 
            || pTree1->uNodeCount != 2 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK
            || pTree1->pRoot->pRight->nMagic != RBTREE_COLOR_RED )
        {
            printf("��������34��3���ڵ�ɾ����ʧ��\n");
            
        }
        break;    
    case 35:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("35"), StrCompare);
        RBTree_Insert(pTree1, strdup("15"), StrCompare);
        nRet = RBTree_Delete(pTree1, "35", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "20" )  != 0
            || strcmp((char *)(pTree1->pRoot->pLeft->pData), "15" )  != 0
            || pTree1->pRoot->pRight != NULL
            || pTree1->pRoot->pLeft->pRight != NULL 
            || pTree1->uNodeCount != 2 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK
            || pTree1->pRoot->pLeft->nMagic != RBTREE_COLOR_RED )
        {
            printf("��������35��3���ڵ�ɾ����ʧ��\n");
            
        }
        break;    
    case 36:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("35"), StrCompare);
        RBTree_Insert(pTree1, strdup("15"), StrCompare);
        RBTree_Insert(pTree1, strdup("12"), StrCompare);
        nRet = RBTree_Delete(pTree1, "12", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "20" )  != 0
            || strcmp((char *)(pTree1->pRoot->pLeft->pData), "15" )  != 0
            || strcmp((char *)(pTree1->pRoot->pRight->pData), "35" )  != 0
            || pTree1->pRoot->pRight->pLeft != NULL
            || pTree1->pRoot->pLeft->pLeft != NULL 
            || pTree1->uNodeCount != 3 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK
            || pTree1->pRoot->pLeft->nMagic != RBTREE_COLOR_BLACK 
            || pTree1->pRoot->pRight->nMagic != RBTREE_COLOR_BLACK )
        {
            printf("��������36��4���ڵ�ɾ����ʧ��\n");
            
        }
        break;            
    case 37:
        RBTree_Insert(pTree1, strdup("20"), StrCompare);
        RBTree_Insert(pTree1, strdup("35"), StrCompare);
        RBTree_Insert(pTree1, strdup("15"), StrCompare);
        RBTree_Insert(pTree1, strdup("12"), StrCompare);
        nRet = RBTree_Delete(pTree1, "15", StrCompare, free);
        if ( nRet != CAPI_SUCCESS 
            || strcmp((char *)(pTree1->pRoot->pData), "20" )  != 0
            || strcmp((char *)(pTree1->pRoot->pLeft->pData), "12" )  != 0
            || strcmp((char *)(pTree1->pRoot->pRight->pData), "35" )  != 0
            || pTree1->pRoot->pRight->pLeft != NULL
            || pTree1->pRoot->pLeft->pLeft != NULL 
            || pTree1->uNodeCount != 3 
            || pTree1->pRoot->nMagic != RBTREE_COLOR_BLACK
            || pTree1->pRoot->pLeft->nMagic != RBTREE_COLOR_BLACK 
            || pTree1->pRoot->pRight->nMagic != RBTREE_COLOR_BLACK )
        {
            printf("��������37��4���ڵ�ɾ����ʧ��\n");
            
        }
        break;  
    case 38:
        break;
    case 39:
        break;
    case 42: /* Lr-br�� */
        
        RBTree_Delete(pTree1, "32", StrCompare, free);
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "38", NULL, -1);
        ChangeNode(pTree2, "35", "38", -1);
        ChangeNode(pTree2, "30", "35", -1);
        ChangeNode(pTree2, "20", "30", -1);
        
        
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 5
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������42��Lr-br��ʧ��\n");
        }
        break;
    case 43: /* Lr-rb�� */
        
        RBTree_Delete(pTree1, "38", StrCompare, free);
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "38", NULL, -1);
        ChangeNode(pTree2, "30", "32", -1);
        ChangeNode(pTree2, "20", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 5
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������43��Lr-rb��ʧ��\n");
        }
        break;
    case 44: /* Lr-rr�� */
        
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "30", "32", -1);
        ChangeNode(pTree2, "20", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 6
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������44��Lr-rr��ʧ��\n");
        }
        break;
    case 45: /* Lr-bb�� */
        
        RBTree_Delete(pTree1, "32", StrCompare, free);
        RBTree_Delete(pTree1, "38", StrCompare, free);
        RBTree_Delete(pTree1, "20", StrCompare, free);
        
        ChangeNode(pTree2, "32", NULL, -1);
        ChangeNode(pTree2, "38", NULL, -1);
        ChangeNode(pTree2, "35", NULL, -1);
        ChangeNode(pTree2, "45", NULL, -1);
        
        ChangeNode(pTree2, "40", "45", RBTREE_COLOR_BLACK);
        ChangeNode(pTree2, "30", "40", -1);
        ChangeNode(pTree2, "20", "30", -1);
        
        BinTree_Insert(&(pTree2->pRoot), strdup("35"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 4
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������45��Lr-bb��ʧ��\n");
        }
        break;
    case 46: /* Rr-rr�� */
        
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "30", "28", -1);
        ChangeNode(pTree2, "40", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 6
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������46��Rr-rr��ʧ��\n");
        }
        break;
    case 47: /* Rr-rb�� */
        
        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "25", "22", -1);
        ChangeNode(pTree2, "30", "25", -1);
        ChangeNode(pTree2, "40", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 5
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������47��Rr-rb��ʧ��\n");
        }
        break;
    case 48: /* Rr-br�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "30", "28", -1);
        ChangeNode(pTree2, "40", "30", -1);
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 5
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������48��Rr-br��ʧ��\n");
        }
        break;
    case 49: /* Rr-bb�� */
        
        RBTree_Delete(pTree1, "22", StrCompare, free);
        RBTree_Delete(pTree1, "28", StrCompare, free);
        RBTree_Delete(pTree1, "40", StrCompare, free);
        
        ChangeNode(pTree2, "28", NULL, -1);
        ChangeNode(pTree2, "22", NULL, -1);
        ChangeNode(pTree2, "25", NULL, -1);
        ChangeNode(pTree2, "15", NULL, -1);
        ChangeNode(pTree2, "20", "15", RBTREE_COLOR_BLACK);
        ChangeNode(pTree2, "30", "20", -1);
        ChangeNode(pTree2, "40", "30", -1);
        BinTree_Insert(&(pTree2->pRoot), strdup("25"), StrCompare, RBTREE_COLOR_RED);
        pTree2->uNodeCount += 1;
        if ( CompareRBTree(pTree1->pRoot, pTree2->pRoot) == 0 
            || pTree1->uNodeCount != 4
            || pTree1->uNodeCount != pTree2->uNodeCount )
        {
            printf("��������49��Rr-bb��ʧ��\n");
        }
        break;
    default:
        break;
    }
    BinTree_CheckParent(pTree1->pRoot);
    if ( pTree1->pRoot != NULL )
    {
        if (pTree1->pRoot->pParent != NULL )
        {
            printf( "RBTree_Delete Failed.\n Root Node's Parent Node is not NULL\n" );
        }
    }
    RBTree_Destroy(pTree1, free);
    RBTree_Destroy(pTree2, free);
	return;
}


void DRV_RBTree_Find(UINT i)
{
	char *pszTemp;
    RBTREE *pTree = RBTree_Create();
    RBTree_Insert(pTree, strdup("20"), StrCompare);
    RBTree_Insert(pTree, strdup("16"), StrCompare);
    RBTree_Insert(pTree, strdup("28"), StrCompare);
    RBTree_Insert(pTree, strdup("29"), StrCompare);
    RBTree_Insert(pTree, strdup("15"), StrCompare);
    RBTree_Insert(pTree, strdup("17"), StrCompare);
    RBTree_Insert(pTree, strdup("19"), StrCompare);
    switch( i )
    {
    case 1:
		pszTemp = (char *)RBTree_Find(pTree, "20", StrCompare);
		if ( strcmp(pszTemp, "20") != 0 )
		{
			printf("RBTree_Find()��������1 ʧ��\n");
		}
        break;
    case 2:
		pszTemp = (char *)RBTree_Find(pTree, "28", StrCompare);
		if ( strcmp(pszTemp, "28") != 0 )
		{
			printf("RBTree_Find()��������2 ʧ��\n");
		}
        break;
    case 3:
		pszTemp = (char *)RBTree_Find(pTree, "16", StrCompare);
		if ( strcmp(pszTemp, "16") != 0 )
		{
			printf("RBTree_Find()��������3 ʧ��\n");
		}
        break;
    case 4:
		pszTemp = (char *)RBTree_Find(pTree, "29", StrCompare);
		if ( strcmp(pszTemp, "29") != 0 )
		{
			printf("RBTree_Find()��������4 ʧ��\n");
		}
        break;
    case 5:
		pszTemp = (char *)RBTree_Find(pTree, "15", StrCompare);
		if ( strcmp(pszTemp, "15") != 0 )
		{
			printf("RBTree_Find()��������5 ʧ��\n");
		}
        break;    
	case 6:
		pszTemp = (char *)RBTree_Find(pTree, "17", StrCompare);
		if ( strcmp(pszTemp, "17") != 0 )
		{
			printf("RBTree_Find()��������6 ʧ��\n");
		}
        break;    
	case 7:
		pszTemp = (char *)RBTree_Find(pTree, "19", StrCompare);
		if ( strcmp(pszTemp, "19") != 0 )
		{
			printf("RBTree_Find()��������7 ʧ��\n");
		}
        break;    
	case 8:
		RBTree_Delete(pTree,"17", StrCompare, free);
		pszTemp = (char *)RBTree_Find(pTree, "17", StrCompare);
		if ( pszTemp != NULL )
		{
			printf("RBTree_Find()��������8 ʧ��\n");
		}
        break;    		
	case 9:
		RBTree_Delete(pTree,"17", StrCompare, free);
		RBTree_Delete(pTree,"20", StrCompare, free);
		RBTree_Delete(pTree,"16", StrCompare, free);
		RBTree_Delete(pTree,"15", StrCompare, free);
		RBTree_Delete(pTree,"29", StrCompare, free);

		pszTemp = (char *)RBTree_Find(pTree, "28", StrCompare);
		if ( strcmp(pszTemp, "28") != 0 )
		{
			printf("RBTree_Find()��������9 ʧ��\n");
		}
        break;    		
	default:
        break;
    }
	RBTree_Destroy(pTree, free);
	return;
}


