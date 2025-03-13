#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <queue>
#include <pthread.h>

#include "Lab2_FileChecker.hxx"
#include "Lab2_Table.hxx"

namespace fs = std::filesystem;

namespace parallel {

struct Task 
{
    std::string myFilePath;
    std::string mySubString;
};

std::queue <Task> myTaskQueue;
pthread_mutex_t myQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t myQueueCond = PTHREAD_COND_INITIALIZER;
bool myStopThreads = false;

namespace {
void* SearchInFile (void* theArg) {
    while (true) {
        pthread_mutex_lock (&myQueueMutex);
        
        while (myTaskQueue.empty() && !myStopThreads) {
            pthread_cond_wait (&myQueueCond, &myQueueMutex);
        }
        
        if (myStopThreads && myTaskQueue.empty()) {
            pthread_mutex_unlock (&myQueueMutex);
            break;
        }
        
        Task aTask = myTaskQueue.front();
        myTaskQueue.pop();
        pthread_mutex_unlock(&myQueueMutex);

        std::ifstream aFile (aTask.myFilePath);
        if (!aFile.is_open()) {
            continue;
        }

        std::string aLine;
        size_t line_number = 0;
        while (std::getline (aFile, aLine)) {
            line_number++;
            if (aLine.find (aTask.mySubString) != std::string::npos) {
                std::cout << "Found in " << aTask.myFilePath << " at line " << line_number << "\n";
            }
        }
    }
    return nullptr;
}

}

void FileChecker::TraverseDirectory (const std::string& thePath, const std::string& theSubstr)
{
    for (const auto& anEntry : fs::recursive_directory_iterator (thePath)) {
        if (anEntry.is_regular_file() && anEntry.path().extension() == ".txt") {
            pthread_mutex_lock (&myQueueMutex);
            myTaskQueue.push ({anEntry.path().string(), theSubstr});
            pthread_cond_signal (&myQueueCond);
            pthread_mutex_unlock (&myQueueMutex);
        }
    }
}

void FileChecker::StartFileChecker (size_t theNumberOfThreads, 
                                    std::string& thePath, 
                                    std::string& theSubstr, 
                                    Table& theTable)
{
    auto aStart = std::chrono::steady_clock::now();
    std::vector <pthread_t> aThreads (theNumberOfThreads);
    for (auto& aThread : aThreads) {
        pthread_create (&aThread, nullptr, SearchInFile, nullptr);
    }
    
    TraverseDirectory (thePath, theSubstr);
    
    pthread_mutex_lock (&myQueueMutex);
    myStopThreads = true;
    pthread_cond_broadcast (&myQueueCond);
    pthread_mutex_unlock (&myQueueMutex);
    
    for (auto& aThread : aThreads) {
        pthread_join (aThread, nullptr);
    }
    auto anEnd = std::chrono::steady_clock::now();
    auto aTime = std::chrono::duration_cast <std::chrono::nanoseconds> (anEnd - aStart).count();

    theTable.setHeaders ({"Threads", "Time"});

    theTable.addRow ({std::to_string (theNumberOfThreads),
                      std::to_string (aTime)});
}

void FileChecker::TestFileChecker()
{
    Table aTable;
    std::string aPath = "/home/ilya/Desktop/Work/PP/scripts/test_directory";
    std::string aSubstr = "Hello";
    for (size_t aThreads = 1; aThreads <= 16; aThreads *= 2) {
        StartFileChecker (aThreads, aPath, aSubstr, aTable);
    }

    aTable.PrintInTerminal();
    aTable.PrintForWord();
}

}
