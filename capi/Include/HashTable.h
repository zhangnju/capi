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
 *	HashTable.h
 *
 *	DESCRIPTION
 *		Interface to a generic hash object.
 *
 *	HISTORY
 *		04-22-99	create by zhouweiming.
 *
 */
#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#define	    HASHTABLE_SIZE	    32767
#define		MAX_SEARCH_DEEP		30



typedef struct HASHTABLE {
    SINGLENODE **ppBucket;	    /* ������ָ�� */
    UINT 	    uBucketCount;	/* ������Ĵ�С. */
    UINT 	    uNodeCount;	    /* ���е�ʵ�ʽڵ����. */

    UINT        uCurBucketNo;   /* ��ǰҪִ�е�Bucket��� */
    SINGLENODE *pCurEntry;      /* ��ǰbucket�е���һ��Ҫִ�еĽڵ���Ŀ */ 
} HASHTABLE;



/*** Hash Table operation functions ***/
HASHTABLE * HashTable_Create(UINT uBucketCount);

void    HashTable_Destroy(HASHTABLE *pTable, 
                          DESTROYFUNC DestroyFunc );

INT     HashTable_Insert( HASHTABLE *pTable, 
                          void *pData, 
                          HASHFUNC HashFunc);

void *  HashTable_Find(HASHTABLE *pTable, 
                       void *pData, 
                       HASHFUNC HashFunc,
                       COMPAREFUNC CompareFunc);

INT     HashTable_Delete(HASHTABLE *pTable, 
                         void *pData, 
                         HASHFUNC HashFunc,
                         COMPAREFUNC CompareFunc,
                         DESTROYFUNC DataDestroyFunc );

UINT    HashTable_GetNodeCount(HASHTABLE *pTable);

void    HashTable_EnumBegin(HASHTABLE *pTable);
void *  HashTable_EnumNext(HASHTABLE *pTable);


#ifdef __cplusplus
}
#endif

#endif /* __HASHTABLE_H__ */
