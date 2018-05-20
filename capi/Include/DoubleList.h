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

/* 
 *	DoubleList.h
 *
 *	DESCRIPTION
 *		Interface to a generic double list type.
 *
 *	HISTORY
 *		10-06-98	create by zhouweiming.
 *
 */
#ifndef __DOUBLELIST_H__
#define __DOUBLELIST_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DOUBLELIST_st {
    DOUBLENODE	*pHead;     /* 第1个节点的指针 */
    DOUBLENODE	*pTail;     /* 最后1个节点的指针 */
    DOUBLENODE  *pCur;      /* 当前节点的指针 */
    UINT        uCount;     /* 保存链表节点的个数 */
} DOUBLELIST;


/* create/destroy functions */
DOUBLELIST *    DoubleList_Create( void );
void            DoubleList_Destroy( DOUBLELIST * pList, 
                                   DESTROYFUNC DestroyFunc );

/* insertion functions */
INT DoubleList_InsertHead( DOUBLELIST *pList, void *pData );
INT DoubleList_InsertTail( DOUBLELIST *pList, void *pData );

/* removal functions */
void *	DoubleList_PopHead( DOUBLELIST *pList );
void *	DoubleList_PopTail( DOUBLELIST *pList );

INT	DoubleList_Delete( DOUBLELIST *pList, 
                             void *pMatchData, 
				 COMPAREFUNC CompareFunc, 
				 DESTROYFUNC DestroyFunc );


/* retrieval functions */
void *	DoubleList_GetHead( DOUBLELIST *pList );
void * 	DoubleList_GetTail( DOUBLELIST *pList );


void *	DoubleList_Find( DOUBLELIST *pList, void *pMatchData, 
                        COMPAREFUNC CompareFunc );



void            DoubleList_EnumBegin( DOUBLELIST *pList );
void       *    DoubleList_EnumNext( DOUBLELIST *pList );

DOUBLENODE *    DoubleList_EnumNode( DOUBLELIST *pList );

DOUBLENODE *    DoubleList_PopNode( DOUBLELIST *pList, DOUBLENODE *pNode );


/* miscellaneous functions */
unsigned        DoubleList_GetCount( DOUBLELIST *pList );
DOUBLELIST *    DoubleList_Copy( DOUBLELIST *pList, COPYFUNC CopyFunc );

/* sort functions */
INT             DoubleList_InsertSort( DOUBLELIST *pList, 
                                      COMPAREFUNC CompareFunc );

INT             DoubleList_Merge( DOUBLELIST *pListA, DOUBLELIST *pListB, 
                                 COMPAREFUNC CompareFunc );
DOUBLELIST*     DoubleList_Split( DOUBLELIST *pList, unsigned count );

INT             DoubleList_MergeSort(DOUBLELIST *pList, 
                                 COMPAREFUNC CompareFunc, 
                                 UINT uInsertSortCount);

INT DoubleList_RadixSort( DOUBLELIST *pList,
                         UINT uRadix,
                         UINT uMaxKeyLen,
                         GETKEYFUNC GetKeyFunc );

/* traverse functions */
void DoubleList_Traverse( DOUBLELIST *pList, TRAVERSEFUNC TraverseFunc );

#define MAX_RADIX  1000

#ifdef __cplusplus
}
#endif

#endif /* __DOUBLELIST_H__ */
