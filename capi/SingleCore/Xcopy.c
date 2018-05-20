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

#pragma warning(disable : 4996)

/** 将一个目录及子目录下的所有文件复制到另外一个目录下
    @param char *pszSrcDir - 要拷贝的源目录 
    @param char *pszTargeDir - 目标目录 
    @param BOOL bOverWrite - 覆盖标志，FALSE表示覆盖 
    @return void - 无 
*/
void Xcopy( char *pszSrcDir, char *pszTargeDir, BOOL bOverWrite )
{
    char   lBaseSearch[MAX_PATH];
    HANDLE   lhFile;
    WIN32_FIND_DATA lfData;
    sprintf( lBaseSearch, "%s\\*.*", pszSrcDir );
    lhFile = FindFirstFile( lBaseSearch, &lfData );
    if ( lhFile == INVALID_HANDLE_VALUE ) {
        return;
    }
    do {
        CreateDirectory( pszTargeDir, NULL );
        if ( !strcmp( lfData.cFileName, "." ) ||
            !strcmp( lfData.cFileName, ".." )) {
                /* 跳过 . and .. 不处理 */
                continue;
        }
        if ( lfData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            /* 目录 */
            char szBaseDir[MAX_PATH];
            char szTargeDir[MAX_PATH];
            sprintf( szBaseDir, "%s\\%s", pszSrcDir, lfData.cFileName );
            sprintf( szTargeDir, "%s\\%s", pszTargeDir, lfData.cFileName );
            Xcopy( szBaseDir, szTargeDir, bOverWrite );
        }
        else {
            /* 普通文件 */
            char szBaseDir[MAX_PATH];
            char szTargeDir[MAX_PATH];
            sprintf( szBaseDir, "%s\\%s", pszSrcDir, lfData.cFileName );
            sprintf( szTargeDir, "%s\\%s", pszTargeDir, lfData.cFileName );
            CopyFile( szBaseDir, szTargeDir, bOverWrite );
        }
    } while ( FindNextFile( lhFile, &lfData ));
    FindClose( lhFile );
}
