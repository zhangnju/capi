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

#include "CapiGlobal.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Parallel_BubbleSort.h"

/**	串行冒泡排序函数

	@param	void **ppData - 待排序数据	
	@param	int nDataLen - 数据长度	
	@param	COMPAREFUNC func - 数据比较函数	
	@return	void - 无	
*/
void BubbleSort(void **ppData, int nDataLen, COMPAREFUNC func)
{
	int i, j;

	for ( i = 0; i < nDataLen-1; i++ )
	{
		for ( j = 0; j < nDataLen-i-1; j++ )
		{
			if ( (*func)(ppData[j], ppData[j+1]) > 0 )
			{
				void *pData = ppData[j];
				ppData[j] = ppData[j+1];
				ppData[j+1] = pData;
			}
		}
	}
	return;
}

/**	并行冒泡排序函数

	@param	void **ppData - 待排序数据	
	@param	int nDataLen - 数据长度	
	@param	COMPAREFUNC func - 数据比较函数	
	@return	void - 无	
*/
void Parallel_BubbleSort(void **ppData, int nDataLen, COMPAREFUNC func)
{
	int i, j;

	for ( i = 0; i < nDataLen-1; i++ )
	{
		if ( (i & 0x1) == 0 ) //i为偶数
		{
#pragma omp parallel for
			for ( j = 0; j < nDataLen - 1; j += 2 )
			{
				if ( (*func)(ppData[j], ppData[j+1]) > 0 )
				{
					void *pData = ppData[j];
					ppData[j] = ppData[j+1];
					ppData[j+1] = pData;
				}
			}
		}
		else // i为奇数
		{
#pragma omp parallel for
			for ( j = 1; j < nDataLen - 1; j += 2 )
			{
				if ( (*func)(ppData[j], ppData[j+1]) > 0 )
				{
					void *pData = ppData[j];
					ppData[j] = ppData[j+1];
					ppData[j+1] = pData;
				}
			}
		}
	}
	return;
}

