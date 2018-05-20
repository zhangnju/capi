
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
 *	Parallel_RadixSort_Array.h
 *
 *	DESCRIPTION
 *		Interface to a generic Parallel Radix Sort object.
 *
 *	HISTORY
 *		09-09-2007	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_RADIXSORT_ARRAY_H__
#define __PARALLEL_RADIXSORT_ARRAY_H__

struct DATA_ARRAY {
    void **ppData;
    UINT   uCount;
};


struct RADIX_BOX2 {
    DATA_ARRAY *pDataArray;
    UINT	uBoxLen;
};


void Parallel_RadixSort(SINGLELIST *pSingleList,  
						UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc);

void Parallel_RadixSort_Array(void **ppData, UINT uDataLen,
							  UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc);

void Parallel_RadixSort_Array2(void **ppData, UINT uDataLen,
							  UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc);


#endif // __PARALLEL_RADIXSORT_ARRAY_H__
