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
 *  DSpaceList.h
 *
 *  DESCRIPTION
 *      Interface to a dynamic available space list type.
 *
 *  HISTORY
 *      10-22-2004      create by zhouweiming.
 *
 */

#ifndef _DSPACELIST_H_
#define _DSPACELIST_H_

#include "SpList.h"

#ifdef __cplusplus
extern "C" {
#endif

#define     DSPACENODE_IN_LIST      1
#define     DSPACENODE_NOT_IN_LIST  0

typedef struct DSPACENODE_st {
    struct DSPACENODE_st * pPrev;        /* ǰһ���ڵ�ָ�� */
    struct DSPACENODE_st * pNext;        /* ��һ���ڵ�ָ�� */
    SPLIST               * pSpList;      /*���ÿռ��ָ�� */
    UINT                   uInListFlag;  /* ��ʾ���ڵ��Ƿ��������еı�־ */
    UINT                   uPos;         /* �ڵ��������е��±�λ�� */
} DSPACENODE;

typedef struct DSPACELIST_st {
    DSPACENODE  *pDSNode;         /* ����������ÿռ���DSPACENODE���� */
    DSPACENODE  *pEmpty;          /* ���ÿռ����û�б�ʹ�õĽڵ������ͷ */
    DSPACENODE  *pHead;           /* �����ɿռ��DSPACENODE����ͷ�� */
    DSPACENODE  *pTail;           /* �����ɿռ��DSPACENODE����β�� */
    UINT        uDataCount;       /* ÿ�����ÿռ���е����ݸ��� */
    UINT        uDataSize;        /* ���ݴ�С */
    UINT        uDSNodeCount;     /* �ܵ�DSPACENODE�ڵ����� */
    UINT        uCurrentDSNode;   /* ��ǰ����ʹ�õ�DSPACENODE���� */
    UINT        uMinDSNodeCount;  /* ��Сʹ�õ�DSPACENODE���� */
    UINT        uUsedDSNodeCount; /* ����ʹ�õ�DSPACENODE���� */
} DSPACELIST;

DSPACELIST *DSpaceList_Create(UINT uDataSize, UINT uDataCount,
                              UINT uMinDSNodeCount, UINT uDSNodeCount);
void        DSpaceList_Destroy(DSPACELIST *pList);

void       *DSpaceList_Alloc(DSPACELIST *pList);
void        DSpaceList_Free(DSPACELIST *pList, void *pData);

#ifdef __cplusplus
}
#endif

#endif /* _DSPACELIST_H_ */

