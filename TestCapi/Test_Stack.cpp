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
#include "TestApi.h"
#include "ApiHook.h"
#include "TestCapi.h"
#include "Stack.h"

void DRV_Stack_Create(UINT i);
void DRV_Stack_Pop(UINT i);
void DRV_Stack_Push(UINT i);
void DRV_Stack_Destroy(UINT i);

typedef STACK Stack;

void Test_Stack()
{
    int     i;
    for ( i = 1; i < 50; i++ )
    {
        DRV_Stack_Create(i);
        DRV_Stack_Pop(i);
        DRV_Stack_Push(i);
		DRV_Stack_Destroy(i);
    }    
}

REGISTER_TESTFUNC(Test_Stack)

void DRV_Stack_Create(UINT i)
{
    STACK *pStack = NULL;
    switch( i )
    {
    case 1:
        pStack = Stack_Create(0);
		assertEquals(pStack, NULL);
        break;
    case 2:
        pStack = Stack_Create(1);
		assertNotEquals(pStack, NULL);
		assertNotEquals(pStack->ppBase, NULL);

		assertEquals(pStack->uStackSize, 1);
		assertEquals(pStack->uTop, 0);
        break;
    case 3:
        pStack = Stack_Create(2);
		assertNotEquals(pStack, NULL);
		assertNotEquals(pStack->ppBase, NULL);

		assertEquals(pStack->uStackSize, 2);
		assertEquals(pStack->uTop, 0);
        break;
    case 4:
        pStack = Stack_Create(1024);
		assertNotEquals(pStack, NULL);
		assertNotEquals(pStack->ppBase, NULL);

		assertEquals(pStack->uStackSize, 1024);
		assertEquals(pStack->uTop, 0);
        break;
    default:
        break;
    }
    if ( pStack != NULL && pStack->uStackSize != 0  )
    {
        Stack_Destroy(pStack, NULL);
    }
}


void DRV_Stack_Push(UINT i)
{
    INT     nRet;
    Stack *pStack = NULL;
    pStack = Stack_Create(2);
    switch( i )
    {
    case 1:
        nRet = Stack_Push(pStack, strdup("Stack_1"));

		assertEquals(nRet, CAPI_SUCCESS);
		assertEquals(strcmp((char *)pStack->ppBase[0], "Stack_1"), 0 );
		assertEquals(pStack->uStackSize, 2);
		assertEquals(pStack->uTop, 1);
        break;
    case 2:
        Stack_Push(pStack, strdup("Stack_1"));
        nRet = Stack_Push(pStack, strdup("Stack_2"));

		assertEquals(nRet, CAPI_SUCCESS);
		assertEquals(strcmp((char *)pStack->ppBase[0], "Stack_1"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[1], "Stack_2"), 0);
		assertEquals(pStack->uStackSize, 4);
		assertEquals(pStack->uTop, 2);

        break;
    case 3:
        Stack_Push(pStack, strdup("Stack_1"));
        Stack_Push(pStack, strdup("Stack_2"));
        nRet = Stack_Push(pStack, strdup("Stack_3"));

		assertEquals(nRet, CAPI_SUCCESS);
		assertEquals(strcmp((char *)pStack->ppBase[0], "Stack_1"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[1], "Stack_2"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[2], "Stack_3"), 0);
		assertEquals(pStack->uStackSize, 4);
		assertEquals(pStack->uTop, 3);
        
        break;
    case 4:
        break;
    default:
        break;
    }
    if ( pStack != NULL  )
    {
        Stack_Destroy(pStack, free);
    }
}

void DRV_Stack_Pop(UINT i)
{
    void    *pData;
    Stack *pStack = NULL;
    pStack = Stack_Create(128);
    switch( i )
    {
    case 1:
        Stack_Push(pStack, strdup("Stack_8"));
        Stack_Push(pStack, strdup("Stack_3"));
        Stack_Push(pStack, strdup("Stack_1"));
        Stack_Push(pStack, strdup("Stack_4"));
        Stack_Push(pStack, strdup("Stack_6"));
        Stack_Push(pStack, strdup("Stack_7"));
        Stack_Push(pStack, strdup("Stack_9"));
        Stack_Push(pStack, strdup("Stack_5"));
        Stack_Push(pStack, strdup("Stack_2"));
        Stack_Push(pStack, strdup("Stack_0"));
        Stack_Push(pStack, strdup("Stack_99"));

        pData = Stack_Pop(pStack);
		assertEquals(strcmp((char *)pData, "Stack_99"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[0], "Stack_8"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[1], "Stack_3"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[2], "Stack_1"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[3], "Stack_4"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[4], "Stack_6"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[5], "Stack_7"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[6], "Stack_9"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[7], "Stack_5"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[8], "Stack_2"), 0);
		assertEquals(strcmp((char *)pStack->ppBase[9], "Stack_0"), 0);

		assertEquals(pStack->uStackSize, 128);
		assertEquals(pStack->uTop, 10);

        free(pData);
        pData = Stack_Pop(pStack);

		assertEquals(strcmp((char *)pData, "Stack_0"), 0);
        free(pData);

        break;
    case 2:
        Stack_Push(pStack, strdup("Stack_8"));
        pData = Stack_Pop(pStack);

		assertEquals(pStack->uStackSize, 128);
		assertEquals(pStack->uTop, 0);
		assertEquals(strcmp((char *)pData, "Stack_8"), 0);
        free(pData);

        break;
    case 3:
        Stack_Push(pStack, strdup("Stack_8"));
        Stack_Push(pStack, strdup("Stack_5"));
        
		pData = Stack_Pop(pStack);
		assertEquals(pStack->uStackSize, 128);
		assertEquals(pStack->uTop, 1);
		assertEquals(strcmp((char *)pData, "Stack_5"), 0);
        free(pData);

        pData = Stack_Pop(pStack);
		assertEquals(pStack->uStackSize, 128);
		assertEquals(pStack->uTop, 0);
		assertEquals(strcmp((char *)pData, "Stack_8"), 0);
        free(pData);

        pData = Stack_Pop(pStack);
		assertEquals(pStack->uStackSize, 128);
		assertEquals(pStack->uTop, 0);
		assertEquals(pData, NULL);

        break; 
    case 4:
        pData = Stack_Pop(pStack);
		assertEquals(pStack->uStackSize, 128);
		assertEquals(pStack->uTop, 0);
		assertEquals(pData, NULL);

		assertTrue(Stack_IsEmpty(pStack));
        break;
    default:
        break;
    }
    if ( pStack != NULL  )
    {
        Stack_Destroy(pStack, free);
    }
}


void Stub_free(void *p)
{
}

void DRV_Stack_Destroy(UINT i)
{

#ifdef _DEBUG
    void    *pData;
    void	*pData2;
    void	*pBase;
    INT    flag  = FREE_MEM_FLAG;

    Stack *pStack = NULL;
    pStack = Stack_Create(128);
    switch( i )
    {
    case 1:
		pBase = (void *)pStack->ppBase;
		Stack_Destroy(pStack, NULL);
        printf ( "flag = 0x%lx\n", *(INT *)pBase);
		assertEquals(*(INT *)pStack, flag);
		assertEquals(*(INT *)pBase, flag);
		break;
	case 2:
		Stack_Push(pStack, (void *)100);
		pData = pStack->ppBase[0];
		pBase = (void *)pStack->ppBase;
		Stack_Destroy(pStack, NULL);
		assertEquals(pData, (void *)100);
		assertEquals(*(INT *)pStack, flag);
		assertEquals(*(INT *)pBase, flag);
		break;
	case 3:
		Stack_Push(pStack, strdup("123456"));
		pData = pStack->ppBase[0];
		pBase = pStack->ppBase;
		Stack_Destroy(pStack, free);
		assertEquals(*(INT *)pData, flag);
		assertEquals(*(INT *)pStack, flag);
		assertEquals(*(INT *)pBase, flag);
		break;
	case 4:
		Stack_Push(pStack, strdup("123456"));
		Stack_Push(pStack, strdup("7891011"));
		pData = pStack->ppBase[0];
		pData2 = pStack->ppBase[1];
		pBase = pStack->ppBase;
		Stack_Destroy(pStack, free);
		assertEquals(*(INT *)pData, flag);
		assertEquals(*(INT *)pData2, flag);
		assertEquals(*(INT *)pStack, flag);
		assertEquals(*(INT *)pBase, flag);
		break;
	default:
        Stack_Destroy(pStack, NULL);
		break;
	}
#endif // #ifdef _DEBUG

}

 