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


/**	����������Ĵ�������

	@param	void - ��	
	@return	MTLIST * - ����������ָ��	
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


/**	������������ͷź���

	@param	MTLIST *pList - ����������ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
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

/**	��ȡ�˳���־

	@param	MTLIST *pList - ����������ָ��	
    @return	UINT - �˳���־ CAPI_EXIT_TASK��ʾ�����˳�����
            	   CAPI_NOT_EXIT_TASK��ʾ�������˳�����
	
*/
UINT MTList_GetExitFlag(MTLIST *pList)
{
    return MTask_GetExitFlag(pList->pMTask);
}


/**	�����˳���־����

	@param	MTLIST *pList - ����������ָ��	
    @param	UINT uExitFlag - �˳���־ CAPI_EXIT_TASK��ʾ�����˳�����
            	   CAPI_NOT_EXIT_TASK��ʾ�������˳�����	
	@return	void - ��	
*/
void MTList_SetExitFlag(MTLIST *pList, UINT uExitFlag)
{
    MTask_SetExitFlag(pList->pMTask, uExitFlag);
}

/**	�������������ڽ�������ѭ��ǰҪ�ȵ����������

	@param	MTLIST *pList - ����������ָ��	
	@return	void - ��	
*/
void MTList_EnterTask(MTLIST *pList)
{
    MTask_EnterTask(pList->pMTask);
}

/**	�뿪������

	@param	MTLIST *pList - ����������ָ��	
	@return	void - ��	
*/
void MTList_LeaveTask(MTLIST *pList)
{
    MTask_LeaveTask(pList->pMTask);
}

/**	�������ݵ�����������ͷ���ĺ���

	@param	MTLIST *pList - ����������ָ��	
	@param	void *pData - Ҫ���������	
	@return	INT - ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS		
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

/**	ɾ������������ͷ���ڵ㺯��

	@param	MTLIST *pList - ����������ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	INT -  ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS		
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

/**	�������ݵ�����������β���ĺ���

	@param	MTLIST *pList - ����������ָ��	
	@param	void *pData - Ҫ���뵽β��������	
	@return	INT - 	ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS	
*/
INT MTList_InsertTail(MTLIST *pList, void *pData)
{
    INT nRet;

    MTask_Lock( pList->pMTask );

    nRet = DoubleList_InsertTail(pList->pList, pData);
    
    MTask_Unlock( pList->pMTask );
    
    return nRet;
}


/**	��������������ͷ���ڵ�ĺ���

	@param	MTLIST *pList - ����������ָ��	
	@return	void * - ������ͷ���ڵ�����	
*/
void * MTList_PopHead(MTLIST *pList)
{
    void * pData;
    
    MTask_Lock( pList->pMTask );
    
    pData = DoubleList_PopHead(pList->pList);
    
    MTask_Unlock( pList->pMTask );
    
    return pData;
}

/**	��������������β���ڵ�ĺ���

	@param	MTLIST *pList - ����������ָ��	
	@return	void * - ������β���ڵ�����	
*/
void *  MTList_PopTail(MTLIST *pList)
{
    void * pData;
    
    MTask_Lock( pList->pMTask );
    
    pData = DoubleList_PopTail(pList->pList);
    
    MTask_Unlock( pList->pMTask );
    
    return pData;
}


/**	����������ı�����ʼ������

	@param	MTLIST *pList - ����������ָ��	
	@return	void - ��	
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


/**	����������ı�����һ���ڵ㺯��

	@param	MTLIST *pList - ����������ָ��	
	@param	VISITFUNC VisitFunc - ���ݷ��ʺ���	
	@return	INT - ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS		
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


/**	�����������ö����һ���ڵ㲢���ڵ㿽������ִ�еĺ���
    �ȷ��ʽڵ㣬���ڵ㿽���������������ٵ���VisitFunc���������ݡ�

	@param	MTLIST *pList - ����������ָ��	
	@param	VISITFUNC VisitFunc - �ڵ����ݷ��ʻص�����	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@param	COPYFUNC CopyFunc - ���ݿ�������	
	@return	INT - ȡ����VisitFunc�ķ���ֵ��	
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

/**	�����������ö�ٲ���ɾ���ڵ㺯��
    �ȷ��ʽڵ㣬���ڵ㵯�������ٽ�����Ȼ������VisitFunc���ʽڵ㣬��󽫵�����
    �ڵ��ͷŵ�

	@param	MTLIST *pList - ����������ָ��	
	@param	VISITFUNC VisitFunc - ���ݷ��ʺ���	
	@param	DESTROYFUNC DestroyFunc - �����ͷź���	
	@return	INT - ȡ����VisitFunc�ķ���ֵ��		
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

/**	����������ı����¼����ڵ㺯��

        @param	MTLIST *pList - ����������ָ��	
        @param	VISITFUNC VisitFunc - ���ݷ��ʺ���	
        @param	INT nCount - Ҫ���ʵĽڵ����	
        @return	INT - ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS		
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