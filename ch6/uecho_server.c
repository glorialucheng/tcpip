#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

void error_handing(char *m) {
    fputs(m, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[]) {
    int serv_sock;
    char message[BUF_SIZE];
    int str_len;
    socklen_t clnt_addr_sz;
    struct sockaddr_in serv_addr, clnt_addr;

    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (serv_sock == -1) {
        error_handing("serv_sock error! ");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        error_handing("bind() error! ");
    }  // 在UDP里面bind函数可有可无，若不绑定则系统会随机选择一个端口号进行服务，若绑定则一直使用绑定的那个端口

    while (1) {
        clnt_addr_sz = sizeof(clnt_addr);
        str_len = recvfrom(serv_sock, message, BUF_SIZE, 0, (struct sockaddr*) &clnt_addr, &clnt_addr_sz);
        // 从sock中读取数据保存到message中，并将发送方的信息存储在clnt_addr指针里面
        sendto(serv_sock, message, str_len, 0, (struct sockaddr*) &clnt_addr, clnt_addr_sz);
        // 将message发送到clnt_addr指向的套接字里面
    }

    close(serv_sock);

    return 0;
}