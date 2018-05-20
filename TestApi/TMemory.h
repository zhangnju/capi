/* Copyright (c) 2006-2008
 * Author: Weiming Zhou
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  
 */
/* 
 *	TMemory.h
 *
 *	DESCRIPTION
 *		Interface to a memory leak and overrun check object. 
 *
 *	HISTORY
 *		08-22-2006	create by zhouweiming.
 *
 */


#ifndef __TMEMORYLEAK_H__
#define __TMEMORYLEAK_H__

#include "THashTable.h"

#define MEM_CHECK_FLAG  0xfeeefeee


class TCheckMemory {
public:

    TCheckMemory();
    void Destroy();
    ~TCheckMemory();

    void TCheckMemoryLeak();
    void TCheckMemoryOverrun();
    void TCheckMemAll(); //Both check memory leaks and overrun.
};

extern "C" {
void * Tmalloc_dbg(size_t size, char *pszFile, UINT uLine);
void  Tfree_dbg(void *p, char *pszFile, UINT uLine);
void * Tcalloc_dbg(size_t num, size_t size, char *pszFile, UINT uLine);
void * Trealloc_dbg(void *memblock, size_t size, char *pszFile, UINT uLine);

char *Tstrdup_dbg(const char *strSource, char *pszFile, UINT uLine);
wchar_t *Twcsdup_dbg(const wchar_t *strSource, char *pszFile, UINT uLine);

}

void *Tmalloc(size_t size);
void *Trealloc(void *p, size_t size);
void Tfree( void *p);

void TMemHookInit(void);

void *unhooked_malloc(size_t size);
void *unhooked_realloc(void *p, size_t size);
void unhooked_free(void *p);

void TMemHookClose(void);

/**	检查malloc分配的内存泄漏情况

	@return	INT - 0表示没有泄漏，大于0表示泄漏的内存块数。
*/
INT TCheckMallocLeak(void);

#endif //__TESTAPI_H__
