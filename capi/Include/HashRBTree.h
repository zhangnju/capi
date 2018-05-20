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
 *	HashRBTree.h
 *
 *	DESCRIPTION
 *		Interface to a generic HashRBTree type.
 *
 *	HISTORY
 *		05-02-2004	create by zhouweiming.
 *
 */

#ifndef __HASHRBTREE_H__
#define __HASHRBTREE_H__


#ifdef __cplusplus
extern "C" {
#endif

#if 0
typedef struct HASHRBTREENODE_st {
    struct HASHRBTREENODE_st *pLeft;
    struct HASHRBTREENODE_st *pRight;
    struct HASHRBTREENODE_st *pParent;
    INT                      nColor;
    
    struct HASHRBTREENODE_st *pNext;

    void    *pKey;
    UINT    ukeyLen;
    void    *pData;
    UINT    uDataLen;
} HASHRBTREENODE;
#endif


typedef struct HASHRBTREENODE_st {
    RBTREENODE  TreeNode;
    struct HASHRBTREENODE_st *pNext;
} HASHRBTREENODE;

typedef struct HASHRBTREE_st {
    RBTREE          *pTree;         /* 红黑树的指针 */  
    HASHRBTREENODE **ppBucket;     /* 哈希表的索引 */
    UINT 	    uBucketCount;  /* 哈希表索引中桶的数量 */        
} HASHRBTREE;

HASHRBTREE *HashRBTree_Create(UINT uBucketCount);
void HashRBTree_Destroy(HASHRBTREE *pHashRBTree, DESTROYFUNC DestroyFunc);


INT HashRBTree_Insert(HASHRBTREE *pHashRBTree, void *pData, HASHFUNC HashFunc,
                      COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc);

INT HashRBTree_Delete(HASHRBTREE *pHashRBTree, void *pData, 
                      HASHFUNC HashFunc, 
                      COMPAREFUNC CompareFunc,
                      DESTROYFUNC DestroyFunc);

void * HashRBTree_HashFind(HASHRBTREE *pHashRBTree, void *pData, 
                           HASHFUNC HashFunc, 
                           COMPAREFUNC CompareFunc );

void * HashRBTree_TreeFind(HASHRBTREE *pHashRBTree, void *pData, 
                           COMPAREFUNC CompareFunc );

void HashRBTree_EnumBegin(HASHRBTREE *pHashRBTree);

void *HashRBTree_EnumNext(HASHRBTREE *pHashRBTree);




#ifdef __cplusplus
}
#endif

#endif /* __HASHRBTREE_H__ */
