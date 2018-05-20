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

/**	树的创建函数

	@return	TREE * - 新创建的树的指针	
*/
TREE * Tree_Create()
{
    TREE *    pNewTree;

    pNewTree = (TREE *)malloc(sizeof(struct TREE_st));
    if ( pNewTree == NULL )
    {
        return NULL;
    }

    /* 创建叶子列表 */
    pNewTree->pLeafList = DoubleList_Create();
    if ( pNewTree->pLeafList == NULL )
    {
        free( pNewTree );
        return NULL;
    }

    /* 创建子树列表 */
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


/**	树的释放函数，采用后序遍历算法进行释放

	@param TREE * pTree - 要释放的树的指针	
	@param DESTROYFUNC LeafDestroyFunc - 叶子节点的数据释放函数	
	@param DESTROYFUNC PropDestroyFunc - 属性释放函数	
	@return	void - 无	
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

    /* 逐个遍历子树，递归调用Tree_Destroy对子树进行释放 */
    while ( (pData = DoubleList_EnumNext(pList)) != NULL )
    {
        Tree_Destroy((TREE *)pData, LeafDestroyFunc, PropDestroyFunc);
    }

    /* 释放属性 */
    if ( pTree->pProperties != NULL )
    {
        PropDestroyFunc( pTree->pProperties );
    }
    
    /* 释放叶子列表 */
    DoubleList_Destroy( pTree->pLeafList, LeafDestroyFunc );

    /* 释放树的结构体 */
    free( pTree );
}


/**	给树添加叶子节点

	@param	TREE * pTree - 要添加叶子的树的指针	
	@param	void * pLeafData - 要添加的叶子的数据	
	@return	INT - CAPI_FAILE表示失败，CAPI_SUCCESS表示成功	
*/
INT Tree_AddLeaf(TREE * pTree, void * pLeafData)
{
    if ( pTree == NULL )
    {
        return CAPI_FAILED;
    }
    return DoubleList_InsertTail( pTree->pLeafList, pLeafData );
}


/**	树的删除叶子函数

	@param	TREE * pTree - 要删除叶子的树指针	
	@param	void * pLeafData - 叶子匹配数据的指针	
    @param	COMPAREFUNC CompareFunc - 数据比较回调函数	
	@param	DESTROYFUNC LeafDestroyFunc - 叶子数据释放函数	
	@return	INT - CAPI_FAILE表示失败，CAPI_SUCCESS表示成功	
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


/**	树的设置属性函数

	@param	TREE * pTree - 树指针	
	@param  void * pProperties - 属性指针	
	@param  DESTROYFUNC PropDestroyFunc - 属性释放函数	
	@return	INT - CAPI_FAILE表示失败，CAPI_SUCCESS表示成功	
*/
INT Tree_SetProperties( TREE * pTree, 
                        void * pProperties, 
                        DESTROYFUNC PropDestroyFunc)
{
    if ( pTree == NULL )
    {
        return CAPI_FAILED;
    }

    /* 如果原来已经设置了属性则需要将其释放以防止内存泄漏 */
    if ( pTree->pProperties != NULL )
    {
        PropDestroyFunc( pTree->pProperties );
    }

    pTree->pProperties = pProperties;

    return CAPI_SUCCESS;
}


/**	树的获取属性函数

	@param	TREE * pTree - 树指针	
	@return	void * - 成功返回属性指针，返回NULL表示失败或属性为NULL	
*/
void * Tree_GetProperties(TREE * pTree)
{
    if ( pTree == NULL )
    {
        return NULL;
    }

    return pTree->pProperties;
}


/**	给树添加子树的函数

	@param	TREE * pTree - 树指针	
	@param	TREE * pSubTree -子树指针	
	@return	INT - CAPI_FAILE表示失败，CAPI_SUCCESS表示成功	
*/
INT Tree_AddSubTree(TREE * pTree, TREE * pSubTree)
{
    if ( pTree == NULL || pSubTree == NULL )
    {
        return CAPI_FAILED;
    }

    return DoubleList_InsertTail( pTree->pSubTreeList, (void *)pSubTree );
}


/**	树的删除子树函数

	@param	TREE * pTree - 树指针	
	@param	TREE * pSubTree - 子树指针	
	@param	DESTROYFUNC LeafDestroyFunc - 叶子释放函数	
	@param	DESTROYFUNC PropDestroyFunc - 属性释放函数	
	@return	INT - CAPI_FAILE表示失败，CAPI_SUCCESS表示成功	
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

    /* 遍历子树链表查找对应的子树进行删除 */
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


/**	树的拷贝函数，采用先序遍历算法进行拷贝

	@param	TREE * pTree - 树指针	
	@param	COPYFUNC LeafCopyFunc - 叶子拷贝函数	
	@param	COPYFUNC PropCopyFunc - 属性拷贝函数	
	@return	TREE * - 拷贝后的新的树的指针	
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
    
    /* 拷贝叶子列表 */
    pNewTree->pLeafList = DoubleList_Copy( pTree->pLeafList, LeafCopyFunc );

    /* 拷贝属性 */
    pNewTree->pProperties = (*PropCopyFunc)( pTree->pProperties );

    pList = pTree->pSubTreeList;

    /* 逐个遍历子树列表调用Tree_Copy递归拷贝子树列表 */
    DoubleList_EnumBegin(pList);

    while( (pData = DoubleList_EnumNext(pList)) != NULL )
    {
        TREE *pSubTree = Tree_Copy((TREE*)pData, LeafCopyFunc, PropCopyFunc);
        DoubleList_InsertTail(pNewList, (void *)pSubTree);
    }
   
    return pNewTree;
}

/**	树的按属性查找子树函数

	@param	TREE  * pTree -树指针	
	@param	void  * pProperties - 要查找的属性指针	
	@param	COMPAREFUNC   CompareFunc - 属性比较函数	
	@return	TREE * - 成功返回查找到的子树指针，失败返回NULL	
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
    
    /* 如果已经和树的属性相同，返回当前的树指针 */
    if ( (*CompareFunc)( pProperties, pTree->pProperties )  == 0 )
    {
        return pTree;
    }

    pList = pTree->pSubTreeList;

    DoubleList_EnumBegin(pList);

    /* 逐个遍历子树并递归调用本函数在子树中进行查找 */
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
