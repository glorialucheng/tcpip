#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig) {
    if (sig == SIGALRM) {
        puts("time out! ");
    }
    alarm(2);
}

void keycontrol(int sig) {
    if (sig == SIGINT) {
        puts("ctrl + c! ");
    }
}

int main(int argc, char const *argv[]) {
    int i;
    signal(SIGALRM, timeout);  // “监听”alarm信号，监听到则调用timeout函数
    signal(SIGINT, keycontrol);  // “监听”ctrl + c
    alarm(2);  // 2秒后产生一个alarm
    for (i = 0; i < 3; ++i)
    {
        puts("wait......");
        sleep(100);  // 在sleep 2秒后由于产生了alarm信号，系统将进程唤醒，并执行timeout函数
                     // 并且被系统唤醒后不再执行sleep剩下的98秒
    }
    return 0;
}