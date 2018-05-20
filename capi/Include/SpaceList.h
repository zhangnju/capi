/*
 *
 *  DESCRIPTION
 *      Interface to a available space list type.
 *
 *  HISTORY
 *      10-21-2004      create.
 *
 */

#ifndef _SPACELIST_H_
#define _SPACELIST_H_


typedef struct SPACENODE_st {
    struct SPACENODE_st * pNext;
    void *pData;
} SPACENODE;

typedef struct SPACELIST_st {
    void        *pBlock;
    SPACENODE   *pHead;
    UINT        uSpaceCount;
    UINT        uDataSize;
} SPACELIST;

SPACELIST *SpaceList_Create(UINT uSpaceCount,UINT uDataSize);
void SpaceList_Destroy(SPACELIST *pSpaceList);

void *SpaceList_Alloc(SPACELIST *pSpaceList);
void SpaceList_Free(SPACELIST *pSpaceList, void *pData);

#endif /* _SPACELIST_H_ */

