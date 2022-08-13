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

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        printf("usage: %s <groupID> <port>\n", argv[0]);
        exit(1);
    }

    int send_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (send_sock == -1) {
        error_handing("send_sock error! ");
    }

    struct sockaddr_in broad_addr;
    memset(&broad_addr, 0, sizeof(broad_addr));
    broad_addr.sin_family = AF_INET;
    broad_addr.sin_addr.s_addr = inet_addr(argv[1]);  // 广播ip
    broad_addr.sin_port = htons(atoi(argv[2]));  // 广播port

    int brd = 1;
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, (void *) &brd, sizeof(brd));

    FILE *fp;
    if ((fp = fopen("news.txt", "r")) == NULL)
        error_handing("file open error! ");

    char buf[BUF_SIZE];
    while (!feof(fp)) {
        fgets(buf, BUF_SIZE, fp);
        sendto(send_sock, buf, strlen(buf), 0, (struct sockaddr*) &broad_addr, sizeof(broad_addr));
        sleep(2);
    }
    fclose(fp);
    close(send_sock);
    return 0;
}