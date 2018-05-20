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
 *	Parallel_Search.h
 *
 *	DESCRIPTION
 *		Interface to search algorithms.
 *
 *	HISTORY
 *		08-20-2008	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_SEARCH_H__
#define __PARALLEL_SEARCH_H__

int OrdinalFind(void **ppData, void *pData, UINT uStart, UINT uEnd, 
                COMPAREFUNC comp);

int DimidiateFind(void **ppData,void *pData,UINT uStart, UINT uEnd,
                  COMPAREFUNC CompareFunc);
int BinSearch(void **ppData, int begin, int end, void *pData, COMPAREFUNC Comp);
int Parallel_SearchMaxData(void **ppData, int nLen, COMPAREFUNC comp);
int Parallel_SearchData(void **ppData, int nLen, void *pData, COMPAREFUNC comp);
int Parallel_SearchData2(void **ppData, int nLen, void *pData, COMPAREFUNC comp);
int Parallel_SearchData3(void **ppData, int nLen, void *pData, COMPAREFUNC comp);


#endif // __PARALLEL_SEARCH_H__


