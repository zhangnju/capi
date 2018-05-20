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

/*
 * Graph.h
 *
 * DESCRIPTION
 *      Graph algorithm module.
 *
 * HISTROY
 *		09-12-2003	create by zhouweiming.
 *
 */
#ifndef __GRAPH_H__
#define __GRAPH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define     GRAPH_NO_PATH   -1
#define     GRAPH_ERROR     -2
#define     GRAPH_SUCCESS   0

#define     GRAPH_EDGE_NOT_TREE     2
#define     GRAPH_EDGE_SEARCHED     1
#define     GRAPH_EDGE_NOT_SEARCH   0

#define     GRAPH_MAX_DISTANCE  0x7fffffff

typedef     long    DISTANCE;

typedef struct GRAPHNODE_st {
    void        *pProperties;
    SINGLELIST  *pEdgeList;
    void        *pMagic;        /* 内部算法使用，外部不可以改变 */
    INT         nMagic;         /* 内部算法使用，外部不可以改变 */
} GRAPHNODE, *LPGRAPHNODE;

typedef struct EDGE_st {
    DISTANCE    distance;
    INT         nMagic;
    GRAPHNODE   *pNode1;
    GRAPHNODE   *pNode2;
    void        *pProperties;
} EDGE, *LPEDGE;


typedef struct GRAPH_st {
    GRAPHNODE   **ppNodeArray;
    SINGLELIST  *pEdgeList;
    INT         nNodeCount;
    void        *pProperties;
} GRAPH, *LPGRAPH;

/* Destroy callback function definition */
typedef void (*DESTROYFUNC)( void *pData );
typedef INT  (*EDGEDUMPFUNC)(LPEDGE pEdge);
typedef INT  (*GRAPHNODEDUMPFUNC)(LPGRAPHNODE pGraphNode);

/* Function prototypes */
GRAPHNODE * GraphNode_New( );
void        GraphNode_Destroy(LPGRAPHNODE pGraphNode,
                              DESTROYFUNC GraphNodePropDestroyFunc);

INT         GraphNode_AddEdge(LPGRAPHNODE pGraphNode, 
                              LPEDGE pEdge);

INT         GraphNode_RemoveEdge(LPGRAPHNODE pGraphNode, 
                                 LPEDGE pEdge,
                                 DESTROYFUNC EdgePropDestroyFunc);

EDGE      * Edge_New();
void        Edge_Destroy(LPEDGE pEdge,
                         DESTROYFUNC EdgePropDestroyFunc);

GRAPH     * Graph_New(INT nNodeCount);

void        Graph_Destroy(LPGRAPH pGraph,
                   DESTROYFUNC GraphPropDestroyFunc,
                   DESTROYFUNC GraphNodePropDestroyFunc,
                   DESTROYFUNC EdgePropDestroyFunc);

INT         Graph_GetNodeCount(GRAPH *pGraph);

INT         Graph_GetEdgeCount(GRAPH *pGraph);

/* Graph's best short path algorithm */
DISTANCE    Graph_GetShortestPath(LPGRAPH pGraph, 
                                   LPGRAPHNODE pSrcNode, 
                                   LPGRAPHNODE pTagNode);

DISTANCE    Graph_GetShortestPath2(LPGRAPH pGraph, 
                                   LPGRAPHNODE pSrcNode, 
                                   LPGRAPHNODE pTagNode);

INT         Graph_DepthFirstSearch(LPGRAPH pGraph, 
                                   LPGRAPHNODE pOrgNode,
                                   LPEDGE pTagEdge,
                                   EDGEDUMPFUNC EdgeDumpFunc);

INT         Graph_BreadFirstSearch(LPGRAPH pGraph, 
                                   LPGRAPHNODE pOrgNode,
                                   LPGRAPHNODE pTagNode,
                                   GRAPHNODEDUMPFUNC GraphNodeDumpFunc,
                                   EDGEDUMPFUNC EdgeDumpFunc);

INT         Graph_SortEdgeInOrder(LPGRAPH pGraph);

INT         Graph_GetMinTree(LPGRAPH pGraph);

#ifdef __cplusplus
}
#endif

#endif /* __GRAPH_H__ */
