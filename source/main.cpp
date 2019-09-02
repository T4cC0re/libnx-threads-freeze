#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>
#include <thread>
#include <vector>
#include "ThreadPool.hpp"

#define NUM_THREADS 2 // Set to whatever you want. I set it to 2, to keep way below core count.

std::vector<std::future<int>> vthreads;

int call_from_thread(u32 threadNr) {
    int i = 0;
    // Make the thread busy
    while (i < 10000) {
        i++;
    }
    return i * threadNr;
}

void collectResultsBlocking() {
    printf("collectResultsBlocking called\n");
    for (u32 i = 0; i < vthreads.size(); i++){
         printf("Thread %d result: %d\n", i, vthreads.at(i).get());
    }
    vthreads.clear();
    printf("collectResultsBlocking done\n");
}

int main(int argc, char* argv[])
{
    consoleInit(NULL);
    u32 counter = 0;

    socketInitializeDefault();
    nxlinkStdio();

    printf("Hello World! %d\n", counter);
    counter++;

    ThreadPool pool{NUM_THREADS}; // Threadpool is initialized and threads started

    printf("Hello World! %d\n", counter);
    counter++;

    for (auto i = 0; i < NUM_THREADS; ++i)
    {
        // Threads get work
        vthreads.push_back(pool.enqueue([=] {
            return call_from_thread(i);
        }));
    }

    printf("Hello World! %d\n", counter);
    counter++;

    int i = 0;
    // Make the main thread busy, too
    while (i < 30000) {
        i++;
    }
    printf("i = %d\n", i);
    //Get results from future.
    collectResultsBlocking();

    socketExit();
    consoleExit(NULL);
    return 0;
}
