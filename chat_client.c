/*=====================================
* 文件名：chat_client.c
* 日期：2015.06.09
* 描述：
*=====================================*/
#include "chat.h"

int main(int argc, char *argv[])
{
	int epfd;
	int client_sock;
	int epoll_events_count;
	int i;
	int pipefd[2];
	int pid;
	int ret;
	int is_client_work = 1;
	char message[BUF_SIZE];
	struct sockaddr_in server_addr;
	static struct epoll_event events[2];
	server_addr.sin_family = PF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	client_sock = socket(PF_INET, SOCK_STREAM, 0);
	if (client_sock < 0)
		err_sys("socket error");
	if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		err_sys("connect error");
	if (pipe(pipefd) < 0)
		err_sys("pipe error");
    
	epfd = epoll_create(EPOLL_SIZE);
	if (epfd < 0)
		err_sys("epoll-create errol");
	addfd(epfd, client_sock, 1);
	addfd(epfd, pipefd[0], 1);

	if((pid = fork()) < 0)
		err_sys("fork error");
	else if (pid == 0) //child
	{
		close(pipefd[0]);
		printf("input 'exit' to quit chatting\n");
		while (is_client_work)
		{
			bzero(&message, BUF_SIZE);
			fgets(message, BUF_SIZE, stdin);
			
			if (strncasecmp(message, EXIT, strlen(EXIT)) == 0)
			{
				is_client_work = 0;
			}
			else
			{
				if (write(pipefd[1], message, strlen(message)-1) < 0)
					err_sys("write pipe[1] error");
			}
		}
	}
	else    //parent
	{
		close(pipefd[1]);
		while (is_client_work){
			epoll_events_count = epoll_wait(epfd, events, 2, -1);
			for (i=0; i< epoll_events_count; i++)
			{
				bzero(&message, BUF_SIZE);
				if (events[i].data.fd == client_sock) //server send message
				{
					ret = recv(client_sock, message, BUF_SIZE, 0);
					if (ret == 0)
					{
						printf("server closed sock = %d\n", client_sock);
						close(client_sock);
						is_client_work = 0;
					}
					else
						printf("%s\n",message);
				}
				else
				{
					ret = read(events[i].data.fd, message, BUF_SIZE);
					if (ret == 0)
						is_client_work = 0;
					else
					{
						send(client_sock, message, BUF_SIZE, 0);
					}
				}
			}	
		}
	}

	if (pid == 0)
	{
		close(pipefd[1]);
	}
	else
	{
		close(pipefd[0]);
		close(client_sock);
	}

    return 0;
}
