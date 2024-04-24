#ifndef MIN_HEAP_TIMER
#define MIN_HEAP_TIMER

#include <iostream>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/uio.h>
#define BUFFER_SIZE 64
using std::exception;

class heap_timer;

/*绑定socket和定时器*/
struct client_data
{
    sockaddr_in address;
    int sockfd;
    char buf[BUFFER_SIZE]; // 备注
    heap_timer *timer;
};

/*定时器类*/
class heap_timer
{
public:
   heap_timer()
    {
    }
    heap_timer(int delay)
    {
        expire = time(NULL) + delay;
    }

public:
    time_t expire;                  // 定时器生效绝对时间
    void (*cb_func)(client_data *); // 定时器回调函数
    client_data *user_data;         // 用户数据
};

/*时间堆类*/
class time_heap
{
private:
    /* data */
    int capacity;       /*堆容量*/
    int cur_size;       /*堆数组当前包含元素的个数*/
    heap_timer **array; /*堆数组，每个array指向一个heap_timer *指针(数组)*/

    /*最小堆下滤操作，确保堆数组以hole结点为根的子树具有最小堆性质*/
    void percolate_down(int hole);

    /*数组扩容*/
    void resize();

public:
    /*添加目标定时器timer*/
    void add_timer(heap_timer *timer);

    /*删除目标定时器timer*/
    void del_timer(heap_timer *timer);

    /*调整目标定时器adjust_timer*/
    void adjust_timer(heap_timer *&timer);

    /*获得堆顶部的定时器*/
    heap_timer *top() const;// 声明const后 该函数不能修改任何成员变量

    /*删除堆顶部的定时器*/
    void pop_timer();
    /*心搏函数*/
    void tick();
    bool empty() const { return cur_size == 0; }

    /*初始化大小为cap的空堆*/
    time_heap();

    /*构造函数二，用已有数据来初始化堆*/
    time_heap(heap_timer **init_array, int size, int cap);

    /*销毁时间堆*/
    ~time_heap();
};

class Utils
{
public:
    Utils() {}
    ~Utils() {}

    void init(int timeslot);

    // 对文件描述符设置非阻塞
    int setnonblocking(int fd);

    // 将内核事件表注册读事件，ET模式，选择开启EPOLLONESHOT
    void addfd(int epollfd, int fd, bool one_shot);

    // 信号处理函数
    static void sig_handler(int sig);

    // 设置信号函数
    void addsig(int sig, void(handler)(int), bool restart = true);

    // 定时处理任务，重新定时以不断触发SIGALRM信号
    void timer_handler();

    void show_error(int connfd, const char *info);

public:
    static int *u_pipefd;
    time_heap m_timer_heap;
    static int u_epollfd;
    int m_TIMESLOT;
};

#endif