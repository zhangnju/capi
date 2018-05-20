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
#include "HashTable.h"
#include "Cache.h"

#pragma warning(disable : 4996)

#define     DEFAULT_COLLAGE_TIME            (1000*30)
#define     DEFAULT_MIN_ACCESS_TIMES        5

/**	����һ��CACHE�ļ������

	@param	UINT uTotalSize - CACHE�Ĵ�С	
	@param	UINT uCollageTime - CACHE����ʱ����	
	@param	UINT uBucketCount - ����CACHE�Ĺ�ϣ���BUCKET��С	
	@return	CACHE * - �ɹ����ش�����CACHEָ�룬ʧ�ܷ���NULL.	
*/
CACHE *Cache_Create(UINT uTotalSize, UINT uCollageTime, UINT uBucketCount,
                    UINT uMinAccessTimes)
{
    CACHE   *pCache;

    pCache = (CACHE *)malloc(sizeof(CACHE));
    if ( pCache != NULL )
    {
        pCache->uBucketCount = uBucketCount;
        pCache->uTotalSize = uTotalSize;
        pCache->uUsedSize = 0;
        pCache->CollageTime = DEFAULT_COLLAGE_TIME;
        pCache->uMinAccessTimes = DEFAULT_MIN_ACCESS_TIMES;

        pCache->pTable = HashTable_Create(uBucketCount);
        if ( pCache->pTable == NULL )
        {
            free( pCache );
            pCache = NULL;
        }
    }
    return pCache;
}

/**	��ϣ����������ϣֵ�Ļص�����������CACHE����ģ��ʹ��

	@param	CACHEFILE *pCacheFile - CACHEFILEָ��	
	@param	INT uBucketCount - ��ϣ���BUCKET��С	
	@return	INT - ��������Ĺ�ϣֵ	
*/
INT HashCacheFile(void *p, UINT uBucketCount)
{
    return HashString(((CACHEFILE *)p)->pszFileName, uBucketCount);
}


/**	��ϣ��ıȽϻص���������CACHEFILE�ṹ��һ���ļ������бȽ�

	@param	void *p1 - ָ��һ��CACHEFILE�ṹ	
	@param	void *p2 - ָ��һ���ļ������ַ���	
	@return	INT - ��ȷ���0��p1<p2����-1, p1>p2����1	
*/
INT CacheFileCompare(void *p1, void *p2)
{
    CACHEFILE   *pCacheFile;
    char *pszFileName;
    
    pCacheFile = (CACHEFILE *)p1;
    pszFileName = (char *)p2;

    return stricmp(pCacheFile->pszFileName, pszFileName);
}

/**	��ϣ��Ļص������������ͷ�һ��CACHEFILE�ṹ��

	@param	void *p - ָ��Ҫ�ͷŵ�CACHEFILE�ṹ��	
	@return	void - ��	
*/
void CacheFileDestroy(void *p)
{
    CACHEFILE   *pCacheFile = (CACHEFILE *)p;

    if ( pCacheFile != NULL )
    {
        if ( pCacheFile->pszFileName != NULL )
        {
            free(pCacheFile->pszFileName);
        }
        if ( pCacheFile->pFileData != NULL )
        {
            free(pCacheFile->pFileData);
        }
    }
}

/**	CACHE���������ļ�����

	@param	CACHE *pCache - CACHEָ��	
	@param	char *pszFileName -�ļ���	
	@param	char *pFileData - ������ڴ��е��ļ�����ָ��	
	@param	UINT uFileLength - �ļ�����	
	@return	CACHEFILE * - �ɹ�����һ��CACHEFILEָ�룬ʧ�ܷ���NULL	
*/
CACHEFILE *Cache_AddFile(CACHE *pCache, char *pszFileName, 
                char *pFileData, UINT uFileLength)
{
    CACHEFILE   *pCacheFile;

    pCacheFile = (CACHEFILE *)malloc(sizeof(CACHEFILE));
    if ( pCacheFile != NULL )
    {
        pCacheFile->pFileData = pFileData;
        pCacheFile->pszFileName = strdup(pszFileName);
        pCacheFile->uAccessTimes = 0;
        pCacheFile->uFileLength = uFileLength;

        pCache->uUsedSize += uFileLength;
        if ( pCache->uUsedSize > pCache->uTotalSize )
        {
            Cache_Clean(pCache);
        }
        HashTable_Insert(pCache->pTable, (void *)pCacheFile, HashCacheFile);
    }
    return pCacheFile;
}

/**	��CACHE������ɾ��һ���ļ�

	@param	CACHE *pCache - CACHEָ��	
	@param	char *pszFileName - Ҫɾ�����ļ���	
	@return	void - ��	
*/
void Cache_Delete(CACHE *pCache, char *pszFileName)
{
    CACHEFILE   *pCacheFile;
    pCacheFile = (CACHEFILE *)HashTable_Find(pCache->pTable, 
        (void *)pszFileName, HashCacheFile, CacheFileCompare);
    if ( pCacheFile != NULL )
    {
        pCache->uUsedSize -= pCacheFile->uFileLength;
        
        HashTable_Delete(pCache->pTable, (void *)pszFileName, 
            HashCacheFile, CacheFileCompare, CacheFileDestroy);
    }
}



/**	��CACHE�в���һ���ļ�

	@param	CACHE *pCache - CACHEָ��	
	@param	char *pszFileName - Ҫ���ҵ��ļ���	
	@return	CACHEFILE * - �ɹ�����һ��CACHEFILEָ�룬ʧ�ܷ���NULL	
*/
CACHEFILE *Cache_Find(CACHE *pCache, char *pszFileName)
{
    CACHEFILE   *pCacheFile;

    pCacheFile = (CACHEFILE *)HashTable_Find(pCache->pTable, 
        (void *)pszFileName, HashCacheFile, CacheFileCompare);
    if ( pCacheFile == NULL)
    {
        FILE    *fp;
        UINT    uFileLen;
        char    *pFileData;

        pCacheFile = NULL;
        
        fp = fopen(pszFileName, "rb");
        if (fp != NULL)
        {
            fseek(fp, 0, SEEK_END);
            uFileLen = ftell(fp);
            
            fseek(fp, 0, SEEK_SET);

            pFileData = (char *)malloc(uFileLen);
            if ( pFileData != NULL )
            {
                fread(pFileData, uFileLen, sizeof(char), fp);
                
                pCacheFile = Cache_AddFile(pCache, pszFileName, 
                    pFileData, uFileLen);
            }
            fclose(fp);
        }
        else
        {
            return NULL;
        }
    }
    pCacheFile->uAccessTimes += 1;
    pCacheFile->LastAccessTime = clock();
    return pCacheFile;
}


/**	��CACHE�ͷŵ�

	@param	CACHE *pCache - Ҫ�ͷŵ�CACHEָ��	
	@return	void - ��	
*/
void Cache_Destroy(CACHE *pCache)
{
    if ( pCache != NULL )
    {
        HashTable_Destroy(pCache->pTable, CacheFileDestroy);
        free(pCache);
    }
}

/**	CACHE���������, �����CACHE�����еķ��ʴ�������ĳ����������

	@param	CACHE *pCache - Ҫ���������CACHEָ��	
	@return	void - ��	
*/
void Cache_Clean(CACHE *pCache)
{
    CACHEFILE   *pCacheFile;
    clock_t     CurTime;

    HashTable_EnumBegin(pCache->pTable);

    CurTime = clock();
    
    while ((pCacheFile = HashTable_EnumNext(pCache->pTable)) != NULL )
    {
        if ( pCacheFile->uAccessTimes < pCache->uMinAccessTimes 
            || (pCacheFile->LastAccessTime - CurTime) 
                < pCache->CollageTime )
        {
            pCache->uUsedSize -= pCacheFile->uFileLength;
            
            HashTable_Delete(pCache->pTable, (void *)(pCacheFile->pszFileName), 
                HashCacheFile, CacheFileCompare, CacheFileDestroy);
        }
    }   
}

/**	������С���ʴ�������

	@param	CACHE *pCache - CACHEָ��	
	@param	UINT uMinAccessTimes - ��С���ʴ���	
	@return	void - ��	
*/
void Cache_SetMinAccessTimes(CACHE *pCache, UINT uMinAccessTimes)
{
    if ( pCache != NULL )
    {
        pCache->uMinAccessTimes = uMinAccessTimes;
    }
}

/**	����CACHE����ʱ����

	@param	CACHE *pCache - CACHEָ��	
	@param	clock_t CollageTime - CACHE����ʱ����	
	@return	void -��	
*/
void Cache_SetCollageTime(CACHE *pCache, clock_t CollageTime)
{
    if ( pCache != NULL )
    {
        pCache->CollageTime = CollageTime;
    }
}