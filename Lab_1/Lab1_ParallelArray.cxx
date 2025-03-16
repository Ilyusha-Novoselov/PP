#include <array>
#include <iostream>
#include <vector>
#include <chrono>
#include <pthread.h>

#include "Table.hxx"

struct ThreadData {
    int* myArray;
    size_t myStart;
    size_t myEnd;
    size_t myNumOfOperations;
    void (*myFunc)(int&, size_t);
};

void Func (int& theValue, size_t theNumOfOperations) 
{
    for (size_t i = 0; i < theNumOfOperations; ++i) {
        theValue += i;
    }
}

void *ThreadJob (void* theArgs)
{
    ThreadData* aData = static_cast <ThreadData*> (theArgs);
    for (size_t i = aData->myStart; i < aData->myEnd; ++i) {
        aData->myFunc (aData->myArray[i], aData->myNumOfOperations);
    }

    return nullptr;
}


void ParallelArrayProcessing (size_t theNum, 
                              size_t theArraySize, 
                              size_t theNumOfOperations, 
                              Table& theTable, 
                              bool aLogging)
{
    std::vector <int> anArrayParallel (theArraySize);
    for (size_t i = 0; i < theArraySize; ++i) {
        anArrayParallel[i] = rand() % 10;
    }
    std::vector <int> anArrayNotParallel (anArrayParallel);

    if (aLogging) {
        std::cout << "Source array: ";
        for (int aNum : anArrayParallel) {
            std::cout << aNum << " ";
        }
        std::cout << std::endl;
    }
    

    std::vector <pthread_t> aThreads (theNum);
    std::vector <ThreadData> aThreadsData (theNum);

    size_t aChunkSize = (theArraySize + theNum - 1) / theNum;

    int anErr;

    for (size_t i = 0; i < theNum; ++i) {
        aThreadsData[i].myArray = anArrayParallel.data();
        aThreadsData[i].myStart = i * aChunkSize;
        aThreadsData[i].myEnd = (i == theNum - 1) ? theArraySize : (i + 1) * aChunkSize;
        aThreadsData[i].myNumOfOperations = theNumOfOperations;
        aThreadsData[i].myFunc = Func;
        anErr = pthread_create (&aThreads[i], nullptr, ThreadJob, &aThreadsData[i]);
        if (anErr != 0) {
            std::cerr << "Error creating thread: " << std::endl;
            exit(-1);
        }
    }
    auto aStart = std::chrono::steady_clock::now();
    for (auto& aThread : aThreads) {
        pthread_join (aThread, nullptr);
    }
    auto anEnd = std::chrono::steady_clock::now();
    auto anParallelTime = std::chrono::duration <double> (anEnd - aStart).count();

    if (aLogging) {
        std::cout << "Final parallel array: ";
        for (int aNum : anArrayParallel) {
            std::cout << aNum << " ";
        }
    }
    

    aStart = std::chrono::steady_clock::now();
    for (int& aNum : anArrayNotParallel) {
        Func (aNum, theNumOfOperations);
    }
    anEnd = std::chrono::steady_clock::now();
    auto anNotParallelTime = std::chrono::duration <double> (anEnd - aStart).count();

    if (aLogging) {
        std::cout << "\nFinal not parallel array: ";
        for (int aNum : anArrayNotParallel) {
            std::cout << aNum << " ";
        }
    }

    theTable.setHeaders({"Threads", "Array Size", "Operations", "Speedup", "Efficiency"});

    theTable.addRow({std::to_string (theNum),
                     std::to_string (theArraySize),
                     std::to_string (theNumOfOperations),
                     std::to_string (anNotParallelTime / anParallelTime),
                     std::to_string ((anNotParallelTime / anParallelTime) / theNum)});
}

int main() {
    std::array <size_t, 6> anArraySizes = {50, 100, 500, 1000, 5000, 10000};
    Table aTable;
    for (size_t aThreads = 1; aThreads <= 8; aThreads *= 2) {
        for (size_t anArraySize : anArraySizes) {
            for (size_t anOp = 100; anOp <= 100000; anOp *= 10) {
                ParallelArrayProcessing (aThreads, anArraySize, anOp, aTable, false);
            }
        }
    }

    aTable.PrintInTerminal();

    return 0;
}