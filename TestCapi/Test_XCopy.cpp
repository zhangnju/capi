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
#include <io.h>
#include <stdio.h>
#include "capiglobal.h"
#include "Testapi.h"

extern "C" void Xcopy( char *pszSrcDir, char *pszTargeDir, BOOL bOverWrite );



void TestCase_XCopy(void);

void Test_XCopy()
{
    TestCase_XCopy();
}

//REGISTER_TESTFUNC(Test_XCopy)


void MakeDir(char *dir)
{
    CreateDirectory(dir, NULL);
    if ( _access(dir, 0 ) == 0 )
    {
        printf("Directory %s created successful.\n", dir);
    }
    printf("Directory %s created successful.\n", dir);
    return;
}

void BuildTestFile(char *filename, char *content, int len)
{
    FILE *fp;

    fp = fopen( filename, "wb");
    if ( fp != NULL )
    {
        fwrite(content,len, 1, fp);
        fclose(fp);

        if ( _access(filename, 0 ) == 0 )
        {
            printf("File %s created successful.\n", filename);
        }
    }
    return;
}


int CheckFile(char *filename)
{
    if ( _access(filename, 0 ) == -1 )
    {
        printf("File %s not exist.\n", filename);
        return -1;
    }
    return 0;
}


void TestCase_XCopy(void)
{

    MakeDir("c:\\Test_XCopy");
    MakeDir("c:\\Test_XCopy\\aa");
    MakeDir("c:\\Test_XCopy\\bb");
    MakeDir("c:\\Test_XCopy\\cc");
    MakeDir("c:\\Test_XCopy\\aa\\11");
    MakeDir("c:\\Test_XCopy\\aa\\12");
    MakeDir("c:\\Test_XCopy\\bb\\21\\");


    BuildTestFile("c:\\Test_XCopy\\test1.txt", "1234", 4);
    BuildTestFile("c:\\Test_XCopy\\aa\\test2.txt", "3456", 4);
    BuildTestFile("c:\\Test_XCopy\\bb\\test3.txt", "7908", 4);
    BuildTestFile("c:\\Test_XCopy\\aa\\11\\test4.txt", "4444", 4);


    printf("Press a key for continue to test XCopy()...\n");
    getchar();

    Xcopy("C:\\Test_XCopy", "C:\\Test_XCopy22", TRUE);


    CheckFile("c:\\Test_XCopy22\\test1.txt");
    CheckFile("c:\\Test_XCopy22\\aa\\test2.txt");
    CheckFile("c:\\Test_XCopy22\\bb\\test3.txt");
    CheckFile("c:\\Test_XCopy22\\aa\\11\\test4.txt");

    CheckFile("c:\\Test_XCopy22\\cc");
    CheckFile("c:\\Test_XCopy22\\aa\\12");
    CheckFile("c:\\Test_XCopy22\\bb\\21");

}

