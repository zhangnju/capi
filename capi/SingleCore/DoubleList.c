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

#include "CapiGlobal.h"
#include "DoubleList.h"


/**	˫������Ĵ���������������������ǿյ�û�нڵ�������

	@param	void - ��	
	@return	DOUBLELIST * - ʧ�ܷ���NULL, �ɹ�ʱ����һ��˫������ṹ��ָ��	
*/
DOUBLELIST * DoubleList_Create( void )
{
    DOUBLELIST  *pList;

    /* �����ڴ���� */
    pList = (DOUBLELIST *)malloc(sizeof(DOUBLELIST));
    if ( pList != NULL )
    {
        /* ��ʼ������ṹ���ָ���ԱΪ�գ�����ڵ����Ϊ0 */
        pList->pHead = NULL;
        pList->pTail = NULL;
        pList->pCur = NULL;
        pList->uCount = 0;
    }

    return pList;
}


/**	˫��������ͷź���

	@param	DOUBLELIST *pList - Ҫ�ͷŵ�˫�������ָ��	
	@param	DESTROYFUNC pDestroyFunc - ����ڵ������ͷŻص�����	
	@return	void - ��	
*/
void DoubleList_Destroy( DOUBLELIST *pList, DESTROYFUNC DestroyFunc )
{
    DOUBLENODE  *pNode;

    if ( pList )
    {
        /* ��ͷ�ڵ㿪ʼ��һ����һ���ͷ�����ڵ㼰�ڵ����� */
        pNode = pList->pHead;
        while ( pNode != NULL )
        {
            DOUBLENODE  *pDelNode;

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
        free( pList );
    }
}


/**	˫���������ӽڵ㺯������ӵĽڵ����˫�������ͷ��

	@param	DOUBLELIST *pList - Ҫ��ӵ�˫������ָ��	
	@param	void *pData - Ҫ��ӵĽڵ������ָ��	
	@return	INT - ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS	
*/
INT	DoubleList_InsertHead( DOUBLELIST *pList, void *pData )
{
    DOUBLENODE  *pNode;

    /* ����У�� */
    if ( pList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* �½�һ���ڵ� */
    pNode = (DOUBLENODE *)malloc( sizeof(DOUBLENODE) );
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;  /* ���ڵ�����ָ��ָ�򴫽��������� */

    /* ���½��ڵ����һ�ڵ�ָ��ָ��ͷ�ڵ㣬����ͷ�ڵ��Ϊ�½��Ľڵ� */
    pNode->pNext = pList->pHead;
    pNode->pPrev = NULL;
    if ( pList->pHead != NULL )
    {
        pList->pHead->pPrev = pNode;
    }
    pList->pHead = pNode;

    /*
     * �ж��Ƿ�β�ڵ�ָ��Ϊ�գ����Ϊ�ձ�ʾԭ��������û�нڵ㣬
     * ��ʱӦ�ý�β�ڵ�ָ���¼���Ľڵ� 
     */
    if ( pList->pTail == NULL )
    {
        pList->pTail = pNode;
    }

    /* ������ڵ����ݼ�1 */
    pList->uCount++;
    
    return CAPI_SUCCESS;
}


/**	˫���������ӽڵ㺯������ӵĽڵ����˫�������β��

	@param	DOUBLELIST *pList - Ҫ��ӵ�˫������ָ��	
	@param	void *pData - Ҫ��ӵĽڵ������ָ��
	@return	INT - ʧ�ܷ���CAPI_FAILED���ɹ�����CAPI_SUCCESS	
*/
INT	DoubleList_InsertTail( DOUBLELIST *pList, void *pData )
{
    DOUBLENODE  *pNode;

    /* ����У�� */
    if ( pList == NULL || pData == NULL )
    {
        return CAPI_FAILED;
    }

    /* �½�һ���ڵ� */
    pNode = (DOUBLENODE *)malloc( sizeof(DOUBLENODE) );
    if ( pNode == NULL )
    {
        return CAPI_FAILED;
    }
    pNode->pData = pData;  /* ���ڵ�����ָ��ָ�򴫽��������� */
    pNode->pNext = NULL;   /* ���ڵ����һ�ڵ㸳Ϊ��ָ��NULL */
    pNode->pPrev = pList->pTail;

    /*
     * �ж��Ƿ�β�ڵ�ָ��Ϊ�գ����Ϊ�ձ�ʾԭ��������û�нڵ㣬
     * ��ʱӦ�ý�β�ڵ�ָ���¼���Ľڵ�, ����ͷ�ڵ�ָ��ҲӦ��ָ���½ڵ� 
     */
    if ( pList->pTail == NULL )
    {
        pList->pHead = pNode;
    }
    else
    {
        /*
         * ���β�ڵ�ָ�벻Ϊ�գ���ʱӦ�ý�β�ڵ���һ�ڵ�ָ��ָ���¼���� 
         * �ڵ�,����β�ڵ�ָ��ҲӦ��ָ���½ڵ� 
         */
        pList->pTail->pNext = pNode;
    }

    pList->pTail = pNode;

    /* ������ڵ����ݼ�1 */
    pList->uCount++;
    
    return CAPI_SUCCESS;
}


/**	˫������ĵ���ͷ�ڵ㺯��

	@param	DOUBLELIST *pList - Ҫ������˫������ָ��	
	@return	void * - ʧ�ܷ���NULL, �ɹ�����Ҫ������ͷ�ڵ������ָ��	
*/
void *	DoubleList_PopHead( DOUBLELIST *pList )
{
	DOUBLENODE	*pPopNode;   /* ����ָ��Ҫ�������ݵĽڵ��ָ�� */
	void		*pPopData;   /* ����ָ��Ҫ���������ݵ�ָ�� */

    /* ����У�� */
    if ( pList == NULL || pList->pHead == NULL ) 
    {
		return NULL;
    }

    /* ��Ҫ�������ݵĽڵ�ָ��ָ������ͷ�ڵ㣬��������ָ��ָ��ͷ�ڵ������ */
    pPopNode = pList->pHead;
    pPopData = pPopNode->pData;

    /* 
     * �жϵ�ǰ�ڵ�ָ���Ƿ�ָ��ͷ�ڵ㣬���ָ��ͷ�ڵ�����Ҫ����
     * ָ��ͷ�ڵ����һ�ڵ� 
     */
    if ( pList->pCur == pList->pHead )
    {
        pList->pCur = pList->pHead->pNext;
    }

    /* ��ͷ�ڵ�ָ��ָ��ͷ�ڵ����һ�ڵ� */
    pList->pHead = pList->pHead->pNext;
    if ( pList->pHead != NULL )
    {
        pList->pHead->pPrev = NULL;
    }
	
    /* ������ڵ�������1 */
    pList->uCount--;

    /* �������Ľڵ������Ѿ�Ϊ0���ʾԭ��ֻ��һ���ڵ㣬����ͷ�ڵ��
     * ��ʱ�����Ѿ�Ϊ�գ�û�нڵ������棬Ӧ�ý�β�ڵ�ָ�븳��
     * ��ǰ�ڵ�ָ������ǰ���Ѿ�������ˣ����ֻ��һ���ڵ�϶�Ϊ��
     * �������ﲻ��Ҫ����ǰ�ڵ�ָ��
     */
    if ( pList->uCount == 0 ) {
		pList->pTail = NULL;
	}
	
    /* �ͷŵ����Ľڵ�, ע�����ﲢû���ͷŽڵ�����ָ�� */
    free( pPopNode );

    return pPopData;    /* ����ͷ�ڵ������ָ�� */
}


/** ˫������ĵ���β�ڵ㺯��
    �����������Ҫ����β�ڵ��ǰһ�ڵ㣬���Ч�ʺܵ�

    @param	DOUBLELIST *pList - Ҫ������˫������ָ��	
    @return	void * -  ʧ�ܷ���NULL, �ɹ�����Ҫ������β�ڵ������ָ��	
*/
void *	DoubleList_PopTail( DOUBLELIST *pList )
{
    DOUBLENODE	*pPopNode;   /* ����ָ��Ҫ�������ݵĽڵ��ָ�� */
    void		*pPopData;   /* ����ָ��Ҫ���������ݵ�ָ�� */

    /* ����У�� */
    if ( pList == NULL || pList->pHead == NULL ) 
    {
        return NULL;
	}

    /* ��Ҫ�������ݵĽڵ�ָ��ָ������ͷ�ڵ㣬��������ָ��ָ��ͷ�ڵ������ */
    pPopNode = pList->pTail;
    pPopData = pPopNode->pData;

    /* �жϵ�ǰ�ڵ�ָ���Ƿ�ָ��β�ڵ㣬���ָ��ͷ�ڵ����轫��
     * ָ��սڵ� 
     */
    if ( pList->pCur == pList->pTail )
    {
        pList->pCur = NULL;
    }

    /* ��������ͷ�ڵ��β�ڵ���ͬ���ʾԭ��ֻ��һ���ڵ㣬����β�ڵ��
     * �����Ѿ�Ϊ�գ�û�нڵ������棬��ʱӦ�ý�ͷ�ڵ�ָ�븳��
     * ��ǰ�ڵ�ָ������ǰ���Ѿ�������ˣ����ֻ��һ���ڵ���϶�Ϊ��
     * �������ﲻ��Ҫ����ǰ�ڵ�ָ��
     */
    if ( pList->pTail == pList->pHead )
    {
	    pList->pHead = NULL;
    }
    else 
    {
        pList->pTail->pPrev->pNext = NULL;
    }
    pList->pTail = pList->pTail->pPrev;

   
    /* ������ڵ�������1 */
    pList->uCount--;

    /* �ͷŵ����Ľڵ�, ע�����ﲢû���ͷŽڵ�����ָ�� */
    free( pPopNode );

    return pPopData;    /* ����ͷ�ڵ������ָ�� */    
}



/**	�����ɾ���ڵ㺯��������ɾ����pMatchData��������ͬ���ݵĽڵ�
    ������������ͬ���ݵĽڵ�����ֻɾ����һ������ͬ���ݵĽڵ�

	@param	DOUBLELIST *pList - Ҫ������˫������ָ��	
	@param	void *pMatchData - Ҫɾ���ڵ��ƥ������	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ��������Ƚ�pMatchData����������
                                      �ڵ�����Ƿ����	
	@param	DESTROYFUNC DestroyFunc - ����ڵ�������ͷź���	
	@return	INT (by default) - CAPI_FAILED��ʾʧ�ܻ�������û��ƥ������ݣ�
                               CAPI_SUCCESS��ʾ�ɹ�ɾ��	
*/
INT	 DoubleList_Delete( DOUBLELIST *pList,  
                        void *pMatchData, 
                        COMPAREFUNC CompareFunc, 
                        DESTROYFUNC DestroyFunc )
{
    DOUBLENODE  *pNode;
    DOUBLENODE  *pPrevNode;

    /* ����У�� */
    if ( pList == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }

    pNode = pList->pHead;
    pPrevNode = pNode;

    while ( pNode != NULL )
    {
        /* �ȽϽڵ������Ƿ�Ͳ�������ƥ�� */
        if ( (*CompareFunc)( pNode->pData, pMatchData ) == 0 )
        {
            if ( pPrevNode == pNode )
            {
                /* ͷ�ڵ�ƥ�����ˣ���Ҫɾ��ͷ�ڵ� */
                pList->pHead = pNode->pNext;
                if ( pList->pHead != NULL )
                {
                    pList->pHead->pPrev = NULL;
                }
                else
                {
                    /* 
                     * ������ֻ��һ���ڵ�
                     * ��ʱ��Ҫ������β�ڵ�ָ�������ǰ�ڵ�ָ�븳��
                     */
                    pList->pTail = NULL;
                    pList->pCur = NULL;
                }
            }
            else
            {
                pPrevNode->pNext = pNode->pNext;
                if ( pNode->pNext != NULL )
                {
                    pNode->pNext->pPrev = pPrevNode;
                }

                if ( pList->pTail == pNode )
                {
                    /* 
                     * ���β�ڵ��pNode��ͬ������ɾ������β�ڵ�
                     * ��ʱ��Ҫ��β�ڵ�ָ��ָ��Ҫɾ���ڵ��ǰһ���ڵ�
                     */
                    pList->pTail = pPrevNode;
                }

                if ( pList->pCur == pNode )
                {
                    /* 
                     * �������ǰ�ڵ��pNode��ͬ������ɾ�����ǵ�ǰ�ڵ�
                     * ��ʱ��Ҫ����ǰ�ڵ�ָ��ָ��Ҫɾ���ڵ��ǰһ���ڵ�
                     */
                    pList->pCur = pNode->pNext;
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


/**	˫������Ļ�ȡ����ڵ���������

	@param	DOUBLELIST *pList - Ҫ������˫������ָ��	
	@return	UINT - ����ڵ�������Ϊ0��ʾ�����ǿյ�û�нڵ�����������
                   ���߲����Ƿ�	
*/
UINT DoubleList_GetCount(DOUBLELIST *pList)
{
    if ( pList == NULL )
    {
        return 0;
    }
    return pList->uCount;
}


/**	˫������Ļ�ȡͷ�ڵ㺯��

	@param	DOUBLELIST *pList - Ҫ������˫������ָ��	
	@return	void * - ͷ�ڵ������ָ��	
*/
void *	DoubleleList_GetHead( DOUBLELIST *pList )
{
    if ( pList == NULL )
    {
        return NULL;
    }

    if ( pList->pHead == NULL )
    {
        return NULL;
    }

    return pList->pHead->pData;
}


/**	˫������Ļ�ȡβ�ڵ㺯��

	@param	DOUBLELIST *pList - Ҫ������˫������ָ��	
	@return	void * - β�ڵ������ָ��	
*/
void * 	DoubleList_GetTail( DOUBLELIST *pList )
{
    if ( pList == NULL )
    {
        return NULL;
    }

    if ( pList->pTail != NULL )
    {
        return pList->pTail->pData;
    }
    else
    {
        return NULL;
    }
}

/**	˫������Ĳ��Һ���

	@param	DOUBLELIST *pList - ˫������ָ��	
	@param	void *pMatchData - Ҫ���ҵ�ƥ������	
	@param	COMPAREFUNC CompareFunc - ����ƥ��ȽϺ���	
	@return	void * - ���ҵ�����˫�������е�ƥ������	
*/
void * DoubleList_Find( DOUBLELIST *pList, void *pMatchData, 
                       COMPAREFUNC CompareFunc )
{
	DOUBLENODE	*pNode;

    pNode = pList->pHead;
	while ( pNode )
	{
		if ( (*CompareFunc)( pNode->pData, pMatchData ) == 0  )
		{
			void * pData;
			pData = pNode->pData;

			return pData;
		}
		pNode = pNode->pNext;
	}
	return NULL;
}

/**	˫�������ö�ٳ�ʼ������

	@param	SINGLELIST *pSingleList - Ҫ������˫������ָ��
	@return	void - ��	
*/
void DoubleList_EnumBegin( DOUBLELIST *pList )
{
    pList->pCur = pList->pHead;
    return;
}


/**	˫������ö����һ���ڵ�ĺ�������һ�ε��ô˺���ǰ����
    �ȵ���DoubleList_EnumBegin()����

	@param	DOUBLELIST *pList - Ҫ������˫�������ָ��	
	@return	void * - ö�ٵ��Ľڵ�����ָ��	
*/
void * DoubleList_EnumNext( DOUBLELIST *pList )
{
    DOUBLENODE  *pCur;
    
    pCur = pList->pCur;
    if ( pCur != NULL )
    {
        pList->pCur = pCur->pNext;
        
        return pCur->pData;
    }
    return NULL;
}


/**	˫�������ö�ٽڵ㺯������1�ε��ô˺���ǰҪ�ȵ���DoubleList_EnumBegin()����

	@param	DOUBLELIST *pList - ˫������ָ��	
	@return	DOUBLENODE * - ��˫��������ö�ٵ��Ľڵ�ָ��	
*/
DOUBLENODE * DoubleList_EnumNode( DOUBLELIST *pList )
{
    DOUBLENODE  *pCur;
    
    pCur = pList->pCur;
    if ( pCur != NULL )
    {
        pList->pCur = pCur->pNext;
        
        return pCur;
    }
    return NULL;
}


/**	˫��ڵ��ָ���ڵ�ɾ������

	@param	DOUBLELIST *pList - ˫������ָ��	
	@param	DOUBLENODE *pNode - Ҫ������ָ���ڵ�ָ��	
	@return	DOUBLENODE * - �����Ľڵ�ָ�룬��pNode��ͬ
*/
DOUBLENODE * DoubleList_PopNode( DOUBLELIST *pList, DOUBLENODE *pNode )
{
    /* �޸�ǰһ���ڵ�ĺ���ָ�� */
    if ( pNode->pPrev != NULL )
    {
        pNode->pPrev->pNext = pNode->pNext;
    }
    else
    {
        pList->pHead = pNode->pNext;
    }

    /* �޸ĺ�һ���ڵ��ǰ��ָ�� */
    if ( pNode->pNext != NULL )
    {
        pNode->pNext->pPrev = pNode->pPrev;
    }
    else
    {
        pList->pTail = pNode->pPrev;
    }

    /* �ж��Ƿ�pCurָ��ָ�򵯳��ڵ㣬������������pCur */
    if ( pList->pCur == pNode )
    {
        pList->pCur = pNode->pNext;
    }

    pList->uCount -= 1;

    /* ���ش����pNodeָ�� */
    return pNode;
}

/**	˫������Ĳ���������
    �����ǰ��մ�С����������У�����˵�Ĵ�С����CompareFunc��������
    ����û�����ͨ��CompareFunc�ķ���ֵ����������ʹ��˳���������������

	@param	DOUBLELIST *pList - Ҫ������˫������ָ��	
	@param	COMPAREFUNC CompareFunc - �ڵ����ݱȽϺ���	
	@return	INT - �ɹ�����1��ʧ�ܷ���0	
*/
INT DoubleList_InsertSort( DOUBLELIST *pList, COMPAREFUNC CompareFunc )
{
    DOUBLENODE  *pNode;     /* ��������pList����ʱָ�� */
    DOUBLENODE  *pPrevNode; /* pNode��ǰһ�ڵ�ָ�� */

    pNode = pList->pHead;
    pPrevNode = NULL;

    if ( pNode == NULL )
    {
        /* ������û�нڵ㣬���ǰ��������Ѿ��ź����� */
        return CAPI_SUCCESS;
    }

    while ( pNode->pNext != NULL )
    {
        DOUBLENODE  *pTempNode;

        pTempNode = pList->pHead;
        pPrevNode = NULL;
        while ( pTempNode != pNode->pNext )
        {
            if ( (*CompareFunc)( pNode->pNext->pData, pTempNode->pData ) < 0 )
            {
                DOUBLENODE  *pCurNode = pNode->pNext;

                /* ��pCurNode�ڵ㵯���� */
                pNode->pNext = pNode->pNext->pNext;
                if ( pNode->pNext != NULL )
                {
                    pNode->pNext->pPrev = pNode;
                }

                /* ִ�в������ */
                if ( pPrevNode != NULL ) /* ���벻��ͷ�ڵ�ǰ����� */
                {
                    /* ��pNode->pNext����pTempNode ֮ǰ */
                    pPrevNode->pNext = pCurNode;
                    pCurNode->pPrev = pPrevNode;

                    pCurNode->pNext = pTempNode;
                    pTempNode->pPrev = pCurNode;
                }
                else /* ������ͷ�ڵ�ǰ����� */
                {
                    /* ��pNode->pNext�ڵ��Ϊͷ�ڵ� */
                    pCurNode->pNext = pTempNode;
                    pCurNode->pPrev = NULL;
                    pList->pHead->pPrev = pCurNode;
                    pList->pHead = pCurNode;
                }

                /* �޸�βָ��ָ�� */
                if ( pCurNode == pList->pTail )
                {
                    pList->pTail = pNode;
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
    } /* while ( pNode->pNext != NULL ) */

    return 1;
}


/**	�������Ѿ��ź����������кϲ�
    �������������ǰ���С����������У���β�ڵ����
    �ϲ��������Ҳ�Ǵ�С�����������

	@param	DOUBLELIST *pListA - Ҫ�ϲ�������	
	@param	DOUBLELIST *pListB -Ҫ�ϲ�������	
	@param	COMPAREFUNC CompareFunc - �ڵ�ȽϺ���	
	@return	INT - 0 ��ʾʧ�ܣ�ʧ��ʱ������û�б仯
                  1 ��ʾ�ɹ����ɹ�ʱpListA�Ǻϲ���Ľ����
                  pListB�ᱻ�ͷŵ�
*/
INT DoubleList_Merge(DOUBLELIST *pListA, 
                     DOUBLELIST *pListB, 
                     COMPAREFUNC CompareFunc)
{
    DOUBLENODE	*pNodeA; /* ����ָ������pListA�Ľڵ����ʱָ�� */
    DOUBLENODE  *pNodeB; /* ����ָ������pListB�Ľڵ����ʱָ�� */
    DOUBLENODE  *pPrevA; /* pNodeA��ǰһ�ڵ�ָ�� */

    pNodeA = pListA->pHead;
    pNodeB = pListB->pHead;
    pPrevA = NULL;

    while ( pNodeB != NULL )
    {
        while ( pNodeA != NULL )
		{
            if ( (*CompareFunc)( pNodeA->pData, pNodeB->pData ) >= 0 )
			{
                DOUBLENODE *pNode;
				
				/* ��pNodeB���������浽pNode�� */
                pNode = pNodeB;
                pNodeB = pNodeB->pNext;
				
				/* ��pNode���뵽pNodeAǰ�� */
                if ( pPrevA == NULL )
                {
                    /* ������ͷָ��ǰ���������Ҫ�޸�ͷָ�� */
                    pListA->pHead = pNode;
                    pNode->pNext = pNodeA;
                    pNodeA->pPrev = pNode;
                }
                else
                {
                    pPrevA->pNext = pNode;
                    pNode->pPrev = pPrevA;
                    pNode->pNext = pNodeA;
                    pNodeA->pPrev = pNode;
                }
                pPrevA = pNode;
			
				break;
			}
            pPrevA = pNodeA;
			pNodeA = pNodeA->pNext;
		}

		/*
         * ���pListB���������ݶ���������A�����ݣ�
         * ��pListB���뵽pListAβ��.
         */
        if ( pNodeA == NULL )
		{
			pListA->pTail->pNext = pNodeB;
            pNodeB->pPrev = pListA->pTail;

			pListA->pTail = pListB->pTail;
			
			break;
		}				
    }
	
    /* �޸�pListA�Ľڵ������� */
    pListA->uCount += pListB->uCount;

	free( pListB );

	return 1;
}

/**	��һ��˫��������������˫������
    ֻ�Ǵ�ԭ�������м�Ͽ��������ԭ������������ĵ�һ������

	@param	DOUBLELIST *pList - Ҫ��������˫������	
	@param	UINT nCount - ������ĵ�һ������Ľڵ����, ����С��ԭ����ڵ����	
	@return	DOUBLELIST * - �ɹ�ʱ����������ĵڶ�������ʧ�ܷ���NULL��	
*/
DOUBLELIST * DoubleList_Split(DOUBLELIST *pList, UINT uCount)
{
    DOUBLENODE	*pNode;       /* ���������������Ľڵ��ָ�� */
	DOUBLELIST  *pSecondList; /* ������¼������ĵڶ��������ָ�� */
	UINT    	uIndex;       /* ��ʱѭ������� */

	
    /* ����һ�������� */
    pSecondList = DoubleList_Create();
    if ( pSecondList == NULL )
    {
        return NULL;
    }
    /* ����У�� */
    if ( uCount == 0 || pList->uCount <= uCount)
    {
        return NULL;
    }

	/* ��ȡҪ������λ�� */
    pNode = pList->pHead;
    for ( uIndex = 1; uIndex < uCount; uIndex++ )
    {
        pNode = pNode->pNext;
    }
	
	/* ���ڶ������������. */
    pSecondList->pHead = pNode->pNext;
    pSecondList->pTail = pList->pTail;

    pSecondList->uCount = pList->uCount - uCount;

	/* �޸ĵ�һ�����������. */
    pList->pTail = pNode;
    pList->uCount = uCount;

	/* ����һ������β�ڵ��pNextָ��ָ��NULL */
    pList->pTail->pNext = NULL;

    /* ���ڶ��������ͷ�ڵ��ǰһ�ڵ�ָ�븳�ɿ� */
    pSecondList->pHead->pPrev = NULL;
    return pSecondList;
}


/**	������ʹ�ù鲢�������򣬹鲢�Ͳ���������ʹ�ã���ʹ�ù鲢����
   ����������Ԫ����������С��һ��ֵʱ��ʹ�ò�������
    
	@param	DOUBLELIST *pList - Ҫ���������ָ��	
	@param	COMPAREFUNC CompareFunc - ����ڵ�ȽϺ���	
	@param	UINT uInsertSortCount - ʹ�ò�������ʱ������ڵ������������ڵ�
                                    ����С�����ֵʱ��ʹ�ò��������㷨
                                    
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT DoubleList_MergeSort(DOUBLELIST *pList, 
                         COMPAREFUNC CompareFunc, 
                         UINT uInsertSortCount)
{
   	DOUBLELIST	 *pSecondList;

    /* ����У�� */
    if ( pList == NULL || CompareFunc == NULL )
    {
        return CAPI_FAILED;
    }
    
    if ( pList->uCount < 2 )
    {
        /* ����ڵ����������������Ϊ���Ѿ��ź���� */
        return CAPI_SUCCESS;
    }

    /*
     * �������ڵ����С�ڸ���������������ĸ�����ֱ��ʹ�ò������� 
	 */
    if ( pList->uCount <= uInsertSortCount )
	{
		(void)DoubleList_InsertSort( pList, CompareFunc );
	}
    else
	{
		/* �������������� */
        pSecondList = DoubleList_Split(pList, (pList->uCount) / 2 );
		
        /* �������ĵ�һ��������еݹ���ù鲢���� */
        (void)DoubleList_MergeSort( pList, CompareFunc, uInsertSortCount );
		
        /* �������ĵڶ���������еݹ���ù鲢���� */
		(void)DoubleList_MergeSort( pSecondList, CompareFunc, uInsertSortCount );

		/* ���ź������������ϳ�һ��. */
		(void)DoubleList_Merge( pList, pSecondList, CompareFunc );
	}

    return CAPI_SUCCESS; 
}


/**	˫������ı�������

	@param	DOUBLELIST *pList - Ҫ����˫������ָ��	
	@param	TRAVERSEFUNC TraverseFunc - �ڵ����ݵı�����������	
	@return	INT - �ɹ�����1��ʧ�ܷ���0	
*/
void DoubleList_Traverse( DOUBLELIST *pList, TRAVERSEFUNC TraverseFunc )
{
    DOUBLENODE  *pNode;

    pNode = pList->pHead;
    
    /* ��ʼ�������� */
    while ( pNode != NULL )
    {
        (*TraverseFunc)( pNode->pData ); /* ���ñ����ص������������� */
        pNode = pNode->pNext;
    }

    return;
}


/**	����������ݵĵ�uKeyIndexλ�ϵ�Ԫ�ؽ��з��࣬
    �������ǵĴ�С�����Ӧ��������

	@param	DOUBLELIST *pList - ˫������ָ��	
	@param	UINT       uRadix - ��������Ļ�������������������йأ�
                                һ�����������Ļ���Ϊ16���ַ����Ļ������Ϊ255��
	@param	UINT       uKeyIndex - �ڶ���λ	
	@param	DOUBLENODE **ppHead - ������¼ͷָ�������	
	@param	DOUBLENODE **ppTail - ��¼���ӵ�βָ��	
	@param	GETKEYFUNC GetKeyFunc - ��ȡ���ݵĵ�uKeyIndexλ�ϵ�Ԫ��ֵ	
	@return	void - �ޡ�	
*/
void DoubleList_Distribute(DOUBLELIST *pList,
                          UINT       uRadix,
                          UINT       uKeyIndex,
                          DOUBLENODE **ppHead,
                          DOUBLENODE **ppTail,
                          GETKEYFUNC GetKeyFunc )
{
    DOUBLENODE   *pNode;
    UINT         i;
    
    /* ��ʼ���ӱ� */
    for ( i = 0; i < uRadix; i++ )
    {
        ppHead[i] = NULL;
        ppTail[i] = NULL;
    }
    
    pNode = pList->pHead;
    
    while ( pNode != NULL )
    {
        UINT uRadixIndex = (*GetKeyFunc)(pNode->pData, uKeyIndex);
        
        if ( ppHead[uRadixIndex] == NULL )
        {
            ppHead[uRadixIndex] = pNode;
            ppTail[uRadixIndex] = pNode;
            pNode = pNode->pNext;
            ppTail[uRadixIndex]->pNext = NULL;
            ppTail[uRadixIndex]->pPrev = NULL;
        }
        else
        {
            ppTail[uRadixIndex]->pNext = pNode;
            pNode->pPrev = ppTail[uRadixIndex];
            ppTail[uRadixIndex] = pNode;
            pNode = pNode->pNext;
            ppTail[uRadixIndex]->pNext = NULL;
        }
    }   
}

/**	�Ի��������зֺ�������ӽ����ռ���������������������һ������

	@param	DOUBLELIST *pList - ˫������ָ��	
	@param	UINT        uRadix - ����	
	@param	DOUBLENODE **ppHead - ������¼ͷָ�������	
	@param	DOUBLENODE **ppTail - ��¼���ӵ�βָ��	
	@return	void - ��
*/
void DoubleList_Collect(DOUBLELIST *pList,
                       UINT        uRadix,
                       DOUBLENODE **ppHead,
                       DOUBLENODE **ppTail )
{
    DOUBLENODE  *pHead;
    DOUBLENODE  *pTail;
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
        return;
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
            pTail->pNext->pPrev = pTail;
            pTail = ppTail[uRadixIndex];
        }
    }

    pList->pHead = pHead;
    pList->pTail = pTail;

    return ;
}


/**	˫������Ļ��������㷨������

	@param	DOUBLELIST *pList - Ҫ�����˫������ָ��	
	@param	UINT uRadix - �������ַ�������Ե��ֽ�Ϊ���Ļ�����Ϊ256	
                                ������10���Ƽ�������Ļ�������Ϊ10
	@param	UINT uMaxKeyLen - �ؼ��ʵĳ��ȣ��ַ������ֽ�Ϊ��λ�򳤶�Ϊ�ַ���
                              ���������ܳ��ȣ����32λ������16����Ϊ��λ��
                              ��������󳤶�Ϊ8
	@param	GETKEYFUNC GetKeyFunc - �ؼ��ʻ�ȡ�ص�����	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT DoubleList_RadixSort( DOUBLELIST *pList,
                         UINT uRadix,
                         UINT uMaxKeyLen,
                         GETKEYFUNC GetKeyFunc )
{
    DOUBLENODE  **ppHead;  /* ������¼��������ͷ�ڵ��˫ָ�� */
    DOUBLENODE  **ppTail;  /* ������¼��������β�ڵ��˫ָ�� */
    UINT        i;         /* ��ʱѭ������ */
    
    /* �����������ڴ� */
    ppHead = (DOUBLENODE **)malloc( uRadix * sizeof(DOUBLENODE *) );
    if ( ppHead == NULL )
    {
        return CAPI_FAILED;
    }
    ppTail = (DOUBLENODE **)malloc( uRadix * sizeof(DOUBLENODE *) );
    if ( ppTail == NULL )
    {
        free(ppHead);
        return CAPI_FAILED;
    }
    
    /* ��˳��Թؼ��ֵĵ�iλ���з�����ռ����� */
    for ( i = 0; i < uMaxKeyLen; i++ )
    {
        DoubleList_Distribute(pList, uRadix, i, ppHead, ppTail, GetKeyFunc);
        
        DoubleList_Collect(pList, uRadix, ppHead, ppTail);
    }
    
    /* �ͷŷ�������� */
    free( ppHead );
    free( ppTail );
    
    return CAPI_SUCCESS;
}
