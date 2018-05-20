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
 *	CMTask.h
 *
 *	DESCRIPTION
 *		Interface to a Multiple threads exit object.
 *
 *	HISTORY
 *		04-26-2008	create by zhouweiming.
 *
 */
#ifndef __CMTASK_H__
#define __CMTASK_H__


class CMTask {
private:
    EVENT m_ExitEvent;	        // 退出事件
    LONG volatile m_lExitFlag;	// 退出标志
    LONG volatile m_lTaskCount;	// 操作的子任务数量

public:
    CMTask();
    virtual ~CMTask();

    LONG GetExitFlag();
    void SetExitFlag();

    void EnterTask();
    void LeaveTask();
};


#endif // __CMTASK_H__