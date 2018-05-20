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
#include "CapiGlobal.h"
#include "HashList.h"

#pragma warning(disable : 4996)

/**	哈希链表的创建函数

	@param	UINT uBucketCount - 索引表的大小	
	@return	HASHLIST * - 成功返回哈希表的指针，失败返回NULL	
*/
HASHLIST *HashList_Create(UINT uBucketCount)
{
    HASHLIST    *pHashList;

    /* 假设uBucketCount最小值不能低于MINIUM_BUCKET_COUNT */
    if ( uBucketCount < MINIUM_BUCKET_COUNT )
    {
        uBucketCount = MINIUM_BUCKET_COUNT;
    }

    pHashList = (HASHLIST *)malloc(sizeof(HASHLIST));
    if ( pHashList != NULL)
    {
        /* 创建哈希链表里的哈希表的索引表并初始化哈希表 */
        pHashList->ppBuckets = (HASHLISTNODE **)malloc(uBucketCount 
            * sizeof(HASHLISTNODE *));
        if ( pHashList->ppBuckets == NULL )
        {
            free(pHashList);
            return NULL;
        }
        memset((void *)pHashList->ppBuckets, 0, 
            uBucketCount * sizeof(HASHLISTNODE *));

        /* 初始化哈希表里面的双向链表 */
        pHashList->pHead = NULL;
        pHashList->pTail = NULL;
        pHashList->uBucketCount = uBucketCount;
        pHashList->uNodeCount = 0;
    }

    return pHashList;
}


/**	哈希链表的释放函数

	@param	HASHLIST *pHashList - 哈希链表指针	
	@param  DESTROYFUNC DestroyFunc - 数据释放回调函数	
	@return	void - 无	
*/
void HashList_Destroy(HASHLIST *pHashList, 
                      DESTROYFUNC DestroyFunc )
{
    UINT uIndex;
    if ( pHashList == NULL )
    {
        return;
    }

    for ( uIndex = 0; uIndex < pHashList->uBucketCount; uIndex++ )
    {
        HASHLISTNODE    *pNode;
        pNode = pHashList->ppBuckets[uIndex];
        while ( pNode != NULL )
        {
            HASHLISTNODE *pNodeToFree;

            pNodeToFree = pNode;
            pNode = pNode->pBucketNext;
            if ( DestroyFunc != NULL && pNodeToFree->pData != NULL )
            {
                (*DestroyFunc)(pNodeToFree->pData);
            }

            free(pNodeToFree);
        }
    }

    free(pHashList->ppBuckets);
    free(pHashList);

    return;
}


/**	哈希链表的数据插入函数，同时插入到哈希表和链表中，
    插入链表时是插入在链表的头部

	@param	HASHLIST *pHashList - 哈希链表指针	
	@param	void *pData - 要插入的数据指针	
	@param	HASHFUNC HashFunc - 哈希函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功	
*/
INT HashList_InsertHead(HASHLIST *pHashList, void *pData, HASHFUNC HashFunc)
{
    HASHLISTNODE    *pNode;
    UINT            uBucketIndex;

    if ( pHashList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* 生成哈希链表的节点 */
    pNode = (HASHLISTNODE *)malloc(sizeof(HASHLISTNODE));
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;
    pNode->pBucketNext = NULL;
    pNode->pListPrev = NULL;
    pNode->pListNext = pHashList->pHead;

    /* 插入到哈希表中 */
    uBucketIndex = (*HashFunc)(pData, pHashList->uBucketCount);

    if ( pHashList->ppBuckets[uBucketIndex] == NULL )
    {
        pHashList->ppBuckets[uBucketIndex] = pNode;
    }
    else
    {
        HASHLISTNODE    *pTempNode;
        pTempNode = pHashList->ppBuckets[uBucketIndex];
        while ( pTempNode->pBucketNext != NULL )
        {
            pTempNode = pTempNode->pBucketNext;
        }
        pTempNode->pBucketNext = pNode;
    }

    /* 插入到链表中 */
    if ( pHashList->pHead == NULL )
    {
        pHashList->pHead = pNode;
        pHashList->pTail = pNode;
    }
    else
    {
        pNode->pListNext = pHashList->pHead;
        pHashList->pHead->pListPrev = pNode;
        pHashList->pHead = pNode;
    }

    pHashList->uNodeCount += 1;

    return CAPI_SUCCESS;
}


/**	哈希链表的单个节点删除函数，要同时从哈希表和链表中删除

	@param	HASHLIST *pHashList - 哈希链表指针	
	@param  void           *pData - 数据指针	
	@param  HASHFUNC        HashFunc - 哈希函数	
	@param  COMPAREFUNC     CompareFunc - 数据比较函数	
	@param  DESTROYFUNC     DestroyFunc - 数据释放函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功		
*/
INT HashList_Delete(HASHLIST *pHashList, 
                    void           *pData,
                    HASHFUNC        HashFunc,
                    COMPAREFUNC     CompareFunc, 
                    DESTROYFUNC     DestroyFunc)
{
    HASHLISTNODE    *pNode;
    HASHLISTNODE    *pPrevNode;
    UINT            uIndex;

    if ( pHashList == NULL || HashFunc == NULL 
        || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }

    uIndex = (*HashFunc)(pData, pHashList->uBucketCount);
    pNode = pHashList->ppBuckets[uIndex];
    pPrevNode = NULL;

    while ( pNode != NULL )
    {
        if ( (*CompareFunc)(pNode->pData, pData ) == 0 )
        {
            if (pPrevNode == NULL )
            {
                pHashList->ppBuckets[uIndex] = pNode->pBucketNext;
            }
            else
            {
                pPrevNode->pBucketNext = pNode->pBucketNext;
            }

            /* 从链表中删除节点 */
            if ( pNode->pListPrev != NULL )
            {
                pNode->pListPrev->pListNext = pNode->pListNext;
            }
            else
            {
                /* pNode 是链表头指针 */
                pHashList->pHead = pNode->pListNext;
            }

            if ( pNode->pListNext != NULL )
            {
                pNode->pListNext->pListPrev = pNode->pListPrev;
            }
            else
            {
                /* 现在在链表尾部 */
                pHashList->pTail = pNode;
            }

            if ( pNode->pData != NULL && DestroyFunc != NULL )
            {
                (*DestroyFunc)(pNode->pData);
            }

            free(pNode);

            pHashList->uNodeCount -= 1;

            return CAPI_SUCCESS;
        }
        pPrevNode = pNode;
        pNode = pNode->pBucketNext;
    }

    return CAPI_FAILED;
}


/**	哈希链表的查找节点函数

	@param	HASHLIST *pHashList - 哈希链表指针	
	@param	void *pData - 要查找的数据指针	
	@param	HASHFUNC HashFunc - 哈希函数	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	HASHLISTNODE * - 成功返回查找到的哈希链表节点指针，
                             失败返回NULL	
*/
HASHLISTNODE *HashList_FindNode(HASHLIST *pHashList, 
                                void *pData, 
                                HASHFUNC HashFunc, 
                                COMPAREFUNC CompareFunc)
{
    HASHLISTNODE    *pNode;
    UINT            uIndex;

    if ( pHashList == NULL || HashFunc == NULL 
        || CompareFunc == NULL )
    {
        return NULL;
    }

    uIndex = (*HashFunc)(pData, pHashList->uBucketCount);
    pNode = pHashList->ppBuckets[uIndex];

    /* try to find the key from the HashTable */
    while ( pNode != NULL )
    {
        if ( (*CompareFunc)(pNode->pData, pData) == 0 )
        {
            /* 发现匹配的节点，返回节点指针 */
            return pNode;
        }
        pNode = pNode->pBucketNext;
    }

    /* 没有找到的情况下，返回NULL */
    return NULL;
}

 
/**	哈希链表的查找数据函数

	@param	HASHLIST *pHashList - 哈希链表指针	
	@param	void *pData - 要查找的数据指针	
	@param	HASHFUNC HashFunc - 哈希函数	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	void * - 成功返回查找到的数据指针，失败返回NULL	
*/
void *HashList_FindData(HASHLIST *pHashList, 
                        void *pData, 
                        HASHFUNC HashFunc, 
                        COMPAREFUNC CompareFunc)
{
    HASHLISTNODE    *pNode;
    UINT            uIndex;

    if ( pHashList == NULL || HashFunc == NULL 
        || CompareFunc == NULL )
    {
        return NULL;
    }

    uIndex = (*HashFunc)(pData, pHashList->uBucketCount);
    pNode = pHashList->ppBuckets[uIndex];

    /* 从哈希表中查找匹配数据 */
    while ( pNode != NULL )
    {
        if ( (*CompareFunc)(pNode->pData, pData) == 0 )
        {
            /* 发现匹配的节点，返回节点指针 */
            return pNode->pData;
        }
        pNode = pNode->pBucketNext;
    }

    /* 没有找到的情况下，返回NULL */
    return NULL;
}


/**	哈希链表的插入排序函数，用插入排序算法对哈希链表里的链表进行排序

	@param	HASHLIST *pHashList - 哈希链表指针	
	@param	COMPAREFUNC CompareFunc - 数据比较函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功 	
*/
INT HashList_InsertSort(HASHLIST *pHashList, COMPAREFUNC CompareFunc)
{
    HASHLISTNODE	*pNode;

    if ( pHashList == NULL || CompareFunc == NULL )
    {
        return 0;
    }

    pNode = pHashList->pHead;
    if ( pNode == NULL )
    {
		/* 没有节点在链表中，把它当作已经排好了序 */
		return 1;
    }

    while ( pNode->pListNext != NULL )
    {
		if ( (*CompareFunc)( pNode->pListNext->pData, pNode->pData ) < 0 )
		{
			HASHLISTNODE	*pTempNode;
			pTempNode = pNode->pListPrev;
			while ( pTempNode != NULL )
			{
				if ( (*CompareFunc)( pNode->pListNext->pData, 
                    pTempNode->pData ) >= 0 )
				{
                    HASHLISTNODE    *pCurNode;

                    /* 将节点弹出来 */
                    pCurNode = pNode->pListNext;
                    pNode->pListNext = pNode->pListNext->pListNext;
                    if ( pCurNode->pListNext != NULL )
                    {
                        pCurNode->pListNext->pListPrev = pNode;
                    }

                    /* 将节点插入到对应位置上 */
                    pCurNode->pListNext = pTempNode->pListNext;
                    pCurNode->pListPrev = pTempNode;
                    pTempNode->pListNext->pListPrev = pCurNode;
                    pTempNode->pListNext = pCurNode;
					
					break;
				}
				pTempNode = pTempNode->pListPrev;
			}

			/* 如果所有数据都大于该节点数据，将该节点插入到链表头部 */
			if ( pTempNode == NULL )
			{
                HASHLISTNODE    *pCurNode;

                /* 将节点弹出来 */
                pCurNode = pNode->pListNext;
                pNode->pListNext = pNode->pListNext->pListNext;
                if ( pCurNode->pListNext != NULL )
                {
                    pCurNode->pListNext->pListPrev = pNode;
                }

                /* 将节点插入链表头部 */
                pCurNode->pListPrev = NULL;
                pCurNode->pListNext = pHashList->pHead;
                pHashList->pHead = pCurNode;
			}
		}
		else
		{
			pNode = pNode->pListNext;
		}
    }

    return 1;
}



/*
 *	HashString( void *str )
 *	Calculate the hash value of a string.
 *	Parameters:
 *		void *str,		the string that need calculate.
 *	Return Values:
 *		the hash value of the string.
 */
UINT HashStr( void *str, UINT str_len, UINT numBuckets )
{
    char	*s;
//    int		i;
    int		hashval;
	int		ret;
	int		j;

//	strupr( (char *)str );
    s = (char *)str;
    hashval = 0;

#if 0
    for ( i = 0; i < 5 && s[i] != '\0'; i++ )
    {
		hashval += hashval << 3;
		hashval += s[i] ;
    }
#endif
	j = 0;
	ret = 0;
    while ( *s != '\0' )
	{
		if ( j == 5 )
		{
			j = 0;
			ret += hashval;
			hashval = 0;
		}
		hashval += hashval << 3;
//		hashval += tolower( (unsigned char)*s );
		s++;
		j++;
	}
	ret += hashval;

    return ret % numBuckets;
}

/*
 *	StrCompare( )
 *	Compare if two string is equal.
 *	Return Values:
 *		1		equal
 *		0		not equal
 */
UINT	HashStrCompare( void *str1, UINT str1_len, void *str2, UINT str2_len )
{
	return stricmp( (char *)str1, (char *)str2 );
}

void HashFree(void *pData, UINT uDataLen)
{
    free( pData );
}
