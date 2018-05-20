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

/**	�ڴ���亯��������ԭʼ��malloc������
    �ڿ������Թ���ʱ��malloc���궨�帲�ǣ��޷����õ�ԭʼ��malloc������
    ����CapiMalloc����ʵ�ֶ�ԭʼmalloc�ĵ��á�

	@param	size_t size - Ҫ�����ڴ���С	
	@return	void * - ���䵽���ڴ�ָ��	
*/
void *org_malloc(size_t size)
{
    return malloc(size);
}

/**	�ڴ���亯��������ԭʼ��malloc������
    �ڿ������Թ���ʱ��realloc���궨�帲�ǣ��޷����õ�ԭʼ��realloc������
    ����CapiMalloc����ʵ�ֶ�ԭʼrealloc�ĵ��á�

	@param	void *p - ԭ�ڴ��ָ��	
	@param	size_t size - �������ڴ��С	
	@return	void * - �����������ڴ�ָ��	
*/
void *org_realloc(void *p, size_t size)
{
    return realloc(p, size);
}


void org_free(void *p)
{
    free(p);
}

