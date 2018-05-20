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
*	CDHashAVLTree.h
*
*	DESCRIPTION
*		Interface to a generic static distributed AVL tree object.
*
*	HISTORY
*		10-16-2008	create by zhouweiming.
*
*/
#ifndef __CDHASHAVLTREE_H__
#define __CDHASHAVLTREE_H__


#include "CapiGlobal.h"
#include "BinTree.h"
#include "AVLTree.h"
#include "FastLock.h"


class CDHashAVLTree {
PRIVATE:
    AVLTREENODE		**m_ppBucket;   //桶
    FASTLOCK        *m_pLock;       //锁的数组，和桶的数量相等
    int             m_nBucketCount; //桶的数量
    HASHFUNC        m_HashFunc;     //哈希函数
    COMPAREFUNC     m_Compare;      //数据比较函数
    DESTROYFUNC     m_Destroy;      //数据释放函数
    COPYFUNC        m_Copy;         //数据拷贝函数
public:
    CDHashAVLTree(int nBucketCount, HASHFUNC HashFunc, 
        COMPAREFUNC Compare, DESTROYFUNC Destroy, COPYFUNC CopyFunc);
    virtual ~CDHashAVLTree();

    INT Insert(void *pData);
    INT Delete(void *pData);
    void *Find(void *pData);
};


#endif // __CDHASHAVLTREE_H__


