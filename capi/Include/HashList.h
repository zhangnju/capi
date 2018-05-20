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
    struct HASHLISTNODE_st *pListNext;   /* ����ĺ���ڵ�ָ�� */
    struct HASHLISTNODE_st *pListPrev;   /* �����ǰ��ڵ�ָ�� */
    struct HASHLISTNODE_st *pBucketNext; /* ��ϣ�������ָ�� */
    void			 *pData;			 /* ����ָ�� */
} HASHLISTNODE;

typedef struct HASHLIST_st {
    HASHLISTNODE **ppBuckets;    /* ��ϣ��������� */
    UINT         uBucketCount;   /* ������Ĵ�С */
    HASHLISTNODE *pHead;         /* �����ͷָ�� */
    HASHLISTNODE *pTail;         /* �����βָ�� */
    UINT         uNodeCount;     /* ��ϣ�����еĽڵ���� */
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
