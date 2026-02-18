#include "include/Epoll.h"
#include "include/Channel.h"

class Channel;
class Epoll;
class ThreadPool;
class EventLoop
{
private:
    Epoll *ep;
    bool quit;
    ThreadPool *pool;

public:
    EventLoop();
    ~EventLoop();
    void loop();
    void updateChannel(Channel *);
    void setThreadPool(ThreadPool *);
};