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
 *	TestApi.h
 *
 *	DESCRIPTION
 *		Macros for malloc¡¢free¡¢realloc etc.
 *      if some source need check memory leak and overrun, it need use
 *		this header file.
 *
 *	HISTORY
 *		08-15-2006	create by zhouweiming.
 *
 */

#ifndef __TESTAPI_H__
#define __TESTAPI_H__

//#define _TESTAPI_DEBUG
#include "CapiGlobal.h"
#include "TMemory.h"
#include "SortTable.h"
#include "TestCaseMgr.h"
#include "TestApi_Assert.h"
#include "TestManager.h"

#ifdef _TESTAPI_DEBUG

#define malloc(s)         Tmalloc_dbg(s, __FILE__, __LINE__)
#define free(s)           Tfree_dbg(s, __FILE__, __LINE__)
//#define calloc(c, s)      Tcalloc_dbg(c, s,  __FILE__, __LINE__)
//#define realloc(p, s)     Trealloc_dbg(p, s, __FILE__, __LINE__)
#define strdup(s)         Tstrdup_dbg(s,  __FILE__, __LINE__)
#define wcsdup(s)         Twcsdup_dbg(s,  __FILE__, __LINE__)


#endif // _TESTAPI_DEBUG





#endif // __TESTAPI_H__
