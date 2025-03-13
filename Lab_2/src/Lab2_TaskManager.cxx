#include <iostream>
#include <cstring>
#include <random>
#include <unistd.h>
#include <chrono>
#include <pthread.h>

#include "Lab2_TaskManager.hxx"
#include "Lab2_Table.hxx"

namespace parallel {

pthread_mutex_t    myMutex;
pthread_spinlock_t mySpinlock;
std::vector <int>  myTasks;
size_t             myCurrentTask;
bool               myIsEnableMutex;
bool               mySpinMutex;
int64_t            myLockTime = 0;
int64_t            myUnlockTime = 0;
int64_t            myGeneralTime = 0;

enum StoreState
{
    EMPTY,
    FULL
} myState;

namespace {
void DoTask (size_t theCurrentTask, bool theIsLogging)
{
    if (theIsLogging) {
        std::cout << pthread_self() << " running task " << theCurrentTask << std::endl;
    }
    for (int i = 0; i < 10000; i++) {
        myTasks [theCurrentTask]++;
    }
}

void Wait (StoreState anExpectedState)
{
    int anErr;
    while (myState != anExpectedState) {
        anErr = pthread_mutex_unlock (&myMutex);
        if (anErr != 0) {
            __ERR_EXIT__ (anErr, "Cannot unlock mutex");
        }

        usleep(100);

        anErr = pthread_mutex_lock(&myMutex);
        if (anErr != 0) {
            __ERR_EXIT__ (anErr, "Cannot lock mutex");
        }
    }
}
}

void *TaskManager::ThreadJob (void* theArgs)
{
    int anErr = 0;
    while (true) {
        if (myIsEnableMutex) {
            anErr = pthread_mutex_lock (&myMutex);
            if (anErr != 0) {
                __ERR_EXIT__ (anErr, "Cannot lock mutex");
            }
        }

        size_t aCurrentTask = myCurrentTask;
        if (!myIsEnableMutex) {
            sleep (rand() % 3);
        }
        myCurrentTask++;
        if (myIsEnableMutex) {
            anErr = pthread_mutex_unlock (&myMutex);
            if (anErr != 0) {
                __ERR_EXIT__ (anErr, "Cannot unlock mutex");
            }
        }
        

        if (aCurrentTask < myTasks.size()) {
            DoTask (aCurrentTask, true);
        } else {
            return nullptr;
        }
    } 
}

void TaskManager::CheckTheExample (bool theIsEnableMutex) 
{
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    std::uniform_int_distribution <int> valueDist(1, 100);
    myIsEnableMutex = theIsEnableMutex;

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
//----------------------------------------------------------------------------------------//
void *TaskManager::ThreadJobMutexSpin (void* theArgs)
{
    int anErr;
    while (true) {
        auto aStart = std::chrono::steady_clock::now();
        if (mySpinMutex) {
            anErr = pthread_mutex_lock (&myMutex);
        } else {
            anErr = pthread_spin_lock (&mySpinlock);
        }
        auto anEnd = std::chrono::steady_clock::now();
        auto aLockTime = std::chrono::duration_cast <std::chrono::nanoseconds> (anEnd - aStart).count();

        myLockTime += aLockTime;
        if(anErr != 0) {
            __ERR_EXIT__ (anErr, "Cannot lock");
        }

        size_t aCurrentTask = myCurrentTask;
        myCurrentTask++;

        aStart = std::chrono::steady_clock::now();
        if (mySpinMutex) {
            anErr = pthread_mutex_unlock (&myMutex);
        } else {
            anErr = pthread_spin_unlock (&mySpinlock);
        }
        anEnd = std::chrono::steady_clock::now();;
        auto aUnlockTime = std::chrono::duration_cast <std::chrono::nanoseconds> (anEnd - aStart).count();
        myUnlockTime += aUnlockTime;
        if(anErr != 0) {
            __ERR_EXIT__ (anErr, "Cannot lock");
        }

        if (aCurrentTask < myTasks.size()) {
            DoTask (aCurrentTask, false);
        } else {
            return nullptr;
        }
    }

}

void TaskManager::EstimatePrimitiveSynh (size_t theTaskSize, size_t theNumberOfThreads, bool theSpinMutex, Table& theTable)
{
    std::vector <pthread_t> aThreads (theNumberOfThreads);
    
    myTasks.clear();
    std::random_device rd; 
    std::mt19937 gen (rd()); 
    std::uniform_int_distribution <int> valueDist (1, 10);
    for (size_t i = 0; i < theTaskSize; ++i) {
        myTasks.push_back (valueDist (gen));
    }
    
    mySpinMutex = theSpinMutex;
    myCurrentTask = 0;
    myLockTime = 0;
    myUnlockTime = 0;
    myGeneralTime = 0;

    int anErr;
    if (theSpinMutex) {
        anErr = pthread_mutex_init (&myMutex, nullptr);
    } else {
        anErr = pthread_spin_init (&mySpinlock, PTHREAD_PROCESS_PRIVATE);
    }
    if(anErr != 0) {
        __ERR_EXIT__ (anErr, "Cannot initialize mutex/spin");
    }
    auto aStart = std::chrono::steady_clock::now();
    for (auto& aThread : aThreads) {
        anErr = pthread_create (&aThread, nullptr, ThreadJobMutexSpin, nullptr);
    }
    if(anErr != 0) {
        __ERR_EXIT__ (anErr, "Cannot create thread");
    }

    for (auto& aThread : aThreads) {
        anErr = pthread_join (aThread, nullptr);
    }
    auto anEnd = std::chrono::steady_clock::now();
    myGeneralTime = std::chrono::duration_cast <std::chrono::nanoseconds> (anEnd - aStart).count();

    if (theSpinMutex) {
        pthread_mutex_destroy (&myMutex);
    } else {
        pthread_spin_destroy (&mySpinlock);
    }

    theTable.setHeaders ({"Mutex/Spin", "Threads", "Num of Tasks", "Lock time", "Unlock time", "Lock/Unlock time", "All time"});

    theTable.addRow ({theSpinMutex ? "Mutex" : "Spin",
                      std::to_string (theNumberOfThreads),
                      std::to_string (theTaskSize),
                      std::to_string (myLockTime / theTaskSize),
                      std::to_string (myUnlockTime / theTaskSize),
                      std::to_string ((myLockTime + myUnlockTime) / theTaskSize),
                      std::to_string (myGeneralTime)});
}
//----------------------------------------------------------------------------------------//
void *TaskManager::Producer (void* theArgs)
{
    while (true)
    {
        pthread_mutex_lock(&myMutex);

        Wait (EMPTY);

        myState = FULL;
        std::cout << "Producing...Done" << std::endl;
        usleep (500000);

        pthread_mutex_unlock(&myMutex);
    }
}

void *TaskManager::Consumer (void* theArgs)
{
    while (true)
    {
        pthread_mutex_lock(&myMutex);

        Wait (FULL);

        myState = EMPTY;
        std::cout << "Consuming...Done" << std::endl;
        usleep (500000);

        pthread_mutex_unlock(&myMutex);
    }
}

void TaskManager::SimulateCondVar()
{
    myState = EMPTY;
    pthread_t aThread1, aThread2;
    pthread_mutex_init (&myMutex, nullptr);

    pthread_create (&aThread1, nullptr, Producer, nullptr);
    pthread_create (&aThread2, nullptr, Consumer, nullptr);

    pthread_join (aThread1, nullptr);
    pthread_join (aThread2, nullptr);

    pthread_mutex_destroy (&myMutex);
}


}
