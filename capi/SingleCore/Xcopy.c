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

/** ��һ��Ŀ¼����Ŀ¼�µ������ļ����Ƶ�����һ��Ŀ¼��
    @param char *pszSrcDir - Ҫ������ԴĿ¼ 
    @param char *pszTargeDir - Ŀ��Ŀ¼ 
    @param BOOL bOverWrite - ���Ǳ�־��FALSE��ʾ���� 
    @return void - �� 
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
                /* ���� . and .. ������ */
                continue;
        }
        if ( lfData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
            /* Ŀ¼ */
            char szBaseDir[MAX_PATH];
            char szTargeDir[MAX_PATH];
            sprintf( szBaseDir, "%s\\%s", pszSrcDir, lfData.cFileName );
            sprintf( szTargeDir, "%s\\%s", pszTargeDir, lfData.cFileName );
            Xcopy( szBaseDir, szTargeDir, bOverWrite );
        }
        else {
            /* ��ͨ�ļ� */
            char szBaseDir[MAX_PATH];
            char szTargeDir[MAX_PATH];
            sprintf( szBaseDir, "%s\\%s", pszSrcDir, lfData.cFileName );
            sprintf( szTargeDir, "%s\\%s", pszTargeDir, lfData.cFileName );
            CopyFile( szBaseDir, szTargeDir, bOverWrite );
        }
    } while ( FindNextFile( lhFile, &lfData ));
    FindClose( lhFile );
}
