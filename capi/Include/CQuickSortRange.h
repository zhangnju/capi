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
 *	CQuickSortRange.h
 *
 *	DESCRIPTION
 *		Module for CQuickSortRange class 
 *
 *	HISTORY
 *		09-20-2008	create by zhouweiming.
 *
 */
#ifndef __CQUICKSORTRANGE_H__
#define __CQUICKSORTRANGE_H__

#include "Parallel_For.h"

template <class T> 
int QuickSort_Split(T *pData, int nBegin, int nEnd);

template <class T>
void QuickSort(T *pData, int nBegin, int nEnd);

template <class T>
void Parallel_QuickSort(T *pData, int nBegin, int nEnd);

template <class T>
class CQuickSortRange : public CRange {
private:
    T *  m_pData;
    int  m_nBegin;
    int  m_nEnd;
public:
    CQuickSortRange(T *pDataArray, int nBegin, int nEnd);
    virtual CRange * Split();
};


template <class T>
CQuickSortRange<T>::CQuickSortRange(T *pDataArray, int nBegin, int nEnd)
{
    m_pData = pDataArray;
    m_nBegin = nBegin;
    m_nEnd = nEnd;
}

#define MIN_QUICKSORT_SIZE      512

template <class T>
CRange * CQuickSortRange<T>::Split()
{
    if ( m_nEnd - m_nBegin <= MIN_QUICKSORT_SIZE )
    {
        QuickSort(m_pData, m_nBegin, m_nEnd);
        return NULL;
    }

    int nMid = QuickSort_Split(m_pData, m_nBegin, m_nEnd);

    CQuickSortRange<T> *pRange = new CQuickSortRange<T>(m_pData, nMid+1, m_nEnd);

    m_nEnd = nMid - 1;

    return pRange;
}

template <class T>
int QuickSort_Split(T *pData, int nBegin, int nEnd)
{
    T SelData;
    int nLow;
    int nHigh;

    nLow = nBegin;
    nHigh = nEnd;

    SelData = pData[nLow];
    while ( nLow < nHigh )
    {
        while ( (pData[nHigh] > SelData)  && (nLow != nHigh) )
        {
            --nHigh;
        }
        if ( nHigh != nLow )
        {
            pData[nLow] = pData[nHigh];
            ++nLow;
        }

        while ( ( pData[nLow] < SelData ) && (nLow != nHigh) )
        {
            ++nLow;
        }
        if ( nLow != nHigh )
        {
            pData[nHigh] = pData[nLow];
            --nHigh;
        }
    }
    pData[nLow] = SelData;

    return nLow;
}

template <class T>
void QuickSort(T *pData, int nBegin, int nEnd)
{
    int nMid = QuickSort_Split(pData, nBegin, nEnd);
    if ( nMid > nBegin )
    {
        QuickSort(pData, nBegin, nMid - 1);
    }

    if ( nEnd > nMid )
    {
        QuickSort(pData, nMid + 1, nEnd);
    }
}

template <class T>
void Parallel_QuickSort(T *pData, int nBegin, int nEnd)
{
    CQuickSortRange<T> *pRange = new CQuickSortRange<T>(pData, nBegin, nEnd);
    Parallel_For(pRange);
}

#endif // __CQUICKSORTRANGE_H__


