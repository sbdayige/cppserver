#include "include/EventLoop.h"
#include "include/Socket.h"
#include "include/Server.h"
#include "include/Channel.h"
#include "include/InetAddress.h"
#include "include/Acceptor.h"
#include "include/Connection.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr)
{
    acceptor = new Acceptor(loop);
    std::function<void(Socket *)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server()
{
    delete acceptor;
    for (auto &item : connections)
    {
        delete item.second;
    }
}

void Server::newConnection(Socket *sock)
{
    Connection *conn = new Connection(loop, sock);
    std::function<void(Socket *)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[sock->getFd()] = conn;
}

void Server::deleteConnection(Socket *sock)
{
    int sockfd = sock->getFd();
    if (connections.find(sockfd) != connections.end())
    {
        Connection *conn = connections[sockfd];
        connections.erase(sockfd);
        delete conn;
    }
}