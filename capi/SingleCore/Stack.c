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

/**	创建一个栈

	@param	UINT uStackSize - 栈的大小	
	@return	STACK * - 成功返回栈指针，失败返回NULL。	
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


/**	栈的释放函数，它会将栈中剩余的未弹出数据释放掉，

	@param	STACK * pStack - 栈指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
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


/**	栈的弹出函数，弹出栈顶数据，
    弹出的数据需要调用者自行释放掉。

	@param	STACK * pStack - 栈指针	
    @return	void * - 成功返回栈顶数据，栈为空则返回NULL	
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


/**	压栈函数，将数据压入栈顶，数据可以为空

	@param	STACK * pStack - 栈指针	
	@param	void *pData - 要压入的数据，可以为空	
	@return	INT - 成功返回CAPI_SUCCESS, 失败返回CAPI_FAILED	
*/
INT Stack_Push( STACK * pStack, void *pData )
{
	if ( pStack == NULL )
	{
		return CAPI_FAILED;
	}

    /* 判断栈是否满了，如果满了则将栈空间增大一倍 */
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

/**	判断栈是否为空

	@param	STACK *pStack - 栈指针	
	@return	INT - 1表示为空，0表示不为空	
*/
INT Stack_IsEmpty(STACK *pStack)
{
    if ( pStack->uTop == 0 )
    {
        return 1;
    }
    return 0;
}
