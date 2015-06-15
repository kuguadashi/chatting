/*=====================================
* 文件名：chat_server.c
* 日期：2015.06.04
* 描述：
*=====================================*/
#include "chat.h"

int main(int argc, char *argv[])
{
	int epfd;
	int epoll_events_count;
	int i;
	int clientfd;
	int ret;
	static int index=0;
	socklen_t client_addr_len; 
	struct sockaddr_in server_addr;
	static struct epoll_event events[EPOLL_SIZE];
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
		err_sys("socket error");
	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		err_sys("bind error");
	if (listen(server_sock, 5) < 0)
		err_sys("listen error");
	
	printf("start to listen:%s\n", SERVER_IP);
	
	epfd = epoll_create(EPOLL_SIZE);
	if (epfd < 0)
		err_sys("epoll-create errol");
	addfd(epfd, server_sock, 1);

	while (1)
	{
		epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
		if (epoll_events_count < 0)
		{
			perror("epoll error");
			break;
		}
		printf("epoll_events_count = %d\n", epoll_events_count);
		
		for (i = 0; i < epoll_events_count; i++)
		{
			int sockfd = events[i].data.fd;
			if (sockfd == server_sock)  //new user connect
			{
				struct sockaddr_in client_addr;
				client_addr_len = sizeof(client_addr);
				clientfd = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_len);
				
				printf("client connection from:%s:%d(IP:PORT),clientfd = %d\n",
						inet_ntoa(client_addr.sin_addr), 
						ntohs(client_addr.sin_port), clientfd);
				addfd(epfd, clientfd, 1);

				count++;
				clientfd_fifo[index++] = clientfd;
			    printf("add new clientfd = %d to epoll\n", clientfd);
				printf("Now there are %d clients in the room\n", count);

				printf("welcome!\n");
				char message[BUF_SIZE];
				bzero(message, BUF_SIZE);
				sprintf(message, SERVER_WELCOME, clientfd);
				ret = send(clientfd, message, BUF_SIZE, 0);
				if (ret < 0)
					err_sys("send error");
			}
			else
			{
				ret = send_broadcast_message(sockfd);
				if (ret < 0)
					err_sys("send_broadcast_message error");
			}
		}
	}
	close(server_sock);
	close(epfd);
    return 0;
}
