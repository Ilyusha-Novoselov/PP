#ifndef _ThreadFunctions_HeaderFile
#define _ThreadFunctions_HeaderFile

namespace parallel {

struct ThreadParams {
    int myParamFirst;
    double myParamSecond;
};

class ThreadFunctions {
public:
    ThreadFunctions() = default;

    static void *ThreadJobSimple (void* theArgs);

    static void *ThreadJobOperations (void* theOps);

    static void *ThreadJobWithParams (void* theParams);
};

}

#endif
