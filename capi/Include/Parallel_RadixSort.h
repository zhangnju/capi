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
 *	Parallel_RadixSort.h
 *
 *	DESCRIPTION
 *		Interface to a Load Balanced Parallel RadixSort function.
 *	HISTORY
 *		10-11-2007	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_RADIXSORT_H__
#define __PARALLEL_RADIXSORT_H__

#include <omp.h>
#include "CapiGlobal.h"
#include "CapiCommon.h"
#include "SerialRadixSort.h"
#include "CQuickSortRange.h"


/**	负载平衡的并行基数排序
    对数组进行并行基数排序

    @param	T *ppData - 待排序数据	
    @param	UINT uDataLen - 数据长度	
    @param	UINT uRadix - 基数	
    @param	UINT uMaxKeyLen - 最大关键词长度	
    @param	GETKEY GetKeyFunc - 关键词取位回调函数	
    @return	void - 无	
*/
template <class T, class GETKEY>
void Parallel_RadixSort_LBR(T *pData, UINT uDataLen,
                               UINT uRadix, UINT uMaxKeyLen, GETKEY GetKeyFunc)
{
    int i;
    int nProcessors = omp_get_num_procs();
    UINT uCount = uDataLen / nProcessors;

    if ( uDataLen - uCount * nProcessors > 2 )
    {
        uCount++;
    }

    T *pOutData = new T[uDataLen];
    DATA_ARRAY_TEMP<T>  **ppDataArray = new DATA_ARRAY_TEMP<T> *[nProcessors];
    UINT **ppuBoxDataCount = new UINT *[nProcessors];

    for ( i = 0; i < nProcessors; i++ )
    {
        ppDataArray[i] = new DATA_ARRAY_TEMP<T>[uRadix];
        ppuBoxDataCount[i] = new UINT[uRadix];
    }

    UINT uKeyIndex;

    for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
    {
        clock_t     t1, t2;
        //下面代码完成全局的一轮将数据分配到箱子里的操作

#pragma omp parallel for num_threads(nProcessors) schedule(static, 1)
        for ( i = 0; i < nProcessors; i++ )
        {
            //每个线程计算自己的数据起始位置,其中end是开区间位置
            UINT begin = i * uCount;
            UINT end = (i+1) * uCount;
            if ( end > uDataLen )
            {
                end = uDataLen;
            }

            T * pOut = pOutData + begin;

            T *p = pData + begin;
        t1 = clock();
           Serial_Partitioned<T>(p, end-begin, uRadix, uKeyIndex, GetKeyFunc, 
                ppuBoxDataCount[i], pOut, ppDataArray[i]);
        t2 = clock();

        printf( "Serial_Partitioned, ThreadId = %ld, time = %ld\n", omp_get_thread_num(), t2 - t1);

        }//for ( i = 0; i < nProcessors; i++ )

        UINT *puCount = ppuBoxDataCount[0];
#pragma omp parallel for num_threads(dtn(uRadix, 4096)) //最小循环次数设为多少需要测试
        for ( i = 0; i < (int)uRadix; i++ )
        {
            int k;
            for ( k = 1; k < nProcessors; k++ )
            {
                UINT * pk = ppuBoxDataCount[k];
                puCount[i] += pk[i];
            }
        }

        //计算前缀和
        for ( i = 1; i < (int)uRadix; i++ )
        {
            puCount[i] += puCount[i-1];
        }  
       
        //得到各段数据的起始位置
        for ( i = uRadix - 1; i > 0; i-- )
        {
            puCount[i] = puCount[i-1];
        }
        puCount[0] = 0;

        //收集操作，将pOutData里的数据重新放回ppData里
        int m;
        t1 = clock();

#pragma omp parallel for private(i) schedule(dynamic)
        for ( m = 0; m < (int)uRadix; m++ )
        {
            int nIndex = puCount[m];
            for ( i = 0; i < nProcessors; i++ )
            {
                DATA_ARRAY_TEMP<T> *pDataArray = ppDataArray[i];
                T *pDataTemp = pDataArray[m].pData;
                int k;
                for ( k = 0; k < (int)pDataArray[m].uCount; k++ )
                {
                    pData[nIndex] = pDataTemp[k];
                    ++nIndex;
                }
            }
        }
        //进行下一轮的分配与收集操作
        t2 = clock();

        printf( "Parallel Collected, time = %ld\n", t2 - t1);

    }	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )

    //释放表
    for ( i = 0; i < nProcessors; i++ )
    {
        delete [] ppDataArray[i];
        delete [] ppuBoxDataCount[i];
    }

    delete [] ppDataArray;
    delete [] ppuBoxDataCount;
    delete [] pOutData;
    return;
}


/**	分区的并行基数排序
    对数组进行并行基数排序

    @param	T *ppData - 待排序数据	
    @param	UINT uDataLen - 数据长度	
    @param	UINT uRadix - 基数	
    @param	UINT uMaxKeyLen - 最大关键词长度	
    @param	GETKEY GetKeyFunc - 关键词取位回调函数	
    @return	void - 无	
*/
template <class T, class GETKEY>
void Parallel_RadixSort_PPR(T *pData, UINT uDataLen,
                               UINT uRadix, UINT uMaxKeyLen, GETKEY GetKeyFunc)
{
    int i;
    int nProcessors = omp_get_num_procs();
    UINT uCount = uDataLen / nProcessors;

    if ( uDataLen - uCount * nProcessors > 2 )
    {
        uCount++;
    }

    T *pOutData = new T[uDataLen];
    DATA_ARRAY_TEMP<T>  **ppDataArray = new DATA_ARRAY_TEMP<T> *[nProcessors];
    UINT **ppuBoxDataCount = new UINT *[nProcessors];

    for ( i = 0; i < nProcessors; i++ )
    {
        ppDataArray[i] = new DATA_ARRAY_TEMP<T>[uRadix];
        ppuBoxDataCount[i] = new UINT[uRadix+1];
    }


        //下面代码完成全局的一轮将数据分配到箱子里的操作

#pragma omp parallel for num_threads(nProcessors) schedule(static, 1)
        for ( i = 0; i < nProcessors; i++ )
        {
            //每个线程计算自己的数据起始位置,其中end是开区间位置
            UINT begin = i * uCount;
            UINT end = (i+1) * uCount;
            if ( end > uDataLen )
            {
                end = uDataLen;
            }

            T * pOut = pOutData + begin;

            T *p = pData + begin;
           Serial_Partitioned<T>(p, end-begin, uRadix, uMaxKeyLen-1, GetKeyFunc, 
                ppuBoxDataCount[i], pOut, ppDataArray[i]);

        }//for ( i = 0; i < nProcessors; i++ )

        UINT *puCount = ppuBoxDataCount[0];
#pragma omp parallel for num_threads(dtn(uRadix, 4096)) //最小循环次数设为多少需要测试
        for ( i = 0; i < (int)uRadix; i++ )
        {
            int k;
            for ( k = 1; k < nProcessors; k++ )
            {
                UINT * pk = ppuBoxDataCount[k];
                puCount[i] += pk[i];
            }
        }

        //计算前缀和
        for ( i = 1; i < (int)uRadix; i++ )
        {
            puCount[i] += puCount[i-1];
        }
       
        //得到各段数据的起始位置
        for ( i = uRadix - 1; i > 0; i-- )
        {
            puCount[i] = puCount[i-1];
        }
        puCount[0] = 0;

        //收集操作，将pOutData里的数据重新放回pData里
        int m;

#pragma omp parallel for private(i) schedule(dynamic)
        for ( m = 0; m < (int)uRadix; m++ )
        {
            int nIndex = puCount[m];
            for ( i = 0; i < nProcessors; i++ )
            {
                DATA_ARRAY_TEMP<T> *pDataArray = ppDataArray[i];
                T *pDataTemp = pDataArray[m].pData;
                int k;
                for ( k = 0; k < (int)pDataArray[m].uCount; k++ )
                {
                    pData[nIndex] = pDataTemp[k];
                    ++nIndex;
                }
            }
        }

    puCount[uRadix] = uDataLen;

//#pragma omp parallel for
    for( m = 0; m < (int)uRadix; m++ )
    {
        T *pInData = pData + puCount[m];
        T *pOut = pOutData + puCount[m];
        T *pTempData;
        UINT *puBoxData = new UINT[uRadix];
        DATA_ARRAY_TEMP<T> *pDataArray = new DATA_ARRAY_TEMP<T>[uRadix];

        for ( i = 0; i < (int)uMaxKeyLen - 1; i++ )
        {
            Serial_Partitioned(pInData, puCount[m+1] - puCount[m], uRadix, i, GetKeyFunc, 
                puBoxData, pOut, pDataArray);
            pTempData = pInData;
            pInData = pOut;
            pOut = pTempData;
        }
        if ( pOut != (pData + puCount[m]) )
        {
            for ( i = (int)puCount[m]; i < (int)puCount[m+1]; i++ )
            {
                pData[i] = pOutData[i];
            }
        }
        delete[] puBoxData;
        delete[] pDataArray;
    }
    

    //释放表
    for ( i = 0; i < nProcessors; i++ )
    {
        delete [] ppDataArray[i];
        delete [] ppuBoxDataCount[i];
    }

    delete [] ppDataArray;
    delete [] ppuBoxDataCount;
    delete [] pOutData;
    return;
}


#endif //__PARALLEL_RADIXSORT_H__