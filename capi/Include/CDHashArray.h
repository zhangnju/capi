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
*	CDHashArray.h
*
*	DESCRIPTION
*		Interface to a generic dynamatic distributed array search object.
*
*	HISTORY
*		09-19-2008	create by zhouweiming.
*
*/
#ifndef __CDARRAYSEARCH_H__
#define __CDARRAYSEARCH_H__


#include "FastLock.h"
#include "dmalloc.h"

#define CDHASHARRAY_FIND_TABLE	 0
#define CDHASHARRAY_INDEX_TABLE  1

#define MAX_NODE_COUNT_FOR_SPLIT      128


template <class T>
struct INDEXNODE {
	FASTLOCK	    lock;
	T			    Key;
	LONG volatile	lType; //FIND_TABLE or INDEX_TABLE;
	void 			*pSubTable; //sub table pointer，if lType is DARRAYSEARCH_FIND_TABLE, 
	// pSubTable is a Search Array,
	// if lType is DARRAYSEARCH_INDEX_TABLE, 
	// then pSubTable pointer to a INDEXNODE array .
};



///////////////////////////////////////////////////////////////////////////
////                    下面是CIndexTable类的代码                      ////
///////////////////////////////////////////////////////////////////////////


#define		CINDEXTABLE_NODE_COUNT	2

template <class T, class SearchArray>
class CIndexTable {
public:
	INDEXNODE<T>	m_Node[CINDEXTABLE_NODE_COUNT]; //index node array

	~CIndexTable()
	{
		if ( m_Node[0].lType == CDHASHARRAY_FIND_TABLE )
		{
			SearchArray *p = (SearchArray *)m_Node[0].pSubTable;
			delete p;
			FastLockClose(&(m_Node[0].lock));
		}
		else
		{
			CIndexTable<T, SearchArray> *pTable = (CIndexTable<T, SearchArray> *)m_Node[0].pSubTable;
			pTable->~CIndexTable();
		}
		if ( m_Node[1].lType == CDHASHARRAY_FIND_TABLE )
		{
			SearchArray *p = (SearchArray *)m_Node[1].pSubTable;
			delete p;
			FastLockClose(&(m_Node[1].lock));
		}
		else
		{
			CIndexTable<T, SearchArray> *pTable = (CIndexTable<T, SearchArray> *)m_Node[1].pSubTable;
			pTable->~CIndexTable();
		}
	}

	INDEXNODE<T> *UpperBound( T &Data)
	{
		if ( Data <= m_Node[0].Key )
		{
			return &(m_Node[0]);
		}
		else 
		{
			return &(m_Node[1]);
		}
	}

	void Split(SearchArray *pArray)
	{
		SearchArray *pNewArray = pArray->Split();

		T Key;
		pArray->GetMax(Key);
		m_Node[0].Key = Key;

		FastLockInit(&(m_Node[0].lock));
		FastLockInit(&(m_Node[1].lock));

		m_Node[0].pSubTable = pArray;
		m_Node[1].pSubTable = pNewArray;

		m_Node[0].lType = CDHASHARRAY_FIND_TABLE;
		m_Node[1].lType = CDHASHARRAY_FIND_TABLE;

		return ;
	}
};

///////////////////////////////////////////////////////////////////////////
////                    下面是CSearchArray类的代码                     ////
///////////////////////////////////////////////////////////////////////////


#define		DEFAULT_SEARCH_ARRAY_SIZE	14

template <class T>
class CSearchArray {
PRIVATE:
	T * m_pData;
	int m_nCount;
	int m_nSize;
public:
	CSearchArray(int nSize);
	~CSearchArray();

	void * operator new(size_t size)
	{
        //下面代码需要改成当dmalloc()失败时，应抛出异常
		return dmalloc(size);
	}

	void * operator new[](size_t size)
	{
        //下面代码需要改成当dmalloc()失败时，应抛出异常
		return dmalloc(size);
	}

	void operator delete(void *p)
	{
		dfree(p);
	}

	void operator delete[](void *p)
	{
		dfree(p);
	}


	void Insert(T &Data);
	void Delete(T &Data);
	void Find(T &Data, T &OutData);

	void GetMax(T &Data);

	CSearchArray<T> *Split();
};

template <class T>
CSearchArray<T>::CSearchArray(int nSize)
{
	m_pData = new T[nSize];
	m_nCount = 0;
	m_nSize = nSize;
}


template <class T>
CSearchArray<T>::~CSearchArray()
{
	delete [] m_pData;
}

template <class T>
void CSearchArray<T>::Find(T &Data, T &OutData)
{
	int i;

	int nMid = m_nCount >> 1; //除以2

	if ( m_pData[nMid] > Data )
	{
		for ( i = 0; i < nMid; i++ )
		{
			if ( m_pData[i] == Data )
			{
				OutData = m_pData[i];
				break;
			}
		}
	}
	else
	{
		for ( i = nMid; i < m_nCount; i++ )
		{
			if ( m_pData[i] == Data )
			{
				OutData = m_pData[i];
				break;
			}
		}
	}
}

template <class T>
void CSearchArray<T>::Insert(T &Data)
{
	int i, j;
    int nMid = m_nCount >> 1; //除以2

    if ( m_pData[nMid] > Data )
    {
        for ( i = 0; i < nMid; i++ )
        {
            if ( Data <= m_pData[i] )
            {
                break;
            }
        }
    }
    else
    {
        for ( i = nMid; i < m_nCount; i++ )
        {
            if ( Data <= m_pData[i] )
            {
                break;
            }
        }
    }

	for ( j = m_nCount; j > i; j-- )
	{
		m_pData[j] = m_pData[j-1];
	}
	m_pData[i] = Data;
	m_nCount += 1;
}


template <class T>
void CSearchArray<T>::Delete(T &Data)
{
	int i,j;
	for ( i = 0; i < m_nCount; i++ )
	{
		if ( m_pData[i] == Data )
		{
			break;
		}
	}
	for ( j = i; j < m_nCount - 1; j++)
	{
		m_pData[j] = m_pData[j+1];
	}
	m_nCount -= 1;
}


template <class T>
void CSearchArray<T>::GetMax(T &Data)
{
	Data = m_pData[m_nCount-1];
}


/**	将一个SearchArray类分拆成两个CSearchArray类
	劈开后的两个类，一个为自身，另外一个通过返回值返回

	@return	CSearchArray<T> * - 返回一个被分拆开的新的CSearchArray类指针	
*/
template <class T>
CSearchArray<T> * CSearchArray<T>::Split()
{
	CSearchArray<T> *pArray = new CSearchArray<T>(DEFAULT_SEARCH_ARRAY_SIZE);
	int nCount  = m_nCount >> 1; //除以2

	int i;
	for ( i = nCount; i< m_nCount; i++ )
	{
		pArray->Insert(m_pData[i]);
	}
	m_nCount -= nCount;

	return pArray;
}

///////////////////////////////////////////////////////////////////////////
////                    下面是CDHashArray类的代码                      ////
///////////////////////////////////////////////////////////////////////////

template <class T, class SearchArray>
class CDHashArray {
PRIVATE:
	INDEXNODE<T> * m_pIndexNode;
	int m_nBucketCount;
	int m_nSplitCount;

	HASHFUNC	m_HashFunc;

	void SubTableInsert(INDEXNODE<T> *pNode, T &Data);
	void SubTableDelete(INDEXNODE<T> *pNode, T &Data);
	void SubTableFind(INDEXNODE<T> *pNode, T &Data, T &OutData);
public:

	CDHashArray(int nBucketCount, HASHFUNC HashFunc);
	virtual ~CDHashArray();

	void * operator new(size_t size)
	{
        //下面代码需要改成当dmalloc()失败时，应抛出异常
		return dmalloc(size);
	}

	void * operator new[](size_t size)
	{
        //下面代码需要改成当dmalloc()失败时，应抛出异常
		return dmalloc(size);
	}

	void operator delete(void *p)
	{
		dfree(p);
	}

	void operator delete[](void *p)
	{
		dfree(p);
	}

	void Insert(T &Data);
	void Delete(T &Data);

	void Find(T &Data, T &OutData);
};


template <class T, class SearchArray>
CDHashArray<T, SearchArray>::CDHashArray(int nBucketCount, HASHFUNC HashFunc)
{
	m_pIndexNode = new INDEXNODE<T>[nBucketCount];
	m_nBucketCount = nBucketCount;
	m_HashFunc = HashFunc;

	int i;
	for ( i = 0; i < nBucketCount; i++ )
	{
		FastLockInit(&(m_pIndexNode[i].lock));
		m_pIndexNode[i].lType = CDHASHARRAY_FIND_TABLE;
		m_pIndexNode[i].pSubTable = new SearchArray(DEFAULT_SEARCH_ARRAY_SIZE);
	}
}


template <class T, class SearchArray>
CDHashArray<T, SearchArray>::~CDHashArray()
{
	int i;
	for ( i = 0; i < m_nBucketCount; i++ )
	{
		if ( m_pIndexNode[i].lType == CDHASHARRAY_FIND_TABLE )
		{
			SearchArray *p = (SearchArray *)m_pIndexNode[i].pSubTable;
			delete p;
		}
		else
		{
			CIndexTable<T, SearchArray> *pTable = (CIndexTable<T, SearchArray> *)m_pIndexNode[i].pSubTable;
			delete pTable;
		}
		FastLockClose(&(m_pIndexNode[i].lock));
	}
	delete [] m_pIndexNode;
}

/**	子表插入函数

	@param	INDEXNODE<T> *pNode - 要查找的索引节点	
	@param	T &Data - 要查找的数据	
	@return	void - 无	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::SubTableInsert(INDEXNODE<T> *pNode, T &Data)
{
	FastLockAcquire(&(pNode->lock));

	LONG lType = pNode->lType;
	if ( lType == CDHASHARRAY_FIND_TABLE )
	{
		//将子表地址转换成一个索引表指针;
		SearchArray	*pArray = (SearchArray *)pNode->pSubTable;

		pArray->Insert(Data);

		if ( pArray->m_nCount == pArray->m_nSize )
		{
			CIndexTable<T, SearchArray> *pTable = new CIndexTable<T, SearchArray>;
			
			pTable->Split(pArray);

			pNode->pSubTable = pTable;

			//查找表的子表类型 = 索引表(原子操作);
			AtomicIncrement(&(pNode->lType));
		}
	}
	else
	{
		//将查找表指针转换为索引表指针;
		CIndexTable<T, SearchArray>	*pTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//在索引表中找到对应的子表;
		INDEXNODE<T> *pTempNode = pTable->UpperBound(Data);

		//由于在获取锁前，其他线程将表的类型由查找表改为了索引表
		//这种情况下有可能有很多其他线程访问这个索引表时是不需要使用锁的
		//因此这里需要递归调用,防止找到的查找子表又发生大量插入而变为索引表
		//这样会发生多级加锁解锁操作，不过发生这种情况的概率非常低
		SubTableInsert(pTempNode, Data); 
	}
	FastLockRelease(&(pNode->lock));
	return ;
}


/**	分布式搜索数组的插入数据函数

	@param	T &Data - 要插入的数据	
	@return	void - 无	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::Insert(T &Data)
{
	int nPos = (*m_HashFunc)((void *)Data, m_nBucketCount);
	INDEXNODE<T> *pNode = &(m_pIndexNode[nPos]);

	LONG lType = pNode->lType;
	while ( lType == CDHASHARRAY_INDEX_TABLE )
	{
		//将子表地址转换成一个索引表指针;
		CIndexTable<T, SearchArray>	*pIndexTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//最终子表 ＝ 在索引表中进行二分查找到对应的索引节点;
		pNode = pIndexTable->UpperBound(Data);

		//子表类型 ＝ 索引节点的子表类型;
		lType = pNode->lType;
	}

	SubTableInsert(pNode, Data);
	return;
}

/**	分布式搜索数组的子表删除数据函数

	@param	INDEXNODE<T> *pNode - 要删除数据的索引节点	
	@param	T &Data - 要删除的数据	
	@return	void - 无	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::SubTableDelete(INDEXNODE<T> *pNode, T &Data)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == CDHASHARRAY_FIND_TABLE)
	{
		SearchArray	*pArray = (SearchArray *)pNode->pSubTable;

		//在查找表中删除包含Data的节点;
		pArray->Delete(Data);
	}
	else
	{
		//将查找表指针转换为索引表指针;
		CIndexTable<T, SearchArray>	*pTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//在索引表中进行二分查找到对应的子表;
		pNode = pTable->UpperBound(Data);

		//由于在获取锁前，其他线程将表的类型由查找表改为了索引表
		//这种情况下有可能有很多其他线程访问这个索引表时是不需要使用锁的
		//因此递归调用,防止找到的查找子表又发生大量插入而变为索引表
		//不过这样会发生多级加锁解锁操作，不过发生这种情况的概率非常低
		SubTableDelete(pNode, Data); 
	}
	FastLockRelease(&(pNode->lock));

	return;	
}



/**	分布式搜索数组的删除数据函数

	@param	T &Data - 要删除的数据	
	@return	void - 无	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::Delete(T &Data)
{
	int nPos = (*m_HashFunc)((void *)Data, m_nBucketCount);
	INDEXNODE<T> *pNode = &(m_pIndexNode[nPos]);

	LONG lType = pNode->lType;
	while ( lType == CDHASHARRAY_INDEX_TABLE )
	{
		//将子表地址转换成一个索引表指针;
		CIndexTable<T, SearchArray>	*pIndexTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//最终子表 ＝ 在索引表中进行二分查找到对应的索引节点;
		pNode = pIndexTable->UpperBound(Data);

		//子表类型 ＝ 索引节点的子表类型;
		lType = pNode->lType;
	}

	SubTableDelete(pNode, Data);
	return;
}

/**	分布式搜索数组的子表查找数据函数

	@param	INDEXNODE<T> *pNode - 要删除数据的索引节点	
	@param	T &Data - 要查找的数据	
	@param	T &OutData - 存放找到的数据	
	@return	void - 无	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::SubTableFind(INDEXNODE<T> *pNode, T &Data, T &OutData)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == CDHASHARRAY_FIND_TABLE)
	{
		SearchArray	*pArray = (SearchArray *)pNode->pSubTable;

		//在查找表中查找包含Data的节点;
		pArray->Find(Data, OutData);
	}
	else
	{
		//将查找表指针转换为索引表指针;
		CIndexTable<T, SearchArray>	*pTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//在索引表中进行二分查找到对应的子表;
		pNode = pTable->UpperBound(Data);

		//由于在获取锁前，其他线程将表的类型由查找表改为了索引表
		//这种情况下有可能有很多其他线程访问这个索引表时是不需要使用锁的
		//因此递归调用,防止找到的查找子表又发生大量插入而变为索引表
		//不过这样会发生多级加锁解锁操作，不过发生这种情况的概率非常低
		SubTableFind(pNode, Data, OutData); 
	}
	FastLockRelease(&(pNode->lock));

	return;	
}

/**	分布式搜索数组的查找数据函数

	@param	T &Data - 要查找的数据	
	@param	T &OutData - 存放找到的数据	
	@return	void - 无	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::Find(T &Data, T &OutData)
{
	int nPos = (*m_HashFunc)((void *)Data, m_nBucketCount);
	INDEXNODE<T> *pNode = &(m_pIndexNode[nPos]);

	LONG lType = pNode->lType;
	while ( lType == CDHASHARRAY_INDEX_TABLE )
	{
		//将子表地址转换成一个索引表指针;
		CIndexTable<T, SearchArray>	*pIndexTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//最终子表 ＝ 在索引表中进行二分查找到对应的索引节点;
		pNode = pIndexTable->UpperBound(Data);

		//子表类型 ＝ 索引节点的子表类型;
		lType = pNode->lType;
	}

	SubTableFind(pNode, Data, OutData);
	return;
}


#endif // __CDARRAYSEARCH_H__

