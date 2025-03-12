#include <iostream>
#include <cstdlib>
#include <thread>
#include <array>

#include "Lab1_ThreadManager.hxx"
#include "Lab1_WebServers.hxx"

namespace {
void ParallelArrayProcessingTest()
{
    std::array <size_t, 6> anArraySizes = {50, 100, 500, 1000, 5000, 10000};
    Table aTable;
    for (size_t aThreads = 1; aThreads <= 8; aThreads *= 2) {
        for (size_t anArraySize : anArraySizes) {
            for (size_t anOp = 100; anOp <= 100000; anOp *= 10) {
                parallel::ThreadManager::ParallelArrayProcessing (aThreads, anArraySize, anOp, aTable, false);
            }
        }
    }

    aTable.Print();
}

void clearScreen() {
    #ifdef __unix__
        system("clear");
    #else
        system("cls");
    #endif
}

}

int main (int argc, char *argv[]) {
    int aChoice;

    while (true) {
        clearScreen();

        std::cout << "1. Start One Thread\n";
        std::cout << "2. Start N Threads\n";
        std::cout << "3. Estimate Threads\n";
        std::cout << "4. Start Threads With Attributes\n";
        std::cout << "5. Start Thread With Params\n";
        std::cout << "6. Start Thread With AttrOut\n";
        std::cout << "7. Parallel Array Processing\n";
        std::cout << "8. One Thread Server\n";
        std::cout << "9. One Thread Server With Php\n";
        std::cout << "10. MultiThread Server\n";
        std::cout << "11. MultiThread Server With Php\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> aChoice;

        switch(aChoice) {
            case 1: {
                parallel::ThreadManager::StartOneThread();
                break;
            }
            case 2: {
                int aNumThreads;
                std::cout << "Enter the number of threads: ";
                std::cin >> aNumThreads;
                parallel::ThreadManager::StartNThreads (aNumThreads);
                break;
            }
            case 3: {
                bool aParam;
                std::cout << "Enter logging: ";
                std::cin >> aParam;
                parallel::ThreadManager::EstimateThreads (aParam);
                break;
            }
            case 4: {
                parallel::ThreadManager::StartThreadsWithAttr();
                break;
            }
            case 5: {
                parallel::ThreadManager::StartThreadWithParams();
                break;
            }
            case 6: {
                parallel::ThreadManager::StartThreadWithAttrOut();
                break;
            }
            case 7: {
                ParallelArrayProcessingTest();
                break;
            }
            case 8: {
                parallel::WebServers::OneThreadServer();
                break;
            }
            case 9: {
                parallel::WebServers::OneThreadServerWithPhp();
                break;
            }
            case 10: {
                parallel::WebServers::MultiThreadServer();
                break;
            }
            case 11: {
                parallel::WebServers::MultiThreadServerWithPhp();
                break;
            }
            case 0: {
                std::cout << "Exiting program...\n";
                return 0;
            }
            default:
                std::cout << "Wrong choice.\n";
                break;
        }

        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }

    return 0;
}
