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
#include "CapiCommon.h"
#include "SingleList.h"
#include "Graph.h"
#include "Parallel_Graph.h"

#define   MIN_ITERATOR_NUM      100

void SearchNextVertex(LPGRAPH pGraph, GRAPHNODE **ppNode, INT nCount,
                            GRAPHNODE **ppSNode, DISTANCE *pnDis);


/**	����ͼ�������ڵ������·������
    �������ÿ���ڵ��pMagic��������ŵ�Դ�ڵ����·������һ���ڵ�ָ��
    nMagic������ŵ�Դ�ڵ�����·�����ܾ���

    @param	LPGRAPH pGraph - Ҫ�����ͼ	
    @param	LPGRAPHNODE pSrcNode - Դ�ڵ�	
    @param	LPGRAPHNODE pTagNode - Ŀ��ڵ�	
    @return	DISTANCE - GRAPH_ERROR ��ʾ��������GRAPH_NOT_PATH ��ʾû��·��
    �ɹ�ʱ������������·�����ܾ���
*/
DISTANCE Parallel_GetShortestPath(LPGRAPH pGraph, LPGRAPHNODE pSrcNode, 
                                 LPGRAPHNODE pTagNode)
{
    INT         i, k;    /* ѭ������ */
    GRAPHNODE   **ppNode; /* ��������Ѿ�����õĵ�pSrcNode���·���Ľڵ�����*/
    DISTANCE    nTagDis;  /* ��������pTagNode��pSrcNode���·���ľ���ֵ */
    GRAPHNODE   **ppSNode; /* ����������ʱ��� */
    DISTANCE    *pnDis;

    nTagDis = -1;  /* ��ʼ��Ϊ-1, ����ʼʱpTagNode��pSrcNode��û�м����·�� */

    /* ����У�� */
    if ( NULL == pGraph || NULL == pSrcNode || NULL == pTagNode )
    {
        return GRAPH_ERROR;         
    }
    pnDis = (DISTANCE *)malloc(sizeof(DISTANCE) * pGraph->nNodeCount);
    if ( NULL == pnDis )
    {
        return GRAPH_ERROR;
    }

    ppSNode = (GRAPHNODE **)malloc(pGraph->nNodeCount * sizeof(GRAPHNODE *));
    if ( NULL == ppSNode )
    {
        free(pnDis);
        return GRAPH_ERROR;
    }
    memset(ppSNode, 0, pGraph->nNodeCount * sizeof(GRAPHNODE *) );


    ppNode = (GRAPHNODE **)malloc(pGraph->nNodeCount * 2 * sizeof(GRAPHNODE *));
    if ( NULL == ppNode )
    {
        free(pnDis);
        free(ppSNode);
        return GRAPH_ERROR;
    }

#pragma omp parallel for num_threads(dtn(pGraph->nNodeCount, MIN_ITERATOR_NUM))
    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        pGraph->ppNodeArray[i]->nMagic = -1; /* ��ʼ��Ϊ-1��ʾ��û�м���� */
        pGraph->ppNodeArray[i]->pMagic = NULL;
    }

    ppSNode[0] = pSrcNode;
    ppSNode[0]->nMagic = 0; /*��ʼ��Ϊ0��ͬʱҲ��ʾpSrcNode��pSrcNode�ľ���Ϊ0*/
    ppSNode[0]->pMagic = NULL;

    /* k��1��ʼѭ������ΪppNode[0]�Ѿ�������� */
    for ( k = 1; k < pGraph->nNodeCount; k++ ) 
    {
        /* ������������δ����ڵ��е�pSrcNode�ڵ���̾���ľ���ֵ */
        DISTANCE    nTotalDis;  

        /* ������ʾ����δ����ڵ��е�pSrcNode�ڵ���̾���Ľڵ� */
        GRAPHNODE   *pTNode;    

        pTNode  = NULL;
        nTotalDis = GRAPH_MAX_DISTANCE;

        SearchNextVertex(pGraph, ppSNode, k, ppNode, pnDis);


        // ���k�ϴ�ʱ������k����500ʱ���Ը�Ϊʹ�ò�������
        INT index = -1;
        for ( i = 0; i < k; i++ )
        {
            if ( nTotalDis > pnDis[i] )
            {
                nTotalDis = pnDis[i];
                index = i;
            }
        }

        if ( index != -1 )
        {
            pTNode = ppNode[index * 2];

            pTNode->nMagic = nTotalDis;
            pTNode->pMagic = ppNode[index * 2 + 1];
            if ( pTNode == pTagNode )
            {
                /* �Ѿ����㵽��pTagNode��pSrcNode�����·�� */
                nTagDis = nTotalDis;
                break;
            }
        }
        else
        {
            //�˴�Ӧ�ò��ᱻִ�е�����Ϊ���·�����Ǵ��ڵģ�����Ŀ�궥�㲻��ͼ��
            break;
        }

       /* �Ѿ��ҳ���һ����pSrcNode�����·��pTNode, 
        * ��һ��ѭ����Ҫ����ppSNode�е����� 
        */
        ppSNode[k] = pTNode;
    } /* for ( k = 1 */

    free(ppNode);
    free(pnDis);
    free(ppSNode);
    return nTagDis; /* ����Ŀ��ڵ㵽Դ�ڵ�����·�� */
}


/**	�����K��������㣬������󣬽�������ppNode��pnDis������

    @param	LPGRAPH pGraph - Ҫ�����ͼ	
    @param	GRAPHNODE **ppSNode - ��Ÿ����������ļ���S	
    @param	INT nCount - ����S�еĶ������
    @param	GRAPHNODE **ppNode - ��ż���S�и��������������С���붥��
    @param	DISTANCE *pnDis - ��ż���S�и��������������С���붥��ľ���ֵ
    @return	void - ��
*/
void SearchNextVertex(LPGRAPH pGraph,GRAPHNODE **ppSNode, INT nCount,
                            GRAPHNODE **ppNode, DISTANCE *pnDis)
{
    INT  i;

#pragma omp parallel for num_threads(dtn(nCount, 1))
    for ( i = 0; i < nCount; i++ )
    {
        /* ������ʾ����δ����ڵ㵽ĳ������õĽڵ����̾�����Ǹ��ڵ� */
        GRAPHNODE  * pShortNode; 
        SINGLENODE * pNode;
        SINGLELIST * pList;
        DISTANCE     nMinDis;

        nMinDis = GRAPH_MAX_DISTANCE;
        pShortNode = NULL;

        pList = ppSNode[i]->pEdgeList;

        pNode = pList->pHead;

        while ( pNode != NULL )
        {
            EDGE        *pEdge;
            GRAPHNODE   *pGraphNode;
            pEdge = (EDGE *)pNode->pData;
            if ( pEdge->pNode1 == ppSNode[i] )
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

            if ( nMinDis > ppSNode[i]->nMagic + pEdge->distance )
            {
                nMinDis = ppSNode[i]->nMagic + pEdge->distance;
                pShortNode = pGraphNode;
            }

            pNode = pNode->pNext;
        }// while ( pNode != NULL )

        //������δ�����������α�������⣬�������Ҫ�Ļ����ԸĽ�
        pnDis[i] = nMinDis; 
        ppNode[i*2] = pShortNode;
        ppNode[i*2 + 1] = ppSNode[i];
    } /* for ( i = 0 */
    
    return;
}