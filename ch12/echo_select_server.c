#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUF_SIZE 32

void error_handing(char *m) {
    fputs(m, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[]) {
    int serv_sock, clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    
    if (argc != 2) {
        printf("usage: %s <port>\n", argv[0]);
        exit(1);
    }
    // 创建服务器socket
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1) {
        error_handing("serv_sock error! ");
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
        error_handing("bind() error! ");
    }

    if (listen(serv_sock, 5) == -1) {
        error_handing("listen() error! ");
    }

    int fd_max = serv_sock;
    int str_len, fd_num;
    fd_set reads, copy_reads;
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads);  // 需要监听serv sock，以建立连接
    
    struct timeval timeout;
    char buf[BUF_SIZE];

    while (1) {
        copy_reads = reads;
        timeout.tv_sec = 5;  // timeout指针也会被select函数重置，因此每次循环都要重新设置
        timeout.tv_usec = 0;
        fd_num = select(fd_max + 1, &copy_reads, 0, 0, &timeout);
        if (fd_num == -1)
            break;
        if (fd_num == 0) {
            puts("timeout! ");
            continue;
        }
        // 到这里fd num > 0
        int i;
        for (i = 0; i < fd_max + 1; ++i) {  // 只能通过for循环和if确定哪些fd发生了变化
            if (FD_ISSET(i, &copy_reads)) {  // fd为i的流发生了变化
                if (i == serv_sock) { // 进一步确定i是serv sock还是clnt sock，此处为serv sock，需要建立连接
                    socklen_t addr_size = sizeof(clnt_addr);
                    // 仅仅建立连接，不完成逻辑代码
                    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &addr_size);
                    FD_SET(clnt_sock, &reads);  // 并将clnt sock放入监听数组reads，注意这里是原来的reads，不是copy reads
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock;
                    // fd_max = (fd_max < clnt_sock) ? clnt_sock : fd_max;
                    printf("connected client: %d\n", clnt_sock);
                    
                } else {  // 到这里则是监听到clnt sock流发生变化，这个值为i，是OS分配的
                    // 处理客户端的请求的逻辑代码，即从clnt sock里读写数据
                    str_len = read(i, buf, BUF_SIZE);
                    if (str_len == 0) {
                        // 客户端关闭连接，则将此socket关闭，并从reads中清除
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client: %d\n", i);
                    } else {
                        write(i, buf, str_len);
                    }

                }  // else
            }  // if

        } // for
    }

    close(serv_sock);
    return 0;
}
