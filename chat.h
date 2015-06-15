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
#include <sys/epoll.h>
#include <sys/types.h>
#include <netinet/in.h>

#define SERVER_IP	"127.0.0.1"
#define SERVER_PORT	6666
#define EPOLL_SIZE	5000
#define BUF_SIZE	0xFFFF
#define SERVER_WELCOME	"Welcom to join the chat room!\
							Your chat ID is:client #%d"
#define SERVER_MSG	"ClientID %d say:%s"

#define EXIT	"EXIT"
#define CAUTION	"There is only one in the chatting room"
int count;
int clientfd_fifo[BUF_SIZE];

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

void addfd(int epollfd, int fd, int enable_et)
{
	struct epoll_event ev;
	ev.data.fd = fd;
	ev.events = EPOLLIN;
	if (enable_et != 0)
		ev.events = EPOLLIN | EPOLLET;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
	set_noblocking(fd);
	printf("fd added to epoll\n\n");
}

int send_broadcast_message(int clientfd)
{
	int i;
	int len;
	char buf[BUF_SIZE];
	char message[BUF_SIZE];
	
	bzero(buf, BUF_SIZE);
	bzero(message, BUF_SIZE);

	printf("read from ClientID: %d\n", clientfd);

	len = recv(clientfd, buf, BUF_SIZE, 0);
	if (len == 0)
	{
		close(clientfd);
		count--;
		printf("ClientID:%d closed,there are %d clients in the room\n", clientfd, count);
	}
	else
	{
		if (count == 1)
		{
			send(clientfd, CAUTION, strlen(CAUTION), 0);
			return len;
		}
		sprintf(message, SERVER_MSG, clientfd, buf);
		for (i = 0; i < count; i++ )
		{
			if (clientfd_fifo[i] != clientfd)
				if (send(clientfd_fifo[i], message, BUF_SIZE, 0) < 0)
					err_sys("send_broadcast_message error");
		}
	}
	return len;

}
