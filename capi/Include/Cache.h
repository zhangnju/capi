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
    char    *pszFileName;    /* Cache�ڵ��е��ļ��� */
    UINT    uAccessTimes;    /* ���ڵ��ļ������ʵĴ��� */
    char    *pFileData;      /* �ļ��е����� */
    UINT    uFileLength;     /* �ļ����� */
    clock_t LastAccessTime;  /* �ļ����һ�α����ʵ�ʱ�� */
} CACHEFILE;

typedef struct CACHE_st {
    UINT        uTotalSize;      /* Cacheռ�ÿռ�����С */
    UINT        uUsedSize;       /* Cache��ʹ���˵Ŀռ��С */
    UINT        uBucketCount;    /* ����CACHE�Ĺ�ϣ���BUCKET��С */
    clock_t     CollageTime;    /* ����������ʲ�Ƶ���ļ���ʱ�� */
    UINT        uMinAccessTimes; /* ��С�ķ��ʴ�����Ϊ���ʱʹ�� */
    HASHTABLE   *pTable;         /* ����Cache�ļ��Ĺ�ϣ�� */
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