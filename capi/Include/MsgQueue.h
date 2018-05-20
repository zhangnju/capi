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
 *	MsgQueue.h
 *
 *	DESCRIPTION
 *		Interface to a generic message queue object.
 *	HISTORY
 *		09-9-2000	create by zhouweiming.
 *
 */
#ifndef __MSGQUEUE_H__
#define __MSGQUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif


typedef struct MSGQUEUE_st {
    MTLIST      *pList;    /* 多任务链表 */
    SEMAPHORE   pSem;      /* 处理队列为空情况的计数信号量*/
    INT         nMaxLen;   /* 队列的最大长度 */
} MSGQUEUE;


MSGQUEUE * MsgQueue_Create(INT nMaxLen);
void       MsgQueue_Destroy(MSGQUEUE *pQueue, DESTROYFUNC DestroyFunc);
INT        MsgQueue_Send(MSGQUEUE *pQueue, void *pData);
void     * MsgQueue_Recv(MSGQUEUE *pQueue);

void       MsgQueue_EnumBegin(MSGQUEUE *pQueue);
INT        MsgQueue_EnumNext(MSGQUEUE *pQueue, VISITFUNC VisitFunc);


#ifdef __cplusplus
}
#endif

#endif /* __MSGQUEUE_H__ */