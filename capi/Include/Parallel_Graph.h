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
 *	Parallel_Graph.h
 *
 *	DESCRIPTION
 *		Interface to a generic Parallel_Graph function.
 *	HISTORY
 *		09-20-2007	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_GRAPH_H__
#define __PARALLEL_GRAPH_H__



DISTANCE Parallel_GetShortestPath(LPGRAPH pGraph, LPGRAPHNODE pSrcNode, 
                                  LPGRAPHNODE pTagNode);

#endif //__PARALLEL_GRAPH_H__
