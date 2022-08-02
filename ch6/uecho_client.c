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
    int sock;
    char message[BUF_SIZE];
    int str_len;
    socket_t addr_sz;
    struct sockaddr_in serv_addr, from_addr;

    if (argc != 3) {
        printf("usage: %s <ip> <port>\n", argv[0]);
        exit(1);
    }

    sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        error_handing("socket() error! ");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    while (1) {
        fputs("input message(Q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;
        sendto(sock, message, strlen(message), 0, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
        addr_sz = sizeof(from_addr);
        str_len = recvfrom(sock, message, BUF_SIZE, 0, (struct sockaddr*) &from_addr, &addr_sz);
        message[str_len] = 0;
        printf("mesage from server: %s\n", message);
    }

    close(sock);
    
    return 0;
}