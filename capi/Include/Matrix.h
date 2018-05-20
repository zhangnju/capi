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
 *	Matrix.h
 *
 *	DESCRIPTION
 *		Interface to a Matrix operation.
 *
 *	HISTORY
 *		05-06-2008	create by zhouweiming.
 *
 */
#ifndef __MATRIX_H__
#define __MATRIX_H__

void Matrix_Multiply(int *a, int row_a, int col_a, 
                     int *b, int row_b,int col_b,
                     int *c, int c_size);


void Parallel_Matrix_Multiply(int *a, int row_a, int col_a, 
                              int *b, int row_b,int col_b,
                              int *c, int c_size );

#endif /* __MATRIX_H__ */