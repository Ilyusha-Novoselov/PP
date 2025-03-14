#include <iostream>
#include <pthread.h>

void *ThreadJob (void* theArgs)
{
    int aDetach = 0;
    size_t aGuard = 0;
    size_t aStackSize = 0;

    pthread_attr_t aThreadAttr;
    pthread_getattr_np (pthread_self(), &aThreadAttr);
    pthread_attr_getstacksize (&aThreadAttr, &aStackSize);
    pthread_attr_getdetachstate (&aThreadAttr, &aDetach);
    pthread_attr_getguardsize (&aThreadAttr, &aGuard);

    std::cout << "Detach state: " + std::to_string (aDetach) << std::endl;
    std::cout << "Guard size: " + std::to_string (aGuard) << std::endl;
    std::cout << "Stack size: " + std::to_string (aStackSize) << std::endl;

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