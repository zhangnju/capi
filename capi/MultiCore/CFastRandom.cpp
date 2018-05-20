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

#include "CFastRandom.h"

// Table of primes used by fast random-number generator.
// Also serves to keep anything else from being placed in the same
// cache line as the global data items preceding it.
static const unsigned g_uPrimes[] = {
    0x9e3779b1, 0xffe6cc59, 0x2109f6dd, 0x43977ab5,
    0xba5703f5, 0xb495a877, 0xe1626741, 0x79695e6b,
    0xbc98c09f, 0xd5bee2b3, 0x287488f9, 0x3af18231,
    0x9677cd4d, 0xbe3a6929, 0xadc6a877, 0xdcf0674b,
    0xbe4d6fe9, 0x5f15e201, 0x99afc3fd, 0xf3f16801,
    0xe222cfff, 0x24ba5fdb, 0x0620452d, 0x79f149e3,
    0xc8b93f49, 0x972702cd, 0xb07dd827, 0x6c97d5ed,
    0x085a3d61, 0x46eb5ea7, 0x3d9910ed, 0x2e687b5b,
    0x29609227, 0x6eb081f1, 0x0954c4e1, 0x9d114db9,
    0x542acfa9, 0xb3e6bd7b, 0x0742d917, 0xe9f3ffa7,
    0x54581edb, 0xf2480f45, 0x0bb9288f, 0xef1affc7,
    0x85fa0ca7, 0x3ccc14db, 0xe6baf34b, 0x343377f7,
    0x5ca19031, 0xe6d9293b, 0xf0a9f391, 0x5d2e980b,
    0xfc411073, 0xc3749363, 0xb892d829, 0x3549366b,
    0x629750ad, 0xb98294e5, 0x892d9483, 0xc235baf3,
    0x3d2402a3, 0x6bdef3c9, 0xbec333cd, 0x40c9520f
};


CFastRandom::CFastRandom( unsigned uSeed ) 
{
    m_uSeed = uSeed;
    m_uPrime = g_uPrimes[uSeed%(sizeof(g_uPrimes)/sizeof(g_uPrimes[0]))];
}


CFastRandom::CFastRandom( ) 
{
    m_uSeed = 1;
    m_uPrime = g_uPrimes[0];
}


void CFastRandom::Init( unsigned uSeed ) 
{
    m_uSeed = uSeed;
    m_uPrime = g_uPrimes[uSeed%(sizeof(g_uPrimes)/sizeof(g_uPrimes[0]))];
}


unsigned short CFastRandom::Get() 
{
    unsigned short uRandom = m_uSeed >> 16;
    m_uSeed = m_uSeed * m_uPrime + 1;
    return uRandom;
}

