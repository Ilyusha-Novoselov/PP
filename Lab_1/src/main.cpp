#include "ThreadManager.h"

int main (int argc, char *argv[]) {

    //parallel::ThreadManager::StartOneThread();

    //parallel::ThreadManager::StartNThreads (5);

    //parallel::ThreadManager::EstimateThreads (false);

    //parallel::ThreadManager::StartThreadsWithAttr();

    parallel::ThreadManager::StartThreadWithParams();

    return 0;
}
