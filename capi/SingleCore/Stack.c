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
#include "stack.h"

/**	����һ��ջ

	@param	UINT uStackSize - ջ�Ĵ�С	
	@return	STACK * - �ɹ�����ջָ�룬ʧ�ܷ���NULL��	
*/
STACK *Stack_Create(UINT uStackSize)
{
    STACK *	pStack;

    if ( uStackSize == 0 )
    {
        return NULL;
    }

    pStack = (STACK *)malloc( sizeof(struct STACK_st) );
    if ( pStack != NULL )
    {
        pStack->ppBase = (void **)malloc( uStackSize * sizeof(void *) );
        if ( pStack->ppBase == NULL )
	    {
		    free( pStack );
		    pStack = NULL;
	    }
        else
        {
    	    pStack->ppBase[0] = NULL;
            pStack->uTop = 0;
	        pStack->uStackSize = uStackSize;
        }
	}

    return pStack;
}


/**	ջ���ͷź��������Ὣջ��ʣ���δ���������ͷŵ���

	@param	STACK * pStack - ջָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
*/
void Stack_Destroy(STACK * pStack, DESTROYFUNC DestroyFunc)
{
    if ( pStack != NULL )
    {
        if ( pStack->ppBase != NULL )
	    {
			if ( DestroyFunc != NULL )
			{
				UINT i;
				for ( i = 0; i < pStack->uTop; i++ )
				{
					if ( pStack->ppBase[i] != NULL )
					{
						(*DestroyFunc)(pStack->ppBase[i]);
					}
				}
			}
			free( pStack->ppBase );
        }
        free( pStack );
        pStack = NULL;
    }
}


/**	ջ�ĵ�������������ջ�����ݣ�
    ������������Ҫ�����������ͷŵ���

	@param	STACK * pStack - ջָ��	
    @return	void * - �ɹ�����ջ�����ݣ�ջΪ���򷵻�NULL	
*/
void * Stack_Pop( STACK * pStack )
{
	void	*pData;

	if ( pStack == NULL || pStack->uTop == 0 )
	{
		return NULL;
	}

	pStack->uTop -= 1;

	pData = pStack->ppBase[pStack->uTop];
	
	return pData;
}


/**	ѹջ������������ѹ��ջ�������ݿ���Ϊ��

	@param	STACK * pStack - ջָ��	
	@param	void *pData - Ҫѹ������ݣ�����Ϊ��	
	@return	INT - �ɹ�����CAPI_SUCCESS, ʧ�ܷ���CAPI_FAILED	
*/
INT Stack_Push( STACK * pStack, void *pData )
{
	if ( pStack == NULL )
	{
		return CAPI_FAILED;
	}

    /* �ж�ջ�Ƿ����ˣ����������ջ�ռ�����һ�� */
	if ( pStack->uTop >= pStack->uStackSize - 1 )
	{
		pStack->ppBase = (void **)realloc( pStack->ppBase, 
            ( pStack->uStackSize * 2 ) * sizeof( void * ) );
		if ( pStack->ppBase == NULL )
		{
			return CAPI_FAILED;
		}
		pStack->uStackSize *= 2;
	}
	pStack->ppBase[pStack->uTop] = pData;
	pStack->uTop += 1;

	return CAPI_SUCCESS;
}

/**	�ж�ջ�Ƿ�Ϊ��

	@param	STACK *pStack - ջָ��	
	@return	INT - 1��ʾΪ�գ�0��ʾ��Ϊ��	
*/
INT Stack_IsEmpty(STACK *pStack)
{
    if ( pStack->uTop == 0 )
    {
        return 1;
    }
    return 0;
}
