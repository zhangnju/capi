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


/**	计算图中两个节点间的最短路径函数
    计算完后，每个节点的pMagic是用来存放到源节点最短路径的下一个节点指针
    nMagic用来存放到源节点的最短路径的总距离

    @param	LPGRAPH pGraph - 要计算的图	
    @param	LPGRAPHNODE pSrcNode - 源节点	
    @param	LPGRAPHNODE pTagNode - 目标节点	
    @return	DISTANCE - GRAPH_ERROR 表示参数错误，GRAPH_NOT_PATH 表示没有路径
    成功时返回两点间最短路径的总距离
*/
DISTANCE Parallel_GetShortestPath(LPGRAPH pGraph, LPGRAPHNODE pSrcNode, 
                                 LPGRAPHNODE pTagNode)
{
    INT         i, k;    /* 循环变量 */
    GRAPHNODE   **ppNode; /* 用来存放已经计算好的到pSrcNode最短路径的节点数组*/
    DISTANCE    nTagDis;  /* 用来保存pTagNode到pSrcNode最短路径的距离值 */
    GRAPHNODE   **ppSNode; /* 保存计算的临时结果 */
    DISTANCE    *pnDis;

    nTagDis = -1;  /* 初始化为-1, 即初始时pTagNode和pSrcNode还没有计算出路径 */

    /* 参数校验 */
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
        pGraph->ppNodeArray[i]->nMagic = -1; /* 初始化为-1表示还没有计算过 */
        pGraph->ppNodeArray[i]->pMagic = NULL;
    }

    ppSNode[0] = pSrcNode;
    ppSNode[0]->nMagic = 0; /*初始化为0，同时也表示pSrcNode到pSrcNode的距离为0*/
    ppSNode[0]->pMagic = NULL;

    /* k从1开始循环是因为ppNode[0]已经不需计算 */
    for ( k = 1; k < pGraph->nNodeCount; k++ ) 
    {
        /* 用来保存所有未计算节点中到pSrcNode节点最短距离的距离值 */
        DISTANCE    nTotalDis;  

        /* 用来表示所有未计算节点中到pSrcNode节点最短距离的节点 */
        GRAPHNODE   *pTNode;    

        pTNode  = NULL;
        nTotalDis = GRAPH_MAX_DISTANCE;

        SearchNextVertex(pGraph, ppSNode, k, ppNode, pnDis);


        // 如果k较大时，比如k超过500时可以改为使用并行搜索
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
                /* 已经计算到了pTagNode和pSrcNode的最短路径 */
                nTagDis = nTotalDis;
                break;
            }
        }
        else
        {
            //此处应该不会被执行到，因为最短路径总是存在的，除非目标顶点不在图中
            break;
        }

       /* 已经找出了一个到pSrcNode的最短路径pTNode, 
        * 下一轮循环需要扩大ppSNode中的数量 
        */
        ppSNode[k] = pTNode;
    } /* for ( k = 1 */

    free(ppNode);
    free(pnDis);
    free(ppSNode);
    return nTagDis; /* 返回目标节点到源节点的最短路径 */
}


/**	计算第K个最近顶点，计算完后，结果存放在ppNode和pnDis数组中

    @param	LPGRAPH pGraph - 要计算的图	
    @param	GRAPHNODE **ppSNode - 存放各个最近顶点的集合S	
    @param	INT nCount - 集合S中的顶点个数
    @param	GRAPHNODE **ppNode - 存放集合S中各个顶点的相邻最小距离顶点
    @param	DISTANCE *pnDis - 存放集合S中各个顶点的相邻最小距离顶点的距离值
    @return	void - 无
*/
void SearchNextVertex(LPGRAPH pGraph,GRAPHNODE **ppSNode, INT nCount,
                            GRAPHNODE **ppNode, DISTANCE *pnDis)
{
    INT  i;

#pragma omp parallel for num_threads(dtn(nCount, 1))
    for ( i = 0; i < nCount; i++ )
    {
        /* 用来表示所有未计算节点到某个已算好的节点的最短距离的那个节点 */
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

        //下面这段代码存在随机的伪共享问题，如果有需要的话可以改进
        pnDis[i] = nMinDis; 
        ppNode[i*2] = pShortNode;
        ppNode[i*2 + 1] = ppSNode[i];
    } /* for ( i = 0 */
    
    return;
}