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

/* DeQue中的环形队列结构体定义 */
typedef struct DEQUEBLOCK_st {
	UINT uHead;    /* 环形队列的头部 */
	UINT uTail;	   /* 环形队列的头部 */
	UINT uMapPos;  /* 所对应map中的位置 */
	void **ppData; /* 数据指针数组 */
} DEQUEBLOCK;

/*  DeQue的结构体定义 */
typedef struct DEQUE_st {
	DEQUEBLOCK **ppMap;   /* 管理环形队列的map数组指针 */
	DEQUEBLOCK *pFirst;   /* 第一个环形队列指针 */
	DEQUEBLOCK *pLast;    /* 最后一个环形队列指针 */
	UINT uMapSize;        /* Map大小 */
	UINT uBlockSize;      /* 环形队列大小 */
    UINT uCurMap;         /* 枚举操作时的当前MAP */  
    UINT uCurPos;         /* 枚举操作时的当前位置 */
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
