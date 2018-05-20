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
 *	MStack.h
 *
 *	DESCRIPTION
 *		用数组实现的支持多任务操作的栈.
 *	HISTORY
 *		10-16-2005	create by zhouweiming.
 *
 */
#ifndef __MSTACK_H__
#define __MSTACK_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MSTACK_st {
    STACK       *pStack;
    FASTLOCK    FastLock;
} MSTACK;

MSTACK * MStack_Create(UINT uStackSize);
void     MStack_Destroy(MSTACK * pMStack, DESTROYFUNC DestroyFunc);

void *   MStack_Pop( MSTACK * pMStack );
INT      MStack_Push( MSTACK * pMStack, void *pData );

INT      MStack_IsEmpty( MSTACK *pMStack );

#ifdef __cplusplus
}
#endif

#endif /* __MSTACK_H__ */
