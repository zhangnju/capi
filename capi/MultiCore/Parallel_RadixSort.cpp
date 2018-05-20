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
#include <stdio.h>

#include <omp.h>
#include <capiglobal.h>
#include <singlelist.h>
#include <sorttable.h>
#include "ArrayList.h"
#include "SerialRadixSort.h"
#include "Parallel_RadixSort_Array.h"


typedef struct RADIX_BOX_st {
    SORTTABLE	**ppTable;
    UINT		uBoxLen; 
} RADIX_BOX;

struct ARRAYLIST_ARRAY {
	ARRAYLIST	**ppList;
	UINT		uRadix;
	int			nProcessors;
	UINT		uDataBlockLen;
};

struct RADIXBOX_POSITION {
	UINT	uBoxArrayIndex;
	UINT	uArrayListIndex;
	UINT	uPos;
};


/**	获取整数的第uKeyIndex位数字，以16进制为单位
因此获取到的数字最大为0xf，作基数排序时，
基数为16，最大关键词长度为4

@param	UINT uData - 整数值	
@param	UINT uKeyIndex - 16进制整数的第多少位，32位整数最大位数为8	
@return	UINT - 返回第uKeyIndex位的数字	
*/
UINT GetHalfByteKey( UINT uData, UINT uKeyIndex )
{
    unsigned char r;
    if ( uKeyIndex > 8 )
    {
        return 0;
    }

    r = (uData >> ((uKeyIndex - 1)*4)) & 0xf;

    return r;   
}



/**	获取整数的第uKeyIndex位数字，以256进制为单位
因此获取到的数字最大为0xff，作基数排序时，
基数为256，最大关键词长度为8

@param	UINT uData - 整数值	
@param	UINT uKeyIndex - 256进制整数的第多少位，32位整数最大位数为4	
@return	UINT - 返回第uKeyIndex位的数字	
*/
UINT GetByteKey( UINT uData, UINT uKeyIndex )
{
    unsigned char r;
    if ( uKeyIndex > 4 )
    {
        return 0;
    }

    r = (uData >> ((uKeyIndex - 1)*8)) & 0xff;

    return r;   
}



/**	对链表的数据的第uKeyIndex位上的元素进行分类，
    依照它们的大小放入对应的箱子中

	@param	SINGLELIST *pSingleList - 单向链表指针	
	@param	UINT       uRadix - 基数排序的基数，与具体数据类型有关，
                                一般来讲整数的基数为16，字符串的基数最大为255。
	@param	UINT       uKeyIndex - 第多少位	
	@param	SINGLENODE **ppHead - 用来记录头指针的箱子	
	@param	SINGLENODE **ppTail - 记录箱子的尾指针	
	@param	GETKEYFUNC GetKeyFunc - 获取数据的第uKeyIndex位上的元素值	
	@return	void - 无	
*/
static void SingleList_Distribute(SINGLELIST *pSingleList,
                          UINT       uRadix,
                          UINT       uKeyIndex,
                          SINGLENODE **ppHead,
                          SINGLENODE **ppTail,
                          GETKEYFUNC GetKeyFunc )
{
    SINGLENODE   *pNode;
    UINT         i;

    /* 初始化子表 */
    for ( i = 0; i < uRadix; i++ )
    {
        ppHead[i] = NULL;
        ppTail[i] = NULL;
    }

    pNode = pSingleList->pHead;

    while ( pNode != NULL )
    {
        UINT uRadixIndex = (*GetKeyFunc)(pNode->pData, uKeyIndex);
         
        if ( ppHead[uRadixIndex] == NULL )
        {
           ppHead[uRadixIndex] = pNode;
           ppTail[uRadixIndex] = pNode;
           pNode = pNode->pNext;
           ppTail[uRadixIndex]->pNext = NULL;
        }
        else
        {
            ppTail[uRadixIndex]->pNext = pNode;
            ppTail[uRadixIndex] = ppTail[uRadixIndex]->pNext;
            pNode = pNode->pNext;
            ppTail[uRadixIndex]->pNext = NULL;
        }
    }
}


/**	基数排序的收集操作

	@param	SINGLENODE **ppHead - 存放各个箱子的头指针数组	
	@param	SINGLENODE **ppTail - 存放各个箱子的尾指针数组	
	@param	UINT uRadix - 基数	
	@return	SINGLELIST * - 收集完成后返回收集好的链表
*/
SINGLELIST * Collect(SINGLENODE **ppHead, SINGLENODE **ppTail, UINT uRadix)
{
	UINT			i;

	SINGLELIST	*pList = SingleList_Create();
	SINGLENODE	*pHead;
	SINGLENODE  *pTail;

	//将uRadix条链连接成一条链表
	for ( i = 0; i < uRadix; i++ )
	{
		//查找第一个非空的子表
		if ( ppHead[i] != NULL )
		{
			break;
		}
	}
	if ( ppHead[i] == NULL)
	{
		//没有找到非空子表,除非传入的待排序链表中没有数据才会出现这种情况
		return NULL;
	}

	pHead = ppHead[i];
	pTail = ppTail[i];

	for ( i += 1; i < uRadix; i++ )
	{
		//继续查找非空的子表
		if ( ppHead[i] == NULL )
		{
			continue;
		}
		else
		{
			pTail->pNext = ppHead[i];
			pTail = ppTail[i];
		}
	}
	pList->pHead = pHead;
	pList->pTail = pTail;

	return pList;
}


/**	将一条链表劈成若干条链表

	@param	SINGLELIST *pSingleList - 链表	
	@param	UINT uListNum - 链表数量	
	@param	SINGLELIST ***pppList - 劈成的链表数量	
	@return	SINGLELIST ** - 返回劈成后的链表数组	
*/
SINGLELIST ** SplitListToArrayList(SINGLELIST *pSingleList,UINT uListNum, SINGLELIST ***pppList)
{
	UINT i;
	UINT uCount = pSingleList->uCount / uListNum;

	SINGLENODE	*pCur = pSingleList->pHead;
	SINGLELIST	**ppList = *pppList;

	// split the pSingleList to a singlelist array ppList
	// every singlelist in ppList have the same data count except for the last.
	for ( i = 0; i < uListNum - 1; i++ )
	{
		UINT		uNodeCount = 1;

		ppList[i]->pHead = pCur;

		while ( pCur->pNext != NULL && uNodeCount < uCount )
		{
			pCur = pCur->pNext;
			uNodeCount++;
		}
		ppList[i]->pTail = pCur;
		pCur = pCur->pNext;
		ppList[i]->pTail->pNext = NULL;
		ppList[i]->uCount = uNodeCount;
	}

	ppList[i]->pHead = pCur;
	ppList[i]->pTail = pSingleList->pTail;
	ppList[i]->uCount = pSingleList->uCount - uCount * (uListNum - 1);

	return ppList;
}

/**	并行基数排序函数
    对单向链表内的数据进行并行基数排序

	@param	SINGLELIST *pSingleList - 要排序的链表	
	@param	UINT uRadix - 基数	
	@param	UINT uMaxKeyLen - 最大关键词长度	
	@param	GETKEYFUNC GetKeyFunc - 关键词取位的回调函数	
	@return	void - xxxx	
*/
void Parallel_RadixSort(SINGLELIST *pSingleList,  
						UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{
	int i;
	SINGLELIST *pList;
	int nProcessors = (UINT)omp_get_num_procs();
    if ( pSingleList == NULL || pSingleList->uCount == 0 )
    {
        return;
    }
	UINT uCount = pSingleList->uCount / nProcessors;


	if ( pSingleList->uCount - uCount * nProcessors > 2 )
	{
		uCount++;
	}

	SINGLELIST **ppList = (SINGLELIST **)malloc(nProcessors * sizeof(SINGLELIST *));
	if ( ppList == NULL )
	{
		return;
	}
	for ( i = 0; i < nProcessors; i++ )
	{
		ppList[i] = SingleList_Create();
	}


	SINGLENODE	***pppHead;
	SINGLENODE	***pppTail;
	
	pppHead = (SINGLENODE ***)malloc(nProcessors * sizeof(void **));
	if ( pppHead == NULL )
	{
		free(ppList);
		return;
	}
	else
	{
		pppTail = (SINGLENODE ***)malloc(nProcessors * sizeof(void **));
		if ( pppTail == NULL )
		{
			free(ppList);
			free(pppHead);
			return;
		}
	}
#pragma omp for
	for ( i = 0; i < nProcessors; i++ )
	{
		/* 给箱子申请内存 */
		pppHead[i] = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
		pppTail[i] = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
#if 0
		//没有在失败时释放内存，存在内存泄漏
		if ( pppHead[i] == NULL || pppTail[i] == NULL )
		{
			free(ppList);
			free(pppHead);
			free(pppTail);
			return;
		}
#endif
		UINT index;
		SINGLENODE	**ppHead;
		SINGLENODE	**ppTail;

		ppHead = pppHead[i];
		ppTail = pppTail[i];
		for ( index = 0; index < uRadix; index++ )
		{
			ppHead[index] = NULL;
			ppTail[index] = NULL;
		}
	}

	UINT k;
	for ( k = 0; k < uMaxKeyLen; k++ )
	{
		// 将一个链表拆分成多个链表，拆分好的多个链表放入ppList数组中
		SplitListToArrayList(pSingleList, nProcessors, &ppList);
#pragma omp parallel for 
		for ( i = 0; i < nProcessors; i++ )
		{
			// 对拆分好的链表数组进行并行分配操作
			SingleList_Distribute(ppList[i], uRadix, k, pppHead[i], pppTail[i],GetKeyFunc);
		}
		//进行转置操作,将各个线程的位数相同的箱子连成一条链，共uRadix条链
#pragma omp parallel for
		for ( i = 0; i < (int)uRadix; i++)
		{
			int j;
			SINGLENODE **ppTail0 = pppTail[0];
			SINGLENODE **ppHead0 = pppHead[0];

			SINGLENODE **ppHead;
			SINGLENODE **ppTail;
			
			//查找第一个非空子表
			for ( j = 0; j < nProcessors; j++ )
			{
				ppHead = pppHead[j];
				if ( ppHead[i] != NULL )
				{
					ppTail = pppTail[j];
					break;
				}
			}
			if ( ppHead[i] == NULL )
			{
				//没有找到非空子表
				ppHead0[i] = NULL;
				ppTail0[i] = NULL;
			}
			else
			{
				ppHead0[i] = ppHead[i];
				ppTail0[i] = ppTail[i];
			}

			for ( j += 1; j < nProcessors; j++ )
			{
				SINGLENODE **ppTempHead = pppHead[j];

				if ( ppTempHead[i] == NULL ) //为空表，里面没有数据
				{
					continue;
				}
				else
				{
					SINGLENODE **ppTempTail = pppTail[j];
					ppTail0[i]->pNext = ppTempHead[i];
					ppTail0[i] = ppTempTail[i];
				}
			}
		}//for ( i = 0; i < uRadix; k++)

		pList = Collect(pppHead[0], pppTail[0], uRadix);
		pSingleList->pHead = pList->pHead;
		pSingleList->pTail = pList->pTail;

		//注意不能调用SingleList_Destroy()函数来释放，否则会将链表中的节点释放掉
		free(pList);

		//至此已经重新连成了一条链表，下面需要重新将连接好的链表里的数据
		//拆分成nProcessors个子链表以分配到各个线程中,进行下一轮分配操作
	}//for ( k = 0; k < uMaxKeyLen; k++ )

	//下面将排序过程中使用的临时内存释放掉

	for ( i = 0; i < nProcessors; i++ )
	{
		//注意不能调用SingleList_Destroy()函数来释放，否则会将链表中的节点释放掉
		free(ppList[i]);
	}

	free((void *)ppList);
	for ( i = 0; i < nProcessors; i++ )
	{
		free(pppHead[i]);
		free(pppTail[i]);
	}
	free(pppHead);
	free(pppTail);
}




/**	填入描述

	@param	RADIX_BOX *pBox - xxxx	
	@param	UINT uRadix - xxxx	
	@return	void - xxxx	
*/
void RadixBox_Init(RADIX_BOX *pBox, UINT uRadix)
{
	pBox->ppTable = (SORTTABLE **)malloc(sizeof(SORTTABLE *) * uRadix);
	pBox->uBoxLen = uRadix;
}



/**	并行基数排序
对数组进行并行基数排序

@param	void **ppData - 待排序数据	
@param	UINT uDataLen - 数据长度	
@param	UINT uRadix - 基数	
@param	UINT uMaxKeyLen - 最大关键词长度	
@param	GETKEYFUNC GetKeyFunc - 关键词取位回调函数	
@return	void - 无	
*/
template <class T>
void Parallel_RadixSort_Array1(T *pData, UINT uDataLen,
                              UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{
    int i;
    int nProcessors = omp_get_num_procs();
    UINT uCount = uDataLen / nProcessors;

    if ( uDataLen - uCount * nProcessors > 2 )
    {
        uCount++;
    }

    T *pOutData = new T[uDataLen];
    DATA_ARRAY  **ppDataArray = new DATA_ARRAY *[nProcessors];
    int **ppuBoxDataCount = new int *[nProcessors];

    for ( i = 0; i < nProcessors; i++ )
    {
        ppDataArray[i] = new DATA_ARRAY;
        ppuBoxDataCount[i] = new int[uRadix];
    }

    UINT uKeyIndex;

    for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
    {
        UINT k;
        //下面代码完成全局的一轮将数据分配到箱子里的操作
        //#pragma omp parallel for private(k)	
        for ( i = 0; i < nProcessors; i++ )
        {
            UINT	*pDataSum = (UINT *)malloc(uRadix * sizeof(UINT));

            // 初始化元素计数为0
            for ( k = 0; k < uRadix; k++ )
            {
                pDataSum[k] = 0;
            }

            //每个线程计算自己的数据中各个位的元素个数
            UINT begin = i * uCount;
            UINT end = (i+1) * uCount;
            if ( end > uDataLen )
            {
                end = uDataLen;
            }

            T * pOut = pOutData + begin;

            void *p = pData + begin;
            Serial_Partitioned(p, end-begin, uRadix, uKeyIndex, GetKeyFunc, 
                ppuBoxDataCount[i], pOut, ppDataArray[i]);

        }//for ( i = 0; i < nProcessors; i++ )

        //收集操作，将pOutData里的数据重新放回ppData里


        for ( i = 0; i < nProcessors; i++ )
        {
            DATA_ARRAY *
            for ( k = 0; i < ppDataArray[i])
            ppDataArray[i].ppData[i]
        }


       //释放表
        for ( i = 0; i < nProcessors; i++ )
        {
            for ( k = 0; k < uRadix; k ++ )
            {
                SortTable_Destroy(pBox[i].ppTable[k], NULL);
            }
        }
        //进行下一轮的分配与收集操作
    }	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
}






/**	并行基数排序
    对数组进行并行基数排序

	@param	void **ppData - 待排序数据	
	@param	UINT uDataLen - 数据长度	
	@param	UINT uRadix - 基数	
	@param	UINT uMaxKeyLen - 最大关键词长度	
	@param	GETKEYFUNC GetKeyFunc - 关键词取位回调函数	
	@return	void - 无	
*/
void Parallel_RadixSort_Array(void **ppData, UINT uDataLen,
							  UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{
	int i;
	int nProcessors = omp_get_num_procs();
	UINT uCount = uDataLen / nProcessors;

	if ( uDataLen - uCount * nProcessors > 2 )
	{
		uCount++;
	}

	RADIX_BOX	*pBox = (RADIX_BOX *)malloc(sizeof(RADIX_BOX) * nProcessors);
    if ( pBox == NULL )
    {
        return;
    }

	void *pTemp = malloc(sizeof(SORTTABLE *) * uRadix * nProcessors);
    if ( pTemp == NULL )
    {
        free( pBox );
        return;
    }

	for ( i = 0; i < nProcessors; i++ )
	{
		pBox[i].ppTable = (SORTTABLE **)((char *)pTemp + i * sizeof(SORTTABLE *) * uRadix);
		pBox[i].uBoxLen = uRadix;
		//RadixBox_Init(&(pBox[i]), uRadix);
	}

	UINT uKeyIndex;

	for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
	{
		UINT k;
		//下面代码完成全局的一轮将数据分配到箱子里的操作
//#pragma omp parallel for private(k)	
		for ( i = 0; i < nProcessors; i++ )
		{
			UINT	*pDataSum = (UINT *)malloc(uRadix * sizeof(UINT));

			// 初始化元素计数为0
			for ( k = 0; k < uRadix; k++ )
			{
				pDataSum[k] = 0;
			}

			//每个线程计算自己的数据中各个位的元素个数
			UINT begin = i * uCount;
			UINT end = (i+1) * uCount;
			if ( end > uDataLen )
			{
				end = uDataLen;
			}
			for ( k = begin ; k < end; k++ )
			{
				UINT uPos = (*GetKeyFunc)(ppData[k], uKeyIndex);
				pDataSum[uPos] += 1;
			}


			//按计算出来的元素个数来分配箱子的空间大小
			for ( k = 0; k < uRadix; k++ )
			{
				pBox[i].ppTable[k] = SortTable_Create(pDataSum[k]);
			}

			// 将数据分配到各个箱子里
			for ( k = begin ; k < end; k++ )
			{
				UINT uPos = (*GetKeyFunc)(ppData[k], uKeyIndex);
				SortTable_Add(pBox[i].ppTable[uPos], ppData[k]);
			}
		}//for ( i = 0; i < nProcessors; i++ )

		//收集操作，将数据重新放回ppData里
		UINT uIndex = 0;
		for ( k = 0; k < uRadix; k++ )
		{
			for ( i = 0; i < nProcessors; i++ )
			{
				UINT j;
				SORTTABLE *pTable = pBox[i].ppTable[k];
				if ( pTable != NULL )
				{
					UINT uCount = pTable->uCursorCount;
					for ( j = 0; j < uCount; j++ )
					{
						ppData[uIndex] = pTable->ppData[j];
						uIndex++;
					}
				}
			}
		}
		//释放表
		for ( i = 0; i < nProcessors; i++ )
		{
			for ( k = 0; k < uRadix; k ++ )
			{
				SortTable_Destroy(pBox[i].ppTable[k], NULL);
			}
		}
		//进行下一轮的分配与收集操作
	}	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
}



/**	填入描述

	@param	UINT uRadix - xxxx	
	@param	int nProcessors - xxxx	
	@param	UINT uDataBlockLen - xxxx	
	@return	ARRAYLIST_ARRAY * - xxxx	
*/
ARRAYLIST_ARRAY *BoxArray_Create(UINT uRadix, int nProcessors, UINT uDataBlockLen)
{
	int i;
	ARRAYLIST_ARRAY *pBoxArray;
	pBoxArray = (ARRAYLIST_ARRAY *)malloc(sizeof(ARRAYLIST_ARRAY) * nProcessors);
	if ( pBoxArray == NULL )
	{
		return NULL;
	}

	for ( i = 0; i < nProcessors; i++ )
	{
		pBoxArray[i].ppList = (ARRAYLIST **)malloc(sizeof(ARRAYLIST *) * uRadix);
		UINT k;
		for ( k = 0; k < uRadix; k++ )
		{
			pBoxArray[i].ppList[k] = ArrayList_Create(uDataBlockLen);
		}
	}
	pBoxArray->nProcessors = nProcessors;
	pBoxArray->uDataBlockLen = uDataBlockLen;
	pBoxArray->uRadix = uRadix;
	return pBoxArray;
}


/**	填入描述

	@param	ARRAYLIST_ARRAY *pBoxArray - xxxx	
	@return	void - xxxx	
*/
void BoxArray_Destroy(ARRAYLIST_ARRAY *pBoxArray)
{
	int i;
	if ( pBoxArray == NULL )
	{
		return ;
	}

	for ( i = 0; i < pBoxArray->nProcessors; i++ )
	{
		UINT k;
		for ( k = 0; k < pBoxArray->uRadix; k++ )
		{
			ArrayList_Destroy(pBoxArray[i].ppList[k], NULL);
		}
		free(pBoxArray[i].ppList);
	}
	free(pBoxArray);
}


/**	填入描述

	@param	ARRAYLIST_ARRAY *pBoxArray - xxxx	
	@param	UINT uProcessors - xxxx	
	@param	UINT uCount - xxxx	
	@return	RADIXBOX_POSITION * - xxxx	
*/
RADIXBOX_POSITION *GetPosArray(ARRAYLIST_ARRAY *pBoxArray, UINT uProcessors, UINT uCount)
{
	UINT		uTotalSize = 0;
	UINT		uSize;
	UINT		uPosIndex;
	ARRAYLIST	*pList;
//	ARRAYNODE	*pNode;
	UINT		uBoxArrayIndex = 0;
	UINT		uArrayListIndex = 0;

	RADIXBOX_POSITION	*pPos = 
		(RADIXBOX_POSITION *)malloc(sizeof(RADIXBOX_POSITION)*(uProcessors+1));

	pList = pBoxArray[uBoxArrayIndex].ppList[uArrayListIndex];
	if ( pList->pTail != NULL )
	{
		uSize = (pList->uNodeCount - 1) * pList->uDataLen + pList->pTail->uCurPos;
	}
	else
	{
		uSize = 0;
	}
	uTotalSize += uSize;

	pPos[0].uArrayListIndex = 0;
	pPos[0].uBoxArrayIndex = 0;
	pPos[0].uPos = 0;

	uPosIndex = 1;

	while ( uPosIndex < uProcessors )
	{
		if ( uTotalSize < uCount )
		{
			uBoxArrayIndex++;
			if ( uBoxArrayIndex >= uProcessors )
			{
				uBoxArrayIndex = 0;
				uArrayListIndex++;
			}
			pList = pBoxArray[uBoxArrayIndex].ppList[uArrayListIndex];
			if ( pList->pTail != NULL )
			{
				uSize = (pList->uNodeCount - 1) * pList->uDataLen + pList->pTail->uCurPos;
			}
			else
			{
				uSize = 0;
			}
			uTotalSize += uSize;
			continue;
		}
		else
		{
			pPos[uPosIndex].uArrayListIndex = uArrayListIndex;
			pPos[uPosIndex].uBoxArrayIndex = uBoxArrayIndex;
			pPos[uPosIndex].uPos = uCount + uSize - uTotalSize;
			uPosIndex++;

			uTotalSize -= uCount;
		}
	}

	pPos[uPosIndex].uArrayListIndex = pBoxArray->uRadix - 1;
	pPos[uPosIndex].uBoxArrayIndex = uProcessors - 1;
	pList = pBoxArray[uProcessors - 1].ppList[pBoxArray->uRadix-1];
	if ( pList->pTail != NULL )
	{
		pPos[uPosIndex].uPos = (pList->uNodeCount - 1) * pList->uDataLen 
			+ pList->pTail->uCurPos;
	}
	else
	{
		pPos[uPosIndex].uPos = 0;
	}
	return pPos;
}


/**	填入描述

	@param	void **ppData - 待排序数据	
	@param	UINT uDataLen - 数据长度	
	@param	UINT uRadix - 基数	
	@param	UINT uMaxKeyLen - 关键词长度	
	@param	GETKEYFUNC GetKeyFunc - 关键词取位回调函数	
	@return	INT - CAPI_SUCCESS	
*/
INT Parallel_RadixSort_ArrayList(void **ppData, UINT uDataLen,
							  UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{
	UINT	i,j,k;
	ARRAYLIST_ARRAY	*pBoxArray;
	ARRAYLIST_ARRAY *pBoxArrayTemp;
	UINT nProcessors = omp_get_num_procs();
	UINT uCount = uDataLen / nProcessors;

	if ( uDataLen - uCount * nProcessors > 2 )
	{
		++uCount;
	}
	UINT	uDataBlockLen = uCount / uRadix + 128;
	pBoxArray = BoxArray_Create(uRadix, nProcessors, uDataBlockLen);
	
	UINT uKeyIndex = 0;

	// 进行第1轮分配操作
	for ( i = 0; i < nProcessors; i++ )
	{
		//计算各个线程操作数据的起始位置
		UINT begin = i * uCount;
		UINT end = (i+1) * uCount;
		if ( end > uDataLen )
		{
			end = uDataLen;
		}
		ARRAYLIST	*pList;
		ARRAYNODE	*pNode;

		for ( k = begin ; k < end; k++ )
		{
			UINT uPos = (*GetKeyFunc)(ppData[k], uKeyIndex);

			pList = pBoxArray[i].ppList[uPos];

			pNode = pList->pTail;

			if ( pNode->uCurPos < uDataBlockLen )
			{
				pNode->ppData[pNode->uCurPos] = ppData[k];
				pNode->uCurPos++;
			}
			else
			{
				pNode = ArrayNode_Create(uDataBlockLen);
				if ( pNode == NULL )
				{
					return CAPI_FAILED;
				}
				pList->pTail->pNext = pNode;
				pList->pTail = pNode;
				pList->uNodeCount++;

				pNode->ppData[0] = ppData[k];
				pNode->uCurPos = 1;
			}
		}

	}//第1轮分配操作结束

	for ( uKeyIndex = 1; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
	{
		RADIXBOX_POSITION	*pPos = GetPosArray(pBoxArray, nProcessors, uCount);
		pBoxArrayTemp = BoxArray_Create(uRadix, nProcessors, uDataBlockLen);

		// 进行第uKeyIndex轮分配操作
		for ( i = 0; i < nProcessors; i++ )
		{
			ARRAYLIST	*pList;
			ARRAYNODE	*pNode;
			//计算各个线程操作数据的起始位置
			UINT	uArrayListIndexStart = pPos[i].uArrayListIndex;
			UINT	uArrayListIndexEnd = pPos[i+1].uArrayListIndex;
			
			UINT	uBoxArrayIndexStart = pPos[i].uBoxArrayIndex;
			UINT	uBoxArrayIndexEnd = pPos[i+1].uBoxArrayIndex;

			UINT	uStartPos = pPos[i].uPos;
			UINT	uEndPos = pPos[i+1].uPos;

			pList = pBoxArray[uBoxArrayIndexStart].ppList[uArrayListIndexStart];
			pNode = pList->pHead;

			UINT uNodeNo = uStartPos / pList->uDataLen;
			UINT uStart = uStartPos - uNodeNo * pList->uDataLen;
			for ( k = 0; k < uNodeNo; k++ )
			{
				pNode = pNode->pNext;
			}

			while ( uArrayListIndexStart < uArrayListIndexEnd )
			{
				while ( uBoxArrayIndexStart < nProcessors )
				{
					while ( pNode != NULL )
					{
						for ( k = uStart; k < pNode->uCurPos; k++ )
						{
							UINT uPos = (*GetKeyFunc)(pNode->ppData[k], uKeyIndex);
							pList = pBoxArrayTemp[i].ppList[uPos];
							ArrayList_Add(pList, pNode->ppData[k]);

							//Add(pNode->ppData[i]);
						}
						pNode = pNode->pNext;
						uStart = 0;
					}
					++uBoxArrayIndexStart;
					pList = pBoxArray[uBoxArrayIndexStart].ppList[uArrayListIndexStart];
					pNode = pList->pHead;
				}
				++uArrayListIndexStart;
			}
			uBoxArrayIndexStart = 0;

			while ( uBoxArrayIndexStart < uBoxArrayIndexEnd )
			{
				pList = pBoxArray[uBoxArrayIndexStart].ppList[uArrayListIndexEnd];
				pNode = pList->pHead;

				while ( pNode != NULL )
				{
					for ( k = uStart; k < pNode->uCurPos; k++ )
					{
						UINT uPos = (*GetKeyFunc)(pNode->ppData[k], uKeyIndex);
						pList = pBoxArrayTemp[i].ppList[uPos];
						ArrayList_Add(pList, pNode->ppData[k]);
						//Add(pNode->ppData[i]);
					}
					pNode = pNode->pNext;
					uStart = 0;
				}
				++uBoxArrayIndexStart;
			}

			pList = pBoxArray[uBoxArrayIndexEnd].ppList[uArrayListIndexEnd];
			pNode = pList->pHead;
			uNodeNo = uEndPos / pList->uDataLen;

			for ( j = 0; j < uNodeNo; j++ )
			{
				for ( k = 0; k < pNode->uCurPos; k++)
				{
					UINT uPos = (*GetKeyFunc)(pNode->ppData[k], uKeyIndex);
					pList = pBoxArrayTemp[i].ppList[uPos];
					ArrayList_Add(pList, pNode->ppData[k]);
					//Add(pNode->ppData[i]);
				}
				pNode = pNode->pNext; 
			}
			if ( pNode != NULL )
			{
				UINT uEnd = uEndPos - uNodeNo* pList->uDataLen;
				for ( i = 0; i < uEnd; i++ )
				{
					UINT uPos = (*GetKeyFunc)(pNode->ppData[k], uKeyIndex);
					pList = pBoxArrayTemp[i].ppList[uPos];
					ArrayList_Add(pList, pNode->ppData[k]);
					//Add(pNode->ppData[i]);			
				}
			}
		}//第uKeyIndex轮分配操作结束
		
		//释放pBoxArray
		BoxArray_Destroy(pBoxArray);
		pBoxArray = pBoxArrayTemp;
	}
    return CAPI_SUCCESS;
}



/**	并行基数排序

	@param	void **ppData - 待排序数据	
	@param	UINT uDataLen - 数据长度	
	@param	UINT uRadix - 基数	
	@param	UINT uMaxKeyLen - 最大关键词长度	
	@param	GETKEYFUNC GetKeyFunc - 关键词取位回调函数	
	@return	void - 无	
*/
void Parallel_RadixSort_Array2(void **ppData, UINT uDataLen,
							  UINT uRadix, UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{
	int i;
	int nProcessors = omp_get_num_procs();
	UINT uCount = uDataLen / nProcessors;

	if ( uDataLen - uCount * nProcessors > 2 )
	{
		uCount++;
	}

	RADIX_BOX2	*pBox = (RADIX_BOX2 *)malloc(sizeof(RADIX_BOX2) * nProcessors);

	void *pTemp = malloc(sizeof(DATA_ARRAY *) * uRadix * nProcessors);

	for ( i = 0; i < nProcessors; i++ )
	{
		pBox[i].pDataArray = (DATA_ARRAY *)((char *)pTemp + i * sizeof(DATA_ARRAY) * uRadix);
		pBox[i].uBoxLen = uRadix;
	}

	void ** ppTempData = (void **)malloc( sizeof(void *) * uDataLen );

	UINT uKeyIndex;

	for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
	{
		UINT k;
		//下面代码完成全局的一轮将数据分配到箱子里的操作
#pragma omp parallel for private(k)	
		for ( i = 0; i < nProcessors; i++ )
		{
			UINT	*pDataSum = (UINT *)malloc(uRadix * sizeof(UINT));

			// 初始化元素计数为0
			for ( k = 0; k < uRadix; k++ )
			{
				pDataSum[k] = 0;
			}

			//每个线程计算自己的数据中各个位的元素个数
			UINT begin = i * uCount;
			UINT end = (i+1) * uCount;
			if ( end > uDataLen )
			{
				end = uDataLen;
			}
			for ( k = begin ; k < end; k++ )
			{
				UINT uPos = (*GetKeyFunc)(ppData[k], uKeyIndex);
				pDataSum[uPos] += 1;
			}

			void **ppPrivateData = (void **)(&(ppTempData[begin]));
			DATA_ARRAY *pDataArray = pBox[i].pDataArray;

			pDataArray[0].ppData = ppPrivateData;

			int nDataSum = 0;
			//按计算出来的元素个数来分配箱子的空间大小
			for ( k = 1; k < uRadix; k++ )
			{
				//pBox[i].ppTable[k] = SortTable_Create(pDataSum[k]);
				nDataSum += pDataSum[k-1];
				pDataArray[k].ppData = (void **)( &(ppPrivateData[nDataSum]) ); 
				pDataArray[k].uCount = 0;
			}


			// 将数据分配到各个箱子里
			for ( k = begin ; k < end; k++ )
			{
				UINT uPos = (*GetKeyFunc)(ppData[k], uKeyIndex);
				pDataArray[uPos].ppData[pDataArray[uPos].uCount] = ppData[k];
				pDataArray[uPos].uCount += 1;

				//SortTable_Add(pBox[i].ppTable[uPos], ppData[k]);
			}
		}//for ( i = 0; i < nProcessors; i++ )

		//收集操作，将数据重新放回ppData里
		UINT uIndex = 0;
		for ( k = 0; k < uRadix; k++ )
		{
			for ( i = 0; i < nProcessors; i++ )
			{
				UINT j;
				DATA_ARRAY *pArray = &(pBox[i].pDataArray[k]);
				UINT uCount = pArray->uCount;
				for ( j = 0; j < uCount; j++ )
				{
					ppData[uIndex] = pArray->ppData[j];
					uIndex++;
				}
			}
		}
		//进行下一轮的分配与收集操作
	}	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
	free(ppTempData);
	free(pTemp);
	free(pBox);
}


/**	分区的并行基数排序函数

	@param	void **ppData - 待排序数据	
	@param	UINT uDataLen - 数据长度	
	@param	UINT uRadix - 基数	
	@param	UINT uMaxKeyLen - 关键词长度	
	@param	GETKEYFUNC GetKeyFunc - 关键词取位回调函数	
	@return	void - 无	
*/
void Parallel_Partitioned_RadixSort(void **ppData, UINT uDataLen, UINT uRadix, 
									UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{

	//各个线程计算自己的各个箱子大小
	int     i, nProcessors;
    UINT    k;

	nProcessors = omp_get_num_procs();
	UINT uCount = uDataLen / nProcessors;

	if ( uDataLen - uCount * nProcessors > 2 )
	{
		uCount++;
	}

	UINT	**ppDataSum = (UINT **)malloc(sizeof(UINT *) * nProcessors);
#pragma omp parallel for private(k)	
	for ( i = 0; i < nProcessors; i++ )
	{
		UINT	*pDataSum;
		pDataSum = (UINT *)malloc(uRadix * sizeof(UINT));
		ppDataSum[i] = pDataSum;

		// 初始化元素计数为0
		for ( k = 0; k < uRadix; k++ )
		{
			pDataSum[k] = 0;
		}

		//每个线程计算自己的数据中各个位的元素个数
		UINT begin = i * uCount;
		UINT end = (i+1) * uCount;
		if ( end > uDataLen )
		{
			end = uDataLen;
		}
		for ( k = begin ; k < end; k++ )
		{
			UINT uPos = (*GetKeyFunc)(ppData[k], 0);
			pDataSum[uPos] += 1;
		}
	}

	//计算全局的各个箱子的大小
	UINT	*pGlobalDataSum = (UINT *)malloc(uRadix * sizeof(UINT));
	for ( k = 0; k < uRadix; k++ )
	{
		pGlobalDataSum[k] = 0;
	}

	for ( i = 1; i < nProcessors; i++ )
	{
		UINT *pTempDataSum = ppDataSum[i];
		for ( k = 0; k < uRadix; k++ )
		{
			pGlobalDataSum[k] += pTempDataSum[k];
		}
	}


	//分配一个大的数组，数组长度等于输入数据的长度
	void **ppTempData = (void **)malloc(uDataLen * sizeof(void *));

	//将数组划分成一系列的箱子，保证箱子按照收集操作的顺序摆放
	RADIX_BOX2 *pBox = (RADIX_BOX2 *)malloc(sizeof(RADIX_BOX2)* nProcessors);

	void *pTemp = malloc(sizeof(DATA_ARRAY *) * uRadix * nProcessors);

	for ( i = 0; i < nProcessors; i++ )
	{
		pBox[i].pDataArray = (DATA_ARRAY *)((char *)pTemp + i * sizeof(DATA_ARRAY) * uRadix);
		pBox[i].uBoxLen = uRadix;
	}


	void **pp = ppTempData;

	for ( i = 0; i < (int)uRadix; i++ )
	{
		for ( k = 0; k < (UINT)nProcessors; k++ )
		{
			pBox[k].pDataArray[i].ppData = pp;//pBox[i].pDataArray[k-1].ppData + ppDataSum[i][k-1]; 
			pp += ppDataSum[k][i];
			pBox[k].pDataArray[i].uCount = 0;
		}
	}

	//各个线程将数据放入到对应的箱子里，这样可以得到前一个全局箱子的所有数据
    //一定小于后一个全局箱子的所有数据
	UINT begin = 0;
	UINT end = 0;
	for ( i = 0; i < nProcessors; i++ )
	{
		begin = i * uCount;
		end = (i+1) * uCount;
		if ( end > uDataLen )
		{
			end = uDataLen;
		}
		DATA_ARRAY	*pDataArray = pBox[i].pDataArray;
		for ( k = begin ; k < end; k++ )
		{
			UINT uPos = (*GetKeyFunc)(ppData[k], 0);
			pDataArray[uPos].ppData[pDataArray[uPos].uCount] = ppData[k];
			pDataArray[uPos].uCount += 1;

			//SortTable_Add(pBox[i].ppTable[uPos], ppData[k]);
		}
	}


	//对uRadix个全局箱子内的数据进行排序，每个线程内使用串行排序算法即可
	begin = 0;
	end = 0;

	for ( i = 0; i < (int)uRadix; i++ )
	{
		end = begin + pGlobalDataSum[i];  
//		Serial_RadixSort(pTempData, 
	}


	//对所有的全局箱子排好序后，自然得到了一个有序的数据序列
}