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
 *
 *  DESCRIPTION
 *      Interface to a Memory Pool module.
 *
 *  HISTORY
 *      04-18-2008      create by zhouweiming.
 *
 */

#ifndef _MEMORYPOOL_H_
#define _MEMORYPOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#define  MAX_MEMORY_CLASS                  19

extern UINT g_auMemSize[MAX_MEMORY_CLASS];

int        MemoryPool_GetMemSizeIndex(UINT uMemSize);

void       MemoryPool_Init();
void       MemoryPool_Push(FREENODE *pNode, UINT uMemSize);
FREENODE * MemoryPool_Pop(UINT uMemSize);


#ifdef __cplusplus
}
#endif

#endif /* _MEMORYPOOL_H_ */
