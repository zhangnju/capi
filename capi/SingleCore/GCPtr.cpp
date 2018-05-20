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
HASHTABLE   *g_pTable; //��ϣ��ָ��
HASHTABLE   *g_pMTable; //�������ϣ��ָ��
CFastLock   g_lock; ////�����������ü�����д����ϣ���������
MTASK       *g_pMTask;


/**	�����ڴ��ռ��㷨�ĳ�ʼ������

	@param	INT nBucketCount - ��ϣ���Ͱ������	
	@return	INT - CAPI_SUCCESS��ʾ�ɹ���CAPI_FAILED��ʾʧ��	
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

/**	�����ڴ��ռ��㷨���ڴ���亯��

	@param	size - Ҫ���ɵ��ڴ��С�����ֽ�Ϊ��λ	
	@return	�ɹ����ط��䵽���ڴ��ַ��ʧ�ܷ���NULL	
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

/**	�����ڴ��ռ��㷨���ڴ��ͷź���

	@param	void *p - Ҫ�ͷŵ��ڴ��ַ	
	@return	void - ��	
*/
void GC_Free(void *p)
{
    void *pFree = (void *)((char *)p - DOUBLE_INT_LEN);
    free(pFree);
}

/**	�����ڴ��ռ��㷨���ڴ�й©��麯��

	@return	void -��	
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
        if ( *pRef  != 0 ) //�ж����ü����Ƿ�Ϊ0
        {
            pLine = (INT *)((char *)p + *pSize + DOUBLE_INT_LEN);
            pszFile = (char *)pLine + INT_LEN;
            printf("File: %s, Line: %d have memory leak.\n", pszFile, *pLine);
        }
    }
}

#else

/**	�����ڴ��ռ��㷨���ڴ���亯��

	@param	size - Ҫ���ɵ��ڴ��С�����ֽ�Ϊ��λ	
	@return	�ɹ����ط��䵽���ڴ��ַ��ʧ�ܷ���NULL	
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

/**	�����ڴ��ռ��㷨���ڴ��ͷź���

	@param	void *p - Ҫ�ͷŵ��ڴ��ַ	
	@return	void - ��	
*/
void GC_Free(void *p)
{
    void *pFree = (void *)((char *)p - INT_LEN);
    free(pFree);
}
#endif 

/**	�����ڴ��ռ��㷨���ֹ��ͷ��ڴ溯��

	@param	void *p - Ҫ�ͷŵ��ڴ��ַ	
	@return	void - ��	
*/
void GC_ManualFree(void *p)
{
    void *pFree = (void *)((char *)p - INT_LEN);
    
    HashTable_Delete(g_pTable, pFree, HashInt, IntCompare, NULL);

    free(pFree);
}

/**	�����ռ�����
        ������ϣ�����������ü���Ϊ0���ڴ��ͷŵ�

	@return	void -��	
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



/**	�������µ������ڴ��ռ��㷨�ĳ�ʼ������

	@param	INT nBucketCount - ��ϣ���Ͱ������	
	@return	INT - �ɹ�����CAPI_SUCCESS��ʧ�ܷ���CAPI_FAILED.	
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

/**	�������µ������ڴ��ռ��㷨���ڴ���亯��

	@param	size_t size - Ҫ������ڴ��С�����ֽ�Ϊ��λ	
	@return	void * - �ɹ����ط��䵽���ڴ��ַ��ʧ�ܷ���NULL	
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


/**	�������µ������ڴ��ռ��㷨���ڴ��ͷź���

	@param	void *p - Ҫ�ͷŵ��ڴ��ַ	
	@return	void - ��	
*/
void MGC_Free(void *p)
{
    void *pFree = (void *)((char *)p - INT_LEN);
    free(pFree);
}

/**	�������µ������ڴ��ռ��㷨���ֹ��ͷ��ڴ溯��

	@param	void *p - Ҫ�ͷŵ��ڴ��ַ	
	@return	void - ��	
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

/**	֧�ֶ�����������ռ�����
        ������ϣ�����������ü���Ϊ0���ڴ��ͷŵ�

	@return	void -��	
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

/**	֧�ֶ�����������ռ�����
        ������ϣ�����������ü���Ϊ0���ڴ��ͷŵ�

	@return	void -��	
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
        
            /* Ϊ���Ч�ʼ��ٽ���������ÿ��������ѭ��һ������ */
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

/**	�����ռ��Ĺرպ����������ڳ������е������ִ�У��������atexit()
        ������ִ���������

	@return	void -��	
*/
void MGC_Shutdown()
{

    MTask_Destroy(g_pMTask);

    CScopedLock<CFastLock> slock(g_lock);

    HashTable_Destroy(g_pTable, free);
    HashTable_Destroy(g_pMTable, free);

}

