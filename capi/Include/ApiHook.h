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

/**	ApiHook ģ���ʼ������

	@param	INT nMaxHookFuncCounts - �������õĹ�������	
	@return	INT (by default) - NULL ��ʾʧ��
*/
HANDLE	ApiHook_Init(UINT nMaxHookFuncCounts);

/**	ͨ����ַ������ĳ�������Ĺ��Ӻ���

	@param	HANDLE hApiHook - ��ApiHook_Init()�������ɵľ��	
	@param	DWORD dwSrcFuncAddr - Դ������ַ	
	@param	DWORD dwNewFuncAddr - ���Ӻ�����ַ	
	@return	INT (by default) - -1��ʾʧ�ܣ�>=0��ʾ��hook�����е����	
*/
INT	ApiHook_SetByAddr(HANDLE hApiHook, DWORD dwSrcFuncAddr, DWORD dwNewFuncAddr);
INT	ApiHook_SetByName(HANDLE hApiHook, char *pszDllName, char *pszFuncName, DWORD dwNewFuncAddr);

/**	ȡ��ĳ�������Ĺ��Ӻ�������

	@param	HANDLE hApiHook - ��ApiHook_Init()�������ɵľ��	
	@param	DWORD dwSrcFuncAddr - Դ������ַ	
	@return	INT (by default) - -1��ʾʧ�ܣ�>=0��ʾ��hook�����е����	
*/
INT	ApiHook_UnSetByAddr(HANDLE hApiHook, DWORD dwSrcFuncAddr );
INT	ApiHook_UnSetByName(HANDLE hApiHook, char *pszDllName, char *pszFuncName);

/**	��ȡ�������õĹ�������

	@param	HANDLE hApiHook - ��ApiHook_Init()�������ɵľ��	
	@return	INT - �������õĹ�������	
*/
UINT     ApiHook_GetMaxHooks(HANDLE hApiHook);

/**	ApiHookģ��رպ���

	@param	HANDLE - ��ApiHook_Init()�������ɵľ��		
	@return	INT (by default) - ʧ�ܷ���-1, �ɹ�����0��	
*/
INT	ApiHook_Close(HANDLE);

#ifdef __cplusplus
}
#endif

#endif //__APIHOOK_H__
