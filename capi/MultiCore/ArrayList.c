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
#include "CAPIGLOBAL.H"
#include "ArrayList.h"

ARRAYNODE *ArrayNode_Create(UINT uDataLen)
{
	ARRAYNODE	*pNode;

	pNode = (ARRAYNODE *)malloc(sizeof(ARRAYNODE));
	if ( pNode != NULL )
	{
		pNode->ppData = (void **)malloc(sizeof(void *)*uDataLen);
		if ( pNode->ppData == NULL )
		{
			free(pNode);
			pNode = NULL;
		}
		else
		{
			pNode->uDataLen = uDataLen;
			pNode->pNext = NULL;
		}
	}
	return pNode;
}


ARRAYLIST *ArrayList_Create(UINT uDataLen)
{
	ARRAYLIST	*pList;

	pList = (ARRAYLIST *)malloc(sizeof(ARRAYLIST));
	if ( pList != NULL )
	{
		ARRAYNODE *pNode = ArrayNode_Create(uDataLen);
		if ( pNode != NULL )
		{
			free(pList);
			pList = NULL;
		}
		else
		{
			pList->pHead = pNode;
			pList->pTail = pNode;
			pList->uNodeCount = 1;
			pList->uCurPos = 0;
			pList->uDataLen = uDataLen;
		}
	}
	return pList;
}

INT	ArrayList_Add(ARRAYLIST	*pArrayList, void *pData)
{
	ARRAYNODE	*pNode;
	pNode = pArrayList->pTail;

	if ( pArrayList->uCurPos < pArrayList->uDataLen )
	{
		pNode->ppData[pArrayList->uCurPos] = pData;
		pNode->uCurPos++;
		pArrayList->uCurPos++;
	}
	else
	{
		pNode = ArrayNode_Create(pArrayList->uDataLen);
		if ( pNode == NULL )
		{
			return CAPI_FAILED;
		}
		pArrayList->pTail->pNext = pNode;
		pArrayList->pTail = pNode;
		pArrayList->uNodeCount++;

		pNode->ppData[0] = pData;
		pArrayList->uCurPos = 1;
	}
	return CAPI_SUCCESS;
}

void ArrayList_Destroy(ARRAYLIST *pArrayList, DESTROYFUNC func)
{
	if ( pArrayList != NULL )
	{
		UINT		i;
		ARRAYNODE	*pNextNode;
		ARRAYNODE	*pNode = pArrayList->pHead;

		while ( pNode->pNext != NULL )
		{
			pNextNode = pNode->pNext;

			if ( func != NULL )
			{
				//释放节点内的所有数据
				for ( i = 0; i < pArrayList->uDataLen; i++ )
				{
					(*func)( pNode->ppData[i] );
				}
			}
			//释放节点内存
			free(pNode->ppData);
			free(pNode);

			pNode = pNextNode;
		}
		//释放尾部节点内的数据
		if ( func != NULL )
		{
			for ( i = 0; i < pArrayList->uCurPos; i++ )
			{
				(*func)(pNode->ppData[i]);
			}
		}
		//释放尾部节点的内存
		free(pNode);
		//释放ArrayList结构体本身占用的内存
		free(pArrayList);
	}
}