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

/* 
 *	Cache.h
 *
 *	DESCRIPTION
 *		Interface to a generic cache type.
 *
 *	HISTORY
 *		10-02-2003	create by zhouweiming.
 *
 */
#ifndef __CACHE_H__
#define __CACHE_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CACHEFILE_st {
    char    *pszFileName;    /* Cache节点中的文件名 */
    UINT    uAccessTimes;    /* 本节点文件被访问的次数 */
    char    *pFileData;      /* 文件中的数据 */
    UINT    uFileLength;     /* 文件长度 */
    clock_t LastAccessTime;  /* 文件最后一次被访问的时间 */
} CACHEFILE;

typedef struct CACHE_st {
    UINT        uTotalSize;      /* Cache占用空间最大大小 */
    UINT        uUsedSize;       /* Cache中使用了的空间大小 */
    UINT        uBucketCount;    /* 管理CACHE的哈希表的BUCKET大小 */
    clock_t     CollageTime;    /* 定期清除访问不频繁文件的时间 */
    UINT        uMinAccessTimes; /* 最小的访问次数，为清除时使用 */
    HASHTABLE   *pTable;         /* 保存Cache文件的哈希表 */
} CACHE;

CACHE *Cache_Create(UINT uTotalSize, UINT uCollageTime, UINT uBucketCount,
                    UINT uMinAccessTimes);
void       Cache_Destroy(CACHE *pCache);
CACHEFILE* Cache_AddFile(CACHE *pCache, char *pszFileName, 
               char *pFileData, UINT uFileLength);
void       Cache_Delete(CACHE *pCache, char *pszFileName);
CACHEFILE *Cache_Find(CACHE *pCache, char *pszFileName);
void       Cache_Clean(CACHE *pCache);

void Cache_SetMinAccessTimes(CACHE *pCache, UINT uMinAccessTimes);
void Cache_SetCollageTime(CACHE *pCache, clock_t CollageTime);

#ifdef __cplusplus
}
#endif

#endif /* __CACHE_H__ */