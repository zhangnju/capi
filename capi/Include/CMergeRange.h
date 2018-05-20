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
    T * m_pData;       //要归并的数据
    int m_nBegin;      //要归并的第1个区间开始位置
    int m_nEnd;        //要归并的第1个区间结束位置
    int m_nBegin2;     //要归并的第2个区间开始位置
    int m_nEnd2;       //要归并的第2个区间结束位置
    int m_nGrainSize;  //最小的归并区间粒度大小  
    T * m_pOutData;    //存放归并后的数据的内存缓冲区

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


/**	将一个CMergeRange拆分成2个CMergeRange的接口函数
    当两个区间的大小之和小于给定的粒度时，不再进行拆分，
    而是将两个子区间进行归并

	@return	CRange * - 返回一个新拆分的CRange对象，另外一个存放在this对象里	
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


/**	将一个CMergeRange拆分成2个CMergeRange的接口函数

	@return	CMergeRange<T> * - 返回一个新拆分的CMergeRange对象，另外
                               一个新拆分的CMergeRange对象存放在this对象里
*/
template <class T>
CMergeRange<T> * CMergeRange<T>::Inter_Split()
{
    int nLen;
    if ( m_nEnd - m_nBegin < m_nEnd2 - m_nBegin2 )
    {
        //将两个区间进行交换，以保证先将大区间进行平分
        int nTemp;
        nTemp = m_nBegin2;
        m_nBegin2 = m_nBegin;
        m_nBegin = nTemp;

        nTemp = m_nEnd2;
        m_nEnd2 = m_nEnd;
        m_nEnd = nTemp;
    }

    int nMid1 = (m_nBegin + m_nEnd) >> 1 ; //除以2得到中间位置
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
                -1, -2, pOutData); //第2张表为空，所以结束位置设为-2

            m_nEnd = nMid1;
            //m_nEnd2保留原值不需要改变
        }
        else
        {
            //这种情况中，m_nBegin2和m_nEnd2实际上是相等的
            nLen = nMid1 - m_nBegin + 1;
            assertTrue(nLen != -1)
           T *pOutData = m_pOutData + nLen;

            pRange = new CMergeRange<T>(m_pData, nMid1 + 1, m_nEnd,
                nMid2, m_nEnd2, pOutData); 

            m_nEnd = nMid1;
            m_nBegin2 = -1;
            m_nEnd2 = -2; //第2张表为空，所以结束位置设为-2
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
            m_nEnd2 = -2; //设为-2表明第2张要归并的表是空表
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


/**	并行归并函数

	@param	T *pData - 要归并的数据	
	@param	int nBegin1 - 第1个区间开始位置	
	@param	int nEnd1 - 第1个区间结束位置	
	@param	int nBegin2 - 第2个区间开始位置	
	@param	int nEnd2 - 第2个区间结束位置	
	@param	T *pOutData - 存放归并后的数据	
	@return	void - 无	
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



/**	二分查找低边界函数，调用此函数前必须对表从小到大排好序

    @param	T *pData - 数据数组指针
    @param	T Data - 要查找的匹配数据
    @param	int nBegin - 查找区间的开始位置
    @param	int nEnd - 查找区间的结束位置
    @return	void * - 返回查到小于等于匹配数据的位置，如果无匹配数据则返回
                     区间上边界或下边界位置。
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
        nMid = ( nLow + nHigh ) >> 1; //除以2
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



/**	串行归并函数

    @param	T *pData - 要归并的数据	
    @param	int nBegin1 - 第1个区间开始位置	
    @param	int nEnd1 - 第1个区间结束位置	
    @param	int nBegin2 - 第2个区间开始位置	
    @param	int nEnd2 - 第2个区间结束位置	
    @param	T *pOutData - 存放归并后的数据	
	@return	void - 无	
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

        //如果j已经到了尾部
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


