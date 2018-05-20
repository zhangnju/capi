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
 *		���л�������ģ��.
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


/**	��������л�������ķ�������

    @param	T *pData - ����������	
    @param	UINT uDataLen - ���ݳ���	
    @param	UINT uRadix - ����	
    @param	UINT uKeyIndex - �ؼ����������ڼ�������λ��	
    @param	GETKEY GetKeyFunc - �ؼ���ȡλ�ص�����	
    @param	UINT *puBoxDataCount - OUT, ��¼ÿ���������ݸ���������	
    @param	T *pOut - OUT����ŷ��������������
    @param	DATA_ARRAY_TEMP<T> *pArray - OUT�����������ĸ�������������Ϣ
    @return	T * - ����pOutָ��	
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

    // ������������е�Ԫ�������������puBoxDataCount������
    for ( i = 0; i < uDataLen; i++ )
    {
        uIndex = (*GetKeyFunc)(pData[i],uKeyIndex);
        puBoxDataCount[uIndex] += 1;
    }

    // ������������������е�λ��
    T *p = pOutData;
    for ( i = 0; i < uRadix; i++ )
    {
        pArray[i].pData = p;
        p += puBoxDataCount[i];
        pArray[i].uCount = 0;
    }

    // �����ݷ��䵽����������
    for ( i = 0; i < uDataLen; i++ )
    {
        uIndex = (*GetKeyFunc)(pData[i],uKeyIndex);

        pArray[uIndex].pData[pArray[uIndex].uCount] = pData[i];
        pArray[uIndex].uCount += 1;
    }

    return pOutData;
}


/**	��������д��л�������ĺ���
    ����Ľ����Ȼ������pData������

    @param	T *pData - ����������	
    @param	UINT uDataLen - ���ݳ���	
    @param	UINT uRadix - ����	
    @param	UINT uMaxKeyLen - ���ؼ��ʳ��ȣ�����32λ������65536Ϊ����ʱ��
                              ���ؼ��ʳ���Ϊ2	
    @param	GETKEY GetKeyFunc - �ؼ���ȡλ�ص�����	
    @return	void - ��	
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

