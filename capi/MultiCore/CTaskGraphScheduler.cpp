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
#include <stdio.h>
#include <omp.h>
#include "CapiGlobal.h"
#include "CTaskTeam.h"
#include "CTaskGraphScheduler.h"


/**	����ͼ������Ĺ��캯��

    @return	constructor - ��	
*/
CTaskGraphScheduler::CTaskGraphScheduler()
{
    m_pGraphNodeList = NULL;
    m_pLayeredList = NULL;
#ifdef _DEBUG
    printf("CTaskGraphScheduler::CTaskGraphScheduler() called.\n");
#endif
}

/**	����ͼ���������������

    @return	destructor - ��	
*/
CTaskGraphScheduler::~CTaskGraphScheduler()
{
#ifdef _DEBUG
    printf("CTaskGraphScheduler::~CTaskGraphScheduler() called.\n");
#endif
    DestroyLayeredList();
    DestroyGraphNodeList();
}


/**	�ֲ�������ͷź���
    �Ƚ������ڵĸ���TaskTeam�ͷŵ����������������ͷŵ�

	@return	void - ��	
*/
void CTaskGraphScheduler::DestroyLayeredList()
{
    if ( m_pLayeredList != NULL )
    {
        SINGLENODE  *pNode;

        pNode = m_pLayeredList->pHead;

        while ( pNode != NULL )
        {
            CTaskTeam *pTaskTeam = (CTaskTeam *)pNode->pData;

            delete pTaskTeam;

            pNode = pNode->pNext;
        }

        SingleList_Destroy(m_pLayeredList, NULL);
    }
    return;
}


/**	ͼ�Ľڵ�������ͷź���
    ������ͼ��m_GraphNodeList�����и����ڵ��ͷŵ�

	@return	void - ��	
*/
void CTaskGraphScheduler::DestroyGraphNodeList()
{
    if ( m_pGraphNodeList != NULL )
    {
        DOUBLENODE  *pNode;

        pNode = m_pGraphNodeList->pHead;

        while ( pNode != NULL )
        {
            GRAPHNODE   *pGraphNode = (GRAPHNODE *)(pNode->pData);
            if ( pGraphNode != NULL )
            {
                TASK *pTask = (TASK *)pGraphNode->pProperties;
                delete pTask;

                SingleList_Destroy(pGraphNode->pEdgeList, Edge_Destroy);
                delete pGraphNode;
            }
            pNode = pNode->pNext;
        }

        DoubleList_Destroy(m_pGraphNodeList, NULL);
    }
    return;
}


/**	����ͼ��ͼ����ȼ��㺯��
    ���㷽��Ϊ����һ���ͼ�ڵ���������б߶�Ӧ�ڵ���ȼ�1

	@param	GRAPHNODE *pNode - Ҫ������ͼ�ڵ�	
	@return	void - ��	
*/
void CTaskGraphScheduler::DecInDegree(GRAPHNODE *pNode)
{
    EDGE *pEdge;

    if ( pNode == NULL )
    {
        return;
    }

    SINGLELIST  *pList = pNode->pEdgeList;
    SINGLENODE  *pListNode = pList->pHead;

    while ( pListNode != NULL )
    {
        pEdge = (EDGE *)pListNode->pData;
        GRAPHNODE *pTagNode = pEdge->pNode2;
        pTagNode->nMagic -= 1;
    
        pListNode = pListNode->pNext;
    }
}


/**	����ͼ�ķֲ㺯��
    ��һ������ͼ���зֲ㣬�ֺõĲ�����m_pLayeredList��
    ÿ������һ��TASKTEAM�ṹ�����棬
    m_pLayeredList�����е�ÿ���ڵ���һ��TASKTEAMָ��

	@return	void - ��	
*/
void CTaskGraphScheduler::Layered()
{

    if ( m_pLayeredList != NULL )
    {
        DestroyLayeredList();
    }
    m_pLayeredList = SingleList_Create();

    for (;;)
    {
        DOUBLENODE  *pDNode = m_pGraphNodeList->pHead;

        if ( pDNode == NULL )
        {
            break;
        }

        CTaskTeam *pTaskTeam = new CTaskTeam(DEFAULT_ONE_LAYER_TASK_COUNT);

        // Find the node that in-degree is 0, and put to a task team.
        while ( pDNode != NULL )
        {
            GRAPHNODE *pGraphNode = (GRAPHNODE *)pDNode->pData;

            if ( pGraphNode->nMagic == 0 )
            {
                pDNode = DoubleList_PopNode(m_pGraphNodeList, pDNode);

                pTaskTeam->AddTask((TASK *)pGraphNode);
            }
            DOUBLENODE *pTempNode = pDNode;
            pDNode = pDNode->pNext;
            free(pTempNode);
        }

        SingleList_InsertTail(m_pLayeredList, (void *)pTaskTeam);

        int i;
        TASK **ppTaskArray = pTaskTeam->GetTaskArray();
        for ( i = 0; i < pTaskTeam->GetTaskCount(); i++ )
        {
            DecInDegree((GRAPHNODE *)ppTaskArray[i]);
        }
    }

    //��ÿ��CTaskTeam�ڵ�����ڵ���GRAPHNODE���͸�ΪTASK����
    SINGLENODE  *pNode = m_pLayeredList->pHead;
    while ( pNode != NULL )
    {
        CTaskTeam *pTaskTeam = (CTaskTeam *)pNode->pData;

        TASK **ppTaskArray = pTaskTeam->GetTaskArray();
        GRAPHNODE *pGraphNode;
        int i;

        for ( i = 0; i < pTaskTeam->GetTaskCount(); i++ )
        {
            pGraphNode = (GRAPHNODE *)ppTaskArray[i];
            if ( pGraphNode!= NULL )
            {
                ppTaskArray[i] = (TASK *)pGraphNode->pProperties;
                SingleList_Destroy(pGraphNode->pEdgeList, Edge_Destroy);
                delete pGraphNode;
            }
        }
        pNode = pNode->pNext;
    }

    return ;
}

/**	����ͼ�����������������������ӿں�����
    ���һ����������ͼ��

    @param	THREADFUNC TaskFunc - Ҫ��ӵ�����ִ�к���	
    @param	void *pPara - Ҫ��ӵ�����ִ�к����Ĳ���	
    @param	DWORD dwTime - Ҫ��ӵ����������ִ��ʱ��	
	@return	GRAPHNODE * - ���ذ�����������ͼ�ڵ�ָ��	
*/
GRAPHNODE * CTaskGraphScheduler::AddTask(THREADFUNC TaskFunc, void *pPara, DWORD dwTaskTime)
{
    GRAPHNODE   *pGraphNode = new GRAPHNODE;

    TASK        *pTask = new TASK;

    pTask->pArg = pPara;
    pTask->func = TaskFunc;
    pTask->time_consume = dwTaskTime;

    pGraphNode->pProperties = pTask;

    pGraphNode->nMagic = 0;
    pGraphNode->pEdgeList = SingleList_Create();

    if ( m_pGraphNodeList == NULL )
    {
        m_pGraphNodeList = DoubleList_Create();
    }
    
    DoubleList_InsertTail(m_pGraphNodeList, pGraphNode);

    return pGraphNode;
}


/**	����ͼ���������������ͼ������ϵ����������ӿں�����
    �����һ����pSrcNode������pTagNode�������

	@param	GRAPHNODE *pSrcNode - ����ͼ�д���ӱߵ����	
	@param	GRAPHNODE *pTagNode - ����ͼ�д���ӱߵ��յ�	
	@return	void - xxxx	
*/
void CTaskGraphScheduler::AddRelation(GRAPHNODE *pSrcNode, GRAPHNODE *pTagNode)
{
    SINGLELIST *pList = pSrcNode->pEdgeList;

    EDGE *pEdge = new EDGE;

    pEdge->pNode1 = pSrcNode;
    pEdge->pNode2 = pTagNode;

    SingleList_InsertTail(pList, pEdge);

    pTagNode->nMagic += 1;
}



/**	����ͼ��������ִ�����к���������ӿں�����
    ��������ͼ�ֲ㣬����������������е��Ȳ��Ҳ���ִ�С�

	@return	void - ��	
*/
void CTaskGraphScheduler::ExecAllTask()
{
    Layered();

    SINGLENODE  *pNode = m_pLayeredList->pHead;

    while ( pNode != NULL )
    {
        CTaskTeam  *pTaskTeam = (CTaskTeam *)pNode->pData;

        pTaskTeam->Schedule();
        pTaskTeam->Exec();

        pNode = pNode->pNext;
#ifdef _DEBUG
        printf( "Exec one layer task finished.\n");
#endif
    }

    return;
}

