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
 *	CapiGlobal.h
 *
 *	DESCRIPTION
 *		global Prototypes and definition for capi library.
 *
 *	HISTORY
 *		10-03-2003	create by zhouweiming.
 *
 */
#ifndef __CAPIGLOBAL_H__
#define __CAPIGLOBAL_H__

#ifdef _WIN32 
#include <windows.h>
#endif
//#include <stdlib.h>
#include <memory.h>
//#include <string.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _TEST 
#define PRIVATE public
#else
#define PRIVATE private
#endif

#define CAPI_SUCCESS        0
#define CAPI_FAILED         1
#define CAPI_NO_MEMORY      2
#define CAPI_NOT_FOUND      3


typedef int             BOOL;
typedef long            LONG;
typedef unsigned long   DWORD;
typedef unsigned int    UINT;
typedef int             INT;
typedef void * HANDLE;




/* 单向链表的节点结构 */
typedef struct SINGLENODE_st {
	struct SINGLENODE_st *pNext;     /* 下一个节点指针 */
    void	*pData;                  /* 数据指针 */
} SINGLENODE, *PSINGLENODE;

typedef struct DOUBLENODE_st {
    struct DOUBLENODE_st *pNext;     /* 下一个节点指针 */
    struct DOUBLENODE_st *pPrev;     /* 前一个节点指针 */
    void    *pData;                  /* 数据指针 */
} DOUBLENODE;

typedef unsigned int (_stdcall *THREADFUNC)(void *pArgs);

typedef struct  TASK_st {
    THREADFUNC  func;         //任务执行函数
    void        *pArg;        //任务函数的参数
    DWORD       time_consume; //任务执行的耗时
} TASK;


/**	通用类型数据比较函数

	@param	void *pData1 -要比较的第1个参数	
	@param	void *pData2 -要比较的第2个参数	
        @return	INT - 小于0表示pData1小于pData2, 等于0表示pData1等于pData2,
                  大于0表示pData1大于pData2
*/
typedef INT (*COMPAREFUNC) ( void *pData1, void *pData2 );

/**	通用类型数据释放函数

	@param	void *pData -要释放的数据	
	@return	 -无	
*/
typedef void (*DESTROYFUNC) ( void *pData );

/**	通用类型数据的遍历执行函数

	@param	void *pData - 要操作的数据指针	
	@return	void - 无	
*/
typedef void (*TRAVERSEFUNC)( void *pData );


/**	通用类型数据的遍历执行函数

	@param	void *pData - 要操作的数据指针	
	@return	void - 无	
*/
typedef INT (*VISITFUNC)(void *pData);

/**	通用数据拷贝函数

	@param	void *pData - 要拷贝的数据	
	@return	void * - 成功返回拷贝的数据，失败返回NULL	
*/
typedef void *(*COPYFUNC)(void *pData);

/**	基数排序的获取关键字转换成序号的函数

	@param	void *pData - 关键字指针	
    @param	UINT *uKeyIndex - 关键字的位数	
	@return	UINT - 关键字转换后的序号	
*/
typedef UINT (*GETKEYFUNC)( void *pData, UINT uKeyIndex );

/**	计算哈希值的回调函数

        @param  void *pKey - 要计算哈希值的关键词	
        @param  UINT uBucketNum - 哈希表中的BUCKET数组大小	
	@return UINT - 返回计算出的哈希值，一般用作哈希表BUCKET数组下标。	
*/
typedef UINT (*HASHFUNC)(void *pKey, UINT uBucketNum);

#if defined(_WIN32)

//#define     LOCK            HANDLE
#define     EVENT           HANDLE
#define     SEMAPHORE       HANDLE

//#define LockCreate()        CreateMutex(NULL, FALSE, NULL)
//#define Lock(x)             (void)WaitForSingleObject((x), INFINITE)
//#define Unlock(x)           (void)ReleaseMutex(x)
//#define LockClose(x)        (void)CloseHandle(x)

#define EventCreate()       CreateEvent(NULL, TRUE, FALSE, NULL)
#define WaitEvent(x)        (void)WaitForSingleObject((x), INFINITE)
#define SendEvent(x)        (void)SetEvent(x)
#define EventClose(x)       (void)CloseHandle(x)


#define SemaCreate(x,y)     CreateSemaphore(NULL,x,y,NULL)
#define SemaWait(x)         WaitForSingleObject(x,INFINITE)
#define SemaRelease(x,y)    ReleaseSemaphore(x,y,NULL)
#define SemaClose(x)        CloseHandle(x)

#define CAPI_Yield()        SwitchToThread()

BOOL AtomicCAS(LONG volatile *dest, LONG newvalue, LONG oldvalue);

#define AtomicIncrement(x)  InterlockedIncrement(x)
#define AtomicDecrement(x)  InterlockedDecrement(x)
#define AtomicWrite(x, y)     InterlockedExchange(x, y)

#else
BOOL AtomicCAS(LONG volatile *dest, LONG newvalue, LONG oldvalue);
LONG AtomicWrite(LONG volatile *Target, LONG Value);
LONG AtomicIncrement(LONG volatile *Target);
BOOL TAS(LONG volatile *value);
LONG AtomicDecrement(LONG volatile *Target);

#endif /* _WIN32 */
    
BOOL AtomicCAS64(LONGLONG volatile *dest, LONGLONG newvalue, LONGLONG oldvalue);

#define AtomicIncrement64(x)  InterlockedIncrement64(x)
#define AtomicDecrement64(x)  InterlockedDecrement64(x)
#define AtomicWrite64(x, y)     InterlockedExchange64(x, y)

#define    CAPI_EXIT_TASK      1
#define    CAPI_NOT_EXIT_TASK  0


/* memory allocation functions */
void *CapiMalloc(size_t size);
void *CapiRealloc(void *p, size_t size);
void CapiFree(void *p);


/*** external functions ***/
UINT        HashInt( void *pKey, UINT uBucketCount );
INT         StrCompare( void *pStr1, void *pStr2 );
INT         PointerCompare(void *p1, void *p2);
INT         IntCompare(void *pInt1, void *pInt2);
INT         BinCompare( void *str1, int str1_len, void *str2, int str2_len );

UINT        HashString( void *pStr, UINT uBucketCount );
UINT        HashBin( void *pData, UINT uLength, UINT uBucketCount );


UINT GetStrKeyNoCase( void *pszData, UINT uKeyIndex );
UINT GetStrKey( void *pszData, UINT uKeyIndex );
UINT GetIntKey( void *pData, UINT uKeyIndex );
UINT GetWordKey( UINT uData, UINT uKeyIndex );



#ifdef __cplusplus
}
#endif

#endif /* __CAPIGLOBAL_H__ */
