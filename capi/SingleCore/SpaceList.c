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
#include "SpaceList.h"

SPACELIST *SpaceList_Create(UINT uSpaceCount, UINT uDataSize)
{
    SPACELIST   *pSpaceList;
    SPACENODE   *pNode;
    UINT        i;
    
    pSpaceList = (SPACELIST *)malloc( sizeof(SPACELIST)
        + uSpaceCount * (uDataSize + sizeof(SPACENODE)) );
    if ( pSpaceList != NULL )
    {
        pSpaceList->uSpaceCount = uSpaceCount;
        pSpaceList->uDataSize = uDataSize;
        
        pSpaceList->pBlock = (void *)((char *)pSpaceList +sizeof(SPACELIST) );
        
        /* build the empty list */
        pSpaceList->pHead = (SPACENODE *)pSpaceList->pBlock;
        pNode = pSpaceList->pHead;
            
        for (i = 0; i < uSpaceCount; i++)
        {
            pNode->pData = (void *)((char *)pNode + sizeof(SPACENODE));
            pNode->pNext = (SPACENODE *)((char *)pNode->pData + uDataSize);
            pNode = pNode->pNext;
        }
        pNode->pNext = NULL;
    }
                
    return pSpaceList;
}


void SpaceList_Destroy(SPACELIST *pSpaceList)
{
    if ( pSpaceList != NULL )
    {
        free( pSpaceList );
    }
}

void *SpaceList_Alloc(SPACELIST *pSpaceList)
{
    SPACENODE   *pNode;
    
    pNode = pSpaceList->pHead;
    if ( pNode != NULL )
    {
        pSpaceList->pHead = pNode->pNext;
    }
    return (void *)pNode;
}

void SpaceList_Free(SPACELIST *pSpaceList, void *pData)
{
    SPACENODE  *pNode;
    
    pNode =(SPACENODE *)pData;
    
    pNode->pNext = pSpaceList->pHead;
    pNode->pData = (void *)((char *)pNode + sizeof(SPACENODE));
    pSpaceList->pHead = pNode;
}

