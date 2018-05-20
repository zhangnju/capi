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
 *	MTask.h
 *
 *	DESCRIPTION
 *		Interface to a generic multiple task object.
 *	HISTORY
 *		09-09-2000	create by zhouweiming.
 *
 */
#ifndef __MTASK_H__
#define __MTASK_H__

#include "FastLock.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MTASK_NO_EXIT      0
#define MTASK_EXIT         1

typedef struct MTASK_st {
    FASTLOCK    FastLock;    /* 操作锁 */
    EVENT       pExitEvent;  /* 退出事件 */
    
    UINT        uExitFlag;   /* 退出标志 */
    UINT        uTaskCount;  /* 操作的子任务数量 */
} MTASK;


MTASK * MTask_Create();
void    MTask_Destroy(MTASK *pMTask);

void    MTask_Lock(MTASK *pMTask);
void    MTask_Unlock(MTASK *pMTask);

UINT    MTask_GetExitFlag(MTASK *pMTask);
void    MTask_SetExitFlag(MTASK *pMTask, UINT uExitFlag);
void    MTask_EnterTask(MTASK *pMTask);
void    MTask_LeaveTask(MTASK *pMTask);


#ifdef __cplusplus
}
#endif

#endif /* __MTASK_H__ */
