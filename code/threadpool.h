#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>
#include "locker.h"
#include "./mysql/sql_connection_pool.h"
/*线程池类，将它定义为模板类是为了代码复用。模板参数T是任务类*/
template <typename T>
class threadpool
{
public:
    /*参数thread_number是线程池中线程的数量，max_requests是请求队列中最多允
    许的、等待处理的请求的数量*/
    threadpool(int actor_model,connection_pool* connPool, int thread_number = 8, int max_requests = 10000);
    ~threadpool();
    /*往请求队列中添加任务*/
    bool append(T *request,int state);
    bool append_p(T *request);
private:
    /*工作线程运行的函数，它不断从工作队列中取出任务并执行之*/
    static void *worker(void *arg); //静态函数作为pthread_create的参数，因为成员函数会多传递一个this作为参数，不符合参数列表
    void run();

private:
    int m_thread_number;        /*线程池中的线程数*/
    int m_max_requests;         /*请求队列中允许的最大请求数*/
    pthread_t *m_threads;       //*描述线程池的数组，其大小为m_thread_number*/
    std::list<T *> m_workqueue; /*请求队列*/
    locker m_queuelocker;       /*保护请求队列的互斥锁*/
    sem m_queuestat;            /*是否有任务需要处理*/
    bool m_stop;                /*是否结束线程*/
    connection_pool *m_connPool;//数据库
    int m_actor_model;          //模型切换
};
template <typename T>
threadpool<T>::threadpool( int actor_model, connection_pool *connPool, int thread_number, int max_requests) : m_actor_model(actor_model),m_thread_number(thread_number), m_max_requests(max_requests), m_threads(NULL),m_connPool(connPool)
{
    if ((thread_number <= 0) || (max_requests <= 0))
    {
        throw std::exception();
    }
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
    {
        throw std::exception();
    }
    /*创建thread_number个线程,并将它们都设置为脱离线程*/
    for (int i = 0; i < thread_number; ++i)
    {
        printf("create the%dth thread\n", i);
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i])) // 设置为分离状态
        {//将线程进行分离后，不用单独对工作线程进行回收
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
threadpool<T>::~threadpool()
{
    delete[] m_threads;

}
template <typename T>
bool threadpool<T>::append(T *request,int state)
{
    /*操作工作队列时要加锁，因为它被所有线程共享*/
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    request->m_state=state;
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post(); // 释放有工作信号
    return true;
}
template <typename T>
bool threadpool<T>::append_p(T *request)
{
    /*操作工作队列时要加锁，因为它被所有线程共享*/
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post(); // 释放有工作信号
    return true;
}
template <typename T>
void *threadpool<T>::worker(void *arg)
{
    threadpool *pool = (threadpool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void threadpool<T>::run()
{
    while (1)
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if (!request)
        {
            continue;
        }

        if(1==m_actor_model)
        {
            if(0==request->m_state)
            {
                if(request->read())     
                {
                    request->improv=1;
                    connectionRAII mysqlcon(&request->mysql, m_connPool);
                    request->process();
                }
                 else
                {
                    request->improv = 1;
                    request->timer_flag = 1;
                }
            }
            else{
             if (request->write())
                {
                    request->improv = 1;
                }
                else
                {
                    request->improv = 1;
                    request->timer_flag = 1;
                }
                }
        }
        else{
                  connectionRAII mysqlcon(&request->mysql, m_connPool);
                request->process();
        }
       
    }
}

#endif