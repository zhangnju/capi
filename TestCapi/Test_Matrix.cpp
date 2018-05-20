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
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "CapiGlobal.h"
#include "Matrix.h"
#include "TestApi.h"
#include "TestCapi.h"


void TestCase_Matrix_Multiply_1(void);
void TestCase_Matrix_Multiply_2(void);
void TestCase_Matrix_Multiply_3(void);
void TestCase_Matrix_Multiply_4(void);

void TestCase_Parallel_Matrixply_1(void);
void TestCase_Parallel_Matrixply_2(void);
void TestCase_Parallel_Matrixply_3(void);

void Test_Matrix()
{
    TestCase_Add(TestCase_Matrix_Multiply_1);
    TestCase_Add(TestCase_Matrix_Multiply_2);
    TestCase_Add(TestCase_Matrix_Multiply_3);
    TestCase_Add(TestCase_Matrix_Multiply_4);

    TestCase_Add(TestCase_Parallel_Matrixply_1);
    TestCase_Add(TestCase_Parallel_Matrixply_2);
    TestCase_Add(TestCase_Parallel_Matrixply_3);
}

REGISTER_TESTFUNC(Test_Matrix)


int a1[2][2] = {{11, 20}, {15, 30} };
int b1[2][2] = {{5, 8}, {50, 10}};

int x[2][3] = {{1,2,3}, {4,5,6}};
int y[3][2] = {{1,2}, {3,4},{5,6}};

int a2[4][4] = {{5, 10, 15, 20}, {100, 100, 150, 200}, {30, 40, 50, 60}, {70,80, 90, 100}};
int b2[4][4] = {{15,20, 25, 30}, {200, 300, 250, 400}, {15, 25, 35, 45}, {170,180, 190, 200}};


int a3[3][3] = {1,2,3,4,5,6,7,8,9};
int b3[3][3] = {1,2,3,0,3,5,2,0,5};

#if 0
TestCase_Matrix_Multiply_1
a[0][0] = 1055    a[0][1] = 288
a[1][0] = 1575    a[1][1] = 420
TestCase_Matrix_Multiply_2
a[0][0] = 5700    a[0][1] = 7075    a[0][2] = 6950    a[0][3] = 8825
a[1][0] = 57750    a[1][1] = 71750    a[1][2] = 70750    a[1][3] = 89750
a[2][0] = 19400    a[2][1] = 24650    a[2][2] = 23900    a[2][3] = 31150
a[3][0] = 35400    a[3][1] = 45650    a[3][2] = 43900    a[3][3] = 58150
TestCase_Matrix_Multiply_3
a[0][0] = 22    a[0][1] = 28
a[1][0] = 49    a[1][1] = 64
TestCase_Matrix_Multiply_4
a[0][0] = 7    a[0][1] = 8    a[0][2] = 28
a[1][0] = 16    a[1][1] = 23    a[1][2] = 67
a[2][0] = 25    a[2][1] = 38    a[2][2] = 106
TestCase_Parallel_Matrix_Multiply_1
a[0][0] = 1055    a[0][1] = 288
a[1][0] = 1575    a[1][1] = 420
TestCase_Parallel_Matrix_Multiply_2
a[0][0] = 5700    a[0][1] = 7075    a[0][2] = 6950    a[0][3] = 8825
a[1][0] = 57750    a[1][1] = 71750    a[1][2] = 70750    a[1][3] = 89750
a[2][0] = 19400    a[2][1] = 24650    a[2][2] = 23900    a[2][3] = 31150
a[3][0] = 35400    a[3][1] = 45650    a[3][2] = 43900    a[3][3] = 58150
TestCase_Parallel_Matrix_Multiply_3
a[0][0] = 22    a[0][1] = 28
a[1][0] = 49    a[1][1] = 64

#endif


void DumpMatrix(int *a, int row, int col)
{
    int i, j;

    for ( i = 0; i < row; i++ )
    {
        for ( j = 0; j < col; j++ )
        {
            printf( "a[%ld][%ld] = %ld    ", i, j, a[i*row+j] );
        }
        printf("\n");
    }
}


void TestCase_Matrix_Multiply_1(void)
{
    int c1[2][2];

    Matrix_Multiply(reinterpret_cast<int *>(a1), 2, 2, 
        reinterpret_cast<int *>(b1), 2, 2, reinterpret_cast<int *>(c1), 4);

 //   printf("TestCase_Matrix_Multiply_1\n");
//    DumpMatrix((int *)c1, 2, 2);
}

void TestCase_Matrix_Multiply_2(void)
{
    int c1[4][4];

    Matrix_Multiply(reinterpret_cast<int *>(a2), 4, 4, 
        reinterpret_cast<int *>(b2), 4, 4, reinterpret_cast<int *>(c1), 16);

//    printf("TestCase_Matrix_Multiply_2\n");
//    DumpMatrix(reinterpret_cast<int *>(c1), 4, 4);
}


void TestCase_Matrix_Multiply_3(void)
{
    int c1[2][2];

    Matrix_Multiply(reinterpret_cast<int *>(x), 2, 3, 
        reinterpret_cast<int *>(y), 3, 2, reinterpret_cast<int *>(c1), 4);

//    printf("TestCase_Matrix_Multiply_3\n");
//    DumpMatrix(reinterpret_cast<int *>(c1), 2, 2);
}

void TestCase_Matrix_Multiply_4(void)
{
    int c1[3][3];

    Matrix_Multiply(reinterpret_cast<int *>(a3), 3, 3, 
        reinterpret_cast<int *>(b3), 3, 3, reinterpret_cast<int *>(c1), 9);

//    printf("TestCase_Matrix_Multiply_4\n");
//    DumpMatrix(reinterpret_cast<int *>(c1), 3, 3);
}


void TestCase_Parallel_Matrixply_1(void)
{
    int c1[2][2];

    Parallel_Matrix_Multiply(reinterpret_cast<int *>(a1), 2, 2, 
        reinterpret_cast<int *>(b1), 2, 2, reinterpret_cast<int *>(c1), 4);

//    printf("TestCase_Parallel_Matrix_Multiply_1\n");
//    DumpMatrix(reinterpret_cast<int *>(c1), 2, 2);
}

void TestCase_Parallel_Matrixply_2(void)
{
    int c1[4][4];

    Parallel_Matrix_Multiply(reinterpret_cast<int *>(a2), 4, 4, 
        reinterpret_cast<int *>(b2), 4, 4, reinterpret_cast<int *>(c1), 16);

//    printf("TestCase_Parallel_Matrix_Multiply_2\n");
//    DumpMatrix(reinterpret_cast<int *>(c1), 4, 4);
}

void TestCase_Parallel_Matrixply_3(void)
{
    int c1[2][2];

    Parallel_Matrix_Multiply(reinterpret_cast<int *>(x), 2, 3, 
        reinterpret_cast<int *>(y), 3, 2, reinterpret_cast<int *>(c1), 4);

//    printf("TestCase_Parallel_Matrix_Multiply_3\n");
//    DumpMatrix(reinterpret_cast<int *>(c1), 2, 2);
}
