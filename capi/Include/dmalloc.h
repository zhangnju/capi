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
 *
 *  DESCRIPTION
 *      Interface to a Distributed Memory management module.
 *
 *  HISTORY
 *      04-19-2008      create by zhouweiming.
 *
 */

#ifndef __DMALLOC_H__
#define __DMALLOC_H__


extern "C" void *dmalloc(UINT uSize);
extern "C" void dfree(void *p);


#endif //__DMALLOC_H__