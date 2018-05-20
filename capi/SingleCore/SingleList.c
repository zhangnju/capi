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
#include "SingleList.h"

 
/**	��������Ĵ���������������������ǿյ�û�нڵ�������

	@param	void - ��	
	@return	SINGLELIST * - ʧ�ܷ���NULL, �ɹ�ʱ����һ����������ṹ��ָ��	
*/
SINGLELIST * SingleList_Create( void )
{
    SINGLELIST  *pSingleList;

    /* �����ڴ���� */
    pSingleList = (SINGLELIST *)malloc(sizeof(SINGLELIST));
    if ( pSingleList != NULL )
    {
        /* ��ʼ������ṹ���ָ���ԱΪ�գ�����ڵ����Ϊ0 */
        pSingleList->pCur = NULL;
        pSingleList->pHead = NULL;
        pSingleList->pTail = NULL;
        pSingleList->uCount = 0;
    }

    return pSingleList;
}


/**	����������ͷź���

	@param	SINGLELIST *pSingleList - Ҫ�ͷŵĵ��������ָ��	
	@param	DESTROYFUNC pDestroyFunc - ����ڵ������ͷŻص�����	
	@return	void - ��	
*/
void SingleList_Destroy( SINGLELIST *pSingleList, DESTROYFUNC DestroyFunc )
{
    SINGLENODE  *pNode;

    if ( pSingleList )
    {
        /* ��ͷ�ڵ㿪ʼ��һ����һ���ͷ�����ڵ㼰�ڵ����� */
        pNode = pSingleList->pHead;
        while ( pNode != NULL )
        {
            SINGLENODE  *pDelNode;

            pDelNode = pNode;
            pNode = pNode->pNext;

            if ( DestroyFunc != NULL && pDelNode->pData != NULL )
            {
                /* �ͷ����� */
                (*DestroyFunc)( pDelNode->pData );
            }
            free( pDelNode ); /* �ͷŽڵ� */
        }
        /* �ͷ�����ṹ�� */ 
        free( pSingleList );
    }
}


/**	�����������ӽڵ㺯������ӵĽڵ���ڵ��������ͷ��

	@param	SINGLELIST *pSingleList - Ҫ��ӵĵ�������ָ��	
	@param	void *pData - Ҫ��ӵĽڵ������ָ��	
	@return	INT - ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS	
*/
INT	SingleList_InsertHead( SINGLELIST *pSingleList, void *pData )
{
    SINGLENODE  *pNode;

    /* ����У�� */
    if ( pSingleList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* �½�һ���ڵ� */
    pNode = (SINGLENODE *)malloc( sizeof(SINGLENODE) );
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;  /* ���ڵ�����ָ��ָ�򴫽��������� */

    /* ���½��ڵ����һ�ڵ�ָ��ָ��ͷ�ڵ㣬����ͷ�ڵ��Ϊ�½��Ľڵ� */
    pNode->pNext = pSingleList->pHead;
    pSingleList->pHead = pNode;

    /*
     * �ж��Ƿ�β�ڵ�ָ��Ϊ�գ����Ϊ�ձ�ʾԭ��������û�нڵ㣬
     * ��ʱӦ�ý�β�ڵ�ָ���¼���Ľڵ� 
     */
    if ( pSingleList->pTail == NULL )
    {
        pSingleList->pTail = pNode;
    }

    /* ������ڵ����ݼ�1 */
    pSingleList->uCount++;
    
    return CAPI_SUCCESS;
}


/**	�����������ӽڵ㺯������ӵĽڵ���ڵ��������β��

	@param	SINGLELIST *pSingleList - Ҫ��ӵĵ�������ָ��	
	@param	void *pData - Ҫ��ӵĽڵ������ָ��
	@return	INT - ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS	
*/
INT	SingleList_InsertTail( SINGLELIST *pSingleList, void *pData )
{
    SINGLENODE  *pNode;

    /* ����У�� */
    if ( pSingleList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* �½�һ���ڵ� */
    pNode = (SINGLENODE *)malloc( sizeof(SINGLENODE) );
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;  /* ���ڵ�����ָ��ָ�򴫽��������� */
    pNode->pNext = NULL;   /* ���ڵ����һ�ڵ㸳Ϊ��ָ��NULL */

    /*
     * �ж��Ƿ�β�ڵ�ָ��Ϊ�գ����Ϊ�ձ�ʾԭ��������û�нڵ㣬
     * ��ʱӦ�ý�β�ڵ�ָ���¼���Ľڵ�, ����ͷ�ڵ�ָ��ҲӦ��ָ���½ڵ� 
     */
    if ( pSingleList->pTail == NULL )
    {
        pSingleList->pTail = pNode;
        pSingleList->pHead = pNode;
    }
    else
    {
        /*
         * ���β�ڵ�ָ�벻Ϊ�գ���ʱӦ�ý�β�ڵ���һ�ڵ�ָ��ָ���¼���Ľڵ�, 
         * ����β�ڵ�ָ��ҲӦ��ָ���½ڵ� 
         */
        pSingleList->pTail->pNext = pNode;
        pSingleList->pTail = pNode;
    }

    /* ������ڵ����ݼ�1 */
    pSingleList->uCount++;
    
    return CAPI_SUCCESS;
}


/**	��������ĵ���ͷ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	void * - ʧ�ܷ���NULL, �ɹ�����Ҫ������ͷ�ڵ������ָ��	
*/
void *	SingleList_PopHead( SINGLELIST *pSingleList )
{
	SINGLENODE	*pPopNode;   /* ����ָ��Ҫ�������ݵĽڵ��ָ�� */
	void		*pPopData;   /* ����ָ��Ҫ���������ݵ�ָ�� */

    /* ����У�� */
    if ( pSingleList == NULL || pSingleList->pHead == NULL ) 
    {
		return NULL;
    }

    /* ��Ҫ�������ݵĽڵ�ָ��ָ������ͷ�ڵ㣬��������ָ��ָ��ͷ�ڵ������ */
    pPopNode = pSingleList->pHead;
    pPopData = pPopNode->pData;

    /* 
     * �жϵ�ǰ�ڵ�ָ���Ƿ�ָ��ͷ�ڵ㣬���ָ��ͷ�ڵ�����Ҫ����
     * ָ��ͷ�ڵ����һ�ڵ� 
     */
    if ( pSingleList->pCur == pSingleList->pHead )
    {
        pSingleList->pCur = pSingleList->pHead->pNext;
    }

    /* ��ͷ�ڵ�ָ��ָ��ͷ�ڵ����һ�ڵ� */
    pSingleList->pHead = pSingleList->pHead->pNext;
	
    /* ������ڵ�������1 */
    pSingleList->uCount--;

    /* �������Ľڵ������Ѿ�Ϊ0���ʾԭ��ֻ��һ���ڵ㣬����ͷ�ڵ��
     * �����Ѿ�Ϊ�գ�û�нڵ������棬��ʱӦ�ý�β�ڵ�ָ�븳��
     * ��ǰ�ڵ�ָ������ǰ���Ѿ�������ˣ����ֻ��һ���ڵ�Ļ��϶�Ϊ��
     * �������ﲻ��Ҫ����ǰ�ڵ�ָ��
     */
    if ( pSingleList->uCount == 0 ) {
		pSingleList->pTail = NULL;
	}
	
    /* �ͷŵ����Ľڵ�, ע�����ﲢû���ͷŽڵ�����ָ�� */
    free( pPopNode );

    return pPopData;    /* ����ͷ�ڵ������ָ�� */
}


/** ��������ĵ���β�ڵ㺯��
    �����������Ҫ����β�ڵ��ǰһ�ڵ㣬���Ч�ʺܵ�

    @param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
    @return	void * -  ʧ�ܷ���NULL, �ɹ�����Ҫ������β�ڵ������ָ��	
*/
void *	SingleList_PopTail( SINGLELIST *pSingleList )
{
    SINGLENODE	*pPopNode;   /* ����ָ��Ҫ�������ݵĽڵ��ָ�� */
    SINGLENODE	*pTailPrevNode;   /* ����ָ��β�ڵ����һ�ڵ��ָ�� */
    void		*pPopData;   /* ����ָ��Ҫ���������ݵ�ָ�� */

    /* ����У�� */
    if ( pSingleList == NULL || pSingleList->pHead == NULL ) 
    {
        return NULL;
	}

    /* ��Ҫ�������ݵĽڵ�ָ��ָ������ͷ�ڵ㣬��������ָ��ָ��ͷ�ڵ������ */
    pPopNode = pSingleList->pTail;
    pPopData = pPopNode->pData;

    pTailPrevNode = pSingleList->pHead;

    /* ��������ͷ�ڵ��β�ڵ���ͬ���ʾԭ��ֻ��һ���ڵ㣬����β�ڵ��
     * ��ʱ�����Ѿ�Ϊ�գ�û�нڵ������棬��ʱӦ�ý�ͷ�ڵ�ָ�븳��
     * ��ǰ�ڵ�ָ������ǰ���Ѿ�������ˣ����ֻ��һ���ڵ�Ļ��϶�Ϊ��
     * �������ﲻ��Ҫ����ǰ�ڵ�ָ��
     */
    if ( pSingleList->pTail == pSingleList->pHead )
    {
        pTailPrevNode = NULL;
	    pSingleList->pHead = NULL;
    }
    else 
    {
        while ( pTailPrevNode != NULL )
        {
            if ( pTailPrevNode->pNext == pSingleList->pTail )
            {
                break;
            }
            pTailPrevNode = pTailPrevNode->pNext;
        }
    }

    /* 
     * �жϵ�ǰ�ڵ�ָ���Ƿ�ָ��β�ڵ㣬���ָ��ͷ�ڵ�Ļ�����Ҫ����
     * ָ��β�ڵ��ǰһ�ڵ� 
     */
    if ( pSingleList->pCur == pSingleList->pTail )
    {
        pSingleList->pCur = pTailPrevNode;
    }

    /* ��β�ڵ�ָ��ָ��β�ڵ��ǰһ�ڵ� */
    pSingleList->pTail = pTailPrevNode;

    if ( pTailPrevNode != NULL )
    {
        pTailPrevNode->pNext = NULL;
    }
    
    /* ������ڵ�������1 */
    pSingleList->uCount--;

    /* �ͷŵ����Ľڵ�, ע�����ﲢû���ͷŽڵ�����ָ�� */
    free( pPopNode );

    return pPopData;    /* ����ͷ�ڵ������ָ�� */    
}


/**	�����ɾ���ڵ㺯��������ɾ����pMatchData��������ͬ���ݵĽڵ�
    ������������ͬ���ݵĽڵ�����ֻɾ����һ������ͬ���ݵĽڵ�

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@param	void *pMatchData - Ҫɾ���ڵ��ƥ������	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ��������Ƚ�pMatchData����������
                                      �ڵ�����Ƿ����	
	@param	DESTROYFUNC DestroyFunc - ����ڵ�������ͷź���	
	@return	INT (by default) - CAPI_FAILED��ʾʧ�ܻ�������û��ƥ������ݣ�CAPI_SUCCESS��ʾ�ɹ�ɾ��	
*/
INT	 SingleList_Delete( SINGLELIST *pSingleList,  
                        void *pMatchData, 
                        COMPAREFUNC CompareFunc, 
                        DESTROYFUNC DestroyFunc )
{
    SINGLENODE  *pNode;
    SINGLENODE  *pPrevNode;

    /* ����У�� */
    if ( pSingleList == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pSingleList->pHead;
    pPrevNode = pNode;

    while ( pNode != NULL )
    {
        /* �ȽϽڵ������Ƿ��ƥ������ƥ�� */
        if ( (*CompareFunc)( pNode->pData, pMatchData ) == 0 )
        {
            if ( pPrevNode == pNode )
            {
                /* ͷ�ڵ�ƥ�����ˣ���Ҫɾ��ͷ�ڵ� */
                pSingleList->pHead = pNode->pNext;
                if ( pSingleList->pTail == pNode )
                {
                    /* 
                     * ���β�ڵ��pNode��ͬ������������ֻ��һ���ڵ�
                     * ��ʱ��Ҫ������β�ڵ�ָ�������ǰ�ڵ�ָ�븳��
                     */
                    pSingleList->pTail = NULL;
                    pSingleList->pCur = NULL;
                }
            }
            else
            {
                pPrevNode->pNext = pNode->pNext;
                if ( pSingleList->pTail == pNode )
                {
                    /* 
                     * ���β�ڵ��pNode��ͬ������ɾ������β�ڵ�
                     * ��ʱ��Ҫ��β�ڵ�ָ��ָ��Ҫɾ���ڵ��ǰһ���ڵ�
                     */
                    pSingleList->pTail = pPrevNode;
                }

                if ( pSingleList->pCur == pNode )
                {
                    /* 
                     * �������ǰ�ڵ��pNode��ͬ������ɾ�����ǵ�ǰ�ڵ�
                     * ��ʱ��Ҫ����ǰ�ڵ�ָ��ָ��Ҫɾ���ڵ����һ���ڵ�
                     */
                    pSingleList->pCur = pNode->pNext;
                }
            }

            /* �ͷŽڵ����ݺͽڵ�ռ�õ��ڴ� */
            if ( DestroyFunc != NULL && pNode->pData != NULL )
            {
                (*DestroyFunc)( pNode->pData );
            }
            free( pNode );

            break;
        }
        pPrevNode = pNode;
        pNode = pNode->pNext;
    }

    return CAPI_SUCCESS;
}


/**	��������Ļ�ȡָ��λ�����ݵĺ���

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@param	UINT uIndex - Ҫ��ȡ������λ�ã�������0��ʼ����	
	@return	void * - ����λ�ýڵ������ָ��	
*/
void *  SingleList_GetAt( SINGLELIST *pSingleList, UINT uIndex )
{
    UINT        i;
    SINGLENODE  *pNode;

    if ( pSingleList == NULL || pSingleList->uCount >= uIndex )
    {
        return NULL; 
    }

    pNode = pSingleList->pHead;
    for ( i = 0; i < uIndex; i++ )
    {
        pNode = pNode->pNext;
    }

    return pNode->pData;
}


/**	��������Ļ�ȡ����ڵ���������

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	UINT - ����ڵ�������Ϊ0��ʾ�����ǿյ�û�нڵ�������������߲����Ƿ�	
*/
UINT    SingleList_GetCount(SINGLELIST *pSingleList)
{
    if ( pSingleList == NULL )
    {
        return 0;
    }
    return pSingleList->uCount;
}


/**	��������Ļ�ȡͷ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	void * - ͷ�ڵ������ָ��	
*/
void *	SingleList_GetHead( SINGLELIST *pSingleList )
{
    if ( pSingleList == NULL )
    {
        return NULL;
    }

    if ( pSingleList->pHead == NULL )
    {
        return NULL;
    }

    return pSingleList->pHead->pData;
}


/**	��������Ļ�ȡ��ǰ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ�� 	
	@return	void * - ��ǰ�ڵ������ָ��	
*/
void *	SingleList_GetCursor( SINGLELIST *pSingleList )
{
    if ( pSingleList == NULL )
    {
        return NULL;
    }

    if ( pSingleList->pCur == NULL )
    {
        return NULL;
    }

    return pSingleList->pCur->pData;
}


/**	��������Ļ�ȡβ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	void * - β�ڵ������ָ��	
*/
void * 	SingleList_GetTail( SINGLELIST *pSingleList )
{
    if ( pSingleList == NULL )
    {
        return NULL;
    }

    if ( pSingleList->pTail != NULL )
    {
        return pSingleList->pTail->pData;
    }
    else
    {
        return NULL;
    }
}


/**	���������ö�ٳ�ʼ������

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��
	@return	void - ��	
*/
void SingleList_EnumBegin( SINGLELIST *pSingleList )
{
    pSingleList->pCur = pSingleList->pHead;

    return;
}


/**	��������ö����һ���ڵ�ĺ�������һ�ε��ô˺���ǰ����
    �ȵ���SingleList_EnumBegin()����

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ��������ָ��	
	@return	void * - ö�ٵ��Ľڵ�����ָ��	
*/
void *  SingleList_EnumNext( SINGLELIST *pSingleList )
{
    SINGLENODE  *pCur;

    pCur = pSingleList->pCur;
    if ( pCur != NULL )
    {
        pSingleList->pCur = pCur->pNext;

        return pCur->pData;
    }
    return NULL;
}



/**	��������ı�������

	@param	SINGLELIST *pSingleList - Ҫ������������ָ��	
	@param	TRAVERSEFUNC TraverseFunc - �ڵ����ݵı�����������	
	@return	INT - �ɹ�����CAPI_SUCCESS��ʧ�ܷ���CAPI_FAILED	
*/
INT SingleList_Traverse( SINGLELIST *pSingleList, TRAVERSEFUNC TraverseFunc )
{
    SINGLENODE  *pNode;

    /* ����У�� */
    if ( pSingleList == NULL || TraverseFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pSingleList->pHead;
    
    /* ��ʼ�������� */
    while ( pNode != NULL )
    {
        (*TraverseFunc)( pNode->pData ); /* ���ñ����ص������������� */
        pNode = pNode->pNext;
    }

    return CAPI_SUCCESS;
}


/**	��������Ĳ���������
    �����ǰ��մ�С����������У�����˵�Ĵ�С����CompareFunc��������
    ����û�����ͨ��CompareFunc�ķ���ֵ����������ʹ��˳���������������

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@param	COMPAREFUNC CompareFunc - �ڵ����ݱȽϺ���	
	@return	INT - �ɹ�����CAPI_SUCCESS��ʧ�ܷ���CAPI_FAILED	
*/
INT SingleList_InsertSort( SINGLELIST *pSingleList, COMPAREFUNC CompareFunc )
{
    SINGLENODE  *pNode;     /* ��������pSingleList����ʱָ�� */
    SINGLENODE  *pPrevNode; /* pNode��ǰһ�ڵ�ָ�� */

    /* ����У�� */
    if ( pSingleList == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pSingleList->pHead;
    pPrevNode = NULL;

    if ( pNode == NULL )
    {
        /* ������û�нڵ㣬���ǰ��������Ѿ��ź����� */
        return CAPI_SUCCESS;
    }

    while ( pNode->pNext != NULL )
    {
        SINGLENODE  *pTempNode;

        pTempNode = pSingleList->pHead;
        pPrevNode = NULL;
        while ( pTempNode != pNode->pNext )
        {
            if ( (*CompareFunc)( pNode->pNext->pData, pTempNode->pData ) < 0 )
            {
                SINGLENODE  *pCurNode = pNode->pNext;

                /* ִ�в������ */
                if ( pPrevNode != NULL ) /* ���벻��ͷ�ڵ�ǰ����� */
                {
                    /* ��pCurNode�ڵ㵯���� */
                    pNode->pNext = pNode->pNext->pNext;

                    /* ��pNode->pNext����pTempNode ֮ǰ */
                    pPrevNode->pNext = pCurNode;
                    pCurNode->pNext = pTempNode;
                }
                else /* ������ͷ�ڵ�ǰ����� */
                {
                    /* ��pCurNode�ڵ㵯���� */
                    pNode->pNext = pNode->pNext->pNext;

                    /* ��pNode->pNext�ڵ��Ϊͷ�ڵ� */
                    pSingleList->pHead = pCurNode;
                    pCurNode->pNext = pTempNode;
                }

                /* �޸�βָ��ָ�� */
                if ( pCurNode == pSingleList->pTail )
                {
                    pSingleList->pTail = pNode;
                }

                break;
            }

            pPrevNode = pTempNode;
            pTempNode = pTempNode->pNext;
        } /* while ( pTempNode != pNode->pNext ) */

        if ( pTempNode == pNode->pNext ) /* û�в������� */
        {
            pNode = pNode->pNext;
        }
        else /* �Ѿ��������� */
        {
            /* �Ѿ�������轫pNodeָ����ƣ���Ϊǰ������������Ѿ��ƶ����� */
        }
    }

    return CAPI_SUCCESS;
}


/**	��һ����������ֳ�������������
    ֻ�Ǵ�ԭ�������м�Ͽ��������ԭ�����ɷֿ���ĵ�һ������

	@param	SINGLELIST *pSingleList - Ҫ�������ĵ�������	
	@param	UINT nCount - �ֿ���ĵ�һ������Ľڵ����	
	@return	SINGLELIST * - �ɹ�ʱ���طֿ���ĵڶ�������ʧ�ܷ���NULL��	
*/
SINGLELIST * SingleList_Split(SINGLELIST *pSingleList, UINT uCount)
{
    SINGLENODE	*pNode;       /* ��������ֿ����Ľڵ��ָ�� */
	SINGLELIST  *pSecondList; /* ������¼�ֿ���ĵڶ��������ָ�� */
	UINT    	uIndex;       /* ��ʱѭ������� */

	if ( pSingleList == NULL )
	{
		return NULL;
	}
	
    /* ����У�� */
    if ( uCount == 0 || pSingleList->uCount <= uCount)
	{
		return NULL;
	}

    /* ����һ�������� */
    pSecondList = SingleList_Create();
    if ( pSecondList == NULL )
    {
        return NULL;
    }

	/* ��ȡҪ�ֿ���λ�� */
    pNode = pSingleList->pHead;
    for ( uIndex = 1; uIndex < uCount; uIndex++ )
    {
        pNode = pNode->pNext;
    }
	
	/* ���ڶ������������. */
    pSecondList->pHead = pNode->pNext;
    pSecondList->pTail = pSingleList->pTail;
    pSecondList->uCount = pSingleList->uCount - uCount;

	/* �޸ĵ�һ�����������. */
    pSingleList->pTail = pNode;
    pSingleList->uCount = uCount;

	/* ����һ������β�ڵ��pNextָ��ָ��NULL */
    pSingleList->pTail->pNext = NULL;

    return pSecondList;
}


/**	�������Ѿ��ź����������кϲ�
    �����������밴��С����������У���β�ڵ����
    �ϲ��������Ҳ�Ǵ�С�����������

	@param	SINGLELIST *pSingleListA - Ҫ�ϲ�������	
	@param	SINGLELIST *pSingleListB -Ҫ�ϲ�������	
	@param	COMPAREFUNC CompareFunc - �ڵ�ȽϺ���	
	@return	INT - CAPI_FAILED ��ʾʧ�ܣ�ʧ��ʱ������û�б仯
                  CAPI_SUCCESS ��ʾ�ɹ����ɹ�ʱpSingleListA�Ǻϲ���Ľ����
                  pSingleListB�ᱻ�ͷ�
*/
INT SingleList_Merge(SINGLELIST *pSingleListA, 
                     SINGLELIST *pSingleListB, 
                     COMPAREFUNC CompareFunc)
{
    SINGLENODE	*pNodeA; /* ����ָ������pSingleListA�Ľڵ����ʱָ�� */
    SINGLENODE  *pNodeB; /* ����ָ������pSingleListB�Ľڵ����ʱָ�� */
    SINGLENODE  *pPrevA; /* pNodeA��ǰһ�ڵ�ָ�� */

    /*  ����У�� */
    if ( pSingleListA == NULL || pSingleListB == NULL || CompareFunc == NULL )
    {
		return CAPI_FAILED;
    }

    pNodeA = pSingleListA->pHead;
    pNodeB = pSingleListB->pHead;
    pPrevA = NULL;

    while ( pNodeB != NULL )
    {
        while ( pNodeA != NULL )
		{
            if ( (*CompareFunc)( pNodeA->pData, pNodeB->pData ) >= 0 )
			{
                SINGLENODE *pNode;
				
				/* ��pNodeB���������浽pNode�� */
                pNode = pNodeB;
                pNodeB = pNodeB->pNext;
				
				/* ��pNode���뵽pNodeAǰ�� */
                if ( pPrevA == NULL )
                {
                    /* ������ͷָ��ǰ���������Ҫ�޸�ͷָ�� */
                    pSingleListA->pHead = pNode;
                    pNode->pNext = pNodeA;
                }
                else
                {
                    pPrevA->pNext = pNode;
                    pNode->pNext = pNodeA;
                }
                pPrevA = pNode;
			
				break;
			}
            pPrevA = pNodeA;
			pNodeA = pNodeA->pNext;
		}

		/*
         * ���pSingleListB���������ݶ���������A�����ݣ�
         * ��pSingleListB����pSingleListAβ��.
         */
        if ( pNodeA == NULL )
		{
			pSingleListA->pTail->pNext = pNodeB;
			pSingleListA->pTail = pSingleListB->pTail;
			
			break;
		}				
    }
	
    /* �޸�pSingleListA�Ľڵ������� */
    pSingleListA->uCount += pSingleListB->uCount;

	free( pSingleListB );

	return CAPI_SUCCESS;
}


/**	������ʹ�ù鲢�������򣬹鲢�Ͳ���������ʹ�ã���ʹ�ù鲢����
   ����������Ԫ����������С��һ��ֵʱ��ʹ�ò�������
    
	@param	SINGLELIST *pSingleList - Ҫ���������ָ��	
	@param	COMPAREFUNC CompareFunc - ����ڵ�ȽϺ���	
	@param	UINT uInsertSortCount - ʹ�ò�������ʱ������ڵ������������ڵ�
                                    ����С�����ֵʱ��ʹ�ò��������㷨
                                    
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT SingleList_MergeSort(SINGLELIST *pSingleList, 
                         COMPAREFUNC CompareFunc, 
                         UINT uInsertSortCount)
{
   	SINGLELIST	 *pSecondList;

    /* ����У�� */
	if ( pSingleList == NULL || CompareFunc == NULL )
	{
		return CAPI_FAILED;
	}

    if ( pSingleList->uCount < 2 )
    {
        /* ����ڵ����������������Ϊ���Ѿ��ź���� */
        return CAPI_SUCCESS;
    }

    /*
     * �������ڵ����С�ڸ���������������ĸ�����ֱ��ʹ�ò������� 
	 */
    if ( pSingleList->uCount <= uInsertSortCount )
	{
		(void)SingleList_InsertSort( pSingleList, CompareFunc );
	}
    else
	{
		/* ������ֳ����� */
        pSecondList = SingleList_Split(pSingleList, (pSingleList->uCount) / 2 );
		
        /* �Է����ĵ�һ��������еݹ���ù鲢���� */
        (void)SingleList_MergeSort( pSingleList, CompareFunc, uInsertSortCount );
		
        /* �Է����ĵڶ���������еݹ���ù鲢���� */
		(void)SingleList_MergeSort( pSecondList, CompareFunc, uInsertSortCount );

		/* ���ź������������ϳ�һ��. */
		(void)SingleList_Merge( pSingleList, pSecondList, CompareFunc );
	}

    return CAPI_SUCCESS; 
}

/**	����������ݵĵ�uKeyIndexλ�ϵ�Ԫ�ؽ��з��࣬
    �������ǵĴ�С�����Ӧ��������

	@param	SINGLELIST *pSingleList - ��������ָ��	
	@param	UINT       uRadix - ��������Ļ�������������������йأ�
                                һ�����������Ļ���Ϊ16���ַ����Ļ������Ϊ255��
	@param	UINT       uKeyIndex - �ڶ���λ	
	@param	SINGLENODE **ppHead - ������¼ͷָ�������	
	@param	SINGLENODE **ppTail - ��¼���ӵ�βָ��	
	@param	GETKEYFUNC GetKeyFunc - ��ȡ���ݵĵ�uKeyIndexλ�ϵ�Ԫ��ֵ	
	@return	void - ��	
*/
static void SingleList_Distribute(SINGLELIST *pSingleList,
                          UINT       uRadix,
                          UINT       uKeyIndex,
                          SINGLENODE **ppHead,
                          SINGLENODE **ppTail,
                          GETKEYFUNC GetKeyFunc )
{
    SINGLENODE   *pNode;
    UINT         i;

    /* ��ʼ���ӱ� */
    for ( i = 0; i < uRadix; i++ )
    {
        ppHead[i] = NULL;
        ppTail[i] = NULL;
    }

    pNode = pSingleList->pHead;

    while ( pNode != NULL )
    {
        UINT uRadixIndex = (*GetKeyFunc)(pNode->pData, uKeyIndex);
         
        if ( ppHead[uRadixIndex] == NULL )
        {
           ppHead[uRadixIndex] = pNode;
           ppTail[uRadixIndex] = pNode;
           pNode = pNode->pNext;
           ppTail[uRadixIndex]->pNext = NULL;
        }
        else
        {
            ppTail[uRadixIndex]->pNext = pNode;
            ppTail[uRadixIndex] = ppTail[uRadixIndex]->pNext;
            pNode = pNode->pNext;
            ppTail[uRadixIndex]->pNext = NULL;
        }
    }
}


/**	�Ի��������зֺ�������ӽ����ռ���������������������һ������

	@param	SINGLELIST *pSingleList - ��������ָ��	
	@param	UINT        uRadix - ����	
	@param	SINGLENODE **ppHead - ������¼ͷָ�������	
	@param	SINGLENODE **ppTail - ��¼���ӵ�βָ��	
	@return	void - �ޡ�
*/
static void SingleList_Collect(SINGLELIST *pSingleList,
                       UINT        uRadix,
                       SINGLENODE **ppHead,
                       SINGLENODE **ppTail )
{
    SINGLENODE  *pHead;
    SINGLENODE  *pTail;
    UINT        uRadixIndex;

    /* �����1���ǿ��ӱ� */
    uRadixIndex = 0;
    while ( uRadixIndex < uRadix )
    {
        if ( ppHead[uRadixIndex] == NULL )
        {
            uRadixIndex++;
            continue;
        }
        else
        {
            break;
        }
    }

    if ( uRadixIndex == uRadix )
    {
        /* û���ҵ��ǿ��ӱ� */
        return ;
    }

    pHead = ppHead[uRadixIndex];
    pTail = ppTail[uRadixIndex];

    while ( uRadixIndex < uRadix - 1 )
    {
        /* ����������һ���ǿ��ӱ� */
        ++uRadixIndex;
        if ( ppHead[uRadixIndex] == NULL )
        {
            continue;
        }

        if ( uRadixIndex < uRadix )
        {
            /* �ҵ��˷ǿ��ӱ���Ҫ������ǰһ���ǿ��ӱ��������� */
            pTail->pNext = ppHead[uRadixIndex];
            pTail = ppTail[uRadixIndex];
        }
    }

    pSingleList->pHead = pHead;
    pSingleList->pTail = pTail;
}

/**	��������Ļ���������

	@param	SINGLELIST *pSingleList - ��������ָ��	
	@param	UINT uRadix - �������ַ�������Ե��ֽ�Ϊ���Ļ�����Ϊ256	
                                ������10���Ƽ�������Ļ�������Ϊ10
	@param	UINT uMaxKeyLen - �ؼ��ʵĳ��ȣ��ַ������ֽ�Ϊ��λ�򳤶�Ϊ�ַ���
                              ���������ܳ��ȣ����32λ������16����Ϊ��λ��
                              ��������󳤶�Ϊ8
	@param	GETKEYFUNC GetKeyFunc - �ؼ��ʻ�ȡ�ص�����	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT SingleList_RadixSort( SINGLELIST *pSingleList,
                          UINT uRadix,
                          UINT uMaxKeyLen,
                          GETKEYFUNC GetKeyFunc )
{
    SINGLENODE  **ppHead;  /* ������¼��������ͷ�ڵ��˫ָ�� */
    SINGLENODE  **ppTail;  /* ������¼��������β�ڵ��˫ָ�� */
    UINT        i;         /* ��ʱѭ������ */

    /* �����������ڴ� */
    ppHead = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
    if ( ppHead == NULL )
    {
        return CAPI_FAILED;
    }
    ppTail = (SINGLENODE **)malloc( uRadix * sizeof(SINGLENODE *) );
    if ( ppTail == NULL )
    {
        free(ppHead);
        return CAPI_FAILED;
    }

    /* ��˳��Թؼ��ֵĵ�iλ���з�����ռ����� */
    for ( i = 0; i < uMaxKeyLen; i++ )
    {
        SingleList_Distribute(pSingleList, uRadix, i, ppHead, ppTail, GetKeyFunc);

        SingleList_Collect(pSingleList, uRadix, ppHead, ppTail);
    }

    /* �ͷŷ�������� */
    free( ppHead );
    free( ppTail );

    return CAPI_SUCCESS;
}


/**	��ȡ�ַ����ĵ�uKeyIndexλ���ϵ��ַ�
    ��ȡ�����ַ����Զ�ת���ɴ�д��ʽ��

	@param	void *pszData - ָ���ַ�����ָ��	
	@param	UINT uKeyIndex - ��ʾ�ڶ���λ��λ���ǴӺ���ǰ��
	@return	UINT - ���ص�uKeyIndexλ���ַ�ֵ	
*/
UINT GetStrKeyNoCase( void *pszData, UINT uKeyIndex )
{
	char *psz = (char *)pszData;
    UINT    uKey;
		
    if ( psz == NULL || (*psz) == '\0' )
    {
        return 0;
    }
    if ( uKeyIndex >= strlen(psz) )
    {
		return 0;
    }

    uKey = strlen(psz) - (uKeyIndex + 1);

    if( psz[uKey] >= 'a' && psz[uKey] <= 'z' )
    {
	    uKey = (UINT)(unsigned char)((psz[uKey] - 'a') + 'A');
    }
    else
    {
		uKey = (UINT)(unsigned char)(psz[uKey]);
    }

    return uKey;
}

/**	��ȡ�ַ����ĵ�uKeyIndexλ���ϵ��ַ�
    ��ȡ�����ַ����ִ�Сд������ԭֵ���䡣

	@param	void *pszData - ָ���ַ�����ָ��	
	@param	UINT uKeyIndex - ��ʾ�ڶ���λ��λ���ǴӺ���ǰ��
	@return	UINT - ���ص�uKeyIndexλ���ַ�ֵ	
*/
UINT GetStrKey( void *pszData, UINT uKeyIndex )
{
	char *psz = (char *)pszData;
    UINT    uKey;
		
    if ( psz == NULL || (*psz) == '\0' )
    {
        return 0;
    }
    if ( uKeyIndex >= strlen(psz) )
    {
		return 0;
    }

    uKey = strlen(psz) - (uKeyIndex + 1);

    uKey = (UINT)(unsigned char)(psz[uKey]);

	return uKey;
}

/**	��ȡ�����ĵ�uKeyIndexλ���֣���16����Ϊ��λ
    ��˻�ȡ�����������Ϊ0xf������������������ʱ��
    ����Ϊ16�����ؼ��ʳ���Ϊ4

	@param	void *pData - ����ֵ	
	@param	UINT uKeyIndex - �����ĵڶ���λ,16���������8λ	
	@return	UINT - ���ص�uKeyIndexλ������	
*/
UINT GetIntKey( void *pData, UINT uKeyIndex )
{
    UINT    uData;

    if ( uKeyIndex > 8 )
    {
        return 0;
    }

    uData = (UINT)pData;

    uData = (uData >> ((uKeyIndex - 1)*4)) & 0xf;

    return uData;   
}


/**	��ȡ�����ĵ�uKeyIndexλ���֣���65536����Ϊ��λ
��˻�ȡ�����������Ϊ0xffff������������ʱ��
����Ϊ65536�����ؼ��ʳ���Ϊ16

@param	void *pData - ����ֵ	
@param	UINT uKeyIndex - 65536���������ĵڶ���λ��32λ�������λ��Ϊ2	
@return	UINT - ���ص�uKeyIndexλ������	
*/
UINT GetWordKey( UINT uData, UINT uKeyIndex )
{
    unsigned short wData;
    if ( uKeyIndex > 2 )
    {
        return 0;
    }

    wData = (uData >> ((uKeyIndex - 1)*16)) & 0xffff;

    return wData;   
}
