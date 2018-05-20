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
#if _WIN32 || _WIN64
#include <windows.h>
#endif

#include "CapiGlobal.h"

#if _WIN32 

extern "C" BOOL AtomicCAS(LONG volatile *dest, LONG newvalue, LONG oldvalue)
{
    LONG 	ret;
    ret = InterlockedCompareExchange(dest, newvalue, oldvalue);
    if ( ret == oldvalue )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

extern "C" BOOL AtomicCAS64(LONGLONG volatile *dest, LONGLONG newvalue, LONGLONG oldvalue)
{
    // not implement yet
    return FALSE;
}



#if 0
extern "C" BOOL AtomicCAS64(LONGLONG volatile *dest, LONGLONG newvalue, LONGLONG oldvalue)
{
    LONGLONG 	ret;
    ret = InterlockedCompareExchange64(dest, newvalue, oldvalue);
    if ( ret == oldvalue )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif


#else

LONG AtomicWrite(LONG volatile *Target, LONG Value)
{
    LONG	Old;

    do {
        Old = *Target;

    }while (!AtomicCAS(Target, Value, Old));

    return Old;
}

LONG AtomicIncrement(LONG volatile *Target)
{
    LONG	Old;

    do {
        Old = *Target;

    }while (!AtomicCAS(Target, Old + 1, Old));

    return Old;
}


LONG AtomicDecrement(LONG volatile *Target)
{
    LONG	Old;

    do {
        Old = *Target;

    }while (!AtomicCAS(Target, Old - 1, Old));

    return Old;
}

BOOL TAS(LONG volatile *value)
{
    LONG	ret;
    ret = AtomicCAS(value, 1, 0);
    if ( ret == 0 )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


#endif


