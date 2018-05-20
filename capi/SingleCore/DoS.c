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
#include <time.h>
#include "CapiGlobal.h"
#include "BinTree.h"
#include "AVLTree.h"
#include "HashAVLTree.h"

#define     TCPHEAD_LENGTH  20
#define     DEFAULT_CONNECT_TIME    5000
#define     MAX_CONNECT_TIMES       5

typedef struct CLIENTDATA_st {
    DWORD   dwIPAddr;
    clock_t StartClock;
    clock_t TotalClock;
    DWORD   dwConnectTimes;
    INT     nTcpData[TCPHEAD_LENGTH];
} CLIENTDATA;

/**	�ж��Ƿ�DoS�����Ļص�����

	@param	CLIENTDATA * pData - CLIENTDATAָ��	
	@return	BOOL - TRUE��ʾ��DoS������FALSE��ʾ����	
*/
typedef  BOOL   (*ISDOSATTACKFUNC)(CLIENTDATA *pData);

/**	DoSUnattack��������������һ����ϣAVL����

	@param	UINT uBucketCount - ��ϣAVL����BUCKET�����С	
	@return	HASHAVLTREE * - ��ϣAVL��ָ��	
*/
HASHAVLTREE * DoSUnattack_Create(UINT uBucketCount)
{
    HASHAVLTREE *pTree;
    pTree = HashAVLTree_Create(uBucketCount);

    return pTree;
}

/**	��ϣAVL���ıȽϻص��������Ƚ�����IP��ַ�Ƿ����

	@param	void *p1 - CLIENTDATAָ������	
	@param	void *p2 - DWORD����	
	@return	INT - 0��ʾ��ȣ�-1��ʾp1С��p2��1��ʾp1����p2��	
*/
INT IPCompare(void *p1, void *p2)
{
    CLIENTDATA  *pData;
    DWORD dwIPAddr;

    pData = (CLIENTDATA *)p1;
    dwIPAddr = (DWORD)p2;

    if ( pData->dwIPAddr == dwIPAddr )
    {
        return 0;
    }
    else if ( pData->dwIPAddr < dwIPAddr )
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

/**	��ϣAVL���ļ����ϣֵ�Ļص�����

	@param	void *p - Ҫ�����ϣֵ�Ķ�Ӧ�ؼ���ָ��	
	@param	UINT uBucketCount - ��ϣAVL���е�BUCKET�����С	
	@return	INT - BUCKET�����±�	
*/
INT HashClientData(void *p, UINT uBucketCount)
{
    CLIENTDATA  *pData = (CLIENTDATA *)p;

    return pData->dwIPAddr % uBucketCount;
}

/**	���տͻ��˵�����

	@param	HASHAVLTREE *pTree - ��ϣAVL��ָ��	
	@param	DWORD dwIPAddr - �Է�IP��ַ	
	@param	INT *pTcpIpData - ���Ӱ�����	
	@param	ISDOSATTACKFUNC IsDosAttack - �ж��Ƿ�DoS�����Ļص�����	
	@return	void - ��
*/
void DoSUnattack_RecvConnection(HASHAVLTREE *pTree, DWORD dwIPAddr,
                    INT *pTcpIpData, ISDOSATTACKFUNC IsDosAttack)
{
    CLIENTDATA *pData;
    clock_t     ConnectClock;
    
    pData = (CLIENTDATA *)HashAVLTree_Find(pTree, (void *)dwIPAddr, 
                                HashInt, IPCompare);

    ConnectClock = clock();
    if ( pData != NULL )
    {
        pData->dwConnectTimes += 1;
        if ( (*IsDosAttack)(pData) )
        {
            /* ���ֹ������ܾ����ӣ����Է�IP����������� */
        }
    }
    else
    {
        pData = (CLIENTDATA *)malloc(sizeof(CLIENTDATA));
        if ( pData != NULL )
        {
            pData->dwConnectTimes = 1;
            pData->dwIPAddr = dwIPAddr;
            pData->StartClock = clock();
            pData->TotalClock = 0;
            memcpy(pData->nTcpData, pTcpIpData, TCPHEAD_LENGTH * sizeof(INT)); 

            HashAVLTree_Insert(pTree, pData, HashClientData, IPCompare);
        }
    }

    return ;
}

/**	DosUnattack����������, ����Ϳͻ��˵�����

	@param	HASHAVLTREE *pTree - ��ϣAVL��ָ��	
	@param	ISDOSATTACKFUNC IsDosAttack - �ж��Ƿ�DoS�����Ļص�����	
	@return	void - ��	
*/
void DosUattack_ConnectTask(HASHAVLTREE *pTree, 
                           ISDOSATTACKFUNC IsDosAttack)
{
    CLIENTDATA  *pData;

    HashAVLTree_EnumBegin(pTree);

    while( (pData = HashAVLTree_EnumNext(pTree) ) != NULL )
    {
        if ( (*IsDosAttack)(pData) )
        {
            /* ���ֹ������ܾ����ӣ����Է�IP���������*/
            /* ..... */
        }
        else
        {
            /* �Ϳͻ��˽������� */
        }
    }
}
