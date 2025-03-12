#ifndef _ThreadManager_HeaderFile
#define _ThreadManager_HeaderFile

#include <cstddef>

#include "Lab1_Table.hxx"

namespace parallel {

class ThreadManager {
public:
    ThreadManager() = default;

    static void StartOneThread();

    static void StartNThreads (size_t N);

    static void EstimateThreads (bool aLogging);

    static void StartThreadsWithAttr();

    static void StartThreadWithParams();

    static void StartThreadWithAttrOut();

    static void ParallelArrayProcessing (size_t theNum, size_t theArraySize, size_t theNumOfOperations, Table& theTable, bool aLogging);
};

}

#endif
