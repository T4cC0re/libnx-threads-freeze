#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __SWITCH__
#include <switch.h>
#endif
#include <thread>
#include <vector>
#include "ThreadPool.hpp"

#define NUM_THREADS 2 // Set to whatever you want. I set it to 2, to keep way below core count.

std::vector<std::future<int>> vthreads;

int call_from_thread(unsigned int threadNr) {
    volatile int i = 0;
    // Make the thread busy
    while (i < 1000000000) {
        i++;
    }
    return i * threadNr;
}

void collectResultsBlocking() {
    printf("collectResultsBlocking called\n");
    for (unsigned int i = 0; i < vthreads.size(); i++){
         printf("Thread %d result: %d\n", i, vthreads.at(i).get());
    }
    vthreads.clear();
    printf("collectResultsBlocking done\n");
}

int main(int argc, char* argv[])
{
    unsigned int counter = 0;

#ifdef __SWITCH__
    consoleInit(NULL);
    socketInitializeDefault();
    nxlinkStdio();
#endif

    printf("Hello World! %d\n", counter);

    while (true) {
        // Confine in scope, so that the threadpool is re-created
        {
            ThreadPool pool{NUM_THREADS}; // Threadpool is initialized and threads started

            printf("Before spawning! %d\n", counter);

            for (auto i = 0; i < NUM_THREADS; ++i)
            {
                // Threads get work
                vthreads.push_back(pool.enqueue([=] {
                    return call_from_thread(i);
                }));
            }

            printf("after spawning! %d\n", counter);

            volatile int i = 0;
            // Make the main thread busy, too
            while (i < 600000000) {
                i++;
            }
            printf("i = %d\n", i);
            //Get results from future.
            collectResultsBlocking();
        }
        counter++;

        if (counter > 3601) {
            break;
        }
    }

#ifdef __SWITCH__
    socketExit();
    consoleExit(NULL);
#endif
    return 0;
}
