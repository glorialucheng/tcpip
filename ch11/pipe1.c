#include <stdio.h>
#include <unistd.h>

#define BUF_SIZE 30

int main(int argc, char const *argv[]) {
    int fds[2];
    char str[] = "hello! ";
    char buf[BUF_SIZE];

    pipe(fds);

    pid_t pid = fork();
    if (pid == 0) {
        write(fds[1], str, sizeof(str));
    } else {
        read(fds[0], buf, BUF_SIZE);
        puts(buf);
    }
    return 0;
}