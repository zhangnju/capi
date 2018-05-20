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
 *	RBTree.h
 *
 *	DESCRIPTION
 *		Interface to a generic red black tree type.
 *
 *	HISTORY
 *		12-06-2003	create by zhouweiming.
 *
 */

#ifndef __RBTREE_H__
#define __RBTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define     RBTREE_COLOR_RED    0
#define     RBTREE_COLOR_BLACK  1

typedef  BINTREEBASENODE  RBTREENODE;

typedef struct RBTREE_st {
    RBTREENODE *pRoot;
    RBTREENODE *pCursor;
    UINT        uNodeCount;
} RBTREE;


typedef RBTREENODE *(*RBTREENODECREATEFUNC)( void *pData);

RBTREE *RBTree_Create(void);
void     RBTree_Destroy(RBTREE *pBinTree, DESTROYFUNC DestroyFunc);

INT      RBTree_Insert(RBTREE *pBinTree, void *pData, COMPAREFUNC CompareFunc);
INT      RBTree_Delete(RBTREE *pTree, void *pData, 
                       COMPAREFUNC CompareFunc,
                       DESTROYFUNC DestroyFunc);

void    *RBTree_Find(RBTREE *pBinTree, void *pData, COMPAREFUNC CompareFunc);

INT RBTree_Inter_Insert(RBTREE *pTree, RBTREENODE *pNode, COMPAREFUNC CompareFunc);

void RBTree_EnumBegin(RBTREE *pTree);
void * RBTree_EnumNext(RBTREE *pTree);

void *RBTree_GetMinium(RBTREE *pTree, RBTREENODE *pNode);
void *RBTree_GetMaxium(RBTREE *pTree, RBTREENODE *pNode);


INT ChangeNode(RBTREE *pTree, char *pSrcData, char *pTagData, INT nMagic);
INT CompareRBTree(RBTREENODE *pSrcNode, RBTREENODE *pTagNode);



#ifdef __cplusplus
}
#endif


#endif /* __RBTREE_H__ */
