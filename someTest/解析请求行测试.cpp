#include <strings.h>
#include <string.h>
#include <iostream>
using namespace std;

char m_method;
char *m_version;
char *m_url;

void parse_request_line(char *text)
{ // 示例：   GET /index.html HTTP/1.1
    m_url = strpbrk(text, " \t");
    /*如果请求行中没有空白字符或“\t”字符，则HTTP请求必有问题*/
    if (!m_url)
    {
        return ;
    }
    *m_url++ = '\0';
    cout<<"url"<<m_url<<endl;
    char *method = text;
    if (strcasecmp(method, "GET") == 0)
    {
        m_method = 'G';
    }
    else if (strcasecmp(method, "POST") == 0)
    {
        m_method = 'P';
    }
    else
    {
        return;
    }

    m_url += strspn(m_url, " \t"); // 除去其余连续开头tab和空格
       cout<<"url"<<m_url<<endl;
    m_version = strpbrk(m_url, " \t");
       cout<<"version"<<m_version<<endl;
    if (!m_version)
    {
        return;
    }
    *m_version++ = '\0';
    m_version += strspn(m_version, " \t");
     cout<<"version"<<m_version<<endl;
    if (strcasecmp(m_version, "HTTP/1.1") != 0)
    {
        return;
    }
    
    if (strncasecmp(m_url, "http://", 7) == 0)
    {
        m_url += 7;
        m_url = strchr(m_url, '/'); // 返回第一个/所指位置
    }
    if (strncasecmp(m_url, "https://", 8) == 0)
    {
        m_url += 8;
        m_url = strchr(m_url, '/');
    }

    if (!m_url || m_url[0] != '/')
    {
        return;
    }
    // 当url为/时，显示判断界面
    if (strlen(m_url) == 1)
    {
        strcat(m_url, "judge.html");
    }
    return ;
}

int main()
{
    char text[]="GET / HTTP/1.1  ";
    parse_request_line(text);
}