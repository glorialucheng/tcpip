#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        puts("child process");
    } else {
        printf("parent process, child process pid = %d \n", pid);
        sleep(30);
    }

    if (pid == 0) {
        puts("end of child process");
    } else {
        puts("end of parent process");
    }
    return 0;
}