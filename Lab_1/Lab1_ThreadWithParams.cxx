#include <iostream>
#include <chrono>
#include <pthread.h>

struct ThreadParams {
    int myID;
    time_t myCreationTime;
};

void *ThreadJob (void* theParams)
{
    ThreadParams *aParams = (ThreadParams *) (theParams);

    if (aParams) {
        std::cout << "Thread: " << aParams->myID << " Creation Time: " << ctime (&aParams->myCreationTime) << std::endl;
    } else {
        std::cout << "Pointer on struct is null" << std::endl;
    }

    return nullptr;
}

int main() {
    pthread_t aThread;

    ThreadParams aParams;
    aParams.myID = 1;
    aParams.myCreationTime = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now());

    int anErr = pthread_create (&aThread, nullptr, ThreadJob, &aParams);

    if (anErr != 0) {
        std::cout << "Cannot create a thread: " << std::endl;
        exit (-1);
    }

    pthread_join (aThread, nullptr);
    
    return 0;
}