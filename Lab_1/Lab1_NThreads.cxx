#include <iostream>
#include <vector>
#include <pthread.h>

void *ThreadJob (void* theArgs)
{
    std::cout << "Thread is running..." << std::endl;

    return nullptr;
}

int main() {
    std::vector <pthread_t> aThreads (5);

    std::cout << "Start N threads" << std::endl;

    for (auto& aThread : aThreads) {
        int anErr = pthread_create (&aThread, nullptr, ThreadJob, nullptr);

        if (anErr != 0) {
            std::cout << "Cannot create a thread: " << std::endl;
            exit (-1);
        }
    }

    for (auto& aThread : aThreads) {
        pthread_join (aThread, nullptr);
    }

    std::cout << "All threads finished" << std::endl;

    return 0;
}