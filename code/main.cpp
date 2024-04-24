#define ET
#include "webserver/webserver.h"


const int port=9006;
const int log_write=0;//日志写入方式,默认同步
const int opt_linger=0;//优雅关闭链接
const int sql_num=8;//数据库连接池内部数量
const int thread_num=1;//线程池内部线程数量
const int close_log=1;//关闭日志
const int actor_model=0; //并发模型，默认paoactor
int main(int argc, char *argv[])
{
  string user = "root";
  string passwd = "123456";
  string databasename = "web";
  WebServer server;
  printf("基础数据初始化开始\n");
  //一些基础变量初始化
  server.init(port,user,passwd,databasename, log_write, 
                     opt_linger, sql_num, thread_num, close_log, actor_model);
printf("基础数据初始化完成\n");
//数据库初始化
server.sql_pool();
printf("数据库初始化完成\n");
// 线程池初始化
server.thread_pool();
printf("线程池初始化完成\n");
// 日志初始化
server.log_write();
printf("日志初始化完成\n");
// 监听端口
server.eventListen();
printf("开始运行\n");
//运行
server.eventLoop();
return 0;

}

