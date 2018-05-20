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
#include <stdlib.h>
#include "CapiGlobal.h"
#include "CapiCommon.h"

/**	矩阵串行乘法函数

    @param	int *a - 指向要相乘的第1个矩阵的指针
    @param	int row_a - 矩阵a的行数
    @param	int col_a - 矩阵a的列数

    @param	int *b - 指向要相乘的第2个矩阵的指针	
    @param	int row_b - 矩阵b的行数
    @param	int col_b - 矩阵b的列数

    @param	int *c - 计算结果的矩阵的指针
    @param	int c_size - 矩阵c的空间大小（总元素个数）

    @return	void - 无	
*/
void Matrix_Multiply(int *a, int row_a, int col_a, 
                      int *b, int row_b,int col_b,
                      int *c, int c_size)
{
    if ( col_a != row_b || c_size < row_a * col_b )
    {
        return;
    }

    int i, j, k;

    for ( i = 0; i < row_a; i++ )
    {
        int row_i = i * col_a;
        int row_c = i * col_b;
        for ( j = 0; j < col_b; j++ )
        {
            c[row_c + j] = 0;
            for ( k = 0; k < row_b; k++ )
            {
                c[row_c + j] += a[row_i + k] * b[k * col_b + j];
            }
        }
    }
}


/**	矩阵并行乘法函数

    @param	int *a - 指向要相乘的第1个矩阵的指针
    @param	int row_a - 矩阵a的行数
    @param	int col_a - 矩阵a的列数

    @param	int *b - 指向要相乘的第2个矩阵的指针	
    @param	int row_b - 矩阵b的行数
    @param	int col_b - 矩阵b的列数

    @param	int *c - 计算结果的矩阵的指针
    @param	int c_size - 矩阵c的空间大小（总元素个数）

    @return	void - 无	
*/
void Parallel_Matrix_Multiply(int *a, int row_a, int col_a, 
                     int *b, int row_b,int col_b,
                     int *c, int c_size )
{
    if ( col_a != row_b )
    {
        return;
    }

    int i, j, k; 
    int index;
    int border = row_a * col_b;

    i = 0; 
    j = 0;
#pragma omp parallel private(i,j,k) num_threads(dtn(border, 1))
    for ( index = 0; index < border; index++ )
    {
        i = index / col_b;
        j = index % col_b;

        int row_i = i * col_a;
        int row_c = i * col_b;

        c[row_c+j] = 0;
        for ( k = 0; k < row_b; k++ )
        {
            c[row_c + j] += a[row_i+k] * b[k*col_b+j];
        }
    }
}


