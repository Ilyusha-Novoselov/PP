#include <iostream>
#include <cstring>
#include <random>
#include <unistd.h>
#include <chrono>
#include <pthread.h>

#define __ERR_EXIT__(theCode, theStr) { std::cerr << theStr << ": " << strerror (theCode) << std::endl; exit (EXIT_FAILURE); }

std::vector <int>  myTasks;
size_t             myCurrentTask;
pthread_mutex_t    myMutex;

void DoTask (size_t theCurrentTask, bool theIsLogging)
{
    if (theIsLogging) {
        std::cout << pthread_self() << " running task " << theCurrentTask << std::endl;
    }
    for (int i = 0; i < 10000; i++) {
        myTasks [theCurrentTask]++;
    }
}

void *ThreadJob (void* theArgs)
{
    int anErr = 0;
    while (true) {
        size_t aCurrentTask = myCurrentTask;
        sleep (rand() % 3);
        myCurrentTask++;

        if (aCurrentTask < myTasks.size()) {
            DoTask (aCurrentTask, true);
        } else {
            return nullptr;
        }
    } 
}

int main() {
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution <int> valueDist(1, 100);

    for (size_t i = 0; i < 10; ++i) {
        myTasks.push_back (valueDist (gen));
    }

    myCurrentTask = 0;
    int anErr = pthread_mutex_init (&myMutex, NULL);
    if (anErr != 0) {
        __ERR_EXIT__ (anErr, "Cannot initialize mutex");
    }

    pthread_t aThread1, aThread2;
    anErr = pthread_create (&aThread1, nullptr, ThreadJob, nullptr);
    if(anErr != 0) {
        __ERR_EXIT__ (anErr, "Cannot create Thread1");
    }
    anErr = pthread_create (&aThread2, nullptr, ThreadJob, nullptr);
    if(anErr != 0) {
        __ERR_EXIT__ (anErr, "Cannot create Thread2");
    }
    pthread_join (aThread1, nullptr);
    pthread_join (aThread2, nullptr);
    pthread_mutex_destroy (&myMutex);
    
    return 0;
}