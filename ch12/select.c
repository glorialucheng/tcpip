#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 32

int main(int argc, char const *argv[]) {
    fd_set reads; 
    FD_ZERO(&reads);
    FD_SET(0, &reads);  // 将需要监听的fd添加到reads数组里面
    // 如FD_SET(2699, &reads);表示将fd为2699的流添加到reads里，即reads[2699] = 1
    // 这里较为特殊，标准输入、标准输出、标准错误的fd分别为0、1、2

    fd_set temp;
    struct timeval timeout;
    char buf[BUF_SIZE];
    int result, str_len;

    while (1) {
        temp = reads;  // select会修改传入的指针所指向的数据，因此传入备份
        timeout.tv_sec = 5;  // timeout指针也会被select函数重置，因此每次循环都要重新设置
        timeout.tv_usec = 0;
        result = select(1, &temp, 0, 0, &timeout);  // 监听temp里面是否存在待读数据的文件描述符，并将结果再保存到temp
        // 返回值，-1表示error，0表示timeout，否则返回监听到的文件描述符（大于0）
        if (result == -1) {
            puts("select() error! ");
            break;
        } else if (result == 0) {
            puts("time out! ");
        } else {  // result == 1
            if (FD_ISSET(0, &temp)) {
                str_len = read(0, buf, BUF_SIZE);
                buf[BUF_SIZE] = 0;
                printf("result = %d, message from stdin: %s\n", result, buf);
            }
        }
    } // while

    return 0;
}