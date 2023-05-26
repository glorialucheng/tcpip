### 环境

1. OS: CentOS 7
2. 编译器 GCC 4.8 

### 基本知识点
#### struct sockaddr_in 结构体
1. 定义在netinet/in.h或arpa/inet.h头文件里面
2. 包含的属性：
     ```
     struct sockaddr_in {
          short sin_family;  // 2 字节，地址族，例如AF_INET表示TCP/IPV4，AF_INET6表示TCP/IPV6，AF_UNIX表示本机通信
          unsigned short sin_port;  // 2 字节，16位TCP/UDP端口号，例如htons(3490)
          struct in_addr sin_addr;  // 4 字节，32位IP地址
          char sin_zero[8];  // 8 字节，不使用
     };
     ```
3. 其中涉及到struct in_addr结构体:
     ```
     struct in_addr {
          unsigned long s_addr;  // 32位IPV4地址打印的时候可以调用inet_ntoa()函数将其转换为char *类型
     };
     ```
 4. 为什么不直接用unsigned long表示in_addr，因为直接传入数字的话，本机字节序（小一般为端序）与网络字节序（大端序）不同
 5. 因此常用函数htons和htonl分别将本机短整型、长整型字节序转化为网络字节序，记忆：host to net short和host to net long
 6. 两个函数原型为uint16_t htons(uint16_t hostshort);和uint32_t htonl(uint32_t hostlong);返回网络字节序的数字
 7. 类似的结构体struct sockaddr，定义在sys/socket.h头文件中，区别是他将端口号和地址定义在一个数组里，他和sockaddr_in长度相同因此可以相互转化
#### socket(AF_INET, SOCK_STREAM, 0)函数
1. 定义在sys/socket.h头文件里面，函数原型为int socket(int af, int type, int protocol);返回文件描述符
2. 参数af即地址族（同上述的sin_family），type表示套接字类型，取值SOCK_STREAM（流格式套接字/面向连接的套接字） 和 SOCK_DGRAM（数据报套接字/无连接的套接字）
3. 参数protocol表示传输协议类型，如IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP传输协议和UDP传输协议
4. 一般情况下根据前两个参数可以推算出第三个参数（即TCP或UDP），所以第三个参数通常为0
#### bzero和memset函数
1. 都定义在sting.h头文件里面，函数原型分别为 void bzero(void \*s, int n);和void \*memset(void \*s,int c,size_t n);
2. 两个函数都是对第一个参数指向内存进行操作，bzero为将s指向的内存前n个字节用0填充，而memset是将前n个字节用参数c填充
3. memset的第二个参数c的取值有限制，因此一般用bzero
#### bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))函数
1. 定义在sys/socket.h头文件中，函数原型为int bind(int sockfd, const struct sockaddr \*address, socklen_t address_len);
2. 一般用于服务端将创建的套接字绑定到本地，返回0或-1表示绑定成功或失败
3. 注意第二个参数是struct sockaddr类型的常量指针，但我们一般新建struct sockaddr_in类型的对象再转换为struct sockaddr类型的指针
4. 第三个参数表示地址参数的长度，一般取sizeof(address)
#### listen(sockfd, MAXLINK)函数
1. 定义在sys/socket.h头文件内，原型为int listen(int sockfd, int backlog)，返回0表示成功，-1表示失败
2. 
#### signal(SIGINT, stopServerRunning)函数
#### accept(sockfd, NULL, NULL)函数
#### recv(connfd, buff, BUFFSIZE - 1, 0)函数
#### send(connfd, buff, strlen(buff), 0)函数
#### close(connfd)函数
