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

#ifdef _WIN32
#include <windows.h>
#include <process.h>
#endif
#include <stdlib.h>
#include <omp.h>
#include <capiglobal.h>
#include <stdio.h>

/**	˳����Һ���
    @param	void **ppData���������ָ��	
    @param	void *pData����Ҫ���ҵ�ƥ������
    @param	COMPAREFUNC CompareFunc�����ȽϺ���
    @return	int �����ɹ����ز鵽�����ݵ��±�λ�ã�ʧ�ܷ���-1
*/
int OrdinalFind(void **ppData, void *pData, UINT uStart, UINT uEnd, 
                COMPAREFUNC comp)
{
    UINT i;
    for ( i = uStart; i < uEnd; i++ )
    {
        if ( (*comp)(ppData[i], pData) == 0 )
        {
            return i;
        }
    }
    return -1;
}


/**	���ֲ��Һ��������ô˺���ǰ����Ա��С�����ź���
    @param	void **ppData���������ָ��	
    @param	void *pData����Ҫ���ҵ�ƥ������
    @param	COMPAREFUNC CompareFunc�����ȽϺ���
    @return	int �����ɹ����ز鵽�����ݵ��±�λ�ã�ʧ�ܷ���-1
*/
int DimidiateFind(void **ppData,void *pData, UINT uStart, UINT uEnd,
                  COMPAREFUNC CompareFunc)
{
    UINT uLow;
    UINT uHigh;
    UINT uMid;
    if ( ppData == NULL || CompareFunc == NULL || pData == NULL 
        || uStart > uEnd  )
    {
        return -1;
    }
    uLow = uStart;
    uHigh = uEnd;
    uMid = 0;
    while ( (INT)uLow <= (INT)uHigh )
    {
        INT nResult;
        uMid = ( uLow + uHigh ) / 2;
        nResult = (*CompareFunc)( ppData[uMid],pData );
        if ( nResult > 0 )
        {
            uHigh = uMid-1;
        }
        else if ( nResult < 0 )
        {
            uLow = uMid + 1;
        }
        else
        {
            /* �Ѿ�������ƥ�����ݣ����� */
            return uMid;
        }
    }
    /* δ�ҵ�ƥ�����ݣ����ؿ�ָ�� */
    return -1;
}

/**	���ֲ��Һ��������ô˺���ǰ����Ա��С�����ź���

	@param	void **ppData - �����ָ��	
	@param	void *pData - Ҫ���ҵ�ƥ������
	@param	COMPAREFUNC Comp - �ȽϺ���
	@return	int - �ɹ����ز鵽�ľ�ȷ���ݻ�պñ�Ҫ���ҵ����ݴ������λ��,
                        ʧ�ܷ���-1��
*/
int BinSearch(void **ppData, int begin, int end, void *pData, COMPAREFUNC Comp)
{
    int nLow;
    int nHigh;
    int nMid;

    nLow = begin;
    nHigh = end;
    nMid = begin;
    while ( nLow <= nHigh )
    {
        INT nResult;
        nMid = ( nLow + nHigh ) / 2;
        nResult = (*Comp)( ppData[nMid], pData );
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
            /* �Ѿ�������ƥ�����ݣ����� */
            return nMid;
        }
    }

    /* δ�ҵ�ƥ�����ݣ����ظպñ���������� */
    return nHigh;
}


int Parallel_SearchMaxData(void **ppData, int nLen, COMPAREFUNC comp)
{
	int i, k;

	int nCore = omp_get_num_procs();

	int *pnMax = (int *)malloc(nCore * sizeof(int));
	if ( pnMax == NULL )
	{
		return -1;
	}

	int nStep = nLen / nCore;

#pragma omp parallel for 
	for ( k = 0; k < nCore; k++ )
	{
		int begin = k * nStep;
		int end = (k+1) * nStep;
		if ( k == nCore-1 )
		{
			end = nLen;
		}
		int nMax = begin;
		for ( i = begin; i < end; i++ )
		{
			if ( (*comp)(ppData[i], ppData[nMax]) > 0 )
			{
				nMax = i;
			}
		}
		pnMax[k] = nMax;
	}
	int nMax = 0;
	for ( i = 1; i < nCore; i++ )
	{
		if ( (*comp)((void *)pnMax[i], (void *)pnMax[nMax]) > 0 )
		{
			nMax = i;
		}
	}
	nMax = pnMax[nMax];
	free(pnMax);
	return nMax;
}


int Parallel_SearchData(void **ppData, int nLen, void *pData, COMPAREFUNC comp)
{
	int i, k;

	int nCore = omp_get_num_procs();

	int nStep = nLen / nCore;

	int nRet = -1;
#pragma omp parallel for 
	for ( k = 0; k < nCore; k++ )
	{
		int begin = k * nStep;
		int end = (k+1) * nStep;
		if ( k == nCore-1 )
		{
			end = nLen;
		}

		for ( i = begin; i < end; i++ )
		{
			if ( (*comp)(ppData[i], pData) == 0 )
			{
				nRet = i;
			}
		}
	}
	return nRet;
}


LONG volatile g_SearchFlag = 0;
static int g_SearchPos = -1;

void SerialSearch(void **ppData, int begin, int end, void *pData, COMPAREFUNC comp)
{
    int i;
    for ( i = begin; i < end; i++ )
    {
        if ( g_SearchFlag != 0 )
        {
            break;
        }
        if ( (*comp)(ppData[i], pData) == 0 )
        {
            g_SearchPos = i;
            AtomicIncrement(&g_SearchFlag);
            break;
        }
    }
#if _DEBUG
    printf("SerialSearch() stopped at i = %ld\n", i);
#endif
}




int Parallel_SearchData2(void **ppData, int nLen, void *pData, COMPAREFUNC comp)
{
    int k;
    int nRet;

    int nCore = omp_get_num_procs();
    int nStep = nLen / nCore;

    g_SearchFlag = 0;
    g_SearchPos = -1;
#pragma omp parallel for num_threads(nCore)
    for ( k = 0; k < nCore; k++ )
    {
        int begin = k * nStep;
        int end = (k+1) * nStep;
        if ( k == nCore-1 )
        {
            end = nLen;
        }

        SerialSearch(ppData, begin, end, pData, comp);
    }

    nRet = g_SearchPos; 

    return nRet;
}

struct PARALLEL_SEARCHDATA {
    void **ppdata;
    int    begin;
    int     end;
    void    *pData;
    COMPAREFUNC comp;
}; 

void SearchTask(LPVOID args)
{
    PARALLEL_SEARCHDATA  *pNode = (PARALLEL_SEARCHDATA *)args;

    SerialSearch(pNode->ppdata, pNode->begin, pNode->end,
        pNode->pData, pNode->comp);
}


int Parallel_SearchData3(void **ppData, int nLen, void *pData, COMPAREFUNC comp)
{
    int k;
    int nRet;

    int nCore = omp_get_num_procs();
    int nStep = nLen / nCore;

    g_SearchFlag = 0;
    g_SearchPos = -1;

    for ( k = 0; k < nCore; k++ )
    {
        int begin = k * nStep;
        int end = (k+1) * nStep;
        if ( k == nCore-1 )
        {
            end = nLen;
        }

        PARALLEL_SEARCHDATA  *pNode = new PARALLEL_SEARCHDATA;
        pNode->begin = begin;
        pNode->end = end;
        pNode->comp = comp;
        pNode->pData = pData;
        pNode->ppdata = ppData;

        _beginthread(SearchTask, 0, (void *)pNode);
    }

    nRet = g_SearchPos; 

    return nRet;
}

