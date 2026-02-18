#include "include/Epoll.h"
#include "include/Channel.h"
#include "include/EventLoop.h"
#include "include/ThreadPool.h"
#include <vector>

EventLoop::EventLoop() : ep(new Epoll()), quit(false), pool(nullptr)
{
}

EventLoop::~EventLoop()
{
    delete ep;
}

void EventLoop::loop()
{
    while (!quit)
    {
        std::vector<Channel *> chs;
        chs = ep->poll();
        for (auto it = chs.begin(); it != chs.end(); ++it)
        {
            if (pool)
            {
                Channel *ch = *it;
                pool->add(std::bind(&Channel::handleEvent, ch));
            }
            else
            {
                (*it)->handleEvent();
            }
        }
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    ep->updateChannel(channel);
}

void EventLoop::setThreadPool(ThreadPool *p)
{
    pool = p;
}