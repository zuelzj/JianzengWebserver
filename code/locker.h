/*封装信号量、条件变量、互斥量 线程间相互通信*/
/*RAII  资源获取即初始化，自动调用构造函数与析构函数实现资源和对象的声明周期绑定
 */
#ifndef LOCKER_H
#define LOCKER_H
#include <exception>
#include <pthread.h>
#include <semaphore.h>
/*封装信号量的类*/
class sem
{
public:
    /*创建初始化信号量*/
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }
    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }
    /*销毁信号量*/
    ~sem()
    {
        sem_destroy(&m_sem);
    }
    /*等待信号量*/
    bool wait()
    { // 原子方式操作，信号量减一为0时阻塞
        return sem_wait(&m_sem) == 0;
    }
    /*增加信号量*/
    bool post()
    { // 原子方式操作，将信号量加一
        return sem_post(&m_sem) == 0;
    }

private:
    sem_t m_sem;
};

/*封装互斥锁的类*/
/*
互斥锁,也成互斥量,可以保护关键代码段,以确保独占式访问.
当进入关键代码段,获得互斥锁将其加锁;离开关键代码段,唤醒等待该互斥锁的线程
*/
class locker
{
public:
    /*创建并初始化互斥锁*/
    locker()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    /*销毁互斥锁*/
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    /*获取互斥锁*/
    bool lock()
    { // 原子方式给互斥锁加锁
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    /*释放互斥锁*/
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};

/*封装条件变量的类*/
/*条件变量提供了一种线程间的通知机制,当某个共享数据达到某个值时,
唤醒等待这个共享数据的线程*/
class cond
{
public:
    /*创建并初始化条件变量*/
    cond()
    { /*条件变量需要用锁来控制*/
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
        if (pthread_cond_init(&m_cond, NULL) != 0)
        { /*构造函数中一旦出现问题，就应该立即释放已经成功分配了的资源*/

            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }
    /*销毁条件变量*/
    ~cond()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }
    /*等待条件变量*/
    bool wait()
    {
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, &m_mutex); // 内部有一次解锁加锁操作
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

     bool timewait(struct timespec t)
    {
        int ret = 0;
        pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, &m_mutex, &t);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

      bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t);
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }
    /*唤醒等待条件变量的线程*/
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
        bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;
    }


private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif