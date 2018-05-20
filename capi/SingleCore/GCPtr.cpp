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
#include <stdio.h>
#include <stdlib.h>
#include "CapiGlobal.h"
#include "HashTable.h"
#include "MTask.h"
#include "CScopedLock.h"
#include "GCPtr.h"

#define     GC_COLLECT_ENUM_COUNT  1024
HASHTABLE   *g_pTable; //哈希表指针
HASHTABLE   *g_pMTable; //多任务哈希表指针
CFastLock   g_lock; ////用来保护引用计数读写及哈希表操作的锁
MTASK       *g_pMTask;


/**	垃圾内存收集算法的初始化函数

	@param	INT nBucketCount - 哈希表的桶的数量	
	@return	INT - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败	
*/
INT GC_Init(INT nBucketCount)
{
    g_pTable = HashTable_Create(nBucketCount);
    if ( g_pTable != NULL )
    {
        return CAPI_SUCCESS;
    }
    return CAPI_FAILED;
}

#ifdef _DEBUG

/**	垃圾内存收集算法的内存分配函数

	@param	size - 要分派的内存大小，以字节为单位	
	@return	成功返回分配到的内存地址，失败返回NULL	
*/
#define GC_Malloc(size)\
{\
    void *p;\
    INT  *q;\
    char *psz;\
    \
    p = malloc( size + DOUBLE_INT_LEN + INT_LEN + strlen(__FILE__) + 1);\
    if ( p == NULL )\
{\
    GC_Collect();\
    p = malloc( size + DOUBLE_INT_LEN + INT_LEN + strlen(__FILE__) + 1);\
    if ( p == NULL )\
{\
    return NULL;\
}\
}\
    \
    HashTable_Insert( g_pTable, p, HashInt);\
    \
    *((INT *)p) = 0;\
    *((INT *)p + 1) = size;\
    \
    q = (INT *)((char *p) + size + DOUBLE_INT_LEN);\
    *q = __LINE__;\
    psz = (char *)p + size + DOUBLE_INT_LEN + INT_LEN;\
    strcpy(psz, __FILE__);\
    \
    return (void *)((char *)p + DOUBLE_INT_LEN);\
}

/**	垃圾内存收集算法的内存释放函数

	@param	void *p - 要释放的内存地址	
	@return	void - 无	
*/
void GC_Free(void *p)
{
    void *pFree = (void *)((char *)p - DOUBLE_INT_LEN);
    free(pFree);
}

/**	垃圾内存收集算法的内存泄漏检查函数

	@return	void -无	
*/
void GC_CheckMemoryLeak()
{
    void *p;
    
    HashTable_EnumBegin(g_pTable);
    
    while ( (p = HashTable_EnumNext(g_pTable)) != NULL )
    {
        INT *pRef;  
        INT *pSize;
        INT *pLine;
        char *pszFile;
        
        pRef = (INT *)p;
        pSize = pRef + 1;
        if ( *pRef  != 0 ) //判断引用计数是否为0
        {
            pLine = (INT *)((char *)p + *pSize + DOUBLE_INT_LEN);
            pszFile = (char *)pLine + INT_LEN;
            printf("File: %s, Line: %d have memory leak.\n", pszFile, *pLine);
        }
    }
}

#else

/**	垃圾内存收集算法的内存分配函数

	@param	size - 要分派的内存大小，以字节为单位	
	@return	成功返回分配到的内存地址，失败返回NULL	
*/
void *GC_Malloc(size_t size)
{
    void *p = malloc( size + INT_LEN );
    if ( p == NULL )
    {
        GC_Collect();
        p = malloc( size + INT_LEN );
        if ( p == NULL )
        {
            return NULL;
        }
    }

    HashTable_Insert( g_pTable, p, HashInt);

    *((INT *)p) = 0;

    return (void *)((char *)p+INT_LEN);
}

/**	垃圾内存收集算法的内存释放函数

	@param	void *p - 要释放的内存地址	
	@return	void - 无	
*/
void GC_Free(void *p)
{
    void *pFree = (void *)((char *)p - INT_LEN);
    free(pFree);
}
#endif 

/**	垃圾内存收集算法的手工释放内存函数

	@param	void *p - 要释放的内存地址	
	@return	void - 无	
*/
void GC_ManualFree(void *p)
{
    void *pFree = (void *)((char *)p - INT_LEN);
    
    HashTable_Delete(g_pTable, pFree, HashInt, IntCompare, NULL);

    free(pFree);
}

/**	垃圾收集函数
        遍历哈希表，将所有引用计数为0的内存释放掉

	@return	void -无	
*/
void GC_Collect()
{
    void *p;
    HashTable_EnumBegin(g_pTable);
    while ( (p = HashTable_EnumNext(g_pTable)) != NULL )
    {
        INT *pRef = (INT *)p - 1;
        if ( *pRef == 0 )
        {
            HashTable_Delete(g_pTable, p, HashInt, IntCompare, NULL);
            GC_Free(p);
        }
    }
}



/**	多任务下的垃圾内存收集算法的初始化函数

	@param	INT nBucketCount - 哈希表的桶的数量	
	@return	INT - 成功返回CAPI_SUCCESS，失败返回CAPI_FAILED.	
*/
INT MGC_Init(INT nBucketCount)
{
    g_pMTable = HashTable_Create(nBucketCount);
    if ( g_pMTable != NULL )
    {
        g_pMTask = MTask_Create();
        if ( g_pMTask != NULL )
        {
            return CAPI_SUCCESS;
        }
        else
        {
            HashTable_Destroy(g_pMTable, NULL);
        }
    }

    return CAPI_FAILED;
}

/**	多任务下的垃圾内存收集算法的内存分配函数

	@param	size_t size - 要分配的内存大小，以字节为单位	
	@return	void * - 成功返回分配到的内存地址，失败返回NULL	
*/
void *MGC_Malloc(size_t size)
{
    void *p = malloc( size + INT_LEN );
    if ( p == NULL )
    {
        MGC_Collect();
        p = malloc( size + INT_LEN );
        if ( p == NULL )
        {
            return NULL;
        }
    }
    
    {
        CScopedLock<CFastLock> slock(g_lock);

        HashTable_Insert( g_pMTable, p, HashInt);
        
        *((INT *)p) = 0;
    }
   
    return (void *)((char *)p + INT_LEN );
}


/**	多任务下的垃圾内存收集算法的内存释放函数

	@param	void *p - 要释放的内存地址	
	@return	void - 无	
*/
void MGC_Free(void *p)
{
    void *pFree = (void *)((char *)p - INT_LEN);
    free(pFree);
}

/**	多任务下的垃圾内存收集算法的手工释放内存函数

	@param	void *p - 要释放的内存地址	
	@return	void - 无	
*/
void MGC_ManualFree(void *p)
{
    void *pFree = (void *)((char *)p - INT_LEN);

    {
        CScopedLock<CFastLock> slock(g_lock);
        HashTable_Delete(g_pMTable, pFree, HashInt, IntCompare, NULL);
    }    

    free(pFree);
}

/**	支持多任务的垃圾收集函数
        遍历哈希表，将所有引用计数为0的内存释放掉

	@return	void -无	
*/
void MGC_Collect()
{
    void *p;

    CScopedLock<CFastLock> slock(g_lock);
    HashTable_EnumBegin(g_pMTable);
    while ( (p = HashTable_EnumNext(g_pMTable)) != NULL )
    {
        INT *pRef = (INT *)((char *)p - INT_LEN);
        if ( *pRef == 0 )
        {
            HashTable_Delete(g_pMTable, p, HashInt, IntCompare, NULL);
            MGC_Free(p);
        }
    }
}

/**	支持多任务的垃圾收集函数
        遍历哈希表，将所有引用计数为0的内存释放掉

	@return	void -无	
*/
void MGC_CollectTask()
{
    void *p;
    UINT i;

    MTask_EnterTask(g_pMTask);

    {
        CScopedLock<CFastLock> slock(g_lock);
        HashTable_EnumBegin(g_pMTable);
    }
    while (MTask_GetExitFlag(g_pMTask) == MTASK_NO_EXIT) 
    {
        {
            CScopedLock<CFastLock> slock(g_lock);
        
            /* 为提高效率减少解锁次数，每次上锁后循环一定次数 */
            for ( i = 0; i < GC_COLLECT_ENUM_COUNT; i++ )
            {
                p = HashTable_EnumNext(g_pMTable);
                if ( p != NULL )
                {
                    INT *pRef = (INT *)((char *)p - INT_LEN);
                    if ( *pRef == 0 )
                    {
                        HashTable_Delete(g_pMTable, p, HashInt, IntCompare, NULL);
                        MGC_Free(p);
                    }
                }
                else
                {
                    break;
                }
            }
        }
        Sleep(100);
    }

    MTask_LeaveTask(g_pMTask);
    
    return;
}

/**	垃圾收集的关闭函数，必须在程序运行的最后面执行，建议调用atexit()
        函数来执行这个函数

	@return	void -无	
*/
void MGC_Shutdown()
{

    MTask_Destroy(g_pMTask);

    CScopedLock<CFastLock> slock(g_lock);

    HashTable_Destroy(g_pTable, free);
    HashTable_Destroy(g_pMTable, free);

}

