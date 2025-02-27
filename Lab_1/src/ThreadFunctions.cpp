#include <iostream>
#include <string>

#include <pthread.h>

#include "ThreadFunctions.h"

namespace parallel {

void *ThreadFunctions::ThreadJobSimple (void* theArgs)
{
    std::cout << "Thread is running..." << std::endl;

    return nullptr;
}

void *ThreadFunctions::ThreadJobOperations (void *theOps)
{
    if (theOps) {
        auto anOps = *static_cast <long long*> (theOps);
        long long sum = 0;
        for (long long i = 0; i < anOps; i++) {
            sum += i;
        }
    }

    return nullptr;
}

void *ThreadFunctions::ThreadJobWithParams (void* theParams)
{
    ThreadParams *aParams = (ThreadParams *) (theParams);

    if (aParams) {
        std::cout << "Thread: " << aParams->myID << " Creation Time: " << ctime (&aParams->myCreationTime) << std::endl;
    } else {
        std::cout << "Pointer on struct is null" << std::endl;
    }

    return nullptr;
}

void *ThreadFunctions::ThreadAttrPrint (void* theArgs)
{
    int aDetach = 0;
    size_t aGuard = 0;
    size_t aStackSize = 0;

#ifdef __unix__
    pthread_attr_t aThreadAttr;
    pthread_getattr_np (pthread_self(), &aThreadAttr);
    pthread_attr_getstacksize (&aThreadAttr, &aStackSize);
    pthread_attr_getdetachstate (&aThreadAttr, &aDetach);
    pthread_attr_getguardsize (&aThreadAttr, &aGuard);
#endif

    std::cout << "Detach state: " + std::to_string (aDetach) << std::endl;
    std::cout << "Guard size: " + std::to_string (aGuard) << std::endl;
    std::cout << "Stack size: " + std::to_string (aStackSize) << std::endl;

    return nullptr;
}

void *ThreadFunctions::ThreadJobProcessArray (void* theArgs)
{
    ThreadData* aData = static_cast <ThreadData*> (theArgs);
    for (size_t i = aData->myStart; i < aData->myEnd; ++i) {
        aData->myFunc (aData->myArray[i], aData->myNumOfOperations);
    }

    return nullptr;
}

}