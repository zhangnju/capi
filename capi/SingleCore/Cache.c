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

/**	创建一个CACHE文件管理表

	@param	UINT uTotalSize - CACHE的大小	
	@param	UINT uCollageTime - CACHE清理时间间隔	
	@param	UINT uBucketCount - 管理CACHE的哈希表的BUCKET大小	
	@return	CACHE * - 成功返回创建的CACHE指针，失败返回NULL.	
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

/**	哈希表操作中求哈希值的回调函数，仅供CACHE管理模块使用

	@param	CACHEFILE *pCacheFile - CACHEFILE指针	
	@param	INT uBucketCount - 哈希表的BUCKET大小	
	@return	INT - 计算出来的哈希值	
*/
INT HashCacheFile(void *p, UINT uBucketCount)
{
    return HashString(((CACHEFILE *)p)->pszFileName, uBucketCount);
}


/**	哈希表的比较回调函数，将CACHEFILE结构和一个文件名进行比较

	@param	void *p1 - 指向一个CACHEFILE结构	
	@param	void *p2 - 指向一个文件名的字符串	
	@return	INT - 相等返回0，p1<p2返回-1, p1>p2返回1	
*/
INT CacheFileCompare(void *p1, void *p2)
{
    CACHEFILE   *pCacheFile;
    char *pszFileName;
    
    pCacheFile = (CACHEFILE *)p1;
    pszFileName = (char *)p2;

    return stricmp(pCacheFile->pszFileName, pszFileName);
}

/**	哈希表的回调函数，用来释放一个CACHEFILE结构体

	@param	void *p - 指向要释放的CACHEFILE结构体	
	@return	void - 无	
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

/**	CACHE管理的添加文件函数

	@param	CACHE *pCache - CACHE指针	
	@param	char *pszFileName -文件名	
	@param	char *pFileData - 存放在内存中的文件数据指针	
	@param	UINT uFileLength - 文件长度	
	@return	CACHEFILE * - 成功返回一个CACHEFILE指针，失败返回NULL	
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

/**	从CACHE管理中删除一个文件

	@param	CACHE *pCache - CACHE指针	
	@param	char *pszFileName - 要删除的文件名	
	@return	void - 无	
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



/**	从CACHE中查找一个文件

	@param	CACHE *pCache - CACHE指针	
	@param	char *pszFileName - 要查找的文件名	
	@return	CACHEFILE * - 成功返回一个CACHEFILE指针，失败返回NULL	
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


/**	将CACHE释放掉

	@param	CACHE *pCache - 要释放的CACHE指针	
	@return	void - 无	
*/
void Cache_Destroy(CACHE *pCache)
{
    if ( pCache != NULL )
    {
        HashTable_Destroy(pCache->pTable, CacheFileDestroy);
        free(pCache);
    }
}

/**	CACHE的清除函数, 清除掉CACHE中所有的访问次数低于某个次数或者

	@param	CACHE *pCache - 要进行清理的CACHE指针	
	@return	void - 无	
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

/**	设置最小访问次数函数

	@param	CACHE *pCache - CACHE指针	
	@param	UINT uMinAccessTimes - 最小访问次数	
	@return	void - 无	
*/
void Cache_SetMinAccessTimes(CACHE *pCache, UINT uMinAccessTimes)
{
    if ( pCache != NULL )
    {
        pCache->uMinAccessTimes = uMinAccessTimes;
    }
}

/**	设置CACHE清理时间间隔

	@param	CACHE *pCache - CACHE指针	
	@param	clock_t CollageTime - CACHE清理时间间隔	
	@return	void -无	
*/
void Cache_SetCollageTime(CACHE *pCache, clock_t CollageTime)
{
    if ( pCache != NULL )
    {
        pCache->CollageTime = CollageTime;
    }
}