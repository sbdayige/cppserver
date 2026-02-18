#include "include/EventLoop.h"
#include "include/Server.h"
#include "include/ThreadPool.h"
#include <thread>
#include <iostream>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

int main()
{
    EventLoop *loop = new EventLoop();
    // create thread pool and bind to event loop
    unsigned int n = std::thread::hardware_concurrency();
    ThreadPool *pool = new ThreadPool(n == 0 ? 4 : static_cast<int>(n));
    loop->setThreadPool(pool);

    Server *server = new Server(loop);
    loop->loop();

    delete server;
    delete pool;
    delete loop;
    return 0;
}