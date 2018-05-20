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


/**	���ֲ��Һ��������ô˺���ǰ����Ա��С�����ź���

    @param	void **ppData - ��������ָ��
    @param	UINT uCount - ppData�����е��������ݸ���
    @param	void *pData - Ҫ���ҵ�ƥ������
    @param	COMPAREFUNC CompareFunc - �ȽϺ���
    @return	void * - �ɹ����ز鵽�ľ�ȷ���ݻ�պñ�Ҫ���ҵ����ݴ������,
                     ʧ�ܷ���-1��
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
            /* �Ѿ�������ƥ�����ݣ����� */
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
            return -1;  //����û�бȴ��������ݴ������
        }
    }
    else
    {
        return nHigh;
    }
}

const int g_ncore = omp_get_num_procs();//��ȡִ�к˵�����


/**	����ѭ��������Ҫ���߳�����
    ����ѭ������������CPU������һ���߳�������Ҫ��ѭ����������
    ���������Ҫ���߳������������������߳�����������CPU����

    @param	int n - ѭ����������	
    @param	int min_n - �����߳���Ҫ�����ٵ�������	
    @return	int - �߳���������������1����CPU������	
*/
int dtn(int n, int min_n)
{
    int max_tn = n / min_n;
    int tn = max_tn > g_ncore ? g_ncore : max_tn;  //tn��ʾҪ���õ��߳�����
    if ( tn < 1 )
    {
        tn = 1;
    }
    return tn;
}


/**	����ѭ��������Ҫ���߳�����
    ����ѭ������������CPU������һ���߳�������Ҫ��ѭ����������
    ���������Ҫ���߳������������������߳�����������CPU����������

    @param	int n - ѭ����������	
    @param	int min_n - �����߳���Ҫ�����ٵ�������	
    @return	int - �߳���������������2����CPU������	
*/
int dtn2(int n, int min_n)
{
    int max_tn = n / min_n;
    int tn = max_tn > 2*g_ncore ? 2*g_ncore : max_tn;  //tn��ʾҪ���õ��߳�����
    if ( tn < 1 )
    {
        tn = 1;
    }
    return tn;
}



/**	���������ַ֮��ĵ�1��Cache���׵�ַ
    ���������ַ�պ�Ϊһ��Cache���׵�ַ����ô����������Cache���׵�ַ

    @param	void *pAddr - �����ĵ�ַ	
    @return	void * - ���ظ�����ַ֮��ĵ�1��Cache���׵�ַ	
*/
void *GetCacheAlignedAddr(void *pAddr)
{
    int  m = CACHE_LINE_SIZE;

    void *pRet = (void *)(((UINT)pAddr+m-1)&(-m));

    return pRet;
}


/**	���������ַ֮��ĵ�1��Cache���׵�ַ
���������ַ�պ�Ϊһ��Cache���׵�ַ����ô����������������һ��Cache���׵�ַ

@param	void *pAddr - �����ĵ�ַ	
@return	void * - ���ظ�����ַ֮��ĵ�1��Cache���׵�ַ	
*/
void *GetNextCacheAlignedAddr(void *pAddr)
{
    int  m = CACHE_LINE_SIZE;

    void *pRet = (void *)(((UINT)(pAddr) + m )&(-m));

    return pRet;
}

