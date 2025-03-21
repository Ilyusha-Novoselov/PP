#include <omp.h>
#include <iostream>
#include <cmath>

int Func (int theParam) {
    volatile double aResult = 0.0;
    for (unsigned long long i = 0; i < 10000000; ++i) {
        aResult += std::sin (i) * std::cos (i) + std::exp (std::log (i + 1));
    }
    return theParam;
}

int main() {
    int FirstArray [100], SecondArray [100];

    for (size_t i = 0; i < 100; ++i) {
        SecondArray [i] = i;
    }
    
    #pragma omp parallel for
    for (size_t i = 0; i < 100; ++i) {
        FirstArray [i] = Func (SecondArray [i]);
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