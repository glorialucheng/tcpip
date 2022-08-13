#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 16

void error_handing(char *m) {
    fputs(m, stderr);
    fputc('\n', stderr);
    exit(1);
}

// 无法运行，原因未知，报错Segmentation fault (core dumped)
int main(int argc, char const *argv[]) {
    
    if (argc != 3) {
        printf("usage: %s <groupID> <port>\n", argv[0]);
        exit(1);
    }

    int sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        error_handing("socket() error! ");
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    addr.sin_port = htons(atoi(argv[2]));

    bind(sock, (struct sockaddr*) &addr, sizeof(addr));

    struct ip_mreq join_addr;
    join_addr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_addr.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &join_addr, sizeof(join_addr));

    char buf[BUF_SIZE];
    int str_len;
    while (1) {
        str_len = recvfrom(sock, buf, BUF_SIZE-1, 0, NULL, 0);
        if (str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }
    close(sock);
    return 0;
}