#include <iostream>
#include <cstring>
#include <random>
#include <unistd.h>
#include <chrono>
#include <pthread.h>

#define __ERR_EXIT__(theCode, theStr) { std::cerr << theStr << ": " << strerror (theCode) << std::endl; exit (EXIT_FAILURE); }

pthread_mutex_t myMutex;

enum StoreState
{
    EMPTY,
    FULL
} myState;

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

void *Producer (void* theArgs)
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

void *Consumer (void* theArgs)
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

int main() {
    myState = EMPTY;
    pthread_t aThread1, aThread2;
    pthread_mutex_init (&myMutex, nullptr);

    pthread_create (&aThread1, nullptr, Producer, nullptr);
    pthread_create (&aThread2, nullptr, Consumer, nullptr);

    pthread_join (aThread1, nullptr);
    pthread_join (aThread2, nullptr);

    pthread_mutex_destroy (&myMutex);
}