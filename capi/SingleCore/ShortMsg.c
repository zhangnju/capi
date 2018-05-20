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
#include <stdio.h>
#include <time.h>
#include "CapiGlobal.h"
#include "DoubleList.h"

#define MAX_PHONE_NUMBER_LEN    16

typedef struct SHORTMSG_st {
    time_t		time;
    char 		pszSrcPhone[MAX_PHONE_NUMBER_LEN];
    char 		pszTagPhone[MAX_PHONE_NUMBER_LEN];
    char *		pszMsg;
    INT			nState;
} SHORTMSG;

typedef struct SHORTMSGCACHE_st {
    DOUBLELIST  *pList;
} SHORTMSGCACHE;

/**	����Ϣ���͵Ļص�����ָ�붨��

	@param	SHORTMSG - ָ��һ������Ϣ��ָ��	
	@return	INT - CAPI_FAILED��ʾ����ʧ�ܣ�CAPI_SUCCESS��ʾ���ͳɹ�	
*/
typedef INT (*SHORTMSGSENDFUNC)(SHORTMSG *pMsg);

/**	����ϢCACHE�Ĵ�������

	@return	SHORTMSGCACHE * -�ɹ����ش����Ķ���ϢCACHE����ָ�룬
                                 ʧ�ܷ���NULL��	
*/
SHORTMSGCACHE *ShortMsgCache_Create()
{
    SHORTMSGCACHE   *pCache;

    pCache = (SHORTMSGCACHE *)malloc(sizeof(SHORTMSGCACHE));
    if ( pCache != NULL )
    {
        pCache->pList = DoubleList_Create();
        if ( pCache->pList == NULL )
        {
            free(pCache);
            pCache = NULL;
        }
    }
    return pCache;
}

/**	����ϢCACHE�ͷŵĻص������������ͷ�һ������Ϣ

	@param	void *p - ָ��һ������ϢSHORTMSG��ָ��	
	@return	void - ��	
*/
void ShortMsgDestroy(void *p)
{
    SHORTMSG    *pMsg = (SHORTMSG *)p;
    if ( pMsg != NULL )
    {
        if ( pMsg->pszMsg != NULL )
        {
            free( pMsg->pszMsg );
        }
        free(pMsg);
    }
}

/**	����ϢCACAE���ͷź������������б���Ķ���Ϣȫ���ͷŵ�

	@param	SHORTMSGCACHE *pCache - Ҫ�ͷŵĶ���ϢCACHE����ָ��	
	@return	void - ��	
*/
void ShortMsgCache_Destroy(SHORTMSGCACHE *pCache)
{
    if ( pCache != NULL )
    {
        DoubleList_Destroy(pCache->pList, ShortMsgDestroy);
        free(pCache);
    }
}

/**	����ϢCACHE���պ����������յ��Ķ���Ϣ����������

	@param	SHORTMSGCACHE *pCache - ����ϢCACHE����ָ��	
	@param	SHORTMSG *pMsg - ����Ϣָ�룬ָ��һ������Ϣ	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT ShortMsgCache_Recv(SHORTMSGCACHE *pCache, SHORTMSG *pMsg)
{
    return DoubleList_InsertTail(pCache->pList, (void *)pMsg);
}

/**	����Ϣ���ͺ�������CACHE��ȡ��һ����Ϣ���ͳ�ȥ
        �������ʧ�ܻὫ��Ϣ���²�������β����

	@param	SHORTMSGCACHE *pCache - ����ϢCACHE����ָ��	
	@param	SHORTMSGSENDFUNC SendFunc - ���Ͷ���Ϣ�Ļص�����	
	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ���	
*/
INT ShortMsgCache_Send(SHORTMSGCACHE *pCache, SHORTMSGSENDFUNC SendFunc)
{
    SHORTMSG    *pMsg;

    pMsg = DoubleList_PopHead(pCache->pList);
    if ( pMsg == NULL )
    {
        return CAPI_FAILED;
    }
    if ( (*SendFunc)(pMsg) != CAPI_SUCCESS )
    {
        DoubleList_InsertTail(pCache->pList, (void *)pMsg);
        return CAPI_FAILED;
    }
    else
    {
        ShortMsgDestroy(pMsg);
    }
    return CAPI_SUCCESS;
}

