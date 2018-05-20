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

#include <stdlib.h>
#include "CapiGlobal.h"
#include "BigMemory.h"
#include "FreeList.h"
#include "MemoryPool.h"

/**	FreeList�Ĵ�������

	@param	UINT uMemSize -������ڴ���С	
	@return	FREELIST * -�ɹ����ؿռ�����ָ�룬ʧ�ܷ���NULL	
*/
FREELIST * FreeList_Create( UINT uMemSize)
{
	FREELIST   *pFreeList;
	FREENODE   *pNode;
	UINT		uMemCount;
	UINT		i;

    BLOCK  *pBlock = BigMemory_AllocBlock(uMemSize);
    if ( pBlock == NULL )
    {
        return NULL;
    }

	pFreeList = (FREELIST *)pBlock->pDataPtr;

	if ( pFreeList != NULL )
	{
        uMemCount = (pBlock->uBlockSize - sizeof(FREELIST)) / uMemSize;
#ifdef USE_BUMP  /* ���ʹ��pBumpָ��󣬾Ͳ���Ҫ�������ʼ���� */
        pFreeList->pBump = (FREENODE *)((char *)pFreeList + sizeof(FREELIST));
        pFreeList->pTailEdge = (void *)((char *)pFreeList->pBump + uMemCount * uMemSize);

        pFreeList->pHead = NULL;
        pFreeList->uFreeCount = 0;
#else 
        /* ��������ʼ��freelist���� */
        pFreeList->pHead = (FREENODE *)((char *)pFreeList +sizeof(FREELIST));
        pNode = pFreeList->pHead;

        for (i = 0; i < uMemCount - 1; i++) 
        {
            pNode->pNext = (FREENODE *)((char *)pNode + uMemSize);
            pNode = pNode->pNext;
        }
        pNode->pNext = NULL;
        pFreeList->uFreeCount = uMemCount; 
#endif
		pFreeList->uMemCount = uMemCount;
		pFreeList->uMemSize = uMemSize;
	}

	return pFreeList;
}


/**	FreeList�ĳ�ʼ������
    ��һ��û�п����ڴ���Ѿ�����freelist�ṹ����г�ʼ��

	@param	UINT uMemSize -������ڴ���С	
	@return	FREELIST * -�ɹ����ؿռ�����ָ�룬ʧ�ܷ���NULL	
*/
FREELIST * FreeList_Init( FREELIST *pFreeList, UINT uMemSize)
{
	FREENODE   *pNode;
	UINT		uMemCount;
	UINT		i;
	void		*pMem;

    BLOCK *pBlock = BigMemory_AllocBlock(uMemSize);
    if ( pBlock == NULL )
    {
        return NULL;
    }
	pMem = pBlock->pDataPtr;
	uMemCount = pBlock->uBlockSize / uMemSize;
	if ( pMem != NULL )
	{
#ifdef USE_BUMP  /* ���ʹ��pBumpָ��󣬾Ͳ���Ҫ�������ʼ���� */
        pFreeList->pBump = (FREENODE *)pMem;
        pFreeList->pTailEdge = (void *)((char *)pMem + uMemCount * uMemSize);

        pFreeList->pHead = NULL;
        pFreeList->uFreeCount = 0;
#else 
        /* ����freelist���� */
		pFreeList->pHead = (FREENODE *)pMem;
		pNode = pFreeList->pHead;

		for (i = 0; i < uMemCount - 1; i++) 
		{
			pNode->pNext = (FREENODE *)((char *)pNode + uMemSize);
			pNode = pNode->pNext;
		}
		pNode->pNext = NULL;
		pFreeList->uFreeCount = uMemCount; 
#endif
		pFreeList->uMemCount = uMemCount;
		pFreeList->uMemSize = uMemSize;
	}

	return pFreeList;
}


/**	FreeList�ĳ�ʼ������

@param	UINT uMemSize -������ڴ���С	
@return	FREELIST * -�ɹ����ؿռ�����ָ�룬ʧ�ܷ���NULL	
*/
void FreeList_EmptyInit( FREELIST *pFreeList, UINT uMemSize)
{
	pFreeList->pHead = NULL;
#ifdef USE_BUMP
    pFreeList->pBump = NULL;
#endif
	pFreeList->uFreeCount = 0; 
	pFreeList->uMemCount = 0;
	pFreeList->uMemSize = uMemSize;

	return;
}


/**	FreeList���ͷź���

	@param	FREELIST *pFreeList - FreeListָ��	
	@return	void -��	
*/
void FreeList_Destroy(FREELIST *pFreeList)
{
	if ( pFreeList != NULL )
	{
		free( pFreeList );
	}
}

/**	FreeList�ķ����ڴ溯��

	@param	FREELIST *pFreeList - FreeListָ��	
	@return	void * -�ɹ����ط��䵽���ڴ棬���û�������ڴ���з����򷵻�NULL	
*/
void * FreeList_Alloc(FREELIST *pFreeList)
{
	FREENODE   *pNode;
#ifdef USE_BUMP
	pNode = pFreeList->pBump;
	if ( pNode != NULL )
	{
		pFreeList->pBump = (FREENODE *)((char *)pFreeList->pBump + pFreeList->uMemSize);

		if ( (UINT)pFreeList->pBump >= (UINT)pFreeList->pTailEdge )
		{
			pFreeList->pBump = NULL;
		}
		return (void *)pNode;
	}

	/* if pBump has no memory to use, then try to allocate from pHead */
#endif
	pNode = pFreeList->pHead;
	if ( pNode != NULL )
	{
		pFreeList->pHead = pNode->pNext;
		pFreeList->uFreeCount--;
		return (void *)pNode;
	}

	return NULL;
}

/**	FreeList���ͷ��ڴ溯��

	@param	FREELIST *pFreeList - FreeListָ��	
	@param	void *pData -Ҫ�ͷŵ��ڴ�����ָ��	
	@return	void -��	
*/
void FreeList_Free(FREELIST *pFreeList, void *pData)
{
	FREENODE  *pNode;

	pNode= (FREENODE *)pData;
	pNode->pNext = pFreeList->pHead;
	pFreeList->pHead = pNode;
	pFreeList->uFreeCount++;
}


/* ���Կ��ǽ�FreeListsģ��ĳɺ�DSpaceListģ��һ����
 * ʹ��freelist��������freelist 
 */
/**	FreeLists�ĳ�ʼ������

    @param	FREELISTS *pFreeLists - FreeListsָ��	
    @param	UINT uMemSize - ������ڴ��С��λ	
	@return	void  - ��	
*/
void FreeLists_Init(FREELISTS *pFreeLists, UINT uMemSize)
{
	UINT i;
	for( i = 0; i < MAX_FREELIST_IN_FREELISTS; i++)
	{
		FreeList_EmptyInit(&(pFreeLists->aFreeList[i]), uMemSize);
	}

	pFreeLists->uFreeListCount = MAX_FREELIST_IN_FREELISTS;
	pFreeLists->uMemSize = uMemSize;
	pFreeLists->uMemCount = pFreeLists->aFreeList[0].uMemCount;
	pFreeLists->uCurAlloc = 0;
	pFreeLists->uCurFree = 0;
}

/**	��FreeLists�з���һ���ڴ�

	@param	FREELISTS *pFreeLists - FreeListsָ��	
	@return	void * - ���ط�����ڴ�ָ�룬���ϵͳ�ڴ治���򷵻�NULL	
*/
void *FreeLists_Alloc(FREELISTS *pFreeLists)
{
	FREELIST	*pFreeList;
	UINT		uCurAlloc;
	UINT		i;
	FREENODE *  pNode;
    UINT        uMemSize;

	uCurAlloc = pFreeLists->uCurAlloc;
	pFreeList = &(pFreeLists->aFreeList[uCurAlloc]);
	if ( pFreeList->uFreeCount > 0 )
	{
		return FreeList_Alloc(pFreeList);
	}

	for ( i = uCurAlloc+1; i < pFreeLists->uFreeListCount; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount > 0 )
		{
			pFreeLists->uCurAlloc = i;
			return FreeList_Alloc(pFreeList);
		}
	}

	for ( i = 0; i < uCurAlloc; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount > 0 )
		{
			pFreeLists->uCurAlloc = i;
			return FreeList_Alloc(pFreeList);
		}
	}

	/* if no memory have been allocated, then get a memory block 
	 * from the memory pool 
	 */
    uMemSize = pFreeList->uMemSize;
    pFreeList = &(pFreeLists->aFreeList[0]);
	pNode = MemoryPool_Pop(uMemSize);
    if ( pNode == NULL )
    {
        // �������Ѿ�û�пɹ������FREENODE ���� 
        // ��BigMemory�з���һ��Block�������ظ�������
        BLOCK       *pBlock;
        void		*pMem;
        UINT		uMemCount;

        pBlock = BigMemory_AllocBlock(uMemSize);
        if ( pBlock == NULL )
        {
            return NULL;
        }
        pMem = pBlock->pDataPtr;
        uMemCount = pBlock->uMemCount;
#ifdef USE_BUMP
        pFreeList->pBump = pMem;
        pFreeList->pTailEdge = (void *)((char *)pMem + uMemCount * uMemSize);
#else
        pNode = (FREENODE *)pMem;

        for (i = 0; i < uMemCount - 1; i++) 
        {
            pNode->pNext = (FREENODE *)((char *)pNode + uMemSize);
            pNode = pNode->pNext;
        }
        pNode->pNext = NULL;
#endif
        pFreeList->pHead = (FREENODE *)pMem;
        pFreeList->uMemCount = uMemCount;
    }
    else
    {
#ifdef USE_BUMP
        pFreeList->pBump = NULL;
#endif
        pFreeList->pHead = pNode;
        pFreeList->uMemCount = (BIG_MEMORY_BLOCK_SIZE - sizeof(BLOCK)) / uMemSize;
    }
    pFreeList->uFreeCount = pFreeList->uMemCount;	
	pFreeLists->uCurAlloc = 0;
	pFreeLists->uCurFree = 0;

	return FreeList_Alloc(pFreeList);
}

/**	�ͷ�һ���ڴ浽FreeLists��
    ���뱣֤�ͷŵ��ڴ��С��FreeLists������ڴ��С��ͬ

	@param	FREELISTS *pFreeLists - FreeListsָ��	
	@param	void *pData - �ͷŵ��ڴ�ָ��	
	@return	void - ��	
*/
void FreeLists_Free(FREELISTS *pFreeLists, void *pData)
{
	FREELIST	*pFreeList;
	UINT		uCurFree;
	UINT		i;

	uCurFree = pFreeLists->uCurFree;
	pFreeList = &(pFreeLists->aFreeList[uCurFree]);
	if ( pFreeList->uFreeCount < pFreeList->uMemCount )
	{
		FreeList_Free(pFreeList, pData);
		goto LabelMemoryPool;
	}

	for ( i = uCurFree+1; i < pFreeLists->uFreeListCount; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount < pFreeList->uMemCount )
		{
			pFreeLists->uCurFree = i;
			FreeList_Free(pFreeList, pData);
			goto LabelMemoryPool;
		}
	}

	for ( i = 0; i < uCurFree; i++)
	{
		pFreeList = &(pFreeLists->aFreeList[i]);
		if ( pFreeList->uFreeCount < pFreeList->uMemCount )
		{
			pFreeLists->uCurFree = i;
			FreeList_Free(pFreeList, pData);
			goto LabelMemoryPool;
		}
	}

LabelMemoryPool:

	if ( pFreeList->uFreeCount == pFreeList->uMemCount )
	{
		UINT	uTotalCount = 0;
		
        /* Calculate the total count of freelist that have full memory */
		for ( i = 0 ; i < pFreeLists->uFreeListCount; i++ )
		{
			if ( pFreeLists->aFreeList[i].uFreeCount == pFreeLists->uMemCount )
			{
				uTotalCount++;
			}
		}
		if ( uTotalCount > MIN_FULL_FREELIST_COUNT )
		{
			MemoryPool_Push(pFreeList->pHead, pFreeList->uMemSize);
#ifdef USE_BUMP
            pFreeList->pBump = NULL;
#endif
			pFreeList->pHead = NULL;
			pFreeList->uFreeCount = 0;
		}
	}
	return;
}

