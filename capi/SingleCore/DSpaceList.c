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
#include "DSpaceList.h"

/**	��̬�ȳߴ��ڴ�������Ĵ�������

	@param	UINT uDataSize -������Ŀռ��������ݴ�С	
	@param	UINT uDataCount -������Ŀռ�������������	
	@param	UINT uMinDSNodeCount -��С����Ŀռ��������	
	@param	UINT uDSNodeCount -�������Ŀռ��������	
	@return	DSPACELIST * - �ɹ����صȳߴ��ڴ�������ָ�룬ʧ�ܷ���NULL
*/
DSPACELIST *DSpaceList_Create(UINT uDataSize, UINT uDataCount,
                              UINT uMinDSNodeCount, UINT uDSNodeCount)
{
    DSPACELIST  *pList;
    UINT        i;

    pList = (DSPACELIST *)malloc(sizeof(DSPACELIST));
    if ( pList == NULL )
    {
        return NULL;
    }
    pList->pDSNode = (DSPACENODE *)malloc(uDSNodeCount * sizeof(DSPACENODE));
    if ( pList->pDSNode == NULL )
    {
        free(pList);
        return NULL;
    }

    for ( i = 0; i < uDSNodeCount - 1; i++ )
    {
        pList->pDSNode[i].pNext = &(pList->pDSNode[i+1]);
        pList->pDSNode[i].uPos = i;
    }
    
    for ( i = 1; i < uDSNodeCount; i++ )
    {
        pList->pDSNode[i].pSpList = NULL;
        pList->pDSNode[i].uInListFlag = DSPACENODE_NOT_IN_LIST;
        pList->pDSNode[i].pPrev = NULL;
    }
    
    pList->pDSNode[uDSNodeCount-1].pNext = NULL;
    pList->pDSNode[uDSNodeCount-1].uPos = uDSNodeCount-1;

    pList->pEmpty = &(pList->pDSNode[1]);

    pList->pDSNode[0].pSpList = SpList_Create(uDataCount, uDataSize, 0);
    if ( pList->pDSNode[0].pSpList == NULL )
    {
        free(pList->pDSNode);
        free(pList);
        return NULL;
    }
    
    /* ����0���ڵ���뵽˫�������� */
    pList->pHead = &(pList->pDSNode[0]);
    pList->pTail = pList->pHead;
    pList->pHead->pNext = NULL;
    pList->pHead->pPrev = NULL;
    pList->pDSNode[0].uInListFlag = DSPACENODE_IN_LIST;

    if ( uDSNodeCount < uMinDSNodeCount )
    {
        uDSNodeCount = uMinDSNodeCount;
    }

    pList->uCurrentDSNode = 0;
    pList->uDataCount = uDataCount;
    pList->uDataSize = uDataSize;
    pList->uDSNodeCount = uDSNodeCount;
    pList->uMinDSNodeCount = uMinDSNodeCount;
    pList->uUsedDSNodeCount = 1;


    return pList;
}


/**	��̬�ȳߴ��ڴ���������ͷź���

	@param	DSPACELIST *pList -��̬�ȳߴ��ڴ�������ָ��	
	@return	void -��	
*/
void DSpaceList_Destroy(DSPACELIST *pList)
{
    UINT    i;

    if ( pList != NULL )
    {
        for ( i = 0; i < pList->uDSNodeCount; i++ )
        {
            if ( pList->pDSNode[i].pSpList != NULL )
            {
                SpList_Destroy(pList->pDSNode[i].pSpList);
            }
        }
        free(pList->pDSNode);
        free(pList);
    }
}


/**	��̬�ȳߴ��ڴ���������ڴ���亯��

	@param	DSPACELIST *pList -��̬�ȳߴ��ڴ�������ָ��	
	@return	void * -�ɹ����ط��䵽���ڴ棬ʧ�ܷ���NULL	
*/
void *DSpaceList_Alloc(DSPACELIST *pList)
{
    UINT        i;
    void       *pData;
    DSPACENODE *pDSNode;

    pData = NULL;
    pDSNode = pList->pHead;

    if ( pDSNode != NULL )
    {
        pData = SpList_Alloc(pDSNode->pSpList);
        if ( pDSNode->pSpList->uFreeCount == 0 )
        {
            /* ͷ���ڵ�ָ��Ŀ��ÿռ����û�����ɿռ䣬��ͷ���ڵ㵯�� */
            pList->pHead = pDSNode->pNext;
            if ( pList->pHead != NULL )
            {
                pList->pHead->pPrev = NULL;
            }
			/* ����else��俪ʼ��4�д���Ϊ������2006-07-14 */
			else
			{
				pList->pTail = NULL;
			}
            pDSNode->uInListFlag = DSPACENODE_NOT_IN_LIST;
        }
    }
    else
    {
        /* ���Ҳ����·����µĿ��ÿռ����ʹ�� */
        if ( pList->pEmpty == NULL )
        {
            UINT j;
            UINT uLen; 
            DSPACENODE *p;

            uLen = 2 * pList->uDSNodeCount;
            /* ���ÿռ�������꣬��DSPACENODE��Ŀռ�����һ�� */
            p = (DSPACENODE *)malloc( uLen * sizeof(DSPACENODE));
            if ( p == NULL )
            {
                return NULL;
            }

            /* �ͽ����ݸ��Ƶ��·�����ڴ��� */
            for ( i = 0; i < pList->uDSNodeCount; i++ )
            {
                if ( pList->pDSNode[i].pNext != NULL )
                {
                    p[i].pNext = &(p[(pList->pDSNode[i].pNext)->uPos]);
                }
                else
                {
                    p[i].pNext = NULL;
                }
                if ( pList->pDSNode[i].pPrev != NULL )
                {
                    p[i].pPrev = &(p[(pList->pDSNode[i].pPrev)->uPos]);
                }
                else
                {
                    p[i].pPrev = NULL;
                }
                p[i].pSpList = pList->pDSNode[i].pSpList;
                p[i].uInListFlag = pList->pDSNode[i].uInListFlag;
                p[i].uPos = i;
            }
            
            pList->uDSNodeCount = uLen;
            
            free(pList->pDSNode);
            pList->pDSNode = p;
            
            /* ���³�ʼ����������Ӳ��ֵ����� */
            for ( j = i; j < pList->uDSNodeCount-1; j++ )
            {
                pList->pDSNode[j].pSpList = NULL;
                pList->pDSNode[j].uInListFlag = DSPACENODE_NOT_IN_LIST;
                pList->pDSNode[j].pNext = &(pList->pDSNode[j+1]);
                pList->pDSNode[j].pPrev = NULL;
                pList->pDSNode[j].uPos = j;
            }
            pList->pDSNode[j].pSpList = NULL;
            pList->pDSNode[j].pNext = NULL;
            pList->pDSNode[j].uInListFlag = DSPACENODE_NOT_IN_LIST;
            pList->pDSNode[j].pPrev = NULL;
            pList->pDSNode[j].uPos = j;

            pList->pEmpty = &(pList->pDSNode[i]);
        }

        pDSNode = pList->pEmpty;
        pList->pEmpty = pList->pEmpty->pNext;

        pDSNode->pSpList = SpList_Create(pList->uDataCount, 
                                         pList->uDataSize, pDSNode->uPos);
        if ( pDSNode->pSpList != NULL )
        {
            pList->uUsedDSNodeCount++;

            /* ��pDSNode���뵽˫������β�� */
            pDSNode->pNext = NULL;
            pDSNode->pPrev = pList->pTail;
            if ( pList->pTail != NULL )
            {
                pList->pTail->pNext = pDSNode;
            }
            else
            {
                pList->pHead = pDSNode;
            }
            pList->pTail = pDSNode;
            pDSNode->uInListFlag = DSPACENODE_IN_LIST;

            pData = SpList_Alloc(pDSNode->pSpList);
        }
    }
    return pData;
}


/**	��̬�ȳߴ��ڴ���������ڴ��ͷź���

	@param	DSPACELIST *pList -��̬�ȳߴ��ڴ�������ָ��	
	@param	void *pData -Ҫ�ͷŵ��ڴ�����	
	@return	void -��	
*/
void DSpaceList_Free(DSPACELIST *pList, void *pData)
{
    SPNODE      *pNode;
    DSPACENODE  *pDSNode;

    pNode = (SPNODE *)((char *)pData - sizeof(SPNODE));
    pDSNode = &(pList->pDSNode[pNode->uPos]);
    
    /* �ж��Ƿ���Ͻ��������ÿռ���ͷŵ����� */
    if ( pDSNode->pSpList->uFreeCount == pList->uDataCount - 1 /* ���Ӽ�1 2006-07-14 */
        && pList->uUsedDSNodeCount > pList->uMinDSNodeCount )
        //&& pNode->uPos != pList->uCurrentDSNode ) //��ɾ�����������
    {
        /* �ͷŴ˽ڵ�ָ��Ŀ��ÿռ�� */
        SpList_Destroy(pDSNode->pSpList);
        pDSNode->pSpList = NULL;

		if ( pDSNode->uInListFlag == DSPACENODE_IN_LIST )
		{
			/* ��˫��������ɾ���˽ڵ� */
			if ( pDSNode->pPrev != NULL )
			{
				pDSNode->pPrev->pNext = pDSNode->pNext;
			}
			if ( pDSNode->pNext != NULL )
			{
				pDSNode->pNext->pPrev = pDSNode->pPrev;
			}
			if ( pList->pHead == pDSNode ) /* ©��һ���Ⱥ�=��2006-07-14 */
			{
				pList->pHead = pDSNode->pNext;
			}
			if ( pList->pTail == pDSNode )
			{
				pList->pTail = pDSNode->pPrev;
			}
		}
        /* pDSNode���뵽pEmptyΪͷ�ڵ��δʹ�������� */
        pDSNode->pNext = pList->pEmpty;
        pDSNode->pPrev = NULL;
        pDSNode->uInListFlag = DSPACENODE_NOT_IN_LIST;
        pList->pEmpty = pDSNode;


        /* ���µ�ǰʹ�õ�DSPACENODE�����ļ��� */
        pList->uUsedDSNodeCount--;
    }
    else
    {
        SpList_Free(pDSNode->pSpList, pData);                                  
        if ( pDSNode->uInListFlag == DSPACENODE_NOT_IN_LIST )
        {
            /* �������˫������β�� */
            pDSNode->pPrev = pList->pTail;
            pDSNode->pNext = NULL;
            if ( pList->pTail != NULL )
            {
                pList->pTail->pNext = pDSNode;
            }
            else
            {
                pList->pHead = pDSNode;
            }
            pList->pTail = pDSNode;

            pDSNode->uInListFlag = DSPACENODE_IN_LIST;
        }
    }
    return;
}
