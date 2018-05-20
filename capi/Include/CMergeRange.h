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
 *	CMergeRange.h
 *
 *	DESCRIPTION
 *		Module for CMergeRange class 
 *
 *	HISTORY
 *		09-21-2008	create by zhouweiming.
 *
 */
#ifndef __CMERGERANGE_H__
#define __CMERGERANGE_H__

#include "Parallel_For.h"


#define     MIN_MERGE_GRAIN_SIZE       512

template <class T>
void SequentialMerge(T *pData, int nBegin1, int nEnd1, 
                     int nBegin2, int nEnd2, T *pOutData);

template <class T>
int LowerBound(T *pData, T Data, int nBegin, int nEnd);


template <class T>
void Parallel_Merge(T *pData, int nBegin1, int nEnd1, 
                    int nBegin2, int nEnd2, T * pOutData);


template <class T>
class CMergeRange : public CRange {
public:
    T * m_pData;       //Ҫ�鲢������
    int m_nBegin;      //Ҫ�鲢�ĵ�1�����俪ʼλ��
    int m_nEnd;        //Ҫ�鲢�ĵ�1���������λ��
    int m_nBegin2;     //Ҫ�鲢�ĵ�2�����俪ʼλ��
    int m_nEnd2;       //Ҫ�鲢�ĵ�2���������λ��
    int m_nGrainSize;  //��С�Ĺ鲢�������ȴ�С  
    T * m_pOutData;    //��Ź鲢������ݵ��ڴ滺����

    CMergeRange<T> *Inter_Split();
    void Merge();
public:
    CMergeRange(T *pDataArray, int nBegin1, int nEnd1,
        int nBegin2, int nEnd2, T *pOutData);
    virtual CRange * Split();

    void SetGrainSize(int nGrainSize) {
        m_nGrainSize = nGrainSize;
    };
};


template <class T>
CMergeRange<T>::CMergeRange(T *pDataArray, int nBegin1, int nEnd1,
                            int nBegin2, int nEnd2, T *pOutData)
{
    m_pData = pDataArray;
    m_nBegin = nBegin1;
    m_nEnd = nEnd1;
    m_nBegin2 = nBegin2;
    m_nEnd2 = nEnd2;
    m_nGrainSize = MIN_MERGE_GRAIN_SIZE;

    m_pOutData = pOutData;

}

template <class T>
void CMergeRange<T>::Merge()
{
    SequentialMerge(m_pData, m_nBegin, m_nEnd, 
        m_nBegin2, m_nEnd2, m_pOutData);
}


/**	��һ��CMergeRange��ֳ�2��CMergeRange�Ľӿں���
    ����������Ĵ�С֮��С�ڸ���������ʱ�����ٽ��в�֣�
    ���ǽ�������������й鲢

	@return	CRange * - ����һ���²�ֵ�CRange��������һ�������this������	
*/
template <class T>
CRange * CMergeRange<T>::Split()
{
    int nLen = m_nEnd - m_nBegin + m_nEnd2 - m_nBegin2 + 2;

    if ( nLen <= m_nGrainSize ) 
    {
        Merge();
        return NULL;
    }
    return Inter_Split();
}


/**	��һ��CMergeRange��ֳ�2��CMergeRange�Ľӿں���

	@return	CMergeRange<T> * - ����һ���²�ֵ�CMergeRange��������
                               һ���²�ֵ�CMergeRange��������this������
*/
template <class T>
CMergeRange<T> * CMergeRange<T>::Inter_Split()
{
    int nLen;
    if ( m_nEnd - m_nBegin < m_nEnd2 - m_nBegin2 )
    {
        //������������н������Ա�֤�Ƚ����������ƽ��
        int nTemp;
        nTemp = m_nBegin2;
        m_nBegin2 = m_nBegin;
        m_nBegin = nTemp;

        nTemp = m_nEnd2;
        m_nEnd2 = m_nEnd;
        m_nEnd = nTemp;
    }

    int nMid1 = (m_nBegin + m_nEnd) >> 1 ; //����2�õ��м�λ��
    int nMid2 = LowerBound(m_pData, m_pData[nMid1], m_nBegin2, m_nEnd2);

    CMergeRange<T> *pRange; 

    if ( nMid2 < 0 )
    {
        nLen = nMid1 - m_nBegin + 1;
        assertTrue(nLen != -1)
        T *pOutData = m_pOutData + nLen;

        pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
            -1, -2, pOutData);

        m_nEnd = nMid1;
        m_nBegin2 = -1;
        m_nEnd2 = -2;
    }
    else if ( nMid2 == m_nEnd2 )
    {
        if ( m_pData[nMid2] <= m_pData[nMid1] )
        {
            nLen = nMid1 - m_nBegin + nMid2 - m_nBegin2 + 2;
            assertTrue(nLen != -1)
           T *pOutData = m_pOutData + nLen;

            pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
                -1, -2, pOutData); //��2�ű�Ϊ�գ����Խ���λ����Ϊ-2

            m_nEnd = nMid1;
            //m_nEnd2����ԭֵ����Ҫ�ı�
        }
        else
        {
            //��������У�m_nBegin2��m_nEnd2ʵ��������ȵ�
            nLen = nMid1 - m_nBegin + 1;
            assertTrue(nLen != -1)
           T *pOutData = m_pOutData + nLen;

            pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
                nMid2, m_nEnd2, pOutData); 

            m_nEnd = nMid1;
            m_nBegin2 = -1;
            m_nEnd2 = -2; //��2�ű�Ϊ�գ����Խ���λ����Ϊ-2
        }
    }
    else if ( nMid2 == m_nBegin2 )
    {
        if ( m_pData[nMid2] > m_pData[nMid1] )
        {
            nLen = nMid1 - m_nBegin + 1;

            assertTrue(nLen != -1)

            T *pOutData = m_pOutData + nLen;

            pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
                    nMid2, m_nEnd2, pOutData);

            m_nEnd = nMid1;
            m_nBegin2 = -1;
            m_nEnd2 = -2; //��Ϊ-2������2��Ҫ�鲢�ı��ǿձ�
        }
        else
        {
            nLen = nMid1 - m_nBegin + 2;
            assertTrue(nLen != -1)
           T *pOutData = m_pOutData + nLen;

            if ( nMid2 < m_nEnd2 )
            {
                pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
                    nMid2 + 1, m_nEnd2, pOutData);
            }
            else
            {
                pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
                    -1, -2, pOutData);
            }
            m_nEnd = nMid1;
            m_nEnd2 = nMid2;
        }
    }
    else
    {
        nLen = nMid1 - m_nBegin + nMid2 - m_nBegin2 + 2;
        assertTrue(nLen != -1)
        T *pOutData = m_pOutData + nLen;

        pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
            nMid2 + 1, m_nEnd2, pOutData);

        m_nEnd = nMid1;
        m_nEnd2 = nMid2;
    }

    pRange->SetGrainSize(m_nGrainSize);

    return pRange;
}


/**	���й鲢����

	@param	T *pData - Ҫ�鲢������	
	@param	int nBegin1 - ��1�����俪ʼλ��	
	@param	int nEnd1 - ��1���������λ��	
	@param	int nBegin2 - ��2�����俪ʼλ��	
	@param	int nEnd2 - ��2���������λ��	
	@param	T *pOutData - ��Ź鲢�������	
	@return	void - ��	
*/
template <class T>
void Parallel_Merge(T *pData, int nBegin1, int nEnd1, 
                    int nBegin2, int nEnd2, T *pOutData)
{
    CMergeRange  *pRange = new CMergeRange<T>(pData, nBegin1, nEnd1, 
                                nBegin2, nEnd2, pOutData);
    Parallel_For( pRange );
    return;
}



/**	���ֲ��ҵͱ߽纯�������ô˺���ǰ����Ա��С�����ź���

    @param	T *pData - ��������ָ��
    @param	T Data - Ҫ���ҵ�ƥ������
    @param	int nBegin - ��������Ŀ�ʼλ��
    @param	int nEnd - ��������Ľ���λ��
    @return	void * - ���ز鵽С�ڵ���ƥ�����ݵ�λ�ã������ƥ�������򷵻�
                     �����ϱ߽���±߽�λ�á�
*/
template <class T>
int LowerBound(T *pData, T Data, int nBegin, int nEnd)
{
    int nLow;
    int nHigh;
    int nMid;

    nLow = nBegin;
    nHigh = nEnd;
    nMid = nBegin;
    while ( nLow <= nHigh )
    {
//        INT nResult;
        nMid = ( nLow + nHigh ) >> 1; //����2
        if ( pData[nMid] > Data )
        {
            nHigh = nMid - 1;
        }
        else if ( pData[nMid] < Data )
        {
            nLow = nMid + 1;
        }
        else
        {
            return nMid;
        }
    }
    if ( nMid == nBegin )
    {
        return nBegin;
    }
    else if ( nMid == nEnd )
    {
        if ( pData[nMid] > Data )
        {
            return nMid - 1;
        }
        return nEnd;
    }
    else
    {
        return nHigh;
    }
}



/**	���й鲢����

    @param	T *pData - Ҫ�鲢������	
    @param	int nBegin1 - ��1�����俪ʼλ��	
    @param	int nEnd1 - ��1���������λ��	
    @param	int nBegin2 - ��2�����俪ʼλ��	
    @param	int nEnd2 - ��2���������λ��	
    @param	T *pOutData - ��Ź鲢�������	
	@return	void - ��	
*/
template <class T>
void SequentialMerge(T *pData, int nBegin1, int nEnd1, 
                    int nBegin2, int nEnd2, T *pOutData)
{
    int i, j, k;
    
    T *pNewData = pOutData;

    i = nBegin1;
    j = nBegin2;
    k = 0;

    for( i = nBegin1; i <= nEnd1; i++ )
    {
        for ( ; j <= nEnd2; j++ )
        {
            if ( pData[i] < pData[j] )
            {
                pNewData[k] = pData[i];
                ++k;
                break;
            }
            else
            {
                pNewData[k] = pData[j];
                ++k;
            }
        }

        //���j�Ѿ�����β��
        if ( j > nEnd2 )
        {
            for ( ; i <= nEnd1; i++ )
            {
                pNewData[k] = pData[i];
                ++k;
            }
            break;
        }
    }

    if ( i > nEnd1 )
    {
        for ( ; j <= nEnd2; j++ )
        {
            pNewData[k] = pData[j];
            ++k;
        }
    }

    return;
}

#endif // __CMERGERANGE_H__


