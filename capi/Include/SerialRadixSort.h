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
 *	SerialRadixSort.h
 *
 *	DESCRIPTION
 *		串行基数排序模块.
 *
 *	HISTORY
 *		09-03-2008	create by zhouweiming.
 *
 */

#ifndef __SERIAL_RADIXSORT_H__
#define __SERIAL_RADIXSORT_H__

#include <stdlib.h>
#include <capiglobal.h>


typedef UINT (*GETUINTKEYFUNC)(UINT key, UINT index);


template <class T>
struct DATA_ARRAY_TEMP {
    T      *pData;
    UINT   uCount;
};

UINT GetHalfByteKey( UINT uData, UINT uKeyIndex );
UINT GetByteKey( UINT uData, UINT uKeyIndex );


/**	对数组进行基数排序的分区函数

    @param	T *pData - 待排序数据	
    @param	UINT uDataLen - 数据长度	
    @param	UINT uRadix - 基数	
    @param	UINT uKeyIndex - 关键词索引（第几个基数位）	
    @param	GETKEY GetKeyFunc - 关键词取位回调函数	
    @param	UINT *puBoxDataCount - OUT, 记录每个箱子数据个数的数组	
    @param	T *pOut - OUT，存放分区结束后的数据
    @param	DATA_ARRAY_TEMP<T> *pArray - OUT，输出分区后的各个箱子数据信息
    @return	T * - 返回pOut指针	
*/
template <class T,class GETKEY>
T *Serial_Partitioned(T *pData, UINT uDataLen,UINT uRadix, 
                          UINT uKeyIndex, GETKEY GetKeyFunc,
                          UINT *puBoxDataCount, T *pOutData,
                          DATA_ARRAY_TEMP<T> *pArray)
{
    UINT    i;
    UINT	uIndex;

    if ( uDataLen == 0 )
    {
        return pOutData;
    }

    for ( i = 0; i < uRadix; i++ )
    {
        puBoxDataCount[i] = 0;
    }

    // 计算各个箱子中的元素数量，存放在puBoxDataCount数组中
    for ( i = 0; i < uDataLen; i++ )
    {
        uIndex = (*GetKeyFunc)(pData[i],uKeyIndex);
        puBoxDataCount[uIndex] += 1;
    }

    // 计算各个箱子在数组中的位置
    T *p = pOutData;
    for ( i = 0; i < uRadix; i++ )
    {
        pArray[i].pData = p;
        p += puBoxDataCount[i];
        pArray[i].uCount = 0;
    }

    // 将数据分配到各个箱子里
    for ( i = 0; i < uDataLen; i++ )
    {
        uIndex = (*GetKeyFunc)(pData[i],uKeyIndex);

        pArray[uIndex].pData[pArray[uIndex].uCount] = pData[i];
        pArray[uIndex].uCount += 1;
    }

    return pOutData;
}


/**	对数组进行串行基数排序的函数
    排序的结果仍然保存在pData数组中

    @param	T *pData - 待排序数据	
    @param	UINT uDataLen - 数据长度	
    @param	UINT uRadix - 基数	
    @param	UINT uMaxKeyLen - 最大关键词长度，比如32位整数以65536为基数时，
                              最大关键词长度为2	
    @param	GETKEY GetKeyFunc - 关键词取位回调函数	
    @return	void - 无	
*/
template <class T, class GETKEY>
void Serial_RadixSort(T *pData, UINT uDataLen, UINT uRadix,
                      UINT uMaxKeyLen, GETKEY GetKeyFunc)
{
    UINT	 *puBoxDataCount = (UINT *)malloc(sizeof(UINT) * uRadix);
    T *pOutData =  new T[uDataLen];
    DATA_ARRAY_TEMP<T> *pArray = new DATA_ARRAY_TEMP<T>[uRadix];

    UINT i;

    T *pInData;
    T *pTempData;

    pInData = pData;
 //   clock_t     t1, t2;

    for ( i = 0; i < uMaxKeyLen; i++ )
    {
 //       t1 = clock();
        Serial_Partitioned(pInData, uDataLen, uRadix, i, GetKeyFunc, 
            puBoxDataCount, pOutData, pArray);
        pTempData = pInData;
        pInData = pOutData;
        pOutData = pTempData;
  //      t2 = clock();

  //      printf( "Serial_RadixSort, Serial_Partitioned, time = %ld\n", t2 - t1);
    }

    if ( pInData != pData )
    {
        for ( i = 0; i < uDataLen; i++ )
        {
            pData[i] = pOutData[i];
        }
    }
    delete []pOutData;
    delete [] puBoxDataCount;
    delete [] pArray;
}


#endif // __SERIAL_RADIXSORT_H__

