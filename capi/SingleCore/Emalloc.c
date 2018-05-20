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

/**	SORTTABLE�Ļص������������Ƚ�����DSPACELIST�����Ƿ����

	@param	void *p1 - ��1��DSPACELISTָ��	
	@param	void *p2 - ��2��DSPACELISTָ��	
	@return	INT - �μ�COMPAREFUNC��˵��	
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

/**	Emalloc�ڴ����Ĵ�������

	@param	void -��	
	@return	INT -�ɹ�CAPI_SUCCESS,ʧ��CAPI_FAILED.	
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


/**	Emalloc�ڴ����������ͷź���

	@param	void - ��	
	@return	void - ��	
*/
void Emalloc_Destroy(void)
{
    if ( g_pTable != NULL )
    {
        SortTable_Destroy(g_pTable, DSpaceList_Destroy);
    }
}

/**	Emalloc�ڴ�������Ӻ��������ָ����С��̬�ȳߴ��ڴ�鵽
        Emalloc�ڴ������

	@param	UINT uMemSize - ��̬�ȳߴ���ڴ���С	
	@param	UINT uMemCount - ��̬�ȳߴ���ڴ������	
	@return	INT - �ɹ�CAPI_SUCCESS,ʧ��CAPI_FAILED.	
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

/**	Emalloc�ڴ�����������������ӽ�Emalloc�ڴ�����еĶ�̬�ȳߴ��ڴ�
        ���ڴ���С��С�����ź���

	@return	void - ��	
*/
void Emalloc_Sort()
{
    SortTable_Sort(g_pTable, DSpaceList_Compare);
}

/**	SORTTABLE�ıȽϻص�����

	@param	void *p1 - DSPACELISTָ��	
	@param	void *p2 - ������ڴ����ݴ�С	
	@return	INT -�μ�COMPAREFUNC��˵��	
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

/**	�ڴ���亯���������������һ����С�պ���Ȼ�պô��ڵĶ�̬�ȳߴ��ڴ�
        �����з��䣬����Ҳ�����ʹ��malloc()���з���

	@param	size_t size - Ҫ������ڴ��С�����ֽ�Ϊ��λ	
	@return	void * - �ɹ����ط�����ڴ�ָ�룬ʧ�ܷ���NULL��	
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


/**	��Ӧ��Emalloc()�ڴ������ͷź���

	@param	void *p - Ҫ�ͷŵ��ڴ�ָ��	
	@param	size_t size - �ڴ��С	
	@return	void - ��	
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

/**	�ڴ���亯���������������һ����С�պ���Ȼ�պô��ڵĶ�̬�ȳߴ��ڴ�
        �����з��䣬����Ҳ�����ʹ��malloc()���з���

	@param	size_t size - Ҫ������ڴ��С�����ֽ�Ϊ��λ	
	@return	void * - �ɹ����ط�����ڴ�ָ�룬ʧ�ܷ���NULL��	
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

/**	��Ӧ��Emalloc()�ڴ������ͷź���

	@param	void *p - Ҫ�ͷŵ��ڴ�ָ��	
	@param	size_t size - �ڴ��С	
	@return	void - ��	
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
