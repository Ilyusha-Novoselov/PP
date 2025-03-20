#include <omp.h>
#include <iostream>

int main() {
    int FirstArray [100], SecondArray [100];

    for (size_t i = 0; i < 100; ++i) {
        SecondArray [i] = i;
    }
    
    #pragma omp parallel for
    for (size_t i = 0; i < 100; ++i) {
        FirstArray [i] = SecondArray [i];
        SecondArray [i] = 2 * FirstArray [i];
    }
    int aResult = 0;

    #pragma omp parallel for reduction (+ : aResult)
    for (size_t i = 0; i < 100; ++i) {
        aResult += (FirstArray [i] + SecondArray [i]);
    }
    std::cout << "Result = " << aResult << std::endl;

    return 0;
}