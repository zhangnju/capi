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
#include <windows.h>
#endif
#include "CapiGlobal.h"

#include "MTask.h"
#include "DoubleList.h"
#include "MTList.h"


/**	多任务链表的创建函数

	@param	void - 无	
	@return	MTLIST * - 多任务链表指针	
*/
MTLIST *MTList_Create(void)
{
    MTLIST  *pList;
    pList = (MTLIST *)malloc(sizeof(MTLIST));
    if ( pList != NULL )
    {
        pList->pList = DoubleList_Create();
        if ( pList->pList != NULL )
        {
            pList->pMTask = MTask_Create();
            if ( pList->pMTask == NULL )
            {
                free(pList->pList);
                free(pList);
                pList = NULL;
            }      
        }
        else
        {
            free(pList);
            pList = NULL;
        }
    }
    return pList;
}


/**	多任务链表的释放函数

	@param	MTLIST *pList - 多任务链表指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
*/
void MTList_Destroy(MTLIST *pList, DESTROYFUNC DestroyFunc)
{
    if ( pList == NULL )
    {
        return;
    }

    MTask_Destroy(pList->pMTask);

    DoubleList_Destroy(pList->pList, DestroyFunc);

    free( pList );
}

/**	获取退出标志

	@param	MTLIST *pList - 多任务链表指针	
    @return	UINT - 退出标志 CAPI_EXIT_TASK表示可以退出任务，
            	   CAPI_NOT_EXIT_TASK表示不可以退出任务
	
*/
UINT MTList_GetExitFlag(MTLIST *pList)
{
    return MTask_GetExitFlag(pList->pMTask);
}


/**	设置退出标志函数

	@param	MTLIST *pList - 多任务链表指针	
    @param	UINT uExitFlag - 退出标志 CAPI_EXIT_TASK表示可以退出任务，
            	   CAPI_NOT_EXIT_TASK表示不可以退出任务	
	@return	void - 无	
*/
void MTList_SetExitFlag(MTLIST *pList, UINT uExitFlag)
{
    MTask_SetExitFlag(pList->pMTask, uExitFlag);
}

/**	进入任务函数，在进入任务循环前要先调用这个函数

	@param	MTLIST *pList - 多任务链表指针	
	@return	void - 无	
*/
void MTList_EnterTask(MTLIST *pList)
{
    MTask_EnterTask(pList->pMTask);
}

/**	离开任务函数

	@param	MTLIST *pList - 多任务链表指针	
	@return	void - 无	
*/
void MTList_LeaveTask(MTLIST *pList)
{
    MTask_LeaveTask(pList->pMTask);
}

/**	插入数据到多任务链表头部的函数

	@param	MTLIST *pList - 多任务链表指针	
	@param	void *pData - 要插入的数据	
	@return	INT - 失败返回CAPI_FAILED，成功返回CAPI_SUCCESS		
*/
INT MTList_InsertHead(MTLIST *pList, void *pData)
{
    INT         nRet = CAPI_FAILED;

    if ( pList == NULL )
    {
        return CAPI_FAILED;
    }
    
    MTask_Lock( pList->pMTask );

    nRet = DoubleList_InsertHead(pList->pList, pData);
    
    MTask_Unlock( pList->pMTask );

    return nRet;
}

/**	删除多任务链表头部节点函数

	@param	MTLIST *pList - 多任务链表指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	INT -  失败返回CAPI_FAILED，成功返回CAPI_SUCCESS		
*/
INT MTList_DeleteHead(MTLIST *pList, DESTROYFUNC DestroyFunc)
{
    void  *pData;

    if ( pList == NULL )
    {
        return CAPI_FAILED;
    }
    
    MTask_Lock( pList->pMTask );

    pData = DoubleList_PopHead(pList->pList);

    MTask_Unlock( pList->pMTask );

    if ( DestroyFunc != NULL && pData != NULL )
    {
        (*DestroyFunc)(pData);
    }

    return CAPI_SUCCESS;

}

/**	插入数据到多任务链表尾部的函数

	@param	MTLIST *pList - 多任务链表指针	
	@param	void *pData - 要插入到尾部的数据	
	@return	INT - 	失败返回CAPI_FAILED，成功返回CAPI_SUCCESS	
*/
INT MTList_InsertTail(MTLIST *pList, void *pData)
{
    INT nRet;

    MTask_Lock( pList->pMTask );

    nRet = DoubleList_InsertTail(pList->pList, pData);
    
    MTask_Unlock( pList->pMTask );
    
    return nRet;
}


/**	弹出多任务链表头部节点的函数

	@param	MTLIST *pList - 多任务链表指针	
	@return	void * - 弹出的头部节点数据	
*/
void * MTList_PopHead(MTLIST *pList)
{
    void * pData;
    
    MTask_Lock( pList->pMTask );
    
    pData = DoubleList_PopHead(pList->pList);
    
    MTask_Unlock( pList->pMTask );
    
    return pData;
}

/**	弹出多任务链表尾部节点的函数

	@param	MTLIST *pList - 多任务链表指针	
	@return	void * - 弹出的尾部节点数据	
*/
void *  MTList_PopTail(MTLIST *pList)
{
    void * pData;
    
    MTask_Lock( pList->pMTask );
    
    pData = DoubleList_PopTail(pList->pList);
    
    MTask_Unlock( pList->pMTask );
    
    return pData;
}


/**	多任务链表的遍历初始化函数

	@param	MTLIST *pList - 多任务链表指针	
	@return	void - 无	
*/
void MTList_EnumBegin(MTLIST *pList)
{
    if ( pList == NULL )
    {
        return; 
    }

    MTask_Lock( pList->pMTask );

    DoubleList_EnumBegin(pList->pList);

    MTask_Unlock( pList->pMTask );

    return;
}


/**	多任务链表的遍历下一个节点函数

	@param	MTLIST *pList - 多任务链表指针	
	@param	VISITFUNC VisitFunc - 数据访问函数	
	@return	INT - 失败返回CAPI_FAILED，成功返回CAPI_SUCCESS		
*/
INT MTList_EnumNext(MTLIST *pList, VISITFUNC VisitFunc)
{
    void    *pData;
    INT nRet = CAPI_FAILED;

    MTask_Lock( pList->pMTask );
    
    pData = DoubleList_EnumNext(pList->pList);

    if ( VisitFunc != NULL )
    {
        nRet = (*VisitFunc)(pData);
    }

    MTask_Unlock( pList->pMTask );

    return nRet;
}


/**	多任务链表的枚举下一个节点并将节点拷贝出来执行的函数
    先访问节点，将节点拷贝出来，解锁后再调用VisitFunc来访问数据。

	@param	MTLIST *pList - 多任务链表指针	
	@param	VISITFUNC VisitFunc - 节点数据访问回调函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@param	COPYFUNC CopyFunc - 数据拷贝函数	
	@return	INT - 取决于VisitFunc的返回值。	
*/
INT MTList_EnumNextCopy(MTLIST *pList, VISITFUNC VisitFunc,
                        DESTROYFUNC DestroyFunc, COPYFUNC CopyFunc)
{
    void    *pData;
    INT nRet = CAPI_FAILED;

    MTask_Lock( pList->pMTask );
    
    pData = DoubleList_EnumNext(pList->pList);

    pData = (*CopyFunc)( pData );

    MTask_Unlock( pList->pMTask );
    
    nRet = (*VisitFunc)( pData );

    if ( DestroyFunc != NULL && pData != NULL )
    {
        (*DestroyFunc)(pData);
    }

    return nRet;
}

/**	多任务链表的枚举并且删除节点函数
    先访问节点，将节点弹出来，再解锁，然后再用VisitFunc访问节点，最后将弹出的
    节点释放掉

	@param	MTLIST *pList - 多任务链表指针	
	@param	VISITFUNC VisitFunc - 数据访问函数	
	@param	DESTROYFUNC DestroyFunc - 数据释放函数	
	@return	INT - 取决于VisitFunc的返回值。		
*/
INT MTList_EnumNextAndDelete(MTLIST *pList, VISITFUNC VisitFunc,
                             DESTROYFUNC DestroyFunc)
{
    DOUBLENODE *pNode;
    INT nRet = CAPI_FAILED;
    
    MTask_Lock( pList->pMTask );
    
    pNode = DoubleList_EnumNode(pList->pList);

    DoubleList_PopNode(pList->pList, pNode);
    
    MTask_Unlock( pList->pMTask );
    
    nRet = (*VisitFunc)( pNode->pData );
    
    if ( DestroyFunc != NULL && pNode->pData != NULL )
    {
        (*DestroyFunc)(pNode->pData);
    }
    free(pNode);
    
    return nRet;
}

/**	多任务链表的遍历下几个节点函数

        @param	MTLIST *pList - 多任务链表指针	
        @param	VISITFUNC VisitFunc - 数据访问函数	
        @param	INT nCount - 要访问的节点个数	
        @return	INT - 失败返回CAPI_FAILED，成功返回CAPI_SUCCESS		
*/
void MTList_EnumNextSeveral(MTLIST *pList, VISITFUNC VisitFunc, INT nCount)
{
    void    *pData;
    int     i;

    MTask_Lock( pList->pMTask );
    for ( i = 0; i < nCount; i++ )
    {
        pData = DoubleList_EnumNext(pList->pList);
        
        if ( VisitFunc != NULL )
        {
            (*VisitFunc)(pData);
        }
    }
    MTask_Unlock( pList->pMTask );
}