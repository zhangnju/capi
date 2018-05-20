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
 *	ApiHook.h
 *
 *	DESCRIPTION
 *		Interface to a hook type.
 *
 *	HISTORY
 *		10-25-99	create by zhouweiming.
 *
 */

#ifndef __APIHOOK_H__
#define __APIHOOK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**	ApiHook 模块初始化函数

	@param	INT nMaxHookFuncCounts - 最大可设置的钩子数量	
	@return	INT (by default) - NULL 表示失败
*/
HANDLE	ApiHook_Init(UINT nMaxHookFuncCounts);

/**	通过地址来设置某个函数的钩子函数

	@param	HANDLE hApiHook - 由ApiHook_Init()函数生成的句柄	
	@param	DWORD dwSrcFuncAddr - 源函数地址	
	@param	DWORD dwNewFuncAddr - 钩子函数地址	
	@return	INT (by default) - -1表示失败，>=0表示在hook数组中的序号	
*/
INT	ApiHook_SetByAddr(HANDLE hApiHook, DWORD dwSrcFuncAddr, DWORD dwNewFuncAddr);
INT	ApiHook_SetByName(HANDLE hApiHook, char *pszDllName, char *pszFuncName, DWORD dwNewFuncAddr);

/**	取消某个函数的钩子函数设置

	@param	HANDLE hApiHook - 由ApiHook_Init()函数生成的句柄	
	@param	DWORD dwSrcFuncAddr - 源函数地址	
	@return	INT (by default) - -1表示失败，>=0表示在hook数组中的序号	
*/
INT	ApiHook_UnSetByAddr(HANDLE hApiHook, DWORD dwSrcFuncAddr );
INT	ApiHook_UnSetByName(HANDLE hApiHook, char *pszDllName, char *pszFuncName);

/**	获取最大可设置的钩子数量

	@param	HANDLE hApiHook - 由ApiHook_Init()函数生成的句柄	
	@return	INT - 最大可设置的钩子数量	
*/
UINT     ApiHook_GetMaxHooks(HANDLE hApiHook);

/**	ApiHook模块关闭函数

	@param	HANDLE - 由ApiHook_Init()函数生成的句柄		
	@return	INT (by default) - 失败返回-1, 成功返回0。	
*/
INT	ApiHook_Close(HANDLE);

#ifdef __cplusplus
}
#endif

#endif //__APIHOOK_H__
