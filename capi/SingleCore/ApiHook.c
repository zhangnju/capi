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
#include "ApiHook.h"

#pragma warning(disable : 4996)

typedef struct APIHOOKDATA_st {
	DWORD	dwSrcFuncAddr;   /* Դ�����ĵ�ַ */
	DWORD	dwNewFuncAddr;   /* ���Ӻ����ĵ�ַ */
	BYTE	byHeaderCode[6]; /* Դ������ʼ6�ֽ� */
	WORD	wFlag;           /* ������ʾ�Ƿ�������HOOK�ı�־ */
	char	*pszDllName;     
	char	*pszFuncName;    
} APIHOOKDATA;

typedef struct APIHOOK_st {
	APIHOOKDATA	*pHookData;
	UINT		uMaxFunctions;
} APIHOOK;


/**	ApiHook ģ���ʼ������

	@param	INT nMaxHookFuncCounts - �������õĹ�������	
	@return	INT (by default) - NULL ��ʾʧ��
*/
HANDLE ApiHook_Init(UINT uMaxFunctions)
{
	APIHOOK	* pApiHook = (APIHOOK *)malloc(sizeof(APIHOOK));

	if ( pApiHook )
	{
		pApiHook->uMaxFunctions = uMaxFunctions;
		pApiHook->pHookData = (APIHOOKDATA *)malloc(sizeof(APIHOOKDATA) * uMaxFunctions);
		if ( NULL != pApiHook->pHookData)
		{
			memset( pApiHook->pHookData, 0, sizeof(APIHOOKDATA) * uMaxFunctions );
			return (HANDLE)pApiHook;
		}
		free(pApiHook);
	}

	return NULL;
}


/**	ͨ����ַ������ĳ�������Ĺ��Ӻ���

	@param	HANDLE hApiHook - ��ApiHook_Init()�������ɵľ��	
	@param	DWORD dwSrcFuncAddr - Դ������ַ	
	@param	DWORD dwNewFuncAddr - ���Ӻ�����ַ	
	@return	INT (by default) - -1��ʾʧ�ܣ�>=0��ʾ��hook�����е����	
*/
INT ApiHook_SetByAddr(HANDLE hApiHook, DWORD dwSrcFuncAddr, DWORD dwNewFuncAddr)
{
	DWORD	dwOldProtect;
	DWORD	dwNewProtect;
	DWORD   lpSrcFunc;
    DWORD   lppNewFunc;
    UINT     i;
	INT		nAlreadyFlag = 0;

	APIHOOK		*pApiHook = (APIHOOK *)hApiHook;
	if ( NULL == hApiHook )
	{
		return -1;
	}

	lpSrcFunc = dwSrcFuncAddr;
    /* �����Ƿ��ѱ������˹��� */
	for ( i = 0; i < pApiHook->uMaxFunctions; i++ )
	{
		if ( pApiHook->pHookData[i].dwSrcFuncAddr == lpSrcFunc )
		{
			/* ����Ѿ��������˹��ӣ������ı�. */
			nAlreadyFlag = 1;
			break;
		}

	}
	/* ���û������Դ�����Ĺ��Ӻ������ڱ����ҳ�һ���ɹ���¼��λ��. */
	if ( i == pApiHook->uMaxFunctions )
	{
		for ( i = 0; i < pApiHook->uMaxFunctions; i++ )
		{
			if (pApiHook->pHookData[i].wFlag == 0 )
			{
				break;
			}
		}		
		if ( i == pApiHook->uMaxFunctions )
		{
			return -1;
		}
	}

    /* ���µĹ��Ӻ�����ַ��¼������ */
	pApiHook->pHookData[i].dwNewFuncAddr = dwNewFuncAddr;
	
	/* ������δ��뽫Դ����ͷ��6���ֽڱ��浽���� */
	lppNewFunc = (DWORD)(&(pApiHook->pHookData[i].dwNewFuncAddr) );

	if ( !nAlreadyFlag )
	{
		/* ��Դ������ʼ��6���ֽڱ��浽 byHeaderCode.�� */
        memcpy( pApiHook->pHookData[i].byHeaderCode, (const void *)lpSrcFunc, 6);
	}

    /* ������δ��뽫Դ�����ײ�6���ֽڸĳ�Ϊһ����ת���º�����ַ��ָ�� */
    if ( VirtualProtect( (LPVOID)lpSrcFunc,
                         6,
                         PAGE_EXECUTE_READWRITE,
                         &dwOldProtect ) == 0 ) 
    {
        /* 
         * Failure.
         */
        return -1;
    }
	
    *(unsigned char *)lpSrcFunc = (unsigned char)0xff;
    *(((unsigned char *)lpSrcFunc)+1) = (unsigned char)0x25;

    memcpy( (void *)(lpSrcFunc+2), 
            (const void *)&lppNewFunc, 
            4 );                            /* address  */ 

    if ( VirtualProtect( (LPVOID)lpSrcFunc,
                         6,
                         dwOldProtect,
						 &dwNewProtect) == 0 ) {
        /* 
         * Failure.
         */
        return -1;
    };

	pApiHook->pHookData[i].wFlag = 1;
	pApiHook->pHookData[i].dwSrcFuncAddr = lpSrcFunc; 

	/* ʹ������FlushInstructionCache������֧�ֶ��CPU�ϵ�CACAHE�ĸ��£�
	 * ����Ĵ���ֻ�������ڴ���ĵ�ַ��û�и���CPU CACHE��Ķ�Ӧ���ݡ�
	 */
	FlushInstructionCache(GetCurrentProcess(), NULL, 0);

	return (INT)i;
}


INT ApiHook_SetByName(HANDLE hApiHook,
			   char *pszDllName, 
			   char *pszFuncName, 
			   DWORD dwNewFuncAddr)
{
    DWORD dwFuncAddr; 
    APIHOOK *pApiHook;
    INT i;

    HMODULE hModule = GetModuleHandle( pszDllName );
    if ( !hModule ) {
        /* 
         * Winsock 1.1 not loaded (would happen for non-Winsock apps, and
         * apps that exclusively use Winsock 2.
         */
        return -1;
    }

    /*
     * Step 2:
     *  Find the address of the function.
     */
    dwFuncAddr = (DWORD)GetProcAddress( hModule, pszFuncName );
    if ( !dwFuncAddr ) {
        /*
         * This should never happen.
         */
        return -1;
    }

    i = ApiHook_SetByAddr(hApiHook, dwFuncAddr, dwNewFuncAddr);
    pApiHook = (APIHOOK *)hApiHook;
    if ( pApiHook->pHookData[i].pszDllName )
    {
        free(pApiHook->pHookData[i].pszDllName);
        pApiHook->pHookData[i].pszDllName = NULL;
    }
    if ( pApiHook->pHookData[i].pszFuncName )
    {
        free( pApiHook->pHookData[i].pszFuncName );
        pApiHook->pHookData[i].pszFuncName = NULL;
    }
    pApiHook->pHookData[i].pszDllName = strdup(pszDllName);
    pApiHook->pHookData[i].pszFuncName = strdup(pszFuncName);

    return i;
}


/**	ȡ��ĳ�������Ĺ��Ӻ�������

	@param	HANDLE hApiHook - ��ApiHook_Init()�������ɵľ��	
	@param	DWORD dwSrcFuncAddr - Դ������ַ	
	@return	INT (by default) - -1��ʾʧ�ܣ�>=0��ʾ��hook�����е����	
*/
INT	ApiHook_UnSetByAddr(HANDLE hApiHook, DWORD dwSrcFuncAddr )
{
	UINT i;
	DWORD	dwOldProtect;
	DWORD	dwNewProtect;
    DWORD lpSrcFunc;
    
	APIHOOK	*pApiHook = (APIHOOK *)hApiHook;

	if ( NULL == hApiHook )
	{
		return -1;
	}
	for ( i = 0; i < pApiHook->uMaxFunctions; i++ )
	{
		if ( pApiHook->pHookData[i].dwSrcFuncAddr == dwSrcFuncAddr )
		{
			break;
		}
	}

	if ( i == pApiHook->uMaxFunctions )
	{
		return -1;
	}
	
	lpSrcFunc = pApiHook->pHookData[i].dwSrcFuncAddr;
     
    /* �޸�lpSrcFuncָ����ڴ�����Ϊ���Զ�д�� */
    if ( VirtualProtect( (LPVOID)lpSrcFunc, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect ) == 0 ) 
    {
        return -1;
    }

    memcpy( (void *)lpSrcFunc, pApiHook->pHookData[i].byHeaderCode, 6 );

    /* �ָ�lpSrcFuncָ����ڴ����� */
    if ( VirtualProtect( (LPVOID)lpSrcFunc, 6, dwOldProtect, &dwNewProtect ) == 0 ) 
    {
        return -1;
    }

    pApiHook->pHookData[i].wFlag = 0;
    if ( pApiHook->pHookData[i].pszDllName )
    {
        free(pApiHook->pHookData[i].pszDllName);
        pApiHook->pHookData[i].pszDllName = NULL;
    }
    if ( pApiHook->pHookData[i].pszFuncName )
    {
        free( pApiHook->pHookData[i].pszFuncName );
        pApiHook->pHookData[i].pszFuncName = NULL;
    }
    /* ʹ������FlushInstructionCache������֧�ֶ��CPU�ϵ�CACAHE�ĸ��£�
    * ����Ĵ���ֻ�������ڴ���ĵ�ַ��û�и���CPU CACHE��Ķ�Ӧ���ݡ�
    */
    FlushInstructionCache(GetCurrentProcess(), NULL, 0);

    return (INT)i;
}


INT ApiHook_UnSetByName(HANDLE hApiHook,
			   char *pszDllName, 
			   char *pszFuncName )
{
	INT		ret;
    HMODULE hModule;
    DWORD dwSrcFuncAddr;

    ret = -1;
    hModule = GetModuleHandle( pszDllName );
    if ( !hModule ) {
        /* 
         * Winsock 1.1 not loaded (would happen for non-Winsock apps, and
         * apps that exclusively use Winsock 2.
         */
        return -1;
    }

    /*
     * Step 2:
     *  Find the address of the function.
     */
    dwSrcFuncAddr = (DWORD)GetProcAddress( hModule, pszFuncName );
    if ( !dwSrcFuncAddr ) {
        /*
         * This should never happen.
         */
        return -1;
    }

    ret = ApiHook_UnSetByAddr(hApiHook, dwSrcFuncAddr);

    return ret;
}


UINT ApiHook_GetMaxHooks(HANDLE hApiHook)
{
    UINT uCount;
    UINT	i;
	APIHOOK	*pApiHook;
    
    pApiHook= (APIHOOK *)hApiHook;
    uCount = 0;

	if ( !hApiHook )
	{
		return 0;
	}
	for ( i = 0; i < pApiHook->uMaxFunctions; i++ )
	{
		if ( pApiHook->pHookData[i].wFlag == 1 )
		{
			uCount += 1;
		}
	}
	return uCount;
}


/**	ApiHookģ��رպ���

	@param	HANDLE - ��ApiHook_Init()�������ɵľ��		
	@return	INT (by default) - ʧ�ܷ���-1, �ɹ�����0��	
*/
INT ApiHook_Close(HANDLE hApiHook)
{
	if ( NULL != hApiHook )
	{
        UINT i;
		APIHOOK *p = (APIHOOK *)hApiHook; 
		for ( i = 0; i < p->uMaxFunctions; i++ )
		{
			if ( p->pHookData[i].wFlag )
			{
				DWORD	dwOldProtect;
				DWORD	dwNewProtect;

				DWORD lpSrcFunc = p->pHookData[i].dwSrcFuncAddr;
				/* 
				 * Rewrite the bytes for future calls.
				 */
				if ( VirtualProtect( (LPVOID)lpSrcFunc,
									 6,
									 PAGE_EXECUTE_READWRITE,
									 &dwOldProtect ) == 0 ) {
					/* 
					 * Failure.
					 */
					return -1;
				}

				memcpy( (void *)lpSrcFunc, 
						(const void *)p->pHookData[i].byHeaderCode, 
						6 );

				if ( VirtualProtect( (LPVOID)lpSrcFunc,
									 6,
									 dwOldProtect,
									 &dwNewProtect ) == 0 ) {
					/* 
					 * Failure.
					 */
					return -1;
				}
			}
			if ( p->pHookData[i].pszDllName )
			{
				free( p->pHookData[i].pszDllName );
			}
			if ( p->pHookData[i].pszFuncName )
			{
				free( p->pHookData[i].pszFuncName );
			}
		}
		free(p->pHookData);
		free(p);
	}
    /* ʹ������FlushInstructionCache������֧�ֶ��CPU�ϵ�CACAHE�ĸ��£�
    * ����Ĵ���ֻ�������ڴ���ĵ�ַ��û�и���CPU CACHE��Ķ�Ӧ���ݡ�
    */
    FlushInstructionCache(GetCurrentProcess(), NULL, 0);


	return 0;
}

