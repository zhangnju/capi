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
 *	TestApi_Assert.h
 *
 *	DESCRIPTION
 *		Prototypes and definition for assert macros.
 *
 *	History:
 *		08-3-2006	create by zhouweiming.
 *
 */

#ifndef __TESTAPI_ASSERT_H__
#define __TESTAPI_ASSERT_H__

#define assertFalse(x)\
{\
	if ( (x) )\
	{\
		printf( "File %s, Line:%d, assert failed.\n", __FILE__, __LINE__ );\
	}\
}

#define assertTrue(x)\
{\
	if ( !(x) )\
	{\
		printf( "File %s, Line:%d, assert failed.\n", __FILE__, __LINE__ );\
	}\
}

#define assertEquals(x, y)\
{\
	if ( (x) != (y) )\
	{\
		printf( "File %s, Line:%d, assert failed.\n", __FILE__, __LINE__);\
	}\
}


#define assertNotEquals(x, y)\
{\
	if ( (x) == (y) )\
	{\
		printf( "File %s, Line:%d, assert failed.\n", __FILE__, __LINE__);\
	}\
}

#define assertLess(x, y)\
{\
	if ( (x) >= (y) )\
	{\
		printf( "File %s, Line:%d, assert failed.\n", __FILE__, __LINE__);\
	}\
}

#endif /* __TESTAPI_ASSERT_H__ */
