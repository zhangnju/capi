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

// 箱子结构体定义
// 一个箱子中包含管理不同尺寸内存的多个FreeLists
typedef struct BIN_st  {
    FREELISTS       aFreeLists[MAX_MEMORY_CLASS];
} BIN;

extern "C" void ThreadExit_Notification(void* arg);


#ifdef USE_WINTHREAD
static DWORD	            g_TlsPointer;  // 线程本地存储索引指针
#else
pthread_key_t               g_TlsPointer;
#endif

// 全局的箱子队列，主要为箱子重复利用而设计
CSharedQueue<BIN *, CLocalQueue<BIN *>>  g_BinQueue(8); 

LONG volatile       g_InitFlag = 0;
LONG volatile       g_dmalloc_init_finished_flag = 0;

CFastLock		    g_InitdmallocFastLock;

/**	初始化线程本地存储

	@return	 void - 无	
*/
static void ThreadTls_Init(void)
{
#ifdef USE_WINTHREAD
	g_TlsPointer = TlsAlloc();
#else
    int status = pthread_key_create( &g_TlsPointer, ThreadExit_Notification );
#endif
}

/**	创建一个箱子

	@return	BIN * - 返回创建的箱子指针	
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


/**	获取当前线程对应的箱子指针函数
	通过访问线程本地存储来取得箱子指针

	@return	 BIN * - 获取当前线程对应的箱子指针	
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



/**	初始化分布式内存管理器
	主要是初始化线程本地存储、初始化大块内存管理器，
	创建一定数量的箱子并加入到箱子队列中

	@param	int nMaxThreads - 初始线程数量，初始线程数量决定着初始预创建好的箱子数量，
					初始箱子数量比线程数量少时，线程初始化时会创建新的箱子，
					因此初始线程数量不能超出最大线程数量，否则会造成箱子数量的浪费。
	@return	void - 无	
*/
static void Init_dmalloc(int nMaxThreads)
{
    BIN     *pBin;
    
	// 初始化线程本地存储
	ThreadTls_Init();

    //获取系统内存的大小
    MEMORYSTATUS stat;
    GlobalMemoryStatus (&stat);

    // 初始化大块内存管理器,使用可用物理内存
    // 如果需要，可以改成使用可用虚拟内存
    BigMemory_Init(stat.dwAvailPhys/BIG_MEMORY_SIZE);

    // 初始化内存池
    MemoryPool_Init();

    int i;
	
	// 可以考虑将下面循环并行化执行
    for ( i = 0; i < nMaxThreads; i++ )
    {
		// 创建箱子，并加入箱子队列中
        pBin = CreateBin();
        g_BinQueue.EnQueue(pBin);
    }
}


static void CheckInit_dmalloc()
{
    CScopedLock<CFastLock>    slock(g_InitdmallocFastLock);

    if ( 0 == g_InitFlag )
    {
        // flag 等于1表明是第1个将g_InitFlag变量进行原子加1的线程
        // 这样确保了Init_dmalloc()只被执行一次。
        Init_dmalloc(omp_get_num_procs());
        g_InitFlag = 1;
        AtomicIncrement(&g_dmalloc_init_finished_flag);
    }
}


/**	分布式内存管理的内存分配函数

	@param	UINT uSize - 要分配的内存大小	
	@return	void * - 返回分配到的内存指针，如果系统内存不够用时会返回NULL	
*/
extern "C" DLLAPI void * dmalloc(UINT uSize)
{
    BIN     *pBin;

    if ( 0 == g_dmalloc_init_finished_flag )
    {
        CheckInit_dmalloc();
    }

	// 先通过线程本地存储找到箱子指针
    pBin = GetBinOfCurThread();

	// 通过内存大小获取箱子中的FreeLists索引
    int index = MemoryPool_GetMemSizeIndex(uSize);

	// 从FreeLists中分配内存
    void * p = FreeLists_Alloc( &(pBin->aFreeLists[index]));
    
    return p;
}

/**	分布式内存管理的内存释放函数

	@param	void *p - 要释放的内存	
	@return	void - 无	
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


/**	关闭分布式内存管理
	这个函数最好不要轻易使用
	除非能够确定所有分配的内存都在调用这个函数前被释放了
	当有全局的类变量并且类中采用dmalloc分配了内存时，
	那么程序退出时还有内存在使用，就不能调用这个函数了，
	而要让进程自动去释放内存。

	@return	void - 无	
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


/**	线程退出时的处理函数
	主要为线程退出时处理线程本地存储而设计
	可用作pthread_key_create的回调函数，
	在Windows中则用作DLL入口函数中的DLL_THREAD_DETACH消息中使用

	@param	void* arg - 参数，不使用	
	@return	void - 无	
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
		// 将退出线程的箱子放入队列中便于其他线程重复使用
        g_BinQueue.EnQueue(pBin);
    }
    return;
}

