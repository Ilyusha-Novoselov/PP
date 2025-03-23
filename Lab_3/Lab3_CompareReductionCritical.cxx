#include <array> 
#include <omp.h> 
#include <iostream> 
#include <chrono> 
#include <unordered_map>

#include <Table.hxx>

double Reduction (const size_t theNumOfThreads, const size_t theArraySize) {
    int aSum = 0;
    std::vector<int> anArray (theArraySize);
    int anId, aSize; 

    for (size_t i = 0; i < theArraySize; ++i) {
        anArray[i] = i; 
    }

    omp_set_num_threads (theNumOfThreads);

    auto aStart = std::chrono::steady_clock::now();

    #pragma omp parallel private (aSize, anId) reduction (+ : aSum)
    { 
        anId = omp_get_thread_num(); 
        aSize = omp_get_num_threads(); 
        int anIntPart = theArraySize / aSize; 
        int aRemainder = theArraySize % aSize; 
        int anArrayLocalSize = anIntPart + ((anId < aRemainder) ? 1 : 0); 
        int aStart = anIntPart * anId + ((anId < aRemainder) ? anId : aRemainder); 
        int end = aStart + anArrayLocalSize; 
        for(size_t i = aStart; i < end; ++i) {
            aSum += anArray[i];
        }
    }
    auto anEnd = std::chrono::steady_clock::now();
    auto aTime = std::chrono::duration <double> (anEnd - aStart).count();

    return aTime;
}

double Critical (const size_t theNumOfThreads, const size_t theArraySize) {
    int aSum = 0; 
    std::vector<int> anArray (theArraySize);
    int anId, aSize; 

    for (size_t i = 0; i < theArraySize; ++i) {
        anArray[i] = i; 
    }

    omp_set_num_threads (theNumOfThreads);

    auto aStart = std::chrono::steady_clock::now();

    #pragma omp parallel private (aSize, anId)
    { 
        anId = omp_get_thread_num(); 
        aSize = omp_get_num_threads(); 
        int anIntPart = theArraySize / aSize; 
        int aRemainder = theArraySize % aSize; 
        int anArrayLocalSize = anIntPart + ((anId < aRemainder) ? 1 : 0); 
        int aStart = anIntPart * anId + ((anId < aRemainder) ? anId : aRemainder); 
        int end = aStart + anArrayLocalSize; 
        for(size_t i = aStart; i < end; ++i) {
            #pragma omp critical
            aSum += anArray[i]; 
        }
    }
    auto anEnd = std::chrono::steady_clock::now();
    auto aTime = std::chrono::duration <double> (anEnd - aStart).count();

    return aTime;
}

int main() {
    Table aTable;
    aTable.setHeaders ({"Threads",
                        "Array Size",
                        "Reduction",
                        "Speedup (Red)",
                        "Efficiency (Red)"
                        "Critical",
                        "Speedup (Crit)",
                        "Efficiency (Crit)"});

    std::array <size_t, 10> anArraySizes = {50, 100, 500, 1000, 5000, 10000, 100000, 1000000, 10000000, 100000000};
    
    std::unordered_map <size_t, std::pair <double, double>> aNotParallelTime;
    for (size_t anArraySize : anArraySizes) {
        double aReductionTime = Reduction (1, anArraySize);
        double aCriticalTime = Critical (1, anArraySize);
        aNotParallelTime [anArraySize] = std::make_pair (aReductionTime, aCriticalTime);
        aTable.addRow ({std::to_string (1),
                        std::to_string (anArraySize),
                        std::to_string (aReductionTime),
                        "-",
                        "-",
                        std::to_string (aCriticalTime),
                        "-",
                        "-"});
    }

    for (size_t aThreads = 2; aThreads <= 16; aThreads *= 2) {
        for (size_t anArraySize : anArraySizes) {
            double aReductionTime = Reduction (aThreads, anArraySize);
            double aCriticalTime = Critical (aThreads, anArraySize);
            aTable.addRow ({std::to_string (aThreads),
                            std::to_string (anArraySize),
                            std::to_string (aReductionTime),
                            std::to_string (aNotParallelTime [anArraySize].first / aReductionTime),
                            std::to_string ((aNotParallelTime [anArraySize].first / aReductionTime) / aThreads),
                            std::to_string (aCriticalTime),
                            std::to_string (aNotParallelTime [anArraySize].second / aCriticalTime),
                            std::to_string ((aNotParallelTime [anArraySize].second / aCriticalTime) / aThreads)});
        }
    }

    aTable.PrintInTerminal();
    aTable.PrintForWord();
    
    return 0; 
} 