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


/**	��ȡ�����ĵ�uKeyIndexλ���֣���16����Ϊ��λ
��˻�ȡ�����������Ϊ0xf������������ʱ��
����Ϊ16�����ؼ��ʳ���Ϊ4

@param	UINT uData - ����ֵ	
@param	UINT uKeyIndex - 16���������ĵڶ���λ��32λ�������λ��Ϊ8	
@return	UINT - ���ص�uKeyIndexλ������	
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



/**	��ȡ�����ĵ�uKeyIndexλ���֣���256����Ϊ��λ
��˻�ȡ�����������Ϊ0xff������������ʱ��
����Ϊ256�����ؼ��ʳ���Ϊ8

@param	UINT uData - ����ֵ	
@param	UINT uKeyIndex - 256���������ĵڶ���λ��32λ�������λ��Ϊ4	
@return	UINT - ���ص�uKeyIndexλ������	
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



/**	����������ݵĵ�uKeyIndexλ�ϵ�Ԫ�ؽ��з��࣬
    �������ǵĴ�С�����Ӧ��������

	@param	SINGLELIST *pSingleList - ��������ָ��	
	@param	UINT       uRadix - ��������Ļ�������������������йأ�
                                һ�����������Ļ���Ϊ16���ַ����Ļ������Ϊ255��
	@param	UINT       uKeyIndex - �ڶ���λ	
	@param	SINGLENODE **ppHead - ������¼ͷָ�������	
	@param	SINGLENODE **ppTail - ��¼���ӵ�βָ��	
	@param	GETKEYFUNC GetKeyFunc - ��ȡ���ݵĵ�uKeyIndexλ�ϵ�Ԫ��ֵ	
	@return	void - ��	
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

    /* ��ʼ���ӱ� */
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


/**	����������ռ�����

	@param	SINGLENODE **ppHead - ��Ÿ������ӵ�ͷָ������	
	@param	SINGLENODE **ppTail - ��Ÿ������ӵ�βָ������	
	@param	UINT uRadix - ����	
	@return	SINGLELIST * - �ռ���ɺ󷵻��ռ��õ�����
*/
SINGLELIST * Collect(SINGLENODE **ppHead, SINGLENODE **ppTail, UINT uRadix)
{
	UINT			i;

	SINGLELIST	*pList = SingleList_Create();
	SINGLENODE	*pHead;
	SINGLENODE  *pTail;

	//��uRadix�������ӳ�һ������
	for ( i = 0; i < uRadix; i++ )
	{
		//���ҵ�һ���ǿյ��ӱ�
		if ( ppHead[i] != NULL )
		{
			break;
		}
	}
	if ( ppHead[i] == NULL)
	{
		//û���ҵ��ǿ��ӱ�,���Ǵ���Ĵ�����������û�����ݲŻ�����������
		return NULL;
	}

	pHead = ppHead[i];
	pTail = ppTail[i];

	for ( i += 1; i < uRadix; i++ )
	{
		//�������ҷǿյ��ӱ�
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


/**	��һ��������������������

	@param	SINGLELIST *pSingleList - ����	
	@param	UINT uListNum - ��������	
	@param	SINGLELIST ***pppList - ���ɵ���������	
	@return	SINGLELIST ** - �������ɺ����������	
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

/**	���л���������
    �Ե��������ڵ����ݽ��в��л�������

	@param	SINGLELIST *pSingleList - Ҫ���������	
	@param	UINT uRadix - ����	
	@param	UINT uMaxKeyLen - ���ؼ��ʳ���	
	@param	GETKEYFUNC GetKeyFunc - �ؼ���ȡλ�Ļص�����	
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
		/* �����������ڴ� */
		pppHead[i] = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
		pppTail[i] = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
#if 0
		//û����ʧ��ʱ�ͷ��ڴ棬�����ڴ�й©
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
		// ��һ�������ֳɶ��������ֺõĶ���������ppList������
		SplitListToArrayList(pSingleList, nProcessors, &ppList);
#pragma omp parallel for 
		for ( i = 0; i < nProcessors; i++ )
		{
			// �Բ�ֺõ�����������в��з������
			SingleList_Distribute(ppList[i], uRadix, k, pppHead[i], pppTail[i],GetKeyFunc);
		}
		//����ת�ò���,�������̵߳�λ����ͬ����������һ��������uRadix����
#pragma omp parallel for
		for ( i = 0; i < (int)uRadix; i++)
		{
			int j;
			SINGLENODE **ppTail0 = pppTail[0];
			SINGLENODE **ppHead0 = pppHead[0];

			SINGLENODE **ppHead;
			SINGLENODE **ppTail;
			
			//���ҵ�һ���ǿ��ӱ�
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
				//û���ҵ��ǿ��ӱ�
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

				if ( ppTempHead[i] == NULL ) //Ϊ�ձ�����û������
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

		//ע�ⲻ�ܵ���SingleList_Destroy()�������ͷţ�����Ὣ�����еĽڵ��ͷŵ�
		free(pList);

		//�����Ѿ�����������һ������������Ҫ���½����Ӻõ������������
		//��ֳ�nProcessors���������Է��䵽�����߳���,������һ�ַ������
	}//for ( k = 0; k < uMaxKeyLen; k++ )

	//���潫���������ʹ�õ���ʱ�ڴ��ͷŵ�

	for ( i = 0; i < nProcessors; i++ )
	{
		//ע�ⲻ�ܵ���SingleList_Destroy()�������ͷţ�����Ὣ�����еĽڵ��ͷŵ�
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




/**	��������

	@param	RADIX_BOX *pBox - xxxx	
	@param	UINT uRadix - xxxx	
	@return	void - xxxx	
*/
void RadixBox_Init(RADIX_BOX *pBox, UINT uRadix)
{
	pBox->ppTable = (SORTTABLE **)malloc(sizeof(SORTTABLE *) * uRadix);
	pBox->uBoxLen = uRadix;
}



/**	���л�������
��������в��л�������

@param	void **ppData - ����������	
@param	UINT uDataLen - ���ݳ���	
@param	UINT uRadix - ����	
@param	UINT uMaxKeyLen - ���ؼ��ʳ���	
@param	GETKEYFUNC GetKeyFunc - �ؼ���ȡλ�ص�����	
@return	void - ��	
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
        //����������ȫ�ֵ�һ�ֽ����ݷ��䵽������Ĳ���
        //#pragma omp parallel for private(k)	
        for ( i = 0; i < nProcessors; i++ )
        {
            UINT	*pDataSum = (UINT *)malloc(uRadix * sizeof(UINT));

            // ��ʼ��Ԫ�ؼ���Ϊ0
            for ( k = 0; k < uRadix; k++ )
            {
                pDataSum[k] = 0;
            }

            //ÿ���̼߳����Լ��������и���λ��Ԫ�ظ���
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

        //�ռ���������pOutData����������·Ż�ppData��


        for ( i = 0; i < nProcessors; i++ )
        {
            DATA_ARRAY *
            for ( k = 0; i < ppDataArray[i])
            ppDataArray[i].ppData[i]
        }


       //�ͷű�
        for ( i = 0; i < nProcessors; i++ )
        {
            for ( k = 0; k < uRadix; k ++ )
            {
                SortTable_Destroy(pBox[i].ppTable[k], NULL);
            }
        }
        //������һ�ֵķ������ռ�����
    }	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
}






/**	���л�������
    ��������в��л�������

	@param	void **ppData - ����������	
	@param	UINT uDataLen - ���ݳ���	
	@param	UINT uRadix - ����	
	@param	UINT uMaxKeyLen - ���ؼ��ʳ���	
	@param	GETKEYFUNC GetKeyFunc - �ؼ���ȡλ�ص�����	
	@return	void - ��	
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
		//����������ȫ�ֵ�һ�ֽ����ݷ��䵽������Ĳ���
//#pragma omp parallel for private(k)	
		for ( i = 0; i < nProcessors; i++ )
		{
			UINT	*pDataSum = (UINT *)malloc(uRadix * sizeof(UINT));

			// ��ʼ��Ԫ�ؼ���Ϊ0
			for ( k = 0; k < uRadix; k++ )
			{
				pDataSum[k] = 0;
			}

			//ÿ���̼߳����Լ��������и���λ��Ԫ�ظ���
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


			//�����������Ԫ�ظ������������ӵĿռ��С
			for ( k = 0; k < uRadix; k++ )
			{
				pBox[i].ppTable[k] = SortTable_Create(pDataSum[k]);
			}

			// �����ݷ��䵽����������
			for ( k = begin ; k < end; k++ )
			{
				UINT uPos = (*GetKeyFunc)(ppData[k], uKeyIndex);
				SortTable_Add(pBox[i].ppTable[uPos], ppData[k]);
			}
		}//for ( i = 0; i < nProcessors; i++ )

		//�ռ����������������·Ż�ppData��
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
		//�ͷű�
		for ( i = 0; i < nProcessors; i++ )
		{
			for ( k = 0; k < uRadix; k ++ )
			{
				SortTable_Destroy(pBox[i].ppTable[k], NULL);
			}
		}
		//������һ�ֵķ������ռ�����
	}	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
}



/**	��������

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


/**	��������

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


/**	��������

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


/**	��������

	@param	void **ppData - ����������	
	@param	UINT uDataLen - ���ݳ���	
	@param	UINT uRadix - ����	
	@param	UINT uMaxKeyLen - �ؼ��ʳ���	
	@param	GETKEYFUNC GetKeyFunc - �ؼ���ȡλ�ص�����	
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

	// ���е�1�ַ������
	for ( i = 0; i < nProcessors; i++ )
	{
		//��������̲߳������ݵ���ʼλ��
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

	}//��1�ַ����������

	for ( uKeyIndex = 1; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
	{
		RADIXBOX_POSITION	*pPos = GetPosArray(pBoxArray, nProcessors, uCount);
		pBoxArrayTemp = BoxArray_Create(uRadix, nProcessors, uDataBlockLen);

		// ���е�uKeyIndex�ַ������
		for ( i = 0; i < nProcessors; i++ )
		{
			ARRAYLIST	*pList;
			ARRAYNODE	*pNode;
			//��������̲߳������ݵ���ʼλ��
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
		}//��uKeyIndex�ַ����������
		
		//�ͷ�pBoxArray
		BoxArray_Destroy(pBoxArray);
		pBoxArray = pBoxArrayTemp;
	}
    return CAPI_SUCCESS;
}



/**	���л�������

	@param	void **ppData - ����������	
	@param	UINT uDataLen - ���ݳ���	
	@param	UINT uRadix - ����	
	@param	UINT uMaxKeyLen - ���ؼ��ʳ���	
	@param	GETKEYFUNC GetKeyFunc - �ؼ���ȡλ�ص�����	
	@return	void - ��	
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
		//����������ȫ�ֵ�һ�ֽ����ݷ��䵽������Ĳ���
#pragma omp parallel for private(k)	
		for ( i = 0; i < nProcessors; i++ )
		{
			UINT	*pDataSum = (UINT *)malloc(uRadix * sizeof(UINT));

			// ��ʼ��Ԫ�ؼ���Ϊ0
			for ( k = 0; k < uRadix; k++ )
			{
				pDataSum[k] = 0;
			}

			//ÿ���̼߳����Լ��������и���λ��Ԫ�ظ���
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
			//�����������Ԫ�ظ������������ӵĿռ��С
			for ( k = 1; k < uRadix; k++ )
			{
				//pBox[i].ppTable[k] = SortTable_Create(pDataSum[k]);
				nDataSum += pDataSum[k-1];
				pDataArray[k].ppData = (void **)( &(ppPrivateData[nDataSum]) ); 
				pDataArray[k].uCount = 0;
			}


			// �����ݷ��䵽����������
			for ( k = begin ; k < end; k++ )
			{
				UINT uPos = (*GetKeyFunc)(ppData[k], uKeyIndex);
				pDataArray[uPos].ppData[pDataArray[uPos].uCount] = ppData[k];
				pDataArray[uPos].uCount += 1;

				//SortTable_Add(pBox[i].ppTable[uPos], ppData[k]);
			}
		}//for ( i = 0; i < nProcessors; i++ )

		//�ռ����������������·Ż�ppData��
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
		//������һ�ֵķ������ռ�����
	}	//for ( uKeyIndex = 0; uKeyIndex < uMaxKeyLen; uKeyIndex++ )
	free(ppTempData);
	free(pTemp);
	free(pBox);
}


/**	�����Ĳ��л���������

	@param	void **ppData - ����������	
	@param	UINT uDataLen - ���ݳ���	
	@param	UINT uRadix - ����	
	@param	UINT uMaxKeyLen - �ؼ��ʳ���	
	@param	GETKEYFUNC GetKeyFunc - �ؼ���ȡλ�ص�����	
	@return	void - ��	
*/
void Parallel_Partitioned_RadixSort(void **ppData, UINT uDataLen, UINT uRadix, 
									UINT uMaxKeyLen, GETKEYFUNC GetKeyFunc)
{

	//�����̼߳����Լ��ĸ������Ӵ�С
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

		// ��ʼ��Ԫ�ؼ���Ϊ0
		for ( k = 0; k < uRadix; k++ )
		{
			pDataSum[k] = 0;
		}

		//ÿ���̼߳����Լ��������и���λ��Ԫ�ظ���
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

	//����ȫ�ֵĸ������ӵĴ�С
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


	//����һ��������飬���鳤�ȵ����������ݵĳ���
	void **ppTempData = (void **)malloc(uDataLen * sizeof(void *));

	//�����黮�ֳ�һϵ�е����ӣ���֤���Ӱ����ռ�������˳��ڷ�
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

	//�����߳̽����ݷ��뵽��Ӧ��������������Եõ�ǰһ��ȫ�����ӵ���������
    //һ��С�ں�һ��ȫ�����ӵ���������
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


	//��uRadix��ȫ�������ڵ����ݽ�������ÿ���߳���ʹ�ô��������㷨����
	begin = 0;
	end = 0;

	for ( i = 0; i < (int)uRadix; i++ )
	{
		end = begin + pGlobalDataSum[i];  
//		Serial_RadixSort(pTempData, 
	}


	//�����е�ȫ�������ź������Ȼ�õ���һ���������������
}