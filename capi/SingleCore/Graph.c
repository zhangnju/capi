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


/**	比较两个指针大小的函数

    @param	void *p1 - 要比较的指针1	
    @param	void *p2 - 要比较的指针2
    @return	INT - 0表示相等，1表示p1>p2, -1表示p1<p2	
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


/**	图的节点创建函数

    @return	GRAPHNODE * - 成功返回创建的图节点指针，失败返回NULL。	
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


/**	图的节点释放函数

    @param	LPGRAPHNODE pGraphNode - 图节点指针	
    @param	DESTROYFUNC GraphNodePropDestroyFunc - 图节点属性释放函数	
    @return	void - 无	
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
        /* 这里不需要释放数据, 因为边在另外链表中有保存 */
        SingleList_Destroy(pGraphNode->pEdgeList, NULL);
    }

    free(pGraphNode);
}


/**	图节点的添加边函数

    @param	LPGRAPHNODE pGraphNode - 图节点指针
    @param	LPEDGE pEdge - 边指针	
    @return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
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


/**	图节点的删除边函数

    @param	LPGRAPHNODE pGraphNode - 图节点指针	
    @param	LPEDGE  pEdge - 边指针	
    @param	DESTROYFUNC EdgePropDestroyFunc - 边属性释放函数	
    @return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功。	
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


/**	边的创建函数

    @return	EDGE * - 成功返回创建的边指针，失败返回NULL。	
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


/**	边的释放函数

    @param	LPEDGE pEdge - 边指针
    @param	DESTROYFUNC EdgePropDestroyFunc - 边属性释放函数	
    @return	void - 无	
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


/**	图的创建函数

    @param	INT nNodeCount - 要创建的图的最大节点数量	
    @return	GRAPH * - 称工返回创建的图指针，失败返回NULL。	
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


/**	图的释放函数

    @param	LPGRAPH pGraph - 图的指针	
    @param	DESTROYFUNC GraphPropDestroyFunc - 图的属性释放函数	
    @param	DESTROYFUNC GraphNodePropDestroyFunc - 图的属性释放函数	
    @param	DESTROYFUNC EdgePropDestroyFunc - 边的属性释放函数	
    @return	void - 无	
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


/**	获取指定图的顶点个数

    @param	GRAPH *pGraph -图的指针	
    @return	INT -成功图的顶点个数, 失败返回-1.
*/
INT Graph_GetNodeCount(GRAPH *pGraph)
{
    if ( NULL == pGraph )
    {
        return -1;
    }
    return pGraph->nNodeCount;
}


/**	获取指定图的边的数量

    @param	GRAPH *pGraph -图的指针	
    @return	INT -成功返回图的边的数量, 失败返回-1.	
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

/**	计算图中两个节点间的最短路径函数
        计算完后，每个节点的pMagic用来存放到源节点最短路径的下一个节点指针
        nMagic用来存放到源节点的最短路径的总距离

    @param	LPGRAPH pGraph - 要计算的图	
    @param	LPGRAPHNODE pSrcNode - 源节点	
    @param	LPGRAPHNODE pTagNode - 目标节点	
    @return	DISTANCE - GRAPH_ERROR 表示参数错误
                           GRAPH_NOT_PATH 表示没有路径
                           成功时返回两点间最短路径的总距离
*/
DISTANCE Graph_GetShortestPath(LPGRAPH pGraph, LPGRAPHNODE pSrcNode, 
                                LPGRAPHNODE pTagNode)
{
    INT       i, j, k;   /* 循环变量 */
    INT       nCalCount; /* 用来记录已经计算好的到pSrcNode最短路径的节点数量 */
    GRAPHNODE **ppNode;  /* 用来存放已经计算好的到pSrcNode最短路径的节点数组 */
    DISTANCE  nTagDis;   /* 用来保存pTagNode到pSrcNode最短路径的距离值 */

    nTagDis = -1;  /* 初始化为-1, 即初始时pTagNode和pSrcNode还没有计算出路径 */

    /* 参数校验 */
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
        pGraph->ppNodeArray[i]->nMagic = -1; /* 初始化为-1表示还没有计算过 */
        pGraph->ppNodeArray[i]->pMagic = NULL;
    }

    ppNode[0] = pSrcNode;
    ppNode[0]->nMagic = 0; /*初始化为0，同时也表示pSrcNode到pSrcNode的距离为0*/
    ppNode[0]->pMagic = NULL;
    nCalCount = 1;         /* 初始化为1，表示pSrcNode这1个节点已经计算完了 */

    /* k从1开始循环是因为ppNode[0]已经不需计算 */
    for ( k = 1; k < pGraph->nNodeCount; k++ ) 
    {
        /* nTotalDis用来保存所有未计算节点中到pSrcNode节点最短距离的距离值 */
        DISTANCE    nTotalDis; 
        
        /* pSNode用来表示目标节点到pSrcNode最短路径上和目标节点相邻的节点 */
        GRAPHNODE   *pSNode;   
        
        /* pTNode用来表示所有未计算节点中到pSrcNode节点最短距离的节点 */
        GRAPHNODE   *pTNode;   

        pSNode  = NULL;
        pTNode  = NULL;
        nTotalDis = 0;

        for ( i = 0; i < nCalCount; i++ )
        {
            DISTANCE     nMinDis;
            /* 用来表示所有未计算节点到某个已算好的节点的最短距离的那个节点 */
            GRAPHNODE  * pShortNode; 
            SINGLENODE * pNode;
            SINGLELIST * pList;

            nMinDis = 0;
            pShortNode = NULL;

            for ( j = 0; j < pGraph->nNodeCount; j++ )
            {
                if ( pGraph->ppNodeArray[j]->nMagic != -1 ) 
                {
                    /* 节点已经计算出结果 */
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
                        break; /* 在邻居中找到对应节点后应中止查找 */
                    }
                    pNode = pNode->pNext;
                } /* while ( p ) */
            } /* for ( j = 0 */
            if ( nTotalDis == 0 )
            {
                nTotalDis = nMinDis; /* 重新初始化nTotaldis */
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
            pTNode->pMagic = pSNode; /* 在pMagic中存放最短路径上的相邻节点 */
            if ( pTNode == pTagNode )
            {
                /* 已经计算到了pTagNode和pSrcNode的最短路径 */
                nTagDis = nTotalDis;
                break;
            }
        }
        
        /* 已经找出了一个到pSrcNode的最短路径pTNode, 
         * 下一轮循环需要扩大ppNode中的数量 
         */
        ppNode[nCalCount] = pTNode;
        nCalCount++;
    } /* for ( k = 0 */
    free(ppNode);
    return nTagDis; /* 返回目标节点到源节点的最短路径 */
}

/**	计算图中两个节点间的最短路径函数
    计算完后，每个节点的pMagic是用来存放到源节点最短路径的下一个节点指针
    nMagic用来存放到源节点的最短路径的总距离

    @param	LPGRAPH pGraph - 要计算的图	
    @param	LPGRAPHNODE pSrcNode - 源节点	
    @param	LPGRAPHNODE pTagNode - 目标节点	
    @return	DISTANCE - GRAPH_ERROR 表示参数错误
                           GRAPH_NOT_PATH 表示没有路径
                           成功时返回两点间最短路径的总距离
*/
DISTANCE Graph_GetShortestPath2(LPGRAPH pGraph, LPGRAPHNODE pSrcNode, 
                                 LPGRAPHNODE pTagNode)
{
    INT         i, k;    /* 循环变量 */
    INT         nCalCount;/* 用来记录已经计算好的到pSrcNode最短路径的节点数量*/
    GRAPHNODE   **ppNode; /* 用来存放已经计算好的到pSrcNode最短路径的节点数组*/
    DISTANCE    nTagDis;  /* 用来保存pTagNode到pSrcNode最短路径的距离值 */

    nTagDis = -1;  /* 初始化为-1, 即初始时pTagNode和pSrcNode还没有计算出路径 */

    /* 参数校验 */
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
        pGraph->ppNodeArray[i]->nMagic = -1; /* 初始化为-1表示还没有计算过 */
        pGraph->ppNodeArray[i]->pMagic = NULL;
    }

    ppNode[0] = pSrcNode;
    ppNode[0]->nMagic = 0; /*初始化为0，同时也表示pSrcNode到pSrcNode的距离为0*/
    ppNode[0]->pMagic = NULL;
    nCalCount = 1;           /* 初始化为1，表示pSrcNode这1个节点已经计算完了 */

    /* k从1开始循环是因为ppNode[0]已经不需计算 */
    for ( k = 1; k < pGraph->nNodeCount; k++ ) 
    {
        /* 用来保存所有未计算节点中到pSrcNode节点最短距离的距离值 */
        DISTANCE    nTotalDis;  
        
        /* 用来表示目标节点到pSrcNode最短路径上和目标节点相邻的节点 */
        GRAPHNODE   *pSNode;    
        
        /* 用来表示所有未计算节点中到pSrcNode节点最短距离的节点 */
        GRAPHNODE   *pTNode;    

        pSNode  = NULL;
        pTNode  = NULL;
        nTotalDis = 0;

        for ( i = 0; i < nCalCount; i++ )
        {
            /* 用来表示所有未计算节点到某个已算好的节点的最短距离的那个节点 */
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
                nTotalDis = nMinDis; /* 重新初始化nTotaldis */
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
            pTNode->pMagic = pSNode; /* 在pMagic中存放最短路径上的相邻节点 */
            if ( pTNode == pTagNode )
            {
                /* 已经计算到了pTagNode和pSrcNode的最短路径 */
                nTagDis = nTotalDis;
                break;
            }
        }
        
        /* 已经找出了一个到pSrcNode的最短路径pTNode, 
         * 下一轮循环需要扩大ppNode中的数量 
         */
        ppNode[nCalCount] = pTNode;
        nCalCount++;
    } /* for ( k = 0 */
    free(ppNode);
    return nTagDis; /* 返回目标节点到源节点的最短路径 */
}






/**	对图进行深度优先搜索
        计算完后，每个节点的pMagic是用来存放父节点指针
        节点的nMagic是用来存放DFS编码
        边的nMagic是用来存放是否使用的标志, 搜索完后, 标志为GRAPH_EDGE_SEARCHED
        的边构成一颗生成树, 树的根节点为搜索起点

    @param	LPGRAPH pGraph - 要搜索的图	
    @param	LPGRAPHNODE pOrgNode - 要搜索的起点, 为图中的某个顶点	
    @param	LPEDGE  pTagEdge - 要搜索的指定的边, 当搜索到此边后停止搜索
                                   如果为NULL则一直将整个图完全遍历搜索
    @param	EDGEDUMPFUNC EdgeDumpFunc - 边的回调函数, 为NULL则不执行回调
    @return	INT - 0 表示失败，1表示搜索完成, 2表示搜索到指定边pTagEdge.	
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

    /* 初始化变量 */
    nControlFlag = 0;
    nDfsCode = 0;
    pCurrentNode = NULL;

    /* 初始化所有节点的DFS编码为0以及父节点指针为空 */
    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        /* 本算法中，nMagic被用来记录DFS编码 */
        pGraph->ppNodeArray[i]->nMagic = 0;    
        /* 本算法中，pMagic被用来记录父节点 */
        pGraph->ppNodeArray[i]->pMagic = NULL; 
    }

    /* 初始化所有的边为0表示未用过，
     * 注：pEdge->nMagic为GRAPH_EDGE_NOT_SEARCH时表示边未用过，
     * 为GRAPH_EDGE_SEARCHED表示已被搜索过 
     */
    pListNode = pGraph->pEdgeList->pHead;

    while ( pListNode != NULL )
    {
        pEdge = (EDGE *)pListNode->pData;
        /* 初始化nMagic为GRAPH_EDGE_NOT_SEARCH表示此边未被搜索过 */
        pEdge->nMagic = GRAPH_EDGE_NOT_SEARCH; 
        pListNode = pListNode->pNext;
    }

    pCurrentNode = pOrgNode;

    /* 将所有节点的邻居列表当前位置放置在链表头部以便于后面从头搜索 */
    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        SingleList_EnumBegin(pGraph->ppNodeArray[i]->pEdgeList);
    }

    for ( ; ; )
    {
        void  *pData;
        if ( nControlFlag == 0 )
        {
            /* 步骤2：给顶点标上DFS编码 */
            nDfsCode++;
            pCurrentNode->nMagic = nDfsCode;
        }
        else
        {
            nControlFlag = 0;
        }

        pEdge = NULL;
        /* 步骤3：查找未用过的关联边 */
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

        if ( NULL != pEdge ) /* 判断是否找到未用过的边 */
        {
            GRAPHNODE   *pNextNode;

            /* 步骤4：已经找到了一条未用过的关联边 */

            /* 取出当前边的相邻节点放到pNextNode中 */
            if ( pEdge->pNode1 == pCurrentNode )
            {
                pNextNode = pEdge->pNode2;
            }
            else
            {
                pNextNode = pEdge->pNode1;
            }

            /* 判断相邻节点的DFS编码是否为0 */
            if ( pNextNode->nMagic == 0 )
            {
                pEdge->nMagic = GRAPH_EDGE_SEARCHED; /* 将此边标示为用过 */

                /* 将当前节点作为搜索到的节点的父节点 */
                pNextNode->pMagic = pCurrentNode; 
                /* 将当前节点指针指向搜索到的相邻节点 */
                pCurrentNode = pNextNode;   

                nControlFlag = 0;

                if ( EdgeDumpFunc )
                {
                    (*EdgeDumpFunc)(pEdge);
                }

                continue;   /* 转步骤2 */
            }
            else
            {
                /* 此边已经搜索过但不在生成树中 */
                pEdge->nMagic = GRAPH_EDGE_NOT_TREE; 

                if ( EdgeDumpFunc )
                {
                    (*EdgeDumpFunc)(pEdge);
                }

                nControlFlag = 1;
                /* 转步骤3 */
            }

            if ( pEdge == pTagEdge)
            {
                return 2;
            }
        }
        else
        {
            /* 步骤5：如果当前节点编码为1即到了pOrgNode，则已经搜索完毕需中止*/
            if ( pCurrentNode->nMagic == 1 )
            {
                break;
            }

            /* 步骤6：回溯到父节点转步骤3继续查找 */
            pCurrentNode = pCurrentNode->pMagic;

            nControlFlag = 1;
            continue; /* 转步骤3 */
        }
    }
    return 1;
}



/**	图的宽度优先搜索函数
        搜索完后, 节点的nMagic用来存放BFS编码
        节点的pMagic用来存放下一个被搜索到的节点的指针, 因此由pMagic可以得到搜索的
        节点顺序, 边的nMagic是用来存放是否被搜索的标志, 标志为GRAPH_EDGE_SEARCHED
        的边构成一颗生成树, 树的根节点为搜索起点

    @param	LPGRAPH pGraph - 要搜索的图	
    @param	LPGRAPHNODE pOrgNode - 要搜索的起点	
    @param	LPGRAPHNODE pTagNode - 要搜索的目标节点	
    @param	GRAPHNODEDUMPFUNC GraphNodeDumpFunc - 顶点回调函数	
    @param	EDGEDUMPFUNC EdgeDumpFunc - 边回调函数	
    @return	INT - 0表示失败, 1表示搜索完成, 2表示搜索到指定目标节点pTagNode	
*/
INT Graph_BreadFirstSearch(LPGRAPH pGraph, 
                       LPGRAPHNODE pOrgNode,
                       LPGRAPHNODE pTagNode,
                       GRAPHNODEDUMPFUNC GraphNodeDumpFunc,
                       EDGEDUMPFUNC EdgeDumpFunc)
{
    INT         nBfsCode;       /* BFS编码 */
    GRAPHNODE   *pCurrentNode;  /* 记录当前要操作的节点 */
    GRAPHNODE   *pLastNode;     /* 记录最后一个要操作的节点 */
    INT         i;
    SINGLENODE    *pTmpNode;

    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        /* nMagic在此算法中是用来记录节点的BFS编码 */
        pGraph->ppNodeArray[i]->nMagic = -1;  /* 初始化为-1表示还没有编码 */
        
        /* pMagic在此算法中是用力将已搜索到但未对本节点进行搜索的节点形成链表*/
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


    nBfsCode = 0;               /* 初始化为0 */
    pCurrentNode = pOrgNode;
    pCurrentNode->nMagic = nBfsCode;  /* nMagic用来记录BFS编码 */

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
                /* 边已经被搜索过了 */
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
            
            if ( pNode->nMagic == -1 )  /* 等于-1表示节点还未编码 */
            {
                pNode->nMagic = nBfsCode;

                pLastNode->pMagic = pNode;
                pLastNode = pNode;

                pEdge->nMagic = GRAPH_EDGE_SEARCHED;

                if ( GraphNodeDumpFunc )
                {
                    /* 对顶点执行回调函数 */
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
            /* 调用边的回调函数 */
            if ( EdgeDumpFunc )
            {
                (*EdgeDumpFunc)(pEdge);
            }

        }
        /* 将当前节点指向下一个未搜索节点 */
        pCurrentNode = pCurrentNode->pMagic;
    }
    return 1;
}

/**	距离比较函数

    @param	void *p1 - 要比较的边指针1	
    @param	void *p2 - 要比较的边指针2	
    @return	INT - 1表示p1的距离大于p2的距离
                  0表示p1的距离等于p2的距离
                  -1表示p1的距离小于p2的距离
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

/**	图的边排序函数，对所有边使用归并排序算法进行排序

    @param	LPGRAPH pGraph - 图指针	
    @return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败。	
*/
INT Graph_SortEdgeInOrder(LPGRAPH pGraph)
{
    if ( NULL == pGraph )
    {
        return CAPI_FAILED;
    }
    return SingleList_MergeSort(pGraph->pEdgeList, DistanceCompare, 32);
}


/**	利用Kruskal算法求最小生成树
        计算过程中,节点的nMagic用来记录父顶点编号
        计算完后, nMagic为GRAPH_EDGE_SEARCHED的边组成一颗最小生成树

    @param	LPGRAPH pGraph - 要求最小生成树的图	
    @return	INT - CAPI_FAILED表示失败, 1表示成功	
*/
INT Graph_GetMinTree(LPGRAPH pGraph)
{
    SINGLENODE    *pEdgeListNode;
    INT         i;
    INT         *pNodeNo; /* 用来记录所有节点父节点号便于进行成环的判断 */

    if ( NULL == pGraph )
    {
        return CAPI_FAILED;
    }

    pNodeNo = (INT *)malloc(pGraph->nNodeCount * sizeof(INT));
    if ( NULL == pNodeNo )
    {
        return CAPI_FAILED;
    }

    /* 初始化所有节点的nMagic为节点编号, 父节点号为-1表示还没有加入到生成树中 */
    for ( i = 0; i < pGraph->nNodeCount; i++ )
    {
        pGraph->ppNodeArray[i]->nMagic = i;
        pNodeNo[i] = -1; /* 初始化所有节点的父节点号为-1表示还没有计算过 */
    }

    Graph_SortEdgeInOrder(pGraph);
    
    /* 初始化所有边的nMagic为GRPAH_EDGE_NOT_SEARCH */
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
        INT         bnf;    /* 一条边开始顶点的父节点编号 */
        INT         edf;    /* 一条边结束顶点的父节点编号 */

        pEdge = (EDGE *)pEdgeListNode->pData;

        bnf = pEdge->pNode1->nMagic; /* nMagic表示节点编号 */
        while ( pNodeNo[bnf] != -1 )
        {
            bnf = pNodeNo[bnf];
        }

        edf = pEdge->pNode2->nMagic; /* nMagic表示节点编号 */
        while ( pNodeNo[edf] != -1 )
        {
            edf = pNodeNo[edf];
        }

        if ( bnf != edf )
        {
            /* 没有成环, 将编号为edf的节点作为编号为bnf的节点的父节点 */
            pNodeNo[bnf] = edf; 

            /* 将此边标示为已经搜索过, 也就是说放入了生成树中 */
            pEdge->nMagic = GRAPH_EDGE_SEARCHED;
        }
        else
        {
            /* 已经成环成环, 将此边标示为不在生成树中 */
            pEdge->nMagic = GRAPH_EDGE_NOT_TREE;
        }

        pEdgeListNode = pEdgeListNode->pNext;
    } /* while */ 
    
    free(pNodeNo);
    return 1;
}

