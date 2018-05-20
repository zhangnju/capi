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
#include <stdio.h>
#include "TestApi.h"
#include "SingleList.h"
#include "Graph.h"
#include "Parallel_Graph.h"

void TestCase_Parallel_GetShortestPath_1(void);
void TestCase_Graph_GetShortestPath_1(void);

void Test_Graph()
{
    TestCase_Add(TestCase_Graph_GetShortestPath_1);
    TestCase_Add(TestCase_Parallel_GetShortestPath_1);
}

REGISTER_TESTFUNC(Test_Graph)


static void SetEdge(GRAPH *pGraph, int node1, int node2, DISTANCE nDis)
{
    EDGE *pEdge = Edge_New();
    pEdge->distance = nDis;
    pEdge->pNode1 = pGraph->ppNodeArray[node1];
    pEdge->pNode2 = pGraph->ppNodeArray[node2];
    GraphNode_AddEdge(pGraph->ppNodeArray[node1], pEdge);
    GraphNode_AddEdge(pGraph->ppNodeArray[node2], pEdge);
    SingleList_InsertTail(pGraph->pEdgeList, pEdge);
}

static GRAPH * BuildGraphInstance()
{
    GRAPH  *pGraph = Graph_New(6);

    int i;
    for ( i = 0; i < 6; i++ )
    {
        pGraph->ppNodeArray[i] = GraphNode_New();

        pGraph->ppNodeArray[i]->pProperties = (void *)('A'+i);
    }

    SetEdge(pGraph, 0, 1, 15);
    SetEdge(pGraph, 0, 2, 25);
    SetEdge(pGraph, 0, 3, 19);
    SetEdge(pGraph, 0, 4, 12);
    SetEdge(pGraph, 1, 2, 18);
    SetEdge(pGraph, 2, 3, 35);
    SetEdge(pGraph, 3, 4, 5);
    SetEdge(pGraph, 3, 5, 15);
    SetEdge(pGraph, 4, 5, 45);

    return pGraph;
}


void TestCase_Graph_GetShortestPath_1(void)
{
    GRAPH  *pGraph = BuildGraphInstance();

    DISTANCE    nDis = -1;

    nDis = Graph_GetShortestPath2(pGraph, pGraph->ppNodeArray[0], pGraph->ppNodeArray[5]);

    assertTrue(nDis == 32);
    GRAPHNODE *pNode = (GRAPHNODE *)pGraph->ppNodeArray[5]->pMagic;
    assertTrue(pNode->pProperties == (void *)('A'+3));

    Graph_Destroy(pGraph, NULL, NULL, NULL);
}

void TestCase_Parallel_GetShortestPath_1(void)
{
    GRAPH  *pGraph = BuildGraphInstance();

    DISTANCE    nDis = -1;

    nDis = Parallel_GetShortestPath(pGraph, pGraph->ppNodeArray[0], pGraph->ppNodeArray[5]);

    assertTrue(nDis == 32);
    GRAPHNODE *pNode = (GRAPHNODE *)pGraph->ppNodeArray[5]->pMagic;
    assertTrue(pNode->pProperties == (void *)('A'+3));
    Graph_Destroy(pGraph, NULL, NULL, NULL);

}


