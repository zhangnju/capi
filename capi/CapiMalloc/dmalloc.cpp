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
#ifdef _WIN32
#define _WIN32_WINNT 0x0403 
#include <windows.h>
#endif
#include <omp.h>
#include "CapiGlobal.h"
#include "CScopedLock.h"
#include "BigMemory.h"
#include "FreeList.h"
#include "MemoryPool.h"
#include "CLocalQueue.h"
#include "CSharedQueue.h"

#ifdef _WIN32
#define		DLLAPI		__declspec(dllexport)
#endif

#define USE_WINTHREAD

#ifdef USE_WINTHREAD
#define    TlsGetValueFunc   TlsGetValue
#define    TlsSetValueFunc   TlsSetValue
#else
#define    TlsGetValueFunc   pthread_getspecific
#define    TlsSetValueFunc   pthread_setspecific
#endif

// ���ӽṹ�嶨��
// һ�������а�������ͬ�ߴ��ڴ�Ķ��FreeLists
typedef struct BIN_st  {
    FREELISTS       aFreeLists[MAX_MEMORY_CLASS];
} BIN;

extern "C" void ThreadExit_Notification(void* arg);


#ifdef USE_WINTHREAD
static DWORD	            g_TlsPointer;  // �̱߳��ش洢����ָ��
#else
pthread_key_t               g_TlsPointer;
#endif

// ȫ�ֵ����Ӷ��У���ҪΪ�����ظ����ö����
CSharedQueue<BIN *, CLocalQueue<BIN *>>  g_BinQueue(8); 

LONG volatile       g_InitFlag = 0;
LONG volatile       g_dmalloc_init_finished_flag = 0;

CFastLock		    g_InitdmallocFastLock;

/**	��ʼ���̱߳��ش洢

	@return	 void - ��	
*/
static void ThreadTls_Init(void)
{
#ifdef USE_WINTHREAD
	g_TlsPointer = TlsAlloc();
#else
    int status = pthread_key_create( &g_TlsPointer, ThreadExit_Notification );
#endif
}

/**	����һ������

	@return	BIN * - ���ش���������ָ��	
*/
static BIN *CreateBin()
{
    BIN  *pBin = new BIN;

    UINT i;

    for ( i = 0; i < MAX_MEMORY_CLASS; i++)
    {
        FreeLists_Init(&(pBin->aFreeLists[i]), g_auMemSize[i] );
    }
    return pBin;
}


/**	��ȡ��ǰ�̶߳�Ӧ������ָ�뺯��
	ͨ�������̱߳��ش洢��ȡ������ָ��

	@return	 BIN * - ��ȡ��ǰ�̶߳�Ӧ������ָ��	
*/
static BIN * GetBinOfCurThread(void)
{
	BIN 	*pBin;
	pBin = (BIN *)TlsGetValueFunc(g_TlsPointer);
	if ( pBin == (BIN *)0 )
	{
        if ( g_BinQueue.DeQueue(pBin) == CAPI_FAILED )
        {
            pBin = CreateBin();
        }
		TlsSetValueFunc(g_TlsPointer, (void *)pBin);
	}
	return pBin;
}



/**	��ʼ���ֲ�ʽ�ڴ������
	��Ҫ�ǳ�ʼ���̱߳��ش洢����ʼ������ڴ��������
	����һ�����������Ӳ����뵽���Ӷ�����

	@param	int nMaxThreads - ��ʼ�߳���������ʼ�߳����������ų�ʼԤ�����õ�����������
					��ʼ�����������߳�������ʱ���̳߳�ʼ��ʱ�ᴴ���µ����ӣ�
					��˳�ʼ�߳��������ܳ�������߳��������������������������˷ѡ�
	@return	void - ��	
*/
static void Init_dmalloc(int nMaxThreads)
{
    BIN     *pBin;
    
	// ��ʼ���̱߳��ش洢
	ThreadTls_Init();

    //��ȡϵͳ�ڴ�Ĵ�С
    MEMORYSTATUS stat;
    GlobalMemoryStatus (&stat);

    // ��ʼ������ڴ������,ʹ�ÿ��������ڴ�
    // �����Ҫ�����Ըĳ�ʹ�ÿ��������ڴ�
    BigMemory_Init(stat.dwAvailPhys/BIG_MEMORY_SIZE);

    // ��ʼ���ڴ��
    MemoryPool_Init();

    int i;
	
	// ���Կ��ǽ�����ѭ�����л�ִ��
    for ( i = 0; i < nMaxThreads; i++ )
    {
		// �������ӣ����������Ӷ�����
        pBin = CreateBin();
        g_BinQueue.EnQueue(pBin);
    }
}


static void CheckInit_dmalloc()
{
    CScopedLock<CFastLock>    slock(g_InitdmallocFastLock);

    if ( 0 == g_InitFlag )
    {
        // flag ����1�����ǵ�1����g_InitFlag��������ԭ�Ӽ�1���߳�
        // ����ȷ����Init_dmalloc()ֻ��ִ��һ�Ρ�
        Init_dmalloc(omp_get_num_procs());
        g_InitFlag = 1;
        AtomicIncrement(&g_dmalloc_init_finished_flag);
    }
}


/**	�ֲ�ʽ�ڴ������ڴ���亯��

	@param	UINT uSize - Ҫ������ڴ��С	
	@return	void * - ���ط��䵽���ڴ�ָ�룬���ϵͳ�ڴ治����ʱ�᷵��NULL	
*/
extern "C" DLLAPI void * dmalloc(UINT uSize)
{
    BIN     *pBin;

    if ( 0 == g_dmalloc_init_finished_flag )
    {
        CheckInit_dmalloc();
    }

	// ��ͨ���̱߳��ش洢�ҵ�����ָ��
    pBin = GetBinOfCurThread();

	// ͨ���ڴ��С��ȡ�����е�FreeLists����
    int index = MemoryPool_GetMemSizeIndex(uSize);

	// ��FreeLists�з����ڴ�
    void * p = FreeLists_Alloc( &(pBin->aFreeLists[index]));
    
    return p;
}

/**	�ֲ�ʽ�ڴ������ڴ��ͷź���

	@param	void *p - Ҫ�ͷŵ��ڴ�	
	@return	void - ��	
*/
extern "C"  DLLAPI void dfree(void *p)
{
    BIN   * pBin;
    BLOCK * pBlock = BigMemory_GetBlockPtr(p);

    pBin = GetBinOfCurThread();

    int index = MemoryPool_GetMemSizeIndex(pBlock->uMemSize);

    FreeLists_Free(&(pBin->aFreeLists[index]), p);
    return;
}


/**	�رշֲ�ʽ�ڴ����
	���������ò�Ҫ����ʹ��
	�����ܹ�ȷ�����з�����ڴ涼�ڵ����������ǰ���ͷ���
	����ȫ�ֵ�������������в���dmalloc�������ڴ�ʱ��
	��ô�����˳�ʱ�����ڴ���ʹ�ã��Ͳ��ܵ�����������ˣ�
	��Ҫ�ý����Զ�ȥ�ͷ��ڴ档

	@return	void - ��	
*/
extern "C" DLLAPI void Close_dmalloc()
{
    BIN *pBin;

    while ( g_BinQueue.DeQueue(pBin) != CAPI_FAILED )
    {
        delete pBin;
    }

    BigMemory_Close();

#ifdef USE_WINTHREAD
    TlsFree(g_TlsPointer);
#endif

    return;
}


/**	�߳��˳�ʱ�Ĵ�����
	��ҪΪ�߳��˳�ʱ�����̱߳��ش洢�����
	������pthread_key_create�Ļص�������
	��Windows��������DLL��ں����е�DLL_THREAD_DETACH��Ϣ��ʹ��

	@param	void* arg - ��������ʹ��	
	@return	void - ��	
*/
extern "C" void ThreadExit_Notification(void* arg)
{
    if ( g_dmalloc_init_finished_flag == 0 )
    {
        return;
    }

    BIN * pBin = GetBinOfCurThread();

    if ( pBin )
    {
		// ���˳��̵߳����ӷ�������б��������߳��ظ�ʹ��
        g_BinQueue.EnQueue(pBin);
    }
    return;
}

