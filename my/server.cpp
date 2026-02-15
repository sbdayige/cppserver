#include <cstring>
#include <cstdio>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "tools/tools.h"
#include "include/Socket.h"
#include "include/Epoll.h"
#include "include/InetAddress.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void handleReadEvent(int sockfd)
{
    char buf[READ_BUFFER];
    while (true)
    { // 由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        }
        else if (bytes_read == -1 && errno == EINTR)
        { // 客户端正常中断、继续读取
            printf("continue reading");
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        { // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if (bytes_read == 0)
        { // EOF，客户端断开连接
            printf("EOF, client fd %d disconnected\n", sockfd);
            close(sockfd); // 关闭socket会自动将文件描述符从epoll树上移除
            break;
        }
    }
}

int main()
{
    Socket *serverSocket = new Socket();
    InetAddress *serverAddr = new InetAddress("127.0.0.1", 8888);
    serverSocket->bind(serverAddr);
    serverSocket->listen();
    serverSocket->setnonblocking();
    Epoll *epoll = new Epoll();
    epoll->addFd(serverSocket->getFd(), EPOLLIN | EPOLLET);
    while (true)
    {
        std::vector<epoll_event> events = epoll->poll();
        int nfds = events.size();
        for (int i = 0; i < nfds; ++i)
        {
            if (events[i].data.fd == serverSocket->getFd())
            {
                InetAddress *clnt_addr = new InetAddress();                      // 会发生内存泄露！没有delete
                Socket *clnt_sock = new Socket(serverSocket->accept(clnt_addr)); // 会发生内存泄露！没有delete
                printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking();
                epoll->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);
            }
            else if (events[i].events & EPOLLIN)
            {
                handleReadEvent(events[i].data.fd);
            }
            else
            {
                printf("something else happened\n");
            }
        }
    }
    return 0;
}