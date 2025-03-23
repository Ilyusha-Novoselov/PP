#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <vector>
#include <deque>
#include <pthread.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#include <Table.hxx>

namespace fs = std::filesystem;

struct Task {
    std::string myFilePath;
    std::string mySubstr;
};

std::deque <Task> myTaskQueue;
pthread_mutex_t myQueueMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t myQueueCond = PTHREAD_COND_INITIALIZER;
bool myStopThreads = false;

size_t myProcessedFiles = 0;
std::vector <size_t> myFilesProcessedByThread;
pthread_mutex_t myCountMutex = PTHREAD_MUTEX_INITIALIZER;

void SearchInFileMmap (const std::string& theFileName, const std::string& theSubstring, int theThreadId) {
    int aFd = open(theFileName.c_str(), O_RDONLY | O_DIRECT);
    if (aFd == -1) return;

    struct stat aSb;
    if (fstat (aFd, &aSb) == -1) {
        close (aFd);
        return;
    }

    char* aData = (char*)mmap (nullptr, aSb.st_size, PROT_READ, MAP_PRIVATE, aFd, 0);
    if (aData == MAP_FAILED) {
        close (aFd);
        return;
    }

    std::string aFileContent (aData, aSb.st_size);
    size_t aPos = 0;
    while ((aPos = aFileContent.find(theSubstring, aPos)) != std::string::npos) {
        aPos += theSubstring.length();  // Продолжаем искать с конца текущего вхождения
    }

    munmap (aData, aSb.st_size);
    close (aFd);

    // Увеличиваем общее количество просмотренных файлов
    pthread_mutex_lock(&myCountMutex);
    myProcessedFiles++;
    myFilesProcessedByThread[theThreadId]++;  // Увеличиваем счетчик конкретного потока
    pthread_mutex_unlock(&myCountMutex);
}

// Потоковая функция
void* SearchInFile(void* theArg) {
    int aThreadId = *(int*)theArg;

    while (true) {
        std::vector <Task> aBatch;

        pthread_mutex_lock (&myQueueMutex);
        while (myTaskQueue.empty() && !myStopThreads) {
            pthread_cond_wait(&myQueueCond, &myQueueMutex);
        }

        if (myStopThreads && myTaskQueue.empty()) {
            pthread_mutex_unlock (&myQueueMutex);
            break;
        }

        while (!myTaskQueue.empty() && aBatch.size() < 500) {
            aBatch.push_back (std::move (myTaskQueue.back()));
            myTaskQueue.pop_back();
        }

        pthread_mutex_unlock (&myQueueMutex);

        for (const auto& aTask : aBatch) {
            SearchInFileMmap (aTask.myFilePath, aTask.mySubstr, aThreadId);
        }
    }
    return nullptr;
}

// Обход директории и добавление файлов в очередь
void TraverseDirectory (const std::string& thePath, const std::string& theSubstr) {
    std::vector <std::string> aFiles;
    
    for (const auto& anEntry : fs::recursive_directory_iterator (thePath)) {
        if (anEntry.is_regular_file()) {
            aFiles.push_back (anEntry.path().string());
        }
    }

    pthread_mutex_lock (&myQueueMutex);
    for (const auto& aFile : aFiles) {
        myTaskQueue.push_back ({aFile, theSubstr});
    }
    pthread_mutex_unlock (&myQueueMutex);

    pthread_cond_broadcast (&myQueueCond);
}

// Основная функция запуска потоков
void StartFileChecker(size_t theThreadCount, const std::string& thePath, const std::string& theSubstr, Table& theTable) {
    auto aStart = std::chrono::steady_clock::now();
    
    std::vector <pthread_t> aThreads (theThreadCount);
    myFilesProcessedByThread.assign (theThreadCount, 0);

    myProcessedFiles = 0;

    TraverseDirectory (thePath, theSubstr);

    // Создаем массив индексов для потоков
    std::vector <int> threadIds (theThreadCount);
    for (size_t i = 0; i < theThreadCount; ++i) {
        threadIds[i] = i; // Заполняем уникальными значениями для каждого потока
        pthread_create (&aThreads[i], nullptr, SearchInFile, &threadIds[i]); // Передаем копию индекса
    }

    pthread_mutex_lock (&myQueueMutex);
    myStopThreads = true;
    pthread_mutex_unlock (&myQueueMutex);
    
    pthread_cond_broadcast (&myQueueCond);

    for (auto& aThread : aThreads) {
        pthread_join (aThread, nullptr);
    }

    auto anEnd = std::chrono::steady_clock::now();
    auto anElapsed = std::chrono::duration <double> (anEnd - aStart).count();

    std::string threadFiles;
    for (size_t i = 0; i < theThreadCount; ++i) {
        threadFiles += std::to_string (i + 1) + "-" + std::to_string (myFilesProcessedByThread[i]) + " ";
    }

    theTable.setHeaders({"Threads", "Time", "Total Files", "Files per Thread"});
    theTable.addRow({std::to_string (theThreadCount), 
                     std::to_string (anElapsed), 
                     std::to_string (myProcessedFiles), 
                     threadFiles});
}

int main() {
    Table theTable;
    std::string thePath = "/usr";
    std::string theSubstr = "#if";
    for (size_t aThreads = 1; aThreads <= 16; aThreads *= 2) {
        StartFileChecker(aThreads, thePath, theSubstr, theTable);
    }

    theTable.PrintInTerminal();
    theTable.PrintForWord();
    
    return 0;
}
