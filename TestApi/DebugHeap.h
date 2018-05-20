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

#ifndef __DEBUGHEAP_H__
#define __DEBUGHEAP_H__



void *org_malloc(size_t size);
void *org_realloc(void *p, size_t size);
void org_free(void *p);



#endif /* __DEBUGHEAP_H__ */
