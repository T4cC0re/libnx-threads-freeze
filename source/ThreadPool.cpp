#include "ThreadPool.hpp"

ThreadPool::ThreadPool(std::size_t numThreads)
{
    start(numThreads);
    printf("Threadpool created\n");
}

ThreadPool::~ThreadPool()
{
    stop();
    printf("Threadpool destructed\n");
}

void ThreadPool::start(std::size_t numThreads)
{
    for (auto i = 0u; i < numThreads; ++i)
    {
        printf("spawning thread %d/%lu\n", i + 1, numThreads);
        mThreads.emplace_back([=]{
#ifdef __SWITCH__
            u32 prio;
            s32 preferred;
            u32 mask;
            u64 threadId;
            Handle h = threadGetCurHandle();
            svcGetThreadPriority(&prio, h);
            svcGetThreadId(&threadId, h);
            svcGetThreadCoreMask(&preferred, &mask, h);

            printf("[%ld] Prio: %x, preferred CPU: %d, mask: %x\n", threadId, prio, preferred, mask);
#endif

            while (true)
            {
                Task task;

                {
                    std::unique_lock<std::mutex> lock{mEventMutex};

                    mEventVar.wait(lock, [=] { return mStopping || !mTasks.empty(); });

                    if (mStopping && mTasks.empty())
                        break;

                    task = std::move(mTasks.front());
                    mTasks.pop();
                }

                printf("running in thread\n");
                task();
            }
        });
        printf("spawned thread %d/%lu\n", i + 1, numThreads);
    }
    printf("all threads\n");
}

void ThreadPool::stop() noexcept
{
    {
        std::unique_lock<std::mutex> lock{mEventMutex};
        mStopping = true;
    }

    mEventVar.notify_all();

    for (auto &thread : mThreads) {
        thread.join();
    }
}
