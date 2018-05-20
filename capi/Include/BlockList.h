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
 *	BlockList.h
 *
 *	DESCRIPTION
 *		Interface to a use a whole block memory list type.
 *
 *	HISTORY
 *		10-02-99	create by zhouweiming.
 *
 */

#ifndef _BLOCKLIST_H_
#define _BLOCKLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BLOCKLIST_st {
    void        *pBlock;        /* �����ڴ�ָ�� */
    SINGLENODE  *pEmpty;        /* ���ɿռ��ͷָ�� */
    SINGLENODE  *pHead;         /* ����ͷָ�� */
    UINT        uDataSize;      /* ���ݴ�С */
    UINT        uMaxDataCount;  /* �������ݽڵ���� */
    UINT        uFreeCount;     /* ���ɿռ�ڵ����� */
} BLOCKLIST;

BLOCKLIST * BlockList_Create(UINT uDataSize, UINT uMaxDataCount);
void        BlockList_Destroy(BLOCKLIST *pList);

INT         BlockList_InsertHead(BLOCKLIST *pList, void *pData, UINT uDataLen);
void        BlockList_DeleteHead(BLOCKLIST *pList);

void      * BlockList_Alloc(BLOCKLIST *pList);
void        BlockList_Free(BLOCKLIST *pList, void *pData);

#ifdef __cplusplus
}
#endif

#endif /* _BLOCKLIST_H_ */
