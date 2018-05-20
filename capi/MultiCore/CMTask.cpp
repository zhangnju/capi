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
#include "CapiGlobal.h"
#include "CMTask.h"

/**	CMTask�Ĺ��캯��

*/
CMTask::CMTask():m_lTaskCount(0), m_lExitFlag(0)
{
    m_ExitEvent = EventCreate();
    if ( m_ExitEvent == NULL )
    {
        return;
    }
}


/**	CMTask����������

*/
CMTask::~CMTask()
{
    AtomicIncrement(&m_lExitFlag);
    if ( m_lTaskCount != 0 )
    {
        WaitEvent( m_ExitEvent );
    }
    /* �رղ������˳��¼�*/
    EventClose( m_ExitEvent );  
}


/**	CMTask��ȡ�˳���־����

    @return	LONG��������0��ʾ���˳�;���ش���0��������ʾ�˳�
*/
LONG CMTask::GetExitFlag()
{
    return m_lExitFlag;
}


/**	CMTask�������˳���־����

    @return	void������	
*/
void CMTask::SetExitFlag()
{
    AtomicIncrement(&m_lExitFlag);
}


/**	CMTask�Ľ����������������������ʹ����������1

    @return	void������	
*/
void CMTask::EnterTask()
{
    AtomicIncrement(&m_lTaskCount);
}


/**	CMTask���뿪������������������ʹ����������1������������Ϊ0ʱ
    �����˳��¼�֪ͨ���ڻ�ȡ���˳��¼��󣬾Ϳ��Զ��߳�ʹ�õĹ�����Դ
    �����ͷ��ˡ�

    @return	void������	
*/
void CMTask::LeaveTask()
{
    LONG ret;
    ret = AtomicDecrement(&m_lTaskCount);
    if ( ret == 0 )
    {
        SendEvent( m_ExitEvent );
    }
}
