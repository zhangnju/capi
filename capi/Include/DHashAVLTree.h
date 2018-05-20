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
 *	DHashAVLTree.h
 *
 *	DESCRIPTION
 *		Interface to a generic dynamatic Hash AVL Tree object.
 *
 *	HISTORY
 *		07-19-2007	create by zhouweiming.
 *
 */
#ifndef __DHASHAVLTREE_H__
#define __DHASHAVLTREE_H__

#include "CapiGlobal.h"
#include "FastLock.h"

#define FIND_TABLE	0
#define INDEX_TABLE 1


#define MAX_NODE_COUNT_FOR_SPLIT      128

struct INDEXNODE {
    FASTLOCK	    lock;
    void			*pKey;
    LONG volatile	lType; //FIND_TABLE or INDEX_TABLE;
    void 			*pSubTable; //sub table pointer£¬if lType is FIND_TABLE, pSubTable is a AVLTree,
                           // if lType is INDEX_TABLE, then pSubTable pointer to a INDEXNODE array .
};

struct INDEXTABLE {
    INDEXNODE	*pNode; //index node array
    UINT		uCount; //index node count
    INDEXNODE   *pParent; //the parent node that pointer to this index table
};

struct DHASHAVLTREE {
    INDEXNODE	*pIndexNode;
    UINT		uBucketCount;
    UINT        uMaxNodeCount;
};


DHASHAVLTREE *DHashAVLTree_Create(UINT uBucketCount);
void DHashAVLTree_Destroy(DHASHAVLTREE *pTree, DESTROYFUNC KeyDestroy);

void *DHashAVLTree_Find(DHASHAVLTREE *pTree, void *pKey, HASHFUNC HashFunc,
                        COMPAREFUNC CompareFunc, COPYFUNC CopyFunc);
void DHashAVLTree_Insert(DHASHAVLTREE *pTree, void *pKey,
                         COMPAREFUNC CompareFunc, HASHFUNC HashFunc,
						 COPYFUNC CopyFunc);
void DHashAVLTree_Delete(DHASHAVLTREE *pTree, void *pKey, HASHFUNC HashFunc,
                         COMPAREFUNC CompareFunc, DESTROYFUNC DestroyFunc);


template <class T> 
class DSearch {
private:
    DHASHAVLTREE *m_pTree;
    COMPAREFUNC  m_CompareFunc;
    HASHFUNC     m_HashFunc;
    COPYFUNC     m_CopyFunc;
    DESTROYFUNC  m_DestroyFunc;
    UINT         m_uBucketCount;
public:
    DSearch(UINT uBucketCount) : m_uBucketCount(uBucketCount)
    {
        m_pTree = DHashAVLTree_Create(uBucketCount);
    }
    ~DSearch()
    {
        DHashAVLTree_Destroy(m_pTree, m_DestroyFunc);
    }

    void SetFunctions(COMPAREFUNC CompareFunc, HASHFUNC HashFunc, 
                  COPYFUNC CopyFunc, DESTROYFUNC DestroyFunc)
    {
        m_CompareFunc = CompareFunc;
        m_HashFunc = HashFunc;
        m_CopyFunc = CopyFunc;
        m_DestroyFunc = DestroyFunc;
    }
    void Insert(T *pData)
    {
        DHashAVLTree_Insert(m_pTree, pData, m_CompareFunc, m_HashFunc);
    }
    void Delete(T *pData)
    {
        DHashAVLTree_Delete(m_pTree, pData, m_HashFunc, m_CompareFunc, m_DestroyFunc);
    }

    T *Find(T *pData)
    {
        return reinterpret_cast<T *>DHashAVLTree_Find(m_pTree, 
            reinterpret_cast<T *>pData, m_HashFunc, m_CompareFunc, m_CopyFunc);
    }
};


#endif // __DHASHAVLTREE_H__

