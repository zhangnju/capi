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
 *	SortTable.h
 *
 *	DESCRIPTION
 *		Interface to a generic Sort table type.
 *
 *	HISTORY
 *		09-18-98	create by zhouweiming.
 *
 */
#ifndef __SORTTABLE_H__
#define __SORTTABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SORTTABLE_st {
    void    **ppData;       /* 存放数据指针的指针数组 */
    UINT    uCursorCount;   /* 当前排序表中的数据数量 */ 
    UINT    uMaxCount;      /* 排序表中的最大数据数量 */
} SORTTABLE; 


SORTTABLE *	SortTable_Create(UINT uMaxCount);
INT		SortTable_Add( SORTTABLE *pTable, void *pData );
void		SortTable_Destroy( SORTTABLE *pTable, DESTROYFUNC DestroyFunc );

void *		SortTable_FindData(SORTTABLE *pTable, void *pData, 
                               COMPAREFUNC CompareFunc);
void *          SortTable_BlurFind(SORTTABLE *pTable, void *pData, 
                               COMPAREFUNC CompareFunc);
void *		SortTable_GetByID( SORTTABLE *pTable, UINT uId );

INT             SortTable_Sort(SORTTABLE *pTable, COMPAREFUNC CompareFunc);

#ifdef __cplusplus
}
#endif

#endif /* __SORTTABLE_H__ */
