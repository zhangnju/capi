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

#include "capiglobal.h"
#include "dmalloc.h"
#include "CLockFreeQueue.h"

#if 0   // Sequential code
void *CLockFreeQueue::PopFront()
{
    SINGLENODE *pNewHead;
    SINGLENODE *pOldHead;
    void     *pData;

    pNewHead = m_pHead->pNext;
    if ( pNewHead == NULL )
    {
        return NULL;
    }

    pData = pNewHead->pData;

    pOldHead = m_Head;
    m_pHead = pNewHead;

    dfree(pOldHead);

    return pData;
}



void CLockFreeQueue::PushBack(void *pData)
{
    SINGLENODE *pNode = (SINGLENODE *)dmalloc(sizeof(SINGLENODE));

    pNode->pNext = NULL;
    pNode->pData = pData;

    m_pTail->pNext = pNode;
    m_pTail = pNode;
}

#endif

CLockFreeQueue::~CLockFreeQueue()
{
    SINGLENODE *pNode;
    SINGLENODE *pTempNode; 

    pNode = m_pHead;

    if ( m_DestroyFunc != NULL )
    {
        while ( pNode != NULL )
        {
            pTempNode = pNode;
            pNode = pNode->pNext;
            (*m_DestroyFunc)(pTempNode->pData);
            delete pTempNode;
        }
    }
    else
    {
        while ( pNode != NULL )
        {
            pTempNode = pNode;
            pNode = pNode->pNext;
            delete pTempNode;
        }
    }
}

/**	CLockFreeQueue���Ӳ�������

	@return	void * - ���ض���ͷ���е�����	
*/
void * CLockFreeQueue::DeQueue()
{
    SINGLENODE    *pHead, *pTail, *pNext;
    void        *pData = NULL;


    for ( ;; )
    {
        pHead = m_pHead;
        pTail = m_pTail;

        pNext = pHead->pNext;
        if ( pHead == m_pHead )
        {
            if ( pHead == pTail )
            {
                // ���洦��������û�����ݵ����
                if ( pNext == NULL )
                {
                    return NULL;
                }
                // ���������̲߳���ڵ����һ������
                AtomicCAS((LONG volatile *)&m_pTail, (LONG)pNext, (LONG)pTail);
            }
            else
            {
                pData = pNext->pData;
                if ( AtomicCAS((LONG volatile *)&m_pHead, (LONG)pNext, (LONG)pHead) )
                {
                    break;
                }
            }
        }
    }
    delete pHead;

    return pData;
}


/**	CLockFreeQueue���Ӳ�������

	@param	void *pData - Ҫ���ӵ�����	
	@return	void - ��	
*/
void CLockFreeQueue::EnQueue(void *pData)
{
    SINGLENODE *pNewNode = new SINGLENODE;
    pNewNode->pData = pData;
    pNewNode->pNext = NULL;

    SINGLENODE *pTail;
    SINGLENODE *pNext;

    for (;;)
    {
        pTail = m_pTail;
        pNext = pTail->pNext;
        if ( m_pTail == pTail )
        {
            if ( pNext == NULL )
            {
                if ( AtomicCAS((LONG volatile *)&(pTail->pNext), (LONG)pNewNode, (LONG)NULL) )
                {
                    break;
                }
            }
            else
            {
                // �����̲߳���������һ������
                AtomicCAS((LONG volatile *)&m_pTail, (LONG)pNext, (LONG)pTail);
            }
        }
    }
    AtomicCAS((LONG volatile *)&m_pTail, (LONG)pNewNode, (LONG)pTail);
}


