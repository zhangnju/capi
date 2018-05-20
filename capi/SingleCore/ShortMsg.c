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

/**	短信息发送的回调函数指针定义

	@param	SHORTMSG - 指向一条短信息的指针	
	@return	INT - CAPI_FAILED表示发送失败，CAPI_SUCCESS表示发送成功	
*/
typedef INT (*SHORTMSGSENDFUNC)(SHORTMSG *pMsg);

/**	短信息CACHE的创建函数

	@return	SHORTMSGCACHE * -成功返回创建的短信息CACHE对象指针，
                                 失败返回NULL。	
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

/**	短信息CACHE释放的回调函数，用来释放一条短信息

	@param	void *p - 指向一条短信息SHORTMSG的指针	
	@return	void - 无	
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

/**	短信息CACAE的释放函数，将链表中保存的短信息全部释放掉

	@param	SHORTMSGCACHE *pCache - 要释放的短信息CACHE对象指针	
	@return	void - 无	
*/
void ShortMsgCache_Destroy(SHORTMSGCACHE *pCache)
{
    if ( pCache != NULL )
    {
        DoubleList_Destroy(pCache->pList, ShortMsgDestroy);
        free(pCache);
    }
}

/**	短信息CACHE接收函数，将接收到的短信息存入链表中

	@param	SHORTMSGCACHE *pCache - 短信息CACHE对象指针	
	@param	SHORTMSG *pMsg - 短信息指针，指向一条短信息	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功。	
*/
INT ShortMsgCache_Recv(SHORTMSGCACHE *pCache, SHORTMSG *pMsg)
{
    return DoubleList_InsertTail(pCache->pList, (void *)pMsg);
}

/**	短消息发送函数，从CACHE里取出一条信息发送出去
        发送如果失败会将信息重新插入链表尾部。

	@param	SHORTMSGCACHE *pCache - 短信息CACHE对象指针	
	@param	SHORTMSGSENDFUNC SendFunc - 发送短信息的回调函数	
	@return	INT - CAPI_FAILED表示失败，CAPI_SUCCESS表示成功。	
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

