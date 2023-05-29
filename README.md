### 环境

1. OS: CentOS 7
2. 编译器 GCC 4.8 

### Socket基本知识
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
3. 第一个参数的取值为PF_INET或者AF_INET，两者有细微差别
4. 参数protocol表示传输协议类型，如IPPROTO_TCP和IPPTOTO_UDP，分别表示TCP传输协议和UDP传输协议
5. 一般情况下根据前两个参数可以推算出第三个参数（即TCP或UDP），所以第三个参数通常为0
#### bzero和memset函数
1. 都定义在sting.h头文件里面，函数原型分别为 void bzero(void \*s, int n);和void \*memset(void \*s,int c,size_t n);
2. 两个函数都是对第一个参数指向内存进行操作，bzero为将s指向的内存前n个字节用0填充，而memset是将前n个字节用参数c填充
3. memset的第二个参数c的取值有限制，因此一般用bzero
#### bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr))函数
1. 定义在sys/socket.h头文件中，函数原型为int bind(int sockfd, const struct sockaddr \*address, socklen_t address_len);
2. 一般用于服务端将创建的套接字绑定到本地，返回0或-1表示绑定成功或失败
3. 注意第二个参数是struct sockaddr类型的常量指针，但我们一般新建struct sockaddr_in类型的对象再转换为struct sockaddr类型的指针
4. 第三个参数表示地址参数的长度，一般取sizeof(address)
#### listen(sockfd, 10)函数
1. 定义在sys/socket.h头文件内，原型为int listen(int sockfd, int backlog)，返回0表示成功，-1表示失败
2. 可以理解成将第一个参数的socket设置为“被动状态”，使该socket成为一个服务进程，可以接受其他socket的请求，因此用在服务端
3. 第二个参数可以理解成内核的处理请求队列的最大长度，一般小于30，也可以取值SOMAXCONN表示由系统决定（一般比较大）
#### accept(fd, NULL, NULL)函数
1. 函数原型为int accept(int sock, struct sockaddr \*addr, socklen_t \*addrlen);
2. 返回一个新的套接字来和客户端通信，第二个参数addr用来保存客户端的IP地址和端口号，而第一个参数sock是服务器端的套接字
3. listen函数只是让套接字进入监听状态，并没有真正接收客户端请求，listen后面的代码会继续执行，直到遇到accept()
4. accept函数会阻塞程序执行（后面代码不能被执行），直到有新的请求到来
5. 注意返回值为新的套接字，用于和客户端进行通信，但是用于listen的那个套接字仍然在监听是否有请求
#### recv(fd, buff, BUFFSIZE - 1, 0)函数
1. 定义在sys/socket.h头文件里面，函数原型为ssize_t recv(int sockfd, void \*buff, size_t nbytes, int flags);
2. 作用是将第一个参数的套接字里的接收缓冲区中的数据复制到第二个参数指向的内存中，返回实际复制到的字节数
3. 第四个参数一般为0
4. 当该套接字的接收缓冲区里面的数据长度大于第三个参数时，recv没能将缓冲区的数据完全复制完，需要多次recv
#### send(fd, buff, strlen(buff), 0)函数
1. 类似于recv函数，原型为ssize_t send(int socket, const void \*buf, size_t len, int flags);
2. 将第二个参数指向的内存的数据复制到第一个参数的套接字的发送缓冲区，返回实际复制到的字节数
3. 第四个参数一般为0
4. 当第三个参数大于该套接字的发送缓冲区长度时，send没有将该内存的数据完全复制到套接字的发送缓冲区，需要多次send
#### write和read函数
1. 这两个函数的作用比recv和send函数大
2. recv和send是仅仅对socket起作用的读写函数，而write和read是对广泛的文件描述符都能进行读写操作
3. 其他用法大致相同
#### close(fd)函数
1. 关闭该socket
#### inet_pton
1. 定义在arpa/inet.h头文件中，函数原型为int inet_pton(int af, const char \*s, void \*dst);
2. 作用是将第二个参数指向的字符串即十进制点分IP地址转换为二进制网络地址，存在第三个参数的内存里
3. 第一个参数只能是AF_INET或AF_INET6，表示协议族
4. 返回1表示转换成功，否则转换失败
#### inet_ntop函数
1. 定义在arpa/inet.h头文件，函数原型为const char \*inet_ntop(int af, const void \*src, char \*dst, socklen_t size);
2. 与inet_pton相反，将第二个参数指向的IP地址从二进制转换为点分十进制，存在dst指向的内存中，该内存大小可由第四个参数设置
3. 第一个参数只能是AF_INET或AF_INET6，表示协议族
4. 返回指向dst所指向的内存的常量指针

#### inet_aton函数
1. 函数原型int inet_aton(const char \*cp, struct in_addr \*inp);
2. 将第一个参数指向的点分十进制IPV4地址转为二进制网络地址，存在第二个参数指向的内存中
3. 转换成功返回非0，失败返回0
4. 第二个参数可以为NULL，则仅仅检查第一个参数指向的的IPV4地址是否合法，而不存储结果
5. 仅适用于IPV4，一般用inet_pton，因为inet_pton适用于IPV4和IPV6

#### inet_ntoa函数
1. 函数原型为char \*inet_ntoa(struct in_addr in);
2. 将IPV4地址转换为十进制点分地址
3. 第一个参数为需要转换的IPV4地址，注意不是指针
4. 返回值为转换后的结果的静态内存地址（容易被覆盖）
5. 仅适用于IPV4，一般用inet_ntop，因为inet_ntop适用于IPV4和IPV6，并且返回结果不会被覆盖

#### inet_addr函数
1. 函数原型为in_addr_t inet_addr(const char \*cp);
2. 作用类似于inet_aton函数，但是参数和返回值不同
3. 返回转换后的二进制网络地址本身（struct in_addr_t类型）
4. 仅适用于IPV4，一般用inet_pton替代
#### alarm函数
1. 函数原型为unsigned int alarm(unsigned int seconds);
2. 在seconds秒后向内核向当前进程发送一个SIGALRM信号
3. 进程收到该信号，默认动作终止，每个进程都有且只有唯一的一个定时器

#### pipe函数
1. 函数原型为int pipe(int fd\[2\]);
2. 创建一个管道，成功返回0，失败返回-1
3. 将创建的管道两端写入参数fd的两个位置，fd\[0\]和fd\[1\]分别存储管道的读、写端
4. 调用pipe函数时在内核中开辟一块缓冲区（称为管道）用于通信，它有一个读端一个写端，可以有多个读、写文件描述符指向管道的读、写端

### Linux/Unix基本知识
#### fopen和open函数
1. fopen是ANSIC标准中的C语言库函数，在不同的系统中应该调用不同的内核api，返回文件指针，函数原型为FILE \*fopen(const char \*filename, const char \*mode)
2. open是UNIX/Linux系统调用函数，返回的是文件描述符
3. open的两个函数原型为int open(const char \*pathname, int flags);和int open(const char \*pathname, int flags, mode_t mode);
4. fopen是有缓冲区的高级IO函数，运行在用户态。但是open没有缓冲区是一个低级IO函数，运行在内核态
5. 类似的有fopen, fclose, fread, fwrite, fgetc, fputc, fgets, fputs和open, close, read, write, getc, putc, getchar, putchar等
#### 

