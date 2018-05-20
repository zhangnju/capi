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
 *	AVLTree.h
 *
 *	DESCRIPTION
 *		Interface to a AVL tree type.
 *
 *	HISTORY
 *		10-15-2000	create by zhouweiming.
 *
 */

#ifndef __AVLTREE_H__
#define __AVLTREE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef  BINTREEBASENODE  AVLTREENODE;

typedef struct AVLTREE_st {
    AVLTREENODE *pRoot;       /* AVL树的根节点 */
    UINT        uNodeCount;   /* AVL树的节点个数 */
} AVLTREE;

AVLTREE * AVLTree_Create(void);
void      AVLTree_Destroy(AVLTREE *pTree, DESTROYFUNC DestroyFunc);

UINT      AVLTree_GetSubTreeCount(AVLTREENODE *pStartNode);


void    * AVLTree_Find(AVLTREE *pTree, void *pData, COMPAREFUNC CompareFunc);

INT       AVLTreeNode_Insert(AVLTREENODE **ppRootNode, void *pData, 
                             COMPAREFUNC CompareFunc);
INT       AVLTree_Insert(AVLTREE *pTree, void *pData, COMPAREFUNC CompareFunc);

INT       AVLTreeNode_Delete(AVLTREENODE **ppRoot, void *pData, 
                       COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc);
INT       AVLTree_Delete(AVLTREE *pTree, void *pData, COMPAREFUNC CompareFunc,
                         DESTROYFUNC DestroyFunc);

#ifdef __cplusplus
}
#endif


#endif /* __AVLTREE_H__ */
