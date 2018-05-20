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
#include <windows.h>
#include <stdio.h>
#include "CapiGlobal.h"
#include "QuickSort.h"
#include "Stack.h"



void *GetMid(void *p1, void *p2, void *p3, COMPAREFUNC Comp)
{
	void *pMid;
	if ( (*Comp)(p1, p2) > 0 )
	{
		if ( (*Comp)(p2, p3) > 0 )
		{
			pMid = p2;
		}
		else
		{
			if ( (*Comp)(p1, p3) > 0 )
			{
				pMid = p3;
			}
			else
			{
				pMid = p1;
			}
		}
	}
	else
	{
		if ( (*Comp)(p1, p3) > 0 )
		{
			pMid = p3;
		}
		else
		{
			if ( (*Comp)(p2, p3) > 0 )
			{
				pMid = p3;
			}
			else
			{
				pMid = p2;
			}			
		}
	}
	return pMid;
}




UINT Split(void **ppData, UINT uStart, UINT uEnd,
                     COMPAREFUNC CompareFunc)
{
    void *pSelData;
    UINT uLow;
    UINT uHigh;

    uLow = uStart;
    uHigh = uEnd;

    pSelData = ppData[uLow];
    while ( uLow < uHigh )
    {
        while ( (*CompareFunc)(ppData[uHigh], pSelData) > 0 
            && uLow != uHigh )
        {
            --uHigh;
        }
        if ( uHigh != uLow )
        {
            ppData[uLow] = ppData[uHigh];
            ++uLow;
        }

        while ( (*CompareFunc)( ppData[uLow], pSelData ) < 0
            && uLow != uHigh )
        {
             ++uLow;
        }
        if ( uLow != uHigh )
        {
            ppData[uHigh] = ppData[uLow];
            --uHigh;
        }
    }
    ppData[uLow] = pSelData;

    return uLow;
}


void QuickSort(void **ppData, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc)
{
    UINT uMid = Split(ppData, uStart, uEnd, CompareFunc );
    if ( uMid > uStart )
    {
        QuickSort(ppData, uStart, uMid - 1, CompareFunc);
    }

    if ( uEnd > uMid )
    {
        QuickSort(ppData, uMid + 1, uEnd, CompareFunc);
    }
}


/**	快速排序算法的使用栈的非递归算法函数
    @param	void *ppData——数据指针数组	
    @param	UINT uStart——表中要排序数据区间的起点，为数组下标	
    @param	UINT uEnd——表中要排序数据区间的终点，为数组下标	
    @param	COMPAREFUNC CompareFunc——数据比较函数
    @return	void——无 
*/
void QuickSort2(void **ppData, UINT uStart, UINT uEnd, 
               COMPAREFUNC CompareFunc)
{
    STACK   *pStack;
    UINT    uLow;
    UINT    uHigh;
    UINT    uMid;

    if ( uStart >= uEnd )
    {
        return;
    }

    uLow = uStart;
    uHigh = uEnd;

    pStack = Stack_Create(uHigh - uLow);

    (void)Stack_Push(pStack, (void *)uLow);
    (void)Stack_Push(pStack, (void *)uHigh);

    while ( !Stack_IsEmpty(pStack) )
    {
        uHigh = (UINT)Stack_Pop(pStack);
        uLow = (UINT)Stack_Pop(pStack);
        if ( uLow < uHigh )
        {
            uMid = Split(ppData, uLow, uHigh, CompareFunc );
            if ( uMid > uLow )
            {
                (void)Stack_Push(pStack, (void *)uLow );
                (void)Stack_Push(pStack, (void *)(uMid - 1 ));
            }
            if ( uHigh > uMid )
            {
                (void)Stack_Push(pStack, (void *)(uMid + 1) );
                (void)Stack_Push(pStack, (void *)uHigh);
            }
        }
    }
    Stack_Destroy(pStack, NULL);
}


/**	排序表的快速排序函数，不使用栈调用的非递归算法，将指定区间的数据用
    快速排序算法排好

    @param	void *ppData——数据指针数组	
    @param	UINT uStart——要排序的区间起点 
    @param	UINT uEnd——要排序的区间终点
    @param	COMPAREFUNC CompareFunc——数据比较函数
    @return	void——无
*/

void QuickSort3(void **ppData, UINT uStart, UINT uEnd, 
                COMPAREFUNC CompareFunc)
{
    UINT   *puStack;
    UINT    uStackTop;
    UINT    uLow;
    UINT    uHigh;
    UINT    uMid;

    if ( uEnd - uStart < 1 )
    {
        return;
    }

    uLow = uStart;
    uHigh = uEnd;

    puStack = (UINT *)malloc((uHigh - uLow + 1) * sizeof(UINT));
    if ( puStack == NULL )
    {
        return;
    }
    uStackTop = 0;

    puStack[uStackTop] = uLow;
    ++uStackTop;
    puStack[uStackTop] = uHigh;
    ++uStackTop;

    while ( uStackTop != 0 )
    {
        --uStackTop;
        uHigh = puStack[uStackTop];

        --uStackTop;
        uLow = puStack[uStackTop];

        if ( uLow < uHigh )
        {
            uMid = Split(ppData, uLow, uHigh, CompareFunc );
            if ( uMid - uLow > uHigh - uMid )
            {
                puStack[uStackTop] = uLow;
                ++uStackTop;
                puStack[uStackTop] = uMid - 1;
                ++uStackTop;
                if ( uHigh > uMid )
                {
                    puStack[uStackTop] = uMid + 1;
                    ++uStackTop;
                    puStack[uStackTop] = uHigh;
                    ++uStackTop;
                }            
            }
            else
            {
                puStack[uStackTop] = uMid + 1;
                ++uStackTop;
                puStack[uStackTop] = uHigh;
                ++uStackTop;
                if ( uMid > uLow )
                {
                    puStack[uStackTop] = uLow;
                    ++uStackTop;
                    puStack[uStackTop] = uMid - 1;
                    ++uStackTop;
                }
            }

        }
    }
    free( puStack );
}

void ** Merge(void **ppData, UINT uStart, UINT uEnd, 
           void **ppData2, UINT uStart2, UINT uEnd2, COMPAREFUNC cfunc)
{
    UINT i, j, k;
    UINT u1, u2, v1,v2;
    void **pp1;
    void **pp2;

    void **pp = (void **)malloc( (uEnd-uStart+1+uEnd2-uStart2+1) * sizeof(void *));
    if ( pp == NULL )
    {
        return NULL;
    }

    if ( (*cfunc)(ppData2[uStart2], ppData[uStart]) > 0 )
    {
        u1 = uStart;
        u2 = uEnd;
        v1 = uStart2; 
        v2 = uEnd2;
        pp1 = ppData;
        pp2 = ppData2;
    }
    else
    {        
        u1 = uStart2;
        u2 = uEnd2;
        v1 = uStart; 
        v2 = uEnd;
        pp1 = ppData2;
        pp2 = ppData;
    }

    k = 0;
    pp[k] = pp1[u1];
    j = v1;
    for (i = u1+1; i <= u2; i++ )
    {
        while ( j <= v2 )
        {
            if ( (*cfunc)(pp2[j], pp1[i]) < 0 )
            {
                ++k;
                pp[k] = pp2[j];
                j++;
            }
            else
            {
                break;
            }
        }
        ++k;
        pp[k] = pp1[i];
    }

    if ( j < v2 )
    {
        for ( i = j; i <= v2; i++)
        {
            ++k;
            pp[k] = pp2[i];
        }
    }
    return pp;
}

#if 0

typedef struct SORTNODE_st {
	void **		ppData;
	UINT		uStart;
	UINT		uEnd;
	COMPAREFUNC func;
} SORTNODE;


DWORD WINAPI QuickSort_Thread(void *arg)
{
	SORTNODE	*pNode = (SORTNODE *)arg;
	QuickSort(pNode->ppData, pNode->uStart, pNode->uEnd, pNode->func);
	return 1;
}

#define THREAD_COUNT	4

INT MQuickSort(void **ppData, UINT uStart, UINT uEnd, COMPAREFUNC CompareFunc)
{
    void **pp1;
    void **pp2;
    void **pp3;
	INT			i;
	SORTNODE	Node[THREAD_COUNT];
	HANDLE		hThread[THREAD_COUNT];

	INT		nRet = CAPI_FAILED;

	for ( i = 0; i < THREAD_COUNT; i++)
	{
		Node[i].ppData = ppData;
		if ( i == 0 )
		{
			Node[i].uStart = uStart;
		}
		else
		{
			Node[i].uStart = uEnd * i /THREAD_COUNT + 1;  
		}
		Node[i].uEnd = uEnd *(i+1) / THREAD_COUNT;
		Node[i].func = CompareFunc;

		hThread[i] = CreateThread(NULL, 0, QuickSort_Thread, &(Node[i]), 0, NULL);
	}

	for ( i = 0; i < THREAD_COUNT; i++ )
	{
		WaitForSingleObject(hThread[i], INFINITE);
	}


    pp1 = Merge(ppData, uStart, uEnd/4, ppData, uEnd/4+1, uEnd/2, CompareFunc);

    pp2 = Merge(ppData, uEnd/2+1, uEnd*3/4, ppData, uEnd*3/4+1, uEnd, CompareFunc);

    if ( pp1 != NULL && pp2 != NULL )
    {
        pp3 = Merge(pp1, 0, uEnd/2-uStart, pp2, 0, uEnd - uEnd/2 - 1, CompareFunc);

        if ( pp3 != NULL )
        {
            UINT i;
          
            for ( i = uStart; i <= uEnd; i++)
            {
                ppData[i] = pp3[i-uStart];
            }
            free(pp3);
            nRet = CAPI_SUCCESS;
        }
    }
    if( pp1 != NULL)
    {
        free( pp1 );
    }
    if ( pp2 != NULL )
    {
        free( pp2 );
    }

    return nRet;
}


void Parallel_QuickSort1(void **ppData, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc)
{
	UINT uMid = Split(ppData, uStart, uEnd, CompareFunc );
#pragma omp parallel sections 
	{
#pragma omp section
		{
			if ( uMid > uStart )
			{
				QuickSort(ppData, uStart, uMid - 1, CompareFunc);
			}
		}
#pragma omp section
		{
			if ( uEnd > uMid )
			{
				QuickSort(ppData, uMid + 1, uEnd, CompareFunc);
			}
		}
	}
}

#endif 
