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
 *	SingleList.h
 *
 *	DESCRIPTION
 *		��������ģ��.
 *
 *	HISTORY
 *		10-03-98	create by zhouweiming.
 *
 */

#ifndef __SINGLELIST_H__
#define __SINGLELIST_H__

#ifdef __cplusplus
extern "C" {
#endif


/* ��������Ľṹ�� */
typedef struct SINGLELIST_st {
	SINGLENODE	    *pHead;        /* ��1���ڵ��ָ�� */
	SINGLENODE	    *pTail;		   /* ���1���ڵ��ָ�� */
    SINGLENODE      *pCur;         /* ��ǰ�ڵ��ָ�� */
	UINT             uCount;       /* ��������ڵ�ĸ��� */
} SINGLELIST, *PSINGLELIST;


/* �����ö�����ݽṹ */
typedef struct SINGLELIST_ENUM_st {
    SINGLENODE  *pCur;         /* ��ǰö�ٵĽڵ�ָ�� */
    COMPAREFUNC  CompareFunc;  /* �ڵ����ݱȽϺ��� */
    void        *pMatchData;   /* Ҫö�ٵ�ƥ������ */
} SINGLELIST_ENUM;


/**	��������Ĵ���������������������ǿյ�û�нڵ�������

	@param	void - ��	
	@return	SINGLELIST * - ʧ�ܷ���NULL, �ɹ�ʱ����һ����������ṹ��ָ��	
*/
SINGLELIST * SingleList_Create( void );

/**	����������ͷź���

	@param	SINGLELIST *pSingleList - Ҫ�ͷŵĵ��������ָ��	
	@param	DESTROYFUNC pDestroyFunc - ����ڵ������ͷŻص�����	
	@return	void - ��	
*/
void SingleList_Destroy( SINGLELIST *pSingleList, DESTROYFUNC DestroyFunc );

/**	�����������ӽڵ㺯������ӵĽڵ���ڵ��������ͷ��

	@param	SINGLELIST *pSingleList - Ҫ��ӵĵ�������ָ��	
	@param	void *pData - Ҫ��ӵĽڵ������ָ��	
	@return	INT - ʧ�ܷ���0���ɹ�����1	
*/
INT	SingleList_InsertHead( SINGLELIST *pSingleList, void *pData );

/**	�����������ӽڵ㺯������ӵĽڵ���ڵ��������β��

	@param	SINGLELIST *pSingleList - Ҫ��ӵĵ�������ָ��	
	@param	void *pData - Ҫ��ӵĽڵ������ָ��
	@return	INT - ʧ�ܷ���0���ɹ�����1	
*/
INT	SingleList_InsertTail( SINGLELIST *pSingleList, void *pData );

/**	��������ĵ���ͷ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	void * - ʧ�ܷ���NULL, �ɹ�����Ҫ������ͷ�ڵ������ָ��	
*/
void *	SingleList_PopHead( SINGLELIST *pSingleList );

/**	��������ĵ���β�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	void * -  ʧ�ܷ���NULL, �ɹ�����Ҫ������β�ڵ������ָ��	
*/
void *	SingleList_PopTail( SINGLELIST *pSingleList );

/**	�����ɾ���ڵ㺯��������ɾ����pMatchData��������ͬ���ݵĽڵ�
    ������������ͬ���ݵĽڵ�����ֻɾ����һ������ͬ���ݵĽڵ�

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@param	void *pMatchData - Ҫɾ���ڵ��ƥ������	
	@param	COMPAREFUNC CompareFunc - ���ݱȽϺ��������Ƚ�pMatchData����������
                                      �ڵ�����Ƿ����	
	@param	DESTROYFUNC DestroyFunc - ����ڵ�������ͷź���	
	@return	INT (by default) - 0��ʾʧ�ܻ�������û��ƥ������ݣ�1��ʾ�ɹ�ɾ��	
*/
INT		SingleList_Delete( SINGLELIST *pSingleList,  
                        void *pMatchData, 
                        COMPAREFUNC CompareFunc, 
                        DESTROYFUNC DestroyFunc );

/**	��������Ļ�ȡָ��λ�����ݵĺ���

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@param	UINT uIndex - Ҫ��ȡ������λ��	
	@return	void * - ����λ�ýڵ������ָ��	
*/
void *  SingleList_GetAt( SINGLELIST *pSingleList, UINT uIndex );

/**	��������Ļ�ȡ����ڵ���������

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	UINT - ����ڵ�������Ϊ0��ʾ�����ǿյ�û�нڵ�����������	
*/
UINT    SingleList_GetCount(SINGLELIST *pSingleList);

/**	��������Ļ�ȡͷ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	void * - ͷ�ڵ������ָ��	
*/
void *	SingleList_GetHead( SINGLELIST *pSingleList );

/**	��������Ļ�ȡ��ǰ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ�� 	
	@return	void * - ��ǰ�ڵ������ָ��	
*/
void *	SingleList_GetCursor( SINGLELIST *pSingleList );

/**	��������Ļ�ȡβ�ڵ㺯��

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@return	void * - β�ڵ������ָ��	
*/
void * 	SingleList_GetTail( SINGLELIST *pSingleList );


/**	���������ö�ٺ���

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��
	@return	void  - ��
*/
void SingleList_EnumBegin( SINGLELIST *pSingleList );

/**	��������ö����һ���ڵ�ĺ�����
    ��һ�ε��ô˺���ǰ�����ȵ���SingleList_EnumBegin()����

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ��������ָ��	
	@return	void * - ö�ٵ��Ľڵ�����ָ��	
*/
void *  SingleList_EnumNext( SINGLELIST *pSingleList );


/**	��������ı�������

	@param	SINGLELIST *pSingleList - Ҫ������������ָ��	
	@param	TRAVERSEFUNC TraverseFunc - �ڵ����ݵı�����������	
	@return	INT - �ɹ�����1��ʧ�ܷ���0	
*/
INT SingleList_Traverse( SINGLELIST *pSingleList, TRAVERSEFUNC TraverseFunc );

/**	��������Ĳ���������
    �����ǰ��մ�С����������У�����˵�Ĵ�С����CompareFunc��������
    ����û�����ͨ��CompareFunc�ķ���ֵ����������ʹ��˳���������������

	@param	SINGLELIST *pSingleList - Ҫ�����ĵ�������ָ��	
	@param	COMPAREFUNC CompareFunc - �ڵ����ݱȽϺ���	
	@return	INT - �ɹ�����1��ʧ�ܷ���0	
*/
INT SingleList_InsertSort( SINGLELIST *pSingleList, COMPAREFUNC CompareFunc );

SINGLELIST * SingleList_Split(SINGLELIST *pSingleList, UINT uCount);

INT SingleList_Merge( SINGLELIST *pSingleListA, 
                      SINGLELIST *pSingleListB, 
                      COMPAREFUNC CompareFunc );

INT SingleList_MergeSort( SINGLELIST *pSingleList, 
                          COMPAREFUNC CompareFunc, 
                          UINT uInsertSortCount );

INT SingleList_RadixSort( SINGLELIST *pSingleList,
                          UINT uRadix,
                          UINT uMaxKeyLen,
                          GETKEYFUNC GetKeyFunc );



#ifdef __cplusplus
}
#endif


#endif /* __SINGLELIST_H__ */
