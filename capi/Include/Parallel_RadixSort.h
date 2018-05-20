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


/**	����ƽ��Ĳ��л�������
    ��������в��л�������

    @param	T *ppData - ����������	
    @param	UINT uDataLen - ���ݳ���	
    @param	UINT uRadix - ����	
    @param	UINT uMaxKeyLen - ���ؼ��ʳ���	
    @param	GETKEY GetKeyFunc - �ؼ���ȡλ�ص�����	
    @return	void - ��	
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
        //����������ȫ�ֵ�һ�ֽ����ݷ��䵽������Ĳ���

#pragma omp parallel for num_threads(nProcessors) schedule(static, 1)
        for ( i = 0; i < nProcessors; i++ )
        {
            //ÿ���̼߳����Լ���������ʼλ��,����end�ǿ�����λ��
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
#pragma omp parallel for num_threads(dtn(uRadix, 4096)) //��Сѭ��������Ϊ������Ҫ����
        for ( i = 0; i < (int)uRadix; i++ )
        {
            int k;
            for ( k = 1; k < nProcessors; k++ )
            {
                UINT * pk = ppuBoxDataCount[k];
                puCount[i] += pk[i];
            }
        }

        //����ǰ׺��
        for ( i = 1; i < (int)uRadix; i++ )
        {
            puCount[i] += puCount[i-1];
        }  
       
        //�õ��������ݵ���ʼλ��
        for ( i = uRadix - 1; i > 0; i-- )
        {
            puCount[i] = puCount[i-1];
        }
        puCount[0] = 0;

        //�ռ���������pOutData����������·Ż�ppData��
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
        //������һ�ֵķ������ռ�����
        t2 = clock();

        printf( "Parallel Collected, time = %ld\n", t2 - t1);

    }	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )

    //�ͷű�
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


/**	�����Ĳ��л�������
    ��������в��л�������

    @param	T *ppData - ����������	
    @param	UINT uDataLen - ���ݳ���	
    @param	UINT uRadix - ����	
    @param	UINT uMaxKeyLen - ���ؼ��ʳ���	
    @param	GETKEY GetKeyFunc - �ؼ���ȡλ�ص�����	
    @return	void - ��	
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


        //����������ȫ�ֵ�һ�ֽ����ݷ��䵽������Ĳ���

#pragma omp parallel for num_threads(nProcessors) schedule(static, 1)
        for ( i = 0; i < nProcessors; i++ )
        {
            //ÿ���̼߳����Լ���������ʼλ��,����end�ǿ�����λ��
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
#pragma omp parallel for num_threads(dtn(uRadix, 4096)) //��Сѭ��������Ϊ������Ҫ����
        for ( i = 0; i < (int)uRadix; i++ )
        {
            int k;
            for ( k = 1; k < nProcessors; k++ )
            {
                UINT * pk = ppuBoxDataCount[k];
                puCount[i] += pk[i];
            }
        }

        //����ǰ׺��
        for ( i = 1; i < (int)uRadix; i++ )
        {
            puCount[i] += puCount[i-1];
        }
       
        //�õ��������ݵ���ʼλ��
        for ( i = uRadix - 1; i > 0; i-- )
        {
            puCount[i] = puCount[i-1];
        }
        puCount[0] = 0;

        //�ռ���������pOutData����������·Ż�pData��
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
    

    //�ͷű�
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