#include "ThreadPool.hpp"

ThreadPool::ThreadPool(std::size_t numThreads)
{
    start(numThreads);
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::start(std::size_t numThreads)
{
    for (auto i = 0u; i < numThreads; ++i)
    {
        printf("spawning thread %d/%d\n", i + 1, numThreads);
        mThreads.emplace_back([=]{
            u32 prio;
            s32 preferred;
            u32 mask;
            u64 threadId;
            Handle h = threadGetCurHandle();
            svcGetThreadPriority(&prio, h);
            svcGetThreadId(&threadId, h);
            svcGetThreadCoreMask(&preferred, &mask, h);

            printf("[%ld] Prio: %x, preferred CPU: %d, mask: %x\n", threadId, prio, preferred, mask);

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
        printf("spawned thread %d/%d\n", i + 1, numThreads);
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

//int main()
//{
//    {
//        ThreadPool pool{36};
//
//        for (auto i = 0; i < 36; ++i)
//        {
//            pool.enqueue([] {
//                auto f = 1000000000;
//                while (f > 1)
//                    f /= 1.00000001;
//            });
//        }
//    }
//
//    return 0;
//}