#include "heap_timer.h"
#include "../http/http_conn.h"
    /*最小堆下滤操作，确保堆数组以hole结点为根的子树具有最小堆性质*/
 void time_heap::  percolate_down(int hole)
    {
        heap_timer *temp=array[hole];
        int child=0;
        for(;((hole*2+1))<=(cur_size-1);hole=child)
        {
            child=hole*2+1;
            if((child<(cur_size-1))&&array[child+1]->expire<array[child]->expire)
            {
                ++child; //选出左右孩子中最小的结点
            }
            if(array[child]->expire<temp->expire)
            {
                array[hole]=array[child];
            }
            else{
                break;
            }
            array[hole]=temp;
        }
    }

    /*数组扩容*/
void  time_heap::resize()
    {
        heap_timer** temp=new heap_timer*[2*capacity];
        if (!temp)
        {                           // 初始化失败
            throw std::exception(); // 寻找异常 ,一旦 throw 语句被执行，就会立即跳出当前的函数或代码块.
        }
        capacity=2*capacity;
        for (int i = 0; i <capacity; ++i)
        {
            temp[i] = nullptr;
        }
        for(int i=0;i<cur_size;++i)
        {temp[i]=array[i];}
        delete[] array;
        array=temp;
    }



    /*添加目标定时器timer*/
    void time_heap:: add_timer(heap_timer* timer)
    {
            if(!timer)
            {
                return;
            }
        if(cur_size>=capacity)//容量不够扩大一倍容量
        {
            resize();
        }
        /*新插入一个元素，当前堆大小加1,hole是新建空穴的位置*/
        int hole=cur_size++;
        int parent=0;
        for(;hole>0;hole=parent)
        {
            parent=(hole-1)/2;
            if(array[parent]->expire<=timer->expire)
            {
                break;
            }
            array[hole]=array[parent];
        }
        array[hole]=timer;
    }

    /*删除目标定时器timer*/
    void time_heap:: del_timer(heap_timer* timer)
    {
        if(!timer)
        {
        return ;
        }
        /*仅仅将目标定时器的回调函数设置为空，即所谓的延迟销毁。这将节省真正删除该定
时器造成的开销，但这样做容易使堆数组膨胀*/
        timer->cb_func=NULL;
    }
    void time_heap::adjust_timer(heap_timer *&timer)
    {
        heap_timer *temp=new heap_timer;
        temp->cb_func=timer->cb_func;
        temp->expire=timer->expire;
        temp->user_data=timer->user_data;
        del_timer(timer);
        timer=temp;
        add_timer(temp);
    }

    /*获得堆顶部的定时器*/
    heap_timer* time_heap:: top() const  //声明const后 该函数不能修改任何成员变量
    {
        if(empty())
        {
            return NULL;
        }
    return array[0];
    }

    /*删除堆顶部的定时器*/
    void time_heap:: pop_timer()
    {
        if(empty())
        {
            return;
        }
        if(array[0])
        {
            delete array[0];
        array[0]=array[--cur_size];
        percolate_down(0);//下滤操作
        }
    }

    /*心搏函数*/
    void time_heap:: tick()
    {
        heap_timer* tmp=array[0];
        time_t cur=time(NULL);//循环处理堆中到期的定时器
        while(!empty())
        {
            if(!tmp)
            {
                break;
            }
        /*如果堆顶定时器没到期，则退出循环 */
        if(tmp->expire>cur)
        {
            break;
        }
        /*执行定时任务*/
        if(array[0]->cb_func)
        {
            array[0]->cb_func(array[0]->user_data);
        }
        /*删除堆顶元素*/
        pop_timer();
        tmp=array[0];
        }
    }


    /*初始化大小为cap的空堆*/
   time_heap:: time_heap() : capacity(50), cur_size(0)
    {
        array = new heap_timer *[capacity]; // 只分配了指针数组的空间，每个数组中heap_timer的空间未分配
        if (!array)
        {                           // 初始化失败
            throw std::exception(); // 寻找异常 ,一旦 throw 语句被执行，就会立即跳出当前的函数或代码块.
        }
        for (int i = 0; i < capacity; ++i)
        {
            array[i] = NULL;
        }

    }
    /*构造函数二，用已有数据来初始化堆*/
  time_heap::time_heap(heap_timer **init_array, int size, int cap): capacity(cap), cur_size(size)
    {
        if (capacity < size)
        {
            throw std::exception();
        }
        array = new heap_timer *[capacity];
        if (!array)
        {                           // 初始化失败
            throw std::exception(); // 寻找异常 ,一旦 throw 语句被执行，就会立即跳出当前的函数或代码块.
        }
        for (int i = 0; i < capacity; ++i)
        {
            array[i] = NULL; // 初始化为空
        }
        if (size != 0)
        {
            /*初始化堆数组*/
            for (int i = 0; i < size; ++i)
            {
                array[i] = init_array[i];
            }
            for (int i = (cur_size - 1) / 2; i > 0; --i)
            { /*对数组中的第[(cur_size-1)/2]～0个元素执行下虑操作*/
                percolate_down(i);
            }
        }
    }
    /*销毁时间堆*/
time_heap:: ~time_heap()
    {
        for (int i = 0; i < cur_size; ++i)
        {
            delete array[i];  //释放每一个定时器
        }
        delete[] array;  //释放定时器数组
    }



void Utils::init(int timeslot)
{
    m_TIMESLOT=timeslot;
}

//对文件描述符设置非阻塞
int Utils::setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

//将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
void Utils::addfd(int epollfd, int fd, bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;

#ifdef ET
        event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
#endif

#ifdef LT
        event.events = EPOLLIN | EPOLLRDHUP;
#endif

    if (one_shot)
        event.events |= EPOLLONESHOT;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}

//信号处理函数
void Utils::sig_handler(int sig)
{
    //为保证函数的可重入性，保留原来的errno
    int save_errno = errno;
    int msg = sig;
    send(u_pipefd[1], (char *)&msg, 1, 0);
    errno = save_errno;
}

//设置信号函数
void Utils::addsig(int sig, void(handler)(int), bool restart)
{
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart)
        sa.sa_flags |= SA_RESTART;//SA_RESTART 标志指示系统在某些阻塞的系统调用被信号中断后应自动重启这些调用
    sigfillset(&sa.sa_mask); //信号处理期间屏蔽其他所有信号
    assert(sigaction(sig, &sa, NULL) != -1);
}

//定时处理任务，重新定时以不断触发SIGALRM信号
void Utils::timer_handler()
{
    m_timer_heap.tick();
    alarm(m_TIMESLOT);
}

void Utils::show_error(int connfd, const char *info)
{
    send(connfd, info, strlen(info), 0);
    close(connfd);
}

int *Utils::u_pipefd = 0;
int Utils::u_epollfd = 0;

class Utils;

void cb_func(client_data *user_data)
{
    epoll_ctl(Utils::u_epollfd, EPOLL_CTL_DEL, user_data->sockfd, 0);
    assert(user_data);
    close(user_data->sockfd);
    http_conn::m_user_count--;
}
