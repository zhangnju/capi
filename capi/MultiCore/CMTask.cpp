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

/**	CMTask的构造函数

*/
CMTask::CMTask():m_lTaskCount(0), m_lExitFlag(0)
{
    m_ExitEvent = EventCreate();
    if ( m_ExitEvent == NULL )
    {
        return;
    }
}


/**	CMTask的析构函数

*/
CMTask::~CMTask()
{
    AtomicIncrement(&m_lExitFlag);
    if ( m_lTaskCount != 0 )
    {
        WaitEvent( m_ExitEvent );
    }
    /* 关闭操作的退出事件*/
    EventClose( m_ExitEvent );  
}


/**	CMTask获取退出标志函数

    @return	LONG——返回0表示不退出;返回大于0的整数表示退出
*/
LONG CMTask::GetExitFlag()
{
    return m_lExitFlag;
}


/**	CMTask的设置退出标志函数

    @return	void——无	
*/
void CMTask::SetExitFlag()
{
    AtomicIncrement(&m_lExitFlag);
}


/**	CMTask的进入操作任务函数，其作用是使计数变量加1

    @return	void——无	
*/
void CMTask::EnterTask()
{
    AtomicIncrement(&m_lTaskCount);
}


/**	CMTask的离开操作任务函数，作用是使计数变量减1，当计数变量为0时
    发送退出事件通知，在获取到退出事件后，就可以对线程使用的共享资源
    进行释放了。

    @return	void——无	
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
