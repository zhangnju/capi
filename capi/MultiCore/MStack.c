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
#endif
#include "CapiGlobal.h"
#include "FastLock.h"
#include "Stack.h"
#include "MStack.h"


/**	多线程栈的创建函数

	@param	UINT uStackSize - 栈的大小	
	@return	MSTACK * - 成功返回栈指针，失败返回NULL	
*/
MSTACK * MStack_Create(UINT uStackSize)
{
    MSTACK *pMStack = (MSTACK *)malloc(sizeof(MSTACK));
    if ( pMStack != NULL )
    {
        pMStack->pStack = Stack_Create(uStackSize);
        if ( pMStack->pStack != NULL )
        {
            /* 创建锁 */
            FastLockInit(&(pMStack->FastLock));
            return pMStack;
        }
        free(pMStack);
    }
    return NULL;
}


/**	多线程栈的释放函数

	@param	MSTACK * pMStack - 栈指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
*/
void MStack_Destroy(MSTACK * pMStack, DESTROYFUNC DestroyFunc)
{
    if ( pMStack != NULL )
    {
        FastLockAcquire(&(pMStack->FastLock));
        Stack_Destroy(pMStack->pStack, DestroyFunc);
        FastLockClose(&(pMStack->FastLock));

        free(pMStack);
    }
}


/**	多线程栈的弹出操作函数

	@param	MSTACK * pMStack - 栈指针	
	@return	void * - 弹出的数据指针	
*/
void * MStack_Pop( MSTACK * pMStack )
{
    void *pData;

    FastLockAcquire(&(pMStack->FastLock));
    pData = Stack_Pop(pMStack->pStack);
    FastLockRelease(&(pMStack->FastLock));

    return pData;
}


/**	栈的压入操作函数

	@param	MSTACK * pMStack - 栈指针	
	@param	void *pData - 数据指针	
	@return	INT - CAPI_SUCCESS表示成功，CAIP_FAILED表示失败	
*/
INT MStack_Push( MSTACK * pMStack, void *pData )
{
    INT nRet;

    FastLockAcquire(&(pMStack->FastLock));
    nRet = Stack_Push(pMStack->pStack, pData);
    FastLockRelease(&(pMStack->FastLock));

    return nRet;
}


/**	判断栈是否是空的

	@param	MSTACK *pMStack - 栈指针	
	@return	INT - 0表示为空，1表示非空	
*/
INT MStack_IsEmpty( MSTACK *pMStack )
{
    INT nRet;

    FastLockAcquire(&(pMStack->FastLock));
    nRet = Stack_IsEmpty(pMStack->pStack);
    FastLockRelease(&(pMStack->FastLock));
    
    return nRet;
}


