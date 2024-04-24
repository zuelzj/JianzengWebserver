/**修改socket选项
 * #include＜sys/socket.h＞
 *int getsockopt(int sockfd,int level,intoption_name,void*option_value,socklen_t*restrict option_len);
 *int setsockopt(int sockfd,int level,int option_name,constvoid*option_value,socklen_t option_len);
 * 
 * 立即重用  SO_REUSEADDR选项   不论是否在TIME_WAIT状态 强制重用
 * 接收发送缓冲区 大小修改  SO_RCVBUF和SO_SNDBUF选项 有最小值限定
 * SO_RCVLOWAT和SO_SNDLOWAT选项分别表示TCP接收缓冲区和发送缓冲区的低水位标记(查过标记可读写)
 * SO_LINGER选项用于控制close系统调用在关闭TCP连接时的行为
*/