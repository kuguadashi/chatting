/*=====================================
* 文件名：chat_server.c
* 日期：2015.06.04
* 描述：
*=====================================*/
#include "chat.h"

int main(int argc, char *argv[])
{
    struct sockaddr_in server_addr;
	struct epoll_event events[EPOLL_SIZE];
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	int server_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (server_sock < 0)
		err_sys("socket error");
	if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		err_sys("bind error");
	if (listen(server_sock, 5) < 0)
		err_sys("listen error")
	

	while (1)
		int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
		if (epoll_events_count < 0)
		{
			perror("epoll error");
			break;
		}
		printf("epoll_events_count = %d\n", epoll_events_count);


		events


    return 0;
}
