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
 *      Interface to a available space list type.
 *
 *  HISTORY
 *      10-23-2004      create by zhouweiming.
 *
 */

#ifndef _SPLIST_H_
#define _SPLIST_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SPNODE_st {
    UINT               uPos;   /* 位置，即在数组中的位置，为数组下标 */
    struct SPNODE_st * pNext;  /* 下一个节点指针 */
} SPNODE;

typedef struct SPLIST_st {
    void        *pBlock;    /* 一块连续内存 */
    SPNODE      *pHead;     /* 自由空间链表头部节点 */
    UINT        uFreeCount; /* 自由空间节点的个数 */
} SPLIST;

SPLIST * SpList_Create(UINT uSpaceCount, UINT uDataSize, UINT uPos);
void     SpList_Destroy(SPLIST *pSpList);

void   * SpList_Alloc(SPLIST *pSpList);
void     SpList_Free(SPLIST *pSpList, void *pData);

#ifdef __cplusplus
}
#endif

#endif /* _SPLIST_H_ */

