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

void read_routine(int sock, char *buf) {
    while (1) {
        int str_len = read(sock, buf, BUF_SIZE);
        if (str_len == 0) {
            return;
        }
        buf[str_len] = 0;
        printf("message from server: %s", buf);
    }
}

void write_routine(int sock, char *buf) {
    while (1) {
        fgets(buf, BUF_SIZE, stdin);
        if (!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")) {
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, buf, strlen(buf));
    }
}

int main(int argc, char const *argv[]) {
	struct sockaddr_in serv_addr;

	if (argc != 3) {
		printf("usage: %s <ip> <port>\n", argv[0]);
		exit(1);
	}

	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		error_handing("socket() error! ");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
    	error_handing("connect() error! ");  	
    }

    pid_t pid = fork();

    char buf[BUF_SIZE];

    if (pid == 0) {
        write_routine(sock, buf);
    } else {
        read_routine(sock, buf);
    }

    close(sock);
	return 0;
}