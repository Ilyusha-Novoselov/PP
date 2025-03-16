#include <iostream>
#include <chrono>
#include <vector>
#include <functional>
#include <pthread.h>

#include "Table.hxx"

struct ThreadData
{
    std::vector <int> *myData;
    std::vector <int> *myMappedResults;
    std::function <int (int)> myMapFunc;
    size_t myStart, myEnd;
};

struct ReduceData
{
    std::vector <int> *myData;
    std::function <int (int, int)> reduceFunc;
    int myResult;
    size_t myStart, myEnd;
};

struct ResultOfMapReduce
{
    int myResult;
    double myMapTime;
    double myReduceTime;
};

void* MapThread (void* theArg) {
    ThreadData* aThreadData = (ThreadData*)theArg;
    for (size_t i = aThreadData->myStart; i < aThreadData->myEnd; i++) {
        (*aThreadData->myMappedResults)[i] = aThreadData->myMapFunc ((*aThreadData->myData)[i]);
    }
    return nullptr;
}

void* ReduceThread (void* arg) {
    ReduceData* aReduceData = (ReduceData*)arg;
    int aResult = (*aReduceData->myData)[aReduceData->myStart];
    for (size_t i = aReduceData->myStart + 1; i < aReduceData->myEnd; i++) {
        aResult = aReduceData->reduceFunc(aResult, (*aReduceData->myData)[i]);
    }
    aReduceData->myResult = aResult;
    return nullptr;
}

ResultOfMapReduce  DoMapReduce (std::vector <int>& myData, 
                                std::function <int (int)> myMapFunc, 
                                std::function <int (int, int)> reduceFunc,
                                int aNumThreads)
{
    size_t aDataSize = myData.size();
    std::vector <int> myMappedResults(aDataSize);
    std::vector <pthread_t> aThreads(aNumThreads);
    std::vector <ThreadData> aThreadData(aNumThreads);

    // Шаг 1: MAP (разделение работы по потокам)
    size_t chunkSize = (aDataSize + aNumThreads - 1) / aNumThreads;
    auto aStart= std::chrono::steady_clock::now();
    for (int i = 0; i < aNumThreads; ++i) {
        size_t myStart = i * chunkSize;
        size_t myEnd = std::min(myStart + chunkSize, aDataSize);
        aThreadData[i] = { &myData, &myMappedResults, myMapFunc, myStart, myEnd };
        pthread_create (&aThreads[i], nullptr, MapThread, &aThreadData[i]);
    }
    for (int i = 0; i < aNumThreads; ++i) {
        pthread_join (aThreads[i], nullptr);
    }
    auto anEnd = std::chrono::steady_clock::now();
    auto anParallelTimeMap = std::chrono::duration <double> (anEnd - aStart).count();

    // Шаг 2: REDUCE (объединение результатов)
    std::vector<ReduceData> aReduceData (aNumThreads);
    std::vector<int> aReduceResults (aNumThreads);

    aStart = std::chrono::steady_clock::now();
    for (int i = 0; i < aNumThreads; ++i) {
        size_t myStart = i * chunkSize;
        size_t myEnd = std::min (myStart + chunkSize, aDataSize);
        if (myStart < myEnd) {
            aReduceData[i] = { &myMappedResults, reduceFunc, 0, myStart, myEnd };
            pthread_create (&aThreads[i], nullptr, ReduceThread, &aReduceData[i]);
        }
    }
    for (int i = 0; i < aNumThreads; ++i) {
        if (aReduceData[i].myStart < aReduceData[i].myEnd) {
            pthread_join (aThreads[i], nullptr);
            aReduceResults[i] = aReduceData[i].myResult;
        }
    }
    anEnd = std::chrono::steady_clock::now();
    auto anParallelTimeReduce = std::chrono::duration <double> (anEnd - aStart).count();

    // Финальное объединение
    int aFinalResult = aReduceResults[0];
    for (size_t i = 1; i < aReduceResults.size(); i++) {
        aFinalResult = reduceFunc (aFinalResult, aReduceResults[i]);
    }

    ResultOfMapReduce aResult;
    aResult.myResult = aFinalResult;
    aResult.myMapTime = anParallelTimeMap;
    aResult.myReduceTime = anParallelTimeReduce;
    return aResult;
}

void StartMapReduce (Table& theTable, size_t theNumOfThreads, size_t theArraySize)
{
    std::vector <int> aData;
    for (size_t i = 0; i < theArraySize; ++i) {
        aData.emplace_back (2);
    }

    // Функция map: возведение в квадрат
    auto myMapFunc = [](int x) { return x * x; };

    // Функция reduce: сумма значений
    auto reduceFunc = [](int a, int b) { return a + b; };

    auto aStart = std::chrono::steady_clock::now();
    auto aResult = DoMapReduce (aData, myMapFunc, reduceFunc, theNumOfThreads);
    auto anEnd = std::chrono::steady_clock::now();
    auto anParallelTime = std::chrono::duration <double> (anEnd - aStart).count();

    theTable.setHeaders ({"Threads", "Array Size", "Result", "Map time", "Reduce time", "General time"});

    theTable.addRow ({std::to_string (theNumOfThreads),
                      std::to_string (theArraySize),
                      std::to_string (aResult.myResult),
                      std::to_string (aResult.myMapTime),
                      std::to_string (aResult.myReduceTime),
                      std::to_string (anParallelTime)});
}

int main() {
    std::pair <int, double> aResult;
    std::array <int64_t, 10> anArraySizes = {50, 100, 500, 1000, 5000, 10000, 100000, 1000000, 10000000, 100000000};
    Table aTable;
    for (size_t aThreads = 1; aThreads <= 16; aThreads *= 2) {
        for (size_t anArraySize : anArraySizes) {
            StartMapReduce (aTable, aThreads, anArraySize);
        }
    }

    aTable.PrintInTerminal();
    aTable.PrintForWord();

    return 0;
}