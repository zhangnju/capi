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
 *	HashList.h
 *
 *	DESCRIPTION
 *		Interface to a generic HashList type.
 *
 *	HISTORY
 *		10-12-2003	create by zhouweiming.
 *
 */

#ifndef __HASHLIST_H__
#define __HASHLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MINIUM_BUCKET_COUNT   32


typedef struct HASHLISTNODE_st {
    struct HASHLISTNODE_st *pListNext;   /* 链表的后向节点指针 */
    struct HASHLISTNODE_st *pListPrev;   /* 链表的前向节点指针 */
    struct HASHLISTNODE_st *pBucketNext; /* 哈希表的链接指针 */
    void			 *pData;			 /* 数据指针 */
} HASHLISTNODE;

typedef struct HASHLIST_st {
    HASHLISTNODE **ppBuckets;    /* 哈希表的索引表 */
    UINT         uBucketCount;   /* 索引表的大小 */
    HASHLISTNODE *pHead;         /* 链表的头指针 */
    HASHLISTNODE *pTail;         /* 链表的尾指针 */
    UINT         uNodeCount;     /* 哈希链表中的节点个数 */
} HASHLIST;


HASHLIST *HashList_Create(UINT uBucketCount);

void HashList_Destroy(HASHLIST *pHashList, 
                       DESTROYFUNC DestroyFunc);

INT HashList_InsertHead(HASHLIST *pHashList, void *pData, HASHFUNC HashFunc);

INT HashList_Delete(HASHLIST *pHashList, 
                    void           *pData,
                    HASHFUNC        HashFunc,
                    COMPAREFUNC     HashCompareFunc, 
                    DESTROYFUNC     DestroyFunc);


HASHLISTNODE *HashList_FindNode(HASHLIST *pHashList, 
                                void *pData, 
                                HASHFUNC HashFunc, 
                                COMPAREFUNC CompareFunc);

void *HashList_FindData(HASHLIST *pHashList, 
                        void *pData, 
                        HASHFUNC HashFunc, 
                        COMPAREFUNC CompareFunc);

INT HashList_InsertSort(HASHLIST *pHashList, COMPAREFUNC CompareFunc);


UINT        HashStr( void *str, UINT str_len, UINT numBuckets );
UINT        HashStrCompare( void *str1, UINT str1_len, void *str2, UINT str2_len );
void        HashFree(void *pData, UINT uDataLen);

#ifdef __cplusplus
}
#endif

#endif /* __HASHLIST_H__ */
