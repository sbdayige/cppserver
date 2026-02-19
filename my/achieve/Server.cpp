#include "include/EventLoop.h"
#include "include/Socket.h"
#include "include/Server.h"
#include "include/Channel.h"
#include "include/InetAddress.h"
#include "include/Acceptor.h"
#include "include/Connection.h"
#include "include/ThreadPool.h"
#include <functional>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <thread>

#define READ_BUFFER 1024

Server::Server(EventLoop *_loop) : mainReactor(_loop), acceptor(nullptr), thpool(nullptr)
{
    acceptor = new Acceptor(mainReactor);
    std::function<void(Socket *)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);

    int size = std::thread::hardware_concurrency();
    if (size <= 0)
        size = 4;
    thpool = new ThreadPool(size);
    for (int i = 0; i < size; ++i)
    {
        subReactors.push_back(new EventLoop());
    }

    for (int i = 0; i < size; ++i)
    {
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, subReactors[i]);
        thpool->add(sub_loop);
    }
}

Server::~Server()
{
    delete acceptor;
    for (auto &item : connections)
    {
        delete item.second;
    }
    for (auto &r : subReactors)
    {
        delete r;
    }
    delete thpool;
}

void Server::newConnection(Socket *sock)
{
    int idx = sock->getFd() % subReactors.size();
    EventLoop *sub = subReactors[idx];
    Connection *conn = new Connection(sub, sock);
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