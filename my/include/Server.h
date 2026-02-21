#pragma once
#include <map>
#include <vector>

class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop *mainReactor;
    Acceptor *acceptor;
    std::map<int, Connection *> connections;
    std::vector<EventLoop *> subReactors;
    ThreadPool *thpool;
    std::function<void(Connection *)> onConnectionCallback;

public:
    Server(EventLoop *);
    ~Server();

    void newConnection(Socket *sock);
    void deleteConnection(Socket *sock);
    void OnConnect(std::function<void(Connection *)> cb);
};
