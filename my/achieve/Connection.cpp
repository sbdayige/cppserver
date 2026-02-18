#include "include/Connection.h"
#include "include/Socket.h"
#include "include/Channel.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "tools/tools.h"

#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr)
{
    readBuffer = new Buffer();
    channel = new Channel(loop, sock->getFd());
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setCallback(cb);
    channel->enableReading();
}

Connection::~Connection()
{
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd)
{
    char buf[READ_BUFFER];
    while (true)
    {
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            readBuffer->append(buf, bytes_read);
        }
        else if (bytes_read == -1 && errno == EINTR)
        {
            continue;
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        { // 非阻塞IO，这个条件表示数据全部读取完毕
            printf("finish reading once\n");
            if (readBuffer->size() > 0)
            {
                printf("message from client fd %d: %s\n", sockfd, readBuffer->c_str());
                errif(write(sockfd, readBuffer->c_str(), readBuffer->size()) == -1, "socket write error");
                readBuffer->clear();
            }
            break;
        }
        else if (bytes_read == 0)
        {
            printf("EOF, client fd %d disconnected\n", sockfd);
            deleteConnectionCallback(sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket *)> _cb)
{
    deleteConnectionCallback = _cb;
}
