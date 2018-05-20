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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "CapiGlobal.h"
#include "DoubleList.h"
#include "Tree.h"

/**	���Ĵ�������

	@return	TREE * - �´���������ָ��	
*/
TREE * Tree_Create()
{
    TREE *    pNewTree;

    pNewTree = (TREE *)malloc(sizeof(struct TREE_st));
    if ( pNewTree == NULL )
    {
        return NULL;
    }

    /* ����Ҷ���б� */
    pNewTree->pLeafList = DoubleList_Create();
    if ( pNewTree->pLeafList == NULL )
    {
        free( pNewTree );
        return NULL;
    }

    /* ���������б� */
    pNewTree->pSubTreeList = DoubleList_Create();
    if ( pNewTree->pSubTreeList == NULL )
    {
        DoubleList_Destroy( pNewTree->pLeafList, NULL );
        free( pNewTree );
        return NULL;
    }

    pNewTree->pProperties = NULL;

    return pNewTree;
}


/**	�����ͷź��������ú�������㷨�����ͷ�

	@param TREE * pTree - Ҫ�ͷŵ�����ָ��	
	@param DESTROYFUNC LeafDestroyFunc - Ҷ�ӽڵ�������ͷź���	
	@param DESTROYFUNC PropDestroyFunc - �����ͷź���	
	@return	void - ��	
*/
void Tree_Destroy( TREE * pTree, 
                   DESTROYFUNC LeafDestroyFunc, 
                   DESTROYFUNC PropDestroyFunc )
{
    DOUBLELIST * pList;
    void *pData;

    if ( pTree == NULL || LeafDestroyFunc == NULL 
        || PropDestroyFunc == NULL )
    {
        return ;
    }

    pList = pTree->pSubTreeList;
	if ( pList == NULL ) 
    {
		return;
	}

    DoubleList_EnumBegin(pList);

    /* ��������������ݹ����Tree_Destroy�����������ͷ� */
    while ( (pData = DoubleList_EnumNext(pList)) != NULL )
    {
        Tree_Destroy((TREE *)pData, LeafDestroyFunc, PropDestroyFunc);
    }

    /* �ͷ����� */
    if ( pTree->pProperties != NULL )
    {
        PropDestroyFunc( pTree->pProperties );
    }
    
    /* �ͷ�Ҷ���б� */
    DoubleList_Destroy( pTree->pLeafList, LeafDestroyFunc );

    /* �ͷ����Ľṹ�� */
    free( pTree );
}


/**	�������Ҷ�ӽڵ�

	@param	TREE * pTree - Ҫ���Ҷ�ӵ�����ָ��	
	@param	void * pLeafData - Ҫ��ӵ�Ҷ�ӵ�����	
	@return	INT - CAPI_FAILE��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT Tree_AddLeaf(TREE * pTree, void * pLeafData)
{
    if ( pTree == NULL )
    {
        return CAPI_FAILED;
    }
    return DoubleList_InsertTail( pTree->pLeafList, pLeafData );
}


/**	����ɾ��Ҷ�Ӻ���

	@param	TREE * pTree - Ҫɾ��Ҷ�ӵ���ָ��	
	@param	void * pLeafData - Ҷ��ƥ�����ݵ�ָ��	
    @param	COMPAREFUNC CompareFunc - ���ݱȽϻص�����	
	@param	DESTROYFUNC LeafDestroyFunc - Ҷ�������ͷź���	
	@return	INT - CAPI_FAILE��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT Tree_RemoveLeaf(TREE * pTree, void * pLeafData, 
                    COMPAREFUNC CompareFunc,
                    DESTROYFUNC LeafDestroyFunc)
{
    if ( pTree == NULL )
    {
        return CAPI_FAILED;
    }

    return DoubleList_Delete(pTree->pLeafList, pLeafData, CompareFunc, LeafDestroyFunc);
}


/**	�����������Ժ���

	@param	TREE * pTree - ��ָ��	
	@param  void * pProperties - ����ָ��	
	@param  DESTROYFUNC PropDestroyFunc - �����ͷź���	
	@return	INT - CAPI_FAILE��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT Tree_SetProperties( TREE * pTree, 
                        void * pProperties, 
                        DESTROYFUNC PropDestroyFunc)
{
    if ( pTree == NULL )
    {
        return CAPI_FAILED;
    }

    /* ���ԭ���Ѿ���������������Ҫ�����ͷ��Է�ֹ�ڴ�й© */
    if ( pTree->pProperties != NULL )
    {
        PropDestroyFunc( pTree->pProperties );
    }

    pTree->pProperties = pProperties;

    return CAPI_SUCCESS;
}


/**	���Ļ�ȡ���Ժ���

	@param	TREE * pTree - ��ָ��	
	@return	void * - �ɹ���������ָ�룬����NULL��ʾʧ�ܻ�����ΪNULL	
*/
void * Tree_GetProperties(TREE * pTree)
{
    if ( pTree == NULL )
    {
        return NULL;
    }

    return pTree->pProperties;
}


/**	������������ĺ���

	@param	TREE * pTree - ��ָ��	
	@param	TREE * pSubTree -����ָ��	
	@return	INT - CAPI_FAILE��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT Tree_AddSubTree(TREE * pTree, TREE * pSubTree)
{
    if ( pTree == NULL || pSubTree == NULL )
    {
        return CAPI_FAILED;
    }

    return DoubleList_InsertTail( pTree->pSubTreeList, (void *)pSubTree );
}


/**	����ɾ����������

	@param	TREE * pTree - ��ָ��	
	@param	TREE * pSubTree - ����ָ��	
	@param	DESTROYFUNC LeafDestroyFunc - Ҷ���ͷź���	
	@param	DESTROYFUNC PropDestroyFunc - �����ͷź���	
	@return	INT - CAPI_FAILE��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT Tree_RemoveSubTree(TREE * pTree, 
                   TREE * pSubTree, 
                   DESTROYFUNC LeafDestroyFunc,
                   DESTROYFUNC PropDestroyFunc )
{
    DOUBLELIST  *pList;
	DOUBLENODE	*pNode;

    if ( pTree == NULL || pSubTree == NULL || LeafDestroyFunc == NULL 
        || PropDestroyFunc == NULL)
    {
        return CAPI_FAILED;
    }

    pList = pTree->pSubTreeList;
	if ( pList == NULL ) 
    {
		return CAPI_FAILED;
	}

    /* ��������������Ҷ�Ӧ����������ɾ�� */
	DoubleList_EnumBegin(pList);
	while ( (pNode = DoubleList_EnumNode(pList) ) != NULL ) 
    {
        if ( (TREE *)(pNode->pData) == pSubTree )
        {
            pNode = DoubleList_PopNode(pList, pNode);
            Tree_Destroy( (TREE *)(pNode->pData), LeafDestroyFunc,
                           PropDestroyFunc );
            free(pNode);
            break;
        }
	}

	return CAPI_SUCCESS;    
}


/**	���Ŀ���������������������㷨���п���

	@param	TREE * pTree - ��ָ��	
	@param	COPYFUNC LeafCopyFunc - Ҷ�ӿ�������	
	@param	COPYFUNC PropCopyFunc - ���Կ�������	
	@return	TREE * - ��������µ�����ָ��	
*/
TREE * Tree_Copy( TREE * pTree,
                  COPYFUNC LeafCopyFunc, 
                  COPYFUNC PropCopyFunc)
{
    TREE       *  pNewTree;
	DOUBLELIST *  pNewList;
    DOUBLELIST *  pList;
    void       *  pData;

    if ( pTree == NULL || LeafCopyFunc == NULL || PropCopyFunc == NULL )
    {
        return NULL;
    }

    pNewList = DoubleList_Create();
    if ( pNewList == NULL )
    {
        return NULL;
    }

    pNewTree = Tree_Create();
    if ( pNewTree == NULL )
    {
        DoubleList_Destroy(pNewList, NULL);
        return NULL;
    }
    
    /* ����Ҷ���б� */
    pNewTree->pLeafList = DoubleList_Copy( pTree->pLeafList, LeafCopyFunc );

    /* �������� */
    pNewTree->pProperties = (*PropCopyFunc)( pTree->pProperties );

    pList = pTree->pSubTreeList;

    /* ������������б����Tree_Copy�ݹ鿽�������б� */
    DoubleList_EnumBegin(pList);

    while( (pData = DoubleList_EnumNext(pList)) != NULL )
    {
        TREE *pSubTree = Tree_Copy((TREE*)pData, LeafCopyFunc, PropCopyFunc);
        DoubleList_InsertTail(pNewList, (void *)pSubTree);
    }
   
    return pNewTree;
}

/**	���İ����Բ�����������

	@param	TREE  * pTree -��ָ��	
	@param	void  * pProperties - Ҫ���ҵ�����ָ��	
	@param	COMPAREFUNC   CompareFunc - ���ԱȽϺ���	
	@return	TREE * - �ɹ����ز��ҵ�������ָ�룬ʧ�ܷ���NULL	
*/
TREE * Tree_FindSubTreeByProp( TREE  * pTree,
              void  * pProperties,
              COMPAREFUNC   CompareFunc )
{
    TREE *              pSubTree;
    DOUBLELIST  *       pList;
    void        *       pData;

    if ( !pTree )
    {
        return NULL;
    }
    
    /* ����Ѿ�������������ͬ�����ص�ǰ����ָ�� */
    if ( (*CompareFunc)( pProperties, pTree->pProperties )  == 0 )
    {
        return pTree;
    }

    pList = pTree->pSubTreeList;

    DoubleList_EnumBegin(pList);

    /* ��������������ݹ���ñ������������н��в��� */
    while ( (pData = DoubleList_EnumNext(pList)) != NULL )
    {
        pSubTree = Tree_FindSubTreeByProp((TREE *)pData, pProperties, 
                                          CompareFunc);
        if ( pSubTree != NULL )
        {
            return pSubTree;
        }
    }
  
    return NULL;
}
