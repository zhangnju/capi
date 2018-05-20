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
	void 			*pSubTable; //sub table pointer��if lType is DARRAYSEARCH_FIND_TABLE, 
	// pSubTable is a Search Array,
	// if lType is DARRAYSEARCH_INDEX_TABLE, 
	// then pSubTable pointer to a INDEXNODE array .
};



///////////////////////////////////////////////////////////////////////////
////                    ������CIndexTable��Ĵ���                      ////
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
////                    ������CSearchArray��Ĵ���                     ////
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
        //���������Ҫ�ĳɵ�dmalloc()ʧ��ʱ��Ӧ�׳��쳣
		return dmalloc(size);
	}

	void * operator new[](size_t size)
	{
        //���������Ҫ�ĳɵ�dmalloc()ʧ��ʱ��Ӧ�׳��쳣
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

	int nMid = m_nCount >> 1; //����2

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
    int nMid = m_nCount >> 1; //����2

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


/**	��һ��SearchArray��ֲ������CSearchArray��
	������������࣬һ��Ϊ��������һ��ͨ������ֵ����

	@return	CSearchArray<T> * - ����һ�����ֲ𿪵��µ�CSearchArray��ָ��	
*/
template <class T>
CSearchArray<T> * CSearchArray<T>::Split()
{
	CSearchArray<T> *pArray = new CSearchArray<T>(DEFAULT_SEARCH_ARRAY_SIZE);
	int nCount  = m_nCount >> 1; //����2

	int i;
	for ( i = nCount; i< m_nCount; i++ )
	{
		pArray->Insert(m_pData[i]);
	}
	m_nCount -= nCount;

	return pArray;
}

///////////////////////////////////////////////////////////////////////////
////                    ������CDHashArray��Ĵ���                      ////
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
        //���������Ҫ�ĳɵ�dmalloc()ʧ��ʱ��Ӧ�׳��쳣
		return dmalloc(size);
	}

	void * operator new[](size_t size)
	{
        //���������Ҫ�ĳɵ�dmalloc()ʧ��ʱ��Ӧ�׳��쳣
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

/**	�ӱ���뺯��

	@param	INDEXNODE<T> *pNode - Ҫ���ҵ������ڵ�	
	@param	T &Data - Ҫ���ҵ�����	
	@return	void - ��	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::SubTableInsert(INDEXNODE<T> *pNode, T &Data)
{
	FastLockAcquire(&(pNode->lock));

	LONG lType = pNode->lType;
	if ( lType == CDHASHARRAY_FIND_TABLE )
	{
		//���ӱ��ַת����һ��������ָ��;
		SearchArray	*pArray = (SearchArray *)pNode->pSubTable;

		pArray->Insert(Data);

		if ( pArray->m_nCount == pArray->m_nSize )
		{
			CIndexTable<T, SearchArray> *pTable = new CIndexTable<T, SearchArray>;
			
			pTable->Split(pArray);

			pNode->pSubTable = pTable;

			//���ұ���ӱ����� = ������(ԭ�Ӳ���);
			AtomicIncrement(&(pNode->lType));
		}
	}
	else
	{
		//�����ұ�ָ��ת��Ϊ������ָ��;
		CIndexTable<T, SearchArray>	*pTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//�����������ҵ���Ӧ���ӱ�;
		INDEXNODE<T> *pTempNode = pTable->UpperBound(Data);

		//�����ڻ�ȡ��ǰ�������߳̽���������ɲ��ұ��Ϊ��������
		//����������п����кܶ������̷߳������������ʱ�ǲ���Ҫʹ������
		//���������Ҫ�ݹ����,��ֹ�ҵ��Ĳ����ӱ��ַ��������������Ϊ������
		//�����ᷢ���༶������������������������������ĸ��ʷǳ���
		SubTableInsert(pTempNode, Data); 
	}
	FastLockRelease(&(pNode->lock));
	return ;
}


/**	�ֲ�ʽ��������Ĳ������ݺ���

	@param	T &Data - Ҫ���������	
	@return	void - ��	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::Insert(T &Data)
{
	int nPos = (*m_HashFunc)((void *)Data, m_nBucketCount);
	INDEXNODE<T> *pNode = &(m_pIndexNode[nPos]);

	LONG lType = pNode->lType;
	while ( lType == CDHASHARRAY_INDEX_TABLE )
	{
		//���ӱ��ַת����һ��������ָ��;
		CIndexTable<T, SearchArray>	*pIndexTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//�����ӱ� �� ���������н��ж��ֲ��ҵ���Ӧ�������ڵ�;
		pNode = pIndexTable->UpperBound(Data);

		//�ӱ����� �� �����ڵ���ӱ�����;
		lType = pNode->lType;
	}

	SubTableInsert(pNode, Data);
	return;
}

/**	�ֲ�ʽ����������ӱ�ɾ�����ݺ���

	@param	INDEXNODE<T> *pNode - Ҫɾ�����ݵ������ڵ�	
	@param	T &Data - Ҫɾ��������	
	@return	void - ��	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::SubTableDelete(INDEXNODE<T> *pNode, T &Data)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == CDHASHARRAY_FIND_TABLE)
	{
		SearchArray	*pArray = (SearchArray *)pNode->pSubTable;

		//�ڲ��ұ���ɾ������Data�Ľڵ�;
		pArray->Delete(Data);
	}
	else
	{
		//�����ұ�ָ��ת��Ϊ������ָ��;
		CIndexTable<T, SearchArray>	*pTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//���������н��ж��ֲ��ҵ���Ӧ���ӱ�;
		pNode = pTable->UpperBound(Data);

		//�����ڻ�ȡ��ǰ�������߳̽���������ɲ��ұ��Ϊ��������
		//����������п����кܶ������̷߳������������ʱ�ǲ���Ҫʹ������
		//��˵ݹ����,��ֹ�ҵ��Ĳ����ӱ��ַ��������������Ϊ������
		//���������ᷢ���༶������������������������������ĸ��ʷǳ���
		SubTableDelete(pNode, Data); 
	}
	FastLockRelease(&(pNode->lock));

	return;	
}



/**	�ֲ�ʽ���������ɾ�����ݺ���

	@param	T &Data - Ҫɾ��������	
	@return	void - ��	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::Delete(T &Data)
{
	int nPos = (*m_HashFunc)((void *)Data, m_nBucketCount);
	INDEXNODE<T> *pNode = &(m_pIndexNode[nPos]);

	LONG lType = pNode->lType;
	while ( lType == CDHASHARRAY_INDEX_TABLE )
	{
		//���ӱ��ַת����һ��������ָ��;
		CIndexTable<T, SearchArray>	*pIndexTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//�����ӱ� �� ���������н��ж��ֲ��ҵ���Ӧ�������ڵ�;
		pNode = pIndexTable->UpperBound(Data);

		//�ӱ����� �� �����ڵ���ӱ�����;
		lType = pNode->lType;
	}

	SubTableDelete(pNode, Data);
	return;
}

/**	�ֲ�ʽ����������ӱ�������ݺ���

	@param	INDEXNODE<T> *pNode - Ҫɾ�����ݵ������ڵ�	
	@param	T &Data - Ҫ���ҵ�����	
	@param	T &OutData - ����ҵ�������	
	@return	void - ��	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::SubTableFind(INDEXNODE<T> *pNode, T &Data, T &OutData)
{
	FastLockAcquire(&(pNode->lock));

	if (pNode->lType == CDHASHARRAY_FIND_TABLE)
	{
		SearchArray	*pArray = (SearchArray *)pNode->pSubTable;

		//�ڲ��ұ��в��Ұ���Data�Ľڵ�;
		pArray->Find(Data, OutData);
	}
	else
	{
		//�����ұ�ָ��ת��Ϊ������ָ��;
		CIndexTable<T, SearchArray>	*pTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//���������н��ж��ֲ��ҵ���Ӧ���ӱ�;
		pNode = pTable->UpperBound(Data);

		//�����ڻ�ȡ��ǰ�������߳̽���������ɲ��ұ��Ϊ��������
		//����������п����кܶ������̷߳������������ʱ�ǲ���Ҫʹ������
		//��˵ݹ����,��ֹ�ҵ��Ĳ����ӱ��ַ��������������Ϊ������
		//���������ᷢ���༶������������������������������ĸ��ʷǳ���
		SubTableFind(pNode, Data, OutData); 
	}
	FastLockRelease(&(pNode->lock));

	return;	
}

/**	�ֲ�ʽ��������Ĳ������ݺ���

	@param	T &Data - Ҫ���ҵ�����	
	@param	T &OutData - ����ҵ�������	
	@return	void - ��	
*/
template <class T, class SearchArray>
void CDHashArray<T, SearchArray>::Find(T &Data, T &OutData)
{
	int nPos = (*m_HashFunc)((void *)Data, m_nBucketCount);
	INDEXNODE<T> *pNode = &(m_pIndexNode[nPos]);

	LONG lType = pNode->lType;
	while ( lType == CDHASHARRAY_INDEX_TABLE )
	{
		//���ӱ��ַת����һ��������ָ��;
		CIndexTable<T, SearchArray>	*pIndexTable = (CIndexTable<T, SearchArray> *)pNode->pSubTable;

		//�����ӱ� �� ���������н��ж��ֲ��ҵ���Ӧ�������ڵ�;
		pNode = pIndexTable->UpperBound(Data);

		//�ӱ����� �� �����ڵ���ӱ�����;
		lType = pNode->lType;
	}

	SubTableFind(pNode, Data, OutData);
	return;
}


#endif // __CDARRAYSEARCH_H__

