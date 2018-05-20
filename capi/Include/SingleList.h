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
 *	SingleList.h
 *
 *	DESCRIPTION
 *		单向链表模块.
 *
 *	HISTORY
 *		10-03-98	create by zhouweiming.
 *
 */

#ifndef __SINGLELIST_H__
#define __SINGLELIST_H__

#ifdef __cplusplus
extern "C" {
#endif


/* 单向链表的结构体 */
typedef struct SINGLELIST_st {
	SINGLENODE	    *pHead;        /* 第1个节点的指针 */
	SINGLENODE	    *pTail;		   /* 最后1个节点的指针 */
    SINGLENODE      *pCur;         /* 当前节点的指针 */
	UINT             uCount;       /* 保存链表节点的个数 */
} SINGLELIST, *PSINGLELIST;


/* 链表的枚举数据结构 */
typedef struct SINGLELIST_ENUM_st {
    SINGLENODE  *pCur;         /* 当前枚举的节点指针 */
    COMPAREFUNC  CompareFunc;  /* 节点数据比较函数 */
    void        *pMatchData;   /* 要枚举的匹配数据 */
} SINGLELIST_ENUM;


/**	单向链表的创建函数，创建完后链表还是空的没有节点在里面

	@param	void - 无	
	@return	SINGLELIST * - 失败返回NULL, 成功时返回一个单向链表结构体指针	
*/
SINGLELIST * SingleList_Create( void );

/**	单向链表的释放函数

	@param	SINGLELIST *pSingleList - 要释放的单向链表的指针	
	@param	DESTROYFUNC pDestroyFunc - 链表节点数据释放回调函数	
	@return	void - 无	
*/
void SingleList_Destroy( SINGLELIST *pSingleList, DESTROYFUNC DestroyFunc );

/**	单向链表的添加节点函数，添加的节点放在单向链表的头部

	@param	SINGLELIST *pSingleList - 要添加的单向链表指针	
	@param	void *pData - 要添加的节点的数据指针	
	@return	INT - 失败返回0，成功返回1	
*/
INT	SingleList_InsertHead( SINGLELIST *pSingleList, void *pData );

/**	单向链表的添加节点函数，添加的节点放在单向链表的尾部

	@param	SINGLELIST *pSingleList - 要添加的单向链表指针	
	@param	void *pData - 要添加的节点的数据指针
	@return	INT - 失败返回0，成功返回1	
*/
INT	SingleList_InsertTail( SINGLELIST *pSingleList, void *pData );

/**	单向链表的弹出头节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	void * - 失败返回NULL, 成功返回要弹出的头节点的数据指针	
*/
void *	SingleList_PopHead( SINGLELIST *pSingleList );

/**	单向链表的弹出尾节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	void * -  失败返回NULL, 成功返回要弹出的尾节点的数据指针	
*/
void *	SingleList_PopTail( SINGLELIST *pSingleList );

/**	链表的删除节点函数，它将删除和pMatchData参数有相同数据的节点
    如果有许多有相同数据的节点它将只删除第一个有相同数据的节点

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@param	void *pMatchData - 要删除节点的匹配数据	
	@param	COMPAREFUNC CompareFunc - 数据比较函数用来比较pMatchData参数和链表
                                      节点参数是否相等	
	@param	DESTROYFUNC DestroyFunc - 链表节点的数据释放函数	
	@return	INT (by default) - 0表示失败或链表中没有匹配的数据，1表示成功删除	
*/
INT		SingleList_Delete( SINGLELIST *pSingleList,  
                        void *pMatchData, 
                        COMPAREFUNC CompareFunc, 
                        DESTROYFUNC DestroyFunc );

/**	单向链表的获取指定位置数据的函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@param	UINT uIndex - 要获取的索引位置	
	@return	void * - 索引位置节点的数据指针	
*/
void *  SingleList_GetAt( SINGLELIST *pSingleList, UINT uIndex );

/**	单向链表的获取链表节点数量函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	UINT - 链表节点数量，为0表示链表是空的没有节点在链表里面	
*/
UINT    SingleList_GetCount(SINGLELIST *pSingleList);

/**	单向链表的获取头节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	void * - 头节点的数据指针	
*/
void *	SingleList_GetHead( SINGLELIST *pSingleList );

/**	单向链表的获取当前节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针 	
	@return	void * - 当前节点的数据指针	
*/
void *	SingleList_GetCursor( SINGLELIST *pSingleList );

/**	单向链表的获取尾节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	void * - 尾节点的数据指针	
*/
void * 	SingleList_GetTail( SINGLELIST *pSingleList );


/**	单向链表的枚举函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针
	@return	void  - 无
*/
void SingleList_EnumBegin( SINGLELIST *pSingleList );

/**	单向链表枚举下一个节点的函数，
    第一次调用此函数前必须先调用SingleList_EnumBegin()函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表的指针	
	@return	void * - 枚举到的节点数据指针	
*/
void *  SingleList_EnumNext( SINGLELIST *pSingleList );


/**	单向链表的遍历函数

	@param	SINGLELIST *pSingleList - 要操作单向链表指针	
	@param	TRAVERSEFUNC TraverseFunc - 节点数据的遍历操作函数	
	@return	INT - 成功返回1，失败返回0	
*/
INT SingleList_Traverse( SINGLELIST *pSingleList, TRAVERSEFUNC TraverseFunc );

/**	单向链表的插入排序函数
    排序是按照从小到大进行排列，这里说的大小是由CompareFunc来决定的
    因此用户可以通过CompareFunc的返回值设置来决定使用顺序排序或逆序排序

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@param	COMPAREFUNC CompareFunc - 节点数据比较函数	
	@return	INT - 成功返回1，失败返回0	
*/
INT SingleList_InsertSort( SINGLELIST *pSingleList, COMPAREFUNC CompareFunc );

SINGLELIST * SingleList_Split(SINGLELIST *pSingleList, UINT uCount);

INT SingleList_Merge( SINGLELIST *pSingleListA, 
                      SINGLELIST *pSingleListB, 
                      COMPAREFUNC CompareFunc );

INT SingleList_MergeSort( SINGLELIST *pSingleList, 
                          COMPAREFUNC CompareFunc, 
                          UINT uInsertSortCount );

INT SingleList_RadixSort( SINGLELIST *pSingleList,
                          UINT uRadix,
                          UINT uMaxKeyLen,
                          GETKEYFUNC GetKeyFunc );



#ifdef __cplusplus
}
#endif


#endif /* __SINGLELIST_H__ */
