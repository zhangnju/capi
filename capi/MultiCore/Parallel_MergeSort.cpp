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

#include <stdlib.h>
#include <capiglobal.h>
#include <time.h>
#include <stdio.h>
#include <omp.h>
#include "CapiCommon.h"

#define		MIN_PARALLEL_ARRAY__COUNT	512
#define		MIN_PARALLEL_SORT_COUNT		1024

extern "C" void QuickSort(void **ppData, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc);

int * MergeInt(int *pnData, int nStart1, int nEnd1, int nStart2, int nEnd2 )
{
	int i, j, k, nLen;
	int *pnNewData;

	nLen = nEnd1 - nStart1 + nEnd2 - nStart2 + 2;


	pnNewData = (int *)malloc(sizeof(int) * nLen);

	if ( pnNewData == NULL )
	{
		return NULL;
	}

	i = nStart1;
	j = nStart2;
	k = 0;

	for( i = nStart1; i <= nEnd1; i++ )
	{
		for ( ; j <= nEnd2; j++ )
		{
			if ( pnData[i] < pnData[j] )
			{
				pnNewData[k] = pnData[i];
				++k;
				break;
			}
			else
			{
				pnNewData[k] = pnData[j];
				++k;
			}
		}

		//如果j已经到了尾部
		if ( j > nEnd2 )
		{
			for ( ; i <= nEnd1; i++ )
			{
				pnNewData[k] = pnData[i];
				++k;
			}
			break;
		}
	}

	if ( i > nEnd1 )
	{
		for ( ; j <= nEnd2; j++ )
		{
			pnNewData[k] = pnData[j];
			++k;
		}
	}

	return pnNewData;
}


/**	串行归并函数
    归并好的数据存放在输出参数ppOutData中

	@param	void **ppData - 待归并的数据	
	@param	int nStart1 - 第1个区间的开始位置(包括nStart1)	
	@param	int nEnd1 - 第1个区间的结束位置(包括nEnd1)	
	@param	int nStart2 - 第2个区间的开始位置(包括nStart2)		
	@param	int nEnd2 - 第2个区间的结束位置(包括nEnd2)		
	@param	COMPAREFUNC func - 比较函数	
	@param	void **ppOutData - 存放归并后的数据	
	@return	void** - 返回归并后的数据指针(等于ppOutData)	
*/
void** Serial_Merge(void **ppData, int nStart1, int nEnd1, 
					int nStart2, int nEnd2, 
					COMPAREFUNC func, void **ppOutData)
{
	int i, j, k;

	i = nStart1;
	j = nStart2;
	k = nStart1;

	for( i = nStart1; i <= nEnd1; i++ )
	{
		for ( ; j <= nEnd2; j++ )
		{
			if ( (*func)(ppData[i], ppData[j]) < 0 )
			{
				ppOutData[k] = ppData[i];
				++k;
				break;
			}
			else
			{
				ppOutData[k] = ppData[j];
				++k;
			}
		}

		//如果j已经到了尾部
		if ( j > nEnd2 )
		{
			for ( ; i <= nEnd1; i++ )
			{
				ppOutData[k] = ppData[i];
				++k;
			}
			break;
		}
	}

	if ( i > nEnd1 )
	{
		for ( ; j <= nEnd2; j++ )
		{
			ppOutData[k] = ppData[j];
			++k;
		}
	}

	return ppOutData;
}


//待修改调试， 需要参考书籍
void Serial_Merge3(void **ppData, int nStart1, int nStart2, int nEnd2, COMPAREFUNC func)
{
	int i, k;
	int old = -1;

	k = nStart2;
	for ( i = nStart1; i < nStart2, k <= nEnd2; )
	{
		if ( (*func)(ppData[i], ppData[k]) <= 0 )
		{
			++i;
			continue;
		}
		else
		{
			void *pData = ppData[i];
			ppData[i] = ppData[k];
			ppData[k] = pData;
			++k;
			old = k-1;
			while ( (k <= nEnd2) && ((*func)(ppData[k], ppData[old]) <= 0) )
			{
				++i;
				pData = ppData[i];
				ppData[i] = ppData[k];
				ppData[k] = pData;
				++k;
			}
			break;
		}
	}

	if ( old != -1 && k < nEnd2)
	{
		if ( old < k )
		{
			Serial_Merge3(ppData, old, k, nEnd2, func);
		}
		if ( i+1 < old && old < nEnd2 )
		{
			Serial_Merge3(ppData, i+1, old, nEnd2, func);
		}
	}
}


/**	串行归并函数
    归并好的数据写回到输入参数ppData中

    @param	void **ppData - 待归并的数据	
    @param	int nStart1 - 第1个区间的开始位置(包括nStart1)	
    @param	int nEnd1 - 第1个区间的结束位置(包括nEnd1)	
    @param	int nStart2 - 第2个区间的开始位置(包括nStart2)		
    @param	int nEnd2 - 第2个区间的结束位置(包括nEnd2)		
    @param	COMPAREFUNC func - 比较函数	
    @return	void - 无	
*/
void Serial_Merge2(void **ppData, int nStart1, int nStart2, int nEnd2, COMPAREFUNC func)
{
	int i, j, k, nLen;
	void **ppOutData;

	nLen = nEnd2 - nStart1 + 1;

	ppOutData = (void **)malloc(sizeof(void *) * nLen);

	if ( ppOutData == NULL )
	{
		return;
	}

	i = nStart1;
	j = nStart2;
	k = 0;

	for( i = nStart1; i < nStart2; i++ )
	{
		for ( ; j <= nEnd2; j++ )
		{
			if ( (*func)(ppData[i], ppData[j]) < 0 )
			{
				ppOutData[k] = ppData[i];
				++k;
				break;
			}
			else
			{
				ppOutData[k] = ppData[j];
				++k;
			}
		}

		//如果j已经到了尾部
		if ( j > nEnd2 )
		{
			for ( ; i < nStart2; i++ )
			{
				ppOutData[k] = ppData[i];
				++k;
			}
			break;
		}
	}

	if ( i >= nStart2 )
	{
		for ( ; j <= nEnd2; j++ )
		{
			ppOutData[k] = ppData[j];
			++k;
		}
	}
#pragma omp parallel for 
	for ( i = nStart1; i <= nEnd2; i++ )
	{
		ppData[i] = ppOutData[i-nStart1];
	}

	free(ppOutData);
	return;
}

#define     MIN_MERGE_COUNT     1024

/**	串行归并排序函数

	@param	void **ppData - 待排序数据	
	@param	int nBegin - 排序区间的开始位置	
	@param	int nEnd - 排序区间的结束位置	
	@param	COMPAREFUNC func - 数据大小比较函数	
	@return	void - 无	
*/
void Serial_MergeSort(void **ppData, int nBegin, int nEnd, COMPAREFUNC func)
{
    if ( nEnd - nBegin < MIN_MERGE_COUNT )
    {
        QuickSort(ppData, nBegin, nEnd, func);
        return;
    }
    int nMid = (nBegin + nEnd) >> 1;  //除以2

    Serial_MergeSort(ppData, nBegin, nMid, func);
    Serial_MergeSort(ppData, nMid+1, nEnd, func);
    
    Serial_Merge2(ppData, nBegin, nMid+1, nEnd, func);
}

#define MAX_MERGE_TURN      24

/**	并行归并快速排序函数

	@param	void **ppData - 待排序数据	
	@param	int nLen - 待排序数据长度	
	@param	COMPAREFUNC func - 数据比较回调函数	
	@return	void - 无	
*/
void Parallel_MergeSort(void **ppData, int nLen, COMPAREFUNC func)
{
	int i, k;
	int	nStep;
	int	nLoopCount;
    int nCore;
	int nStart1, nEnd1;

	if ( nLen < MIN_PARALLEL_SORT_COUNT )
	{
		Serial_MergeSort(ppData, 0, nLen - 1, func );
		return;
	}

    nCore = omp_get_num_procs();

    int nCount = nLen / MIN_PARALLEL_SORT_COUNT;

    int nTurn = MAX_MERGE_TURN;
	nLoopCount = 1 << nTurn;   //nLoopCount等于2的nTurn次方

    //求出最合适的等于2的幂次方的nLoopCount
    while ( nLoopCount > nCount )
    {
        nLoopCount = nLoopCount >> 1; //除以2
        --nTurn;
    }

    int nTemp = nCore << 1; //nTemp等于nCore乘以2
    while ( (1 << nTurn) > nTemp )
    {
        --nTurn;   //减少nTurn的次数，便于减少后面归并时线程创建的次数
        nLoopCount = nLoopCount >> 1;
    }

    //判断nTurn是否为奇数
    if ( (nLoopCount > nCore) && (nTurn > 0x1) && ((nTurn & 0x1) == 0x1) ) 
    {
        --nTurn;   //把nTurn变成偶数，便于后面不拷贝数据
        nLoopCount = nLoopCount >> 1;
    }

	nStep = nLen / nLoopCount;

    int *p = new int[nLoopCount*2 + CACHE_LINE_SIZE];
    int *pnPos = (int *)GetCacheAlignedAddr(p);
    
    //将数据分成nLoopCount个小区间，并行地对每个区间进行串行快速排序
#pragma omp parallel for private(nStart1, nEnd1) num_threads(nCore)
	for ( i = 0; i < nLoopCount; i++)
	{
		nStart1 = i * nStep;
		nEnd1 = nStart1 + nStep - 1;
		if ( i == nLoopCount - 1 )
		{
			nEnd1 = nLen - 1;
		}
		Serial_MergeSort(ppData, nStart1, nEnd1, func);
        pnPos[i + i] = nStart1;
        pnPos[i + i + 1] = nEnd1;
	}

    //对排好序的各个相邻区间进行并行归并操作
    void **pp = new void *[nLen + CACHE_LINE_SIZE];
    void ** ppOutData = (void **)GetCacheAlignedAddr((void *)pp);
    void ** ppMergeData = ppData;
    void ** ppTempOutData = ppOutData;
    void ** ppSwap;

    nStep = 2;
	for ( k = 0; k < nTurn; k++ )
	{
#pragma omp parallel for num_threads(nCore)
		for ( i = 0; i < nLoopCount-1; i += 2 )
		{
            int nPos = i * nStep;
			
            Serial_Merge(ppMergeData, pnPos[nPos], pnPos[nPos+1], 
                pnPos[nPos+nStep], pnPos[nPos+nStep+1], func, ppTempOutData);

            pnPos[nPos+1] = pnPos[nPos+nStep+1];
		}
		
        nLoopCount = nLoopCount >> 1; //除以2
        nStep += nStep;

        ppSwap = ppMergeData;
        ppMergeData = ppTempOutData;
        ppTempOutData = ppSwap;
	}

    //将数据写回ppData中，如果nTurn为偶数则下面判断内的循环不会被执行
    if ( ppMergeData == ppOutData )
    {
#pragma omp parallel for num_threads(nCore)
        for ( i = 0; i < nLen; i++ )
        {
            ppData[i] = ppOutData[i];
        }
    }

    delete [] p;
    delete [] pp;
	return;
}


/**	并行归并快速排序函数

    @param	void **ppData - 待排序数据	
    @param	int nLen - 待排序数据长度	
    @param	int nTurn - 并行归并轮次，一般满足的nTurn次方等于CPU核数	
    @param	COMPAREFUNC func - 数据比较回调函数	
    @return	void - 无	
*/
void Parallel_MergeSort2(void **ppData, int nLen, 
                        int nTurn, COMPAREFUNC func)
{
    int i, k;
    int	nStep;
    int	nCore;
    int nStart1, nEnd1, nStart2, nEnd2;

    if ( nLen < MIN_PARALLEL_SORT_COUNT )
    {
        QuickSort(ppData, 0, nLen - 1, func );
        return;
    }

    nCore = 1<< nTurn ; //nCore等于2的nTurn次方

    nStep = nLen / nCore;

    //将数据分成若干个小区间，并行地对每个区间进行串行快速排序
#pragma omp parallel for private(nStart1, nEnd1) num_threads(nCore)
    for ( i = 0; i < nCore; i++)
    {
        nStart1 = i * nStep;
        nEnd1 = (i+1) * nStep - 1;
        if ( i == nCore - 1 )
        {
            nEnd1 = nLen - 1;
        }
        QuickSort(ppData, nStart1, nEnd1, func);
    }

    //将排好序的各个区间进行并行归并操作
    for ( k = 0; k < nTurn; k++ )
    {
#pragma omp parallel for private(nStart1, nEnd1, nStart2, nEnd2) num_threads(nCore) 
        for ( i = 0; i < nCore-1; i += 2 )
        {
            nStart1 = i * nStep;
            nEnd1 = nStart1 + nStep - 1;
            nStart2 = nEnd1 + 1;
            nEnd2 = nEnd1 + nStep;
            if ( i == nCore - 2 )
            {
                nEnd2 = nLen - 1;
            }
            Serial_Merge2(ppData, nStart1, nStart2, nEnd2, func);
        }

        nCore = nCore / 2;
        nStep += nStep;
    }
    return;
}
