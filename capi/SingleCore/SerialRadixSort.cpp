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


struct DATA_ARRAY {
    void **ppData;
    UINT   uCount;
};


struct RADIX_BOX2 {
    DATA_ARRAY *pDataArray;
    UINT	uBoxLen;
};

void **Serial_Partitioned(void **ppData, UINT uDataLen,UINT uRadix, 
                          UINT uKeyIndex, GETKEYFUNC GetKeyFunc,
                          UINT *puBoxDataCount, void **ppOutData,
                          DATA_ARRAY *pArray)
{
    UINT    i;
    UINT	uIndex;

    for ( i = 0; i < uRadix; i++ )
    {
        puBoxDataCount[i] = 0;
    }

    // 计算各个箱子中的元素数量
    for ( i = 0; i < uDataLen; i++ )
    {
        uIndex = (*GetKeyFunc)(ppData[i],uKeyIndex);
        puBoxDataCount[uIndex] += 1;
    }


    // 计算各个箱子在数组中的位置
    void **pp = ppOutData;
    for ( i = 0; i < uRadix; i++ )
    {
        pArray[i].ppData = pp;
        pp += puBoxDataCount[i];
        pArray[i].uCount = 0;
    }

    // 将数据分配到各个箱子里
    for ( i = 0; i < uDataLen; i++ )
    {
        uIndex = (*GetKeyFunc)(ppData[i],uKeyIndex);

        pArray[uIndex].ppData[pArray[uIndex].uCount] = ppData[i];
        pArray[uIndex].uCount += 1;
    }

    return ppOutData;
}


void Serial_RadixSort(void **ppData, UINT uDataLen, UINT uRadix,
                      UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{
    UINT	 *puBoxDataCount = (UINT *)malloc(sizeof(UINT) * uRadix);
    void **ppOutData = (void **)malloc(sizeof(void *) * uDataLen);
    DATA_ARRAY *pArray = (DATA_ARRAY *)malloc(sizeof(DATA_ARRAY) * uRadix);

    UINT i;

    void **ppInData;
    void **ppTempData;

    ppInData = ppData;

    for ( i = 0; i < uMaxKeyLen; i++ )
    {
        Serial_Partitioned(ppInData, uDataLen, uRadix, i, GetKeyFunc, 
            puBoxDataCount, ppOutData, pArray);
        ppTempData = ppInData;
        ppInData = ppOutData;
        ppOutData = ppTempData;
    }

    if ( ppOutData == ppData )
    {
        free( ppInData );
    }
    else
    {
        for ( i = 0; i < uDataLen; i++ )
        {
            ppData[i] = ppOutData[i];
        }
        free(ppOutData);
    }
    free(puBoxDataCount);
    free(pArray);
}
