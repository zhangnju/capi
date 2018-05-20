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
 *	HashAVLTree.h
 *
 *	DESCRIPTION
 *		Interface to a generic HashAVLTree type.
 *
 *	HISTORY
 *		08-08-2004	create by zhouweiming.
 *
 */

#ifndef __HASHAVLTREE_H__
#define __HASHAVLTREE_H__


#ifdef __cplusplus
extern "C" {
#endif

typedef BINTREEBASENODE AVLTREENODE;

typedef struct HASHAVLTREE_st {
    AVLTREENODE **ppBucket;	/* ������ָ�� */
    UINT 	uBucketCount;	/* ������Ĵ�С. */
    UINT 	uNodeCount;	/* ���е�ʵ�ʽڵ����. */
    
    UINT        uCurBucketNo;   /* ��ǰҪִ�е�Bucket��� */
    AVLTREENODE *pCurEntry;      /* ��ǰbucket�е���һ��Ҫִ�еĽڵ���Ŀ */ 
} HASHAVLTREE;


HASHAVLTREE *HashAVLTree_Create(UINT uBucketCount);
void HashAVLTree_Destroy(HASHAVLTREE *pHashAVLTree, DESTROYFUNC DestroyFunc);


INT HashAVLTree_Insert(HASHAVLTREE *pHashAVLTree, void *pData, HASHFUNC HashFunc,
                      COMPAREFUNC CompareFunc);

INT HashAVLTree_Delete(HASHAVLTREE *pHashAVLTree, void *pData, 
                      HASHFUNC HashFunc, 
                      COMPAREFUNC CompareFunc,
                      DESTROYFUNC DestroyFunc);

void * HashAVLTree_Find(HASHAVLTREE *pHashAVLTree, void *pData, 
                       HASHFUNC HashFunc, 
                       COMPAREFUNC CompareFunc );

void HashAVLTree_EnumBegin(HASHAVLTREE *pHashAVLTree);

void *HashAVLTree_EnumNext(HASHAVLTREE *pHashAVLTree);




#ifdef __cplusplus
}
#endif

#endif /* __HASHAVLTREE_H__ */
