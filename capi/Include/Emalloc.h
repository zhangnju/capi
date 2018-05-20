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
 *	Emalloc.h
 *
 *	DESCRIPTION
 *		Module use equal size memeory management algorithm 
                to manage memory.
 *
 *	HISTORY
 *		09-09-2000	create by zhouweiming.
 *
 */


#ifndef __EMALLOC_H__
#define __EMALLOC_H__


#ifdef __cplusplus
extern "C" {
#endif

/* create/destroy functions */
INT Emalloc_Create( void );
void Emalloc_Destroy(void);

INT Emalloc_Add(UINT uMemSize, UINT uMemCount);

void *Emalloc(size_t size);
void Efree(void *p, size_t size);


#ifdef __cplusplus
}
#endif

#endif /* __EMALLOC_H__ */
