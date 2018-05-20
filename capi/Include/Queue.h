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
 *	Queue.h
 *
 *	DESCRIPTION
 *		Interface to a generic Queue type.
 *
 *	HISTORY
 *		09-18-98	create by zhouweiming.
 *
 */
#ifndef __QUEUE_H__
#define __QUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct QUEUE_st {
	void	**ppData;		/* 存放数据指针的指针数组 */
	UINT    uMaxCount;		/* 队列中的最大数据数量 */
    UINT    uHead;          /* 队列头部位置 */
    UINT    uTail;          /* 队列尾部位置 */
} QUEUE; 


QUEUE *	Queue_Create(UINT uMaxCount);
void	Queue_Destroy( QUEUE *pQueue, DESTROYFUNC DestroyFunc );

INT	    Queue_InsertTail( QUEUE *pQueue, void *pData );
void *  Queue_PopHead(QUEUE *pQueue);

INT     Queue_IsFull(QUEUE *pQueue);
INT     Queue_IsEmpty(QUEUE *pQueue);


#ifdef __cplusplus
}
#endif

#endif /* __QUEUE_H__ */
