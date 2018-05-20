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
 *	DeQueue.h
 *
 *	DESCRIPTION
 *		Interface to a generic dynamatic queue type.
 *
 *	HISTORY
 *		09-19-2004	create by zhouweiming.
 *
 */
#ifndef __DEQUE_H__
#define __DEQUE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* DeQue�еĻ��ζ��нṹ�嶨�� */
typedef struct DEQUEBLOCK_st {
	UINT uHead;    /* ���ζ��е�ͷ�� */
	UINT uTail;	   /* ���ζ��е�ͷ�� */
	UINT uMapPos;  /* ����Ӧmap�е�λ�� */
	void **ppData; /* ����ָ������ */
} DEQUEBLOCK;

/*  DeQue�Ľṹ�嶨�� */
typedef struct DEQUE_st {
	DEQUEBLOCK **ppMap;   /* �����ζ��е�map����ָ�� */
	DEQUEBLOCK *pFirst;   /* ��һ�����ζ���ָ�� */
	DEQUEBLOCK *pLast;    /* ���һ�����ζ���ָ�� */
	UINT uMapSize;        /* Map��С */
	UINT uBlockSize;      /* ���ζ��д�С */
    UINT uCurMap;         /* ö�ٲ���ʱ�ĵ�ǰMAP */  
    UINT uCurPos;         /* ö�ٲ���ʱ�ĵ�ǰλ�� */
} DEQUE;


DEQUE *	DeQue_Create(UINT uMapSize, UINT uBlockSize);
void	DeQue_Destroy( DEQUE *pQue, DESTROYFUNC DestroyFunc );

INT	    DeQue_InsertTail( DEQUE *pQue, void *pData );

void *  DeQue_PopHead(DEQUE *pQue);

void    DeQue_EnumBegin(DEQUE *pQue);
void *  DeQue_EnumNext(DEQUE *pQue);

#ifdef __cplusplus
}
#endif

#endif /* __DEQUE_H__ */
