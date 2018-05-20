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
*	QuickSort.h
*
*	DESCRIPTION
*		Interface to a generic QuickSort function.
*
*	HISTORY
*		09-26-2008	create by zhouweiming.
*
*/
#ifndef __QUICKSORT_H__
#define __QUICKSORT_H__

#ifdef __cplusplus
extern "C" {
#endif


void QuickSort(void **ppData, UINT uStart, UINT uEnd, 
               COMPAREFUNC CompareFunc);



#ifdef __cplusplus
}
#endif

#endif /* __QUICKSORT_H__ */
