#include <omp.h> 
#include <iostream> 

int main() { 
    int aSum = 0; 
    const size_t anArraySize = 100; 
    int anArray [anArraySize], anId, aSize; 

    for (size_t i = 0; i < 100; ++i) {
        anArray[i] = i; 
    }

    #pragma omp parallel private (aSize, anId)
    { 
        anId = omp_get_thread_num(); 
        aSize = omp_get_num_threads(); 
        int anIntPart = anArraySize / aSize; 
        int aRemainder = anArraySize % aSize; 
        int anArrayLocalSize = anIntPart + ((anId < aRemainder) ? 1 : 0); 
        int aStart = anIntPart * anId + ((anId < aRemainder) ? anId : aRemainder); 
        int end = aStart + anArrayLocalSize; 
        for(size_t i = aStart; i < end; ++i) {
            #pragma omp critical
            aSum += anArray[i]; 
        }
        std::cout << "Thread " << anId << ", partial aSum = " << aSum << std::endl; 
    } 
    std::cout << "Final aSum = " << aSum << std::endl; 
    return 0; 
} 