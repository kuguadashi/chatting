/*=====================================
* 文件名：chat_server.c
* 日期：2015.06.04
* 描述：
*=====================================*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <epoll.h>


#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	"6666"
#define EPOLL_SIZE	5000
#define BUF_SIZE	0xFFFF
#define SERVER_WELCOME	"Welcom to join the chat room!\
							Your chat ID is:client #%d"
#define SERVER_MSG	"ClientID %d say:%s"

#define EXIT	"EXIT"
#define CAUTION	"There is only one in the chatting room"

void err_sys(char *string)
{
	perror(string);
	exit(1);
}

int set_noblocking(int sockfd)
{
	fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK);
	return 0;
}

void addfd(int epollfd, int fd, bool enable_et)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if (enable_et)
		ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	set_noblocking(fd);
	printf("fd added to epoll\n\n");
}
