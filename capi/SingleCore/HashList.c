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

/**	��ϣ����Ĵ�������

	@param	UINT uBucketCount - ������Ĵ�С	
	@return	HASHLIST * - �ɹ����ع�ϣ���ָ�룬ʧ�ܷ���NULL	
*/
HASHLIST *HashList_Create(UINT uBucketCount)
{
    HASHLIST    *pHashList;

    /* ����uBucketCount��Сֵ���ܵ���MINIUM_BUCKET_COUNT */
    if ( uBucketCount < MINIUM_BUCKET_COUNT )
    {
        uBucketCount = MINIUM_BUCKET_COUNT;
    }

    pHashList = (HASHLIST *)malloc(sizeof(HASHLIST));
    if ( pHashList != NULL)
    {
        /* ������ϣ������Ĺ�ϣ�����������ʼ����ϣ�� */
        pHashList->ppBuckets = (HASHLISTNODE **)malloc(uBucketCount 
            * sizeof(HASHLISTNODE *));
        if ( pHashList->ppBuckets == NULL )
        {
            free(pHashList);
            return NULL;
        }
        memset((void *)pHashList->ppBuckets, 0, 
            uBucketCount * sizeof(HASHLISTNODE *));

        /* ��ʼ����ϣ�������˫������ */
        pHashList->pHead = NULL;
        pHashList->pTail = NULL;
        pHashList->uBucketCount = uBucketCount;
        pHashList->uNodeCount = 0;
    }

    return pHashList;
}


/**	��ϣ������ͷź���

	@param	HASHLIST *pHashList - ��ϣ����ָ��	
	@param  DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
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


/**	��ϣ��������ݲ��뺯����ͬʱ���뵽��ϣ��������У�
    ��������ʱ�ǲ����������ͷ��

	@param	HASHLIST *pHashList - ��ϣ����ָ��	
	@param	void *pData - Ҫ���������ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT HashList_InsertHead(HASHLIST *pHashList, void *pData, HASHFUNC HashFunc)
{
    HASHLISTNODE    *pNode;
    UINT            uBucketIndex;

    if ( pHashList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* ���ɹ�ϣ����Ľڵ� */
    pNode = (HASHLISTNODE *)malloc(sizeof(HASHLISTNODE));
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;
    pNode->pBucketNext = NULL;
    pNode->pListPrev = NULL;
    pNode->pListNext = pHashList->pHead;

    /* ���뵽��ϣ���� */
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

    /* ���뵽������ */
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


/**	��ϣ����ĵ����ڵ�ɾ��������Ҫͬʱ�ӹ�ϣ���������ɾ��

	@param	HASHLIST *pHashList - ��ϣ����ָ��	
	@param  void           *pData - ����ָ��	
	@param  HASHFUNC        HashFunc - ��ϣ����	
	@param  COMPAREFUNC     CompareFunc - ���ݱȽϺ���	
	@param  DESTROYFUNC     DestroyFunc - �����ͷź���	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�		
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

            /* ��������ɾ���ڵ� */
            if ( pNode->pListPrev != NULL )
            {
                pNode->pListPrev->pListNext = pNode->pListNext;
            }
            else
            {
                /* pNode ������ͷָ�� */
                pHashList->pHead = pNode->pListNext;
            }

            if ( pNode->pListNext != NULL )
            {
                pNode->pListNext->pListPrev = pNode->pListPrev;
            }
            else
            {
                /* ����������β�� */
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


/**	��ϣ����Ĳ��ҽڵ㺯��

	@param	HASHLIST *pHashList - ��ϣ����ָ��	
	@param	void *pData - Ҫ���ҵ�����ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	HASHLISTNODE * - �ɹ����ز��ҵ��Ĺ�ϣ����ڵ�ָ�룬
                             ʧ�ܷ���NULL	
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
            /* ����ƥ��Ľڵ㣬���ؽڵ�ָ�� */
            return pNode;
        }
        pNode = pNode->pBucketNext;
    }

    /* û���ҵ�������£�����NULL */
    return NULL;
}

 
/**	��ϣ����Ĳ������ݺ���

	@param	HASHLIST *pHashList - ��ϣ����ָ��	
	@param	void *pData - Ҫ���ҵ�����ָ��	
	@param	HASHFUNC HashFunc - ��ϣ����	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	void * - �ɹ����ز��ҵ�������ָ�룬ʧ�ܷ���NULL	
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

    /* �ӹ�ϣ���в���ƥ������ */
    while ( pNode != NULL )
    {
        if ( (*CompareFunc)(pNode->pData, pData) == 0 )
        {
            /* ����ƥ��Ľڵ㣬���ؽڵ�ָ�� */
            return pNode->pData;
        }
        pNode = pNode->pBucketNext;
    }

    /* û���ҵ�������£�����NULL */
    return NULL;
}


/**	��ϣ����Ĳ������������ò��������㷨�Թ�ϣ������������������

	@param	HASHLIST *pHashList - ��ϣ����ָ��	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ���	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ� 	
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
		/* û�нڵ��������У����������Ѿ��ź����� */
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

                    /* ���ڵ㵯���� */
                    pCurNode = pNode->pListNext;
                    pNode->pListNext = pNode->pListNext->pListNext;
                    if ( pCurNode->pListNext != NULL )
                    {
                        pCurNode->pListNext->pListPrev = pNode;
                    }

                    /* ���ڵ���뵽��Ӧλ���� */
                    pCurNode->pListNext = pTempNode->pListNext;
                    pCurNode->pListPrev = pTempNode;
                    pTempNode->pListNext->pListPrev = pCurNode;
                    pTempNode->pListNext = pCurNode;
					
					break;
				}
				pTempNode = pTempNode->pListPrev;
			}

			/* ����������ݶ����ڸýڵ����ݣ����ýڵ���뵽����ͷ�� */
			if ( pTempNode == NULL )
			{
                HASHLISTNODE    *pCurNode;

                /* ���ڵ㵯���� */
                pCurNode = pNode->pListNext;
                pNode->pListNext = pNode->pListNext->pListNext;
                if ( pCurNode->pListNext != NULL )
                {
                    pCurNode->pListNext->pListPrev = pNode;
                }

                /* ���ڵ��������ͷ�� */
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
