#include <iostream>
#include <vector>
#include <chrono>

#include <pthread.h>

#include "ThreadFunctions.h"
#include "ThreadManager.h"

namespace parallel {

void ThreadManager::StartOneThread() 
{
    pthread_t aThread;
    int anErr = pthread_create (&aThread, nullptr, ThreadFunctions::ThreadJobSimple, nullptr);

    if (anErr != 0) {
        std::cout << "Cannot create a thread: " << strerror (anErr) << std::endl;
        exit (-1);
    }

    pthread_join (aThread, nullptr);
}

void ThreadManager::StartNThreads (size_t N) 
{
    std::vector <pthread_t> aThreads (N);

    std::cout << "Start N threads" << std::endl;

    for (auto& aThread : aThreads) {
        int anErr = pthread_create (&aThread, nullptr, ThreadFunctions::ThreadJobSimple, nullptr);

        if (anErr != 0) {
            std::cout << "Cannot create a thread: " << strerror (anErr) << std::endl;
            exit (-1);
        }
    }

    for (auto& aThread : aThreads) {
        pthread_join (aThread, nullptr);
    }

    std::cout << "All threads finished" << std::endl;
}

void ThreadManager::EstimateThreads (bool aLogging)
{
    pthread_t aThread;
    long long anOperations = 0;

    auto aStart = std::chrono::high_resolution_clock::now();
    pthread_create (&aThread, nullptr, ThreadFunctions::ThreadJobOperations, &anOperations);
    auto anEnd = std::chrono::high_resolution_clock::now();
    pthread_join (aThread, nullptr);

    auto aThreadCreationTime = std::chrono::duration <double> (anEnd - aStart).count();
    std::cout << "Thread creation time without operations: " << aThreadCreationTime << " sec" << std::endl;

    // Подбор минимального количества операций
    long long anOptimalOperations = 0;
    for (size_t i = 0; i < 3; ++i) {
        while (true) {
            pthread_create (&aThread, nullptr, ThreadFunctions::ThreadJobOperations, &anOperations);
            aStart = std::chrono::high_resolution_clock::now();
            pthread_join (aThread, nullptr);
            anEnd = std::chrono::high_resolution_clock::now();
            auto anExecuteTime = std::chrono::duration <double> (anEnd - aStart).count();

            if (aLogging) {
                std::cout << "Operations: " << anOperations << " | Time: " << anExecuteTime << " sec" << std::endl;
            }

            if (anExecuteTime / aThreadCreationTime > 10.) {
                anOptimalOperations += anOperations;
                if (aLogging) {
                    std::cout << "\nOptimal operations: " << anOperations << std::endl << std::endl;
                }
                break;
            }

            anOperations += 1000;
        }
    }
    std::cout << "\nFinal optimal operations: " << anOptimalOperations / 3 << std::endl;
}

void ThreadManager::StartThreadsWithAttr()
{
    std::pair <pthread_t, pthread_t> aThreads;
    std::pair <pthread_attr_t, pthread_attr_t> anAttr;
    int anErr;

    pthread_attr_init (&anAttr.first);
    pthread_attr_init (&anAttr.second);

    pthread_attr_setdetachstate (&anAttr.first, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate (&anAttr.second, PTHREAD_CREATE_DETACHED);

    size_t aStackSize = 2 * 1024 * 1024; // 2 MB стек
    pthread_attr_setstacksize (&anAttr.first, aStackSize);

#if 0
    size_t aGuardSize = 4096; // 4 KB охранная зона
    pthread_attr_setguardsize (&anAttr.first, aGuardSize);

    std::unique_ptr <void> aStackMemory = std::make_unique <void> (aStackSize);
    if (aStackMemory == nullptr) {
        std::cout << "Memory allocation for stack failed!" << std::endl;
        exit(-1);
    }
    pthread_attr_setstack (&anAttr.second, aStackMemory, aStackSize);
#endif

    anErr = pthread_create (&aThreads.first, &anAttr.first, ThreadFunctions::ThreadJobSimple, nullptr);
    if (anErr != 0) {
        std::cout << "Cannot create Thread1: " << strerror (anErr) << std::endl;
        exit(-1);
    }

    anErr = pthread_create (&aThreads.second, &anAttr.second, ThreadFunctions::ThreadJobSimple, nullptr);
    if (anErr != 0) {
        std::cout << "Cannot create Thread2: " << strerror (anErr) << std::endl;
        exit(-1);
    }

    pthread_join (aThreads.first, nullptr);

    pthread_attr_destroy (&anAttr.first);
    pthread_attr_destroy (&anAttr.second);

    std::cout << "Main thread finished execution" << std::endl;
}

void ThreadManager::StartThreadWithParams()
{
    pthread_t aThread;

    ThreadParams aParams;
    aParams.myParamFirst = 1;
    aParams.myParamSecond = 2.5;

    int anErr = pthread_create (&aThread, nullptr, ThreadFunctions::ThreadJobWithParams, &aParams);

    if (anErr != 0) {
        std::cout << "Cannot create a thread: " << strerror (anErr) << std::endl;
        exit (-1);
    }

    pthread_join (aThread, nullptr);
}

}