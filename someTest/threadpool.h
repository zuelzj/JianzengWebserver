#ifndef _THREAD_POOL
#define _THREAD_POOL
#include<locker.h>
#define MAX_WORK 66535

template<typename T>
class threadpool
{
public:     
            
            //添加任务
     void addWork(T* user);
            //获取任务
    T* getWork();
    static threadpool* create();
private:
    //线程工作内容
    static void worker(void *args);
    //构造函数
    threadpool();
    //析构函数
    ~threadpool();
//任务队列 list deque
    list<T*> workerQueue;
//队列锁
    locker mutex;
//信号量管理队列
    sem se;
//线程
    pthread_t *m_threads;
//最大任务量
    int max;
//线程数量
    int thread_number;
    //单例模式
    static threadpool* tp;
};

// threadpool<user>* tp=NULL;

template<typename T>
threadpool<T>::threadpool():max(MAX_WORK),thread_number(8)
{

        m_threads = new pthread_t[thread_number];
    for(int i=0;i<thread_number;++i)
    {
        pthread_create(m_threads+i,NULL,worker,this);
        pthread_detach(m_threads[i]); //分离态

    }

}


template<typename T>
void threadpool<T>::addWork(T* user)
{

    mutex.lock();
    if(list.size()>=max)
    {
         mutex.unlock();
        return ;
    }
    list.push_back(user);
    se.post();
    mutex.unlock();
}

template<typename T>
T* threadpool<T>::getWork()
{
    se.wait();
    mutex.lock();
    if(list.size()==0)
    {
        mutex.unlock();
        return ;
    }
    T* temp= list.front();
    list.pop_front();
    mutex.unlock();
    return temp;
}


template<typename T>
void threadpool<T>:: worker(void* m_tp)
{

   threadpool* pool= (threadpool*)m_tp;
   while(1)
   {
    pool->getWork()->process();
   }

}


template<typename T>
 threadpool<T>:: ~threadpool(){

    delete[] m_threads;
 }






#endif