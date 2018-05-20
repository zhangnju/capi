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
#include "SpList.h"

/**	�ռ�����Ĵ�������

	@param	UINT uSpaceCount - �ռ�����	
	@param	UINT uDataSize -���ݴ�С	
	@param	UINT uPos -�ڹ��������е�λ��	
	@return	SPLIST * -�ɹ����ؿռ�����ָ�룬ʧ�ܷ���NULL	
*/
SPLIST *SpList_Create(UINT uSpaceCount, UINT uDataSize, UINT uPos)
{
    SPLIST   *pSpList;
    SPNODE   *pNode;
    UINT     i;
    
    pSpList = (SPLIST *)malloc( sizeof(SPLIST)
        + uSpaceCount * (uDataSize + sizeof(SPNODE)) );
    if ( pSpList != NULL )
    {
        pSpList->pBlock = (void *)((char *)pSpList +sizeof(SPLIST) );
        
        /* �������ɿռ����� */
        pSpList->pHead = (SPNODE *)pSpList->pBlock;
        pNode = pSpList->pHead;
        
        for (i = 0; i < uSpaceCount - 1; i++) /* ��uSpaceCount��ΪuSpaceCount-1 */
        {
            pNode->uPos = uPos;
            pNode->pNext = (SPNODE *)((char *)pNode + sizeof(SPNODE)
                + uDataSize);
            pNode = pNode->pNext;
        }
		pNode->uPos = uPos; /* �����ӵ�һ�д��� */
        pNode->pNext = NULL;

        pSpList->uFreeCount = uSpaceCount;
    }
    
    return pSpList;
}


/**	�ռ�������ͷź���

	@param	SPLIST *pSpList - �ռ�����ָ��	
	@return	void -��	
*/
void SpList_Destroy(SPLIST *pSpList)
{
    if ( pSpList != NULL )
    {
        free( pSpList );
    }
}

/**	�ռ�����ķ����ڴ溯��

	@param	SPLIST *pSpList - �ռ�����ָ��	
	@return	void * -�ɹ����ط��䵽���ڴ棬���û�������ڴ���з����򷵻�NULL	
*/
void *SpList_Alloc(SPLIST *pSpList)
{
    SPNODE   *pNode;
    
    pNode = pSpList->pHead;
    if ( pNode != NULL )
    {
        pSpList->pHead = pNode->pNext;
        pSpList->uFreeCount--;
        return (void *)((char *)pNode + sizeof(SPNODE));
    }
    return NULL;
}

/**	�ռ�������ͷ��ڴ溯��

	@param	SPLIST *pSpList -�ռ�����ָ��	
	@param	void *pData -Ҫ�ͷŵ��ڴ�����ָ��	
	@return	void -��	
*/
void SpList_Free(SPLIST *pSpList, void *pData)
{
    SPNODE  *pNode;
    
    pNode= (SPNODE *)((char *)pData - sizeof(SPNODE));
    pNode->pNext = pSpList->pHead;
    pSpList->pHead = pNode;
    pSpList->uFreeCount++;
}

