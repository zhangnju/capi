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
 *	Parallel_For.h
 *
 *	DESCRIPTION
 *		Interface to a generic Parallel_For function.
 *	HISTORY
 *		09-20-2008	create by zhouweiming.
 *
 */
#ifndef __PARALLEL_FOR_H__
#define __PARALLEL_FOR_H__

#include "CThreadPool.h"
#include "CDistributedQueue.h"
#include "CNestTaskScheduler.h"

class CRange {
protected:
    CNestTaskScheduler  *m_pTaskScheduler;
public:
    CRange(){};
    CRange(CNestTaskScheduler *p);
    void SetTaskScheduler(CNestTaskScheduler *p);
    CNestTaskScheduler *GetTaskScheduler();

    virtual CRange * Split() = 0;
};

void Parallel_For(CRange *pRange );


#endif //__PARALLEL_FOR_H__
