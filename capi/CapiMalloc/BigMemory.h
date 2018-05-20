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
 *      Interface to a Big Memory management module.
 *
 *  HISTORY
 *      04-16-2008      create by zhouweiming.
 *
 */

#ifndef __BIGMEMORY_H__
#define __BIGMEMORY_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BLOCK_st {
    UINT    uMemSize;     // �ڴ����Ĵ�С
    UINT    uMemCount;    // �ڴ���������
    UINT    uBlockSize;   // ��Block�п����ڴ�Ĵ�С
    void    *pDataPtr;    // ָ���1�����õ��ڴ�ָ��
    char    reserved[48]; // ����Cache��
} BLOCK, *BLOCKPTR;


#define		BIG_MEMORY_BLOCK_SIZE		(32768)
#define		BIG_MEMORY_SIZE    (BIG_MEMORY_BLOCK_SIZE*32)

INT BigMemory_Init(UINT uMaxBigMemoryCount);

BLOCK *BigMemory_AllocBlock(UINT uMemSize);

void BigMemory_Close();

BLOCK * BigMemory_GetBlockPtr(void *p);

#ifdef __cplusplus
}
#endif

#endif /* __BIGMEMORY_H__ */
