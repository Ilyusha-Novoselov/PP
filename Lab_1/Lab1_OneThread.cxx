#include <iostream>
#include <pthread.h>

void *ThreadJob (void* theArgs)
{
    std::cout << "Thread is running..." << std::endl;

    return nullptr;
}

int main() {
    pthread_t aThread;
    int anErr = pthread_create (&aThread, nullptr, ThreadJob, nullptr);

    if (anErr != 0) {
        std::cout << "Cannot create a thread: " << std::endl;
        exit (-1);
    }

    pthread_join (aThread, nullptr);
    
    return 0;
}