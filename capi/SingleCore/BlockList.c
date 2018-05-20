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
#include "BlockList.h"

/**	�����ڴ�����Ĵ�������

	@param	UINT uDataSize - ���ݴ�С	
	@param	UINT uMaxDataCount - ���������ɴ�����ݵĸ���	
	@return	BLOCKLIST * -�����ڴ�����ָ��	
*/
BLOCKLIST * BlockList_Create(UINT uDataSize, UINT uMaxDataCount)
{
    BLOCKLIST   *pList;
    SINGLENODE  *pNode;
    UINT        i;
    
    pList = (BLOCKLIST *)malloc( sizeof(BLOCKLIST)
        + uMaxDataCount * (uDataSize + sizeof(SINGLENODE)) );
    if ( pList != NULL )
    {
        pList->pBlock = (void *)((char *)pList +sizeof(BLOCKLIST) );
        
        /* ���������� */
        pList->pEmpty = (SINGLENODE *)pList->pBlock;
        pNode = pList->pEmpty;
        
        for (i = 0; i < uMaxDataCount-1; i++)
        {
            pNode->pNext = (SINGLENODE *)((char *)pNode + sizeof(SINGLENODE)
                + uDataSize);
            pNode = pNode->pNext;
        }
        pNode->pNext = NULL;
        
        pList->uFreeCount = uMaxDataCount;
        pList->uDataSize = uDataSize;
        pList->uMaxDataCount = uMaxDataCount;
        pList->pHead = NULL;
    }
    
    return pList;
}

/**	�����ڴ�������ͷź���

	@param	BLOCKLIST *pList - �����ڴ�����ָ��	
	@return	void - ��	
*/
void BlockList_Destroy(BLOCKLIST *pList)
{
    if ( pList != NULL )
    {
        free( pList );
    }
}

/**	 �����ڴ�����Ĳ������ݺ���

	@param	BLOCKLIST *pList - �����ڴ�����ָ��	
	@param	void *pData - Ҫ����Ľڵ�����ָ��	
	@param	UINT uDataLen - Ҫ��������ݵĳ���	
	@return	INT - ʧ�ܷ���-1���ɹ�����0��	
*/
INT BlockList_InsertHead(BLOCKLIST *pList, void *pData, UINT uDataLen)
{
    SINGLENODE  *pNode;
    if ( uDataLen > pList->uDataSize )
    {
        return -1;
    }

    pNode = pList->pEmpty;
    if ( pNode != NULL )
    {
        pList->pEmpty = pNode->pNext;
    }
    else
    {
        return -1;
    }

    pNode->pNext = pList->pHead;
	pNode->pData = (void *)((char *)pNode + sizeof(SINGLENODE)); /* Add 2006-07-14 */
    memcpy(pNode->pData, pData, uDataLen);
    
    pList->pHead = pNode;
    pList->uFreeCount--;

    return 0;
}

/**	�����ڴ������ɾ��ͷ���ڵ㺯��

	@param	BLOCKLIST *pList - �����ڴ�����ָ��		
	@return	void - ��	
*/
void  BlockList_DeleteHead(BLOCKLIST *pList)
{
    SINGLENODE	*pPopNode;   /* ����ָ��Ҫ�������ݵĽڵ��ָ�� */

    /* ����У�� */
    if ( pList == NULL || pList->pHead == NULL ) 
    {
        return;
    }

    /* ��������ͷ�ڵ� */
    pPopNode = pList->pHead;
    pList->pHead = pList->pHead->pNext;

    /* �������Ľڵ���뵽�������ͷ�� */
    pPopNode->pNext = pList->pEmpty;
    pList->pEmpty = pPopNode;
	
    /* ������ڵ�������1 */
    pList->uFreeCount++;

    return;    
}


/**	�����ڴ�������ڴ���亯��
        �ӿ����е���һ���ڵ㣬������Ϊ������ڴ�

	@param	BLOCKLIST *pList - �����ڴ�����ָ��	
	@return	void * - �ɹ����ط��䵽���ڴ�ָ�룬ʧ�ܷ���NULL	
*/
void * BlockList_Alloc(BLOCKLIST *pList)
{
    SINGLENODE   *pNode;
    
    pNode = pList->pEmpty;
    if ( pNode != NULL )
    {
        pList->pEmpty = pNode->pNext;
        pList->uFreeCount--;
        return (void *)pNode;  
    }
    return NULL;
}


/**	�����ڴ�������ڴ��ͷź���
        ���ڴ��ͷŵ����뵽������

	@param	BLOCKLIST *pList - �����ڴ�����ָ��
	@param	void *pData - Ҫ�ͷŵ�����ָ��	
	@return	void - ��	
*/
void BlockList_Free(BLOCKLIST *pList, void *pData)
{
    SINGLENODE  *pNode;
    
    pNode= (SINGLENODE *)pData;
    pNode->pNext = pList->pEmpty;
    pList->pEmpty = pNode;
    pList->uFreeCount++;
}


