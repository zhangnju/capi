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
#include <omp.h>
#include "CapiGlobal.h"
#include "CapiCommon.h"

int power2(int n)
{
	if ( n > 32 )
	{
		return -1;
	}
	return 1 << n;
}


/**	二分查找函数，调用此函数前必须对表从小到大排好序

    @param	void **ppData - 数据数组指针
    @param	UINT uCount - ppData数组中的已有数据个数
    @param	void *pData - 要查找的匹配数据
    @param	COMPAREFUNC CompareFunc - 比较函数
    @return	void * - 成功返回查到的精确数据或刚好比要查找的数据大的数据,
                     失败返回-1。
*/
int  BFind(void **ppData, UINT uCount, void *pData, COMPAREFUNC CompareFunc)
{
    int nLow;
    int nHigh;
    int nMid;

    if ( ppData == NULL || CompareFunc == NULL || pData == NULL )
    {
        return NULL;
    }

    nLow = 0;
    nHigh = uCount - 1;
    nMid = 0;
    while ( nLow <= nHigh )
    {
        INT nResult;
        nMid = ( nLow + nHigh ) / 2;
        nResult = (*CompareFunc)( ppData[nMid], pData );
        if ( nResult > 0 )
        {
            nHigh = nMid - 1;
        }
        else if ( nResult < 0 )
        {
            nLow = nMid + 1;
        }
        else
        {
            /* 已经发现了匹配数据，返回 */
            return nMid;
        }
    }
    if ( nMid == nLow )
    {
        return nLow;
    }
    else if ( nMid == nHigh )
    {
        if ( nMid < (int)(uCount - 1))
        {
            return nMid + 1;
        }
        else
        {
            return -1;  //表中没有比待查找数据大的数据
        }
    }
    else
    {
        return nHigh;
    }
}

const int g_ncore = omp_get_num_procs();//获取执行核的数量


/**	计算循环迭代需要的线程数量
    根据循环迭代次数和CPU核数及一个线程最少需要的循环迭代次数
    来计算出需要的线程数量，计算出的最大线程数量不超过CPU核数

    @param	int n - 循环迭代次数	
    @param	int min_n - 单个线程需要的最少迭代次数	
    @return	int - 线程数量，（最大等于1倍的CPU核数）	
*/
int dtn(int n, int min_n)
{
    int max_tn = n / min_n;
    int tn = max_tn > g_ncore ? g_ncore : max_tn;  //tn表示要设置的线程数量
    if ( tn < 1 )
    {
        tn = 1;
    }
    return tn;
}


/**	计算循环迭代需要的线程数量
    根据循环迭代次数和CPU核数及一个线程最少需要的循环迭代次数
    来计算出需要的线程数量，计算出的最大线程数量不超过CPU核数的两倍

    @param	int n - 循环迭代次数	
    @param	int min_n - 单个线程需要的最少迭代次数	
    @return	int - 线程数量，（最大等于2倍的CPU核数）	
*/
int dtn2(int n, int min_n)
{
    int max_tn = n / min_n;
    int tn = max_tn > 2*g_ncore ? 2*g_ncore : max_tn;  //tn表示要设置的线程数量
    if ( tn < 1 )
    {
        tn = 1;
    }
    return tn;
}



/**	计算给定地址之后的第1个Cache行首地址
    如果给定地址刚好为一个Cache行首地址，那么计算结果等于Cache行首地址

    @param	void *pAddr - 给定的地址	
    @return	void * - 返回给定地址之后的第1个Cache行首地址	
*/
void *GetCacheAlignedAddr(void *pAddr)
{
    int  m = CACHE_LINE_SIZE;

    void *pRet = (void *)(((UINT)pAddr+m-1)&(-m));

    return pRet;
}


/**	计算给定地址之后的第1个Cache行首地址
如果给定地址刚好为一个Cache行首地址，那么计算结果等于它的下一个Cache行首地址

@param	void *pAddr - 给定的地址	
@return	void * - 返回给定地址之后的第1个Cache行首地址	
*/
void *GetNextCacheAlignedAddr(void *pAddr)
{
    int  m = CACHE_LINE_SIZE;

    void *pRet = (void *)(((UINT)(pAddr) + m )&(-m));

    return pRet;
}

