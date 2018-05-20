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
#include "BinTree.h"
#include "RBTree.h"
#include "MTree.h"

typedef struct MTREE_st {
    RBTREE      *pRBTree;
    RBTREENODE  *pCursor;
    MTASK       *pMTask;
} MTREE;



/**	���������Ĵ�������

	@param	void - ��	
	@return	HANDLE - �ɹ����ض��������ľ����ʧ�ܷ���NULL	
*/
HANDLE MTree_Create(void)
{
    MTREE   *pTree;

    pTree = (MTREE *)malloc(sizeof(MTREE));
    if ( pTree != NULL )
    {
        pTree->pMTask = MTask_Create();
        if ( pTree->pMTask != NULL )
        {
            pTree->pRBTree = RBTree_Create();
            if ( pTree->pRBTree != NULL )
            {
                pTree->pCursor = NULL;
            }
            else
            {
                free( pTree->pMTask);
                free( pTree );
                pTree = NULL;
            }
        }
        else
        {
            free( pTree );
            pTree = NULL;
        }
    }
    return (HANDLE)pTree;
}


/**	�����������ͷź���

	@param	HANDLE hTree - ���������ľ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
*/
void MTree_Destroy(HANDLE hTree, DESTROYFUNC DestroyFunc)
{
    MTREE   *pTree;

    pTree = (MTREE *)hTree;
    if ( pTree == NULL )
    {
        return;
    }

    MTask_Destroy(pTree->pMTask);
#ifdef _DEBUG
    BinTree_CheckParent(pTree->pRBTree->pRoot);
#endif

    RBTree_Destroy( pTree->pRBTree, DestroyFunc );

    free( pTree );
}


/**	���������������˳���־����

	@param	HANDLE hTree - �����������	
    @param	UINT uExitFlag - �˳���־��MTASK_NO_EXIT��ʾ���˳�����
                    MTASK_EXIT��ʾ�˳�����	
	@return	void - ��	
*/
void MTree_SetExitFlag(HANDLE hTree, UINT uExitFlag )
{
    MTREE   *pTree;

    pTree = (MTREE *)hTree;
    
    MTask_SetExitFlag(pTree->pMTask, uExitFlag);
}


/**	���������Ļ�ȡ�˳���־����

	@param	HANDLE hTree - �����������	
    @return	UINT - ���ػ�ȡ�����˳���־��MTASK_NO_EXIT��ʾ���˳�����
                    MTASK_EXIT��ʾ�˳�����
	
*/
UINT MTree_GetExitFlag(HANDLE hTree )
{
    MTREE   *pTree;

    pTree = (MTREE *)hTree;
    
    return MTask_GetExitFlag(pTree->pMTask);
}


/**	���������Ľ���������

	@param	HANDLE hTree - ���������ľ��	
	@return	void - ��	
*/
void MTree_EnterTask(HANDLE hTree )
{
    MTREE   *pTree;

    pTree = (MTREE *)hTree;

    MTask_EnterTask(pTree->pMTask);
}


/**	�����������뿪������

	@param	HANDLE hTree - ���������ľ��	
	@return	void - ��	
*/
void MTree_LeaveTask(HANDLE hTree )
{
    MTREE   *pTree;

    pTree = (MTREE *)hTree;

    MTask_LeaveTask(pTree->pMTask);
}



/**	�������������ݲ��뺯��

	@param	HANDLE hTree - ���������ľ��	
	@param	void *pData - Ҫ���������ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	INT - ��RBTree_Insert()�����ķ���ֵ��ͬ	
*/
INT MTree_Insert(HANDLE hTree, void *pData, COMPAREFUNC CompareFunc)
{
    MTREE   *pTree;
    INT     nRet;

    pTree = (MTREE *)hTree;
    
    MTask_Lock(pTree->pMTask);

    nRet = RBTree_Insert(pTree->pRBTree, pData, CompareFunc);

    MTask_Unlock(pTree->pMTask);

    return nRet;
}


/**	����������ɾ�����ݺ���

	@param	HANDLE hTree - ���������ľ��	
	@param	void *pData - Ҫɾ��������ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	INT - ͬRBTree_Delete()����	
*/
INT MTree_Delete(HANDLE hTree, void *pData, COMPAREFUNC CompareFunc, 
                 DESTROYFUNC DestroyFunc)
{
    MTREE   *pTree;
    INT     nRet;

    pTree = (MTREE *)hTree;

    MTask_Lock(pTree->pMTask);

    nRet = RBTree_Delete(pTree->pRBTree, pData, CompareFunc, DestroyFunc);

    MTask_Unlock(pTree->pMTask);

    return nRet;
}


/**	����������ö�ٿ�ʼ����

	@param	HANDLE hTree - �����������	
	@return	void - ��	
*/
void MTree_EnumBegin(HANDLE hTree)
{
    MTREE   *pTree;

    pTree = (MTREE *)hTree;
    MTask_Lock(pTree->pMTask);

    RBTree_EnumBegin(pTree->pRBTree);

    MTask_Unlock(pTree->pMTask);

}


/**	����������ö����һ���������ڵ㺯��

	@param	HANDLE hTree - ���������ľ��	
	@param	COPYFUNC CopyFunc - ���ݿ����ص�����	
	@return	void * - ���ؿ���������	
*/
void * MTree_EnumNextCopy(HANDLE hTree, COPYFUNC CopyFunc)
{
    MTREE   *pTree;
    void    *pData;
    void    *pRetData;

    pTree = (MTREE *)hTree;

    MTask_Lock(pTree->pMTask);

    pData = RBTree_EnumNext(pTree->pRBTree);

    pRetData = (*CopyFunc)(pData);

    MTask_Unlock(pTree->pMTask);

    return pRetData;
}

