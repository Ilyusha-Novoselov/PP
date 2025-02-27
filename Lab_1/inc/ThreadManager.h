#ifndef _ThreadManager_HeaderFile
#define _ThreadManager_HeaderFile

#include <cstddef>

namespace parallel {

class ThreadManager {
public:
    ThreadManager() = default;

    // Запуск одного потока
    static void StartOneThread();

    // Запуск n потоков
    static void StartNThreads (size_t N);

    // Оценка стоимости запуска потока с операцией
    static void EstimateThreads (bool aLogging);

    // Запуск потоков с разными атрибутами
    static void StartThreadsWithAttr();

    // Запуск потока со набором параметров
    static void StartThreadWithParams();

    // Запуск потока с отображением атрибутов
    static void StartThreadWithAttrOut();

    static void ParallelArrayProcessing (size_t theNum, size_t theArraySize, size_t theNumOfOperations, bool aLogging);
};

}

#endif
