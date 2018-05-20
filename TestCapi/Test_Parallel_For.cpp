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


// Test_Parallel_Search.cpp
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "Testapi.h"
#include "CapiCommon.h"
#include "Parallel_For.h"
#include "CQuickSortRange.h"
#include "CMergeRange.h"

extern "C" void QuickSort(void **ppData, UINT uStart, UINT uEnd, 
               COMPAREFUNC CompareFunc);
#if 0
int DRangeCompare(void *p1, void *p2)
{
    DRange *pRange1 = (DRange *)p1;
    DRange *pRange2 = (DRange *)p2;

    if ( pRange1->addr > pRange2->addr )
    {
        return 1;
    }
    else if ( pRange1->addr < pRange2->addr )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}
#endif

void TestCase_Parallel_For(void);
void TestCase_Parallel_For2(void);
void TestCase_Parallel_For_QuickSort(void);
void TestCase_Parallel_For_Merge(void);
void TestCase_LowerBound(void);
void TestCase_Inter_Split(void);

void Test_Parallel_For()
{
    TestCase_Add(TestCase_Parallel_For);

    TestCase_Add(TestCase_Parallel_For2);

    TestCase_Add(TestCase_Parallel_For_QuickSort);
    TestCase_Add(TestCase_Parallel_For_Merge);
    TestCase_Add(TestCase_LowerBound);
    TestCase_Add(TestCase_Inter_Split);
}

//REGISTER_TESTFUNC(Test_Parallel_For)


void TestCase_Parallel_For(void)
{
}

void TestCase_Parallel_For2(void)
{
}


#define QUICKSORT_DATA_SIZE    1000000

void TestCase_Parallel_For_QuickSort(void)
{
    int * pData = new int[QUICKSORT_DATA_SIZE];

    int *pCopyData = new int[QUICKSORT_DATA_SIZE];

    UINT uRand = (UINT)time(NULL);

    srand(uRand);

    int i;

    for (i = 0; i < QUICKSORT_DATA_SIZE; i++ )
    {
        pData[i] = rand();
        pCopyData[i] = pData[i];
    }
    CQuickSortRange<int> *pRange = new CQuickSortRange<int>(pData, 0, QUICKSORT_DATA_SIZE-1);

    clock_t t1, t2;

    t1 = clock();

    Parallel_For(pRange);

    t2 = clock();

    printf("Parallel_For QuickSort 1000000 datas, time = %ld\n", t2 - t1 );

    for (i = 0; i < QUICKSORT_DATA_SIZE-1; i++ )
    {
        assertTrue( pData[i] <= pData[i+1] );
        //printf("pData[%d] = %ld\n", i, pData[i]);
    }
#if 1
    t1 = clock();
    QuickSort(pCopyData, 0, QUICKSORT_DATA_SIZE-1);
    t2 = clock();

    printf("QuickSort 1000000 datas, time = %ld\n", t2 - t1 );

    for (i = 0; i < QUICKSORT_DATA_SIZE-1; i++ )
    {
        assertTrue( pCopyData[i] <= pCopyData[i+1] );
        //printf("pData[%d] = %ld\n", i, pData[i]);
    }

#endif
    delete [] pData;
    delete [] pCopyData;
}

#define MERGE_SIZE  8000000

int g_nData[16] = {3938, 21906, 22027, 14902, 220, 10149, 1677, 26049,
13139, 11325, 29447, 18116, 23855, 1848, 6837, 24354};

int g_nData2[16] = {220, 1677, 1848, 3938, 6837, 10149, 11325, 13139, 14902, 
        18116, 21906, 22027, 23855, 24354, 26049, 29447};

void TestCase_Parallel_For_Merge(void)
{
    int * pData = new int[MERGE_SIZE];
    int * pCopyData = new int[MERGE_SIZE];
    UINT uRand = (UINT)time(NULL);

    srand(uRand);

    int i;
#if 0
    FILE *fp = fopen("D:\\MergeData.Dat", "rb");

    if ( fp != NULL )
    {
        fread(pData, sizeof(int), MERGE_SIZE, fp);
        fclose(fp);
    }
#endif
    for (i = 0; i < MERGE_SIZE; i++ )
    {
        pData[i] = rand();
        pCopyData[i] = pData[i];
//        printf("Data[%d] = %ld\n", i, pData[i]);
    }



    QuickSort(pData, 0, MERGE_SIZE/2);

    QuickSort(pData, MERGE_SIZE/2+1, MERGE_SIZE-1);

#if 0
    for (i = 0; i < MERGE_SIZE; i++ )
    {
 //       printf("pData[%d] = %ld;\n", i, pData[i]);
        if ( pData[i] == 23284 )
        {
            printf( "pData[%d] = %ld, pData[%d] = %ld, pData[%d] = %ld\n",
                i-1, pData[i-1], i, pData[i], i+1, pData[i+1]);
        }
    }
#endif

    int *pOutData = new int[MERGE_SIZE];

    clock_t t1, t2;

    t1 = clock();
    SequentialMerge(pData, 0, MERGE_SIZE /2, 
        MERGE_SIZE/2 + 1, MERGE_SIZE - 1, pOutData);
    t2 = clock();

    printf("SequentialMerge %ld time = %ld\n", MERGE_SIZE, t2-t1);

    for (i = 0; i < MERGE_SIZE/2; i++ )
    {
        assertTrue( pOutData[i] <= pOutData[i+1] );
//        assertTrue( pOutData[i] == g_nData2[i] );
    }
    int *pOutData2 = new int[MERGE_SIZE];
    CMergeRange<int> *pRange = new CMergeRange<int>(pData, 0, MERGE_SIZE /2, 
        MERGE_SIZE/2+1, MERGE_SIZE-1, pOutData2);

    pRange->SetGrainSize(1000000);

    t1 = clock();
    Parallel_For(pRange);
    t2 = clock();
    printf("ParallelMerge %ld time = %ld\n", MERGE_SIZE, t2-t1);

    QuickSort(pCopyData, 0, MERGE_SIZE -1);


//    QuickSort((void **)g_ppDRange, 0, g_nDRangeIndex-1, DRangeCompare);

    for (i = 0; i < MERGE_SIZE - 1; i++ )
    {
//        printf("pOutData2[%d] = %ld\n", i, pOutData2[i]);

        assertTrue( pOutData2[i] <= pOutData2[i+1] );
#if 0
        if ( pOutData2[i] > pOutData2[i+1] )
        {
            printf("pOutData2[%d] = %ld, pOutData2[%d] = %ld\n", i-2, pOutData2[i-2], i-1, pOutData2[i-1]);
            printf("pOutData2[%d] = %ld, pOutData2[%d] = %ld\n", i, pOutData2[i], i+1, pOutData2[i+1]);
            printf("pOutData2[%d] = %ld, pOutData2[%d] = %ld\n", i+2, pOutData2[i+2], i+3, pOutData2[i+3]);

            DRange  dr;
            dr.addr =(UINT) &(pOutData2[i+1]);

            int nPos = BFind((void **)g_ppDRange, g_nDRangeIndex, &dr, DRangeCompare);

            DRange *p = g_ppDRange[nPos];

            printf("Addr of pOutData2 = 0x%lx\n", pOutData2);
            printf("Addr of pOutData2[%d] = 0x%lx\n", i+1, &(pOutData2[i+1]));

            printf( "DRange: %ld-%ld, %ld-%ld, 0x%lx\n", p->Pos[0], p->Pos[1], p->Pos[2], p->Pos[3], p->addr);
            int k;
            for ( k = p->Pos[0]; k <= p->Pos[1]; k++)
            {
                printf("pData[%d] = %ld\n", k, pData[k]);
            }
            for ( k = p->Pos[2]; k <= p->Pos[3]; k++)
            {
                printf("pData[%d] = %ld\n", k, pData[k]);
            }

            nPos += 1;
            p = g_ppDRange[nPos];
            printf( "DRange: %ld-%ld, %ld-%ld, 0x%lx\n", p->Pos[0], p->Pos[1], p->Pos[2], p->Pos[3], p->addr);
            for ( k = p->Pos[0]; k <= p->Pos[1]; k++)
            {
                printf("pData[%d] = %ld\n", k, pData[k]);
            }
            for ( k = p->Pos[2]; k <= p->Pos[3]; k++)
            {
                printf("pData[%d] = %ld\n", k, pData[k]);
            }

        }
#endif
    }
//    printf("pOutData2[%d] = %ld\n", i, pOutData2[i]);
    for (i = 0; i < MERGE_SIZE; i++ )
    {
        assertTrue( pCopyData[i] == pOutData2[i] );
        if ( pCopyData[i] != pOutData2[i] )
        {
            printf("pCopyData[%d] = %ld, pOutData2[%d] = %ld\n", i-1, pCopyData[i-1],
                i-1, pOutData2[i-1]);
            printf("pCopyData[%d] = %ld, pOutData2[%d] = %ld\n", i, pCopyData[i],
                i, pOutData2[i]);
        }
    }
    delete [] pData;
    delete [] pCopyData;
    delete [] pOutData;
    delete [] pOutData2;

}



void TestCase_LowerBound(void)
{
    int a[8] = {1, 3, 3, 4, 5, 8, 8, 10};


    int nRet = LowerBound(a, 0, 0, 7);
    assertTrue(nRet == 0);

    nRet = LowerBound(a, 2, 0, 7);
    assertTrue(nRet == 0);
    nRet = LowerBound(a, 3, 0, 7);
    assertTrue(nRet == 1);


    nRet = LowerBound(a, 5, 0, 7);
    assertTrue(nRet == 4);

    nRet = LowerBound(a, 7, 0, 7);
    assertTrue(nRet == 4);

    nRet = LowerBound(a, 8, 0, 7);
    assertTrue(nRet == 5);

    nRet = LowerBound(a, 9, 0, 7);
    assertTrue(nRet == 6);

    nRet = LowerBound(a, 10, 0, 7);
    assertTrue(nRet == 7);

    nRet = LowerBound(a, 12, 0, 7);
    assertTrue(nRet == 7);

    int b[1] = {5};

    nRet = LowerBound(b, 12, 0, 0);
    assertTrue(nRet == 0);
    nRet = LowerBound(b, 5, 0, 0);
    assertTrue(nRet == 0);

    nRet = LowerBound(b, 3, 0, 0);
    assertTrue(nRet == 0);

}


void TestCase_Inter_Split(void)
{
    int a[8] = {2, 3, 4, 5, 6, 7, 8, 9};
    int b[8];

    CMergeRange<int>  *pR1 = new CMergeRange<int>(a, 0, 1, 2, 2, b);

    CMergeRange<int> *pR = pR1->Inter_Split();
    
    assertTrue(pR->m_nBegin == 1 && pR->m_nEnd == 1 
        && pR->m_nBegin2 == 2 && pR->m_nEnd2 == 2 );

    assertTrue(pR1->m_nBegin == 0 && pR1->m_nEnd == 0 
        && pR1->m_nBegin2 == -1 && pR1->m_nEnd2 == -2 );

    pR->Merge();
    pR1->Merge();

    assertTrue(b[0] == a[0] && b[1] == a[1] && b[2] == a[2] );

    delete pR1;
    delete pR;

    a[0] = 8;
    a[1] = 12;
    a[2] = 6;

    pR1 = new CMergeRange<int>(a, 0, 1, 2, 2, b);
    pR = pR1->Inter_Split();
    assertTrue(pR->m_nBegin == 1 && pR->m_nEnd == 1 
        && pR->m_nBegin2 == -1 && pR->m_nEnd2 == -2 );
    assertTrue(pR1->m_nBegin == 0 && pR1->m_nEnd == 0 
        && pR1->m_nBegin2 == 2 && pR1->m_nEnd2 == 2 );
    pR->Merge();
    pR1->Merge();
    assertTrue(b[0] == a[2] && b[1] == a[0] && b[2] == a[1] );
    delete pR;
    delete pR1;


    a[0] = 8;
    a[1] = 12;
    a[2] = 15;

    pR1 = new CMergeRange<int>(a, 0, 2, -1, -2, b);
    pR = pR1->Inter_Split();
    assertTrue(pR->m_nBegin == 2 && pR->m_nEnd == 2 
        && pR->m_nBegin2 == -1 && pR->m_nEnd2 == -2 );
    assertTrue(pR1->m_nBegin == 0 && pR1->m_nEnd == 1 
        && pR1->m_nBegin2 == -1 && pR1->m_nEnd2 == -2 );
    pR->Merge();
    pR1->Merge();
    assertTrue(b[0] == a[0] && b[1] == a[1] && b[2] == a[2] );
    delete pR;
    delete pR1;


    a[0] = 9;
    a[1] = 13;
    a[2] = 10;

    pR1 = new CMergeRange<int>(a, 0, 1, 2, 2, b);
    pR = pR1->Inter_Split();
    assertTrue(pR->m_nBegin == 1 && pR->m_nEnd == 1 
        && pR->m_nBegin2 == 2 && pR->m_nEnd2 == 2 );
    assertTrue(pR1->m_nBegin == 0 && pR1->m_nEnd == 0 
        && pR1->m_nBegin2 == -1 && pR1->m_nEnd2 == -2 );
    pR->Merge();
    pR1->Merge();
    assertTrue(b[0] == a[0] && b[1] == a[2] && b[2] == a[1] );
    delete pR;
    delete pR1;


    a[0] = 16;
    a[1] = 20;
    a[2] = 16;

    pR1 = new CMergeRange<int>(a, 0, 1, 2, 2, b);
    pR = pR1->Inter_Split();
    assertTrue(pR->m_nBegin == 1 && pR->m_nEnd == 1 
        && pR->m_nBegin2 == -1 && pR->m_nEnd2 == -2 );
    assertTrue(pR1->m_nBegin == 0 && pR1->m_nEnd == 0 
        && pR1->m_nBegin2 == 2 && pR1->m_nEnd2 == 2 );
    pR->Merge();
    pR1->Merge();
    assertTrue(b[0] == a[0] && b[1] == a[2] && b[2] == a[1] );
    delete pR;
    delete pR1;


    a[0] = 17;
    a[1] = 21;
    a[2] = 21;

    pR1 = new CMergeRange<int>(a, 0, 1, 2, 2, b);
    pR = pR1->Inter_Split();
    assertTrue(pR->m_nBegin == 1 && pR->m_nEnd == 1 
        && pR->m_nBegin2 == 2 && pR->m_nEnd2 == 2 );
    assertTrue(pR1->m_nBegin == 0 && pR1->m_nEnd == 0 
        && pR1->m_nBegin2 == -1 && pR1->m_nEnd2 == -2 );
    pR->Merge();
    pR1->Merge();
    assertTrue(b[0] == a[0] && b[1] == a[1] && b[2] == a[2] );
    delete pR;
    delete pR1;


}




