#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


void error_handing(char *m) {
    fputs(m, stderr);
    fputc('\n', stderr);
    exit(1);
}

int cal(int count, int nums[], int op) {
    int res = 0, i = 0;
    switch(op) {
        case 1:
            for (i = 0; i < count; i++)
                res += nums[i];
            break;
        case 2:
            nums[0] = -nums[0];
            for (i = 0; i < count; i++)
                res -= nums[i];
            break;
        case 3:
            res = 1;
            for (i = 0; i < count; i++)
                res *= nums[i];
            break;
    }
    return res;
}

int main(int argc, char*argv[]) {
    int serv_sock, clnt_sock;

    struct sockaddr_in serv_addr, clnt_addr;

    socklen_t clnt_addr_size;
    
    if (argc != 2) {
    	printf("usage: %s <port>\n", argv[0]);
    	exit(1);
    }

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

    clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
        error_handing("accept() error! ");
    }

    int opinfo[5];
    read(clnt_sock, opinfo, sizeof(opinfo));
    
    int result = cal(opinfo[0], opinfo + 1, opinfo[4]);
    write(clnt_sock, &result, sizeof(result));

    close(clnt_sock);    
    close(serv_sock);

    return 0;
}
