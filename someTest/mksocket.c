#include<stdio.h>
#include<netinet/in.h>
#include<string.h>
#include <stdbool.h>
#include<stdlib.h>
#include<signal.h>
#include<sys/socket.h>
#include<assert.h>
#include<arpa/inet.h>
#include<libgen.h>
#include <errno.h>
#define BUF_SIZE 1024 //接受缓存区大小
static bool stop=false;
static void handle_term(int sig)
{
stop=true;
}
int main(int argc,char *argv[])
{
    signal(SIGTERM,handle_term);//软件请求正常终止信号 SIGTERM
    if(argc<=3)
    {
    printf("usage:%s ip_address port_number backlog\n",basename(argv[0]));//basename(argv[0]) 的代码，它的作用是从程序的命令行参数中获取程序的名称。argv[0] 通常包含程序被调用时的路径和名称。basename 函数会从这个字符串中去除任何目录路径，只返回程序的名称。
    return 1;
    }
    const char*ip=argv[1];
    int port=atoi(argv[2]);     //字符串转int
    int backlog=atoi(argv[3]);  //内核监听队列的最大长度，长度超过backlog将不再处理新的客户链接
    int sock=socket(PF_INET,SOCK_STREAM,0);//创建一个IPV4协议族、TCP协议的套接字，0表示默认协议
    assert(sock>=0);  //表达式为假的，触发assert
    /*创建一个IPv4 socket地址*/
    struct sockaddr_in address;
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET; //IPV4结构
    inet_pton(AF_INET,ip,&address.sin_addr);//将10进制ip转换为网络字节序的二进制
    address.sin_port=htons(port); //大端转小端
    int ret=bind(sock,(struct sockaddr*)&address,sizeof(address));//绑定socket
    assert(ret!=-1);
    ret=listen(sock,backlog);
    assert(ret!=-1);
    // /*循环等待连接，直到有SIGTERM信号将它中断*/
    // while(!stop)
    // {
    // sleep(1);
    // }
    // /*关闭socket，见后文*/
    // close(sock);
    skeep(20);
      struct sockaddr_in client;
      socklen_t client_addrlength=sizeof(client);
      int connfd=accept(sock,(struct sockaddr*)&client,&client_addrlength);
            //accept只是从监听队列中取出链接，不论链接处于何种状态
      //监听sock
      if(connfd<0)
        {
            printf("errno is :%d\n",errno);
        }
        else{
            /*接受连接成功则打印出客户端的IP地址和端口号*/
        char remote[INET_ADDRSTRLEN];  //ipV4地址 
        printf("connected with ip:%s and port:%d\n",inet_ntop(AF_INET,&client.sin_addr,remote,INET_ADDRSTRLEN),ntohs(client.sin_port));
        /*
            *数据读写
            *ssize_t recv(int sockfd,void*buf,size_t len,int flags);
            *ssize_t send(int sockfd,const void*buf,size_t len,int flags);
        */
        char buffer[BUF_SIZE];
        memset(buffer,'\0',BUF_SIZE);
        ret=recv(connfd,buffer,BUF_SIZE-1,0);
        printf("got%d bytes of normal data'%s'\n",ret,buffer);
        memset(buffer,'\0',BUF_SIZE);
        ret=recv(connfd,buffer,BUF_SIZE-1,MSG_OOB); //带外数据  紧急标志
        printf("got%d bytes of oob data'%s'\n",ret,buffer);
        memset(buffer,'\0',BUF_SIZE);
        ret=recv(connfd,buffer,BUF_SIZE-1,0);
        printf("got%d bytes of normal data'%s'\n",ret,buffer);  //后续将截断带外数据只剩正常数据
        close(connfd);
        
        close(connfd);

        }
        
    close(sock);
    return 0;
    

}