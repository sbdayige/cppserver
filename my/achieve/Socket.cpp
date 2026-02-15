#include "Socket.h"
#include "InetAddress.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "tools/tools.h"

Socket::Socket() : fd(-1)
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd)
{
    errif(fd == -1, "socket create error");
}

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress *addr)
{
    int bind_result = ::bind(fd, (sockaddr *)&addr->addr, addr->addr_len);
    errif(bind_result == -1, "socket bind error");
}

void Socket::listen()
{
    int listen_result = ::listen(fd, SOMAXCONN);
    errif(listen_result == -1, "socket listen error");
}

void Socket::setnonblocking()
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *addr)
{
    int clnt_sockfd = ::accept(fd, (sockaddr *)&addr->addr, &addr->addr_len);
    errif(clnt_sockfd == -1, "socket accept error");
    return clnt_sockfd;
}

int Socket::getFd()
{
    return fd;
}