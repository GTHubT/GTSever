#include <sys/epoll.h>   /* epoll_wait, epoll_create, epoll_ctl */
#include <sys/socket.h>  /* socket, bind, listen, accept... */
#include <sys/fcntl.h>   /* fcntl */
#include <arpa/inet.h>   /* inet_ntop */
#include <unistd.h>      /* fork */
#include <cstdio>       /* printf */
#include <netinet/in.h>  /* htons */
#include <cstring>      /* memset */
#include <cerrno>	 /* errno */
#include <iostream>
#include <algorithm>
#include <sys/wait.h>

#define PROCESS_NUM	7
#define MAX_EPOLLEVENT	10

bool setnoblocksock(int fd){
	int flag;
	if ((flag = (fcntl(fd, F_GETFL, 0)))<0){
		printf("get socket property failed!");
		return false;
	}
	flag |= O_NONBLOCK;
	if ((fcntl(fd, F_SETFL, flag))<0){
		printf("set socket nonblock failed!");
		return false;
	}
	return true;
}

int main(){
	sockaddr_in server_addr;
	bzero(&server_addr, sizeof(sockaddr_in));
	int listen_fd = socket(AF_INET, IPPROTO_TCP,0);
	server_addr.sin_port = htons(30000);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = -1;

	do {
		if (!setnoblocksock(listen_fd))
			break;
        bool reuseaddr = true;
        if (!setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(bool))){
            printf("set socket to reuse socket addr failed!");
            break;
        } else{
            printf("set reuse addr success!\n");
        }

		if ((ret = bind(listen_fd, (sockaddr*)&server_addr, sizeof(sockaddr))) !=0 ){
			printf("bind listen_fd socket to addr failed! bind port = %d, error code = %d\n", htons(5000), errno);
			break;
		}

		if ((ret = listen(listen_fd, SOMAXCONN)) != 0){
			printf("listen socket failed!\n");
			break;
		}

	}while(0);

	if (ret != 0){
		return -1;
	}
    for (int i = 0; i < PROCESS_NUM; i++){
        int pid = fork();
        if (pid == 0){
            /* now create epoll */
            int epfd = epoll_create(1);
            if (epfd < 0 ){
                printf("create epoll fd failed!\n");
                return -1;
            }

            struct epoll_event ev, evlist[MAX_EPOLLEVENT];
            //ev.events |= EPOLLIN|EPOLLET|EPOLLEXCLUSIVE; // EXLUSIVE flag will help avoid thunder herd, but some time still have thunder herd, just depend on the realization, we can use ET mode, this will avoid thunder herd
            ev.events |= EPOLLIN|EPOLLET;
            ev.data.fd = listen_fd;
            if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev) == -1){
                printf("add listen fd to epoll failed!\n");
                return -1;
            }
            printf ("now fork some process to accept the new connect!\n");
            
            printf("i am process %d \n", getpid());
			int ready_num = epoll_wait(epfd, evlist, MAX_EPOLLEVENT, -1); /* -1 is use for block mode, 0 is just check ready list, >0 will wait for some time */
			if (ready_num <=0){
				printf("there are no ready event!\n");
				exit(0);
			}
            printf("get ready event num = %d\n", ready_num);
			for (int j = 0; j <  ready_num; j++){
				if (evlist[j].data.fd == listen_fd){ /* the ready event is listen fd */
					printf("now process %d get the new connect, and will process it!\n", getpid());
					//sleep(2);
					sockaddr_in cli_addr;
					socklen_t addr_len = sizeof(cli_addr);
					int new_conn = accept(listen_fd, (sockaddr*)&cli_addr, &addr_len);
					if (new_conn > 0){
						printf("get new connect!\n");
					}				
				}
			}
            close(epfd);
            printf("now exit process %d\n", getpid());
            exit(0);
		}
	}

    int status = -1;
    waitpid(0, &status, WUNTRACED);
    char a = 'c';
    std::cin>>a;
	return 0;
}
