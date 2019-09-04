#pragma once
#include <condition_variable>
#include <functional>
#include <iostream>
#include <future>
#include <vector>
#include <thread>
#include <queue>
#ifdef __SWITCH__
#include <switch.h>
#endif

using Task = std::function<void()>;
class ThreadPool
{
public:

    explicit ThreadPool(std::size_t numThreads);

    ~ThreadPool();

    template<class T>
    auto enqueue(T task)->std::future<decltype(task())>
    {
        printf("enq\n");
        auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(std::move(task));

        {
            std::unique_lock<std::mutex> lock{mEventMutex};
            mTasks.emplace([=] {
                (*wrapper)();
            });
        }

        mEventVar.notify_one();
        return wrapper->get_future();
    }

    std::vector<std::thread> mThreads;

    std::condition_variable mEventVar;

    std::mutex mEventMutex;
    bool mStopping = false;

    std::queue<Task> mTasks;

private:
    void start(std::size_t numThreads);

    void stop() noexcept;
};