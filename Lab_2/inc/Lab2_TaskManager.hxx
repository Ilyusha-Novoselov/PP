#ifndef _TaskManager_HeaderFile
#define _TaskManager_HeaderFile

#include <cstddef>
#include <vector>
#include <pthread.h>

namespace parallel {

#define __ERR_EXIT__(theCode, theStr) { std::cerr << theStr << ": " << strerror (theCode) << std::endl; exit (EXIT_FAILURE); }

class TaskManager {
public:
    TaskManager() = default;

    static void *ThreadJob (void* theArgs);

    static void CheckTheExample();
};

}

#endif
