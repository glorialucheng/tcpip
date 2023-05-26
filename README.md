### 环境

1. OS: CentOS 7
2. 编译器 GCC 4.8 

### 基本知识点
#### struct sockaddr_in 结构体
1. 定义在#include<netinet/in.h>或#include <arpa/inet.h>里面
2. 包含的属性：
     ```
     struct sockaddr_in {
          short            sin_family;  // 2 字节，地址族，例如AF_INET, AF_INET6
          unsigned short   sin_port;  // 2 字节，16位TCP/UDP端口号，例如htons(3490)
          struct in_addr   sin_addr;  // 4 字节，32位IP地址
          char             sin_zero[8];  // 8 字节，不使用
     };
     ```
3. 其中涉及到struct in_addr结构体:
     ```
     struct in_addr {
          unsigned long s_addr;  // 32位IPV4地址打印的时候可以调用inet_ntoa()函数将其转换为char *类型
     };
     ```
#### socket(AF_INET, SOCK_STREAM, 0)函数
#### bzero和memset函数
#### bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))函数
#### listen(sockfd, MAXLINK)函数
#### signal(SIGINT, stopServerRunning)函数
#### accept(sockfd, NULL, NULL)函数
#### recv(connfd, buff, BUFFSIZE - 1, 0)函数
#### send(connfd, buff, strlen(buff), 0)函数
#### close(connfd)函数
