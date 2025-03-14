#include <iostream>
#include <chrono>
#include <pthread.h>

void *ThreadJob (void *theOps)
{
    if (theOps) {
        auto anOps = *static_cast <long long*> (theOps);
        long long sum = 0;
        for (long long i = 0; i < anOps; i++) {
            sum += i;
        }
    }

    return nullptr;
}

int main() {
    bool aLogging = false;
    pthread_t aThread;
    long long anOperations = 0;

    auto aStart = std::chrono::high_resolution_clock::now();
    pthread_create (&aThread, nullptr, ThreadJob, &anOperations);
    auto anEnd = std::chrono::high_resolution_clock::now();
    pthread_join (aThread, nullptr);

    auto aThreadCreationTime = std::chrono::duration <double> (anEnd - aStart).count();
    std::cout << "Thread creation time without operations: " << aThreadCreationTime << " sec" << std::endl;

    long long anOptimalOperations = 0;
    for (size_t i = 0; i < 3; ++i) {
        anOperations = 0;
        while (true) {
            pthread_create (&aThread, nullptr, ThreadJob, &anOperations);
            aStart = std::chrono::high_resolution_clock::now();
            pthread_join (aThread, nullptr);
            anEnd = std::chrono::high_resolution_clock::now();
            auto anExecuteTime = std::chrono::duration <double> (anEnd - aStart).count();

            if (aLogging) {
                std::cout << "Operations: " << anOperations << " | Time: " << anExecuteTime << " sec" << std::endl;
            }

            if (anExecuteTime / aThreadCreationTime > 10.) {
                anOptimalOperations += anOperations;
                if (aLogging) {
                    std::cout << "\nOptimal operations: " << anOperations << std::endl << std::endl;
                }
                break;
            }

            anOperations += 10000;
        }
    }
    std::cout << "\nFinal optimal operations: " << anOptimalOperations / 3 << std::endl;

    return 0;
}