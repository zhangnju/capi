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
#include <stdio.h>
#include "CapiGlobal.h"
#include "SortTable.h"
#include "DSpaceList.h"
#include "Emalloc.h"

#define     SORTTABLE_MAXCOUNT  128
#define     MINDSNODECOUNT      5
#define     DSNODECOUNT         64

SORTTABLE   *g_pTable = NULL;

/**	SORTTABLE的回调函数，用来比较两个DSPACELIST对象是否相等

	@param	void *p1 - 第1个DSPACELIST指针	
	@param	void *p2 - 第2个DSPACELIST指针	
	@return	INT - 参见COMPAREFUNC的说明	
*/
INT DSpaceList_Compare(void *p1, void *p2)
{
    DSPACELIST  *pList1, *pList2;

    pList1 = (DSPACELIST *)p1;
    pList2 = (DSPACELIST *)p2;

    if ( pList1->uDataSize > pList2->uDataSize )
    {
        return 1;
    }
    else if ( pList1->uDataSize < pList2->uDataSize )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/**	Emalloc内存管理的创建函数

	@param	void -无	
	@return	INT -成功CAPI_SUCCESS,失败CAPI_FAILED.	
*/
INT Emalloc_Create( void )
{
    g_pTable = SortTable_Create(SORTTABLE_MAXCOUNT);
    if ( g_pTable == NULL )
    {
        return CAPI_FAILED;
    }
    return CAPI_SUCCESS;
}


/**	Emalloc内存管理的整体释放函数

	@param	void - 无	
	@return	void - 无	
*/
void Emalloc_Destroy(void)
{
    if ( g_pTable != NULL )
    {
        SortTable_Destroy(g_pTable, DSpaceList_Destroy);
    }
}

/**	Emalloc内存管理的添加函数，添加指定大小动态等尺寸内存块到
        Emalloc内存管理中

	@param	UINT uMemSize - 动态等尺寸的内存块大小	
	@param	UINT uMemCount - 动态等尺寸的内存块数量	
	@return	INT - 成功CAPI_SUCCESS,失败CAPI_FAILED.	
*/
INT Emalloc_Add(UINT uMemSize, UINT uMemCount)
{
    DSPACELIST *pList = DSpaceList_Create(uMemSize, uMemCount, 
                                MINDSNODECOUNT, DSNODECOUNT);
    if ( pList == NULL )
    {
        return CAPI_FAILED;
    }
    return SortTable_Add(g_pTable, pList);
}

/**	Emalloc内存管理的排序函数，将添加进Emalloc内存管理中的动态等尺寸内存
        按内存块大小从小到大排好序

	@return	void - 无	
*/
void Emalloc_Sort()
{
    SortTable_Sort(g_pTable, DSpaceList_Compare);
}

/**	SORTTABLE的比较回调函数

	@param	void *p1 - DSPACELIST指针	
	@param	void *p2 - 分配的内存数据大小	
	@return	INT -参加COMPAREFUNC的说明	
*/
INT DSpaceList_CmpData(void *p1, void *p2)
{
    DSPACELIST  *pList;
    
    pList = (DSPACELIST *)p1;
    if ( pList->uDataSize < (UINT)p2 )
    {
        return -1;
    }
    else if ( pList->uDataSize > (UINT)p2 )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

#ifdef _DEBUG

#define MEMORY_CHECKSUM     0xBFBFBFBF

/**	内存分配函数，从排序表中找一个大小刚好相等或刚好大于的动态等尺寸内存
        来进行分配，如果找不到则使用malloc()进行分配

	@param	size_t size - 要分配的内存大小，以字节为单位	
	@return	void * - 成功返回分配的内存指针，失败返回NULL。	
*/
void *Emalloc(size_t size)
{
    void *p;

    DSPACELIST *pList = SortTable_BlurFind(g_pTable, (void *)(size+4), 
                            DSpaceList_CmpData);
    if ( pList != NULL )
    {
        p = DSpaceList_Alloc(pList);
    }
    else
    {
        p = malloc(size+4);
    }

    if ( p != NULL )
    {
        *((INT *)((char *)p + size)) = MEMORY_CHECKSUM;
    }

    return p;
}


/**	对应于Emalloc()内存分配的释放函数

	@param	void *p - 要释放的内存指针	
	@param	size_t size - 内存大小	
	@return	void - 无	
*/
void Efree(void *p, size_t size)
{ 
    DSPACELIST *pList;

    if ( *((INT *)((char *)p + size)) != MEMORY_CHECKSUM )
    {
        printf("Memory address overrun: Address=%ld, size=%d\n", (INT)p, size);
    }

    pList = SortTable_BlurFind(g_pTable, (void *)size, 
                            DSpaceList_CmpData);
    if ( pList != NULL )
    {
        DSpaceList_Free(pList, p);
    }
    else
    {
        free(p);
    }
}

#else

/**	内存分配函数，从排序表中找一个大小刚好相等或刚好大于的动态等尺寸内存
        来进行分配，如果找不到则使用malloc()进行分配

	@param	size_t size - 要分配的内存大小，以字节为单位	
	@return	void * - 成功返回分配的内存指针，失败返回NULL。	
*/
void *Emalloc(size_t size)
{
    DSPACELIST *pList = SortTable_BlurFind(g_pTable, (void *)size, 
                            DSpaceList_CmpData);
    if ( pList != NULL )
    {
        return DSpaceList_Alloc(pList);
    }
    else
    {
        return malloc(size);
    }
}

/**	对应于Emalloc()内存分配的释放函数

	@param	void *p - 要释放的内存指针	
	@param	size_t size - 内存大小	
	@return	void - 无	
*/
void Efree(void *p, size_t size)
{ 
    DSPACELIST *pList = SortTable_BlurFind(g_pTable, (void *)size, 
                            DSpaceList_CmpData);
    if ( pList != NULL )
    {
        DSpaceList_Free(pList, p);
    }
    else
    {
        free(p);
    }
}

#endif

#if 0

#define MEMORY_BLOCK_SIZE  32768
main()
{
    int i;
    int uMemSize;
    char *psz;
    
    uMemSize = 8;
    
    Emalloc_Create();

    for ( i = 0; i < 10; i++ )
    {
        Emalloc_Add(uMemSize, MEMORY_BLOCK_SIZE / uMemSize);
        uMemSize *= 2;
    }

    psz = Emalloc(48);
    free(psz, 48);

    Emalloc_Destroy();
}

#endif
