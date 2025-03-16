#include <iostream>
#include <pthread.h>

void *ThreadJob (void* theArgs)
{
    std::cout << "Thread is running..." << std::endl;

    return nullptr;
}

int main() {
    std::pair <pthread_t, pthread_t> aThreads;
    std::pair <pthread_attr_t, pthread_attr_t> anAttr;
    int anErr;

    pthread_attr_init (&anAttr.first);
    pthread_attr_init (&anAttr.second);

    pthread_attr_setdetachstate (&anAttr.first, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setdetachstate (&anAttr.second, PTHREAD_CREATE_DETACHED);

    size_t aStackSize = 2 * 1024 * 1024;
    pthread_attr_setstacksize (&anAttr.first, aStackSize);

    size_t aGuardSize = 4096;
    pthread_attr_setguardsize (&anAttr.first, aGuardSize);

    void* aStackMemory = malloc (aStackSize);
    if (aStackMemory == nullptr) {
        std::cout << "Memory allocation for stack failed!" << std::endl;
        exit(-1);
    }
    pthread_attr_setstack (&anAttr.second, aStackMemory, aStackSize);

    anErr = pthread_create (&aThreads.first, &anAttr.first, ThreadJob, nullptr);
    if (anErr != 0) {
        std::cout << "Cannot create Thread1: " << std::endl;
        exit(-1);
    }

    anErr = pthread_create (&aThreads.second, &anAttr.second, ThreadJob, nullptr);
    if (anErr != 0) {
        std::cout << "Cannot create Thread2: " << std::endl;
        exit(-1);
    }

    pthread_join (aThreads.first, nullptr);

    pthread_attr_destroy (&anAttr.first);
    pthread_attr_destroy (&anAttr.second);
    free (aStackMemory);

    std::cout << "Main thread finished execution" << std::endl;

    return 0;
}