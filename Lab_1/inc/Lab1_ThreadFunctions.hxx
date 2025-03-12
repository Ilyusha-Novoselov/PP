#ifndef _ThreadFunctions_HeaderFile
#define _ThreadFunctions_HeaderFile

#include <cstddef>
#include <ctime>

namespace parallel {

struct ThreadParams {
    int myID;
    time_t myCreationTime;
};

struct ThreadData {
    int* myArray;
    size_t myStart;
    size_t myEnd;
    size_t myNumOfOperations;
    void (*myFunc)(int&, size_t);
};

class ThreadFunctions {
public:
    ThreadFunctions() = default;

    static void *ThreadJobSimple (void* theArgs);

    static void *ThreadJobOperations (void* theOps);

    static void *ThreadJobWithParams (void* theParams);

    static void *ThreadAttrPrint (void* theArgs);

    static void *ThreadJobProcessArray (void* theArgs);
};

}

#endif
