#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define TTL 64
#define BUF_SIZE 16

void error_handing(char *m) {
    fputs(m, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char const *argv[]) {
    
    if (argc != 3) {
        printf("usage: %s <groupID> <port>\n", argv[0]);
        exit(1);
    }

    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (send_sock == -1) {
        error_handing("send_sock error! ");
    }

    struct sockaddr_in mul_addr;
    memset(&mul_addr, 0, sizeof(mul_addr));
    mul_addr.sin_family = AF_INET;
    mul_addr.sin_addr.s_addr = inet_addr(argv[1]);  // 多播ip
    mul_addr.sin_port = htons(atoi(argv[2]));  // 多播port

    int time_live = TTL;
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST, (void *) &time_live, sizeof(time_live));

    FILE *fp;
    if ((fp = fopen("news.txt", "r")) == NULL)
        error_handing("file open error! ");

    char buf[BUF_SIZE];
    while (!feof(fp)) {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*) &mul_addr, sizeof(mul_addr));
        sleep(2);
    }
    fclose(fp);
    close(send_sock);
    return 0;
}