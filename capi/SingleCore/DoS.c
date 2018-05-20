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

/**	判断是否DoS攻击的回调函数

	@param	CLIENTDATA * pData - CLIENTDATA指针	
	@return	BOOL - TRUE表示是DoS攻击，FALSE表示不是	
*/
typedef  BOOL   (*ISDOSATTACKFUNC)(CLIENTDATA *pData);

/**	DoSUnattack创建函数，创建一个哈希AVL树。

	@param	UINT uBucketCount - 哈希AVL树的BUCKET数组大小	
	@return	HASHAVLTREE * - 哈希AVL树指针	
*/
HASHAVLTREE * DoSUnattack_Create(UINT uBucketCount)
{
    HASHAVLTREE *pTree;
    pTree = HashAVLTree_Create(uBucketCount);

    return pTree;
}

/**	哈希AVL树的比较回调函数，比较两个IP地址是否相等

	@param	void *p1 - CLIENTDATA指针类型	
	@param	void *p2 - DWORD类型	
	@return	INT - 0表示相等，-1表示p1小于p2，1表示p1大于p2。	
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

/**	哈希AVL树的计算哈希值的回调函数

	@param	void *p - 要计算哈希值的对应关键词指针	
	@param	UINT uBucketCount - 哈希AVL树中的BUCKET数组大小	
	@return	INT - BUCKET数组下标	
*/
INT HashClientData(void *p, UINT uBucketCount)
{
    CLIENTDATA  *pData = (CLIENTDATA *)p;

    return pData->dwIPAddr % uBucketCount;
}

/**	接收客户端的连接

	@param	HASHAVLTREE *pTree - 哈希AVL树指针	
	@param	DWORD dwIPAddr - 对方IP地址	
	@param	INT *pTcpIpData - 连接包数据	
	@param	ISDOSATTACKFUNC IsDosAttack - 判断是否DoS攻击的回调函数	
	@return	void - 无
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
            /* 发现攻击，拒绝连接，将对方IP加入黑名单中 */
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

/**	DosUnattack的连接任务, 负责和客户端的连接

	@param	HASHAVLTREE *pTree - 哈希AVL树指针	
	@param	ISDOSATTACKFUNC IsDosAttack - 判断是否DoS攻击的回调函数	
	@return	void - 无	
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
            /* 发现攻击，拒绝连接，将对方IP计入黑名单*/
            /* ..... */
        }
        else
        {
            /* 和客户端建立连接 */
        }
    }
}
