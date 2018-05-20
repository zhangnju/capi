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
#include <string.h>
#include "CapiGlobal.h"
#include "SingleList.h"

 
/**	单向链表的创建函数，创建完后链表还是空的没有节点在里面

	@param	void - 无	
	@return	SINGLELIST * - 失败返回NULL, 成功时返回一个单向链表结构体指针	
*/
SINGLELIST * SingleList_Create( void )
{
    SINGLELIST  *pSingleList;

    /* 分配内存操作 */
    pSingleList = (SINGLELIST *)malloc(sizeof(SINGLELIST));
    if ( pSingleList != NULL )
    {
        /* 初始化链表结构体各指针成员为空，链表节点个数为0 */
        pSingleList->pCur = NULL;
        pSingleList->pHead = NULL;
        pSingleList->pTail = NULL;
        pSingleList->uCount = 0;
    }

    return pSingleList;
}


/**	单向链表的释放函数

	@param	SINGLELIST *pSingleList - 要释放的单向链表的指针	
	@param	DESTROYFUNC pDestroyFunc - 链表节点数据释放回调函数	
	@return	void - 无	
*/
void SingleList_Destroy( SINGLELIST *pSingleList, DESTROYFUNC DestroyFunc )
{
    SINGLENODE  *pNode;

    if ( pSingleList )
    {
        /* 从头节点开始，一个接一个释放链表节点及节点数据 */
        pNode = pSingleList->pHead;
        while ( pNode != NULL )
        {
            SINGLENODE  *pDelNode;

            pDelNode = pNode;
            pNode = pNode->pNext;

            if ( DestroyFunc != NULL && pDelNode->pData != NULL )
            {
                /* 释放数据 */
                (*DestroyFunc)( pDelNode->pData );
            }
            free( pDelNode ); /* 释放节点 */
        }
        /* 释放链表结构体 */ 
        free( pSingleList );
    }
}


/**	单向链表的添加节点函数，添加的节点放在单向链表的头部

	@param	SINGLELIST *pSingleList - 要添加的单向链表指针	
	@param	void *pData - 要添加的节点的数据指针	
	@return	INT - 失败返回CAPI_FAILED，成功返回CAPI_SUCCESS	
*/
INT	SingleList_InsertHead( SINGLELIST *pSingleList, void *pData )
{
    SINGLENODE  *pNode;

    /* 参数校验 */
    if ( pSingleList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* 新建一个节点 */
    pNode = (SINGLENODE *)malloc( sizeof(SINGLENODE) );
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;  /* 将节点数据指针指向传进来的数据 */

    /* 将新建节点的下一节点指针指向头节点，并将头节点改为新建的节点 */
    pNode->pNext = pSingleList->pHead;
    pSingleList->pHead = pNode;

    /*
     * 判断是否尾节点指针为空，如果为空表示原来链表中没有节点，
     * 此时应该将尾节点指向新加入的节点 
     */
    if ( pSingleList->pTail == NULL )
    {
        pSingleList->pTail = pNode;
    }

    /* 将链表节点数据加1 */
    pSingleList->uCount++;
    
    return CAPI_SUCCESS;
}


/**	单向链表的添加节点函数，添加的节点放在单向链表的尾部

	@param	SINGLELIST *pSingleList - 要添加的单向链表指针	
	@param	void *pData - 要添加的节点的数据指针
	@return	INT - 失败返回CAPI_FAILED，成功返回CAPI_SUCCESS	
*/
INT	SingleList_InsertTail( SINGLELIST *pSingleList, void *pData )
{
    SINGLENODE  *pNode;

    /* 参数校验 */
    if ( pSingleList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* 新建一个节点 */
    pNode = (SINGLENODE *)malloc( sizeof(SINGLENODE) );
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;  /* 将节点数据指针指向传进来的数据 */
    pNode->pNext = NULL;   /* 将节点的下一节点赋为空指针NULL */

    /*
     * 判断是否尾节点指针为空，如果为空表示原来链表中没有节点，
     * 此时应该将尾节点指向新加入的节点, 并且头节点指针也应该指向新节点 
     */
    if ( pSingleList->pTail == NULL )
    {
        pSingleList->pTail = pNode;
        pSingleList->pHead = pNode;
    }
    else
    {
        /*
         * 如果尾节点指针不为空，此时应该将尾节点下一节点指针指向新加入的节点, 
         * 并且尾节点指针也应该指向新节点 
         */
        pSingleList->pTail->pNext = pNode;
        pSingleList->pTail = pNode;
    }

    /* 将链表节点数据加1 */
    pSingleList->uCount++;
    
    return CAPI_SUCCESS;
}


/**	单向链表的弹出头节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	void * - 失败返回NULL, 成功返回要弹出的头节点的数据指针	
*/
void *	SingleList_PopHead( SINGLELIST *pSingleList )
{
	SINGLENODE	*pPopNode;   /* 用来指向要弹出数据的节点的指针 */
	void		*pPopData;   /* 用来指向要弹出的数据的指针 */

    /* 参数校验 */
    if ( pSingleList == NULL || pSingleList->pHead == NULL ) 
    {
		return NULL;
    }

    /* 将要弹出数据的节点指针指向链表头节点，弹出数据指针指向头节点的数据 */
    pPopNode = pSingleList->pHead;
    pPopData = pPopNode->pData;

    /* 
     * 判断当前节点指针是否指向头节点，如果指向头节点则需要将其
     * 指向头节点的下一节点 
     */
    if ( pSingleList->pCur == pSingleList->pHead )
    {
        pSingleList->pCur = pSingleList->pHead->pNext;
    }

    /* 将头节点指针指向头节点的下一节点 */
    pSingleList->pHead = pSingleList->pHead->pNext;
	
    /* 将链表节点数量减1 */
    pSingleList->uCount--;

    /* 如果链表的节点数量已经为0则表示原来只有一个节点，弹出头节点后，
     * 链表已经为空，没有节点在里面，此时应该将尾节点指针赋空
     * 当前节点指针由于前面已经处理过了，如果只有一个节点的话肯定为空
     * 所以这里不需要处理当前节点指针
     */
    if ( pSingleList->uCount == 0 ) {
		pSingleList->pTail = NULL;
	}
	
    /* 释放弹出的节点, 注意这里并没有释放节点数据指针 */
    free( pPopNode );

    return pPopData;    /* 返回头节点的数据指针 */
}


/** 单向链表的弹出尾节点函数
    这个函数由于要查找尾节点的前一节点，因此效率很低

    @param	SINGLELIST *pSingleList - 要操作的单向链表指针	
    @return	void * -  失败返回NULL, 成功返回要弹出的尾节点的数据指针	
*/
void *	SingleList_PopTail( SINGLELIST *pSingleList )
{
    SINGLENODE	*pPopNode;   /* 用来指向要弹出数据的节点的指针 */
    SINGLENODE	*pTailPrevNode;   /* 用来指向尾节点的上一节点的指针 */
    void		*pPopData;   /* 用来指向要弹出的数据的指针 */

    /* 参数校验 */
    if ( pSingleList == NULL || pSingleList->pHead == NULL ) 
    {
        return NULL;
	}

    /* 将要弹出数据的节点指针指向链表头节点，弹出数据指针指向头节点的数据 */
    pPopNode = pSingleList->pTail;
    pPopData = pPopNode->pData;

    pTailPrevNode = pSingleList->pHead;

    /* 如果链表的头节点和尾节点相同则表示原来只有一个节点，弹出尾节点后，
     * 此时链表已经为空，没有节点在里面，此时应该将头节点指针赋空
     * 当前节点指针由于前面已经处理过了，如果只有一个节点的话肯定为空
     * 所以这里不需要处理当前节点指针
     */
    if ( pSingleList->pTail == pSingleList->pHead )
    {
        pTailPrevNode = NULL;
	    pSingleList->pHead = NULL;
    }
    else 
    {
        while ( pTailPrevNode != NULL )
        {
            if ( pTailPrevNode->pNext == pSingleList->pTail )
            {
                break;
            }
            pTailPrevNode = pTailPrevNode->pNext;
        }
    }

    /* 
     * 判断当前节点指针是否指向尾节点，如果指向头节点的话则需要将其
     * 指向尾节点的前一节点 
     */
    if ( pSingleList->pCur == pSingleList->pTail )
    {
        pSingleList->pCur = pTailPrevNode;
    }

    /* 将尾节点指针指向尾节点的前一节点 */
    pSingleList->pTail = pTailPrevNode;

    if ( pTailPrevNode != NULL )
    {
        pTailPrevNode->pNext = NULL;
    }
    
    /* 将链表节点数量减1 */
    pSingleList->uCount--;

    /* 释放弹出的节点, 注意这里并没有释放节点数据指针 */
    free( pPopNode );

    return pPopData;    /* 返回头节点的数据指针 */    
}


/**	链表的删除节点函数，它将删除和pMatchData参数有相同数据的节点
    如果有许多有相同数据的节点它将只删除第一个有相同数据的节点

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@param	void *pMatchData - 要删除节点的匹配数据	
	@param	COMPAREFUNC CompareFunc - 数据比较函数用来比较pMatchData参数和链表
                                      节点参数是否相等	
	@param	DESTROYFUNC DestroyFunc - 链表节点的数据释放函数	
	@return	INT (by default) - CAPI_FAILED表示失败或链表中没有匹配的数据，CAPI_SUCCESS表示成功删除	
*/
INT	 SingleList_Delete( SINGLELIST *pSingleList,  
                        void *pMatchData, 
                        COMPAREFUNC CompareFunc, 
                        DESTROYFUNC DestroyFunc )
{
    SINGLENODE  *pNode;
    SINGLENODE  *pPrevNode;

    /* 参数校验 */
    if ( pSingleList == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pSingleList->pHead;
    pPrevNode = pNode;

    while ( pNode != NULL )
    {
        /* 比较节点数据是否和匹配数据匹配 */
        if ( (*CompareFunc)( pNode->pData, pMatchData ) == 0 )
        {
            if ( pPrevNode == pNode )
            {
                /* 头节点匹配上了，需要删除头节点 */
                pSingleList->pHead = pNode->pNext;
                if ( pSingleList->pTail == pNode )
                {
                    /* 
                     * 如果尾节点和pNode相同，表明链表里只有一个节点
                     * 此时需要将链表尾节点指针和链表当前节点指针赋空
                     */
                    pSingleList->pTail = NULL;
                    pSingleList->pCur = NULL;
                }
            }
            else
            {
                pPrevNode->pNext = pNode->pNext;
                if ( pSingleList->pTail == pNode )
                {
                    /* 
                     * 如果尾节点和pNode相同，表明删除的是尾节点
                     * 此时需要将尾节点指针指向要删除节点的前一个节点
                     */
                    pSingleList->pTail = pPrevNode;
                }

                if ( pSingleList->pCur == pNode )
                {
                    /* 
                     * 如果链表当前节点和pNode相同，表明删除的是当前节点
                     * 此时需要将当前节点指针指向要删除节点的下一个节点
                     */
                    pSingleList->pCur = pNode->pNext;
                }
            }

            /* 释放节点数据和节点占用的内存 */
            if ( DestroyFunc != NULL && pNode->pData != NULL )
            {
                (*DestroyFunc)( pNode->pData );
            }
            free( pNode );

            break;
        }
        pPrevNode = pNode;
        pNode = pNode->pNext;
    }

    return CAPI_SUCCESS;
}


/**	单向链表的获取指定位置数据的函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@param	UINT uIndex - 要获取的索引位置，索引从0开始计算	
	@return	void * - 索引位置节点的数据指针	
*/
void *  SingleList_GetAt( SINGLELIST *pSingleList, UINT uIndex )
{
    UINT        i;
    SINGLENODE  *pNode;

    if ( pSingleList == NULL || pSingleList->uCount >= uIndex )
    {
        return NULL; 
    }

    pNode = pSingleList->pHead;
    for ( i = 0; i < uIndex; i++ )
    {
        pNode = pNode->pNext;
    }

    return pNode->pData;
}


/**	单向链表的获取链表节点数量函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	UINT - 链表节点数量，为0表示链表是空的没有节点在链表里面或者参数非法	
*/
UINT    SingleList_GetCount(SINGLELIST *pSingleList)
{
    if ( pSingleList == NULL )
    {
        return 0;
    }
    return pSingleList->uCount;
}


/**	单向链表的获取头节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	void * - 头节点的数据指针	
*/
void *	SingleList_GetHead( SINGLELIST *pSingleList )
{
    if ( pSingleList == NULL )
    {
        return NULL;
    }

    if ( pSingleList->pHead == NULL )
    {
        return NULL;
    }

    return pSingleList->pHead->pData;
}


/**	单向链表的获取当前节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针 	
	@return	void * - 当前节点的数据指针	
*/
void *	SingleList_GetCursor( SINGLELIST *pSingleList )
{
    if ( pSingleList == NULL )
    {
        return NULL;
    }

    if ( pSingleList->pCur == NULL )
    {
        return NULL;
    }

    return pSingleList->pCur->pData;
}


/**	单向链表的获取尾节点函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@return	void * - 尾节点的数据指针	
*/
void * 	SingleList_GetTail( SINGLELIST *pSingleList )
{
    if ( pSingleList == NULL )
    {
        return NULL;
    }

    if ( pSingleList->pTail != NULL )
    {
        return pSingleList->pTail->pData;
    }
    else
    {
        return NULL;
    }
}


/**	单向链表的枚举初始化函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针
	@return	void - 无	
*/
void SingleList_EnumBegin( SINGLELIST *pSingleList )
{
    pSingleList->pCur = pSingleList->pHead;

    return;
}


/**	单向链表枚举下一个节点的函数，第一次调用此函数前必须
    先调用SingleList_EnumBegin()函数

	@param	SINGLELIST *pSingleList - 要操作的单向链表的指针	
	@return	void * - 枚举到的节点数据指针	
*/
void *  SingleList_EnumNext( SINGLELIST *pSingleList )
{
    SINGLENODE  *pCur;

    pCur = pSingleList->pCur;
    if ( pCur != NULL )
    {
        pSingleList->pCur = pCur->pNext;

        return pCur->pData;
    }
    return NULL;
}



/**	单向链表的遍历函数

	@param	SINGLELIST *pSingleList - 要操作单向链表指针	
	@param	TRAVERSEFUNC TraverseFunc - 节点数据的遍历操作函数	
	@return	INT - 成功返回CAPI_SUCCESS，失败返回CAPI_FAILED	
*/
INT SingleList_Traverse( SINGLELIST *pSingleList, TRAVERSEFUNC TraverseFunc )
{
    SINGLENODE  *pNode;

    /* 参数校验 */
    if ( pSingleList == NULL || TraverseFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pSingleList->pHead;
    
    /* 开始遍历链表 */
    while ( pNode != NULL )
    {
        (*TraverseFunc)( pNode->pData ); /* 调用遍历回调函数处理数据 */
        pNode = pNode->pNext;
    }

    return CAPI_SUCCESS;
}


/**	单向链表的插入排序函数
    排序是按照从小到大进行排列，这里说的大小是由CompareFunc来决定的
    因此用户可以通过CompareFunc的返回值设置来决定使用顺序排序或逆序排序

	@param	SINGLELIST *pSingleList - 要操作的单向链表指针	
	@param	COMPAREFUNC CompareFunc - 节点数据比较函数	
	@return	INT - 成功返回CAPI_SUCCESS，失败返回CAPI_FAILED	
*/
INT SingleList_InsertSort( SINGLELIST *pSingleList, COMPAREFUNC CompareFunc )
{
    SINGLENODE  *pNode;     /* 用来遍历pSingleList的临时指针 */
    SINGLENODE  *pPrevNode; /* pNode的前一节点指针 */

    /* 参数校验 */
    if ( pSingleList == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pSingleList->pHead;
    pPrevNode = NULL;

    if ( pNode == NULL )
    {
        /* 链表中没有节点，我们把它当作已经排好了序 */
        return CAPI_SUCCESS;
    }

    while ( pNode->pNext != NULL )
    {
        SINGLENODE  *pTempNode;

        pTempNode = pSingleList->pHead;
        pPrevNode = NULL;
        while ( pTempNode != pNode->pNext )
        {
            if ( (*CompareFunc)( pNode->pNext->pData, pTempNode->pData ) < 0 )
            {
                SINGLENODE  *pCurNode = pNode->pNext;

                /* 执行插入操作 */
                if ( pPrevNode != NULL ) /* 插入不在头节点前的情况 */
                {
                    /* 将pCurNode节点弹出来 */
                    pNode->pNext = pNode->pNext->pNext;

                    /* 将pNode->pNext插入pTempNode 之前 */
                    pPrevNode->pNext = pCurNode;
                    pCurNode->pNext = pTempNode;
                }
                else /* 插入在头节点前的情况 */
                {
                    /* 将pCurNode节点弹出来 */
                    pNode->pNext = pNode->pNext->pNext;

                    /* 将pNode->pNext节点变为头节点 */
                    pSingleList->pHead = pCurNode;
                    pCurNode->pNext = pTempNode;
                }

                /* 修改尾指针指向 */
                if ( pCurNode == pSingleList->pTail )
                {
                    pSingleList->pTail = pNode;
                }

                break;
            }

            pPrevNode = pTempNode;
            pTempNode = pTempNode->pNext;
        } /* while ( pTempNode != pNode->pNext ) */

        if ( pTempNode == pNode->pNext ) /* 没有插入的情况 */
        {
            pNode = pNode->pNext;
        }
        else /* 已经插入的情况 */
        {
            /* 已经插入后不需将pNode指针后移，因为前面操作过程中已经移动过了 */
        }
    }

    return CAPI_SUCCESS;
}


/**	将一个单向链表分成两个单向链表
    只是从原来链表中间断开，分完后原链表变成分开后的第一条链表

	@param	SINGLELIST *pSingleList - 要被劈开的单向链表	
	@param	UINT nCount - 分开后的第一个链表的节点个数	
	@return	SINGLELIST * - 成功时返回分开后的第二条链表，失败返回NULL。	
*/
SINGLELIST * SingleList_Split(SINGLELIST *pSingleList, UINT uCount)
{
    SINGLENODE	*pNode;       /* 用来保存分开处的节点的指针 */
	SINGLELIST  *pSecondList; /* 用来记录分开后的第二条链表的指针 */
	UINT    	uIndex;       /* 临时循环体变量 */

	if ( pSingleList == NULL )
	{
		return NULL;
	}
	
    /* 参数校验 */
    if ( uCount == 0 || pSingleList->uCount <= uCount)
	{
		return NULL;
	}

    /* 创建一条空链表 */
    pSecondList = SingleList_Create();
    if ( pSecondList == NULL )
    {
        return NULL;
    }

	/* 获取要分开的位置 */
    pNode = pSingleList->pHead;
    for ( uIndex = 1; uIndex < uCount; uIndex++ )
    {
        pNode = pNode->pNext;
    }
	
	/* 填充第二条链表的内容. */
    pSecondList->pHead = pNode->pNext;
    pSecondList->pTail = pSingleList->pTail;
    pSecondList->uCount = pSingleList->uCount - uCount;

	/* 修改第一条链表的内容. */
    pSingleList->pTail = pNode;
    pSingleList->uCount = uCount;

	/* 将第一条链表尾节点的pNext指针指向NULL */
    pSingleList->pTail->pNext = NULL;

    return pSecondList;
}


/**	将两个已经排好序的链表进行合并
    两个链表都必须按从小到达进行排列，即尾节点最大
    合并后的链表也是从小到大进行排列

	@param	SINGLELIST *pSingleListA - 要合并的链表	
	@param	SINGLELIST *pSingleListB -要合并的链表	
	@param	COMPAREFUNC CompareFunc - 节点比较函数	
	@return	INT - CAPI_FAILED 表示失败，失败时，参数没有变化
                  CAPI_SUCCESS 表示成功，成功时pSingleListA是合并后的结果，
                  pSingleListB会被释放
*/
INT SingleList_Merge(SINGLELIST *pSingleListA, 
                     SINGLELIST *pSingleListB, 
                     COMPAREFUNC CompareFunc)
{
    SINGLENODE	*pNodeA; /* 用来指向链表pSingleListA的节点的临时指针 */
    SINGLENODE  *pNodeB; /* 用来指向链表pSingleListB的节点的临时指针 */
    SINGLENODE  *pPrevA; /* pNodeA的前一节点指针 */

    /*  参数校验 */
    if ( pSingleListA == NULL || pSingleListB == NULL || CompareFunc == NULL )
    {
		return CAPI_FAILED;
    }

    pNodeA = pSingleListA->pHead;
    pNodeB = pSingleListB->pHead;
    pPrevA = NULL;

    while ( pNodeB != NULL )
    {
        while ( pNodeA != NULL )
		{
            if ( (*CompareFunc)( pNodeA->pData, pNodeB->pData ) >= 0 )
			{
                SINGLENODE *pNode;
				
				/* 将pNodeB弹出来保存到pNode中 */
                pNode = pNodeB;
                pNodeB = pNodeB->pNext;
				
				/* 将pNode插入到pNodeA前面 */
                if ( pPrevA == NULL )
                {
                    /* 插入在头指针前的情况，需要修改头指针 */
                    pSingleListA->pHead = pNode;
                    pNode->pNext = pNodeA;
                }
                else
                {
                    pPrevA->pNext = pNode;
                    pNode->pNext = pNodeA;
                }
                pPrevA = pNode;
			
				break;
			}
            pPrevA = pNodeA;
			pNodeA = pNodeA->pNext;
		}

		/*
         * 如果pSingleListB的所有数据都大于链表A的数据，
         * 将pSingleListB插入pSingleListA尾部.
         */
        if ( pNodeA == NULL )
		{
			pSingleListA->pTail->pNext = pNodeB;
			pSingleListA->pTail = pSingleListB->pTail;
			
			break;
		}				
    }
	
    /* 修改pSingleListA的节点总数量 */
    pSingleListA->uCount += pSingleListB->uCount;

	free( pSingleListB );

	return CAPI_SUCCESS;
}


/**	对链表使用归并插入排序，归并和插入排序结合使用，先使用归并排序，
   当链表里面元素数量个数小于一定值时便使用插入排序。
    
	@param	SINGLELIST *pSingleList - 要排序的链表指针	
	@param	COMPAREFUNC CompareFunc - 链表节点比较函数	
	@param	UINT uInsertSortCount - 使用插入排序时的链表节点个数，当链表节点
                                    个数小于这个值时会使用插入排序算法
                                    
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功。	
*/
INT SingleList_MergeSort(SINGLELIST *pSingleList, 
                         COMPAREFUNC CompareFunc, 
                         UINT uInsertSortCount)
{
   	SINGLELIST	 *pSecondList;

    /* 参数校验 */
	if ( pSingleList == NULL || CompareFunc == NULL )
	{
		return CAPI_FAILED;
	}

    if ( pSingleList->uCount < 2 )
    {
        /* 如果节点个数少于两个，认为是已经排好序的 */
        return CAPI_SUCCESS;
    }

    /*
     * 如果链表节点个数小于给定的做插入排序的个数，直接使用插入排序。 
	 */
    if ( pSingleList->uCount <= uInsertSortCount )
	{
		(void)SingleList_InsertSort( pSingleList, CompareFunc );
	}
    else
	{
		/* 将链表分成两半 */
        pSecondList = SingleList_Split(pSingleList, (pSingleList->uCount) / 2 );
		
        /* 对分完后的第一个链表进行递归调用归并排序 */
        (void)SingleList_MergeSort( pSingleList, CompareFunc, uInsertSortCount );
		
        /* 对分完后的第二个链表进行递归调用归并排序 */
		(void)SingleList_MergeSort( pSecondList, CompareFunc, uInsertSortCount );

		/* 将排好序的两个链表合成一个. */
		(void)SingleList_Merge( pSingleList, pSecondList, CompareFunc );
	}

    return CAPI_SUCCESS; 
}

/**	对链表的数据的第uKeyIndex位上的元素进行分类，
    依照它们的大小放入对应的箱子中

	@param	SINGLELIST *pSingleList - 单向链表指针	
	@param	UINT       uRadix - 基数排序的基数，与具体数据类型有关，
                                一般来讲整数的基数为16，字符串的基数最大为255。
	@param	UINT       uKeyIndex - 第多少位	
	@param	SINGLENODE **ppHead - 用来记录头指针的箱子	
	@param	SINGLENODE **ppTail - 记录箱子的尾指针	
	@param	GETKEYFUNC GetKeyFunc - 获取数据的第uKeyIndex位上的元素值	
	@return	void - 无	
*/
static void SingleList_Distribute(SINGLELIST *pSingleList,
                          UINT       uRadix,
                          UINT       uKeyIndex,
                          SINGLENODE **ppHead,
                          SINGLENODE **ppTail,
                          GETKEYFUNC GetKeyFunc )
{
    SINGLENODE   *pNode;
    UINT         i;

    /* 初始化子表 */
    for ( i = 0; i < uRadix; i++ )
    {
        ppHead[i] = NULL;
        ppTail[i] = NULL;
    }

    pNode = pSingleList->pHead;

    while ( pNode != NULL )
    {
        UINT uRadixIndex = (*GetKeyFunc)(pNode->pData, uKeyIndex);
         
        if ( ppHead[uRadixIndex] == NULL )
        {
           ppHead[uRadixIndex] = pNode;
           ppTail[uRadixIndex] = pNode;
           pNode = pNode->pNext;
           ppTail[uRadixIndex]->pNext = NULL;
        }
        else
        {
            ppTail[uRadixIndex]->pNext = pNode;
            ppTail[uRadixIndex] = ppTail[uRadixIndex]->pNext;
            pNode = pNode->pNext;
            ppTail[uRadixIndex]->pNext = NULL;
        }
    }
}


/**	对基数排序中分好类的箱子进行收集操作，将箱子重新连成一条链表

	@param	SINGLELIST *pSingleList - 单向链表指针	
	@param	UINT        uRadix - 基数	
	@param	SINGLENODE **ppHead - 用来记录头指针的箱子	
	@param	SINGLENODE **ppTail - 记录箱子的尾指针	
	@return	void - 无。
*/
static void SingleList_Collect(SINGLELIST *pSingleList,
                       UINT        uRadix,
                       SINGLENODE **ppHead,
                       SINGLENODE **ppTail )
{
    SINGLENODE  *pHead;
    SINGLENODE  *pTail;
    UINT        uRadixIndex;

    /* 查早第1个非空子表 */
    uRadixIndex = 0;
    while ( uRadixIndex < uRadix )
    {
        if ( ppHead[uRadixIndex] == NULL )
        {
            uRadixIndex++;
            continue;
        }
        else
        {
            break;
        }
    }

    if ( uRadixIndex == uRadix )
    {
        /* 没有找到非空子表 */
        return ;
    }

    pHead = ppHead[uRadixIndex];
    pTail = ppTail[uRadixIndex];

    while ( uRadixIndex < uRadix - 1 )
    {
        /* 继续查找下一个非空子表 */
        ++uRadixIndex;
        if ( ppHead[uRadixIndex] == NULL )
        {
            continue;
        }

        if ( uRadixIndex < uRadix )
        {
            /* 找到了非空子表，需要把它和前一个非空子表链接起来 */
            pTail->pNext = ppHead[uRadixIndex];
            pTail = ppTail[uRadixIndex];
        }
    }

    pSingleList->pHead = pHead;
    pSingleList->pTail = pTail;
}

/**	单向链表的基数排序函数

	@param	SINGLELIST *pSingleList - 单向链表指针	
	@param	UINT uRadix - 基数，字符串如果以单字节为基的话基数为256	
                                整数以10进制计算基数的话，基数为10
	@param	UINT uMaxKeyLen - 关键词的长度，字符串以字节为单位则长度为字符串
                              本身最大可能长度，如果32位整数以16进制为单位的
                              话，则最大长度为8
	@param	GETKEYFUNC GetKeyFunc - 关键词获取回调函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功。	
*/
INT SingleList_RadixSort( SINGLELIST *pSingleList,
                          UINT uRadix,
                          UINT uMaxKeyLen,
                          GETKEYFUNC GetKeyFunc )
{
    SINGLENODE  **ppHead;  /* 用来记录各个箱子头节点的双指针 */
    SINGLENODE  **ppTail;  /* 用来记录各个箱子尾节点的双指针 */
    UINT        i;         /* 临时循环变量 */

    /* 给箱子申请内存 */
    ppHead = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
    if ( ppHead == NULL )
    {
        return CAPI_FAILED;
    }
    ppTail = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
    if ( ppTail == NULL )
    {
        free(ppHead);
        return CAPI_FAILED;
    }

    /* 按顺序对关键字的第i位进行分配和收集操作 */
    for ( i = 0; i < uMaxKeyLen; i++ )
    {
        SingleList_Distribute(pSingleList, uRadix, i, ppHead, ppTail, GetKeyFunc);

        SingleList_Collect(pSingleList, uRadix, ppHead, ppTail);
    }

    /* 释放分配的箱子 */
    free( ppHead );
    free( ppTail );

    return CAPI_SUCCESS;
}


/**	获取字符串的第uKeyIndex位置上的字符
    获取到的字符会自动转换成大写形式。

	@param	void *pszData - 指向字符串的指针	
	@param	UINT uKeyIndex - 表示第多少位，位置是从后向前算
	@return	UINT - 返回第uKeyIndex位的字符值	
*/
UINT GetStrKeyNoCase( void *pszData, UINT uKeyIndex )
{
	char *psz = (char *)pszData;
    UINT    uKey;
		
    if ( psz == NULL || (*psz) == '\0' )
    {
        return 0;
    }
    if ( uKeyIndex >= strlen(psz) )
    {
		return 0;
    }

    uKey = strlen(psz) - (uKeyIndex + 1);

    if( psz[uKey] >= 'a' && psz[uKey] <= 'z' )
    {
	    uKey = (UINT)(unsigned char)((psz[uKey] - 'a') + 'A');
    }
    else
    {
		uKey = (UINT)(unsigned char)(psz[uKey]);
    }

    return uKey;
}

/**	获取字符串的第uKeyIndex位置上的字符
    获取到的字符区分大小写，保持原值不变。

	@param	void *pszData - 指向字符串的指针	
	@param	UINT uKeyIndex - 表示第多少位，位置是从后向前算
	@return	UINT - 返回第uKeyIndex位的字符值	
*/
UINT GetStrKey( void *pszData, UINT uKeyIndex )
{
	char *psz = (char *)pszData;
    UINT    uKey;
		
    if ( psz == NULL || (*psz) == '\0' )
    {
        return 0;
    }
    if ( uKeyIndex >= strlen(psz) )
    {
		return 0;
    }

    uKey = strlen(psz) - (uKeyIndex + 1);

    uKey = (UINT)(unsigned char)(psz[uKey]);

	return uKey;
}

/**	获取整数的第uKeyIndex位数字，以16进制为单位
    因此获取到的数字最大为0xf，对整数作基数排序时，
    基数为16，最大关键词长度为4

	@param	void *pData - 整数值	
	@param	UINT uKeyIndex - 整数的第多少位,16进制下最大8位	
	@return	UINT - 返回第uKeyIndex位的数字	
*/
UINT GetIntKey( void *pData, UINT uKeyIndex )
{
    UINT    uData;

    if ( uKeyIndex > 8 )
    {
        return 0;
    }

    uData = (UINT)pData;

    uData = (uData >> ((uKeyIndex - 1)*4)) & 0xf;

    return uData;   
}


/**	获取整数的第uKeyIndex位数字，以65536进制为单位
因此获取到的数字最大为0xffff，作基数排序时，
基数为65536，最大关键词长度为16

@param	void *pData - 整数值	
@param	UINT uKeyIndex - 65536进制整数的第多少位，32位整数最大位数为2	
@return	UINT - 返回第uKeyIndex位的数字	
*/
UINT GetWordKey( UINT uData, UINT uKeyIndex )
{
    unsigned short wData;
    if ( uKeyIndex > 2 )
    {
        return 0;
    }

    wData = (uData >> ((uKeyIndex - 1)*16)) & 0xffff;

    return wData;   
}
