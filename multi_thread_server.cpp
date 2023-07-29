#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
using namespace std;
	
	
void * thmain(void *);


vector<pthread_t> vth;  // 存放线程id的容器。

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;      // 初始化条件变量。
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;   // 初始化互斥锁。

vector<int> sockqueue;
pthread_t thid;

int main() {
	// 启动10个线程，线程数比cpu核数略多。
	for (int ii=0;ii<10;ii++) {
    		pthread_create(&thid,NULL,thmain,(void *)(long)ii);
		vth.push_back(thid);
	}
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_addr, clnt_addr;
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(atoi("5555"));

	bind(serv_sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	listen(serv_sock, 5);
	socklen_t addr_size = sizeof(clnt_addr);
	
	while(true) {
		clnt_sock = accept(serv_sock, (struct sockaddr*) &clnt_addr, &addr_size);
		
		pthread_mutex_lock(&mutex);  // 加锁。
		sockqueue.push_back(clnt_sock); // 把客户端的sock放入队列。
		pthread_mutex_unlock(&mutex); // 解锁。
		pthread_cond_signal(&cond);   // 触发条件，激活一个线程。
	}
}

void * thmain(void * arg) {

	int pthnum=(int)(long)arg;  // 线程的编号。

	pthread_detach(pthread_self());  // 分离线程。
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);       // 把线程设置为可取消。
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);  // 取消方式为立即取消。
	
	
	int  sockfd;
	int  ibuflen=0;
	char strrecvbuf[1024],strsendbuf[1024];
	
	while(true) {
		pthread_mutex_lock(&mutex);  // 加锁。

		// 如果队列为空，等待。 
    		while (sockqueue.size() == 0) {
			// 设置线程超时间为20秒。
			struct timeval now;
			struct timespec outtime;
			gettimeofday(&now, NULL);
			outtime.tv_sec=now.tv_sec+20;
			outtime.tv_nsec=now.tv_usec;

			pthread_cond_timedwait(&cond,&mutex,&outtime);  // 等待条件被触发。
		}
		
		printf("Thread ID: %d \n", pthnum);

		// 从队列中获取第一条记录，然后删除该记录。
		sockfd=sockqueue[0];
		sockqueue.erase(sockqueue.begin());
		pthread_mutex_unlock(&mutex);  // 解锁。
		read(sockfd, strrecvbuf, 1024);
		printf("Recive:\n%s", strrecvbuf);
		memset(strsendbuf,0,sizeof(strsendbuf));
		sprintf(strsendbuf,\
           		"HTTP/1.1 200 OK\r\n"\
           		"Server: my multi-thread webserver\r\n"\
           		"Content-Type: text/html;charset=gbk\r\n\r\n"\
           		"<message>ok!!!</message>\n");
		write(sockfd, strsendbuf, 1024);
		close(sockfd);
	}	
	
	return 0;
}
