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
    struct FREENODE_st * pNext;  /* ��һ���ڵ�ָ�� */
} FREENODE;

typedef struct FREELIST_st {
    FREENODE    *pHead;     /* freelistͷ���ڵ㣬��ʼ��ʱpHeadΪ�գ�ֻ�е��ͷ����ڴ�ʱ��
                               �ڴ��Żᱻ���뵽pHeadָ�������ͷ�� */
    UINT        uFreeCount; /* pHeadָ��Ŀ��ÿռ�ڵ�ĸ��� */
    UINT		uMemCount;  /* freelist�������ÿռ�ڵ���� */
    UINT		uMemSize;	/* �����ڴ�Ĵ�С */
#ifdef USE_BUMP
    FREENODE    *pBump;     /* ��ʼָ��pBlock��Ϊ�����ʼ��������������
                               ��pBumpָ��������ڴ汻������ʱ�Ŵ�pHead���з��� */
    void		*pTailEdge; /* �����ڴ��β���߽� */
    UINT		uReserved[10]; /* ����64�ֽ� */
#else
    UINT		uReserved[12]; /* ����64�ֽ� */ 
#endif
} FREELIST;


/* FreeLists�ṹ�嶨��
* һ��FreeLists��������һ�ֳߴ���ڴ�
*/
typedef struct FREELISTS_st {
    FREELIST  aFreeList[MAX_FREELIST_IN_FREELISTS]; //�����ڴ����FreeList����
    UINT      uFreeListCount;      // �п����ڴ��FreeList����
    UINT      uCurAlloc;           // ��ǰ���������FreeList�������е�����
    UINT	  uCurFree;            // ��ǰ�����ͷŵ�FreeList�������е�����
    UINT	  uMemSize;            // �ڴ��С
    UINT	  uMemCount;           // һ��FreeList�е�����ڴ����
    UINT      uReserved[11];       //����64�ֽ�
} FREELISTS;


/* FreeListģ��ĺ������� */
FREELIST *	FreeList_Create(UINT uMemSize);
FREELIST *  FreeList_Init( FREELIST *pFreeList, UINT uMemSize);
void		FreeList_EmptyInit( FREELIST *pFreeList, UINT uMemSize);
void		FreeList_Destroy(FREELIST *pFreeList);

void   *	FreeList_Alloc(FREELIST *pFreeList);
void		FreeList_Free(FREELIST *pFreeList, void *pData);

/* FreeListsģ��ĺ������� */
void        FreeLists_Init(FREELISTS *pFreeLists, UINT uMemSize);

void *      FreeLists_Alloc(FREELISTS *pFreeLists);
void        FreeLists_Free(FREELISTS *pFreeLists, void *pData);



#ifdef __cplusplus
}
#endif

#endif /* _FREELIST_H_ */
