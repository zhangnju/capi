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
 *	CLockFreeStack.h
 *
 *	DESCRIPTION
 *		Module for Lock-free stack class 
 *
 *	HISTORY
 *		03-08-2008	create by zhouweiming.
 *
 */
#ifndef __CLOCKFREESTACK_H__
#define __CLOCKFREESTACK_H__

struct LINK {
    SINGLENODE   *Ptr;
    int    nRefCount;
};

class  CLockFreeStack {
private:
    LINK         m_Head;
    DESTROYFUNC  m_DestroyFunc;
public:
    CLockFreeStack() { 
        m_Head.Ptr = NULL; 
        m_Head.nRefCount = 0;
        m_DestroyFunc = NULL;
    };
    virtual ~CLockFreeStack();

    void SetDataDestroyFunc(DESTROYFUNC DestroyFunc) {
        m_DestroyFunc = DestroyFunc;
    };

    void *Pop();
    void Push(void *pData);

};

#endif // __CLOCKFREESTACK_H__

