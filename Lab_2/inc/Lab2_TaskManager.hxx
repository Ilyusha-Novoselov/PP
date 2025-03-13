#ifndef _TaskManager_HeaderFile
#define _TaskManager_HeaderFile

#include <cstddef>
#include <vector>
#include <pthread.h>

class Table;

namespace parallel {

#define __ERR_EXIT__(theCode, theStr) { std::cerr << theStr << ": " << strerror (theCode) << std::endl; exit (EXIT_FAILURE); }

class TaskManager {
public:
    TaskManager() = default;

    static void *ThreadJob (void* theArgs);

    static void CheckTheExample (bool theIsEnableMutex);

//----------------------------------------------------------------------------------------//

    static void *ThreadJobMutexSpin (void* theArgs);

    static void EstimatePrimitiveSynh (size_t theTaskSize, size_t theNumberOfThreads, bool theSpinMutex, Table& theTable);

//----------------------------------------------------------------------------------------//

    static void *Producer (void* theArgs);

    static void *Consumer (void* theArgs);

    static void SimulateCondVar();

};

}

#endif
