#include "include/ThreadPool.h"
#include <stdexcept>

ThreadPool::ThreadPool(int size) : stop(false)
{
    for (int i = 0; i < size; ++i)
    {
        threads.emplace_back(std::thread([this]()
                                         {
            while (true)
            {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    cv.wait(lock, [this]() { return stop || !tasks.empty(); });
                    if (stop && tasks.empty())
                        return;
                    task = tasks.front();
                    tasks.pop();
                }
                try {
                    task();
                } catch(const std::exception &e) {
                    fprintf(stderr, "ThreadPool task exception: %s\n", e.what());
                }
            } }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();
    for (auto &t : threads)
    {
        if (t.joinable())
            t.join();
    }
}
