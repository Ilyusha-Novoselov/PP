#include <iostream>

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
        volatile long long sum = 0;
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
        std::cout << "First parameter: " << aParams->myParamFirst << " Second parameter: " << aParams->myParamSecond << std::endl;
    } else {
        std::cout << "Pointer on struct is null" << std::endl;
    }

    return nullptr;
}

}