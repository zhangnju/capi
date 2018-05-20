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
	DWORD	dwSrcFuncAddr;   /* 源函数的地址 */
	DWORD	dwNewFuncAddr;   /* 钩子函数的地址 */
	BYTE	byHeaderCode[6]; /* 源函数起始6字节 */
	WORD	wFlag;           /* 用来表示是否设置了HOOK的标志 */
	char	*pszDllName;     
	char	*pszFuncName;    
} APIHOOKDATA;

typedef struct APIHOOK_st {
	APIHOOKDATA	*pHookData;
	UINT		uMaxFunctions;
} APIHOOK;


/**	ApiHook 模块初始化函数

	@param	INT nMaxHookFuncCounts - 最大可设置的钩子数量	
	@return	INT (by default) - NULL 表示失败
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


/**	通过地址来设置某个函数的钩子函数

	@param	HANDLE hApiHook - 由ApiHook_Init()函数生成的句柄	
	@param	DWORD dwSrcFuncAddr - 源函数地址	
	@param	DWORD dwNewFuncAddr - 钩子函数地址	
	@return	INT (by default) - -1表示失败，>=0表示在hook数组中的序号	
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
    /* 查找是否已被设置了钩子 */
	for ( i = 0; i < pApiHook->uMaxFunctions; i++ )
	{
		if ( pApiHook->pHookData[i].dwSrcFuncAddr == lpSrcFunc )
		{
			/* 如果已经被设置了钩子，仅仅改变. */
			nAlreadyFlag = 1;
			break;
		}

	}
	/* 如果没有设置源函数的钩子函数，在表中找出一个可供记录的位置. */
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

    /* 将新的钩子函数地址记录到表中 */
	pApiHook->pHookData[i].dwNewFuncAddr = dwNewFuncAddr;
	
	/* 以下这段代码将源函数头部6个字节保存到表中 */
	lppNewFunc = (DWORD)(&(pApiHook->pHookData[i].dwNewFuncAddr) );

	if ( !nAlreadyFlag )
	{
		/* 将源函数起始处6个字节保存到 byHeaderCode.中 */
        memcpy( pApiHook->pHookData[i].byHeaderCode, (const void *)lpSrcFunc, 6);
	}

    /* 以下这段代码将源函数首部6个字节改成为一条跳转到新函数地址的指令 */
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

	/* 使用以下FlushInstructionCache调用来支持多核CPU上的CACAHE的更新，
	 * 上面的代码只更改了内存里的地址，没有更改CPU CACHE里的对应内容。
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


/**	取消某个函数的钩子函数设置

	@param	HANDLE hApiHook - 由ApiHook_Init()函数生成的句柄	
	@param	DWORD dwSrcFuncAddr - 源函数地址	
	@return	INT (by default) - -1表示失败，>=0表示在hook数组中的序号	
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
     
    /* 修改lpSrcFunc指向的内存属性为可以读写的 */
    if ( VirtualProtect( (LPVOID)lpSrcFunc, 6, PAGE_EXECUTE_READWRITE, &dwOldProtect ) == 0 ) 
    {
        return -1;
    }

    memcpy( (void *)lpSrcFunc, pApiHook->pHookData[i].byHeaderCode, 6 );

    /* 恢复lpSrcFunc指向的内存属性 */
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
    /* 使用以下FlushInstructionCache调用来支持多核CPU上的CACAHE的更新，
    * 上面的代码只更改了内存里的地址，没有更改CPU CACHE里的对应内容。
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


/**	ApiHook模块关闭函数

	@param	HANDLE - 由ApiHook_Init()函数生成的句柄		
	@return	INT (by default) - 失败返回-1, 成功返回0。	
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
    /* 使用以下FlushInstructionCache调用来支持多核CPU上的CACAHE的更新，
    * 上面的代码只更改了内存里的地址，没有更改CPU CACHE里的对应内容。
    */
    FlushInstructionCache(GetCurrentProcess(), NULL, 0);


	return 0;
}

