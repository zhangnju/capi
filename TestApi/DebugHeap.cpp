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
#include "DebugHeap.h"

/**	内存分配函数，调用原始的malloc函数。
    在开发调试功能时，malloc被宏定义覆盖，无法调用到原始的malloc函数。
    调用CapiMalloc可以实现对原始malloc的调用。

	@param	size_t size - 要分配内存块大小	
	@return	void * - 分配到的内存指针	
*/
void *org_malloc(size_t size)
{
    return malloc(size);
}

/**	内存分配函数，调用原始的malloc函数。
    在开发调试功能时，realloc被宏定义覆盖，无法调用到原始的realloc函数。
    调用CapiMalloc可以实现对原始realloc的调用。

	@param	void *p - 原内存块指针	
	@param	size_t size - 扩大后的内存大小	
	@return	void * - 返回扩大后的内存指针	
*/
void *org_realloc(void *p, size_t size)
{
    return realloc(p, size);
}


void org_free(void *p)
{
    free(p);
}

