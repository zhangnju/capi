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

#ifdef _WIN32
#include <windows.h>
#endif
#include "CapiGlobal.h"
#include "DoubleList.h"
#include "MTask.h"
#include "MTList.h"
#include "MsgQueue.h"

#define DEFAULT_MSGQUEUE_LEN    65535

/**	��Ϣ���еĴ�������

	@param	INT nMaxLen - ��Ϣ���е���󳤶�	
	@return	MSGQUEUE * - ���ش�������Ϣ����ָ�룬ʧ�ܷ���NULL	
*/
MSGQUEUE * MsgQueue_Create(INT nMaxLen)
{
    MSGQUEUE    *pQueue;

    pQueue = (MSGQUEUE *)malloc(sizeof(MSGQUEUE));
    if ( pQueue != NULL )
    {
        pQueue->pSem = SemaCreate(0, nMaxLen);
        if ( pQueue->pSem != NULL )
        {
            pQueue->nMaxLen = nMaxLen;
            pQueue->pList = MTList_Create();
            if ( pQueue->pList == NULL )
            {
                SemaClose(pQueue->pSem);
                free(pQueue);
                pQueue = NULL;
            }
        }
        else
        {
            free(pQueue);
            pQueue = NULL;
        }
    }
    return pQueue;
}


/**	��Ϣ���е��ͷź���

	@param	MSGQUEUE *pQueue - ��Ϣ����ָ��	
	@param	DESTROYFUNC DestroyFunc - �����ͷŻص�����	
	@return	void - ��	
*/
void MsgQueue_Destroy(MSGQUEUE *pQueue, DESTROYFUNC DestroyFunc)
{
    if ( pQueue != NULL )
    {
        INT nMaxLen;
        if ( pQueue->nMaxLen > DEFAULT_MSGQUEUE_LEN )
        {
            nMaxLen = pQueue->nMaxLen;
        }
        else
        {
            nMaxLen = DEFAULT_MSGQUEUE_LEN;
        }
        SemaRelease(pQueue->pSem, nMaxLen ); /* �����������Ľ��ղ������Լ���*/
        MTList_Destroy(pQueue->pList, DestroyFunc);
        SemaClose(pQueue->pSem);
        free(pQueue);
    }
}


/**	��Ϣ���еķ��ͺ����������ݷ��͵���Ϣ������

	@param	MSGQUEUE *pQueue - ��Ϣ����ָ��	
	@param	void *pData - Ҫ���͵���Ϣ�����������ָ��	
	@return	INT - ͬMTList_InsertTail()����	
*/
INT MsgQueue_Send(MSGQUEUE *pQueue, void *pData)
{
    INT     nRet;

    nRet = MTList_InsertTail(pQueue->pList, pData);
    SemaRelease(pQueue->pSem, 1); /* ��������1 */

    return nRet;
}


/**	��Ϣ���еĽ������ݺ���������Ϣ�����н������ݳ���

	@param	MSGQUEUE *pQueue - ��Ϣ����ָ��	
	@return	void * - ���ش���Ϣ�����н��յ�������	
*/
void * MsgQueue_Recv(MSGQUEUE *pQueue)
{
    SemaWait(pQueue->pSem); /* ��������1������Ϊ0�������ס */
    return MTList_PopHead(pQueue->pList);
}

/**	��Ϣ���е�ö�ٳ�ʼ������

	@param	MSGQUEUE *pQueue - ��Ϣ����ָ��	
	@return	void - ��	
*/
void MsgQueue_EnumBegin(MSGQUEUE *pQueue)
{
    MTList_EnumBegin(pQueue->pList);
}

/**	��Ϣ���е�ö����һ�����ݺ���

	@param	MSGQUEUE *pQueue - ��Ϣ����ָ��	
	@param	VISITFUNC VisitFunc - ���ݷ��ʻص�����	
	@return	INT - ͬMTList_EnumNext()����	
*/
INT MsgQueue_EnumNext(MSGQUEUE *pQueue, VISITFUNC VisitFunc)
{
    return MTList_EnumNext(pQueue->pList, VisitFunc);
}

#define MAX_QUEUE_SIZE  4096
#define MAX_LISTEN_SIZE 16
#define MAX_THREADS     10
#define DEFAULT_PORT    8080
#if  0
MSGQUEUE    *g_pQue;

void ProcessRequest(void *args);

/**	��ʼ����������ʼ��ʱ���ȴ�����Ϣ���У���Ԥ����MAX_THREADS���߳�

	@return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT Init()
{
    INT i;

    g_pQue = MsgQueue_Create(MAX_QUEUE_SIZE);
    if ( g_pQue == NULL )
    {
        return CAPI_FAILED;
    }
    for ( i = 0; i < MAX_THREADS; i++ )
    {
        _beginthread(ProcessRequest, 0, NULL);
    }

    return CAPI_SUCCESS;
}

/**	��������Ĵ�����
        ʵ�ֽ����յ��������SOCKET��ʶ�����뵽��Ϣ������

        @return	INT - CAPI_FAILED��ʾʧ�ܣ�CAPI_SUCCESS��ʾ�ɹ�	
*/
INT RecvQuest()
{
    struct	sockaddr_in local, from;
    SOCKET	s, msgsock;
    int		fromlen;

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s ==  INVALID_SOCKET)
    {
        return CAPI_FAILED;
    }
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(DEFAULT_PORT);
    
    if ( bind(s, (struct sockaddr*)&local, sizeof(local)) == SOCKET_ERROR) 
    {
        return CAPI_FAILED;
    }
    if ( listen(s, MAX_LISTEN_SIZE ) == SOCKET_ERROR)
    {
        return CAPI_FAILED;
    }

    while( msgsock = accept(s, (struct sockaddr*)&from, &fromlen) 
        != INVALID_SOCKET )
    {
        SOCKET *pNewSock = new SOCKET;
        *pNewSock = msgsock;
        MsgQueue_Send(g_pQue, (void *)pNewSock);
    }
    return CAPI_SUCCESS;
}

/**	�����������������
        ��������Ǵ����������ں����������洦����տͻ��˷������ַ�����Hello!����
        ��������Ӧ��OK"�ؿͻ��ˡ�

	@param	void * -δʹ��	
	@return	void -��	
*/
void ProcessRequest(void *)
{
    SOCKET *pSock;
    while (pSock = MsgQueue_Recv(g_pQue))
    {
        int  len;
        char buf[1024];
        len = recv(*pSock, buf, sizeof(buf)-1, 0);
        if ( len > 0 )
        {
            buf[len] = '\0';
            if ( strcmp(buf, "Hello!")== 0 )
            {
                strcpy(buf, "Ok");
                send(*pSock, buf, strlen(buf), 0);
            }
        }
    }
}


/**	��SOCKET��Ϣ��ӡ����Ļ�Ļص�����

	@param	void *pData - ����ָ��	
	@return	INT - ȡ����WriteToScreen()���� 	
*/
INT PrintAllIPToScreen(void *pData)
{
    SOCKET *pSock = pData;
    return WriteToScreen(*pSock);    
}
#endif