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
#include "CapiGlobal.h"
#include "SortTable.h"
#include "Stack.h"

void SortTable_QuickSort2(SORTTABLE *pTable, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc);

void SortTable_QuickSort(SORTTABLE *pTable, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc);

void SortTable_QuickSort3(SORTTABLE *pTable, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc);

/**	�����Ĵ�������

	@param	UINT uMaxCount - ���������ߴ�	
	@return	SORTTABLE * - �ɹ����������ָ�룬ʧ�ܷ���NULL	
*/
SORTTABLE *	SortTable_Create(UINT uMaxCount)
{
    SORTTABLE *pTable;

    if ( uMaxCount == 0 )
    {
        return NULL;
    }

    pTable = (SORTTABLE *)malloc(sizeof(struct SORTTABLE_st));
    if ( pTable != NULL )
    {
        pTable->ppData = (void **)malloc(uMaxCount * sizeof(void *));
        if ( pTable->ppData != NULL )
        {
            pTable->ppData[0] = NULL;
            pTable->uMaxCount = uMaxCount;
            pTable->uCursorCount = 0;
        }
        else
        {
            free( pTable );
            pTable = NULL;
        }
    }
    return pTable;
}

/**	�������ͷź���

	@param	SORTTABLE *pTable - �����ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷź���	
	@return	void - ��	
*/
void SortTable_Destroy( SORTTABLE *pTable, DESTROYFUNC DestroyFunc )
{
    if ( pTable != NULL )
    {
        if ( DestroyFunc != NULL )
        {
            UINT i;
            for ( i = 0; i < pTable->uCursorCount; i++ )
            {
                (*DestroyFunc)( pTable->ppData[i] );
            }
        }
        free( pTable->ppData );
        free( pTable );
    }
}

/**	������������ݺ��������ݱ���ӵ���β��

	@param	SORTTABLE *pTable - �����ָ��	
	@param	void *pData - ���������ָ��	
	@return	INT (by default) - �ɹ�����CAPI_SUCCESS, ʧ�ܷ���CAPI_FAILED	
*/
INT	SortTable_Add( SORTTABLE *pTable, void *pData )
{
    if ( pTable == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }
    if ( pTable->uCursorCount >= pTable->uMaxCount )
    {
        pTable->ppData = (void **)realloc(pTable->ppData, 
            pTable->uMaxCount * sizeof(void *) * 2 );
        if (pTable->ppData == NULL )
        {
            return CAPI_FAILED;
        }
        pTable->uMaxCount *= 2;
    }
    pTable->ppData[pTable->uCursorCount] = pData;
    pTable->uCursorCount += 1;
    
    return CAPI_SUCCESS;
}

/**	�����Ķ��ֲ��Һ��������ô˺���ǰ����Ա��С�����ź���

	@param	SORTTABLE *pTable - �����ָ��	
	@param	void *pData - Ҫ���ҵ�ƥ������
	@param	COMPAREFUNC CompareFunc - �ȽϺ���
	@return	void * - �ɹ����ز鵽�����ݣ�ʧ�ܷ���NULL
*/
void * SortTable_FindData(SORTTABLE *pTable, void *pData, 
                               COMPAREFUNC CompareFunc)
{
    UINT uLow;
    UINT uHigh;
    UINT uMid;

    if ( pTable == NULL || CompareFunc == NULL || pData == NULL 
        || pTable->uCursorCount == 0 )
    {
        return NULL;
    }

    uLow = 0;
    uHigh = pTable->uCursorCount - 1;
    uMid = 0;
    while ( uLow <= uHigh )
    {
        INT nResult;
        uMid = ( uLow + uHigh ) / 2;
        nResult = (*CompareFunc)( pTable->ppData[uMid], pData );
        if ( nResult > 0 )
        {
            uHigh = uMid - 1;
        }
        else if ( nResult < 0 )
        {
            uLow = uMid + 1;
        }
        else
        {
            /* �Ѿ�������ƥ�����ݣ����� */
            return pTable->ppData[uMid];
        }
    }

    /* δ�ҵ�ƥ�����ݣ����ؿ�ָ�� */
    return NULL;
}

/**	�����Ķ��ֲ��Һ��������ô˺���ǰ����Ա��С�����ź���
    �˺������ܺ�upper_bound()������ͬ

	@param	SORTTABLE *pTable - �����ָ��	
	@param	void *pData - Ҫ���ҵ�ƥ������
	@param	COMPAREFUNC CompareFunc - �ȽϺ���
	@return	void * - �ɹ����ز鵽�ľ�ȷ���ݻ�պñ�Ҫ���ҵ����ݴ������,
                        ʧ�ܷ���NULL��
*/
void * SortTable_BlurFind(SORTTABLE *pTable, void *pData, 
                               COMPAREFUNC CompareFunc)
{
    INT nLow;
    INT nHigh;
    INT nMid;

    if ( pTable == NULL || CompareFunc == NULL || pData == NULL 
        || pTable->uCursorCount == 0 )
    {
        return NULL;
    }

    nLow = 0;
    nHigh = pTable->uCursorCount - 1;
    nMid = 0;
    while ( nLow <= nHigh )
    {
        INT nResult;
        nMid = ( nLow + nHigh ) / 2;
        nResult = (*CompareFunc)( pTable->ppData[nMid], pData );
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
            return pTable->ppData[nMid];
        }
    }

    /* δ�ҵ�ƥ�����ݣ����ظպñ���������� */
    if ( nMid == nLow )
    {
        return pTable->ppData[nLow];
    }
    else if ( nMid == nHigh )
    {
        if ( nMid < (int)(pTable->uCursorCount - 1) )
        {
            return pTable->ppData[nMid + 1];
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return pTable->ppData[nHigh];
    }
}

/**	�����İ������±�����ȡ���ݺ���

	@param	SORTTABLE *pTable -�����ָ��	
	@param	int uId - �����±�����	
	@return	void * - �ɹ���������ָ�룬��������������������ǰ����
                     �ķ�Χ�򷵻�NULL
*/
void * SortTable_GetByID( SORTTABLE *pTable, UINT uId )
{
    void    *pData = NULL;

    if ( pTable != NULL )
    {
        if ( uId < pTable->uCursorCount )
        {
            pData = pTable->ppData[uId];
        }
    }
    return pData;
}

/**	������������

	@param	SORTTABLE *pTable -�����ָ��	
	@param	COMPAREFUNC CompareFunc -���ݱȽϺ���	
	@return	INT -xxxx	
*/
INT SortTable_Sort(SORTTABLE *pTable, COMPAREFUNC CompareFunc)
{
    SortTable_QuickSort(pTable, 0, 
        pTable->uCursorCount - 1, CompareFunc);
    return CAPI_SUCCESS;
}

/**	�����Ŀ�������������������������Χ���ݰ�ָ���������������֣�
    һ����ȫ��С��ָ�����ݣ���һ����ȫ������ָ������
    ָ������ȡ������Χ������еĵ�һ������

	@param	SORTTABLE *pTable -�����ָ��	
	@param	UINT uStart - ���п�ʼλ��	
	@param	UINT uEnd - ���н���λ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	UINT - ָ�������ڱ��е�λ�ã������������±�	
*/
static UINT SortTable_Split(SORTTABLE *pTable, UINT uStart, UINT uEnd,
                     COMPAREFUNC CompareFunc)
{
    void *pSelData;
    UINT uLow;
    UINT uHigh;

    uLow = uStart;
    uHigh = uEnd;

    pSelData = pTable->ppData[uLow];
    while ( uLow < uHigh )
    {
        while ( (*CompareFunc)(pTable->ppData[uHigh], pSelData) > 0 
            && uLow != uHigh )
        {
            --uHigh;
        }
        if ( uHigh != uLow )
        {
            pTable->ppData[uLow] = pTable->ppData[uHigh];
            ++uLow;
        }

        while ( (*CompareFunc)( pTable->ppData[uLow], pSelData ) < 0
            && uLow != uHigh )
        {
             ++uLow;
        }
        if ( uLow != uHigh )
        {
            pTable->ppData[uHigh] = pTable->ppData[uLow];
            --uHigh;
        }
    }
    pTable->ppData[uLow] = pSelData;

    return uLow;
}

/**	���������㷨��ʹ��ջ�ķǵݹ��㷨����

	@param	SORTTABLE *pTable - �����ָ��	
	@param	UINT uStart - ���е�Ҫ���������������㣬Ϊ�����±�	
	@param	UINT uEnd - ����Ҫ��������������յ㣬Ϊ�����±�	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	void - �� 	
*/
void SortTable_QuickSort2(SORTTABLE *pTable, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc)
{
    STACK   *pStack;
    UINT    uLow;
    UINT    uHigh;
    UINT    uMid;

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
            uMid = SortTable_Split(pTable, uLow, uHigh, CompareFunc );
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

/**	�����Ŀ�������������ʹ��ջ���õķǵݹ��㷨
    ��ָ������������ÿ��������㷨�ź�

	@param	SORTTABLE *pTable - �����ָ��	
	@param	UINT uStart - Ҫ������������	
	@param	UINT uEnd - Ҫ����������յ�		
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	void - ��	
*/
void SortTable_QuickSort(SORTTABLE *pTable, UINT uStart, UINT uEnd, 
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
            uMid = SortTable_Split(pTable, uLow, uHigh, CompareFunc );
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

/**	�����Ŀ�������������ָ������������ÿ��������㷨�ź�

	@param	SORTTABLE *pTable - �����ָ��	
	@param	UINT uStart - Ҫ������������	
	@param	UINT uEnd - Ҫ����������յ�		
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	void - ��	
*/
void SortTable_QuickSort3(SORTTABLE *pTable, UINT uStart, UINT uEnd, 
                        COMPAREFUNC CompareFunc)
{
    UINT uMid = SortTable_Split(pTable, uStart, uEnd, CompareFunc );
    if ( uMid > uStart )
    {
        SortTable_QuickSort3(pTable, uStart, uMid - 1, CompareFunc);
    }

    if ( uEnd > uMid )
    {
        SortTable_QuickSort3(pTable, uMid + 1, uEnd, CompareFunc);
    }
}
