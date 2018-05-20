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

		//���j�Ѿ�����β��
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


/**	���й鲢����
    �鲢�õ����ݴ�����������ppOutData��

	@param	void **ppData - ���鲢������	
	@param	int nStart1 - ��1������Ŀ�ʼλ��(����nStart1)	
	@param	int nEnd1 - ��1������Ľ���λ��(����nEnd1)	
	@param	int nStart2 - ��2������Ŀ�ʼλ��(����nStart2)		
	@param	int nEnd2 - ��2������Ľ���λ��(����nEnd2)		
	@param	COMPAREFUNC func - �ȽϺ���	
	@param	void **ppOutData - ��Ź鲢�������	
	@return	void** - ���ع鲢�������ָ��(����ppOutData)	
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

		//���j�Ѿ�����β��
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


//���޸ĵ��ԣ� ��Ҫ�ο��鼮
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


/**	���й鲢����
    �鲢�õ�����д�ص��������ppData��

    @param	void **ppData - ���鲢������	
    @param	int nStart1 - ��1������Ŀ�ʼλ��(����nStart1)	
    @param	int nEnd1 - ��1������Ľ���λ��(����nEnd1)	
    @param	int nStart2 - ��2������Ŀ�ʼλ��(����nStart2)		
    @param	int nEnd2 - ��2������Ľ���λ��(����nEnd2)		
    @param	COMPAREFUNC func - �ȽϺ���	
    @return	void - ��	
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

		//���j�Ѿ�����β��
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

/**	���й鲢������

	@param	void **ppData - ����������	
	@param	int nBegin - ��������Ŀ�ʼλ��	
	@param	int nEnd - ��������Ľ���λ��	
	@param	COMPAREFUNC func - ���ݴ�С�ȽϺ���	
	@return	void - ��	
*/
void Serial_MergeSort(void **ppData, int nBegin, int nEnd, COMPAREFUNC func)
{
    if ( nEnd - nBegin < MIN_MERGE_COUNT )
    {
        QuickSort(ppData, nBegin, nEnd, func);
        return;
    }
    int nMid = (nBegin + nEnd) >> 1;  //����2

    Serial_MergeSort(ppData, nBegin, nMid, func);
    Serial_MergeSort(ppData, nMid+1, nEnd, func);
    
    Serial_Merge2(ppData, nBegin, nMid+1, nEnd, func);
}

#define MAX_MERGE_TURN      24

/**	���й鲢����������

	@param	void **ppData - ����������	
	@param	int nLen - ���������ݳ���	
	@param	COMPAREFUNC func - ���ݱȽϻص�����	
	@return	void - ��	
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
	nLoopCount = 1 << nTurn;   //nLoopCount����2��nTurn�η�

    //�������ʵĵ���2���ݴη���nLoopCount
    while ( nLoopCount > nCount )
    {
        nLoopCount = nLoopCount >> 1; //����2
        --nTurn;
    }

    int nTemp = nCore << 1; //nTemp����nCore����2
    while ( (1 << nTurn) > nTemp )
    {
        --nTurn;   //����nTurn�Ĵ��������ڼ��ٺ���鲢ʱ�̴߳����Ĵ���
        nLoopCount = nLoopCount >> 1;
    }

    //�ж�nTurn�Ƿ�Ϊ����
    if ( (nLoopCount > nCore) && (nTurn > 0x1) && ((nTurn & 0x1) == 0x1) ) 
    {
        --nTurn;   //��nTurn���ż�������ں��治��������
        nLoopCount = nLoopCount >> 1;
    }

	nStep = nLen / nLoopCount;

    int *p = new int[nLoopCount*2 + CACHE_LINE_SIZE];
    int *pnPos = (int *)GetCacheAlignedAddr(p);
    
    //�����ݷֳ�nLoopCount��С���䣬���еض�ÿ��������д��п�������
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

    //���ź���ĸ�������������в��й鲢����
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
		
        nLoopCount = nLoopCount >> 1; //����2
        nStep += nStep;

        ppSwap = ppMergeData;
        ppMergeData = ppTempOutData;
        ppTempOutData = ppSwap;
	}

    //������д��ppData�У����nTurnΪż���������ж��ڵ�ѭ�����ᱻִ��
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


/**	���й鲢����������

    @param	void **ppData - ����������	
    @param	int nLen - ���������ݳ���	
    @param	int nTurn - ���й鲢�ִΣ�һ�������nTurn�η�����CPU����	
    @param	COMPAREFUNC func - ���ݱȽϻص�����	
    @return	void - ��	
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

    nCore = 1<< nTurn ; //nCore����2��nTurn�η�

    nStep = nLen / nCore;

    //�����ݷֳ����ɸ�С���䣬���еض�ÿ��������д��п�������
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

    //���ź���ĸ���������в��й鲢����
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
