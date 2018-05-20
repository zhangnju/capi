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
#include <stdio.h>
#include <omp.h>
#include "CapiGlobal.h"
#include "QuickSort.h"
#include "Parallel_MergeSort.h"
#include "CTaskTeam.h"


void Task_Destroy(void *pArg)
{
    TASK *pTask = (TASK *)pArg;

    if ( pTask != NULL )
    {
        delete pTask;
    }
}


/**	任务比较函数，比较两个任务耗时的大小
    用于排序时，将会将任务按时间大小从大到小依次排好

    @param	void *p1 - 第1个任务的指针	
    @param	void *p2 - 第2个任务的指针	
    @return	static int - -1表示第1个大于第2个，1表示第1个小于第2个， 0表示相等	
*/
static int Task_Compare(void *p1, void *p2)
{
    TASK *pTask1 = (TASK *)p1;
    TASK *pTask2 = (TASK *)p2;

    if ( pTask1->time_consume > pTask2->time_consume )
    {
        return -1;
    }
    else if ( pTask1->time_consume < pTask2->time_consume )
    {
        return 1;
    }
    else
    {
        return 0;
    }   
}


/**	任务图的边释放函数

    @param	void *p - 边EDGE指针	
    @return	static void - 无	
*/
void Edge_Destroy(void *p)
{
    EDGE    *pEdge = (EDGE *)p;
    delete p;
}



CTaskTeam::CTaskTeam()
{
    m_nMaxCount = DEFAULT_TASK_COUNT_IN_TASKTEAM;
    m_nTaskCount = 0;
    m_ppTaskArray = NULL;
    m_nThreadCount = 0;
    m_ppSingleList = NULL;
    m_pnDeltaArray = NULL;
}


CTaskTeam::CTaskTeam(int nInitTaskCount)
{
#ifdef _DEBUG
    printf("CTaskTeam::CTaskTeam() called.\n");
#endif
    m_nMaxCount = nInitTaskCount;
    m_nTaskCount = 0;
    m_ppTaskArray = NULL;
    m_nThreadCount = 0;
    m_ppSingleList = NULL;
    m_pnDeltaArray = NULL;
}


CTaskTeam::~CTaskTeam()
{
#ifdef _DEBUG
    printf("CTaskTeam::~CTaskTeam() called.\n");
#endif
    if ( m_ppTaskArray != NULL )
    {
        int i;
        for ( i = 0; i < m_nTaskCount; i++ )
        {
            TASK *pTask = m_ppTaskArray[i];
            if ( pTask != NULL )
            {
                delete pTask;
            }
        }
        delete [] m_ppTaskArray;
    }

    if ( m_ppSingleList != NULL )
    {
        int i;
        for ( i = 0; i < m_nThreadCount; i++ )
        {
            SingleList_Destroy(m_ppSingleList[i], Task_Destroy);
        }
        delete [] m_ppSingleList;
    }
     
    if ( m_pnDeltaArray != NULL )
    {
        delete [] m_pnDeltaArray;
    }
}

/**	查找最大的负数误差值

    @param	int *a - 要查找的数组	
    @param	int count - 数组元素个数	
    @return	int - 返回最大负数误差所在数组的下标位置	
*/
int CTaskTeam::FindMaxDelta()
{
    int i;
    int nMaxValue = 0;
    int nPos;

    for ( i = 0; i < m_nThreadCount; i++ )
    {
        if ( m_pnDeltaArray[i] < 0 && m_pnDeltaArray[i] < nMaxValue )
        {
            nMaxValue = m_pnDeltaArray[i];
            nPos = i;
        }
    }
    return i;
}


/**	添加任务到任务组中

    @param	TASK *pTaskNode - 任务节点指针	
    @return	void - 无	
*/
void CTaskTeam::AddTask(TASK *pTaskNode)
{
    if ( m_ppTaskArray == NULL )
    {
        m_ppTaskArray = new TASK *[m_nMaxCount];
    }
    if ( m_nTaskCount >= m_nMaxCount )
    {
        //将数组大小扩大一倍
        m_nMaxCount *= 2;
        TASK **pp = new TASK *[m_nMaxCount];
        int i;
        for ( i = 0; i < m_nTaskCount; i++ )
        {
            pp[i] = m_ppTaskArray[i];
        }

        TASK **ppTemp = m_ppTaskArray;
        m_ppTaskArray = pp;
        delete [] ppTemp;
    }
    m_ppTaskArray[m_nTaskCount] = pTaskNode;
    m_nTaskCount += 1;
}


void CTaskTeam::AddTask(THREADFUNC TaskFunc, void *pPara, DWORD dwTaskTime)
{
    TASK    *pTask = new TASK;
    pTask->func = TaskFunc;
    pTask->pArg = pPara;
    pTask->time_consume = dwTaskTime;

    AddTask(pTask);
}


TASK *CTaskTeam::GetTask(int nPos)
{
    if ( nPos >= m_nTaskCount )
    {
        return NULL;
    }
    return m_ppTaskArray[nPos];
}

TASK **CTaskTeam::GetTaskArray()
{
    return m_ppTaskArray;
}


int CTaskTeam::GetTaskCount()
{
    return m_nTaskCount;
}


/**	任务组调度函数
    将一个任务组内的所有任务按任务执行时间平均分配到各个线程中
    分配的结果存放在m_ppSignleList成员中
    其中m_pnDeltaArray[i]保存了m_ppSingleList[i]中各任务耗时总和与平均值的误差

    @return	void - 无	
*/
void CTaskTeam::Schedule()
{
    int nThreads = omp_get_num_procs();
    int nTaskCount = m_nTaskCount;

    if ( nTaskCount < nThreads )
    {
        nThreads = nTaskCount;
    }

    m_nThreadCount = nThreads;

    QuickSort((void **)m_ppTaskArray, 0, nTaskCount-1, Task_Compare);

    int      i, k;
    DWORD    dwSum = 0;
    DWORD    dwAvgTime;       

    for ( i = 0; i < nTaskCount; i++ )
    {
        TASK *pTask = (TASK *)m_ppTaskArray[i];
        pTask->time_consume *= nThreads; //避免计算平均值时出现整数截断误差
        dwSum += pTask->time_consume;
    }

    dwAvgTime = dwSum / nThreads;


    DWORD dwCurPos = 0;   //记录m_ppTaskArray中的位置
    int   total_delta = 0;
    int  *delta_array = new int[nThreads];  //记录分配到各个线程中任务总时间和平均时间的误差的数组

    int   nTurn;
    DWORD dwTempTime1;
    DWORD dwTempTime2;

    SINGLELIST **ppList = new SINGLELIST *[nThreads];

    for ( nTurn = 0; nTurn < nThreads; nTurn++ )
    {
        DWORD dwTime = 0;
        int   delta1, delta2;  //误差变量

        ppList[nTurn] = SingleList_Create();

        for ( i = dwCurPos; i < nTaskCount; i++ )
        {
            if ( m_ppTaskArray[i] == NULL )
            {
                continue;
            }
            TASK *pTask = m_ppTaskArray[i];
            dwTime += pTask->time_consume;
            if ( dwTime > dwAvgTime )
            {
                dwTempTime1 = dwTime -(int)pTask->time_consume;
                dwTempTime2 = dwTempTime1;
                break;
            }
        }

        if ( i == dwCurPos) //边界条件，单个的任务耗时大于平均时间的情况
        {
            SingleList_InsertTail(ppList[nTurn], m_ppTaskArray[i]);
            m_ppTaskArray[i] = NULL;

            dwCurPos += 1;
            delta_array[nTurn] = 0-(int)dwTempTime1;
            total_delta -= dwTempTime1;
            continue;
        }
        else
        {
            for ( k = dwCurPos; k < i; k++ )
            {
                if ( m_ppTaskArray[k] == NULL )
                {
                    continue;
                }
                SingleList_InsertTail(ppList[nTurn], m_ppTaskArray[k]);
                m_ppTaskArray[k] = NULL;
            }
            dwCurPos = i;
            if ( i == nTaskCount )
            {
                break; //所有的任务都被分配完了
            }
        }

        int nMinPos = -1;

        for ( k = i+1; k < nTaskCount; k++ )
        {
            if ( m_ppTaskArray[k] == NULL )
            {
                continue;
            }
            TASK *pTask = m_ppTaskArray[k];
            dwTempTime2 = dwTempTime1 + pTask->time_consume;
            nMinPos = k;
            if ( dwTempTime2 <= dwAvgTime )
            {
                pTask = m_ppTaskArray[k-1];
                dwTempTime1 += pTask->time_consume;
                delta1 = dwTempTime1 - dwAvgTime; // delta1 > 0
                delta2 = dwTempTime2 - dwAvgTime; // delta2 < 0

                // 采用使总累积误差绝对值最小的方法来选择任务时间
                if ( total_delta + delta1 <= -total_delta -delta2 )
                {
                    //put k-1 to 
                    SingleList_InsertTail(ppList[nTurn], m_ppTaskArray[k-1]);
                    m_ppTaskArray[k-1] = NULL;
                    total_delta += delta1;
                    delta_array[nTurn] = delta1;
                }
                else
                {
                    //put k to 
                    SingleList_InsertTail(ppList[nTurn], m_ppTaskArray[k]);
                    m_ppTaskArray[k] = NULL;
                    delta_array[nTurn] = delta2;
                    total_delta += delta2;
                }

                nMinPos = -1;
                break;
            }
        }//for(k=i+1;

        //如果上面的整个循环都完成后，没有找到使dwTempTime2 < dwAvgTime的情况
        if ( nMinPos != -1 )
        {
            TASK *pTask = m_ppTaskArray[nMinPos];
            dwTempTime2 = dwTempTime1 + pTask->time_consume;
            delta1 = dwTempTime1 - dwAvgTime; // delta1 < 0
            delta2 = dwTempTime2 - dwAvgTime; // delta2 > 0

            // 采用使总累积误差绝对值最小的方法来选择任务时间
            if ( -total_delta - delta1 <= total_delta + delta2 )
            {
                //put nothing to 
                total_delta += delta1;
                delta_array[nTurn] = delta1;
            }
            else
            {
                //put nMinPos to 
                SingleList_InsertTail(ppList[nTurn], m_ppTaskArray[nMinPos]);
                m_ppTaskArray[nMinPos] = NULL;
                delta_array[nTurn] = delta2;
                total_delta += delta2;
            }
        }

        //当最后一个线程的任务被安排完后，还剩余有多余任务待安排的情况
        if ( nTurn == nThreads - 1 )
        {
            if ( k < nTaskCount )
            {
                int     nInsertPos;
                TASK *  pTask;

                for ( i = k; i < nTaskCount; i++ )
                {
                    if ( m_ppTaskArray[i] == NULL )
                    {
                        continue;
                    }
                    nInsertPos = FindMaxDelta();
                    pTask = m_ppTaskArray[i];
                    delta_array[nInsertPos] += pTask->time_consume;
                    SingleList_InsertTail(ppList[nTurn-1], m_ppTaskArray[i]);          
                }
            }
        } // if ( nTurn == nThreads - 1 )
    } // for(nTurn = 0; nTurn < m_nThreadCount; nTurn++)

    // 将分配好的任务时间复原
    for ( i = 0; i < nThreads; i++ )
    {
        SINGLENODE  *pNode;
        TASK        *pTask;

        pNode = ppList[i]->pHead;
        while ( pNode != NULL )
        {
            pTask = (TASK *)pNode->pData;
            pTask->time_consume /= nThreads; //将时间复原
            pNode = pNode->pNext;
        }
    }

    if ( m_ppSingleList != NULL )
    {
        int i;
        for ( i = 0; i < m_nThreadCount; i++ )
        {
            SingleList_Destroy(m_ppSingleList[i], Task_Destroy);
        }
        delete [] m_ppSingleList;
    }
    m_ppSingleList = ppList;

    if ( m_pnDeltaArray != NULL )
    {
        delete [] m_pnDeltaArray;
    }
    m_pnDeltaArray = delta_array;

    return;
}

/**	任务组执行函数
    将一个调度好的任务组并行执行，采用openmp的fork/join方式执行

    @return	void - 无	
*/
void CTaskTeam::Exec()
{
    int i;

#pragma omp parallel for num_threads(m_nThreadCount)
    for ( i = 0; i < m_nThreadCount; i++)
    {
        SINGLELIST  *pList = m_ppSingleList[i];
        if ( pList != NULL )
        {
            SINGLENODE  *pNode = pList->pHead;

            while ( pNode != NULL )
            {
                TASK *pTask = (TASK *)pNode->pData;

                THREADFUNC func = pTask->func;
                
                (*func)(pTask->pArg);
                
                pNode = pNode->pNext;
#ifdef _DEBUG 
                printf( "Thread No.%ld, Time = %ld\n", omp_get_thread_num(), pTask->time_consume);
#endif            
            }//while
        }// if
    }// for
}

