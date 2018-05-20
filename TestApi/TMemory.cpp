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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "capiglobal.h"
#include "TMemory.h"
#include "TestApi_Assert.h"
#include "ApiHook.h"
#include "DebugHeap.h"
#include "FastLock.h"

#pragma warning(disable : 4996)

typedef struct MEMNODE_st{
    DWORD           dwAddr;
    DWORD           dwRetAddr;
    unsigned int    uSize;
    unsigned int    uLine;
    char    *       pszFileName;
} MEMNODE;

#define             MAX_MEMHOOK_FUNCTIONS   32
static HANDLE       g_hHook = NULL;
static FASTLOCK     g_LockMalloc;


THashTable   g_MemTable;

UINT HashMemNode(void *pKey, UINT uBucketCount)
{
    MEMNODE *pNode = (MEMNODE *)pKey;
    return (pNode->dwAddr % uBucketCount);
}

int MemNodeCompare(void *p1, void *p2)
{
    MEMNODE *pNode1;
    pNode1 = (MEMNODE *)p1;

    if ( pNode1->dwAddr == (DWORD)p2 )
    {
        return 0;
    }
    else if ( pNode1->dwAddr > (DWORD)p2 )
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

void MemNodeDestroy(void *p)
{
    MEMNODE *pNode = (MEMNODE *)p;
    if ( pNode != NULL )
    {
        unhooked_free(pNode->pszFileName);
        unhooked_free((void *)pNode);
    }
}


TCheckMemory::TCheckMemory()
{
    INT nRet;
    nRet = g_MemTable.Create(DEFAULT_HASHTABLE_SIZE, HashMemNode, 
        MemNodeCompare, MemNodeDestroy);
    assertTrue( nRet == CAPI_SUCCESS );
}



extern "C" void *Tmalloc_dbg(size_t size, char *pszFile, unsigned int uLine)
{
    char *pAddr;
    DWORD dwRetAddr = 0;

    __asm {
        mov eax, dword ptr [ebp+4]
        mov dwRetAddr, eax
    }
    
    pAddr = (char *)unhooked_malloc(size+sizeof(UINT));

    assertTrue( pAddr != NULL);

    *(UINT *)(pAddr+size) = MEM_CHECK_FLAG;
    
    MEMNODE *pNode = NULL;
    
    pNode = (MEMNODE *)unhooked_malloc(sizeof(MEMNODE));
    assertTrue( pNode != NULL);

    pNode->dwAddr = (DWORD)pAddr;
    pNode->uSize = size;
    pNode->dwRetAddr = dwRetAddr;
    pNode->pszFileName = (char *)unhooked_malloc(strlen(pszFile)+1);
    assertTrue( pNode->pszFileName != NULL);

    strcpy(pNode->pszFileName, pszFile);
    pNode->uLine = uLine;

    (void)g_MemTable.Insert(pNode);
    
    return (void *)pAddr;
}

extern "C" void Tfree_dbg(void *p, char *pszFile, UINT uLine)
{
    MEMNODE *pNode;
    pNode = (MEMNODE *)g_MemTable.Find( p, HashInt );

    if ( pNode != NULL)
    {
        char *psz = (char *)pNode->dwAddr;
        UINT *pTail = (UINT *)(psz + pNode->uSize);
        if ( *pTail != MEM_CHECK_FLAG )
        {
            char msg[1024];
            sprintf(msg, "Memory overrun at 0x%x\n\t malloc position: "
                    "File:%s, Line:%d\n \tfree position: File:%s, Line:%d\n", 
              pNode->dwAddr, pNode->pszFileName, pNode->uLine, pszFile, uLine);
            printf(msg);
            printf("psz = %s\n", psz);
        }
        (void)g_MemTable.Delete((void *)psz, HashInt);
        unhooked_free(p);
    }
    else
    {
        unhooked_free(p);
        printf("Try to free a undefined memeory not alloced by malloc().\n"
            "File:%s, Line:%d.\n", pszFile, uLine);
    }
}

void *Tcalloc_dbg(size_t num, size_t size, char *pszFile, UINT uLine)
{
    return Tmalloc_dbg(num * size, pszFile, uLine);
}

void *Trealloc_dbg(void *memblock, size_t size, char *pszFile, UINT uLine)
{
    MEMNODE *pNode;
    DWORD dwRetAddr = 0;

    __asm {
        mov eax, dword ptr [ebp+4]
        mov dwRetAddr, eax
    }
    
        pNode = (MEMNODE *)g_MemTable.Find(memblock, HashInt);
        if ( pNode != NULL)
        {
            MEMNODE *pNewNode;

            char *pAddr = (char *)unhooked_malloc(size+sizeof(UINT));
            assertTrue( pAddr != NULL );

            memcpy(pAddr, memblock, pNode->uSize);
            Tfree(memblock);

            *(UINT *)(pAddr+size) = MEM_CHECK_FLAG;

            pNewNode = (MEMNODE *)unhooked_malloc(sizeof(MEMNODE));
            assertTrue( pNewNode != NULL );

            pNewNode->dwAddr = (DWORD)pAddr;
            pNewNode->uSize = size;
            pNewNode->dwRetAddr = dwRetAddr;
            pNewNode->pszFileName = pszFile;
            pNewNode->uLine = uLine;
        
            (void)g_MemTable.Insert(pNewNode);

            return pAddr;
        }
    return unhooked_realloc(memblock, size);
}

char *Tstrdup_dbg(const char *strSource, char *pszFile, UINT uLine)
{
    char *psz = (char *)Tmalloc_dbg(strlen(strSource)+1, pszFile, uLine);
    strcpy(psz, strSource);
    return psz;
}

wchar_t *Twcsdup_dbg(const wchar_t *strSource, char *pszFile, UINT uLine)
{
    wchar_t *psz = (wchar_t *)Tmalloc_dbg(wcslen(strSource)+2, pszFile, uLine);
    (void)wcscpy(psz, strSource);
    return psz;
}


void *Tmalloc(size_t size)
{
    char *pAddr;
    DWORD dwRetAddr = 0;

    __asm {
        mov eax, dword ptr [ebp+4]
        mov dwRetAddr, eax
    }
    
    pAddr = (char *)unhooked_malloc(size+sizeof(UINT));

    assertTrue( pAddr != NULL);

    *(UINT *)(pAddr+size) = MEM_CHECK_FLAG;
    
    MEMNODE *pNode = NULL;
    
    pNode = (MEMNODE *)unhooked_malloc(sizeof(MEMNODE));

    assertTrue( pNode != NULL);

    pNode->dwAddr = (DWORD)pAddr;
    pNode->uSize = size;
    pNode->dwRetAddr = dwRetAddr;
    pNode->pszFileName = NULL;
    pNode->uLine = 0;

    (void)g_MemTable.Insert(pNode);
    
    return (void *)pAddr;
}


void *Trealloc(void *p, size_t size)
{
    MEMNODE *pNode;
    DWORD dwRetAddr = 0;

    __asm {
        mov eax, dword ptr [ebp+4]
        mov dwRetAddr, eax
    }
    
        pNode = (MEMNODE *)g_MemTable.Find( p, HashInt );
        if ( pNode != NULL)
        {
            MEMNODE *pNewNode;

            char *pAddr = (char *)unhooked_malloc(size+sizeof(UINT));
            assertTrue( pAddr != NULL );

            (void)memcpy(pAddr, p, pNode->uSize);
            Tfree(p);

            *(UINT *)(pAddr+size) = MEM_CHECK_FLAG;

            pNewNode = (MEMNODE *)unhooked_malloc(sizeof(MEMNODE));

            pNewNode->dwAddr = (DWORD)pAddr;
            pNewNode->uSize = size;
            pNewNode->dwRetAddr = dwRetAddr;
            pNewNode->pszFileName = NULL;
            pNewNode->uLine = 0;
        
            (void)g_MemTable.Insert(pNewNode);

            return pAddr;
        }
    return unhooked_realloc(p, size);
}

void Tfree( void *p)
{
    MEMNODE *pNode;
    g_MemTable.Lock();
    pNode = (MEMNODE *)g_MemTable.Find_S( p, HashInt );
    if ( pNode != NULL)
    {
        char *psz = (char *)pNode->dwAddr;
        UINT *pTail = (UINT *)(psz + pNode->uSize);
        if ( *pTail != MEM_CHECK_FLAG )
        {
            char msg[1024];
            sprintf(msg, "Memory overrun at 0x%x\n\t  position: "
                    "File:%s, Line:%d, Func Address: 0x%x\n", pNode->dwAddr, 
                    pNode->pszFileName, pNode->uLine, pNode->dwRetAddr);
            printf(msg);
            printf("psz = %s\n", psz);

        }
        (void)g_MemTable.Delete_S((void *)psz, HashInt);
    }
    else
    {
        //printf("Try to free a unhooked memeory not alloced by Tmalloc().\n");
    }
    unhooked_free(p);
    g_MemTable.Unlock();
}


void TCheckMemory::TCheckMemoryLeak()
{
    UINT    uNodeCount;
    MEMNODE *pNode;
    char msg[1024];

    uNodeCount = g_MemTable.GetNodeCount();
    if ( uNodeCount == 0 )
    {
        return;
    }
    g_MemTable.Lock();
    g_MemTable.EnumBegin();
    while( (pNode = (MEMNODE *)g_MemTable.EnumNext()) != NULL )
    {
        sprintf(msg, "Memory leak at 0x%x\n\t malloc() position: "
                     "File:%s, Line:%d\n",
                      pNode->dwAddr, pNode->pszFileName, pNode->uLine);
        printf(msg);
    }

    g_MemTable.Unlock();
    sprintf(msg, "TCheckMemoryLeak(): Total memory leak count: %d\n", 
        uNodeCount);
    printf(msg);
}

void TCheckMemory::TCheckMemoryOverrun()
{
    UINT    uNodeCount;
    UINT    uOverRunCount;
    MEMNODE *pNode;
    char msg[1024];

    uNodeCount = g_MemTable.GetNodeCount();
    if ( uNodeCount == 0 )
    {
        return;
    }
    uOverRunCount = 0;
    g_MemTable.Lock();
    g_MemTable.EnumBegin();
    while( (pNode = (MEMNODE *)g_MemTable.EnumNext()) != NULL )
    {
        char *psz = (char *)pNode->dwAddr;
        UINT *pTail = (UINT *)(psz + pNode->uSize);
        if (  *pTail != MEM_CHECK_FLAG )
        {
            sprintf(msg, "Memory overrun at 0x%x\n\t malloc() position: "
                    "File:%s, Line:%d\n", 
                pNode->dwAddr, pNode->pszFileName, pNode->uLine);
            printf(msg);
            uOverRunCount += 1;
        }
    }
    g_MemTable.Unlock();

    sprintf(msg, "TCheckMemory(): Total memory overrun count: %d\n", 
        uOverRunCount);
    printf(msg);
}

void TCheckMemory::TCheckMemAll()
{
    UINT    uNodeCount;
    UINT    uOverRunCount;
    MEMNODE *pNode;
    char msg[1024];

    uNodeCount = g_MemTable.GetNodeCount();
    if ( uNodeCount == 0 )
    {
        return;
    }
    uOverRunCount = 0;

    printf("Check Memory All Begin...\n\n");
    g_MemTable.Lock();
    g_MemTable.EnumBegin();
    while ((pNode = (MEMNODE *)g_MemTable.EnumNext_S()) != NULL)
    {
        char *psz = (char *)pNode->dwAddr;
        UINT *pTail = (UINT *)(psz + pNode->uSize);
        if ( *pTail != MEM_CHECK_FLAG )
        {
            sprintf(msg, "Memory overrun at 0x%x\n\t malloc() position: "
                "File:%s, Line:%d, Func Address:0x%x\n", 
                pNode->dwAddr, pNode->pszFileName, pNode->uLine, 
                pNode->dwRetAddr);

            printf(msg);
            printf("psz = 0x%lx\n", *pTail);
            uOverRunCount += 1;
        }
        sprintf(msg, "Memory leak at 0x%x\n\t malloc() position: "
                     "File:%s, Line:%d, Func Address:0x%x\n",
                     pNode->dwAddr, pNode->pszFileName, pNode->uLine,
                     pNode->dwRetAddr);
        printf(msg);
    }
    g_MemTable.Unlock();
    sprintf(msg, "TCheckMemory(): Total memory leak count: %d, "
        "memory overrun count: %d\n", 
        uNodeCount, uOverRunCount);
    printf(msg);
}

void TCheckMemory::Destroy()
{
}

TCheckMemory::~TCheckMemory()
{
}


void TMemHookInit()
{
    g_hHook = ApiHook_Init(MAX_MEMHOOK_FUNCTIONS);
    (void)ApiHook_SetByAddr(g_hHook, (DWORD)free, (DWORD)Tfree);   
    (void)ApiHook_SetByAddr(g_hHook, (DWORD)malloc, (DWORD)Tmalloc);   
    (void)ApiHook_SetByAddr(g_hHook, (DWORD)realloc, (DWORD)Trealloc); 
    FastLockInit(&g_LockMalloc);

    
}

void *unhooked_malloc(size_t size)
{
    void *pAddr;
    FastLockAcquire(&g_LockMalloc);
    pAddr = HeapAlloc(GetProcessHeap(), 0, size);
//    pAddr = LocalAlloc(LMEM_FIXED, size);
    FastLockRelease(&g_LockMalloc);
    return pAddr;

    FastLockAcquire(&g_LockMalloc);
    if ( g_hHook != NULL )
    {
        (void)ApiHook_UnSetByAddr(g_hHook, (DWORD)malloc);
        pAddr = malloc(size);
        (void)ApiHook_SetByAddr(g_hHook, (DWORD)malloc, (DWORD)Tmalloc); 
    }
    else
    {
        pAddr = org_malloc(size);
    }
    FastLockRelease(&g_LockMalloc);
    return pAddr;
}

void *unhooked_realloc(void *p, size_t size)
{
    void *pAddr;
    FastLockAcquire(&g_LockMalloc);
    if ( g_hHook != NULL )
    {
        (void)ApiHook_UnSetByAddr(g_hHook, (DWORD)realloc);
        pAddr = realloc(p, size);
        (void)ApiHook_SetByAddr(g_hHook, (DWORD)realloc, (DWORD)Trealloc); 
    }
    else
    {
        pAddr = org_realloc(p, size);
    }
    FastLockRelease(&g_LockMalloc);
    return pAddr;
}

void unhooked_free(void *p)
{
    FastLockAcquire(&g_LockMalloc);
    HeapFree(GetProcessHeap(), 0, p);
    //LocalFree(p);
    FastLockRelease(&g_LockMalloc);
    return;

    FastLockAcquire(&g_LockMalloc);
    if ( g_hHook != NULL )
    {
        (void)ApiHook_UnSetByAddr(g_hHook, (DWORD)free);
        free(p);
        (void)ApiHook_SetByAddr(g_hHook, (DWORD)free, (DWORD)Tfree);   
    }
    else
    {
        org_free(p);
    }
    FastLockRelease(&g_LockMalloc);
}

void TMemHookClose()
{
    FastLockAcquire(&g_LockMalloc);
    (void)ApiHook_Close(g_hHook);
    g_hHook = NULL;
    FastLockRelease(&g_LockMalloc);
}
