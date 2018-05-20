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
 *      Interface to a available free list type.
 *
 *  HISTORY
 *      04-16-2008      create by zhouweiming.
 *
 */

#ifndef _FREELIST_H_
#define _FREELIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#define		MAX_FREELIST_IN_FREELISTS    16
#define     MIN_FULL_FREELIST_COUNT      3

typedef struct FREENODE_st {
    struct FREENODE_st * pNext;  /* 下一个节点指针 */
} FREENODE;

typedef struct FREELIST_st {
    FREENODE    *pHead;     /* freelist头部节点，初始化时pHead为空，只有当释放了内存时，
                               内存块才会被插入到pHead指向链表的头部 */
    UINT        uFreeCount; /* pHead指向的可用空间节点的个数 */
    UINT		uMemCount;  /* freelist的最大可用空间节点个数 */
    UINT		uMemSize;	/* 分配内存的大小 */
#ifdef USE_BUMP
    FREENODE    *pBump;     /* 初始指向pBlock，为避免初始化整个链表而设计
                               当pBump指向的连续内存被分配完时才从pHead进行分配 */
    void		*pTailEdge; /* 连续内存的尾部边界 */
    UINT		uReserved[10]; /* 对齐64字节 */
#else
    UINT		uReserved[12]; /* 对齐64字节 */ 
#endif
} FREELIST;


/* FreeLists结构体定义
* 一个FreeLists用作管理一种尺寸的内存
*/
typedef struct FREELISTS_st {
    FREELIST  aFreeList[MAX_FREELIST_IN_FREELISTS]; //用作内存管理FreeList数组
    UINT      uFreeListCount;      // 有空闲内存的FreeList数量
    UINT      uCurAlloc;           // 当前用作分配的FreeList在数组中的索引
    UINT	  uCurFree;            // 当前用作释放的FreeList在数组中的索引
    UINT	  uMemSize;            // 内存大小
    UINT	  uMemCount;           // 一个FreeList中的最大内存个数
    UINT      uReserved[11];       //对齐64字节
} FREELISTS;


/* FreeList模块的函数定义 */
FREELIST *	FreeList_Create(UINT uMemSize);
FREELIST *  FreeList_Init( FREELIST *pFreeList, UINT uMemSize);
void		FreeList_EmptyInit( FREELIST *pFreeList, UINT uMemSize);
void		FreeList_Destroy(FREELIST *pFreeList);

void   *	FreeList_Alloc(FREELIST *pFreeList);
void		FreeList_Free(FREELIST *pFreeList, void *pData);

/* FreeLists模块的函数定义 */
void        FreeLists_Init(FREELISTS *pFreeLists, UINT uMemSize);

void *      FreeLists_Alloc(FREELISTS *pFreeLists);
void        FreeLists_Free(FREELISTS *pFreeLists, void *pData);



#ifdef __cplusplus
}
#endif

#endif /* _FREELIST_H_ */
