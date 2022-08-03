#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>

void read_child_process(int sig) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("removed pid = %d\n", pid);
        printf("child send: %d\n", WEXITSTATUS(status));
    }
}

int main(int argc, char const *argv[]) {
    pid_t pid;
    struct sigaction act;
    act.sa_handler = read_child_process;
    sigemptyset(&act.sa_mask);  // “监听”是否有子进程终止
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid = fork();
    if (pid == 0) {  // 子进程1
        puts("im child process! ");
        sleep(10);
        return 12;  // 10秒后子进程终止会产生一个SIGCHLD信号
    } else {
        printf("child process id = %d\n", pid);

        pid = fork();
        if (pid == 0) {  // 子进程2
            puts("im child process! ");
            sleep(10);
            return 24;  // 10秒后子进程终止会产生一个SIGCHLD信号
        } else {  // 父进程
            printf("child process id = %d\n", pid);
            int i;
            for (i = 0; i < 5; ++i) {
                puts("wait...");
                sleep(5);
            }

        }
    }
    return 0;
}