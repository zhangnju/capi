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
    AVLTREENODE		**m_ppBucket;   //Ͱ
    FASTLOCK        *m_pLock;       //�������飬��Ͱ���������
    int             m_nBucketCount; //Ͱ������
    HASHFUNC        m_HashFunc;     //��ϣ����
    COMPAREFUNC     m_Compare;      //���ݱȽϺ���
    DESTROYFUNC     m_Destroy;      //�����ͷź���
    COPYFUNC        m_Copy;         //���ݿ�������
public:
    CDHashAVLTree(int nBucketCount, HASHFUNC HashFunc, 
        COMPAREFUNC Compare, DESTROYFUNC Destroy, COPYFUNC CopyFunc);
    virtual ~CDHashAVLTree();

    INT Insert(void *pData);
    INT Delete(void *pData);
    void *Find(void *pData);
};


#endif // __CDHASHAVLTREE_H__


