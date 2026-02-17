#pragma once

class EventLoop;
class Socket;
class Acceptor;
class Connection;
#include <map>

class Server
{
private:
    EventLoop *loop;
    Acceptor *acceptor;
    std::map<int, Connection *> connections;

public:
    Server(EventLoop *);
    ~Server();

    void newConnection(Socket *sock);
    void deleteConnection(Socket *sock);
};
