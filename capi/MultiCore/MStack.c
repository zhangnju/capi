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


/**	���߳�ջ�Ĵ�������

	@param	UINT uStackSize - ջ�Ĵ�С	
	@return	MSTACK * - �ɹ�����ջָ�룬ʧ�ܷ���NULL	
*/
MSTACK * MStack_Create(UINT uStackSize)
{
    MSTACK *pMStack = (MSTACK *)malloc(sizeof(MSTACK));
    if ( pMStack != NULL )
    {
        pMStack->pStack = Stack_Create(uStackSize);
        if ( pMStack->pStack != NULL )
        {
            /* ������ */
            FastLockInit(&(pMStack->FastLock));
            return pMStack;
        }
        free(pMStack);
    }
    return NULL;
}


/**	���߳�ջ���ͷź���

	@param	MSTACK * pMStack - ջָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
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


/**	���߳�ջ�ĵ�����������

	@param	MSTACK * pMStack - ջָ��	
	@return	void * - ����������ָ��	
*/
void * MStack_Pop( MSTACK * pMStack )
{
    void *pData;

    FastLockAcquire(&(pMStack->FastLock));
    pData = Stack_Pop(pMStack->pStack);
    FastLockRelease(&(pMStack->FastLock));

    return pData;
}


/**	ջ��ѹ���������

	@param	MSTACK * pMStack - ջָ��	
	@param	void *pData - ����ָ��	
	@return	INT - CAPI_SUCCESS��ʾ�ɹ���CAIP_FAILED��ʾʧ��	
*/
INT MStack_Push( MSTACK * pMStack, void *pData )
{
    INT nRet;

    FastLockAcquire(&(pMStack->FastLock));
    nRet = Stack_Push(pMStack->pStack, pData);
    FastLockRelease(&(pMStack->FastLock));

    return nRet;
}


/**	�ж�ջ�Ƿ��ǿյ�

	@param	MSTACK *pMStack - ջָ��	
	@return	INT - 0��ʾΪ�գ�1��ʾ�ǿ�	
*/
INT MStack_IsEmpty( MSTACK *pMStack )
{
    INT nRet;

    FastLockAcquire(&(pMStack->FastLock));
    nRet = Stack_IsEmpty(pMStack->pStack);
    FastLockRelease(&(pMStack->FastLock));
    
    return nRet;
}


