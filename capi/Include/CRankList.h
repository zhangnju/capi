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
 *	CRankList.h
 *
 *	DESCRIPTION
 *		Module for CRankList class
 *      Use one step of Radix sort for sorting.
 *
 *	HISTORY
 *		02-20-2008	create by zhouweiming.
 *
 */
#ifndef __CRANKLIST_H__
#define __CRANKLIST_H__

#include "CapiCommon.h"

template <class T>
class CRankList 
{
public:
    struct RANKDATA 
    {
        RANKDATA  *pNext;
        int       nLevel;
        T         data;
    };

    struct ARRAYNODE 
    {
        RANKDATA *ptr;
        ARRAYNODE *pNext;
    }; 

    CRankList()
    {
        m_pArray = NULL;
        m_Head.ptr = NULL;
        m_Head.pNext = NULL;
        m_nDataCount = 0;
    }

    CRankList(int nDataCount)
    {
        m_pArray = NULL;
        m_Head.ptr = NULL;
        m_Head.pNext = NULL;
        m_nDataCount = nDataCount;
        Initialize(nDataCount);
    };

    virtual ~CRankList()
    {
        if ( m_pArray != NULL )
        {
            delete [] m_pArray;
        }
    };

    void Initialize(int nDataCount);
    void RankSort(T *pData, int nDataCount, int nPower, int nMaxLevel);

private:
    ARRAYNODE  *m_pArray;
    ARRAYNODE  m_Head;
    int        m_nDataCount;
};

template <class T>
void CRankList<T>::Initialize(int nDataCount)
{
    if ( m_pArray != NULL )
    {
        delete [] m_pArray;
    }
    m_pArray = new ARRAYNODE[nDataCount];

    int i;
    for ( i = 0; i < nDataCount; i++ )
    {
        ARRAYNODE *p = &(m_pArray[i]);
        p->pNext = p + 1;
        p->ptr = NULL;
    }

    m_pArray[nDataCount-1].pNext = NULL;
    m_Head.pNext = &(m_pArray[0]);
    m_Head.ptr = NULL;
    m_nDataCount = nDataCount;
}




template <class T>
void CRankList<T>::RankSort(T *pData, int nDataCount, int nPower, int nMaxLevel)
{
    int i;
    int nArray = power2(nPower);

    RANKDATA  *pRankData = new RANKDATA[nDataCount];


    //进行一趟分类操作
    for ( i = 0; i < nDataCount; i++ )
    {
        int residue;
        int quotient;

        residue = pData[i] & (nArray-1); // 求pData/nArray的余数
        quotient = (pData[i] &~(nArray-1)) >> nPower; //求pData/nArray的商

        RANKDATA *p = &(pRankData[i]);

        p->data = pData[i];
        p->nLevel = quotient;
        p->pNext = NULL;

        if ( m_pArray[residue].ptr == NULL )
        {
            m_pArray[residue].ptr = p;
        }
        else
        {
            //插入到有序链表中
            RANKDATA *pCur = m_pArray[residue].ptr;
            RANKDATA *pPrev = NULL;

            while ( pCur != NULL )
            {
                if ( pCur->nLevel < p->nLevel )
                {
                    pPrev = pCur;
                    pCur = pCur->pNext;
                }
                else
                {
                    p->pNext = pCur;
                    if ( pPrev == NULL )
                    {
                        m_pArray[residue].ptr = p;
                    }
                    else
                    {
                        pPrev->pNext = p;
                    }
                    break;
                }
            } // while()
            if ( pCur == NULL )
            {
                pPrev->pNext = p;
            }
        } // else
    }// for ()


    // 进行收集操作
    int nLevel;
    int nIndex = 0;
    for ( nLevel = 0; nLevel < nMaxLevel; nLevel++ )
    {
        ARRAYNODE *pCur;
        ARRAYNODE *pPrev;

        if ( nIndex == nDataCount )
        {
            //所有数据都被收集完毕，退出
            break;
        }
        pPrev = &m_Head;
        pCur = m_Head.pNext;

        while ( pCur != NULL )
        {
            RANKDATA *pRank = pCur->ptr;
            while ( pRank != NULL )
            {
                if ( pRank->nLevel == nLevel )
                {
                    pData[nIndex] = pRank->data;
                    nIndex += 1;
                    pRank = pRank->pNext;
                }
                else
                {
                    //do nothing
                    break;
                }
            }
            pCur->ptr = pRank;
            if ( pCur->ptr == NULL )
            {
                pPrev->pNext = pCur->pNext;
            }
            else
            {
                pPrev = pCur;
            }
            pCur = pCur->pNext;
        } //while()

    } // for()

    //排序完毕，释放内存,排好序的数据仍然存放在pData数组里
    delete [] pRankData;
    return;
}


#endif //__CRANKLIST_H__

