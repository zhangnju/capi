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
 *	CFastRandom.h
 *
 *	DESCRIPTION
 *		Module for Fast Random class 
 *
 *	HISTORY
 *		06-08-2008	create by zhouweiming.
 *
 */
#ifndef __CFASTRANDOM_H__
#define __CFASTRANDOM_H__

// A fast random number generator.
// Uses linear congruential method. 
class CFastRandom {
private:
    unsigned int m_uSeed; 
    unsigned int m_uPrime;
public:
    // Get a random number.
    unsigned short Get();

    // Construct a random number generator.
    CFastRandom( unsigned uSeed );
    CFastRandom();
    void Init(unsigned uSeed);
};


#endif //__CFASTRANDOM_H__

