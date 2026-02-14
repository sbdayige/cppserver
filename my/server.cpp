#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <cstring>
#include <cstdio>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "tools/tools.h"
#include <fcntl.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void setnonblocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int epfd = epoll_create1(0);
    struct epoll_event events[MAX_EVENTS], ev;

    errif(sockfd == -1, "socket create error");
    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    bzero(&ev, sizeof(ev));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(8888);

    int bind_result = bind(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr));
    errif(bind_result == -1, "socket bind error");

    int listen_result = listen(sockfd, SOMAXCONN);
    errif(listen_result == -1, "socket listen error");

    ev.events = EPOLLIN | EPOLLET; // 在代码中使用了ET模式，且未处理错误，在day12进行了修复，实际上接受连接最好不要用ET模式
    ev.data.fd = sockfd;
    setnonblocking(sockfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
    while (true)
    {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        errif(nfds == -1, "fail epoll wait");
        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == sockfd)
            {
                struct sockaddr_in clnt_addr;
                socklen_t clnt_addr_len = sizeof(clnt_addr);
                bzero(&clnt_addr, clnt_addr_len);

                int new_client_sockfd = accept(sockfd, (sockaddr *)&clnt_addr, &clnt_addr_len);
                errif(new_client_sockfd == -1, "socket accept error");
                printf("new client fd %d! IP: %s Port: %d\n", new_client_sockfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
                bzero(&ev, sizeof(ev));
                setnonblocking(new_client_sockfd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = new_client_sockfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, new_client_sockfd, &ev);
            }
            else if (events[i].events & EPOLLIN)
            {
                char buffer[READ_BUFFER];
                while (true)
                {
                    bzero(&buffer, sizeof(buffer));
                    ssize_t read_bytes = read(events[i].data.fd, buffer, sizeof(buffer));
                    if (read_bytes > 0)
                    {
                        int write_result = write(events[i].data.fd, buffer, sizeof(buffer));
                        printf("write_result = %d\n", write_result);
                        printf("message from client fd %d: %s\n", events[i].data.fd, buffer);
                    }
                    else if (read_bytes == -1)
                    {
                        if (errno == EINTR)
                        {
                            printf("continue reading\n");
                            continue;
                        }
                        else if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            printf("finish reading once, read later\n");
                            break;
                        }
                    }
                    else if (read_bytes == 0)
                    {
                        printf("client fd %d disconnected\n", events[i].data.fd);
                        close(events[i].data.fd);
                        break;
                    }
                }
            }
            else
            {
                printf("someting else \n");
            }
        }
    }
    close(sockfd);
    return 0;
}