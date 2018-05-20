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

#include <windows.h>
#include "CapiGlobal.h"
#include "SingleList.h"
#include "Graph.h"


/**	�Ƚ�����ָ���С�ĺ���

    @param	void *p1 - Ҫ�Ƚϵ�ָ��1	
    @param	void *p2 - Ҫ�Ƚϵ�ָ��2
    @return	INT - 0��ʾ��ȣ�1��ʾp1>p2, -1��ʾp1<p2	
*/
INT PointerCompare(void *p1, void *p2)
{
    if ( p1 == p2 )
    {
        return 0;
    }
    else if ( (INT)p1 < (INT)p2 )
    {
        return -1;
    }
    else
    {
        return 1;
    }
}


/**	ͼ�Ľڵ㴴������

    @return	GRAPHNODE * - �ɹ����ش�����ͼ�ڵ�ָ�룬ʧ�ܷ���NULL��	
*/
GRAPHNODE * GraphNode_New()
{
    LPGRAPHNODE     pNewNode;
    pNewNode    = (GRAPHNODE *)malloc(sizeof(GRAPHNODE));
    if ( !pNewNode )
    {
        return NULL;
    }
    pNewNode->pProperties = NULL;
    pNewNode->pMagic = NULL;
    pNewNode->nMagic = -1;
    pNewNode->pEdgeList = SingleList_Create();
    if ( NULL == pNewNode->pEdgeList )
    {
        free( pNewNode );
        return NULL;
    }
    return pNewNode;
}


/**	ͼ�Ľڵ��ͷź���

    @param	LPGRAPHNODE pGraphNode - ͼ�ڵ�ָ��	
    @param	DESTROYFUNC GraphNodePropDestroyFunc - ͼ�ڵ������ͷź���	
    @return	void - ��	
*/
void GraphNode_Destroy(LPGRAPHNODE pGraphNode,
                  DESTROYFUNC GraphNodePropDestroyFunc )
{
    if ( NULL == pGraphNode )
    {
        return;
    }

    if ( pGraphNode->pProperties )
    {
        if ( GraphNodePropDestroyFunc )
        {
            (*GraphNodePropDestroyFunc)(pGraphNode->pProperties);
        }
    }

    if ( pGraphNode->pEdgeList != NULL ) 
    {
        /* ���ﲻ��Ҫ�ͷ�����, ��Ϊ���������������б��� */
        SingleList_Destroy(pGraphNode->pEdgeList, NULL);
    }

    free(pGraphNode);
}


/**	ͼ�ڵ����ӱߺ���

    @param	LPGRAPHNODE pGraphNode - ͼ�ڵ�ָ��
    @param	LPEDGE pEdge - ��ָ��	
    @return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
*/
INT GraphNode_AddEdge(LPGRAPHNODE pGraphNode, LPEDGE pEdge)
{
    if ( NULL == pGraphNode )
    {
        return CAPI_FAILED;
    }

    if ( NULL == pEdge )
    {
        return CAPI_FAILED;
    }

    return SingleList_InsertTail(pGraphNode->pEdgeList, pEdge);
}


/**	ͼ�ڵ��ɾ���ߺ���

    @param	LPGRAPHNODE pGraphNode - ͼ�ڵ�ָ��	
    @param	LPEDGE  pEdge - ��ָ��	
    @param	DESTROYFUNC EdgePropDestroyFunc - �������ͷź���	
    @return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT GraphNode_RemoveEdge(LPGRAPHNODE pGraphNode, 
                             LPEDGE  pEdge,
                             DESTROYFUNC EdgePropDestroyFunc)
{
    if ( NULL == pGraphNode )
    {
        return CAPI_FAILED;
    }

    if ( NULL == pEdge )
    {
        return CAPI_FAILED;
    }

    if ( pGraphNode->pEdgeList == NULL ) {
        return CAPI_FAILED;
    }

    return SingleList_Delete(pGraphNode->pEdgeList, pEdge, 
        PointerCompare, EdgePropDestroyFunc);

}


/**	�ߵĴ�������

    @return	EDGE * - �ɹ����ش����ı�ָ�룬ʧ�ܷ���NULL��	
*/
EDGE * Edge_New()
{
    LPEDGE     pEdge;
     
    pEdge = (LPEDGE)malloc( sizeof(EDGE) );
    if ( NULL != pEdge )
    {
        pEdge->distance = 0;
        pEdge->pProperties = NULL;
        pEdge->nMagic = -1;
        pEdge->pNode1 = NULL;
        pEdge->pNode2 = NULL;
    }
    return pEdge;
}


/**	�ߵ��ͷź���

    @param	LPEDGE pEdge - ��ָ��
    @param	DESTROYFUNC EdgePropDestroyFunc - �������ͷź���	
    @return	void - ��	
*/
void Edge_Destroy(LPEDGE pEdge,
                  DESTROYFUNC EdgePropDestroyFunc)
{
    if ( NULL == pEdge )
    {
        return;
    }
    if ( pEdge->pProperties )
    {
        if ( EdgePropDestroyFunc )
        {
            (*EdgePropDestroyFunc)(pEdge->pProperties);
        }
    }
    free(pEdge);
    pEdge = NULL;
}


/**	ͼ�Ĵ�������

    @param	INT nNodeCount - Ҫ������ͼ�����ڵ�����	
    @return	GRAPH * - �ƹ����ش�����ͼָ�룬ʧ�ܷ���NULL��	
*/
GRAPH * Graph_New(INT nNodeCount)
{
    GRAPH   *pGraph;

    if ( nNodeCount <= 0 )
    {
        return NULL;
    }

    pGraph = (GRAPH *)malloc(sizeof(GRAPH));
    if ( NULL == pGraph )
    {
        return NULL;
    }
    pGraph->nNodeCount = nNodeCount;
    pGraph->ppNodeArray = (GRAPHNODE **)malloc(nNodeCount * sizeof(GRAPHNODE *));
    if ( pGraph->ppNodeArray)
    {
        memset(pGraph->ppNodeArray, 0, nNodeCount * sizeof(GRAPHNODE *) );
        pGraph->pProperties = NULL;
        pGraph->pEdgeList = SingleList_Create();
        if ( NULL == pGraph->pEdgeList )
        {
            free(pGraph->ppNodeArray);
            free(pGraph);
            pGraph = NULL;
        }
    }
    else
    {
        free(pGraph);
        pGraph = NULL;
    }

    return pGraph;
}


/**	ͼ���ͷź���

    @param	LPGRAPH pGraph - ͼ��ָ��	
    @param	DESTROYFUNC GraphPropDestroyFunc - ͼ�������ͷź���	
    @param	DESTROYFUNC GraphNodePropDestroyFunc - ͼ�������ͷź���	
    @param	DESTROYFUNC EdgePropDestroyFunc - �ߵ������ͷź���	
    @return	void - ��	
*/
void Graph_Destroy(LPGRAPH pGraph,
                   DESTROYFUNC GraphPropDestroyFunc,
                   DESTROYFUNC GraphNodePropDestroyFunc,
                   DESTROYFUNC EdgePropDestroyFunc)
{
    if ( pGraph )
    {
        if ( pGraph->ppNodeArray )
        {
            INT i;
            for ( i = 0; i < pGraph->nNodeCount; i++ )
            {
                if ( pGraph->ppNodeArray[i] != NULL )
                {
                    GraphNode_Destroy(pGraph->ppNodeArray[i], 
                        GraphNodePropDestroyFunc );
                    pGraph->ppNodeArray[i] = NULL;
                }
            }
            free( pGraph->ppNodeArray );
            pGraph->ppNodeArray = NULL;
        }
        if ( pGraph->pProperties )
        {
            if ( GraphPropDestroyFunc )
            {
                (*GraphPropDestroyFunc)(pGraph->pProperties);
            }
            pGraph->pProperties = NULL;
        }

        if ( pGraph->pEdgeList )
        {
            SINGLENODE  *pNode;
            pNode = pGraph->pEdgeList->pHead;
            while ( pNode != NULL ) 
            {
                SINGLENODE	*pFreeNode;
                
                /* reset the pointers here so we don't lose them */
                pFreeNode = pNode;
                pNode = pNode->pNext;
                
                /* do data-specific purge. */
                Edge_Destroy( pFreeNode->pData, EdgePropDestroyFunc );
                
                /* free memory */
                free( pFreeNode );
            }
            free( pGraph->pEdgeList );
            pGraph->pEdgeList = NULL;
        }

        free(pGraph);
        pGraph = NULL;
    }
}


/**	��ȡָ��ͼ�Ķ������

    @param	GRAPH *pGraph -ͼ��ָ��	
    @return	INT -�ɹ�ͼ�Ķ������, ʧ�ܷ���-1.
*/
INT Graph_GetNodeCount(GRAPH *pGraph)
{
    if ( NULL == pGraph )
    {
        return -1;
    }
    return pGraph->nNodeCount;
}


/**	��ȡָ��ͼ�ıߵ�����

    @param	GRAPH *pGraph -ͼ��ָ��	
    @return	INT -�ɹ�����ͼ�ıߵ�����, ʧ�ܷ���-1.	
*/
INT Graph_GetEdgeCount(GRAPH *pGraph)
{
    INT     nEdgeCount;

    nEdgeCount = 0;
    if ( NULL == pGraph )
    {
        return -1;
    }
    
    nEdgeCount = pGraph->pEdgeList->uCount;

    return nEdgeCount;
}

/**	����ͼ�������ڵ������·������
        �������ÿ���ڵ��pMagic������ŵ�Դ�ڵ����·������һ���ڵ�ָ��
        nMagic������ŵ�Դ�ڵ�����·�����ܾ���

    @param	LPGRAPH pGraph - Ҫ�����ͼ	
    @param	LPGRAPHNODE pSrcNode - Դ�ڵ�	
    @param	LPGRAPHNODE pTagNode - Ŀ��ڵ�	
    @return	DISTANCE - GRAPH_ERROR ��ʾ��������
                           GRAPH_NOT_PATH ��ʾû��·��
                           �ɹ�ʱ������������·�����ܾ���
*/
DISTANCE Graph_GetShortestPath(LPGRAPH pGraph, LPGRAPHNODE pSrcNode, 
                                LPGRAPHNODE pTagNode)
{
    INT       i, j, k;   /* ѭ������ */
    INT       nCalCount; /* ������¼�Ѿ�����õĵ�pSrcNode���·���Ľڵ����� */
    GRAPHNODE **ppNode;  /* ��������Ѿ�����õĵ�pSrcNode���·���Ľڵ����� */
    DISTANCE  nTagDis;   /* ��������pTagNode��pSrcNode���·���ľ���ֵ */

    nTagDis = -1;  /* ��ʼ��Ϊ-1, ����ʼʱpTagNode��pSrcNode��û�м����·�� */

    /* ����У�� */
    if ( NULL == pGraph || NULL == pSrcNode || NULL == pTagNode )
    {
        return GRAPH_ERROR;         
    }

    ppNode = (GRAPHNODE **)malloc(pGraph->nNodeCount * sizeof(GRAPHNODE *));
    if ( NULL == ppNode )
    {
        return GRAPH_ERROR;
    }
    memset(ppNode, 0, pGraph->nNodeCount * sizeof(GRAPHNODE *) );

    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        pGraph->ppNodeArray[i]->nMagic = -1; /* ��ʼ��Ϊ-1��ʾ��û�м���� */
        pGraph->ppNodeArray[i]->pMagic = NULL;
    }

    ppNode[0] = pSrcNode;
    ppNode[0]->nMagic = 0; /*��ʼ��Ϊ0��ͬʱҲ��ʾpSrcNode��pSrcNode�ľ���Ϊ0*/
    ppNode[0]->pMagic = NULL;
    nCalCount = 1;         /* ��ʼ��Ϊ1����ʾpSrcNode��1���ڵ��Ѿ��������� */

    /* k��1��ʼѭ������ΪppNode[0]�Ѿ�������� */
    for ( k = 1; k < pGraph->nNodeCount; k++ ) 
    {
        /* nTotalDis������������δ����ڵ��е�pSrcNode�ڵ���̾���ľ���ֵ */
        DISTANCE    nTotalDis; 
        
        /* pSNode������ʾĿ��ڵ㵽pSrcNode���·���Ϻ�Ŀ��ڵ����ڵĽڵ� */
        GRAPHNODE   *pSNode;   
        
        /* pTNode������ʾ����δ����ڵ��е�pSrcNode�ڵ���̾���Ľڵ� */
        GRAPHNODE   *pTNode;   

        pSNode  = NULL;
        pTNode  = NULL;
        nTotalDis = 0;

        for ( i = 0; i < nCalCount; i++ )
        {
            DISTANCE     nMinDis;
            /* ������ʾ����δ����ڵ㵽ĳ������õĽڵ����̾�����Ǹ��ڵ� */
            GRAPHNODE  * pShortNode; 
            SINGLENODE * pNode;
            SINGLELIST * pList;

            nMinDis = 0;
            pShortNode = NULL;

            for ( j = 0; j < pGraph->nNodeCount; j++ )
            {
                if ( pGraph->ppNodeArray[j]->nMagic != -1 ) 
                {
                    /* �ڵ��Ѿ��������� */
                    continue;
                }

                pList = pGraph->ppNodeArray[j]->pEdgeList;

                pNode = pList->pHead;

                while ( pNode != NULL )
                {
                    EDGE    *pEdge = (EDGE *)pNode->pData;
                    if ( pEdge->pNode1 == ppNode[i] 
                        || pEdge->pNode2 == ppNode[i] )
                    {
                        if ( nMinDis == 0 )
                        {
                            nMinDis = ppNode[i]->nMagic + pEdge->distance;
                            pShortNode = pGraph->ppNodeArray[j];
                        }
                        else
                        {
                            if (nMinDis > ppNode[i]->nMagic + pEdge->distance)
                            {
                                nMinDis = ppNode[i]->nMagic + pEdge->distance;
                                pShortNode = pGraph->ppNodeArray[j];
                            }
                        }
                        break; /* ���ھ����ҵ���Ӧ�ڵ��Ӧ��ֹ���� */
                    }
                    pNode = pNode->pNext;
                } /* while ( p ) */
            } /* for ( j = 0 */
            if ( nTotalDis == 0 )
            {
                nTotalDis = nMinDis; /* ���³�ʼ��nTotaldis */
                pTNode = pShortNode;
                pSNode = ppNode[i];
            }
            else
            {
                if ( nTotalDis > nMinDis )
                {
                    nTotalDis = nMinDis;
                    pTNode = pShortNode;
                    pSNode = ppNode[i];
                }
            }
        } /* for ( i = 0 */
        if ( NULL != pTNode )
        {
            pTNode->nMagic = nTotalDis;
            pTNode->pMagic = pSNode; /* ��pMagic�д�����·���ϵ����ڽڵ� */
            if ( pTNode == pTagNode )
            {
                /* �Ѿ����㵽��pTagNode��pSrcNode�����·�� */
                nTagDis = nTotalDis;
                break;
            }
        }
        
        /* �Ѿ��ҳ���һ����pSrcNode�����·��pTNode, 
         * ��һ��ѭ����Ҫ����ppNode�е����� 
         */
        ppNode[nCalCount] = pTNode;
        nCalCount++;
    } /* for ( k = 0 */
    free(ppNode);
    return nTagDis; /* ����Ŀ��ڵ㵽Դ�ڵ�����·�� */
}

/**	����ͼ�������ڵ������·������
    �������ÿ���ڵ��pMagic��������ŵ�Դ�ڵ����·������һ���ڵ�ָ��
    nMagic������ŵ�Դ�ڵ�����·�����ܾ���

    @param	LPGRAPH pGraph - Ҫ�����ͼ	
    @param	LPGRAPHNODE pSrcNode - Դ�ڵ�	
    @param	LPGRAPHNODE pTagNode - Ŀ��ڵ�	
    @return	DISTANCE - GRAPH_ERROR ��ʾ��������
                           GRAPH_NOT_PATH ��ʾû��·��
                           �ɹ�ʱ������������·�����ܾ���
*/
DISTANCE Graph_GetShortestPath2(LPGRAPH pGraph, LPGRAPHNODE pSrcNode, 
                                 LPGRAPHNODE pTagNode)
{
    INT         i, k;    /* ѭ������ */
    INT         nCalCount;/* ������¼�Ѿ�����õĵ�pSrcNode���·���Ľڵ�����*/
    GRAPHNODE   **ppNode; /* ��������Ѿ�����õĵ�pSrcNode���·���Ľڵ�����*/
    DISTANCE    nTagDis;  /* ��������pTagNode��pSrcNode���·���ľ���ֵ */

    nTagDis = -1;  /* ��ʼ��Ϊ-1, ����ʼʱpTagNode��pSrcNode��û�м����·�� */

    /* ����У�� */
    if ( NULL == pGraph || NULL == pSrcNode || NULL == pTagNode )
    {
        return GRAPH_ERROR;         
    }

    ppNode = (GRAPHNODE **)malloc(pGraph->nNodeCount * sizeof(GRAPHNODE *));
    if ( NULL == ppNode )
    {
        return GRAPH_ERROR;
    }
    memset(ppNode, 0, pGraph->nNodeCount * sizeof(GRAPHNODE *) );

    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        pGraph->ppNodeArray[i]->nMagic = -1; /* ��ʼ��Ϊ-1��ʾ��û�м���� */
        pGraph->ppNodeArray[i]->pMagic = NULL;
    }

    ppNode[0] = pSrcNode;
    ppNode[0]->nMagic = 0; /*��ʼ��Ϊ0��ͬʱҲ��ʾpSrcNode��pSrcNode�ľ���Ϊ0*/
    ppNode[0]->pMagic = NULL;
    nCalCount = 1;           /* ��ʼ��Ϊ1����ʾpSrcNode��1���ڵ��Ѿ��������� */

    /* k��1��ʼѭ������ΪppNode[0]�Ѿ�������� */
    for ( k = 1; k < pGraph->nNodeCount; k++ ) 
    {
        /* ������������δ����ڵ��е�pSrcNode�ڵ���̾���ľ���ֵ */
        DISTANCE    nTotalDis;  
        
        /* ������ʾĿ��ڵ㵽pSrcNode���·���Ϻ�Ŀ��ڵ����ڵĽڵ� */
        GRAPHNODE   *pSNode;    
        
        /* ������ʾ����δ����ڵ��е�pSrcNode�ڵ���̾���Ľڵ� */
        GRAPHNODE   *pTNode;    

        pSNode  = NULL;
        pTNode  = NULL;
        nTotalDis = 0;

        for ( i = 0; i < nCalCount; i++ )
        {
            /* ������ʾ����δ����ڵ㵽ĳ������õĽڵ����̾�����Ǹ��ڵ� */
            GRAPHNODE  * pShortNode; 
            SINGLENODE * pNode;
            SINGLELIST * pList;
            DISTANCE     nMinDis;

            nMinDis = GRAPH_MAX_DISTANCE;
            pShortNode = NULL;

            pList = ppNode[i]->pEdgeList;

            pNode = pList->pHead;

            while ( pNode != NULL )
            {
                EDGE        *pEdge;
                GRAPHNODE   *pGraphNode;
                pEdge = (EDGE *)pNode->pData;
                if ( pEdge->pNode1 == ppNode[i] )
                {
                    pGraphNode = pEdge->pNode2;
                }
                else
                {
                    pGraphNode = pEdge->pNode1;
                }
                if ( pGraphNode->nMagic != -1 )
                {
                    pNode = pNode->pNext;
                    continue;
                }

                if ( nMinDis > ppNode[i]->nMagic + pEdge->distance )
                {
                    nMinDis = ppNode[i]->nMagic + pEdge->distance;
                    pShortNode = pGraphNode;
                }

                pNode = pNode->pNext;
            }


            if ( nTotalDis == 0 )
            {
                nTotalDis = nMinDis; /* ���³�ʼ��nTotaldis */
                pTNode = pShortNode;
                pSNode = ppNode[i];
            }
            else
            {
                if ( nTotalDis > nMinDis )
                {
                    nTotalDis = nMinDis;
                    pTNode = pShortNode;
                    pSNode = ppNode[i];
                }
            }
        } /* for ( i = 0 */
        if ( NULL != pTNode )
        {
            pTNode->nMagic = nTotalDis;
            pTNode->pMagic = pSNode; /* ��pMagic�д�����·���ϵ����ڽڵ� */
            if ( pTNode == pTagNode )
            {
                /* �Ѿ����㵽��pTagNode��pSrcNode�����·�� */
                nTagDis = nTotalDis;
                break;
            }
        }
        
        /* �Ѿ��ҳ���һ����pSrcNode�����·��pTNode, 
         * ��һ��ѭ����Ҫ����ppNode�е����� 
         */
        ppNode[nCalCount] = pTNode;
        nCalCount++;
    } /* for ( k = 0 */
    free(ppNode);
    return nTagDis; /* ����Ŀ��ڵ㵽Դ�ڵ�����·�� */
}






/**	��ͼ���������������
        �������ÿ���ڵ��pMagic��������Ÿ��ڵ�ָ��
        �ڵ��nMagic���������DFS����
        �ߵ�nMagic����������Ƿ�ʹ�õı�־, �������, ��־ΪGRAPH_EDGE_SEARCHED
        �ı߹���һ��������, ���ĸ��ڵ�Ϊ�������

    @param	LPGRAPH pGraph - Ҫ������ͼ	
    @param	LPGRAPHNODE pOrgNode - Ҫ���������, Ϊͼ�е�ĳ������	
    @param	LPEDGE  pTagEdge - Ҫ������ָ���ı�, ���������˱ߺ�ֹͣ����
                                   ���ΪNULL��һֱ������ͼ��ȫ��������
    @param	EDGEDUMPFUNC EdgeDumpFunc - �ߵĻص�����, ΪNULL��ִ�лص�
    @return	INT - 0 ��ʾʧ�ܣ�1��ʾ�������, 2��ʾ������ָ����pTagEdge.	
*/
INT Graph_DepthFirstSearch(LPGRAPH pGraph, 
                           LPGRAPHNODE pOrgNode,
                           LPEDGE      pTagEdge,
                           EDGEDUMPFUNC EdgeDumpFunc)
{
    LPGRAPHNODE     pCurrentNode;
    INT             nDfsCode;
    INT             i;
    SINGLENODE      *pListNode;
    EDGE            *pEdge;
    INT             nControlFlag;

    if ( NULL == pGraph || NULL == pOrgNode )
    {
        return CAPI_FAILED;
    }

    /* ��ʼ������ */
    nControlFlag = 0;
    nDfsCode = 0;
    pCurrentNode = NULL;

    /* ��ʼ�����нڵ��DFS����Ϊ0�Լ����ڵ�ָ��Ϊ�� */
    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        /* ���㷨�У�nMagic��������¼DFS���� */
        pGraph->ppNodeArray[i]->nMagic = 0;    
        /* ���㷨�У�pMagic��������¼���ڵ� */
        pGraph->ppNodeArray[i]->pMagic = NULL; 
    }

    /* ��ʼ�����еı�Ϊ0��ʾδ�ù���
     * ע��pEdge->nMagicΪGRAPH_EDGE_NOT_SEARCHʱ��ʾ��δ�ù���
     * ΪGRAPH_EDGE_SEARCHED��ʾ�ѱ������� 
     */
    pListNode = pGraph->pEdgeList->pHead;

    while ( pListNode != NULL )
    {
        pEdge = (EDGE *)pListNode->pData;
        /* ��ʼ��nMagicΪGRAPH_EDGE_NOT_SEARCH��ʾ�˱�δ�������� */
        pEdge->nMagic = GRAPH_EDGE_NOT_SEARCH; 
        pListNode = pListNode->pNext;
    }

    pCurrentNode = pOrgNode;

    /* �����нڵ���ھ��б�ǰλ�÷���������ͷ���Ա��ں����ͷ���� */
    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        SingleList_EnumBegin(pGraph->ppNodeArray[i]->pEdgeList);
    }

    for ( ; ; )
    {
        void  *pData;
        if ( nControlFlag == 0 )
        {
            /* ����2�����������DFS���� */
            nDfsCode++;
            pCurrentNode->nMagic = nDfsCode;
        }
        else
        {
            nControlFlag = 0;
        }

        pEdge = NULL;
        /* ����3������δ�ù��Ĺ����� */
        while ((pData = SingleList_EnumNext(pCurrentNode->pEdgeList)) != NULL)
        {         
            pEdge = (EDGE *)pData;

            if ( pEdge->nMagic == GRAPH_EDGE_NOT_SEARCH )
            {
                break;
            }
        }

        if ( pData == NULL )
        {
            pEdge = NULL;
        }

        if ( NULL != pEdge ) /* �ж��Ƿ��ҵ�δ�ù��ı� */
        {
            GRAPHNODE   *pNextNode;

            /* ����4���Ѿ��ҵ���һ��δ�ù��Ĺ����� */

            /* ȡ����ǰ�ߵ����ڽڵ�ŵ�pNextNode�� */
            if ( pEdge->pNode1 == pCurrentNode )
            {
                pNextNode = pEdge->pNode2;
            }
            else
            {
                pNextNode = pEdge->pNode1;
            }

            /* �ж����ڽڵ��DFS�����Ƿ�Ϊ0 */
            if ( pNextNode->nMagic == 0 )
            {
                pEdge->nMagic = GRAPH_EDGE_SEARCHED; /* ���˱߱�ʾΪ�ù� */

                /* ����ǰ�ڵ���Ϊ�������Ľڵ�ĸ��ڵ� */
                pNextNode->pMagic = pCurrentNode; 
                /* ����ǰ�ڵ�ָ��ָ�������������ڽڵ� */
                pCurrentNode = pNextNode;   

                nControlFlag = 0;

                if ( EdgeDumpFunc )
                {
                    (*EdgeDumpFunc)(pEdge);
                }

                continue;   /* ת����2 */
            }
            else
            {
                /* �˱��Ѿ��������������������� */
                pEdge->nMagic = GRAPH_EDGE_NOT_TREE; 

                if ( EdgeDumpFunc )
                {
                    (*EdgeDumpFunc)(pEdge);
                }

                nControlFlag = 1;
                /* ת����3 */
            }

            if ( pEdge == pTagEdge)
            {
                return 2;
            }
        }
        else
        {
            /* ����5�������ǰ�ڵ����Ϊ1������pOrgNode�����Ѿ������������ֹ*/
            if ( pCurrentNode->nMagic == 1 )
            {
                break;
            }

            /* ����6�����ݵ����ڵ�ת����3�������� */
            pCurrentNode = pCurrentNode->pMagic;

            nControlFlag = 1;
            continue; /* ת����3 */
        }
    }
    return 1;
}



/**	ͼ�Ŀ��������������
        �������, �ڵ��nMagic�������BFS����
        �ڵ��pMagic���������һ�����������Ľڵ��ָ��, �����pMagic���Եõ�������
        �ڵ�˳��, �ߵ�nMagic����������Ƿ������ı�־, ��־ΪGRAPH_EDGE_SEARCHED
        �ı߹���һ��������, ���ĸ��ڵ�Ϊ�������

    @param	LPGRAPH pGraph - Ҫ������ͼ	
    @param	LPGRAPHNODE pOrgNode - Ҫ���������	
    @param	LPGRAPHNODE pTagNode - Ҫ������Ŀ��ڵ�	
    @param	GRAPHNODEDUMPFUNC GraphNodeDumpFunc - ����ص�����	
    @param	EDGEDUMPFUNC EdgeDumpFunc - �߻ص�����	
    @return	INT - 0��ʾʧ��, 1��ʾ�������, 2��ʾ������ָ��Ŀ��ڵ�pTagNode	
*/
INT Graph_BreadFirstSearch(LPGRAPH pGraph, 
                       LPGRAPHNODE pOrgNode,
                       LPGRAPHNODE pTagNode,
                       GRAPHNODEDUMPFUNC GraphNodeDumpFunc,
                       EDGEDUMPFUNC EdgeDumpFunc)
{
    INT         nBfsCode;       /* BFS���� */
    GRAPHNODE   *pCurrentNode;  /* ��¼��ǰҪ�����Ľڵ� */
    GRAPHNODE   *pLastNode;     /* ��¼���һ��Ҫ�����Ľڵ� */
    INT         i;
    SINGLENODE    *pTmpNode;

    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        /* nMagic�ڴ��㷨����������¼�ڵ��BFS���� */
        pGraph->ppNodeArray[i]->nMagic = -1;  /* ��ʼ��Ϊ-1��ʾ��û�б��� */
        
        /* pMagic�ڴ��㷨��������������������δ�Ա��ڵ���������Ľڵ��γ�����*/
        pGraph->ppNodeArray[i]->pMagic = NULL;  
    }

    pTmpNode = pGraph->pEdgeList->pHead;
    while ( NULL != pTmpNode )
    {
        EDGE    *pEdge;
        pEdge = (EDGE *)pTmpNode->pData;
        pEdge->nMagic = GRAPH_EDGE_NOT_SEARCH; 
        pTmpNode = pTmpNode->pNext;
    }


    nBfsCode = 0;               /* ��ʼ��Ϊ0 */
    pCurrentNode = pOrgNode;
    pCurrentNode->nMagic = nBfsCode;  /* nMagic������¼BFS���� */

    pLastNode = pOrgNode;

    while ( NULL != pCurrentNode )
    {
        SINGLENODE    *pListNode;
        nBfsCode = pCurrentNode->nMagic + 1;

        pListNode = pCurrentNode->pEdgeList->pHead;
        while ( NULL != pListNode )
        {
            EDGE        *pEdge;
            GRAPHNODE   *pNode;

            pEdge = (EDGE *)pListNode->pData;
            
            pListNode = pListNode->pNext;

            if ( pEdge->nMagic != GRAPH_EDGE_NOT_SEARCH )
            {
                /* ���Ѿ����������� */
                continue;
            }

            if ( pEdge->pNode2 == pCurrentNode )
            {
                pNode = pEdge->pNode1;
            }
            else
            {
               pNode = pEdge->pNode2;
            }
            
            if ( pNode->nMagic == -1 )  /* ����-1��ʾ�ڵ㻹δ���� */
            {
                pNode->nMagic = nBfsCode;

                pLastNode->pMagic = pNode;
                pLastNode = pNode;

                pEdge->nMagic = GRAPH_EDGE_SEARCHED;

                if ( GraphNodeDumpFunc )
                {
                    /* �Զ���ִ�лص����� */
                    (*GraphNodeDumpFunc)(pNode);
                }
                if ( pNode == pTagNode )
                {
                    return 2;
                }
            }
            else
            {
                pEdge->nMagic = GRAPH_EDGE_NOT_TREE;
            }
            /* ���ñߵĻص����� */
            if ( EdgeDumpFunc )
            {
                (*EdgeDumpFunc)(pEdge);
            }

        }
        /* ����ǰ�ڵ�ָ����һ��δ�����ڵ� */
        pCurrentNode = pCurrentNode->pMagic;
    }
    return 1;
}

/**	����ȽϺ���

    @param	void *p1 - Ҫ�Ƚϵı�ָ��1	
    @param	void *p2 - Ҫ�Ƚϵı�ָ��2	
    @return	INT - 1��ʾp1�ľ������p2�ľ���
                  0��ʾp1�ľ������p2�ľ���
                  -1��ʾp1�ľ���С��p2�ľ���
*/
INT DistanceCompare(void *p1, void *p2)
{
    EDGE    *pEdge1;
    EDGE    *pEdge2;

    pEdge1 = (EDGE *)p1;
    pEdge2 = (EDGE *)p2;

    if ( pEdge1->distance > pEdge2->distance )
    {
        return 1;
    }
    else if ( pEdge1->distance < pEdge2->distance )
    {
        return -1;
    }
    return 0;
}

/**	ͼ�ı��������������б�ʹ�ù鲢�����㷨��������

    @param	LPGRAPH pGraph - ͼָ��	
    @return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ�ܡ�	
*/
INT Graph_SortEdgeInOrder(LPGRAPH pGraph)
{
    if ( NULL == pGraph )
    {
        return CAPI_FAILED;
    }
    return SingleList_MergeSort(pGraph->pEdgeList, DistanceCompare, 32);
}


/**	����Kruskal�㷨����С������
        ���������,�ڵ��nMagic������¼��������
        �������, nMagicΪGRAPH_EDGE_SEARCHED�ı����һ����С������

    @param	LPGRAPH pGraph - Ҫ����С��������ͼ	
    @return	INT - CAPI_FAILED��ʾʧ��, 1��ʾ�ɹ�	
*/
INT Graph_GetMinTree(LPGRAPH pGraph)
{
    SINGLENODE    *pEdgeListNode;
    INT         i;
    INT         *pNodeNo; /* ������¼���нڵ㸸�ڵ�ű��ڽ��гɻ����ж� */

    if ( NULL == pGraph )
    {
        return CAPI_FAILED;
    }

    pNodeNo = (INT *)malloc(pGraph->nNodeCount * sizeof(INT));
    if ( NULL == pNodeNo )
    {
        return CAPI_FAILED;
    }

    /* ��ʼ�����нڵ��nMagicΪ�ڵ���, ���ڵ��Ϊ-1��ʾ��û�м��뵽�������� */
    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        pGraph->ppNodeArray[i]->nMagic = i;
        pNodeNo[i] = -1; /* ��ʼ�����нڵ�ĸ��ڵ��Ϊ-1��ʾ��û�м���� */
    }

    Graph_SortEdgeInOrder(pGraph);
    
    /* ��ʼ�����бߵ�nMagicΪGRPAH_EDGE_NOT_SEARCH */
    pEdgeListNode = pGraph->pEdgeList->pHead;
    while ( NULL != pEdgeListNode )
    {
        EDGE        *pEdge;
        pEdge = (EDGE *)pEdgeListNode->pData;

        pEdge->nMagic = GRAPH_EDGE_NOT_SEARCH;

        pEdgeListNode = pEdgeListNode->pNext;

    }

    pEdgeListNode = pGraph->pEdgeList->pHead;
    while ( NULL != pEdgeListNode )
    {
        EDGE        *pEdge;
        INT         bnf;    /* һ���߿�ʼ����ĸ��ڵ��� */
        INT         edf;    /* һ���߽�������ĸ��ڵ��� */

        pEdge = (EDGE *)pEdgeListNode->pData;

        bnf = pEdge->pNode1->nMagic; /* nMagic��ʾ�ڵ��� */
        while ( pNodeNo[bnf] != -1 )
        {
            bnf = pNodeNo[bnf];
        }

        edf = pEdge->pNode2->nMagic; /* nMagic��ʾ�ڵ��� */
        while ( pNodeNo[edf] != -1 )
        {
            edf = pNodeNo[edf];
        }

        if ( bnf != edf )
        {
            /* û�гɻ�, �����Ϊedf�Ľڵ���Ϊ���Ϊbnf�Ľڵ�ĸ��ڵ� */
            pNodeNo[bnf] = edf; 

            /* ���˱߱�ʾΪ�Ѿ�������, Ҳ����˵�������������� */
            pEdge->nMagic = GRAPH_EDGE_SEARCHED;
        }
        else
        {
            /* �Ѿ��ɻ��ɻ�, ���˱߱�ʾΪ������������ */
            pEdge->nMagic = GRAPH_EDGE_NOT_TREE;
        }

        pEdgeListNode = pEdgeListNode->pNext;
    } /* while */ 
    
    free(pNodeNo);
    return 1;
}

