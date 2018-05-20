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
 *	MTree.h
 *
 *	DESCRIPTION
 *		Interface to a generic multiple task red black tree object.
 *      使用一个锁来保护枚举的情况，
        添加和删除操作如果和当前枚举节点相关的话需要
        修改枚举变量当前节点指针
 *	HISTORY
 *		10-06-2004	create by zhouweiming.
 *
 */
#ifndef __MTREE_H__
#define __MTREE_H__


#ifdef __cplusplus
extern "C" {
#endif

HANDLE  MTree_Create(void);
void    MTree_Destroy(HANDLE hTree, DESTROYFUNC DestroyFunc);

INT     MTree_Insert(HANDLE hTree, void *pData, COMPAREFUNC CompareFunc);
INT     MTree_Delete(HANDLE hTree, void *pData, COMPAREFUNC CompareFunc, 
                     DESTROYFUNC DestroyFunc);

void    MTree_LeaveTask(HANDLE hTree );
void    MTree_EnterTask(HANDLE hTree );

UINT    MTree_GetExitFlag(HANDLE hTree );
void    MTree_SetExitFlag(HANDLE hTree, UINT uExitFlag );

void    MTree_EnumBegin(HANDLE hTree);
void *  MTree_EnumNextCopy(HANDLE hTree, COPYFUNC CopyFunc);

#ifdef __cplusplus
}
#endif


#endif /* __MTREE_H__ */
