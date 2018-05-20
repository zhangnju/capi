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


/**	任务图调度类的构造函数

    @return	constructor - 无	
*/
CTaskGraphScheduler::CTaskGraphScheduler()
{
    m_pGraphNodeList = NULL;
    m_pLayeredList = NULL;
#ifdef _DEBUG
    printf("CTaskGraphScheduler::CTaskGraphScheduler() called.\n");
#endif
}

/**	任务图调度类的析构函数

    @return	destructor - 无	
*/
CTaskGraphScheduler::~CTaskGraphScheduler()
{
#ifdef _DEBUG
    printf("CTaskGraphScheduler::~CTaskGraphScheduler() called.\n");
#endif
    DestroyLayeredList();
    DestroyGraphNodeList();
}


/**	分层链表的释放函数
    先将链表内的各个TaskTeam释放掉，并将整个链表释放掉

	@return	void - 无	
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


/**	图的节点链表的释放函数
    将任务图内m_GraphNodeList链表中各个节点释放掉

	@return	void - 无	
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


/**	任务图子图的入度计算函数
    计算方法为将上一层的图节点出发的所有边对应节点入度减1

	@param	GRAPHNODE *pNode - 要操作的图节点	
	@return	void - 无	
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


/**	任务图的分层函数
    将一个任务图进行分层，分好的层存放在m_pLayeredList中
    每个层由一个TASKTEAM结构来保存，
    m_pLayeredList链表中的每个节点存放一个TASKTEAM指针

	@return	void - 无	
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

    //将每个CTaskTeam内的任务节点由GRAPHNODE类型改为TASK类型
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

/**	任务图调度器的添加任务函数（对外接口函数）
    添加一个任务到任务图中

    @param	THREADFUNC TaskFunc - 要添加的任务执行函数	
    @param	void *pPara - 要添加的任务执行函数的参数	
    @param	DWORD dwTime - 要添加的任务所需的执行时间	
	@return	GRAPHNODE * - 返回包含添加任务的图节点指针	
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


/**	任务图调度器的添加任务图依赖关系函数（对外接口函数）
    即添加一条从pSrcNode出发到pTagNode的有向边

	@param	GRAPHNODE *pSrcNode - 任务图中待添加边的起点	
	@param	GRAPHNODE *pTagNode - 任务图中待添加边的终点	
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



/**	任务图调度器的执行所有函数（对外接口函数）
    负责将任务图分层，并将各层任务组进行调度并且并行执行。

	@return	void - 无	
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

