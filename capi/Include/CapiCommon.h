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
 *	CapiCommon.h
 *
 *	DESCRIPTION
 *		Interface to a common functions.
 *
 *	HISTORY
 *		10-02-2007	create by zhouweiming.
 *
 */
#ifndef __CAPICOMMON_H__
#define __CAPICOMMON_H__



#define CACHE_LINE_SIZE     64

int power2(int n);

int BFind(void **ppData, UINT uCount, void *pData, COMPAREFUNC CompareFunc);

int dtn(int n, int min_n);
int dtn2(int n, int min_n);


void *GetCacheAlignedAddr(void *pAddr);
void *GetNextCacheAlignedAddr(void *pAddr);

#endif //__CAPICOMMON_H__

