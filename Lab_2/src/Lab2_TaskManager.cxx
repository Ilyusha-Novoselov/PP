#include <iostream>
#include <cstring>
#include <random>
#include <pthread.h>

#include "Lab2_TaskManager.hxx"

namespace parallel {

pthread_mutex_t   myMutex;
std::vector <int> myTasks;
size_t            myCurrentTask;

namespace {
void DoTask (size_t theCurrentTask)
{
    std::cout << pthread_self() << " running task " << theCurrentTask << std::endl;
    for (int i = 0; i < 10000; i++) {
        myTasks [theCurrentTask]++;
    }
}
}

void *TaskManager::ThreadJob (void* theArgs)
{
    while (true) {
        int anErr = pthread_mutex_lock (&myMutex);
        if(anErr != 0) {
            __ERR_EXIT__ (anErr, "Cannot lock mutex");
        }

        size_t aCurrentTask = myCurrentTask;
        myCurrentTask++;
        anErr = pthread_mutex_unlock (&myMutex);
        if(anErr != 0) {
            __ERR_EXIT__ (anErr, "Cannot unlock mutex");
        }

        if(aCurrentTask < myTasks.size()) {
            DoTask (aCurrentTask);
        } else {
            return nullptr;
        }
    } 
}

void TaskManager::CheckTheExample() 
{
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
}

}
