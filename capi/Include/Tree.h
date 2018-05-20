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
 *	Tree.h
 *
 *	DESCRIPTION
 *		Interface to a generic tree object.
 *
 *	HISTORY
 *		06-19-98	create by zhouweiming.
 *
 */

#ifndef __TREE_H__
#define __TREE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct TREE_st {
    DOUBLELIST    * pLeafList;
    DOUBLELIST    * pSubTreeList;
    void          *pProperties;
} TREE, *LPTREE;


typedef int    (*COMPAREFUNC)( void *prop1, void *prop2 );


// Initialize and destroy functions
TREE *  Tree_Create();

void    Tree_Destroy( 
             TREE * pTree, 
             DESTROYFUNC LeafDestroyFunc, 
             DESTROYFUNC PropDestroyFunc );

// Leaf functions
int     Tree_AddLeaf(TREE * pTree, void *pLeafData);
int     Tree_RemoveLeaf(TREE * pTree, 
                        void *pLeafData, 
                        COMPAREFUNC CompareFunc,
                        DESTROYFUNC LeafDestroyFunc);

// Properties functions
int     Tree_SetProperties(TREE * pTree, 
                           void * pProperties, 
                           DESTROYFUNC PropDestroyFunc);

void *  Tree_GetProperties(TREE * pTree);

// Sub pTree functions
int     Tree_AddSubTree(TREE * pTree, TREE * sub);

int     Tree_RemoveSubTree(
               TREE * pTree, 
               TREE * sub, 
               DESTROYFUNC leaffunc,
               DESTROYFUNC propfunc );

// Copy function
TREE *    Tree_Copy(
              TREE * pTree,
              COPYFUNC LeafCopyFunc, 
              COPYFUNC PropCopyFunc );

// Search functions
TREE *    Tree_FindSubTreeByProp(
              TREE  * pTree,
              void  * pProperties,
              COMPAREFUNC   CompareFunc
              );

#ifdef __cplusplus
}
#endif

#endif  //__TREE_H__