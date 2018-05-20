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
 *	GCPtr.h
 *
 *	DESCRIPTION
 *		Interface to a generic garbage collection type.
 *
 *	HISTORY
 *		07-18-2003	create by zhouweiming.
 *
 */

#define INT_LEN         4
#define DOUBLE_INT_LEN  8

INT GC_Init(INT nBucketCount);
void *GC_Malloc(size_t size);
void GC_Free(void *p);
void GC_Collect();

INT MGC_Init(INT nBucketCount);
void *MGC_Malloc(size_t size);
void MGC_Free(void *p);
void MGC_ManualFree(void *p);
void MGC_Collect();
void MGC_CollectTask();
void MGC_Shutdown();

extern CFastLock        g_lock;  //用来保护引用计数读写及哈希表操作的锁


// 支持垃圾内存回收的类GCPtr，使用模板实现
template <class T> class GCPtr {
public:
    T *m_pAddr;     //用来记住定义的指针地址便于析构函数使用
    
public:
    GCPtr(T *t = NULL) 
    {
        m_pAddr = t;
        if ( m_pAddr != NULL )
        {
            INT *p = (INT *)m_pAddr - 1;
            *p += 1;
        }
    };

    ~GCPtr() 
    {
        if ( m_pAddr != NULL )
        {
            INT *p = (INT *)m_pAddr - 1;
            *p -= 1;
        }
    };

    // 拷贝构造函数
    GCPtr(const GCPtr &gcPtr)
    {
        INT *p = (INT *)gcPtr.m_pAddr - 1;
        *p += 1;

        m_pAddr = gcPtr.m_pAddr;
    }

    T & operator* () { return *m_pAddr; };
    T * operator-> () { return m_pAddr; };
    operator T *() { return m_pAddr; };
    
    T *operator=(T *t) 
    {
        // 将原来指向的内存引用计数减1
        INT *p = (INT *)m_pAddr - 1;
        *p -= 1;
        
        m_pAddr = t; 

        // 将新指向的内存的引用计数加1
        p = (INT *)t - 1;
        *p += 1;

        return t;
    };

    GCPtr & operator=(GCPtr &r) 
    { 
        // 将原来指向的内存引用计数减1
        INT *p = (INT *)m_pAddr - 1;
        *p -= 1;

        m_pAddr = r.m_pAddr; 
        
        // 将新指向的内存的引用计数加1
        p = (INT *)r.m_pAddr - 1;
        *p += 1;

        return r;
    };
    
};



// 支持垃圾内存回收的类GCPtr，使用模板实现
template <class T> class MGCPtr {
public:
    T *m_pAddr;     //用来记住定义的指针地址便于析构函数使用
    
public:
    MGCPtr(T *t = NULL) 
    {
        m_pAddr = t;
        INT *p = (INT *)((char *)m_pAddr - INT_LEN);

        CScopedLock<CFastLock> slock(g_lock);
        *p += 1;
    };
    
    ~MGCPtr() 
    {
        INT *p = (INT *)((char *)m_pAddr - INT_LEN);

        CScopedLock<CFastLock> slock(g_lock);
        *p -= 1;
    };
    
    // 拷贝构造函数
    MGCPtr(const MGCPtr &gcPtr)
    {
        INT *p = (INT *)((char *)gcPtr.m_pAddr - INT_LEN);
        
        {
            CScopedLock<CFastLock> slock(g_lock);
            *p += 1;
        }
        
        m_pAddr = gcPtr.m_pAddr;
    }
    
    T & operator* () { return *m_pAddr; };
    T * operator-> () { return m_pAddr; };
    operator T *() { return m_pAddr; };
    
    T *operator=(T *t) 
    {
        // 将原来指向的内存引用计数减1
        INT *p = (INT *)((char *)m_pAddr - INT_LEN);
        
        {
            CScopedLock<CFastLock> slock(g_lock);

            *p -= 1;            
            m_pAddr = t; 
            
            // 将新指向的内存的引用计数加1
            p = (INT *)((char *)t - INT_LEN);
            *p += 1;
        }
     
        return t;
    };
    
    MGCPtr & operator=(MGCPtr &r) 
    { 
        // 将原来指向的内存引用计数减1
        INT *p = (INT *)((char *)m_pAddr - INT_LEN);
        
        {
            CScopedLock<CFastLock> slock(g_lock);
            *p -= 1;
            
            m_pAddr = t; 
            
            // 将新指向的内存的引用计数加1
            p = (INT *)((char *)r.m_pAddr - INT_LEN);
            
            *p += 1;
        }
          
        return r;
    };
    
};
