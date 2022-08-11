#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUF_SIZE 1024

// 接受连接的socket、逻辑处理的socket
int acpt_sock, recv_sock;

void error_handing(char *m) {
    fputs(m, stderr);
    fputc('\n', stderr);
    exit(1);
}

// 用于接收紧急消息
void urg_handler(int sig) {
    char buf[BUF_SIZE];
    int str_len = recv(recv_sock, buf, sizeof(buf) - 1, MSG_OOB);
    buf[str_len] = 0;
    printf("urgent message: %s \n", buf);
}

int main(int argc, char const *argv[]) {
    
    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        exit(1);
    }

    acpt_sock = socket(PF_INET, SOCK_STREAM, 0);

    struct sockaddr_in recv_addr, serv_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(acpt_sock, (struct sockaddr*) &recv_addr, sizeof(recv_addr)) == -1) {
        error_handing("bind() error! ");
    }

    if (listen(acpt_sock, 5) == -1) {
        error_handing("listen() error! ");
    }

    socklen_t addr_size = sizeof(serv_addr);
    recv_sock = accept(acpt_sock, (struct sockaddr*) &serv_addr, &addr_size);

    fcntl(recv_sock, F_SETOWN, getpid());

    struct sigaction act;
    act.sa_handler = urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGURG, &act, 0);  // “监听”到SIGURG信号后调用act里面的handler绑定的函数

    int str_len;
    while((str_len = recv(recv_sock, buf, sizeof(buf), 0)) != 0) {  // 接收普通消息
        if (str_len == -1) {
            continue;
        }
        buf[str_len] = 0;
        puts(buf);  
    }

    close(recv_sock);
    close(acpt_sock);
    return 0;
}