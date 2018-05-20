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
 *	Parallel_PrefixSum.h
 *
 *	DESCRIPTION
 *		Interface to a generic Parallel_PrefixSum function.
 *	HISTORY
 *		10-11-2008	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_PREFIXSUM_H__
#define __PARALLEL_PREFIXSUM_H__

#include <omp.h>

#define  MIN_PRARLLEL_PREFIXSUM_COUNT    8192

/**	串行前缀和计算函数

	@param	T * pInput - 输入数据	
	@param	T *pOutput - 输出数据（前缀和）	
	@param	int nLen - 输入数据长度	
	@return	void - 无	
*/
template <class T>
void Sequential_PrefixSum(T * pInput, T *pOutput, int nLen)
{
    int i;

    pOutput[0] = pInput[0];
    for ( i = 1; i < nLen; i++ )
    {
        pOutput[i] = pInput[i] + pOutput[i-1];
    }
}

/**	并行前缀和计算函数

	@param	T * pInput - 输入数据	
	@param	T *pOutput - 输出数据（前缀和）	
	@param	int nLen - 输入数据长度	
	@return	void - 无	
*/
template<class T>
void Parallel_PrefixSum(T * pInput, T *pOutput, int nLen)
{
    int i;

    int nCore = omp_get_num_procs();

	if ( nCore < 4 || nLen < MIN_PRARLLEL_PREFIXSUM_COUNT )
    {
        Sequential_PrefixSum(pInput, pOutput, nLen);
        return;
    }

	int nStep = nLen / nCore;
    if ( nStep * nCore < nLen )
    {
        nStep += 1;
    }

#pragma omp parallel for num_threads(nCore)
    for ( i = 0; i < nCore; i++ )
    {
        int k;
        int nStart = i * nStep;
        int nEnd = (i+1) * nStep;
        if ( nEnd > nLen )
        {
            nEnd = nLen;
        }
        pOutput[nStart] = pInput[nStart];
        for ( k = nStart+1; k < nEnd; k++ )
        {
            pOutput[k] = pInput[k] + pOutput[k-1];
        }
    }

    for ( i = 2; i < nCore; i++ )
    {
        pOutput[i * nStep - 1] += pOutput[(i-1) * nStep - 1];
    }

    pOutput[nLen-1] += pOutput[(nCore-1)*nStep - 1];

#pragma omp parallel for num_threads(nCore-1)
    for ( i = 1; i < nCore; i++ )
    {
        int k;
        int nStart = i * nStep;
        int nEnd = (i+1) * nStep - 1;
        if ( nEnd >= nLen )
        {
            nEnd = nLen - 1;
        }
        for ( k = nStart; k < nEnd; k++ )
        {
            pOutput[k] += pOutput[nStart-1];
        }
    }
    return;
}


#endif //__PARALLEL_PREFIXSUM_H__
