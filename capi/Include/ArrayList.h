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
*	ArrayList.h
*
*	DESCRIPTION
*		Interface to a ArrayList Object type.
*
*	HISTORY
*		10-25-2007	create by zhouweiming.
*
*/

#ifndef __ARRAYLIST_H__
#define __ARRAYLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ARRAYNODE_st {
	void		**ppData;
	UINT		uDataLen;
	UINT		uCurPos;
	struct ARRAYNODE_st	*pNext;
} ARRAYNODE;

typedef struct ARRAYLIST_st {
	ARRAYNODE	*pHead;
	ARRAYNODE	*pTail;
	UINT		uNodeCount;
	UINT		uCurPos;
	UINT		uDataLen;
} ARRAYLIST;


ARRAYNODE *ArrayNode_Create(UINT uDataLen);


ARRAYLIST *ArrayList_Create(UINT uDataLen);

INT	ArrayList_Add(ARRAYLIST	*pArrayList, void *pData);

void ArrayList_Destroy(ARRAYLIST *pArrayList, DESTROYFUNC func);

#ifdef __cplusplus
}
#endif

#endif //__ARRAYLIST_H__
