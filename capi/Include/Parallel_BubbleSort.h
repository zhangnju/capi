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
 *	Parallel_BubbleSort.h
 *
 *	DESCRIPTION
 *		Interface to a generic multiple task object.
 *	HISTORY
 *		09-09-2007	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_BUBBLESORT_H__
#define __PARALLEL_BUBBLESORT_H__


void BubbleSort(void **ppData, int nDataLen, COMPAREFUNC func);
void Parallel_BubbleSort(void **ppData, int nDataLen, COMPAREFUNC func);
void Test_Parallel_BubbleSort();


#endif //__PARALLEL_BUBBLESORT_H__