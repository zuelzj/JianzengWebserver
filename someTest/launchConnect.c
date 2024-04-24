// #include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>


void lanuchConnect(char *argv[])
{

    int sock=socket(PF_INET,SOCK_STREAM,0);//创建一个IPV4协议族、TCP协议的套接字，0表示默认协议
    const char*ip=argv[1];        //要连接的服务器目的ip
    int port=atoi(argv[2]);     //字符串转int  目的端口
    struct sockaddr_in address; //服务器地址结构体
    bzero(&address,sizeof(address));
    address.sin_family=AF_INET; //IPV4结构
    inet_pton(AF_INET,ip,&address.sin_addr);//将10进制ip转换为网络字节序的二进制
    address.sin_port=htons(port); //大端转小端

    int connectfd=connect(sock,(struct sockaddr*)&address,sizeof(address));
        /*
            *数据读写
            *
            * ssize_t recv(int sockfd,void*buf,size_t len,int flags);//flag参数提供额外的控制
            * ssize_t send(int sockfd,const void*buf,size_t len,int flags);
        */
        const char*oob_data="abc";
        const char*normal_data="123";
        send(sock,normal_data,strlen(normal_data),0);
        send(sock,oob_data,strlen(oob_data),MSG_OOB);       //紧急数据
        send(sock,normal_data,strlen(normal_data),0);






    close(sock);//将fd引用计数减一，为0时才真正关闭
    shutdown(sock,SHUT_RDWR);//立刻终止链接
}
