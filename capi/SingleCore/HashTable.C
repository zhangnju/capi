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

#include <stdlib.h>
#include <string.h>
#include "CapiGlobal.h"
#include "HashTable.h"

#pragma warning(disable : 4996)

/**	哈希表的创建函数

	@param	UINT uBucketCount - 索引的大小	
	@return	HASHTABLE * - 成功返回哈希表的指针，失败返回NULL	
*/
HASHTABLE *	HashTable_Create(UINT uBucketCount)
{
    HASHTABLE	*pTable;

    if ( uBucketCount == 0 )
    {
        return NULL;
    }
    
    pTable = (HASHTABLE *)malloc( sizeof(HASHTABLE) );
    if ( pTable == NULL )
    {
        return NULL;
    }

    pTable->uNodeCount = 0;
    pTable->uBucketCount = uBucketCount;

    pTable->ppBucket = (SINGLENODE **)malloc( uBucketCount 
        * sizeof(SINGLENODE *));

    if (pTable->ppBucket == NULL)
    {
        free( pTable );
        return NULL;
    }

    memset(pTable->ppBucket, 0, uBucketCount * sizeof(SINGLENODE *));

    pTable->pCurEntry = NULL;
    pTable->uCurBucketNo = 0;

    return pTable;
}


/**	哈希表的释放函数

	@param	HASHTABLE *pTable - 哈希表指针	
	@param	DESTROYFUNC DestroyFunc - 数据释放函数，为NULL时只释放节点辅助
                                      空间，不释放数据	
	@return	void - 无	
*/
void HashTable_Destroy( HASHTABLE *pTable, 
                        DESTROYFUNC DestroyFunc)
{
    SINGLENODE **ppBucket;
    SINGLENODE  *pNode;
    SINGLENODE  *pFreeNode;
    UINT i;


    if ( pTable == NULL )
    {
        return;
    }
    ppBucket = pTable->ppBucket;
    for ( i = 0; i < pTable->uBucketCount; i++ ) 
    {
        pNode = ppBucket[i];
        while ( pNode != NULL )
        {
            if ( DestroyFunc != NULL )
            {
                (*DestroyFunc)(pNode->pData);
            }
            pFreeNode = pNode;
            pNode = pNode->pNext;
            free(pFreeNode);
        }
    }
    free(ppBucket);

    /* 将ppbucket设成空指针以避免哈希表被重新使用时造成内存泄漏 */
    pTable->ppBucket = NULL;

    free( pTable );
}


/**	哈希表的插入函数

	@param	HASHTABLE *pTable - 哈希表指针	
	@param	void *pData - 数据指针，其中包含关键词信息	
	@param	ASHFUNC HashFunc - 哈希函数，用来计算关键词的索引	
	@return	INT (by default) - CAPI_SUCCESS表示成功，CAPI_FAILED表示失败	
*/
INT	HashTable_Insert( HASHTABLE *pTable, 
                    void *pData, 
                    HASHFUNC HashFunc )
{
    UINT		uIndex;
    SINGLENODE	*pNode;
    SINGLENODE	*pNewNode;

    if ( pTable == NULL || pData == NULL || HashFunc == NULL )
    {
        return CAPI_FAILED;
    }

    uIndex = (*HashFunc)( pData, pTable->uBucketCount );
    pNode = (pTable->ppBucket)[uIndex];

    pNewNode = (SINGLENODE *)malloc( sizeof(SINGLENODE) );

    if ( pNewNode == NULL )
    {
        return CAPI_FAILED;
    }
	
    /* 将新节点插入到链表的头部 */
    pNewNode->pData = pData;
    pNewNode->pNext = pNode;

    (pTable->ppBucket)[uIndex] = pNewNode;
    pTable->uNodeCount += 1;

    return CAPI_SUCCESS;
}


/**	哈希表的查找函数

	@param	HASHTABLE *pTable - 哈希表指针	
	@param  void *pData - 包含关键词信息的数据指针	
	@param  HASHFUNC HashFunc - 哈希表的计算索引函数	
	@param  COMPAREFUNC CompareFunc - 关键词比较函数	
	@return	void * - 查到的数据指针，未查到返回NULL	
*/
void *	HashTable_Find(HASHTABLE *pTable, 
							   void *pData, 
							   HASHFUNC HashFunc,
							   COMPAREFUNC CompareFunc)
{
	UINT			uIndex;
	SINGLENODE *	pNode;

	if ( pTable == NULL || HashFunc == NULL || CompareFunc == NULL )
	{
		return NULL;
	}

	uIndex = (*HashFunc)( pData, pTable->uBucketCount );
	pNode = (pTable->ppBucket)[uIndex];
	
	/* 在 HASHTABLE 中进行查找 */
	while ( pNode != NULL )
	{
		if ( (*CompareFunc)( pNode->pData, pData) == 0 )
		{
			/* 已经找到了关键词，返回 */
			return pNode->pData;
		}
		pNode = pNode->pNext;
	}

    return NULL;
}


/**	哈希表的删除函数

	@param	HASHTABLE *pTable - 哈西表指针	
	@param  void *pData - 包含关键词信息的数据指针	
	@param  HASHFUNC HashFunc - 哈希函数，用来计算关键词的索引	
	@param  COMPAREFUNC CompareFunc - 关键词比较函数	
	@param  DESTROYFUNC DataDestroyFunc - 数据释放函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功	
*/
INT HashTable_Delete(HASHTABLE *pTable, 
					 void *pData, 
					 HASHFUNC HashFunc,
					 COMPAREFUNC CompareFunc,
					 DESTROYFUNC DataDestroyFunc )

{
	UINT			uIndex;
	SINGLENODE *	pNode;
	SINGLENODE *    pPrevNode;

	if ( pTable == NULL || pData == NULL || HashFunc == NULL 
        || CompareFunc == NULL )
	{
		return CAPI_FAILED;
	}

	uIndex = (*HashFunc)( pData, pTable->uBucketCount );
	pNode = (pTable->ppBucket)[uIndex];
    pPrevNode = pNode;

    /* 从哈希表中查找 */
    while ( pNode != NULL )
    {
        if ( (*CompareFunc)( pNode->pData, pData ) == 0 )
        {
            if ( pPrevNode == pNode )
            {
                pTable->ppBucket[uIndex] = pNode->pNext;
            }
            else
            {
                pPrevNode->pNext = pNode->pNext;
            }

            /* 删除对应节点 */
            if ( DataDestroyFunc != NULL )
            {
                (*DataDestroyFunc)(pNode->pData);
            }
            free(pNode);

            pTable->uNodeCount -= 1;

            return 1;
        }

        pPrevNode = pNode;
        pNode = pNode->pNext;
    }

    return 0;
}

/**	获取哈希表中的实际节点个数

	@param	HASHTABLE *pTable - 哈希表指针	
	@return	UINT - 返回哈希表中的实际节点数量	
*/
UINT HashTable_GetNodeCount(HASHTABLE *pTable)
{
    return pTable->uNodeCount;
}


/**	哈希表的逐个节点遍历初始化函数

	@param	HASHTABLE *pTable - 哈希表指针	
	@return	void - 无	
*/
void HashTable_EnumBegin(HASHTABLE *pTable)
{
    pTable->uCurBucketNo = 0;
    pTable->pCurEntry = pTable->ppBucket[0];
}


/**	哈希表的逐个节点遍历函数

	@param	HASHTABLE *pTable - 哈希表指针	
	@return	void * - 遍历到的节点的数据指针	
*/
void * HashTable_EnumNext(HASHTABLE *pTable)
{
    void *pData;

	while ( pTable->pCurEntry == NULL )
	{
		pTable->uCurBucketNo += 1;
		if ( pTable->uCurBucketNo >= pTable->uBucketCount )
		{
			return NULL;
		}
		pTable->pCurEntry = pTable->ppBucket[pTable->uCurBucketNo];
	}

	pData = pTable->pCurEntry->pData;

    pTable->pCurEntry = pTable->pCurEntry->pNext;

    return pData;
}


/**	哈希表的计算整数的索引函数

	@param	UINT uKey - 关键词	
	@param	UINT uBucketCount - 哈希表的大小，用来做整除的模	
	@return	UINT - 索引值	
*/
UINT HashInt( void *pKey, UINT uBucketCount )
{
	return ((UINT)pKey) % uBucketCount;
}


/**	字符串类型数据的关键词索引计算函数
    将字符串折叠成每5个字符一段，每段看成是8进制整数，将各段相加后再取余数

	@param	void *pStr - 字符串指针	
	@param	UINT uBucketCount - 最大桶的个数，实际上被用作整除的模	
	@return	UINT - 字符串关键词的索引	
*/
UINT HashString( void *pStr, UINT uBucketCount )
{
    unsigned char	*psz;
    UINT	uHashValue;
    UINT	uRet;
    UINT	i;

    psz = (unsigned char *)pStr;
    uHashValue = 0;

    i = 0;
    uRet = 0;
    while ( *psz != '\0' )
    {
	    if ( i == 5 )
	    {
		    i = 0;
		    uRet += uHashValue;
		    uHashValue = 0;
	    }
	    uHashValue += uHashValue << 3;
	    uHashValue += (UINT)tolower( *psz );
	    psz++;
	    i++;
    }

    uRet += uHashValue;

    return uRet % uBucketCount;
}


/*
 *	HashBin( void *str )
 *	Calculate the hash value of a string.
 *	Parameters:
 *		void *str,		the string that need calculate.
 *	Return Values:
 *		the hash value of the string.
 */
UINT HashBin( void *pData, UINT uLength, UINT uBucketCount )
{
    unsigned char	*psz;
    UINT	uHashValue;
    UINT	uRet;
    UINT	i;
    UINT	j;

    psz = (unsigned char *)pData;
    uHashValue = 0;


    j = 0;
    uRet = 0;
    i = 0;
    while ( i < uLength )
    {
	    if ( j == 5 )
	    {
		    j = 0;
		    uRet += uHashValue;
		    uHashValue = 0;
	    }
	    uHashValue += uHashValue << 3;
	    uHashValue += (UINT)tolower( *psz );
	    psz++;
	    j++;
	    i++;
    }

    uRet += uHashValue;

    return uRet % uBucketCount;
}


/*
 *	BinCompare( )
 *	Compare if two binary data is equal.
 *	Return Values:
 *		0		equal
 *		1		first > second 
 *      -1      first < second
 */
INT BinCompare( void *str1, int str1_len, void *str2, int str2_len )
{
	char	*psz1, *psz2;
	int		i;
    int     len;

    len = str1_len < str2_len ? str1_len:str2_len;

	psz1 = (char *)str1;
	psz2 = (char *)str2;

	i = 0;
	while ( i < len )
	{
		if ( *psz1 > *psz2 )
		{
			return 1;
		}
        else if ( *psz1 < *psz2 )
        {
            return -1;
        }
		psz1++;
		psz2++;
		i++;
	}
    if ( str1_len < str2_len )
    {
        return -1;
    }
    else if ( str1_len > str2_len )
    {
        return 1;
    }
    else
    {
    	return 0;
    }
}


/*
 *	StrCompare( )
 *	Compare if two string is equal.
 *	Return Values:
 *		1		pStr1 greater than pStr2
 *		0		pStr1 equal to pStr2
 *      -1      pStr1 less than pStr2
 */
INT StrCompare( void *pStr1, void *pStr2 )
{
	return stricmp( (char *)pStr1, (char *)pStr2 );
}


/**	整数的比较函数

	@param	void *pInt1 - 第1个要比较的整数	
	@param	void *pInt2 - 第2个要比较的整数	
	@return	INT -  1     p1 > pt2
                   0     p1 = p2
                  -1     p1 < p2
*/
INT IntCompare(void *p1, void *p2)
{
    if ( (INT)p1 > (INT)p2 )
    {
        return 1;
    }
    else if ( (INT)p1 < (INT)p2 )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

