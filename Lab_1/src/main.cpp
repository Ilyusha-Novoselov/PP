#include <iostream>
#include "ThreadManager.h"
#include <cstdlib>

void clearScreen() {
    #ifdef __unix__
        system("clear");
    #else
        system("cls");
    #endif
}

int main (int argc, char *argv[]) {
    int choice;

    while (true) {
        clearScreen();

        std::cout << "1. Start One Thread\n";
        std::cout << "2. Start N Threads\n";
        std::cout << "3. Estimate Threads\n";
        std::cout << "4. Start Threads With Attributes\n";
        std::cout << "5. Start Thread With Params\n";
        std::cout << "6. Start Thread With AttrOut\n";
        std::cout << "7. Parallel Array Processing\n";
        std::cout << "0. Exit\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        switch(choice) {
            case 1: {
                parallel::ThreadManager::StartOneThread();
                break;
            }
            case 2: {
                int numThreads;
                std::cout << "Enter the number of threads: ";
                std::cin >> numThreads;
                parallel::ThreadManager::StartNThreads(numThreads);
                break;
            }
            case 3: {
                bool param;
                std::cout << "Enter logging: ";
                std::cin >> param;
                parallel::ThreadManager::EstimateThreads (param);
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
                int param1, param2, param3;
                bool param4;
                std::cout << "Enter the number of threads: ";
                std::cin >> param1;
                std::cout << "Enter the size of the arrays: ";
                std::cin >> param2;
                std::cout << "Enter the number of operations applicable to the array element: ";
                std::cin >> param3;
                parallel::ThreadManager::ParallelArrayProcessing(param1, param2, param3, false);
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
