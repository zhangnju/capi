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
 *	Parallel_MergeSort.h
 *
 *	DESCRIPTION
 *		Interface to a generic parallel merge sort object.
 *	HISTORY
 *		09-09-2007	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_MERGESORT_H__
#define __PARALLEL_MERGESORT_H__


void** Serial_Merge(void **ppData, int nStart1, int nEnd1, int nStart2, int nEnd2, 
                    COMPAREFUNC func, void **ppOutData);

int * MergeInt(int *pnData, int nStart1, int nEnd1, int nStart2, int nEnd2 );

void Serial_MergeSort(void **ppData, int nBegin, int nEnd, COMPAREFUNC func);

void Serial_Merge2(void **ppData, int nStart1, int nStart2, int nEnd2, COMPAREFUNC func);
void Serial_Merge3(void **ppData, int nStart1, int nStart2, int nEnd2, COMPAREFUNC func);

void Parallel_MergeSort(void **ppData, int nLen, COMPAREFUNC func);
void Parallel_MergeSort2(void **ppData, int nLen, 
                         int nTurn, COMPAREFUNC func);

#endif // __PARALLEL_MERGESORT_H__
